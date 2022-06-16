/* =========================================================================
Copyright (c) 2017 by QUALCOMM Technologies Incorporated.  All Rights Reserved.
============================================================================ */
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/sysmsg.h>
#include <sys/iomsg.h>
#include <qvm/vdev.h>
#include <sys/iofunc.h>
#include <sys/resmgr.h>
#include <sys/dispatch.h>
#include <sys/neutrino.h>
#include <sys/procmgr.h>
#include "vdev_mmu_dcmd.h"
#include "mem_api.h"

#define BUFFER_SIZE			1024		///< Arbitrary buffer size
#define DEVNAME_MAX         128

typedef struct xpu_state {
    pthread_mutex_t	mux;
    pthread_t tid;
    uint32_t data;
}xpu_state_t;

typedef struct _xpu_dev {
    iofunc_attr_t  hdr;
    dispatch_t *dpp;
    dispatch_context_t *ctp;
    int pathID;
    qvm_vdev_t *vdev;
} xpu_dev_t;

typedef struct _mmus2_ocb {
    iofunc_ocb_t hdr;
    xpu_dev_t *dev;
} mmus2_ocb_t;


static int mmu_s2_map(uint64_t ipaddr, uint64_t paddr, uint32_t size, map_attr_t attr, qvm_vdev_t* vdev);
static int mmu_s2_unmap(uint64_t ipaddr, uint32_t size, qvm_vdev_t* vdev);
static int mmu_s1_unmap(uint64_t ipaddr, uint32_t size ) ;
static IOFUNC_OCB_T * _ocb_calloc(resmgr_context_t *ctp, IOFUNC_ATTR_T  *device);
static void _ocb_free(IOFUNC_OCB_T *ocb);
static int io_read (resmgr_context_t *ctp, io_read_t *msg, RESMGR_OCB_T *ocb);
static int io_write (resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T  *ocb);
static int io_devctl (resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T  *ocb);
static int mmu_s2_rm(void* args);
static void __attribute__((constructor)) mmus2_register(void);

static int mmu_s2_map(uint64_t ipaddr, uint64_t paddr, uint32_t size, map_attr_t attr, qvm_vdev_t* vdev)
{
    struct qvm_region rgn;
    uint32_t flags = QRF_PASS_EX;
    if (attr & MAP_ATTR_READ) {
        flags |= QRF_PASS_RD;
    }
    if (attr & MAP_ATTR_WRITE) {
        flags |= QRF_PASS_WR;
    }
    rgn.flags = flags; //QRF_PASS_RD|QRF_PASS_WR|QRF_PASS_EX;
    rgn.vdev = NULL;
    //rgn.remap = NULL;
    rgn.host_location = paddr;
    rgn.guest.type = QST_MEMORY;
    rgn.guest.location = ipaddr;
    rgn.guest.length = size;
    rgn.guest.flags = QSF_NONE;
    qvm_region_set(vdev->v_gsp, 1, &rgn);
    //qvm_report("map %lx->%lx, %x, %x\n", ipaddr, paddr, size, attr);
    
    return EOK;
}

static int mmu_s2_unmap(uint64_t ipaddr, uint32_t size, qvm_vdev_t* vdev)
{
    struct qvm_region rgn;
    rgn.flags = QRF_NONE;
    rgn.vdev = NULL;
    //rgn.remap = NULL;
    rgn.host_location = 0;
    rgn.guest.type = QST_MEMORY;
    rgn.guest.location = ipaddr;
    rgn.guest.length = size;
    rgn.guest.flags = QSF_NONE;
    qvm_region_set(vdev->v_gsp, 1, &rgn);
    //qvm_report("unmap %lx, %x\n", ipaddr, size);
   
    return EOK;
}

static int mmu_s1_unmap(uint64_t paddr, uint32_t size ) 
{
    uintptr_t va = 0; 

    if ( EOK == lookup_va_from_pa ( paddr, &va )){
        int const rc = munmap ( (void*) va , size ) ;
        if ( rc != EOK ) {
            qvm_report("munmap(va=0x%lx(pa=0x%lx),size=0x%x) failed err=%d", 
                    va, paddr, size, errno);
            return rc; 
        }
    }

    //qvm_report("pa=0x%lx, size=0x%x isn't mapped in s1", paddr ,size );

    //Not all addresses may be mapped in stage1 of qvm.
    //If we can't find a corresponding va, then return success.
    return EOK ;
}

static IOFUNC_OCB_T * _ocb_calloc(resmgr_context_t *ctp, IOFUNC_ATTR_T  *device)
{
    mmus2_ocb_t *ocb;
    if (NULL == (ocb = calloc(1, sizeof(*ocb)))) {
        return NULL;
        }

    ocb->dev = (xpu_dev_t*)device;
    return ((IOFUNC_OCB_T*)ocb);
}

