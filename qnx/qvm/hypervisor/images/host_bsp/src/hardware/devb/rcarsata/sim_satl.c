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

// Module Description: SCSI/ATA Translation

#include <sim_eide.h>

/*****************************************************************************/
#if defined(EIDE_SATL_DEBUG)
    #define satl_log            eide_slogf
#else
    #define satl_log(_a, _b, _c, _d, _fmt, ...)

    #if defined UNUSED
        #undef UNUSED
        #define UNUSED(_a)  (_a) = (_a)
    #endif

    #if defined EXEC_TRACE
        #undef EXEC_TRACE
        #define EXEC_TRACE
    #endif

#endif

/*****************************************************************************/
int eide_xlat_status(EIDE_TARGET *tgt, int rtype,
                        int status, int error, int *key, int *asc, int *ascq)
{
    EXEC_TRACE;
    *key = SK_NO_SENSE;
    *asc = 0;
    *ascq = 0;
    do {
        if (tgt->tflags & EIDE_TFLG_REMOVABLE) {
            /* Check for 0xff to handle PCMCIA removal */
            if (error == 0xff && status == 0xff) {
                tgt->tflags &= ~EIDE_TFLG_PRESENT;
                *key = SK_NOT_RDY;
                *asc = ASC_MEDIA_NOT_PRESENT;
                *ascq = 0;
                break;
            }
        }

        if (status & EIDE_STATUS_BSY) {
            *key = SK_NOT_RDY;
            *asc = ASC_NOT_READY;
            *ascq = 0;
            break;
        }

        if (status & EIDE_STATUS_DWF) {
            *key = SK_HARDWARE;
            *asc = ASC_INTERNAL_TARGET_FAILURE;
            *ascq = 0;
            break;
        }

        if (status & EIDE_STATUS_ERR) {
            if (tgt->tflags & EIDE_TFLG_REMOVABLE) {
                if (error & EIDE_ERR_ATA_NM) {
                    *key = SK_NOT_RDY;
                    *asc = ASC_MEDIA_NOT_PRESENT;
                    *ascq = 0;
                    break;
                }
                else if (error & EIDE_ERR_ATA_MC) {
                    *key = SK_UNIT_ATN;
                    *asc = ASC_MEDIUM_CHANGED;
                    *ascq = 0;
                    break;
                }
                else if (error & EIDE_ERR_ATA_MCR) {
                    *key = SK_UNIT_ATN;
                    *asc = ASC_OPERATOR_REQUEST;
                    *ascq = ASCQ_OPERATOR_MEDIUM_REMOVAL;
                    break;
                }
            }
            if (error & EIDE_ERR_ATA_CRC) {
                *key = SK_MEDIUM;
                *asc = ASC_UNRECOVERABLE_READ_ERROR;
                *ascq = ASCQ_UNRECOVERABLE_CIRC;
            }
            else if (error & EIDE_ERR_ATA_UNC) {
                *key = SK_MEDIUM;
                *asc = ASC_UNRECOVERABLE_READ_ERROR;
                *ascq = 0;
            }
            else if (error & EIDE_ERR_ATA_BBK) {
                *key = SK_MEDIUM;
                *asc = 0;
                *ascq = 0;
            }
            else if (error & (EIDE_ERR_ATA_IDNF)) {
                /* Invalid user-accessible addresses */
                *key = SK_MEDIUM;
                *asc = ASC_RECORD_NOT_FOUND;
                *ascq = ASCQ_RECORD_NOT_FOUND;
            }
            else if ((error & EIDE_ERR_ATA_WP) && (rtype & EIDE_RTYPE_WRITE)) {
                *key = SK_DATA_PROT;
                *asc = ASC_WRITE_PROTECTED;
                *ascq = 0;
            }
            else if (error & EIDE_ERR_ATA_ABRT) {
                *key = SK_CMD_ABORT;
                *asc = 0;
                *ascq = 0;
            }
        }
    } while (0);

    return (CAM_SUCCESS);
}

