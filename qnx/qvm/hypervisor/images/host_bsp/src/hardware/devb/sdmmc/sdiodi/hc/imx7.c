/*
 * $QNXLicenseC:
 * Copyright 2017, QNX Software Systems.
 * Copyright 2016, Freescale Semiconductor, Inc.
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

#ifdef SDIO_HC_IMX7
#include <arm/mx7x.h>
#include <arm/imx7/imx7_usdhc.h>
#include <imx7.h>

#ifdef IMX_SDHCX_DEBUG
static int imx_sdhcx_reg_dump(sdio_hc_t *hc, const char *func, int line)
{
    imx_sdhcx_hc_t     *sdhc;
    uintptr_t           base;

    sdhc = (imx_sdhcx_hc_t *)hc->cs_hdl;
    base = sdhc->base;

    sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s: line %d", func, line);
    sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s: HCTL_VER %x, CAP %x",
               __FUNCTION__,
               in16(base + IMX7_SDHCX_HCTL_VERSION),
               in32(base + IMX7_SDHCX_CAP));

    sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1,
               "%s: SDMA_ARG2 %x, ADMA %x, BLK %x, ARG %x, CMD %x, RSP10 %x, RSP32 %x, RSP54 %x,  RSP76 %x",
               __FUNCTION__,
               in32(base + IMX7_SDHCX_DS_ADDR),
               in32(base + IMX7_SDHCX_ADMA_ADDRL),
               in32(base + IMX7_SDHCX_BLK_ATT),
               in32(base + IMX7_SDHCX_ARG),
               in32(base + IMX7_SDHCX_CMD),
               in32(base + IMX7_SDHCX_RESP0),
               in32(base + IMX7_SDHCX_RESP1),
               in32(base + IMX7_SDHCX_RESP2),
               in32(base + IMX7_SDHCX_RESP3));

    sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1,
               "%s: PSTATE %x, PCTL %x, SYSCTL %x, IS %x, IE %x, ISE %x, ADMA_ES %x ADMA_ADDRL %x",
               __FUNCTION__,
               in32(base + IMX7_SDHCX_PSTATE),
               in32(base + IMX7_SDHCX_PCTL),
               in32(base + IMX7_SDHCX_SYSCTL),
               in32(base + IMX7_SDHCX_IS),
               in32(base + IMX7_SDHCX_IE),
               in32(base + IMX7_SDHCX_ISE),
               in32(base + IMX7_SDHCX_ADMA_ES),
               in32(base + IMX7_SDHCX_ADMA_ADDRL));

    return (EOK);
}
#endif

/**
 *
 * @param hc Host controller handle
 * @param reg Register
 * @param mask Mask
 * @param val Value to check against
 * @param usec Delay time
 *
 * @return Execution status
 */
static int imx_sdhcx_waitmask(sdio_hc_t *hc, uint32_t reg, uint32_t mask, uint32_t val, uint32_t usec)
{
    imx_sdhcx_hc_t     *sdhc;
    uintptr_t           base;
    uint32_t            cnt;
    int                 stat;
    int                 rval;
    uint32_t            iter;

    sdhc = (imx_sdhcx_hc_t *)hc->cs_hdl;
    base = sdhc->base;
    stat = ETIMEDOUT;
    rval = 0;

    /* Fast poll for 1ms - 1us intervals */
    for (cnt = min(usec, 1000); cnt; cnt--) {
        if (((rval = in32(base + reg)) & mask) == val) {
            stat = EOK;
            break;
        }
        nanospin_ns(1000L);
    }

    if ((usec > 1000) && stat) {
        iter = usec / 1000L;
        for (cnt = iter; cnt; cnt--) {
            if (((rval = in32(base + reg)) & mask) == val) {
                stat = EOK;
                break;
            }
            delay(1);
        }
    }

    return (stat);
}

/**
 * Reset
 * @param hc Host controller handle
 * @param rst Reset mask (type)
 *
 * @return Execution status
 */
static int imx_sdhcx_reset(sdio_hc_t *hc, uint32_t rst)
{
    imx_sdhcx_hc_t     *sdhc;
    uintptr_t           base;
    uint32_t            sctl;
    int                 status;

    sdhc = (imx_sdhcx_hc_t *)hc->cs_hdl;
    base = sdhc->base;

    sctl = in32(base + IMX7_SDHCX_SYSCTL);

    /* Wait up to 100 ms for reset to complete */
    out32(base + IMX7_SDHCX_SYSCTL, sctl | rst);
    status = imx_sdhcx_waitmask(hc, IMX7_SDHCX_SYSCTL, rst, 0, 100000);

    return (status);
}

/**
 * PIO transfer
 * @param hc Host controller handle
 * @param cmd Command
 *
 * @return Execution status
 */
static int imx_sdhcx_pio_xfer(sdio_hc_t *hc, sdio_cmd_t *cmd)
{
    imx_sdhcx_hc_t     *sdhc;
    uintptr_t           base;
    int                 len;
    int                 msk;
    int                 blksz;
    uint8_t             *addr;
    int                 blks = cmd->blks;

#ifdef IMX_SDHCX_DEBUG
    uint64_t cps, cycle1, cycle2 = 0;
    cps = SYSPAGE_ENTRY(qtime)->cycles_per_sec;
    cycle1 = ClockCycles();
#endif

    sdhc = (imx_sdhcx_hc_t *)hc->cs_hdl;
    base = sdhc->base;
    msk = (cmd->flags & SCF_DIR_IN) ? IMX7_SDHCX_PSTATE_BRE : IMX7_SDHCX_PSTATE_BWE;

    /* Multiple blocks */
    while (blks--) {
        blksz = cmd->blksz;
        while (blksz) {
            if (sdio_sg_nxt(hc, &addr, &len, blksz)) {
                break;
            }
            blksz -= len;
            len /= 4;

            /* BRE or BWE is asserted when the available buffer is more than the watermark level */
            while (len) {
                int xfer_len = min(len, IMX7_SDHCX_WATML_WR);
                len -= xfer_len;

                /* Wait until the read or write buffer is ready */
                if (imx_sdhcx_waitmask(hc, IMX7_SDHCX_PSTATE, msk, msk, IMX7_SDHCX_TRANSFER_TIMEOUT)) {
                    sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 1,
                               "pio read: timedout for BREN in present state register");
                    return ETIMEDOUT;
                }

                if ((cmd->flags & SCF_DIR_IN)) {
                        in32s(addr, xfer_len, base + IMX7_SDHCX_DATA);
                } else {
                        out32s(addr, xfer_len, base + IMX7_SDHCX_DATA);
                }
                addr += xfer_len * 4;
            }
        }
    }

#ifdef IMX_SDHCX_DEBUG
    cycle2 = ClockCycles();
    sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1,
               "%s: CMD %d  flags:%x, cmd->blks: %d, cmd->blksz: %d, it took %d us",
               __FUNCTION__, cmd->opcode, cmd->flags, cmd->blks, cmd->blksz,
               (int)((cycle2 - cycle1) * 1000 / (cps/1000)));
