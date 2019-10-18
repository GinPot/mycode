/*
 * $QNXLicenseC:
 * Copyright 2017, QNX Software Systems.
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

#ifdef SDIO_HC_LS10XX
#include <ls10xx.h>

//#define LS10XX_DEBUG

#ifdef LS10XX_DEBUG
static int ls10xx_sdhcx_reg_dump(sdio_hc_t *hc, const char *func, int line)
{
    LS10XX_LOG(hc, _SLOG_ERROR, "===========================================================");

    LS10XX_LOG(hc, _SLOG_ERROR, "HCTL_VER %x CAP %x CAP2 %x",
                ls10xx_in32(hc, LS10XX_HOSTVER),
                ls10xx_in32(hc, LS10XX_CAP), ls10xx_in32(hc, LS10XX_CAP2));

    LS10XX_LOG(hc, _SLOG_ERROR, "SDMA_ARG2 %x ADMA %x ADMA_ES %x BLK %x ARG %x CMD %x RSP10 %x RSP32 %x RSP54 %x RSP76 %x",
                ls10xx_in32(hc, LS10XX_DS_ADDR),
                ls10xx_in32(hc, LS10XX_ADMA_ADDR),
                ls10xx_in32(hc, LS10XX_ADMA_ES),
                ls10xx_in32(hc, LS10XX_BLK_ATT),
                ls10xx_in32(hc, LS10XX_ARG),
                ls10xx_in32(hc, LS10XX_CMD),
                ls10xx_in32(hc, LS10XX_RESP0),
                ls10xx_in32(hc, LS10XX_RESP1),
                ls10xx_in32(hc, LS10XX_RESP2),
                ls10xx_in32(hc, LS10XX_RESP3));

    LS10XX_LOG(hc, _SLOG_ERROR, "IS %x IE %x ISE %x WATML %x FORCE %x TUNE_CTRL %x TUNE_PTR %x",
                ls10xx_in32(hc, LS10XX_IS),
                ls10xx_in32(hc, LS10XX_IE),
                ls10xx_in32(hc, LS10XX_ISE),
                ls10xx_in32(hc, LS10XX_WATML),
                ls10xx_in32(hc, LS10XX_FORCE_EVENT),
                ls10xx_in32(hc, LS10XX_TUNE_CTRL),
                ls10xx_in32(hc, LS10XX_TUNE_PTR));

    LS10XX_LOG(hc, _SLOG_ERROR, "PSTATE %x PCTL %x SYSCTL %x SYSCTL2 %x CLKCTRL %x ESDHCCTL %x",
                ls10xx_in32(hc, LS10XX_PSTATE),
                ls10xx_in32(hc, LS10XX_PCTL),
                ls10xx_in32(hc, LS10XX_SYSCTL),
                ls10xx_in32(hc, LS10XX_AC12_SYSCTL2),
                ls10xx_in32(hc, LS10XX_CLKCTRL),
                ls10xx_in32(hc, LS10XX_ESDHC_CTRL));

    LS10XX_LOG(hc, _SLOG_ERROR, "===========================================================");
    return(EOK);
}
#endif

static int ls10xx_waitmask(sdio_hc_t *hc, uint32_t reg, uint32_t mask, uint32_t val, uint32_t usec)
{
    uint32_t cnt;
    int      stat = ETIMEDOUT;
    int      rval = 0;
    uint32_t iter;

    /* fast poll for 1ms - 1us intervals */
    for(cnt = min(usec, 1000); cnt; cnt--) {
        if(((rval = ls10xx_in32(hc, reg)) & mask) == val) {
            stat = EOK;
            break;
        }
        nanospin_ns(1000L);
    }

    if(usec > 1000 && stat) {
        iter    = usec / 1000L;
        for(cnt = iter; cnt; cnt--) {
            if(((rval = ls10xx_in32(hc, reg)) & mask) == val) {
                stat = EOK;
                break;
            }
            delay(1);
        }
    }

    return(stat);
}

// Wait for the sdio data lines status. high = 1 means line are high, 0 lines are low
int ls10xx_dataLines_wait(sdio_hc_t *hc, uint32_t high)
{
    uint32_t dlsl = LS10XX_PSTATE_DL0SL | LS10XX_PSTATE_DL1SL |
                    LS10XX_PSTATE_DL2SL | LS10XX_PSTATE_DL3SL;

    if (ls10xx_waitmask(hc, LS10XX_PSTATE, dlsl, (high ? dlsl : 0), LS10XX_CLOCK_TIMEOUT)) {
        return (EIO);
    }

    return(EOK);
}

#ifndef LS10XX_CD_USE_SDIO_POLL_TIMER
static uint32_t ls10xx_cd_irq_enable(sdio_hc_t *hc) {
    uint32_t ie;
    uint32_t ise;
    uint32_t pstate;

    ls10xx_waitmask(hc, LS10XX_PSTATE, LS10XX_CARD_STABLE, LS10XX_CARD_STABLE, 1000);

    ie = ls10xx_in32(hc, LS10XX_IE);
    ise = ls10xx_in32(hc, LS10XX_ISE);
    pstate = ls10xx_in32(hc, LS10XX_PSTATE);

    ls10xx_out32(hc, LS10XX_IS, (LS10XX_INTR_CREM | LS10XX_INTR_CINS));
    ie  &= ~(LS10XX_INTR_CREM | LS10XX_INTR_CINS);
    ise &= ~(LS10XX_INTR_CREM | LS10XX_INTR_CINS);

    if (pstate & LS10XX_PSTATE_CI) {
        ie  |= LS10XX_INTR_CREM;
        ise |= LS10XX_INTR_CREM;
    } else {
        ie  |= LS10XX_INTR_CINS;
        ise |= LS10XX_INTR_CINS;
    }

    ls10xx_out32(hc, LS10XX_IE,  ie);
    ls10xx_out32(hc, LS10XX_ISE, ise);

    LS10XX_LOG(hc, _SLOG_DEBUG1, "pstate: %x is:%x ie:%x ise:%x", ls10xx_in32(hc, LS10XX_PSTATE),
                ls10xx_in32(hc, LS10XX_IS), ls10xx_in32(hc, LS10XX_IE), ls10xx_in32(hc, LS10XX_ISE));

    return (pstate & LS10XX_PSTATE_CI);
}
#endif

