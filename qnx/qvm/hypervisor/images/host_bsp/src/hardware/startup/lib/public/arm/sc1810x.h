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


/*
 * Socionext SC1810x ARM Cortex A9 Core
 */

#define BIT(x)                                  (1 << x)

/* -------------------------------------------------------------------------
 * Devices
 * -------------------------------------------------------------------------
 */
#define SC1810X_HWI_UART                        "uart"
#define SC1810X_HWI_WDT                         "wdt"
#define SC1810X_HWI_DMA                         "dma"

/*
 * System Memory Map
 */

/* -------------------------------------------------------------------------
 * BOOT ROM (ROMCP)
 * -------------------------------------------------------------------------
 */
#define SC1810X_BOOTROM_BASE                    0xFFFF0000
#define SC1810X_SCU_BASE                        0xFFFFA000


/* -------------------------------------------------------------------------
 * PL310 (L2 Cache) Base Address
 * -------------------------------------------------------------------------
 */
#define SC1810X_PL310_BASE                      0xFFFF8000

#define L2_CACHE_ID                             0x000
#define L2_CACHE_TYPE                           0x004
#define L2_CTRL                                 0x100
#define L2_AUX_CTRL                             0x104
#define L2_TAG_RAM_L_CTRL                       0x108
#define L2_DATA_RAM_L_CTRL                      0x10c
#define L2_EVENT_CNT_CTRL                       0x200
#define L2_EVENT_CNT_1_CFG                      0x204
#define L2_EVENT_CNT_0_CFG                      0x208
#define L2_EVENT_CNT_1_VAL                      0x20c
#define L2_EVENT_CNT_0_VAL                      0x210
#define L2_INTR_MASK                            0x214
#define L2_MASK_INTR_STATUS                     0x218
#define L2_RAW_INTR_STATUS                      0x21C
#define L2_INTR_CLEAR                           0x220
#define L2_CACHE_SYNC                           0x730
#define L2_INVAL_LINE_BY_PA                     0x770
#define L2_INVAL_BY_WAY                         0x77C
#define L2_CLEAN_LINE_BY_PA                     0x7B0
#define L2_CLEAN_LINE_BY_WAY                    0x7B8
#define L2_CLEAN_BY_WAY                         0x7Bc
#define L2_CLEAN_INVAL_LINE_BY_PA               0x7F0
#define L2_CLEAN_INVAL_LINE_BY_WAY              0x7F8
#define L2_CLEAN_INVAL_BY_WAY                   0x7FC
#define L2_DEBUG_CTRL                           0xF40
#define L2_PREFETCH_CTRL                        0xF60
#define L2_POWER_CTRL                           0xF80
#define L2_ALL_WAYS                             0xFFFF


/* -------------------------------------------------------------------------
 * Interrupt controller
 * -------------------------------------------------------------------------
 */
#define SC1810X_EXIRC_BASE                      0x3B400000

/* -------------------------------------------------------------------------
 * Clock Reset Generator
 * -------------------------------------------------------------------------
 */
#define SC1810X_CRG_BASE                        0x3B500000
#define SC1810X_CRG_SIZE                        0x19000
#define SCRRMR1                                 0x0000
#define SCRRMR2                                 0x0004
#define SCRSRR                                  0x0010
#define SCRISE                                  0x0020
#define SCRISV                                  0x0024

#define PLLIDIV_MASK                            0xFF
#define LDIVRC_MASK                             0x7
#define ODIVCHXNUM_MASK                         0xFF

