/*
 * $QNXLicenseC:
 * Copyright 2016, 2017 QNX Software Systems.
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

// Module Description:  board specific interface

#include <internal.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <hw/inout.h>
#include <sys/mman.h>
#include <rcar_gen3.h>
#include <drvr/hwinfo.h>
#include <hw/hwinfo_rcar.h>

static void query_hwinfo(sdmmc_bs_ext *bsext, uint32_t idx)
{
    unsigned hwi_off, tag_idx;
    hwi_tag *tag;

    hwi_off = hwi_find_device(RCAR_HWI_SDHI_PWR, idx);
    if(hwi_off != HWI_NULL_OFF) {
        tag_idx = 0;
        tag = hwi_tag_find(hwi_off, HWI_TAG_NAME_location, 0);
        if(tag) {
            bsext->pwr_pbase = tag->location.base;
            bsext->pwr_size = tag->location.len;

            while((tag = hwi_tag_find(hwi_off, HWI_TAG_NAME_regname, &tag_idx)) != NULL) {
                if(strcmp(RCAR_HWI_SDHI_PWR_REG,  __hwi_find_string(tag->regname.regname)) == 0) {
                    bsext->pwr_offset = tag->regname.offset;
                } else if(strcmp(RCAR_HWI_SDHI_PWR_VDD,  __hwi_find_string(tag->regname.regname)) == 0) {
                    bsext->pwr_vdd = tag->regname.offset;
                } else if(strcmp(RCAR_HWI_SDHI_PWR_IF,  __hwi_find_string(tag->regname.regname)) == 0) {
                    bsext->pwr_if = tag->regname.offset;
                }
            }
        }
    }

    hwi_off = hwi_find_device(RCAR_HWI_SDHI_PFC, idx);
    if(hwi_off != HWI_NULL_OFF) {
        tag_idx = 0;
        tag = hwi_tag_find(hwi_off, HWI_TAG_NAME_location, 0);
        if(tag) {
            bsext->pfc_pbase = tag->location.base;
            bsext->pfc_size = tag->location.len;

            while((tag = hwi_tag_find(hwi_off, HWI_TAG_NAME_regname, &tag_idx)) != NULL) {
                if(strcmp(RCAR_HWI_SDHI_PFC_REG,  __hwi_find_string(tag->regname.regname)) == 0) {
                    bsext->pfc_offset = tag->regname.offset;
                } else if(strcmp(RCAR_HWI_SDHI_PFC_PMMR,  __hwi_find_string(tag->regname.regname)) == 0) {
                    bsext->pfc_pmmr = tag->regname.offset;
                } else if(strcmp(RCAR_HWI_SDHI_PFC_MASK,  __hwi_find_string(tag->regname.regname)) == 0) {
                    bsext->pfc_mask = tag->regname.offset;
                } else if(strcmp(RCAR_HWI_SDHI_PFC_SHIFT,  __hwi_find_string(tag->regname.regname)) == 0) {
                    bsext->pfc_mask <<= tag->regname.offset;
                }
            }
        }
    }
}

static int rcar_bs_cd(sdio_hc_t *hc)
{
    rcar_sdmmc_t    *sdmmc = (rcar_sdmmc_t *)hc->cs_hdl;
    sdmmc_bs_ext    *bsext = (sdmmc_bs_ext *)hc->bs_hdl;
    int             cstate, pstate;

    cstate = CD_RMV;

    if(hc->flags & HC_FLAG_DEV_MMC) {
        cstate = (CD_INS);
    } else {
        pstate = sdmmc_read(sdmmc->vbase, MMC_SD_INFO1);

        hc->caps |= HC_CAP_CD_INTR;

        if ((pstate & SDH_INFO1_CD)) {
            cstate  |= CD_INS;
            if (!(pstate & SDH_INFO1_WP) && !(bsext->nowp))
                cstate |= CD_WP;
        }
    }

    return cstate;
}

static int rcar_bs_signal_voltage(sdio_hc_t *hc, int signal_voltage)
{
    sdmmc_bs_ext    *bsext = (sdmmc_bs_ext *)hc->bs_hdl;
    uint32_t        pocctrl0;

    if (!bsext->pfc_base || !bsext->pwr_base) {
        /* No signal voltage line to use, just return EOK */
        return (EOK);
    }

    if (signal_voltage == SIGNAL_VOLTAGE_1_8)
        delay(10);

    // Signal IO voltage level
    pocctrl0  = in32(bsext->pfc_base + bsext->pfc_offset);
    if (signal_voltage == SIGNAL_VOLTAGE_3_3) {
        pocctrl0 |=  bsext->pfc_mask;
    } else {
        pocctrl0 &= ~bsext->pfc_mask;
    }
    // It's not safe to do a read modified write for common registers!
    out32(bsext->pfc_base + bsext->pfc_pmmr, ~pocctrl0);
    out32(bsext->pfc_base + bsext->pfc_offset, pocctrl0);

    if (in32(bsext->pfc_base + bsext->pfc_offset) != pocctrl0) {
        sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s: Failed to change signal level %d!", __func__, signal_voltage);
        return (EIO);
    }

    // Interface voltage control VDDQVA_SDx
    if (signal_voltage == SIGNAL_VOLTAGE_3_3)
        out32(bsext->pwr_base + bsext->pwr_offset, in32(bsext->pwr_base + bsext->pwr_offset) | (1 << bsext->pwr_if));
    else
        out32(bsext->pwr_base + bsext->pwr_offset, in32(bsext->pwr_base + bsext->pwr_offset) & ~(1 << bsext->pwr_if));
    hc->signal_voltage = signal_voltage;

    return (EOK);
}