int ls10xx_clock_set(sdio_hc_t *hc, uint32_t on)
{
    uint32_t sysctl = ls10xx_in32(hc, LS10XX_SYSCTL);

    if (on) {
        sysctl |= LS10XX_SYSCTL_SDCLKEN;
    } else {
       // Disable SD clock
      sysctl &= ~LS10XX_SYSCTL_SDCLKEN;
    }
    ls10xx_out32(hc, LS10XX_SYSCTL, sysctl);

    if (ls10xx_waitmask(hc, LS10XX_PSTATE, LS10XX_PSTATE_SDSTB, LS10XX_PSTATE_SDSTB,
                    LS10XX_CLOCK_TIMEOUT)) {
        LS10XX_LOG(hc, _SLOG_ERROR, "Failed to set clock, PSTATE: %#x SYSCTL %#x",
                    ls10xx_in32(hc, LS10XX_PSTATE), ls10xx_in32(hc, LS10XX_SYSCTL));
        return (EIO);
    }
    nanospin_ns(1000L);

    return(EOK);
}

static int ls10xx_reset(sdio_hc_t *hc, uint32_t rst)
{
    LS10XX_LOG(hc, _SLOG_DEBUG2, "");

    /* wait up to 100 ms for reset to complete */
    ls10xx_out32(hc, LS10XX_SYSCTL, ls10xx_in32(hc, LS10XX_SYSCTL) | rst );

    if (ls10xx_waitmask(hc, LS10XX_SYSCTL, rst, 0, 100000) != EOK) {
        LS10XX_LOG(hc, _SLOG_ERROR, "Time out, rst %#x", rst);
    }
    return(EOK);
}

static int ls10xx_pio_xfer(sdio_hc_t *hc, sdio_cmd_t *cmd)
{
    int      len;
    int      msk = (cmd->flags & SCF_DIR_IN) ? LS10XX_PSTATE_BRE : LS10XX_PSTATE_BWE;
    int      blksz;
    uint8_t  *addr;
    int      blks = cmd->blks;
    ls10xx_hc_t *sdhc = (ls10xx_hc_t *) hc->cs_hdl;

    LS10XX_LOG(hc, _SLOG_DEBUG2, "");

#ifdef LS10XX_DEBUG
      uint64_t    cps, cycle1, cycle2=0;
    cps = SYSPAGE_ENTRY(qtime)->cycles_per_sec;
    cycle1 = ClockCycles();
#endif

    /* Multiple blocks */
    while(blks--) {
        blksz    = cmd->blksz;
        while(blksz) {
            if(sdio_sg_nxt(hc, &addr, &len, blksz)) {
                break;
            }
            blksz -= len;
            len   /= 4;

            /* BRE or BWE is asserted when the available buffer is more than the watermark level */
            while(len) {
                int xfer_len = min(len, LS10XX_WATML_WR);
                len -= xfer_len;

                /* Wait until the read or write buffer is ready */
                if(ls10xx_waitmask(hc, LS10XX_PSTATE, msk, msk, LS10XX_TRANSFER_TIMEOUT)) {
                         LS10XX_LOG(hc, _SLOG_ERROR,
                             " pio read: timedout for BREN in present state register");
                         return ETIMEDOUT;
                }

                if((cmd->flags & SCF_DIR_IN))
                    ls10xx_sdhcx_data_in32s(addr, xfer_len, sdhc->base + LS10XX_DATA);
                else
                    ls10xx_sdhcx_data_out32s(addr, xfer_len, sdhc->base + LS10XX_DATA);

                addr += xfer_len * 4;
            }
        }
    }

#ifdef LS10XX_DEBUG
    cycle2 = ClockCycles();
    LS10XX_LOG(hc, _SLOG_INFO, "CMD %d  flags:%x, cmd->blks: %d, cmd->blksz: %d, it took %d us",
        cmd->opcode, cmd->flags, cmd->blks, cmd->blksz, (int)((cycle2 - cycle1) * 1000 / (cps/1000)));
#endif

    return(EOK);
}

