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

#ifndef _LS10XX_H_INCLUDED
#define _LS10XX_H_INCLUDED

#include <internal.h>
#include <aarch64/ls10xx.h>

#define LS10XX_LOG(hc, severity, fmt, ...)  \
        sdio_slogf( _SLOGC_SDIODI, severity, hc->cfg.verbosity, severity-_SLOG_ERROR,  \
                            "%s: " fmt, __FUNCTION__, ##__VA_ARGS__)


// These should be only used for transfereing from/to the data buffer register, and since we set
// the Endian mode in LS10XX_PCTL to Little Endian, we have to use the LE in/out functions.
#define ls10xx_sdhcx_data_in32s         in32s
#define ls10xx_sdhcx_data_out32s        out32s

#define LS10XX_TUNING_RETRIES           40
#define LS10XX_CLOCK_TIMEOUT            20000
#define LS10XX_COMMAND_TIMEOUT          1000000  /* The card could take very long time to process the transfered data */
#define LS10XX_TRANSFER_TIMEOUT         1000000

#define LS10XX_SIZE                     4096
#define LS10XX_CLOCK_DEFAULT            250000000

#define LS10XX_DS_ADDR           0x00

#define LS10XX_BLK_ATT           0x04
    #define LS10XX_BLK_SDMA_BNDRY_512K  ( 3 << 8 )
    #define LS10XX_BLK_BLKSIZE_MASK     0x00001fff
    #define LS10XX_BLK_BLKCNT_MASK      0xffff0000
    #define LS10XX_BLK_BLKCNT_SHIFT     16

#define LS10XX_ARG               0x08

#define LS10XX_CMD               0x0C        // Command and transfer mode register
    #define LS10XX_CMD_DMAEN             (1 << 0)
    #define LS10XX_CMD_BCEN              (1 << 1)
    #define LS10XX_CMD_ACEN_ACMD12       (1 << 2)
    #define LS10XX_CMD_ACEN_ACMD23       (2 << 2)
    #define LS10XX_CMD_DTDSEL            (1 << 4)
    #define LS10XX_CMD_MSBSEL            (1 << 5)
    #define LS10XX_CMD_RSP_TYPE_136      (1 << 16)    // Response length 136 bit
    #define LS10XX_CMD_RSP_TYPE_48       (2 << 16)    // Response length 48 bit
    #define LS10XX_CMD_RSP_TYPE_48b      (3 << 16)    // Response length 48 bit with busy after response
    #define LS10XX_CMD_CCCE              (1 << 19)    // Comamnd CRC Check Enable
    #define LS10XX_CMD_CICE              (1 << 20)    // Comamnd Index Check Enable
    #define LS10XX_CMD_DP                (1 << 21)    // Data Present
    #define LS10XX_CMD_TYPE_CMD12        (3 << 22)    // CMD12 or CMD52 "I/O Abort"

#define LS10XX_RESP0             0x10
#define LS10XX_RESP1             0x14
#define LS10XX_RESP2             0x18
#define LS10XX_RESP3             0x1C

#define LS10XX_DATA              0x20

#define LS10XX_PSTATE            0x24    // Present State
    #define LS10XX_PSTATE_DLSL_MSK       (0xFF << 24)
    //Data line 0 level. Checked for card's busy state after write transaction
    #define LS10XX_PSTATE_DL3SL          (1 << 27)
    #define LS10XX_PSTATE_DL2SL          (1 << 26)
    #define LS10XX_PSTATE_DL1SL          (1 << 25)
    #define LS10XX_PSTATE_DL0SL          (1 << 24)
    #define LS10XX_PSTATE_CLSL           (1 << 23)
    #define LS10XX_PSTATE_WP             (1 << 19)
    #define LS10XX_PSTATE_CD             (1 << 18)
    #define LS10XX_PSTATE_CI             (1 << 16)
    #define LS10XX_PSTATE_BRE            (1 << 11)    // Buffer Read Ready
    #define LS10XX_PSTATE_BWE            (1 << 10)    // Buffer Write Ready
    #define LS10XX_PSTATE_RTA            (1 <<  9)    // Read Transfer Active
    #define LS10XX_PSTATE_WTA            (1 <<  8)    // Write Transfer Active
    #define LS10XX_PSTATE_SDOFF          (1 <<  7)    // SD clock gated off
    #define LS10XX_PSTATE_SDSTB          (1 <<  3)    // SD clock stable
    #define LS10XX_PSTATE_DLA            (1 <<  2)    // data line active
    #define LS10XX_PSTATE_DATI           (1 <<  1)    // Command inhibit
    #define LS10XX_PSTATE_CMDI           (1 <<  0)    // Command inhibit

    #define LS10XX_CARD_STABLE           (LS10XX_PSTATE_CD | \
                                          LS10XX_PSTATE_SDSTB | \
                                          LS10XX_PSTATE_CI)
    #define LS10XX_DMA_IDLE              (LS10XX_PSTATE_DL0SL | \
                                          LS10XX_PSTATE_CLSL_MSK | \
                                          LS10XX_PSTATE_DATI | \
                                          LS10XX_PSTATE_CMDI)