#endif

    return (EOK);
}

/**
 * Interrupt event
 * @param hc Host controller handle
 *
 * @return Execution status
 */
static int imx_sdhcx_intr_event(sdio_hc_t *hc)
{
    imx_sdhcx_hc_t     *sdhc;
    sdio_cmd_t          *cmd;
    uint32_t            sts;
    int                 cs;
    int                 idx;
    uint32_t            rsp;
    uint8_t             rbyte;
    uintptr_t           base;

    sdhc = (imx_sdhcx_hc_t *)hc->cs_hdl;
    base = sdhc->base;
    cs = CS_CMD_INPROG;
    sts = in32(base + IMX7_SDHCX_IS);

#ifdef IMX_SDHCX_DEBUG
    sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1,
               "%s: is:%x ie:%x ise:%x\n", __FUNCTION__,
               in32(base + IMX7_SDHCX_IS),
               in32(base + IMX7_SDHCX_IE),
               in32(base + IMX7_SDHCX_ISE));
#endif

    /*
     * Errata TKT070753, Rev 1.0, IMX6DQCE
     * on mx6q, there is low possibility that DATA END interrupt comes earlier than DMA
     * END interrupt which is conflict with standard host controller spec. In this case, read the
     * status register again will workaround this issue.
     */
    if ((sts & IMX7_SDHCX_INTR_TC) && !(sts & IMX7_SDHCX_INTR_DMA)) {
        sts = in32(base + IMX7_SDHCX_IS);
    }
    /* Debounce delay when card inserted or removed. */
    if ((sts & (IMX7_SDHCX_INTR_CINS | IMX7_SDHCX_INTR_CREM))) {
        delay(100);
    }
    /* Clear interrupt events. CINS, CREM flags cleared always since sts variable
     * is loaded before debounce delay. */
    out32(base + IMX7_SDHCX_IS, sts | (IMX7_SDHCX_INTR_CINS | IMX7_SDHCX_INTR_CREM));

    /* Card insertion and card removal events */
    if ((sts & (IMX7_SDHCX_INTR_CINS | IMX7_SDHCX_INTR_CREM))) {
        sdio_hc_event(hc, HC_EV_CD);
    }

    if ((cmd = hc->wspc.cmd) == NULL) {
        return (EOK);
    }

    /* Tuning commands */
    if ((cmd->opcode == SD_SEND_TUNING_BLOCK) || (cmd->opcode == MMC_SEND_TUNING_BLOCK)) {
        /* Though we don't need the data, we need to clear the buffer */
        if (sts & IMX7_SDHCX_INTR_BRR) {
            imx_sdhcx_pio_xfer(hc, cmd);
        }

        if (sts & IMX7_SDHCX_IS_ERRI) {
            cs = CS_CMD_CMP_ERR;
        } else {
            cs = CS_CMD_CMP;
        }

        sdio_cmd_cmplt(hc, cmd, cs);
        return EOK;
    }

    /* Check of errors */
    if (sts & IMX7_SDHCX_IS_ERRI) {
        sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 1,
                   "%s, ERROR in HC, CMD%d, sts: 0x%x:  is 0x%x, ac12 0x%x, IMX7_SDHCX_IS_ERRI: 0x%x",
                   __FUNCTION__, cmd->opcode, sts, in32(base + IMX7_SDHCX_IS),
        in32(base + IMX7_SDHCX_AC12), IMX7_SDHCX_IS_ERRI);
        //
        if (sts & IMX7_SDHCX_INTR_DTO) {
            cs  = CS_DATA_TO_ERR;
        }
        if (sts & IMX7_SDHCX_INTR_DCRC) {
            cs = CS_DATA_CRC_ERR;
        }
        if (sts & IMX7_SDHCX_INTR_DEB) {
            cs  = CS_DATA_END_ERR;
        }
        if (sts & IMX7_SDHCX_INTR_CTO) {
            cs  = CS_CMD_TO_ERR;
        }
        if (sts & IMX7_SDHCX_INTR_CCRC) {
            cs = CS_CMD_CRC_ERR;
        }
        if (sts & IMX7_SDHCX_INTR_CEB) {
            cs  = CS_CMD_END_ERR;
        }
        if (sts & IMX7_SDHCX_INTR_CIE) {
            cs  = CS_CMD_IDX_ERR;
        }
        if (sts & IMX7_SDHCX_INTR_DMAE) {
            cs = CS_DATA_TO_ERR;
        }
        if (sts & IMX7_SDHCX_INTR_ACE) {
            cs  = CS_DATA_TO_ERR;
        }
        if (!cs) {
            cs = CS_CMD_CMP_ERR;
        }
        imx_sdhcx_reset(hc, IMX7_SDHCX_SYSCTL_SRC | IMX7_SDHCX_SYSCTL_SRD);
    } else {
        /* End of command */
        if ((sts & IMX7_SDHCX_INTR_CC)) {
            cs = CS_CMD_CMP;

            // Errata ENGcm03648
            if (cmd->flags & SCF_RSP_BUSY) {
                int timeout = 16 * 1024 * 1024;

                while (!(in32(base + IMX7_SDHCX_PSTATE) & IMX7_SDHCX_PSTATE_DL0SL) && timeout--) {
                    nanospin_ns(100);
                }

                if (timeout <= 0) {
                    cs = CS_CMD_TO_ERR;
                    imx_sdhcx_reset(hc, IMX7_SDHCX_SYSCTL_SRC | IMX7_SDHCX_SYSCTL_SRD);
                    sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 1,
                               "%s: busy done wait timeout for cmd: %d", __func__, cmd->opcode);
                }
            }

            if ((cmd->flags & SCF_RSP_136)) {
                /*
                 * CRC is not included in the response reg, left
                 * shift 8 bits to match the 128 CID/CSD structure
                 */
                for (idx = 0, rbyte = 0; idx < 4; idx++) {
                    rsp = in32(base + IMX7_SDHCX_RESP0 + idx * 4);
                    cmd->rsp[3-idx] = (rsp << 8) + rbyte;
                    rbyte = rsp >> 24;
                }
            } else if ((cmd->flags & SCF_RSP_PRESENT)) {
                cmd->rsp[0] = in32(base + IMX7_SDHCX_RESP0);
            }
        }

        /* End of data transfer */
        if (sts & IMX7_SDHCX_INTR_TC) {
            cs = CS_CMD_CMP;
            cmd->rsp[0] = in32(base + IMX7_SDHCX_RESP0);
        }

        /* Doesn't need to do anything for DMA interrupt */
        if ((sts & IMX7_SDHCX_INTR_DMA)) {
            cs = CS_CMD_CMP;
        }

        if ((sts & (IMX7_SDHCX_INTR_BWR | IMX7_SDHCX_INTR_BRR))) {
            if (EOK == imx_sdhcx_pio_xfer(hc, cmd)) {
                cs = CS_CMD_CMP;
            } else {
                cs = CS_DATA_TO_ERR;
            }
        }
    }

    if (cs != CS_CMD_INPROG) {
        if ((cs == CS_DATA_TO_ERR) || (cs == CS_CMD_TO_ERR)) {
            // Timeout error case, check if card removed
            if (!(hc->entry.cd(hc) & CD_INS)) {
                    cs = CS_CARD_REMOVED;
            }
        }
        sdio_cmd_cmplt(hc, cmd, cs);
    }

    return (EOK);
}

