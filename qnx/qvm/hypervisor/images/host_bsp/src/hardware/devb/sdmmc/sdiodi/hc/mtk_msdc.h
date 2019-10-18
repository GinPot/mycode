/*
 * $QNXLicenseC:
 * Copyright 2018, QNX Software Systems.
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

#ifndef _MTK_MSDC_H_INCLUDED
#define _MTK_MSDC_H_INCLUDED

#include <internal.h>
#include <hw/dma.h>



#define MTK_MSDC_TUNING_RETRIES   40
#define MTK_MSDC_CLOCK_TIMEOUT    10000
#define MTK_MSDC_COMMAND_TIMEOUT  1000000  /* The card could take very long time to process the transfered data */
#define MTK_MSDC_TRANSFER_TIMEOUT 1000000



#define MAX_BD_NUM          1024

/*--------------------------------------------------------------------------*/
/* Common Definition                                                        */
/*--------------------------------------------------------------------------*/
#define MSDC_BUS_1BITS          0x0
#define MSDC_BUS_4BITS          0x1
#define MSDC_BUS_8BITS          0x2

/*--------------------------------------------------------------------------*/
/* Register Offset                                                          */
/*--------------------------------------------------------------------------*/
#define MSDC_CFG         0x0
#define MSDC_IOCON       0x04
#define MSDC_PS          0x08
#define MSDC_INT         0x0c
#define MSDC_INTEN       0x10
#define MSDC_FIFOCS      0x14
#define SDC_CFG          0x30
#define SDC_CMD          0x34
#define SDC_ARG          0x38
#define SDC_STS          0x3c
#define SDC_RESP0        0x40
#define SDC_RESP1        0x44
#define SDC_RESP2        0x48
#define SDC_RESP3        0x4c
#define SDC_BLK_NUM      0x50
#define SDC_VOL_CHG      0x54
#define SDC_CSTS         0x58
#define SDC_CSTS_EN      0x5c
#define SDC_DATCRC_STS   0x60
#define SDC_ADV_CFG0     0x64
#define EMMC_CFG0        0x70
#define EMMC_CFG1        0x74
#define EMMC_STS         0x78
#define EMMC_IOCON       0x7c
#define SDC_ACMD_RESP    0x80
#define SDC_ACMD19_TRG   0x84
#define SDC_ACMD19_STS   0x88
#define MSDC_DMA_SA_H4   0x8c
#define MSDC_DMA_SA      0x90
#define MSDC_DMA_CA      0x94
#define MSDC_DMA_CTRL    0x98
#define MSDC_DMA_CFG     0x9c
#define MSDC_SW_DBG_SEL  0xa0
#define MSDC_SW_DBG_OUT  0xa4
#define MSDC_DMA_LENGTH  0xa8
#define MSDC_PATCH_BIT   0xb0
#define MSDC_PATCH_BIT0  0xb0
#define MSDC_PATCH_BIT1  0xb4
#define MSDC_PATCH_BIT2  0xb8
#define MSDC_PAD_TUNE0   0xf0
#define MSDC_PAD_TUNE1   0xf4
#define MSDC_DAT_RD_DLY0 0xf8
#define MSDC_DAT_RD_DLY1 0xfc
#define MSDC_DAT_RD_DLY2 0x100
#define MSDC_DAT_RD_DLY3 0x104
#define PAD_CTL0         0x180
#define PAD_DS_CTL0      0x184
#define PAD_DS_TUNE      0x188
#define PAD_CMD_TUNE     0x18c
#define PAD_DAT01_TUNE   0x190
#define PAD_DAT23_TUNE   0x194
#define PAD_DAT45_TUNE   0x198
#define PAD_DAT67_TUNE   0x19c
#define EMMC51_CFG0      0x204
#define EMMC50_CFG0      0x208
#define EMMC50_CFG1      0x20c
#define EMMC50_CFG2      0x21c
#define EMMC50_CFG3      0x220
#define EMMC50_CFG4      0x224
#define SDC_FIFO_CFG     0x228

/*--------------------------------------------------------------------------*/
/* Register Mask                                                            */
/*--------------------------------------------------------------------------*/

