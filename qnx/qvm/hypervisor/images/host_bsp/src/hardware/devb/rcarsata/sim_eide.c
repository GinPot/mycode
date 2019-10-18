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

// Module Description: EIDE to CAM interface

#include <sim_eide.h>

/******************************************************************************/
#if defined(EIDE_DEBUG)
    #define eide_log            eide_slogf
#else
    #define eide_log(_a, _b, _c, _d, _fmt, ...)

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
/* Globals */
EIDE_CTRL eide_ctrl = { { 0 },0 };

const MODULE_ENTRY sim_module = { "rcarsata", &eide_sim_args,
                                &eide_sim_attach, &eide_sim_detach };

CAM_SIM_ENTRY eide_sim_entry = { &eide_sim_init, &eide_sim_action };

/******************************************************************************/
int main(int argc, char *argv[])
{
    int             *version, ret;
    char            *op;

    EXEC_TRACE;
    TAILQ_INIT(&eide_ctrl.hlist);
    eide_ctrl.gflags    = EIDE_GFLG_BMSTR;

    if ((eide_ctrl.pagesize = sysconf(_SC_PAGESIZE)) == -1) {
        eide_ctrl.pagesize = SYSPAGE_ENTRY(system_private)->pagesize;
    }

    /* Check rcarsata options */
    op = cam_module_args(argc, argv, "rcarsata");
    if (op) {
        cam_free(op);
    } else {
        eide_slogf(NULL, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 0,
            "%s : rcarsata options not specified", __FUNCTION__);
        exit( EXIT_FAILURE );
    }

    version = dlsym(RTLD_DEFAULT, "cam_version");

    eide_log(NULL, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, 1,
        "%s cam ver %d sim %d.%02d%c (%s %s)",
        CAM_STRINGIZE(__DEVB_NAME__), version ? *version : 0,
        SIM_EIDE_VERSION / 100, SIM_EIDE_VERSION % 100, SIM_EIDE_LETTER,
        __DATE__, __TIME__);

    ret = cam_configure(&sim_module, 1, argc, argv);
    return ret;
}

/******************************************************************************/
ssize_t eide_slogf(EIDE_TARGET *tgt, int opcode,
                                int severity, int vlevel, const char *fmt, ...)
{
    ssize_t     ret;
    va_list     arglist;
    int         verbosity;

    ret         = 0;
    verbosity   = eide_ctrl.verbosity;

    if (tgt) {
        verbosity = tgt->verbosity ? tgt->verbosity : tgt->hba->verbosity;
    }

    if (verbosity > 5) {
        va_start(arglist, fmt);
        vfprintf(stderr, fmt, arglist);
        va_end(arglist);
        fprintf(stderr, "\n");
    }

    if (vlevel <= 5 && verbosity >= vlevel) {
        va_start(arglist, fmt);
        ret = vslogf(opcode, severity, fmt, arglist);
        va_end(arglist);
    }
    return (ret);
}

/******************************************************************************/
void eide_display_ccb(EIDE_TARGET *tgt, CCB *ccb_ptr)
{
    CCB_SCSIIO  *ccb;
    uint32_t    i;
    char        buf[50];
    uint8_t     log_level = 4;//0;//4;

    EXEC_TRACE;
    for (ccb = ccb_ptr; ccb;
            ccb = (((CCB_HEADER *)ccb_ptr)->cam_func_code == XPT_SCSI_IO &&
            (ccb->cam_ch.cam_flags & CAM_CDB_LINKED)) ?
                    (CCB_SCSIIO *)ccb->cam_next_ccb : 0) {
        switch (((CCB_HEADER *)ccb_ptr)->cam_func_code) {
            case XPT_SCSI_IO:
                eide_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, log_level,
                        "EIDE XPT_SCSI_IO");

                eide_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, log_level,
                        "addr=%p len=%d stat=%d path=%d targ=%d lun=%d "
                        "flag=%08lx timeout=%ld vuflag=%04lx",
                        ccb->cam_ch.my_addr,
                        ccb->cam_ch.cam_ccb_len,
                        ccb->cam_ch.cam_status,
                        ccb->cam_ch.cam_path_id,
                        ccb->cam_ch.cam_target_id,
                        ccb->cam_ch.cam_target_lun,
                        ccb->cam_ch.cam_flags,
                        ccb->cam_timeout,
                        ccb->cam_vu_flags);

                eide_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, log_level,
                        "pdrv=%p next=%p req_map=%p cbfcnp=%p xfer_len=%ld "
                        "sense_ptr=%p sense_len=%d sense_resid=%d scsi_stat=%d",
                        ccb->cam_pdrv_ptr,
                        ccb->cam_next_ccb,
                        ccb->cam_req_map,
                        ccb->cam_cbfcnp,
                        ccb->cam_dxfer_len,
                        ccb->cam_sense_ptr,
                        ccb->cam_sense_len,
                        ccb->cam_sense_resid,
                        ccb->cam_scsi_status);

                if (ccb->cam_ch.cam_flags & CAM_SCATTER_VALID) {
                    SG_ELEM *sg = ccb->cam_data.cam_sg_ptr;

                    eide_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 5,
                            "num_sg=%d\n", ccb->cam_sglist_cnt);
                    for (i = 0; i < ccb->cam_sglist_cnt; i++, sg++) {
                        eide_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 5,
                            "sgentry %d: addr=%08x count=%ld",
                            i + 1, sg->cam_sg_address, sg->cam_sg_count);
                    }
                }
                else {
                    eide_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 5,
                            "data_ptr=%08x", ccb->cam_data.cam_data_ptr);
                }

                if (ccb->cam_cdb_len) {
                    for (i = 0; i < ccb->cam_cdb_len; i++) {
                        if (ccb->cam_ch.cam_flags & CAM_CDB_POINTER) {
                            sprintf(&buf[i * 3], "%02x ",
                                    ccb->cam_cdb_io.cam_cdb_ptr[i]);
                        }
                        else {
                            sprintf(&buf[i * 3], "%02x ",
                                    ccb->cam_cdb_io.cam_cdb_bytes[i]);
                        }
                    }
                    eide_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, log_level, ""
                        "cdblen=%d : (%s)", ccb->cam_cdb_len, buf);
                }

                if ((ccb->cam_ch.cam_flags &
                                    CAM_CDB_LINKED) && ccb->cam_next_ccb) {
                    eide_log(tgt, _SLOGC_SIM_RCAR_SATA,
                                    _SLOG_ERROR, log_level, "LINKED==>");
                }
                break;

            case XPT_DEVCTL: {
                CCB_HEADER *ccb = ccb_ptr;
                CCB_DEVCTL *ccbd = ccb_ptr;

                UNUSED(ccbd);
                UNUSED(ccb);

                eide_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, log_level,
                        "EIDE type=%x addr=%p len=%d stat=%d path=%d "
                        "targ=%d lun=%d flag=%08lx, dcmd=%x, dsize=%d",
                        ccb->cam_func_code,
                        ccb->my_addr,
                        ccb->cam_ccb_len,
                        ccb->cam_status,
                        ccb->cam_path_id,
                        ccb->cam_target_id,
                        ccb->cam_target_lun,
                        ccb->cam_flags,
                        ccbd->cam_devctl_dcmd,
                        ccbd->cam_devctl_size);
                break;
            }

            default: {
                CCB_HEADER *ccb = ccb_ptr;

                UNUSED(ccb);

                eide_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, log_level,
                        "EIDE type=%d addr=%p len=%d stat=%d path=%d "
                        "targ=%d lun=%d flag=%08lx",
                        ccb->cam_func_code,
                        ccb->my_addr,
                        ccb->cam_ccb_len,
                        ccb->cam_status,
                        ccb->cam_path_id,
                        ccb->cam_target_id,
                        ccb->cam_target_lun,
                        ccb->cam_flags);
                break;
            }
        }
    }
}

/******************************************************************************/
int eide_sense(EIDE_TARGET *tgt, int sflgs,
                                        int sense, int asc, int ascq, int info)
{
    EIDE_REGS               *regs = &tgt->regs;
    SCSI_SENSE              *sf;
    SCSI_SENSE_DESCRIPTOR   *sd;
    ATA_STATUS_DESCRIPTOR   *asd;
    ATA_TASKFILE            tf;
    CCB_SCSIIO              *ccb = tgt->nexus;
    uint8_t                 log_level = 5;//0;//5;

    EXEC_TRACE;

    eide_log(NULL, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, log_level,
            "%s - sflgs = %08x sense = %08x asc = %08x ascq = %08x info = %08x",
            __FUNCTION__, (uint32_t)sflgs, (uint32_t)sense, (uint32_t)asc,
            (uint32_t)ascq, (uint32_t)info);

    sf = (SCSI_SENSE *)ccb->cam_sense_ptr;
    sd = (SCSI_SENSE_DESCRIPTOR *)ccb->cam_sense_ptr;
    asd = (ATA_STATUS_DESCRIPTOR *)(sd + 1);

    ccb->cam_ch.cam_status = CAM_REQ_CMP_ERR;
    ccb->cam_scsi_status = SCS_CHECK;

    if (sf == NULL) {
        return (ccb->cam_ch.cam_status);
    }

    memset(sf, 0, ccb->cam_sense_len);
    if (sflgs & EIDE_SENSE_DESC) {
        if (ccb->cam_sense_len < (sizeof(*sd) + sizeof(*asd))) {
            return (ccb->cam_ch.cam_status);
        }
        ccb->cam_ch.cam_status  |= CAM_AUTOSNS_VALID;
        sd->error = SENSE_ERROR | SENSE_DATA_FMT_DESCRIPTOR;    // Error code
        sd->sense = sense;      // Sense key
        sd->asc = asc;          // Additional sense code (Invalid field in CDB)
        sd->ascq = ascq;        // Additional sense code qualifier
        sd->asl = sizeof(*asd); // Additional sense length

        asd->descriptor_type= SENSE_DTYPE_ATA;
        asd->descriptor_len = ATA_SD_DLEN;
        asd->status = (uchar_t)in32(regs->status);
        asd->error = (uchar_t)in32(regs->error);
        asd->device = EIDE_SELECT_DRIVE(0);

        sata_tf_read(tgt, &tf);
        if (sflgs & EIDE_SENSE_EXTEND) {
            asd->flags = ATA_SD_FLG_EXTEND;
            asd->sector_count = tf.seccnt;
            asd->esector_count = tf.hob_seccnt;
            asd->lba_low = tf.lbal;
            asd->elba_low = tf.hob_lbal;
            asd->lba_mid = tf.lbam;
            asd->elba_mid = tf.hob_lbam;
            asd->lba_high = tf.lbah;
            asd->elba_high = tf.hob_lbah;
        }
        else {
            asd->sector_count = tf.seccnt;
            asd->lba_low = tf.lbal;
            asd->lba_mid = tf.lbam;
            asd->lba_high = tf.lbah;
        }
    }
    else {
        if (ccb->cam_sense_len < sizeof(*sf)) {
            return (ccb->cam_ch.cam_status);
        }
        ccb->cam_ch.cam_status |= CAM_AUTOSNS_VALID;
        sf->error = SENSE_ERROR;// Error code
        sf->sense = sense;      // Sense key
        sf->asc = asc;          // Additional sense code (Invalid field in CDB)
        sf->ascq = ascq;        // Additional sense code qualifier
        UNALIGNED_PUT32(sf->info, ENDIAN_BE32(info));
    }

    return (ccb->cam_ch.cam_status);
}

