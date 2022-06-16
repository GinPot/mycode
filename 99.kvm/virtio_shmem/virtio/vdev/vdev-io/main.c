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

#include "priv.h"
#include "vdev_io_dcmd.h"

typedef struct _io_dev_s {
    iofunc_attr_t  hdr;
    unsigned nregions;
    int *coid_p;
    const struct qvm_region *regions_p;
} io_dev_t;

typedef struct _io_ocb {
    iofunc_ocb_t hdr;
    int coid;
    const io_dev_t *dev;
} io_ocb_t;

static IOFUNC_OCB_T * _ocb_calloc(resmgr_context_t *ctp, IOFUNC_ATTR_T  *device);
static void purge_a_coid ( int *coid_array , unsigned nregions , int coid );
static void _ocb_free(IOFUNC_OCB_T *_ocb);
static int update_coid_array ( int *coid_array , int coid ,
        const struct qvm_region *region_p, unsigned nregions ,
        const struct _achunk *chunk_p, unsigned nchunks,
        int validated);
static int io_devctl (resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T  *_ocb);
static int vio_rm(void* args);

static IOFUNC_OCB_T * _ocb_calloc(resmgr_context_t *ctp, IOFUNC_ATTR_T  *device)
{
    io_ocb_t *ocb;
    if (NULL == (ocb = calloc(1, sizeof(*ocb)))) {
        return NULL;
    }

    ocb->dev = (io_dev_t*)device;
    ocb->coid = -1;
    return ((IOFUNC_OCB_T*)ocb);
}

static void purge_a_coid ( int *coid_array , unsigned nregions , int coid ) {
    for ( int i = 0 ; i < nregions ; i++ ) {
        if ( coid_array [i] == coid ) {
            coid_array [i] = 0;
        }
    }
}

static void _ocb_free(IOFUNC_OCB_T *_ocb)
{
    io_ocb_t* ocb = (io_ocb_t*)_ocb;

    if ( ocb->coid != -1 )  {
        purge_a_coid ( ocb->dev->coid_p , ocb->dev->nregions , ocb->coid ) ;
        (void ) ConnectDetach ( ocb->coid ) ;
    }
    free(ocb);
}

static int update_coid_array ( int *coid_array , int coid ,
        const struct qvm_region *region_p, unsigned nregions ,
        const struct _achunk *chunk_p, unsigned nchunks,
        int validated)
{
    int in_idx , rgn_idx = 0;

    for ( in_idx = 0 ; in_idx < nchunks ; in_idx ++ )
    {
        for ( ; rgn_idx < nregions ; rgn_idx ++ )
        {
            if ( (chunk_p[in_idx].pa == region_p[rgn_idx].guest.location) &&
                    ( chunk_p[in_idx].n == region_p[rgn_idx].guest.length >> 12))
            {
                if ( coid_array[rgn_idx] ) {
                    return EBUSY;
                }

                if ( validated ) {
                    coid_array[rgn_idx] = coid;
                }
                // looking good so far.
                break;
            }
        }

        /* Didn't find any hits for this chunk, error out */
        if ( rgn_idx >= nregions ) {
            return ERANGE;
        }
    }

    if ( validated ) {
        return EOK;
    } else {
        return update_coid_array ( coid_array , coid , region_p , nregions ,
                chunk_p , nchunks , 1);
    }
}

