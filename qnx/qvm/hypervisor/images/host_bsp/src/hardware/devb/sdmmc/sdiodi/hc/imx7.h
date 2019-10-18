/*
 * $QNXLicenseC:
 * Copyright 2013, QNX Software Systems.
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

#ifndef IMX7_HC_H_
#define IMX7_HC_H_

#include <internal.h>

#define IMX7_SDHCX_TUNING_RETRIES       40
#define IMX7_SDHCX_CLOCK_TIMEOUT        10000
#define IMX7_SDHCX_COMMAND_TIMEOUT      1000000  /* The card could take very long time to process the transfered data */
#define IMX7_SDHCX_TRANSFER_TIMEOUT     1000000

#define IMX7_CLOCK_DEFAULT              196364000

#define IMX7_SDHCX_ADMA2_MAX_XFER       (1024 * 60)

/** 32 bit ADMA descriptor definition */
typedef struct _imx_sdhcx_adma32_t {
    uint16_t    attr;
    uint16_t    len;
    uint32_t    addr;
} imx_sdhcx_adma32_t;

#define IMX7_SDHCX_ADMA2_VALID          (1 << 0)        /* Valid */
#define IMX7_SDHCX_ADMA2_END            (1 << 1)        /* End of descriptor, transfer complete interrupt will be generated */
#define IMX7_SDHCX_ADMA2_INT            (1 << 2)        /* Generate DMA interrupt, will not be used */
#define IMX7_SDHCX_ADMA2_NOP            (0 << 4)        /* No OP, go to the next descriptor */
#define IMX7_SDHCX_ADMA2_SET            (1 << 4)        /* No OP, go to the next descriptor */
#define IMX7_SDHCX_ADMA2_TRAN           (2 << 4)        /* Transfer data */
#define IMX7_SDHCX_ADMA2_LINK           (3 << 4)        /* Link to another descriptor */

/** Processor specific structure */
typedef struct _imx_usdhcx_hc {
    void                        *bshdl;
    uintptr_t                   base;
    imx_sdhcx_adma32_t*        adma;
    uint32_t                    admap;
#define SF_USE_SDMA             0x01
#define SF_USE_ADMA             0x02
#define SF_TUNE_SDR50           0x04
#define SF_SDMA_ACTIVE          0x10
#define ADMA_DESC_MAX           256
    sdio_sge_t                  sgl[ADMA_DESC_MAX];
    uint32_t                    flags;
    int                         sdma_iid;       /**< SDMA interrupt id */
    uintptr_t                   sdma_irq;
    uintptr_t                   sdma_base;
    uint32_t                    mix_ctrl;
    uint32_t                    intmask;
    _Uint64t                    usdhc_addr;     /** Used to determine which controller it belongs to */
} imx_sdhcx_hc_t;

extern int imx7_sdhcx_init(sdio_hc_t *hc);
extern int imx7_sdhcx_dinit(sdio_hc_t *hc);

#endif /* IMX7_HC_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/devb/sdmmc/sdiodi/hc/imx7.h $ $Rev: 840817 $")
#endif
