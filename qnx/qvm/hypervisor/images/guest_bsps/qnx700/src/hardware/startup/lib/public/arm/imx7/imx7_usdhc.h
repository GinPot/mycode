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

#ifndef IMX7_USDHC_H_
#define IMX7_USDHC_H_

/*
 * Enhanced Secured Digital Host Controllers
 */
#define IMX7_USDHC1_BASE                 0x30B40000
#define IMX7_USDHC2_BASE                 0x30B50000
#define IMX7_USDHC3_BASE                 0x30B60000

#define IMX7_USDHC_SIZE                  0x10000
#define IMX7_USDHC_MAP_SIZE              0x100

#define IMX7_USDHC1_IRQ                  54
#define IMX7_USDHC2_IRQ                  55
#define IMX7_USDHC3_IRQ                  56

#define IMX7_SDHCX_SIZE                 0x1000

#define IMX7_SDHCX_DS_ADDR              0x00

#define IMX7_SDHCX_BLK_ATT                      0x04
    #define IMX7_SDHCX_BLK_SDMA_BOUNDARY_512K   (3 << 8)
    #define IMX7_SDHCX_BLK_BLKSIZE_MASK         0x00001FFF
    #define IMX7_SDHCX_BLK_BLKCNT_MASK          0xFFFF0000
    #define IMX7_SDHCX_BLK_BLKCNT_SHIFT         16

#define IMX7_SDHCX_ARG                  0x08

#define IMX7_SDHCX_CMD                  0x0C         // Command and transfer mode register
    #define IMX7_SDHCX_CMD_RSP_TYPE_136 (1 << 16)    // Response length 136 bit
    #define IMX7_SDHCX_CMD_RSP_TYPE_48  (2 << 16)    // Response length 48 bit
    #define IMX7_SDHCX_CMD_RSP_TYPE_48b (3 << 16)    // Response length 48 bit with busy after response
    #define IMX7_SDHCX_CMD_CCCE         (1 << 19)    // Command CRC Check Enable
    #define IMX7_SDHCX_CMD_CICE         (1 << 20)    // Command Index Check Enable
    #define IMX7_SDHCX_CMD_DP           (1 << 21)    // Data Present
    #define IMX7_SDHCX_CMD_TYPE_CMD12   (3 << 22)    // CMD12 or CMD52 "I/O Abort"

#define IMX7_SDHCX_RESP0                0x10
#define IMX7_SDHCX_RESP1                0x14
#define IMX7_SDHCX_RESP2                0x18
#define IMX7_SDHCX_RESP3                0x1C

#define IMX7_SDHCX_DATA                 0x20

#define IMX7_SDHCX_PSTATE               0x24    // Present State
    #define IMX7_SDHCX_PSTATE_DLSL_MSK  (0xFF << 24)
 /* Data line 0 level. Checked for card's busy state after write transaction */
    #define IMX7_SDHCX_PSTATE_DL3SL     (1 << 27)
    #define IMX7_SDHCX_PSTATE_DL2SL     (1 << 26)
    #define IMX7_SDHCX_PSTATE_DL1SL     (1 << 25)
    #define IMX7_SDHCX_PSTATE_DL0SL     (1 << 24)
    #define IMX7_SDHCX_PSTATE_CLSL      (1 << 23)
    #define IMX7_SDHCX_PSTATE_WP        (1 << 19)
    #define IMX7_SDHCX_PSTATE_CD        (1 << 18)
    #define IMX7_SDHCX_PSTATE_CI        (1 << 16)
    #define IMX7_SDHCX_PSTATE_RTR       (1 << 12)    // Re-Tuning Request
    #define IMX7_SDHCX_PSTATE_BRE       (1 << 11)    // Buffer Read Ready
    #define IMX7_SDHCX_PSTATE_BWE       (1 << 10)    // Buffer Write Ready
    #define IMX7_SDHCX_PSTATE_RTA       (1 <<  9)    // Read Transfer Active
    #define IMX7_SDHCX_PSTATE_WTA       (1 <<  8)    // Write Transfer Active
    #define IMX7_SDHCX_PSTATE_SDOFF     (1 <<  7)    // SD clock gated off
    #define IMX7_SDHCX_PSTATE_PEROFF    (1 <<  6)    // ipg_perclk gated off
    #define IMX7_SDHCX_PSTATE_HCKOFF    (1 <<  5)    // hckl gated off
    #define IMX7_SDHCX_PSTATE_IPGOFF    (1 <<  4)    // ipg_clk gated off
    #define IMX7_SDHCX_PSTATE_SDSTB     (1 <<  3)    // SD clock stable
    #define IMX7_SDHCX_PSTATE_DLA       (1 <<  2)    // data line active
    #define IMX7_SDHCX_PSTATE_DATI      (1 <<  1)    // Command inhibit
    #define IMX7_SDHCX_PSTATE_CMDI      (1 <<  0)    // Command inhibit

    #define IMX7_SDHCX_CARD_STABLE      (IMX7_SDHCX_PSTATE_CD | \
                                         IMX7_SDHCX_PSTATE_SDSTB | \
                                         IMX7_SDHCX_PSTATE_CI)