/******************************************************************************/
void *eide_alloc(size_t size)
{
    void    *addr;

    EXEC_TRACE;
#if (defined(_NTO_CAM_VERSION) && (_NTO_CAM_VERSION))
    addr = xpt_alloc(XPT_ALLOC_CONTIG | XPT_ALLOC_NOCACHE, size, NULL);
#else
    addr = mmap(NULL, size, PROT_READ | PROT_WRITE | PROT_NOCACHE,
                    MAP_ANON | MAP_PHYS | MAP_PRIVATE | MAP_NOX64K, NOFD, 0);
#endif
    return (addr);
}

/******************************************************************************/
int eide_free(void *addr, size_t sz)
{
    EXEC_TRACE;
#if (defined(_NTO_CAM_VERSION) && (_NTO_CAM_VERSION))
    return (xpt_free(addr, 512));
#else
    return (munmap(addr, 512));
#endif
}

/******************************************************************************/
void *eide_dma_mem_alloc(size_t size, size_t *psize)
{
    EXEC_TRACE;

    *psize = size;

    return eide_alloc(size);
}

/******************************************************************************/
int eide_dma_mem_free(void *addr, size_t sz)
{
    EXEC_TRACE;
    if (addr == NULL || sz == 0)
        return 0;

    return (munmap(addr, sz));
}

/******************************************************************************/
void eide_dump(char *buffer, long cnt)
{
    char    *ptr, output[80];
    int     res, i, j, k, n;
    long    l;
    char    ref[80];

    EXEC_TRACE;
    l = 0L;
    res = cnt;
    ptr = buffer;

    while (res) {
#define BL 16
#define AL BL/2*5+4
        sprintf(output, "%08lx:  %-*.*s\"%-*.*s\"",
                                        l, AL, AL, "  ", BL, BL, "  ");
        k = strlen(output) - (BL + 1);
        j = l % BL;
        i = 12 + (j << 1) + (j >> 1) + (j >= (BL / 2) ? 2 : 0);
        for (; j < BL && res; j++, res--, ptr++, l++) {
            n = ((int) *ptr >> 4) & 0xf;
            output [i++] = (n < 10 ? (char) n + '0': (char) n + ('a' - 10));
            n = (int) *ptr & 0xf;
            output [i++] = (n < 10 ? (char) n + '0': (char) n + ('a' - 10));
            if (j & 1)
                i++;
            if (j == BL / 2 - 1)
                i += 2;
#define UC unsigned char
            output [j + k] = ((UC) *ptr < ' ' || (UC) *ptr > '~') ? '.' : *ptr;
#undef UC
        }
        eide_log(NULL, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1, "%s", output);

        memcpy(ref, output, 80);
#undef BL
#undef AL
    }
}

/******************************************************************************/
void eide_strip_spaces(char *src, char *dst, int len)
{
    char    pchar;

    /* Remove leading spaces */
    for (; len && *src == ' '; len--) {
        src++;
    }

    for (pchar = 0, *dst = 0; len; len--, src++, dst++) {
        if (src == NULL) {
            break;
        }
        if (!isascii(*src)) {
            continue;
        }
        if (*src == ' ' && pchar == ' ') {
            dst--; break;
        }
        pchar = *dst = *src;
    }
    *dst = 0;
}

/******************************************************************************/
int eide_free_hba(SIM_HBA *hba)
{
    SIM_EIDE_EXT    *ext;

    EXEC_TRACE;
    ext = (SIM_EIDE_EXT *)hba->ext;
    TAILQ_REMOVE(&eide_ctrl.hlist, hba, hlink);
    eide_ctrl.nhba--;
    free(ext);
    free(hba);
    return (CAM_SUCCESS);
}

/******************************************************************************/
int eide_sim_idetach(SIM_HBA *hba)
{
    SIM_EIDE_EXT    *ext = (SIM_EIDE_EXT *)hba->ext;
    EIDE_TARGET     *tgt = &ext->targets[0];

    EXEC_TRACE;
    ata_dinit(tgt);

    sata_dinit(hba);

    if (hba->iid != -1) {
        InterruptDetach(hba->iid);
    }

    if (hba->tid != -1) {
        pthread_cancel(hba->tid); // Drop a signal on our pulse handling thread
    }

    if (hba->coid != -1) {
        ConnectDetach(hba->coid);
    }

    if (hba->chid != -1) {
        ChannelDestroy(hba->chid);
    }

    if (hba->simq) {
        simq_dinit(hba->simq);
    }

    eide_free_hba(hba);

    return (CAM_SUCCESS);
}

/******************************************************************************/
int eide_dump_regs(EIDE_TARGET *tgt)
{
    EIDE_REGS       *regs;
    ATA_TASKFILE    tf;

    regs    = &tgt->regs;

    sata_tf_read(tgt, &tf);

    eide_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, 5,
            "%s: port %d, error %x, status %x, "
            "seccnt %x, secno %x, cyllo %x, cylhi %x",
        __FUNCTION__, tgt->port, in32(regs->error),
        in32(regs->status), tf.seccnt, tf.lbal, tf.lbam, tf.lbah);

    return (CAM_SUCCESS);
}

/******************************************************************************/
int eide_reset(EIDE_TARGET *tgt)
{
    SIM_HBA         *hba;

    hba         = tgt->hba;

    EXEC_TRACE;
    if (hba->simq) {
        simq_timer_settime(hba->simq, SIMQ_TIMER_DISABLE);
    }

    sata_port_reset(tgt, 1);

    if (hba->simq) {
        simq_timer_settime(hba->simq, SIMQ_TIMER_ENABLE);
    }
    return ((get_tf_status_reg(tgt) & EIDE_STATUS_BSY) ? CAM_FAILURE : CAM_SUCCESS);
}

/******************************************************************************/
int eide_bsr(int cmode)
{
    uint32_t mode = 0;

    EXEC_TRACE;
    do {
        mode++;
        cmode >>= 1;
    } while (cmode);
    return (mode - 1);
}

/******************************************************************************/
int eide_pio_mode(EIDE_TARGET *tgt, EIDE_IDENTIFY *ident)
{
    uint32_t        mode;

    mode    = 0;

    EXEC_TRACE;
    if ((ident->field_validity & IDENT_FVALID_64_70) &&
                                    (ident->pio_modes & 0x03)) {
        mode = (ident->pio_modes << 3) | 0x07;
    }
    else {
        mode = ident->opio_mode & 0x07;
    }

    mode = eide_bsr(mode);

    return (tgt->pio_mode = mode);
}

/******************************************************************************/
int eide_mdma_mode(EIDE_TARGET *tgt)
{
    SIM_HBA         *hba;
    SIM_EIDE_EXT    *ext;
    EIDE_IDENTIFY   *ident;
    uint32_t        mode;

    EXEC_TRACE;
    hba     = (SIM_HBA *)tgt->hba;
    ext     = (SIM_EIDE_EXT *)hba->ext;
    ident   = &tgt->identify;

    if (!(ident->capabilities & IDENT_CAP_DMA_SUP) ||
            !(tgt->tflags & EIDE_TFLG_USE_MDMA) ||
            !(ext->eflags & EIDE_EFLG_USE_BMSTR) ||
            !ident->multiword_dma) {
        return (-1);
    }

    mode = ident->multiword_dma & 0x0f;
    mode = tgt->mdma_mode ? tgt->mdma_mode : mode;
    mode = eide_bsr(mode);

    return (tgt->mdma_mode = mode);
}