#define LS10XX_PCTL              0x28    // Protocol Control
    #define LS10XX_PCTL_DTW1              (0 << 1)    // Data Bus Width 1 bit
    #define LS10XX_PCTL_DTW4              (1 << 1)    // Data Bus Width 4 bit
    #define LS10XX_PCTL_DTW8              (2 << 1)    // Data Bus Width 8 bit
    #define LS10XX_PCTL_BEM               (0 << 4)    // Big endian mode
    #define LS10XX_PCTL_LEM               (2 << 4)    // little endian mode
    #define LS10XX_PCTL_CDTL              (1 << 6)    // card detect test level
    #define LS10XX_PCTL_CDSS              (1 << 7)    // card detect signal sel
    #define LS10XX_PCTL_DMA_MSK           (3 << 8)    // DMA select mask
    #define LS10XX_PCTL_SDMA              (0 << 8)    // simple DMA select
    #define LS10XX_PCTL_ADMA1             (1 << 8)    // ADMA1 select
    #define LS10XX_PCTL_ADMA2             (2 << 8)    // ADMA2 select
    #define LS10XX_PCTL_VOLSEL_18V        (1 << 10)    //
    #define LS10XX_PCTL_SABGREQ           (1 << 16)   // stop at block gap request
    #define LS10XX_PCTL_CREQ              (1 << 17)   // Continue request
    #define LS10XX_PCTL_RWCTL             (1 << 18)   // read wait control
    #define LS10XX_PCTL_IABG              (1 << 19)   // interrupt at block gap
    #define LS10XX_PCTL_WECINT            (1 << 24)   // wake event enable on interrupt
    #define LS10XX_PCTL_WECINS            (1 << 25)   // wake event enable on insertion
    #define LS10XX_PCTL_WECRM             (1 << 26)   // wake event enable on removal

#define LS10XX_SYSCTL            0x2C    // Clock Control/Timeout/Software reset
    #define LS10XX_SYSCTL_SDCLK_IPGEN     (1 << 0)
    #define LS10XX_SYSCTL_SDCLK_HCKEN     (1 << 1)
    #define LS10XX_SYSCTL_SDCLK_PEREN     (1 << 2)
    #define LS10XX_SYSCTL_SDCLKEN         (1 << 3)
    #define LS10XX_SYSCTL_DVS_SHIFT       (4)    // clock divisor shift
    #define LS10XX_SYSCTL_DVS_MSK         (0xf << 4)    // clock divisor
    #define LS10XX_SYSCTL_SDCLKFS_SHIFT   (8)
    #define LS10XX_SYSCTL_SDCLKFS_MSK     (0xff << 8)
    #define LS10XX_SYSCTL_DTO_SHIFT       (16)    // Data timeout counter shift
    #define LS10XX_SYSCTL_DTO_MSK         (0xf << 16)    // Data timeout counter
    #define LS10XX_SYSCTL_DTO_MAX         (0xf << 16)    // Timeout = TCF x 2^27
    #define LS10XX_SYSCTL_SRA             (1 << 24)    // Software reset for all
    #define LS10XX_SYSCTL_SRC             (1 << 25)    // Software reset for mmci_cmd line
    #define LS10XX_SYSCTL_SRD             (1 << 26)    // Software reset for mmci_dat line
    #define LS10XX_SYSCTL_INITA           (1 << 27)    // Initialization active


