/*
 * $QNXLicenseC:
 * Copyright 2018 QNX Software Systems.
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

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <hw/inout.h>
#include <sys/mman.h>
#include <internal.h>
#include <sys/syspage.h>
#include <inttypes.h>
#include <fcntl.h>


#ifdef SDIO_HC_MTK_MSDC_SDMMC
#include <mtk_msdc.h>


static const uint32_t err_ints_mask = MSDC_INTEN_RSPCRCERR | MSDC_INTEN_CMDTMO |
            MSDC_INTEN_ACMDCRCERR | MSDC_INTEN_ACMDTMO |
            MSDC_INTEN_DATTMO | MSDC_INTEN_DATCRCERR |
            MSDC_INTEN_DMA_BDCSERR | MSDC_INTEN_DMA_GPDCSERR | MSDC_INTEN_DMA_PROTECT |
            MSDC_INTEN_AXIRSPERR;
static const uint32_t cmd_ints_mask = MSDC_INTEN_CMDRDY;
static const uint32_t data_ints_mask = MSDC_INTEN_XFER_COMPL;

static void mtk_msdc_sdmmc_reset(sdio_hc_t *hc);

static int mtk_msdc_sdmmc_dma_setup(sdio_hc_t *hc, sdio_cmd_t *cmd);
static int mtk_msdc_sdmmc_timing(sdio_hc_t *hc, int timing);


static void msdc_set_timeout(sdio_hc_t *hc, uint32_t ns, uint32_t clks);
static int msdc_cmd_done(sdio_hc_t *hc, int events);
static int msdc_data_xfer_done(sdio_hc_t *hc, int events);
static void msdc_cmd_error(sdio_hc_t *hc, sdio_cmd_t *cmd, int status, const char *func);
static void msdc_dma_reset(sdio_hc_t *hc);
static void msdc_fifo_reset(sdio_hc_t *hc, const char *func);

extern int mtk_bs_signal_voltage(sdio_hc_t *hc, int signal_voltage);

//#ifdef MTK_MSDC_DEBUG
static int mtk_msdc_reg_dump( sdio_hc_t *hc, const char *func, int line, int level )
{
    mtk_msdc_sdmmc_t    *sdmmc = (mtk_msdc_sdmmc_t *)hc->cs_hdl;
    uintptr_t           base = sdmmc->base;

    sdio_slogf( _SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, level, "%s: %s: line %d hc idx%d base_addr 0x%x", __FUNCTION__, func, line, hc->cfg.idx, (unsigned int)hc->cfg.base_addr[0] );
    sdio_slogf( _SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, level, "%s: CFG %x IOCON  %x PS %x", func,
                in32( base + MSDC_CFG ),
                in32( base + MSDC_IOCON ),
                in32( base + MSDC_PS ));

    sdio_slogf( _SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, level, "%s: INT %x INTEN  %x FIFOCS %x", func,
                in32( base + MSDC_INT ),
                in32( base + MSDC_INTEN ),
                in32( base + MSDC_FIFOCS ));

    sdio_slogf( _SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, level, "%s: SDC_CFG %x SDC_CMD %x SDC_ARG  %x SDC_STS %x", func,
                in32( base + SDC_CFG ),
                in32( base + SDC_CMD ),
                in32( base + SDC_ARG ),
                in32( base + SDC_STS ));

    sdio_slogf( _SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, level, "%s: SDC_RESP0 %x SDC_RESP1 %x SDC_RESP2 %x SDC_RESP3 %x", func,
                in32( base + SDC_RESP0 ),
                in32( base + SDC_RESP1 ),
                in32( base + SDC_RESP2 ),
                in32( base + SDC_RESP3 ));

    sdio_slogf( _SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, level, "%s: SDC_BLK_NUM %x SDC_VOL_CHG %x", func,
                in32( base + SDC_BLK_NUM ),
                in32( base + SDC_VOL_CHG ));

    sdio_slogf( _SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, level, "%s: SDC_CSTS %x SDC_CSTS_EN %x SDC_DATCRC_STS %x SDC_ADV_CFG0 %x", func,
                in32( base + SDC_CSTS ),
                in32( base + SDC_CSTS_EN ),
                in32( base + SDC_DATCRC_STS ),
                in32( base + SDC_ADV_CFG0 ));

    sdio_slogf( _SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, level, "%s: EMMC_CFG0 %x EMMC_CFG1 %x EMMC_STS  %x EMMC_IOCON %x", func,
                in32( base + EMMC_CFG0 ),
                in32( base + EMMC_CFG1 ),
                in32( base + EMMC_STS ),
                in32( base + EMMC_IOCON ));

    sdio_slogf( _SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, level, "%s: SDC_ACMD_RESP %x SDC_ACMD19_TRG %x SDC_ACMD19_STS %x", func,
                in32( base + SDC_ACMD_RESP ),
                in32( base + SDC_ACMD19_TRG ),
                in32( base + SDC_ACMD19_STS ));

    sdio_slogf( _SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, level, "%s: DMA_SA_H4 %x DMA_SA %x DMA_CA %x", func,
                in32( base + MSDC_DMA_SA_H4 ),
                in32( base + MSDC_DMA_SA ),
                in32( base + MSDC_DMA_CA ));

    sdio_slogf( _SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, level, "%s: DMA_CTRL %x DMA_CFG %x SW_DBG_SEL %x SW_DBG_OUT %x DMA_LENGTH %d", func,
                in32( base + MSDC_DMA_CTRL ),
                in32( base + MSDC_DMA_CFG ),
                in32( base + MSDC_SW_DBG_SEL ),
                in32( base + MSDC_SW_DBG_OUT ),
                in32( base + MSDC_DMA_LENGTH ));

    sdio_slogf( _SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, level, "%s: PATCH_BIT0 %x PATCH_BIT1 %x PATCH_BIT2 %x", func,
                in32( base + MSDC_PATCH_BIT0 ),
                in32( base + MSDC_PATCH_BIT1 ),
                in32( base + MSDC_PATCH_BIT2 ));

    sdio_slogf( _SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, level, "%s: PAD_TUNE %x PAD_TUNE0 %x PAD_TUNE1 %x", func,
                in32( base + 0xec ),
                in32( base + MSDC_PAD_TUNE0 ),
                in32( base + MSDC_PAD_TUNE1 ));

    sdio_slogf( _SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, level, "%s: DAT_RD_DLY0 %x DAT_RD_DLY1 %x DAT_RD_DLY2 %x DAT_RD_DLY3 %x", func,
                in32( base + MSDC_DAT_RD_DLY0 ),
                in32( base + MSDC_DAT_RD_DLY1 ),
                in32( base + MSDC_DAT_RD_DLY2 ),
                in32( base + MSDC_DAT_RD_DLY3 ));

    sdio_slogf( _SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, level, "%s: PAD_CTL0 %x PAD_DS_CTL0 %x PAD_DS_TUNE %x PAD_CMD_TUNE %x", func,
                in32( base + PAD_CTL0 ),
                in32( base + PAD_DS_CTL0 ),
                in32( base + PAD_DS_TUNE ),
                in32( base + PAD_CMD_TUNE ));

    sdio_slogf( _SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, level, "%s: PAD_DAT01_TUNE %x PAD_DAT23_TUNE %x PAD_DAT45_TUNE %x PAD_DAT67_TUNE %x", func,
                in32( base + PAD_DAT01_TUNE ),
                in32( base + PAD_DAT23_TUNE ),
                in32( base + PAD_DAT45_TUNE ),
                in32( base + PAD_DAT67_TUNE ));

    sdio_slogf( _SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, level, "%s: EMMC51_CFG0 %x EMMC50_CFG0 %x", func,
                in32( base + EMMC51_CFG0 ),
                in32( base + EMMC50_CFG0 ));

    sdio_slogf( _SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, level, "%s: EMMC50_CFG1 %x EMMC50_CFG2 %x EMMC50_CFG3 %x EMMC50_CFG4 %x", func,
                in32( base + EMMC50_CFG1 ),
                in32( base + EMMC50_CFG2 ),
                in32( base + EMMC50_CFG3 ),
                in32( base + EMMC50_CFG4 ));

    sdio_slogf( _SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, level, "%s: SDC_FIFO_CFG %x", func,
                in32( base + SDC_FIFO_CFG ));

    return( EOK );
}
//#endif



static int mtk_msdc_waitmask( sdio_hc_t *hc, uint32_t reg, uint32_t mask, uint32_t val, uint32_t usec )
{
    uint32_t            cnt;
    int                 stat;
    int                 rval;
    uint32_t            iter;
    mtk_msdc_sdmmc_t    *sdmmc = (mtk_msdc_sdmmc_t *)hc->cs_hdl;
    uintptr_t           base = sdmmc->base;

    stat    = ETIMEDOUT;
    rval    = 0;
    if(usec == 0)
        usec = 1;

    /* fast poll for 1ms - 1us intervals, or first 1ms of longer intervals */
    for( cnt = min( usec, 1000 ); cnt; cnt-- ) {
        if( ( ( rval = in32( base + reg ) ) & mask ) == val ) {
            stat = EOK;
            break;
        }
        nanospin_ns( 1000L );
    }

    if( usec > 1000 && stat ) {
        iter    = ((usec + 1000L - 1) / 1000L) - 1;
        for( cnt = iter; cnt; cnt-- ) {
            if( ( ( rval = in32( base + reg ) ) & mask ) == val ) {
                stat = EOK;
                break;
            }
            delay( 1 );
        }
    }

    return( stat );
}


static int mtk_msdc_intr_event( sdio_hc_t *hc )
{
    sdio_cmd_t          *cmd = hc->wspc.cmd;
    uint32_t            events, event_mask;
    mtk_msdc_sdmmc_t    *sdmmc = (mtk_msdc_sdmmc_t *)hc->cs_hdl;
    uintptr_t           base = sdmmc->base;

    events = in32(base + MSDC_INT);
    event_mask = in32(base + MSDC_INTEN);

    sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 2,  "%s: hc idx%d base_addr 0x%x events=%08X unmasked_events=%08X, cmd %d", __func__,
            hc->cfg.idx, (unsigned int)hc->cfg.base_addr[0], events, events & event_mask, cmd->opcode);

    /* clear interrupts */
    out32(base + MSDC_INT, events);
    sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 2,  "%s:%d events=%08X", __func__, __LINE__, events);

    if (cmd) {
        if(msdc_cmd_done(hc, events) == EOK && ((cmd->flags & (SCF_CTYPE_ADTC|SCF_DATA_MSK)) == 0 || msdc_data_xfer_done(hc, events) == EOK))
                sdio_cmd_cmplt( hc, cmd, CS_CMD_CMP );
    }

    out32(base + MSDC_INTEN, 0);

    return (EOK);
}