/**
 * uSDHC Interrupt/Board (GPIO) specific event
 * @param hc Host controller handle
 * @param ev Event
 *
 * @return Execution status
 */
static int imx_sdhcx_event(sdio_hc_t *hc, sdio_event_t *ev)
{
    int status;

    switch (ev->code) {
        case HC_EV_INTR:
            status = imx_sdhcx_intr_event(hc);
            InterruptUnmask(hc->cfg.irq[0], hc->hc_iid);
            break;
        default:
            status = bs_event(hc, ev);
            break;
    }

    return (status);
}

/**
 * ADMA configuration
 * @param hc Host controller handle
 * @param cmd Command
 *
 * @return Execution status
 */
static int imx_sdhcx_adma_setup(sdio_hc_t *hc, sdio_cmd_t *cmd)
{
    imx_sdhcx_hc_t             *sdhc;
    imx_sdhcx_adma32_t         *adma;
    sdio_sge_t                  *sgp;
    int                         sgc;
    int                         sgi;
    int                         acnt;
    int                         alen;
    int                         sg_count;
    paddr_t                     paddr;

    sdhc = (imx_sdhcx_hc_t *)hc->cs_hdl;
    adma = (imx_sdhcx_adma32_t *)sdhc->adma;

    sgc = cmd->sgc;
    sgp = cmd->sgl;

    if (!(cmd->flags & SCF_DATA_PHYS)) {
        sdio_vtop_sg(sgp, sdhc->sgl, sgc, cmd->mhdl);
        sgp = sdhc->sgl;
    }

    for (sgi = 0, acnt = 0; sgi < sgc; sgi++, sgp++) {
        paddr = sgp->sg_address;
        sg_count = sgp->sg_count;
        while (sg_count) {
            alen = min(sg_count, IMX7_SDHCX_ADMA2_MAX_XFER);
            adma->attr = IMX7_SDHCX_ADMA2_VALID | IMX7_SDHCX_ADMA2_TRAN;
            adma->addr = paddr;
            adma->len = alen;
            sg_count -= alen;
            paddr += alen;
            adma++;
            if (++acnt > ADMA_DESC_MAX) {
                return (ENOTSUP);
            }
        }
    }

    adma--;
    adma->attr |= IMX7_SDHCX_ADMA2_END;

    out32(sdhc->base + IMX7_SDHCX_ADMA_ADDRL, sdhc->admap);

    return (EOK);
}

/**
 * SDMA configuration
 * @param hc Host controller handle
 * @param cmd Command
 *
 * @return EPL always
 */
static int imx_sdhcx_sdma_setup(sdio_hc_t *hc, sdio_cmd_t *cmd)
{
    imx_sdhcx_hc_t     *sdhc;
    sdio_sge_t          *sgp;
    int                 sgc;

    sdhc = (imx_sdhcx_hc_t *)hc->cs_hdl;

    sgc = cmd->sgc;
    sgp = cmd->sgl;

    if (!(cmd->flags & SCF_DATA_PHYS)) {
        sdio_vtop_sg(sgp, sdhc->sgl, sgc, cmd->mhdl);
        sgp = sdhc->sgl;
    }

    out32(sdhc->base + IMX7_SDHCX_DS_ADDR, sgp->sg_address);

    return (EOK);
}

/**
 * Transfer setup
 * @param hc Host controller handle
 * @param cmd Command
 * @param command Command
 * @param imask Mask
 *
 * @return Execution status
 */
static int imx_sdhcx_xfer_setup(sdio_hc_t *hc, sdio_cmd_t *cmd, uint32_t *command, uint32_t *imask)
{
    imx_sdhcx_hc_t     *sdhc;
    uintptr_t           base;
    uint32_t            pctl, mix_ctrl;
    int                 status = EOK;

    sdhc = (imx_sdhcx_hc_t *)hc->cs_hdl;
    base = sdhc->base;
    pctl = in32(base + IMX7_SDHCX_PCTL) & ~IMX7_SDHCX_PCTL_DMA_MSK;
    mix_ctrl = 0;

    /* Data present */
    *command |= IMX7_SDHCX_CMD_DP;

    if (cmd->flags & SCF_DIR_IN) {
        mix_ctrl |=  IMX7_SDHCX_MIX_CTRL_DDIR;
    }

    *imask |= IMX7_SDHCX_INTR_DTO | IMX7_SDHCX_INTR_DCRC | IMX7_SDHCX_INTR_DEB | IMX7_SDHCX_INTR_TC;

    status = sdio_sg_start(hc, cmd->sgl, cmd->sgc);

    if (cmd->sgc && (hc->caps & HC_CAP_DMA)) {
        if ((sdhc->flags & SF_USE_ADMA)) {
            if ((status = imx_sdhcx_adma_setup(hc, cmd)) == EOK) {
                pctl |= IMX7_SDHCX_PCTL_ADMA2;
            }
        } else {
            if ((status = imx_sdhcx_sdma_setup(hc, cmd)) == EOK) {
                pctl |= IMX7_SDHCX_PCTL_SDMA;
            }
        }

        if (status == EOK) {
            *imask |= IMX7_SDHCX_INTR_DMAE;
            mix_ctrl |= IMX7_SDHCX_MIX_CTRL_DE;
        }
    }

    /* Use PIO */
    if (status || !(hc->caps & HC_CAP_DMA)) {
        if ((cmd->flags & SCF_DATA_PHYS)) {
           return (status);
        }
        status = EOK;

        *imask |= (cmd->flags & SCF_DIR_IN) ? IMX7_SDHCX_INTR_BRR : IMX7_SDHCX_INTR_BWR;
    }

    if (cmd->blks > 1) {
        mix_ctrl |= IMX7_SDHCX_MIX_CTRL_MBS | IMX7_SDHCX_MIX_CTRL_BCE;
        if ((hc->caps & HC_CAP_ACMD23) && (cmd->flags & SCF_SBC)) {
            mix_ctrl |= IMX7_SDHCX_MIX_CTRL_ACMD23;
        }
        else if ((hc->caps & HC_CAP_ACMD12)) {
            mix_ctrl |= IMX7_SDHCX_MIX_CTRL_ACMD12;
        }
    }

    sdhc->mix_ctrl = mix_ctrl;

    out32(base + IMX7_SDHCX_PCTL, pctl);
    out32(base + IMX7_SDHCX_BLK_ATT, cmd->blksz | (cmd->blks << IMX7_SDHCX_BLK_BLKCNT_SHIFT));

    return (status);
}