/* MSDC_CFG mask */
#define MSDC_CFG_MODE           (0x1 << 0)  /* RW */
#define MSDC_CFG_CKPDN          (0x1 << 1)  /* RW */
#define MSDC_CFG_RST            (0x1 << 2)  /* RW */
#define MSDC_CFG_PIO            (0x1 << 3)  /* RW */
//#define MSDC_CFG_CKDRVEN        (0x1 << 4)  /* RW */
#define MSDC_CFG_BV18SDT        (0x1 << 5)  /* RW */
#define MSDC_CFG_BV18PSS        (0x1 << 6)  /* R  */
#define MSDC_CFG_CKSTB          (0x1 << 7)  /* R  */
#define MSDC_CFG_CKDIV          (0xfff << 8) /* RW */
#define MSDC_CFG_CKMOD          (0x3 << 20) /* RW   */
#define MSDC_CFG_HS400_CK_MODE  (0x1 << 22) /* RW */
#define MSDC_CFG_START_BITY     (0x3 << 23) /* RW */
#define MSDC_CFG_SCLKSTOPSEL	(0x1 << 25) /* RW */

/* MSDC_IOCON mask */
#define MSDC_IOCON_sdmmc104CKS  (0x1 << 0)  /* RW */
#define MSDC_IOCON_RSPL         (0x1 << 1)  /* RW */
#define MSDC_IOCON_DSPL         (0x1 << 2)  /* RW */
#define MSDC_IOCON_DDLSEL       (0x1 << 3)  /* RW */
#define MSDC_IOCON_DDR50CKD     (0x1 << 4)  /* RW */
#define MSDC_IOCON_DSPLSEL      (0x1 << 5)  /* RW */
#define MSDC_IOCON_W_DSPL       (0x1 << 8)  /* RW */
#define MSDC_IOCON_D0SPL        (0x1 << 16) /* RW */
#define MSDC_IOCON_D1SPL        (0x1 << 17) /* RW */
#define MSDC_IOCON_D2SPL        (0x1 << 18) /* RW */
#define MSDC_IOCON_D3SPL        (0x1 << 19) /* RW */
#define MSDC_IOCON_D4SPL        (0x1 << 20) /* RW */
#define MSDC_IOCON_D5SPL        (0x1 << 21) /* RW */
#define MSDC_IOCON_D6SPL        (0x1 << 22) /* RW */
#define MSDC_IOCON_D7SPL        (0x1 << 23) /* RW */
#define MSDC_IOCON_RISCSZ       (0x3 << 24) /* RW */

/* MSDC_PS mask */
#define MSDC_PS_CDEN            (0x1 << 0)  /* RW */
#define MSDC_PS_CDSTS           (0x1 << 1)  /* R  */
#define MSDC_PS_CDDEBOUNCE      (0xf << 12) /* RW */
#define MSDC_PS_DAT             (0xff << 16)    /* R  */
#define MSDC_PS_CMD             (0x1 << 24) /* R  */
#define MSDC_PS_WP              (0x1 << 31) /* R  */

/* MSDC_INT mask */
#define MSDC_INT_MMCIRQ         (0x1 << 0)  /* W1C */
#define MSDC_INT_CDSC           (0x1 << 1)  /* W1C */
#define MSDC_INT_ACMDRDY        (0x1 << 3)  /* W1C */
#define MSDC_INT_ACMDTMO        (0x1 << 4)  /* W1C */
#define MSDC_INT_ACMDCRCERR     (0x1 << 5)  /* W1C */
#define MSDC_INT_DMAQ_EMPTY     (0x1 << 6)  /* W1C */
#define MSDC_INT_SDIOIRQ        (0x1 << 7)  /* W1C */
#define MSDC_INT_CMDRDY         (0x1 << 8)  /* W1C */
#define MSDC_INT_CMDTMO         (0x1 << 9)  /* W1C */
#define MSDC_INT_RSPCRCERR      (0x1 << 10) /* W1C */
#define MSDC_INT_CSTA           (0x1 << 11) /* R */
#define MSDC_INT_XFER_COMPL     (0x1 << 12) /* W1C */
#define MSDC_INT_DXFER_DONE     (0x1 << 13) /* W1C */
#define MSDC_INT_DATTMO         (0x1 << 14) /* W1C */
#define MSDC_INT_DATCRCERR      (0x1 << 15) /* W1C */
#define MSDC_INT_ACMD19_DONE    (0x1 << 16) /* W1C */
#define MSDC_INT_DMA_BDCSERR    (0x1 << 17) /* W1C */
#define MSDC_INT_DMA_GPDCSERR   (0x1 << 18) /* W1C */
#define MSDC_INT_DMA_PROTECT    (0x1 << 19) /* W1C */