static void _ocb_free(IOFUNC_OCB_T *ocb)
{
    free(ocb);
}

static int io_read (resmgr_context_t *ctp, io_read_t *msg, RESMGR_OCB_T *ocb)
{
    int nparts = 0;
    return (_RESMGR_NPARTS(nparts));
}

static int io_write (resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T  *ocb)
{
    int status;
    char* buf = NULL;
    mmus2_ocb_t* mmus2_ocb = (mmus2_ocb_t*)ocb;

    if ((status = iofunc_write_verify(ctp, msg, (iofunc_ocb_t*)ocb, NULL)) != EOK)
        return (status);

    if ((msg->i.xtype & _IO_XTYPE_MASK) != _IO_XTYPE_NONE)
        return(ENOSYS);

    _IO_SET_WRITE_NBYTES (ctp, msg->i.nbytes);

    if((msg->i.nbytes  > 20) ||
            (msg->i.nbytes != (ctp->size-sizeof(msg->i)))){
        /* Genuine io_write commands are typically small,
         * about 10 to 20 bytes. Set up an upper limit on the size,
         * so if client passes us an absurdly large value, we would simply
         * return EINVAL */
        return EINVAL;
    }

    buf = (char *) calloc(1,msg->i.nbytes + 1);
    if (buf == NULL){
        return(ENOMEM);
    }

    resmgr_msgread(ctp, buf, msg->i.nbytes, sizeof(msg->i));
    buf [msg->i.nbytes-1] = '\0'; /* just in case the text is not NULL terminated */

    //qvm_report("%s: %s\n", __func__, buf);
    // TODO : this is for test only
    if (0 == strncmp(buf, "lock", strlen("lock"))) {
        mmu_s2_unmap(0xb0000000, 0x8000000, mmus2_ocb->dev->vdev);
    } else if(0 == strncmp(buf, "unlock", strlen("unlock"))) {
        mmu_s2_map(0xb0000000, 0xb0000000, 0x8000000, MAP_ATTR_READ|MAP_ATTR_WRITE, mmus2_ocb->dev->vdev);
    } 

    free(buf);

    if (msg->i.nbytes > 0)
        mmus2_ocb->hdr.flags |= IOFUNC_ATTR_MTIME | IOFUNC_ATTR_CTIME;

    return (_RESMGR_NPARTS (0));
}

static int io_devctl (resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T  *ocb)
{
    mmus2_ocb_t* mmus2_ocb = (mmus2_ocb_t*)ocb;
    int nbytes=0;
    int status = iofunc_devctl_default(ctp, msg, (iofunc_ocb_t *)ocb);
    if (status != _RESMGR_DEFAULT){
        return status;
    }

    status =  iofunc_devctl_verify(ctp, msg,(iofunc_ocb_t*) ocb,
            _IO_DEVCTL_VERIFY_OCB_READ|_IO_DEVCTL_VERIFY_LEN);
    if (status != EOK)
    {
        qvm_report("devctl verify failed : %s\n",strerror(errno));
        return status;
    }

    nbytes=ctp->size-sizeof(msg->i);
    switch (msg->i.dcmd) {
        case _DCMD_VM_MMU_MAP:
            if (nbytes != sizeof(msg_vm_mmu_map_t)) {
                goto error;
            }
            msg_vm_mmu_map_t *m = (msg_vm_mmu_map_t *)_DEVCTL_DATA(msg->i);
            status = mmu_s2_map(m->cmd.ipaddr, m->cmd.paddr, m->cmd.size, m->cmd.attr, mmus2_ocb->dev->vdev);
            if ( status != EOK ) {
                return status ;
            }
            break;
        case _DCMD_VM_MMU_UNMAP:
            if (nbytes != sizeof(msg_vm_mmu_unmap_t)) {
                goto error;
            }
            msg_vm_mmu_unmap_t *um = (msg_vm_mmu_unmap_t *)_DEVCTL_DATA(msg->i);
            status = mmu_s2_unmap(um->cmd.ipaddr, um->cmd.size, mmus2_ocb->dev->vdev);
            if ( status != EOK ) {
                return status ;        
            }

            /* The memory being unmapped from s2 may originate from guest's system heap.
             * In all likelihood this memory would get xPU protected once this call path 
             * returns to the guest. Thus if this memory is also mapped on the s1 of qvm,
             * then unmap it to avoid xPU violations due to speculative fetches.
             *
             * Note that we don't bother re-mapping it on a subsequent _DCMD_VM_MMU_MAP call, 
             * since the host qvm is not expected to peek/poke into this CMA region  of guest's 
             * system heap. If this assumption is incorrect then we'd have large issue,
             * since we can't simply do a MAP_FIXED on the physical memory back to it's orginal 
             * virtual address */
            status = mmu_s1_unmap(um->cmd.ipaddr, um->cmd.size);
            if ( status != EOK ) {
                return status ;        
            }
            break;
        default:
            qvm_report("%s: unknown dcmd %d\n", __func__, msg->i.dcmd);
            break;
    }
error:
    return _RESMGR_PTR(ctp, msg, sizeof(msg->o) + msg->o.nbytes);
}