/**
 * Process command
 * @param hc Host controller handle
 * @param cmd Command
 *
 * @return Execution status
 */
static int imx_sdhcx_cmd(sdio_hc_t *hc, sdio_cmd_t *cmd)
{
    imx_sdhcx_hc_t     *sdhc;
    uintptr_t           base;
    uint32_t            pmask, pval;
    uint32_t            imask;
    int                 status;
    uint32_t            command, reg;

#ifdef IMX_SDHCX_DEBUG
    uint64_t cps, cycle1, cycle2 = 0;
    cps = SYSPAGE_ENTRY(qtime)->cycles_per_sec;
    cycle1 = ClockCycles();
#endif

    sdhc = (imx_sdhcx_hc_t *)hc->cs_hdl;
    base = sdhc->base;

    /* Command inhibit (CMD) and CMD line signal level state */
    pmask = IMX7_SDHCX_PSTATE_CMDI | IMX7_SDHCX_PSTATE_CLSL;

    command = cmd->opcode << 24;

    if ((cmd->opcode == MMC_STOP_TRANSMISSION) || (cmd->opcode == SD_STOP_TRANSMISSION)) {
        command |= IMX7_SDHCX_CMD_TYPE_CMD12;
    }

    imask = IMX7_SDHCX_IE_DFLTS;

    if ((cmd->flags & SCF_DATA_MSK)) {
        pmask |= IMX7_SDHCX_PSTATE_DATI | IMX7_SDHCX_PSTATE_DL0SL;

        if ((cmd->blks && (status = imx_sdhcx_xfer_setup(hc, cmd, &command, &imask))) != EOK) {
            return (status);
        }
    } else {
        /* Enable command complete intr */
        imask |= IMX7_SDHCX_INTR_CC;
    }

    if ((cmd->flags & SCF_RSP_PRESENT)) {
        if (cmd->flags & SCF_RSP_136) {
            command |= IMX7_SDHCX_CMD_RSP_TYPE_136;
        } else if (cmd->flags & SCF_RSP_BUSY) {
            command |= IMX7_SDHCX_CMD_RSP_TYPE_48b;

            /* Command 12 can be asserted even if data lines are busy */
            if ((cmd->opcode != MMC_STOP_TRANSMISSION) && (cmd->opcode != SD_STOP_TRANSMISSION)) {
                    pmask |= IMX7_SDHCX_PSTATE_DATI | IMX7_SDHCX_PSTATE_DL0SL;
            }
        } else {
            command |= IMX7_SDHCX_CMD_RSP_TYPE_48;
        }
        /* Index check */
        if (cmd->flags & SCF_RSP_OPCODE) {
            command |= IMX7_SDHCX_CMD_CICE;
        }
         /* CRC check */
        if (cmd->flags & SCF_RSP_CRC) {
            command |= IMX7_SDHCX_CMD_CCCE;
        }
    }

    reg = in32(base + IMX7_SDHCX_PSTATE);

    /* If host requires tuning */
    if (reg & (IMX7_SDHCX_PSTATE_RTR)) {
        sdio_hc_event(hc, HC_EV_TUNE);
    }

    /* Wait till card is ready to handle next command */
    pval = pmask & (IMX7_SDHCX_PSTATE_CLSL | IMX7_SDHCX_PSTATE_DL0SL);
    if (((reg & pmask) != pval) &&
        (status = imx_sdhcx_waitmask(hc, IMX7_SDHCX_PSTATE, pmask, pval, IMX7_SDHCX_COMMAND_TIMEOUT))) {
        sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 1,
                   "%s: Timeout CMD_INHIBIT at CMD%d", __FUNCTION__, cmd->opcode);
        return (status);
    }

    /* IDLE state, need to initialize clock */
    if (cmd->opcode == 0) {
        out32(base + IMX7_SDHCX_SYSCTL, (in32(base + IMX7_SDHCX_SYSCTL) | IMX7_SDHCX_SYSCTL_INITA));
        if ((status = imx_sdhcx_waitmask(hc, IMX7_SDHCX_SYSCTL, IMX7_SDHCX_SYSCTL_INITA, 0,
                                          IMX7_SDHCX_COMMAND_TIMEOUT))) {
            sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 1,
                       "%s: Timeout IMX7_SDHCX_SYSCTL at CMD%d", __FUNCTION__, cmd->opcode);
            return (status);
        }
    }

    sdhc->mix_ctrl |= ( in32(base + IMX7_SDHCX_MIX_CTRL) & \
                                    (IMX7_SDHCX_MIX_CTRL_EXE_TUNE | \
                                     IMX7_SDHCX_MIX_CTRL_SMP_CLK_SEL | \
                                     IMX7_SDHCX_MIX_CTRL_AUTOTUNE_EN | \
                                     IMX7_SDHCX_MIX_CTRL_FBCLK_SEL |
                                     IMX7_SDHCX_MIX_CTRL_HS400_MODE |
                                     IMX7_SDHCX_MIX_CTRL_DDR_EN) );

    out32(base + IMX7_SDHCX_MIX_CTRL, sdhc->mix_ctrl);
    sdhc->mix_ctrl = 0;

    /* Rev 1.0 i.MX6x chips require the watermark register to be set prior to
     * every SD command being sent. If the watermark is not set only SD interface
     * 3 works.
     */
    out32(base + IMX7_SDHCX_WATML, ( (0x8 << IMX7_SDHCX_WATML_WRBRSTLENSHIFT) |
                                     (IMX7_SDHCX_WATML_WR << IMX7_SDHCX_WATML_WRWMLSHIFT) |
                                     (0x8 << IMX7_SDHCX_WATML_RDBRSTLENSHIFT) |
                                     (IMX7_SDHCX_WATML_RD << IMX7_SDHCX_WATML_RDWMLSHIFT) ));

    /* Enable interrupts */
    out32(base + IMX7_SDHCX_IS, IMX7_SDHCX_INTR_CLR_ALL);
    out32(base + IMX7_SDHCX_IE, imask);
    out32(base + IMX7_SDHCX_ARG, cmd->arg);
    out32(base + IMX7_SDHCX_CMD, command);

#ifdef IMX_SDHCX_DEBUG
    cycle2 = ClockCycles();
    sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1,
               "%s(), Issuing CMD%d,  cmd_arg:0x%x command:0x%x blks: %d, blksz: %d, mix_ctrl: 0x%x, it took %d us\n",
               __FUNCTION__, hc->wspc.cmd->opcode, cmd->arg, command, cmd->blks, cmd->blksz,
               in32(base + IMX7_SDHCX_MIX_CTRL), (int)((cycle2 - cycle1) * 1000 / (cps/1000)));
#endif

    return (EOK);
}

