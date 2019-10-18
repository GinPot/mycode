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


#ifndef    __ARM_MX6UL_H_INCLUDED
#define    __ARM_MX6UL_H_INCLUDED


/*
 * System Memory Map for the i.MX6 UltraLite
 */

/*
 * BOOT ROM (ROMCP)
 */
#define MX6UL_BOOTROM_BASE                       0x00000000
#define MX6UL_BOOTROM_SILICON_REV                0x48

#define MX6UL_WEIM_CS0_BASE                      0x08000000  /* NOR or SRAM */
#define MX6UL_QSPI_MEMORY_BASE                   0x60000000
#define MX6UL_QSPI_MEMORY_SIZE                   (256 * 1024 * 1024 )

/*
 * QSPI NOR Flash
 */
#define MX6UL_QSPI1_A1_BASE                      0x60000000
#define MX6UL_QSPI1_A2_BASE                      0x64000000
#define MX6UL_QSPI1_B1_BASE                      0x68000000
#define MX6UL_QSPI1_B2_BASE                      0x6C000000

/*
 * Enhanced Secured Digital Host Controllers
 */
#define MX6UL_USDHC1_BASE                        0x02190000
#define MX6UL_USDHC2_BASE                        0x02194000
#define MX6UL_USDHC_SIZE                         0x4000
#define MX6UL_USDHC_MAP_SIZE                     0x100

#define MX6UL_USDHC1_IRQ                         54
#define MX6UL_USDHC2_IRQ                         55

/*
 * Multi Mode DDR Controller (MMDC)
 */
#define MX6UL_MMDC_BASE                          0x021B0000
#define MX6UL_MMDC_SIZE                          0x4000

/*
 * UARTs
 */
#define MX6UL_UART1_BASE                         0x02020000
#define MX6UL_UART2_BASE                         0x021E8000
#define MX6UL_UART3_BASE                         0x021EC000
#define MX6UL_UART4_BASE                         0x021F0000
#define MX6UL_UART5_BASE                         0x021F4000

#define MX6UL_UART_SIZE                          0x4000

#define MX6UL_UART1_IRQ                          58
#define MX6UL_UART2_IRQ                          59
#define MX6UL_UART3_IRQ                          60
#define MX6UL_UART4_IRQ                          61
#define MX6UL_UART5_IRQ                          62

/*
 * USB Base Address
 */

#define MX6UL_USBOTG1_BASE                       0x02184000
#define MX6UL_USBOTG2_BASE                       0x02184200
#define MX6UL_USB_SIZE                           0x200

#define MX6UL_USB_CMD                            0x140

#define MX6UL_USBOTG1_IRQ                        75
#define MX6UL_USBOTG2_IRQ                        74

/* USB non-core registers */
#define MX6UL_USB_OTG1_CTRL                        0x00
#define MX6UL_USB_OTG2_CTRL                        0x04
#define MX6UL_USB_UH1_CTRL                        0x08
#define MX6UL_USB_UH1_HSIC_CTRL                    0x10
#define MX6UL_USB_OTG1_PHY_CTRL_0                0x18
#define MX6UL_USB_OTG2_PHY_CTRL_0                0x1c
/* USB non-core register bit masks */
#define MX6UL_USB_OTG_CTRL__PWR_POL_MASK       0x200
#define MX6UL_USB_OTG_CTRL__OC_POL_MASK        0x100

/*
 * ANATOP
 * Used for configuring PLLs, PFDs
 */
#define MX6UL_ANATOP_BASE                        0x020C8000
#define MX6UL_ANATOP_SIZE                        0x1000

#define MX6UL_ANATOP_PLL1_SYS                    0x00
#define MX6UL_ANATOP_PLL3_USB1                   0x10
#define MX6UL_ANATOP_PLL7_USB2                   0x20
#define MX6UL_ANATOP_PLL2_SYS_BUS                0x30
#define MX6UL_ANATOP_PLL4_AUDIO                  0x70
#define MX6UL_ANATOP_PLL4_AUDIO_NUM              0x80
#define MX6UL_ANATOP_PLL4_AUDIO_DENOM            0x90
#define MX6UL_ANATOP_PLL5_VIDEO                  0xA0
#define MX6UL_ANATOP_PLL5_VIDEO_SET              0xA4
#define MX6UL_ANATOP_PLL5_VIDEO_CLR              0xA8
#define MX6UL_ANATOP_PLL5_VIDEO_NUM              0xB0
#define MX6UL_ANATOP_PLL5_VIDEO_DENOM            0xC0
#define MX6UL_ANATOP_PLL6_ENET                   0xE0
#define MX6UL_ANATOP_PFD_480                     0xF0
#define MX6UL_ANATOP_PFD_480_SET                 0xF4
#define MX6UL_ANATOP_PFD_480_CLR                 0xF8
#define MX6UL_ANATOP_PFD_480_TOG                 0xFC
#define MX6UL_ANATOP_PFD_528                     0x100
#define MX6UL_ANATOP_PFD_528_SET                 0x104
#define MX6UL_ANATOP_PFD_528_CLR                 0x108
#define MX6UL_ANATOP_PFD_528_TOG                 0x10c
#define MX6UL_ANATOP_MISC2                       0x170
#define MX6UL_ANATOP_PFD0_OFFSET                    0
#define MX6UL_ANATOP_PFD0_MASK                    (0x3f << MX6UL_ANATOP_PFD0_OFFSET)
#define MX6UL_ANATOP_PFD1_OFFSET                    8
#define MX6UL_ANATOP_PFD1_MASK                    (0x3f << MX6UL_ANATOP_PFD1_OFFSET)
#define MX6UL_ANATOP_PFD2_OFFSET                    16
#define MX6UL_ANATOP_PFD2_MASK                    (0x3f << MX6UL_ANATOP_PFD2_OFFSET)
#define MX6UL_ANATOP_PFD3_OFFSET                    24
#define MX6UL_ANATOP_PFD3_MASK                    (0x3f << MX6UL_ANATOP_PFD3_OFFSET)
#define MX6UL_ANATOP_PFD0_GATE                    7
#define MX6UL_ANATOP_PFD1_GATE                    15
#define MX6UL_ANATOP_PFD2_GATE                    23
#define MX6UL_ANATOP_PFD3_GATE                    31
#define MX6UL_ANATOP_PFD_ALL_CLOCK_GATES         0x80808080

#define ANATOP_SETREG_OFFSET                    0x04
#define ANATOP_CLRREG_OFFSET                    0x08
#define ANATOP_TOGGLE_OFFSET                    0x0C

