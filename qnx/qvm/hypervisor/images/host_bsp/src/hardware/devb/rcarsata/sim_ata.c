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


// Module Description: ATA interface

#include <sim_eide.h>

/*****************************************************************************/
#if defined(EIDE_ATA_DEBUG)
    #define ata_log         eide_slogf
#else
    #define ata_log(_a, _b, _c, _d, _fmt, ...)

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
uint64_t ata_tf_lba(EIDE_TARGET *tgt, int extended)
{
    ATA_TASKFILE    tf;
    uint64_t        lba;

    EXEC_TRACE;

    sata_tf_read(tgt, &tf);

    if (extended) {
        lba = tf.lbal;
        lba |= (tf.hob_lbal << 24);
        lba |= (tf.lbam << 8);
        lba |= ((uint64_t)tf.hob_lbam << 32);
        lba |= (tf.lbah << 16);
        lba |= ((uint64_t)tf.hob_lbah << 40);
    }
    else {
        lba = tf.lbal;
        lba |= (tf.lbam << 8);
        lba |= (tf.lbah << 16);
        lba |= ((tf.device & 0xf) << 24);
    }
    return (lba);
}

/******************************************************************************/
uint64_t ata_lba_error(EIDE_TARGET *tgt)
{
    uint64_t        lba = 0;

    EXEC_TRACE;
    switch (tgt->cmd) {
        case EIDE_CMD_READ_SECTORS:
        case EIDE_CMD_READ_MULTIPLE:
        case EIDE_CMD_READ_VER_SECTORS:
        case EIDE_CMD_READ_DMA:
            break;

        case EIDE_CMD_READ_SECTORS_EXT:
        case EIDE_CMD_READ_MULTIPLE_EXT:
        case EIDE_CMD_READ_VER_SECTORS_EXT:
        case EIDE_CMD_READ_DMA_EXT:
            lba = ata_tf_lba(tgt, CAM_TRUE);
            break;

        case EIDE_CMD_FLUSH_CACHE:
            lba = ata_tf_lba(tgt, CAM_FALSE);
            break;

        case EIDE_CMD_FLUSH_CACHE_EXT:
            lba = ata_tf_lba(tgt, CAM_TRUE);
            break;

        default:
            lba = 0;
            break;
    }

    return (lba);
}

/******************************************************************************/
int ata_error(EIDE_TARGET *tgt, int scsi_status, int cam_status)
{
    CCB_SCSIIO      *ccb = tgt->nexus;
    uint8_t         error;
    uint8_t         status;
    uint32_t        lba;

    EXEC_TRACE;
    status = get_tf_status_reg(tgt);
    error = get_tf_error_reg(tgt);

    ata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 4,
        "%s:  status %x, error %x, cam_status %d", __FUNCTION__,
        status, error, cam_status);

    if (!(tgt->tflags & EIDE_TFLG_PRESENT)) {
        eide_sense(tgt, EIDE_SENSE_FIXED,
                SK_NOT_RDY, ASC_MEDIA_NOT_PRESENT, 0x00, 0x00);
        eide_post_ccb(tgt, ccb);
        return (CAM_SUCCESS);
    }

    if ((status & (EIDE_STATUS_BSY | EIDE_STATUS_ERR)) == EIDE_STATUS_ERR) {
        lba = ata_lba_error(tgt);
        if (ccb->cam_ch.cam_flags & CAM_DIR_OUT) {
            if (error & EIDE_ERR_ATA_WP) {
                eide_sense(tgt, EIDE_SENSE_FIXED, SK_DATA_PROT, 0x00, 0x00, 0x00);
                eide_post_ccb(tgt, ccb);
                return (CAM_SUCCESS);
            }
        }
        if (error & (EIDE_ERR_ATA_UNC | EIDE_ERR_ATA_BBK | EIDE_ERR_ATA_ABRT)) {
            eide_sense(tgt, EIDE_SENSE_FIXED, SK_MEDIUM, 0x00, 0x00, lba);
            eide_post_ccb(tgt, ccb);
            return (CAM_SUCCESS);
        }
    }

    return (eide_error(tgt, scsi_status, cam_status));
}

/******************************************************************************/
int ata_command(EIDE_TARGET *tgt, int cmd, int dev, int feat, int sc,
                int lbal, int lbam, int lbah, void *data, int dlen,
                int flgs, int (*cbf)(EIDE_TARGET *, uint8_t), int timeo)
{
    SG_ELEM         sge;
    ATA_TASKFILE    tf;

    EXEC_TRACE;
    memset(&tf, 0, sizeof(tf));
    tf.tflags = ATA_TF_ATA | (cbf ?
                            ATA_TF_IMMEDIATE : ATA_TF_INTR_WAIT) | flgs;
    tf.feature = feat;
    tf.seccnt = sc;
    tf.lbal = lbal;
    tf.lbam = lbam;
    tf.lbah = lbah;
    tf.hob_feature = feat >> 8;
    tf.hob_seccnt = sc >> 8;
    tf.hob_lbal = lbal >> 8;
    tf.hob_lbam = lbam >> 8;
    tf.hob_lbah = lbah >> 8;
    tf.device = dev;
    tf.command = cmd;

    tgt->nfcnp = cbf;

    sge.cam_sg_count = dlen;
    sge.cam_sg_address = (paddr_t)(data);

    return (sata_command(tgt, &tf, NULL, 0, &sge, dlen ? 1 : 0, timeo));
}

/******************************************************************************/
int ata_rw_cbf(EIDE_TARGET *tgt, uint8_t timeo)
{
    CCB_SCSIIO      *ccb = tgt->nexus;

    EXEC_TRACE;
    tgt->status = get_tf_status_reg(tgt);

    if ((tgt->status & (EIDE_STATUS_BSY |
            EIDE_STATUS_ERR | EIDE_STATUS_DRQ)) || timeo == CAM_CMD_TIMEOUT) {
        ata_error(tgt, SCS_GOOD, CAM_CMD_TIMEOUT);
        return (CAM_SUCCESS);
    }
    ccb->cam_ch.cam_status  = CAM_REQ_CMP;
    ccb->cam_scsi_status    = SCS_GOOD;
    eide_post_ccb(tgt, ccb);
    return (CAM_SUCCESS);
}