static int imx_sdhcx_abort(sdio_hc_t *hc, sdio_cmd_t *cmd)
{
    return (EOK);
}

/**
 * Configure power capabilities
 * @param hc Host controller handle
 * @param vdd Power
 *
 * @return EOK always
 */
static int imx_sdhcx_pwr(sdio_hc_t *hc, int vdd)
{
    imx_sdhcx_hc_t     *sdhc;
    uintptr_t           base;
    uint32_t            reg;

    sdhc = (imx_sdhcx_hc_t *)hc->cs_hdl;
    base = sdhc->base;

    if (!hc->vdd || !vdd) {
        /* Reset core */
        imx_sdhcx_reset(hc, IMX7_SDHCX_SYSCTL_SRA);
        /* Make a clean environment */
        out32(base + IMX7_SDHCX_MIX_CTRL, 0);
        out32(base + IMX7_SDHCX_TUNE_CTRL_STATUS, 0);
    }

    if (vdd) {
        reg = in32(base + IMX7_SDHCX_VEND_SPEC) & ~IMX7_SDHCX_VEND_SPEC_SDVS_MSK;
        switch (vdd) {
            case OCR_VDD_17_195:
                reg |= IMX7_SDHCX_VEND_SPEC_SDVS1V8;
                sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1,
                           "%s(): setting power to 1.8v\n", __FUNCTION__);
                break;
            case OCR_VDD_29_30:
            case OCR_VDD_30_31:
            case OCR_VDD_32_33:
            case OCR_VDD_33_34:
                reg |= IMX7_SDHCX_VEND_SPEC_SDVS3V0;
                sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1,
                           "%s(): setting power to 3.3v\n", __FUNCTION__);
                break;
            default:
                sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1,
                           "%s(): unrecognized voltage level. vdd: 0x%x\n", __FUNCTION__, vdd);
                return (EINVAL);
        }

        out32(base + IMX7_SDHCX_VEND_SPEC, reg);
        out32(base + IMX7_SDHCX_ISE, IMX7_SDHCX_ISE_DFLTS);
    }
    //
    hc->vdd = vdd;
    return (EOK);
}

/**
 * Bus mode
 * @param hc Host controller handle
 * @param bus_mode Bus mode
 *
 * @return EOK always
 */
static int imx_sdhcx_bus_mode(sdio_hc_t *hc, int bus_mode)
{
    hc->bus_mode = bus_mode;
    return (EOK);
}

/**
 * Configure bus width
 * @param hc Host controller handle
 * @param width Bus width
 *
 * @return EOK always
 */
static int imx_sdhcx_bus_width(sdio_hc_t *hc, int width)
{
    imx_sdhcx_hc_t     *sdhc;
    uintptr_t           base;
    uint32_t            hctl;

    sdhc = (imx_sdhcx_hc_t *)hc->cs_hdl;
    base = sdhc->base;
    hctl = in32(base + IMX7_SDHCX_PCTL) & ~(IMX7_SDHCX_PCTL_DTW4 | IMX7_SDHCX_PCTL_DTW8);

    switch (width) {
        case BUS_WIDTH_8:
            hctl |=  IMX7_SDHCX_PCTL_DTW8;
            break;
        case BUS_WIDTH_4:
            hctl |= IMX7_SDHCX_PCTL_DTW4;
            break;
        case BUS_WIDTH_1:
        default:
            break;
    }

    out32(base + IMX7_SDHCX_PCTL, hctl);
    hc->bus_width = width;
    return (EOK);
}

/**
 * Configure clock frequency
 * @param hc Host controller handle
 * @param clk Clock value
 *
 * @return EOK always
 */
static int imx_sdhcx_clk(sdio_hc_t *hc, int clk)
{
    imx_sdhcx_hc_t         *sdhc;
    uintptr_t               base;
    uint32_t                sctl;
    int                     pre_div = 1, div = 1;
    int                     ddr_mode = 0;
    uint32_t                dll_ctrl_reg, strdll_ctrl_reg;

    sdhc = (imx_sdhcx_hc_t *)hc->cs_hdl;
    base = sdhc->base;
    sctl = in32(base + IMX7_SDHCX_SYSCTL);

    /* Stop clock */
    sctl &= ~(IMX7_SDHCX_SYSCTL_DTO_MSK | IMX7_SDHCX_SYSCTL_SDCLKFS_MSK);
    sctl |= IMX7_SDHCX_SYSCTL_DTO_MAX | IMX7_SDHCX_SYSCTL_SRC | IMX7_SDHCX_SYSCTL_SRD;
    out32(base + IMX7_SDHCX_SYSCTL, sctl);//TODO verify this added line

    if ((hc->timing == TIMING_DDR50) || (hc->timing == TIMING_HS400)) {
        ddr_mode = 1;
        pre_div = 2;
    }
    if (clk > hc->clk_max) {
        clk = hc->clk_max;
    }

    while ((hc->clk_max / (pre_div * 16) > clk) && (pre_div < 256)) {
        pre_div *= 2;
    }

    while ((hc->clk_max / (pre_div * div) > clk) && (div < 16)) {
        div++;
    }

    sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 5, "desired SD clock: %d, actual: %d\n",
               clk, hc->clk_max / pre_div / div);

    pre_div >>= (1 + ddr_mode);
    div--;

    if ((hc->timing == TIMING_HS200) && (clk > 100000000)) {
        out32(base + IMX7_SDHCX_DLL_CTRL, (IMX7_SDHCX_DLL_CTRL_RESET | IMX7_SDHCX_DLL_CTRL_ENABLE));
    }
    if ((hc->timing == TIMING_HS400) && (clk > 100000000)) {
        out32(base + IMX7_uSDHCx_STROBE_DLL_CTRL, (IMX7_uSDHCx_STROBE_DLL_CTRL_RESET | IMX7_uSDHCx_STROBE_DLL_CTRL_ENABLE));
    }

    sctl = ( (0xE << IMX7_SDHCX_SYSCTL_DTO_SHIFT) |
             (pre_div << IMX7_SDHCX_SYSCTL_SDCLKFS_SHIFT) |
             (div << IMX7_SDHCX_SYSCTL_DVS_SHIFT) |
             0xF );

    /* Enable clock to the card */
    out32(base + IMX7_SDHCX_SYSCTL, sctl);

    /* Wait for clock to stabilize */
    imx_sdhcx_waitmask(hc, IMX7_SDHCX_PSTATE, IMX7_SDHCX_PSTATE_SDSTB, IMX7_SDHCX_PSTATE_SDSTB, IMX7_SDHCX_CLOCK_TIMEOUT);

    if ((hc->timing == TIMING_HS200) && (clk > 100000000)) {
        dll_ctrl_reg = in32(base + IMX7_SDHCX_DLL_CTRL);
        dll_ctrl_reg &= ~IMX7_SDHCX_DLL_CTRL_RESET;
        out32(base + IMX7_SDHCX_DLL_CTRL, dll_ctrl_reg);
        delay(1);
        if ((in32(base + IMX7_SDHCX_DLL_STATUS) & (IMX7_SDHCX_DLL_STATUS_REF_LOCK | IMX7_SDHCX_DLL_STATUS_SLV_LOCK))
            != (IMX7_SDHCX_DLL_STATUS_REF_LOCK | IMX7_SDHCX_DLL_STATUS_SLV_LOCK)) {
            sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "Delay line not locked");
        }
    }
    if ((hc->timing == TIMING_HS400) && (clk > 100000000)) {
        strdll_ctrl_reg = in32(base + IMX7_uSDHCx_STROBE_DLL_CTRL);
        strdll_ctrl_reg &= ~IMX7_uSDHCx_STROBE_DLL_CTRL_RESET;
        out32(base + IMX7_uSDHCx_STROBE_DLL_CTRL, strdll_ctrl_reg);
        delay(1);
        if ((in32(base + IMX7_uSDHCx_STROBE_DLL_STATUS) & (IMX7_uSDHCx_STROBE_DLL_STS_REF_LOCK | IMX7_uSDHCx_STROBE_DLL_STS_SLV_LOCK))
            != (IMX7_uSDHCx_STROBE_DLL_STS_REF_LOCK | IMX7_uSDHCx_STROBE_DLL_STS_SLV_LOCK)) {
            sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "Strobe delay line not locked");
        }
    }

    nanospin_ns(1000L);

    hc->clk = clk;

    return (EOK);
}