/*****************************************************************************/
int eide_ata_pthru_cbf(EIDE_TARGET *tgt, uint8_t timeo)
{
    SIM_EIDE_PDATA      *pdata;
    CCB_SCSIIO          *ccb = tgt->nexus;
    ATA_PASS_THRU       *pt;
    int                 key = 0;
    int                 asc = 0;
    int                 ascq = 0;
    int                 sflgs;
    int                 cstatus = CAM_REQ_CMP;

    EXEC_TRACE;
    pdata = (SIM_EIDE_PDATA *)ccb->cam_sim_priv;
    pt = (ATA_PASS_THRU *)&ccb->cam_cdb_io;

    switch (pt->protocol & ATA_PROTO_MSK) {
        case ATA_PROTO_UDMA_DATA_OUT:
        case ATA_PROTO_UDMA_DATA_IN:
        case ATA_PROTO_DMA:
        case ATA_PROTO_DEVICE_DIAGNOSTIC:
            break;

        case ATA_PROTO_PIO_DATA_OUT:
        case ATA_PROTO_PIO_DATA_IN:
            tgt->status = get_tf_status_reg(tgt);
            cstatus = (tgt->status & EIDE_STATUS_ERROR) ?
                                    CAM_REQ_CMP_ERR : CAM_REQ_CMP;
            break;

        case ATA_PROTO_DATA_NONE:
            break;
    }

    if (cstatus != CAM_REQ_INPROG) {
        sflgs = EIDE_SENSE_DESC;
        if (pt->protocol & ATA_PROTO_EXTEND) {
            sflgs |= EIDE_SENSE_EXTEND;
        }

        tgt->status = get_tf_status_reg(tgt);
        tgt->error = get_tf_error_reg(tgt);

        eide_xlat_status(tgt, pdata->rtype,
                    tgt->status, tgt->error, &key, &asc, &ascq);
        if ((tgt->status & EIDE_STATUS_ERR) ||
                                (pt->flags & ATA_FLG_CK_COND)) {
            cstatus = eide_sense(tgt, sflgs, key, asc, ascq, 0);
        }

        if ((tgt->status & EIDE_STATUS_BSY) ||
                ((tgt->status & EIDE_STATUS_DRQ) &&
                !(tgt->error & EIDE_ERR_ATA_ABRT)) || timeo == CAM_CMD_TIMEOUT) {
            eide_init_device(tgt, EIDE_INIT_RESET);
            cstatus = CAM_CMD_TIMEOUT;
        }

        ccb->cam_ch.cam_status  = cstatus;
        eide_post_ccb(tgt, ccb);
    }
    return (cstatus);
}