#define LS10XX_IS                0x30    // Interrupt status register
#define LS10XX_IE                0x34    // Interrupt SD enable register
#define LS10XX_ISE               0x38    // Interrupt signal enable register
    #define LS10XX_INTR_CC                (1 <<  0)    // Command Complete
    #define LS10XX_INTR_TC                (1 <<  1)    // Transfer Complete
    #define LS10XX_INTR_BGE               (1 <<  2)    // Block Gap Event
    #define LS10XX_INTR_DMA               (1 <<  3)    // DMA interupt
    #define LS10XX_INTR_BWR               (1 <<  4)    // Buffer Write Ready interrupt
    #define LS10XX_INTR_BRR               (1 <<  5)    // Buffer Read Ready interrupt
    #define LS10XX_INTR_CINS              (1 <<  6)    // Card Insertion
    #define LS10XX_INTR_CREM              (1 <<  7)    // Card Removal
    #define LS10XX_INTR_CIRQ              (1 <<  8)    // Card interrupt
    #define LS10XX_SDCHX_INTR_RETUNE      (1 << 12)    // Re-Tuning
    #define LS10XX_INTR_CTO               (1 << 16)    // Command Timeout error
    #define LS10XX_INTR_CCRC              (1 << 17)    // Command CRC error
    #define LS10XX_INTR_CEB               (1 << 18)    // Command End Bit error
    #define LS10XX_INTR_CIE               (1 << 19)    // Command Index error
    #define LS10XX_INTR_DTO               (1 << 20)    // Data Timeout error
    #define LS10XX_INTR_DCRC              (1 << 21)    // Data CRC error
    #define LS10XX_INTR_DEB               (1 << 22)    // Data End Bit error
    #define LS10XX_INTR_ACE               (1 << 24)    // ACMD12 error
    #define LS10XX_INTR_TNE               (1 << 26)    // Tuning  Error
    #define LS10XX_INTR_DMAE              (1 << 28)    // DMA Error

    #define LS10XX_IE_DFLTS            (LS10XX_INTR_CTO | LS10XX_INTR_CCRC \
                                        | LS10XX_INTR_CEB | LS10XX_INTR_CIE \
                                        | LS10XX_INTR_ACE)

    #define LS10XX_IS_ERRI            (LS10XX_INTR_DMAE \
                                        | LS10XX_INTR_CCRC | LS10XX_INTR_CTO \
                                        | LS10XX_INTR_CEB | LS10XX_INTR_CIE \
                                        | LS10XX_INTR_DTO | LS10XX_INTR_DCRC \
                                        | LS10XX_INTR_DEB | LS10XX_INTR_ACE)
    #define LS10XX_ISE_DFLTS           0x117F11FF
    #define LS10XX_INTR_CLR_ALL        0xffffffff

#define LS10XX_AC12_SYSCTL2      0x3C
    #define LS10XX_AC12_NE                (1 << 0)
    #define LS10XX_AC12_TOE               (1 << 1)
    #define LS10XX_AC12_CE                (1 << 2)
    #define LS10XX_AC12_EBE               (1 << 3)
    #define LS10XX_AC12_IE                (1 << 4)
    #define LS10XX_AC12_CNIB              (1 << 7)
    #define LS10XX_SYSCTL2_UHSM_MASK      (7 << 16)
    #define LS10XX_SYSCTL2_UHSM_SDR12     (0 << 16)
    #define LS10XX_SYSCTL2_UHSM_SDR25     (1 << 16)
    #define LS10XX_SYSCTL2_UHSM_SDR50     (2 << 16)
    #define LS10XX_SYSCTL2_UHSM_SDR104    (3 << 16)
    #define LS10XX_SYSCTL2_UHSM_DDR       (4 << 16)
    #define LS10XX_SYSCTL2_EXTN           (1 << 22)
    #define LS10XX_SYSCTL2_SMPCLKSEL      (1 << 23)
    #define LS10XX_SYSCTL2_AIE            (1 << 30)


