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

// Module Description: SATA interface

#include <sim_eide.h>

/*****************************************************************************/
#if defined(EIDE_SATA_DEBUG)
    #define sata_log            eide_slogf
#else
    #define sata_log(_a, _b, _c, _d, _fmt, ...)

    #if defined UNUSED
        #undef UNUSED
        #define UNUSED(_a)  (_a) = (_a)
    #endif

    #if defined EXEC_TRACE
        #undef EXEC_TRACE
        #define EXEC_TRACE
    #endif

#endif

#define ATA_HOB         (1 << 7)            /* LBA48 selector */
#define ATA_DEVCTL_OBS  (1 << 3)            /* Obsolete bit in devctl reg */
#define ATA_SRST        (1 << 2)            /* Software reset */
#define ATA_DEVICE_OBS  (1 << 7) | (1 << 5) /* Obsolete bits in dev reg */

#if defined(EIDE_SATA_DEBUG)
/*****************************************************************************/
static char *sata_cmd_description(int cmd)
{
    static EIDE_COMMAND eide_cmd_desc[] = {
        { EIDE_CMD_MEDIA_EJECT, "Media Eject" },
        { EIDE_CMD_ACK_MEDIA_CHNG, "Ack Media Change" },
        { EIDE_CMD_BOOT_POST_BOOT, "Boot Post Boot" },
        { EIDE_CMD_BOOT_PRE_BOOT, "Boot Pre Boot" },
        { EIDE_CMD_CHECK_POWER_MODE, "Check Power Mode" },
        { EIDE_CMD_DOOR_LOCK, "Door Lock" },
        { EIDE_CMD_DOOR_UNLOCK, "Door Unlock" },
        { EIDE_CMD_EXECUTE_DRV_DIAGS, "Execute Drive Diag" },
        { EIDE_CMD_DOWNLOAD_MICROCODE, "Download Microcode" },
        { EIDE_CMD_FORMAT_TRACK, "Format Track" },
        { EIDE_CMD_ATA_IDENTIFY, "ATA Identify" },
        { EIDE_CMD_IDLE, "Idle" },
        { EIDE_CMD_IDLE_IMMEDIATE, "Idle Immediate" },
        { EIDE_CMD_INIT_DRV_PARMS, "Init Drive Parms" },
        { EIDE_CMD_NOP, "NOP" },
        { EIDE_CMD_ATAPI_PKT_CMD, "ATAPI Pkt Cmd" },
        { EIDE_CMD_ATAPI_IDENTIFY, "ATAPI Identify" },
        { EIDE_CMD_SERVICE, "Service" },
        { EIDE_CMD_SMART, "SMART" },
        { EIDE_CMD_READ_LOG_EXT, "Read Log Ext" },
        { EIDE_CMD_WRITE_LOG_EXT, "Write Log Ext" },
        { EIDE_CMD_ATAPI_SOFT_RESET, "ATAPI Soft Reset" },
        { EIDE_CMD_READ_BUFFER, "Read Buffer" },
        { EIDE_CMD_READ_DMA_QUEUED, "Read DMA Queued w/retry" },
        { EIDE_CMD_READ_DMA_QUEUED_EXT, "Read DMA Queued Ext" },
        { EIDE_CMD_READ_DMA, "Read DMA w/retry" },
        { EIDE_CMD_READ_DMA_R, "Read DMA wo/retry" },
        { EIDE_CMD_READ_DMA_EXT, "Read DMA Ext" },
        { EIDE_CMD_READ_LONG, "Read Long w/retry" },
        { EIDE_CMD_READ_LONG_R, "Read Long wo/retry" },
        { EIDE_CMD_READ_MULTIPLE, "Read Multiple" },
        { EIDE_CMD_READ_MULTIPLE_EXT, "Read Multiple Ext" },
        { EIDE_CMD_READ_SECTORS, "Read Sectors w/retry" },
        { EIDE_CMD_READ_SECTORS_R, "Read Sectors wo/retry" },
        { EIDE_CMD_READ_SECTORS_EXT, "Read Sectors Ext" },
        { EIDE_CMD_READ_VER_SECTORS, "Read Verify Sectors w/retry" },
        { EIDE_CMD_READ_VER_SECTORS_R, "Read Verify Sectors wo/retry" },
        { EIDE_CMD_READ_VER_SECTORS_EXT, "Read Verify Sectors Ext" },
        { EIDE_CMD_RECALIBRATE, "Recalibrate" },
        { EIDE_CMD_SEEK, "Seek" },
        { EIDE_CMD_SET_FEATURE, "Set Feature" },
        { EIDE_CMD_SET_MULTIPLE_MODE, "Set Multiple Mode" },
        { EIDE_CMD_SLEEP, "Sleep" },
        { EIDE_CMD_STANDBY, "Standby" },
        { EIDE_CMD_STANDBY_IMMEDIATE, "Standby Immediate" },
        { EIDE_CMD_WRITE_BUFFER, "Write Buffer" },
        { EIDE_CMD_WRITE_DMA, "Write DMA w/retry" },
        { EIDE_CMD_WRITE_DMA_R, "Write DMA wo/retry" },
        { EIDE_CMD_WRITE_DMA_EXT, "Write DMA Ext" },
        { EIDE_CMD_WRITE_DMA_QUEUED, "Write DMA Queued" },
        { EIDE_CMD_WRITE_DMA_QUEUED_EXT, "Write DMA Queued Ext" },
        { EIDE_CMD_WRITE_LONG, "Write Long w/retry" },
        { EIDE_CMD_WRITE_LONG_R, "Write Long wo/retry" },
        { EIDE_CMD_WRITE_MULTIPLE, "Write Multiple" },
        { EIDE_CMD_WRITE_MULTIPLE_EXT, "Write Multiple Ext" },
        { EIDE_CMD_SAME, "Same" },
        { EIDE_CMD_WRITE_SECTORS, "Write Sectors w/retry" },
        { EIDE_CMD_WRITE_SECTORS_R, "Write Sectors wo/retry" },
        { EIDE_CMD_WRITE_SECTORS_EXT, "Write Sectors Ext" },
        { EIDE_CMD_WRITE_VERIFY, "Write Verify" },
        { EIDE_CMD_FLUSH_CACHE, "Flush Cache" },
        { EIDE_CMD_FLUSH_CACHE_EXT, "Flush Cache Ext" },
        { EIDE_CMD_READ_NATIVE_MAX_ADDR, "Read Native Max Addr" },
        { EIDE_CMD_READ_NATIVE_MAX_ADDR_EXT, "Read Native Max Addr Ext" },
        { EIDE_CMD_REQUEST_SENSE, "Request Sense" },
        { EIDE_CMD_GET_MEDIA_STATUS, "Get Media Status" },
        { EIDE_CMD_DATA_SET_MGNT, "Data Set Mgnt" },
        { 0, NULL }
    };

    EIDE_COMMAND    *c;

    c = eide_cmd_desc;
    for (; c->desc; c++) {
        if (c->cmd == cmd) {
            return (c->desc);
        }
    }
    return ("UNKNOWN");
}

/******************************************************************************/
static int sata_log_regs(EIDE_TARGET *tgt)
{
    sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, 5,
        "%s: port=%d, sstatus=0x%08x, serror=0x%08x, scontrol=0x%08x, "
        "sactive=0x%08x",
        __FUNCTION__, tgt->port, in32(tgt->regs.sstatus),
        in32(tgt->regs.serror), in32(tgt->regs.scontrol),
        in32(tgt->regs.sactive));
    return (CAM_SUCCESS);
}

#else

    #define sata_log_regs(_a)
    #define sata_cmd_description(_a)

#endif      /* EIDE_SATA_DEBUG */

/*****************************************************************************/

static int sata_drv_init(EIDE_TARGET *tgt)
{
    SIM_HBA         *hba = tgt->hba;
    SIM_EIDE_EXT    *ext = (SIM_EIDE_EXT *)hba->ext;
    uintptr_t       bar = (uintptr_t)ext->bar;
    rcar_sata_drv   *sata_drv = &tgt->sata_drv;

    EXEC_TRACE;

#if defined(SATA_DATA_DUMP)
    pthread_mutex_init(&sata_drv->mutex, NULL);
#endif

    tgt->prd = (rcar_sata_prd *)eide_alloc(sizeof(rcar_sata_prd) * COMMAND_PRD_MAX_NUM);
    out32(bar + ATAPI_DTB_ADR_REG, cam_mphys(tgt->prd));

    sata_drv->prd_align = RCAR_SATA_4B_ALIGN;

    return (CAM_SUCCESS);
}