static int ls10xx_intr_event(sdio_hc_t *hc)
{
    sdio_cmd_t  *cmd;
    uint32_t    sts = ls10xx_in32(hc, LS10XX_IS);
    int         cs = CS_CMD_INPROG;
    int         idx;
    uint32_t    rsp;
    uint8_t     rbyte;

    LS10XX_LOG(hc, _SLOG_DEBUG2, "is:%x ie:%x ise:%x",
                ls10xx_in32(hc, LS10XX_IS), ls10xx_in32(hc, LS10XX_IE), ls10xx_in32(hc, LS10XX_ISE));

    /* Clear interrupt events */
    ls10xx_out32(hc, LS10XX_IS, sts);

#ifndef LS10XX_CD_USE_SDIO_POLL_TIMER
    /* Card insertion and card removal events */
    if((sts & (LS10XX_INTR_CINS | LS10XX_INTR_CREM))) {
        uint32_t inserted = ls10xx_cd_irq_enable(hc);
        if ( (inserted && (sts & LS10XX_INTR_CINS)) || ( !inserted && (sts & LS10XX_INTR_CREM)) ) {
            sdio_hc_event(hc, HC_EV_CD);
        }
    }
#endif

    if((cmd = hc->wspc.cmd) == NULL) {
        return(EOK);
    }

    /* Tuning commands */
    if(cmd->opcode == SD_SEND_TUNING_BLOCK || cmd->opcode == MMC_SEND_TUNING_BLOCK) {
        /* Though we don't need the data, we need to clear the buffer */
        //if(sts & LS10XX_INTR_BRR) {
        //    ls10xx_pio_xfer(hc, cmd);
        //}

        if(sts & LS10XX_IS_ERRI)
            cs = CS_CMD_CMP_ERR;
        else
            cs = CS_CMD_CMP;

        sdio_cmd_cmplt(hc, cmd, cs);
        return EOK;
    }

    /* Check of errors */
    if(sts & LS10XX_IS_ERRI) {
        LS10XX_LOG(hc, _SLOG_ERROR,
                "ERROR in HC, CMD%d, sts: 0x%x:  is 0x%x, ac12 0x%x, LS10XX_IS_ERRI: 0x%x",
                cmd->opcode, sts, ls10xx_in32(hc, LS10XX_IS), ls10xx_in32(hc, LS10XX_AC12_SYSCTL2), LS10XX_IS_ERRI);

        if(sts & LS10XX_INTR_DTO)     cs = CS_DATA_TO_ERR;
        if(sts & LS10XX_INTR_DCRC)    cs = CS_DATA_CRC_ERR;
        if(sts & LS10XX_INTR_DEB)     cs = CS_DATA_END_ERR;
        if(sts & LS10XX_INTR_CTO)     cs = CS_CMD_TO_ERR;
        if(sts & LS10XX_INTR_CCRC)    cs = CS_CMD_CRC_ERR;
        if(sts & LS10XX_INTR_CEB)     cs = CS_CMD_END_ERR;
        if(sts & LS10XX_INTR_CIE)     cs = CS_CMD_IDX_ERR;
        if(sts & LS10XX_INTR_DMAE)    cs = CS_DATA_TO_ERR;
        if(sts & LS10XX_INTR_ACE)     cs = CS_DATA_TO_ERR;
        if(!cs)                       cs = CS_CMD_CMP_ERR;

        LS10XX_LOG(hc, _SLOG_ERROR, "ERROR code is: 0x%X    sts is: 0x%X", cs, sts);
        ls10xx_reset(hc, LS10XX_SYSCTL_SRC | LS10XX_SYSCTL_SRD);
    } else {
        /* End of command */
        if((sts & LS10XX_INTR_CC)) {
            cs = CS_CMD_CMP;
             // Errata ENGcm03648
            if(cmd->flags & SCF_RSP_BUSY) {
                int timeout = 16 * 1024 * 1024;

                while (!(ls10xx_in32(hc, LS10XX_PSTATE) & LS10XX_PSTATE_DL0SL) && timeout--)
                    nanospin_ns(100);

                if (timeout <= 0) {
                    cs = CS_CMD_TO_ERR;
                    ls10xx_reset(hc, LS10XX_SYSCTL_SRC | LS10XX_SYSCTL_SRD);
                    LS10XX_LOG(hc, _SLOG_ERROR, "busy done wait timeout for cmd: %d", cmd->opcode);
                }
            }

            if((cmd->flags & SCF_RSP_136)) {
                /*
                 *  CRC is not included in the response reg, left
                 *  shift 8 bits to match the 128 CID/CSD structure
                 */
                for(idx = 0, rbyte = 0; idx < 4; idx++) {
                    rsp                = ls10xx_in32(hc, LS10XX_RESP0
                                                       + idx * 4);
                    cmd->rsp[3-idx] = (rsp << 8) + rbyte;
                    rbyte            = rsp >> 24;
                }
            } else if((cmd->flags & SCF_RSP_PRESENT)) {
                cmd->rsp[0] = ls10xx_in32(hc, LS10XX_RESP0);
            }
        }

        /* End of data transfer */
        if (sts & LS10XX_INTR_TC) {
            cs = CS_CMD_CMP;
            cmd->rsp[0] = ls10xx_in32(hc, LS10XX_RESP0);
        }

        /* Doesn't need to do anything for DMA interrupt */
        if((sts & LS10XX_INTR_DMA)) {
            cs = CS_CMD_CMP;
        }

        if((sts & (LS10XX_INTR_BWR | LS10XX_INTR_BRR))) {
            if (EOK == ls10xx_pio_xfer(hc, cmd))
                cs = CS_CMD_CMP;
            else
                cs = CS_DATA_TO_ERR;
        }
    }

    if (cs != CS_CMD_INPROG) {
        if ((cs == CS_DATA_TO_ERR) || (cs == CS_CMD_TO_ERR)) {
            // Timeout error case, check if card removed
            if(!(hc->entry.cd(hc) & CD_INS))
                cs = CS_CARD_REMOVED;
        }
        sdio_cmd_cmplt(hc, cmd, cs);
    }

    return(EOK);
}

static int ls10xx_sdhcx_event(sdio_hc_t *hc, sdio_event_t *ev)
{
    int status;

    switch(ev->code) {
        case HC_EV_INTR:
            status = ls10xx_intr_event(hc);
            InterruptUnmask(hc->cfg.irq[0], hc->hc_iid);
            break;

        default:
            status = bs_event(hc, ev);
            break;
    }

    return(status);
}

static int ls10xx_adma_setup(sdio_hc_t *hc, sdio_cmd_t *cmd)
{
    ls10xx_hc_t       *sdhc = (ls10xx_hc_t *) hc->cs_hdl;
    ls10xx_adma32_t   *adma = (ls10xx_adma32_t *) sdhc->adma;
    sdio_sge_t        *sgp  = cmd->sgl;
    int               sgc   = cmd->sgc;
    int               sgi;
    int               acnt;
    int               alen;
    int               sg_count;
    paddr_t           paddr;

    if(!(cmd->flags & SCF_DATA_PHYS)) {
        sdio_vtop_sg(sgp, sdhc->sgl, sgc, cmd->mhdl);
        sgp = sdhc->sgl;
    }

    for(sgi = 0, acnt = 0; sgi < sgc; sgi++, sgp++) {
        paddr        = sgp->sg_address;
        sg_count    = sgp->sg_count;
        while(sg_count) {
            alen        = min(sg_count, LS10XX_ADMA2_MAX_XFER);
            adma->attr    = LS10XX_ADMA2_VALID | LS10XX_ADMA2_TRAN;
            adma->addr    = paddr;
            adma->len    = alen;
            sg_count    -= alen;
            paddr        += alen;
            adma++;
            if(++acnt > ADMA_DESC_MAX) {
                return(ENOTSUP);
            }
        }
    }

    adma--;
    adma->attr |= LS10XX_ADMA2_END;

    ls10xx_out32(hc, LS10XX_ADMA_ADDR, sdhc->admap);

    return(EOK);
}