static int rcar_sdhi_signal_voltage(sdio_hc_t *hc, int signal_voltage)
{
    rcar_sdmmc_t     *sdmmc;
    uint16_t        clkctl;
    uint16_t        info2;
    int             cnt;

    sdmmc = (rcar_sdmmc_t *)hc->cs_hdl;

    hc->signal_voltage = signal_voltage;

    if (signal_voltage == SIGNAL_VOLTAGE_3_3)
        return (rcar_bs_signal_voltage(hc, signal_voltage));

    for (cnt = SDHI_TMOUT; cnt >= 0; --cnt) {
        info2 = sdmmc_read(sdmmc->vbase, MMC_SD_INFO2);
        if (!(info2 & SDH_INFO2_CBSY) && (info2 & SDH_INFO2_SCLKDIVEN))
            break;

        nanospin_ns(1000);
    }
    if (cnt <= 0) {
        sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s: Busy state! Cannot change the voltage!", __func__);
        return (EAGAIN);
    }

    clkctl = 0x0100;
    if (sdmmc_read(sdmmc->vbase, MMC_SD_CLK_CTRL) & 0x0200)
        clkctl |= 0x0200;

    sdmmc_write(sdmmc->vbase, MMC_SD_CLK_CTRL, ~(clkctl) & sdmmc_read(sdmmc->vbase, MMC_SD_CLK_CTRL));

    /* Check to see if DAT[3:0] is 0000b  */
    if (sdmmc_read(sdmmc->vbase, MMC_SD_INFO2) & 0x0080) {
        sdio_slogf( _SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 1, "%s:  DAT0 is not LOW", __FUNCTION__);
        sdmmc_write(sdmmc->vbase, MMC_SD_CLK_CTRL, sdmmc_read(sdmmc->vbase, MMC_SD_CLK_CTRL) | clkctl);
        return (EIO);
    }

    rcar_bs_signal_voltage(hc, signal_voltage);

    /* Wait for at least 5ms and this depends on card manufacturer */
    delay(20);

    /* Start SDCLK */
    sdmmc_write(sdmmc->vbase, MMC_SD_CLK_CTRL, sdmmc_read(sdmmc->vbase, MMC_SD_CLK_CTRL) | 0x0100);
    cnt = 20;

    /* Check to see if DAT[3:0] is 1111b  */
    do {
        delay(10); /* Wait for 10ms */
    } while (!(sdmmc_read(sdmmc->vbase, MMC_SD_INFO2) & 0x0080) && (--cnt != 0));

    if (cnt == 0) {
        sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 1, "%s:  DAT0 is not HIGH", __FUNCTION__);
        return (EIO);
    }

    if (clkctl & 0x0200)
        sdmmc_write(sdmmc->vbase, MMC_SD_CLK_CTRL, sdmmc_read(sdmmc->vbase, MMC_SD_CLK_CTRL) | 0x0200);

    return (EOK);
}