/*****************************************************************************/
/* Hardware reset, like Power-on and COMRESET */
static void sata_hardware_reset(EIDE_TARGET *tgt)
{
    SIM_HBA         *hba = tgt->hba;
    SIM_EIDE_EXT    *ext = (SIM_EIDE_EXT *)hba->ext;
    uintptr_t       bar = (uintptr_t)ext->bar;
    uint32_t        scrscon;

    EXEC_TRACE;

    /* Disable the SATA interface and put PHY off-line */
    scrscon = in32(bar + SCRSCON_REG);
    scrscon = (scrscon & 0xfffffff0) | 0x4;
    out32(bar + SCRSCON_REG, scrscon);

    /* Issue PHY wake/reset, Hardware_reset_asserted */
    scrscon = in32(bar + SCRSCON_REG);
    scrscon = (scrscon & 0xfffffff0) | 0x1;
    out32(bar + SCRSCON_REG, scrscon);

    delay(100);     /* Sleep 100ms */

    /* Resume PHY, COMRESET negated, the device initialize hardware
     * and execute diagnostics, send good status-signature to host,
     * which is D2H register FIS, and then the device enter idle state.
     */
    scrscon = in32(bar + SCRSCON_REG);
    scrscon = (scrscon & 0xfffffff0);
    out32(bar + SCRSCON_REG, scrscon);

    delay(100);     /* Sleep 100ms */
}

/*****************************************************************************/
static uint32_t sata_wait_idle(EIDE_TARGET *tgt)
{
    SIM_HBA             *hba = tgt->hba;
    SIM_EIDE_EXT        *ext = (SIM_EIDE_EXT *)hba->ext;
    uintptr_t           bar = (uintptr_t)ext->bar;
    rcar_sata_tf_regs   *tfregs = (rcar_sata_tf_regs *)(bar + SDATA_REG);
    uint32_t            status, bits = (EIDE_STATUS_BSY | EIDE_STATUS_DRQ);
    uint32_t            max = 100000;

    status = tfregs->ssstatus_sscom;

    while (status != 0xff && (status & bits) && (max-- > 0))
    {
        nanospin_ns(100);
        status = tfregs->ssstatus_sscom;
    }

    return status;
}

static void sata_exec_command(EIDE_TARGET *tgt,
                   ATA_TASKFILE *tf);
/******************************************************************************/
void sata_tf_load(EIDE_TARGET *tgt, ATA_TASKFILE *tf)
{
    SIM_HBA             *hba = tgt->hba;
    SIM_EIDE_EXT        *ext = (SIM_EIDE_EXT *)hba->ext;
    uintptr_t           bar = (uintptr_t)ext->bar;
    rcar_sata_tf_regs   *tfregs = (rcar_sata_tf_regs *)(bar + SDATA_REG);

    if (tf->control != tgt->ctl)
    {
        tfregs->ssaltsts_ssdevcon = tf->control;
        tgt->ctl = tf->control;
        sata_wait_idle(tgt);
    }

    if ((tgt->tflags & EIDE_TFLG_USE_LBA)
            && (tf->tflags & (ATA_TF_EXP | ATA_TF_ATA)))
    {
        tfregs->sserr_ssfeatures = tf->hob_feature;
        tfregs->sseccnt          = tf->hob_seccnt;
        tfregs->slbalow          = tf->hob_lbal;
        tfregs->slbamid          = tf->hob_lbam;
        tfregs->slbahigh         = tf->hob_lbah;
    }
    tfregs->sserr_ssfeatures = tf->feature;
    tfregs->sseccnt          = tf->seccnt;
    tfregs->slbalow          = tf->lbal;
    tfregs->slbamid          = tf->lbam;
    tfregs->slbahigh         = tf->lbah;
    tfregs->sdevhead         = tf->device;

    sata_wait_idle(tgt);
}

/******************************************************************************/
static void sata_enable_intr(EIDE_TARGET *tgt)
{
    SIM_HBA         *hba = tgt->hba;
    SIM_EIDE_EXT    *ext = (SIM_EIDE_EXT *)hba->ext;
    uint32_t        bar = (uint32_t)ext->bar;

    EXEC_TRACE;

    /* ack and mask */
    out32(bar + SATAINTSTAT_REG, 0);
    out32(bar + SATAINTMASK_REG, 0);//0x7ff);
    /* enable interrupts */
    out32(bar + ATAPI_INT_ENABLE_REG, ATAPI_INT_ENABLE_SATAINT);
}

/******************************************************************************/
static void sata_disable_intr(EIDE_TARGET *tgt)
{
    SIM_HBA         *hba = tgt->hba;
    SIM_EIDE_EXT    *ext = (SIM_EIDE_EXT *)hba->ext;;
    uint32_t        bar = (uint32_t)ext->bar;

    EXEC_TRACE;

    /* disable interrupts */
    out32(bar + ATAPI_INT_ENABLE_REG, 0);
    /* ack and mask */
    out32(bar + SATAINTSTAT_REG, 0);
    out32(bar + SATAINTMASK_REG, 0x7ff);
}

/******************************************************************************/
static int sata_disable_port(EIDE_TARGET *tgt)
{
    SIM_HBA         *hba = tgt->hba;
    SIM_EIDE_EXT    *ext = (SIM_EIDE_EXT *)hba->ext;
    uint32_t        bar = (uint32_t)ext->bar;
    uint32_t        val32;

    EXEC_TRACE;

    /* Bring the SATA controller off-line and place the PHY in reset */
    val32 = in32(bar + ATAPI_CONTROL1_REG);
    val32 |= ATAPI_CONTROL1_RESET;

    out32(bar + ATAPI_CONTROL1_REG, val32);

    delay(10);

    return (CAM_SUCCESS);
}

#if 0
/******************************************************************************/
static int sata_enable_port(EIDE_TARGET *tgt)
{
    SIM_HBA         *hba = tgt->hba;
    SIM_EIDE_EXT    *ext = (SIM_EIDE_EXT *)hba->ext;
    uint32_t        bar = (uint32_t)ext->bar;
    uint32_t        val32;

    EXEC_TRACE;

    /* Bring the SATA controller off-line and place the PHY in reset */
    val32 = in32(bar + ATAPI_CONTROL1_REG);
    val32 &= ~ATAPI_CONTROL1_RESET;
    out32(bar + ATAPI_CONTROL1_REG, val32);

    delay(10);

    return (CAM_SUCCESS);
}
#endif

#if defined(SATA_DATA_DUMP)
/******************************************************************************/
static void sata_data_dump(EIDE_TARGET *tgt, void *dptr, uint32_t len, uint8_t log)
{
    int             i, k;
    uint8_t         *pb;

    EXEC_TRACE;
    if (!len || dptr == NULL) {
        return;
    }

    _mutex_lock(&tgt->sata_drv.mutex);

    if (log) {
        char    log_line[3 * 32 + 1];
        char    bchar[4];

        sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, 5,
                "%s - Dump of SG data of %d bytes", __FUNCTION__, len);

        memset(log_line, 0, 3 * 32 + 1);
        for (pb = dptr, i = 0, k = 0; i < len; i++, pb++) {
            sprintf(bchar, "%02x ", *pb);
            strcat(log_line, bchar);
            if (!((k + 1) % 32)) {
                sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, 5,
                        "%s - %s", __FUNCTION__, log_line);
                memset(log_line, 0, 3 * 32 + 1);
                k = 0;
            } else {
                k++;
            }
        }
        if (k) {
            sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, 5,
                "%s - %s", __FUNCTION__, log_line);
        }
    } else {
        printf("\nDump of SG data of %d bytes\n", len);

        for (pb = dptr, i = 0, k = 0; i < len; i++, pb++) {
            printf("%02x ", *pb);
            if (!((k + 1) % 32)) {
                printf("\n\t");
                k = 0;
            } else {
                k++;
            }
        }
        printf("\n");
    }
    _mutex_unlock(&tgt->sata_drv.mutex);
}

/******************************************************************************/
static void sata_data_dump_all(EIDE_TARGET *tgt)
{
    int             i, j, k;
    void            *dptr;
    uint8_t         *pb;
    char            log_line[3 * 32 + 1];
    char            bchar[4];
    uint8_t         log_level = 5;//0;//5;

    EXEC_TRACE;
    if (!tgt->sata_drv.cmd_sg.all_count) {
        return;
    }

    _mutex_lock(&tgt->sata_drv.mutex);

    for (i = 0; i < tgt->sata_drv.cmd_sg.all_count; i++) {
        sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, log_level,
                "%s - Dump of SG-%d data of %d bytes", __FUNCTION__, i,
                tgt->sata_drv.cmd_sg.sga[i].isge.cam_sg_count);

        dptr = mmap(NULL,
            tgt->sata_drv.cmd_sg.sga[i].isge.cam_sg_count,
            PROT_READ | PROT_WRITE | PROT_NOCACHE, MAP_PHYS | MAP_SHARED, NOFD,
            tgt->sata_drv.cmd_sg.sga[i].isge.cam_sg_address);
        if (dptr == NULL) {
            continue;
        }

        memset(log_line, 0, 3 * 32 + 1);
        for (pb = dptr, j = 0, k = 0; j < tgt->sata_drv.
                        cmd_sg.sga[i].isge.cam_sg_count; j++, pb++) {
            sprintf(bchar, "%02x ", *pb);
            strcat(log_line, bchar);
            if (!((k + 1) % 32)) {
                sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, log_level,
                        "%s - %s", __FUNCTION__, log_line);
                memset(log_line, 0, 3 * 32 + 1);
                k = 0;
            } else {
                k++;
            }
        }
        if (k) {
            sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, log_level,
                "%s - %s", __FUNCTION__, log_line);
        }
        munmap(dptr, tgt->sata_drv.cmd_sg.sga[i].isge.cam_sg_count);
    }

    tgt->sata_drv.cmd_sg.all_count = 0;

    _mutex_unlock(&tgt->sata_drv.mutex);
}