/**
 * Configure timing
 * @param hc Host controller handle
 * @param timing Timing value
 *
 * @return EOK always
 */
static int imx_sdhcx_timing(sdio_hc_t *hc, int timing)
{
    imx_sdhcx_hc_t     *sdhc;
    uintptr_t           base;
    uint32_t            mix_ctl;

    sdhc = (imx_sdhcx_hc_t *)hc->cs_hdl;
    base = sdhc->base;

    hc->timing = timing;
    mix_ctl = in32(base + IMX7_SDHCX_MIX_CTRL);

    if (timing == TIMING_DDR50) {
        mix_ctl |= IMX7_SDHCX_MIX_CTRL_DDR_EN;
        mix_ctl &= ~IMX7_SDHCX_MIX_CTRL_HS400_MODE;
    } else if (timing == TIMING_HS400) {
        mix_ctl |= IMX7_SDHCX_MIX_CTRL_DDR_EN;
        mix_ctl |= IMX7_SDHCX_MIX_CTRL_HS400_MODE;
    } else {
        mix_ctl &= ~IMX7_SDHCX_MIX_CTRL_DDR_EN;
        mix_ctl &= ~IMX7_SDHCX_MIX_CTRL_HS400_MODE;
    }

    out32(base + IMX7_SDHCX_MIX_CTRL, mix_ctl);
    imx_sdhcx_clk(hc, hc->clk);

    return(EOK);
}

/**
 * Signal voltage configuration
 * @param hc Host controller handle
 * @param signal_voltage Signal voltage value
 *
 * @return Execution status
 */
static int imx_sdhcx_signal_voltage(sdio_hc_t *hc, int signal_voltage)
{
    imx_sdhcx_hc_t     *sdhc;
    uintptr_t           base;
    uint32_t            reg, dlsl;

    sdhc = (imx_sdhcx_hc_t *)hc->cs_hdl;
    base = sdhc->base;


    if ((hc->version < IMX7_SDHCX_SPEC_VER_3) || (hc->signal_voltage == signal_voltage)) {
        return (EOK);
    }

    /* Its expected that eMMC operates in 1.8 V by default. In this case we skip
     * code to switch to 1.8 V */
    if ((hc->caps & HC_CAP_SLOT_TYPE_EMBEDDED) && (hc->flags & HC_FLAG_DEV_MMC)) {
        if (signal_voltage == SIGNAL_VOLTAGE_1_8) {
            hc->signal_voltage = signal_voltage;
            return (EOK);
        }
    }

    if ((signal_voltage == SIGNAL_VOLTAGE_3_3) || (signal_voltage == SIGNAL_VOLTAGE_3_0)) {
        reg = in32(base + IMX7_SDHCX_VEND_SPEC);
        reg &= ~IMX7_SDHCX_VEND_SPEC_SDVS_MSK;
        reg |= IMX7_SDHCX_VEND_SPEC_SDVS3V0;
        out32(base + IMX7_SDHCX_VEND_SPEC, reg);

        sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s: switched to  3.3V ", __FUNCTION__);
    } else if (signal_voltage == SIGNAL_VOLTAGE_1_8) {
        reg = in32(base + IMX7_SDHCX_VEND_SPEC);
        reg &= ~IMX7_SDHCX_VEND_SPEC_FRC_SDCLK_ON;

        /* Stop sd clock */
        out32(base + IMX7_SDHCX_VEND_SPEC, reg);

        dlsl = IMX7_SDHCX_PSTATE_DL0SL | IMX7_SDHCX_PSTATE_DL1SL | IMX7_SDHCX_PSTATE_DL2SL | IMX7_SDHCX_PSTATE_DL3SL;
        if (imx_sdhcx_waitmask(hc, IMX7_SDHCX_PSTATE, dlsl, 0, IMX7_SDHCX_CLOCK_TIMEOUT)) {
            sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1,
                       "%s() : Failed to switch to 1.8V, can't stop SD Clock", __FUNCTION__);
            return (EIO);
        }
        reg |= IMX7_SDHCX_VEND_SPEC_SDVS1V8;
        out32(base + IMX7_SDHCX_VEND_SPEC, reg);

        /* Sleep at least 5ms */
        delay(5);

        /* Restore sd clock status */
        out32(base + IMX7_SDHCX_VEND_SPEC, reg | IMX7_SDHCX_VEND_SPEC_FRC_SDCLK_ON);
        delay(1);

        reg = in32(base + IMX7_SDHCX_VEND_SPEC);

        /* Data lines should be high now */
        if (!(reg & IMX7_SDHCX_VEND_SPEC_SDVS1V8) ||
            !(in32(base + IMX7_SDHCX_PSTATE) & dlsl)) {
            sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1,
                       "%s(): Failed to switch to 1.8V, DATA lines remain in low", __FUNCTION__);
            return (EIO);
        }
        sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s: switched to 1.8V ", __FUNCTION__);
    } else {
        return (EINVAL);
    }

    hc->signal_voltage = signal_voltage;

    /* The board specific code may need to do something  */
#ifdef BS_PAD_CONF
    bs_pad_conf(hc, SDIO_TRUE);
#endif

    return (EOK);
}

/**
 * Pre-tuning functionality
 * @param hc Host controller handle
 * @param tuning Tuning value
 */