static int io_devctl (resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T  *_ocb)
{
    io_ocb_t* ocb = (io_ocb_t*)_ocb;
    int rc = iofunc_devctl_default(ctp, msg, (iofunc_ocb_t *)ocb);
    if (rc != _RESMGR_DEFAULT){
        qvm_report("devctl_default verify failed : %s\n",strerror(errno));
        return rc;
    }

    rc = iofunc_devctl_verify(ctp, msg, (IOFUNC_OCB_T *)_ocb,
            _IO_DEVCTL_VERIFY_LEN | _IO_DEVCTL_VERIFY_MSG_LEN |
            _IO_DEVCTL_VERIFY_OCB_READ | _IO_DEVCTL_VERIFY_OCB_WRITE );
    if (rc != EOK)
    {
        qvm_report("devctl_verify verify failed : %s\n",strerror(errno));
        return rc;
    }

    const int dcmd = msg->i.dcmd;
    switch (dcmd) {
        case _DCMD_REG_IO_FAULT:
            {
                /* Are there enough bytes to read the first two vectors ?*/
                if ( msg->i.nbytes < (sizeof (int) + sizeof (unsigned))){
                    return EINVAL;
                }

                const uint8_t *data =  (uint8_t*) _DEVCTL_DATA(msg->i) ;
                const int *chid = (int*) data;
                const unsigned *nchunks = (unsigned* )(data + sizeof (int));

                if ( *nchunks == 0 || *nchunks > 64 ) {
                    return EINVAL;
                }

                /* Now check if our computed msg size matches up*/
                if ( msg->i.nbytes != (
                            sizeof (int) +
                            sizeof (unsigned) +
                            *nchunks * sizeof (struct _achunk)))
                {
                    return EINVAL;
                }

                const struct _achunk *chunks_p = (struct _achunk* )(data +
                        sizeof (int) + sizeof ( unsigned ));
#if 0
                for ( int i = 0  ; i < *nchunks ; i++ ) {
                    qvm_report ("(%d pa=0x%lx size=0x%x)\n", i ,
                            chunks_p[i].pa ,
                            chunks_p[i].n * __PAGESIZE);
                }
#endif

                ocb->coid = ConnectAttach(0, ctp->info.pid, *chid,
                                _NTO_SIDE_CHANNEL,
                                _NTO_COF_CLOEXEC);
                if ( ocb->coid == -1 )
                {
                    qvm_report("ConnectAttach(pid=%d,coid=%d) failed\n",
                            ctp->info.pid , *chid ) ;
                    return EINVAL;
                }

                if ( EOK != ( rc = update_coid_array (
                                ocb->dev->coid_p , ocb->coid ,
                                ocb->dev->regions_p, ocb->dev->nregions ,
                                chunks_p , *nchunks , 0 )))
                {
                    (void ) ConnectDetach ( ocb->coid ) ;
                    return rc;
                }

                rc = EOK;
                break;
            }
        default:
            qvm_report("%s: unknown dcmd %d\n", __func__, dcmd ) ;
            break;
    }
    memset(&msg->o, 0, sizeof(msg->o));
    msg->o.nbytes  = 0;
    msg->o.ret_val = rc;

    return _RESMGR_PTR(ctp, msg, sizeof(msg->o) + msg->o.nbytes);
}

static int vio_rm(void* args)
{
    qvm_vdev_t *vdev = (qvm_vdev_t*) args;
    resmgr_attr_t        rattr;
    dispatch_t           *dpp;
    dispatch_context_t   *ctp;
    resmgr_connect_funcs_t  connect_funcs;
    resmgr_io_funcs_t       io_funcs;
    iofunc_funcs_t      ocb_funcs = { _IOFUNC_NFUNCS, _ocb_calloc, _ocb_free };
    iofunc_mount_t      mount = { 0, IOFUNC_PC_ACL, 0, 0, &ocb_funcs };
    char dev_path [64];
    io_dev_t *dev = vdev->v_device;

    dev->regions_p = get_io_regions ( &dev->nregions ) ;
    if ( NULL == ( dev->coid_p = calloc ( dev->nregions , sizeof ( int )) )) {
        goto out1;
    }

    qvm_vdev_path(vdev, dev_path, sizeof(dev_path));

    // initialize resource manager attributes
    memset(&rattr, 0, sizeof(resmgr_attr_t));
    rattr.nparts_max = 10;
    rattr.msg_max_size = 64 * 1024;

    // initialize functions for handling messages
    iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs,
            _RESMGR_IO_NFUNCS, &io_funcs);

    io_funcs.devctl = io_devctl;
    dpp = dispatch_create ();
    if (dpp == NULL) {
        qvm_report("%s:  couldn't dispatch_create: %s\n", __func__, strerror (errno));
        goto out2;
    }

    iofunc_attr_init (&dev->hdr, S_IFCHR | 0666, NULL, NULL);
    dev->hdr.mount = &mount;

    int rmid = resmgr_attach (dpp, &rattr, dev_path , _FTYPE_ANY, 0,
            &connect_funcs, &io_funcs, ( RESMGR_HANDLE_T *)dev);
    if (rmid == -1) {
        qvm_report("%s:  resmgr_attach failed: %s\n", __func__, strerror (errno));
        goto out3;
    }

    ctp = dispatch_context_alloc (dpp);
    if(ctp == NULL) {
        qvm_report("%s:  dispatch_context_alloc failed: %s\n", __func__, strerror (errno));
        goto out4;
    }

    (void) pthread_setname_np ( pthread_self ( ) , dev_path ) ;

    while(1){
        ctp = dispatch_block (ctp);
        if(ctp){
            dispatch_handler (ctp);
        }else{
            break;
        }
    }

    // shouldn't get here.
    (void ) dispatch_context_free ( ctp ) ;
out4:
    (void ) resmgr_detach ( dpp , rmid , _RESMGR_DETACH_ALL ) ;
out3:
    (void ) dispatch_destroy ( dpp ) ;
out2:
    free ( dev->coid_p) ;
out1:
    qvm_report("rm exiting\n");
    pthread_exit ( ( void * ) -1 ) ;
}