#define IMX7_SDHCX_DMA_IDLE             (IMX7_SDHCX_PSTATE_DL0SL | \
                                         IMX7_SDHCX_PSTATE_CLSL_MSK | \
                                         IMX7_SDHCX_PSTATE_DATI | \
                                         IMX7_SDHCX_PSTATE_CMDI)

#define IMX7_SDHCX_PCTL                 0x28    // Protocol Control
    #define IMX7_SDHCX_PCTL_LED         (1 << 0)    // LED Control
    #define IMX7_SDHCX_PCTL_DTW1        (0 << 1)    // Data Bus Width 1 bit
    #define IMX7_SDHCX_PCTL_DTW4        (1 << 1)    // Data Bus Width 4 bit
    #define IMX7_SDHCX_PCTL_DTW8        (2 << 1)    // Data Bus Width 8 bit
    #define IMX7_SDHCX_PCTL_D3CD        (1 << 3)    // Data 3 card detection
    #define IMX7_SDHCX_PCTL_BEM         (0 << 4)    // Big endian mode
    #define IMX7_SDHCX_PCTL_HWEM        (1 << 4)    // Half-word endian mode
    #define IMX7_SDHCX_PCTL_LEM         (2 << 4)    // little endian mode
    #define IMX7_SDHCX_PCTL_CDTL        (1 << 6)    // card detect test level
    #define IMX7_SDHCX_PCTL_CDSS        (1 << 7)    // card detect signal sel
    #define IMX7_SDHCX_PCTL_DMA_MSK     (3 << 8)    // DMA select mask
    #define IMX7_SDHCX_PCTL_SDMA        (0 << 8)    // simple DMA select
    #define IMX7_SDHCX_PCTL_ADMA1       (1 << 8)    // ADMA1 select
    #define IMX7_SDHCX_PCTL_ADMA2       (2 << 8)    // ADMA2 select
    #define IMX7_SDHCX_PCTL_SABGREQ     (1 << 16)   // stop at block gap request
    #define IMX7_SDHCX_PCTL_CREQ        (1 << 17)   // Continue request
    #define IMX7_SDHCX_PCTL_RWCTL       (1 << 18)   // read wait control
    #define IMX7_SDHCX_PCTL_IABG        (1 << 19)   // interrupt at block gap
    #define IMX7_SDHCX_PCTL_RDN8        (1 << 20)   // read done no 8 clock
    #define IMX7_SDHCX_PCTL_WECINT      (1 << 24)   // wake event enable on interrupt
    #define IMX7_SDHCX_PCTL_WECINS      (1 << 25)   // wake event enable on insertion
    #define IMX7_SDHCX_PCTL_WECRM       (1 << 26)   // wake event enable on removal
    #define IMX7_SDHCX_PCTL_BLE         (1 << 27)   // burst length enabled for INCR
    #define IMX7_SDHCX_PCTL_BLE_16      (2 << 27)   // burst length enabled for INCR16
    #define IMX7_SDHCX_PCTL_BLE_16WRAP  (4 << 27)   // burst length enabled for INCR16 wrap

#define IMX7_SDHCX_SYSCTL                       0x2C            // Clock Control/Timeout/Software reset
    #define IMX7_SDHCX_SYSCTL_DVS_SHIFT         (4)             // clock divisor shift
    #define IMX7_SDHCX_SYSCTL_DVS_MSK           (0xF << 4)      // clock divisor
    #define IMX7_SDHCX_SYSCTL_SDCLKFS_SHIFT     (8)
    #define IMX7_SDHCX_SYSCTL_SDCLKFS_MSK       0xFF00
    #define IMX7_SDHCX_SYSCTL_DTO_SHIFT         (16)            // Data timeout counter shift
    #define IMX7_SDHCX_SYSCTL_DTO_MSK           (0xF << 16)     // Data timeout counter
    #define IMX7_SDHCX_SYSCTL_DTO_MAX           (0xF << 16)     // Timeout = TCF x 2^27
    #define IMX7_SDHCX_SYSCTL_SRA               (1 << 24)       // Software reset for all
    #define IMX7_SDHCX_SYSCTL_SRC               (1 << 25)       // Software reset for mmci_cmd line
    #define IMX7_SDHCX_SYSCTL_SRD               (1 << 26)       // Software reset for mmci_dat line
    #define IMX7_SDHCX_SYSCTL_INITA             (1 << 27)       // Initialization active
    #define IMX7_SDHCX_CLK_MAX_DIV_SPEC_VER_3   2046

