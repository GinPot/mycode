/*
 * $QNXLicenseC:
 * Copyright 2015, 2017 QNX Software Systems.
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
 * Renesas R-Car processor with ARMv7
 * Gen2 shared definitions: H2, M2, W2H, E2X
 * _EREV stands for Early HW Rev Definitions
 */

#ifndef __ARM_RCAR_GEN2_H_INCLUDED
#define __ARM_RCAR_GEN2_H_INCLUDED

#define RCAR_GEN2_A7_CLK                 780000000UL     /* Fixed clock for A7 core */
#define RCAR_GEN2_EXT_CLK                20000000        /* External crystal clock */

/*
 * GPIO
 */
#define RCAR_GEN2_GPIO0_BASE             0xE6050000
#define RCAR_GEN2_GPIO1_BASE             0xE6051000
#define RCAR_GEN2_GPIO2_BASE             0xE6052000
#define RCAR_GEN2_GPIO3_BASE             0xE6053000
#define RCAR_GEN2_GPIO4_BASE             0xE6054000
#define RCAR_GEN2_GPIO5_BASE             0xE6055000
#define RCAR_GEN2_GPIO6_BASE             0xE6055400
#define RCAR_GEN2_GPIO7_BASE             0xE6055800

 /* GPIO Registers, offset from GPIO_BASE */
 #define RCAR_GEN2_GPIO_IOINTSEL         0x0000
 #define RCAR_GEN2_GPIO_INOUTSEL         0x0004
 #define RCAR_GEN2_GPIO_OUTDT            0x0008
 #define RCAR_GEN2_GPIO_INDT             0x000C
 #define RCAR_GEN2_GPIO_INTDT            0x0010
 #define RCAR_GEN2_GPIO_INTCLR           0x0014
 #define RCAR_GEN2_GPIO_INTMSK           0x0018
 #define RCAR_GEN2_GPIO_MSKCLR           0x001C
 #define RCAR_GEN2_GPIO_POSNEG           0x0020
 #define RCAR_GEN2_GPIO_EDGLEVEL         0x0024
 #define RCAR_GEN2_GPIO_FILONOFF         0x0028
 #define RCAR_GEN2_GPIO_INTMSKS          0x0038
 #define RCAR_GEN2_GPIO_MSKCLRS          0x003C
 #define RCAR_GEN2_GPIO_OUTDTSEL         0x0040
 #define RCAR_GEN2_GPIO_OUTDTH           0x0044
 #define RCAR_GEN2_GPIO_OUTDTL           0x0048
 #define RCAR_GEN2_GPIO_BOTHEDGE         0x004C


/*
 * Pin Function Control
 */
#define RCAR_GEN2_PFC_BASE               0xE6060000

 /* PFC registers, offset from RCAR_GEN2_PFC_BASE */
 #define RCAR_GEN2_PFC_PMMR              0x00
 #define RCAR_GEN2_PFC_GPSR0             0x04
 #define RCAR_GEN2_PFC_GPSR1             0x08
 #define RCAR_GEN2_PFC_GPSR2             0x0C
 #define RCAR_GEN2_PFC_GPSR3             0x10
 #define RCAR_GEN2_PFC_GPSR4             0x14
 #define RCAR_GEN2_PFC_GPSR5             0x18
 #define RCAR_GEN2_PFC_IPSR0             0x20
 #define RCAR_GEN2_PFC_IPSR1             0x24
 #define RCAR_GEN2_PFC_IPSR2             0x28
 #define RCAR_GEN2_PFC_IPSR3             0x2C
 #define RCAR_GEN2_PFC_IPSR4             0x30
 #define RCAR_GEN2_PFC_IPSR5             0x34
 #define RCAR_GEN2_PFC_IPSR6             0x38
 #define RCAR_GEN2_PFC_IPSR7             0x3C
 #define RCAR_GEN2_PFC_IPSR8             0x40
 #define RCAR_GEN2_PFC_IPSR9             0x44
 #define RCAR_GEN2_PFC_IPSR10            0x48
 #define RCAR_GEN2_PFC_IPSR11            0x4C
 #define RCAR_GEN2_PFC_IPSR12            0x50
 #define RCAR_GEN2_PFC_IPSR13            0x54
 #define RCAR_GEN2_PFC_IPSR14            0x58
 #define RCAR_GEN2_PFC_IPSR15            0x5C
 #define RCAR_GEN2_PFC_IPSR16            0x160
 #define RCAR_GEN2_PFC_MODSEL            0x90
 #define RCAR_GEN2_PFC_MODSEL2           0x94
 #define RCAR_GEN2_PFC_MODSEL3           0x98
 #define RCAR_GEN2_PFC_PUPR0             0x100
 #define RCAR_GEN2_PFC_PUPR1             0x104
 #define RCAR_GEN2_PFC_PUPR2             0x108
 #define RCAR_GEN2_PFC_PUPR3             0x10C
 #define RCAR_GEN2_PFC_PUPR4             0x110
 #define RCAR_GEN2_PFC_PUPR5             0x114
 #define RCAR_GEN2_PFC_PUPR6             0x118
 #define RCAR_GEN2_PFC_IOCTL0            0x60
 #define RCAR_GEN2_PFC_IOCTL1            0x64
 #define RCAR_GEN2_PFC_IOCTL4            0x84
 #define RCAR_GEN2_PFC_IOCTL5            0x88
 #define RCAR_GEN2_PFC_IOCTL6            0x8C
 #define RCAR_GEN2_PFC_IOCTL7            0x70
 #define RCAR_GEN2_PFC_DDR3GPEN          0x240
 #define RCAR_GEN2_PFC_DDR3GPOE          0x244
 #define RCAR_GEN2_PFC_DDR3GPOD          0x248
 #define RCAR_GEN2_PFC_DDR3GPID          0x24C

/*
 * Clock Pulse Generator
 */
