/*
 * $QNXLicenseC:
 * Copyright 2013,2017, QNX Software Systems.
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

#ifndef	_RCAR_SDHI_H_INCLUDED
#define	_RCAR_SDHI_H_INCLUDED

#include <internal.h>
#include <hw/dma.h>

#define SDHI_CHANNELS           3

/*
 * SDIO/MMC Memory-mapped registers
 */

#define SDHI_CMD			0x0000
#define SDHI_PORTSEL			0x0004
#define SDHI_ARG0			0x0008
#define SDHI_ARG1			0x000C
#define SDHI_STOP			0x0010
#define SDHI_SECCNT			0x0014
#define SDHI_RSP00			0x0018
#define SDHI_RSP01			0x001C
#define SDHI_RSP02			0x0020
#define SDHI_RSP03			0x0024
#define SDHI_RSP04			0x0028
#define SDHI_RSP05			0x002C
#define SDHI_RSP06			0x0030
#define SDHI_RSP07			0x0034
#define SDHI_INFO1			0x0038
#define SDHI_INFO2			0x003C
#define SDHI_INFO1_MASK			0x0040
#define SDHI_INFO2_MASK			0x0044
#define SDHI_CLK_CTRL			0x0048
#define SDHI_SIZE			0x004C
#define SDHI_OPTION			0x0050
#define SDHI_ERR_STS1			0x0058
#define SDHI_ERR_STS2			0x005C
#define SDHI_BUF0			0x0060
#define SDHI_SDIO_MODE			0x0068
#define SDHI_SDIO_INFO1			0x006C
#define SDHI_SDIO_INFO1_MASK		0x0070
#define SDHI_CC_EXT_MODE		0x01B0
#define SDHI_SOFT_RST			0x01C0
#define SDHI_VERSION			0x01C4
#define SDHI_EXT_ACC			0x01C8
#define SDHI_SDIF_MODE			0x01CC
#define SDHI_EXT_SWAP			0x01E0

#define SDHI_DMACR_XXXX			0x0324
#define SDHI_DMACR_490C			0x01CC

#define	MMC_DM_CM_SEQ_MODE		0x0808
#define	MMC_DM_CM_DTRAN_MODE		0x0820
#define	MMC_DM_CM_DTRAN_CTRL		0x0828
#define	MMC_DM_CM_RST			0x0830
#define	MMC_DM_CM_INFO1			0x0840
#define	MMC_DM_CM_INFO1_MASK		0x0848
#define	MMC_DM_CM_INFO2			0x0850
#define	MMC_DM_CM_INFO2_MASK		0x0858
#define	MMC_DM_DTRAN_ADDR		0x0880

/* SCC registers */
#define SD_SCC_DTCNTL			0x300
#define SD_SCC_TAPSET			0x304
#define SD_SCC_DT2FF			0x308
#define SD_SCC_CKSEL			0x30C
#define SD_SCC_RVSCNTL			0x310
#define SD_SCC_RVSREQ			0x314

#define	MMC_SCC_DTCNTL			0x1000	//32
#define	MMC_SCC_TAPSET			0x1008	//32
#define	MMC_SCC_DT2FF			0x1010	//32
#define	MMC_SCC_CKSEL			0x1018	//32
#define	MMC_SCC_RVSCNTL			0x1020	//32
#define	MMC_SCC_RVSREQ			0x1028	//32
#define	MMC_SCC_TMPPORT2		0x1038	//32
#define	MMC_SCC_TMPPORT3		0x1048	//32