/******************************************************************************/
void ata_capacity(EIDE_TARGET *tgt, CCB_SCSIIO *ccb, int action)
{
    void            *dptr;
    READ_CAPACITY   *cp;
    READ_CAPACITY16 *cp16;
    EIDE_IDENTIFY   *ident;
    uint32_t        la;
    uint32_t        lps;
    uint8_t         log_level = 5;//0;//5;

    EXEC_TRACE;
    ident = &tgt->identify;

    if ((dptr = mmap(NULL, ccb->cam_dxfer_len,
            PROT_READ | PROT_WRITE | PROT_NOCACHE, MAP_PHYS | MAP_SHARED, NOFD,
            ccb->cam_data.cam_data_ptr)) == MAP_FAILED) {
        ccb->cam_ch.cam_status  = CAM_REQ_CMP_ERR;
        return;
    }

    ccb->cam_ch.cam_status  = CAM_REQ_CMP;
    ccb->cam_scsi_status    = SCS_GOOD;

    if (action == SAI_READ_CAPACITY_16) {
        cp16 = (READ_CAPACITY16 *)dptr;
        /* Returned logical block address */
        cp16->lba = ENDIAN_BE64(tgt->geometry.sectors);
        /* Block length in bytes (512) */
        cp16->blk_size = ENDIAN_BE32(512);

        la = 0;
        lps = 0;
        if (IDENT_IS_FIELD_VALID(ident->phys_sctr_size)) {
            if (ident->phys_sctr_size & IDENT_PSS_MLP) {
                lps = ident->phys_sctr_size & 0xf;
            }
            if (IDENT_IS_FIELD_VALID(ident->lba_alignment)) {
                if (ident->lba_alignment & 0x3fff) {
                    la = (1 << lps) - (ident->lba_alignment & 0x3fff);
                }
            }
        }
        cp16->exponents = lps;
        cp16->tp_lalba[0] = la >> 8;
        cp16->tp_lalba[1] = la;
        if (IDENT_MAJOR_VERSION(ident->major_version) >= 7) {
            if ((ident->data_set_mgnt & IDENT_DSM_TRIM)) {
                cp16->tp_lalba[0] |= RD_CAP_TPE;
                if ((ident->additional_sup & (IDENT_AS_DRAT |
                        IDENT_AS_RZAT)) == (IDENT_AS_DRAT | IDENT_AS_RZAT)) {
                    cp16->tp_lalba[0] |= RD_CAP_TPRZ;
                }
            }
        }

        ata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, log_level,
            "%s action=%x lba = %08x:%08x blk_size=%08x exponents=%02x "
            "tp_lalba[0]=%02x tp_lalba[1]=%02x (%s)",
            __FUNCTION__, action,
            (uint32_t)(cp16->lba >> 32), (uint32_t)(cp16->lba),
            cp16->blk_size, cp16->exponents, cp16->tp_lalba[0],
            cp16->tp_lalba[1], (tgt->tflags & EIDE_TFLG_MEDIA_NOT_PRESENT) ?
            "Media NOT present" : "Media present");
    } else {
        cp = (READ_CAPACITY *)dptr;
        /* Returned logical block address */
        cp->lba = ENDIAN_BE32(tgt->geometry.sectors);
        /* Block length in bytes (512) */
        cp->blk_size = ENDIAN_BE32(512);

        ata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, log_level,
            "%s : action=%x lba = %08x blk_size=%08x (%s)",
            __FUNCTION__, action, cp->lba, cp->blk_size,
            (tgt->tflags & EIDE_TFLG_MEDIA_NOT_PRESENT) ?
            "Media NOT present" : "Media present");
    }

    if (tgt->tflags & EIDE_TFLG_MEDIA_NOT_PRESENT) {
        eide_sense(tgt, EIDE_SENSE_FIXED,
                    SK_NOT_RDY, ASC_MEDIA_NOT_PRESENT, 0, 0);
    }

    munmap(dptr, ccb->cam_dxfer_len);
}

/******************************************************************************/
int ata_std_inquiry(EIDE_TARGET *tgt, CCB_SCSIIO *ccb)
{
    SCSI_INQUIRY    *iptr;

    EXEC_TRACE;
    if ((iptr = (SCSI_INQUIRY *)mmap(NULL, ccb->cam_dxfer_len,
            PROT_READ | PROT_WRITE | PROT_NOCACHE, MAP_PHYS | MAP_SHARED, NOFD,
            ccb->cam_data.cam_data_ptr)) == MAP_FAILED) {
        return (ccb->cam_ch.cam_status = CAM_REQ_CMP_ERR);
    }

    ccb->cam_ch.cam_status  = CAM_REQ_CMP;
    ccb->cam_scsi_status    = SCS_GOOD;

    memset(iptr, 0, sizeof(*iptr));
    iptr->peripheral = D_DIR_ACC;
    iptr->rmb = (tgt->tflags & EIDE_TFLG_REMOVABLE) ? CAM_TRUE : CAM_FALSE;
    iptr->version = INQ_VER_SPC3;   // SPC-3
    iptr->adlen = 32;               // Additional Data Length
    memset(iptr->vend_id, 0x20, 8); // Vendor identification
    strncpy((char *)iptr->prod_rev, (char *)tgt->identify.firmware_revision, 4);
    strncpy((char *)iptr->prod_id, (char *)tgt->identify.model_number, 16);
    munmap(iptr, ccb->cam_dxfer_len);

    return (ccb->cam_ch.cam_status);
}