static int ls10xx_sdma_setup(sdio_hc_t *hc, sdio_cmd_t *cmd)
{
    ls10xx_hc_t   *sdhc = (ls10xx_hc_t *) hc->cs_hdl;
    sdio_sge_t        *sgp = cmd->sgl;;
    int                sgc = cmd->sgc;

    if(!(cmd->flags & SCF_DATA_PHYS)) {
        sdio_vtop_sg(sgp, sdhc->sgl, sgc, cmd->mhdl);
        sgp = sdhc->sgl;
    }

    ls10xx_out32(hc, LS10XX_DS_ADDR, sgp->sg_address);

    return(EOK);
}

static int ls10xx_xfer_setup(sdio_hc_t *hc, sdio_cmd_t *cmd, uint32_t *command, uint32_t *imask)
{
    int         status = EOK;
    ls10xx_hc_t *sdhc = (ls10xx_hc_t *) hc->cs_hdl;
    uint32_t    pctl = ls10xx_in32(hc, LS10XX_PCTL) & ~LS10XX_PCTL_DMA_MSK;

    LS10XX_LOG(hc, _SLOG_DEBUG2, "");

    /* Data present */
    *command    |= LS10XX_CMD_DP;

    if (cmd->flags & SCF_DIR_IN)
        *command    |=  LS10XX_CMD_DTDSEL;

    *imask  |= LS10XX_INTR_DTO | LS10XX_INTR_DCRC | LS10XX_INTR_DEB | LS10XX_INTR_TC;

    status = sdio_sg_start(hc, cmd->sgl, cmd->sgc);

    if(cmd->sgc && (hc->caps & HC_CAP_DMA)) {
        if((sdhc->flags & SF_USE_ADMA)) {
            if((status = ls10xx_adma_setup(hc, cmd)) == EOK)
                pctl |= LS10XX_PCTL_ADMA2;
        } else {
            if((status = ls10xx_sdma_setup(hc, cmd)) == EOK)
                pctl |= LS10XX_PCTL_SDMA;
        }

        if (status == EOK) {
            *imask |= LS10XX_INTR_DMAE;
            *command |= LS10XX_CMD_DMAEN;
        }
    }

    /* Use PIO */
    if(status || !(hc->caps & HC_CAP_DMA)) {
        if((cmd->flags & SCF_DATA_PHYS)) {
            return(status);
        }
        status = EOK;

        *imask |= (cmd->flags & SCF_DIR_IN) ?
              LS10XX_INTR_BRR : LS10XX_INTR_BWR;
    }

    if(cmd->blks > 1) {
        *command |= LS10XX_CMD_MSBSEL | LS10XX_CMD_BCEN;
        if((hc->caps & HC_CAP_ACMD23) && (cmd->flags & SCF_SBC)) {
            *command |= LS10XX_CMD_ACEN_ACMD23;
        }
        else if((hc->caps & HC_CAP_ACMD12)) {
            *command |= LS10XX_CMD_ACEN_ACMD12;
        }
    }

    ls10xx_out32(hc, LS10XX_PCTL, pctl);
    ls10xx_out32(hc, LS10XX_BLK_ATT, cmd->blksz | (cmd->blks << LS10XX_BLK_BLKCNT_SHIFT));

    return(status);
}

static int ls10xx_sdhcx_cmd(sdio_hc_t *hc, sdio_cmd_t *cmd)
{
    uint32_t        pmask, pval;
    uint32_t        imask;
    int             status;
    uint32_t        command, reg;

#ifdef LS10XX_DEBUG
      uint64_t    cps, cycle1, cycle2=0;
    cps = SYSPAGE_ENTRY(qtime)->cycles_per_sec;
    cycle1 = ClockCycles();
#endif

    /* Command inhibit (CMD) and CMD line signal level state */
    pmask   = LS10XX_PSTATE_CMDI | LS10XX_PSTATE_CLSL;
    command = cmd->opcode << 24;

    if((cmd->opcode == MMC_STOP_TRANSMISSION) || (cmd->opcode == SD_STOP_TRANSMISSION))
        command |= LS10XX_CMD_TYPE_CMD12;

    imask = LS10XX_IE_DFLTS;
    // Maintain the CINS/CREM IE settings.
    imask |= (ls10xx_in32(hc, LS10XX_IE) & (LS10XX_INTR_CINS | LS10XX_INTR_CREM));

    if (cmd->opcode == MMC_SEND_TUNING_BLOCK || cmd->opcode == SD_SEND_TUNING_BLOCK ) {
        imask |= LS10XX_INTR_BRR;
    }

    if((cmd->flags & SCF_DATA_MSK)) {
        pmask |= LS10XX_PSTATE_DATI | LS10XX_PSTATE_DL0SL;

        if((cmd->blks && (status = ls10xx_xfer_setup(hc, cmd, &command, &imask))) != EOK)
            return(status);
    } else {
        /* Enable command complete intr */
        imask |= LS10XX_INTR_CC;
    }

    if((cmd->flags & SCF_RSP_PRESENT)) {
        if (cmd->flags & SCF_RSP_136)
            command |= LS10XX_CMD_RSP_TYPE_136;
        else if (cmd->flags & SCF_RSP_BUSY) {
            command |= LS10XX_CMD_RSP_TYPE_48b;

            /* command 12 can be asserted even if data lines are busy */
            if((cmd->opcode != MMC_STOP_TRANSMISSION) && (cmd->opcode != SD_STOP_TRANSMISSION))
                pmask |= LS10XX_PSTATE_DATI | LS10XX_PSTATE_DL0SL;
        } else {
            command |= LS10XX_CMD_RSP_TYPE_48;
        }

        if (cmd->flags & SCF_RSP_OPCODE)        // Index check
            command |= LS10XX_CMD_CICE;

        if (cmd->flags & SCF_RSP_CRC)             // CRC check
            command |= LS10XX_CMD_CCCE;
    }

#if 1
    reg = ls10xx_in32(hc, LS10XX_ESDHC_CTRL);

    /* If host requires tunning */
    if(reg & LS10XX_ESDHC_CTRL_RTR) {
        sdio_hc_event(hc, HC_EV_TUNE);
    }
#endif

    reg = ls10xx_in32(hc, LS10XX_PSTATE);

    /* wait till card is ready to handle next command */
    pval = pmask & (LS10XX_PSTATE_CLSL | LS10XX_PSTATE_DL0SL);
    if (((reg & pmask) != pval) &&
        (status = ls10xx_waitmask(hc, LS10XX_PSTATE, pmask, pval, LS10XX_COMMAND_TIMEOUT))) {
        LS10XX_LOG(hc, _SLOG_ERROR, "Timeout CMD_INHIBIT at CMD%d", cmd->opcode);
        return(status);
    }

    /* IDLE state, need to initialize clock */
    if (cmd->opcode == 0) {
        ls10xx_out32(hc, LS10XX_SYSCTL, (ls10xx_in32(hc, LS10XX_SYSCTL) | LS10XX_SYSCTL_INITA));
        if((status = ls10xx_waitmask(hc, LS10XX_SYSCTL, LS10XX_SYSCTL_INITA, 0, LS10XX_COMMAND_TIMEOUT))) {
            LS10XX_LOG(hc, _SLOG_ERROR, "Timeout LS10XX_SYSCTL at CMD%d", cmd->opcode);
            return(status);
        }
    }

    /* enable interrupts */
    ls10xx_out32(hc, LS10XX_IS, LS10XX_INTR_CLR_ALL);
    ls10xx_out32(hc, LS10XX_IE, imask);
    ls10xx_out32(hc, LS10XX_ARG, cmd->arg);
    ls10xx_out32(hc, LS10XX_CMD, command);

#ifdef LS10XX_DEBUG
    cycle2 = ClockCycles();
    LS10XX_LOG(hc, _SLOG_DEBUG2,
        "Issuing CMD%d, cmd_arg:0x%x command:0x%x blks: %d, blksz: %d, it took %d us",
        cmd->opcode, cmd->arg, command, cmd->blks, cmd->blksz,
        (int)((cycle2 - cycle1) * 1000 / (cps/1000)));
#endif

    return(EOK);
}