#define	SDMMC_SCC_DTCNTL(sdhi)  ((((sdhi->rev & RCAR_SDHI_VER_MASK) == 0xCC10)||((sdhi->rev & RCAR_SDHI_VER_MASK) == 0xCD10)) ? MMC_SCC_DTCNTL  : SD_SCC_DTCNTL)
#define	SDMMC_SCC_TAPSET(sdhi)  ((((sdhi->rev & RCAR_SDHI_VER_MASK) == 0xCC10)||((sdhi->rev & RCAR_SDHI_VER_MASK) == 0xCD10)) ? MMC_SCC_TAPSET  : SD_SCC_TAPSET)
#define	SDMMC_SCC_DT2FF(sdhi)   ((((sdhi->rev & RCAR_SDHI_VER_MASK) == 0xCC10)||((sdhi->rev & RCAR_SDHI_VER_MASK) == 0xCD10)) ? MMC_SCC_DT2FF	: SD_SCC_DT2FF)
#define	SDMMC_SCC_CKSEL(sdhi)   ((((sdhi->rev & RCAR_SDHI_VER_MASK) == 0xCC10)||((sdhi->rev & RCAR_SDHI_VER_MASK) == 0xCD10)) ? MMC_SCC_CKSEL	: SD_SCC_CKSEL)
#define	SDMMC_SCC_RVSCNTL(sdhi) ((((sdhi->rev & RCAR_SDHI_VER_MASK) == 0xCC10)||((sdhi->rev & RCAR_SDHI_VER_MASK) == 0xCD10)) ? MMC_SCC_RVSCNTL : SD_SCC_RVSCNTL)
#define	SDMMC_SCC_RVSREQ(sdhi)  ((((sdhi->rev & RCAR_SDHI_VER_MASK) == 0xCC10)||((sdhi->rev & RCAR_SDHI_VER_MASK) == 0xCD10)) ? MMC_SCC_RVSREQ  : SD_SCC_RVSREQ)

// Command register bits
#define SDHI_CMD_AC12            (0 << 14)   // CMD12 is automatically issued
#define SDHI_CMD_NOAC12          (1 << 14)
#define SDHI_CMD_DAT_MULTI       (1 << 13)   // multi block transfer
#define SDHI_CMD_DAT_READ        (1 << 12)   // read
#define SDHI_CMD_ADTC            (1 << 11)   // command with data
#define SDHI_CMD_NORSP           (3 <<  8)   // no response
#define SDHI_CMD_RSPR1           (4 <<  8)   // R1, R5, R6, R7
#define SDHI_CMD_RSPR1B          (5 <<  8)   // R1b
#define SDHI_CMD_RSPR2           (6 <<  8)   // R2
#define SDHI_CMD_RSPR3           (7 <<  8)   // R3, R4
#define SDHI_CMD_ACMD            (6 <<  8)   // ACMD

// Stop register
#define SDHI_STOP_STP            (1 <<  0)
#define SDHI_STOP_SEC            (1 <<  8)

// CLK_CTRL register
#define SDHI_CLKCTRL_SCLKEN      (1 <<  8)

// INFO1
#define SDHI_INFO1_WP            (1 <<  7)   // write protect
#define SDHI_INFO1_CD            (1 <<  5)   // card detection state
#define SDHI_INFO1_INST          (1 <<  4)   // card insertion
#define SDHI_INFO1_RMVL          (1 <<  3)   // card removal
#define SDHI_INFO1_AE            (1 <<  2)   // access end
#define SDHI_INFO1_RE            (1 <<  0)   // response end

// INFO2
#define SDHI_INFO2_ILA           (1 << 15)   // illegal access error
#define SDHI_INFO2_CBSY          (1 << 14)   // command response busy
#define SDHI_INFO2_SCLKDIVEN     (1 << 13)   // SD bus busy
#define SDHI_INFO2_BWE           (1 <<  9)   // SD_BUF Write Enable
#define SDHI_INFO2_BRE           (1 <<  8)   // SD_BUF Read Enable
#define SDHI_INFO2_RTO           (1 <<  6)   // Response Timeout
#define SDHI_INFO2_BIRA          (1 <<  5)   // SD_BUF Illegal Read Access
#define SDHI_INFO2_BIWA          (1 <<  4)   // SD_BUF Illegal Write Access
#define SDHI_INFO2_DTO           (1 <<  3)   // Timeout (except response timeout)
#define SDHI_INFO2_ENDE          (1 <<  2)   // END Error
#define SDHI_INFO2_CRCE          (1 <<  1)   // CRC Error
#define SDHI_INFO2_CMDE          (1 <<  0)   // CMD Error
#define SDHI_INFO2_ALL_ERR       (SDHI_INFO2_CMDE | SDHI_INFO2_CRCE | SDHI_INFO2_ENDE | SDHI_INFO2_DTO |    \
                                                  SDHI_INFO2_BIWA | SDHI_INFO2_BIRA | SDHI_INFO2_RTO)

// SD Card Access Control Option Register (SD_OPTION) bit defination
#define SDHI_OPTION_WIDTH_1	(1 << 15)   // Data Bus Width 1 bit
#define SDHI_OPTION_WIDTH_8	(1 << 13)   // Data Bus Width 8 bit

// DMA Mode Enable Register (CC_EXT_MODE) bit defination
#define SDHI_CC_EXT_DMASDRW	(1 <<  1)   // The SD_BUF read/write DMA transfer is enabled

