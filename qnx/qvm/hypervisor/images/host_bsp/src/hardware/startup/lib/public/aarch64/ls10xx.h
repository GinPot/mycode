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
 * NXP LS10XX SOCs:
 */

#ifndef __AARCH64_LS10XX_H_INCLUDED
#define __AARCH64_LS10XX_H_INCLUDED


/*
 * System Memory Map
 */

/*
 * CCSR address map
 */

 /*
  * CPLD address map
  */

#define LS10XX_CPLD_BASE            0x7FB00000
#define LS10XX_CPLD_SIZE            0x18

#define LS10XX_CPLD_VER             0x00
#define LS10XX_CPLD_VER_SUB         0x01
#define LS10XX_CPLD_PCBA_VER        0x02
#define LS10XX_CPLD_SYSTEM_RST      0x03
#define LS10XX_CPLD_SOFT_MUX_ON     0x04
    #define LS10XX_CPLD_SOFT_MUX_ON_EVDD_CTRL_EN    (1 << 7)
#define LS10XX_CPLD_REG_RCW_SRC1    0x05
#define LS10XX_CPLD_REG_RCW_SRC2    0x06
#define LS10XX_CPLD_REG_BANK        0x07
#define LS10XX_CPLD_SYSCLK_SEL      0x08
#define LS10XX_CPLD_UART_SEL        0x09
#define LS10XX_CPLD_SD1REFCLK_SEL   0x0A
#define LS10XX_CPLD_TDMCLK_MUX_SEL  0x0B
#define LS10XX_CPLD_SDHC_SPICS_SEL  0x0C
#define LS10XX_CPLD_STATUS_LED      0x0D
#define LS10XX_CPLD_GLOBAL_RST      0x0E
#define LS10XX_CPLD_TDMR_PRS_N      0x0F
#define LS10XX_CPLD_REG_RTC         0x10
#define LS10XX_CPLD_EVDD_SEL        0x11
    #define LS10XX_CPLD_EVDD_SEL_EVDD_SEL           (1 << 0)

/*
 * GIC-400
 */
#define LS10XX_GIC_BASE             0x01400000

/*
 * Quad serial peripheral interface (QSPI)
 */
#define LS1XX_QSPI_BASE             0x01550000
#define LS1XX_QSPI_SIZE             0x10000

/*
 * SCFG Supplement Configuration Unit */
#define LS10XX_SCFG_BASE            0x01570000
#define LS10XX_SCFG_SIZE            0x4000

/*
 * Serial peripheral interface (SPI)
 */
#define LS10XX_SPI1_BASE            0x02100000
#define LS10XX_SPI1_SIZE            0x10000

/*
 * Enhanced direct memory access (eDMA)
 */
#define LS10XX_EDMA_BASE            0x02C00000
#define LS10XX_EDMA_SIZE            0x10000

/*
 * Direct memory access multiplexer (DMAMUX)
 */
#define LS10XX_DMAMUX1_BASE         0x02C10000
#define LS10XX_DMAMUX2_BASE         0x02C20000
#define LS10XX_DMAMUX_SIZE          0x10000
#define LS10XX_DMAMUX_NUM_SLOTS     64

/*
 * SATA
 */
#define LS10XX_SATA_BASE            0x03200000
#define LS10XX_SATA_SIZE            0x10000

#endif  /* __AARCH64_LS10XX_H_INCLUDED */


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/aarch64/ls10xx.h $ $Rev: 832326 $")
#endif
