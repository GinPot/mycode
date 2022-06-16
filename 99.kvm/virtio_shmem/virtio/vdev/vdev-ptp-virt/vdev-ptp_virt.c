/**===========================================================================

  @file
        vdev-ptp_virt.c

  @brief
        Traps and emulate PTP timestamp read

  @details
        Traps and emulate PTP timestamp read

  @copyright
         Copyright (c) 2012-2017 QUALCOMM Technologies Incorporated.
         All rights reserved. Licensed Material - Restricted Use.
         Qualcomm Confidential and Proprietary.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header$
$DateTime$
$Author$
$Change$

when       who          what, where, why
--------   ---       ------------------------------------------------------
10/22/17   c_pbobde     Initial Version
==========================================================================*/


#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <qvm/vdev.h>
#include <sys/neutrino.h>
#include <inttypes.h>
#include <pthread.h>
#include <hw/inout.h>

#include <sys/resmgr.h>
#include <sys/ioctl.h>
#include <net/if_ether.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <hw/dcmd_qc_ethernet.h>

#define NTN_TS_ADDR_SPACE_SIZE             (0x1000)

/*Device specific state information*/
typedef struct _ptp_virt_dev_s {
    uint32_t          ptp_virt_base;
    paddr_t           ts_sec_addr;
    paddr_t           ts_nsec_addr;
    char              ifname[IF_NAMESIZE];
} ptp_virt_dev_t;

/*
   Initialize the mutex to synchronize access to ptp_virt_dev_t variables
   between vdev read requests.
   */
static pthread_mutex_t   vdev_mutex = PTHREAD_MUTEX_INITIALIZER;

/*IO mem region index*/
enum {
    REGION_PTP_VIRT_IDX,
};

/*vdev-ptp-virt command line options*/
enum {
    OPTION_PTP_VIRT_BASE,
    OPTION_PTP_VIRT_VERBOSE,
    OPTION_PTP_VIRT_IFNAME,
};

/*IO mem regions for trap and emulate*/
static struct qvm_region trap_regions [] = {
    [ REGION_PTP_VIRT_IDX ] = {
        .flags          = QRF_VIRT_RD | QRF_VIRT_WR ,
        .guest.location = 0,
        .guest.length   = __PAGESIZE,
        .guest.type     = QST_MEMORY,
        .guest.flags    = QSF_NONE,
    },
};

/*ptp_virt log macro*/
#define QVM_LOG(severity,fmt,...)\
    do { \
        struct timespec tp;\
        if ((severity) <= g_verbose) { \
            clock_gettime( CLOCK_REALTIME, &tp);\
            (void) qvm_report("%02d:%02d:%02d :: %s: " fmt,(int)(tp.tv_sec / (60 * 60)) , (int)((tp.tv_sec % (60 * 60)) / 60) , (int)((tp.tv_sec % (60 * 60) % 60)), __func__, ##__VA_ARGS__);\
        } \
    } while (0)

/* Macro to flip byte order */
#define FLIP_UINT32_BYTE_ORDER(data)\
    do {\
        int i, sz=sizeof(uint32_t);\
        uint8_t temp;\
        union {\
            uint32_t num;\
            uint8_t arr[4];\
        } bytes;\
        bytes.num = data;\
        for(i=0;i<sz/2;i++) {\
            temp = bytes.arr[i];\
            bytes.arr[i] = bytes.arr[sz-i-1];\
            bytes.arr[sz-i-1] = temp;\
        }\
        data = bytes.num;\
    } while(0);

/* Macro to truncate 16 LSBs*/
#define TRUNC(_a) (_a& (~(0x1000-1)))

/* Macro to collect 16 bit offset*/
#define OFF(_a) (_a & (0x1000-1))

/* Wrapper definitions for severity levels*/
enum _LOGGER_SEVERITY{
    PTPVIRTLOG_ERROR,          //unexpected recoverable error
    PTPVIRTLOG_WARNING,        //expected error
    PTPVIRTLOG_INFO,           //information
    PTPVIRTLOG_DEBUG,          //debug messages (e.g., normal detail).
};

/*log verbosity for qvm_report*/
static uint8_t g_verbose = PTPVIRTLOG_ERROR;

/**
 * Perform timestamp reg address read from ethernet
 * @param       reg_addr reg address structure
 * @param       ifname   ethernet interface name
 * @returns     int      status of reg read call
 */
static int drvspec_read_sys_time_req_addr(sys_time_reg_addr_t* reg_addr, const char *ifname)
{
    int socket_fd;
    struct ifdrv ifdrv;
    iov_t siov[3];
    io_devctl_t msg;
    int ret = 0;

    strlcpy(ifdrv.ifd_name, ifname, IF_NAMESIZE);
    ifdrv.ifd_cmd = DEVNP_ETH_SYS_TIME_REG_ADDR_RD_CMD;
    ifdrv.ifd_data = NULL;
    ifdrv.ifd_len = sizeof(sys_time_reg_addr_t);
    memset(&msg, 0, sizeof(msg));
    msg.i.type = _IO_DEVCTL;
    msg.i.combine_len = sizeof(msg.i);
    msg.i.dcmd = SIOCGDRVSPEC;
    msg.i.nbytes = IOCPARM_LEN((unsigned)SIOCGDRVSPEC);
    msg.i.zero = 0;
    memset(&siov, 0, sizeof(siov));
    SETIOV(&siov[0], &msg, sizeof(io_devctl_t));
    SETIOV(&siov[1], &ifdrv, sizeof(struct ifdrv));
    SETIOV(&siov[2], reg_addr, sizeof(sys_time_reg_addr_t));

    socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        QVM_LOG(PTPVIRTLOG_ERROR, "%s Failed to create socket\n", __func__);
        ret = -1;
    } else {
        ret = MsgSendv(socket_fd, siov, 3, siov, 3);
        if (ret == -1) {
            QVM_LOG(PTPVIRTLOG_ERROR, "%s Failed to collect reg addresses\n", __func__);
        }
        close(socket_fd);
    }

    return ret;
}

