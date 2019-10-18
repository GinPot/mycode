/*
 * $QNXLicenseC:
 * Copyright 2016, QNX Software Systems.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

// Module Description: ATAPI interface

#include <sim_eide.h>

/*****************************************************************************/
#if defined(EIDE_ATAPI_DEBUG)
    #define atapi_log           eide_slogf
#else
    #define atapi_log(_a, _b, _c, _d, _fmt, ...)

    #if defined UNUSED
        #undef UNUSED
        #define UNUSED(_a)  (_a) = (_a)
    #endif

    #if defined EXEC_TRACE
        #undef EXEC_TRACE
        #define EXEC_TRACE
    #endif

#endif

/******************************************************************************/
int atapi_error(EIDE_TARGET *tgt, int scsi_status, int cam_status)
{
    CCB_SCSIIO      *ccb = tgt->nexus;

    EXEC_TRACE;
    if (ccb->cam_next_ccb) { /* Internal request failure (request sense) */
        ((CCB_SCSIIO *)ccb->cam_next_ccb)->cam_ch.cam_status = CAM_AUTOSENSE_FAIL;
    }

    return (eide_error(tgt, scsi_status, cam_status));
}

/******************************************************************************/
void atapi_ccb_init(EIDE_TARGET *tgt, CCB_SCSIIO *ccb, void (*cbfcnp)(),
        void *data, int dlen, void *sdata, int slen, int flags, int timeout)
{
    SIM_HBA             *hba;
    SIMQ_DATA           *pdata;
    struct timespec     time;

    EXEC_TRACE;
    hba     = tgt->hba;
    pdata   = (SIMQ_DATA *)ccb->cam_sim_priv;
    memset(ccb, 0, sizeof(*ccb));

    ccb->cam_ch.cam_func_code   = XPT_SCSI_IO;
    ccb->cam_ch.cam_target_id   = tgt->port;
    ccb->cam_ch.cam_flags       = flags;
    ccb->cam_timeout            = timeout;

    cam_set_pdrvr(ccb, tgt);
    ccb->cam_sense_len          = slen;
    ccb->cam_sense_ptr          = sdata;
    ccb->cam_dxfer_len          = dlen;
    ccb->cam_data.cam_data_ptr  = (paddr_t)(data);
    cam_set_cbf(ccb, cbfcnp);

    simq_clock_gettime(hba->simq, &time);
    pdata->timeout = time.tv_sec + timeout;
}

/******************************************************************************/
void atapi_spindle_cbf(CCB_SCSIIO *ccb)
{
    EIDE_TARGET     *tgt;
    CCB_DEVCTL      *dccb;
    eide_apm_t      *apm;

    EXEC_TRACE;
    tgt     = (EIDE_TARGET *)ccb->cam_pdrv_ptr;
    dccb    = (CCB_DEVCTL *)tgt->ts;
    apm     = (eide_apm_t *)(dccb->cam_devctl_data);

    atapi_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1, "%s:  ", __FUNCTION__);

    if (apm->level == EIDE_MODE_SLEEP) {
        tgt->tflags |= EIDE_TFLG_APM_SLEEP;
    }
    else {
        tgt->tflags |= EIDE_TFLG_APM_STANDBY_MEDIA;
    }
    dccb->cam_devctl_status = EOK;
    tgt->ts = NULL;
    tgt->nexus = (CCB_SCSIIO *)dccb;
    dccb->cam_ch.cam_status = CAM_REQ_CMP;
    eide_post_ccb(tgt, (CCB_SCSIIO *)dccb);
    xpt_ccb_free(ccb);
}

/******************************************************************************/
int atapi_spindle(EIDE_TARGET *tgt, CCB_SCSIIO *ccb, int loej, int start)
{
    SIM_HBA         *hba = tgt->hba;
    uint8_t         *cdb;

    EXEC_TRACE;
    tgt->ts = tgt->nexus; /* Save ccb */
    tgt->nexus = ccb;

    atapi_ccb_init(tgt, ccb, atapi_spindle_cbf,
                    NULL, 0, tgt->ws.buf, CCB_REQSENSE_MAX,
                    CAM_DIR_NONE | CAM_SIM_QFRZDIS,
                    hba->timeout);

    cdb = (uint8_t *)cam_cdb(ccb);
    ccb->cam_cdb_len = 6;
    cdb[0] = SC_SPINDLE;
    cdb[4] = (loej << 1) | start;
    atapi_io(tgt);

    return (CAM_REQ_INPROG);
}