/*****************************************************************************/
int eide_ata_pthru(EIDE_TARGET *tgt, CCB_SCSIIO *ccb)
{
    SIM_EIDE_PDATA      *pdata;
    ATA_PASS_THRU       *pt;
    ATA_TASKFILE        tf;
    int                 key;
    int                 asc;
    int                 ascq;
    int                 sflgs;
    int                 cstatus;
    SG_ELEM             *sge;
    SG_ELEM             sg_elem;
    int                 sgc;

    EXEC_TRACE;
    pdata   = (SIM_EIDE_PDATA *)ccb->cam_sim_priv;
    sflgs   = EIDE_SENSE_DESC;
    pt      = (ATA_PASS_THRU *)&ccb->cam_cdb_io;
    key     = asc = ascq = 0;
    sgc     = 0;
    sge     = NULL;
    cstatus = CAM_REQ_CMP;
    if (pt->protocol & ATA_PROTO_EXTEND) {
        sflgs |= EIDE_SENSE_EXTEND;
    }

    memset(&tf, 0, sizeof(tf));
    switch (pt->protocol & ATA_PROTO_MSK) {
        case ATA_PROTO_RESPONSE:
            return (eide_sense(tgt, sflgs, SK_NO_SENSE, 0, 0, 0));

        case ATA_PROTO_FPDMA:
        case ATA_PROTO_UDMA_DATA_OUT:
        case ATA_PROTO_UDMA_DATA_IN:
        case ATA_PROTO_DEVICE_RESET:
            return (eide_sense(tgt, sflgs, SK_ILLEGAL,
                                        ASC_INVALID_FIELD, 0, 0));

        case ATA_PROTO_DEVICE_DIAGNOSTIC:
            break;

        case ATA_PROTO_DMA_QUEUED:
        case ATA_PROTO_DMA:
            return (eide_sense(tgt, sflgs, SK_ILLEGAL,
                                        ASC_INVALID_FIELD, 0, 0));

        case ATA_PROTO_PIO_DATA_OUT:
            pdata->rtype = EIDE_RTYPE_WRITE;
            break;
        case ATA_PROTO_PIO_DATA_IN:
        case ATA_PROTO_DATA_NONE:
            break;

        case ATA_PROTO_HRST:
        case ATA_PROTO_SRST:
            eide_init_device(tgt, EIDE_INIT_RESET);
            if (pt->flags & ATA_FLG_CK_COND) {
                return (eide_sense(tgt, sflgs, SK_NO_SENSE, 0, 0, 0));
            }
            return (CAM_REQ_CMP);

        default:
            return (eide_sense(tgt, sflgs, SK_ILLEGAL,
                                        ASC_INVALID_FIELD, 0, 0));
    }

    // special case a couple of commands
    switch (pt->command) {
        case EIDE_CMD_ATAPI_PKT_CMD:
            return (eide_sense(tgt, sflgs, SK_ILLEGAL,
                                        ASC_INVALID_FIELD, 0, 0));
        default:
            break;
    }

    if (pt->protocol & ATA_MCOUNT_MSK) {
        if (tgt->rw_multiple != (1 << (pt->protocol >> 5))) {
            return (eide_sense(tgt, sflgs, SK_ILLEGAL,
                                        ASC_INVALID_FIELD, 0, 0));
        }
    }

    tgt->nfcnp      = eide_ata_pthru_cbf;
    tf.tflags       = ATA_TF_ATA | ATA_TF_IMMEDIATE;
    tf.tflags       |= (pt->protocol & ATA_PROTO_EXTEND) ? ATA_TF_EXP : 0;
    if ((ccb->cam_ch.cam_flags & CAM_DIR_NONE) != CAM_DIR_NONE) {
        tf.tflags       |= ((ccb->cam_ch.cam_flags & CAM_DIR_IN) ?
                                    ATA_TF_DIR_IN : ATA_TF_DIR_OUT);
    }
    tf.control      = pt->control;
    tf.device       = pt->device;
    tf.command      = pt->command;
    tf.feature      = pt->features;
    tf.hob_feature  = pt->efeatures;
    tf.seccnt       = pt->sector_count;
    tf.hob_seccnt   = pt->esector_count;
    tf.lbal         = pt->lba_low;
    tf.hob_lbal     = pt->elba_low;
    tf.lbam         = pt->lba_mid;
    tf.hob_lbam     = pt->elba_mid;
    tf.lbah         = pt->lba_high;
    tf.hob_lbah     = pt->elba_high;

    if (ccb->cam_dxfer_len) {
        if (ccb->cam_ch.cam_flags & CAM_SCATTER_VALID) {
            sge = ccb->cam_data.cam_sg_ptr;
            sgc = ccb->cam_sglist_cnt;
        }
        else {
            sge                 = &sg_elem;
            sge->cam_sg_address = ccb->cam_data.cam_data_ptr;
            sge->cam_sg_count   = ccb->cam_dxfer_len;
            sgc                 = 1;
        }
    }

    if (sata_command(tgt, &tf, NULL, 0, sge, sgc,
                                ccb->cam_timeout) == CAM_SUCCESS) {
        return (CAM_REQ_INPROG);
    }

    tgt->status = get_tf_status_reg(tgt);
    tgt->error = get_tf_error_reg(tgt);

    eide_xlat_status(tgt, pdata->rtype,
                        tgt->status, tgt->error, &key, &asc, &ascq);
    if ((tgt->status & EIDE_STATUS_ERR) ||
                            (pt->flags & ATA_FLG_CK_COND)) {
        cstatus = eide_sense(tgt, sflgs, key, asc, ascq, 0);
    }

    if (tgt->status & (EIDE_STATUS_BSY | EIDE_STATUS_DRQ)) {
        eide_init_device(tgt, EIDE_INIT_RESET);
        cstatus = CAM_CMD_TIMEOUT;
    }

    return (cstatus);
}