int mtk_msdc_sdmmc_event( sdio_hc_t *hc, sdio_event_t *ev )
{
    int                 status;

    sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 2,  "%s: ev->code = %08X", __func__, ev->code);
    switch( ev->code ) {
        case HC_EV_INTR:
            status = mtk_msdc_intr_event( hc );
            InterruptUnmask( hc->cfg.irq[0], hc->hc_iid );
            break;

        default:
            status = bs_event( hc, ev );
            break;
    }

    return( status );
}

static uint8_t msdc_dma_calcs(uint8_t *buf, uint32_t len)
{
    uint32_t i, sum = 0;

    for (i = 0; i < len; i++)
        sum += buf[i];
    return 0xff - (uint8_t) sum;
}

static int mtk_msdc_sdmmc_dma_setup(sdio_hc_t *hc, sdio_cmd_t *cmd)
{
    mtk_msdc_sdmmc_t    *sdmmc = (mtk_msdc_sdmmc_t *)hc->cs_hdl;
    struct msdc_dma     *dma = sdmmc->dma;
    uintptr_t           base = sdmmc->base;
    sdio_sge_t          *sgp;
    int                 sgc;
    int                 sgi;
    int                 bdcnt;
    int                 alen;
    int                 sg_count;
    paddr64_t           paddr;
    uint32_t            dma_ctrl, flags;
    struct mt_gpdma_desc    *gpd;
    struct mt_bdma_desc     *bd;

    sgc = cmd->sgc;
    sgp = cmd->sgl;

    if (!(cmd->flags & SCF_DATA_PHYS)) {
        sdio_vtop_sg(sgp, sdmmc->sgl, sgc, cmd->mhdl);
        sgp = sdmmc->sgl;
    }

    gpd = dma->gpd;
    bd  = dma->bd;

    /* modify gpd */
    flags  = gpd->flags;
    flags |= GPDMA_DESC_HWO | GPDMA_DESC_BDP;
    /* need to clear first. use these bits to calc checksum */
    flags &= ~GPDMA_DESC_CHECKSUM;
    gpd->flags = flags;
    gpd->flags |= msdc_dma_calcs((uint8_t *) gpd, 16) << 8;

    /* modify bd */
    for( sgi = 0, bdcnt = 0; sgi < sgc; sgi++, sgp++ ) {
        paddr       = sgp->sg_address;
        sg_count    = sgp->sg_count;

        while( sg_count ) {
            if( bdcnt >= MAX_BD_NUM ) {
                return( ENOTSUP );
            }
            alen = min(sg_count, BDMA_DESC_BUFLEN);
            /* init bd */
            flags = bd[bdcnt].flags;
            flags &= ~(BDMA_DESC_BLKPAD | BDMA_DESC_DWPAD | BDMA_DESC_PTRH4);
            flags |= (uint32_t)(paddr>>32) << 28; /* high 4-bits of paddr */
            bd[bdcnt].ptr = (uint32_t)paddr;
            bd[bdcnt].bd_data_len = alen;

            sg_count    -= alen;
            paddr       += alen;
            if (sgi == sgc-1 && sg_count == 0) /* the last bd */
                flags |= BDMA_DESC_EOL;
            else
                flags &= ~BDMA_DESC_EOL;

            /* checksum need to clear first */
            flags &= ~BDMA_DESC_CHECKSUM;
            bd[bdcnt].flags = flags;
            bd[bdcnt].flags |= msdc_dma_calcs((uint8_t *)(&bd[bdcnt]), 16) << 8;
            bdcnt++;
        }
    }
    sdmmc_clr_bits(base + MSDC_CFG, MSDC_CFG_PIO); /* Use dma mode */
    out32(base + MSDC_DMA_LENGTH, 0); /* clear since only used for Basic DMA mode */
    sdmmc_set_field(base + MSDC_DMA_CFG, MSDC_DMA_CFG_DECSEN, 1); /* Enable descriptor checksum */

    dma_ctrl = in32(base + MSDC_DMA_CTRL);
    dma_ctrl &= ~(MSDC_DMA_CTRL_BRSTSZ | MSDC_DMA_CTRL_MODE | MSDC_DMA_CTRL_LASTBUF);
    dma_ctrl |= ((MSDC_DMA_CTRL_BRST_64B << MSDC_DMA_CTRL_BRSTSZ_SHIFT) | MSDC_DMA_CTRL_MODE); /* 64b burst, enable enhanced DMA descriptors */
    out32(base + MSDC_DMA_CTRL, dma_ctrl);

    out32(base + MSDC_DMA_SA, (uint32_t)dma->gpd_addr);
    out32(base + MSDC_DMA_SA_H4, (uint32_t)(dma->gpd_addr>>32));

    return (EOK);
}

static int mtk_msdc_sdmmc_cd( sdio_hc_t *hc )
{
    sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 3, "%s:%d idx%d base_addr 0x%x", __func__, __LINE__, hc->cfg.idx, (unsigned int)hc->cfg.base_addr[0]);
    return( CD_INS );        // external logic required
}


#define mmc_resp_type(cmd)  ((cmd)->flags & (SCF_RSP_PRESENT| SCF_RSP_136| SCF_RSP_CRC| SCF_RSP_BUSY | SCF_RSP_OPCODE))
static inline uint32_t  msdc_cmd_find_resp(sdio_cmd_t *cmd)
{
     uint32_t resp;

    switch (mmc_resp_type(cmd)) {
        /* Actually, R1, R5, R6, R7 are the same */
    case SCF_RSP_R1:
        resp = 0x1;
        break;
    case SCF_RSP_R1B:
        resp = 0x7;
        break;
    case SCF_RSP_R2:
        resp = 0x2;
        break;
    case SCF_RSP_R3:
        resp = 0x3;
        break;
    case SCF_RSP_NONE:
    default:
        resp = 0x0;
        break;
    }

    return resp;
}

/* SDIO commands                         type  argument     response */
#define SD_IO_SEND_OP_COND          5 /* bcr  [23:0] OCR         R4  */
#define SD_IO_RW_DIRECT            52 /* ac   [31:0] See below   R5  */
#define SD_IO_RW_EXTENDED          53 /* adtc [31:0] See below   R5  */


#define MMC_CMD_MASK    (3 << 5)        /* non-SPI command type */
#define mmc_cmd_type(cmd)   ((cmd)->flags & MMC_CMD_MASK)

/*
 * SD_IO_RW_DIRECT argument format:
 *
 *      [31] R/W flag
 *      [30:28] Function number
 *      [27] RAW flag
 *      [25:9] Register address
 *      [7:0] Data
 */

/*
 * SD_IO_RW_EXTENDED argument format:
 *
 *      [31] R/W flag
 *      [30:28] Function number
 *      [27] Block mode
 *      [26] Increment address
 *      [25:9] Register address
 *      [8:0] Byte/block count
 */
static inline uint32_t  msdc_cmd_prepare_raw_cmd(sdio_hc_t *hc, sdio_cmd_t *cmd)
{
    /* rawcmd :
     * vol_swt << 30 | auto_cmd << 28 | blklen << 16 | go_irq << 15 |
     * stop << 14 | rw << 13 | dtype << 11 | rsptyp << 7 | brk << 6 | opcode
     */
    uint32_t opcode = cmd->opcode;
    uint32_t resp = msdc_cmd_find_resp(cmd);
    uint32_t rawcmd = (opcode & 0x3f) | ((resp & 0x7) << 7);

    sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 3, "%s:%d CMD %d", __func__, __LINE__, cmd->opcode);

#define MMC_CMD_ADTC    (1 << 5)

    if ((opcode == SD_IO_RW_DIRECT && cmd->flags == (unsigned int) -1) || opcode == MMC_STOP_TRANSMISSION)
        rawcmd |= (0x1 << 14);
    else if (opcode == SD_VOLTAGE_SWITCH)
        rawcmd |= (0x1 << 30);
    else if (opcode == SD_AC_SEND_SCR ||
         opcode == SD_AC_SEND_NUM_WR_BLOCKS ||
         (opcode == SD_SWITCH_FUNC && mmc_cmd_type(cmd) == MMC_CMD_ADTC) ||
         (opcode == SD_SEND_STATUS && mmc_cmd_type(cmd) == MMC_CMD_ADTC) ||
         (opcode == MMC_SEND_EXT_CSD && mmc_cmd_type(cmd) == MMC_CMD_ADTC))
        rawcmd |= (0x1 << 11);

    if (cmd->flags & SCF_DATA_MSK) {

        if (cmd->blks > 1) {
            if((hc->caps & HC_CAP_ACMD23) && (cmd->flags & SCF_SBC))
                rawcmd |= 0x2 << 28; /* AutoCMD23 */
            else if(hc->caps & HC_CAP_ACMD12)
                rawcmd |= 0x1 << 28; /* AutoCMD12 */
        }

        rawcmd |= ((cmd->blksz & 0xFFF) << 16);
        if (cmd->flags & SCF_DIR_OUT)
            rawcmd |= (0x1 << 13);
        if (cmd->blks > 1)
            rawcmd |= (0x2 << 11);
        else
            rawcmd |= (0x1 << 11);

//        if (sdmmc->timeout_ns != data->timeout_ns || sdmmc->timeout_clks != data->timeout_clks)
//            msdc_set_timeout(host, data->timeout_ns, data->timeout_clks);
        msdc_set_timeout(hc, 100 * 1000*1000, 3 * 1048576); /* 100ms + default DTOC */
    }
    return rawcmd;
}

static void msdc_fifo_reset(sdio_hc_t *hc, const char *func)
{
    mtk_msdc_sdmmc_t    *sdmmc = (mtk_msdc_sdmmc_t *)hc->cs_hdl;
    uintptr_t           base = sdmmc->base;

    sdmmc_set_bits(base + MSDC_FIFOCS, MSDC_FIFOCS_CLR); //clear FIFO
    if( EOK != mtk_msdc_waitmask(hc, MSDC_FIFOCS, MSDC_FIFOCS_CLR, 0, MTK_MSDC_CLOCK_TIMEOUT) )
    {
        sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0,  "%s: MSDC_FIFOCS_CLR error MSDC_FIFOCS 0x%x", func, in32(base + MSDC_FIFOCS));
    }
}