static int rcar_bs_pwr(sdio_hc_t *hc, int vdd)
{
    rcar_sdmmc_t    *sdmmc = (rcar_sdmmc_t *)hc->cs_hdl;
    sdmmc_bs_ext    *bsext = (sdmmc_bs_ext *)hc->bs_hdl;

    sdmmc_write(sdmmc->vbase, MMC_SOFT_RST, (vdd == 0)? SOFT_RST_ON:SOFT_RST_OFF);
    delay(1);

    if (!bsext->pwr_base) {
        return (EOK);
    }

    if(!(hc->flags & HC_FLAG_DEV_MMC)) {
        if (vdd != 0)
            out32(bsext->pwr_base + bsext->pwr_offset, in32(bsext->pwr_base + bsext->pwr_offset) | (1 << bsext->pwr_vdd));
        else {
            out32(bsext->pwr_base + bsext->pwr_offset, in32(bsext->pwr_base + bsext->pwr_offset) & ~(1 << bsext->pwr_vdd));
        }
        delay(10); //need delay to reset and power up stably
    }

    bsext->rcar_pwr(hc, vdd);

    return (EOK);
}

static int my_getsubopt(char **optionp, char * const *tokens, char **valuep)
{
    char        *p, *opt;
    int         len, index;
    const char  *token;

    *valuep = 0;

    for (opt = p = *optionp, len = 0; *p && *p != ':'; p++, len++) {
        if (*p == '=') {
            for (*valuep = ++p; *p && *p != ':'; p++) {
                /* Nothing to do */
            }
            break;
        }
    }
    if (*p) {
        *p++ = '\0';
    }
    *optionp = p;

    for (index = 0; (token = *tokens++); index++) {
        if (*token == *opt && !strncmp(token, opt, len) && token[len] == '\0') {
            return index;
        }
    }
    *valuep = opt;
    return -1;
}

static int rcar_bs_args(sdio_hc_t *hc, char *options)
{
    sdmmc_bs_ext    *bsext = (sdmmc_bs_ext *)hc->bs_hdl;
    char            *value, *cp;
    int             opt;

    static char     *opts[] = {
                        "nowp",     /* no write protected capability */
                        "pwr",      /* pwr base reg, size, offset */
                        "pwr_vdd",  /* vdd pin */
                        "pwr_if",   /* vddqva pin */
                        "pfc",      /* signal IO base reg, size, offset, pmmr */
                        "pfc_mask", /* signal IO pin */
                        NULL
                    };

    bsext->nowp = 0;

    while (options && *options != '\0') {
        if ((opt = my_getsubopt(&options, opts, &value)) == -1) {
        }

        switch (opt) {
            case 0:
                bsext->nowp = 1;
                break;
            case 1:
                bsext->pwr_pbase = strtoul(value, NULL, 0);
                if ((cp = strchr(value, '/'))) {
                    value = cp+1;
                    bsext->pwr_size = strtoul(value, NULL, 0);
                }
                if ((cp = strchr(value, '/'))) {
                    value = cp+1;
                    bsext->pwr_offset = strtoul(value, NULL, 0);
                }
                break;
            case 2:
                bsext->pwr_vdd = strtoul(value, NULL, 0);
                break;
            case 3:
                bsext->pwr_if = strtoul(value, NULL, 0);
                break;
            case 4:
                bsext->pfc_pbase = strtoul(value, NULL, 0);
                if ((cp = strchr(value, '/'))) {
                    value = cp+1;
                    bsext->pfc_size = strtoul(value, NULL, 0);
                }
                if ((cp = strchr(value, '/'))) {
                    value = cp+1;
                    bsext->pfc_offset = strtoul(value, NULL, 0);
                }
                if ((cp = strchr(value, '/'))) {
                    value = cp+1;
                    bsext->pfc_pmmr = strtoul(value, NULL, 0);
                }
                break;
            case 5:
                bsext->pfc_mask = strtoul(value, NULL, 0);
                break;
            default:
                break;
        }
    }

    return (EOK);
}