#else

    #define sata_data_dump(_a, _b, _c, _d)
    #define sata_data_dump_all(_a)
    #define sata_dump_fields(_a, _b, _c, _d, _e)

#endif  /* SATA_DATA_DUMP */

#if defined(SATA_ALIGN_ADDR_AND_LEN)
/******************************************************************************/
static int sata_free_sge(EIDE_TARGET *tgt)
{
    uint32_t    si, cnt = 0, free = 0;

    EXEC_TRACE;

    if (tgt->sata_drv.cmd_sg.chg_count == 0) {
        sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, 5,
                "%s - No SG to free", __FUNCTION__);
        return (CAM_SUCCESS);
    }

    for (si = 0 ; si < tgt->sata_drv.cmd_sg.chg_count; si++) {
        if (tgt->sata_drv.cmd_sg.sga[si].chg == SG_CHANGED_NONE) {
            continue;
        }
        tgt->sata_drv.cmd_sg.sga[si].chg = SG_CHANGED_NONE;

        free = 0;
        if (tgt->sata_drv.cmd_sg.sga[si].cptr != NULL) {
            if (eide_dma_mem_free(tgt->sata_drv.cmd_sg.sga[si].cptr,
                    tgt->sata_drv.cmd_sg.sga[si].alloc_size) != CAM_SUCCESS) {

                sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_CRITICAL, 1,
                    "%s - Cannot unmap copied SG-%d memory", __FUNCTION__, si);
            }
            tgt->sata_drv.cmd_sg.sga[si].cptr = NULL;
            free++;
        }

        if (tgt->sata_drv.cmd_sg.sga[si].iptr != NULL) {
            if (munmap(tgt->sata_drv.cmd_sg.sga[si].iptr,
                tgt->sata_drv.cmd_sg.sga[si].isge.cam_sg_count) != CAM_SUCCESS) {

                sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_CRITICAL, 1,
                    "%s - Cannot unmap original SG-%d memory", __FUNCTION__, si);
            }
            tgt->sata_drv.cmd_sg.sga[si].iptr = NULL;
            free++;
        }
        if (free) {
            cnt++;
        }
    }

    sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, 5,
            "%s - %d SGs have been freed", __FUNCTION__, cnt);

    return (CAM_SUCCESS);
}

/******************************************************************************/
static int sata_copy_sge(EIDE_TARGET *tgt)
{
    uint32_t    si, cnt = 0;

    EXEC_TRACE;

    if (tgt->sata_drv.cmd_sg.chg_count == 0) {
        sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, 5,
                "%s - No SG to copy", __FUNCTION__);
        return (CAM_SUCCESS);
    }

    for (si = 0 ; si < tgt->sata_drv.cmd_sg.chg_count; si++) {
        if (tgt->sata_drv.cmd_sg.sga[si].chg == SG_CHANGED_NONE) {
            continue;
        }
        if (tgt->sata_drv.cmd_sg.is_read_cmd &&
                            tgt->sata_drv.cmd_sg.sga[si].iptr &&
                            tgt->sata_drv.cmd_sg.sga[si].cptr) {
            /* Read command - Copy read data */
            memcpy(tgt->sata_drv.cmd_sg.sga[si].iptr,
                        tgt->sata_drv.cmd_sg.sga[si].cptr,
                        tgt->sata_drv.cmd_sg.sga[si].isge.cam_sg_count);

            sata_data_dump(tgt, tgt->sata_drv.cmd_sg.sga[si].iptr,
                    tgt->sata_drv.cmd_sg.sga[si].isge.cam_sg_count, 1);

            cnt++;
        }
    }

    sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, 5,
            "%s - %d SGs have been copied", __FUNCTION__, cnt);

    sata_free_sge(tgt);

    return (CAM_SUCCESS);
}

/******************************************************************************/
static SG_ELEM *sata_check_sge(EIDE_TARGET *tgt, SG_ELEM *sg_first, int nsg,
                                            ATA_TASKFILE *tf, uint32_t sg_align)
{
    SG_ELEM         *sg = sg_first;
    paddr_t         sg_addr;
    uint32_t        sg_len, si, sg_chg_len;

    EXEC_TRACE;

    tgt->sata_drv.cmd_sg.is_read_cmd = tf->tflags & ATA_TF_DIR_IN;
    tgt->sata_drv.cmd_sg.chg_count = 0;
    tgt->sata_drv.cmd_sg.all_count = 0;

    /* Change SG if needed */
    for (si = 0 ; si < nsg; si++, sg++) {
        sg_addr = sg->cam_sg_address;

        sg_len = (uint32_t)sg->cam_sg_count;

        tgt->sata_drv.cmd_sg.sga[si].chg = SG_CHANGED_NONE;

        tgt->sata_drv.cmd_sg.sga[si].isge.cam_sg_address = sg_addr;
        tgt->sata_drv.cmd_sg.sga[si].isge.cam_sg_count = sg_len;
        tgt->sata_drv.cmd_sg.all_count++;

        tgt->sata_drv.cmd_sg.sga[si].iptr = NULL;
        tgt->sata_drv.cmd_sg.sga[si].cptr = NULL;

        if (sg_addr & sg_align) {
            tgt->sata_drv.cmd_sg.sga[si].chg |= SG_CHANGED_ADDR;

            sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, 5,
                "%s - SG address = %08x - Unaligned at %d bytes",
                 __FUNCTION__, (uint32_t)sg_addr, sg_align + 1);
        }

        if (sg_len & sg_align) {
            tgt->sata_drv.cmd_sg.sga[si].chg |= SG_CHANGED_LEN;
            sg_chg_len = sg_len + (sg_align + 1) - (sg_len & sg_align);

            sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, 5,
                "%s - SG length = %d - Unaligned at %d bytes",
                 __FUNCTION__, (uint32_t)sg_len, sg_align + 1);
        } else {
            sg_chg_len = sg_len;
        }

        if (tgt->sata_drv.cmd_sg.sga[si].chg != SG_CHANGED_NONE) {
            tgt->sata_drv.cmd_sg.chg_count++;
            if (tgt->sata_drv.cmd_sg.sga[si].chg & SG_CHANGED_LEN){
                tgt->sata_drv.cmd_sg.sga[si].cptr = eide_dma_mem_alloc(
                        sg_chg_len, &tgt->sata_drv.cmd_sg.sga[si].alloc_size);
            } else {
                tgt->sata_drv.cmd_sg.sga[si].cptr = eide_dma_mem_alloc(
                        sg_len, &tgt->sata_drv.cmd_sg.sga[si].alloc_size);
            }
            if (tgt->sata_drv.cmd_sg.sga[si].cptr == NULL) {
                sata_free_sge(tgt);
                return (sg_first);
            }

            tgt->sata_drv.cmd_sg.sga[si].iptr = mmap(NULL, sg_len,
                    PROT_READ | PROT_WRITE | PROT_NOCACHE, MAP_PHYS |
                    MAP_SHARED, NOFD, sg_addr);
            if (tgt->sata_drv.cmd_sg.sga[si].iptr == NULL) {
                sata_free_sge(tgt);
                return (sg_first);
            }

            if (!tgt->sata_drv.cmd_sg.is_read_cmd) {
                /* Write command - Copy data to be written */
                memcpy(tgt->sata_drv.cmd_sg.sga[si].cptr,
                            tgt->sata_drv.cmd_sg.sga[si].iptr, sg_len);
                munmap(tgt->sata_drv.cmd_sg.sga[si].iptr, sg_len);
                /* Pointer no more needed */
                tgt->sata_drv.cmd_sg.sga[si].iptr = NULL;
            }

            sg_len = sg_chg_len;
            tgt->sata_drv.cmd_sg.sga[si].csge.cam_sg_count = sg_len;
            tgt->sata_drv.cmd_sg.sga[si].csge.cam_sg_address =
                                cam_mphys(tgt->sata_drv.cmd_sg.sga[si].cptr);

            sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, 5,
                    "%s - Changed SG : sg_addr = %08x, sg_len = %d",
                    __FUNCTION__, (uint32_t)sg_addr, (uint32_t)sg_len);

        } else {
            tgt->sata_drv.cmd_sg.sga[si].csge.cam_sg_count =
                    tgt->sata_drv.cmd_sg.sga[si].isge.cam_sg_count;
            tgt->sata_drv.cmd_sg.sga[si].csge.cam_sg_address =
                    tgt->sata_drv.cmd_sg.sga[si].isge.cam_sg_address;
        }
    }

    if (tgt->sata_drv.cmd_sg.chg_count){
        sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, 5,
                "%s - %d of %d SGs have been changed",
                __FUNCTION__, tgt->sata_drv.cmd_sg.chg_count,
                tgt->sata_drv.cmd_sg.all_count);
        return &tgt->sata_drv.cmd_sg.sga[0].csge;
    } else {
        sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, 5,
                "%s - No SG changed", __FUNCTION__);
        return sg_first;
    }
}