#define LS10XX_CAP               0x40    // Capability Registers bits 0 to 31
    #define LS10XX_CAP_S18                (1 << 26)    // 1.8V support
    #define LS10XX_CAP_S30                (1 << 25)    // 3.0V support
    #define LS10XX_CAP_S33                (1 << 24)    // 3.3V support
    #define LS10XX_CAP_SRS                (1 << 23)    // Suspend/Resume support
    #define LS10XX_CAP_DMA                (1 << 22)    // DMA support
    #define LS10XX_CAP_HS                 (1 << 21)    // High-Speed support
    #define LS10XX_CAP_ADMAS              (1 << 20)    // ADMA support
    #define LS10XX_CAP_MBL512             (0 << 16)    // Max block length 512
    #define LS10XX_CAP_MBL1024            (1 << 16)    // Max block length 1024
    #define LS10XX_CAP_MBL2048            (2 << 16)    // Max block length 2048

#define LS10XX_CAP_BASE_CLK( _c, _v )    ( ( ( _v ) >= LS10XX_SPEC_VER_3 ? \
                                            ( ( ( _c ) >> 8 ) & 0xff ) :            \
                                            ( ( ( _c ) >> 8 ) & 0x3f ) ) * 1000 * 1000 )

#define LS10XX_WATML             0x44
  #define LS10XX_WATML_WRBRSTLENSHIFT      24
  #define LS10XX_WATML_WRBRSTLENMASK       (0x1F << LS10XX_WATML_WRBRSTLENSHIFT)
  #define LS10XX_WATML_WRWMLSHIFT          16
  #define LS10XX_WATML_WRWMLMASK           (0xFF << LS10XX_WATML_WRWMLSHIFT)
  #define LS10XX_WATML_RDBRSTLENSHIFT      8
  #define LS10XX_WATML_RDBRSTLENMASK       (0x1F << LS10XX_WATML_RDBRSTLENSHIFT)
  #define LS10XX_WATML_RDWMLSHIFT          0
  #define LS10XX_WATML_RDWMLMASK           (0xFF << LS10XX_WATML_RDWMLSHIFT)
  #define LS10XX_WATML_WR                  (16)
  #define LS10XX_WATML_RD                  LS10XX_WATML_WR

#define LS10XX_FORCE_EVENT       0x50
#define LS10XX_ADMA_ES           0x54
#define LS10XX_ADMA_ADDR         0x58

#define LS10XX_HOSTVER           0xFC
    #define LS10XX_HOSTVER_VENDVER_MSK    (0xff << 8)
    #define LS10XX_HOSTVER_SPECVER_MSK    0xff
    #define LS10XX_HOSTVER_SPECVER_3      0x2
    #define LS10XX_HOSTVER_SPECVER_2      0x1
    #define LS10XX_HOSTVER_SPECVER_1      0x0

#define LS10XX_CAP2              0x114    // Capability Registers bits 0 to 31
    #define LS10XX_CAP2_RTM               (1 << 14)   // 2 bits
    #define LS10XX_CAP2_UTSDR50           (1 << 13)
    #define LS10XX_CAP2_TCRT              (1 << 8)    // 4 bits
    #define LS10XX_CAP2_DTDS              (1 << 6)
    #define LS10XX_CAP2_DTCS              (1 << 5)
    #define LS10XX_CAP2_DTAS              (1 << 4)
    #define LS10XX_CAP2_DDR50             (1 << 2)
    #define LS10XX_CAP2_SDR104            (1 << 1)
    #define LS10XX_CAP2_SDR50             (1 << 0)

#define LS10XX_TUNE_CTRL         0x120
    #define LS10XX_TUNE_CTRL_ENABLE            (1 << 2)
    #define LS10XX_TUNE_CTRL_MODE3_SW_TUNE     (3 << 0)
    #define LS10XX_TUNE_CTRL_MODE3_SWT_AUTO    (2 << 0)
    #define LS10XX_TUNE_CTRL_MODE2_SWT_RETUNE  (1 << 0)
    #define LS10XX_TUNE_CTRL_MODE1_SWT         (0 << 0)

#define LS10XX_TUNE_PTR         0x128
    #define LS10XX_TUNE_PTR_START_SHIFT         8
    #define LS10XX_TUNE_PTR_MODE3_SWT_AUTO     (2 << 0)
    #define LS10XX_TUNE_PTR_MODE2_SWT_RETUNE   (1 << 0)
    #define LS10XX_TUNE_PTR_MODE1_SWT             (0 << 0)