static void msdc_dma_reset(sdio_hc_t *hc)
{
    mtk_msdc_sdmmc_t    *sdmmc = (mtk_msdc_sdmmc_t *)hc->cs_hdl;
    uintptr_t           base = sdmmc->base;

    sdmmc_set_bits(base + MSDC_PATCH_BIT1, MSDC_PATCH_BIT1_RESET_GDMA);
    delay(1);
    sdmmc_clr_bits(base + MSDC_PATCH_BIT1, MSDC_PATCH_BIT1_RESET_GDMA);
    delay(1);
}

static void msdc_dma_stop(sdio_hc_t *hc)
{
    mtk_msdc_sdmmc_t    *sdmmc = (mtk_msdc_sdmmc_t *)hc->cs_hdl;
    uintptr_t           base = sdmmc->base;
    int                 status = EOK;

    sdmmc_set_bits(base + MSDC_DMA_CTRL, MSDC_DMA_CTRL_STOP); //stop DMA

    if( EOK != (status = mtk_msdc_waitmask(hc, MSDC_DMA_CFG, MSDC_DMA_CFG_STS, 0, MTK_MSDC_CLOCK_TIMEOUT)))
    {
        sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0,  "%s: MSDC_DMA_CFG_STS failed error MSDC_DMA_CFG 0x%x MSDC_DMA_CTRL 0x%x, resetting DMA.", __func__,
                in32(base + MSDC_DMA_CFG), in32(base + MSDC_DMA_CTRL));
        msdc_dma_reset(hc);
    }
}

static void msdc_start_data(sdio_hc_t *hc, sdio_cmd_t *cmd)
{
    mtk_msdc_sdmmc_t    *sdmmc = (mtk_msdc_sdmmc_t *)hc->cs_hdl;
    uintptr_t           base = sdmmc->base;

    if(in32(base + MSDC_DMA_CFG) & MSDC_DMA_CFG_STS) {
        sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0, "%s: error, DMA already active  cmd=%d DMA data: %d blocks; cmd->flags=%d DMA_CFG 0x%x DMA_CTRL 0x%x",
                __func__, cmd->opcode, cmd->blks, cmd->flags, in32(base + MSDC_DMA_CFG), in32(base + MSDC_DMA_CTRL));
        msdc_dma_stop(hc);
    }

    out32(base + SDC_BLK_NUM, cmd->blks);
    mtk_msdc_sdmmc_dma_setup(hc, cmd);

    sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 3, "%s: DMA start  cmd=%d DMA data: %d blocks; cmd->flags=0x%x",
            __func__, cmd->opcode, cmd->blks, cmd->flags);
}

static void mtk_msdc_sdmmc_reset(sdio_hc_t *hc)
{
    mtk_msdc_sdmmc_t *sdmmc = (mtk_msdc_sdmmc_t *)hc->cs_hdl;
    uintptr_t         base  = sdmmc->base;
    int               status, tryagain = 0;

    if(in32(base + MSDC_DMA_CFG) & MSDC_DMA_CFG_STS) {
        sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0,  "%s: MSDC_DMA_CFG_STS DMA active error MSDC_DMA_CFG 0x%x MSDC_DMA_CTRL 0x%x", __func__, in32(base + MSDC_DMA_CFG), in32(base + MSDC_DMA_CTRL));
        sdmmc_set_bits(base + MSDC_DMA_CTRL, MSDC_DMA_CTRL_STOP); //stop DMA
        if( EOK != (status = mtk_msdc_waitmask(hc, MSDC_DMA_CFG, MSDC_DMA_CFG_STS, 0, MTK_MSDC_CLOCK_TIMEOUT)))
        {
            sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0,  "%s: MSDC_DMA_CFG_STS failed error MSDC_DMA_CFG 0x%x", __func__, in32(base + MSDC_DMA_CFG));
            tryagain++;
        }
    }

    sdmmc_set_bits(base + MSDC_CFG, MSDC_CFG_RST);
    if( EOK != (status = mtk_msdc_waitmask(hc, MSDC_CFG, MSDC_CFG_RST, 0, MTK_MSDC_CLOCK_TIMEOUT)))
    {
        sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0,  "%s: MSDC_CFG_RST error MSDC_CFG 0x%x", __func__, in32(base + MSDC_CFG));
    }

    if( tryagain && EOK == (status = mtk_msdc_waitmask(hc, MSDC_DMA_CFG, MSDC_DMA_CFG_STS, 0, MTK_MSDC_CLOCK_TIMEOUT)))
    {
        sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0,  "%s: MSDC_DMA_CFG_STS wait suceeded after CFG_RST MSDC_DMA_CFG 0x%x", __func__, in32(base + MSDC_DMA_CFG));
        tryagain = 0;
    }

    msdc_fifo_reset(hc, __func__);

    if(tryagain)
    {
       if(EOK == (status = mtk_msdc_waitmask(hc, MSDC_DMA_CFG, MSDC_DMA_CFG_STS, 0, MTK_MSDC_CLOCK_TIMEOUT)))
       {
           sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0,  "%s: MSDC_DMA_CFG_STS wait suceeded after CFG_RST+FIFOCS_CLR MSDC_DMA_CFG 0x%x", __func__, in32(base + MSDC_DMA_CFG));
       }
       else
       {
           sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0,  "%s: MSDC_DMA_CFG_STS wait failed after CFG_RST+FIFOCS_CLR MSDC_DMA_CFG 0x%x, resetting DMA", __func__, in32(base + MSDC_DMA_CFG));
           msdc_dma_reset(hc);
       }

       /* clear FIFO again since DMA may have run between last clear and DMA STOP */
       if (((in32(base + MSDC_FIFOCS) & MSDC_FIFOCS_TXCNT) >> 16) || (in32(base + MSDC_FIFOCS) & MSDC_FIFOCS_RXCNT))
           msdc_fifo_reset(hc, __func__);
    }
}

static void msdc_cmd_error(sdio_hc_t *hc, sdio_cmd_t *cmd, int status, const char *func)
{
    mtk_msdc_sdmmc_t    *sdmmc;
    uintptr_t           base;

    sdmmc = (mtk_msdc_sdmmc_t *)hc->cs_hdl;
    base = sdmmc->base;

    sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0,  "%s: %s error: SDC_CMD 0x%x SDC_ARG 0x%x SDC_BLK_NUM %d", __func__, func, in32(base + SDC_CMD), in32(base + SDC_ARG), in32(base + SDC_BLK_NUM));
    if((cmd->flags & (SCF_CTYPE_ADTC|SCF_DATA_MSK)) != 0) {
        sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0,  "%s: %s error, stop DMA: MSDC_DMA_CFG 0x%x MSDC_DMA_CTRL 0x%x", __func__, func, in32(base + MSDC_DMA_CFG), in32(base + MSDC_DMA_CTRL));
        sdmmc_set_bits(base + MSDC_DMA_CTRL, MSDC_DMA_CTRL_STOP); //stop DMA
        if( EOK != (status = mtk_msdc_waitmask(hc, MSDC_DMA_CFG, MSDC_DMA_CFG_STS, 0, MTK_MSDC_CLOCK_TIMEOUT)))
        {
            sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0,  "%s: %s MSDC_DMA_CFG_STS stop DMA failed error MSDC_DMA_CFG 0x%x", __func__, func, in32(base + MSDC_DMA_CFG));
            mtk_msdc_sdmmc_reset(hc);
        }
    }
    sdio_cmd_cmplt( hc, cmd, status );
}

static int msdc_auto_cmd_done(sdio_hc_t *hc, int events, sdio_cmd_t *cmd)
{
    uint32_t *rsp = cmd->rsp;
    mtk_msdc_sdmmc_t    *sdmmc =  (mtk_msdc_sdmmc_t *)hc->cs_hdl;
    uintptr_t           base  = sdmmc->base;

    rsp[0] = in32(base + SDC_ACMD_RESP);

    if (events & MSDC_INT_ACMDRDY) {
        sdmmc->cmd_error = 0;
    } else {
        if (events & MSDC_INT_ACMDCRCERR) {
            sdmmc->cmd_error = EILSEQ;
            sdmmc->error |= REQ_STOP_EIO;
            msdc_cmd_error( hc, cmd, CS_CMD_CRC_ERR, __FUNCTION__ );
        } else if (events & MSDC_INT_ACMDTMO) {
            sdmmc->cmd_error = ETIMEDOUT;
            sdmmc->error |= REQ_STOP_TMO;
            msdc_cmd_error( hc, cmd, CS_CMD_TO_ERR, __FUNCTION__ );
        } else {
            sdmmc->cmd_error = EIO;
            sdmmc->error |= REQ_STOP_EIO;
            msdc_cmd_error( hc, cmd, CS_CMD_CMP_ERR, __FUNCTION__ );
        }
        sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0,  "%s: AUTO_CMD%d arg=%08X; rsp %08X; cmd_error=%d",
            __func__, cmd->opcode, cmd->arg, rsp[0], sdmmc->cmd_error);
    }
    return sdmmc->cmd_error;
}

static int msdc_cmd_done(sdio_hc_t *hc, int events)
{
    sdio_cmd_t *cmd = hc->wspc.cmd;
    uint32_t *rsp = cmd->rsp;
    mtk_msdc_sdmmc_t    *sdmmc = (mtk_msdc_sdmmc_t *)hc->cs_hdl;
    uintptr_t           base = sdmmc->base;

    sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 3, "%s:%d cmd%d", __func__, __LINE__, cmd->opcode);

    if (events & (MSDC_INT_ACMDRDY | MSDC_INT_ACMDCRCERR | MSDC_INT_ACMDTMO) && msdc_auto_cmd_done(hc, events, cmd) != EOK)
        return sdmmc->cmd_error;

    if (cmd->flags &  SCF_RSP_PRESENT) { // QNX SCF bits is different with linux MMC_RSP_PRESENT
        if (cmd->flags & SCF_RSP_136) {   //check this later
            rsp[0] = in32(base + SDC_RESP3);
            rsp[1] = in32(base + SDC_RESP2);
            rsp[2] = in32(base + SDC_RESP1);
            rsp[3] = in32(base + SDC_RESP0);
        } else {
            rsp[0] = in32(base + SDC_RESP0);
        }
    }

    if ( !(events & MSDC_INT_CMDRDY)) {//need to check m=no error as well
        if (cmd->opcode != MMC_SEND_TUNING_BLOCK &&  //QNX define this one as command 21
            cmd->opcode != 21)//MMC_SEND_TUNING_BLOCK_HS200) //linux define thsi one as 21, QNX has HS200 for CMD6
            /*
             * should not clear fifo/interrupt as the tune data
             * may have alreay come.
             */
            mtk_msdc_sdmmc_reset(hc);

        if (events & MSDC_INT_RSPCRCERR) {
            sdmmc->cmd_error = EILSEQ;
            sdmmc->error |= REQ_CMD_EIO;
            msdc_cmd_error( hc, cmd, CS_CMD_CRC_ERR, __FUNCTION__ );
        } else if (events & MSDC_INT_CMDTMO) {
            sdmmc->cmd_error = ETIMEDOUT;
            sdmmc->error |= REQ_CMD_TMO;
            msdc_cmd_error( hc, cmd, CS_CMD_TO_ERR, __FUNCTION__ );
        } else {
            sdmmc->cmd_error = EIO;
            sdmmc->error |= REQ_CMD_EIO;
            msdc_cmd_error( hc, cmd, CS_CMD_CMP_ERR, __FUNCTION__ );
        }
    }
    if (sdmmc->cmd_error)
        sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0,  "%s: CMD%d arg=%08X; rsp %08X; cmd_error=%d",
            __func__, cmd->opcode, cmd->arg, rsp[0], sdmmc->cmd_error);
    return sdmmc->cmd_error;
}