/******************************************************************************/
int atapi_power_mode(EIDE_TARGET *tgt, int mode)
{
    CCB_SCSIIO      *ccb;

    EXEC_TRACE;
    UNUSED(mode);
    if ((ccb = xpt_ccb_alloc()) == NULL) {
        return (ENOMEM);
    }

    atapi_spindle(tgt, ccb, 0, 0);
    return (EINPROGRESS);
}

/******************************************************************************/
void atapi_internal_cbf(CCB_SCSIIO *ccb)
{
    EIDE_TARGET     *tgt;

    EXEC_TRACE;
    tgt = (EIDE_TARGET *)ccb->cam_pdrv_ptr;

    if ((ccb->cam_ch.cam_status & CAM_STATUS_MASK) != CAM_REQ_CMP) {
        atapi_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1, "%s:  request failed", __FUNCTION__);
    }
    xpt_ccb_free(ccb);
}

/******************************************************************************/
void atapi_setup_rsense(EIDE_TARGET *tgt)
{
    SIM_HBA         *hba = tgt->hba;
    CCB_SCSIIO      *ccb;
    CCB_HEADER      *cam_ch;
    CDB             *cdb;

    EXEC_TRACE;
    if ((ccb = xpt_ccb_alloc()) == NULL) {
        simq_ccb_state(tgt->nexus, SIM_CCB_DONE);
        tgt->nexus->cam_ch.cam_status = CAM_REQ_CMP_ERR;
        eide_post_ccb(tgt, tgt->nexus);
        return;
    }

    *ccb = *tgt->nexus; /* Copy ccb */
    cam_ch = &ccb->cam_ch;
    cam_ch->cam_func_code = XPT_SCSI_IO;
    /* Link internal ccb to current ccb */
    ccb->cam_next_ccb = (CCB_HEADER *)tgt->nexus;
    cam_set_cbf(ccb, atapi_internal_cbf);

    cam_set_pdrvr(ccb, tgt);
    cam_ch->cam_status      = CAM_AUTOSNS_VALID;
    ccb->cam_dxfer_len      = CCB_REQSENSE_MAX;
    cam_ch->cam_flags       = CAM_DIR_IN | CAM_DATA_PHYS | CAM_DIS_DISCONNECT |
                                CAM_SIM_QHEAD | CAM_SIM_QFRZDIS;
    ccb->cam_data.cam_data_ptr = tgt->ws.mdl[0].paddr;

    cdb = cam_cdb(ccb);

    memset(cdb, 0, IOCDBLEN);

    ccb->cam_cdb_len = 6;
    cdb->gen6.opcode = SC_RQ_SENSE;
    cdb->gen6.transfer_len = CCB_REQSENSE_MAX;

    simq_ccb_enqueue(hba->simq, ccb);
    tgt->nexus = NULL;
}

/******************************************************************************/
int atapi_io_cbf(EIDE_TARGET *tgt, uint8_t timeo)
{
    CCB_SCSIIO      *ccb = tgt->nexus;
    CCB_SCSIIO      *ccbl;
    CCB_HEADER      *cam_ch = (CCB_HEADER *)ccb;
    uint8_t         error;

    EXEC_TRACE;
    error = get_tf_error_reg(tgt);
    tgt->status = get_tf_status_reg(tgt);

    atapi_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, 5,
            "%s: status=%02x error=%02x cam_status=%02x "
            "cam_flags=%08x timeo =%d",
            __FUNCTION__, tgt->status, error, cam_ch->cam_status,
            cam_ch->cam_flags, timeo);

    if (timeo == CAM_REQ_CMP_ERR) {
        cam_ch->cam_status = CAM_REQ_CMP_ERR;
        eide_post_ccb(tgt, ccb);
        return (CAM_SUCCESS);
    }

    if ((tgt->status & EIDE_STATUS_BSY) || timeo == CAM_CMD_TIMEOUT) {
        atapi_error(tgt, SCS_GOOD, CAM_CMD_TIMEOUT);
        return (CAM_SUCCESS);
    }

    if ((tgt->status & EIDE_STATUS_ERR)) {
        ccb->cam_scsi_status            = SCS_CHECK;

        if (cam_ch->cam_status & CAM_AUTOSNS_VALID) {
            cam_ch->cam_status          = CAM_REQ_CMP;
            ccbl                        = (CCB_SCSIIO *)ccb->cam_next_ccb;
            ccbl->cam_ch.cam_status     = CAM_AUTOSENSE_FAIL;
            eide_post_ccb(tgt, ccb);
            return (CAM_SUCCESS);
        }

        if (!(cam_ch->cam_flags & CAM_DIS_AUTOSENSE)) {
            atapi_setup_rsense(tgt);
            return (CAM_SUCCESS);
        }
        cam_ch->cam_status = CAM_REQ_CMP_ERR;
        eide_post_ccb(tgt, ccb);
        return (CAM_SUCCESS);
    }

    if (cam_ch->cam_status & CAM_AUTOSNS_VALID) {
        cam_ch->cam_status      = CAM_REQ_CMP;
        ccbl                    = (CCB_SCSIIO *)ccb->cam_next_ccb;
        ccbl->cam_ch.cam_status = CAM_AUTOSNS_VALID | CAM_REQ_CMP_ERR;
        if (ds_atapi_sense(tgt, ccbl) != CAM_SUCCESS) {
        }
        memcpy(ccbl->cam_sense_ptr, tgt->ws.buf, ccb->cam_sense_len);
    }
    else {
        cam_ch->cam_status      = CAM_REQ_CMP;
        ccb->cam_scsi_status    = SCS_GOOD;
    }
    eide_post_ccb(tgt, ccb);
    return (CAM_SUCCESS);
}