/* Audio, Video PLL (PLL4,5) offsets */
#define ANATOP_PLLAUDIOVIDEO_TEST_DIV_SEL_MASK     (0x3 << 19)
#define ANATOP_PLLAUDIOVIDEO_TEST_DIV_SEL_OFFSET (19)

/* ENET PLL (PLL6) offsets */
#define ANATOP_PLL6_ENET_LOCK                   (1 << 31)
#define ANATOP_PLL6_ENET_25M_REF_EN             (1 << 21)
#define ANATOP_PLL6_ENET_ENET2_125M_EN          (1 << 20)
#define ANATOP_PLL6_ENET_ENABLE_125             (1 << 19)
#define ANATOP_PLL6_ENET_PFD_OFFSET_EN          (1 << 18)
#define ANATOP_PLL6_ENET_BYPASS                 (1 << 16)
#define ANATOP_PLL6_ENET_BYPASS_CLK_SRC_MASK    (0x3 << 14)
#define ANATOP_PLL6_ENET_BYPASS_CLK_OSC         (0x0 << 14)
#define ANATOP_PLL6_ENET_BYPASS_CLK_ANACLK_1    (0x1 << 14)
#define ANATOP_PLL6_ENET_BYPASS_CLK_ANACLK_2    (0x2 << 14)
#define ANATOP_PLL6_ENET_BYPASS_CLK_XOR         (0x3 << 14)
#define ANATOP_PLL6_ENET_ENET1_125M_EN          (1 << 13)
#define ANATOP_PLL6_ENET_POWERDOWN              (1 << 12)
#define ANATOP_PLL6_ENET_ENET1_DIV_SELECT_MASK   0x0000000C
#define ANATOP_PLL6_ENET_ENET1_DIV_SELECT_25M   (0<<2)
#define ANATOP_PLL6_ENET_ENET1_DIV_SELECT_50M   (1<<2)
#define ANATOP_PLL6_ENET_ENET1_DIV_SELECT_100M  (2<<2)
#define ANATOP_PLL6_ENET_ENET1_DIV_SELECT_125M  (3<<2)
#define ANATOP_PLL6_ENET_ENET0_DIV_SELECT_MASK   0x00000003
#define ANATOP_PLL6_ENET_ENET0_DIV_SELECT_25M    0x0
#define ANATOP_PLL6_ENET_ENET0_DIV_SELECT_50M    0x1
#define ANATOP_PLL6_ENET_ENET0_DIV_SELECT_100M   0x2
#define ANATOP_PLL6_ENET_ENET0_DIV_SELECT_125M   0x3

// Registers below are not documented, but used by Linux code and critical for PLL, chip information
#define MX6UL_ANADIG_REG_CORE                    0x140
#define MX6UL_ANADIG_MISC1                       0x160
#define MX6UL_ANADIG_MISC2                       0x170
#define MX6UL_ANADIG_CHIP_INFO                   0x260

#define ANADIG_REG_CORE_REG2_TRG_MASK           0x7C0000

#define ANADIG_MISC2_CONTROL3_MASK              (0x3 << 30)
#define ANADIG_MISC2_CONTROL3_OFFSET            (30)


/*
 * USB PHY0, 1
 */
#define MX6UL_USBPHY0_BASE                       0x020C9000
#define MX6UL_USBPHY1_BASE                       0x020CA000

#define MX6UL_USBPHY_PWD                         0x00
#define MX6UL_USBPHY_TX                          0x10
#define MX6UL_USBPHY_RX                          0x20
#define MX6UL_USBPHY_CTRL                        0x30
#define MX6UL_USBPHY_STATUS                      0x40
#define MX6UL_USBPHY_DEBUG                       0x50
#define MX6UL_USBPHY_DEBUG0_STATUS               0x60
#define MX6UL_USBPHY_DEBUG1_STATUS               0x70
#define MX6UL_USBPHY_VERSION                     0x80
#define MX6UL_USBPHY_IP                          0x90

/*
 * SNVS Secure Non Voltatile Storage
 */
#define MX6UL_SNVS_BASE                            0x020CC000
#define MX6UL_SNVS_SIZE                            0x4000
#define MX6UL_SNVS_SECURE_IRQ                    51        /* Non TrustZone    */
#define MX6UL_SNVS_NON_SECURE_IRQ                52        /* TrustZone        */

#define MX6UL_SNVS_HPSR                            0x14    /* HP Status Register */
#define MX6UL_SNVS_LPCR                            0x38    /* LP Control Register */
#define MX6UL_SNVS_LPSR                            0x4C    /* LP Status Register */
#define MX6UL_SNVS_LPSRTCMR                        0x50    /* LP Secure Real Time Counter MSB Register */
#define MX6UL_SNVS_LPSRTCLR                        0x54    /* LP Secure Real Time Counter LSB Register */
#define MX6UL_SNVS_LPTAR                            0x58    /* LP Time Alarm Register */
#define MX6UL_SNVS_LPSMCMR                        0x5C    /* LP Secure Monotonic Counter MSB Register */
#define MX6UL_SNVS_LPSMCLR                        0x60    /* LP Secure Monotonic Counter LSB Register */
#define MX6UL_SNVS_LPPGDR                        0x64    /* LP Power Glitch Detector Register */
#define MX6UL_SNVS_LPGPR                            0x68    /* LP General Purpose Register */

/* iMX6UL EVK Macro for pads masking */
#define PAD_MASK(idx)                          (1 << idx)

/*
 * Enhanced Configurable Serial Peripheral Interfaces
 */

#define MX6UL_ECSPI_SIZE                         0x4000

#define MX6UL_ECSPI1_BASE                        0x02008000
#define MX6UL_ECSPI2_BASE                        0x0200C000
#define MX6UL_ECSPI3_BASE                        0x02010000
#define MX6UL_ECSPI4_BASE                        0x02014000
#define MX6UL_ECSPI5_BASE                        0x0228C000

#define MX6UL_ECSPI1_IRQ                         63
#define MX6UL_ECSPI2_IRQ                         64
#define MX6UL_ECSPI3_IRQ                         65
#define MX6UL_ECSPI4_IRQ                         66
#define MX6UL_ECSPI5_IRQ                         18


/*
 * Audio MUX
 */
#define MX6UL_AUDMUX_BASE                        0x021D8000
#define MX6UL_AUDMUX_SIZE                        0x4000