/*****************************************************************************/
int eide_mp_control_pata(EIDE_TARGET *tgt,
                                    int pc, MODE_PARM_HEADER10 *mph, int opt)
{
    SIM_HBA             *hba;
    SIM_EIDE_EXT        *ext;
    EIDE_IDENTIFY       *ident;
    MPAGE_CONTROL_PATA  *mp;
    int                 pio;
    int                 mdma;
    int                 udma;

    EXEC_TRACE;
    hba     = tgt->hba;
    ext     = (SIM_EIDE_EXT *)hba->ext;
    mp      = (MPAGE_CONTROL_PATA *)(mph + 1);
    ident   = &tgt->identify;

    satl_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, 5,
                        "%s:  %s", __FUNCTION__, opt ? "select" : "sense");

    if (opt) {
        pio     = mp->mwdma_pio & 0x3;
        mdma    = (mp->mwdma_pio & 0x70) >> 4;
        udma    = mp->udma & 0x7f;

        if ((mdma || udma) && !(ext->eflags & EIDE_EFLG_USE_BMSTR)) {
            return (eide_sense(tgt, EIDE_SENSE_FIXED,
                                SK_ILLEGAL, ASC_INVALID_FIELD, 0, 0));
        }

        if (udma) {
            if (!(tgt->tflags & EIDE_TFLG_USE_UDMA) ||
                    !((ident->udma_modes & 0x7f) & udma)) {
                return (eide_sense(tgt, EIDE_SENSE_FIXED,
                                SK_ILLEGAL, ASC_INVALID_FIELD, 0, 0));
            }
            tgt->udma_mode  = eide_bsr(udma);
        }
        if (mdma) {
            if (!(tgt->tflags & EIDE_TFLG_USE_MDMA) ||
                    !((ident->multiword_dma & 0x07) & mdma)) {
                return (eide_sense(tgt, EIDE_SENSE_FIXED,
                                SK_ILLEGAL, ASC_INVALID_FIELD, 0, 0));
            }
            tgt->mdma_mode  = eide_bsr(mdma);
        }
        if (pio) {
            if (!(pio & (ident->pio_modes & 0x03)) ||
                    (eide_bsr(pio) + 3) < tgt->pio_mode) {
                return (eide_sense(tgt, EIDE_SENSE_FIXED,
                                SK_ILLEGAL, ASC_INVALID_FIELD, 0, 0));
            }
            tgt->pio_mode = eide_bsr(pio) + 3;
        }

        if (eide_init_device(tgt, EIDE_INIT_NORESET)) {
        }
    }
    else {
        memset(mp, 0, sizeof(MPAGE_CONTROL_PATA));
        mp->pc_page = MP_CONTROL;
        mp->subpage = MSP_CONTROL_PATA;
        UNALIGNED_PUT16(mp->page_length, ENDIAN_BE16(4));
        switch (pc) {
            case PC_CURRENT:            // return current setting
                if (tgt->pio_mode != -1 && tgt->pio_mode > 2) {
                    mp->mwdma_pio = 0x3 >> (4 - tgt->pio_mode);
                }

                if (tgt->mdma_mode != -1) {
                    mp->mwdma_pio |= ((0x7 >> (2 - tgt->mdma_mode)) << 4);
                }

                if (tgt->udma_mode != -1) {
                    mp->udma = 0x7f >> (6 - tgt->udma_mode);
                }
                break;

            case PC_CHANGEABLE:         // return chipset capabilities
                mp->mwdma_pio = 0x3;
                mp->mwdma_pio |= (0x7 << 4);
                mp->udma = 0x7f;
                break;

            case PC_DEFAULT:
            case PC_SAVED:
                return (eide_sense(tgt, EIDE_SENSE_FIXED,
                                    SK_ILLEGAL, ASC_INVALID_FIELD, 0, 0));
        }
    }
    return (CAM_REQ_CMP);
}

/*****************************************************************************/
int eide_log_sense(EIDE_TARGET *tgt, CCB_SCSIIO *ccb)
{
    int                 status;

    EXEC_TRACE;
    status  = CAM_PROVIDE_FAIL;

    return (status);
}

/*****************************************************************************/
int eide_log_select(EIDE_TARGET *tgt, CCB_SCSIIO *ccb)
{
    int                 status;

    EXEC_TRACE;
    status  = CAM_PROVIDE_FAIL;

    return (status);
}