static int ls10xx_sdhcx_abort(sdio_hc_t *hc, sdio_cmd_t *cmd)
{
    return(EOK);
}

static int ls10xx_sdhcx_pwr(sdio_hc_t *hc, int vdd)
{
    // Return if the volatge didnt change.
    if ( (hc->vdd == vdd) || (hc->vdd && vdd) ) {
        return (EOK);
    }

    if(vdd == 0) {            /* reset core */
        ls10xx_reset(hc, LS10XX_SYSCTL_SRC | LS10XX_SYSCTL_SRD);
        // Make sure the tuning block is disabled
        ls10xx_out32(hc, LS10XX_TUNE_CTRL,
                     ls10xx_in32(hc, LS10XX_TUNE_CTRL) & ~LS10XX_TUNE_CTRL_ENABLE);

#ifdef ADMA_SUPPORTED
        if (hc->caps & HC_CAP_DMA) {
            ls10xx_out32(hc, LS10XX_ESDHC_CTRL ,
                    ls10xx_in32(hc, LS10XX_ESDHC_CTRL) | LS10XX_ESDHC_CTRL_SNOOP);
        }
#endif

        // Turn the clock off to the card
        ls10xx_clock_set(hc, 0);

#ifndef LS10XX_CD_USE_SDIO_POLL_TIMER
        ls10xx_cd_irq_enable(hc);
#endif
    }

    ls10xx_out32(hc, LS10XX_ISE, LS10XX_ISE_DFLTS );

    hc->vdd = vdd;

    return (EOK);
}

static int ls10xx_sdhcx_bus_mode(sdio_hc_t *hc, int bus_mode)
{
    hc->bus_mode = bus_mode;
    return (EOK);
}

static int ls10xx_sdhcx_bus_width(sdio_hc_t *hc, int width)
{
    uint32_t hctl = ls10xx_in32(hc, LS10XX_PCTL) & ~(LS10XX_PCTL_DTW4 | LS10XX_PCTL_DTW8);

    switch (width) {
        case BUS_WIDTH_8:
            hctl |=  LS10XX_PCTL_DTW8;
            break;

        case BUS_WIDTH_4:
            hctl |= LS10XX_PCTL_DTW4;
            break;

        case BUS_WIDTH_1:
            hctl |= LS10XX_PCTL_DTW1;
            break;

        default:
            break;
    }

    hctl |= LS10XX_PCTL_LEM;

    ls10xx_out32(hc, LS10XX_PCTL, hctl);
    hc->bus_width = width;
    return(EOK);
}


int ls10xx_sdhcx_clk(sdio_hc_t *hc, int clk)
{
    ls10xx_hc_t *sdhc  = (ls10xx_hc_t *) hc->cs_hdl;
    sdio_hc_cfg_t *cfg = &hc->cfg;
    int       pre_div  = 1, div = 1;
    uint32_t  sctl, ctrl;
    uint32_t  base_clk;

    // first turn off the clock to the card
    ls10xx_clock_set(hc, 0);

    sctl = ls10xx_in32(hc, LS10XX_SYSCTL);
    /* stop clock */
    sctl &= ~(LS10XX_SYSCTL_DTO_MSK | LS10XX_SYSCTL_DVS_MSK | LS10XX_SYSCTL_SDCLKFS_MSK);
    ls10xx_out32(hc, LS10XX_SYSCTL, sctl);

    ctrl = ls10xx_in32(hc, LS10XX_ESDHC_CTRL);
    ctrl &= ~LS10XX_ESDHC_CTRL_PCS;

    if (clk > hc->clk_max)
        clk = hc->clk_max;

    // Datasheet: For any operations above 52 MHz, eSDHC must be clocked by
    // PLL source by setting eSDHCCTL[PCS] to 1.
    if (clk > (52 * 1000 * 1000)) {
        // The eSDHC has an internal /2 dividor on the peripheral clock input
        base_clk = sdhc->per_clk / 2;
        ctrl |= LS10XX_ESDHC_CTRL_PCS;
    } else {
        base_clk = cfg->clk;
    }

    while (((base_clk / pre_div / 16) > clk) && (pre_div < 256))
        pre_div *= 2;

    while ((base_clk / pre_div / div > clk) && (div < 16))
        div++;

    hc->clk = base_clk / pre_div / div;

    LS10XX_LOG(hc, _SLOG_INFO, "desired SD clock: %d, actual: %d", clk, hc->clk);

    pre_div >>= 1;
    div--;

    sctl =  (div << LS10XX_SYSCTL_DVS_SHIFT) | (pre_div << LS10XX_SYSCTL_SDCLKFS_SHIFT);
    sctl |= (0xE << LS10XX_SYSCTL_DTO_SHIFT);
    ls10xx_out32(hc, LS10XX_SYSCTL, sctl);

    /* wait for clock to stabilize */
    ls10xx_waitmask(hc, LS10XX_PSTATE, LS10XX_PSTATE_SDSTB, LS10XX_PSTATE_SDSTB, LS10XX_CLOCK_TIMEOUT);

    // Enable/Disable the esdhc peripheral clock
    // Note: Must be set after the dividors are set and the clock is stable
    ls10xx_out32(hc, LS10XX_ESDHC_CTRL, ctrl);

    // Turn on the clock to the card
    ls10xx_clock_set(hc, 1);

    return(EOK);
}