static void imx_sdhcx_pre_tuning(sdio_hc_t *hc, int tuning)
{
    imx_sdhcx_hc_t     *sdhc;
    uintptr_t           base;
    uint32_t            mix_ctl;

#ifdef IMX_SDHCX_DEBUG
     sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s: tuning at %d\n", __FUNCTION__, tuning);
#endif

     sdhc = (imx_sdhcx_hc_t *)hc->cs_hdl;
     base = sdhc->base;

     imx_sdhcx_reset(hc, IMX7_SDHCX_SYSCTL_SRA);
     delay(1);

     imx_sdhcx_clk(hc, hc->clk);

     mix_ctl = in32(base + IMX7_SDHCX_MIX_CTRL);
     mix_ctl |= (IMX7_SDHCX_MIX_CTRL_EXE_TUNE | IMX7_SDHCX_MIX_CTRL_SMP_CLK_SEL | IMX7_SDHCX_MIX_CTRL_FBCLK_SEL);

     out32(base + IMX7_SDHCX_MIX_CTRL, mix_ctl);
     out32(base + IMX7_SDHCX_TUNE_CTRL_STATUS, tuning << IMX7_SDHCI_TUNE_DLY_CELL_SET_PRE_OFF);
}

/**
 * Send tuning command
 * @param hc Host controller handle
 * @param op Device operand (CMD19 - SD, CMD21 - eMMC)
 *
 * @return Execution status
 */
static int imx_sdhcx_send_tune_cmd(sdio_hc_t *hc, int op)
{
    struct sdio_cmd     cmd;
    int                 tlen;
    int                 status;
    sdio_sge_t          sge;
    uint32_t            *buf;

    tlen = hc->bus_width == BUS_WIDTH_8 ? 128 : 64;

    if ((buf = (uint32_t*)sdio_alloc(tlen)) == NULL) {
        sdio_free_cmd(&cmd);
        return (ENOMEM);
    }

    /* Clear buffer for tuning data */
    memset(buf, 0, tlen);

    sge.sg_count = tlen;
    sge.sg_address = (paddr_t)buf;

    memset((void *)&cmd,0,sizeof(struct sdio_cmd));
    sdio_setup_cmd(&cmd, SCF_CTYPE_ADTC | SCF_RSP_R1, op, 0);

    /* Seems if not read the data out of the buffer, it will be some problems */
    sdio_setup_cmd_io(&cmd, SCF_DIR_IN, 1, tlen, &sge, 1, NULL);
    status = sdio_issue_cmd(&hc->device, &cmd, SDIO_TIME_DEFAULT);

    /* Command error */
    if ((cmd.status > CS_CMD_CMP) || (memcmp(buf, (hc->bus_width == BUS_WIDTH_8) ? sdio_tbp_8bit : sdio_tbp_4bit, tlen))) {
        status = EIO;
    }

    return (status);
}

/**
 * Tuning routine
 * @param hc Host controller handle
 * @param op Device operand (CMD19 - SD, CMD21 - eMMC)
 *
 * @return Execution status
 */
static int imx_sdhcx_tune(sdio_hc_t *hc, int op)
{
    imx_sdhcx_hc_t     *sdhc;
    uintptr_t           base;
    uint32_t            mix_ctl;
    int                 status = EIO;
    int                 min, max;

    sdhc = (imx_sdhcx_hc_t *)hc->cs_hdl;
    base = sdhc->base;

    if (hc->version < IMX7_SDHCX_SPEC_VER_3) {
        return (EOK);
    }

    /* Return if not HS200 or SDR104, and not SDR50 that requires tuning */
    if ((hc->timing != TIMING_SDR104) &&
        (hc->timing != TIMING_HS200) &&
        ((hc->timing == TIMING_SDR50) &&
        !(sdhc->flags & SF_TUNE_SDR50))) {
        return (EOK);
    }

    min = IMX7_SDHCI_TUNE_CTRL_MIN;
    while (min < IMX7_SDHCI_TUNE_CTRL_MAX) {
        imx_sdhcx_pre_tuning(hc, min);
        if ((status = imx_sdhcx_send_tune_cmd(hc, op)) == EOK) {
#ifdef IMX_SDHCX_DEBUG
            sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s() Found the maximum not-good value: %d", __func__, min);
#endif
            break;
        }

        min += IMX7_SDHCI_TUNE_STEP;
    }

    max = min + IMX7_SDHCI_TUNE_STEP;
    while (max < IMX7_SDHCI_TUNE_CTRL_MAX) {
        imx_sdhcx_pre_tuning(hc, max);
        if ((status = imx_sdhcx_send_tune_cmd(hc, op)) != EOK) {
#ifdef IMX_SDHCX_DEBUG
            sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s() Found the maximum good value: %d", __func__, max);
#endif
            max += IMX7_SDHCI_TUNE_STEP;
            break;
        }
        max += IMX7_SDHCI_TUNE_STEP;
    }

    imx_sdhcx_pre_tuning(hc, (min + max) / 2);
    if ((status = imx_sdhcx_send_tune_cmd(hc, op) != EOK)) {
        status = EIO;
#ifdef IMX_SDHCX_DEBUG
        sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 1, "%s(), failed tuning", __func__);
#endif
    }

    mix_ctl = in32(base + IMX7_SDHCX_MIX_CTRL);
    mix_ctl &= ~IMX7_SDHCX_MIX_CTRL_EXE_TUNE;

    /* Use the fixed clock if failed */
    if (status) {
        status = EOK;
        mix_ctl &= ~IMX7_SDHCX_MIX_CTRL_SMP_CLK_SEL;
        sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s(), failed tuning, using the fixed clock", __func__);
    } else {
#ifdef IMX_SDHCX_DEBUG
        sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s(), tuned DLY_CELL_SET_PRE at: %d\n", __func__, (min + max)/2);
#endif
    }

    out32(base + IMX7_SDHCX_MIX_CTRL, mix_ctl);
    return (status);
}

/**
 * Card detect routine
 * @param hc Host controller handle
 *
 * @retval SDIO_TRUE Card inserted
 * @retval SDIO_FALSE Card removed
 */
static int imx_sdhcx_cd(sdio_hc_t *hc)
{
    imx_sdhcx_hc_t     *sdhc;
    uintptr_t           base;
    int                 stat;

    sdhc = (imx_sdhcx_hc_t *)hc->cs_hdl;
    base = sdhc->base;

    stat = imx_sdhcx_waitmask(hc, IMX7_SDHCX_PSTATE, IMX7_SDHCX_CARD_STABLE, IMX7_SDHCX_CARD_STABLE, 100000);

    if (!(in32(base + IMX7_SDHCX_PSTATE) & IMX7_SDHCX_PSTATE_CI)) {
        return (SDIO_FALSE);
    }

    return(stat == EOK ? SDIO_TRUE : SDIO_FALSE);
}

/**
 * Host controller de-initialization
 * @param hc Host controller handle
 *
 * @return EOK always
 */