/**
 * Perform a vdev read operation for the device
 * @param       vdev    virtual device
 * @param       vopnd   state block for virtual device reference operand
 * @param       oopnd   state block for the 'other' operand
 * @param       gcp             guest cpu performing the reference
 * @returns QVRS_* value
 */
static enum qvm_vdev_ref_status vios2_vread(qvm_vdev_t *const vdev,
        unsigned cookie , const struct qvm_state_block *const vopnd,
        const struct qvm_state_block *const oopnd,
        struct guest_cpu *const gcp)
{
    int rc ;
    int coid ;
    uint32_t rval = ~0u;
    const io_dev_t *dev = ( io_dev_t* ) vdev->v_device;

    //qvm_report("%s, read on loc=%lx , cookie = %d\n", __func__, vopnd->location, cookie ) ;

    if ( ( coid = dev->coid_p[cookie] )) {
        const io_req_t req = { .read = 1 , .r = vopnd->location} ;
        rio_resp_t resp = { 0 };
        //TODO Set a time out of 1s on the blocking state.
        rc = MsgSend ( coid, &req, sizeof (req), &resp , sizeof( resp ));
        if ( rc == EOK ) {
            rval = resp;
        }
    }

    qvm_cpu_write(gcp, QXF_NONE,  oopnd, 1, &rval, sizeof(rval));
    return QVRS_NORMAL;
}


/**
 * Perform a vdev write operation for the device
 * @param       vdev    virtual device
 * @param       vopnd   state block for virtual device reference operand
 * @param       oopnd   state block for the 'other' operand
 * @param       gcp             guest cpu performing the reference
 * @returns QVRS_* value
 */
static enum qvm_vdev_ref_status vios2_vwrite(qvm_vdev_t *const vdev,
        unsigned cookie, const struct qvm_state_block *const vopnd,
        const struct qvm_state_block *const oopnd,
        struct guest_cpu *const gcp)
{
    int coid ;
    uint32_t wval;
    const io_dev_t *dev = ( io_dev_t* ) vdev->v_device;
    int rc = qvm_cpu_read(gcp, QXF_NONE,  oopnd, 1, &wval, sizeof(wval));
    if (rc != EOK) {
        return QVRS_ERRNO + rc;
    }

    if ( ( coid = dev->coid_p[cookie] )) {
        const io_req_t req = {
            .read = 0,
            .w={
                .addr = vopnd->location,
                .val = wval
            }
        };

        rc = MsgSend ( coid, &req, sizeof (req), NULL , 0);
        // do nothing on an error.
        (void ) rc ;
    }

    return QVRS_NORMAL;
}

/**
 * Handle process-container operations.
 * @param       vdp             virtual device
 * @param       ctrl    operation
 * @param       arg             parameters for the operation
 * @return      EOK on success, an error otherwise.
 */
static int
vios2_control(qvm_vdev_t *const vdp, unsigned const ctrl,
        const char *const arg)
{

    int rc = EOK;
    pthread_t tid;

    //qvm_report("%s, %u, for %s, factory=%p\n", __func__, ctrl, vdp->v_factory->name, vdp->v_factory);
    switch (ctrl) {
        case VDEV_CTRL_OPTIONS_START:
            {
                vdp->v_block.type = QST_DEFAULT_TYPE_IO;
                break;
            }
        case VDEV_CTRL_OPTIONS_END:
            {
                unsigned nregions;
                struct qvm_region *rgns = get_io_regions ( &nregions ) ;
                //qvm_report("VDEV_CTRL_OPTIONS_END: block loc = %lx\n",vdp->v_block.location ) ;
                for ( int i = 0 ; i < nregions ; i++ ) {
                    //setup vdp
                    rgns[i].vdev = vdp;
                    //setup 'cookie' to array index for quick lookup in vread/vwrite
                    rgns[i].cookie = i;
                }
                qvm_region_set(vdp->v_gsp, nregions, rgns);
                pthread_create(&tid, NULL, (void*)vio_rm, (void*)vdp);
                break;
            }
        case VDEV_CTRL_FIRST_OPTIDX:
        case VDEV_CTRL_STATUS:
        case VDEV_CTRL_TERMINATE:
        case VDEV_CTRL_GEN_FDT:
        default:
            break;
    }

    return rc ;
}

/**
 * Register the virtual device into the framework.
 */
static void __attribute__((constructor)) vios2_register(void)
{
    static struct qvm_vdev_factory vios2_factory = {
        .next = NULL, // patched
        .control = vios2_control,
        .vread = vios2_vread,
        .vwrite = vios2_vwrite,
        .option_list = NULL,
        .name = NULL, // patched
        .factory_flags = QVFF_NONE,
        .acc_sizes = 1u << sizeof(uint32_t),
        .extra_space = sizeof(io_dev_t),
    };
    qvm_register_vdev_factory(&vios2_factory, QVM_VDEV_ABI);
}
