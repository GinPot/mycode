/**===========================================================================

  @file
        vdev-virtio-i2c.c

  @brief
        i2c controller para-virtualization based on virtio

  @details
        Emulates an i2c controller for guest, since it's para-virtualization we
		also need a front end i2c driver.
		FE i2c driver <--> BE i2c driver <--> i2c client <-->
		i2c resource manager <--> i2c phsyical driver

  @copyright
        Copyright (c) 2012-2019 QUALCOMM Technologies Incorporated.
        All rights reserved. Licensed Material - Restricted Use.
        Qualcomm Confidential and Proprietary.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/plt_vm_vdev.qxa_qa/1.0/vdev-virtio-i2c/vdev-virtio-i2c.c#2 $
$DateTime: 2019/02/25 20:00:15 $
$Author: jingfeil $
$Change: 18424426 $

when       who          what, where, why
--------   ---       ------------------------------------------------------
02/20/19   jingfeil  Initial Version
==========================================================================*/

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <qvm/vdev.h>
#include <qvm/vio.h>
#include <sys/neutrino.h>
#include <inttypes.h>
#include <pthread.h>
#include <hw/inout.h>

#include <sys/resmgr.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <amss/i2c_client.h>
#include <amss/core/qc_virtio.h>

/* feature bits */
#define VIRTIO_F_I2C_7BITS		0x1
#define VIRTIO_F_I2C_10BITS		0x2

/* config space */
struct i2c_config {
	int bus_num;	/* hardware bus number*/
	int bus_speed;	/* standard,fast(default),high */
};

/* i2c transfer type */
#define VIO_I2C_READ	0x01
#define VIO_I2C_WRITE	0x02
#define VIO_I2C_RDWR	0x03

/* virtq descriptor index */
enum {
	DESC_IDX_HEAD,
	DESC_IDX_BUF,
	DESC_IDX_END,
};

struct i2c_request {
	uint32_t	type;
	uint32_t	addr;
	uint32_t	len;
	uint32_t	total_len;
};

#define MAX_NAME_LENGTH 11
#define NOT_RUNNING		1

/* Device specific state information */
typedef struct _vio_i2c_dev_s {
	struct vio_state		vs;
	int		fd;
	uint8_t slv_addr;	//current slave address
	bool	status;		//i2c controller status
	struct  i2c_config	cfg; //i2c config space
	char	name[MAX_NAME_LENGTH];
} vio_i2c_dev_t;

static const struct vio_description i2c_vdesc = {
    .device_type = VIRTIO_DT_I2C,
    .num_queues = 1,
    .device_config_size = 16,
    .device_features = {
		VIO_F_BIT(VIRTIO_F_I2C_7BITS),
        VIO_F_BIT(VIRTIO_F_VERSION_1),
    },
};

/* command line options */
enum {
    OPTION_VIO_I2C_VERBOSE,
    OPTION_VIO_I2C_DEVICE,
};