/* Port offset */
#define MX6UL_AUDMUX_PTCR1                       0x00
#define MX6UL_AUDMUX_PDCR1                       0x04
#define MX6UL_AUDMUX_PTCR2                       0x08
#define MX6UL_AUDMUX_PDCR2                       0x0C
#define MX6UL_AUDMUX_PTCR3                       0x10
#define MX6UL_AUDMUX_PDCR3                       0x14
#define MX6UL_AUDMUX_PTCR4                       0x18
#define MX6UL_AUDMUX_PDCR4                       0x1C
#define MX6UL_AUDMUX_PTCR5                       0x20
#define MX6UL_AUDMUX_PDCR5                       0x24
#define MX6UL_AUDMUX_PTCR6                       0x28
#define MX6UL_AUDMUX_PDCR6                       0x2C
#define MX6UL_AUDMUX_PTCR7                       0x30
#define MX6UL_AUDMUX_PDCR7                       0x34
#define MX6UL_AUDMUX_CNMCR                       0x38
#define MX6UL_AUDMUX_CNMCR                       0x38

/* DIGITAL AUDIO MULTIPLEXER (AUDMUX) */
#define AUDMUX_SYNC                             (1<<11)
#define AUDMUX_TFS_DIROUT                       (1<<31)
#define AUDMUX_TFS_PORT4                        (3<<27)
#define AUDMUX_TFS_PORT5                        (4<<27)
#define AUDMUX_TCLK_DIROUT                      (1<<26)
#define AUDMUX_TCLK_PORT4                       (3<<22)
#define AUDMUX_TCLK_PORT5                       (4<<22)
#define AUDMUX_RFS_DIROUT                       (1<<21)
#define AUDMUX_RFS_PORT4                        (3<<17)
#define AUDMUX_RFS_PORT5                        (4<<17)
#define AUDMUX_RCLK_DIROUT                      (1<<16)
#define AUDMUX_RCLK_PORT4                       (3<<12)
#define AUDMUX_RCLK_PORT5                       (4<<12)
#define AUDMUX_RXDSEL_PORT1                     (0<<13)
#define AUDMUX_RXDSEL_PORT2                     (1<<13)
#define AUDMUX_RXDSEL_PORT4                     (3<<13)
#define AUDMUX_RXDSEL_PORT5                     (4<<13)
#define AUDMUX_RXDSEL_MSK                       (7<<13)

#define MX6UL_CCM_BASE                           0x020C4000
#define MX6UL_CCM_SIZE                           0x4000

/* CCM Registers, offset from base address */
#define MX6UL_CCM_CCR                            0x00
#define MX6UL_CCM_CCDR                           0x04
#define MX6UL_CCM_CSR                            0x08
#define MX6UL_CCM_CCSR                           0x0C
#define MX6UL_CCM_CACCR                          0x10
#define MX6UL_CCM_CBCDR                          0x14
    #define MX6UL_CCM_CBCDR_PERIPH_CLK2_PODF_MASK         (0x7 << 27)
    #define MX6UL_CCM_CBCDR_PERIPH_CLK2_PODF_OFFSET       (27)
    #define MX6UL_CCM_CBCDR_PERIPH2_CLK2_SEL              (1 << 26)
    #define MX6UL_CCM_CBCDR_PERIPH_CLK_SEL                (1 << 25)
    #define MX6UL_CCM_CBCDR_MMDC_CH0_PODF_MASK            (0x7 << 19)
    #define MX6UL_CCM_CBCDR_MMDC_CH0_PODF_OFFSET          (19)
    #define MX6UL_CCM_CBCDR_AXI_PODF_MASK                 (0x7 << 16)
    #define MX6UL_CCM_CBCDR_AXI_PODF_OFFSET               (16)
    #define MX6UL_CCM_CBCDR_AHB_PODF_MASK                 (0x7 << 10)
    #define MX6UL_CCM_CBCDR_AHB_PODF_OFFSET               (10)
    #define MX6UL_CCM_CBCDR_IPG_PODF_MASK                 (0x3 << 8)
    #define MX6UL_CCM_CBCDR_IPG_PODF_OFFSET               (8)
    #define MX6UL_CCM_CBCDR_AXI_ALT_SEL                   (1 << 7)
    #define MX6UL_CCM_CBCDR_AXI_SEL                       (1 << 6)
    #define MX6UL_CCM_CBCDR_MMDC_CH1_PODF_MASK            (0x7 << 3)
    #define MX6UL_CCM_CBCDR_MMDC_CH1_PODF_OFFSET          (3)
    #define MX6UL_CCM_CBCDR_PERIPH2_CLK2_PODF_MASK       (0x7 << 0)
    #define MX6UL_CCM_CBCDR_PERIPH2_CLK2_PODF_OFFSET        (0)
#define MX6UL_CCM_CBCMR                          0x18
    #define MX6UL_CCM_CBCMR_GPU2D_CORE_PODF_MASK          (0x7 << 23)
    #define MX6UL_CCM_CBCMR_GPU2D_CORE_PODF_OFFSET       (23)
    #define MX6UL_CCM_CBCMR_PRE_PERIPH2_CLK_SEL_MASK     (0x3 << 21)
    #define MX6UL_CCM_CBCMR_PRE_PERIPH2_CLK_SEL_OFFSET   (21)
    #define MX6UL_CCM_CBCMR_PRE_PERIPH2_CLK2_SEL         (1 << 20)
    #define MX6UL_CCM_CBCMR_PRE_PERIPH_CLK_SEL_MASK      (0x3 << 18)
    #define MX6UL_CCM_CBCMR_PRE_PERIPH_CLK_SEL_OFFSET    (18)
    #define MX6UL_CCM_CBCMR_PERIPH_CLK2_SEL_MASK          (0x3 << 12)
    #define MX6UL_CCM_CBCMR_PERIPH_CLK2_SEL_OFFSET       (12)