/******************************************************************************/
int ata_evpd_inquiry(EIDE_TARGET *tgt, CCB_SCSIIO *ccb)
{
    EIDE_IDENTIFY                   *ident = &tgt->identify;
    VPD_HEADER                      *vh;
    BLOCK_LIMITS                    *bl;
    IDENTIFICATION_DESCRIPTOR       *id;
    BLOCK_DEVICE_CHARACTERISTICS    *bdc;
    uint8_t                         *is;
    uint8_t                         *vpd;
    uint16_t                        lps;

    EXEC_TRACE;
    ccb->cam_scsi_status = SCS_GOOD;
    ccb->cam_ch.cam_status = CAM_REQ_CMP;

    if ((vpd = (uint8_t *)mmap(NULL, ccb->cam_dxfer_len,
            PROT_READ | PROT_WRITE | PROT_NOCACHE, MAP_PHYS | MAP_SHARED, NOFD,
            ccb->cam_data.cam_data_ptr)) == MAP_FAILED) {
        ccb->cam_ch.cam_status  = CAM_REQ_CMP_ERR;
        return (ccb->cam_ch.cam_status);
    }

    vh = (VPD_HEADER *)vpd;
    id = (IDENTIFICATION_DESCRIPTOR *)(vh + 1);
    is = (uint8_t *)(id + 1);
    bl = (BLOCK_LIMITS *)(vh+1);
    bdc = (BLOCK_DEVICE_CHARACTERISTICS *)(vh+1);
    memset(vpd, 0, ccb->cam_dxfer_len);

    switch (ccb->cam_cdb_io.cam_cdb_bytes[2]) {
        case INQ_PC_VPD_SUPPORTED: {
            uint8_t evp_sup[] = { D_DIR_ACC, INQ_PC_VPD_SUPPORTED, 0, 5,
                        INQ_PC_VPD_SUPPORTED, INQ_PC_UNIT_SERIAL_NUMBER,
                        INQ_PC_DEVICE_IDENTIFICATION, INQ_PC_BLOCK_LIMITS,
                        INQ_PC_BLOCK_DEVICE_CHARACTERISTICS };

            if (ccb->cam_dxfer_len < sizeof(evp_sup)) {
                eide_sense(tgt, EIDE_SENSE_FIXED,
                            SK_ILLEGAL, ASC_INVALID_FIELD, 0, 0);
                break;
            }
            memcpy(vpd, evp_sup, sizeof(evp_sup));
            break;
        }

        case INQ_PC_UNIT_SERIAL_NUMBER:
            if (ccb->cam_dxfer_len < (sizeof(*vh) + 20)) {
                eide_sense(tgt, EIDE_SENSE_FIXED,
                            SK_ILLEGAL, ASC_INVALID_FIELD, 0, 0);
                break;
            }
            vh->peripheral  = D_DIR_ACC;
            vh->page_code   = INQ_PC_UNIT_SERIAL_NUMBER;
            vh->page_length = ENDIAN_BE16(20);
            memcpy(vh + 1, ident->serial_number, 20);
            break;

        case INQ_PC_DEVICE_IDENTIFICATION:
            if ((ident->cmd_setfeat_dflt & IDENT_CFD_VALID) &&
                    (ident->cmd_setfeat_dflt & IDENT_CFD_WWN)) {
                if (ccb->cam_dxfer_len < (sizeof(*vh) + sizeof(*id) + 8)) {
                    eide_sense(tgt, EIDE_SENSE_FIXED,
                            SK_ILLEGAL, ASC_INVALID_FIELD, 0, 0);
                    break;
                }
                vh->peripheral      = D_DIR_ACC;
                vh->page_code       = INQ_PC_DEVICE_IDENTIFICATION;
                vh->page_length     = ENDIAN_BE16(12);
                id->assoc_idtype    = IDT_NAA;
                id->proto_codeset   = CS_IDENTIFIER_BINARY;
                id->identifier_length = 8;
                memcpy(is, ident->wwn, 8);
            }
            else {
                if (ccb->cam_dxfer_len >= (sizeof(*vh) +
                                            sizeof(*id) + 8 + 40 + 20)) {
                    eide_sense(tgt, EIDE_SENSE_FIXED,
                                SK_ILLEGAL, ASC_INVALID_FIELD, 0, 0);
                    break;
                }
                eide_sense(tgt, EIDE_SENSE_FIXED,
                                SK_ILLEGAL, ASC_INVALID_FIELD, 0, 0);
                vh->peripheral      = D_DIR_ACC;
                vh->page_code       = INQ_PC_DEVICE_IDENTIFICATION;
                vh->page_length     = ENDIAN_BE16(71);
                id->assoc_idtype    = IDT_T10;
                id->proto_codeset   = CS_IDENTIFIER_ASCII;
                id->identifier_length = 68;
                memcpy(is, "ATA     ", 8);
                memcpy(is + 8, ident->model_number, 40);
                memcpy(is + 48, ident->serial_number, 20);
            }
            break;

        case INQ_PC_BLOCK_LIMITS:
                if (ccb->cam_dxfer_len < (sizeof(*vh) + sizeof(*bl))) {
                eide_sense(tgt, EIDE_SENSE_FIXED,
                                SK_ILLEGAL, ASC_INVALID_FIELD, 0, 0);
                break;
            }
            vh->peripheral      = D_DIR_ACC;
            vh->page_code       = INQ_PC_BLOCK_LIMITS;
            vh->page_length     = ENDIAN_BE16(60);

            lps = 1;
            if (IDENT_IS_FIELD_VALID(ident->phys_sctr_size)) {
                if (ident->phys_sctr_size & IDENT_PSS_MLP) {
                    lps = 1 << (ident->phys_sctr_size & 0xf);
                }
            }
            bl->optimal_xfer_len_granularity = ENDIAN_BE16(lps);

            if (IDENT_MAJOR_VERSION(ident->major_version) >= 7 &&
                    (ident->data_set_mgnt & IDENT_DSM_TRIM)) {
                bl->max_unmap_lba_count = ENDIAN_BE32(EIDE_DSMR_MAX_LBA);
                bl->max_unmap_desc_count = ENDIAN_BE32(EIDE_WS_SIZE / 8);
                bl->optimal_unmap_granularity = ENDIAN_BE32(1);
            }
            break;

        case INQ_PC_BLOCK_DEVICE_CHARACTERISTICS:
            if (ccb->cam_dxfer_len < (sizeof(*vh) + sizeof(*bdc))) {
                eide_sense(tgt, EIDE_SENSE_FIXED,
                        SK_ILLEGAL, ASC_INVALID_FIELD, 0, 0);
                break;
            }
            vh->peripheral = D_DIR_ACC;
            vh->page_code = INQ_PC_BLOCK_DEVICE_CHARACTERISTICS;
            vh->page_length = ENDIAN_BE16(60);

            if (IDENT_MAJOR_VERSION(ident->major_version) >= 7 &&
                                        ident->rotation_rate != 0xffff) {
                bdc->medium_rotation_rate = ENDIAN_BE16(ident->rotation_rate);
            }

            if (IDENT_MAJOR_VERSION(ident->major_version) >= 7 &&
                                ident->nominal_form_factor != 0xffff) {
                bdc->nominal_form_factor = ident->nominal_form_factor;
            }
            break;

        default:
            eide_sense(tgt, EIDE_SENSE_FIXED,
                                    SK_ILLEGAL, ASC_INVALID_FIELD, 0, 0);
            break;
    }

    munmap(vpd, ccb->cam_dxfer_len);
    return (ccb->cam_ch.cam_status);
}

/******************************************************************************/
void ata_inquiry(EIDE_TARGET *tgt, CCB_SCSIIO *ccb)
{
    EXEC_TRACE;
    if (ccb->cam_cdb_io.cam_cdb_bytes[1] & INQ_OPT_EVPD) {
        ata_evpd_inquiry(tgt, ccb);
    }
    else {
        ata_std_inquiry(tgt, ccb);
    }
}

/******************************************************************************/
void ata_test_unit_ready(EIDE_TARGET *tgt, CCB_SCSIIO *ccb)
{
    int             status;

    EXEC_TRACE;
    ccb->cam_scsi_status    = SCS_GOOD;
    ccb->cam_ch.cam_status  = CAM_REQ_CMP;
    status = get_tf_status_reg(tgt);
    if (status & EIDE_STATUS_BSY) {
        eide_init_device(tgt, EIDE_INIT_RESET);
        eide_sense(tgt, EIDE_SENSE_FIXED, SK_NOT_RDY,
                (tgt->tflags & EIDE_TFLG_REMOVABLE) ?
                ASC_MEDIA_NOT_PRESENT : ASC_NOT_READY, 0x00, 0x00);
    }
    else if (!(status & EIDE_STATUS_DRDY)) {
        eide_sense(tgt, EIDE_SENSE_FIXED, SK_NOT_RDY,
                (tgt->tflags & EIDE_TFLG_REMOVABLE) ?
                ASC_MEDIA_NOT_PRESENT : ASC_NOT_READY, 0x00, 0x00);
    }
}

/******************************************************************************/
uint64_t ata_ccb_lba(uint8_t *cdb)
{
    uint64_t    lba;

    EXEC_TRACE;
    switch (cdb[0]) {
        case SC_READ10:
        case SC_WRITE10:
        case SC_READ12:
        case SC_WRITE12:
            lba = ENDIAN_BE32(UNALIGNED_RET32(&cdb[2]));
            break;

        case SC_READ16:
        case SC_WRITE16:
            lba = ENDIAN_BE64(UNALIGNED_RET64(&cdb[2]));
            break;

        default:
            lba = 0;
    }

    return (lba);
}