static int rcar_bs_dinit(sdio_hc_t *hc)
{
    sdmmc_bs_ext *bsext = (sdmmc_bs_ext *)hc->bs_hdl;

    if (bsext->pwr_base) {
        munmap_device_io(bsext->pwr_base, bsext->pwr_size);
    }

    if (bsext->pfc_base) {
        munmap_device_io(bsext->pfc_base, bsext->pfc_size);
    }

    free(bsext);
    hc->bs_hdl = NULL;

    return rcar_sdmmc_dinit(hc);
}

static int rcar_bs_init(sdio_hc_t *hc)
{
    sdmmc_bs_ext    *bsext;
    sdio_hc_cfg_t   *cfg = &hc->cfg;

    if (!(bsext = calloc(1, sizeof(sdmmc_bs_ext))))
        return (ENOMEM);

    hc->bs_hdl = (void *)bsext;

    query_hwinfo(bsext, cfg->idx);
    if(rcar_bs_args(hc, cfg->options))
        return EINVAL;

    if(hc->flags & HC_FLAG_DEV_MMC) {
        /* emmc */
        sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s: EMMC", __FUNCTION__);
        hc->caps |= HC_CAP_SV_1_8V;
        hc->caps |= HC_CAP_XPC_1_8V;
        hc->ocr   = OCR_VDD_32_33 | OCR_VDD_33_34;

        if (rcar_sdmmc_init(hc) != EOK)
            goto fail2;
    } else {
        /* sd */
        sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s: SD", __FUNCTION__);

        if (bsext->pwr_pbase) {
            sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s: pwr base=0x%x offset=0x%x vdd=%d if=%d", __FUNCTION__, bsext->pwr_pbase, bsext->pwr_offset, bsext->pwr_vdd, bsext->pwr_if);
            if((bsext->pwr_base = mmap_device_io(bsext->pwr_size, bsext->pwr_pbase)) == (uintptr_t)MAP_FAILED) {
                goto fail2;
            }
        }

        if (bsext->pfc_pbase) {
            sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s: pfc base=0x%x offset=0x%x pmmr=0x%x mask=0x%x", __FUNCTION__, bsext->pfc_pbase, bsext->pfc_offset, bsext->pfc_pmmr, bsext->pfc_mask);
            if((bsext->pfc_base = mmap_device_io(bsext->pfc_size, bsext->pfc_pbase)) == (uintptr_t)MAP_FAILED) {
                goto fail1;
            }
            hc->caps |= HC_CAP_SV_1_8V | HC_CAP_XPC_1_8V;
        }

        hc->caps |= HC_CAP_SV_3_3V;
        hc->caps |= HC_CAP_XPC_3_3V;
        hc->ocr   = OCR_VDD_17_195 | OCR_VDD_32_33 | OCR_VDD_33_34;

        if (rcar_sdmmc_init(hc) != EOK)
            goto fail0;

        hc->entry.signal_voltage = rcar_sdhi_signal_voltage;
        hc->entry.dinit          = rcar_bs_dinit;
    }

    hc->entry.cd    = rcar_bs_cd;
    bsext->rcar_pwr = hc->entry.pwr;
    hc->entry.pwr   = rcar_bs_pwr;

    return EOK;

fail0:
    if (bsext->pfc_base) {
        munmap_device_io(bsext->pfc_base, bsext->pfc_size);
    }
fail1:
    if (bsext->pwr_base) {
        munmap_device_io(bsext->pwr_base, bsext->pwr_size);
    }
fail2:
    free(bsext);

    return (ENODEV);
}

sdio_product_t  sdio_fs_products[] = {
    { SDIO_DEVICE_ID_WILDCARD, 0, 0, "rcar_gen3", rcar_bs_init },
};

sdio_vendor_t   sdio_vendors[] = {
    { SDIO_VENDOR_ID_WILDCARD, "Renesas", sdio_fs_products },
    { 0, NULL, NULL }
};

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/devb/sdmmc/aarch64/rcar_gen3.le/bs.c $ $Rev: 848134 $")
#endif