static int mmu_s2_rm(void* args)
{
    qvm_vdev_t *vdev = (qvm_vdev_t*) args;

    resmgr_attr_t        rattr;
    dispatch_t           *dpp;
    dispatch_context_t   *ctp;
    resmgr_connect_funcs_t  connect_funcs;
    resmgr_io_funcs_t       io_funcs;
    iofunc_funcs_t      ocb_funcs = { _IOFUNC_NFUNCS, _ocb_calloc, _ocb_free };
    iofunc_mount_t      mount = { 0, IOFUNC_PC_ACL, 0, 0, &ocb_funcs };

    xpu_dev_t *dev = (xpu_dev_t*)calloc(1, sizeof(xpu_dev_t));
    if (!dev) {
        qvm_report("%s: no memory\n", __func__);
        return -1;
    }
    dev->vdev = vdev;

    // initialize resource manager attributes
    memset(&rattr, 0, sizeof(resmgr_attr_t));

    // initialize functions for handling messages
    iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs,
            _RESMGR_IO_NFUNCS, &io_funcs);

    io_funcs.read   = io_read;
    io_funcs.write  = io_write;
    io_funcs.devctl = io_devctl;

    dpp = dispatch_create ();
    if (dpp == NULL) {
        qvm_report("%s:  couldn't dispatch_create: %s\n", __func__, strerror (errno));
        goto exit;
    }

    iofunc_attr_init (&dev->hdr, S_IFCHR | 0666, NULL, NULL);
    dev->hdr.mount = &mount;

    char devname[DEVNAME_MAX];
    qvm_vdev_path(vdev, devname, sizeof(devname));

    pthread_setname_np ( pthread_self() , devname ) ;

    int pathID = resmgr_attach (dpp, &rattr, devname, _FTYPE_ANY, 0,
            &connect_funcs, &io_funcs, ( RESMGR_HANDLE_T *)dev);
    if (pathID == -1) {
        qvm_report("%s:  resmgr_attach failed: %s\n", __func__, strerror (errno));
        goto exit;
    }
    dev->pathID = pathID;

    ctp = dispatch_context_alloc (dpp);
    if(ctp == NULL) {
        qvm_report("%s:  dispatch_context_alloc failed: %s\n", __func__, strerror (errno));
        goto exit;
    }

    while(1){
        ctp = dispatch_block (ctp);
        if(ctp){
            dispatch_handler (ctp);
        }else{
            goto exit;
        }
    }

exit:
    if (!dpp)
        free(dpp);
    if (!dev)
        free(dev);

    qvm_report("rm exiting\n");
    return -1;
}

/**
 * Handle process-container operations.
 * @param	vdp		virtual device
 * @param	ctrl	operation
 * @param	arg		parameters for the operation
 * @return	EOK on success, an error otherwise.
 */
static int
mmus2_control(qvm_vdev_t *const vdp, unsigned const ctrl, const char *const arg) {
    struct xpu_state *state = vdp->v_device;

    //qvm_report("%s, %u\n", __func__, ctrl);
    switch (ctrl) {
        case VDEV_CTRL_OPTIONS_START:
            vdp->v_block.type = QST_DEFAULT_TYPE_IO;
            break;
        case VDEV_CTRL_FIRST_OPTIDX:
            break;
        case VDEV_CTRL_OPTIONS_END:
            pthread_create(&state->tid, NULL, (void*)mmu_s2_rm, (void*)vdp);
            break;
        case VDEV_CTRL_STATUS:
            //qvm_report("data=%d\n", state->data);
            break;
        case VDEV_CTRL_TERMINATE:
            break;
        case VDEV_CTRL_GEN_FDT: ;
                                break;

        default:
                                break;
    }

    return EOK;
}

/**
 * Register the virtual device into the framework.
 */
    static void __attribute__((constructor))
mmus2_register(void)
{
    static struct qvm_vdev_factory mmus2_factory = {
        .next = NULL, // patched
        .control = mmus2_control,
        .vread = NULL,
        .vwrite = NULL,
        .pulse = NULL,
        .option_list = NULL,
        .name = NULL, // patched
        .factory_flags = QVFF_NONE,
        .acc_sizes = 1u << sizeof(uint32_t),
        .extra_space = sizeof(struct xpu_state)
    };
    qvm_register_vdev_factory(&mmus2_factory, QVM_VDEV_ABI);
}