/******************************************************************************/
void ata_rw(EIDE_TARGET *tgt, CCB_SCSIIO *ccb)
{
    uint16_t        sector;
    uint32_t        cylinder;
    int             multi;
    uint32_t        nblks;
    uint64_t        lba;
    ATA_TASKFILE    tf;
    SG_ELEM         *sge;
    SG_ELEM         sg_elem;
    int             sgc;

    EXEC_TRACE;
    memset(&tf, 0, sizeof(tf));

    tgt->nfcnp  = ata_rw_cbf;
    lba         = ata_ccb_lba(ccb->cam_cdb_io.cam_cdb_bytes);
    nblks       = ccb->cam_dxfer_len / 512;
    multi       = (tgt->tflags & EIDE_TFLG_USE_MBLK) && nblks > 1;
    tf.tflags   = ATA_TF_ATA | ATA_TF_IMMEDIATE;
    tf.tflags   |= ((((lba + nblks) & 0xf0000000) ||
                                    nblks > 0xff) ? ATA_TF_EXP : 0);
    tf.tflags   |= ((ccb->cam_ch.cam_flags & CAM_DIR_IN) ?
                                    ATA_TF_DIR_IN : ATA_TF_DIR_OUT);

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

    if (tgt->tflags & EIDE_TFLG_USE_LBA) {
        if (tf.tflags & ATA_TF_EXP) {
            tf.hob_lbal = (lba >> 24) & 0xff;
            tf.hob_lbam = (lba >> 32) & 0xff;
            tf.hob_lbah = (lba >> 40) & 0xff;
            tf.hob_seccnt   = (nblks >> 8) & 0xff;
        }
        else {
            tf.device   = ((lba >> 24) & 0xf);
        }
        tf.device   |= EIDE_LBA_ADDR;
        tf.lbal = lba & 0xff;
        tf.lbam = (lba >> 8) & 0xff;
        tf.lbah = (lba >> 16) & 0xff;
    }
    else {
        sector      = (lba % tgt->geometry.sec_trk) + 1;
        cylinder    = lba / tgt->geometry.sec_trk;
        tf.device   = cylinder % tgt->geometry.heads;
        cylinder    /= tgt->geometry.heads;
        tf.lbal = sector;
        tf.lbam = cylinder & 0xff;
        tf.lbah = cylinder >> 8;
    }

    tf.seccnt       = (nblks & 0xff);

    if (tgt->tflags & EIDE_TFLG_USE_BMSTR) {
        if (tf.tflags & ATA_TF_DIR_IN) {
            tf.command = (tf.tflags & ATA_TF_EXP) ?
                            EIDE_CMD_READ_DMA_EXT : EIDE_CMD_READ_DMA;
        }
        else {
            tf.command = (tf.tflags & ATA_TF_EXP) ?
                            EIDE_CMD_WRITE_DMA_EXT : EIDE_CMD_WRITE_DMA;
        }
    }
    else {
        if (tf.tflags & ATA_TF_DIR_OUT) {
            if (tf.tflags & ATA_TF_EXP) {
                tf.command = multi ? EIDE_CMD_WRITE_MULTIPLE_EXT :
                            EIDE_CMD_WRITE_SECTORS_EXT;
            }
            else {
                tf.command = multi ? EIDE_CMD_WRITE_MULTIPLE :
                            EIDE_CMD_WRITE_SECTORS;
            }
        }
        else {
            if (tf.tflags & ATA_TF_EXP) {
                tf.command = multi ? EIDE_CMD_READ_MULTIPLE_EXT :
                            EIDE_CMD_READ_SECTORS_EXT;
            }
            else {
                tf.command = multi ? EIDE_CMD_READ_MULTIPLE :
                            EIDE_CMD_READ_SECTORS;
            }
        }
    }
    if (sata_command(tgt, &tf, NULL, 0, sge, sgc, 0)) {
        ata_error(tgt, SCS_GOOD, CAM_CMD_TIMEOUT);
        return;
    }
}

/******************************************************************************/
void ata_mpage_rw_err(EIDE_TARGET *tgt, CCB_SCSIIO *ccb, int op)
{
    MODE_PARM_HEADER10      *mph;
    SIM_HBA                 *hba = tgt->hba;

    EXEC_TRACE;
    if  ((mph = (MODE_PARM_HEADER10 *)mmap(NULL, ccb->cam_dxfer_len,
            PROT_READ | PROT_WRITE | PROT_NOCACHE, MAP_PHYS | MAP_SHARED, NOFD,
            ccb->cam_data.cam_data_ptr)) == MAP_FAILED) {
        ccb->cam_ch.cam_status  = CAM_REQ_CMP_ERR;
        return;
    }

    memset(mph, 0, sizeof(*mph));

    if (tgt->identify.cmdsets_sup & IDENT_CMDSETS_MEDIA_STATUS_SUP) {
        if (ata_command(tgt, EIDE_CMD_GET_MEDIA_STATUS, 0, 0, 0, 0, 0, 0,
                    NULL, 0, 0, NULL, hba->timeout) == CAM_SUCCESS) {

            tgt->status = get_tf_status_reg(tgt);
            tgt->error = get_tf_error_reg(tgt);

            if (!(tgt->status & EIDE_STATUS_ERR) &&
                                            (tgt->error & EIDE_ERR_ATA_WP)) {
                mph->device_specific |= MP_DS_WP;
            }
        }
    }

    ccb->cam_ch.cam_status = CAM_REQ_CMP;
    ccb->cam_scsi_status = SCS_GOOD;

    munmap(mph, ccb->cam_dxfer_len);
}

/******************************************************************************/
int ata_write_cache(EIDE_TARGET *tgt, int op)
{
    SIM_HBA     *hba = tgt->hba;
    int         stat;

    EXEC_TRACE;
    if (!(tgt->identify.cmdset_sup & IDENT_CMDSET_WRITE_CACHE)) {
        return (CAM_FAILURE);
    }

    if (op) {
        if ((stat = ata_command(tgt, EIDE_CMD_SET_FEATURE, 0,
                EIDE_FEAT_EN_WCACHE, 0,
                0, 0, 0, NULL, 0, 0, NULL, hba->timeout))) {
            ata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                    "%s:  Write Cache Enable failed", __FUNCTION__);
        }
        else {
            tgt->identify.cmd_feature_enb |= IDENT_CFE_WRITE_CACHE;
        }
    }
    else {

        if ((stat = ata_command(tgt, EIDE_CMD_SET_FEATURE, 0,
                EIDE_FEAT_DIS_WCACHE, 0,
                0, 0, 0, NULL, 0, 0, NULL, hba->timeout))) {
            ata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                    "%s:  Write Cache Disable failed", __FUNCTION__);
        }
        else {
            tgt->identify.cmd_feature_enb &= ~IDENT_CFE_WRITE_CACHE;
        }
    }

    return (stat);
}

/******************************************************************************/
void ata_mpage_caching(EIDE_TARGET *tgt, CCB_SCSIIO *ccb, int op)
{
    MPAGE_CACHING       *mpc;
    MODE_PARM_HEADER10  *mph;

    EXEC_TRACE;
    if ((mph = (MODE_PARM_HEADER10 *)mmap(NULL, ccb->cam_dxfer_len,
            PROT_READ | PROT_WRITE | PROT_NOCACHE, MAP_PHYS | MAP_SHARED, NOFD,
            ccb->cam_data.cam_data_ptr)) == MAP_FAILED) {
        ccb->cam_ch.cam_status  = CAM_REQ_CMP_ERR;
        return;
    }

    mpc                     = (MPAGE_CACHING *)(mph + 1);
    ccb->cam_scsi_status    = SCS_GOOD;
    ccb->cam_ch.cam_status  = CAM_REQ_CMP;

    if (op) {
        if (ata_write_cache(tgt, mpc->flags & MP_CACHE_WCE)) {
            eide_sense(tgt, EIDE_SENSE_FIXED,
                    SK_ILLEGAL, ASC_INVALID_FIELD, 0x00, 0x00);
        }
    }
    else {
        memset(mph, 0, ccb->cam_dxfer_len);
        mph->data_len[1] = 26;
        mpc->pc_page = MP_CACHING;
        mpc->page_length = 18;
        mpc->flags = (tgt->identify.cmd_feature_enb &
                    IDENT_CFE_WRITE_CACHE) ? MP_CACHE_WCE : 0;
    }
    munmap(mph, ccb->cam_dxfer_len);
}