static int ls10xx_sdhcx_timing(sdio_hc_t *hc, int timing)
{
    uint32_t sysctl2 = ls10xx_in32(hc, LS10XX_AC12_SYSCTL2);

    sysctl2 &= ~LS10XX_SYSCTL2_UHSM_MASK;

    ls10xx_clock_set(hc, 0);

    switch (timing) {
        case TIMING_HS400:
        case TIMING_HS200:
        case TIMING_SDR104:
            sysctl2 |= LS10XX_SYSCTL2_UHSM_SDR104;
            break;
        case TIMING_DDR50:
            sysctl2 |= LS10XX_SYSCTL2_UHSM_DDR;
            ls10xx_out32(hc, LS10XX_AC12_SYSCTL2, sysctl2);

            ls10xx_out32(hc, LS10XX_CLKCTRL, ls10xx_in32(hc, LS10XX_CLKCTRL) |
                            LS10XX_CLKCTRL_CMD_CLKCRTRL | LS10XX_CLKCTRL_LPBK_CLKSEL);

            ls10xx_clock_set(hc, 1);

            ls10xx_clock_set(hc, 0);

            ls10xx_out32(hc, LS10XX_ESDHC_CTRL,
                            ls10xx_in32(hc, LS10XX_ESDHC_CTRL) | LS10XX_ESDHC_CTRL_FAF);

            ls10xx_waitmask(hc, LS10XX_ESDHC_CTRL, LS10XX_ESDHC_CTRL_FAF, 0, LS10XX_CLOCK_TIMEOUT);
            break;
        case TIMING_SDR50:
            sysctl2 |= LS10XX_SYSCTL2_UHSM_SDR50;
            break;
        case TIMING_SDR25:
            sysctl2 |= LS10XX_SYSCTL2_UHSM_SDR25;
            break;
        case TIMING_SDR12:
            sysctl2 |= LS10XX_SYSCTL2_UHSM_SDR12;
            break;
        case TIMING_HS:
        case TIMING_LS:
        default:
            break;
    }
    // Set Mode
    if (timing != TIMING_DDR50) {
        ls10xx_out32(hc, LS10XX_AC12_SYSCTL2, sysctl2);
    }

    ls10xx_clock_set(hc, 1);

    hc->timing = timing;

    return(EOK);
}

static int ls10xx_sdhcx_signal_voltage(sdio_hc_t *hc, int signal_voltage)
{
    if((hc->version < LS10XX_HOSTVER_SPECVER_3) || (hc->signal_voltage == signal_voltage)) {
        return(EOK);
    }

    if((signal_voltage == SIGNAL_VOLTAGE_3_3) || (signal_voltage == SIGNAL_VOLTAGE_3_0)) {
        uint32_t pctl = ls10xx_in32(hc, LS10XX_PCTL);

        if (pctl & LS10XX_PCTL_VOLSEL_18V) {
            ls10xx_out32(hc, LS10XX_PCTL, pctl & ~LS10XX_PCTL_VOLSEL_18V);
            LS10XX_LOG(hc, _SLOG_INFO, "Switched ESDHC to  3.3V");
        } else {
            LS10XX_LOG(hc, _SLOG_INFO, "ESDHC already at  3.3V");
        }
    } else if(signal_voltage == SIGNAL_VOLTAGE_1_8) {
        uint32_t pctl = ls10xx_in32(hc, LS10XX_PCTL);

        // NOTE: The SDHC IO voltage need to be set before the switching the voltage. Either the
        // io voltage is set in the SCFG SDHCIOVSELCR register to the desired voltage, or the
        // SDHCIOVSELCR VSELVAL is set to Auto-voltage-selection, which is the case for the
        // ls1012a/ls1043a and it is set in the startup.
        if (pctl & LS10XX_PCTL_VOLSEL_18V) {
            LS10XX_LOG(hc, _SLOG_INFO, "ESDHC already at  1.8V");
        } else {
            // Disable SD clock
            ls10xx_clock_set(hc, 0);

            if (ls10xx_dataLines_wait(hc, 0)) {
                LS10XX_LOG(hc, _SLOG_WARNING,
                    "Switching to 1.8V: Warning: Data lines still High after clock gate");
            }

            ls10xx_out32(hc, LS10XX_PCTL, ls10xx_in32(hc, LS10XX_PCTL) | LS10XX_PCTL_VOLSEL_18V);
            delay(5);

            /* restore sd clock status */
            ls10xx_clock_set(hc, 1);

            /* Data lines should be high now */
            if (ls10xx_dataLines_wait(hc, 1)) {
                LS10XX_LOG(hc, _SLOG_ERROR, "ERROR: Failed to switch to 1.8V, DATA lines remain in low");
                return (EIO);
            }
            LS10XX_LOG(hc, _SLOG_INFO, "Switched ESDHC to  1.8V");
        }
    } else {
        return (EINVAL);
    }

    hc->signal_voltage = signal_voltage;

    return (EOK);
}