/* It is the core layer's responsibility to ensure card status
 * is correct before issue a request. but host design do below
 * checks recommended.
 */
static inline int msdc_cmd_is_ready(sdio_hc_t *hc, sdio_cmd_t *cmd)
{
    int                 status;
    mtk_msdc_sdmmc_t    *sdmmc;

    sdmmc = (mtk_msdc_sdmmc_t *)hc->cs_hdl;

    /* The max busy time we can endure is 20ms */
    if( EOK != (status = mtk_msdc_waitmask(hc, SDC_STS, SDC_STS_CMDBUSY, 0, 20000))) //20ms
    {
        sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0,  "%s: EMMC: CMD %d cannot execute because CMD busy", __func__, cmd->opcode);
        sdmmc->error |= REQ_CMD_BUSY;
        msdc_cmd_done(hc, MSDC_INT_CMDTMO);
        return (EAGAIN);//ETIMEDOUT false
    }

    if (mmc_resp_type(cmd) == SCF_RSP_R1B  || cmd->flags & SCF_DATA_MSK) {  ///data?
        /* R1B or with data, should check SDCBUSY */
        if( EOK !=  (status = mtk_msdc_waitmask(hc, SDC_STS, SDC_STS_SDCBUSY, 0, 20000))) //20ms
        {
            sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0,  "%s:%d EMMC: CMD %d cannot execute because SDC busy", __func__, __LINE__, cmd->opcode);
            sdmmc->cmd_error |= REQ_CMD_BUSY;
            msdc_cmd_done(hc, MSDC_INT_CMDTMO);
            return (EAGAIN);//ETIMEDOUT
        }
    }
    sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 3, "%s:%d CMD %d OK", __func__, __LINE__, cmd->opcode);
    return (EOK);
}

static int mtk_msdc_sdmmc_cmd(sdio_hc_t *hc, sdio_cmd_t *cmd)
{
    uint32_t            rawcmd;
    mtk_msdc_sdmmc_t    *sdmmc;
    uintptr_t           base;

    sdmmc = (mtk_msdc_sdmmc_t *)hc->cs_hdl;
    base = sdmmc->base;

    sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 3, "%s:%d CMD %d", __func__, __LINE__, cmd->opcode);
    if ( EOK != msdc_cmd_is_ready(hc, cmd)){
        sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0, "^^^^^^%s:%d CMD %d  not ready", __func__, __LINE__, cmd->opcode);
        return(-1);
    }

    if (((in32(base + MSDC_FIFOCS) & MSDC_FIFOCS_TXCNT) >> 16) || (in32(base + MSDC_FIFOCS) & MSDC_FIFOCS_RXCNT)) {
        sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0, "%s: TX/RX FIFO non-empty before start of IO (TX %d, RX %d). Resetting", __FUNCTION__,
                (in32(base + MSDC_FIFOCS) & MSDC_FIFOCS_TXCNT) >> 16, in32(base + MSDC_FIFOCS) & MSDC_FIFOCS_RXCNT);
        mtk_msdc_sdmmc_reset(hc);
    }

    sdmmc->cmd_error = 0;
    rawcmd = msdc_cmd_prepare_raw_cmd(hc, cmd);

    out32(base + MSDC_INT, in32(base + MSDC_INT)); // clear any pending ints

    if (cmd->flags & (SCF_CTYPE_ADTC|SCF_DATA_MSK)) { //or   if( ( cmd->flags & SCF_DATA_MSK ) )
        sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 3, "%s:%d data CMD %d rawcmd 0x%x arg 0x%x", __func__, __LINE__, cmd->opcode, rawcmd, cmd->arg);
        msdc_start_data(hc, cmd);//==> mtk_msdc_sdmmc_dma_setup
        sdmmc_set_bits(base + MSDC_INTEN, err_ints_mask|data_ints_mask);
    } else {
        sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 3, "%s:%d      CMD %d rawcmd 0x%x arg 0x%x", __func__, __LINE__, cmd->opcode, rawcmd, cmd->arg);
        sdmmc_set_bits(base + MSDC_INTEN, err_ints_mask|cmd_ints_mask);
    }

    out32(base + SDC_ARG, cmd->arg);
    out32(base + SDC_CMD, rawcmd);


    if (cmd->flags & (SCF_CTYPE_ADTC|SCF_DATA_MSK))
        sdmmc_set_bits(base + MSDC_DMA_CTRL, MSDC_DMA_CTRL_START);

	if(cmd->opcode == 11) //log voltage switch rsp, resp1 = 0 OK
		sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 1, "%s:%d cmd %d resp1 0x%x", __func__, __LINE__, cmd->opcode, cmd->rsp[1]);

    sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 3, "%s:%d CMD %d  OK !!!", __func__, __LINE__, cmd->opcode);
    return (EOK);
}

static int msdc_data_xfer_done(sdio_hc_t *hc, int events)
{
    int                 status = EOK, bytes_xfered = 0;
    mtk_msdc_sdmmc_t    *sdmmc;
    uintptr_t           base;

    sdio_cmd_t *cmd = hc->wspc.cmd;
    sdmmc = (mtk_msdc_sdmmc_t *)hc->cs_hdl;
    base = sdmmc->base;

    if(in32(base + MSDC_DMA_CFG) & MSDC_DMA_CFG_STS) {
        sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0,  "%s: cmd %d: blks %d blksz %d DMA stop: DMA_CFG: 0x%x DMA_CTRL: 0x%x", __FUNCTION__,
                cmd->opcode, cmd->blks, cmd->blksz, in32(base + MSDC_DMA_CFG), in32(base + MSDC_DMA_CTRL));
        sdmmc_set_bits(base + MSDC_DMA_CTRL, MSDC_DMA_CTRL_STOP); //stop DMA
        if( EOK != (status = mtk_msdc_waitmask(hc, MSDC_DMA_CFG, MSDC_DMA_CFG_STS, 0, 20000))) { //20ms?? 1 second
            sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0,  "%s: error DMA stop failed: DMA_CFG: 0x%x", __FUNCTION__, in32(base + MSDC_DMA_CFG));
            msdc_cmd_error( hc, cmd, CS_DATA_TO_ERR, __FUNCTION__ );
            return status;
        }
    }

    if ((events & MSDC_INT_XFER_COMPL)) {
        bytes_xfered = cmd->blks * cmd->blksz;
    } else {
        sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0, "%s: error interrupt events: %x, resetting", __FUNCTION__, events);
        mtk_msdc_sdmmc_reset(hc);
        sdmmc->error |= REQ_DAT_ERR;
        bytes_xfered = 0;
        if (events & MSDC_INT_DATTMO) {
            status = ETIMEDOUT;
            msdc_cmd_error( hc, cmd, CS_DATA_TO_ERR, __FUNCTION__ );
        } else if (events & MSDC_INT_DATCRCERR) {
            status = EILSEQ;
            msdc_cmd_error( hc, cmd, CS_DATA_CRC_ERR, __FUNCTION__ );
        } else {
            status = EIO;
            msdc_cmd_error( hc, cmd, CS_DATA_END_ERR, __FUNCTION__ );
        }
        sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0,  "%s: cmd=%d; blocks=%d data_error=%d xfer_size=%d", __func__, cmd->opcode, cmd->blks, cmd->status, bytes_xfered);
    }

    return(status);
}


static int mtk_msdc_sdmmc_abort(sdio_hc_t *hc, sdio_cmd_t *cmd)
{
    sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0,  "%s: hc idx%d base_addr 0x%x cmd%d", __func__, hc->cfg.idx, (unsigned int)hc->cfg.base_addr[0], cmd->opcode);

    mtk_msdc_reg_dump(hc, __func__, __LINE__, 0);

    mtk_msdc_sdmmc_reset(hc);

    return (EOK);
}

static void msdc_hw_reset(sdio_hc_t *hc)
{
    mtk_msdc_sdmmc_t    *sdmmc = (mtk_msdc_sdmmc_t *)hc->cs_hdl;
    uintptr_t           base = sdmmc->base;

    /* Hardware reset eMMC chip */
    sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 0, "%s: HW Reset eMMC chip", __func__);
    out32(base + EMMC_IOCON, EMMC_IOCON_BOOT_RST);
    delay(1);
    out32(base + EMMC_IOCON, 0);
    delay(1);

    if(EOK != mtk_msdc_waitmask(hc, EMMC_IOCON, EMMC_IOCON_BOOT_RST, 0, 20000)) //20ms
    {
        sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0,  "%s: EMMC reset failed EMMC_IOCON 0x%x", __func__, in32(base+ EMMC_IOCON));
    }
}

static int mtk_msdc_sdmmc_pwr(sdio_hc_t *hc, int vdd)
{
    hc->vdd = vdd;

    sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 3,  "%s: hc idx%d base_addr 0x%x vdd 0x%x", __func__, hc->cfg.idx, (unsigned int)hc->cfg.base_addr[0], vdd);

    if(vdd == 0 && (hc->caps & HC_CAP_SLOT_TYPE_EMBEDDED) != 0) {
        /* Hardware reset eMMC chip */
        msdc_hw_reset(hc);
    }

    return (EOK);
}

static int mtk_msdc_sdmmc_bus_mode(sdio_hc_t *hc, int bus_mode)
{
    hc->bus_mode = bus_mode;
    sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 3,  "%s bus_mode 0x%x", __func__, bus_mode);

    return (EOK);
}