/******************************************************************************/
int ata_mode_sense(EIDE_TARGET *tgt, CCB_SCSIIO *ccb)
{
    EXEC_TRACE;
    switch (ccb->cam_cdb_io.cam_cdb_bytes[2]) {
        case MP_RW_ERR:
            ata_mpage_rw_err(tgt, ccb, 0);
            break;

        case MP_CACHING:
            ata_mpage_caching(tgt, ccb, 0);
            break;

        case MP_SMART:
        default:
            eide_sense(tgt, EIDE_SENSE_FIXED,
                                SK_ILLEGAL, ASC_INVALID_FIELD, 0x00, 0x00);
            break;
    }

    return (ccb->cam_ch.cam_status);
}

/******************************************************************************/
int ata_mode_select(EIDE_TARGET *tgt, CCB_SCSIIO *ccb)
{
    MPAGE_CACHING       *mp;
    MODE_PARM_HEADER10  *mph;

    EXEC_TRACE;
    if ((mph = (MODE_PARM_HEADER10 *)mmap(NULL, ccb->cam_dxfer_len,
            PROT_READ | PROT_WRITE | PROT_NOCACHE, MAP_PHYS | MAP_SHARED, NOFD,
            ccb->cam_data.cam_data_ptr)) == MAP_FAILED) {
        return (ccb->cam_ch.cam_status = CAM_REQ_CMP_ERR);
    }

    mp = (MPAGE_CACHING *)(mph + 1);

    switch (mp->pc_page) {
        case MP_CACHING:
            ata_mpage_caching(tgt, ccb, 1); break;

        case MP_SMART:
        default:
            eide_sense(tgt, EIDE_SENSE_FIXED,
                    SK_ILLEGAL, ASC_INVALID_FIELD, 0x00, 0x00);
            break;
    }

    munmap(mph, ccb->cam_dxfer_len);
    return (ccb->cam_ch.cam_status);
}

/******************************************************************************/
int ata_sync_cbf(EIDE_TARGET *tgt, uint8_t timeo)
{
    CCB_SCSIIO      *ccb = tgt->nexus;

    EXEC_TRACE;
    tgt->status = get_tf_status_reg(tgt);

    if ((tgt->status & EIDE_STATUS_ERR) || timeo == CAM_CMD_TIMEOUT) {
        tgt->error = get_tf_error_reg(tgt);

        if ((tgt->error & EIDE_ERR_ATA_ABRT) &&
                (tgt->identify.cmdsets_sup & IDENT_CMDSETS_FLUSH_SUP)) {
            eide_sense(tgt, EIDE_SENSE_FIXED, SK_MEDIUM,
                        ASC_WRITE_ERROR, 0x00, ata_lba_error(tgt));
        }
        else {
            eide_sense(tgt, EIDE_SENSE_FIXED,
                            SK_ILLEGAL, ASC_INVALID_FIELD, 0x00, 0x00);
        }
    }
    else {
        ccb->cam_ch.cam_status = CAM_REQ_CMP;
        ccb->cam_scsi_status = SCS_GOOD;
    }

    eide_post_ccb(tgt, ccb);

    return (CAM_SUCCESS);
}

/******************************************************************************/
int ata_sync(EIDE_TARGET *tgt, CCB_SCSIIO *ccb)
{
    SIM_HBA         *hba = tgt->hba;
    int             cmd;

    EXEC_TRACE;
    UNUSED(ccb);
    cmd = (tgt->identify.cmdsets_sup & IDENT_CMDSETS_ADDR48_SUP) ?
                EIDE_CMD_FLUSH_CACHE/*_EXT*/ : EIDE_CMD_FLUSH_CACHE;
    if (ata_command(tgt, cmd, 0, 0, 0, 0, 0, 0, NULL, 0,
            0, ata_sync_cbf, hba->timeout)) {
        eide_sense(tgt, EIDE_SENSE_FIXED, SK_ILLEGAL,
                                ASC_INVALID_FIELD, 0x00, 0x00);
        return (CAM_REQ_CMP_ERR);
    }
    return (CAM_REQ_INPROG);
}

/******************************************************************************/
void ata_write_same(EIDE_TARGET *tgt, CCB_SCSIIO *ccb)
{
    SIM_HBA         *hba = tgt->hba;
    EIDE_WORKSPACE  *ws = &tgt->ws;
    CDB             *cdb;
    ATA_TASKFILE    tf;
    SG_ELEM         sge;
    uint64_t        lba;
    uint32_t        nlba;
    uint32_t        tlba;
    uint64_t        *ep;
    uint64_t        entry;
    int             len;
    int             ecnt;

    EXEC_TRACE;
    UNUSED(hba);
    cdb = cam_cdb(ccb);

    ata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1, "%s (ptl %d:%d:%d)",
        __FUNCTION__, (int)hba->pathid, ccb->cam_ch.cam_target_id,
        ccb->cam_ch.cam_target_lun);

    memset(&tf, 0, sizeof(tf));

    lba     = ENDIAN_BE64(UNALIGNED_RET64(cdb->write_same16.lba));
    tlba    = ENDIAN_BE32(UNALIGNED_RET32(cdb->write_same16.transfer_len));

    tf.tflags   = ATA_TF_ATA | ATA_TF_IMMEDIATE | ATA_TF_EXP | ATA_TF_DIR_OUT;
    tf.device   = EIDE_LBA_ADDR;
    tf.command  = EIDE_CMD_DATA_SET_MGNT;
    tf.feature  = EIDE_DSM_TRIM;

    if ((tgt->identify.data_set_mgnt & IDENT_DSM_TRIM) &&
            (cdb->write_same16.opt & WS_OPT_UNMAP)) {
        if (tgt->tflags & EIDE_TFLG_USE_BMSTR) {
            for (ecnt = 0, ep = (uint64_t *)ws->buf;
                            tlba; tlba -= nlba, lba += nlba, ecnt++) {
                nlba    = min(tlba, EIDE_DSMR_MAX_LBA);
                entry   = lba | ((uint64_t)nlba << 48);
                *ep++   = ENDIAN_LE64(entry);
            }

            len = CAM_ALIGN(ecnt * 8, 512);
            memset(ws->buf + ecnt * 8, 0, len - (ecnt * 8)); // zero unused entries

            tf.seccnt       = (len / 512) & 0xff;
            tf.hob_seccnt   = ((len / 512) >> 8) & 0xff;

            sge.cam_sg_count        = len;
            sge.cam_sg_address      = ws->mdl[0].paddr;
            tgt->nfcnp              = ata_rw_cbf;

            if (sata_command(tgt, &tf, NULL, 0, &sge, 1, 0)) {
                ata_error(tgt, SCS_GOOD, CAM_CMD_TIMEOUT);
                return;
            }
            return;
        }
    }

    eide_sense(tgt, EIDE_SENSE_FIXED,
                    SK_ILLEGAL, ASC_INVALID_FIELD, 0x00, 0x00);

    return;
}