/*****************************************************************************/
int eide_mode_sense(EIDE_TARGET *tgt, CCB_SCSIIO *ccb)
{
    MODE_SENSE              *ms;
    MODE_PARM_HEADER10      *mph;
    int                     status;

    EXEC_TRACE;
    ms      = (MODE_SENSE *)ccb->cam_cdb_io.cam_cdb_bytes;
    status  = CAM_PROVIDE_FAIL;

    if ((mph = (MODE_PARM_HEADER10 *)mmap(NULL, ccb->cam_dxfer_len,
            PROT_READ | PROT_WRITE | PROT_NOCACHE, MAP_PHYS | MAP_SHARED, NOFD,
            ccb->cam_data.cam_data_ptr)) == MAP_FAILED) {
        return (CAM_REQ_CMP_ERR);
    }

    switch (ms->pc_page & MPAGE_PAGE_MSK) {
        case MP_CONTROL:
            if (ms->subpage == MSP_CONTROL_PATA) {
                status = eide_mp_control_pata(tgt,
                                        ms->pc_page & PC_MSK, mph, 0);
            }
            break;

        default:
            break;
    }
    munmap(mph, ccb->cam_dxfer_len);
    return (status);
}

/*****************************************************************************/
int eide_mode_select(EIDE_TARGET *tgt, CCB_SCSIIO *ccb)
{
    MPAGE_CONTROL_PATA      *mp;
    MODE_PARM_HEADER10      *mph;
    int                     status;

    EXEC_TRACE;
    if ((mph = (MODE_PARM_HEADER10 *)mmap(NULL, ccb->cam_dxfer_len,
            PROT_READ | PROT_WRITE | PROT_NOCACHE, MAP_PHYS | MAP_SHARED, NOFD,
            ccb->cam_data.cam_data_ptr)) == MAP_FAILED) {
        return (CAM_REQ_CMP_ERR);
    }

    status  = CAM_PROVIDE_FAIL;
    mp      = (MPAGE_CONTROL_PATA *)(mph + 1);

    switch (mp->pc_page & MPAGE_PAGE_MSK) {
        case MP_CONTROL:
            if (mp->subpage == MSP_CONTROL_PATA) {
                status = eide_mp_control_pata(tgt, 0, mph, 1);
            }
            break;

        default:
            break;
    }
    munmap(mph, ccb->cam_dxfer_len);
    return (status);
}

/*****************************************************************************/
int eide_satl(EIDE_TARGET *tgt, CCB_SCSIIO *ccb)
{
    int             status;
    uint8_t         log_level = 5;//0;//5;

    EXEC_TRACE;

    switch (ccb->cam_cdb_io.cam_cdb_bytes[0]) {
        case SC_LSELECT:
            status = eide_log_select(tgt, ccb);

            satl_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, log_level,
                    "%s : cmd = %02x (%s) status = %08x", __FUNCTION__,
                    ccb->cam_cdb_io.cam_cdb_bytes[0], "SC_LSELECT", status);

            break;

        case SC_LSENSE:
            status = eide_log_sense(tgt, ccb);

            satl_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, log_level,
                    "%s : cmd = %02x (%s) status = %08x", __FUNCTION__,
                    ccb->cam_cdb_io.cam_cdb_bytes[0], "SC_LSENSE", status);

            break;

        case SC_MSELECT10:
            status = eide_mode_select(tgt, ccb);

            satl_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, log_level,
                    "%s : cmd = %02x (%s) status = %08x", __FUNCTION__,
                    ccb->cam_cdb_io.cam_cdb_bytes[0], "SC_MSELECT10", status);

            break;

        case SC_MSENSE10:
            status = eide_mode_sense(tgt, ccb);

            satl_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, log_level,
                    "%s : cmd = %02x (%s) status = %08x", __FUNCTION__,
                    ccb->cam_cdb_io.cam_cdb_bytes[0], "SC_MSENSE10", status);

            break;

        case SC_ATA_PT16:
            status = eide_ata_pthru(tgt, ccb);

            satl_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, log_level,
                    "%s : cmd = %02x (%s) status = %08x", __FUNCTION__,
                    ccb->cam_cdb_io.cam_cdb_bytes[0], "SC_ATA_PT16", status);

            break;

        default:
            status = CAM_PROVIDE_FAIL;
            break;
    }

    return (status);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/devb/rcarsata/sim_satl.c $ $Rev: 853240 $")
#endif