#define QVM_LOG(severity,fmt,...)\
    do { \
        struct timespec tp;\
        if ((severity) <= g_verbose) { \
            clock_gettime( CLOCK_REALTIME, &tp);\
            (void) qvm_report("%02d:%02d:%02d :: %s: " fmt, \
					(int)(tp.tv_sec / (60 * 60)), \
					(int)((tp.tv_sec % (60 * 60)) / 60), \
					(int)((tp.tv_sec % (60 * 60) % 60)), \
					__func__, ##__VA_ARGS__);\
        } \
    } while (0)

/* Wrapper definitions for severity levels */
enum _LOGGER_SEVERITY{
    VIOLOG_ERROR,          //unexpected recoverable error
    VIOLOG_WARNING,        //expected error
    VIOLOG_INFO,           //information
    VIOLOG_DEBUG,          //debug messages (e.g., normal detail).
};

/* log verbosity for qvm_report */
static uint8_t g_verbose = VIOLOG_ERROR;

static pthread_mutex_t mutex_xfer = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_xfer = PTHREAD_COND_INITIALIZER;
static bool is_virtq_updated = false;
static pthread_t tid;

void *i2c_transfer(void * arg) {
	qvm_vdev_t *vdp = (qvm_vdev_t*)arg;
	vio_i2c_dev_t *const dev = vdp->v_device;
	struct i2c_request *req = NULL;
	int ret = -1;

	for(;;) {
		pthread_mutex_lock(&mutex_xfer);
		while (is_virtq_updated == false)
			pthread_cond_wait(&cond_xfer, &mutex_xfer);
		is_virtq_updated = false;
		pthread_mutex_unlock(&mutex_xfer);

		struct virtq *const vq = dev->vs.qinfo[0].vq;
		unsigned const first_idx = virtq_avail_get(vq);
		if(first_idx == VIRTQ_IDX_NONE) {
			continue;
		}

		unsigned total = 0;
		unsigned vidx = first_idx;
		for(;;) {
			struct virtq_desc *const desc = &vq->desc[vidx];
			void * const src = qvm_map_guest(vdp->v_gsp, PROT_WRITE, desc->addr, desc->len);

			switch (vidx) {
				case DESC_IDX_HEAD:
					req = (struct i2c_request *)src;
					if(dev->slv_addr != req->addr) {
						i2c_set_slave_addr(dev->fd, req->addr, I2C_ADDRFMT_7BIT);
						dev->slv_addr = req->addr;
					}
					break;

				case DESC_IDX_BUF:
					if (req->len == 0) {
						QVM_LOG(VIOLOG_ERROR, "request len is zero");
						ret = -1;
						break;
					}
					switch (req->type) {
						case VIO_I2C_READ:
							ret = i2c_read(dev->fd, (void*)src, req->len);
							break;
						case VIO_I2C_WRITE:
							ret = i2c_write(dev->fd, (void*)src, req->len);
							break;
						case VIO_I2C_RDWR:
							ret = i2c_combined_writeread(dev->fd, (void*)src, req->len, \
									(void*)src + req->len, req->total_len - req->len);
							break;
						default:
							QVM_LOG(VIOLOG_ERROR, "invalid request type");
							break;
					}
					break;

				case DESC_IDX_END:
					*(unsigned*)src = ret;
					break;
				default:
					break;
			};

			qvm_unmap_guest(vdp->v_gsp, src, desc->len, 0);
			if(!(desc->flags & VIRTQ_DESC_F_NEXT)) {
				break;
			}
			vidx = desc->next;
		}
		virtq_used_put(vq, (uint16_t)first_idx, total);
	}
}

/**
 * Perform a vdev read operation for the device
 * @param       vdev    virtual device
 * @param       vopnd   state block for virtual device reference operand
 * @param       oopnd   state block for the 'other' operand
 * @param       gcp     guest cpu performing the reference
 * @returns		QVRS_* value
 */
static enum qvm_vdev_ref_status vio_i2c_vread(qvm_vdev_t *const vdev,
        unsigned cookie , const struct qvm_state_block *const vopnd,
        const struct qvm_state_block *const oopnd,
        struct guest_cpu *const gcp)
{
    vio_i2c_dev_t *dev = vdev->v_device;
	uint8_t idx = 0;
	int num = 6;

    unsigned const res = vio_read(&dev->vs, cookie, vopnd, oopnd, gcp);
	unsigned const op = res & ~VIOREF_CLASS_DEVCFG;
	switch (res & VIOREF_CLASS_MASK) {
		case VIOREF_CLASS_DEVCFG:
			if (op >= sizeof(struct i2c_config))
				break;
			QVM_LOG(VIOLOG_DEBUG,"read device config op:%d data:%d", op, *(int*)((char*)&dev->cfg+op));
			qvm_cpu_write(gcp, QXF_NONE, oopnd, 1, (void*)((char*)&dev->cfg+op), sizeof(int));
			break;
		default:
			break;
	};

    return QVRS_NORMAL;
}

/**
 * Perform a vdev write operation for the device
 * @param   vdev    virtual device
 * @param	cookie	region cookie
 * @param   vopnd   state block for virtual device reference operand
 * @param   oopnd   state block for the 'other' operand
 * @param   gcp     guest cpu performing the reference
 * @returns QVRS_* value
 */
static enum qvm_vdev_ref_status
vio_i2c_vwrite(qvm_vdev_t *const vdev, unsigned const cookie, const struct qvm_state_block *const vopnd,
        const struct qvm_state_block *const oopnd, struct guest_cpu *const gcp) {
    vio_i2c_dev_t *dev = vdev->v_device;
	uint32_t ospeed = I2C_SPEED_FAST;

    unsigned const res = vio_write(&dev->vs, cookie, vopnd, oopnd, gcp);
    unsigned const op = res & ~VIOREF_CLASS_MASK;
    switch(res & VIOREF_CLASS_MASK) {
    case VIOREF_CLASS_RESET:
        QVM_LOG(VIOLOG_DEBUG,"FE reset device");
        vio_reset(&dev->vs);
        break;
    case VIOREF_CLASS_NOTIFY:
		pthread_mutex_lock(&mutex_xfer);
		is_virtq_updated = true;
		pthread_cond_signal(&cond_xfer);
		pthread_mutex_unlock(&mutex_xfer);
        break;
    case VIOREF_CLASS_ENABLED:
		QVM_LOG(VIOLOG_DEBUG,"enabled");
        //TODO: what do we have to do with queue 'op' now being enabled       JIRA:1640529
        break;
    case VIOREF_CLASS_STATUS:
		switch (op) {
			case VIRTIO_DS_ACKNOWLEDGE:
				QVM_LOG(VIOLOG_DEBUG,"FE set status acknowledge");
				break;
			case VIRTIO_DS_DRIVER_OK:
				QVM_LOG(VIOLOG_DEBUG,"FE probe successfully");
				vio_setup_queues(&dev->vs);
				if (pthread_create(&tid, NULL, &i2c_transfer, (void*)vdev)) {
					QVM_LOG(VIOLOG_ERROR,"create thread i2c_transfer failed");
				}
				break;
			case VIRTIO_DS_FEATURES_OK:
				QVM_LOG(VIOLOG_DEBUG,"FE set status FEATURES_OK");
				break;
			case VIRTIO_DS_DRIVER:
				QVM_LOG(VIOLOG_DEBUG,"FE set status find driver");
				break;
			case VIRTIO_DS_DEVICE_NEEDS_RESET:
				QVM_LOG(VIOLOG_DEBUG,"FE set status need reset");
				break;
			case VIRTIO_DS_FAILED:
				QVM_LOG(VIOLOG_DEBUG,"FE set status failed");
				break;
			default:
				break;
		}
        break;
    case VIOREF_CLASS_DEVCFG:
		if (op >= sizeof(struct i2c_config))
			break;
		qvm_cpu_read(gcp, QXF_NONE, oopnd, 1, (void*)((char*)&dev->cfg+op), sizeof(int));
		QVM_LOG(VIOLOG_DEBUG,"write device config op:%d data:%d", op, *(int*)((char*)&dev->cfg+op));
		if (op == offsetof(struct i2c_config, bus_speed)) {
			if (i2c_set_bus_speed(dev->fd, dev->cfg.bus_speed, &ospeed)) {
				QVM_LOG(VIOLOG_ERROR,"set bus speed failed,errno %d", errno);
			}
			dev->cfg.bus_speed = ospeed;
		}
        break;
    default:
        break;
    }
    return QVRS_NORMAL;
}

/**
 * Handle process-container operations.
 * @param       vdp     virtual device
 * @param       ctrl	operation
 * @param       arg		parameters for the operation
 * @return      EOK on success, an error otherwise.
 */
static int vio_i2c_control(qvm_vdev_t *const vdp, unsigned const ctrl, const char *const arg)
{
    int ret = EOK;
    vio_i2c_dev_t *dev = vdp->v_device;

	if (dev->status == NOT_RUNNING)
		return ret;

    switch (ctrl) {
        case VDEV_CTRL_OPTIONS_START:
			vdp->v_block.type = QST_MEMORY;
			vdp->v_block.location = QSL_NO_LOCATION;
            break;

        case VDEV_CTRL_OPTIONS_END:
			vio_init(vdp, VIOVER_100, &i2c_vdesc, &dev->vs);
			ret = vio_init_queue(&dev->vs, 0, 0x100);
			if (ret != EOK) {
				break;
			}
            break;

        case (VDEV_CTRL_FIRST_OPTIDX + OPTION_VIO_I2C_VERBOSE):
            g_verbose = strtoul(arg, NULL, 16);
            g_verbose = g_verbose > VIOLOG_DEBUG ? VIOLOG_DEBUG : g_verbose;
            QVM_LOG(VIOLOG_DEBUG,"g_verbose = %u", g_verbose);
            break;

        case (VDEV_CTRL_FIRST_OPTIDX + OPTION_VIO_I2C_DEVICE):
			snprintf(dev->name, MAX_NAME_LENGTH, "/dev/%s", arg);
			dev->fd = i2c_open(dev->name);
			if (dev->fd == -1) {
				QVM_LOG(VIOLOG_ERROR, "can't open %s, errno %d", dev->name, errno);
				dev->status = NOT_RUNNING;
				break;
			}
			dev->cfg.bus_num = atoi(arg+3);
            break;

		case VDEV_CTRL_PCI_BARS:
            QVM_LOG(VIOLOG_DEBUG,"pci bars");
			return vio_ctrl_pci_bars(&dev->vs, arg);

        default:
            break;
    }

    return ret ;
}

/**
 * Register the virtual device into the framework.
 */
static void __attribute__((constructor)) vio_i2c_register(void)
{
    static const char * const vio_i2c_options[] = { "verbose", "device", NULL };

    static struct qvm_vdev_factory vio_i2c_factory = {
        .next = NULL, // patched
        .control = vio_i2c_control,
        .vread = vio_i2c_vread,
        .vwrite = vio_i2c_vwrite,
        .option_list = vio_i2c_options,
        .name = NULL, // patched
        .factory_flags = QVFF_NONE,
        .acc_sizes = 1u << sizeof(uint32_t),
		.extra_space = sizeof(vio_i2c_dev_t),
    };
    qvm_register_vdev_factory(&vio_i2c_factory, QVM_VDEV_ABI);
}
