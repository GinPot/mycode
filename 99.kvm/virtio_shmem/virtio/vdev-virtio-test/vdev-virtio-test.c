#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

#include <qvm/vdev.h>
#include <qvm/vio.h>


/* Wrapper definitions for severity levels */
enum _LOGGER_SEVERITY{
    VIOLOG_ERROR,          //unexpected recoverable error
    VIOLOG_WARNING,        //expected error
    VIOLOG_INFO,           //information
    VIOLOG_DEBUG,          //debug messages (e.g., normal detail).
};

/* log verbosity for qvm_report */
static uint8_t g_verbose = VIOLOG_ERROR;

#define QVM_LOG(severity, fmt, ...)\
	if(severity <= g_verbose)\
		qvm_report("%s: " fmt, __func__, ##__VA_ARGS__);

/* command line options */
enum {
	OPTION_VIO_VERBOSE,
	OPTION_VIO_DATA,
};

#define NOT_RUNNING		1

/* Device specific state information */
typedef struct _vio_test_dev_s {
	struct vio_state		vs;
	bool status;
} vio_test_dev_t;

#define VIRTIO_F_I2C_7BITS		0x1

static const struct vio_description test_vdesc = {
	.device_type		= 22,			//Front driver device_id for gest (struct virtio_device_id)
	.num_queues			= 1,
	.device_config_size = 16,
    .device_features = {
		VIO_F_BIT(VIRTIO_F_I2C_7BITS),	//why set it?
        VIO_F_BIT(VIRTIO_F_VERSION_1),	//Version certification？
    },
};

static pthread_mutex_t mutex_xfer = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_xfer = PTHREAD_COND_INITIALIZER;
static bool is_virtq_updated = false;
static pthread_t tid;



void *data_transfer(void *arg)
{
	qvm_vdev_t *vdp = (qvm_vdev_t *)arg;
	vio_test_dev_t *const dev = vdp->v_device;
	int ret = -1;
	
	sleep(1);
	

	for(;;){
		pthread_mutex_lock(&mutex_xfer);
		while(is_virtq_updated == false)
			pthread_cond_wait(&cond_xfer, &mutex_xfer);
		is_virtq_updated = false;
		pthread_mutex_unlock(&mutex_xfer);

		struct virtq *const vq = dev->vs.qinfo[0].vq;
		unsigned first_idx = virtq_avail_get(vq);
		
		QVM_LOG(VIOLOG_DEBUG,"max_size:%d num:%d addr:0x%x len:%d avail_flag:%d avail_idx:%d use_flag:%d use_idx:%d first_idx:%d===GinPot", dev->vs.qinfo[0].max_size,vq->num,vq->desc->addr,vq->desc->len,vq->avail->flags,vq->avail->idx,vq->used->flags,vq->used->idx,first_idx);
		//if(first_idx == VIRTQ_IDX_NONE)
		//	continue;
		
		struct virtq_desc *const desc = &vq->desc[first_idx];
		void *const src = qvm_map_guest(vdp->v_gsp, PROT_WRITE, desc->addr, desc->len);
		char *dest = (char *)src;
		
		QVM_LOG(VIOLOG_DEBUG,"data_transfer:%s===GinPot",(char *)src);

		dest[0] = 'w';
		dest[1] = 'o';
		dest[2] = 'r';
		dest[3] = 'l';
		dest[4] = 'd';

		qvm_unmap_guest(vdp->v_gsp, src, desc->len, 0);

		//virtq_used_put_notify(vq, (uint16_t)first_idx, 10, 1);
		//virtq_used_put(vq, (uint16_t)first_idx, 5);
		//
        //if (NULL != vdp->v_intr.pic) {
        //    ret = qvm_signal_intr(&vdp->v_intr, QIS_OFF);
        //    if (EOK != ret) {
        //        QVM_LOG(VIOLOG_DEBUG, "Couldn't assert the interrupt to the guest system, err=%s  ===GinPot",strerror (errno));
        //    } else {
        //        QVM_LOG(VIOLOG_DEBUG, "Asserted the bark interrupt to the guest system  ===GinPot");
        //    }
        //}
		virtq_used_put(vq, (uint16_t)first_idx, 5);		//tell gest

		QVM_LOG(VIOLOG_DEBUG,"F222222222	===GinPot");
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
 static enum qvm_vdev_ref_status vio_test_vread(qvm_vdev_t *const vdev,
			unsigned cookie, const struct qvm_state_block *const vopnd,
			const struct qvm_state_block *const oopnd,
			struct guest_cpu *const gcp)
{
	vio_test_dev_t *dev = vdev->v_device;
	int config = 0x88888888;
	int i=1;

	unsigned const res = vio_read(&dev->vs, cookie, vopnd, oopnd, gcp);		//When guest readl is called, vio_read must be called here, the following will not be processed, according to the actual situation
	unsigned const op = res & ~VIOREF_CLASS_DEVCFG;
	switch (res & VIOREF_CLASS_MASK) {
		case VIOREF_CLASS_DEVCFG:
			QVM_LOG(VIOLOG_DEBUG,"read device config op:%d  data:%x	===GinPot", op, config);
			qvm_cpu_write(gcp, QXF_NONE, oopnd, 1, (void *)(&config), sizeof(int));
		default:
			break;
	}
	
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
static enum qvm_vdev_ref_status vio_i2c_vwrite(qvm_vdev_t *const vdev, 
			unsigned const cookie, const struct qvm_state_block *const vopnd,
			const struct qvm_state_block *const oopnd, 
			struct guest_cpu *const gcp)
{
	vio_test_dev_t *dev = vdev->v_device;
	
	unsigned const res = vio_write(&dev->vs, cookie, vopnd, oopnd, gcp);
	unsigned const op = res & ~VIOREF_CLASS_MASK;
	switch(res & VIOREF_CLASS_DEVCFG){
		case VIOREF_CLASS_RESET:
			QVM_LOG(VIOLOG_DEBUG,"FE reset 	===GinPot");
			vio_reset(&dev->vs);
			break;
		case VIOREF_CLASS_NOTIFY:							//	1 Front driver load call procedure
			QVM_LOG(VIOLOG_DEBUG,"FE notify	op:%x===GinPot",op);
			pthread_mutex_lock(&mutex_xfer);
			is_virtq_updated = true;
			pthread_cond_signal(&cond_xfer);
			pthread_mutex_unlock(&mutex_xfer);
			break;
		case VIOREF_CLASS_ENABLED:
			QVM_LOG(VIOLOG_DEBUG,"FE enable	===GinPot");
			break;
		case VIOREF_CLASS_STATUS:
			//QVM_LOG(VIOLOG_DEBUG,"FE status	===GinPot");
			switch(op){
				case VIRTIO_DS_ACKNOWLEDGE:					//	2
					QVM_LOG(VIOLOG_DEBUG,"FE set acknowledge	===GinPot");
					break;
				case VIRTIO_DS_DRIVER:						//	3
					QVM_LOG(VIOLOG_DEBUG,"FE find driver	===GinPot");
					break;
				case VIRTIO_DS_DRIVER_OK:					//	5
					QVM_LOG(VIOLOG_DEBUG,"FE probe successfully 	===GinPot");
					vio_setup_queues(&dev->vs);				//Get host virtual addresses for the virtqueue rings
					if(pthread_create(&tid, NULL, &data_transfer, (void *)vdev))
						QVM_LOG(VIOLOG_DEBUG,"create thread data_transfer failed.	===GinPot");
					break;
				case VIRTIO_DS_FEATURES_OK:					//	4
					QVM_LOG(VIOLOG_DEBUG,"FE set features ok	===GinPot");
					break;
				case VIRTIO_DS_DEVICE_NEEDS_RESET:
					QVM_LOG(VIOLOG_DEBUG,"FE needs reset	===GinPot");
				case VIRTIO_DS_FAILED:
					QVM_LOG(VIOLOG_DEBUG,"FE set status failed	===GinPot");
					break;
				default:
					break;
			}
			break;
		case VIOREF_CLASS_DEVCFG:
			QVM_LOG(VIOLOG_DEBUG,"FE devcfg	===GinPot");
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
 static int vio_test_control(qvm_vdev_t *const vdp, unsigned const ctrl, const char *const arg)
 {
	int ret = EOK;
	vio_test_dev_t *dev = vdp->v_device;	//QVM should have created vio_test_dev_t this space size is not initialized
	//char src_intr[50];
	char *ret_intr;
	
	if(dev->status == NOT_RUNNING)
		return ret;
	
	switch(ctrl){							//Lifecycle of a vdev： http://www.qnx.com/developers/docs/7.0.0/index.html#com.qnx.doc.hypervisor.vdev/topic/intro/lifecycle.html
		case VDEV_CTRL_OPTIONS_START:
			QVM_LOG(VIOLOG_DEBUG,"VDEV_CTRL_OPTIONS_START	===GinPot");
			/*intr and loc, looked after by the qvm process,*/
			vdp->v_block.type = QST_MEMORY;	//guest physical address,type must setting.
			//vdp->v_block.location = QSL_NO_LOCATION;
			break;
			
		case (VDEV_CTRL_FIRST_OPTIDX + OPTION_VIO_VERBOSE):
			g_verbose = strtoul(arg, NULL, 10);
			g_verbose = g_verbose > VIOLOG_DEBUG ? VIOLOG_DEBUG : g_verbose;
			//QVM_LOG(VIOLOG_DEBUG,"g_verbose = %u	===GinPot", g_verbose);
			break;
			
		case (VDEV_CTRL_FIRST_OPTIDX + OPTION_VIO_DATA):
			//QVM_LOG(VIOLOG_DEBUG,"data = %s	===GinPot", arg);
			break;
		
		case VDEV_CTRL_OPTIONS_END:
			//QVM_LOG(VIOLOG_DEBUG,"VDEV_CTRL_OPTIONS_END	===GinPot");
			vio_init(vdp, VIOVER_100, &test_vdesc, &dev->vs);
			ret = vio_init_queue(&dev->vs, 0, 0x100);
			if(ret != EOK){
				QVM_LOG(VIOLOG_DEBUG,"vio_init_queue fail	===GinPot");
				break;
			}
			//QVM parses memory and interrupt information automatically
			QVM_LOG(VIOLOG_DEBUG,"111print the vdp->v_intr.pic == 0x%x" " :: vdp->v_intr.param = %d src_id=%d   reg=0x%x  ===GinPot" , (uintptr_t)vdp->v_intr.pic,vdp->v_intr.param, vdp->v_intr.src_id,vdp->v_block.location);
            //if (NULL == vdp->v_intr.pic) {
            //    //snprintf(src_intr, sizeof(src_intr),"gic:%lu","61");
			//	//QVM_LOG(VIOLOG_DEBUG,"irq:%s	===GinPot",src_intr);
            //    ret_intr = parse_intr("gic:61",&vdp->v_intr);
            //    if (ret_intr) {
            //        QVM_LOG(VIOLOG_DEBUG,"print the vdp->v_intr.pic == 0x%x" " :: vdp->v_intr.param = %d src_id=%d   reg=0x%x  ===GinPot" , (uintptr_t)vdp->v_intr.pic,vdp->v_intr.param, vdp->v_intr.src_id,vdp->v_block.location);
            //    } else {
            //        QVM_LOG(VIOLOG_DEBUG,"coudn't construct qvm_intr instance:  ===GinPot");
            //    }
            //}
		case VDEV_CTRL_PCI_BARS:
            QVM_LOG(VIOLOG_DEBUG,"pci bars");
			return vio_ctrl_pci_bars(&dev->vs, arg);
		default:
			break;
	}

	return QVRS_NORMAL;
 }


/**
 * Register the virtual device into the framework.	vdev vdev-virtio-test.so loc 0x1c270000 intr gic:61 verbose 3 data adc,	When Linux DTS sets interrupt, you need to subtract 32 from the GIC value here
    ginpot: virtio-test@1c270000 {
	    compatible = "virtio,mmio";
	    #address-cells = <1>;
	    #size-cells = <0>;
	    reg = <0x1c270000 0x100>;
	    interrupts = <0 29 0>;		//you need to subtract 32 from the GIC value here
	    status = "okay";
    };
 */
static void __attribute__((constructor)) vio_test_register(void)
{
	static const char * const vio_test_options[] = { "verbose", "data", NULL};
	
	static struct qvm_vdev_factory vio_test_factory = {
		.next			= NULL,
		.control		= vio_test_control,
		.vread			= vio_test_vread,		//This read-write function is called when the gest call to read and write memory trap into the host
		.vwrite			= vio_i2c_vwrite,
		.option_list	= vio_test_options,
		.name			= NULL,
		.factory_flags	= QVFF_INTR_NONPCI,
		.acc_sizes		= 1u << sizeof(uint32_t),
		.extra_space	= sizeof(vio_test_dev_t),
	};
	qvm_register_vdev_factory(&vio_test_factory, QVM_VDEV_ABI);
}