/******************************************************************************/
int eide_udma_mode(EIDE_TARGET *tgt, EIDE_IDENTIFY *ident)
{
    SIM_HBA         *hba;
    SIM_EIDE_EXT    *ext;
    uint32_t        mode;

    EXEC_TRACE;
    hba     = tgt->hba;
    ext     = (SIM_EIDE_EXT *)hba->ext;

    if (!(ident->field_validity & IDENT_FVALID_88) ||
            !(tgt->tflags & EIDE_TFLG_USE_UDMA) ||
            !(ext->eflags & EIDE_EFLG_USE_BMSTR) ||
            !ident->udma_modes) {
        return (-1);
    }

    if (tgt->udma_mode) {           // check for command line override
        mode = tgt->udma_mode;
    }                               // see if BIOS has configured chip/drive
    else {
        mode = ident->udma_modes & 0x7f;
    }

    mode = eide_bsr(mode);

    // Limit speed to UDMA 5 to work-around Marvel 88SX8030 PATA to
    // SATA converter UDMA 6 problem
    if (mode > 5 && (!ident->sata_capabilities ||
            ident->sata_capabilities == 0xffff)) {
        mode = 5;
    }
    return (tgt->udma_mode = mode);
}

/******************************************************************************/
int eide_display_identify(EIDE_TARGET *tgt)
{
    SIM_HBA         *hba;
    EIDE_IDENTIFY   *ident;
    int             cudma, cmdma, pio;
    int             mudma, mmdma;

    EXEC_TRACE;
    hba     = (SIM_HBA *)tgt->hba;
    ident   = &tgt->identify;
    cudma   = mudma = cmdma = mmdma = -1;

    if ((ident->field_validity & IDENT_FVALID_88) && ident->udma_modes) {
        mudma = eide_bsr(ident->udma_modes & 0x7f);
        cudma = eide_bsr((ident->udma_modes >> 8) & 0x7f);
    }

    if ((ident->capabilities & IDENT_CAP_DMA_SUP) && ident->multiword_dma) {
        mmdma = eide_bsr(ident->multiword_dma & 0x7);
        cmdma = eide_bsr((ident->multiword_dma >> 8) & 0x7);
    }

    if ((ident->field_validity & IDENT_FVALID_64_70) &&
                                    (ident->pio_modes & 0x03)) {
        pio = (ident->pio_modes << 3) | 0x07;
    }
    else {
        pio = ident->opio_mode & 0x07;
    }

    eide_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, 5,
        "%s: mdl %.40s sn %.20s fw %.8s port %d, "
        "max udma %d, cur udma %d, max mdma %d, cur mdma %d, pio %d, mblk %d",
        __FUNCTION__, tgt->mdl, tgt->sn, tgt->fw,
        tgt->port, mudma, cudma, mmdma, cmdma,
        eide_bsr(pio), ident->rw_multiple & 0xff);

    if (hba->verbosity > 2) {
        eide_log(tgt, _SLOGC_SIM_RCAR_SATA,
                                    _SLOG_ERROR, 1, "IDENTIFY DRIVE data:");
        eide_dump((char *)ident, sizeof(*ident));
    }

    return (CAM_SUCCESS);
}

/******************************************************************************/
int eide_parse_identify(EIDE_TARGET *tgt, EIDE_IDENTIFY *ident)
{
    SIM_HBA         *hba;
    SIM_EIDE_EXT    *ext;
    uint32_t        cnt;
    uint16_t        *wp;
    uint8_t         *bp;
    uint8_t         cksum;

    EXEC_TRACE;
    hba     = tgt->hba;
    ext     = (SIM_EIDE_EXT *)hba->ext;
    bp      = (uint8_t *)ident;

    // Check version
    if (ident->major_version && ident->major_version != 0xffff) {
        if (ident->major_version & 0x10) {      // integrity field ATA-5+
            if ((ident->integrity & 0xff) == 0xa5) {
                cksum = 0;
                for (cnt = 0; cnt < 512; cnt++)
                    cksum += bp[cnt];
                if (cksum != 0) {
                    eide_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                        "%s:  IDENTIFY DRIVE CMD cksum failure", __FUNCTION__);
                }
            }
        }
    }

#if defined(__BIGENDIAN__)
    for (wp = (uint16_t *)ident, cnt = sizeof(*ident) / 2; cnt; cnt--, wp++) {
        *wp = ENDIAN_LE16(*wp);
    }
#else
    // Convert ASCII character strings to correct endian format
    for (wp = (uint16_t *)ident->model_number,
                    cnt = sizeof(ident->model_number) / 2; cnt; cnt--, wp++) {
        *wp = ENDIAN_BE16(*wp);
    }
    for (wp = (uint16_t *)ident->serial_number,
                    cnt = sizeof(ident->serial_number) / 2; cnt; cnt--, wp++) {
        *wp = ENDIAN_BE16(*wp);
    }
    for (wp = (uint16_t *)ident->firmware_revision,
                cnt = sizeof(ident->firmware_revision) / 2; cnt; cnt--, wp++) {
        *wp = ENDIAN_BE16(*wp);
    }
#endif

    if (tgt->tflags & EIDE_TFLG_ATAPI) {
        if (atapi_parse_identify(tgt, ident)) {
            return (CAM_FAILURE);
        }
        if (atapi_init(tgt)) {
            return (CAM_FAILURE);
        }
    }
    else {
        if (ata_parse_identify(tgt, ident)) {
            return (CAM_FAILURE);
        }
        if (ata_init(tgt)) {
            return (CAM_FAILURE);
        }
    }

    eide_strip_spaces((char *)ident->model_number, tgt->mdl, 40);
    eide_strip_spaces((char *)ident->serial_number, tgt->sn, 20);
    eide_strip_spaces((char *)ident->firmware_revision, tgt->fw, 8);

    eide_display_identify(tgt);

    if (!(ident->general_config & IDENT_GC_REMOVABLE)) {
        tgt->tflags &= ~EIDE_TFLG_REMOVABLE;
    }

    if (ident->capabilities & IDENT_CAP_IORDY_DIS) {
        tgt->tflags |= EIDE_TFLG_DIS_IORDY;
    }

    if (ident->capabilities & IDENT_CAP_IORDY_SUP) {
        tgt->tflags |= EIDE_TFLG_IORDY;
    }

    if (ident->cmdsets_sup & IDENT_CMDSETS_APM_SUP) {
        tgt->tflags |= EIDE_TFLG_APM;
    }

    if (ident->cmdsets_sup & IDENT_CMDSETS_QUEUED_SUP) {
        tgt->queue_depth = tgt->queue_depth ?
                            tgt->queue_depth : ident->queue_depth;
    }
    else {
        tgt->tflags &= ~EIDE_TFLG_USE_QUEUED;
    }
    tgt->queue_depth++;

    if (eide_mdma_mode(tgt) == -1) {
        tgt->tflags &= ~EIDE_TFLG_USE_MDMA;
    }

    if (eide_udma_mode(tgt, ident) == -1) {
        tgt->tflags &= ~EIDE_TFLG_USE_UDMA;
    }

    if (!(tgt->tflags & (EIDE_TFLG_USE_UDMA | EIDE_TFLG_USE_MDMA)) ||
            !(ext->eflags & EIDE_EFLG_USE_BMSTR)) {
        tgt->tflags &= ~EIDE_TFLG_USE_BMSTR;
    }

    eide_pio_mode(tgt, ident);

    if (!(ext->s_cap & SATA_CAP_PMD)) {
        // Host doesn't support multi-block
        tgt->tflags &= ~EIDE_TFLG_USE_MBLK;
    }

    return (CAM_SUCCESS);
}

/******************************************************************************/
int eide_identify(EIDE_TARGET *tgt)
{
    SIM_HBA         *hba = tgt->hba;
    EIDE_REGS       *regs = &tgt->regs;
    EIDE_IDENTIFY   *ident = &tgt->identify;
    ATA_TASKFILE    tf;
    SG_ELEM         sge;
    int             status;
    void            *idata;

    /*
     * Retrieve identify info from device.  This routine is called
     * at initialization time, so interrupts are not available
     */
    EXEC_TRACE;
    UNUSED(regs);
    if ((idata = eide_alloc(512)) == MAP_FAILED) {
        eide_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                "%s:  Unable to allocate identify buffer", __FUNCTION__);
        return (CAM_FAILURE);
    }

    for (; ;) {
        memset(&tf, 0, sizeof(tf));
        tf.tflags = ATA_TF_BUSY_WAIT | ATA_TF_DIR_IN;
        tf.command = (tgt->tflags & EIDE_TFLG_ATAPI) ?
                EIDE_CMD_ATAPI_IDENTIFY : EIDE_CMD_ATA_IDENTIFY;
        sge.cam_sg_address  = cam_mphys(idata);
        sge.cam_sg_count    = 512;

        if ((status = sata_command(tgt, &tf, NULL, 0,
                            &sge, 1, hba->timeout)) == CAM_SUCCESS) {
            memcpy(ident, idata, 512);
            status = eide_parse_identify(tgt, ident);
        }
        else {
            eide_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                    "%s:  IDENTIFY failed: port %x, status %x, error %x",
                    __FUNCTION__, tgt->port,
                    in32(regs->status), in32(regs->error));
            break;
        }

        break;
    }
    eide_free(idata, 512);

#ifndef EIDE_ATAPI_SUPPORT
    if (tgt->tflags & EIDE_TFLG_ATAPI) {
        printf("\n%s - ATAPI device not supported\n\n", __FUNCTION__);
        return (CAM_FAILURE);
    }
#endif

    return (status);
}