/* MSDC_INTEN mask */
#define MSDC_INTEN_MMCIRQ       (0x1 << 0)  /* RW */
#define MSDC_INTEN_CDSC         (0x1 << 1)  /* RW */
#define MSDC_INTEN_ACMDRDY      (0x1 << 3)  /* RW */
#define MSDC_INTEN_ACMDTMO      (0x1 << 4)  /* RW */
#define MSDC_INTEN_ACMDCRCERR   (0x1 << 5)  /* RW */
#define MSDC_INTEN_DMAQ_EMPTY   (0x1 << 6)  /* RW */
#define MSDC_INTEN_SDIOIRQ      (0x1 << 7)  /* RW */
#define MSDC_INTEN_CMDRDY       (0x1 << 8)  /* RW */
#define MSDC_INTEN_CMDTMO       (0x1 << 9)  /* RW */
#define MSDC_INTEN_RSPCRCERR    (0x1 << 10) /* RW */
#define MSDC_INTEN_CSTA         (0x1 << 11) /* RW */
#define MSDC_INTEN_XFER_COMPL   (0x1 << 12) /* RW */
#define MSDC_INTEN_DXFER_DONE   (0x1 << 13) /* RW */
#define MSDC_INTEN_DATTMO       (0x1 << 14) /* RW */
#define MSDC_INTEN_DATCRCERR    (0x1 << 15) /* RW */
#define MSDC_INTEN_ACMD19_DONE  (0x1 << 16) /* RW */
#define MSDC_INTEN_DMA_BDCSERR  (0x1 << 17) /* RW */
#define MSDC_INTEN_DMA_GPDCSERR (0x1 << 18) /* RW */
#define MSDC_INTEN_DMA_PROTECT  (0x1 << 19) /* RW */
#define MSDC_INTEN_AXIRSPERR    (0x1 << 23) /* RW */

/* MSDC_FIFOCS mask */
#define MSDC_FIFOCS_RXCNT       (0xff << 0) /* R */
#define MSDC_FIFOCS_TXCNT       (0xff << 16)    /* R */
#define MSDC_FIFOCS_CLR         (0x1 << 31) /* RW */

/* SDC_CFG mask */
#define SDC_CFG_SDIOINTWKUP     (0x1 << 0)  /* RW */
#define SDC_CFG_INSWKUP         (0x1 << 1)  /* RW */
#define SDC_CFG_WRDTOC          (0x1fff << 2)    /* RW */
#define SDC_CFG_BUSWIDTH        (0x3 << 16) /* RW */
#define SDC_CFG_SDIO            (0x1 << 19) /* RW */
#define SDC_CFG_SDIOIDE         (0x1 << 20) /* RW */
#define SDC_CFG_INTATGAP        (0x1 << 21) /* RW */
#define SDC_CFG_DTOC            (0xff << 24)    /* RW */

/* SDC_STS mask */
#define SDC_STS_SDCBUSY         (0x1 << 0)  /* RW */
#define SDC_STS_CMDBUSY         (0x1 << 1)  /* RW */
#define SDC_STS_SWR_COMPL       (0x1 << 31) /* RW */

/* SDC_ADV_CFG0 mask */
#define SDC_ADV_CFG0_RX_ENHANCE_EN (0X1 << 20)

/* EMMC_IOCON mask */
#define EMMC_IOCON_BOOT_RST     (0x1 << 0)  /* RW */

