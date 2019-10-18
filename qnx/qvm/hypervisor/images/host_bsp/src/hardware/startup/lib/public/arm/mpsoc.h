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
 * Xilinx Zynq UltraScale+ MPSoC processor with ARMv8 Cortex-A53
 */

#ifndef    __ARM_MPSOC_H_INCLUDED
#define    __ARM_MPSOC_H_INCLUDED

#include <arm/xzynq.h>

/* -------------------------------------
 * Clock
 * -------------------------------------
 */
#define MPSOC_PSS_REF_CLOCK         33330000UL


/* -------------------------------------
 * TIMER
 * -------------------------------------
 */
#define MPSOC_TIMER_FREQ            100000000


/* -------------------------------------
 * Network
 * -------------------------------------
 */
#define MPSOC_EMAC0_BASE            0xFF0B0000
#define MPSOC_EMAC1_BASE            0xFF0C0000
#define MPSOC_EMAC2_BASE            0xFF0D0000
#define MPSOC_EMAC3_BASE            0xFF0E0000

/* -------------------------------------
 * SD/MMC
 * -------------------------------------
 */
#define MPSOC_SDIO0_BASEADDR        0xFF160000
#define MPSOC_SDIO1_BASEADDR        0xFF170000

/* -------------------------------------
 * GPIO
 * -------------------------------------
 */

#define MPSOC_GPIO_BASE             0xFF0A0000
#define MPSOC_GPIO_SIZE             0x400

/* -------------------------------------
 * SPI
 * -------------------------------------
 */

#define MPSOC_SPI0_BASE             0xFF040000
#define MPSOC_SPI1_BASE             0xFF050000
#define MPSOC_SPI_SIZE              0x400


/* -------------------------------------------------------------------------
 * ARM / SMP
 * -------------------------------------------------------------------------
 */
#define MPSOC_CRF_APB_BASE          0xFD1A0000
#define MPSOC_RST_FPD_APU           0x104

#define MPSOC_APU_BASE              0xFD5C0000
#define MPSOC_APU_CONFIG_0          0x20
#define MPSOC_APU_CONFIG_1          0x24
#define MPSOC_RST_VECTOR_CORE0_L    0x40
#define MPSOC_RST_VECTOR_CORE0_H    0x44
#define MPSOC_RST_VECTOR_CORE1_L    0x48
#define MPSOC_RST_VECTOR_CORE1_H    0x4C
#define MPSOC_RST_VECTOR_CORE2_L    0x50
#define MPSOC_RST_VECTOR_CORE2_H    0x54
#define MPSOC_RST_VECTOR_CORE3_L    0x58
#define MPSOC_RST_VECTOR_CORE3_H    0x5C

#define MPSOC_CPU1_START_ADDRESS    0xFFFFFF0
#define MPSOC_CPU_START_ADDRESS     0xFFFF0000


/* ARM Generic Interrupt Controller (GIC400) Distributor base */
#define MPSOC_GIC_BASE                0xF9010000
/* CPU interface registers offset */
#define MPSOC_GICC_OFFSET             0x10000

/* -------------------------------------------------------------------------
 * Clocks
 * -------------------------------------------------------------------------
 */
#define MPSOC_PERIPH_CLOCK            100000000

/* -------------------------------------------------------------------------
 * Serial ports
 * -------------------------------------------------------------------------
 */

/* UART base addresses */
#define    MPSOC_XUARTPS_UART0_BASE    0xFF000000
#define    MPSOC_XUARTPS_UART1_BASE    0xFF010000

#define    MPSOC_XUARTPS_UART_SIZE     0x1000

/* -------------------------------------
 * EMAC
 * -------------------------------------
 */
#define MPSOC_EMAC0_BASE            0xFF0B0000
#define MPSOC_EMAC1_BASE            0xFF0C0000
#define MPSOC_EMAC2_BASE            0xFF0D0000
#define MPSOC_EMAC3_BASE            0xFF0E0000
#define MPSOC_EMAC_SIZE             0x1000

/* -------------------------------------------------------------------------
 * I2C bus
 * -------------------------------------------------------------------------
 */

#define MPSOC_IRQ_I2C1                 49
#define MPSOC_IRQ_I2C2                 50
#define MPSOC_XIICPS1_BASE_ADDR        0xFF020000
#define MPSOC_XIICPS2_BASE_ADDR        0xFF030000
#define MPSOC_XIICPS_REG_SIZE          0x30

/* -------------------------------------------------------------------------
 * USB
 * -------------------------------------------------------------------------
 */
#define MPSOC_USB0_XHCI_BASE        0xFE200000
#define MPSOC_USB1_XHCI_BASE        0xFE300000


/* -------------------------------------------------------------------------
 * Watchdog
 * -------------------------------------------------------------------------
 */
#define XZYNQ_LPD_WDT_BASEADDR        0xFF150000
#define XZYNQ_FPD_WDT_BASEADDR        0xFD4D0000
#define XZYNQ_CSU_WDT_BASEADDR        0xFFCB0000
#define MPSOC_WDT_SIZE                0x10
#define MPSOC_WDT_CLOCK_FREQ          100000000

/* Interrupts */
#define MPSOC_IRQ_GLOBAL_TIMER        27

#define MPSOC_IRQ_GPIO                48
#define MPSOC_IRQ_SPI0                51
#define MPSOC_IRQ_SPI1                52
#define MPSOC_IRQ_UART0               53
#define MPSOC_IRQ_UART1               54
#define MPSOC_IRQ_CAN0                55
#define MPSOC_IRQ_CAN1                56
#define MPSOC_IRQ_GEM0                89
#define MPSOC_IRQ_GEM0_WAKEUP         90
#define MPSOC_IRQ_GEM1                91
#define MPSOC_IRQ_GEM1_WAKEUP         92
#define MPSOC_IRQ_GEM2                93
#define MPSOC_IRQ_GEM2_WAKEUP         94
#define MPSOC_IRQ_GEM3                95
#define MPSOC_IRQ_GEM3_WAKEUP         96

/* -------------------------------------------------------------------------
 * SDRAM configuration
 * -------------------------------------------------------------------------
 */
#define ZCU102_SDRAM_BANK1_BASE        0
#define ZCU102_SDRAM_BANK2_BASE        MEG(32 * 1024)
#define ZCU102_SDRAM_BANK1_SIZE        MEG(2048)
#define ZCU102_SDRAM_BANK2_SIZE        MEG(2048)

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/arm/mpsoc.h $ $Rev: 853703 $")
#endif