static int mtk_msdc_sdmmc_bus_width(sdio_hc_t *hc, int width)
{
    uint32_t            val;
    mtk_msdc_sdmmc_t    *sdmmc = (mtk_msdc_sdmmc_t *)hc->cs_hdl;
    uintptr_t           base = sdmmc->base;

    sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 3,  "%s width 0x%x", __func__, width);

#ifdef MTK_MSDC_DEBUG
    sdio_slogf( _SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0, "%s: width %d", __FUNCTION__, width );
#endif

    val = in32(base + SDC_CFG);
    val &= ~SDC_CFG_BUSWIDTH;

    switch (width) {
        case BUS_WIDTH_1:
            val |= (MSDC_BUS_1BITS << 16);
            break;
        case BUS_WIDTH_4:
            val |= (MSDC_BUS_4BITS << 16);
            break;
        case BUS_WIDTH_8:
            val |= (MSDC_BUS_8BITS << 16);
            break;
        default:
            sdio_slogf( _SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0, "%s: nonsuported bus width %d", __FUNCTION__, width );
            break;
    }

    out32(base + SDC_CFG, val);
    hc->bus_width = width;
    return (EOK);
}

/* clock control primitives */
static void msdc_set_timeout(sdio_hc_t *hc, uint32_t ns, uint32_t clks)
{
    uint32_t timeout, clk_ns;
    uint32_t mode = 0;
    mtk_msdc_sdmmc_t    *sdmmc =  (mtk_msdc_sdmmc_t *)hc->cs_hdl;
    uintptr_t           base = sdmmc->base;

    sdmmc->timeout_ns = ns;
    sdmmc->timeout_clks = clks;
    if (hc->clk == 0) {
        timeout = 0;
    } else {
        clk_ns  = 1000000000UL / sdmmc->sclk;
        timeout = (ns + clk_ns - 1) / clk_ns + clks;
        /* in 1048576 sclk cycle unit */
        timeout = (timeout + (0x1 << 20) - 1) >> 20;
        sdmmc_get_field(base + MSDC_CFG, MSDC_CFG_CKMOD, &mode);
        /*DDR mode will double the clk cycles for data timeout */
        timeout = mode >= 2 ? timeout * 2 : timeout;
        timeout = timeout > 1 ? timeout - 1 : 0;
        timeout = timeout > 255 ? 255 : timeout;
    }
    sdmmc_set_field(base + SDC_CFG, SDC_CFG_DTOC, timeout);
}

static int mtk_msdc_sdmmc_clk(sdio_hc_t *hc, int  hz)
{
    uint32_t            mode, flags, div, sclk;
    int                 status;
    mtk_msdc_sdmmc_t    *sdmmc =  (mtk_msdc_sdmmc_t *)hc->cs_hdl;
    uintptr_t           base = sdmmc->base;
    sdio_dev_t          *dev = &hc->device;

    sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 3,  "%s:%d hc idx%d base_addr 0x%x: %dhz, hc->timing 0x%x sdmmc->src_clk_freq %d", __func__, __LINE__,
            hc->cfg.idx, (unsigned int)hc->cfg.base_addr[0], hz, hc->timing, sdmmc->src_clk_freq);

#if 0
    if (!hz) {
        sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s:%d set mclk to 0", __func__, __LINE__);
        hc->clk = 0;
        sdmmc_clr_bits(base + MSDC_CFG, MSDC_CFG_CKPDN);
        return EOK;
    }
#endif

    flags = in32(base + MSDC_INTEN);
    sdmmc_clr_bits(base + MSDC_INTEN, flags);
    sdmmc_clr_bits(base + MSDC_CFG, MSDC_CFG_HS400_CK_MODE);

    if(hz > hc->clk_max)
        hz = hc->clk_max;

    /*
     * TIMING_DDR50 should work for both EMMC and SD.
     * EMMC DDR50 seems to have a logic issue.
    */
    if ( (hc->timing == TIMING_HS && (dev->caps & DEV_CAP_DDR50)) || //This is a workaround for EMMC
         hc->timing == TIMING_DDR50 ||
         hc->timing == TIMING_HS400) {

        if (hc->timing == TIMING_HS400)
            mode = 0x3;
        else
            mode = 0x2; /* ddr mode and use divisor */

        if (hz >= (sdmmc->src_clk_freq >> 2)) {
            div = 0; /* mean div = 1/4 in DDR */
            sclk = sdmmc->src_clk_freq >> 2; /* sclk = clk / 4 */
        } else {
            div = (sdmmc->src_clk_freq + ((hz << 2) - 1)) / (hz << 2);
            sclk = (sdmmc->src_clk_freq >> 2) / div;
            div = (div >> 1);
        }

        if (hc->timing == TIMING_HS400 && hz >= (sdmmc->src_clk_freq >> 1)) {
            sdmmc_set_bits(base + MSDC_CFG, MSDC_CFG_HS400_CK_MODE);
            sclk = sdmmc->src_clk_freq >> 1;
            div = 0; /* div is ignored when HS400_CK_MODE is set */
        }
    } else if (hz >= sdmmc->src_clk_freq) {
        mode = 0x1; /* no divisor */
        div = 0;
        sclk = sdmmc->src_clk_freq;
    } else {
        mode = 0x0; /* use divisor */
        if (hz >= (sdmmc->src_clk_freq >> 1)) {
            div = 0; /* mean div = 1/2 */
            sclk = sdmmc->src_clk_freq >> 1; /* sclk = clk / 2 */
        } else {
            div = (sdmmc->src_clk_freq + ((hz << 2) - 1)) / (hz << 2);
            sclk = (sdmmc->src_clk_freq >> 2) / div;
        }
    }

    sdmmc_set_field(base + MSDC_CFG, MSDC_CFG_CKMOD | MSDC_CFG_CKDIV, (mode << 12) | div);
    sdmmc_set_bits(base + MSDC_CFG, MSDC_CFG_CKPDN);

    if( EOK != (status = mtk_msdc_waitmask(hc, MSDC_CFG, MSDC_CFG_CKSTB, MSDC_CFG_CKSTB, 20000))) //20ms
    {
        sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0,  "%s:%d CFG_CKSTB Timeout", __func__,__LINE__);
        return (ETIMEDOUT);
    }

    sdmmc->sclk = sclk;
    sdmmc->mclk = hz;
   /* need because clk changed. */
    msdc_set_timeout(hc, sdmmc->timeout_ns, sdmmc->timeout_clks);
    sdmmc_set_bits(base + MSDC_INTEN, flags);

    /*
     * mmc_select_hs400() will drop to 50Mhz and High speed mode,
     * tune result of hs200/200Mhz is not suitable for 50Mhz
     */
    if (sdmmc->sclk <= 52000000) {
        out32(base + MSDC_IOCON, sdmmc->def_tune_para.iocon);
        out32(base + MSDC_PAD_TUNE0, sdmmc->def_tune_para.pad_tune);
    } else {
        out32(base + MSDC_IOCON, sdmmc->saved_tune_para.iocon);
        out32(base + MSDC_PAD_TUNE0, sdmmc->saved_tune_para.pad_tune);
        out32(base + PAD_CMD_TUNE, sdmmc->saved_tune_para.pad_cmd_tune);
    }

    if (hc->timing == TIMING_HS400)
        sdmmc_set_field(base + PAD_CMD_TUNE, MSDC_PAD_TUNE_CMDRRDLY, sdmmc->hs400_cmd_int_delay);

    sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s:%d sclk: %d, timing: %d, mode: %d, div %d%s", __func__, __LINE__, sdmmc->sclk, hc->timing, mode, div,
            (in32(base + MSDC_CFG) & MSDC_CFG_HS400_CK_MODE) ? " (HS400_CK_MODE)" : "");

    hc->clk = sclk;

    return EOK;
}


int mtk_msdc_sdmmc_signal_voltage(sdio_hc_t *hc, int signal_voltage)
{
    mtk_msdc_sdmmc_t *sdmmc = (mtk_msdc_sdmmc_t *)hc->cs_hdl;
    uintptr_t base = sdmmc->base;

	if(SIGNAL_VOLTAGE_3_3 == signal_voltage) mtk_bs_signal_voltage(hc, signal_voltage);
	else if (SIGNAL_VOLTAGE_1_8 == signal_voltage && !(hc->flags & HC_FLAG_DEV_MMC))
	{
		if(EOK == mtk_msdc_waitmask(hc, SDC_STS, SDC_STS_SDCBUSY, 0, 20000)) //20ms
		{
			sdmmc_set_bits(base + MSDC_CFG, MSDC_CFG_SCLKSTOPSEL);
			sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s: SDC_STS 0x%x MSDC_CFG 0x%x", __func__, in32(base + SDC_STS), in32(base + MSDC_CFG));
			mtk_bs_signal_voltage(hc, signal_voltage);
		} else {
			sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s: SDC_STS 0x%x timeout", __func__, in32(base + SDC_STS));
		}
#if 0 //could not detect the low status
		if(EOK == mtk_msdc_waitmask(hc, MSDC_PS, 0x010f0000, 0, 20000)) //20ms
		{
			sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s: MSDC_PS 0x%x", __func__, in32(base + MSDC_PS));
		} else {
			sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s: MSDC_PS 0x%x timeout", __func__, in32(base + MSDC_PS));
			//POWER CYCLE or HW reset
		}
#endif
		delay(10); //minmum 5ms to keep CLK low
		sdmmc_set_field(base + MSDC_CFG, MSDC_CFG_BV18SDT, 1);

		if(EOK == mtk_msdc_waitmask(hc, MSDC_PS, 0x010f0000, 0x010f0000, 20000)) //20ms
		{
			sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s: MSDC_PS 0x%x", __func__, in32(base + MSDC_PS));
		} else {
			sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s: MSDC_PS 0x%x timeout", __func__, in32(base + MSDC_PS));
		}
	}
    /*
     * It is expected that eMMC operates in 1.8 V by default.
     * In this case we skip code to switch to 1.8 V
     */
    if ((hc->caps & HC_CAP_SLOT_TYPE_EMBEDDED) && (hc->flags & HC_FLAG_DEV_MMC)) {
        if (signal_voltage == SIGNAL_VOLTAGE_1_8) {
			mtk_bs_signal_voltage(hc, signal_voltage);
            hc->signal_voltage = signal_voltage;
            sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 3, "%s: eMMC 1.8v signal_voltage %d", __func__, signal_voltage);
            return (EOK);
        }
    }

    //may needs to set pin for uhs
    hc->signal_voltage = signal_voltage;
	sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 3, "%s: signal_voltage %d", __func__, signal_voltage);
    return (EOK);
}