#define IMX7_SDHCX_IS                   0x30    // Interrupt status register
#define IMX7_SDHCX_IE                   0x34    // Interrupt SD enable register
#define IMX7_SDHCX_ISE                  0x38    // Interrupt signal enable register
    #define IMX7_SDHCX_INTR_CC          (1 <<  0)    // Command Complete
    #define IMX7_SDHCX_INTR_TC          (1 <<  1)    // Transfer Complete
    #define IMX7_SDHCX_INTR_BGE         (1 <<  2)    // Block Gap Event
    #define IMX7_SDHCX_INTR_DMA         (1 <<  3)    // DMA interrupt
    #define IMX7_SDHCX_INTR_BWR         (1 <<  4)    // Buffer Write Ready interrupt
    #define IMX7_SDHCX_INTR_BRR         (1 <<  5)    // Buffer Read Ready interrupt
    #define IMX7_SDHCX_INTR_CINS        (1 <<  6)    // Card Insertion
    #define IMX7_SDHCX_INTR_CREM        (1 <<  7)    // Card Removal
    #define IMX7_SDHCX_INTR_CIRQ        (1 <<  8)    // Card interrupt
    #define IMX7_SDCHX_INTR_RETUNE      (1 << 12)    // Re-Tuning
    #define IMX7_SDCHX_INTR_TP          (1 << 14)    // Re-Tuning
    #define IMX7_SDHCX_INTR_CTO         (1 << 16)    // Command Timeout error
    #define IMX7_SDHCX_INTR_CCRC        (1 << 17)    // Command CRC error
    #define IMX7_SDHCX_INTR_CEB         (1 << 18)    // Command End Bit error
    #define IMX7_SDHCX_INTR_CIE         (1 << 19)    // Command Index error
    #define IMX7_SDHCX_INTR_DTO         (1 << 20)    // Data Timeout error
    #define IMX7_SDHCX_INTR_DCRC        (1 << 21)    // Data CRC error
    #define IMX7_SDHCX_INTR_DEB         (1 << 22)    // Data End Bit error
    #define IMX7_SDHCX_INTR_ACE         (1 << 24)    // ACMD12 error
    #define IMX7_SDHCX_INTR_TNE         (1 << 26)    // Tuning  Error
    #define IMX7_SDHCX_INTR_DMAE        (1 << 28)    // DMA Error

    #define IMX7_SDHCX_IE_DFLTS         (IMX7_SDHCX_INTR_CTO | IMX7_SDHCX_INTR_CCRC \
                                         | IMX7_SDHCX_INTR_CEB | IMX7_SDHCX_INTR_CIE \
                                         | IMX7_SDHCX_INTR_ACE \
                                         | IMX7_SDHCX_INTR_CINS | IMX7_SDHCX_INTR_CREM)

    #define IMX7_SDHCX_IS_ERRI          (IMX7_SDHCX_INTR_DMAE \
                                         | IMX7_SDHCX_INTR_CCRC | IMX7_SDHCX_INTR_CTO \
                                         | IMX7_SDHCX_INTR_CEB | IMX7_SDHCX_INTR_CIE \
                                         | IMX7_SDHCX_INTR_DTO | IMX7_SDHCX_INTR_DCRC \
                                         | IMX7_SDHCX_INTR_DEB | IMX7_SDHCX_INTR_ACE)
    #define IMX7_SDHCX_ISE_DFLTS        0x117FF1FF
    #define IMX7_SDHCX_INTR_CLR_ALL     0xFFFFFFFF

#define IMX7_SDHCX_AC12                         0x3C