#define MX6UL_CCM_CSCMR1                         0x1c
    #define MX6UL_CCM_CSCMR1_ACLK_EIM_SLOW_MASK           (0x3 << 29)
    #define MX6UL_CCM_CSCMR1_ACLK_EIM_SLOW_OFFSET         (29)
    #define MX6UL_CCM_CSCMR1_QSPI1_PODF_MASK               (0x7 << 26)
    #define MX6UL_CCM_CSCMR1_QSPI1_PODF_OFFSET             (26)
    #define MX6UL_CCM_CSCMR1_ACLK_EIM_SLOW_PODF_MASK     (0x7 << 23)
    #define MX6UL_CCM_CSCMR1_ACLK_EIM_SLOW_PODF_OFFSET   (23)
    #define MX6UL_CCM_CSCMR1_GPMI_CLK_SEL                 (1 << 19)
    #define MX6UL_CCM_CSCMR1_BCH_CLK_SEL                  (1 << 18)
    #define MX6UL_CCM_CSCMR1_USDHC2_CLK_SEL               (1 << 17)
    #define MX6UL_CCM_CSCMR1_USDHC1_CLK_SEL               (1 << 16)
    #define MX6UL_CCM_CSCMR1_SAI3_CLK_SEL_MASK            (0x3 << 14)
    #define MX6UL_CCM_CSCMR1_SAI3_CLK_SEL_OFFSET          (14)
    #define MX6UL_CCM_CSCMR1_SAI2_CLK_SEL_MASK            (0x3 << 12)
    #define MX6UL_CCM_CSCMR1_SAI2_CLK_SEL_OFFSET          (12)
    #define MX6UL_CCM_CSCMR1_SAI1_CLK_SEL_MASK            (0x3 << 10)
    #define MX6UL_CCM_CSCMR1_SAI1_CLK_SEL_OFFSET          (10)
    #define MX6UL_CCM_CSCMR1_QSPI1_CLK_SEL_MASK            (0x7 << 7)
    #define MX6UL_CCM_CSCMR1_QSPI1_CLK_SEL_OFFSET          (7)
        /* Macros to select QSPI1 reference clock */
        #define MX6UL_CCM_CSCMR1_QSPI1_CLK_SEL_BV_PLL3        0x0   /* 480   MHz */
        #define MX6UL_CCM_CSCMR1_QSPI1_CLK_SEL_BV_PLL2_PFD0   0x1   /* 352   MHz */
        #define MX6UL_CCM_CSCMR1_QSPI1_CLK_SEL_BV_PLL2_PFD2   0x2   /* 400   MHz */
        #define MX6UL_CCM_CSCMR1_QSPI1_CLK_SEL_BV_PLL2        0x3   /* 528   MHz */
        #define MX6UL_CCM_CSCMR1_QSPI1_CLK_SEL_BV_PLL3_PFD3   0x4   /* 454.7 MHz */
        #define MX6UL_CCM_CSCMR1_QSPI1_CLK_SEL_BV_PLL3_PFD2   0x5   /* 508.2 MHz */
    #define MX6UL_CCM_CSCMR1_PERCLK_PODF_MASK             (0x3F)
#define MX6UL_CCM_CSCMR2                         0x20
#define MX6UL_CCM_CSCDR1                         0x24
    #define MX6UL_CCM_CSCDR1_GPMI_PODF_DIV_2              (0x1 << 22)
    #define MX6UL_CCM_CSCDR1_GPMI_PODF_DIV_4              (0x3 << 22)
    #define MX6UL_CCM_CSCDR1_GPMI_PODF_MASK               (0x7 << 22)
    #define MX6UL_CCM_CSCDR1_GPMI_PODF_OFFSET             (22)
    #define MX6UL_CCM_CSCDR1_BCH_PODF_DIV_2               (0x1 << 19)
    #define MX6UL_CCM_CSCDR1_BCH_PODF_DIV_4               (0x3 << 19)
    #define MX6UL_CCM_CSCDR1_BCH_PODF_MASK                (0x7 << 19)
    #define MX6UL_CCM_CSCDR1_BCH_PODF_OFFSET              (19)
    #define MX6UL_CCM_CSCDR1_USDHC2_PODF_DIV_2            (0x1 << 16)
    #define MX6UL_CCM_CSCDR1_USDHC2_PODF_MASK             (0x7 << 16)
    #define MX6UL_CCM_CSCDR1_USDHC2_PODF_OFFSET           (16)
    #define MX6UL_CCM_CSCDR1_USDHC1_PODF_DIV_2            (0x1 << 11)
    #define MX6UL_CCM_CSCDR1_USDHC1_PODF_MASK             (0x7 << 11)
    #define MX6UL_CCM_CSCDR1_USDHC1_PODF_OFFSET           (11)
    #define MX6UL_CCM_CSCDR1_UART_CLK_SEL                 (0x1 << 6)
    #define MX6UL_CCM_CSCDR1_UART_CLK_SEl_OFFSET          (6)
    #define MX6UL_CCM_CSCDR1_UART_CLK_PODF_MASK           (0x3F)
    #define MX6UL_CCM_CSCDR1_UART_CLK_PODF_OFFSET         (0)
#define MX6UL_CCM_CS1CDR                         0x28
#define MX6UL_CCM_CS2CDR                         0x2c
    #define MX6UL_CCM_CS2CDR_ENFC_CLK_PODF_MASK           (0x3F << 21)
    #define MX6UL_CCM_CS2CDR_ENFC_CLK_PODF_OFFSET         (21)
    #define MX6UL_CCM_CS2CDR_ENFC_CLK_PRED_MASK           (0x7 << 18)
    #define MX6UL_CCM_CS2CDR_ENFC_CLK_PRED_OFFSET         (18)
    #define MX6UL_CCM_CS2CDR_ENFC_CLK_SEL_MASK            (0x3 << 16)
    #define MX6UL_CCM_CS2CDR_ENFC_CLK_SEL_OFFSET          (16)
    #define MX6UL_CCM_CS2CDR_LDB_DI1_CLK_SEL_MASK         (0x7 << 12)
    #define MX6UL_CCM_CS2CDR_LDB_DI1_CLK_SEL_OFFSET       (12)
    #define MX6UL_CCM_CS2CDR_LDB_DI0_CLK_SEL_MASK         (0x7 << 9)
    #define MX6UL_CCM_CS2CDR_LDB_DI0_CLK_SEL_OFFSET       (9)
    #define MX6UL_CCM_CS2CDR_SAI2_CLK_PRED_MASK           (0x7 << 6)
    #define MX6UL_CCM_CS2CDR_SAI2_CLK_PRED_OFFSET         (6)
    #define MX6UL_CCM_CS2CDR_SAI2_CLK_PODF_MASK           (0x3F)
    #define MX6UL_CCM_CS2CDR_SAI2_CLK_PODF_OFFSET         (0)
#define MX6UL_CCM_CDCDR                          0x30
#define MX6UL_CCM_CHSCCDR                        0x34
#define MX6UL_CCM_CSCDR2                         0x38
    #define MX6UL_CCM_CSCDR2_ECSPI_CLK_PODF_MASK        (0x3F << 19)
    #define MX6UL_CCM_CSCDR2_ECSPI_CLK_PODF_OFFSET      (19)
#define MX6UL_CCM_CSCDR3                         0x3c
#define MX6UL_CCM_CSCDR4                         0x40
#define MX6UL_CCM_CWDR                           0x44
#define MX6UL_CCM_CDHIPR                         0x48
#define MX6UL_CCM_CDCR                           0x4c
#define MX6UL_CCM_CTOR                           0x50
#define MX6UL_CCM_CLPCR                          0x54
#define MX6UL_CCM_CISR                           0x58
#define MX6UL_CCM_CIMR                           0x5c
#define MX6UL_CCM_CCOSR                          0x60
#define MX6UL_CCM_CGPR                           0x64
#define MX6UL_CCM_CCGR0                          0x68
#define MX6UL_CCM_CCGR1                          0x6c
    #define MX6UL_CCGR1_GPU3D_ENABLE                  (0x3 << 26)