/******************************************************************************/
void ata_unmap(EIDE_TARGET *tgt, CCB_SCSIIO *ccb)
{
    SIM_HBA                 *hba = tgt->hba;
    EIDE_WORKSPACE          *ws = &tgt->ws;
    CDB                     *cdb;
    UNMAP_PARM_HEADER       *uph;
    UNMAP_BLOCK_DESCRIPTOR  *ubd;
    ATA_TASKFILE            tf;
    SG_ELEM                 sge;
    uint64_t                lba;
    uint32_t                nlba;
    uint64_t                *ep;
    uint64_t                entry;
    int                     len;
    int                     ecnt;
    int                     ndesc;

    EXEC_TRACE;
    UNUSED(hba);
    cdb = cam_cdb(ccb);

    ata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, 5, "%s (ptl %d:%d:%d)",
        __FUNCTION__, (int)hba->pathid, ccb->cam_ch.cam_target_id,
        ccb->cam_ch.cam_target_lun);

    if (ccb->cam_dxfer_len <
            ENDIAN_BE16(UNALIGNED_RET16(cdb->unmap.param_length))) {
        eide_sense(tgt, EIDE_SENSE_FIXED,
                SK_ILLEGAL, ASC_INVALID_FIELD, 0, 0);
        return;
    }

    if ((uph = (UNMAP_PARM_HEADER *)mmap(NULL, ccb->cam_dxfer_len,
            PROT_READ | PROT_WRITE | PROT_NOCACHE, MAP_PHYS | MAP_SHARED, NOFD,
            ccb->cam_data.cam_data_ptr)) == MAP_FAILED) {
        ccb->cam_ch.cam_status  = CAM_REQ_CMP_ERR;
        return;
    }

    ubd = (UNMAP_BLOCK_DESCRIPTOR *)(uph + 1);
    ndesc = ENDIAN_BE16(UNALIGNED_RET16(
            uph->unmap_block_desc_length)) / sizeof(UNMAP_BLOCK_DESCRIPTOR);

    if (ndesc > EIDE_DSMR_MAX_RNG) {
        eide_sense(tgt, EIDE_SENSE_FIXED, SK_ILLEGAL, ASC_INVALID_FIELD, 0, 0);
        munmap(uph, ccb->cam_dxfer_len);
        return;
    }

    if (!ndesc) {
        munmap(uph, ccb->cam_dxfer_len);
        return;
    }

    memset(&tf, 0, sizeof(tf));

    tf.tflags   = ATA_TF_ATA | ATA_TF_IMMEDIATE | ATA_TF_EXP | ATA_TF_DIR_OUT;
    tf.device   = EIDE_LBA_ADDR;
    tf.command  = EIDE_CMD_DATA_SET_MGNT;
    tf.feature  = EIDE_DSM_TRIM;

    if ((tgt->identify.data_set_mgnt & IDENT_DSM_TRIM) &&
            (cdb->write_same16.opt & WS_OPT_UNMAP)) {
        if ((tgt->tflags & EIDE_TFLG_USE_BMSTR) &&
                eide_workspace_init(tgt, EIDE_WS_SIZE) == CAM_SUCCESS) {
            for (ecnt = 0, ep = (uint64_t *)ws->buf;
                                        ndesc; ndesc--, ecnt++, ubd++) {
                lba     = ENDIAN_BE64(UNALIGNED_RET64(ubd->unmap_lba));
                nlba    = ENDIAN_BE32(UNALIGNED_RET32(ubd->unmap_nlba));
                if (nlba > EIDE_DSMR_MAX_LBA) {
                    eide_sense(tgt, EIDE_SENSE_FIXED,
                                SK_ILLEGAL, ASC_INVALID_FIELD, 0, 0);
                    munmap(uph, ccb->cam_dxfer_len);
                    return;
                }
                entry   = lba | ((uint64_t)nlba << 48);
                *ep++   = ENDIAN_LE64(entry);
            }

            len = CAM_ALIGN(ecnt * 8, 512);
            memset(ws->buf + ecnt * 8, 0, len - (ecnt * 8)); // zero unused entries

            tf.seccnt       = (len / 512) & 0xff;
            tf.hob_seccnt   = ((len / 512) >> 8) & 0xff;

            sge.cam_sg_count        = len;
            sge.cam_sg_address      = ws->mdl[0].paddr;
            tgt->nfcnp              = ata_rw_cbf;

            if (sata_command(tgt, &tf, NULL, 0, &sge, 1, 0)) {
                munmap(uph, ccb->cam_dxfer_len);
                ata_error(tgt, SCS_GOOD, CAM_CMD_TIMEOUT);
                return;
            }
            return;
        }
    }

    eide_sense(tgt, EIDE_SENSE_FIXED,
                SK_ILLEGAL, ASC_INVALID_FIELD, 0x00, 0x00);
    munmap(uph, ccb->cam_dxfer_len);
    return;
}

/******************************************************************************/
void ata_service_action(EIDE_TARGET *tgt, CCB_SCSIIO *ccb)
{
    int             action;

    EXEC_TRACE;
    action = ccb->cam_cdb_io.cam_cdb_bytes[1] & SAI_SERVICE_ACTION_MSK;

    switch (action) {
        case SAI_READ_CAPACITY_16:
            ata_capacity(tgt, ccb, action);
            break;

        default:
            eide_sense(tgt, EIDE_SENSE_FIXED,
                    SK_ILLEGAL, ASC_INVALID_FIELD, 0x00, 0x00);
            break;
    }

    return;
}