#else

    #define sata_free_sge(_a)
    #define sata_copy_sge(_a)
    #define sata_check_sge(_a, _b, _c, _d, _e)  (_b)

#endif  /* SATA_ALIGN_ADDR_AND_LEN */

/******************************************************************************/
static int sata_setup_prd(EIDE_TARGET *tgt,
                SG_ELEM *sg_first, int nsg, ATA_TASKFILE *tf)
{
    SG_ELEM     *sg = sg_first;
    int         num_prde = 0;

    /* NOTE : direct & indirect prdt's are contiguously allocated */
    rcar_sata_prd   *prd = tgt->prd;
    paddr_t         sg_addr;
    uint32_t        sg_len, sg_align, si;

    EXEC_TRACE;

    sg_align = tgt->sata_drv.prd_align;

    sg = sata_check_sge(tgt, sg_first, nsg, tf, sg_align);
    if (sg == NULL) {
        return 0;
    }

    /* Setup the PRD table */
    for (si = 0 ; si < nsg && num_prde < COMMAND_PRD_MAX_NUM + 1; si++, sg++) {
        sg_addr = sg->cam_sg_address;
        sg_len = (uint32_t)sg->cam_sg_count;

        /* Warn if each s/g element is not double word aligned */
        if (sg_addr & sg_align) {
            sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                "SG address = %08x - Unaligned at %d bytes",
                (uint32_t)sg_addr, sg_align + 1);
            return 0;
        }
        if (sg_len & sg_align) {
            sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                "SG length = %d - Unaligned at %d bytes",
                (uint32_t)sg_len, sg_align + 1);
            return 0;
        }

        sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, 5,
                "sg_addr = %08x, sg_len = %d",
                (uint32_t)sg_addr, (uint32_t)sg_len);

        prd[si].ddsta = (sg_addr & ~sg_align) ;

        prd[si].ddtrc = (sg_len & ~sg_align);

        ++num_prde;
    }
    prd[si-1].ddsta |= SATA_RCAR_DTEND;

    return (num_prde);
}

/*****************************************************************************/
static void sata_dev_select(EIDE_TARGET *tgt)
{
    SIM_HBA             *hba = tgt->hba;
    SIM_EIDE_EXT        *ext = (SIM_EIDE_EXT *)hba->ext;
    uintptr_t           bar = (uintptr_t)ext->bar;
    rcar_sata_tf_regs   *tfregs = (rcar_sata_tf_regs *)(bar + SDATA_REG);

    tfregs->sdevhead = ATA_DEVICE_OBS;
}

/*****************************************************************************/
static unsigned int sata_dev_check(EIDE_TARGET *tgt)
{
    SIM_HBA             *hba = tgt->hba;
    SIM_EIDE_EXT        *ext = (SIM_EIDE_EXT *)hba->ext;
    uintptr_t           bar = (uintptr_t)ext->bar;
    rcar_sata_tf_regs   *tfregs = (rcar_sata_tf_regs *)(bar + SDATA_REG);
    uint8_t             nsect, lbal;

    sata_dev_select(tgt);

    tfregs->sseccnt = 0x55;
    tfregs->slbalow = 0xaa;

    tfregs->sseccnt = 0xaa;
    tfregs->slbalow = 0x55;

    tfregs->sseccnt = 0x55;
    tfregs->slbalow = 0xaa;

    nsect = tfregs->sseccnt;
    lbal  = tfregs->slbalow;

    if (nsect == 0x55 && lbal == 0xaa)
    {
        return 1;   /* found a device */
    }

    return 0;       /* nothing found */
}


/******************************************************************************/
static int sata_softreset(EIDE_TARGET *tgt)
{
    SIM_HBA         *hba = tgt->hba;
    SIM_EIDE_EXT    *ext = (SIM_EIDE_EXT *)hba->ext;
    uint32_t        bar = (uint32_t)ext->bar;
    uint32_t        val32;

    EXEC_TRACE;

    sata_dev_check(tgt);

    /* software reset.  causes dev0 to be selected */
    val32 = 0;
    out32(bar + SSDEVCON_REG, val32);
    usleep(20);
    out32(bar + SSDEVCON_REG,  val32 | ATA_SRST);
    usleep(20);
    out32(bar + SSDEVCON_REG,  val32 & ~ATA_SRST);
    delay(100);

    return (CAM_SUCCESS);
}

/******************************************************************************/
static int sata_phy_wait(EIDE_TARGET *tgt, int tms)
{
    SIM_HBA         *hba = tgt->hba;
    SIM_EIDE_EXT    *ext = (SIM_EIDE_EXT *)hba->ext;
    uint32_t        bar = (uint32_t)ext->bar;
    uint32_t        sstatus, serror;

    EXEC_TRACE;
    while (tms--) {
        /* Wait for PHY communication established */
        sstatus = in32(bar + SCRSSTS_REG);
        /*
         * Detection. Indicates the interface device detection and PHY state.
         * All other values reserved
         *  0000 No device detected and PHY communication not established
         *       (SSTS_DET_NO_DEVICE)
         *  0001 Device presence detected but PHY communication not
         *       established
         *       (SSTS_DET_DEV_PRESENT)
         *  0011 Device presence detected and PHY communication established
         *       (SSTS_DET_PHY_ONLINE)
         *  0100 PHY in off-line mode as a result of the interface being
         *       disabled or running in a BIST loop-back mode
         *       (SSTS_DET_PHY_OFFLINE)
         */
        if ((sstatus & SSTS_DET_MSK) == SSTS_DET_PHY_ONLINE) {
            serror = in32(bar + SCRSERR_REG);
            out32(bar + SCRSERR_REG, serror);

            return (CAM_SUCCESS);
        }
        nanospin_ns(1000000L);
    }

    sata_log_regs(tgt);

    return (CAM_FAILURE);
}

/******************************************************************************/
static int sata_phy_reset(EIDE_TARGET *tgt)
{
    SIM_HBA         *hba = tgt->hba;
    SIM_EIDE_EXT    *ext = (SIM_EIDE_EXT *)hba->ext;
    uint32_t        bar = (uint32_t)ext->bar;
    int             cretry;

    EXEC_TRACE;

    cretry = SATA_COMM_INIT_RETRIES;

    while (cretry--) {
        /* Issue COMRESET : Perform interface communication initialization
         * sequence to establish communication */
        out32(bar + SCRSCON_REG,
                SCON_IPM_DIS_PARTIAL_SLUMBER | SCON_SPD_ANY | SCON_DET_RESET);

        delay(SATA_COMM_INIT_DELAY);

        out32(bar + SCRSCON_REG,
                SCON_IPM_DIS_PARTIAL_SLUMBER | SCON_SPD_ANY);

        if (sata_phy_wait(tgt, SATA_COMM_INIT_PWAIT) == CAM_SUCCESS) {
            return (CAM_SUCCESS);
        }
    }

    sata_log_regs(tgt);

    return (CAM_FAILURE);
}