int msdc_prepare_hs400_tuning(sdio_hc_t *hc)
{
    mtk_msdc_sdmmc_t *sdmmc = (mtk_msdc_sdmmc_t *)hc->cs_hdl;
    uintptr_t base = sdmmc->base;

    out32(base + PAD_DS_TUNE, 0x14015);//sdmmc->hs400_ds_delay, got from Linux device tree

    /* hs400 mode must set it to 0 */
    sdmmc_clr_bits(base + MSDC_PATCH_BIT2, MSDC_PATCH_BIT2_CFGCRCSTS);

    /* to improve read performance, set outstanding to 2 */
    sdmmc_set_field(base + EMMC50_CFG3, EMMC50_CFG3_OUTS_WR, 2);

    return 0;
}

static int mtk_msdc_sdmmc_timing(sdio_hc_t *hc, int timing)
{
    sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 3,  "%s: timing 0x%x", __func__, timing);

    hc->timing = timing;
    /* TIMING_HS400: EMMC HS400 needs to set clock after finishing HS200 tuning */
    if (hc->timing == TIMING_HS400 || hc->timing == TIMING_LS)
        mtk_msdc_sdmmc_clk(hc, hc->clk);

    return (EOK);
}


static uint32_t test_delay_bit(uint32_t delay, uint32_t bit)
{
    bit %= PAD_DELAY_MAX;
    return delay & (1 << bit);
}

static int get_delay_len(uint32_t  delay, uint32_t  start_bit)
{
    int i;

    for (i = 0; i < (PAD_DELAY_MAX - start_bit); i++) {
        if (test_delay_bit(delay, start_bit + i) == 0)
            return i;
    }
    return PAD_DELAY_MAX - start_bit;
}

static struct msdc_delay_phase get_best_delay(sdio_hc_t *hc, uint32_t delay)
{
    int start = 0, len = 0;
    int start_final = 0, len_final = 0;
    uint8_t final_phase = 0xff;
    struct msdc_delay_phase delay_phase = { 0, };

    if (delay == 0) {
        sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0, "%s: phase error: [map:%x]", __func__, delay);
        delay_phase.final_phase = final_phase;
        return delay_phase;
    }

    while (start < PAD_DELAY_MAX) {
        len = get_delay_len(delay, start);
        if (len_final < len) {
            start_final = start;
            len_final = len;
        }
        start += len ? len : 1;
        if (len >= 12 && start_final < 4)
            break;
    }

    /* The rule is that to find the smallest delay cell */
    if (start_final == 0)
        final_phase = (start_final + len_final / 3) % PAD_DELAY_MAX;
    else
        final_phase = (start_final + len_final / 2) % PAD_DELAY_MAX;
    sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 3, "%s: phase: [map:%x] [maxlen:%d] [final:%d]",__func__,  delay, len_final, final_phase);

    delay_phase.maxlen = len_final;
    delay_phase.start = start_final;
    delay_phase.final_phase = final_phase;
    return delay_phase;
}

int mmc_send_tuning(sdio_hc_t *hc, uint32_t op)
{
    struct sdio_cmd cmd;
    int         tlen = (hc->bus_width == BUS_WIDTH_8) ? 128 : 64;
    int         status;
    sdio_sge_t  sge;
    uint8_t     buf[128];

    sge.sg_count = tlen;
    sge.sg_address = (paddr_t)&buf;

    memset((void *)&cmd, 0, sizeof(struct sdio_cmd));
    sdio_setup_cmd(&cmd, SCF_CTYPE_ADTC | SCF_RSP_R1, op, 0);

    /* Seems if not read the data out of the buffer, it will be some problems */
    sdio_setup_cmd_io(&cmd, SCF_DIR_IN, 1, tlen, &sge, 1, NULL);
    status = sdio_issue_cmd(&hc->device, &cmd, SDIO_TIME_DEFAULT);

    /* Command error */
    if (cmd.status > CS_CMD_CMP){
        status = EIO;
        sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0, "%s error cmd.status %d", __func__, cmd.status);
    }
    return status;
}

static int msdc_tune_response(sdio_hc_t *hc, uint32_t opcode)
{
    uint32_t rise_delay = 0, fall_delay = 0;
    struct msdc_delay_phase final_rise_delay, final_fall_delay = { 0,};
    struct msdc_delay_phase internal_delay_phase;
    uint8_t final_delay, final_maxlen;
    uint32_t internal_delay = 0;
    int status = 0;
    int i, j;
    mtk_msdc_sdmmc_t *sdmmc = (mtk_msdc_sdmmc_t *)hc->cs_hdl;
    uintptr_t base = sdmmc->base;
    if (hc->timing == TIMING_HS200 || hc->timing == TIMING_SDR104)
        sdmmc_set_field(base + MSDC_PAD_TUNE0, MSDC_PAD_TUNE_CMDRRDLY, sdmmc->hs200_cmd_int_delay);

    sdmmc_clr_bits(base + MSDC_IOCON, MSDC_IOCON_RSPL);
    for (i = 0 ; i < PAD_DELAY_MAX; i++) {
        sdmmc_set_field(base + MSDC_PAD_TUNE0, MSDC_PAD_TUNE_CMDRDLY, i);
        /*
         * Using the same parameters, it may sometimes pass the test,
         * but sometimes it may fail. To make sure the parameters are
         * more stable, we test each set of parameters 3 times.
         */
        for (j = 0; j < 3; j++) {
            if (EOK == (status = mmc_send_tuning(hc, opcode))) {
                rise_delay |= (1 << i);
            } else {
                rise_delay &= ~(1 << i);
                break;
            }
        }
    }
    final_rise_delay = get_best_delay(hc, rise_delay);
    /* if rising edge has enough margin, then do not scan falling edge */
    if (final_rise_delay.maxlen >= 12 && final_rise_delay.start < 4)
        goto skip_fall;

    sdmmc_set_bits(base + MSDC_IOCON, MSDC_IOCON_RSPL);
    for (i = 0; i < PAD_DELAY_MAX; i++) {
        sdmmc_set_field(base + MSDC_PAD_TUNE0, MSDC_PAD_TUNE_CMDRDLY, i);
        /*
         * Using the same parameters, it may sometimes pass the test,
         * but sometimes it may fail. To make sure the parameters are
         * more stable, we test each set of parameters 3 times.
         */
        for (j = 0; j < 3; j++) {
            if (EOK == (status = mmc_send_tuning(hc, opcode))) {
                fall_delay |= (1 << i);
            } else {
                fall_delay &= ~(1 << i);
                break;
            }
        }
    }
    final_fall_delay = get_best_delay(hc, fall_delay);
skip_fall:
    final_maxlen = max(final_rise_delay.maxlen, final_fall_delay.maxlen);
    if (final_fall_delay.maxlen >= 12 && final_fall_delay.start < 4)
        final_maxlen = final_fall_delay.maxlen;
    if (final_maxlen == final_rise_delay.maxlen) {
        sdmmc_clr_bits(base + MSDC_IOCON, MSDC_IOCON_RSPL);
        sdmmc_set_field(base + MSDC_PAD_TUNE0, MSDC_PAD_TUNE_CMDRDLY, final_rise_delay.final_phase);
        final_delay = final_rise_delay.final_phase;
    } else {
        sdmmc_set_bits(base + MSDC_IOCON, MSDC_IOCON_RSPL);
        sdmmc_set_field(base + MSDC_PAD_TUNE0, MSDC_PAD_TUNE_CMDRDLY, final_fall_delay.final_phase);
        final_delay = final_fall_delay.final_phase;
    }
    if (sdmmc->hs200_cmd_int_delay)
        goto skip_internal;

    for (i = 0; i < PAD_DELAY_MAX; i++) {
        sdmmc_set_field(base + MSDC_PAD_TUNE0, MSDC_PAD_TUNE_CMDRRDLY, i);
        status = mmc_send_tuning(hc, opcode);
        if (!status)
            internal_delay |= (1 << i);
    }
    sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 3, "%s: Final internal delay: 0x%x", __func__, internal_delay);
    internal_delay_phase = get_best_delay(hc, internal_delay);
    sdmmc_set_field(base + MSDC_PAD_TUNE0, MSDC_PAD_TUNE_CMDRRDLY,  internal_delay_phase.final_phase);
skip_internal:
    sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 3, "%s: Final cmd pad delay: %x", __func__, final_delay);
    return final_delay == 0xff ? EIO : 0;
}

#if 0
static int hs400_tune_response(sdio_hc_t *hc, uint32_t opcode)
{
    uint32_t cmd_delay = 0;
    struct msdc_delay_phase final_cmd_delay = { 0,};
    uint8_t final_delay;
    int status;
    int i, j;

    mtk_msdc_sdmmc_t *sdmmc = (mtk_msdc_sdmmc_t *)hc->cs_hdl;
    uintptr_t base = sdmmc->base;

    msdc_prepare_hs400_tuning(hc);

    /* select EMMC50 PAD CMD tune */
    sdmmc_set_bits(base + PAD_CMD_TUNE, 1);//BIT(0)
    if (hc->timing == TIMING_HS200 || hc->timing == TIMING_SDR104)
        sdmmc_set_field(base + MSDC_PAD_TUNE0, MSDC_PAD_TUNE_CMDRRDLY, sdmmc->hs200_cmd_int_delay);

    if (sdmmc->hs400_cmd_resp_sel_rising)
        sdmmc_clr_bits(base + MSDC_IOCON, MSDC_IOCON_RSPL);
    else
        sdmmc_set_bits(base + MSDC_IOCON, MSDC_IOCON_RSPL);
    for (i = 0 ; i < PAD_DELAY_MAX; i++) {
        sdmmc_set_field(base + PAD_CMD_TUNE, PAD_CMD_TUNE_RX_DLY3, i);
        /*
         * Using the same parameters, it may sometimes pass the test,
         * but sometimes it may fail. To make sure the parameters are
         * more stable, we test each set of parameters 3 times.
         */
        for (j = 0; j < 3; j++) {
            if (EOK == (status = mmc_send_tuning(hc, opcode))) {
                cmd_delay |= (1 << i);
            } else {
                cmd_delay &= ~(1 << i);
                break;
            }
        }
    }
    final_cmd_delay = get_best_delay(hc, cmd_delay);
    sdmmc_set_field(base + PAD_CMD_TUNE, PAD_CMD_TUNE_RX_DLY3, final_cmd_delay.final_phase);
    final_delay = final_cmd_delay.final_phase;

    sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 3, "%s: Final cmd pad delay: %x", __func__, final_delay);
    return final_delay == 0xff ? EIO : 0;
}
#endif