/* MSDC_DMA_CTRL mask */
#define MSDC_DMA_CTRL_START     (0x1 << 0)  /* W */
#define MSDC_DMA_CTRL_STOP      (0x1 << 1)  /* W */
#define MSDC_DMA_CTRL_RESUME    (0x1 << 2)  /* W */
#define MSDC_DMA_CTRL_MODE      (0x1 << 8)  /* RW */
#define MSDC_DMA_CTRL_LASTBUF   (0x1 << 10) /* RW */
#define MSDC_DMA_CTRL_BRSTSZ    (0x7 << 12) /* RW */
#define MSDC_DMA_CTRL_BRSTSZ_SHIFT  (12)
#define     MSDC_DMA_CTRL_BRST_8B   (3)
#define     MSDC_DMA_CTRL_BRST_16B  (4)
#define     MSDC_DMA_CTRL_BRST_32B  (5)
#define     MSDC_DMA_CTRL_BRST_64B  (6)

/* MSDC_DMA_CFG mask */
#define MSDC_DMA_CFG_STS        (0x1 << 0)  /* R */
#define MSDC_DMA_CFG_DECSEN     (0x1 << 1)  /* RW */
#define MSDC_DMA_CFG_AHBHPROT2  (0x2 << 8)  /* RW */
#define MSDC_DMA_CFG_ACTIVEEN   (0x2 << 12) /* RW */
#define MSDC_DMA_CFG_CS12B16B   (0x1 << 16) /* RW */

/* MSDC_PATCH_BIT mask */
#define MSDC_PATCH_BIT_ODDSUPP    (0x1 <<  1)   /* RW */
#define MSDC_INT_DAT_LATCH_CK_SEL (0x7 <<  7)
#define MSDC_CKGEN_MSDC_DLY_SEL   (0x1f << 10)
#define MSDC_PATCH_BIT_IODSSEL    (0x1 << 16)   /* RW */
#define MSDC_PATCH_BIT_IOINTSEL   (0x1 << 17)   /* RW */
#define MSDC_PATCH_BIT_BUSYDLY    (0xf << 18)   /* RW */
#define MSDC_PATCH_BIT_WDOD       (0xf << 22)   /* RW */
#define MSDC_PATCH_BIT_IDRTSEL    (0x1 << 26)   /* RW */
#define MSDC_PATCH_BIT_CMDFSEL    (0x1 << 27)   /* RW */
#define MSDC_PATCH_BIT_INTDLSEL   (0x1 << 28)   /* RW */
#define MSDC_PATCH_BIT_SPCPUSH    (0x1 << 29)   /* RW */
#define MSDC_PATCH_BIT_DECRCTMO   (0x1 << 30)   /* RW */

/* MSDC_PATCH_BIT1 mask */
#define MSDC_PATCH_BIT1_FORCE_STOP_GDMA (0x1 << 17)  /* RW */
#define MSDC_PATCH_BIT1_RESET_GDMA      (0x1 << 15)  /* RW */
#define MSDC_PATCH_BIT1_STOP_DLY_SEL    (0xf << 8)   /* RW */
#define MSDC_PATCH_BIT1_BUSY_CHECK_SEL  (0x1 << 7)   /* RW */

/* MSDC_PATCH_BIT2 mask */
#define MSDC_PATCH_BIT2_RESPWAIT  (0x3 <<  2)
#define MSDC_PATCH_BIT2_CFGRESP   (0x1 << 15)
#define MSDC_PATCH_BIT2_CFGCRCSTS (0x1 << 28)

/* MSDC_PAD_TUNE/MSDC_PAD_TUNE0 mask */
#define MSDC_PAD_TUNE_DATWRDLY    (0x1f <<  0)  /* RW */
#define MSDC_PAD_TUNE_DATRRDLY    (0x1f <<  8)  /* RW */
#define MSDC_PAD_TUNE_RD_SEL      (0x1  << 13)  /* RW */
#define MSDC_PAD_TUNE_RXDLYSEL    (0x1  << 15)  /* RW */
#define MSDC_PAD_TUNE_CMDRDLY     (0x1f << 16)  /* RW */
#define MSDC_PAD_TUNE_CMD_SEL     (0x1  << 21)  /* RW */
#define MSDC_PAD_TUNE_CMDRRDLY    (0x1f << 22)  /* RW */
#define MSDC_PAD_TUNE_CLKTDLY     (0x1f << 27)  /* RW */