int ls10xx_send_tune_cmd(sdio_hc_t *hc, int op)
{
    struct sdio_cmd  cmd;
    int              tlen = (hc->bus_width == BUS_WIDTH_8) ? 128 : 64;
    int              status;
    sdio_sge_t       sge;
    uint8_t          buf[128];

    LS10XX_LOG(hc, _SLOG_DEBUG2, "");

    sge.sg_count = tlen;
    sge.sg_address = (paddr_t)&buf;

    memset((void *)&cmd, 0, sizeof(struct sdio_cmd));
    sdio_setup_cmd(&cmd, SCF_CTYPE_ADTC | SCF_RSP_R1, op, 0);

    /* Seems if not read the data out of the buffer, it will be some problems */
    sdio_setup_cmd_io(&cmd, SCF_DIR_IN, 1, tlen, &sge, 1, NULL);
    status = sdio_issue_cmd(&hc->device, &cmd, SDIO_TIME_DEFAULT);

    /* Command error */
    if (cmd.status > CS_CMD_CMP)
        status = EIO;

    return(status);
}

static int ls10xx_sdhcx_tune(sdio_hc_t *hc, int op)
{
    int status = EOK;
    int  retries = 0;
    uint32_t sysctl2;

    if ((status = ls10xx_clock_set(hc, 0)) != EOK) {
        return status;
    }

    ls10xx_out32(hc, LS10XX_ESDHC_CTRL,
            ls10xx_in32(hc, LS10XX_ESDHC_CTRL) | LS10XX_ESDHC_CTRL_FAF);

    ls10xx_waitmask(hc, LS10XX_ESDHC_CTRL, LS10XX_ESDHC_CTRL_FAF, 0, LS10XX_CLOCK_TIMEOUT);

    // Enable the tuning block
    ls10xx_out32(hc, LS10XX_TUNE_CTRL,
                    ls10xx_in32(hc, LS10XX_TUNE_CTRL) | LS10XX_TUNE_CTRL_ENABLE);

    if ((status = ls10xx_clock_set(hc, 1)) != EOK) {
        goto cleanup;
    }

    // Execute tuning procedures
    ls10xx_out32(hc, LS10XX_AC12_SYSCTL2,
            ls10xx_in32(hc, LS10XX_AC12_SYSCTL2) | LS10XX_SYSCTL2_EXTN);

    status = EIO;
    while (retries++ < LS10XX_TUNING_RETRIES) {
        delay(1);
        if ((status = ls10xx_send_tune_cmd(hc, op)) != EOK) {
            LS10XX_LOG(hc, _SLOG_ERROR, "Failed to send tuning command, status %d", status);
            break;
        }

        sysctl2 = ls10xx_in32(hc, LS10XX_AC12_SYSCTL2);
        if ((sysctl2 & LS10XX_SYSCTL2_SMPCLKSEL) && ((sysctl2 & LS10XX_SYSCTL2_EXTN) == 0)) {
            status = EOK;
            break;
        }
    }

cleanup:
    if (status == EOK) {
        LS10XX_LOG(hc, _SLOG_INFO, "Tuning finished successfully, %d", retries);
    } else {
        LS10XX_LOG(hc, _SLOG_ERROR, "Tuning failed, %d", retries);
        // Tuning failed, clear the excute tuning bit
        ls10xx_out32(hc, LS10XX_AC12_SYSCTL2,
                ls10xx_in32(hc, LS10XX_AC12_SYSCTL2) & ~LS10XX_SYSCTL2_EXTN);
    }

    // Disable the tuning block
    ls10xx_out32(hc, LS10XX_TUNE_CTRL,
            ls10xx_in32(hc, LS10XX_TUNE_CTRL) & ~LS10XX_TUNE_CTRL_ENABLE);
    return(status);
}

static int ls10xx_sdhcx_cd(sdio_hc_t *hc)
{
    uint32_t status;
    int      cstate = CD_RMV;

    if (hc->caps & HC_CAP_SLOT_TYPE_EMBEDDED) {
        return CD_INS;
    }

    if (ls10xx_waitmask(hc, LS10XX_PSTATE, LS10XX_CARD_STABLE, LS10XX_CARD_STABLE, 10000) != EOK){
            goto ret;
    }

    status = ls10xx_in32(hc, LS10XX_PSTATE);
    if (status & LS10XX_PSTATE_CI) {
        cstate |= CD_INS;
        if (!(status & LS10XX_PSTATE_WP)) {
            cstate |= CD_WP;
        } else {
            cstate &= ~CD_WP;
        }
    }

ret:
    return cstate;
}

int ls10xx_sdhcx_dinit(sdio_hc_t *hc)
{
    if (!hc || !hc->cs_hdl)
        return (EOK);

    ls10xx_hc_t *sdhc = (ls10xx_hc_t *) hc->cs_hdl;

    if( sdhc->base ) {
        ls10xx_sdhcx_pwr(hc, 0);
        ls10xx_reset(hc, LS10XX_SYSCTL_SRA);
        ls10xx_out32(hc, LS10XX_ISE, 0);
        ls10xx_out32(hc, LS10XX_IE, 0);

        if(hc->hc_iid != -1) {
            InterruptDetach(hc->hc_iid);
        }
        munmap_device_memory((void *)sdhc->base, hc->cfg.base_addr_size[0]);
    }

    if(sdhc->adma)
        munmap(sdhc->adma, sizeof(ls10xx_adma32_t) * ADMA_DESC_MAX);

    free(sdhc);
    hc->cs_hdl = NULL;

    return(EOK);
}