/******************************************************************************/
void ata_io(EIDE_TARGET *tgt)
{
    CCB_SCSIIO      *ccb = tgt->nexus;
    int             cmd;
    uint8_t         log_level = 5;//0;//5;

    EXEC_TRACE;
    cmd = ccb->cam_cdb_io.cam_cdb_bytes[0];

    if (tgt->tflags & EIDE_TFLG_APM_SLEEP) {
        switch (cmd) {
            case SC_INQUIRY:
            case SC_SPINDLE:
            case SC_RD_CAP:
                break;

            default:
                // should return SK_NOT_RDY, but it causes the CAM
                // layer to issue a start unit.  side effect is that
                // a sc_spindle (sleep) followed by close (SC_SYNC)
                // would wake up the drive.
                eide_sense(tgt, EIDE_SENSE_FIXED, SK_CMD_ABORT, 0, 0x00, 0x00);
                eide_post_ccb(tgt, ccb);
                return;
        }
    }

    switch (cmd) {
        case SC_UNIT_RDY:
            ata_test_unit_ready(tgt, ccb);

            ata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, log_level,
                    "%s : cmd = %02x (%s)", __FUNCTION__,
                    cmd, "SC_UNIT_RDY");

            break;

        case SC_INQUIRY:
            ata_inquiry(tgt, ccb);

            ata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, log_level,
                    "%s : cmd = %02x (%s)", __FUNCTION__,
                    cmd, "SC_INQUIRY");

            break;

        case SC_RD_CAP:
            //delay(ATA_RD_CAP_CMD_DELAY);
            ata_capacity(tgt, ccb, 0);

            ata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, log_level,
                    "%s : cmd = %02x (%s)", __FUNCTION__,
                    cmd, "SC_RD_CAP");
            break;

        case SC_READ10:
        case SC_WRITE10:
        case SC_READ12:
        case SC_WRITE12:
        case SC_READ16:
        case SC_WRITE16:
            ata_rw(tgt, ccb);

            ata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, log_level,
                    "%s : cmd = %02x (%s)", __FUNCTION__,
                    cmd, "SC_RD_WR_xx");

            return;

        case SC_SYNC:
            ata_sync(tgt, ccb);

            ata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, log_level,
                    "%s : cmd = %02x (%s)", __FUNCTION__,
                    cmd, "SC_SYNC");

            break;

        case SC_UNMAP:
            ata_unmap(tgt, ccb);

            ata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, log_level,
                    "%s : cmd = %02x (%s)", __FUNCTION__,
                    cmd, "SC_UNMAP");

            break;

        case SC_MSELECT10:
            ata_mode_select(tgt, ccb);

            ata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, log_level,
                    "%s : cmd = %02x (%s)", __FUNCTION__,
                    cmd, "SC_MSELECT10");

            break;

        case SC_MSENSE10:
            ata_mode_sense(tgt, ccb);

            ata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, log_level,
                    "%s : cmd = %02x (%s)", __FUNCTION__,
                    cmd, "SC_MSENSE10");

            break;

        case SC_WR_SAME16:
            ata_write_same(tgt, ccb);

            ata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, log_level,
                    "%s : cmd = %02x (%s)", __FUNCTION__,
                    cmd, "SC_WR_SAME16");

            break;

        case SC_SERVICE_ACTION_IN16:
            ata_service_action(tgt, ccb);

            ata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, log_level,
                    "%s : cmd = %02x (%s)", __FUNCTION__,
                    cmd, "SC_SERVICE_ACTION_IN16");

            break;

        default:
            eide_sense(tgt, EIDE_SENSE_FIXED,
                        SK_ILLEGAL, ASC_INVALID_FIELD, 0x00, 0x00);

            ata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, log_level,
                    "%s : cmd = %02x (%s)", __FUNCTION__,
                    cmd, "default");

            break;
    }

    if (ccb->cam_ch.cam_status != CAM_REQ_INPROG) {
        eide_post_ccb(tgt, ccb);
    }
}

/******************************************************************************/
int ata_init_device(EIDE_TARGET *tgt)
{
    SIM_HBA         *hba = tgt->hba;
    ATA_TASKFILE    tf;
    int             stat = CAM_SUCCESS;

    EXEC_TRACE;

    /* Spinup drive */
    if (tgt->tflags & EIDE_TFLG_SFR_SPINUP) {
        ata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 2,
                            "%s:  STANDBY SPINUP: ", __FUNCTION__);
        if ((stat = ata_command(tgt, EIDE_CMD_SET_FEATURE, 0,
                EIDE_FEAT_STANDBY_SPINUP, 0,
                0, 0, 0, NULL, 0, 0, NULL, 3 * 60))) {
            ata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                "%s:  STANDBY SPINUP failed: status %x, error %x", __FUNCTION__,
                get_tf_status_reg(tgt), get_tf_error_reg(tgt));
        }
        else {
            tgt->status = get_tf_status_reg(tgt);

            if (tgt->status & EIDE_STATUS_ERR) {
                tgt->tflags |= EIDE_TFLG_APM_SLEEP;
            }
            else {
                tgt->tflags &= ~EIDE_TFLG_SFR_SPINUP;
            }
        }
    }

    if (!stat && !(tgt->tflags & EIDE_TFLG_USE_LBA)) {
        if ((stat = ata_command(tgt, EIDE_CMD_INIT_DRV_PARMS,
                tgt->geometry.heads - 1,
                0, tgt->geometry.sec_trk, 0, 0, 0, NULL, 0, 0,
                NULL, hba->timeout))) {
            ata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                "%s:  INIT_ DRV_PARMS failure port %d, status %x, error %x",
                __FUNCTION__, tgt->port, get_tf_status_reg(tgt),
                get_tf_error_reg(tgt));
        }
        if ((stat = ata_command(tgt, EIDE_CMD_RECALIBRATE, 0, 0, 1,
                0, 0, 0, NULL, 0, 0, NULL, 15))) {
            ata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                "%s:  RECALIBRATE failure port %d, status %x, error %x",
                __FUNCTION__, tgt->port, get_tf_status_reg(tgt),
                get_tf_error_reg(tgt));
        }
    }

    if (tgt->tflags & EIDE_TFLG_USE_MBLK) {         // set multi block
        if (ata_command(tgt, EIDE_CMD_SET_MULTIPLE_MODE, 0, 0,
                tgt->rw_multiple,
                0, 0, 0, NULL, 0, 0, NULL, hba->timeout)) {
            tgt->tflags &= ~EIDE_TFLG_USE_MBLK;
            ata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                "%s:  set multi failure port %d, status %x, error %x",
                __FUNCTION__, tgt->port, get_tf_status_reg(tgt),
                get_tf_error_reg(tgt));
        }
    }

    if (!stat && (tgt->identify.cmdset_sup & IDENT_CMDSET_SMART) &&
                                        (tgt->tflags & EIDE_TFLG_SMART)) {
        if ((stat = ata_command(tgt, EIDE_CMD_SMART, 0,
                EIDE_SMART_ENABLE_OPERATIONS, 0, 0, EIDE_SMART_LBA_MID_SIG,
                EIDE_SMART_LBA_HI_SIG, NULL, 0, 0,
                NULL, hba->timeout)) == CAM_SUCCESS) {
            if ((stat = ata_command(tgt, EIDE_CMD_SMART, 0,
                    EIDE_SMART_STATUS, 0,
                    0, EIDE_SMART_LBA_MID_SIG, EIDE_SMART_LBA_HI_SIG,
                    NULL, 0, 0, NULL, hba->timeout)) == CAM_SUCCESS) {

                sata_tf_read(tgt, &tf);

                if (tf.lbam == 0xf4 && tf.lbah == 0x2c) {
                    ata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR,
                        1, "%s:  SMART STATUS tid %d, indicates "
                        "impending degrading or fault condition",
                        __FUNCTION__, tgt->port);
                }
            }
        }
    }

    if (!stat && (tgt->identify.cmdset_sup & IDENT_CMDSET_WRITE_CACHE)) {
        if (tgt->tflags & EIDE_TFLG_WRITE_CACHE_ON) {
            if ((stat = ata_command(tgt, EIDE_CMD_SET_FEATURE, 0,
                    EIDE_FEAT_EN_WCACHE, 0,
                    0, 0, 0, NULL, 0, 0, NULL, hba->timeout))) {
                ata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                        "%s:  Write Cache enable failed", __FUNCTION__);
            }
        }
        else if (tgt->tflags & EIDE_TFLG_WRITE_CACHE_OFF) {
            if ((stat = ata_command(tgt, EIDE_CMD_SET_FEATURE, 0,
                    EIDE_FEAT_DIS_WCACHE, 0,
                    0, 0, 0, NULL, 0, 0, NULL, hba->timeout))) {
                ata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                        "%s:  Write Cache disable failed", __FUNCTION__);
            }
        }
    }
    if (!stat && (tgt->tflags & EIDE_TFLG_READ_AHEAD_ON)) {
        if ((stat = ata_command(tgt, EIDE_CMD_SET_FEATURE, 0,
                EIDE_FEAT_EN_READ_AHEAD, 0,
                0, 0, 0, NULL, 0, 0, NULL, hba->timeout))) {
            ata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                    "%s:  Read Ahead enable failed", __FUNCTION__);
        }
    }
    else if (!stat && (tgt->tflags & EIDE_TFLG_READ_AHEAD_OFF)) {
        if ((stat = ata_command(tgt, EIDE_CMD_SET_FEATURE, 0,
                EIDE_FEAT_DIS_READ_AHEAD, 0,
                0, 0, 0, NULL, 0, 0, NULL, hba->timeout))) {
            ata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                    "%s:  Read Ahead disable failed", __FUNCTION__);
        }
    }

    if (!stat) {
        stat = ds_ata_init_device(tgt);
    }

    return (stat);
}