/******************************************************************************/
int eide_identify_devices(SIM_HBA *hba)
{
    SIM_EIDE_EXT        *ext;
    EIDE_TARGET         *tgt;
    SIM_CONFIG_INFO     *cfg;
    int                 found;

    EXEC_TRACE;
    ext     = (SIM_EIDE_EXT *)hba->ext;
    cfg     = (SIM_CONFIG_INFO *)&hba->cfg;
    found   = 0;

    eide_log(NULL, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, 5,
        "%s: %.32s vid=0x%x, did=0x%x, class=0x%x, rev=0x%x, "
        "busno=0x%x, dfunc=0x%x, base=%08x:%08x, irq=0x%x",
        __FUNCTION__, cfg->Description, cfg->Device_ID.DevID & 0xffff,
        cfg->Device_ID.DevID >> 16, cfg->Device_ID.LogicalID,
        cfg->Device_ID.SerialNum & 0xff, cfg->BusAccess.PCIAccess.BusNumber,
        cfg->BusAccess.PCIAccess.DevFuncNumber,
        (ext->IOPort_Base >> 32), ext->IOPort_Base, cfg->IRQRegisters[0]);

    eide_ctrl.gflags |= EIDE_GFLG_ENUMERATION;

    tgt = &ext->targets[0];

    if (tgt->tflags & EIDE_TFLG_PRESENT) {
        if (eide_identify(tgt) == CAM_SUCCESS &&
                eide_workspace_init(tgt, EIDE_WS_SIZE) == CAM_SUCCESS) {
            found = 1;
        }
        else {
            tgt->tflags &= ~EIDE_TFLG_PRESENT;
        }
    }

    eide_ctrl.gflags &= ~EIDE_GFLG_ENUMERATION;

    return found ? (CAM_SUCCESS) : (CAM_FAILURE);
}

/******************************************************************************/
int eide_init_device(EIDE_TARGET *tgt, int reset)
{
    SIM_HBA         *hba = tgt->hba;
    EIDE_REGS       *regs = &tgt->regs;
    int             stat = CAM_FAILURE;
    int             mode = 0;
    int             retries;
    ATA_TASKFILE    tf;
    EXEC_TRACE;
    for (retries = 5; retries && stat; retries--) {
        if (reset || ((in32(regs->status) &
                                (EIDE_STATUS_BSY | EIDE_STATUS_DRQ)))) {
            eide_reset(tgt);
        }

        if (!(tgt->tflags & EIDE_TFLG_PRESENT)) {
            return (CAM_SUCCESS);
        }

        if (tgt->tflags & EIDE_TFLG_ATA) {
            if ((stat = ata_init_device(tgt))) {
                continue;
            }
        }
        else {
            if ((stat = atapi_init_device(tgt))) {
                continue;
            }
        }
        /* Don't need to set the mode if it is a true SATA drive */
        if (EIDE_IS_SATA(&tgt->identify)) {
            return (CAM_SUCCESS);
        }

        if (tgt->tflags & EIDE_TFLG_USE_BMSTR) {
            memset(&tf, 0, sizeof(tf));
            mode = (tgt->tflags & EIDE_TFLG_USE_UDMA) ?
                    (EIDE_XFER_MODE_UDMA | tgt->udma_mode) :
                    (EIDE_XFER_MODE_MDMA | tgt->mdma_mode);

            tf.tflags   = ATA_TF_ATA | ATA_TF_INTR_WAIT;
            tf.command  = EIDE_CMD_SET_FEATURE;
            tf.feature  = EIDE_FEAT_XFER_MODE;
            tf.seccnt   = mode;
            if ((stat = sata_command(tgt, &tf, NULL, 0,
                                        NULL, 0, hba->timeout))) {
                eide_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                        "%s:  UDMA set feature failed %x",
                        __FUNCTION__, get_tf_error_reg(tgt));
            }
        }

        /* Set PIO mode if above setup failed or not using DMA */
        if (stat || !(tgt->tflags & EIDE_TFLG_USE_BMSTR)) {

            mode = 0;
            if ((tgt->tflags & EIDE_TFLG_IORDY) && tgt->pio_mode > 2) {
                mode = EIDE_XFER_MODE_PIO_FLOW;
            }
            mode |= tgt->pio_mode;
            memset(&tf, 0, sizeof(tf));
            tf.tflags   = ATA_TF_ATA | ATA_TF_INTR_WAIT;
            tf.command  = EIDE_CMD_SET_FEATURE;
            tf.feature  = EIDE_FEAT_XFER_MODE;
            tf.seccnt   = mode;

            if (!stat && (stat = sata_command(tgt, &tf, NULL, 0, NULL,
                                                0, hba->timeout))) {
                eide_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                        "%s:  PIO set feature failed %x",
                        __FUNCTION__, get_tf_error_reg(tgt));
            }
        }
    }

    return (stat);
}

/******************************************************************************/
int eide_init(SIM_HBA *hba)
{
    uint32_t            stat;

    EXEC_TRACE;

    stat = eide_identify_devices(hba);

    if (!stat) {
        if ((hba->chid = ChannelCreate(_NTO_CHF_DISCONNECT |
                                                    _NTO_CHF_UNBLOCK)) == -1) {
            eide_log(NULL, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                    "%s: Unable to create channel", __FUNCTION__);
            stat = CAM_FAILURE;
        }
    }

    if (!stat) {
        if ((hba->coid = ConnectAttach(0, 0, hba->chid,
                                            _NTO_SIDE_CHANNEL, 0)) == -1) {
            eide_log(NULL, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                    "%s: Unable to attach channel", __FUNCTION__);
            stat = CAM_FAILURE;
        }
    }

    return (stat ? CAM_FAILURE : CAM_SUCCESS);
}

/******************************************************************************/
// This function is provided so that the peripheral driver can enqueue an
// I/O command.
int eide_scsi_io(EIDE_TARGET *tgt, CCB_SCSIIO *ccb)
{
    int             status;
    SIM_HBA         *hba = tgt->hba;

    EXEC_TRACE;
    UNUSED(hba);
    tgt->nexus = ccb;

    eide_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, 5,
        "%s:  flgs=%x, port=%d, cmd=%x ccb->cam_timeout=%d "
        "hba->timeout = %d", __FUNCTION__,
        tgt->tflags, tgt->port, ccb->cam_cdb_io.cam_cdb_bytes[0],
        ccb->cam_timeout, hba->timeout);

    if (tgt->verbosity > 3 || tgt->hba->verbosity > 3) {
        eide_display_ccb(tgt, ccb);
    }

    if (!(tgt->tflags & EIDE_TFLG_PRESENT)) {
        eide_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, 5,
            "%s:  tid %d not present", __FUNCTION__, tgt->port);

        return (CAM_TID_INVALID);
    }

    // Check for SCSI/ATA Translation Layer
    if ((status = eide_satl(tgt, ccb)) == CAM_PROVIDE_FAIL) {
        if (tgt->tflags & EIDE_TFLG_ATAPI) {
            atapi_io(tgt);
        }
        else {
            ata_io(tgt);
        }

        //#warning "Need a delay here ? "
        /* Samsung - delay of 100 mSec ? */
        //delay(100);           /* 100 msec */
        //nanospin_ns(1000000); /* 1 msec */

        return (CAM_REQ_INPROG);
    }

    return (status);
}

/******************************************************************************/
// This function is used to reset the specified SCSI bus.  This
// request shall always result in the SCSI RST signal being
// asserted.
int eide_reset_bus(SIM_HBA *hba, CCB_RESETBUS *ccb)
{
    EXEC_TRACE;
    UNUSED(hba);
    UNUSED(ccb);
    return (CAM_REQ_CMP);
}

/******************************************************************************/
// This function is used to reset the specified SCSI target.  This
// function should not be used in normal operation, but if I/O to a
// particular device hangs up for some reason, drivers can abort the
// I/O and reset the device before trying again.  This request shall
// always result in a bus device reset message being issued over SCSI.
int eide_reset_dev(SIM_HBA *hba, CCB_RESETDEV *ccb)
{
    EXEC_TRACE;
    UNUSED(hba);
    UNUSED(ccb);
    return (CAM_REQ_CMP);
}

/******************************************************************************/
// This function is provided so that the peripheral driver can
// release a frozen SIM queue for the selected Logical Unit.
int eide_rel_simq(SIM_HBA *hba, CCB_RELSIM *ccb)
{
    SIM_EIDE_EXT    *ext;

    EXEC_TRACE;
    ext = (SIM_EIDE_EXT *)hba->ext;

    simq_rel_simq(hba->simq, ccb);

    if (MsgSendPulse(hba->coid, ext->priority, SIM_ENQUEUE, 0) == -1) {
    }
    return (CAM_REQ_CMP);
}

/******************************************************************************/
// This function requests that a SCSI I/O request be terminated by
// identifying the CCB associated with the request.  This request
// does not necessarily result in a terminated I/O process message
// being issued over SCSI.
int eide_term_io(SIM_HBA *hba, CCB_TERMIO *ccb)
{
    EXEC_TRACE;
    UNUSED(hba);
    UNUSED(ccb);
    return (CAM_REQ_CMP_ERR);
}

/******************************************************************************/
// This function is used to get information on the installed HBA
// hardware, including number of HBAs installed
int eide_path_inq(SIM_HBA *hba, CCB_PATHINQ *ccb)
{
    SIM_EIDE_EXT        *ext;

    EXEC_TRACE;
    ext = (SIM_EIDE_EXT *)hba->ext;

    ccb->cam_initiator_id   = 7;
    ccb->cam_version_num    = CAM_VERSION;
    ccb->cam_hba_inquiry    = PI_SOFT_RST | PI_WIDE_16;
    ccb->cam_target_sprt    = 0;
    ccb->cam_hba_misc       = 0;
    ccb->cam_hba_eng_cnt    = 0;
    ccb->cam_sim_priv       = SIM_PRIV;
    ccb->cam_async_flags    = AC_BUS_RESET;
    memset(ccb->cam_vuhba_flags, 0x00, sizeof(*ccb->cam_vuhba_flags));

    ccb->cam_vuhba_flags[CAM_VUHBA_FLAGS] =
                CAM_VUHBA_FLAG_DMA | CAM_VUHBA_FLAG_ATAPI;
    ccb->cam_vuhba_flags[CAM_VUHBA_EFLAGS] |= CAM_VUHBA_EFLAG_DMA_32;

    ccb->cam_vuhba_flags[CAM_VUHBA_MAX_SG]      = hba->nsgelm;
    ccb->cam_vuhba_flags[CAM_VUHBA_MAX_LINKED]  = 1;
    ccb->cam_vuhba_flags[CAM_VUHBA_HEADS] = ext->targets[0].translation.heads;
    ccb->cam_vuhba_flags[CAM_VUHBA_SECTS] = ext->targets[0].translation.sec_trk;
    strncpy((char *)ccb->cam_hba_vid,
                (char *)hba->cfg.Description, sizeof(ccb->cam_hba_vid));

    ccb->cam_vuhba_flags[CAM_VUHBA_EFLAGS] |= CAM_VUHBA_EFLAG_PNP;

    return (CAM_REQ_CMP);
}