void ls10xx_capabilities_set(sdio_hc_t *hc) {
    uint32_t     cap;
    ls10xx_hc_t *sdhc = (ls10xx_hc_t *) hc->cs_hdl;

    // Read Cap register
    cap = ls10xx_in32(hc, LS10XX_CAP);

    hc->caps |= HC_CAP_BW4;

#ifndef LS10XX_CD_USE_SDIO_POLL_TIMER
    hc->caps |= HC_CAP_CD_INTR;
#endif

    if(cap & LS10XX_CAP_HS)
        hc->caps |= HC_CAP_HS;

    if(cap & LS10XX_CAP_DMA || cap & LS10XX_CAP_ADMAS)
        hc->caps |= HC_CAP_DMA;

    if (cap & LS10XX_CAP_SRS)
        hc->caps |= HC_CAP_SLEEP;

    if(cap & LS10XX_CAP_S18) {
        hc->ocr = OCR_VDD_17_195;
        hc->caps |= HC_CAP_SV_1_8V;
    }

    if(cap & LS10XX_CAP_S30) {
        hc->ocr  = OCR_VDD_30_31 | OCR_VDD_29_30;
        hc->caps |= HC_CAP_SV_3_0V;
    }

    if(cap & LS10XX_CAP_S33) {
        hc->ocr  = OCR_VDD_32_33 | OCR_VDD_33_34;
        hc->caps |= HC_CAP_SV_3_3V;
    }

    // Read Cap 2 register
    cap = ls10xx_in32(hc, LS10XX_CAP2);

    if(cap & LS10XX_CAP2_SDR50)
        hc->caps |= HC_CAP_SDR12 | HC_CAP_SDR25 | HC_CAP_SDR50;

    if(cap & LS10XX_CAP2_SDR104)
        hc->caps |= HC_CAP_SDR104;
        hc->caps |= HC_CAP_HS200;

    if(cap & LS10XX_CAP2_DDR50)
        hc->caps |= HC_CAP_DDR50;

    if(cap & LS10XX_CAP2_DTAS)
        hc->caps |= HC_CAP_DRV_TYPE_A;

    if(cap & LS10XX_CAP2_DTCS)
        hc->caps |= HC_CAP_DRV_TYPE_C;

    if(cap & LS10XX_CAP2_DTDS)
        hc->caps |= HC_CAP_DRV_TYPE_D;

    if(cap & LS10XX_CAP2_UTSDR50)
        sdhc->flags |= SF_TUNE_SDR50;

}

static sdio_hc_entry_t ls10xx_sdhcx_hc_entry ={ 16,
               ls10xx_sdhcx_dinit,
               NULL,
               ls10xx_sdhcx_cmd,
               ls10xx_sdhcx_abort,
               ls10xx_sdhcx_event,
               ls10xx_sdhcx_cd,
               ls10xx_sdhcx_pwr,
               ls10xx_sdhcx_clk,
               ls10xx_sdhcx_bus_mode,
               ls10xx_sdhcx_bus_width,
               ls10xx_sdhcx_timing,
               ls10xx_sdhcx_signal_voltage,
               NULL,
               NULL,
               ls10xx_sdhcx_tune,
               NULL
};

int ls10xx_sdhcx_init(sdio_hc_t *hc)
{
    sdio_hc_cfg_t       *cfg;
    ls10xx_hc_t         *sdhc = (ls10xx_hc_t *) hc->cs_hdl;
    struct sigevent     event;

    hc->hc_iid            = -1;
    cfg                   = &hc->cfg;

    memcpy(&hc->entry, &ls10xx_sdhcx_hc_entry, sizeof(sdio_hc_entry_t));

    if (!hc->clk_max)
        hc->clk_max = cfg->clk;

    if(!cfg->base_addr[0])
        return(ENODEV);

    if(!cfg->base_addr_size[0])
        cfg->base_addr_size[0] = LS10XX_SIZE;

    LS10XX_LOG(hc, _SLOG_INFO, "Base addr %#lx, irq %d, clk %d per_clk %d clk_max %d",
            cfg->base_addr[0], cfg->irq[0], cfg->clk, sdhc->per_clk, hc->clk_max);

    sdhc->base = (uintptr_t) mmap_device_io(cfg->base_addr_size[0], cfg->base_addr[0]);

    if( sdhc->base == (uintptr_t) MAP_FAILED ) {
        LS10XX_LOG(hc, _SLOG_ERROR, "SDHCI base mmap_device_io (%s)", strerror(errno));
        ls10xx_sdhcx_dinit(hc);
        return(errno);
    }

    if(!hc->version)
        hc->version = ls10xx_in32(hc, LS10XX_HOSTVER) & LS10XX_HOSTVER_SPECVER_MSK;

    ls10xx_capabilities_set(hc);

#ifdef ADMA_SUPPORTED
    if (hc->caps & HC_CAP_DMA) {
        if(hc->version >= LS10XX_HOSTVER_SPECVER_3) {
            LS10XX_LOG(hc, _SLOG_INFO, "Enabling ADMA");
            hc->cfg.sg_max    = ADMA_DESC_MAX;
            if((sdhc->adma = mmap(NULL, sizeof(ls10xx_adma32_t) * ADMA_DESC_MAX,
                    PROT_READ | PROT_WRITE | PROT_NOCACHE, MAP_PRIVATE | MAP_ANON | MAP_PHYS,
                    NOFD, 0)) == MAP_FAILED) {
                LS10XX_LOG(hc, _SLOG_ERROR, "ADMA mmap %s", strerror(errno));
                ls10xx_sdhcx_dinit(hc);
                return(errno);
            }
            sdhc->flags |= SF_USE_ADMA;
            sdhc->admap = sdio_vtop(sdhc->adma);
            if(hc->version >= LS10XX_HOSTVER_SPECVER_3) {
                hc->caps |= HC_CAP_ACMD23;
            }
        }
        else {
            LS10XX_LOG(hc, _SLOG_INFO, "Enabling SDMA");
            hc->cfg.sg_max = 1;
            sdhc->flags |= SF_USE_SDMA;
        }
        ls10xx_out32(hc, LS10XX_ESDHC_CTRL ,
                ls10xx_in32(hc, LS10XX_ESDHC_CTRL) | LS10XX_ESDHC_CTRL_SNOOP);
    }
#endif /* ADMA_SUPPORTED */
    hc->caps &= cfg->caps;        /* reconcile command line options */

    SIGEV_PULSE_INIT(&event, hc->hc_coid, SDIO_PRIORITY, HC_EV_INTR, NULL);
    if((hc->hc_iid = InterruptAttachEvent(cfg->irq[0], &event, _NTO_INTR_FLAGS_TRK_MSK)) == -1) {
        ls10xx_sdhcx_dinit(hc);
        return(errno);
    }

    /* Only enable the card insertion and removal interrupts */
    ls10xx_out32(hc, LS10XX_ISE, LS10XX_ISE_DFLTS);
#ifndef LS10XX_CD_USE_SDIO_POLL_TIMER
    ls10xx_cd_irq_enable(hc);
#endif
    return(EOK);
}
#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/devb/sdmmc/sdiodi/hc/ls10xx.c $ $Rev: 834210 $")
#endif