#define RCAR_GEN2_CPG_BASE               0xE6150000
#define RCAR_GEN2_CPG_SIZE               0x1000

 /* CPG registers, offset from RCAR_GEN2_CPG_BASE */
 #define RCAR_GEN2_CPG_FRQCRB            0x004       // Frequency Control Register B
 #define RCAR_GEN2_CPG_FRQCRC            0x0E0       // Frequency Control Register C
 #define RCAR_GEN2_CPG_PLLECR            0x0D0       // PLL Enable Control Register
 #define RCAR_GEN2_CPG_PLL0CR            0x0D8       // PLL0 Control Register
 #define RCAR_GEN2_CPG_RGXCR             0x0B4       // RGX Control Register
 #define RCAR_GEN2_CPG_SDCKCR            0x074       // SDHI Clock Frequency Control Register
 #define RCAR_GEN2_CPG_SD2CKCR           0x078       // SDHI 2 Clock Frequency Control Register
 #define RCAR_GEN2_CPG_SD3CKCR           0x26C       // SDHI 2 Clock Frequency Control Register
 #define RCAR_GEN2_CPG_GPUCKCR           0x234       // GPU Clock Frequency Control Register
 #define RCAR_GEN2_CPG_MMC0CKCR          0x240       // MMC0 Clock Frequency Control Register
 #define RCAR_GEN2_CPG_MMC1CKCR          0x244       // MMC1 Clock Frequency Control Register
 #define RCAR_GEN2_CPG_ADSPCKCR          0x25C       // ADSP Clock Frequency Control Register
 #define RCAR_GEN2_CPG_SSPCKCR           0x248       // SSP Clock Frequency Control Register
 #define RCAR_GEN2_CPG_SSPRSCKCR         0x24C       // SSPRS Clock Frequency Control Register
 #define RCAR_GEN2_CPG_RCANCKCR          0x270       // RCAN Clock Frequency Control Register
 #define RCAR_GEN2_CPG_FMMCKCR           0x274       // FMM Clock Frequency Control Register
 #define RCAR_GEN2_CPG_DVFSCR0           0x058       // DVFS Control Register 0
 #define RCAR_GEN2_CPG_DVFSCR1           0x05C       // DVFS Control Register 1

 #define RCAR_GEN2_CPG_MSTPSR0           0x030       // Module Stop Status Register 0
 #define RCAR_GEN2_CPG_MSTPSR1           0x038       // Module Stop Status Register 1
 #define RCAR_GEN2_CPG_MSTPSR2           0x040       // Module Stop Status Register 2
 #define RCAR_GEN2_CPG_MSTPSR3           0x048       // Module Stop Status Register 3
 #define RCAR_GEN2_CPG_MSTPSR4           0x04C       // Module Stop Status Register 4
 #define RCAR_GEN2_CPG_MSTPSR5           0x03C       // Module Stop Status Register 5
 #define RCAR_GEN2_CPG_MSTPSR7           0x1C4       // Module Stop Status Register 7
 #define RCAR_GEN2_CPG_MSTPSR8           0x9A0       // Module Stop Status Register 8
 #define RCAR_GEN2_CPG_MSTPSR9           0x9A4       // Module Stop Status Register 9
 #define RCAR_GEN2_CPG_MSTPSR10          0x9A8       // Module Stop Status Register 10
 #define RCAR_GEN2_CPG_MSTPSR11          0x9AC       // Module Stop Status Register 11

 #define RCAR_GEN2_CPG_RMSTPCR0          0x110       // Realtime Module Stop Control Register 0
 #define RCAR_GEN2_CPG_RMSTPCR1          0x114       // Realtime Module Stop Control Register 1
 #define RCAR_GEN2_CPG_RMSTPCR2          0x118       // Realtime Module Stop Control Register 2
 #define RCAR_GEN2_CPG_RMSTPCR3          0x11C       // Realtime Module Stop Control Register 3
 #define RCAR_GEN2_CPG_RMSTPCR4          0x120       // Realtime Module Stop Control Register 4
 #define RCAR_GEN2_CPG_RMSTPCR5          0x124       // Realtime Module Stop Control Register 5
 #define RCAR_GEN2_CPG_RMSTPCR7          0x12C       // Realtime Module Stop Control Register 7
 #define RCAR_GEN2_CPG_RMSTPCR8          0x980       // Realtime Module Stop Control Register 8
 #define RCAR_GEN2_CPG_RMSTPCR9          0x984       // Realtime Module Stop Control Register 9
 #define RCAR_GEN2_CPG_RMSTPCR10         0x988       // Realtime Module Stop Control Register 10
 #define RCAR_GEN2_CPG_RMSTPCR11         0x98C       // Realtime Module Stop Control Register 11

 #define RCAR_GEN2_CPG_SMSTPCR0          0x130       // System Module Stop Control Register 0
 #define RCAR_GEN2_CPG_SMSTPCR1          0x134       // System Module Stop Control Register 1
 #define RCAR_GEN2_CPG_SMSTPCR2          0x138       // System Module Stop Control Register 2
 #define RCAR_GEN2_CPG_SMSTPCR3          0x13C       // System Module Stop Control Register 3
 #define RCAR_GEN2_CPG_SMSTPCR4          0x140       // System Module Stop Control Register 4
 #define RCAR_GEN2_CPG_SMSTPCR5          0x144       // System Module Stop Control Register 5
 #define RCAR_GEN2_CPG_SMSTPCR7          0x14C       // System Module Stop Control Register 7
 #define RCAR_GEN2_CPG_SMSTPCR8          0x990       // System Module Stop Control Register 8
 #define RCAR_GEN2_CPG_SMSTPCR9          0x994       // System Module Stop Control Register 9
 #define RCAR_GEN2_CPG_SMSTPCR10         0x998       // System Module Stop Control Register 10
 #define RCAR_GEN2_CPG_SMSTPCR11         0x99C       // System Module Stop Control Register 11

 #define RCAR_GEN2_CPG_SRCR0             0x0A0       // Software Reset Register 0
 #define RCAR_GEN2_CPG_SRCR1             0x0A8       // Software Reset Register 1
 #define RCAR_GEN2_CPG_SRCR2             0x0B0       // Software Reset Register 2
 #define RCAR_GEN2_CPG_SRCR3             0x0B8       // Software Reset Register 3
 #define RCAR_GEN2_CPG_SRCR4             0x0BC       // Software Reset Register 4
 #define RCAR_GEN2_CPG_SRCR5             0x0C4       // Software Reset Register 5
 #define RCAR_GEN2_CPG_SRCR7             0x1CC       // Software Reset Register 7
 #define RCAR_GEN2_CPG_SRCR8             0x920       // Software Reset Register 8
 #define RCAR_GEN2_CPG_SRCR9             0x924       // Software Reset Register 9
 #define RCAR_GEN2_CPG_SRCR10            0x928       // Software Reset Register 10
 #define RCAR_GEN2_CPG_SRCR11            0x92C       // Software Reset Register 11

 #define RCAR_GEN2_CPG_SRSTCLR0          0x940       // Software Reset Clear Register 0
 #define RCAR_GEN2_CPG_SRSTCLR1          0x944       // Software Reset Clear Register 1
 #define RCAR_GEN2_CPG_SRSTCLR2          0x948       // Software Reset Clear Register 2
 #define RCAR_GEN2_CPG_SRSTCLR3          0x94C       // Software Reset Clear Register 3
 #define RCAR_GEN2_CPG_SRSTCLR4          0x950       // Software Reset Clear Register 4
 #define RCAR_GEN2_CPG_SRSTCLR5          0x954       // Software Reset Clear Register 5
 #define RCAR_GEN2_CPG_SRSTCLR7          0x95C       // Software Reset Clear Register 7
 #define RCAR_GEN2_CPG_SRSTCLR8          0x960       // Software Reset Clear Register 8
 #define RCAR_GEN2_CPG_SRSTCLR9          0x964       // Software Reset Clear Register 9
 #define RCAR_GEN2_CPG_SRSTCLR10         0x968       // Software Reset Clear Register 10
 #define RCAR_GEN2_CPG_SRSTCLR11         0x96C       // Software Reset Clear Register 11