/******************************************************************************/
//  All CCB requests to the SIM are made through this function.
int eide_sim_action(SIM_HBA *hba, CCB *ccb_ptr)
{
    SIM_EIDE_EXT    *ext;
    CCB_HEADER      *ccb;
    int             cam_status;
    int             tid;

    ccb = (CCB_HEADER *)ccb_ptr;
    ext = (SIM_EIDE_EXT *)hba->ext;
    tid = ccb->cam_target_id;

    EXEC_TRACE;
    eide_log(NULL, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, 5,
            "%s - cam_func_code=%02x cam_flags = %08x tid = %02x",
            __FUNCTION__, ccb->cam_func_code, (uint32_t)ccb->cam_flags, tid);

    if (ccb->cam_flags & (CAM_CDB_PHYS | CAM_NXT_CCB_PHYS |
            CAM_CALLBCK_PHYS | CAM_SNS_BUF_PHYS)) {
        ccb->cam_status = CAM_NO_HBA;
        return (CAM_FAILURE);
    }

    cam_status = CAM_REQ_CMP;

    switch (ccb->cam_func_code) {
        case XPT_NOOP:
            break;
        case XPT_SCSI_IO:
        case XPT_RESET_BUS:
        case XPT_RESET_DEV:
        case XPT_ABORT:
            if (tid >= EIDE_MAX_TID) {
                ccb->cam_status = CAM_TID_INVALID;
                return (CAM_FAILURE);
            }                           // fall through
        case XPT_DEVCTL:
            cam_status = CAM_REQ_INPROG;
            break;
        case XPT_REL_SIMQ:
            cam_status = eide_rel_simq(hba, (CCB_RELSIM *)ccb);
            break;
        case XPT_TERM_IO:
            cam_status = eide_term_io(hba, (CCB_TERMIO *)ccb);
            break;
        case XPT_PATH_INQ:
            cam_status = eide_path_inq(hba, (CCB_PATHINQ *)ccb);
            break;
        case XPT_GDEV_TYPE:
        case XPT_SDEV_TYPE:
        case XPT_SASYNC_CB:
            // These are not serviced by the SIM, the XPT should do them
            cam_status = CAM_REQ_INVALID;
            break;

        case XPT_EN_LUN:
        case XPT_TARGET_IO:
            cam_status = CAM_FUNC_NOTAVAIL;
            break;

        case XPT_VUNIQUE:
            // no special EIDE commands
            cam_status = CAM_REQ_INVALID;
            break;

        case XPT_ENG_INQ:
        case XPT_ENG_EXEC:
        default:
            cam_status = CAM_REQ_INVALID;
            break;
    }

    if (cam_status != CAM_REQ_INPROG) {
        ccb->cam_status = cam_status;
    }
    else {

        ccb->cam_status = CAM_REQ_INPROG;
        simq_ccb_enqueue(hba->simq, (CCB_SCSIIO *)ccb);

        if (MsgSendPulse(hba->coid, ext->priority, SIM_ENQUEUE, 0) == -1) {
            // timer handler will catch request
        }
    }

    return (CAM_SUCCESS);
}

/******************************************************************************/
int eide_ccb_abort(SIM_HBA *hba, CCB_ABORT *ccb_abort)
{
    EXEC_TRACE;
    UNUSED(hba);
    UNUSED(ccb_abort);
    return (CAM_UA_ABORT);
}

/******************************************************************************/
#define LD_CMD_PC_STANDBY_MEDIA     0x80

int eide_power_mode(EIDE_TARGET *tgt, int mode)
{
    SIM_HBA         *hba = tgt->hba;
    int             status;

    EXEC_TRACE;
    status = EOK;

    if ((tgt->tflags & EIDE_TFLG_APM_SLEEP) && mode != LD_CMD_PC_SLEEP) {
        eide_init_device(tgt, EIDE_INIT_RESET);
    }

    switch (mode) {
        case LD_CMD_PC_ACTIVE:      // fall through
        case LD_CMD_PC_IDLE:
            if (tgt->tflags & EIDE_TFLG_APM) {
                status = ata_command(tgt,
                            EIDE_CMD_IDLE_IMMEDIATE, 0, 0, 0, 0,
                            0, 0, NULL, 0, 0, NULL, hba->timeout);
            }
            tgt->tflags &= ~EIDE_TFLG_APM_STANDBY_MEDIA;
            break;

        case LD_CMD_PC_STANDBY:
            if (tgt->tflags & EIDE_TFLG_APM) {
                status = ata_command(tgt,
                            EIDE_CMD_STANDBY_IMMEDIATE, 0, 0, 0, 0,
                            0, 0, NULL, 0, 0, NULL, hba->timeout);
            }
            tgt->tflags &= ~EIDE_TFLG_APM_STANDBY_MEDIA;
            break;

        case LD_CMD_PC_SLEEP:
            if (tgt->tflags & EIDE_TFLG_APM) {
                if (!(tgt->tflags & EIDE_TFLG_APM_SLEEP)) {
                    if ((status = ata_command(tgt,
                            EIDE_CMD_SLEEP, 0, 0, 0, 0,
                            0, 0, NULL, 0, 0, NULL,
                            hba->timeout)) == CAM_SUCCESS) {
                        tgt->tflags |= EIDE_TFLG_APM_SLEEP;
                    }
                }
                break;
            }
            else {
                if (tgt->tflags & EIDE_TFLG_ATAPI) {
#ifdef EIDE_ATAPI_SUPPORT
                    status = atapi_power_mode(tgt, mode); break;
#endif
                }
                status = EINVAL;
            }
            break;

        case LD_CMD_PC_STANDBY_MEDIA:
            if (tgt->tflags & EIDE_TFLG_ATAPI) {
#ifdef EIDE_ATAPI_SUPPORT
                status = atapi_power_mode(tgt, mode); break;
#endif
            }
            status = EINVAL;
            break;

        default:
            status = EINVAL;
            break;
    }

    return (status);
}

/******************************************************************************/
int eide_standby_interval(EIDE_TARGET *tgt, int level, int interval)
{
    SIM_HBA         *hba = tgt->hba;

    EXEC_TRACE;
    if (tgt->tflags & EIDE_TFLG_APM) {
        return (ata_command(tgt, level, 0, 0, interval, 0, 0, 0,
                    NULL, 0, 0, NULL, hba->timeout));
    }
    return (EINVAL);
}

/******************************************************************************/
int eide_apm_level(EIDE_TARGET *tgt, int level)
{
    SIM_HBA         *hba = tgt->hba;

    EXEC_TRACE;
    if (tgt->tflags & EIDE_TFLG_APM) {
        return (ata_command(tgt, EIDE_CMD_SET_FEATURE, 0, EIDE_FEAT_EN_APM,
                    level, 0, 0, 0, NULL, 0, 0, NULL, hba->timeout));
    }
    return (EINVAL);
}

/******************************************************************************/
int eide_check_power_mode(EIDE_TARGET *tgt, uint32_t *level)
{
    ATA_TASKFILE    tf;
    SIM_HBA         *hba = tgt->hba;
    int             status;

    EXEC_TRACE;
    status  = CAM_SUCCESS;
    *level  = EIDE_CPM_ACTIVE;

    if (tgt->tflags & EIDE_TFLG_APM_STANDBY_MEDIA) {
        *level = EIDE_CPM_STANDBY_MEDIA;
    }
    else if (tgt->tflags & EIDE_TFLG_APM_SLEEP) {
        *level  = EIDE_CPM_STANDBY;
    }
    else if (tgt->tflags & EIDE_TFLG_APM) {
        if ((status = ata_command(tgt, EIDE_CMD_CHECK_POWER_MODE, 0, 0, 0,
                0, 0, 0, NULL, 0, 0, NULL, hba->timeout)) == CAM_SUCCESS) {
            sata_tf_read(tgt, &tf);
            *level =  tf.seccnt;
        }
    }
    return (status);
}

/******************************************************************************/
int eide_apm(EIDE_TARGET *tgt, CCB_DEVCTL *ccb)
{
    eide_apm_t      *apm;
    int             status;
    int             cstatus;

    EXEC_TRACE;
    apm         = (eide_apm_t *)(ccb->cam_devctl_data);
    status      = EINVAL;
    cstatus     = CAM_REQ_CMP;

    switch (apm->action) {
        case EIDE_APM_LEVEL:        // set level
            status = eide_apm_level(tgt, apm->level);
            break;

        case EIDE_APM_CPM:          // Check power mode level
            status = eide_check_power_mode(tgt, &apm->level);
            break;

        case EIDE_APM_MODE:         // set power mode
            status = eide_power_mode(tgt, apm->level);
            break;

        case EIDE_APM_STANDBY_INTERVAL: // set standby interval
            status = eide_standby_interval(tgt, apm->level > 0xff ?
                    EIDE_CMD_STANDBY : EIDE_CMD_IDLE, apm->level);
            break;

        default:
            status = EINVAL;
            break;
    }

    if (cstatus == CAM_REQ_CMP) {
        switch (status) {
            case CAM_SUCCESS:
                ccb->cam_devctl_status  = EOK; break;
            case CAM_FAILURE:
                ccb->cam_devctl_status  = EIO; break;
            case ENOMEM:
                ccb->cam_devctl_status  = ENOMEM; break;
            case EINVAL:
                ccb->cam_devctl_status  = EINVAL; break;
            case EINPROGRESS:
                cstatus = CAM_REQ_INPROG; break;
            default:
                ccb->cam_devctl_status  = EINVAL; break;
        }
    }
    return (cstatus);
}