#define MX6UL_CCM_CCGR2                          0x70
#define MX6UL_CCM_CCGR3                          0x74
#define MX6UL_CCM_CCGR4                          0x78
#define MX6UL_CCM_CCGR5                          0x7c
#define MX6UL_CCM_CCGR6                          0x80
#define MX6UL_CCM_CMEOR                          0x88

/* CCM Clock Gating Reset Values */
#define MX6UL_CCM_CCGR0_RESET                    0xFFFFFFFF
#define MX6UL_CCM_CCGR1_RESET                    0xFFFFFFFF
#define MX6UL_CCM_CCGR2_RESET                    0xFFFFFFFF
#define MX6UL_CCM_CCGR3_RESET                    0xFFFFFFFF
#define MX6UL_CCM_CCGR4_RESET                    0xFFFFFFFF
#define MX6UL_CCM_CCGR5_RESET                    0xFFFFFFFF
#define MX6UL_CCM_CCGR6_RESET                    0xFFFFFFFF

/* CCM Clock Gating Values */
#define CCGR0_CG15_AIPS_TZ3                     (0x3 << 30
#define CCGR0_CG13_DCIC2                        (0x3 << 26)
#define CCGR0_CG12_DCIC1                        (0x3 << 24)
#define CCGR0_CG11_CHEETAH_DBG                  (0x3 << 22)
#define CCGR0_CG10_CAN2_SERIAL                  (0x3 << 20)
#define CCGR0_CG9_CAN2_CLK                      (0x3 << 18)
#define CCGR0_CG8_CAN1_SERIAL                   (0x3 << 16)
#define CCGR0_CG7_CAN1_CLK                      (0x3 << 14)
#define CCGR0_CG6_CAAM_IPG                      (0x3 << 12)
#define CCGR0_CG5_CAAM_ACLK                     (0x3 << 10)
#define CCGR0_CG4_CAAM_SECURE_MEM               (0x3 << 8)
#define CCGR0_CG3_ASRC                          (0x3 << 6)
#define CCGR0_CG2_APBH_DMA_HCLK                 (0x3 << 4)
#define CCGR0_CG1_AIPS_TZ2                      (0x3 << 2)
#define CCGR0_CG0_AIPS_TZ1                      (0x3 << 0)

#define CCGR1_CG15_CANDFD                       (0x3 << 30)
#define CCGR1_CG14_OCRAM_S                      (0x3 << 28)
#define CCGR1_CG13_GPU                          (0x3 << 26)
#define CCGR1_CG11_GPT_SERIAL                   (0x3 << 22)
#define CCGR1_CG10_GPT                          (0x3 << 20)
#define CCGR1_CG9_WAKEUP                        (0x3 << 18)
#define CCGR1_CG8_ESAI                          (0x3 << 16)
#define CCGR1_CG7_EPIT2                         (0x3 << 14)
#define CCGR1_CG6_EPIT1                         (0x3 << 12)
#define CCGR1_CG4_ECSPI5                        (0x3 << 8)
#define CCGR1_CG3_ECSPI4                        (0x3 << 6)
#define CCGR1_CG2_ECSPI3                        (0x3 << 4)
#define CCGR1_CG1_ECSPI2                        (0x3 << 2)
#define CCGR1_CG0_ECSPI1                        (0x3 << 0)

#define CCGR2_CG15_PXP                          (0x3 << 30)
#define CCGR2_CG14_LCD                          (0x3 << 28)
#define CCGR2_CG11_IPSYNC                       (0x3 << 22)
#define CCGR2_CG10_IPMUX3                       (0x3 << 20)
#define CCGR2_CG9_IPMUX2                        (0x3 << 18)
#define CCGR2_CG8_IPMUX1                        (0x3 << 16)
#define CCGR2_CG7_IOMUX_IPT                     (0x3 << 14)
#define CCGR2_CG6_OCOTP                         (0x3 << 12)
#define CCGR2_CG5_I2C3                          (0x3 << 10)
#define CCGR2_CG4_I2C2                          (0x3 << 8)
#define CCGR2_CG3_I2C1                          (0x3 << 6)
#define CCGR2_CG1_CSI                           (0x3 << 2)

#define CCGR3_CG14_OCRAM                        (0x3 << 28)
#define CCGR3_CG13_MMDC_CORE_IPG_P1             (0x3 << 26)
#define CCGR3_CG12_MMDC_CORE_IPG_P0             (0x3 << 24)
#define CCGR3_CG10_MMDC_CORE_ACLK_FAST_P0       (0x3 << 20)
#define CCGR3_CG9_MLB                           (0x3 << 18)
#define CCGR3_CG7_QSPI1                         (0x3 << 14)
#define CCGR3_CG6_LDB_DI0                       (0x3 << 12)
#define CCGR3_CG5_LCDIF1                        (0x3 << 10)
#define CCGR3_CG4_LCDIF2                        (0x3 << 8)
#define CCGR3_CG3_DISP_AXI                      (0x3 << 6)
#define CCGR3_CG2_ENET                          (0x3 << 4)
#define CCGR3_CG1_M4                            (0x3 << 2)

#define CCGR4_CG15_RAWNAND_GPMI_INPUT_APB       (0x3 << 30)
#define CCGR4_CG14_RAWNAND_GPMI_BCH_INPUT       (0x3 << 28)
#define CCGR4_CG13_RAWNAND_GPMI_BCH_INPUT       (0x3 << 26)
#define CCGR4_CG12_RAWNAND_BCH_INPUT_APB        (0x3 << 24)
#define CCGR4_CG11_PWM4                         (0x3 << 22)
#define CCGR4_CG10_PWM3                         (0x3 << 20)
#define CCGR4_CG9_PWM2                          (0x3 << 18)
#define CCGR4_CG8_PWM1                          (0x3 << 16)
#define CCGR4_CG7_PL301_PER2_MAINCLK            (0x3 << 14)
#define CCGR4_CG6_PL301_PER1_BCH                (0x3 << 12)
#define CCGR4_CG5_TSC                           (0x3 << 10)
#define CCGR4_CG0_PCIE                          (0x3 << 0)