/*
 * Reset registers
 */
#define RCAR_GEN2_RESET_BASE             0xE6160000
 #define RCAR_GEN2_WDTRSTCR              0x54        // Watchdog reset control register

/*
 * SYStem Controller
 */
#define RCAR_GEN2_SYSC_BASE              0xE6180000
 #define RCAR_GEN2_SYSC_SR               0x00        // Status Register
 #define RCAR_GEN2_SYSC_ISR              0x04        // Interrupt Status Register
 #define RCAR_GEN2_SYSC_ISCR             0x08        // Interrupt Status Clear Register
 #define RCAR_GEN2_SYSC_IER              0x0C        // Interrupt Enable Register
 #define RCAR_GEN2_SYSC_IMR              0x10        // Interrupt Mask Register
 #define RCAR_GEN2_SYSC_PWRSR2           0xC0        // Power Status Register 2
 #define RCAR_GEN2_SYSC_PWROFFCR2        0xC4        // Power Shutoff Control Register 2
 #define RCAR_GEN2_SYSC_PWROFFSR2        0xC8        // Power Shutoff Status Register 2
 #define RCAR_GEN2_SYSC_PWRONCR2         0xCC        // Power Resume Control Register 2
 #define RCAR_GEN2_SYSC_PWRONSR2         0xD0        // Power Resume Status Register 2
 #define RCAR_GEN2_SYSC_PWRER2           0xD4        // Power shutoff/resume Error Register 2


/*
 * Watchdog Timer
 */
#define RCAR_GEN2_RWDT_BASE              0xE6020000  // RCLK Watchdog Timer
#define RCAR_GEN2_SWDT_BASE              0xE6030000  // Secure Watchdog Timer
 #define RCAR_GEN2_WDT_CNT               0x00        // Count Register
 #define RCAR_GEN2_WDT_CSRA              0x04        // Control/Status Register A
 #define RCAR_GEN2_WDT_CSRB              0x08        // Control/Status Register B
#define RCAR_GEN2_WDT_SIZE               0x0C

/*
 * SCU
 */
#define RCAR_GEN2_SCU_BASE               0xEC500000
#define RCAR_GEN2_SCU_SIZE               0x1000

/*
 * SSIU
 */
#define RCAR_GEN2_SSIU_BASE              0xEC540000
 #define RCAR_GEN2_SSIU_MODE0            0x800
 #define RCAR_GEN2_SSIU_MODE1            0x804
 #define RCAR_GEN2_SSIU_MODE2            0x808
 #define RCAR_GEN2_SSIU_MODE3            0x80C
 #define RCAR_GEN2_SSIU_CONTROL          0x810


/*
 * SSI
 */
#define RCAR_GEN2_SSI_BASE               0xEC541000

/*
 * GIC
 */
#define RCAR_GEN2_GIC_BASE               0xF1000000
#define RCAR_GEN2_GIC_CPU_BASE           0xF1002000
#define RCAR_GEN2_GIC_DIST_BASE          0xF1001000

/*
 * INTC
 */
#define RCAR_GEN2_IRQC0_BASE             0xE61C0000
#define RCAR_GEN2_IRQC1_BASE             0xE61C0200
 #define RCAR_GEN2_INTREQ_STS0           0x000       /* R */
 #define RCAR_GEN2_INTEN_STS0            0x004       /* R/WC1 */
 #define RCAR_GEN2_INTEN_SET0            0x008       /* W */
 #define RCAR_GEN2_INTREQ_STS1           0x010       /* R */
 #define RCAR_GEN2_INTEN_STS1            0x014       /* R/WC1 */
 #define RCAR_GEN2_INTEN_SET1            0x018       /* W */
 #define RCAR_GEN2_DETECT_STATUS         0x100       /* R/WC1 */
 #define RCAR_GEN2_CONFIG_00             0x180       /* R/W */


/*
 * SDH interface
 */
#define RCAR_GEN2_SDHI0_BASE             0xEE100000  // SDHI0 Base
#define RCAR_GEN2_SDHI1_BASE             0xEE120000  // SDHI1 Base
#define RCAR_GEN2_SDHI2_BASE             0xEE140000  // SDHI2 Base
#define RCAR_GEN2_SDHI3_BASE             0xEE160000  // SDHI3 Base
#define RCAR_GEN2_SDHI_SIZE              0x2000

/*
 * MMCIF
 */
#define RCAR_GEN2_MMCIF0_BASE            0xEE200000
#define RCAR_GEN2_MMCIF1_BASE            0xEE220000
#define RCAR_GEN2_MMCIF_SIZE             0x1000

/*
 * QSPI
 */
#define RCAR_GEN2_QSPI0_BASE             0xE6B10000
#define RCAR_GEN2_QSPI1_BASE             0xEE200000  //for W2H/E2X
#define RCAR_GEN2_QSPI_SIZE              0x1000

/*
 * SYSDMAC
 */
#define RCAR_GEN2_SYSDMAC0_BASE          0xE6700000
#define RCAR_GEN2_SYSDMAC1_BASE          0xE6720000
#define RCAR_GEN2_SYSDMAC_SIZE           0x3000     // BASE+REGS to DESC

 /* SYSDMAC global registers, offset from BASE */
 #define RCAR_GEN2_SYSDMAC_DMAISTA       0x0020      // DMA interrupt status register
 #define RCAR_GEN2_SYSDMAC_DMASEC        0x0030      // DMA secure control register
 #define RCAR_GEN2_SYSDMAC_DMAOR         0x0060      // DMA operation register
 #define RCAR_GEN2_SYSDMAC_DMACHCLR      0x0080      // DMA channel clear register
 #define RCAR_GEN2_SYSDMAC_DMADPSEC      0x00A0      // DPRAM secure control register
 #define RCAR_GEN2_SYSDMAC_REGS          0x8000

/* SYSDMAC register, offset from BASE + REGS */
 #define RCAR_GEN2_SYSDMAC_REGSIZE       0x80
 #define RCAR_GEN2_SYSDMAC_DESC          (0xA000 - RCAR_GEN2_SYSDMAC_REGS)

