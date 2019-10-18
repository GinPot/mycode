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

/*
 * NXP LS1012A SOCs:
 */

#ifndef __AARCH64_LS1012A_H_INCLUDED
#define __AARCH64_LS1012A_H_INCLUDED

#include "aarch64/ls10xx.h"

/*
 * System Memory Map
 */

/*
 * CCSR address map
 */

/*
 * Enhanced secured digital host controller (eSHDC)
 */
#define LS1012A_eSDHC1_BASE     0x01560000
#define LS1012A_eSDHC2_BASE     0x01580000
#define LS1012A_eSDHC_SIZE      0x10000

/*
 * Device configuration and pin control (DCFG)
 */
#define LS1012A_DCFG_BASE       0x01EE0000
#define LS1012A_DCFG_SIZE       0x100
    #define LS1012A_DCFG_PORSR1      0x0
    #define LS1012A_DCFG_SVR         0xA4
        #define DCFG_SVR_MAJOR_REV_MASK     0xF0
        #define DCFG_SVR_MAJOR_REV_1        0x10
        #define DCFG_SVR_MAJOR_REV(x)       ((x & DCFG_SVR_MAJOR_REV_MASK) >> 4)
        #define DCFG_SVR_MINOR_REV_MASK     0x0F
        #define DCFG_SVR_MINOR_REV_0        0x00
        #define DCFG_SVR_MINOR_REV_1        0x01
        #define DCFG_SVR_MINOR_REV(x)       (x & DCFG_SVR_MINOR_REV_MASK)
    #define LS1012A_DCFG_RSTCR       0xB0
        #define DCFG_RSTCR_RST          (1 << 25)
    #define LS1012A_DCFG_RCW         0x100

/*
 * Clocking module
 */
#define LS1012A_CM_BASE         0x01EE1000
#define LS1012A_CM_SIZE         0x200

/*
 * I2C controllers
 */
#define LS1012A_I2C1_BASE       0x02180000
#define LS1012A_I2C2_BASE       0x02190000
#define LS1012A_I2C_SIZE        0x6

/*
 * DUART
 */
#define LS1012A_DUART1_BASE     0x021C0500
#define LS1012A_DUART_SIZE      0x100

/*
 * GPIOs
 */
#define LS1012A_GPIO1_BASE      0x02300000
#define LS1012A_GPIO2_BASE      0x02310000
#define LS1012A_GPIO_SIZE       0x10000

/*
 * FlexTimer module (FTM)
 */
#define LS1012A_FTM1_BASE       0x029D0000
#define LS1012A_FTM2_BASE       0x029E0000
#define LS1012A_FTM_SIZE        0x10000

/*
 * Watchdog timer
 */
#define LS1012A_WDOG1_BASE      0x02AD0000
#define LS1012A_WDOG2_BASE      0x02AE0000
#define LS1012A_WDOG_SIZE       0x8

/*
 * 2 USB Controllers, 1 PHY
 */
#define LS1012A_USB3_BASE        0x02F00000
#define LS1012A_USB2_BASE        0x08600000
#define LS1012A_USB_SIZE         0x100000

#define LS1012A_USB_PHY_BASE     0x084F0000
#define LS1012A_USB_PHY_SIZE     0x10000

/*
 * IRQ assignments
 */
#define LS1012A_IRQ_TMU_ALARM           65  /* Thermal monitor unit alarm */
#define LS1012A_IRQ_TMU_CALARM          66  /* Thermal monitor unit critical alarm */
#define LS1012A_IRQ_DUART1              86
#define LS1012A_IRQ_I2C1                88
#define LS1012A_IRQ_I2C2                89
#define LS1012A_IRQ_USB1                92
#define LS1012A_IRQ_eSDHC1              94
#define LS1012A_IRQ_SPI1                96
#define LS1012A_IRQ_eSDHC2              97
#define LS1012A_IRQ_GPIO1               98
#define LS1012A_IRQ_GPIO2               99
#define LS1012A_IRQ_GPIO3               100
#define LS1012A_IRQ_SATA                101
#define LS1012A_IRQ_EPU                 102
#define LS1012A_IRQ_SEC_QUE1            103
#define LS1012A_IRQ_SEC_QUE2            104
#define LS1012A_IRQ_SEC_QUE3            105
#define LS1012A_IRQ_SEC_QUE4            106
#define LS1012A_IRQ_SEC_GLOBAL          107
#define LS1012A_IRQ_PLAT_CTRL           108
#define LS1012A_IRQ_uQE                 109  /* uQE (QE interrupt + QE critical + QE error) */
#define LS1012A_IRQ_SEC_MON             110
#define LS1012A_IRQ_CSU                 112
#define LS1012A_IRQ_WDOG1               115
#define LS1012A_IRQ_WDOG2               116
#define LS1012A_IRQ_FTM1                118
#define LS1012A_IRQ_FTM2                119
#define LS1012A_IRQ_SCFG_PFE            122
#define LS1012A_IRQ_TZASC               125
#define LS1012A_IRQ_QSPI                131
#define LS1012A_IRQ_EDMA                135
#define LS1012A_IRQ_CORE0_CTI           136
#define LS1012A_IRQ_CORE0_PMU           138
#define LS1012A_IRQ_AXI_ERR             140
#define LS1012A_IRQ_CCI400              141
#define LS1012A_IRQ_PEX1_INTA           142
#define LS1012A_IRQ_PEX1_INTB           143
#define LS1012A_IRQ_PEX1_INTC           144
#define LS1012A_IRQ_PEX1_INTD           145
#define LS1012A_IRQ_PEX1_MSI            148
#define LS1012A_IRQ_PEX1_PME            149
#define LS1012A_IRQ_PEX1_CFGERR         150
#define LS1012A_IRQ_USB2                171
#define LS1012A_IRQ_SAI1                180
#define LS1012A_IRQ_SAI2                181
#define LS1012A_IRQ_SAI3                182
#define LS1012A_IRQ_SAI4                183
#define LS1012A_IRQ_SAI5                184
#define LS1012A_IRQ_SAI6                185
#define LS1012A_IRQ_CORE0_SRESET        196
#define LS1012A_IRQ_PFE_HIF_INT         204
#define LS1012A_IRQ_PFE_HIF_NCOPY_INT   205
#define LS1012A_IRQ_PFE_HIF_TMR_INT     206
#define LS1012A_IRQ_COMM_IRQ0           224
#define LS1012A_IRQ_MBEE                228
#define LS1012A_IRQ_CORE0_VIRT_CPU_INT  229