#define CCGR5_CG15_SAI2                         (0x3 << 30)
#define CCGR5_CG14_SAI1                         (0x3 << 28)
#define CCGR5_CG13_UART_SERIAL_CLK              (0x3 << 26)
#define CCGR5_CG12_UART_CLK                     (0x3 << 24)
#define CCGR5_CG11_SAI3_CLK                     (0x3 << 22)
#define CCGR5_CG10_SNVS_LP_CLK                  (0x3 << 20)
#define CCGR5_CG9_HP_CLK                        (0x3 << 18)
#define CCGR5_CG7_SPDIF                         (0x3 << 14)
#define CCGR5_CG6_SPBA                          (0x3 << 12)
#define CCGR5_CG3_SDMA                          (0x3 << 6)
#define CCGR5_CG0_ROM                           (0x3 << 0)


#define CCGR6_CG15_PWM7                         (0x3 << 30)
#define CCGR6_CG14_PWM6                         (0x3 << 28)
#define CCGR6_CG13_PWM5                         (0x3 << 26)
#define CCGR6_CG12_I2C4                         (0x3 << 24)
#define CCGR6_CG11_GIS                          (0x3 << 22)
#define CCGR6_CG10_VADC                         (0x3 << 20)
#define CCGR6_CG8_PWM8                          (0x3 << 16)
#define CCGR6_CG5_EMI_SLOW                      (0x3 << 10)
#define CCGR6_CG4_USDHC4                        (0x3 << 8)
#define CCGR6_CG3_USDHC3                        (0x3 << 6)
#define CCGR6_CG2_USDHC2                        (0x3 << 4)
#define CCGR6_CG1_USDHC1                        (0x3 << 2)
#define CCGR6_CG0_USBOH3                        (0x3 << 0)

#define CSCMR1_SSI1_CLK_SEL_MASK                (0x3<<10)
#define CSCMR1_SSI1_CLK_SEL_PLL4                (0x2<<10)
#define CSCMR1_SSI1_CLK_SEL_454PFD              (0x1<<10)

#define CSCMR1_SSI2_CLK_SEL_MASK                (0x3<<12)
#define CSCMR1_SSI2_CLK_SEL_PLL4                (0x2<<12)
#define CSCMR1_SSI2_CLK_SEL_454PFD              (0x1<<12)

#define CSCMR1_SSI3_CLK_SEL_MASK                (0x3<<14)
#define CSCMR1_SSI3_CLK_SEL_PLL4                (0x2<<14)

/*
 * Sony/Philips Digital Interface Transmitter
 */
#define MX6UL_SPDIF_BASE                         0x02004000
#define MX6UL_SPDIF_SIZE                         0x4000

#define MX6UL_SPDIF_IRQ                          84
/*
 * I2C
 */
#define MX6UL_I2C1_BASE                          0x021A0000
#define MX6UL_I2C2_BASE                          0x021A4000
#define MX6UL_I2C3_BASE                          0x021A8000
#define MX6UL_I2C4_BASE                          0x021F8000

#define MX6UL_I2C_SIZE                           0x4000

#define MX6UL_I2C1_IRQ                           68
#define MX6UL_I2C2_IRQ                           69
#define MX6UL_I2C3_IRQ                           70
#define MX6UL_I2C4_IRQ                           67

/*
 * Ethernet MAC-NET
 */
#define MX6UL_ENET1_BASE                         0x02188000
#define MX6UL_ENET2_BASE                         0x020B4000
#define MX6UL_ENET_SIZE                          0x4000
#define MX6UL_ENET_PALR                          0xE4
#define MX6UL_ENET_PAUR                          0xE8


#define MX6UL_ENET1_IRQ                          150
#define MX6UL_ENET2_IRQ                          152

#define MX6UL_ENET1_TIMER_IRQ                    151
#define MX6UL_ENET2_TIMER_IRQ                    153

/*
 * External Interface Module (EIM)
 */
#define MX6UL_EIM_BASE                           0x021B8000
#define MX6UL_EIM_CS0GCR1                        0x00
#define MX6UL_EIM_CS0GCR2                        0x04
#define MX6UL_EIM_CS0RCR1                        0x08
#define MX6UL_EIM_CS0RCR2                        0x0C
#define MX6UL_EIM_CS0WCR1                        0x10
#define MX6UL_EIM_CS0WCR2                        0x14
#define MX6UL_EIM_CS1GCR1                        0x18
#define MX6UL_EIM_CS1GCR2                        0x1C
#define MX6UL_EIM_CS1RCR1                        0x20
#define MX6UL_EIM_CS1RCR2                        0x24
#define MX6UL_EIM_CS1WCR1                        0x28
#define MX6UL_EIM_CS1WCR2                        0x2C
#define MX6UL_EIM_CS2GCR1                        0x30
#define MX6UL_EIM_CS2GCR2                        0x34
#define MX6UL_EIM_CS2RCR1                        0x38
#define MX6UL_EIM_CS2RCR2                        0x3C
#define MX6UL_EIM_CS2WCR1                        0x40
#define MX6UL_EIM_CS2WCR2                        0x44
#define MX6UL_EIM_CS3GCR1                        0x48
#define MX6UL_EIM_CS3GCR2                        0x4C
#define MX6UL_EIM_CS3RCR1                        0x50
#define MX6UL_EIM_CS3RCR2                        0x54
#define MX6UL_EIM_CS3WCR1                        0x58
#define MX6UL_EIM_CS3WCR2                        0x5C
#define MX6UL_EIM_WCR                            0x90

/*
 * CAN
 */
#define MX6UL_CAN1_PORT                          0x02090000
#define MX6UL_CAN1_MEM                           0x02090080
#define MX6UL_CAN1_IRQ                           142

#define MX6UL_CAN2_PORT                          0x02094000
#define MX6UL_CAN2_MEM                           0x02094080
#define MX6UL_CAN2_IRQ                           143

#define MX6UL_CAN_SIZE                           0x4000

/*
 * GPIOs
 */
#define MX6UL_GPIO1_BASE                         0x0209C000
#define MX6UL_GPIO2_BASE                         0x020A0000
#define MX6UL_GPIO3_BASE                         0x020A4000
#define MX6UL_GPIO4_BASE                         0x020A8000
#define MX6UL_GPIO5_BASE                         0x020AC000
#define MX6UL_GPIO_SIZE                          0x4000

/* GPIO Registers, offset from base address */
#define MX6UL_GPIO_DR                            0x00
#define MX6UL_GPIO_GDIR                          0x04
#define MX6UL_GPIO_PSR                           0x08
#define MX6UL_GPIO_ICR1                          0x0C
#define MX6UL_GPIO_ICR2                          0x10
#define MX6UL_GPIO_IMR                           0x14
#define MX6UL_GPIO_ISR                           0x18
#define MX6UL_GPIO_EDR                           0x1C

/*
 * SDMA Base Address
 */
#define MX6UL_SDMA_BASE                          0x020EC000
#define MX6UL_SDMA_IRQ                           34
#define MX6UL_SDMA_SIZE                          0x4000

/* SDMA Registers, offset from base address */
#define MX6UL_SDMA_MC0PTR                        0x00    /* AP (MCU) Channel 0 Pointer */
#define MX6UL_SDMA_INTR                          0x04    /* Channel Interrupts */
#define MX6UL_SDMA_STOP_STAT                     0x08    /* Channel Stop / Channel Status */
#define MX6UL_SDMA_HSTART                        0x0C    /* Channel Start */
#define MX6UL_SDMA_EVTOVR                        0x10    /* Channel Event Override */
#define MX6UL_SDMA_DSPOVR                        0x14    /* Channel DSP (BP) Override */
#define MX6UL_SDMA_HOSTOVR                       0x18    /* Channel AP Override */
#define MX6UL_SDMA_EVTPEND                       0x1C    /* Channel Event Pending */
#define MX6UL_SDMA_RESET                         0x24    /* Reset Register */
#define MX6UL_SDMA_EVTERR                        0x28    /* DMA Request Error Register */
#define MX6UL_SDMA_INTRMASK                      0x2C    /* Channel AP Interrupt Mask */
#define MX6UL_SDMA_PSW                           0x30    /* Schedule Status */
#define MX6UL_SDMA_EVTERRDBG                     0x34    /* DMA Request Error Register */
#define MX6UL_SDMA_CONFIG                        0x38    /* Configuration Register */
#define MX6UL_SDMA_ONCE_ENB                      0x40    /* OnCE Enable */
#define MX6UL_SDMA_ONCE_DATA                     0x44    /* OnCE Data Register */
#define MX6UL_SDMA_ONCE_INSTR                    0x48    /* OnCE Instruction Register */
#define MX6UL_SDMA_ONCE_STAT                     0x4C    /* OnCE Status Register */
#define MX6UL_SDMA_ONCE_CMD                      0x50    /* OnCE Command Register */
#define MX6UL_SDMA_EVT_MIRROT                    0x54    /* DMA Request */
#define MX6UL_SDMA_ILLINSTADDR                   0x58    /* Illegal Instruction Trap Address */
#define MX6UL_SDMA_CHN0ADDR                      0x5C    /* Channel 0 Boot Address */
#define MX6UL_SDMA_XTRIG_CONF1                   0x70    /* Cross-Trigger Events Configuration Register 1 */
#define MX6UL_SDMA_XTRIG_CONF2                   0x74    /* Cross-Trigger Events Configuration Register 2 */
#define MX6UL_SDMA_CHNPRI(n)                     (0x100 + ((n)<<2))   /* Channel Priority n = 0 to 31 */
#define MX6UL_SDMA_CHNENBL(n)                    (0x200 + ((n)<<2))   /* Channel Enable n = 0 to 47 */


/*
 * SNVS_LP
 */
#define MX6UL_SNVS_LP_BASE_ADDR                  0x20B0000

/*
 * Watchdogs
 */
#define MX6UL_WDOG1_BASE                         0x020BC000
#define MX6UL_WDOG2_BASE                         0x020C0000
#define MX6UL_WDOG3_BASE                         0x021E4000
#define MX6UL_WDOG_SIZE                          0x4000

/*
 * Watchdog Timer Registers, offset from base address
 */
#define MX6UL_WDOG_CR                            0x00
// Set default time out value to 30 seconds
#define WT_MASK                                 (0xff << 8)
#define WT                                      (0x3B << 8)
#define WDT                                     (0x01 << 3)
#define WDE                                     (0x01 << 2)
#define WDZST                                   (0x01)
#define MX6UL_WDOG_SR                            0x02
#define MX6UL_WDOG_RSR                           0x04
#define MX6UL_WDOG_ICR                           0x06
#define MX6UL_WDOG_MCR                           0x08

/*
 * General Purpose Timer
 */
#define MX6UL_GPT_BASE                           0x02098000
#define MX6UL_GPT_SIZE                           0x4000

/*
 * I/O MUX Controller
 */
#define MX6UL_IOMUXC_BASE                        0x020E0000
#define MX6UL_IOMUXC_GPR_BASE                    0x020E4000
#define MX6UL_IOMUXC_SIZE                        0x4000
#define MX6UL_IOMUXC_GPR_SIZE                    0x4000

/*
 * IOMUX GPR Registers, offset from GPR base address
 */
#define MX6UL_IOMUX_GPR0                         0x0000
#define MX6UL_IOMUX_GPR1                         0x0004
#define MX6UL_IOMUX_GPR2                         0x0008
#define MX6UL_IOMUX_GPR3                         0x000C
#define MX6UL_IOMUX_GPR4                         0x0010
#define MX6UL_IOMUX_GPR5                         0x0014
#define MX6UL_IOMUX_GPR9                         0x0024
#define MX6UL_IOMUX_GPR10                        0x0028
#define MX6UL_IOMUX_GPR14                        0x0038

/* Macros to handle ANATOP PLL used for enet */
#define MX6UL_IOMUX_GPR1_ENET1_CLK_SEL           (1 << 13)
#define MX6UL_IOMUX_GPR1_ENET2_CLK_SEL           (1 << 14)
#define MX6UL_IOMUX_GPR1_ENET1_TX_CLK_DIR        (1 << 17)
#define MX6UL_IOMUX_GPR1_ENET2_TX_CLK_DIR        (1 << 18)


/*
 * IOMUX Registers, offset from base address
 */
#define MX6UL_IOMUX_SWMUX                        0x0014
#define MX6UL_IOMUX_SWPAD                        0x0204
#define MX6UL_IOMUX_SWINPUT                      0x04B8


/*
 * AHB to IP Bridge (Trust Zone) (AIPSTZ)
 */
#define MX6UL_AIPS1_BASE                        0x02000000
#define MX6UL_AIPS2_BASE                        0x02100000
#define MX6UL_AIPS3_BASE                        0x02200000
#define MX6UL_AIPS1_CONFIG                      (MX6UL_AIPS1_BASE + 0x7c000)
#define MX6UL_AIPS2_CONFIG                      (MX6UL_AIPS2_BASE + 0x7c000)
#define MX6UL_AIPS3_CONFIG                      (MX6UL_AIPS3_BASE + 0x7c000)
#define MX6UL_AIPS_MPR0                         0x00
#define MX6UL_AIPS_MPR1                         0x04
#define MX6UL_AIPS_OPACR0                       0x40
#define MX6UL_AIPS_OPACR1                       0x44
#define MX6UL_AIPS_OPACR2                       0x48
#define MX6UL_AIPS_OPACR3                       0x4C
#define MX6UL_AIPS_OPACR4                       0x50