/******************************************************************************/
int eide_dsn(EIDE_TARGET *tgt, CCB_DEVCTL *ccb)
{
    char            *dsn;
    int             ml;
    int             sl;

    EXEC_TRACE;
    dsn     = (char *)ccb->cam_devctl_data;
    memset(dsn, 0, ccb->cam_devctl_size);
    ccb->cam_devctl_status  = EOK;
    ml = strlen(tgt->mdl); sl = strlen(tgt->sn);
    if (snprintf(dsn, ccb->cam_devctl_size, "%s%s%s", tgt->mdl,
            (ml && sl) ? " " : "", tgt->sn) > ccb->cam_devctl_size) {
        ccb->cam_devctl_status = ENAMETOOLONG;
    }
    return (CAM_REQ_CMP);
}

/******************************************************************************/
int eide_verbosity(EIDE_TARGET *tgt, CCB_DEVCTL *ccb)
{
    SIM_HBA             *hba;
    CAM_VERBOSITY       *ver;

    EXEC_TRACE;
    hba = (SIM_HBA *)tgt->hba;
    ver = (CAM_VERBOSITY *)ccb->cam_devctl_data;

    if (ver->modules == CAM_MODULE_SIM) {
        ccb->cam_devctl_status  = EOK;
        if (ver->flags) {
            hba->verbosity = ver->verbosity;
        }
        else {
            tgt->verbosity = ver->verbosity;
        }
    }
    return (CAM_REQ_CMP);
}

/******************************************************************************/
int eide_endianize(resmgr_context_t *ctp, uint32_t dcmd, void *data, size_t nbytes)
{
    uint32_t            *dptr;
    int                 ssize;
    int                 status;

    EXEC_TRACE;
    status  = EOK;
    ssize   = 0;
    dptr    = (uint32_t *)data;

    if (ctp == NULL || !(ctp->info.flags & _NTO_MI_ENDIAN_DIFF)) {
        return (EOK);
    }

    switch (dcmd) {
        case DCMD_SIM_EIDE_APM:
            ssize = sizeof(eide_apm_t) / 4; break;

        case ~DCMD_SIM_EIDE_APM:
            ssize = sizeof(eide_apm_t) / 4; break;
    }

    if (ssize) {
        if (ssize > (nbytes / 4)) {
            return (EINVAL);
        }
        CAM_ENDIAN_SWAP32(dptr, ssize);
    }

    return (status);
}

/******************************************************************************/
int eide_devctl(EIDE_TARGET *tgt, CCB_DEVCTL *ccb)
{
    uint32_t        dcmd;
    int             estatus;
    int             cstatus;

    EXEC_TRACE;
    dcmd    = ccb->cam_devctl_dcmd;
    cstatus = CAM_REQ_CMP;

    if ((estatus = eide_endianize((get_device_direction(dcmd) & DEVDIR_TO) ?
            ccb->cam_devctl_ctp : NULL, dcmd,
            ccb->cam_devctl_data, ccb->cam_devctl_size))) {
        ccb->cam_devctl_status = estatus;
        return (cstatus);
    }

    switch (dcmd) {
        case DCMD_CAM_VERBOSITY:
            cstatus = eide_verbosity(tgt, ccb);
            break;

        case DCMD_CAM_DEVICE_TEMPERATURE:
            if ((tgt->tflags & EIDE_TFLG_APM_SLEEP) ||
                    !(tgt->tflags & EIDE_TFLG_PRESENT)) {
                ccb->cam_devctl_status  = EIO;
            }
            else {
                cstatus = ds_temperature(tgt, ccb);
            }
            break;

        case DCMD_CAM_DEV_SERIAL_NUMBER:
            cstatus = eide_dsn(tgt, ccb);
            break;

        case DCMD_SIM_EIDE_APM:
            cstatus = eide_apm(tgt, ccb);
            break;

        default:
            cstatus = ds_devctl(tgt, ccb);
            break;
    }

    eide_endianize((get_device_direction(dcmd) & DEVDIR_FROM) ?
            ccb->cam_devctl_ctp : NULL,
            ~dcmd, ccb->cam_devctl_data, ccb->cam_devctl_size);

    return (cstatus);
}

/******************************************************************************/
void eide_start_ccb(SIM_HBA *hba)
{
    SIM_EIDE_EXT    *ext = (SIM_EIDE_EXT *)hba->ext;
    EIDE_TARGET     *tgt;
    CCB_SCSIIO      *ccb;
    int             cam_status;
    uint8_t         log_level = 5;//0;//5;

    do {
        if ((ccb = simq_ccb_dequeue(hba->simq)) == NULL)
            return;

        EXEC_TRACE;

        eide_log(NULL, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, log_level,
                        "%s - cam_func_code = %08x", __FUNCTION__,
                        ccb->cam_ch.cam_func_code);

        tgt = &ext->targets[ccb->cam_ch.cam_target_id];
        cam_status = CAM_REQ_CMP;

        switch (ccb->cam_ch.cam_func_code) {
            case XPT_SCSI_IO:
                // Start scsi cmd, complete with xpt_async() call
                cam_status = eide_scsi_io(tgt, (CCB_SCSIIO *)ccb);
                break;

            case XPT_RESET_BUS:
                cam_status = eide_reset_bus(hba, (CCB_RESETBUS *)ccb);
                break;

            case XPT_RESET_DEV:
                cam_status = eide_reset_dev(hba, (CCB_RESETDEV *)ccb);
                break;

            case XPT_ABORT:
                cam_status = eide_ccb_abort(hba, (CCB_ABORT *)ccb);
                break;

            case XPT_TERM_IO:
                cam_status = eide_term_io(hba, (CCB_TERMIO *)ccb);
                break;

            case XPT_DEVCTL:
                cam_status = eide_devctl(tgt, (CCB_DEVCTL *)ccb);
                break;
        }

        if (cam_status != CAM_REQ_INPROG) {
            ccb->cam_ch.cam_status = cam_status;
            eide_post_ccb(tgt, ccb);
        }
    } while (tgt->nexus == NULL);
}

/******************************************************************************/
void eide_post_ccb(EIDE_TARGET *tgt, CCB_SCSIIO *ccb)
{
    SIM_HBA         *hba = tgt->hba;

    EXEC_TRACE;

    tgt->nexus = NULL;
    simq_post_ccb(hba->simq, ccb);
}

/******************************************************************************/
int eide_process_interrupt(SIM_HBA *hba, uint32_t flags)
{
    SIM_EIDE_EXT    *ext = (SIM_EIDE_EXT *)hba->ext;;

    EXEC_TRACE;
    sata_interrupt(&ext->targets[0]);

    if (flags & EIDE_CONTEXT_INTR) {
        InterruptUnmask(hba->cfg.IRQRegisters[0], hba->iid);
    }

    return (CAM_FALSE);
}

/******************************************************************************/
int eide_timer(SIM_HBA *hba)
{
    SIM_EIDE_EXT    *ext = (SIM_EIDE_EXT *)hba->ext;
    EIDE_TARGET     *tgt;
    CCB_SCSIIO      *ccb;
    SIMQ_DATA       *pdata;
    int             port, timeo = CAM_FALSE;
    struct timespec time;
    uint8_t         log_level = 1;//0;//1;

    if (eide_ctrl.dmode) {
        return (CAM_SUCCESS);
    }

    for (port = 0; port < EIDE_MAX_TID; port++) {
        tgt = &ext->targets[port];
        ccb = tgt->nexus;
        if (ccb) {
            if (ccb->cam_timeout && ccb->cam_timeout != CAM_TIME_INFINITY) {
                pdata = (SIMQ_DATA *)ccb->cam_sim_priv;
                simq_clock_gettime(hba->simq, &time);
                if (pdata->timeout < time.tv_sec) {
                    timeo = CAM_TRUE;
                }
            }

            if (!(tgt->tflags & EIDE_TFLG_PRESENT) || timeo) {
                eide_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, log_level,
                        "%s:  timeout path %d, device %d", __FUNCTION__,
                        (int)hba->pathid, ccb->cam_ch.cam_target_id);

                eide_display_ccb(tgt, ccb);
                if (tgt->nfcnp) {
                    tgt->nfcnp(tgt, CAM_CMD_TIMEOUT);
                }
            }
        }
    }

    return (CAM_SUCCESS);
}

/******************************************************************************/
void eide_pulse_handler(SIM_HBA *hba)
{
    struct _pulse   pulse;
    int             rid;

    while (1) {
        if ((rid = MsgReceivePulse(hba->chid,
                                &pulse, sizeof(pulse), NULL)) == -1) {
            break;
        }

        switch (pulse.code) {
            case SIM_ENQUEUE:
                EXEC_TRACE;
                eide_start_ccb(hba);
                break;

            case SIM_INTERRUPT:
                EXEC_TRACE;
                if (eide_process_interrupt(hba,
                                    EIDE_CONTEXT_INTR) == CAM_TRUE) {
                    InterruptUnmask(hba->cfg.IRQRegisters[0], hba->iid);
                }
                break;

            case SIM_TIMER:
                eide_timer(hba);
                break;

            default:
                EXEC_TRACE;
                eide_log(NULL, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                    "%s: unknown type %x, subtype %, pulse %x, "
                    "value %x, scoid %x", __FUNCTION__, pulse.type,
                    pulse.subtype, pulse.code, pulse.value, pulse.scoid);
                break;
        }
        eide_start_ccb(hba);
    }
}