/* DM_CM_DTRAN_MODE */
#define CH_NUM_DOWNSTREAM	(0 << 16)
#define CH_NUM_UPSTREAM		(1 << 16)
#define BUS_WID_64BIT		(3 << 4)
#define FIXED_ADDRESS		(0 << 0)
#define INCREMENT_ADDRESS	(1 << 0)

/* DM_CM_DTRAN_CTRL */
#define DM_START		(1 << 0)
#define DM_INFO2_DTRAN_ERR0	(1 << 16)
#define DM_INFO2_DTRAN_ERR1	(1 << 17)
#define DM_INFO1_DTRAN_END0	(1 << 16)
#define DM_INFO1_DTRAN_END1	(1 << 17)

/* Definitions for values the RCAR_SDHI_SCC_DTCNTL register */
#define RCAR_SDHI_SCC_DTCNTL_TAPEN      (1 << 0)
/* Definitions for values the RCAR_SDHI_SCC_CKSEL register */
#define RCAR_SDHI_SCC_CKSEL_DTSEL       (1 << 0)
/* Definitions for values the RCAR_SDHI_SCC_RVSCNTL register */
#define RCAR_SDHI_SCC_RVSCNTL_RVSEN     (1 << 0)
/* Definitions for values the RCAR_SDHI_SCC_RVSREQ register */
#define RCAR_SDHI_SCC_RVSREQ_RVSERR     (1 << 2)

#define RCAR_SDHI_HAS_UHS_SCC       1

#define RCAR_SDHI_MAX_TAP           3

/* Maximum number DMA transfer size */
#define RCAR_SDHI_DMA_XMIT_SZ_MAX   6

#define SDHI_TMOUT                  1000000
#define RCAR_SDHI_TUNING_TIMEOUT    150
#define RCAR_SDHI_TUNING_RETRIES    40

/* Product device */
#define PRODUCT_REGISTER    0xFF000044
#define PRODUCT_CUT_MASK    0x00007FF0
#define PRODUCT_H2_BIT      (0x45 << 8)

// Revision register
#define RCAR_SDHI_VER_490C      0x490C
#define RCAR_SDHI_VER_CB0D      0xCB0D
#define RCAR_SDHI_VER_CC0D      0xCC0D
#define RCAR_SDHI_VER_CC10      0xCC10
#define RCAR_SDHI_VER_CD10      0xCD10
#define RCAR_SDHI_VER_MASK      0xFFFF

typedef struct _sdhi_cfg_t
{
    uint32_t    base;
    int         tx_mid_rid;
    int         rx_mid_rid;
    int         clock;
    int         irq;
    int         bus_shift;
} sdhi_cfg_t;

typedef	struct _rcar_sdhi_t {
    void            *bshdl;
    uint32_t        flags;
    uint32_t        pclk;
    int             bus_shift;
    int             card_insert;    // This is to avoid insert and then remove interrupt
    uint32_t        busclk;

    int             irq;
    paddr_t         pbase;
    uintptr_t       base;
    int             tx_mid_rid;
    int             rx_mid_rid;
    uint16_t        rev;            // Revision

    uint16_t        blksz;
    uint16_t        mask1;
    uint16_t        mask2;

    dma_functions_t dmafuncs;
    void            *dma;
#define DMA_DESC_MAX    256
    sdio_sge_t      sgl[DMA_DESC_MAX];
} rcar_sdhi_t;

static inline uint16_t rcar_sdh_read(rcar_sdhi_t *sdhi, int reg)
{
    return in16(sdhi->base + (reg << sdhi->bus_shift));
}

static inline void rcar_sdh_write(rcar_sdhi_t *sdhi, int reg, uint16_t val)
{
    out16(sdhi->base + (reg << sdhi->bus_shift), val);
}

static inline uint32_t rcar_sdh_read32(rcar_sdhi_t *sdhi, int reg)
{
    return in32(sdhi->base + reg);
}

static inline void rcar_sdh_write32(rcar_sdhi_t *sdhi, int reg, uint32_t val)
{
    out32(sdhi->base + reg, val);
}

extern int rcar_sdhi_init(sdio_hc_t *hc);
extern int rcar_sdhi_dinit(sdio_hc_t *hc);

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/devb/sdmmc/sdiodi/hc/rcar_sdhi.h $ $Rev: 835556 $")
#endif