#define PAD_DS_TUNE_DLY1          (0x1f << 2)   /* RW */
#define PAD_DS_TUNE_DLY2          (0x1f << 7)   /* RW */
#define PAD_DS_TUNE_DLY3          (0x1f << 12)  /* RW */

#define PAD_CMD_TUNE_RX_DLY3      (0x1f << 1)  /* RW */

/* EMMC50_CFG mask */
#define EMMC50_CFG_PADCMD_LATCHCK (0x1 << 0)   /* RW */
#define EMMC50_CFG_CRCSTS_EDGE    (0x1 << 3)   /* RW */
#define EMMC50_CFG_CFCSTS_SEL     (0x1 << 4)   /* RW */

#define EMMC50_CFG3_OUTS_WR      (0x1f << 0)  /* RW */

/* SDC_FIFO_CFG mask */
#define SDC_FIFO_CFG_WRVALIDSEL   (0x1 << 24)  /* RW */
#define SDC_FIFO_CFG_RDVALIDSEL   (0x1 << 25)  /* RW */

#define REQ_CMD_EIO  (0x1 << 0)
#define REQ_CMD_TMO  (0x1 << 1)
#define REQ_DAT_ERR  (0x1 << 2)
#define REQ_STOP_EIO (0x1 << 3)
#define REQ_STOP_TMO (0x1 << 4)
#define REQ_CMD_BUSY (0x1 << 5)

#define MSDC_PREPARE_FLAG (0x1 << 0)
#define MSDC_ASYNC_FLAG (0x1 << 1)
#define MSDC_MMAP_FLAG (0x1 << 2)

#define MTK_MMC_AUTOSUSPEND_DELAY   50
#define CMD_TIMEOUT         (HZ/10 * 5) /* 100ms x5 */
#define DAT_TIMEOUT         (HZ    * 5) /* 1000ms x5 */

#define PAD_DELAY_MAX   32 /* PAD delay cells */

/*--------------------------------------------------------------------------*/
/* Descriptor Structure                                                     */
/*--------------------------------------------------------------------------*/
struct mt_gpdma_desc {
    uint32_t flags;
#define GPDMA_DESC_HWO      (0x1 << 0)
#define GPDMA_DESC_BDP      (0x1 << 1)
#define GPDMA_DESC_CHECKSUM (0xff << 8) /* bit8 ~ bit15 */
#define GPDMA_DESC_INT      (0x1 << 16)
#define GPDMA_DESC_NEXTH4   (0xf << 24)
#define GPDMA_DESC_PTRH4    (0xf << 28)
    uint32_t next;
    uint32_t ptr;
    uint32_t gpd_data_len;
#define GPDMA_DESC_BUFLEN   (0xffffff) /* bit0 ~ bit23 */
#define GPDMA_DESC_EXTLEN   (0xff << 24) /* bit24 ~ bit31 */
    uint32_t arg;
    uint32_t blknum;
    uint32_t cmd;
};

struct mt_bdma_desc {
    uint32_t flags;
#define BDMA_DESC_EOL       (0x1 << 0)
#define BDMA_DESC_CHECKSUM  (0xff << 8) /* bit8 ~ bit15 */
#define BDMA_DESC_BLKPAD    (0x1 << 17)
#define BDMA_DESC_DWPAD     (0x1 << 18)
#define BDMA_DESC_NEXTH4    (0xf << 24)
#define BDMA_DESC_PTRH4     (0xf << 28)
    uint32_t next;
    uint32_t ptr;
    uint32_t bd_data_len;
#define BDMA_DESC_BUFLEN    (0xffffff) /* bit0 ~ bit23 */
};