/******************************************************************************/
int ata_init(EIDE_TARGET *tgt)
{
    EXEC_TRACE;
    UNUSED(tgt);

    return (CAM_SUCCESS);
}

/******************************************************************************/
int ata_dinit(EIDE_TARGET *tgt)
{
    SIM_HBA         *hba = tgt->hba;

    EXEC_TRACE;
    if (tgt->ws.buf) {
        munmap(tgt->ws.buf, tgt->ws.sz);
        tgt->ws.buf = NULL;
    }

    if ((tgt->tflags & EIDE_TFLG_PRESENT) && (tgt->tflags & EIDE_TFLG_ATA)) {
        if (ds_ata_dinit_device(tgt) != CAM_SUCCESS) {
        }
        if ((tgt->tflags & EIDE_TFLG_PWRDOWN) && !(tgt->tflags & EIDE_TFLG_CF)) {
            ata_command(tgt, EIDE_CMD_SLEEP, 0, 0, 0, 0, 0, 0, NULL, 0, 0,
                    NULL, hba->timeout);
        }
    }

    return (CAM_SUCCESS);
}

/******************************************************************************/
int ata_parse_identify(EIDE_TARGET *tgt, EIDE_IDENTIFY *ident)
{
    uint32_t        multi;
    uint32_t        override = 0;
    uint32_t        usect;
    uint32_t        dflt_chs;
    uint32_t        cur_chs;
    uint32_t        heads;
    uint32_t        sec_trk;
    uint32_t        cylinders;
    uint64_t        lba48 = 0;//, lba=0xffffffffffffffff;

    EXEC_TRACE;
    /* Default CHS translation */
    dflt_chs = ident->dflt_logical_heads * ident->dflt_logical_sec_trk *
                ident->dflt_logical_cylinders;

    /* Current CHS translation */
    cur_chs = ident->cur_logical_heads * ident->cur_logical_sec_trk *
                ident->cur_logical_cylinders;

    /* User addressable sectors */
    usect = ident->usectors_low | (ident->usectors_hi << 16);

    if (ident->cmdsets_sup & IDENT_CMDSETS_ADDR48_SUP) {
        lba48 = (uint64_t)ident->lba_max0 | ((uint64_t)ident->lba_max1 << 16) |
         ((uint64_t)ident->lba_max2 << 32) | ((uint64_t)ident->lba_max3 << 48);
    }

    if (cur_chs && (ident->field_validity & IDENT_FVALID_54_58)) {
        heads       = ident->cur_logical_heads;
        sec_trk     = ident->cur_logical_sec_trk;
        cylinders   = ident->cur_logical_cylinders;
    }
    else {
        if (dflt_chs && dflt_chs > IDENT_MAX_CHS_SECTORS) {
            ata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                    "%s:  Default CHS translation invalid: port %d, "
                    "heads %d, sec_trk %d cyl %d",
                __FUNCTION__, tgt->port, ident->dflt_logical_heads,
                ident->dflt_logical_sec_trk, ident->dflt_logical_cylinders);
            return (CAM_FAILURE);
        }
        heads       = ident->dflt_logical_heads;
        sec_trk     = ident->dflt_logical_sec_trk;
        cylinders   = ident->dflt_logical_cylinders;
    }

    /* Check for command line override */
    if (!tgt->geometry.heads) {
        tgt->geometry.heads     = heads;
        tgt->geometry.sec_trk   = sec_trk;
        tgt->geometry.cylinders = cylinders;
    }
    else {
        override    = 1;
        heads = tgt->geometry.heads;
        sec_trk = tgt->geometry.sec_trk;
        cylinders = tgt->geometry.cylinders;
    }
    tgt->geometry.sectors = heads * sec_trk * cylinders;

    /* User spec'd CHS mode, but device doesn't support it */
    if (!(tgt->tflags & EIDE_TFLG_USE_LBA) && dflt_chs) {
        ata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                "%s:  Device doesn't support CHS translation:  port %d",
                __FUNCTION__, tgt->port);
        return (CAM_FAILURE);
    }

    /* Device doesn't support LBA, use CHS */
    if (!(ident->capabilities & IDENT_CAP_LBA_SUP)) {
        tgt->tflags &= ~EIDE_TFLG_USE_LBA;
    }
    else if ((tgt->tflags & EIDE_TFLG_USE_LBA) && !override) {
        if (!usect) {
            ata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                    "%s:  Invalid user addressable sectors: "
                    "port %d, usect %d", __FUNCTION__, tgt->port, usect);
        }
        else if (lba48 && (tgt->tflags & EIDE_TFLG_USE_LBA48)) {
            //tgt->geometry.sectors = lba48;
            /* Limit to 32bit */
            #define EIDE_MAX_LBA32  0xffffffff
            tgt->geometry.sectors = lba48 > EIDE_MAX_LBA32 ?
                                            EIDE_MAX_LBA32 : lba48;
        }
        else {
            tgt->geometry.sectors = usect;
        }
    }
    tgt->geometry.sectors--;    /* Set to last addressable address (SCSI) */

    if (tgt->tflags & EIDE_TFLG_USE_MBLK) {
        multi = ident->rw_multiple & 0xff;
        tgt->rw_multiple = tgt->rw_multiple ?
                        min(tgt->rw_multiple, multi) : multi;
        if (tgt->rw_multiple <= 1) {
            tgt->tflags &= ~EIDE_TFLG_USE_MBLK;
            tgt->rw_multiple = 1;
        }
    }

    if (ident->general_config == IDENT_GC_CF_PCCARD_MODE ||
            ident->general_config == IDENT_GC_CF_TRUE_IDE_MODE) {
        tgt->tflags |= EIDE_TFLG_CF;
    }

    if ((ident->cmd_setfeat_dflt & IDENT_CFD_VALID) &&
            (ident->cmd_feature_enb2 & IDENT_CFE_SFR_SPINUP) &&
            ident->specific_config == IDENT_SC_SFR_SPINUP_CMPLT) {
        tgt->tflags |= EIDE_TFLG_SFR_SPINUP | EIDE_TFLG_APM_SLEEP;
    }

    tgt->tflags |= EIDE_TFLG_ATA;
    return (CAM_SUCCESS);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/devb/rcarsata/sim_ata.c $ $Rev: 814232 $")
#endif