/*
 * AUDIODMAC
 */
#define RCAR_GEN2_AUDIODMAC0_BASE        0xEC700000
#define RCAR_GEN2_AUDIODMAC1_BASE        0xEC720000

/*
 * AUDIODMACPP
 */
#define RCAR_GEN2_AUDIODMACPP_BASE       0xEC740000
#define RCAR_GEN2_AUDIODMACPP_SIZE       0x1000

/*
 * I2C
 */
#define RCAR_GEN2_I2C0_BASE              0xE6508000
#define RCAR_GEN2_I2C1_BASE              0xE6518000
#define RCAR_GEN2_I2C2_BASE              0xE6530000
#define RCAR_GEN2_I2C3_BASE              0xE6540000
#define RCAR_GEN2_I2C4_BASE              0xE6520000
#define RCAR_GEN2_I2C5_BASE              0xE6528000

#define RCAR_GEN2_I2C_SIZE               0x100

/*
 * IIC
 */
#define RCAR_GEN2_IIC0_BASE              0xE6500000
#define RCAR_GEN2_IIC1_BASE              0xE6510000
#define RCAR_GEN2_IIC2_BASE              0xE6520000
#define RCAR_GEN2_IIC3_BASE              0xE60B0000

#define RCAR_GEN2_IIC_SIZE               0x100

/*
 * Clock Synchronized Serial Interface with FIFO
 */
#define RCAR_GEN2_MSIOF0_BASE            0xE6E20000
#define RCAR_GEN2_MSIOF1_BASE            0xE6E10000
#define RCAR_GEN2_MSIOF2_BASE            0xE6E00000
#define RCAR_GEN2_MSIOF3_BASE            0xE6C90000

#define RCAR_GEN2_MSIOF_SIZE             0x100

 /* MSIOF registers, offset from base */
 #define RCAR_GEN2_MSIOF_TMDR1           0x00
 #define RCAR_GEN2_MSIOF_TMDR2           0x04
 #define RCAR_GEN2_MSIOF_TMDR3           0x08
 #define RCAR_GEN2_MSIOF_RMDR1           0x10
 #define RCAR_GEN2_MSIOF_RMDR2           0x14
 #define RCAR_GEN2_MSIOF_RMDR3           0x18
 #define RCAR_GEN2_MSIOF_TSCR            0x20
 #define RCAR_GEN2_MSIOF_RSCR            0x22
 #define RCAR_GEN2_MSIOF_CTR             0x28
 #define RCAR_GEN2_MSIOF_FCTR            0x30
 #define RCAR_GEN2_MSIOF_STR             0x40
 #define RCAR_GEN2_MSIOF_IER             0x44
 #define RCAR_GEN2_MSIOF_TFDR            0x50
 #define RCAR_GEN2_MSIOF_RFDR            0x60

/*
 * SCIF
 */
#define RCAR_GEN2_SCIF0_BASE_EREV        0xE6E60000
#define RCAR_GEN2_SCIF1_BASE_EREV        0xE6E68000
#define RCAR_GEN2_SCIF2_BASE_EREV        0xE6E56000

#define RCAR_GEN2_SCIF0_BASE             0xE6E60000
#define RCAR_GEN2_SCIF1_BASE             0xE6E68000
#define RCAR_GEN2_SCIF2_BASE             0xE6E58000
#define RCAR_GEN2_SCIF3_BASE             0xE6EA8000
#define RCAR_GEN2_SCIF4_BASE             0xE6EE0000
#define RCAR_GEN2_SCIF5_BASE             0xE6EE8000
#define RCAR_GEN2_SCIF_SIZE              0x100

#define RCAR_GEN2_SCIF_FIFO_SIZE         16

 /* SCIF registers, offset from SCIF_BASE */
 #define RCAR_GEN2_SCIF_SCSMR            0x00
 #define RCAR_GEN2_SCIF_SCBRR            0x04
 #define RCAR_GEN2_SCIF_SCSCR            0x08
 #define RCAR_GEN2_SCIF_SCFTDR           0x0C
 #define RCAR_GEN2_SCIF_SCFSR            0x10
 #define RCAR_GEN2_SCIF_SCFRDR           0x14
 #define RCAR_GEN2_SCIF_SCFCR            0x18
 #define RCAR_GEN2_SCIF_SCFDR            0x1C
 #define RCAR_GEN2_SCIF_SCSPTR           0x20
 #define RCAR_GEN2_SCIF_SCLSR            0x24
 #define RCAR_GEN2_SCIF_BRG_DL           0x30
 #define RCAR_GEN2_SCIF_BRG_CKS          0x34

 /* SCSMR bit definition */
 #define RCAR_GEN2_SCIF_SCSMR_CHR        (1 << 6)
 #define RCAR_GEN2_SCIF_SCSMR_PE         (1 << 5)
 #define RCAR_GEN2_SCIF_SCSMR_OE         (1 << 4)
 #define RCAR_GEN2_SCIF_SCSMR_STOP       (1 << 3)
 #define RCAR_GEN2_SCIF_SCSMR_CKS_0      (0 << 0)
 #define RCAR_GEN2_SCIF_SCSMR_CKS_4      (1 << 0)
 #define RCAR_GEN2_SCIF_SCSMR_CKS_16     (2 << 0)
 #define RCAR_GEN2_SCIF_SCSMR_CKS_64     (3 << 0)

  /* SCSCR bit definition */
  #define RCAR_GEN2_SCIF_SCSCR_TEIE      (1 << 11)
  #define RCAR_GEN2_SCIF_SCSCR_TIE       (1 << 7)
  #define RCAR_GEN2_SCIF_SCSCR_RIE       (1 << 6)
  #define RCAR_GEN2_SCIF_SCSCR_TE        (1 << 5)
  #define RCAR_GEN2_SCIF_SCSCR_RE        (1 << 4)
  #define RCAR_GEN2_SCIF_SCSCR_REIE      (1 << 3)
  #define RCAR_GEN2_SCIF_SCSCR_CKE_MASK  (3 << 0)

  /* SCFSR bit definition */
  #define RCAR_GEN2_SCIF_SCFSR_PERF(x)   (((x) >> 12) & 0xF)
  #define RCAR_GEN2_SCIF_SCFSR_FERF(x)   (((x) >> 8) & 0xF)
  #define RCAR_GEN2_SCIF_SCFSR_ER        (1 << 7)
  #define RCAR_GEN2_SCIF_SCFSR_TEND      (1 << 6)
  #define RCAR_GEN2_SCIF_SCFSR_TDFE      (1 << 5)
  #define RCAR_GEN2_SCIF_SCFSR_BRK       (1 << 4)
  #define RCAR_GEN2_SCIF_SCFSR_FER       (1 << 3)
  #define RCAR_GEN2_SCIF_SCFSR_PER       (1 << 2)
  #define RCAR_GEN2_SCIF_SCFSR_RDF       (1 << 1)
  #define RCAR_GEN2_SCIF_SCFSR_DR        (1 << 0)

  /* SCFCR bit definition */
  #define RCAR_GEN2_SCIF_SCFCR_RTRG_1    (0 << 6)
  #define RCAR_GEN2_SCIF_SCFCR_RTRG_4    (1 << 6)
  #define RCAR_GEN2_SCIF_SCFCR_RTRG_8    (2 << 6)
  #define RCAR_GEN2_SCIF_SCFCR_RTRG_14   (3 << 6)
  #define RCAR_GEN2_SCIF_SCFCR_TTRG_0    (3 << 4)
  #define RCAR_GEN2_SCIF_SCFCR_TTRG_2    (2 << 4)
  #define RCAR_GEN2_SCIF_SCFCR_TTRG_4    (1 << 4)
  #define RCAR_GEN2_SCIF_SCFCR_TTRG_8    (0 << 4)
  #define RCAR_GEN2_SCIF_SCFCR_MCE       (1 << 3)
  #define RCAR_GEN2_SCIF_SCFCR_TFRST     (1 << 2)
  #define RCAR_GEN2_SCIF_SCFCR_RFRST     (1 << 1)
  #define RCAR_GEN2_SCIF_SCFCR_LOOP      (1 << 0)

  /* SCFDR bit definition */
  #define RCAR_GEN2_SCIF_SCFDR_TX(x)     (((x) >> 8) & 0x1F)
  #define RCAR_GEN2_SCIF_SCFDR_RX(x)     ((x) & 0x1F)

  /* SCSPTR bit definition */
  #define RCAR_GEN2_SCIF_SCSPTR_RTSIO    (1 << 7)
  #define RCAR_GEN2_SCIF_SCSPTR_RTSDT    (1 << 6)
  #define RCAR_GEN2_SCIF_SCSPTR_CTSIO    (1 << 5)
  #define RCAR_GEN2_SCIF_SCSPTR_CTSDT    (1 << 4)
  #define RCAR_GEN2_SCIF_SCSPTR_SPB2IO   (1 << 1)
  #define RCAR_GEN2_SCIF_SCSPTR_SPB2DT   (1 << 0)

  /* SCLSR bit definition */
  #define RCAR_GEN2_SCIF_SCLSR_ORER      (1 << 0)