struct msdc_dma {
    struct scatterlist *sg; /* I/O scatter list */
    struct mt_gpdma_desc *gpd;      /* pointer to gpd array */
    struct mt_bdma_desc *bd;        /* pointer to bd array */
    paddr64_t gpd_addr;    /* the physical address of gpd array */
    paddr64_t bd_addr; /* the physical address of bd array */
};

struct msdc_save_para {
    uint32_t msdc_cfg;
    uint32_t iocon;
    uint32_t sdc_cfg;
    uint32_t pad_tune;
    uint32_t patch_bit0;
    uint32_t patch_bit1;
    uint32_t pad_ds_tune;
    uint32_t pad_cmd_tune;
    uint32_t emmc50_cfg0;
};

struct msdc_tune_para {
    uint32_t iocon;
    uint32_t pad_tune;
    uint32_t pad_cmd_tune;
};

struct msdc_delay_phase {
    uint8_t maxlen;
    uint8_t start;
    uint8_t final_phase;
};



typedef struct _mmcsd_scc_t {
    uint32_t    clk;    /* clock for sdmmc104 */
    uint32_t    tap;    /* sampling clock position for sdmmc104 */
} sdmmc_scc_t;

typedef struct _mtk_msdc_sdmmc_t {
    void            *bshdl;
    paddr_t         pbase;
    paddr_t         base;
    uint32_t        basesize;
    uint32_t        clock;      // MMC clock
    uint32_t        pclk;
    uint32_t        imask;
    uint32_t        datw;
    int             blksz;
    int             busclk;     // MMC bus clock
    uintptr_t       vbase;
    uint32_t        flags;
    int             fd;
    int             error;
#define OF_DMA_ACTIVE     1

    sdio_cmd_t     *cmd;
    int             cmd_error;
    int             irq;
    struct msdc_dma *dma;
    int             dma_iid;
    int             dma_irq;
    struct sigevent dma_ev;

    int             cs;
    sdio_sge_t      sgl[256];//[DMA_DESC_MAX];

    uint32_t timeout_ns;     /* data timeout ns */
    uint32_t timeout_clks;   /* data timeout clks */

    struct clk *src_clk;    /* msdc source clock */
    struct clk *h_clk;      /* msdc h_clk */
    uint32_t mclk;       /* mmc subsystem clock frequency */
    uint32_t src_clk_freq;   /* source clock frequency */
    uint32_t sclk;       /* SD/MS bus clock frequency */


    uint32_t hs400_ds_delay;
    uint32_t hs200_cmd_int_delay; /* cmd internal delay for HS200/SDR104 */
    uint32_t hs400_cmd_int_delay; /* cmd internal delay for HS400 */
    int hs400_cmd_resp_sel_rising;
                 /* cmd response sample selection for HS400 */
    int hs400_mode;    /* current eMMC will run at hs400 mode */
    struct msdc_save_para save_para; /* used when gate HCLK */
    struct msdc_tune_para def_tune_para; /* default tune setting */
    struct msdc_tune_para saved_tune_para; /* tune result of CMD21/CMD19 */


} mtk_msdc_sdmmc_t;

extern int mtk_msdc_sdmmc_init(sdio_hc_t *hc);
extern int mtk_msdc_sdmmc_signal_voltage(sdio_hc_t *hc, int signal_voltage);
extern int mtk_msdc_sdmmc_dinit(sdio_hc_t *hc);


static inline void sdmmc_set_bits(uintptr_t reg, uint32_t val)
{
    out32(reg, val | in32(reg));
}

static inline void sdmmc_clr_bits( uintptr_t reg, uint32_t val)
{
    out32(reg, ~val & in32(reg));
}

static inline void sdmmc_set_field(uintptr_t reg, uint32_t field, uint32_t val)
{
    unsigned int tv = in32(reg);

    tv &= ~field;
    tv |= ((val) << (ffs((unsigned int)field) - 1));
    out32(reg, tv);
}

static inline void sdmmc_get_field(uintptr_t reg, uint32_t field, uint32_t *val)
{
    unsigned int tv = in32(reg);

    *val = ((tv & field) >> (ffs((unsigned int)field) - 1));
}

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/devb/sdmmc/sdiodi/hc/mtk_msdc.h $ $Rev: 852683 $")
#endif