/******************************************************************************/
void *eide_driver_thread(void *data)
{
    SIM_HBA         *hba;
    SIM_EIDE_EXT    *ext;
    EIDE_TARGET     *tgt;
    struct sigevent event;
    int             state;

    EXEC_TRACE;
    hba = (SIM_HBA *)data;
    ext = (SIM_EIDE_EXT *)hba->ext;

    state = HBA_STATE_FAILURE;

    SIGEV_PULSE_INIT(&event, hba->coid, ext->priority, SIM_INTERRUPT, NULL);
    if ((hba->iid = InterruptAttachEvent(hba->cfg.IRQRegisters[0],
                                &event, _NTO_INTR_FLAGS_TRK_MSK)) != -1) {

        tgt = &ext->targets[0];
        tgt->hba = hba;
        if (tgt->tflags & EIDE_TFLG_PRESENT) {
            eide_init_device(tgt, EIDE_INIT_NORESET);
        }

        state = HBA_STATE_INITIALIZED;
    }

    // Wake up sim_init thread
    pthread_sleepon_lock();
    hba->state = state;
    pthread_sleepon_signal(&hba->state);
    pthread_sleepon_unlock();
    if (state == HBA_STATE_INITIALIZED) {
        eide_pulse_handler(hba);
    }
    return (void *)(CAM_SUCCESS);
}

/******************************************************************************/
SIM_HBA *eide_alloc_hba()
{
    SIM_HBA         *hba;
    SIM_EIDE_EXT    *ext;
    EIDE_TARGET     *tgt;
    uint32_t        port;

    EXEC_TRACE;
    if ((eide_ctrl.nhba + 1) > EIDE_MAX_HBA) {  // Max hba's reached
        return (NULL);
    }

    if ((hba = sim_alloc_hba(sizeof(SIM_EIDE_EXT))) == NULL) {
        return (NULL);
    }

    /* Set some defaults */
    hba->nsgelm         = COMMAND_PRD_MAX_NUM;
    hba->stride         = 1;
    hba->verbosity      = eide_ctrl.verbosity;
    hba->tid = hba->coid = hba->chid = hba->iid = -1;

    ext = (SIM_EIDE_EXT *)hba->ext;
    // Set some defaults
    ext->priority = EIDE_SCHED_PRIORITY;
    ext->eflags = EIDE_EFLG_USE_BMSTR;

    for (port = 0; port < EIDE_MAX_TARGETS; port++) {
        tgt             = &ext->targets[port];
        tgt->hba        = hba;
        tgt->verbosity  = hba->verbosity;
        tgt->tflags     |=  EIDE_TFLG_USE_LBA48 | EIDE_TFLG_USE_LBA |
                            EIDE_TFLG_USE_BMSTR | EIDE_TFLG_USE_UDMA |
                            EIDE_TFLG_USE_MDMA | EIDE_TFLG_USE_PIO |
                            EIDE_TFLG_REMOVABLE;// | EIDE_TFLG_PWRDOWN;
        tgt->ws.sz      = EIDE_WS_SIZE;
    }

    /* Add hba to drivers hba list */
    TAILQ_INSERT_TAIL(&eide_ctrl.hlist, hba, hlink);
    eide_ctrl.nhba++;
    return (hba);
}

/******************************************************************************/
int eide_geometry(EIDE_TARGET *tgt, char *arg)
{
    int     heads;
    int     cyls;
    int     sects;
    char    *p;

    EXEC_TRACE;
    heads   = strtol(arg, (char **)&p, 10);
    if (p == 0 || *p != ':') {
        return (EINVAL);
    }

    cyls = strtol(p + 1, (char **)&p, 10);
    if (p == 0 || *p != ':') {
        return (EINVAL);
    }

    sects = strtol(p + 1, (char **)&p, 10);

    tgt->geometry.heads     = heads;
    tgt->geometry.sec_trk   = sects;
    tgt->geometry.cylinders = cyls;
    return (EOK);
}

/******************************************************************************/
#if !defined(SATA_NO_DEVICE_PARAMS)

static char *eide_device_args(char *options, EIDE_TARGET *tgt)
{
    char            *value;
    int             opt;
    int             comma;
    uint32_t        val;

    /* Device specific options */
    static char     *opts[] = {
            "verbose",
            "geometry",
            "chs",
            "multiblk",
            "nobmstr",
            "wcache",
            "rahead",
            "nonremovable",
            "pwrdwn",
            "smart",
            "pio",              /* 10 */
            NULL };

    EXEC_TRACE;
    while (*options != '\0') {
        val = 0;
        comma = (int)strchr(options, ',');
        if ((opt = getsubopt(&options, opts, &value)) == -1) {
            if (comma) {
                options[-1] = ',';  /* Backup opt string */
            }
            while (options[-1] != '\0') {
                options--;
            }
            return (options);
        }

        switch (opt) {
            case 0: /* Verbose */
                EIDE_ARG_VAL(opts[opt], value);
                tgt->verbosity = strtoul(value, 0, 0);
                break;
            case 1: /* Drive geometry */
                EIDE_ARG_VAL(opts[opt], value);
                if (eide_geometry(tgt, value) == -1) {
                    eide_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                            "%s:  invalid geometry", __FUNCTION__);
                }
                break;
            case 2: /* Use CHS mode */
                tgt->tflags &= ~EIDE_TFLG_USE_LBA;
                break;
            case 3: /* Multi-block */
                EIDE_ARG_VAL(opts[opt], value);
                if (!(tgt->rw_multiple = strtoul(value, 0, 0))) {
                    tgt->tflags &= ~EIDE_TFLG_USE_MBLK;
                    tgt->rw_multiple = 1;
                }
                break;
            case 4:
                tgt->tflags &= ~(EIDE_TFLG_USE_UDMA);
                break;
            case 5: /* Enable/disable write cache */
                EIDE_ARG_VAL(opts[opt], value);
                if (!strcmp(value, "on")) {
                    tgt->tflags |= EIDE_TFLG_WRITE_CACHE_ON;
                }
                else if (!strcmp(value, "off")) {
                    tgt->tflags |= EIDE_TFLG_WRITE_CACHE_OFF;
                }
                break;
            case 6: /* Enable/disable read ahead */
                EIDE_ARG_VAL(opts[opt], value);
                if (!strcmp(value, "on")) {
                    tgt->tflags |= EIDE_TFLG_READ_AHEAD_ON;
                }
                else if (!strcmp(value, "off")) {
                    tgt->tflags |= EIDE_TFLG_READ_AHEAD_OFF;
                }
                break;
            case 7: /* Set device as non-removable */
                tgt->tflags &= ~EIDE_TFLG_REMOVABLE;
                break;
            case 8: /* Power device at exit */
                tgt->tflags |= EIDE_TFLG_PWRDOWN;
                break;
            case 9: /* Enable SMART */
                tgt->tflags |= EIDE_TFLG_SMART;
                break;
            case 10: /* PIO mode */
                EIDE_ARG_VAL( opts[opt], value );
                if (!strcmp(value, "off")) {
                    tgt->tflags &= ~EIDE_TFLG_USE_PIO;
                }
                else {
                    if ((val = strtoul(value, 0, 0)) > 4) {
                        val = 4;
                    }
                    tgt->pio_mode = 1 << val;
                }
                break;
            default:
                break;
        }
    }

    return (options);
}

#endif      /* SATA_NO_DEVICE_PARAMS */

/******************************************************************************/
int eide_sim_args(char *options)
{
    SIM_HBA         *hba;
    SIM_EIDE_EXT    *ext;
    EIDE_TARGET     *tgt;
    char            *value;
    int             opt;
    int             val;

    /* Interface specific eide options */
    static char     *opts[] = {
            "ioport",       /* 0 */
            "ioportlen",    /* 1 */
            "irq",          /* 2 */
            "verbose",      /* 3 */
            "priority",     /* 4 */
            "timeout",      /* 5 */
#if !defined(SATA_NO_DEVICE_PARAMS)
            "master",       /* 6 */
#endif      /* SATA_NO_DEVICE_PARAMS */
            NULL };

    EXEC_TRACE;
    if ((hba = eide_alloc_hba()) == NULL) {
        return (CAM_FAILURE);
    }

    hba->timeout = EIDE_DFLT_TIMEOUT;

    ext = (SIM_EIDE_EXT *)hba->ext;

    while (*options != '\0') {
        if ((opt = getsubopt(&options, opts, &value)) == -1) {
            if (sim_drvr_options(hba, value) != EOK) {
                continue;
            }
        }
        switch (opt) {
            case 0: /* SATA controller registers base address */
                if (hba->cfg.NumIOPorts)
                    break;
                EIDE_ARG_VAL(opts[opt], value);

                ext->IOPort_Base = strtoull(value, 0, 0);
                hba->cfg.NumIOPorts = 1;
                break;
            case 1: /* Length of SATA controller registers address space */
                if (hba->cfg.IOPort_Length[0])
                    break;
                EIDE_ARG_VAL(opts[opt], value);
                hba->cfg.IOPort_Length[0] = strtoul(value, 0, 0);
                break;
            case 2: /* IRQ */
                if (hba->cfg.NumIRQs)
                    break;
                EIDE_ARG_VAL(opts[opt], value);
                hba->cfg.IRQRegisters[0] = strtoul(value, 0, 0);
                hba->cfg.NumIRQs = 1;
                break;
            case 3: /* Verbose */
                EIDE_ARG_VAL(opts[opt], value);
                hba->verbosity = strtoul(value, 0, 0);
                tgt = &ext->targets[0];
                if (!tgt->verbosity) {
                    tgt->verbosity = hba->verbosity;
                }
                break;
            case 4: /* Priority */
            {
                int min_pri, max_pri;

                EIDE_ARG_VAL(opts[opt], value);
                val = strtol(value, 0, 0);
                min_pri = sched_get_priority_min(SCHED_RR);
                max_pri = sched_get_priority_max(SCHED_RR);

                if (val >= min_pri && val <= max_pri) {
                    ext->priority = val;
                }
                else {
                    eide_log(NULL, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                            "%s:  invalid priority %d (min = %d max = %d)",
                            __FUNCTION__, val, min_pri, max_pri);
                }
                break;
            }
            case 5: /* I/O request timeout */
                EIDE_ARG_VAL(opts[opt], value);
                val = strtol(value, 0, 0);
                if (val > 1) {
                    hba->timeout = val;
                }
                break;
#if !defined(SATA_NO_DEVICE_PARAMS)
            case 6: /* Device options */
            {
                int     len;

                EIDE_ARG_VAL(opts[opt], value);

                len = strlen( value );
                if( *options != '\0' ) {
                    options[-1] = ',';
                    len++;
                }
                options -= len;
                if (options && *options != NULL) {
                    options = eide_device_args(options, &ext->targets[0]);
                }
                break;
            }
#endif      /* SATA_NO_DEVICE_PARAMS */

            default:
                break;
        }
    }

    return (CAM_SUCCESS);
}