/*
 * HSCIF
 */
 #define RCAR_GEN2_HSCIF0_BASE           0xE62C0000
 #define RCAR_GEN2_HSCIF1_BASE           0xE62C8000
 #define RCAR_GEN2_HSCIF2_BASE           0xE62D0000

/*
 * USB
 */
#define RCAR_GEN2_USB3_BASE              0xEE000000  // USB3.0

#define RCAR_GEN2_USB2_BASE0_EREV        0xEE080000  // USB0, EHCI
#define RCAR_GEN2_USB2_BASE1_EREV        0xEE0A0000  // USB1, EHCI
#define RCAR_GEN2_USB2_BASE2_EREV        0xEE0C0000  // USB2, EHCI

#define RCAR_GEN2_USB2_BASE0             0xEE080000  // USB0, EHCI
#define RCAR_GEN2_USB2_BASE1             0xEE0C0000  // USB1, EHCI

#define RCAR_GEN2_PCI_CONF_EHCI          0x10100     // Offset to PCI configuration Register

 /* Register offset */
 #define OHCI_OFFSET                     0x00
 #define OHCI_SIZE                       0x100
 #define EHCI_OFFSET                     0x100   /* offset: 0100H */
 #define EHCI_SIZE                       0x100
 #define USB_AHB_OFFSET                  0x200
 #define USB_CORE_OFFSET                 0x300

 #define EHCI_USBCMD                     (EHCI_OFFSET + 0x0020)

 /* AHB bridge registers */
 #define INT_ENABLE                      0x00
 #define WAKEON_INTEN                    (1 << 4)
 #define UCOM_INTEN                      (1 << 3)
 #define USBH_INTBEN                     (1 << 2)
 #define USBH_INTAEN                     (1 << 1)
 #define AHB_INTEN                       (1 << 0)
 #define FULL_INTEN                      0x1f

 #define INT_ENABLE_INIT                 FULL_INTEN

 #define INT_STATUS                      0x04

 #define USBCTR                          0x0C
 #define USBCTR_DIRPD                    (1 << 2)
 #define USBCTR_PLL_RST                  (1 << 1)

 /* Core defined registers */
 #define SPD_RSM_TIMSET                  0x0C
 #define OC_TIMSET                       0x10

 /* USBCTR */
 #define DIRPD                           (1 << 2)
 #define PLL_RST                         (1 << 1)

 /* AHB_BUS_CTR */       /* hw default is .. */
 #define PROT_TYPE_CACHEABLE             (1 << 15) /* 0: non-cacheable trns */
 #define PROT_TYPE_BUFFERABLE            (1 << 14) /* 0: non-bufferable trns */
 #define PROT_TYPE_PRIVILEGED            (1 << 13) /* 0: user access */
 #define PROT_TYPE_DATA                  (1 << 12) /* 0: opcode */
 #define PROT_MODE                       (1 << 8)
 #define ALIGN_ADDRESS_1K                (0 << 4)  /* h/w default */
 #define ALIGN_ADDRESS_64BYTE            (1 << 4)
 #define ALIGN_ADDRESS_32BYTE            (2 << 4)
 #define ALIGN_ADDRESS_16BYTE            (3 << 4)
 #define MAX_BURST_LEN_INCR16            (0 << 0)  /* h/w default */
 #define MAX_BURST_LEN_INCR8             (1 << 0)
 #define MAX_BURST_LEN_INCR4             (2 << 0)
 #define MAX_BURST_LEN_SINGLE            (3 << 0)

 /* Init AHB master and slave functions of the host logic */
 #define AHB_BUS_CTR_INIT                0

 #define USBSEL_MASK                     0x00000030
 #define USBSEL_EHCI_OHCI                0x00000010
 #define USBSEL_HSUSB                    0x00000020

 #define USBHS_UGCTRL_PLLRESET           (1 << 0)
 #define OC_TIMSET_INIT                  0x000209ab
 #define SPD_RSM_TIMSET_INIT             0x014e029b

/*
 * HSUSB
 */
#define RCAR_GEN2_HSUSB_BASE0            0xE6590000
#define RCAR_GEN2_HSUSB_BASE1            0xE6598000

 #define RCAR_GEN2_HSUSB_SYSCFG          0x0000
 #define RCAR_GEN2_HSUSB_INTENB0         0x0030
 #define RCAR_GEN2_HSUSB_UGCTRL          0x0180
 #define RCAR_GEN2_HSUSB_UGCTRL2         0x0184

/*
 * DDR3 Controller
 */
#define RCAR_GEN2_DBSC3_0_BASE           0xE6790000

 #define RCAR_GEN2_DBSC3_DBSTATE1        0x00C
 #define RCAR_GEN2_DBSC3_DBACEN          0x010
 #define RCAR_GEN2_DBSC3_DBRFEN          0x014
 #define RCAR_GEN2_DBSC3_DBCMD           0x018
 #define RCAR_GEN2_DBSC3_DBWAIT          0x01C
 #define RCAR_GEN2_DBSC3_DBKIND          0x020
 #define RCAR_GEN2_DBSC3_DBCONF0         0x024
 #define RCAR_GEN2_DBSC3_DBPHYTYPE       0x030
 #define RCAR_GEN2_DBSC3_DBTR0           0x040
 #define RCAR_GEN2_DBSC3_DBTR1           0x044
 #define RCAR_GEN2_DBSC3_DBTR2           0x048
 #define RCAR_GEN2_DBSC3_DBTR3           0x050
 #define RCAR_GEN2_DBSC3_DBTR4           0x054
 #define RCAR_GEN2_DBSC3_DBTR5           0x058
 #define RCAR_GEN2_DBSC3_DBTR6           0x05C
 #define RCAR_GEN2_DBSC3_DBTR7           0x060
 #define RCAR_GEN2_DBSC3_DBTR8           0x064
 #define RCAR_GEN2_DBSC3_DBTR9           0x068
 #define RCAR_GEN2_DBSC3_DBTR10          0x06C
 #define RCAR_GEN2_DBSC3_DBTR11          0x070
 #define RCAR_GEN2_DBSC3_DBTR12          0x074
 #define RCAR_GEN2_DBSC3_DBTR13          0x078
 #define RCAR_GEN2_DBSC3_DBTR14          0x07C
 #define RCAR_GEN2_DBSC3_DBTR15          0x080
 #define RCAR_GEN2_DBSC3_DBTR16          0x084
 #define RCAR_GEN2_DBSC3_DBTR17          0x088
 #define RCAR_GEN2_DBSC3_DBTR18          0x08C
 #define RCAR_GEN2_DBSC3_DBTR19          0x090
 #define RCAR_GEN2_DBSC3_DBBL            0x0B0
 #define RCAR_GEN2_DBSC3_DBADJ0          0x0C0
 #define RCAR_GEN2_DBSC3_DBADJ2          0x0C8
 #define RCAR_GEN2_DBSC3_DBRFCNF0        0x0E0
 #define RCAR_GEN2_DBSC3_DBRFCNF1        0x0E4
 #define RCAR_GEN2_DBSC3_DBRFCNF2        0x0E8
 #define RCAR_GEN2_DBSC3_DBCALCNF        0x0F4
 #define RCAR_GEN2_DBSC3_DBCALTR         0x0F8
 #define RCAR_GEN2_DBSC3_DBRNK0          0x100
 #define RCAR_GEN2_DBSC3_DBPDNCNF        0x180
 #define RCAR_GEN2_DBSC3_DBPDCNT0        0x200
 #define RCAR_GEN2_DBSC3_DBPDCNT1        0x204
 #define RCAR_GEN2_DBSC3_DBPDCNT2        0x208
 #define RCAR_GEN2_DBSC3_DBPDCNT3        0x20C
 #define RCAR_GEN2_DBSC3_DBDFISTAT       0x240
 #define RCAR_GEN2_DBSC3_DBDFICNT        0x244
 #define RCAR_GEN2_DBSC3_DBPDLCK         0x280       // PHY unit lock register
 #define RCAR_GEN2_DBSC3_DBPDRGA         0x290
 #define RCAR_GEN2_DBSC3_DBPDRGD         0x2A0
 #define RCAR_GEN2_DBSC3_DBBS0CNT1       0x304
 #define RCAR_GEN2_DBSC3_DBWT0CNF0       0x380
 #define RCAR_GEN2_DBSC3_DBWT0CNF4       0x390
 #define RCAR_GEN2_DBSC3_DBSCHECNT0      0x500
 #define RCAR_GEN2_DBSC3_DBRSTLCK        0x4000
 #define RCAR_GEN2_DBSC3_DBRST           0x4008


/*
 * Audio Clock Generator
 */
#define RCAR_GEN2_ADG_BASE               0xEC5A0000

 #define RCAR_GEN2_ADG_BRRA              0x00
 #define RCAR_GEN2_ADG_BRRB              0x04
 #define RCAR_GEN2_ADG_SSICKR            0x08
 #define RCAR_GEN2_ADG_CKSEL0            0x0C
 #define RCAR_GEN2_ADG_CKSEL1            0x10
 #define RCAR_GEN2_ADG_CKSEL2            0x14
 #define RCAR_GEN2_ADG_DIV_EN            0x30
 #define RCAR_GEN2_ADG_SRCIN_TIMSEL0     0x34
 #define RCAR_GEN2_ADG_SRCIN_TIMSEL1     0x38
 #define RCAR_GEN2_ADG_SRCIN_TIMSEL2     0x3C
 #define RCAR_GEN2_ADG_SRCIN_TIMSEL3     0x40
 #define RCAR_GEN2_ADG_SRCIN_TIMSEL4     0x44
 #define RCAR_GEN2_ADG_SRCOUT_TIMSEL0    0x48
 #define RCAR_GEN2_ADG_SRCOUT_TIMSEL1    0x4C
 #define RCAR_GEN2_ADG_SRCOUT_TIMSEL2    0x50
 #define RCAR_GEN2_ADG_SRCOUT_TIMSEL3    0x54
 #define RCAR_GEN2_ADG_SRCOUT_TIMSEL4    0x58
 #define RCAR_GEN2_ADG_CMDOUT_TIMSEL     0x5C
 #define RCAR_GEN2_ADG_DTCP_TIMSEL       0x64

/*
 * Ethernet (FELIC)
 */
#define RCAR_GEN2_FELIC_REG_BASE         0xEE700000
#define RCAR_GEN2_FELIC_REG_SIZE         0x0400

/*
 * Video Capture
 */
#define RCAR_GEN2_VIN_BASE0              0xE6EF0000
#define RCAR_GEN2_VIN_BASE1              0xE6EF1000
#define RCAR_GEN2_VIN_BASE2              0xE6EF2000
#define RCAR_GEN2_VIN_BASE3              0xE6EF3000
#define RCAR_GEN2_VIN_SIZE               0x1000
/*
 * L2 Cache
 */
#define RCAR_GEN2_L2CPL310_BASE          0xF0100000

/*
 * LBSC
 */
#define RCAR_GEN2_LBSC_BASE              0xFEC00000

 #define RCAR_GEN2_LBSC_CS0CTRL          0x200
 #define RCAR_GEN2_LBSC_CS1CTRL          0x204
 #define RCAR_GEN2_LBSC_ECS0CTRL         0x208
 #define RCAR_GEN2_LBSC_ECS1CTRL         0x20C
 #define RCAR_GEN2_LBSC_CSWCR0           0x230
 #define RCAR_GEN2_LBSC_CSWCR1           0x234
 #define RCAR_GEN2_LBSC_ECSWCR0          0x238
 #define RCAR_GEN2_LBSC_ECSWCR1          0x23C
 #define RCAR_GEN2_LBSC_CSPWCR0          0x280
 #define RCAR_GEN2_LBSC_CSPWCR1          0x284
 #define RCAR_GEN2_LBSC_ECSPWCR0         0x288
 #define RCAR_GEN2_LBSC_ECSPWCR1         0x28C
 #define RCAR_GEN2_LBSC_EXWTSYNC         0x2A0
 #define RCAR_GEN2_LBSC_CS1GDST          0x2C0
 #define RCAR_GEN2_LBSC_ECS0GDST         0x2C4
 #define RCAR_GEN2_LBSC_ECS1GDST         0x2C8
 #define RCAR_GEN2_LBSC_ATACSCTRL        0x380

/***************************
 * Interrupt ID
 ************************* */
#define RCAR_GEN2_INTCSYS_IRQ0           (  0 + 32)
#define RCAR_GEN2_INTCSYS_IRQ1           (  1 + 32)
#define RCAR_GEN2_INTCSYS_IRQ2           (  2 + 32)
#define RCAR_GEN2_INTCSYS_IRQ3           (  3 + 32)
#define RCAR_GEN2_INTCSYS_GPIO0          (  4 + 32)
#define RCAR_GEN2_INTCSYS_GPIO1          (  5 + 32)
#define RCAR_GEN2_INTCSYS_GPIO2          (  6 + 32)
#define RCAR_GEN2_INTCSYS_GPIO3          (  7 + 32)
#define RCAR_GEN2_INTCSYS_GPIO4          (  8 + 32)
#define RCAR_GEN2_INTCSYS_GPIO5          (  9 + 32)
#define RCAR_GEN2_INTCSYS_IRQ4           ( 12 + 32)
#define RCAR_GEN2_INTCSYS_IRQ5           ( 13 + 32)
#define RCAR_GEN2_INTCSYS_IRQ6           ( 14 + 32)
#define RCAR_GEN2_INTCSYS_IRQ7           ( 15 + 32)
#define RCAR_GEN2_INTCSYS_IRQ8           ( 16 + 32)
#define RCAR_GEN2_INTCSYS_IRQ9           ( 17 + 32)
#define RCAR_GEN2_INTCSYS_I2C4           ( 19 + 32)
#define RCAR_GEN2_INTCSYS_I2C5           ( 20 + 32)
#define RCAR_GEN2_INTCSYS_HSCIF2         ( 21 + 32)
#define RCAR_GEN2_INTCSYS_SCIF2          ( 22 + 32)
#define RCAR_GEN2_INTCSYS_SCIF3          ( 23 + 32)
#define RCAR_GEN2_INTCSYS_SCIF4          ( 24 + 32)
#define RCAR_GEN2_INTCSYS_SCIF5          ( 25 + 32)
#define RCAR_GEN2_INTCSYS_SCIFA0         (144 + 32)
#define RCAR_GEN2_INTCSYS_SCIFA1         (145 + 32)
#define RCAR_GEN2_INTCSYS_SCIFB0         (148 + 32)
#define RCAR_GEN2_INTCSYS_SCIFB1         (149 + 32)
#define RCAR_GEN2_INTCSYS_SCIFB2         (150 + 32)
#define RCAR_GEN2_INTCSYS_SCIFB3         (151 + 32)
#define RCAR_GEN2_INTCSYS_SCIF0          (152 + 32)
#define RCAR_GEN2_INTCSYS_SCIF1          (153 + 32)
#define RCAR_GEN2_INTCSYS_HSCIF0         (154 + 32)
#define RCAR_GEN2_INTCSYS_HSCIF1         (155 + 32)
#define RCAR_GEN2_INTCSYS_MSIOF0         (156 + 32)
#define RCAR_GEN2_INTCSYS_MSIOF1         (157 + 32)
#define RCAR_GEN2_INTCSYS_MSIOF2         (158 + 32)
#define RCAR_GEN2_INTCSYS_MSIOF3         (159 + 32)
#define RCAR_GEN2_INTCSYS_FELIC	        (162 + 32)
#define RCAR_GEN2_INTCSYS_SDHI0          (165 + 32)
#define RCAR_GEN2_INTCSYS_SDHI1          (166 + 32)
#define RCAR_GEN2_INTCSYS_SDHI2          (167 + 32)
#define RCAR_GEN2_INTCSYS_SDHI3          (168 + 32)
#define RCAR_GEN2_INTCSYS_MMC0           (169 + 32)
#define RCAR_GEN2_INTCSYS_MMC1           (170 + 32)
#define RCAR_GEN2_INTCSYS_IIC3           (173 + 32)
#define RCAR_GEN2_INTCSYS_IIC0           (174 + 32)
#define RCAR_GEN2_INTCSYS_IIC1           (175 + 32)
#define RCAR_GEN2_INTCSYS_IIC2           (176 + 32)
#define RCAR_GEN2_INTCSYS_QSPI0          (184 + 32)
#define RCAR_GEN2_INTCSYS_VIN0           (188 + 32)
#define RCAR_GEN2_INTCSYS_VIN1           (189 + 32)
#define RCAR_GEN2_INTCSYS_VIN2           (190 + 32)
#define RCAR_GEN2_INTCSYS_VIN3           (191 + 32)
#define RCAR_GEN2_INTCSYS_QSPI1          (239 + 32)
#define RCAR_GEN2_INTCSYS_I2C2           (286 + 32)
#define RCAR_GEN2_INTCSYS_I2C0           (287 + 32)
#define RCAR_GEN2_INTCSYS_I2C1           (288 + 32)
#define RCAR_GEN2_INTCSYS_I2C3           (290 + 32)


/*
 * DMA request ID
 */
#define RCAR_GEN2_DREQ_SCIFA0_TXI        0x21
#define RCAR_GEN2_DREQ_SCIFA0_RXI        0x22
#define RCAR_GEN2_DREQ_SCIFA1_TXI        0x25
#define RCAR_GEN2_DREQ_SCIFA1_RXI        0x26
#define RCAR_GEN2_DREQ_SCIFA2_TXI        0x27
#define RCAR_GEN2_DREQ_SCIFA2_RXI        0x28
#define RCAR_GEN2_DREQ_SCIFB0_TXI        0x3D
#define RCAR_GEN2_DREQ_SCIFB0_RXI        0x3E
#define RCAR_GEN2_DREQ_SCIFB1_TXI        0x19
#define RCAR_GEN2_DREQ_SCIFB1_RXI        0x1A
#define RCAR_GEN2_DREQ_SCIFB2_TXI        0x1D
#define RCAR_GEN2_DREQ_SCIFB2_RXI        0x1E
#define RCAR_GEN2_DREQ_HSCIF0_TXI        0x39
#define RCAR_GEN2_DREQ_HSCIF0_RXI        0x3A
#define RCAR_GEN2_DREQ_HSCIF1_TXI        0x4D
#define RCAR_GEN2_DREQ_HSCIF1_RXI        0x4E
#define RCAR_GEN2_DREQ_HSCIF2_TXI        0x3B
#define RCAR_GEN2_DREQ_HSCIF2_RXI        0x3C
#define RCAR_GEN2_DREQ_SCIF0_TXI         0x29
#define RCAR_GEN2_DREQ_SCIF0_RXI         0x2A
#define RCAR_GEN2_DREQ_SCIF1_TXI         0x2D
#define RCAR_GEN2_DREQ_SCIF1_RXI         0x2E
#define RCAR_GEN2_DREQ_SCIF2_TXI         0x2B
#define RCAR_GEN2_DREQ_SCIF2_RXI         0x2C
#define RCAR_GEN2_DREQ_SCIF3_TXI         0x2f
#define RCAR_GEN2_DREQ_SCIF3_RXI         0x30
#define RCAR_GEN2_DREQ_SCIF4_TXI         0xFB
#define RCAR_GEN2_DREQ_SCIF4_RXI         0xFC
#define RCAR_GEN2_DREQ_SCIF5_TXI         0xFD
#define RCAR_GEN2_DREQ_SCIF5_RXI         0xFE
#define RCAR_GEN2_DREQ_MSIOF0_TXI_EREV   0x81
#define RCAR_GEN2_DREQ_MSIOF0_RXI_EREV   0x82
#define RCAR_GEN2_DREQ_MSIOF1_TXI_EREV   0x85
#define RCAR_GEN2_DREQ_MSIOF1_RXI_EREV   0x86
#define RCAR_GEN2_DREQ_MSIOF0_TXI        0x51
#define RCAR_GEN2_DREQ_MSIOF0_RXI        0x52
#define RCAR_GEN2_DREQ_MSIOF1_TXI        0x55
#define RCAR_GEN2_DREQ_MSIOF1_RXI        0x56
#define RCAR_GEN2_DREQ_MSIOF2_TXI        0x41
#define RCAR_GEN2_DREQ_MSIOF2_RXI        0x42
#define RCAR_GEN2_DREQ_MSIOF3_TXI        0x45
#define RCAR_GEN2_DREQ_MSIOF3_RXI        0x46
#define RCAR_GEN2_DREQ_QSPI0_TXI         0x17
#define RCAR_GEN2_DREQ_QSPI0_RXI         0x18
#define RCAR_GEN2_DREQ_QSPI1_TXI         0xD1
#define RCAR_GEN2_DREQ_QSPI1_RXI         0xD2
#define RCAR_GEN2_DREQ_SIM_TXI           0xA1
#define RCAR_GEN2_DREQ_SIM_RXI           0xA2
#define RCAR_GEN2_DREQ_I2C0_TXI          0x61
#define RCAR_GEN2_DREQ_I2C0_RXI          0x62
#define RCAR_GEN2_DREQ_I2C1_TXI          0x65
#define RCAR_GEN2_DREQ_I2C1_RXI          0x66
#define RCAR_GEN2_DREQ_I2C2_TXI          0x69
#define RCAR_GEN2_DREQ_I2C2_RXI          0x6A
#define RCAR_GEN2_DREQ_IIC_DVFS_TXI      0x77
#define RCAR_GEN2_DREQ_IIC_DVFS_RXI      0x78
#define RCAR_GEN2_DREQ_SDHI0_TXI         0xCD
#define RCAR_GEN2_DREQ_SDHI0_RXI         0xCE
#define RCAR_GEN2_DREQ_SDHI1_TXI         0xC9
#define RCAR_GEN2_DREQ_SDHI1_RXI         0xCA
#define RCAR_GEN2_DREQ_SDHI2_TXI         0xC1
#define RCAR_GEN2_DREQ_SDHI2_RXI         0xC2
#define RCAR_GEN2_DREQ_SDHI2_C2_TXI      0xC5
#define RCAR_GEN2_DREQ_SDHI2_C2_RXI      0xC6
#define RCAR_GEN2_DREQ_SDHI3_TXI         0xD3
#define RCAR_GEN2_DREQ_SDHI3_RXI         0xD4
#define RCAR_GEN2_DREQ_SDHI3_C2_TXI      0xDF
#define RCAR_GEN2_DREQ_SDHI3_C2_RXI      0xDE
#define RCAR_GEN2_DREQ_TPU0_TXI          0xF1
#define RCAR_GEN2_DREQ_TSIF0_RXI         0xEA
#define RCAR_GEN2_DREQ_TSIF1_RXI         0xF0
#define RCAR_GEN2_DREQ_AXISTATR_RXI      0xA6
#define RCAR_GEN2_DREQ_AXISTATR0_RXI     0xAC
#define RCAR_GEN2_DREQ_AXISTATR1_RXI     0xAA
#define RCAR_GEN2_DREQ_AXISTAT2_RXI      0xA8
#define RCAR_GEN2_DREQ_AXISTAT3C_RXI     0xA4
#define RCAR_GEN2_DREQ_MMCIF0_TXI        0xD1
#define RCAR_GEN2_DREQ_MMCIF0_RXI        0xD2
#define RCAR_GEN2_DREQ_MMCIF1_TXI        0xE1
#define RCAR_GEN2_DREQ_MMCIF1_RXI        0xE2
#define RCAR_GEN2_DREQ_AXSTM_RXI         0xAE

#endif /* __ARM_RCAR_GEN2_H_INCLUDED  */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/arm/r-car-gen2.h $ $Rev: 837545 $")
#endif