/******************************************************************************/
static int sata_establish_comm(EIDE_TARGET *tgt)
{
    SIM_HBA             *hba = tgt->hba;
    SIM_EIDE_EXT        *ext = (SIM_EIDE_EXT *)hba->ext;
    uintptr_t           bar = (uintptr_t)ext->bar;
    rcar_sata_tf_regs   *tfregs = (rcar_sata_tf_regs *)(bar + SDATA_REG);
    uint32_t            sstatus, signature = SATA_SIG_ATA, serror;
    int                 timeo;
    uint32_t            lbal, lbam, lbah;

    EXEC_TRACE;

    tgt->tflags &= ~(EIDE_TFLG_ATAPI | EIDE_TFLG_ATA);

    /* Perform interface communication initialization sequence to establish
     * communication (generates COMRESET toward the drive)
     */
    if (sata_phy_reset(tgt) != CAM_SUCCESS) {
        return (CAM_FAILURE);
    }

    if (sata_phy_wait(tgt, SATA_COMM_INIT_PWAIT)  == CAM_FAILURE) {
        sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
            "%s: sata_phy_wait - path=%d, port=%d, sstatus=0x%08x",
            __FUNCTION__, hba->pathid, tgt->port,
            in32(bar + SCRSSTS_REG));

        return (CAM_FAILURE);
    }

    /* Clear errors */
    serror = in32(bar + SCRSERR_REG);
    out32(bar + SCRSERR_REG, serror);

    for (timeo = EIDE_RESET_TIME * 20; timeo; timeo--) {
        /* Signature update. When set, this bit indicates that the signature
         * is updated in the task file registers.
         */
        lbal = in32((uintptr_t)&tfregs->slbalow);
        lbam = in32((uintptr_t)&tfregs->slbamid);
        lbah = in32((uintptr_t)&tfregs->slbahigh);

        if((lbal == 0x01) && (lbam == 0x14) && (lbah == 0xeb))
        {
            signature = SATA_SIG_ATAPI;
        }

        if (signature == SATA_SIG_ATA || signature == SATA_SIG_ATAPI) {

            tgt->tflags |= EIDE_TFLG_PRESENT;

            if (signature == SATA_SIG_ATAPI) {
                tgt->tflags |= EIDE_TFLG_ATAPI;

                sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                    "CD-ROM device detected\n");
            }
            else {
                tgt->tflags |= EIDE_TFLG_ATA;

                sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                    "Hard Disk device detected\n");
            }
            sstatus = in32(bar + SCRSSTS_REG);
            if ((sstatus & SSTS_SPD_MSK) == SSTS_SPD_GEN1) {
                sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                    "\tSpeed : 1.5 Gbps\n");
            } else {
                if ((sstatus & SSTS_SPD_MSK) == SSTS_SPD_GEN2) {
                    sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                        "\tSpeed : 3.0 Gbps\n");
                }
                else if ((sstatus & SSTS_SPD_MSK) == SSTS_SPD_GEN3) {
                    sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                        "\tSpeed : 6.0 Gbps\n");
                } else {
                    sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                        "\tUnknown communication speed\n");
                }
            }

            return (CAM_SUCCESS);
        }

        if (signature == SATA_SIG_SEMB) {
            sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                "Enclosure Management Bridge device detected - "
                    "Not supported !\n");

            sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                "Signature = %08x: Enclosure management bridge - "
                "NOT SUPPORTED", __FUNCTION__, signature);

            return (CAM_FAILURE);
        }

        if (signature == SATA_SIG_PM) {
            sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                "Port Multiplier device detected - "
                    "Not supported !\n");

            sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                "Signature = %08x: Port Multiplier - "
                "NOT SUPPORTED", __FUNCTION__, signature);

            return (CAM_FAILURE);
        }

        if ((timeo % EIDE_RESET_TIME) == 0) {
            sata_softreset(tgt);
            sata_hardware_reset(tgt);
        }

        delay(50);
    }

    sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
            "%s: Signature = %08x: Unknown device",
            __FUNCTION__, signature);

    sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
            "\nNo SATA device detected\n\n");

    sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
            "%s:  BSY timeout path=%d, port=%d, sig=0x%08x",
            __FUNCTION__, hba->pathid, tgt->port, signature);

    return (CAM_FAILURE);
}

#if (SATA_PNP_SUPPORT == 1)

#if 0
/******************************************************************************/
static void sata_flush_cache_ext(EIDE_TARGET *tgt)
{
    ATA_TASKFILE    tf;
    int32_t         retry;

    #define MAX_CMD_FLUSH_CACHE_EXT_RETRY   10

    EXEC_TRACE;

    for (retry = MAX_CMD_FLUSH_CACHE_EXT_RETRY; retry > 0; retry--) {
        /* Wait 100 mSec */
        delay(100);

        retry--;
        memset(&tf, 0, sizeof(tf));
        tf.tflags = ATA_TF_BUSY_WAIT;
        tf.command = EIDE_CMD_FLUSH_CACHE_EXT;

        /* Wait at most 1 second */
        if (sata_command(tgt, &tf, NULL, 0, NULL, 0, 1) == CAM_SUCCESS) {
            break;
        }
    }
}
#endif

/******************************************************************************/
static void *sata_device_connect(void *data)
{
    EIDE_TARGET     *tgt = (EIDE_TARGET *)data;
    SIM_HBA         *hba = tgt->hba;

    EXEC_TRACE;

    sata_disable_intr(tgt);

    if (sata_port_reset(tgt, 0) == CAM_SUCCESS) {

        if (eide_identify(tgt) == CAM_SUCCESS &&
                eide_workspace_init(tgt, EIDE_WS_SIZE) == CAM_SUCCESS) {

            tgt->tflags |= EIDE_TFLG_PRESENT;
            tgt->tflags &= ~EIDE_TFLG_ENUMERATING;

            eide_init_device(tgt, EIDE_INIT_NORESET);

            sata_enable_intr(tgt);

            xpt_async(AC_FOUND_DEVICES, hba->pathid, 0, 0, NULL, 0);

        } else {
            sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 2,
                                "%s:  identify failure", __FUNCTION__);
        }
    } else {
        sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 2,
                                    "%s:  comm failure", __FUNCTION__);
    }

    tgt->tflags &= ~EIDE_TFLG_ENUMERATING;

    sata_enable_intr(tgt);

    return (NULL);
}

/******************************************************************************/
static void *sata_device_disconnect(void *data)
{
    EIDE_TARGET     *tgt = (EIDE_TARGET *)data;
    SIM_HBA         *hba = tgt->hba;

    EXEC_TRACE;

    sata_disable_intr(tgt);

    xpt_async(AC_LOST_DEVICE, hba->pathid, tgt->port, 0, NULL, 0);

    memset(&tgt->geometry, 0, sizeof(struct _eide_geometry));

    sata_enable_intr(tgt);

    return (NULL);
}

/******************************************************************************/
static void sata_enumerate_port(EIDE_TARGET *tgt, void *(*efunc)(void *tgt))
{
    SIM_HBA             *hba;
    SIM_EIDE_EXT        *ext;
    int                 status;

    EXEC_TRACE;
    hba = tgt->hba;
    ext = (SIM_EIDE_EXT *)hba->ext;

    /* EIDE_GFLG_ENUMERATION is set/clear only in eide_identify_devices()
     * function
     */
    if (eide_ctrl.gflags & EIDE_GFLG_ENUMERATION) {
        sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 2,
                                "%s:  enumerating", __FUNCTION__);

        return;
    }

    if ((status = eide_create_thread(NULL, NULL, efunc, tgt,
            ext->priority, NULL, efunc == sata_device_connect ?
                    "sata_device_connect" : "sata_device_disconnect"))) {

        sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                "%s: pthread_create %s", __FUNCTION__, strerror(status));
    }
}

#else

/******************************************************************************/
static void *sata_device_destroy(void *data)
{
    EIDE_TARGET     *tgt = (EIDE_TARGET *)data;
    SIM_HBA         *hba = tgt->hba;
    pid_t           ppid;

    EXEC_TRACE;
    eide_sim_idetach(hba);

    printf("%s - SATA device disconnected !\n", __FUNCTION__);
    ppid = getpgid(0);
    kill(ppid, SIGKILL);

    return (NULL);
}

/******************************************************************************/
static void sata_destroy_port(EIDE_TARGET *tgt)
{
    SIM_HBA             *hba;
    SIM_EIDE_EXT        *ext;
    int                 status;

    EXEC_TRACE;
    hba = tgt->hba;
    ext = (SIM_EIDE_EXT *)hba->ext;

    if ((status = eide_create_thread(NULL, NULL, sata_device_destroy, tgt,
            ext->priority, NULL, "sata_device_destroy"))) {

        sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                "%s: pthread_create %s", __FUNCTION__, strerror(status));
    }
}

#endif      /* SATA_PNP_SUPPORT */

/******************************************************************************/
void sata_tf_read(EIDE_TARGET *tgt, ATA_TASKFILE *tf)
{
    SIM_HBA             *hba = tgt->hba;
    SIM_EIDE_EXT        *ext = (SIM_EIDE_EXT *)hba->ext;
    uintptr_t           bar = (uintptr_t)ext->bar;
    rcar_sata_tf_regs   *tfregs = (rcar_sata_tf_regs *)(bar + SDATA_REG);

    EXEC_TRACE;

    tf->command     = tfregs->ssstatus_sscom;
    tf->feature     = tfregs->sserr_ssfeatures;
    tf->status      = tfregs->ssstatus_sscom;
    tf->error       = tfregs->sserr_ssfeatures;
    tf->lbal        = tfregs->slbalow;
    tf->lbam        = tfregs->slbamid;
    tf->lbah        = tfregs->slbahigh;
    tf->seccnt      = tfregs->sseccnt;
    tf->device      = tfregs->sdevhead;

    if ((tgt->tflags & EIDE_TFLG_USE_LBA) && (tf->tflags & ATA_TF_EXP))
    {
        tfregs->ssaltsts_ssdevcon = tf->control | ATA_HOB;
        tf->hob_feature = tfregs->sserr_ssfeatures;
        tf->hob_lbal    = tfregs->slbalow;
        tf->hob_lbam    = tfregs->slbamid;
        tf->hob_lbah    = tfregs->slbahigh;
        tf->hob_seccnt  = tfregs->sseccnt;
        tfregs->ssaltsts_ssdevcon = tf->control;
        tgt->ctl = tf->control;
    }
}