/******************************************************************************/
void atapi_io(EIDE_TARGET *tgt)
{
    CCB_SCSIIO      *ccb = tgt->nexus;
    //SIM_EIDE_PDATA    *pdata;
    ATA_TASKFILE    tf;
    SG_ELEM         *sge = NULL;
    static SG_ELEM  sg_elem;
    int             sgc = 0;

    EXEC_TRACE;
    //pdata = (SIM_EIDE_PDATA *)ccb->cam_sim_priv;
    tgt->nfcnp = atapi_io_cbf;

    /* Create task file */
    memset(&tf, 0, sizeof(tf));
    tf.tflags   = ATA_TF_ATAPI | ATA_TF_IMMEDIATE |
                ((ccb->cam_ch.cam_flags & CAM_DIR_IN) ?
                ATA_TF_DIR_IN : ATA_TF_DIR_OUT);
    tf.command = EIDE_CMD_ATAPI_PKT_CMD;
    tf.lbam = 0x00;
    tf.lbah = 0x80;

    tf.feature  = (ccb->cam_dxfer_len &&
            (tgt->tflags & EIDE_TFLG_USE_BMSTR)) ? ATAPI_DMA : 0;
    if (ccb->cam_dxfer_len) {
        if (ccb->cam_ch.cam_flags & CAM_SCATTER_VALID) {
            sge = ccb->cam_data.cam_sg_ptr;
            sgc = ccb->cam_sglist_cnt;
        } else {
            sge = &sg_elem;
            sge->cam_sg_address = ccb->cam_data.cam_data_ptr;
            sge->cam_sg_count = ccb->cam_dxfer_len;
            sgc = 1;
        }
    }

    atapi_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, 5,
            "%s: cam_dxfer_len=%08x, cam_flags=%08x", __FUNCTION__,
            ccb->cam_dxfer_len, ccb->cam_ch.cam_flags);

#if defined(EIDE_ATAPI_DEBUG)
    //scsi_display_ccb(ccb, _SLOG_INFO);
#endif

    if (sata_command(tgt, &tf, ccb->cam_cdb_io.cam_cdb_bytes,
            ccb->cam_cdb_len, sge, sgc, 0)) {
        atapi_error(tgt, SCS_GOOD, CAM_CMD_TIMEOUT);
    }
}

/******************************************************************************/
int atapi_init(EIDE_TARGET *tgt)
{
    EXEC_TRACE;
    UNUSED(tgt);
    return (CAM_SUCCESS);
}

/******************************************************************************/
int atapi_dinit(EIDE_TARGET *tgt)
{
    EXEC_TRACE;
    UNUSED(tgt);
    return (CAM_SUCCESS);
}

/******************************************************************************/
/* This routine initializes an ATAPI device */
int atapi_init_device(EIDE_TARGET *tgt)
{
    EXEC_TRACE;
    UNUSED(tgt);
    return (CAM_SUCCESS);
}

/******************************************************************************/
int atapi_parse_identify(EIDE_TARGET *tgt, EIDE_IDENTIFY *ident)
{
    EXEC_TRACE;
    tgt->queue_depth++; /* Add one for request sense (dma prd) */

    tgt->device_type    = (ident->general_config >> 8) & 0x1f;

    tgt->tflags         |= EIDE_TFLG_ATAPI;

    return (CAM_SUCCESS);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/devb/rcarsata/sim_atapi.c $ $Rev: 814232 $")
#endif