/* G0 clock domain */
#define G0_PLLFREQ1                             0x0200
#define G0_PLLFREQ1_MULTIPLE_28                 0x1C
#define G0_PLLFREQ1_MULTIPLE_32                 0x20
#define G0_PLLFREQ1_MULTIPLE_40                 0x28
#define G0_PLLFREQ1_MULTIPLE_48                 0x30
#define G0_PLLFREQ3                             0x0208
#define G0_ODIVCH0                              0x0240
#define G0_ODIVCH1                              0x0244
#define G0_CMDEN                                0x0280
#define G0_SMSLPMSK                             0x02B0
#define G0_SMSLPCNT                             0x02B4
#define G0_DPSLPMSK                             0x02B8
#define G0_DPSLPCNT                             0x02BC
#define G0_GCRGST                               0x02F0
#define G0_CRSMSLPCNT                           0x03B4
#define G0_CRDPSLPCNT                           0x03BC
#define G0L0_LCRUPC                             0x040C
#define G0L0_LCRCOC0                            0x0410
#define G0L1_LCRUPC                             0x060C
#define G0L1_LCRCOC0                            0x0610

/* G1 clock domain */
#define G1_PLLFREQ1                             0x1200
#define G1_PLLFREQ1_MULTIPLE_32                 0xA0
#define G1_PLLFREQ1_MULTIPLE_36                 0x24
#define G1_PLLFREQ1_MULTIPLE_48                 0xB8
#define G1_PLLFREQ3                             0x1208
#define G1_ODIVCH0                              0x1240
#define G1_ODIVCH1                              0x1244
#define G1_CMDEN                                0x1280
#define G1_SMSLPMSK                             0x12B0
#define G1_SMSLPCNT                             0x12B4
#define G1_DPSLPMSK                             0x12B8
#define G1_DPSLPCNT                             0x12BC
#define G1_GCRGST                               0x12F0
#define G1_CRSMSLPCNT                           0x13B4
#define G1_CRDPSLPCNT                           0x13BC
#define G1L0_LCRUPC                             0x140C
#define G1L0_LCRCOC0                            0x1410
#define G1L0_LCRCOC1                            0x1414
#define G1L1_LCRUPC                             0x160C
#define G1L1_LCRCOC0                            0x1610
#define G1L1_LCRCOC1                            0x1614
#define G1L1_LCRCOC2                            0x1618

/* G2 clock domain */
#define G2_PLLFREQ1                             0x2200
#define G2_PLLFREQ1_MULTIPLE_36                 0x24
#define G2_PLLFREQ1_MULTIPLE_48                 0x30
#define G2_PLLFREQ3                             0x2208
#define G2_ODIVCH0                              0x2240
#define G2_ODIVCH1                              0x2244
#define G2_ODIVCH2                              0x2248
#define G2_ODIVCH3                              0x224C
#define G2_CMDEN                                0x2280
#define G2_SMSLPMSK                             0x22B0
#define G2_SMSLPCNT                             0x22B4
#define G2_DPSLPMSK                             0x22B8
#define G2_DPSLPCNT                             0x22BC
#define G2_GCRGST                               0x22F0
#define G2_CRSMSLPCNT                           0x23B4
#define G2_CRDPSLPCNT                           0x23BC
#define G2L0_LCRUPC                             0x240C
#define G2L0_LCRCOC0                            0x2410
#define G2L0_LCRCOC1                            0x2414
#define G2L1_LCRUPC                             0x260C
#define G2L1_LCRCOC0                            0x2610
#define G2L2_LCRUPC                             0x280C
#define G2L2_LCRCOC0                            0x2810
#define G2L2_LCRCOC1                            0x2814
#define G2L2_LCRCOC2                            0x2818
#define G2L2_LCRCOC3                            0x281C
#define G2L3_LCRUPC                             0x2A0C
#define G2L3_LCRCOC0                            0x2A10
#define G2L3_LCRCOC1                            0x2A14

/* G3 clock domain */
#define G3_PLLFREQ1                             0x3200
#define G3_PLLFREQ1_MULTIPLE_24                 0x98
#define G3_PLLFREQ1_MULTIPLE_32                 0xA0