/*****************************************************************************/
uint8_t get_tf_status_reg(EIDE_TARGET *tgt)
{
    uint32_t        val32;

    EXEC_TRACE;

    val32 = (in32((_Uintptrt)tgt->regs.status));

    return ((uint8_t)val32);
}

/*****************************************************************************/
uint8_t get_tf_error_reg(EIDE_TARGET *tgt)
{
    uint32_t val32;

    EXEC_TRACE;

    val32 = (in32((_Uintptrt)tgt->regs.error));

    return ((uint8_t)val32);
}

#if 0
/******************************************************************************/
static uint8_t sata_dma_status(EIDE_TARGET *tgt)
{
    SIM_HBA         *hba = tgt->hba;
    SIM_EIDE_EXT    *ext = (SIM_EIDE_EXT *)hba->ext;
    uint32_t        bar = (uint32_t)ext->bar;
    uint32_t        status;

    status = in32(bar + ATAPI_STATUS_REG);

    return status;
}
#endif

/******************************************************************************/
static int sata_end_command(EIDE_TARGET *tgt)
{
    SIM_HBA         *hba = tgt->hba;
    SIM_EIDE_EXT    *ext = (SIM_EIDE_EXT *)hba->ext;
    uint32_t        bar = (uint32_t)ext->bar;
    uint32_t        sataintstat, scrserr;
    int             ret = CAM_SUCCESS;

    EXEC_TRACE;

    /* Clear errors and interrupts */
    out32(bar + ATAPI_STATUS_REG, 0);

    scrserr = in32(bar + SCRSERR_REG);
    out32(bar + SCRSERR_REG, scrserr);

    sataintstat = tgt->sataintstat;

    if (scrserr & (SERR_DEV_XCHG | SERR_PHYRDY_CHG)) {
        sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
            "%s: FATAL ERROR : sataintstat=%08x scrserr=%08x",
            __FUNCTION__, sataintstat, scrserr);

        /* Reset controller and PHY */
        sata_softreset(tgt);
        sata_hardware_reset(tgt);

        /* Clear errors and interrupts */
        out32(bar + ATAPI_STATUS_REG, 0);

        ret = CAM_FAILURE;
    }

    return (ret);
}

/******************************************************************************/
int sata_interrupt(EIDE_TARGET *tgt)
{
    SIM_HBA         *hba = tgt->hba;
    SIM_EIDE_EXT    *ext = (SIM_EIDE_EXT *)hba->ext;
    uintptr_t       bar = (uintptr_t)ext->bar;
    int             stat = CAM_SUCCESS;
    uint32_t        sataintstat, sstatus, scrserr;
    uint8_t         log_level = 5;

    EXEC_TRACE;

    sataintstat = in32(bar + SATAINTSTAT_REG);

    /* Clear interrupts */
    out32(bar + SATAINTSTAT_REG, ~sataintstat & 0x7ff);

    sataintstat &= SATA_RCAR_INT_MASK;
    if (!sataintstat)
        goto done;

    sstatus = in32(bar + SCRSSTS_REG);

    tgt->sataintstat = sataintstat;

    scrserr = in32(bar + SCRSERR_REG);
    sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, log_level,
                "%s : sataintstat=%08x scrserr = %08x",
                __FUNCTION__, sataintstat, scrserr);

    stat = sata_end_command(tgt);

    if (sataintstat & SATAINTSTAT_ATA) {
        sata_data_pio(tgt, ATA_TF_DIR_IN);

        tgt->status = get_tf_status_reg(tgt);

        tgt->error = (tgt->status & EIDE_STATUS_ERR) ?
                                    get_tf_error_reg(tgt) : 0;
        if (tgt->nfcnp && tgt->nexus) {
            sata_copy_sge(tgt);
            tgt->nfcnp(tgt,
                        (stat == CAM_SUCCESS) ? 0 : CAM_REQ_CMP_ERR);
        }
    }


#if (SATA_PNP_SUPPORT == 1)
    if (sataintstat & SATAINTSTAT_ATA) {
        if ((sstatus & SSTS_IPM_MSK) == SSTS_IPM_NO_DEVICE &&
                            (tgt->tflags & EIDE_TFLG_PRESENT)) {
            /* Device disconnected */
            sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, 5,
                    "%s - Device disconnected !\n", __FUNCTION__);

            tgt->tflags &= ~EIDE_TFLG_PRESENT;
            if (!( tgt->tflags & EIDE_TFLG_ENUMERATING)) {
                sata_enumerate_port(tgt, sata_device_disconnect);
            }
        } else if ((sstatus & SSTS_IPM_MSK) != SSTS_IPM_NO_DEVICE &&
                                !(tgt->tflags & EIDE_TFLG_PRESENT)) {
            /* Device connected */
            sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, 5,
                    "%s - Device connected !\n", __FUNCTION__);

            if (!( tgt->tflags & EIDE_TFLG_ENUMERATING)) {
                tgt->tflags |= EIDE_TFLG_ENUMERATING;
                sata_enumerate_port(tgt, sata_device_connect);
            }
        }
    }
#else
    if (sataintstat & SATAINTSTAT_ATA) {
        if ((sstatus & SSTS_IPM_MSK) == SSTS_IPM_NO_DEVICE &&
                            (tgt->tflags & EIDE_TFLG_PRESENT)) {
            /* Device disconnected - Kill the driver */
            sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 2,
                    "%s - Device disconnected !\n", __FUNCTION__);

            tgt->tflags &= ~EIDE_TFLG_PRESENT;
            sata_destroy_port(tgt);
        }
    }
#endif  /* SATA_PNP_SUPPORT */

done:
    return (CAM_SUCCESS);
}

unsigned int sata_data_pio_rw(EIDE_TARGET *tgt, void *buffer, int bufferlen, uint32_t dir)
{
    SIM_HBA             *hba = tgt->hba;
    SIM_EIDE_EXT        *ext = (SIM_EIDE_EXT *)hba->ext;
    uintptr_t           bar = (uintptr_t)ext->bar;
    rcar_sata_tf_regs   *tfregs = (rcar_sata_tf_regs *)(bar + SDATA_REG);
    uint16_t            *ptr = buffer;
    int                 words = bufferlen >> 1, count;

    count = words;

    /* Transfer multiple of 2 bytes */
    if (dir == ATA_TF_DIR_IN) {
        while (count--) {
            uint32_t data = tfregs->sdata;

            *ptr++ = (uint16_t)data;
        }
    } else {
        while (count--) {
            tfregs->sdata = *ptr++;
        }
    }

    /* Transfer trailing byte, if any. */
    if (bufferlen & 0x01) {
        /*
         * Use io*16_rep() accessors here as well to avoid pointlessly
         * swapping bytes to and from on the big endian machines...
         */
        if (dir == ATA_TF_DIR_IN) {
            *ptr = (uint16_t)tfregs->sdata;
        } else {
            tfregs->sdata = *ptr;
        }
        words++;
    }

    return (words << 1);
}


/******************************************************************************/
static void sata_exec_command(EIDE_TARGET *tgt,
                   ATA_TASKFILE *tf)
{
    SIM_HBA             *hba = tgt->hba;
    SIM_EIDE_EXT        *ext = (SIM_EIDE_EXT *)hba->ext;
    uintptr_t           bar = (uintptr_t)ext->bar;
    rcar_sata_tf_regs   *tfregs = (rcar_sata_tf_regs *)(bar + SDATA_REG);

    tfregs->ssstatus_sscom = tf->command;

    /* read dummy */
    tfregs->ssaltsts_ssdevcon;
}

/******************************************************************************/
static void sata_dma_setup(EIDE_TARGET *tgt,
                        ATA_TASKFILE *tf)
{
    SIM_HBA         *hba = tgt->hba;
    SIM_EIDE_EXT    *ext = (SIM_EIDE_EXT *)hba->ext;
    uintptr_t       bar = (uintptr_t)ext->bar;
    uint32_t        dmactl;

    /* load PRD table addr. */
    msync(tgt->prd, sizeof(tgt->prd), MS_SYNC);

    /* specify data direction, triple-check start bit is clear */
    dmactl = in32(bar + ATAPI_CONTROL1_REG);
    dmactl &= ~(ATAPI_CONTROL1_RW | ATAPI_CONTROL1_STOP);
    if (dmactl & ATAPI_CONTROL1_START) {
        dmactl &= ~ATAPI_CONTROL1_START;
        dmactl |= ATAPI_CONTROL1_STOP;
    }
    if (tf->tflags & ATA_TF_DIR_IN)
        dmactl |= ATAPI_CONTROL1_RW;
    out32(bar + ATAPI_CONTROL1_REG, dmactl);
}

/******************************************************************************/
static void sata_dma_start(EIDE_TARGET *tgt)
{
    SIM_HBA         *hba = tgt->hba;
    SIM_EIDE_EXT    *ext = (SIM_EIDE_EXT *)hba->ext;
    uint32_t        bar = (uint32_t)ext->bar;
    uint32_t        dmactl;

    /* start host DMA transaction */
    dmactl = in32(bar + ATAPI_CONTROL1_REG);
    dmactl &= ~ATAPI_CONTROL1_STOP;
    dmactl |= ATAPI_CONTROL1_START;
    out32(bar + ATAPI_CONTROL1_REG, dmactl);
}