/**
 * Sets ptp timestamp addresses into device structure
 * @param    dev     ptp_virt device
 */
static void set_ptp_ts_reg_addr ( ptp_virt_dev_t *dev ) {
    int rc = 0;
    sys_time_reg_addr_t sys_time_reg_addr;
    uint32_t ts_reg_base_addr, ts_reg_base_addr_mapped;
    uint16_t off_sec, off_nsec;

    memset(&sys_time_reg_addr, 0, sizeof(sys_time_reg_addr_t));
    rc = drvspec_read_sys_time_req_addr(&sys_time_reg_addr, dev->ifname);

    if (rc == 0) {
        QVM_LOG(PTPVIRTLOG_DEBUG,"PTP Timestamp reg address:(sec): 0x%08x (nsec): 0x%08x\n",
                sys_time_reg_addr.sys_time_sec_reg_addr,
                sys_time_reg_addr.sys_time_nanosec_reg_addr);

        if(TRUNC(sys_time_reg_addr.sys_time_sec_reg_addr) ==
           (ts_reg_base_addr = TRUNC(sys_time_reg_addr.sys_time_nanosec_reg_addr))) {
            off_sec = OFF(sys_time_reg_addr.sys_time_sec_reg_addr);
            off_nsec = OFF(sys_time_reg_addr.sys_time_nanosec_reg_addr);
            ts_reg_base_addr_mapped = mmap_device_io(NTN_TS_ADDR_SPACE_SIZE, ts_reg_base_addr);
            if(ts_reg_base_addr_mapped == MAP_DEVICE_FAILED) {
                QVM_LOG(PTPVIRTLOG_ERROR,"Timestamp reg base address mapping failed");
                rc = -1;
			} else {
                dev->ts_sec_addr = ts_reg_base_addr_mapped + off_sec;
                dev->ts_nsec_addr = ts_reg_base_addr_mapped + off_nsec;
            }
        } else {
            dev->ts_sec_addr = mmap_device_io(4, sys_time_reg_addr.sys_time_sec_reg_addr);
            dev->ts_nsec_addr = mmap_device_io(4, sys_time_reg_addr.sys_time_nanosec_reg_addr);
            if(dev->ts_sec_addr == MAP_DEVICE_FAILED || dev->ts_nsec_addr == MAP_DEVICE_FAILED) {
                QVM_LOG(PTPVIRTLOG_ERROR,"Timestamp reg address mapping failed");
                rc = -1;
            }
        }
    } else {
        QVM_LOG(PTPVIRTLOG_ERROR,"PTP timestamp reg addresses not set\n");
    }
}

/**
 * Perform a vdev read operation for the device
 * @param       vdev    virtual device
 * @param       vopnd   state block for virtual device reference operand
 * @param       oopnd   state block for the 'other' operand
 * @param       gcp             guest cpu performing the reference
 * @returns QVRS_* value
 */
static enum qvm_vdev_ref_status ptp_virt_vread(qvm_vdev_t *const vdev,
        unsigned cookie , const struct qvm_state_block *const vopnd,
        const struct qvm_state_block *const oopnd,
        struct guest_cpu *const gcp)
{
    const ptp_virt_dev_t *dev = ( ptp_virt_dev_t* ) vdev->v_device;
    uint32_t ts[2];
    uint32_t nsec;

    memset(ts, 0, sizeof(ts));
    pthread_mutex_lock(&vdev_mutex);

    if(dev->ts_sec_addr == 0) {
        set_ptp_ts_reg_addr(( ptp_virt_dev_t* ) vdev->v_device);
    }

    if(dev->ts_sec_addr) {
        ts[1] = in32(dev->ts_nsec_addr);
        while(1) {
            ts[0] = in32(dev->ts_sec_addr);
            nsec = in32(dev->ts_nsec_addr);
            if(nsec > ts[1])
                break;

            ts[1] = nsec;
        }
    }