#define G3_PLLFREQ3                             0x3208
#define G3_ODIVCH0                              0x3240
#define G3_ODIVCH1                              0x3244
#define G3_CMDEN                                0x3280
#define G3_SMSLPMSK                             0x32B0
#define G3_SMSLPCNT                             0x32B4
#define G3_DPSLPMSK                             0x32B8
#define G3_DPSLPCNT                             0x32BC
#define G3_GCRGST                               0x32F0
#define G3_CRSMSLPCNT                           0x33B4
#define G3_CRDPSLPCNT                           0x33BC
#define G3L0_LCRUPC                             0x340C
#define G3L0_LCRCOC0                            0x3410
#define G3L0_LCRCOC1                            0x3414

#define LPCCCTL1_UMC1                           0x800C
#define LPCCCTL1_UMC2                           0x8014
#define LPCCCTL1_UMC3                           0x801C
#define LPCCCTL1_UMC4                           0x8024
#define LPCCCTL1_UMC5                           0x802C
#define LPCCCTL1_UMC6                           0x8034
#define LPCCCTL1_UMC7                           0x803C
#define LPCCCTL1_UMC8                           0x8044
#define LPCCCTL1_UMC9                           0x804C
#define LPCCCTL1_UMC10                          0x8054
#define LPCCCTL1_UMC11                          0x805C
#define LPCCCTL1_UMC12                          0x8064
#define LPCCCTL1_UMC13                          0x806C
#define LPCCCTL1_UMC14                          0x8074
#define LPCCCTL1_UMC15                          0x807C
#define LPCCCTL1_UMC16                          0x8084
#define LPCCCTL1_UMC17                          0x808C


#define LPRCTL0_CA9                             0x18400
#define LPRMON0_CA9                             0x18404
#define LPRCTL1_CA9                             0x18408
#define LPRMON1_CA9                             0x1840C
#define LPRCTL2_CA9                             0x18410
#define LPRMON2_CA9                             0x18414
#define LPRCTL3_CA9                             0x18418
#define LPRMON3_CA9                             0x1841C

/* -------------------------------------------------------------------------
 * Chip Controller
 * -------------------------------------------------------------------------
 */
#define SC1810X_CCNT_BASE                       0x3D100000


/* -------------------------------------------------------------------------
 * UART
 * -------------------------------------------------------------------------
 */
#define SC1810X_UART0_BASE                      0x3E000000
#define SC1810X_UART1_BASE                      0x3E010000
#define SC1810X_UART2_BASE                      0x3E020000
#define SC1810X_UART3_BASE                      0x3E030000
#define SC1810X_UART4_BASE                      0x3E040000
#define SC1810X_UART5_BASE                      0x3E050000

#define SC1810X_UART0_IRQ                       168
#define SC1810X_UART1_IRQ                       169
#define SC1810X_UART2_IRQ                       170
#define SC1810X_UART3_IRQ                       171
#define SC1810X_UART4_IRQ                       172
#define SC1810X_UART5_IRQ                       173


/* -------------------------------------------------------------------------
 * DDR SDRAM
 * -------------------------------------------------------------------------
 */
#define SC1810X_DDR0_BASE                       0x40000000
#define SC1810X_DDR1_BASE                       0x80000000
#define SC1810X_DDR_BANK1_SIZE                  0x40000000
#define SC1810X_DDR_BANK1_SIZE                  0x40000000


/* -------------------------------------------------------------------------
 * Watchdog Timer B
 * -------------------------------------------------------------------------
 */
#define SC1810X_WDGTB_BASE                      0x04A00000
#define SC1810X_WDGTB_SIZE                      0x1000

/* Watchdog Protection Register */
#define WDG_PROT                                0x00
#define WDG_PROT_UNLOCK_KEY                     0xEDACCE55

/* Watchdog Counter Register */
#define WDG_CNT                                 0x08

/* Watchdog Reset Cause Register */
#define WDG_RSTCAUSE                            0x0C
#define WDG_RSTCAUSE_BIT0                       BIT(0)
#define WDG_RSTCAUSE_BIT1                       BIT(1)
#define WDG_RSTCAUSE_BIT2                       BIT(2)
#define WDG_RSTCAUSE_BIT3                       BIT(3)
#define WDG_RSTCAUSE_BIT4                       BIT(4)