/******************************************************************************/
int eide_bus_register(SIM_HBA *hba)
{
    int     path;

    EXEC_TRACE;
    if ((path = xpt_bus_register(&eide_sim_entry, hba)) == -1)
        return (CAM_FAILURE);

    eide_log(NULL, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, 5,
            "\t%s REGISTERED : path =%d eide_ctrl.pathid_max = %d",
            __FUNCTION__, path, eide_ctrl.pathid_max);

    if ((hba->pathid = path) > eide_ctrl.pathid_max)
        eide_ctrl.pathid_max = path;

    return (CAM_SUCCESS);
}

/******************************************************************************/
int eide_sim_attach(CAM_ENTRY *centry)
{
    SIM_HBA         *hba;
    SIM_HBA         *nhba;
    int             found;

    EXEC_TRACE;
    found = CAM_FALSE;

    for (hba = TAILQ_FIRST(&eide_ctrl.hlist); hba; hba = nhba) {
        nhba = TAILQ_NEXT(hba, hlink);

        if (sata_init(hba) == CAM_SUCCESS) {
            strncpy((char *)hba->cfg.Description,
                    "RCAR SATA", sizeof(hba->cfg.Description));
            continue;
        }
        eide_free_hba(hba);
    }

    for (hba = TAILQ_FIRST(&eide_ctrl.hlist); hba; hba = nhba) {
        nhba = TAILQ_NEXT(hba, hlink);

        hba->cam_funcs = centry;
        if (hba->cfg.NumIOPorts && eide_init(hba) == CAM_SUCCESS) {
            if (eide_bus_register(hba) == CAM_SUCCESS) {
                found = CAM_TRUE;
                continue;
            }
        }
        eide_sim_idetach(hba);
    }

    return (found ? CAM_SUCCESS : CAM_NO_HBA);
}

/******************************************************************************/
int eide_create_thread(pthread_t *tid, pthread_attr_t *aattr,
    void *(*func)(void *), void *arg, int priority, uint32_t *tstate, char *name)
{
    pthread_t           _tid;
    pthread_attr_t      *pattr;
    pthread_attr_t      attr;
    struct sched_param  param;
    int                 status;

    EXEC_TRACE;

    if (tid == NULL)
        tid = &_tid;
    if (tstate)
        *tstate = EIDE_TSTATE_CREATING;

    pattr = aattr;
    if (aattr == NULL) {
        pattr = &attr;
        pthread_attr_init(&attr);
        pthread_attr_setschedpolicy(&attr, SCHED_RR);
        param.sched_priority = priority;
        pthread_attr_setschedparam(&attr, &param);
        pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_attr_setstacksize(&attr, EIDE_STACK_SIZE);
    }

    if ((status = pthread_create(tid, pattr, func, arg)))
        return (status);

    if (name) {
#ifdef CAM_PTHREAD_SETNAME
#if (_NTO_VERSION >= 632)
        if (pthread_setname_np(*tid, name)) {
            eide_log(NULL, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                "%s:  pthread_setname_np %s", __FUNCTION__, strerror(status));
        }
#endif
#endif
    }

    if (tstate) {
        pthread_sleepon_lock();
        while (!(*tstate)) {
            pthread_sleepon_wait(tstate);
        }
        pthread_sleepon_unlock();
        status = *tstate == EIDE_TSTATE_INITIALIZED ? EOK : EIO;
    }

    return (status);
}

/******************************************************************************/
// This function is called by the XPT to request that the SIM be initialized.
int eide_sim_init(SIM_HBA *hba, int path)
{
    SIM_EIDE_EXT        *ext;
    pthread_attr_t      attr;
    struct sched_param  param;

    EXEC_TRACE;
    ext = (SIM_EIDE_EXT *)hba->ext;

    if (hba->state)
        return (CAM_SUCCESS);

    pthread_attr_init(&attr);
    pthread_attr_setschedpolicy(&attr, SCHED_RR);
    param.sched_priority = ext->priority;
    pthread_attr_setschedparam(&attr, &param);
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&attr, EIDE_STACK_SIZE);

    // Initialize SIM queue routines
    if ((hba->simq = simq_init(hba->coid, hba, EIDE_MAX_TID,
                        1, 1, 2, EIDE_MAX_TID, hba->timeout))) {
        // Create the interface thread and wait for it to
        // initialize/attach the interrupt routine

        if (eide_create_thread(&hba->tid, NULL, eide_driver_thread,
                hba, ext->priority, NULL, "eide_driver_thread") == EOK) {
            pthread_sleepon_lock();

            while (!hba->state)
                pthread_sleepon_wait(&hba->state);

            pthread_sleepon_unlock();
            if (hba->state == HBA_STATE_INITIALIZED)
                return (CAM_SUCCESS);
        }
        else {
            eide_log(NULL, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                    "%s:  Unable to create driver thread", __FUNCTION__);
        }
    }
    else {
        eide_log(NULL, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                                "%s:  simq_init failure", __FUNCTION__);
    }

    eide_sim_idetach(hba);
    return (CAM_FAILURE);
}

/******************************************************************************/
int eide_sim_detach(void)
{
    SIM_HBA     *hba;
    SIM_HBA     *nhba;

    EXEC_TRACE;
    for (hba = TAILQ_FIRST(&eide_ctrl.hlist); hba; hba = nhba) {
        nhba = TAILQ_NEXT(hba, hlink);
        eide_sim_idetach(hba);
    }

    return (CAM_SUCCESS);
}

/******************************************************************************/
int eide_error(EIDE_TARGET *tgt, int scsi_status, int cam_status)
{
    SIM_HBA         *hba = tgt->hba;
    CCB_SCSIIO      *ccb = tgt->nexus;

    EXEC_TRACE;
    UNUSED(hba);

    eide_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
            "%s:  path=%d, device=%d cam_status=%d scsi_status=%d, "
            "status=%x, error=%x, cmd=%x",
        __FUNCTION__, (int)hba->pathid, 0, cam_status, scsi_status,
        get_tf_status_reg(tgt), get_tf_error_reg(tgt),
        ccb->cam_cdb_io.cam_cdb_bytes[0]);

    eide_dump_regs(tgt);

    // Reset the interface when BSY or DRQ is set
    if ((get_tf_status_reg(tgt) & (EIDE_STATUS_BSY | EIDE_STATUS_DRQ))) {
        eide_init_device(tgt, EIDE_INIT_RESET);
    }

    ccb->cam_ch.cam_status  = cam_status;
    ccb->cam_scsi_status    = scsi_status;
    eide_post_ccb(tgt, ccb);
    return (CAM_SUCCESS);
}

/******************************************************************************/
int eide_workspace_init(EIDE_TARGET *tgt, int sz)
{
    EIDE_WORKSPACE      *ws;
    SG_ELEM             *sg;
    char                *buf;
    mdl_t               *mdl;
    int                 pg_size;

    EXEC_TRACE;
    ws      = &tgt->ws;
    pg_size = eide_ctrl.pagesize;

    if (ws->buf && ws->sz >= sz) {
        return (CAM_SUCCESS);
    }

    if (ws->buf) {
        eide_free(ws->buf, ws->sz);
        ws->buf = NULL;
    }

    sz = CAM_ALIGN(sz, pg_size);
    sz = max(sz, ws->sz);

    if ((buf = eide_alloc(sz)) == MAP_FAILED) {
        eide_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                "%s:  Unable to alloc workspace", __FUNCTION__);
        return (CAM_FAILURE);
    }

    ws->sz          = sz;
    ws->buf         = buf;
    sg              = ws->sgl;
    mdl             = ws->mdl;
    ws->ioreq.mdl   = mdl;

    for (; sz; buf += pg_size, sz -= pg_size, sg++, mdl++) {
        sg->cam_sg_address  = (paddr_t)(buf);
        sg->cam_sg_count    = pg_size;
        mdl->vaddr          = buf;
        mdl->paddr          = cam_mphys(buf);
    }
    return (CAM_SUCCESS);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/devb/rcarsata/sim_eide.c $ $Rev: 853240 $")
#endif