/******************************************************************************/
static void sata_dma_stop(EIDE_TARGET *tgt)
{
    SIM_HBA             *hba = tgt->hba;
    SIM_EIDE_EXT        *ext = (SIM_EIDE_EXT *)hba->ext;
    uintptr_t           bar = (uintptr_t)ext->bar;
    rcar_sata_tf_regs   *tfregs = (rcar_sata_tf_regs *)(bar + SDATA_REG);
    uint32_t            dmactl;

    /* force termination of DMA transfer if active */
    dmactl = in32(bar + ATAPI_CONTROL1_REG);
    if (dmactl & ATAPI_CONTROL1_START) {
        dmactl &= ~ATAPI_CONTROL1_START;
        dmactl |= ATAPI_CONTROL1_STOP;
        out32(bar + ATAPI_CONTROL1_REG, dmactl);
    }

    /* one-PIO-cycle guaranteed wait, per spec, for HDMA1:0 transition */
    dmactl = tfregs->ssaltsts_ssdevcon;
}


/*****************************************************************************/
static int sata_wait_register(uint32_t addr, uint32_t mask,
                                            uint32_t val, uint32_t msec_to)
{
    uint32_t    i, temp;

    EXEC_TRACE;

    for (i = 0; (((temp = in32(addr)) & mask) != val) && i < msec_to; i++)
        delay(1);
    return (i < msec_to) ? 0 : -1;
}

/*****************************************************************************/
static int sata_wait_interrupt(EIDE_TARGET *tgt,
                                uint32_t val, uint32_t msec_to)
{
    uint32_t    i;

    EXEC_TRACE;

    for (i = 0; (!(tgt->sataintstat & val)) && ( i < msec_to); i++)
    {
        delay(1);
    }
    return (i < msec_to) ? 0 : -1;
}

/*****************************************************************************/
void sata_data_pio(EIDE_TARGET *tgt, uint32_t dir) {
    sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, 5, "%s cmd_pio=%d num_prde=0x%x",
        __FUNCTION__, tgt->sata_drv.cmd_pio, tgt->sata_drv.num_prde);
    if(tgt->sata_drv.cmd_pio && tgt->sata_drv.num_prde) {
        SIM_HBA             *hba = tgt->hba;
        SIM_EIDE_EXT        *ext = (SIM_EIDE_EXT *)hba->ext;
        uintptr_t           bar = (uintptr_t)ext->bar;
        rcar_sata_tf_regs   *tfregs = (rcar_sata_tf_regs *)(bar + SDATA_REG);

        if (!sata_wait_register((uintptr_t)& tfregs->ssaltsts_ssdevcon,
                EIDE_STATUS_BSY | EIDE_STATUS_DRQ, EIDE_STATUS_DRQ, (1 * 1000))) {
            rcar_sata_prd  *prd = tgt->prd;
            uintptr_t      *mmapaddr;
            int            si = 0;

            while (tgt->sata_drv.num_prde--) {
                mmapaddr = mmap(NULL, prd[si].ddtrc,
                     PROT_READ | PROT_WRITE | PROT_NOCACHE, MAP_PHYS |
                     MAP_SHARED, NOFD, prd[si].ddsta & ~1);

                sata_data_pio_rw(tgt, (void *)mmapaddr, prd[si].ddtrc, dir);

                munmap(mmapaddr, prd[si].ddtrc);

                si++;
            }
        }
    }
}

/******************************************************************************/
int sata_command(EIDE_TARGET *tgt, ATA_TASKFILE *tf, uint8_t *acmd,
                            uint32_t alen, SG_ELEM *sge, int sgc, int timeo)
{
    SIM_HBA             *hba = tgt->hba;
    SIM_EIDE_EXT        *ext = (SIM_EIDE_EXT *)hba->ext;
    uintptr_t           bar = (uintptr_t)ext->bar;
    rcar_sata_tf_regs   *tfregs = (rcar_sata_tf_regs *)(bar + SDATA_REG);
    int                 stat = CAM_SUCCESS;
    rcar_sata_drv       *sata_drv = &tgt->sata_drv;
    uint8_t             log_level = 5;

    EXEC_TRACE;

    tgt->sataintstat = 0;

    if (!timeo)
        timeo = hba->timeout;

    /* Free the copied SGs */
    sata_free_sge(tgt);

    sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, log_level,
            "%s : (%02x) : %s", __FUNCTION__, tf->command,
            sata_cmd_description(tf->command));

    sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_INFO, 5,
        "cmd=%x, feat=%x:%x, lbal=%x:%x, lbam=%x:%x, lbah=%x:%x, "
        "seccnt=%x:%x, dev=%x, cntl=%x",
        tf->command, tf->hob_feature, tf->feature, tf->hob_lbal,
        tf->lbal, tf->hob_lbam, tf->lbam, tf->hob_lbah, tf->lbah,
        tf->hob_seccnt, tf->seccnt, tf->device, tf->control);

    sata_drv->is_read_cmd = tf->tflags & ATA_TF_DIR_IN;

    if (((tf->tflags & ATA_TF_ATAPI) && (tf->feature == 0))
        || (tf->command == EIDE_CMD_ATA_IDENTIFY)) {
        sata_drv->cmd_pio = 1;
    } else {
        sata_drv->cmd_pio = 0;
    }

    /************************/
    /* Task file to Device */
    /************************/
    tgt->cmd = tf->command;

    sata_dma_stop(tgt);

    sata_tf_load(tgt, tf);

    /* Set physical region table descriptor(PRD) */
    if (sgc && (sata_drv->num_prde = sata_setup_prd(tgt,
                                sge, sgc, tf)) == 0) {
        sata_log(tgt, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,
                    "%s: sata_setup_prd failed", __FUNCTION__);

        sata_free_sge(tgt);

        return (CAM_FAILURE);
    }
    if ((sata_drv->num_prde > 0) && (sata_drv->cmd_pio == 0)) {
        sata_dma_setup(tgt, tf);

        sata_dma_start(tgt);
    }

    /* Clear status */
    out32(bar + ATAPI_STATUS_REG, 0);

    /* Issue command */
    sata_exec_command(tgt, tf);

    if (tf->tflags & ATA_TF_ATAPI) {
        if (acmd != NULL) {
            memset(sata_drv->acmd, 0, ACMD_BUFFER_LEN);
            memcpy(sata_drv->acmd, acmd, alen);
            if (sata_wait_register((uintptr_t)& tfregs->ssaltsts_ssdevcon,
                             EIDE_STATUS_BSY | EIDE_STATUS_DRQ, EIDE_STATUS_DRQ, (timeo * 1000))) {
                stat = CAM_FAILURE;
            } else {
                sata_data_pio_rw(tgt, (void *)sata_drv->acmd, ACMD_BUFFER_LEN, ATA_TF_DIR_OUT);
                sata_data_pio(tgt, tf->tflags & ATA_TF_DIR_IN ? ATA_TF_DIR_IN : ATA_TF_DIR_OUT);
            }
        }
    }

    switch (tf->tflags & ATA_TF_TYPE_MSK) {

        case ATA_TF_IMMEDIATE:
            /* Issue command and return. Interrupts must be enabled.
             * Command ends in the interrupt routine. If interrupt
             * does not occur, command is ended by the SIMQ timer
             * with time-out indication (see eide_timer()) */

             break;

        case ATA_TF_INTR_WAIT:
            /* Issue command and wait for command completion. Interrupts
             * must be disabled. Timer is disabled. */
            if (hba->simq) {
                simq_timer_settime(hba->simq, SIMQ_TIMER_DISABLE);
            }

            if (sata_wait_interrupt(tgt, SATAINTSTAT_ATA, (timeo * 1000))) {
                stat = CAM_FAILURE;
            } else {
                sata_data_pio(tgt, tf->tflags & ATA_TF_DIR_IN ? ATA_TF_DIR_IN : ATA_TF_DIR_OUT);
            }
            sata_data_dump_all(tgt);

            sata_free_sge(tgt);
            if (hba->simq) {
                simq_timer_settime(hba->simq, SIMQ_TIMER_ENABLE);
            }

            break;
        case ATA_TF_BUSY_WAIT:
            /* Issue command and wait for command completion. Interrupts
             * must be disabled. Timer is disabled. */
            if (hba->simq) {
                simq_timer_settime(hba->simq, SIMQ_TIMER_DISABLE);
            }

            if (sata_wait_register((uintptr_t)&tfregs->ssstatus_sscom,
                        EIDE_STATUS_BSY | EIDE_STATUS_DRQ, EIDE_STATUS_DRQ, (timeo * 1000))) {
                sata_end_command(tgt);
                stat = CAM_FAILURE;
            } else {
                sata_data_pio(tgt, tf->tflags & ATA_TF_DIR_IN ? ATA_TF_DIR_IN : ATA_TF_DIR_OUT);
                stat = sata_end_command(tgt);
            }

            sata_data_dump_all(tgt);

            sata_free_sge(tgt);
            if (hba->simq) {
                simq_timer_settime(hba->simq, SIMQ_TIMER_ENABLE);
            }

            break;
        default:
            break;
    }

    return (stat);
}