#define LS10XX_CLKCTRL           0x144
    #define LS10XX_CLKCTRL_CMD_CLKCRTRL        (1 << 15)
    #define LS10XX_CLKCTRL_LPBK_CLK_DLY_DIR    (1 << 30)
    #define LS10XX_CLKCTRL_LPBK_CLKSEL         (1 << 31)


#define LS10XX_ESDHC_CTRL        0x40C
    #define LS10XX_ESDHC_CTRL_PCS              (1 << 19)
    #define LS10XX_ESDHC_CTRL_FAF              (1 << 18)
    #define LS10XX_ESDHC_CTRL_RTR              (1 << 17)
    #define LS10XX_ESDHC_CTRL_CRS              (1 << 16)
    #define LS10XX_ESDHC_CTRL_RD_PREFETCH_CNT_SHIFT 8
    #define LS10XX_ESDHC_CTRL_PAD_DIS          (1 << 7)
    #define LS10XX_ESDHC_CTRL_SNOOP            (1 << 6)
    #define LS10XX_ESDHC_CTRL_WR_BUF           (1 << 3)
    #define LS10XX_ESDHC_CTRL_RD_SAFE          (1 << 2)


#define LS10XX_ADMA2_MAX_XFER    (1024 * 60)

// 32 bit ADMA descriptor defination
typedef struct _ls10xx_adma32_t {
    uint16_t    attr;
    uint16_t    len;
    uint32_t    addr;
} ls10xx_adma32_t;

#define LS10XX_ADMA2_VALID                (1 << 0)    // valid
#define LS10XX_ADMA2_END                  (1 << 1)    // end of descriptor, transfer complete interrupt will be generated
#define LS10XX_ADMA2_INT                  (1 << 2)    // generate DMA interrupt, will not be used
#define LS10XX_ADMA2_NOP                  (0 << 4)    // no OP, go to the next desctiptor
#define LS10XX_ADMA2_SET                  (1 << 4)    // no OP, go to the next desctiptor
#define LS10XX_ADMA2_TRAN                 (2 << 4)    // transfer data
#define LS10XX_ADMA2_LINK                 (3 << 4)    // link to another descriptor


typedef struct _ls10xx_usdhcx_hc {
    void            *bshdl;
    uintptr_t        base;
    ls10xx_adma32_t *adma;
    uint32_t         admap;
#define SF_USE_SDMA            0x01
#define SF_USE_ADMA            0x02
#define SF_TUNE_SDR50          0x04
#define SF_SDMA_ACTIVE         0x10
#define ADMA_DESC_MAX          256
    sdio_sge_t       sgl[ADMA_DESC_MAX];
    uint32_t         flags;
    int              sdma_iid;    // SDMA interrupt id
    uintptr_t        sdma_irq;
    uintptr_t        sdma_base;
    uint32_t         per_clk;
}ls10xx_hc_t;

static inline uint32_t ls10xx_in32(sdio_hc_t *hc, uint32_t regoff) {
    ls10xx_hc_t *sdhc = (ls10xx_hc_t *) hc->cs_hdl;
#ifdef LS10XX_ESDHC_ACCESSES_BE
    return inbe32(sdhc->base + regoff);
#else
    return in32(sdhc->base + regoff);
#endif
}

static inline void ls10xx_out32(sdio_hc_t *hc, uint32_t regoff, uint32_t value) {
    ls10xx_hc_t *sdhc = (ls10xx_hc_t *) hc->cs_hdl;
#ifdef LS10XX_ESDHC_ACCESSES_BE
    outbe32(sdhc->base + regoff, value);
#else
    out32(sdhc->base + regoff, value);
#endif
}

extern int ls10xx_dataLines_wait(sdio_hc_t *hc, uint32_t high);
extern int ls10xx_clock_set(sdio_hc_t *hc, uint32_t on);
extern int ls10xx_sdhcx_init( sdio_hc_t *hc );
extern int ls10xx_sdhcx_dinit( sdio_hc_t *hc );

#endif // _LS10XX_H_INCLUDED

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/devb/sdmmc/sdiodi/hc/ls10xx.h $ $Rev: 834047 $")
#endif