/* Watchdog Trigger0 Register */
#define WDG_TRG0                                0x10
/* Bit to be written with TRG0CFG value */
#define WDG_TRG0_WDGTRG0_MASK                   0x0000FFFF

/* Watchdog Trigger1 Register */
#define WDG_TRG1                                0x18
/* Bit to be written with TRG1CFG value */
#define WDG_TRG1_WDGTRG1_MASK                   0x0000FFFF

/* Watchdog Interrupt Configuration Register */
#define WDG_INT                                 0x20
/* Reset/INT configuration bit */
#define WDG_INT_EN                              BIT(17)
/* Pre-Warn interrupt enable */
#define WDG_PRE_EN                              BIT(16)
/* Watchdog interrupt error condition detected */
#define WDG_INT_FLAG                            BIT(1)
/* Watchdog Pre-Warn error condition detected */
#define WDG_PREWARN_FLAG                        BIT(0)

/* Watchdog Interrupt Clear Register */
#define WDG_INTCLR                              0x24
/* Clear INT_FLAG */
#define WDG_INTCLR_INT_CLR                      BIT(1)
/* Clear PRE_FLAG */
#define WDG_INTCLR_PREWARN_CLR                  BIT(0)

/* Watchdog Trigger0 Configuration Register */
#define WDG_TRG0CFG                             0x2C
#define WDG_TRG0CFG_MASK                        0x000000FF
#define TRIGGERKEY_0                            0x00

/* Watchdog Trigger1 Configuration Register */
#define WDG_TRG1CFG                             0x30
#define WDG_TRG1CFG_MASK                        0x000000FF
#define TRIGGERKEY_1                            0x00

/* Watchdog RUN Registers */
/* Lower limit register */
#define WDG_RUNLL                               0x34
#define DEFAULT_RUNLL                           0x00000000

/* Upper Limit RUN Register */
#define WDG_RUNUL                               0x38
#define DEFAULT_RUNUL                           0x01000000

/* Watchdog PSS Registers */
/* Lower limit register */
#define WDG_PSSLL                               0x3C
#define DEFAULT_PSSLL                           0x00000000

/* Upper Limit RUN Register */
#define WDG_PSSUL                               0x40
#define DEFAULT_PSSUL                           0x80000000

/* Watchdog Reset Delay Counter Register */
#define WDG_RSTDLY                              0x44
#define WDG_RSTDLY_MASK                         (0x0000FFFF)
#define DEFAULT_RESET_CLOCK_CYCLES              0x2
#define DEFAULT_RESET_DELAY                     0x00FF
#define MAX_RSTDLY                              0xFFFF

/* Watchdog Configuration Register */
#define WDG_CFG                                 0x48

/* Lock bit: to prevent wdt reconfiguration */
#define WDG_CFG_LOCK                            BIT(24)
#define WDG_CFG_LOCK_BITMASK                    (0x01000000)

/* Enable the counter in RUN state */
#define WDG_CFG_WD_EN_RUN                       BIT(0)
#define WDG_CFG_WD_EN_RUN_BITMASK               (0x00000001)

/* Enable the counter in PSS state */
#define WDG_CFG_WD_EN_PSS                       BIT(1)

/* Allow stoping the clock in PSS state */
#define WDG_CFG_ALLOW_STOP_CLOCK                BIT(2)

/* Enable/Disable debugging */
#define WDG_CFG_DEBUG_EN                        BIT(3)

/* Clock Select bits */
#define WDG_CFG_CLK_SEL_RC                      (0 << 8)
#define WDG_CFG_CLK_SEL_SLOW_RC                 (1 << 8)
#define WDG_CFG_CLK_SEL_SUB                     (2 << 8)
#define WDG_CFG_CLK_SEL_MAIN                    (3 << 8)

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif

