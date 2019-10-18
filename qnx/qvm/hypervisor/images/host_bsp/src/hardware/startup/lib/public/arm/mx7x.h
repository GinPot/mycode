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

#ifndef __ARM_MX7X_H_INCLUDED
#define __ARM_MX7X_H_INCLUDED

/*
 * System Memory Map
 */

/*
 * BOOT ROM (ROMCP)
 */
#define IMX7_BOOTROM_BASE                       0x00000000
#define IMX7_BOOTROM_SILICON_REV                0x48

#define IMX7_WEIM_CS0_BASE                      0x08000000  /* NOR or SRAM */

/*
 * Multi Mode DDR Controller (MMDC)
 */
#define IMX7_MMDC_P0_BASE                       0x021B0000   /* port 0 */
#define IMX7_MMDC_P1_BASE                       0x021B4000   /* port 1 */
#define IMX7_MMDC_SIZE                          0x4000

/*
 * SNVS Secure Non Voltatile Storage
 */
#define IMX7_SNVS_BASE                          0x30370000
#define IMX7_SNVS_SIZE                          0x10000
#define IMX7_SNVS_SECURE_IRQ                    51        /* Non TrustZone    */
#define IMX7_SNVS_NON_SECURE_IRQ                52        /* TrustZone        */

#define IMX7_SNVS_HPSR                          0x14    /* HP Status Register */
#define IMX7_SNVS_LPCR                          0x38    /* LP Control Register */
#define IMX7_SNVS_LPSR                          0x4C    /* LP Status Register */
#define IMX7_SNVS_LPSRTCMR                      0x50    /* LP Secure Real Time Counter MSB Register */
#define IMX7_SNVS_LPSRTCLR                      0x54    /* LP Secure Real Time Counter LSB Register */
#define IMX7_SNVS_LPTAR                         0x58    /* LP Time Alarm Register */
#define IMX7_SNVS_LPSMCMR                       0x5C    /* LP Secure Monotonic Counter MSB Register */
#define IMX7_SNVS_LPSMCLR                       0x60    /* LP Secure Monotonic Counter LSB Register */
#define IMX7_SNVS_LPPGDR                        0x64    /* LP Power Glitch Detector Register */
#define IMX7_SNVS_LPGPR                         0x68    /* LP General Purpose Register */


/* iMX7 Macro for pads masking */
#define PAD_MASK(idx)                          (1 << idx)


#define Q0_PCIE_DIS_B_74LV595_MASK             (1 << 0)
#define Q1_PCIE_RST_B_74LV595_MASK             (1 << 1)
#define Q2_HDMI_RST_B_74LV595_MASK             (1 << 2)
#define Q3_IPOD_RST_B_MKBUS_RST_74LV595_MASK   (1 << 3)
#define Q4_SENSOR_RST_B_74LV595_MASK           (1 << 4)
#define Q5_ENET_RST_B_74LV595_MASK             (1 << 5)
#define Q6_PERI_3V3_EN_74LV595_MASK            (1 << 6)
#define Q7_LCD_PWR_EN_74LV595_MASK             (1 << 7)

/*
 * I/O MUX Controller
 */
#define IMX7_IOMUXC_BASE                        0x30330000
#define IMX7_IOMUXC_SIZE                        0x4000
#define IMX7_IOMUXC_GPR_BASE                    0x30340000

/*
 * IOMUX Registers, offset from base address
 */
#define IMX7_IOMUX_GPR0                         0x0000
#define IMX7_IOMUX_GPR1                         0x0004
#define IMX7_IOMUX_GPR2                         0x0008
#define IMX7_IOMUX_GPR3                         0x000C
#define IMX7_IOMUX_GPR4                         0x0010
#define IMX7_IOMUX_GPR5                         0x0014
#define IMX7_IOMUX_GPR6                         0x0018
#define IMX7_IOMUX_GPR7                         0x001C
#define IMX7_IOMUX_GPR8                         0x0020
#define IMX7_IOMUX_GPR9                         0x0024
#define IMX7_IOMUX_GPR10                        0x0028
#define IMX7_IOMUX_GPR11                        0x002C
#define IMX7_IOMUX_GPR12                        0x0030
#define IMX7_IOMUX_GPR13                        0x0034
#define IMX7_IOMUXC_OBSERVE_MUX_0               0x0038
#define IMX7_IOMUXC_OBSERVE_MUX_1               0x003C
#define IMX7_IOMUXC_OBSERVE_MUX_2               0x0040
#define IMX7_IOMUXC_OBSERVE_MUX_3               0x0044
#define IMX7_IOMUXC_OBSERVE_MUX_4               0x0048

#define IMX7_IOMUX_SWMUX                        0x0014
#define IMX7_IOMUX_SWPAD                        0x026C
#define IMX7_IOMUX_SWINPUT                      0x04DC

/*
 * I/O MUX_LPSR Controller
 */
#define IMX7_IOMUXC_LPSR_BASE                   0x302C0000
#define IMX7_IOMUXC_LPSR_SIZE                   0x4000
#define IMX7_IOMUXC_LPSR_GPR_BASE               0x30270000

#define IMX7_IOMUX_LPSR_SWMUX                   0x0000
#define IMX7_IOMUX_LPSR_SWPAD                   0x0030

/*
 * SDRAM
 */
#define IMX7_SDRAM_64_BIT_BASE                  0x80000000

/*
 * GPC
 */

#define IMX7_GPC_CNTR                           0x00
#define IMX7_GPC_PGR                            0x04
#define IMX7_GPC_IMR1                           0x08
#define IMX7_GPC_IMR2                           0x0C
#define IMX7_GPC_IMR3                           0x10
#define IMX7_GPC_IMR4                           0x14
#define IMX7_GPC_ISR1                           0x18
#define IMX7_GPC_ISR2                           0x1C
#define IMX7_GPC_ISR3                           0x20
#define IMX7_GPC_ISR4                           0x24
#define IMX7_GPC_GPU_CTRL                       0x260

/*
 * On-Chip eFuse OTP Controller
 */
#define IMX7_OCOTP_BASE                         0x30350000


#define IMX7_DDRC_MSTR_BASE                     0x307a0000
#define IMX7_DDR_PHY_BASE                       0x30790000

/*
 * Fast Ethernet Controller
 */

/* iMX7D ENET's base addresses */
#define IMX7_ENET1_BASE                         0x30BE0000
#define IMX7_ENET2_BASE                         0x30BF0000

/* iMX7D ENET's memory size */
#define IMX7_ENET1_MEM_SIZE                     0x4000
#define IMX7_ENET2_MEM_SIZE                     0x4000

/* ENET1 IRQ numbering imx7 RM, page 1227 */
#define IMX7_ENET1_MAC0_IRQ                     (120 + 32)

/* ENET2 IRQ numbering imx7 RM, page 1226 */
#define IMX7_ENET2_MAC0_IRQ                     (102 + 32)

/*
 * GPIO
 */

/* iMX7 GPIO register's base addresses */
#define IMX7_GPIO1_BASE                          0x30200000
#define IMX7_GPIO2_BASE                          0x30210000
#define IMX7_GPIO3_BASE                          0x30220000
#define IMX7_GPIO4_BASE                          0x30230000
#define IMX7_GPIO5_BASE                          0x30240000
#define IMX7_GPIO6_BASE                          0x30250000
#define IMX7_GPIO7_BASE                          0x30260000
#define IMX7_GPIO_SIZE                           0x4000

/* GPIO Registers, offset from base address */
#define IMX7_GPIO_DR                             0x00
#define IMX7_GPIO_GDIR                           0x04
#define IMX7_GPIO_PSR                            0x08
#define IMX7_GPIO_ICR1                           0x0C
#define IMX7_GPIO_ICR2                           0x10
#define IMX7_GPIO_IMR                            0x14
#define IMX7_GPIO_ISR                            0x18
#define IMX7_GPIO_EDR                            0x1C

/*
 * I2C
 */

#define IMX7_I2C1_BASE                          0x30A20000
#define IMX7_I2C2_BASE                          0x30A30000
#define IMX7_I2C3_BASE                          0x30A40000
#define IMX7_I2C4_BASE                          0x30A50000

#define IMX7_I2C1_IRQ                           67
#define IMX7_I2C2_IRQ                           68
#define IMX7_I2C3_IRQ                           69
#define IMX7_I2C4_IRQ                           70

#define IMX7_I2C_SIZE                           0x10000

#define IMX7_I2C_ADRREG                         0x00        /**< Address Register */
#define IMX7_I2C_FRQREG                         0x04        /**< Frequency Divider Register */
#define IMX7_I2C_CTRREG                         0x08        /**< Control Register */
#define IMX7_I2C_STSREG                         0x0C        /**< Status Register */
#define IMX7_I2C_DATREG                         0x10        /**< Data I/O Register */

/** Control Register macros */
#define IMX7_I2C_CTRREG_IEN_MASK                (1 << 7)
#define IMX7_I2C_CTRREG_IIEN_MASK               (1 << 6)
#define IMX7_I2C_CTRREG_MSTA_MASK               (1 << 5)
#define IMX7_I2C_CTRREG_MTX_MASK                (1 << 4)
#define IMX7_I2C_CTRREG_TXAK_MASK               (1 << 3)
#define IMX7_I2C_CTRREG_RSTA_MASK               (1 << 2)
/** Status Register macros */
#define IMX7_I2C_STSREG_ICF_MASK                (1 << 7)
#define IMX7_I2C_STSREG_IAAS_MASK               (1 << 6)
#define IMX7_I2C_STSREG_IBB_MASK                (1 << 5)
#define IMX7_I2C_STSREG_IAL_MASK                (1 << 4)
#define IMX7_I2C_STSREG_SRW_MASK                (1 << 2)
#define IMX7_I2C_STSREG_IIF_MASK                (1 << 1)
#define IMX7_I2C_STSREG_RXAK_MASK               (1 << 0)

/*
 * QSPI
 */

#ifndef IMX7_QSPI_BASE
#define IMX7_QSPI_BASE          0x30BB0000
#endif

#ifndef IMX7_QSPI_SIZE
#define IMX7_QSPI_SIZE          0x5000
#endif

#ifndef IMX7_QSPI_IRQ
#define IMX7_QSPI_IRQ                    139
#endif

/* Registers offset */
#define IMX7_QSPI_MCR_OFFSET             0x00   /**< Module Configuration Register */
#define IMX7_QSPI_IPCR_OFFSET            0x08   /**< IP Configuration Register */
#define IMX7_QSPI_FLSHCR_OFFSET          0x0c   /**< Flash Configuration Register */
#define IMX7_QSPI_BUF0CR_OFFSET          0x10   /**< Buffer0 Configuration Register */
#define IMX7_QSPI_BUF1CR_OFFSET          0x14   /**< Buffer1 Configuration Register */
#define IMX7_QSPI_BUF2CR_OFFSET          0x18   /**< Buffer2 Configuration Register */
#define IMX7_QSPI_BUF3CR_OFFSET          0x1C   /**< Buffer3 Configuration Register */
#define IMX7_QSPI_BFGENCR_OFFSET         0x20   /**< Buffer Generic Configuration Register */
#define IMX7_QSPI_BUF0IND_OFFSET         0x30   /**< Buffer0 Top Index Register */
#define IMX7_QSPI_BUF1IND_OFFSET         0x34   /**< Buffer1 Top Index Register */
#define IMX7_QSPI_BUF2IND_OFFSET         0x38   /**< Buffer2 Top Index Register */
#define IMX7_QSPI_SFAR_OFFSET            0x100  /**< Serial Flash Address Register */
#define IMX7_QSPI_SMPR_OFFSET            0x108  /**< Sampling Register */
#define IMX7_QSPI_RBSR_OFFSET            0x10C  /**< RX Buffer Status Register */
#define IMX7_QSPI_RBCT_OFFSET            0x110  /**< RX Buffer Control Register */
#define IMX7_QSPI_TBSR_OFFSET            0x150  /**< TX Buffer Status Register */
#define IMX7_QSPI_TBDR_OFFSET            0x154  /**< TX Buffer Data Register */
#define IMX7_QSPI_SR_OFFSET              0x15C  /**< Status Register */
#define IMX7_QSPI_FR_OFFSET              0x160  /**< Flag Register */
#define IMX7_QSPI_RSER_OFFSET            0x164  /**< Interrupt and DMA Request Select and Enable Register */
#define IMX7_QSPI_SPNDST_OFFSET          0x168  /**< Sequence Suspend Status Register */
#define IMX7_QSPI_SPTRCLR_OFFSET         0x16C  /**< Sequence Pointer Clear Register */
#define IMX7_QSPI_SFA1AD_OFFSET          0x180  /**< Serial Flash A1Top Address */
#define IMX7_QSPI_SFA2AD_OFFSET          0x184  /**< Serial Flash A2Top Address */
#define IMX7_QSPI_SFB1AD_OFFSET          0x188  /**< Serial Flash B1Top Address */
#define IMX7_QSPI_SFB2AD_OFFSET          0x18C  /**< Serial Flash B2Top Address */
#define IMX7_QSPI_RBDR0_OFFSET           0x200  /**< RX Buffer Data Register 0 */
#define IMX7_QSPI_RBDRn_OFFSET(index)    (IMX7_QSPI_RBDR0_OFFSET + (index * 4)) /**< RX Buffer Data Register n */
#define IMX7_QSPI_LUTKEY_OFFSET          0x300  /**< LUT Key Register */
#define IMX7_QSPI_LCKCR_OFFSET           0x304  /**< LUT Lock Configuration Register */
#define IMX7_QSPI_LUT0_OFFSET            0x310  /**< Look-up Table register 0 */
#define IMX7_QSPI_LUT1_OFFSET            0x314  /**< Look-up Table register 1 */
#define IMX7_QSPI_LUT2_OFFSET            0x318  /**< Look-up Table register 2 */
#define IMX7_QSPI_LUT3_OFFSET            0x31C  /**< Look-up Table register 3 */
#define IMX7_QSPI_LUTn_OFFSET(index)     (IMX7_QSPI_LUT0_OFFSET + (index * 4)) /**< Look-up Table register n */

/* Registers mask */

/* Module Configuration Register */
#define IMX7_QSPI_MCR_MDIS_MASK          0x4000 /**< Module Disable */
#define IMX7_QSPI_MCR_CLR_TXF_MASK       0x800  /**< Clear TX FIFO/Buffer */
#define IMX7_QSPI_MCR_CLR_RXF_MASK       0x400  /**< Clear RX FIFO/Buffer */
#define IMX7_QSPI_MCR_DDR_EN             0x80   /**< SDR and DDR instructions are supported */
#define IMX7_QSPI_MCR_SWRSTHD_MASK       0x2    /**< Software reset for QSPI AHB domain */
#define IMX7_QSPI_MCR_SWRSTSD_MASK       0x1    /**< Software reset for Serial Flash domain */

/* IP Configuration Register */
#define IMX7_QSPI_IPCR_SEQID_MASK        0x7800000 /**< Points to a sequence in the Look-up-table. A write triggers a transaction. */
#define IMX7_QSPI_IPCR_SEQID_SHIFT       24

/* Flash Configuration Register */
#define IMX7_QSPI_TCSS_MASK              0xF    /**< Serial flash CS setup time */
#define IMX7_QSPI_TCSH_MASK              0xF00  /**< Serial flash CS hold time */

/* RX Buffer Status Register */
#define IMX7_QSPI_RBSR_RDBFL_MASK        0x3F00 /**< RX Buffer Fill Level (4 bytes) entries count */
#define IMX7_QSPI_RBSR_RDBFL_SHIFT       8

/* Status Register */
#define IMX7_QSPI_SR_TXFULL_MASK         0x8000000       /**< TX Buffer Full. Asserted when no more data can be stored. */
#define IMX7_QSPI_SR_IP_ACC_MASK         0x2             /**< IP Access: Asserted when transaction currently executed was initiated by IP bus. */
#define IMX7_QSPI_SR_BUSY_MASK           0x1             /**< Module Busy: Module is handling a transaction to an external flash device. */

/* RX Buffer Control Register */
#define IMX7_QSPI_RBCT_RXBRD_MASK        0x100   /**< RX Buffer Readout: 1: RBDR0 to RBDRn */

/* Sequence Pointer Clear Register */
#define IMX7_QSPI_SPTRCLR_IPPTRC_MASK    0x100   /**< 1: Clears the sequence pointer for IP accesses as defined in IPCR. */
#define IMX7_QSPI_SPTRCLR_BFPTRC_MASK    0x1     /**< 1: Clears the sequence pointer for AHB accesses as defined in BFGENCR. */

/* RSER */
/* RX Buffer Drain Interrupt Enable */
#define IMX7_QSPI_RSER_RBDIE_SHIFT       16
#define IMX7_QSPI_RSER_RBDIE_MASK        (1 << IMX7_QSPI_RSER_RBDIE_SHIFT)

/* TX Buffer Fill Interrupt Enable */
#define IMX7_QSPI_RSER_TBFIE_SHIFT       27
#define IMX7_QSPI_RSER_TBFIE_MASK        (1 << IMX7_QSPI_RSER_TBFIE_SHIFT)

/* Transaction Finished Interrupt Enable */
#define IMX7_QSPI_RSER_TFIE_SHIFT        1
#define IMX7_QSPI_RSER_TFIE_MASK         (1 << IMX7_QSPI_RSER_TFIE_SHIFT)

/*
 * SDMA
 */
#define IMX7_SDMA_BASE                  0x30BD0000
#define IMX7_SDMA_IRQ                   34
#define IMX7_SDMA_SIZE                  0x4000

/* SDMA Registers, offset from base address */
#define IMX7_SDMA_MC0PTR                0x00    /* AP (MCU) Channel 0 Pointer */
#define IMX7_SDMA_INTR                  0x04    /* Channel Interrupts */
#define IMX7_SDMA_STOP_STAT             0x08    /* Channel Stop / Channel Status */
#define IMX7_SDMA_HSTART                0x0C    /* Channel Start */
#define IMX7_SDMA_EVTOVR                0x10    /* Channel Event Override */
#define IMX7_SDMA_DSPOVR                0x14    /* Channel DSP (BP) Override */
#define IMX7_SDMA_HOSTOVR               0x18    /* Channel AP Override */
#define IMX7_SDMA_EVTPEND               0x1C    /* Channel Event Pending */
#define IMX7_SDMA_RESET                 0x24    /* Reset Register */
#define IMX7_SDMA_EVTERR                0x28    /* DMA Request Error Register */
#define IMX7_SDMA_INTRMASK              0x2C    /* Channel AP Interrupt Mask */
#define IMX7_SDMA_PSW                   0x30    /* Schedule Status */
#define IMX7_SDMA_EVTERRDBG             0x34    /* DMA Request Error Register */
#define IMX7_SDMA_CONFIG                0x38    /* Configuration Register */
#define IMX7_SDMA_ONCE_ENB              0x40    /* OnCE Enable */
#define IMX7_SDMA_ONCE_DATA             0x44    /* OnCE Data Register */
#define IMX7_SDMA_ONCE_INSTR            0x48    /* OnCE Instruction Register */
#define IMX7_SDMA_ONCE_STAT             0x4C    /* OnCE Status Register */
#define IMX7_SDMA_ONCE_CMD              0x50    /* OnCE Command Register */
#define IMX7_SDMA_EVT_MIRROT            0x54    /* DMA Request */
#define IMX7_SDMA_ILLINSTADDR           0x58    /* Illegal Instruction Trap Address */
#define IMX7_SDMA_CHN0ADDR              0x5C    /* Channel 0 Boot Address */
#define IMX7_SDMA_XTRIG_CONF1           0x70    /* Cross-Trigger Events Configuration Register 1 */
#define IMX7_SDMA_XTRIG_CONF2           0x74    /* Cross-Trigger Events Configuration Register 2 */
#define IMX7_SDMA_CHNPRI(n)             (0x100 + ((n)<<2))   /* Channel Priority n = 0 to 31 */
#define IMX7_SDMA_CHNENBL(n)            (0x200 + ((n)<<2))   /* Channel Enable n = 0 to 47 */

/*
 * System Reset Controller (SRC)
 */
#define IMX7_SRC_BASE                   0x30390000


#define IMX7_SRC_SCR                    0x000
#define IMX7_SRC_A7RCR0                 0x004
#define IMX7_SRC_A7RCR1                 0x008
#define IMX7_SRC_M4RCR                  0x00C
#define IMX7_SRC_ERCR                   0x014
#define IMX7_SRC_SMBR1                  0x058
#define IMX7_SRC_SRSR                   0x05C
#define IMX7_SRC_SISR                   0x068
#define IMX7_SRC_SIMR                   0x06C
#define IMX7_SRC_SBMR2                  0x070
#define IMX7_SRC_GPR1                   0x074
#define IMX7_SRC_GPR2                   0x078
#define IMX7_SRC_GPR3                   0x07C
#define IMX7_SRC_GPR4                   0x080
#define IMX7_SRC_GPR5                   0x084
#define IMX7_SRC_GPR6                   0x088
#define IMX7_SRC_GPR7                   0x08C
#define IMX7_SRC_GPR8                   0x090
#define IMX7_SRC_GPR9                   0x094
#define IMX7_SRC_GPR10                  0x098


#define BP_SRC_A7RCR0_A7_CORE0_RST     4
#define BP_SRC_A7RCR0_A7_DBG_RST       8
#define BP_SRC_A7RCR1_A7_CORE1_ENABLE  1

/*
 * Watchdogs
 */
#define IMX7_WDOG1_BASE                 0x30280000
#define IMX7_WDOG2_BASE                 0x30290000
#define IMX7_WDOG3_BASE                 0x302A0000
#define IMX7_WDOG4_BASE                 0x302B0000

#define IMX7_WDOG_SIZE                  0x10000

/*
 * Watchdog Timer Registers, offset from base address
 */
#define IMX7_WDOG_WCR                   0x00
#define IMX7_WDOG_WSR                   0x02
#define IMX7_WDOG_WRSR                  0x04
#define IMX7_WDOG_WICR                  0x06
#define IMX7_WDOG_WMCR                  0x08


/* WDOG_WCR bit fields */
#define IMX7_WDOG_WCR_WT_MASK           (0xff << 8)
#define IMX7_WDOG_WCR_WDW_MASK          (0x01 << 7)
#define IMX7_WDOG_WCR_SRE_MASK          (0x01 << 6)
#define IMX7_WDOG_WCR_WDA_MASK          (0x01 << 5)
#define IMX7_WDOG_WCR_SRS_MASK          (0x01 << 4)
#define IMX7_WDOG_WCR_WDT_MASK          (0x01 << 3)
#define IMX7_WDOG_WCR_WDE_MASK          (0x01 << 2)
#define IMX7_WDOG_WCR_WDBG_MASK         (0x01 << 1)
#define IMX7_WDOG_WCR_WDZST_MASK        (0x01 << 0)


#endif /* __ARM_MX7X_H_INCLUDED */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/arm/mx7x.h $ $Rev: 834524 $")
#endif