/*
 * Enhanced Periodic Timers
 */
#define MX6UL_EPIT1_BASE                         0x020D0000
#define MX6UL_EPIT2_BASE                         0x020D4000
#define MX6UL_EPIT_SIZE                          0x4000

#define MX6UL_EPIT1_IRQ                          88
#define MX6UL_EPIT2_IRQ                          89

/*
 * EPIT Registers, off set from base address
 */
#define MX6UL_EPIT_CR                            0x00    /* Control Register */
#define MX6UL_EPIT_SR                            0x04    /* Status Register */
#define MX6UL_EPIT_LR                            0x08    /* Load Register */
#define MX6UL_EPIT_CMPR                          0x0C    /* Compare Register */
#define MX6UL_EPIT_CNR                           0x10    /* Counter Register */

/*
 * SDRAM
 */
#define MX6UL_SDRAM_64_BIT_BASE                  0x80000000

/*
 * SIM
 */
#define MX6UL_SIM_BASE                           0x0218C000
#define MX6UL_SIM_SIZE                           0x4000


/*
 * GPC
 */
#define MX6UL_GPC_BASE                           0x020DC000

#define MX6UL_GPC_CNTR                           0x00
#define MX6UL_GPC_PGR                            0x04
#define MX6UL_GPC_IMR1                           0x08
#define MX6UL_GPC_IMR2                           0x0C
#define MX6UL_GPC_IMR3                           0x10
#define MX6UL_GPC_IMR4                           0x14
#define MX6UL_GPC_ISR1                           0x18
#define MX6UL_GPC_ISR2                           0x1C
#define MX6UL_GPC_ISR3                           0x20
#define MX6UL_GPC_ISR4                           0x24

/*
 * On-Chip eFuse OTP Controller
 */
#define MX6UL_OCOTP_BASE                         0x021BC000
#define MX6UL_OCOTP_MAC0                         0x620
#define MX6UL_OCOTP_MAC1                         0x630
#define MX6UL_OCOTP_MAC2                         0x640
/*
  * System Reset Controller (SRC)
  */
#define MX6UL_SRC_BASE                           0x020D8000
#define MX6UL_SRC_SCR                            0x000
#define MX6UL_SRC_SBMR                           0x004
#define MX6UL_SRC_SRSR                           0x008
#define MX6UL_SRC_SAIAR                          0x00c
#define MX6UL_SRC_SAIRAR                         0x010
#define MX6UL_SRC_SISR                           0x014
#define MX6UL_SRC_SIMR                           0x018
#define MX6UL_SRC_SBMR2                          0x01c
#define MX6UL_SRC_GPR1                           0x020
#define MX6UL_SRC_GPR2                           0x024
#define MX6UL_SRC_GPR3                           0x028
#define MX6UL_SRC_GPR4                           0x02c
#define MX6UL_SRC_GPR5                           0x030
#define MX6UL_SRC_GPR6                           0x034
#define MX6UL_SRC_GPR7                           0x038
#define MX6UL_SRC_GPR8                           0x03c
#define MX6UL_SRC_GPR9                           0x040
#define MX6UL_SRC_GPR10                          0x044

#define BP_SRC_SCR_CORE0_RST                     13
#define BP_SRC_SCR_CORES_DBG_RST                 21
#define BP_SRC_SCR_CORE1_ENABLE                  22
#define SCR_WARM_RESET_ENABLE                     1


/*
 * QSPI
 */
#define MX6UL_QSPI_BASE                        (0x021E0000)
#define MX6UL_QSPI_IRQ                          139
#define MX6UL_QSPI_SIZE                         0x4000


/* Trust Zone Address Space Controller (TZASC) */
#define MX6UL_TZASC                             0x021D0000

#define TZASC_CONFIGURATION                    0x000
#define TZASC_ACTION                           0x004
#define TZASC_LOCKDOWN_RANGE                   0x008
#define TZASC_LOCKDOWN_SELECT                  0x00C
#define TZASC_INT_STATUS                       0x010
#define TZASC_INT_CLEAR                        0x014
#define TZASC_FAIL_ADDRESS_LOW                 0x020
#define TZASC_FAIL_ADDRESS_HIGH                0x024
#define TZASC_FAIL_CONTROL                     0x028
#define TZASC_FAIL_ID                          0x02C
#define TZASC_SPECULATION_CONTROL              0x030
#define TZASC_SECURITY_INVERSION_EN            0x034
#define TZASC_REGION_SETUP_LOW(n)              (0x100 + (n*0x10))
#define TZASC_REGION_SETUP_HIGH(n)             (0x104 + (n*0x10))
#define TZASC_REGION_ATTRIBUTES(n)             (0x108 + (n*0x10))
    #define TZASC_REGION_PERM_ENABLEALL    (0xf << 28)
    #define TZASC_REGION_SIZE_64MB         (0x19 << 1)
    #define TZASC_REGION_SIZE_128MB        (0x1a << 1)
    #define TZASC_REGION_SIZE_256MB        (0x1b << 1)
    #define TZASC_REGION_SIZE_512MB        (0x1c << 1)
    #define TZASC_REGION_SIZE_1GB          (0x1d << 1)
    #define TZASC_REGION_SIZE_2GB          (0x1f << 1)
    #define TZASC_REGION_ATTR_ENABLE       (1 << 0)

#define CCM_PLL_ENET__ENET2_ENABLE            (1 << 21)
#define CCM_PLL_ENET__ENET1_ENABLE            (1 << 20)
#define CCM_PLL_ENET__ENABLE_125M             (1 << 19)
#define CCM_PLL_ENET__ENET0_ENABLE            (1 << 13)
#define CCM_PLL_ENET__ENET_1_DIV_125M         (3 << 2)
#define CCM_PLL_ENET__ENET_2_DIV_125M         (3 << 0)

/* ICR macros */
#define ICR_LOW_LEVEL                         0x0
#define ICR_HIGH_LEVEL                        0x1
#define ICR_RISING_EDGE                       0x2
#define ICR_FALLING_EDGE                      0x3

#define _ICR_MASK                             0x3
#define _ICR1(intr, type)                     ((type) << ((intr) * 2))
#define _ICR2(intr, type)                     ((type) << (((intr) - 16) * 2))
#define ICR_MASK(intr)                        (((intr) < 16) ? _ICR1((intr), _ICR_MASK) : _ICR2((intr), _ICR_MASK))
#define ICR_SET_INTR(intr, type)              (((intr) < 16) ? _ICR1((intr), (type))    : _ICR2((intr), (type)))


#endif  /* __ARM_MX6UL_H_INCLUDED */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/arm/mx6ul.h $ $Rev: 851808 $")
#endif