int imx7_sdhcx_dinit(sdio_hc_t *hc)
{
    imx_sdhcx_hc_t *sdhc;

    if (!hc || !hc->cs_hdl) {
        return (EOK);
    }

    sdhc = (imx_sdhcx_hc_t *)hc->cs_hdl;

    if (sdhc->base) {
        imx_sdhcx_pwr(hc, 0);
        imx_sdhcx_reset(hc, IMX7_SDHCX_SYSCTL_SRA);
        out32(sdhc->base + IMX7_SDHCX_ISE, 0);
        out32(sdhc->base + IMX7_SDHCX_IE, 0);

        if (hc->hc_iid != -1) {
            InterruptDetach(hc->hc_iid);
        }
        munmap_device_memory((void *)sdhc->base, hc->cfg.base_addr_size[0]);
    }

    if (sdhc->adma) {
        munmap(sdhc->adma, sizeof(imx_sdhcx_adma32_t) * ADMA_DESC_MAX);
    }

    free(sdhc);
    hc->cs_hdl = NULL;

    return (EOK);
}

/** Controller entry configuration (some overrides done in bs.c file) */
static sdio_hc_entry_t imx_sdhcx_hc_entry = {
    16,
    imx7_sdhcx_dinit, NULL,
    imx_sdhcx_cmd, imx_sdhcx_abort,
    imx_sdhcx_event, imx_sdhcx_cd, imx_sdhcx_pwr,
    imx_sdhcx_clk, imx_sdhcx_bus_mode,
    imx_sdhcx_bus_width, imx_sdhcx_timing,
    imx_sdhcx_signal_voltage, NULL,
    NULL, imx_sdhcx_tune,
    NULL
};

/**
 * Host controller initialization
 * @param hc Host controller handle
 *
 * @return Execution status
 */
int imx7_sdhcx_init(sdio_hc_t *hc)
{
    sdio_hc_cfg_t       *cfg;
    imx_sdhcx_hc_t     *sdhc;
    uint32_t            cap;
    uintptr_t           base;
    struct sigevent     event;

    hc->hc_iid = -1;
    cfg = &hc->cfg;

    memcpy(&hc->entry, &imx_sdhcx_hc_entry, sizeof(sdio_hc_entry_t));

    if (!cfg->base_addr[0]) {
        return (ENODEV);
    }
    if (!cfg->base_addr_size[0]) {
        cfg->base_addr_size[0] = IMX7_SDHCX_SIZE;
    }
    if ((sdhc = hc->cs_hdl = calloc(1, sizeof(imx_sdhcx_hc_t))) == NULL) {
        return (ENOMEM);
    }
    if ((base = sdhc->base = (uintptr_t)mmap_device_io(
              cfg->base_addr_size[0],
              cfg->base_addr[0])) == (uintptr_t)MAP_FAILED) {
            sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 1,
                       "%s: SDHCI base mmap_device_memory (0x%llx) %s",
                       __FUNCTION__, cfg->base_addr[0], strerror(errno));
            imx7_sdhcx_dinit(hc);
            return (errno);
    }

    sdhc->usdhc_addr = cfg->base_addr[0];

    if (!hc->version) {
        hc->version = in16(base + IMX7_SDHCX_HCTL_VERSION) & IMX7_SDHCX_SPEC_VER_MSK;
    }

    cap = in32(base + IMX7_SDHCX_CAP);

    if (cfg->clk) {
        hc->clk_max = cfg->clk;
    } else {
        hc->clk_max = IMX7_CLOCK_DEFAULT;
    }

    hc->caps |= HC_CAP_BSY | HC_CAP_BW4 | HC_CAP_CD_INTR;
    hc->caps |= HC_CAP_ACMD12 | HC_CAP_200MA | HC_CAP_DRV_TYPE_B;

    if (cap & IMX7_SDHCX_CAP_HS) {
        hc->caps |= HC_CAP_HS;
    }
    if (cap & IMX7_SDHCX_CAP_DMA) {
        hc->caps |= HC_CAP_DMA;
    }
    hc->caps |= HC_CAP_SDR104;
    hc->caps |= HC_CAP_SDR50 | HC_CAP_SDR25 | HC_CAP_SDR12;
    hc->caps |= HC_CAP_DDR50 | HC_CAP_HS200 | HC_CAP_HS400;
    sdhc->flags |= SF_TUNE_SDR50;
    if (cap & IMX7_SDHCX_CAP_S18) {
        hc->ocr = OCR_VDD_17_195;
        hc->caps |= HC_CAP_SV_1_8V;
    }

    if ((cap & IMX7_SDHCX_CAP_S30)) {
        hc->ocr = OCR_VDD_30_31 | OCR_VDD_29_30;
        hc->caps |= HC_CAP_SV_3_0V;
    }

    if ((cap & IMX7_SDHCX_CAP_S33)) {
        hc->ocr = OCR_VDD_32_33 | OCR_VDD_33_34;
        hc->caps |= HC_CAP_SV_3_3V;
    }
#ifdef ADMA_SUPPORTED
    if ((cap & IMX7_SDHCX_CAP_DMA)) {
        if (hc->version >= IMX7_SDHCX_SPEC_VER_3) {
            hc->cfg.sg_max = ADMA_DESC_MAX;
            if ((sdhc->adma = mmap(NULL, sizeof(imx_sdhcx_adma32_t) * ADMA_DESC_MAX,
                                   PROT_READ | PROT_WRITE | PROT_NOCACHE,
                                   MAP_PRIVATE | MAP_ANON | MAP_PHYS, NOFD, 0)) == MAP_FAILED) {
                sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 1, "%s: ADMA mmap %s",
                           __FUNCTION__,
                           strerror(errno));
                imx7_sdhcx_dinit(hc);
                return (errno);
            }
            sdhc->flags |= SF_USE_ADMA;
            sdhc->admap = sdio_vtop(sdhc->adma);
            if (hc->version >= IMX7_SDHCX_SPEC_VER_3) {
                hc->caps |= HC_CAP_ACMD23;
            }
        } else {
            hc->cfg.sg_max = 1;
            sdhc->flags |= SF_USE_SDMA;
        }
    }
#endif /* ADMA_SUPPORTED */
    hc->caps &= cfg->caps; /* Reconcile command line options */

    SIGEV_PULSE_INIT(&event, hc->hc_coid, SDIO_PRIORITY, HC_EV_INTR, NULL);
    if ((hc->hc_iid = InterruptAttachEvent(cfg->irq[0], &event, _NTO_INTR_FLAGS_TRK_MSK)) == -1) {
        imx7_sdhcx_dinit(hc);
        return (errno);
    }

    /* Only enable the card insertion and removal interrupts */
    out32(base + IMX7_SDHCX_ISE, IMX7_SDHCX_ISE_DFLTS);
    out32(base + IMX7_SDHCX_IE, IMX7_SDHCX_INTR_CINS | IMX7_SDHCX_INTR_CREM);

    return (EOK);
}
#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/devb/sdmmc/sdiodi/hc/imx7.c $ $Rev: 840817 $")
#endif