static int msdc_tune_data(sdio_hc_t *hc, uint32_t opcode)
{
    uint32_t rise_delay = 0, fall_delay = 0;
    struct msdc_delay_phase final_rise_delay, final_fall_delay = { 0,};
    uint8_t final_delay, final_maxlen;
    int i, ret;
    mtk_msdc_sdmmc_t *sdmmc = (mtk_msdc_sdmmc_t *)hc->cs_hdl;
    uintptr_t base = sdmmc->base;

    sdmmc_clr_bits(base + MSDC_IOCON, MSDC_IOCON_DSPL);
    sdmmc_clr_bits(base + MSDC_IOCON, MSDC_IOCON_W_DSPL);
    for (i = 0 ; i < PAD_DELAY_MAX; i++) {
        sdmmc_set_field(base + MSDC_PAD_TUNE0, MSDC_PAD_TUNE_DATRRDLY, i);
        ret = mmc_send_tuning(hc, opcode);
        if (!ret)
            rise_delay |= (1 << i);
        else {
            /* in this case, need retune response */
            ret = msdc_tune_response(hc, opcode);
            if (ret) break;
        }
    }
    final_rise_delay = get_best_delay(hc, rise_delay);
    /* if rising edge has enough margin, then do not scan falling edge */
    if (final_rise_delay.maxlen >= 12 || (final_rise_delay.start == 0 && final_rise_delay.maxlen >= 4))
        goto skip_fall;

    sdmmc_set_bits(base + MSDC_IOCON, MSDC_IOCON_DSPL);
    sdmmc_set_bits(base + MSDC_IOCON, MSDC_IOCON_W_DSPL);
    for (i = 0; i < PAD_DELAY_MAX; i++) {
        sdmmc_set_field(base + MSDC_PAD_TUNE0, MSDC_PAD_TUNE_DATRRDLY, i);
        ret = mmc_send_tuning(hc, opcode);
        if (!ret)
            fall_delay |= (1 << i);
        else {
            /* in this case, need retune response */
            ret = msdc_tune_response(hc, opcode);
            if (ret) break;
        }
    }
    final_fall_delay = get_best_delay(hc, fall_delay);

skip_fall:
    final_maxlen = max(final_rise_delay.maxlen, final_fall_delay.maxlen);
    if (final_maxlen == final_rise_delay.maxlen) {
        sdmmc_clr_bits(base + MSDC_IOCON, MSDC_IOCON_DSPL);
        sdmmc_clr_bits(base + MSDC_IOCON, MSDC_IOCON_W_DSPL);
        sdmmc_set_field(base + MSDC_PAD_TUNE0, MSDC_PAD_TUNE_DATRRDLY, final_rise_delay.final_phase);
        final_delay = final_rise_delay.final_phase;
    } else {
        sdmmc_set_bits(base + MSDC_IOCON, MSDC_IOCON_DSPL);
        sdmmc_set_bits(base + MSDC_IOCON, MSDC_IOCON_W_DSPL);
        sdmmc_set_field(base + MSDC_PAD_TUNE0, MSDC_PAD_TUNE_DATRRDLY, final_fall_delay.final_phase);
        final_delay = final_fall_delay.final_phase;
    }

    sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 3, "%s: Final data pad delay: %x", __func__, final_delay);
    return final_delay == 0xff ? EIO : 0;
}

static int mtk_msdc_sdmmc_tune(sdio_hc_t *hc, int op)
{
    int ret;
    mtk_msdc_sdmmc_t *sdmmc = (mtk_msdc_sdmmc_t *)hc->cs_hdl;
    uintptr_t        base = sdmmc->base;
    sdio_dev_t       *dev = &hc->device;

    sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 3,  "%s:%d op 0x%x", __func__, __LINE__, op);
    /* return if not HS200, HS400, or SDR104, and not SDR50 that requires tuning */
    if (hc->timing != TIMING_SDR104 && hc->timing != TIMING_HS200 && hc->timing != TIMING_HS400 && (hc->timing != TIMING_SDR50 || 1/*!(sdmmc->flags & SF_TUNE_SDR50)*/))
        return (EOK);

    /* In case of HS400, it  needs to do HS200 tuning */
    if (hc->timing == TIMING_HS200 && (dev->caps & DEV_CAP_HS400))
        msdc_prepare_hs400_tuning(hc);

    ret = msdc_tune_response(hc, op);
    if (ret == EIO) {
        sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0, "%s: Tune response fail!", __func__);
        return ret;
    }

    ret = msdc_tune_data(hc, op);
    if (ret == EIO)
        sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0, "%s: Tune data fail!", __func__);

    sdmmc->saved_tune_para.iocon = in32(base + MSDC_IOCON);
    sdmmc->saved_tune_para.pad_tune = in32(base + MSDC_PAD_TUNE0);
    sdmmc->saved_tune_para.pad_cmd_tune = in32(base + PAD_CMD_TUNE);
    return ret;
}



static int mtk_msdc_sdmmc_dma_init(sdio_hc_t *hc)
{
    sdio_hc_cfg_t *cfg;

    cfg   = &hc->cfg;

    cfg->sg_max = 1;

    return (EOK);
}

int mtk_msdc_sdmmc_dma_dinit(sdio_hc_t *hc)
{
    return( EOK );
}

int mtk_msdc_sdmmc_dinit(sdio_hc_t *hc)
{
    mtk_msdc_sdmmc_t *sdmmc = (mtk_msdc_sdmmc_t *)hc->cs_hdl;
    uintptr_t base = sdmmc->base;

    sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 1,  "%s:%d base 0x%p", __func__, __LINE__, (void*)base);

    if (!hc || !hc->cs_hdl)
        return (EOK);

    if (base) {
        mtk_msdc_sdmmc_reset(hc);
        mtk_msdc_sdmmc_pwr(hc, 0);
        out32(base + MSDC_INTEN, 0);

        if(sdmmc->dma) {
            if(sdmmc->dma->bd)
                sdio_free(sdmmc->dma->bd, sizeof( struct mt_bdma_desc ) * MAX_BD_NUM);
            if(sdmmc->dma->gpd)
                sdio_free(sdmmc->dma->gpd, sizeof( struct mt_gpdma_desc ) * 2);
            free(sdmmc->dma);
            sdmmc->dma = NULL;
        }

        if (hc->hc_iid != -1)
            InterruptDetach(hc->hc_iid);

        munmap_device_io(base, sdmmc->basesize);
    }
    free(sdmmc);
    hc->cs_hdl = NULL;

    return (EOK);
}


static sdio_hc_entry_t mtk_msdc_sdmmc_entry = {
    16,
    mtk_msdc_sdmmc_dinit, NULL,
    mtk_msdc_sdmmc_cmd, mtk_msdc_sdmmc_abort,
    mtk_msdc_sdmmc_event, mtk_msdc_sdmmc_cd, mtk_msdc_sdmmc_pwr,
    mtk_msdc_sdmmc_clk, mtk_msdc_sdmmc_bus_mode,
    mtk_msdc_sdmmc_bus_width, mtk_msdc_sdmmc_timing,
    mtk_msdc_sdmmc_signal_voltage, NULL, NULL, mtk_msdc_sdmmc_tune, NULL,
};

/* init gpd and bd, should be called before hw init */
static void msdc_init_gpd_bd(sdio_hc_t *hc)
{
    mtk_msdc_sdmmc_t *sdmmc = (mtk_msdc_sdmmc_t *)hc->cs_hdl;
    struct msdc_dma *dma = sdmmc->dma;
    struct mt_gpdma_desc *gpd = dma->gpd;
    struct mt_bdma_desc *bd = dma->bd;
    int i;

    memset(gpd, 0, sizeof(struct mt_gpdma_desc) * 2);

    gpd->flags  = GPDMA_DESC_BDP; /* hwo, cs, bd pointer */
    gpd->flags |= (uint32_t)(dma->bd_addr >> 32) << 28; /* high 4-bits of physical address */
    gpd->ptr = (uint32_t)dma->bd_addr; /* low 32-bits of physical address */
    /* gpd->next is must set for desc DMA
     * That's why must alloc 2 gpd structure.
     */
    gpd->flags |= (uint32_t)((dma->gpd_addr + sizeof(struct mt_gpdma_desc)) >> 32) << 24;
    gpd->next = (uint32_t)(dma->gpd_addr + sizeof(struct mt_gpdma_desc));
    memset(bd, 0, sizeof(struct mt_bdma_desc) * MAX_BD_NUM);
    for (i = 0; i < (MAX_BD_NUM - 1); i++) {
        bd[i].flags |= (uint32_t)((dma->bd_addr + sizeof(*bd) * (i + 1)) >> 32) << 24;
        bd[i].next = (uint32_t)(dma->bd_addr + sizeof(*bd) * (i + 1));
    }
}