#define IMX7_SDHCX_CAP                          0x40    // Capability Registers bits 0 to 31
    #define IMX7_SDHCX_CAP_S18                  (1 << 26)    // 1.8V support
    #define IMX7_SDHCX_CAP_S30                  (1 << 25)    // 3.0V support
    #define IMX7_SDHCX_CAP_S33                  (1 << 24)    // 3.3V support
    #define IMX7_SDHCX_CAP_SRS                  (1 << 23)    // Suspend/Resume support
    #define IMX7_SDHCX_CAP_DMA                  (1 << 22)    // DMA support
    #define IMX7_SDHCX_CAP_HS                   (1 << 21)    // High-Speed support
    #define IMX7_SDHCX_CAP_ADMAS                (1 << 20)    // ADMA support
    #define IMX7_SDHCX_CAP_MBL512               (0 << 16)    // Max block length 512
    #define IMX7_SDHCX_CAP_MBL1024              (1 << 16)    // Max block length 512
    #define IMX7_SDHCX_CAP_MBL2048              (2 << 16)    // Max block length 2048
    #define IMX7_SDHCX_CAP_MBL4096              (3 << 16)    // Max block length 2048
    #define IMX7_SDHCX_CAP_RETUNEMODE1          (0 << 16)    // Mode 1
    #define IMX7_SDHCX_CAP_RETUNEMODE2          (1 << 16)    // Mode 2
    #define IMX7_SDHCX_CAP_RETUNEMODE3          (2 << 16)    // Mode 3
    #define IMX7_SDHCX_CAP_SDR50TUNE            (1 << 13)    // SDR 50 requires tuning
    #define IMX7_SDHCX_CAP_DDR50                (1 << 2)     // DDR50 Support
    #define IMX7_SDHCX_CAP_SDR104               (1 << 1)     // SDR104 Support
    #define IMX7_SDHCX_CAP_SDR50                (1 << 0)     // SDR50 Support


#define IMX7_SDHCX_CAP_BASE_CLK(_c, _v)         (((_v) >= IMX7_SDHCX_SPEC_VER_3 ? \
                                                (((_c) >> 8) & 0xFF) :            \
                                                (((_c) >> 8) & 0x3F)) * 1000 * 1000)

#define IMX7_SDHCX_WATML                        0x44
  #define IMX7_SDHCX_WATML_WRBRSTLENSHIFT       24
  #define IMX7_SDHCX_WATML_WRBRSTLENMASK        0x1F000000
  #define IMX7_SDHCX_WATML_WRWMLSHIFT           16
  #define IMX7_SDHCX_WATML_WRWMLMASK            0xFF0000
  #define IMX7_SDHCX_WATML_RDBRSTLENSHIFT       8
  #define IMX7_SDHCX_WATML_RDBRSTLENMASK        0x1F00
  #define IMX7_SDHCX_WATML_RDWMLSHIFT           0
  #define IMX7_SDHCX_WATML_RDWMLMASK            0xFF
  #define IMX7_SDHCX_WATML_WR                   (128)
  #define IMX7_SDHCX_WATML_RD                   IMX7_SDHCX_WATML_WR

#define IMX7_SDHCX_MIX_CTRL                     0x48
        #define IMX7_SDHCX_MIX_CTRL_DE          (1 <<  0)       // DMA Enable
        #define IMX7_SDHCX_MIX_CTRL_BCE         (1 <<  1)       // Block Count Enable
        #define IMX7_SDHCX_MIX_CTRL_ACMD12      (1 <<  2)       // Auto CMD12 Enable
        #define IMX7_SDHCX_MIX_CTRL_DDR_EN      (1 <<  3)       // DDR enable
        #define IMX7_SDHCX_MIX_CTRL_DDIR        (1 <<  4)       // Data Transfer Direction Read
        #define IMX7_SDHCX_MIX_CTRL_MBS         (1 <<  5)       // Multi Block Select
        #define IMX7_SDHCX_MIX_CTRL_ACMD23      (1 <<  7)       // Auto CMD23 Enable
        #define IMX7_SDHCX_MIX_CTRL_EXE_TUNE    (1 << 22)
        #define IMX7_SDHCX_MIX_CTRL_SMP_CLK_SEL (1 << 23)
        #define IMX7_SDHCX_MIX_CTRL_AUTOTUNE_EN (1 << 24)
        #define IMX7_SDHCX_MIX_CTRL_FBCLK_SEL   (1 << 25)
        #define IMX7_SDHCX_MIX_CTRL_HS400_MODE  (1 << 26)
#define IMX7_SDHCX_FORCE_EVENT      0x50
#define IMX7_SDHCX_FORCE_EVENT_INT  (1 << 31) // force interrupt
#define IMX7_SDHCX_ADMA_ES          0x54
#define IMX7_SDHCX_ADMA_ADDRL       0x58
#define IMX7_SDHCX_ADMA_ADDRH       0x5C