/******************************************************************************/
int sata_dinit(SIM_HBA *hba)
{
    SIM_EIDE_EXT    *ext = (SIM_EIDE_EXT *)hba->ext;
    EIDE_TARGET     *tgt = NULL;
    uintptr_t       bar = (uintptr_t)ext->bar;
    rcar_sata_drv   *sata_drv;

    EXEC_TRACE;

    if (ext->bar) {
        // Free port resources
        tgt = &ext->targets[0];

        /* Close instance semaphore */
        if (tgt->sata_sem != SEM_FAILED) {
            char            sem_name[]= "rcarsata-xxxxxxxx";

            sprintf(sem_name, "rcarsata-%08x", (uint32_t)ext->IOPort_Base);
            sem_close(tgt->sata_sem);
            sem_unlink(sem_name);

            tgt->sata_sem = SEM_FAILED;
        }

        eide_dma_mem_free(tgt->prd, sizeof(rcar_sata_prd) * COMMAND_PRD_MAX_NUM);

        sata_drv = &tgt->sata_drv;

#if defined(SATA_DATA_DUMP)
        pthread_mutex_destroy(&sata_drv->mutex);
#endif

        sata_disable_port(tgt);
        /* disable interrupts */
        out32(bar + ATAPI_INT_ENABLE_REG, 0);
        /* ack and mask */
        out32(bar + SATAINTSTAT_REG, 0);
        out32(bar + SATAINTMASK_REG, 0x7ff);

        munmap_device_io(ext->bar, hba->cfg.IOPort_Length[0]);

        ext->bar = 0;
    }

    return (CAM_SUCCESS);
}

/******************************************************************************/
int sata_port_reset(EIDE_TARGET *tgt, uint8_t intr)
{
    SIM_HBA         *hba = tgt->hba;
    SIM_EIDE_EXT    *ext = (SIM_EIDE_EXT *)hba->ext;;
    uint32_t        bar = (uint32_t)ext->bar;
    uint32_t        val32;
    int             stat;

    EXEC_TRACE;

    if (intr) {
        sata_disable_intr(tgt);
    }
    if ((stat = sata_disable_port(tgt)) != CAM_SUCCESS) {
        return stat;
    }

    /* Bring the SATA controller online */
    val32 = in32(bar + ATAPI_CONTROL1_REG);
    val32 &= ~ATAPI_CONTROL1_RESET;
    out32(bar + ATAPI_CONTROL1_REG, val32);

    if ((stat = sata_establish_comm(tgt)) != CAM_SUCCESS) {
        return stat;
    }

    /* Clear pending interrupts */
    out32(bar + ATAPI_STATUS_REG, 0);

    if (intr) {
        sata_enable_intr(tgt);
    }

    return (stat);
}

/*****************************************************************************/
static void sata_init_controller(EIDE_TARGET *tgt)
{
    SIM_HBA         *hba = tgt->hba;
    SIM_EIDE_EXT    *ext = (SIM_EIDE_EXT *)hba->ext;;
    uintptr_t       bar = ext->bar;
    uint32_t        val32;

    /* SATA-IP reset state */
    val32 = in32(bar + ATAPI_CONTROL1_REG);
    val32 |= ATAPI_CONTROL1_RESET;
    out32(bar + ATAPI_CONTROL1_REG, val32);

    /* ISM mode, PRD mode, DTEND flag at bit 0 */
    val32 = in32(bar + ATAPI_CONTROL1_REG);
    val32 |= ATAPI_CONTROL1_ISM;
    val32 |= ATAPI_CONTROL1_DESE;
    val32 |= ATAPI_CONTROL1_DTA32M;
    out32(bar + ATAPI_CONTROL1_REG, val32);

    /* Release the SATA-IP from the reset state */
    val32 = in32(bar + ATAPI_CONTROL1_REG);
    val32 &= ~ATAPI_CONTROL1_RESET;
    out32(bar + ATAPI_CONTROL1_REG, val32);

    /* ack and mask */
    out32(bar + SATAINTSTAT_REG, 0);
    out32(bar + SATAINTMASK_REG, 0x7ff);
    /* enable interrupts */
    out32(bar + ATAPI_INT_ENABLE_REG, ATAPI_INT_ENABLE_SATAINT);
}

/*****************************************************************************/
int sata_init(SIM_HBA *hba)
{
    SIM_EIDE_EXT        *ext = (SIM_EIDE_EXT *)hba->ext;
    SIM_CONFIG_INFO     *cfg = (SIM_CONFIG_INFO *)&hba->cfg;
    EIDE_TARGET         *tgt = NULL;
    EIDE_REGS           *regs;
    rcar_sata_tf_regs   *tfregs;
    uintptr_t           bar;
    char                sem_name[]= "rcarsata-xxxxxxxx";

    EXEC_TRACE;
    tgt = &ext->targets[0];
    tgt->port = 0;
    tgt->sata_sem = SEM_FAILED;

    if (!ext->IOPort_Base || !hba->cfg.NumIRQs) {
        sata_log(NULL, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 2,
            "%s: No base address or interrupt", __FUNCTION__);

        sata_dinit(hba);
        return (CAM_FAILURE);
    }

    if (!cfg->IOPort_Length[0]) {
        cfg->IOPort_Length[0] = SATA_DEFAULT_BASE_LEN;
    }

    if ((void *)(bar = ext->bar =
            (uintptr_t)mmap_device_memory(NULL, cfg->IOPort_Length[0],
                    PROT_READ | PROT_WRITE | PROT_NOCACHE, MAP_SHARED,
                    ext->IOPort_Base)) == MAP_FAILED) {
        sata_log(NULL, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 2,
            "%s: Cannot map the base address", __FUNCTION__);

        sata_dinit(hba);
        return (CAM_FAILURE);
    }

    /* Check if driver already runs on the specified controller */
    sprintf(sem_name, "rcarsata-%08x", (uint32_t)ext->IOPort_Base);

    /* Create instance semaphore */
    tgt->sata_sem = sem_open(sem_name, O_CREAT | O_EXCL, 0666, 1 );
    if (tgt->sata_sem == SEM_FAILED) {
        sata_log(NULL, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 2,
            "%s: R-Car SATA driver already runs on the %08x controller\n",
                __FUNCTION__, (uint32_t)ext->IOPort_Base);
        return CAM_FAILURE;
    }

    /* HBA capabilities register (CAP) */
    /* Supports ATAPI devices */
    ext->s_cap = SATA_CAP_ATAPI;
    /* Supports Native Command Queuing */
    ext->s_cap |= SATA_CAP_SNCQ;
    /* Supports Staggered Spin-up */
    ext->s_cap |= SATA_CAP_SSS;
    /* Supports Port Multiplier */
    ext->s_cap |= SATA_CAP_SPM;
    /* Host doesn't support multi-block */
    ext->s_cap |= SATA_CAP_PMD;

    tfregs = (rcar_sata_tf_regs *)(bar + SDATA_REG);

    regs = &tgt->regs;

    regs->status = (uintptr_t)&tfregs->ssstatus_sscom;
    regs->error = (uintptr_t)&tfregs->sserr_ssfeatures;
    regs->features = (uintptr_t)&tfregs->sserr_ssfeatures;
    regs->seccnt = (uintptr_t)&tfregs->sseccnt;
    regs->cyllo = (uintptr_t)&tfregs->slbalow;
    regs->cylmi = (uintptr_t)&tfregs->slbamid;
    regs->cylhi = (uintptr_t)&tfregs->slbahigh;
    regs->astatus = (uintptr_t)&tfregs->ssaltsts_ssdevcon;

    /******************/
    /* SATA registers */
    /******************/
    regs->sstatus = bar + 0x1400;
    regs->serror = bar + 0x1404;
    regs->scontrol = bar + 0x1408;
    /* SATA_CAR at 0x008 */
    regs->sactive = bar + 0x140C;;

    /* Initialize SATA controller */
    sata_init_controller(tgt);

    if (sata_disable_port(tgt)) {
        sem_close(tgt->sata_sem);
        sem_unlink(sem_name);
        return CAM_FAILURE;
    }

    sata_hardware_reset(tgt);

    if (sata_drv_init(tgt)) {
        sem_close(tgt->sata_sem);
        sem_unlink(sem_name);
        return CAM_FAILURE;
    }

    if (sata_port_reset(tgt, 1)) {
        sem_close(tgt->sata_sem);
        sem_unlink(sem_name);
        return CAM_FAILURE;
    }

    return (CAM_SUCCESS);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/devb/rcarsata/sim_sata.c $ $Rev: 853240 $")
#endif