/*
 * DMA Chanels assignments
 */
#define LS1012A_DMAMUX1_CHANNEL_OFFSET   0
#define LS1012A_DMAMUX2_CHANNEL_OFFSET   (1 * LS10XX_DMAMUX_NUM_SLOTS)

#define LS1012A_DMA_FROM_EPU            (LS1012A_DMAMUX1_CHANNEL_OFFSET + 1)
#define LS1012A_DMA_FLEX_TIMER2_0       (LS1012A_DMAMUX1_CHANNEL_OFFSET + 35)
#define LS1012A_DMA_FLEX_TIMER2_1       (LS1012A_DMAMUX1_CHANNEL_OFFSET + 36)
#define LS1012A_DMA_FLEX_TIMER2_2       (LS1012A_DMAMUX1_CHANNEL_OFFSET + 37)
#define LS1012A_DMA_FLEX_TIMER2_3       (LS1012A_DMAMUX1_CHANNEL_OFFSET + 38)
#define LS1012A_DMA_FLEX_TIMER1_0       (LS1012A_DMAMUX1_CHANNEL_OFFSET + 43)
#define LS1012A_DMA_FLEX_TIMER1_1       (LS1012A_DMAMUX1_CHANNEL_OFFSET + 44)
#define LS1012A_DMA_FLEX_TIMER1_2       (LS1012A_DMAMUX1_CHANNEL_OFFSET + 45)
#define LS1012A_DMA_FLEX_TIMER1_3       (LS1012A_DMAMUX1_CHANNEL_OFFSET + 46)
#define LS1012A_DMA_SPI1_RFDF           (LS1012A_DMAMUX1_CHANNEL_OFFSET + 60)
#define LS1012A_DMA_SPI1_CMD            (LS1012A_DMAMUX1_CHANNEL_OFFSET + 61)
#define LS1012A_DMA_SPI1_TF             (LS1012A_DMAMUX1_CHANNEL_OFFSET + 62)

#define LS1012A_DMA_QSPI_RFDF           (LS1012A_DMAMUX2_CHANNEL_OFFSET + 19)
#define LS1012A_DMA_SAI5_RX             (LS1012A_DMAMUX2_CHANNEL_OFFSET + 34)
#define LS1012A_DMA_SAI5_TX             (LS1012A_DMAMUX2_CHANNEL_OFFSET + 35)
#define LS1012A_DMA_I2C2_RX             (LS1012A_DMAMUX2_CHANNEL_OFFSET + 36)
#define LS1012A_DMA_I2C2_TX             (LS1012A_DMAMUX2_CHANNEL_OFFSET + 37)
#define LS1012A_DMA_I2C1_RX             (LS1012A_DMAMUX2_CHANNEL_OFFSET + 38)
#define LS1012A_DMA_I2C1_TX             (LS1012A_DMAMUX2_CHANNEL_OFFSET + 39)
#define LS1012A_DMA_SAI4_RX             (LS1012A_DMAMUX2_CHANNEL_OFFSET + 40)
#define LS1012A_DMA_SAI4_TX             (LS1012A_DMAMUX2_CHANNEL_OFFSET + 41)
#define LS1012A_DMA_SAI3_RX             (LS1012A_DMAMUX2_CHANNEL_OFFSET + 42)
#define LS1012A_DMA_SAI3_TX             (LS1012A_DMAMUX2_CHANNEL_OFFSET + 43)
#define LS1012A_DMA_SAI2_RX             (LS1012A_DMAMUX2_CHANNEL_OFFSET + 44)
#define LS1012A_DMA_SAI2_TX             (LS1012A_DMAMUX2_CHANNEL_OFFSET + 45)
#define LS1012A_DMA_SAI1_RX             (LS1012A_DMAMUX2_CHANNEL_OFFSET + 46)
#define LS1012A_DMA_SAI1_TX             (LS1012A_DMAMUX2_CHANNEL_OFFSET + 47)

#endif /* __AARCH64_LS1012A_H_INCLUDED */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/aarch64/ls1012a.h $ $Rev: 848103 $")
#endif