#define IMX7_SDHCX_DLL_CTRL                             0x60
    #define IMX7_SDHCX_DLL_CTRL_ENABLE                  (1 << 0)
    #define IMX7_SDHCX_DLL_CTRL_RESET                   (1 << 1)
    #define IMX7_SDHCX_DLL_CTRL_SLV_FORCE_UPD           (1 << 2)
    #define IMX7_SDHCX_DLL_CTRL_SLV_DLY_TARGET0_3       (1 << 3)
    #define IMX7_SDHCX_DLL_CTRL_GATE_UPDATE             (1 << 7)
    #define IMX7_SDHCX_DLL_CTRL_SLV_OVERIDE             (1 << 8)
    #define IMX7_SDHCX_DLL_CTRL_SLV_OVERIDE_VAL         (1 << 9)
    #define IMX7_SDHCX_DLL_CTRL_SLV_DLY_TARGET4_6       (1 << 16)
    #define IMX7_SDHCX_DLL_CTRL_SLV_UPDATE              (1 << 20)
    #define IMX7_SDHCX_DLL_CTRL_REF_UPDATE              (1 << 28)

#define IMX7_SDHCX_DLL_STATUS                           0x64
    #define IMX7_SDHCX_DLL_STATUS_SLV_LOCK              (1 << 0)
    #define IMX7_SDHCX_DLL_STATUS_REF_LOCK              (1 << 1)
    #define IMX7_SDHCX_DLL_STATUS_SLV_SEL               (1 << 2)
    #define IMX7_SDHCX_DLL_STATUS_REF_SEL               (1 << 9)

#define IMX7_SDHCX_TUNE_CTRL_STATUS                     0x68
    #define IMX7_SDHCI_TUNE_DLY_CELL_SET_PRE_OFF        8
    #define IMX7_SDHCI_TUNE_DLY_CELL_SET_PRE_MSK        (0x7F00)
    #define IMX7_SDHCI_TUNE_CTRL_MIN                    0x0
    #define IMX7_SDHCI_TUNE_CTRL_MAX                    ((1 << 7) - 1)
    #define IMX7_SDHCI_TUNE_STEP                        1

/* Strobe DLL register */
#define IMX7_uSDHCx_STROBE_DLL_CTRL              0x70
    #define IMX7_uSDHCx_STROBE_DLL_CTRL_ENABLE   (1 << 0)
    #define IMX7_uSDHCx_STROBE_DLL_CTRL_RESET    (1 << 1)

#define IMX7_uSDHCx_STROBE_DLL_STATUS            0x74
    #define IMX7_uSDHCx_STROBE_DLL_STS_REF_LOCK  (1 << 1)
    #define IMX7_uSDHCx_STROBE_DLL_STS_SLV_LOCK  0x1

#define IMX7_SDHCX_VEND_SPEC                    0xC0
    #define IMX7_SDHCX_VEND_SPEC_SDVS1V8        (1 << 1)    // SD bus voltage 1.8V
    #define IMX7_SDHCX_VEND_SPEC_SDVS3V0        (0 << 1)    // SD bus voltage 3.0V
    #define IMX7_SDHCX_VEND_SPEC_SDVS_MSK       (1 << 1)
    #define IMX7_SDHCX_VEND_SPEC_FRC_SDCLK_ON   (1 << 8)    // SD force SDCLK on

#define IMX7_SDHCX_MMC_BOOT         0xC4
#define IMX7_SDHCX_VEND_SPEC2       0xC8

#define IMX7_uSDHCx_TUNING_CTRL                          0xCC
        #define IMX7_uSDHCx_STD_TUNING_EN                (1 << 24)
        #define IMX7_uSDHCx_TUNING_START_TAP_DEFAULT     0x1
        #define IMX7_uSDHCx_TUNING_START_TAP_MASK        0xFF
        #define IMX7_uSDHCx_TUNING_STEP_MASK             0x00070000
        #define IMX7_uSDHCx_TUNING_STEP_SHIFT            16

#define IMX7_SDHCX_HCTL_VERSION         0xFC
    #define IMX7_SDHCX_SPEC_VER_MSK     0xFF
    #define IMX7_SDHCX_SPEC_VER_3       0x2
    #define IMX7_SDHCX_SPEC_VER_1       0x0

#endif /* IMX7_USDHC_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/arm/imx7/imx7_usdhc.h $ $Rev: 840798 $")
#endif