    qvm_cpu_write(gcp, QXF_NONE,  oopnd, 1, (void*)(&ts), sizeof(ts));
    pthread_mutex_unlock(&vdev_mutex);

    return QVRS_NORMAL;
}

/**
 * Perform a ptp_virt device specific state initialization
 * @param           dev     ptp_virt device
 * @returns         0 if ptp virt addr is set, else -1
 */
static int init_ptp_virt_device ( ptp_virt_dev_t *dev ) {
    if(!dev->ptp_virt_base)
    {
        QVM_LOG(PTPVIRTLOG_ERROR,"ptp_virt_addr not set");
        return -1;
    }
    trap_regions[REGION_PTP_VIRT_IDX].guest.location = dev->ptp_virt_base;
    dev->ts_sec_addr = 0;
    dev->ts_nsec_addr = 0;

    set_ptp_ts_reg_addr(dev);
    return 0;
}

/**
 * Handle process-container operations.
 * @param       vdp               virtual device
 * @param       ctrl                operation
 * @param       arg                parameters for the operation
 * @return          EOK on success, an error otherwise.
 */
static int ptp_virt_control(qvm_vdev_t *const vdp, unsigned const ctrl, const char *const arg)
{

    int rc = EOK;
    ptp_virt_dev_t *dev = vdp->v_device;
    int parent;
    int node;

    switch (ctrl) {
        case VDEV_CTRL_OPTIONS_START:
            /*initialize vdev state*/
            vdp->v_block.type = QST_DEFAULT_TYPE_IO;
            break;

        case VDEV_CTRL_OPTIONS_END:
            if ( EOK != ( rc = init_ptp_virt_device  ( dev )) )
            {
                QVM_LOG(PTPVIRTLOG_ERROR,"Couldn't initialize ptp_virt_device");
                break;
            }

            trap_regions[REGION_PTP_VIRT_IDX].vdev = vdp;
            //setup 'cookie' to array index for quick lookup in vread/vwrite
            trap_regions[REGION_PTP_VIRT_IDX].cookie = REGION_PTP_VIRT_IDX;

            qvm_region_set(vdp->v_gsp, 1, &trap_regions[0]);
            break;

        case (VDEV_CTRL_FIRST_OPTIDX + OPTION_PTP_VIRT_BASE):
            dev->ptp_virt_base = strtoul(arg, NULL, 16);
            QVM_LOG(PTPVIRTLOG_DEBUG,"ptp_virt_base = 0x%08x\n", dev->ptp_virt_base);
            break;

        case (VDEV_CTRL_FIRST_OPTIDX + OPTION_PTP_VIRT_VERBOSE):
            g_verbose = strtoul(arg, NULL, 16);

            if(PTPVIRTLOG_DEBUG < g_verbose)
            {
                g_verbose = PTPVIRTLOG_DEBUG;
            }
            QVM_LOG(PTPVIRTLOG_DEBUG,"g_verbose = %u \n", g_verbose);
            break;

        case (VDEV_CTRL_FIRST_OPTIDX + OPTION_PTP_VIRT_IFNAME):
            strlcpy(dev->ifname, arg, IF_NAMESIZE);
            QVM_LOG(PTPVIRTLOG_DEBUG,"ifname = %s \n", dev->ifname);
            break;

        case VDEV_CTRL_GEN_FDT:
            parent = (int)(uintptr_t)arg;
            node = gfdt_update_node(parent, "ptp_virtual", vdp->v_block.location);

            if(node > 0) {
                uint32_t dtprop = dev->ptp_virt_base;
                FLIP_UINT32_BYTE_ORDER(dtprop);
                gfdt_update_prop_strings(node, "compatible", "qcom,ptp_virtual", NULL);
                gfdt_update_property(node, "qcom,ptp-vdev-addr", &dtprop, sizeof(dtprop));
            }
            else
            {
                QVM_LOG(PTPVIRTLOG_ERROR,"ptp_virtual guest device tree not present\n");
            }
            break;

        case VDEV_CTRL_BOOTUP:
        case VDEV_CTRL_RESET:
        case VDEV_CTRL_TERMINATE:
        case VDEV_CTRL_STATUS:
        case VDEV_CTRL_GEN_ACPI:
        case VDEV_CTRL_POWER:
        default:
            break;
    }

    return rc ;
}

/**
 * Register the virtual device into the framework.
 */
static void __attribute__((constructor)) ptp_virt_register(void)
{
    static const char * const ptp_virt_options[] = { "ptp_virt_base", "verbose", "ifname", NULL };


    static struct qvm_vdev_factory ptp_virt_factory = {
        .next = NULL, // patched
        .control = ptp_virt_control,
        .vread = ptp_virt_vread,
        .vwrite = NULL,
        .option_list = ptp_virt_options,
        .name = NULL, // patched
        .factory_flags = QVFF_NONE,
        .acc_sizes = 1u << sizeof(uint32_t),
        .extra_space = sizeof(ptp_virt_dev_t),
    };
    qvm_register_vdev_factory(&ptp_virt_factory, QVM_VDEV_ABI);
}