static void msdc_init_hw(sdio_hc_t *hc)
{
    mtk_msdc_sdmmc_t *sdmmc = (mtk_msdc_sdmmc_t *)hc->cs_hdl;
    uintptr_t base = sdmmc->base;

    sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s: init hardware", __func__);

#if 0
    if((hc->caps & HC_CAP_SLOT_TYPE_EMBEDDED) != 0)
    {
        /* Hardware reset eMMC chip */
        msdc_hw_reset(hc);
    }
#endif

    /* Configure to MMC/SD mode */
    sdmmc_set_bits(base + MSDC_CFG, MSDC_CFG_MODE);//bit0 1:sd/mmc mode 
    sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s: MSDC_CFG %x", __func__, in32(base + MSDC_CFG));

    /* Reset */
    mtk_msdc_sdmmc_reset(hc);

//  mtk_msdc_reg_dump(hc, __func__, __LINE__, 1);

    /* Disable card detection */
    sdmmc_clr_bits(base + MSDC_PS, MSDC_PS_CDEN);//no this bit, it seems to be related with CD

    /* Disable and clear all interrupts */
    out32(base + MSDC_INTEN, 0);
    out32(base + MSDC_INT, in32(base + MSDC_INT));

    out32(base + MSDC_PAD_TUNE0, 0);
    out32(base + MSDC_IOCON, 0);

    sdmmc_set_field(base + MSDC_IOCON, MSDC_IOCON_DDLSEL, 0);  //lxd: may try 1 for falling edge
    out32(base + MSDC_PATCH_BIT, 0x403c0046);
    //sdmmc_set_field(base + MSDC_PATCH_BIT, MSDC_CKGEN_MSDC_DLY_SEL, 1);
    out32(base + MSDC_PATCH_BIT1, 0xffff4089);

    sdmmc_set_field(base + MSDC_PATCH_BIT1, MSDC_PATCH_BIT1_STOP_DLY_SEL, 3); /*stop_clk_fix*/
    sdmmc_clr_bits(base + MSDC_PATCH_BIT1, MSDC_PATCH_BIT1_BUSY_CHECK_SEL); /*busy_check*/

    sdmmc_set_bits(base + EMMC50_CFG0, EMMC50_CFG_CFCSTS_SEL);

#if 1 /*async_fifo*/
    sdmmc_set_field(base + MSDC_PATCH_BIT2, MSDC_PATCH_BIT2_RESPWAIT, 3);
    sdmmc_set_bits(base + SDC_ADV_CFG0, SDC_ADV_CFG0_RX_ENHANCE_EN); /*enhance_rx*/
    sdmmc_clr_bits(base + MSDC_PATCH_BIT2, MSDC_PATCH_BIT2_CFGRESP);
    sdmmc_set_bits(base + MSDC_PATCH_BIT2, MSDC_PATCH_BIT2_CFGCRCSTS);
#endif

#if 1 /*data_tune*/
    sdmmc_set_bits(base + MSDC_PAD_TUNE0, MSDC_PAD_TUNE_RD_SEL|MSDC_PAD_TUNE_CMD_SEL);
    sdmmc_set_field(base + MSDC_PATCH_BIT, MSDC_INT_DAT_LATCH_CK_SEL, 0/*host->latch_ck*/);
#else
    sdmmc_set_bits(base + MSDC_PAD_TUNE0, MSDC_PAD_TUNE_RXDLYSEL);
#endif

    /* Configure to enable SDIO mode.
     * it's must otherwise sdio cmd5 failed
     */
    sdmmc_set_bits(base + SDC_CFG, SDC_CFG_SDIO);

    /* disable detect SDIO device interrupt function */
    sdmmc_clr_bits(base + SDC_CFG, SDC_CFG_SDIOIDE);

    /* Configure to default data timeout */
    sdmmc_set_field(base + SDC_CFG, SDC_CFG_DTOC, 3);

#if 1 /*stop_clk_fix*/
    sdmmc_clr_bits(base + SDC_FIFO_CFG, SDC_FIFO_CFG_WRVALIDSEL);
    sdmmc_clr_bits(base + SDC_FIFO_CFG, SDC_FIFO_CFG_RDVALIDSEL);
#endif

    sdmmc->def_tune_para.iocon = in32(base + MSDC_IOCON);
    sdmmc->def_tune_para.pad_tune = in32(base + MSDC_PAD_TUNE0);

    sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s: init hardware done!", __func__);
//  mtk_msdc_reg_dump(hc, __func__, __LINE__, 1);
}


int mtk_msdc_sdmmc_init(sdio_hc_t *hc)
{
    sdio_hc_cfg_t   *cfg;
    struct sigevent event;
    mtk_msdc_sdmmc_t *sdmmc = (mtk_msdc_sdmmc_t *)hc->cs_hdl;
    hc->hc_iid  = -1;
    cfg         = &hc->cfg;
    uintptr_t base = sdmmc->base;

    sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s:%d hc idx%d base_addr 0x%x", __func__, __LINE__, cfg->idx, (unsigned int)cfg->base_addr[0]);
    memcpy(&hc->entry, &mtk_msdc_sdmmc_entry, sizeof(sdio_hc_entry_t));

    if( !cfg->base_addr[0] ) return( ENODEV );

    if( !cfg->base_addr_size[0] )  cfg->base_addr_size[0] = 0x700;//MTK_MSDC_SIZE;//0x700

    if ((sdmmc = hc->cs_hdl = calloc(1, sizeof(mtk_msdc_sdmmc_t))) == NULL)  return (ENOMEM);


    if( ( base = sdmmc->base = (uintptr_t)mmap_device_memory( NULL, cfg->base_addr_size[0],
            PROT_READ | PROT_WRITE | PROT_NOCACHE, MAP_SHARED, cfg->base_addr[0] ) ) == (uintptr_t)MAP_FAILED ) {
        sdio_slogf( _SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0, "%s: base mmap_device_memory (0x%" PRIx64 "x) %s", __FUNCTION__, cfg->base_addr[0], strerror( errno ) );
        mtk_msdc_sdmmc_dinit( hc );
        return( errno );
    }

    sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s:%d base 0x%x sdmmc->clock %d", __func__, __LINE__, (unsigned int)base, sdmmc->clock);
    memcpy(&hc->entry, &mtk_msdc_sdmmc_entry, sizeof(sdio_hc_entry_t));

//  mtk_msdc_reg_dump(hc, __func__, __LINE__, 1);

    hc->caps |= HC_CAP_BSY | HC_CAP_BW4 | HC_CAP_BW8 /*| HC_CAP_ERASE*/;
    hc->caps |= HC_CAP_ACMD23;
    //hc->caps |= HC_CAP_ACMD12;
    hc->caps |= HC_CAP_DMA | HC_CAP_DMA64;
    hc->caps |= HC_CAP_HS;
    if((hc->caps & HC_CAP_SLOT_TYPE_EMBEDDED) == 0) {
        hc->caps |= HC_CAP_SDR12 | HC_CAP_SDR25;
        hc->caps |= HC_CAP_DDR50;
        hc->caps |= HC_CAP_SDR50;
        hc->caps |= HC_CAP_SDR104; /* requires tuning */
    } else {
        hc->caps |= HC_CAP_DDR50;
        hc->caps |= HC_CAP_HS200;  /* requires tuning */
        hc->caps |= HC_CAP_HS400;  /* requires tuning */
    }

    hc->ocr  |= OCR_VDD_32_33 | OCR_VDD_33_34;
    hc->caps |= HC_CAP_SV_3_3V;

    sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s:%d hc->caps 0x%lx cfg->caps 0x%lx masked hc->caps 0x%lx ", __func__, __LINE__, hc->caps, cfg->caps, hc->caps & cfg->caps);

    hc->caps &= cfg->caps;      /* reconcile command line options */

    if (mtk_msdc_sdmmc_dma_init(hc) != EOK) {
        sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 0, "%s:%d DMA init failed! ", __func__, __LINE__);
        mtk_msdc_sdmmc_dinit(hc);
        return (ENODEV);
    }

    /* Set host parameters to mmc */

    /* TODO this should come from startup via hwi. For now assume embedded is 400MHz and rest are 200MHz. */
    if((hc->caps & HC_CAP_SLOT_TYPE_EMBEDDED) != 0)
        sdmmc->src_clk_freq = 400 * 1000 * 1000;
    else
        sdmmc->src_clk_freq = 200 * 1000 * 1000;

    if (cfg->clk)
        hc->clk_max = cfg->clk;
    else
        hc->clk_max = sdmmc->src_clk_freq;
//        hc->clk_max = 52 * 1000 * 1000;
    if(hc->clk_max > 200 * 1000 * 1000)
        hc->clk_max = 200 * 1000 * 1000;

#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
    hc->clk_min = DIV_ROUND_UP(sdmmc->src_clk_freq, (4 * 4095));

    sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s:%d cfg->clk %d src_clk_freq %d clk_max %d clk_min %d", __func__, __LINE__,
           cfg->clk, sdmmc->src_clk_freq, hc->clk_max, hc->clk_min);

    /* MMC core transfer sizes tunable parameters */
//    mmc->max_segs = MAX_BD_NUM;
//    mmc->max_seg_size = BDMA_DESC_BUFLEN;
//    mmc->max_blk_size = 2048;
//    mmc->max_req_size = 512 * 1024;
//    mmc->max_blk_count = mmc->max_req_size / 512;

    sdmmc->timeout_clks = 3 * 1048576;
    sdmmc->timeout_ns   = 0;

    if( ( hc->caps & HC_CAP_DMA ) ) {
        hc->cfg.sg_max  = MAX_BD_NUM;
        if ((sdmmc->dma = calloc(1, sizeof(struct msdc_dma))) == NULL)
        {
            mtk_msdc_sdmmc_dinit( hc );
            return (ENOMEM);
        }
        if( ( sdmmc->dma->gpd = sdio_alloc(sizeof( struct mt_gpdma_desc ) * 2)) == NULL)
        {
            sdio_slogf( _SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0,  "%s: dma.gpd sdio_alloc %s", __FUNCTION__, strerror( errno ) );
            mtk_msdc_sdmmc_dinit( hc );
            return( errno );
        }
        sdmmc->dma->gpd_addr = sdio_vtop( sdmmc->dma->gpd );
        if( ( sdmmc->dma->bd = sdio_alloc(sizeof( struct mt_bdma_desc ) * MAX_BD_NUM) ) == NULL )
        {
            sdio_slogf( _SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0,  "%s: dma.bd sdio_alloc %s", __FUNCTION__, strerror( errno ) );
            mtk_msdc_sdmmc_dinit( hc );
            return( errno );
        }
        sdmmc->dma->bd_addr  = sdio_vtop( sdmmc->dma->bd );
        //sdmmc->flags     |= SF_USE_ADMA;

        sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 3, "%s:%d ==>msdc_init_gpd_bd", __func__, __LINE__);
        msdc_init_gpd_bd(hc);
    }

    msdc_init_hw(hc);

    /* we don't want this interrupt at the driver startup */
    SIGEV_PULSE_INIT(&event, hc->hc_coid, SDIO_PRIORITY, HC_EV_INTR, NULL);
    sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 3,  "%s:%d  hc->hc_coid 0x%x, event 0x%p cfg->irq[0] %d ", __func__, __LINE__,  hc->hc_coid, &event, cfg->irq[0]);
    if ((hc->hc_iid = InterruptAttachEvent(cfg->irq[0], &event, _NTO_INTR_FLAGS_TRK_MSK)) == -1) {
        sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 0, "%s:%d error 0x%x", __func__, __LINE__, errno);
        mtk_msdc_sdmmc_dinit(hc);
        return (errno);
    }
    sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 3, "%s:%d EOK", __func__, __LINE__);
    return (EOK);
}
#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/devb/sdmmc/sdiodi/hc/mtk_msdc.c $ $Rev: 852683 $")
#endif
