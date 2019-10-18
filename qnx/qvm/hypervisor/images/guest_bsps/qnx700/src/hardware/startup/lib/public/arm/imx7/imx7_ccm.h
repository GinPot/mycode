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

#ifndef IMX7_CCM_H_
#define IMX7_CCM_H_

/*
 * Clock Control Module (CCM)
 */
#define IMX7_CCM_BASE                            0x30380000
#define IMX7_CCM_SIZE                            0x10000

/* CCM Registers, offset from base address */
#define IMX7_CCM_GPR0                            0x000              /* General Purpose Register */
#define IMX7_CCM_GPR0_SET                        0x004
#define IMX7_CCM_GPR0_CLR                        0x008
#define IMX7_CCM_GPR0_TOG                        0x00C

#define IMX7_CCM_PLL_CTRL(i)                     (0x800 + (16 * i)) /* CCM PLL Control Register 0..32 */
#define IMX7_CCM_PLL_CTRL_SET(i)                 (0x804 + (16 * i))
#define IMX7_CCM_PLL_CTRL_CLR(i)                 (0x808 + (16 * i))
#define IMX7_CCM_PLL_CTRL_TOG(i)                 (0x80C + (16 * i))

#define IMX7_CCM_CCGR(i)                         (0x4000 + (16 * i)) /* CCM Clock Gating Register 0..190 */
#define IMX7_CCM_CCGR_SET(i)                     (0x4004 + (16 * i))
#define IMX7_CCM_CCGR_CLR(i)                     (0x4008 + (16 * i))
#define IMX7_CCM_CCGR_TOG(i)                     (0x400C + (16 * i))

#define IMX7_CCM_TARGET_ROOT(i)                  (0x8000 + (128 * i)) /* Target Register 0..120 */
#define IMX7_CCM_TARGET_ROOT_SET(i)              (0x8004 + (128 * i))
#define IMX7_CCM_TARGET_ROOT_CLR(i)              (0x8008 + (128 * i))
#define IMX7_CCM_TARGET_ROOT_TOG(i)              (0x800C + (128 * i))

#define IMX7_CCM_MISC_ROOT(i)                    (0x8010 + (128 * i)) /* Miscellaneous Register 0..120 */
#define IMX7_CCM_MISC_ROOT_SET(i)                (0x8014 + (128 * i))
#define IMX7_CCM_MISC_ROOT_CLR(i)                (0x8018 + (128 * i))
#define IMX7_CCM_MISC_ROOT_TOG(i)                (0x801C + (128 * i))

#define IMX7_CCM_POST_ROOT(i)                    (0x8020 + (128 * i)) /* Post Divider Register 0..120 */
#define IMX7_CCM_POST_ROOT_SET(i)                (0x8024 + (128 * i))
#define IMX7_CCM_POST_ROOT_CLR(i)                (0x8028 + (128 * i))
#define IMX7_CCM_POST_ROOT_TOG(i)                (0x802C + (128 * i))

#define IMX7_CCM_PRE_ROOT(i)                     (0x8030 + (128 * i)) /* Pre Divider Register 0..120 */
#define IMX7_CCM_PRE_ROOT_SET(i)                 (0x8034 + (128 * i))
#define IMX7_CCM_PRE_ROOT_CLR(i)                 (0x8038 + (128 * i))
#define IMX7_CCM_PRE_ROOT_TOG(i)                 (0x803C + (128 * i))

#define IMX7_CCM_PRE_ACCESS_CTRL(i)              (0x8070 + (128 * i)) /* Access Control Register 0..120 */
#define IMX7_CCM_PRE_ACCESS_CTRL_SET(i)          (0x8074 + (128 * i))
#define IMX7_CCM_PRE_ACCESS_CTRL_CLR(i)          (0x8078 + (128 * i))
#define IMX7_CCM_PRE_ACCESS_CTRL_TOG(i)          (0x807C + (128 * i))

/*
 * CCM PLL Control Register bits
 */
#define IMX7_CCM_PLL_CTRL_SETTING3_MASK          (3<<12)     /* Clock gate control setting for domain 3 */
#define IMX7_CCM_PLL_CTRL_SETTING3_SHIFT         12
#define IMX7_CCM_PLL_CTRL_SETTING2_MASK          (3<<8)      /* Clock gate control setting for domain 2 */
#define IMX7_CCM_PLL_CTRL_SETTING2_SHIFT         8
#define IMX7_CCM_PLL_CTRL_SETTING1_MASK          (3<<4)      /* Clock gate control setting for domain 1 */
#define IMX7_CCM_PLL_CTRL_SETTING1_SHIFT          4
#define IMX7_CCM_PLL_CTRL_SETTING0_MASK          (3<<0)      /* Clock gate control setting for domain 0 */
#define IMX7_CCM_PLL_CTRL_SETTING0_SHIFT         0
#define IMX7_CCM_PLL_CTRL_RESET                  0x00003333  /* Domains 0 - 3 clocks needed all the time */

/*
 * CCM Clock Gating Register bits
 */
#define IMX7_CCM_CCGR_SETTING3_MASK              (3<<12)     /* Clock gate control setting for domain 3 */
#define IMX7_CCM_CCGR_SETTING3_SHIFT             12
#define IMX7_CCM_CCGR_SETTING2_MASK              (3<<8)      /* Clock gate control setting for domain 2 */
#define IMX7_CCM_CCGR_SETTING2_SHIFT             8
#define IMX7_CCM_CCGR_SETTING1_MASK              (3<<4)      /* Clock gate control setting for domain 1 */
#define IMX7_CCM_CCGR_SETTING1_SHIFT             4
#define IMX7_CCM_CCGR_SETTING0_MASK              (3<<0)      /* Clock gate control setting for domain 0 */
#define IMX7_CCM_CCGR_SETTING0_SHIFT             0
#define IMX7_CCM_CCGRx_DISABLE                   0x00000000  /* Domains 0 - 3 clocks not needed */
#define IMX7_CCM_CCGRx_ENABLE                    0x00003333  /* Domains 0 - 3 clocks needed all the time */

/* CCGR device clock gate mapping */
#define IMX7_CCM_CCGR_CPU                        0
#define IMX7_CCM_CCGR_M4                         1
#define IMX7_CCM_CCGR_SIM_MAIN                   4
#define IMX7_CCM_CCGR_SIM_DISPLAY                5
#define IMX7_CCM_CCGR_SIM_ENET                   6
#define IMX7_CCM_CCGR_SIM_M                      7
#define IMX7_CCM_CCGR_SIM_S                      8
#define IMX7_CCM_CCGR_SIM_WAKEUP                 9
#define IMX7_CCM_CCGR_IPMUX1                     10
#define IMX7_CCM_CCGR_IPMUX2                     11
#define IMX7_CCM_CCGR_IPMUX3                     12
#define IMX7_CCM_CCGR_ROM                        16
#define IMX7_CCM_CCGR_OCRAM                      17
#define IMX7_CCM_CCGR_OCRAM_S                    18
#define IMX7_CCM_CCGR_DRAM                       19
#define IMX7_CCM_CCGR_RAWNAND                    20
#define IMX7_CCM_CCGR_QSPI                       21
#define IMX7_CCM_CCGR_WEIM                       22
#define IMX7_CCM_CCGR_ADC                        32
#define IMX7_CCM_CCGR_ANATOP                     33
#define IMX7_CCM_CCGR_SCTR                       34
#define IMX7_CCM_CCGR_OCOTP                      35
#define IMX7_CCM_CCGR_CAAM                       36
#define IMX7_CCM_CCGR_SNVS                       37
#define IMX7_CCM_CCGR_RDC                        38
#define IMX7_CCM_CCGR_MU                         39
#define IMX7_CCM_CCGR_HS                         40
#define IMX7_CCM_CCGR_DVFS                       41
#define IMX7_CCM_CCGR_QOS                        42
#define IMX7_CCM_CCGR_QOS_DISPMIX                43
#define IMX7_CCM_CCGR_QOS_MEGAMIX                44
#define IMX7_CCM_CCGR_CSU                        45
#define IMX7_CCM_CCGR_DBGMON                     46
#define IMX7_CCM_IMX7_CCM_CCGR_DEBUG              47
#define IMX7_CCM_CCGR_TRACE                      48
#define IMX7_CCM_CCGR_SEC_DEBUG                  49
#define IMX7_CCM_CCGR_SEMA1                      64
#define IMX7_CCM_CCGR_SEMA2                      65
#define IMX7_CCM_CCGR_PERFMON1                   68
#define IMX7_CCM_CCGR_PERFMON2                   69
#define IMX7_CCM_CCGR_SDMA                       72
#define IMX7_CCM_CCGR_CSI                        73
#define IMX7_CCM_CCGR_EPDC                       74
#define IMX7_CCM_CCGR_LCDIF                      75
#define IMX7_CCM_CCGR_PXP                        76
#define IMX7_CCM_CCGR_PCIE                       96
#define IMX7_CCM_CCGR_MIPI_CSI                   100
#define IMX7_CCM_CCGR_MIPI_DSI                   101
#define IMX7_CCM_CCGR_MIPI_MEM_PHY               102
#define IMX7_CCM_CCGR_USB_CTRL                   104
#define IMX7_CCM_CCGR_USB_HSIC                   105
#define IMX7_CCM_CCGR_USB_PHY1                   106
#define IMX7_CCM_CCGR_USB_PHY2                   107
#define IMX7_CCM_CCGR_USDHC1                     108
#define IMX7_CCM_CCGR_USDHC2                     109
#define IMX7_CCM_CCGR_USDHC3                     110
#define IMX7_CCM_CCGR_ENET1                      112
#define IMX7_CCM_CCGR_ENET2                      113
#define IMX7_CCM_CCGR_CAN1                       116
#define IMX7_CCM_CCGR_CAN2                       117
#define IMX7_CCM_CCGR_ECSPI1                     120
#define IMX7_CCM_CCGR_ECSPI2                     121
#define IMX7_CCM_CCGR_ECSPI3                     122
#define IMX7_CCM_CCGR_ECSPI4                     123
#define IMX7_CCM_CCGR_GPT1                       124
#define IMX7_CCM_CCGR_GPT2                       125
#define IMX7_CCM_CCGR_GPT3                       126
#define IMX7_CCM_CCGR_GPT4                       127
#define IMX7_CCM_CCGR_FTM1                       128
#define IMX7_CCM_CCGR_FTM2                       129
#define IMX7_CCM_CCGR_PWM1                       132
#define IMX7_CCM_CCGR_PWM2                       133
#define IMX7_CCM_CCGR_PWM3                       134
#define IMX7_CCM_CCGR_PWM4                       135
#define IMX7_CCM_CCGR_I2C1                       136
#define IMX7_CCM_CCGR_I2C2                       137
#define IMX7_CCM_CCGR_I2C3                       138
#define IMX7_CCM_CCGR_I2C4                       139
#define IMX7_CCM_CCGR_SAI1                       140
#define IMX7_CCM_CCGR_SAI2                       141
#define IMX7_CCM_CCGR_SAI3                       142
#define IMX7_CCM_CCGR_SIM1                       144
#define IMX7_CCM_CCGR_SIM2                       145
#define IMX7_CCM_CCGR_UART1                      148
#define IMX7_CCM_CCGR_UART2                      149
#define IMX7_CCM_CCGR_UART3                      150
#define IMX7_CCM_CCGR_UART4                      151
#define IMX7_CCM_CCGR_UART5                      152
#define IMX7_CCM_CCGR_UART6                      153
#define IMX7_CCM_CCGR_UART7                      154
#define IMX7_CCM_CCGR_WDOG1                      156
#define IMX7_CCM_CCGR_WDOG2                      157
#define IMX7_CCM_CCGR_WDOG3                      158
#define IMX7_CCM_CCGR_WDOG4                      159
#define IMX7_CCM_CCGR_GPIO1                      160
#define IMX7_CCM_CCGR_GPIO2                      161
#define IMX7_CCM_CCGR_GPIO3                      162
#define IMX7_CCM_CCGR_GPIO4                      163
#define IMX7_CCM_CCGR_GPIO5                      164
#define IMX7_CCM_CCGR_GPIO6                      165
#define IMX7_CCM_CCGR_GPIO7                      166
#define IMX7_CCM_CCGR_IOMUX                      168
#define IMX7_CCM_CCGR_IOMUX_LPSR                 169
#define IMX7_CCM_CCGR_KPP                        170

/*
 * CCM Target Register bits
 */
#define IMX7_CCM_TARGET_ROOT_ENABLE              (1<<28)   /* Enable/disable this clock */
#define IMX7_CCM_TARGET_ROOT_ENABLE_SHIFT        28
#define IMX7_CCM_TARGET_ROOT_MUX_SHIFT           24
#define IMX7_CCM_TARGET_ROOT_MUX_MASK            (7<<24)
#define IMX7_CCM_TARGET_ROOT_PRE_PODF_SHIFT      16
#define IMX7_CCM_TARGET_ROOT_PRE_PODF_MASK       (7<<16)
#define IMX7_CCM_TARGET_ROOT_PRE_PODF_DIV_1      (0<<16)   /* Pre divider divide by 1 */
#define IMX7_CCM_TARGET_ROOT_PRE_PODF_DIV_2      (1<<16)   /* Pre divider divide by 2 */
#define IMX7_CCM_TARGET_ROOT_PRE_PODF_DIV_3      (2<<16)   /* Pre divider divide by 3 */
#define IMX7_CCM_TARGET_ROOT_PRE_PODF_DIV_4      (3<<16)   /* Pre divider divide by 4 */
#define IMX7_CCM_TARGET_ROOT_PRE_PODF_DIV_5      (4<<16)   /* Pre divider divide by 5 */
#define IMX7_CCM_TARGET_ROOT_PRE_PODF_DIV_6      (5<<16)   /* Pre divider divide by 6 */
#define IMX7_CCM_TARGET_ROOT_PRE_PODF_DIV_7      (6<<16)   /* Pre divider divide by 7 */
#define IMX7_CCM_TARGET_ROOT_PRE_PODF_DIV_8      (7<<16)   /* Pre divider divide by 8 */
#define IMX7_CCM_TARGET_ROOT_POST_PODF_SHIFT     0
#define IMX7_CCM_TARGET_ROOT_POST_PODF_MASK      (0x3F<<0)
#define IMX7_CCM_TARGET_ROOT_POST_PODF_DIV_1     (0<<0)   /* Post divider divide by 1 */
#define IMX7_CCM_TARGET_ROOT_POST_PODF_DIV_2     (1<<0)   /* Post divider divide by 2 */
#define IMX7_CCM_TARGET_ROOT_POST_PODF_DIV_3     (2<<0)   /* Post divider divide by 3 */
#define IMX7_CCM_TARGET_ROOT_POST_PODF_DIV_4     (3<<0)   /* Post divider divide by 4 */
#define IMX7_CCM_TARGET_ROOT_POST_PODF_DIV_5     (4<<0)   /* Post divider divide by 5 */
#define IMX7_CCM_TARGET_ROOT_POST_PODF_DIV_6     (5<<0)   /* Post divider divide by 6 */
#define IMX7_CCM_TARGET_ROOT_POST_PODF_DIV_7     (6<<0)   /* Post divider divide by 7 */
#define IMX7_CCM_TARGET_ROOT_POST_PODF_DIV_8     (7<<0)   /* Post divider divide by 8 */
#define IMX7_CCM_TARGET_ROOT_POST_PODF_DIV_9     (8<<0)   /* Post divider divide by 9 */
#define IMX7_CCM_TARGET_ROOT_POST_PODF_DIV_10    (9<<0)   /* Post divider divide by 10 */


/* Clock device root index */
#define IMX7_CCM_TARGET_ROOT_MUX_ARM_A7_CLK          0
#define IMX7_CCM_TARGET_ROOT_MUX_ARM_M4_CLK          1
#define IMX7_CCM_TARGET_ROOT_MUX_ARM_M0_CLK          2
#define IMX7_CCM_TARGET_ROOT_MUX_MAIN_AXI_CLK        16
#define IMX7_CCM_TARGET_ROOT_MUX_DISP_AXI_CLK        17
#define IMX7_CCM_TARGET_ROOT_MUX_ENET_AXI_CLK        18
#define IMX7_CCM_TARGET_ROOT_MUX_NAND_USDHC_BUS_CLK  19
#define IMX7_CCM_TARGET_ROOT_MUX_AHB_CLK             32
#define IMX7_CCM_TARGET_ROOT_MUX_DRAM_PHYM_CLK       48
#define IMX7_CCM_TARGET_ROOT_MUX_DRAM_CLK            49
#define IMX7_CCM_TARGET_ROOT_MUX_DRAM_PHYM_ALT_CLK   64
#define IMX7_CCM_TARGET_ROOT_MUX_DRAM_ALT_CLK        65
#define IMX7_CCM_TARGET_ROOT_MUX_USB_HSIC_CLK        66
#define IMX7_CCM_TARGET_ROOT_MUX_PCIE_CTRL_CLK       67
#define IMX7_CCM_TARGET_ROOT_MUX_PCIE_PHY_CLK        68
#define IMX7_CCM_TARGET_ROOT_MUX_EPDC_PIXEL_CLK      69
#define IMX7_CCM_TARGET_ROOT_MUX_LCDIF_PIXEL_CLK     70
#define IMX7_CCM_TARGET_ROOT_MUX_MIPI_DSI_EXTSER_CLK 71
#define IMX7_CCM_TARGET_ROOT_MUX_MIPI_CSI_WARP_CLK   72
#define IMX7_CCM_TARGET_ROOT_MUX_MIPI_DPHY_REF_CLK   73
#define IMX7_CCM_TARGET_ROOT_MUX_SAI1_CLK            74
#define IMX7_CCM_TARGET_ROOT_MUX_SAI2_CLK            75
#define IMX7_CCM_TARGET_ROOT_MUX_SAI3_CLK            76
#define IMX7_CCM_TARGET_ROOT_MUX_SPDIF_CLK           77
#define IMX7_CCM_TARGET_ROOT_MUX_ENET1_REF_CLK       78
#define IMX7_CCM_TARGET_ROOT_MUX_ENET1_TIME_CLK      79
#define IMX7_CCM_TARGET_ROOT_MUX_ENET2_REF_CLK       80
#define IMX7_CCM_TARGET_ROOT_MUX_ENET2_TIME_CLK      81
#define IMX7_CCM_TARGET_ROOT_MUX_ENET_PHY_REF_CLK    82
#define IMX7_CCM_TARGET_ROOT_MUX_EIM_CLK             83
#define IMX7_CCM_TARGET_ROOT_MUX_NAND_CLK            84
#define IMX7_CCM_TARGET_ROOT_MUX_QSPI_CLK            85
#define IMX7_CCM_TARGET_ROOT_MUX_USDHC1_CLK          86
#define IMX7_CCM_TARGET_ROOT_MUX_USDHC2_CLK          87
#define IMX7_CCM_TARGET_ROOT_MUX_USDHC3_CLK          88
#define IMX7_CCM_TARGET_ROOT_MUX_CAN1_CLK            89
#define IMX7_CCM_TARGET_ROOT_MUX_CAN2_CLK            90
#define IMX7_CCM_TARGET_ROOT_MUX_I2C1_CLK            91
#define IMX7_CCM_TARGET_ROOT_MUX_I2C2_CLK            92
#define IMX7_CCM_TARGET_ROOT_MUX_I2C3_CLK            93
#define IMX7_CCM_TARGET_ROOT_MUX_I2C4_CLK            94
#define IMX7_CCM_TARGET_ROOT_MUX_UART1_CLK           95
#define IMX7_CCM_TARGET_ROOT_MUX_UART2_CLK           96
#define IMX7_CCM_TARGET_ROOT_MUX_UART3_CLK           97
#define IMX7_CCM_TARGET_ROOT_MUX_UART4_CLK           98
#define IMX7_CCM_TARGET_ROOT_MUX_UART5_CLK           99
#define IMX7_CCM_TARGET_ROOT_MUX_UART6_CLK           100
#define IMX7_CCM_TARGET_ROOT_MUX_UART7_CLK           101
#define IMX7_CCM_TARGET_ROOT_MUX_ECSPI1_CLK          102
#define IMX7_CCM_TARGET_ROOT_MUX_ECSPI2_CLK          103
#define IMX7_CCM_TARGET_ROOT_MUX_ECSPI3_CLK          104
#define IMX7_CCM_TARGET_ROOT_MUX_ECSPI4_CLK          105
#define IMX7_CCM_TARGET_ROOT_MUX_PWM1_CLK            106
#define IMX7_CCM_TARGET_ROOT_MUX_PWM2_CLK            107
#define IMX7_CCM_TARGET_ROOT_MUX_PWM3_CLK            108
#define IMX7_CCM_TARGET_ROOT_MUX_PWM4_CLK            109
#define IMX7_CCM_TARGET_ROOT_MUX_FLEXTIMER1_CLK      110
#define IMX7_CCM_TARGET_ROOT_MUX_FLEXTIMER2_CLK      111
#define IMX7_CCM_TARGET_ROOT_MUX_SIM1_CLK            112
#define IMX7_CCM_TARGET_ROOT_MUX_SIM2_CLK            113
#define IMX7_CCM_TARGET_ROOT_MUX_GPT1_CLK            114
#define IMX7_CCM_TARGET_ROOT_MUX_GPT2_CLK            115
#define IMX7_CCM_TARGET_ROOT_MUX_GPT3_CLK            116
#define IMX7_CCM_TARGET_ROOT_MUX_GPT4_CLK            117
#define IMX7_CCM_TARGET_ROOT_MUX_TRACE_CLK           118
#define IMX7_CCM_TARGET_ROOT_MUX_WDOG_CLK            119
#define IMX7_CCM_TARGET_ROOT_MUX_CSI_MCLK_CLK        120
#define IMX7_CCM_TARGET_ROOT_MUX_AUDIO_MCLK_CLK      121
#define IMX7_CCM_TARGET_ROOT_MUX_CCM_CLKO1_CLK       123
#define IMX7_CCM_TARGET_ROOT_MUX_CCM_CLKO2_CLK       124


/*
 * CCM Miscellaneous Register bits
 */
#define IMX7_CCM_MISC_ROOT_VIOLATE               (1<IMX7_<8)    /* This sticky bit reflects access violation in normal interface of this clock */
#define IMX7_CCM_MISC_ROOT_VIOLATE_SHIFT         8
#define IMX7_CCM_MISC_ROOT_TIMEOUT               (1<<4)    /* This sticky bit reflects time out happened during accessing this clock */
#define IMX7_CCM_MISC_ROOT_TIMEOUT_SHIFT         4
#define IMX7_CCM_MISC_ROOT_AUTHEN_FAIL           (1<<0)    /* This sticky bit reflects access restricted by access control of this clock */
#define IMX7_CCM_MISC_ROOT_AUTHEN_FAIL_SHIFT     0

/*
 * CCM Post Divider Register bits
 */
#define IMX7_CCM_POST_ROOT_BUSY2                 (1<<31)   /* Safe multiplexer is applying new setting */
#define IMX7_CCM_POST_ROOT_BUSY2_SHIFT           31
#define IMX7_CCM_POST_ROOT_SELECT                (1<<28)   /* Selection of pre clock branches */
#define IMX7_CCM_POST_ROOT_SELECT_SHIFT          28
#define IMX7_CCM_POST_ROOT_BUSY1                 (1<<7)    /* Post divider is applying new set value */
#define IMX7_CCM_POST_ROOT_BUSY1_SHIFT           7
#define IMX7_CCM_POST_ROOT_POST_PODF_SHIFT       0
#define IMX7_CCM_POST_ROOT_POST_PODF_MASK        (0x3F<<0)
#define IMX7_CCM_POST_ROOT_POST_PODF_DIV_1       (0<<0)   /* Post divider divide by 1 */
#define IMX7_CCM_POST_ROOT_POST_PODF_DIV_2       (1<<0)   /* Post divider divide by 2 */
#define IMX7_CCM_POST_ROOT_POST_PODF_DIV_3       (2<<0)   /* Post divider divide by 3 */
#define IMX7_CCM_POST_ROOT_POST_PODF_DIV_4       (3<<0)   /* Post divider divide by 4 */
#define IMX7_CCM_POST_ROOT_POST_PODF_DIV_5       (4<<0)   /* Post divider divide by 5 */
#define IMX7_CCM_POST_ROOT_POST_PODF_DIV_6       (5<<0)   /* Post divider divide by 6 */
#define IMX7_CCM_POST_ROOT_POST_PODF_DIV_7       (6<<0)   /* Post divider divide by 7 */
#define IMX7_CCM_POST_ROOT_POST_PODF_DIV_8       (7<<0)   /* Post divider divide by 8 */
#define IMX7_CCM_POST_ROOT_POST_PODF_DIV_9       (8<<0)   /* Post divider divide by 9 */
#define IMX7_CCM_POST_ROOT_POST_PODF_DIV_10      (9<<0)   /* Post divider divide by 10 */

/*
 * CCM Pre Divider Register bits
 */
#define IMX7_CCM_PRE_ROOT_BUSY4                  (1<<31)   /* EN_A field is applied to field */
#define IMX7_CCM_PRE_ROOT_BUSY4_SHIFT            31
#define IMX7_CCM_PRE_ROOT_EN_A                   (1<<28)   /* Branch A clock gate control */
#define IMX7_CCM_PRE_ROOT_EN_A_SHIFT             28
#define IMX7_CCM_PRE_ROOT_MUX_A_MASK             (7<<24)   /* Selection control of multiplexer of branch A */
#define IMX7_CCM_PRE_ROOT_MUX_A_SHIFT            24
#define IMX7_CCM_PRE_ROOT_BUSY3                  (1<<19)   /* Pre divider value for branch A is applied */
#define IMX7_CCM_PRE_ROOT_BUSY3_SHIFT            19
#define IMX7_CCM_PRE_ROOT_PRE_PODF_A_SHIFT       16
#define IMX7_CCM_PRE_ROOT_PRE_PODF_A_MASK        (0x7<<16)
#define IMX7_CCM_PRE_ROOT_PRE_PODF_A_DIV_1       (0<<16)   /* Pre divider for branch A - divide by 1 */
#define IMX7_CCM_PRE_ROOT_PRE_PODF_A_DIV_2       (1<<16)   /* Pre divider for branch A - divide by 2 */
#define IMX7_CCM_PRE_ROOT_PRE_PODF_A_DIV_3       (2<<16)   /* Pre divider for branch A - divide by 3 */
#define IMX7_CCM_PRE_ROOT_PRE_PODF_A_DIV_4       (3<<16)   /* Pre divider for branch A - divide by 4 */
#define IMX7_CCM_PRE_ROOT_PRE_PODF_A_DIV_5       (4<<16)   /* Pre divider for branch A - divide by 5 */
#define IMX7_CCM_PRE_ROOT_PRE_PODF_A_DIV_6       (5<<16)   /* Pre divider for branch A - divide by 6 */
#define IMX7_CCM_PRE_ROOT_PRE_PODF_A_DIV_7       (6<<16)   /* Pre divider for branch A - divide by 7 */
#define IMX7_CCM_PRE_ROOT_PRE_PODF_A_DIV_8       (7<<16)   /* Pre divider for branch A - divide by 8 */
#define IMX7_CCM_PRE_ROOT_BUSY1                  (1<<15)   /* EN_B is applied to field */
#define IMX7_CCM_PRE_ROOT_BUSY1_SHIFT            15
#define IMX7_CCM_PRE_ROOT_EN_B                   (1<<12)   /* Branch B clock gate control */
#define IMX7_CCM_PRE_ROOT_EN_B_SHIFT             12
#define IMX7_CCM_PRE_ROOT_MUX_B_MASK             (7<<8)    /* Selection control of multiplexer of branch B */
#define IMX7_CCM_PRE_ROOT_MUX_B_SHIFT            8
#define IMX7_CCM_PRE_ROOT_BUSY0                  (1<<3)    /* Pre divider value for branch a is applying */
#define IMX7_CCM_PRE_ROOT_BUSY0_SHIFT            3
#define IMX7_CCM_PRE_ROOT_PRE_PODF_B_SHIFT       16
#define IMX7_CCM_PRE_ROOT_PRE_PODF_B_MASK        (0x7<<0)
#define IMX7_CCM_PRE_ROOT_PRE_PODF_B_DIV_1       (0<<0)    /* Pre divider for branch B - divide by 1 */
#define IMX7_CCM_PRE_ROOT_PRE_PODF_B_DIV_2       (1<<0)    /* Pre divider for branch B - divide by 2 */
#define IMX7_CCM_PRE_ROOT_PRE_PODF_B_DIV_3       (2<<0)    /* Pre divider for branch B - divide by 3 */
#define IMX7_CCM_PRE_ROOT_PRE_PODF_B_DIV_4       (3<<0)    /* Pre divider for branch B - divide by 4 */
#define IMX7_CCM_PRE_ROOT_PRE_PODF_B_DIV_5       (4<<0)    /* Pre divider for branch B - divide by 5 */
#define IMX7_CCM_PRE_ROOT_PRE_PODF_B_DIV_6       (5<<0)    /* Pre divider for branch B - divide by 6 */
#define IMX7_CCM_PRE_ROOT_PRE_PODF_B_DIV_7       (6<<0)    /* Pre divider for branch B - divide by 7 */
#define IMX7_CCM_PRE_ROOT_PRE_PODF_B_DIV_8       (7<<0)    /* Pre divider for branch B - divide by 8 */

/*
 * CCM Access Control Register bits
 */
#define IMX7_CCM_ACCESS_ROOT_LOCK                     (1<<31) /* Lock this clock root to use access control */
#define IMX7_CCM_ACCESS_ROOT_LOCK_SHIFT               31
#define IMX7_CCM_ACCESS_ROOT_SEMA_EN                  (1<<28) /* Enable internal semaphore */
#define IMX7_CCM_ACCESS_ROOT_SEMA_EN_SHIFT            28
#define IMX7_CCM_ACCESS_ROOT_DOMAIN3_WHITELIST        (1<<27) /* White list of domains that can change setting of this clock root */
#define IMX7_CCM_ACCESS_ROOT_DOMAIN3_WHITELIST_SHIFT  27
#define IMX7_CCM_ACCESS_ROOT_DOMAIN2_WHITELIST        (1<<26) /* White list of domains that can change setting of this clock root */
#define IMX7_CCM_ACCESS_ROOT_DOMAIN2_WHITELIST_SHIFT  26
#define IMX7_CCM_ACCESS_ROOT_DOMAIN1_WHITELIST        (1<<25) /* White list of domains that can change setting of this clock root */
#define IMX7_CCM_ACCESS_ROOT_DOMAIN1_WHITELIST_SHIFT  25
#define IMX7_CCM_ACCESS_ROOT_DOMAIN0_WHITELIST        (1<<24) /* White list of domains that can change setting of this clock root */
#define IMX7_CCM_ACCESS_ROOT_DOMAIN0_WHITELIST_SHIFT  24
#define IMX7_CCM_ACCESS_ROOT_MUTEX                    (1<<20) /* Semaphore to control access */
#define IMX7_CCM_ACCESS_ROOT_MUTEX_SHIFT              20
#define IMX7_CCM_ACCESS_ROOT_OWNER_ID_SHIFT           16
#define IMX7_CCM_ACCESS_ROOT_OWNER_ID_MASK            (0x3<<16)
#define IMX7_CCM_ACCESS_ROOT_OWNER_ID_DOMAIN_0        (0x0<<16) /* Domain 0 */
#define IMX7_CCM_ACCESS_ROOT_OWNER_ID_DOMAIN_1        (0x1<<16) /* Domain 1 */
#define IMX7_CCM_ACCESS_ROOT_OWNER_ID_DOMAIN_2        (0x2<<16) /* Domain 2 */
#define IMX7_CCM_ACCESS_ROOT_OWNER_ID_DOMAIN_3        (0x3<<16) /* Domain 3 */
#define IMX7_CCM_ACCESS_ROOT_DOMAIN3_INFO_SHIFT       12
#define IMX7_CCM_ACCESS_ROOT_DOMAIN3_INFO_MASK        (0xF<<12)
#define IMX7_CCM_ACCESS_ROOT_DOMAIN2_INFO_SHIFT       8
#define IMX7_CCM_ACCESS_ROOT_DOMAIN2_INFO_MASK        (0xF<<8)
#define IMX7_CCM_ACCESS_ROOT_DOMAIN1_INFO_SHIFT       4
#define IMX7_CCM_ACCESS_ROOT_DOMAIN1_INFO_MASK        (0xF<<4)
#define IMX7_CCM_ACCESS_ROOT_DOMAIN0_INFO_SHIFT       0
#define IMX7_CCM_ACCESS_ROOT_DOMAIN0_INFO_MASK        (0xF<<0)

/*
 * CCM_ANALOG
 * Used for configuring PLLs, PFDs
 */
#define IMX7_CCM_ANALOG_BASE                     0x30360000
#define IMX7_CCM_ANALOG_SIZE                     0x10000

/* CCM_ANALOG Registers, offset from base address */
#define IMX7_CCM_ANALOG_PLL_ARM                  0x60               /* Anadig ARM PLL control Register */
#define IMX7_CCM_ANALOG_PLL_ARM_SET              0x64
#define IMX7_CCM_ANALOG_PLL_ARM_CLR              0x68
#define IMX7_CCM_ANALOG_PLL_ARM_TOG              0x6C
#define IMX7_CCM_ANALOG_PLL_DDR                  0x70               /* Anadig DDR PLL control Register */
#define IMX7_CCM_ANALOG_PLL_DDR_SET              0x74
#define IMX7_CCM_ANALOG_PLL_DDR_CLR              0x78
#define IMX7_CCM_ANALOG_PLL_DDR_TOG              0x7C
#define IMX7_CCM_ANALOG_PLL_DDR_SS               0x80               /* Anadig DDR Spread Spectrum Register */
#define IMX7_CCM_ANALOG_PLL_DDR_NUM              0x90               /* Numerator of DDR PLL Fraction Loop Divider Register */
#define IMX7_CCM_ANALOG_PLL_DDR_DENON            0xA0               /* Denominator of DDR PLL Fraction Loop Divider Register */
#define IMX7_CCM_ANALOG_PLL_480                  0xB0               /* Anadig 480MHz PLL Control Register */
#define IMX7_CCM_ANALOG_PLL_480_SET              0xB4
#define IMX7_CCM_ANALOG_PLL_480_CLR              0xB8
#define IMX7_CCM_ANALOG_PLL_480_TOG              0xBC
#define IMX7_CCM_ANALOG_PFD_480A                 0xC0               /* 480MHz Clock Phase Fractional Divider Control Register A */
#define IMX7_CCM_ANALOG_PFD_480A_SET             0xC4
#define IMX7_CCM_ANALOG_PFD_480A_CLR             0xC8
#define IMX7_CCM_ANALOG_PFD_480A_TOG             0xCC
#define IMX7_CCM_ANALOG_PFD_480B                 0xD0               /* 480MHz Clock Phase Fractional Divider Control Register B */
#define IMX7_CCM_ANALOG_PFD_480B_SET             0xD4
#define IMX7_CCM_ANALOG_PFD_480B_CLR             0xD8
#define IMX7_CCM_ANALOG_PFD_480B_TOG             0xDC
#define IMX7_CCM_ANALOG_PLL_ENET                 0xE0               /* Anadig ENET PLL Control Register */
#define IMX7_CCM_ANALOG_PLL_ENET_SET             0xE4
#define IMX7_CCM_ANALOG_PLL_ENET_CLR             0xE8
#define IMX7_CCM_ANALOG_PLL_ENET_TOG             0xEC
#define IMX7_CCM_ANALOG_PLL_AUDIO                0xF0               /* Anadig Audio PLL control Register */
#define IMX7_CCM_ANALOG_PLL_AUDIO_SET            0xF4
#define IMX7_CCM_ANALOG_PLL_AUDIO_CLR            0xF8
#define IMX7_CCM_ANALOG_PLL_AUDIO_TOG            0xFC
#define IMX7_CCM_ANALOG_PLL_AUDIO_SS             0x100              /* Audio PLL Spread Spectrum Register */
#define IMX7_CCM_ANALOG_PLL_AUDIO_NUM            0x110              /* Numerator of Audio PLL Fractional Loop Divider Register */
#define IMX7_CCM_ANALOG_PLL_AUDIO_DENOM          0x120              /* Denominator of Audio PLL Fractional Loop Divider Register */
#define IMX7_CCM_ANALOG_PLL_VIDEO                0x130              /* Anadig Video PLL control Register */
#define IMX7_CCM_ANALOG_PLL_VIDEO_SET            0x134
#define IMX7_CCM_ANALOG_PLL_VIDEO_CLR            0x138
#define IMX7_CCM_ANALOG_PLL_VIDEO_TOG            0x13C
#define IMX7_CCM_ANALOG_PLL_VIDEO_SS             0x140              /* Video PLL Spread Spectrum Register */
#define IMX7_CCM_ANALOG_PLL_VIDEO_NUM            0x150              /* Numerator of Video PLL Fractional Loop Divider Register */
#define IMX7_CCM_ANALOG_PLL_VIDEO_DENOM          0x160              /* Denominator of Video PLL Fractional Loop Divider Register */
#define IMX7_CCM_ANALOG_CLK_MISC0                0x170              /* Miscellaneous0 Analog Clock Control and Status Register */
#define IMX7_CCM_ANALOG_CLK_MISC0_SET            0x174
#define IMX7_CCM_ANALOG_CLK_MISC0_CLR            0x178
#define IMX7_CCM_ANALOG_CLK_MISC0_TOG            0x17C


/*
 * CCM Anadig ARM PLL control Register bits
 */
#define IMX7_CCM_ANALOG_PLL_ARM_LOCK                   (1<<31)     /* Clock gate control setting for domain 3 */
#define IMX7_CCM_ANALOG_PLL_ARM_LOCK_SHIFT             31
#define IMX7_CCM_ANALOG_PLL_ARM_PLL_ARM_OVERRIDE       (1<<20)
#define IMX7_CCM_ANALOG_PLL_ARM_PLL_ARM_OVERRIDE_SHIFT 20
#define IMX7_CCM_ANALOG_PLL_ARM_PLL_SEL                (1<<19)
#define IMX7_CCM_ANALOG_PLL_ARM_PLL_SEL_SHIFT          19
#define IMX7_CCM_ANALOG_PLL_ARM_LVDS_24MHZ_SEL         (1<<18)
#define IMX7_CCM_ANALOG_PLL_ARM_LVDS_24MHZ_SEL_SHIFT   18
#define IMX7_CCM_ANALOG_PLL_ARM_LVDS_SEL               (1<<17)
#define IMX7_CCM_ANALOG_PLL_ARM_LVDS_SEL_SHIFT         17
#define IMX7_CCM_ANALOG_PLL_ARM_BYPASS                 (1<<16)
#define IMX7_CCM_ANALOG_PLL_ARM_BYPASS_SHIFT           16          /* Bypass the PLL */
#define IMX7_CCM_ANALOG_PLL_ARM_BYPASS_CLK_SRC_MASK    (3<<14)     /* Determines the bypass source */
#define IMX7_CCM_ANALOG_PLL_ARM_BYPASS_CLK_SRC_SHIFT   14
#define IMX7_CCM_ANALOG_PLL_ARM_ENABLE_CLK             (1<<13)     /* Enable the clock output */
#define IMX7_CCM_ANALOG_PLL_ARM_ENABLE_CLK_SHIFT       13
#define IMX7_CCM_ANALOG_PLL_ARM_POWERDOWN              (1<<12)     /* Powers down the PLL */
#define IMX7_CCM_ANALOG_PLL_ARM_POWERDOWN_SHIFT        12
#define IMX7_CCM_ANALOG_PLL_ARM_HOLD_RING_OFF          (1<<11)     /* Analog debug bit */
#define IMX7_CCM_ANALOG_PLL_ARM_HOLD_RING_OFF_SHIFT    11
#define IMX7_CCM_ANALOG_PLL_ARM_DOUBLE_CP              (1<<10)     /* Increases the charge pump gain 2x */
#define IMX7_CCM_ANALOG_PLL_ARM_DOUBLE_CP_SHIFT        10
#define IMX7_CCM_ANALOG_PLL_ARM_HALF_CP                (1<<9)      /* Reduces the charge pump gain 2x */
#define IMX7_CCM_ANALOG_PLL_ARM_HALF_CP_SHIFT          9
#define IMX7_CCM_ANALOG_PLL_ARM_DOUBLE_LF              (1<<8)      /* Increases the frequency of the loop filter 2x */
#define IMX7_CCM_ANALOG_PLL_ARM_DOUBLE_LF_SHIFT        8
#define IMX7_CCM_ANALOG_PLL_ARM_HALF_LF                (1<<7)      /* Reduces the frequency of the loop filter 2x */
#define IMX7_CCM_ANALOG_PLL_ARM_HALF_LF_SHIFT          7
#define IMX7_CCM_ANALOG_PLL_ARM_DIV_SELECT_MASK        (0x7F<<0)   /* PLL loop divider: 54-108, Fout = Fin * div_select/2 */
#define IMX7_CCM_ANALOG_PLL_ARM_DIV_SELECT_SHIFT       0


/*
 * CCM Anadig DDR PLL Control Register bits
 */
#define IMX7_CCM_ANALOG_PLL_DDR_LOCK                   (1<<31)     /* 1 - PLL is currently locked; 0 - PLL is not currently locked */
#define IMX7_CCM_ANALOG_PLL_DDR_LOCK_SHIFT             31
#define IMX7_CCM_ANALOG_PLL_DDR_TEST_DIV_SELECT_MASK   (0x03<<21)  /* Post divider for the PLL clk and lvds outputs */
#define IMX7_CCM_ANALOG_PLL_DDR_TEST_DIV_SELECT_SHIFT  21
#define IMX7_CCM_ANALOG_PLL_DDR_POWERDOWN              (1<<20)     /* Powers down the PLL */
#define IMX7_CCM_ANALOG_PLL_DDR_POWERDOWN_SHIFT        20
#define IMX7_CCM_ANALOG_PLL_DDR_PLL_DDR_OVERRIDE       (1<<19)
#define IMX7_CCM_ANALOG_PLL_DDR_PLL_DDR_OVERRIDE_SHIFT 19
#define IMX7_CCM_ANALOG_PLL_DDR_PFD_OFFSET_EN          (1<<18)     /* Enables an offset in the phase frequency detector */
#define IMX7_CCM_ANALOG_PLL_DDR_PFD_OFFSET_EN_SHIFT    18
#define IMX7_CCM_ANALOG_PLL_DDR_DITHER_ENABLE          (1<<17)     /* Enables dither in the fractional modulator calculation */
#define IMX7_CCM_ANALOG_PLL_DDR_DITHER_ENABLE_SHIFT    17
#define IMX7_CCM_ANALOG_PLL_DDR_BYPASS                 (1<<16)     /* Bypass the PLL */
#define IMX7_CCM_ANALOG_PLL_DDR_BYPASS_SHIFT           16
#define IMX7_CCM_ANALOG_PLL_DDR_BYPASS_CLK_SRC_MASK    (0x03<<14)  /* Determines the bypass source */
#define IMX7_CCM_ANALOG_PLL_DDR_BYPASS_CLK_SRC_SHIFT   14
#define IMX7_CCM_ANALOG_PLL_DDR_ENABLE_CLK             (1<<13)
#define IMX7_CCM_ANALOG_PLL_DDR_ENABLE_CLK_SHIFT       13
#define IMX7_CCM_ANALOG_PLL_DDR_DIV2_ENABLE_CLK        (1<<12)
#define IMX7_CCM_ANALOG_PLL_DDR_DIV2_ENABLE_CLK_SHIFT  12
#define IMX7_CCM_ANALOG_PLL_DDR_HOLD_RING_OFF          (1<<11)     /* Status of ana_irq2 input from analog block */
#define IMX7_CCM_ANALOG_PLL_DDR_HOLD_RING_OFF_SHIFT    11
#define IMX7_CCM_ANALOG_PLL_DDR_DOUBLE_CP              (1<<10)     /* Increases the charge pump gain 2x */
#define IMX7_CCM_ANALOG_PLL_DDR_DOUBLE_CP_SHIFT        10
#define IMX7_CCM_ANALOG_PLL_DDR_HALF_CP                (1<<9)      /* Reduces the charge pump gain 2x */
#define IMX7_CCM_ANALOG_PLL_DDR_HALF_CP_SHIFT          9
#define IMX7_CCM_ANALOG_PLL_DDR_DOUBLE_LF              (1<<8)      /* Increases the frequency of the loop filter 2x */
#define IMX7_CCM_ANALOG_PLL_DDR_DOUBLE_LF_SHIFT        8
#define IMX7_CCM_ANALOG_PLL_DDR_HALF_LF                (1<<7)      /* Reduces the frequency of the loop filter 2x */
#define IMX7_CCM_ANALOG_PLL_DDR_HALF_LF_SHIFT          7
#define IMX7_CCM_ANALOG_PLL_DDR_DIV_SELECT_MASK        (0x7F<<0)   /* The PLL loop divider, valid values for divider: 33 to 44 */
#define IMX7_CCM_ANALOG_PLL_DDR_DIV_SELECT_SHIF        0

/*
 * CCM DDR PLL Spread Spectrum Register bits
 */
#define IMX7_CCM_ANALOG_PLL_DDR_SS_STOP_MASK           (0xFFFF<<16) /* Frequency change = step/B*24MHz */
#define IMX7_CCM_ANALOG_PLL_DDR_SS_STOP_SHIFT          16
#define IMX7_CCM_ANALOG_PLL_DDR_SS_ENABLE              (1<<15)      /* This bit enables the spread spectrum modulation */
#define IMX7_CCM_ANALOG_PLL_DDR_SS_ENABLE_SHIFT        15
#define IMX7_CCM_ANALOG_PLL_DDR_SS_STEP_MASK           (0x7FFF<<0)  /* The max frequency change = stop/B*24MHz */
#define IMX7_CCM_ANALOG_PLL_DDR_SS_STEP_SHIFT          0

/*
 * CCM Numerator of DDR PLL Fractional Loop Divider Register bits
 */
#define IMX7_CCM_ANALOG_PLL_DDR_NUM_A_MASK             (0x7FFFFFFF<<0) /* 30 bit numerator (A) of fractional loop divider (signed integer) */
#define IMX7_CCM_ANALOG_PLL_DDR_NUM_A_SHIFT            0

/*
 * CCM Denominator of DDR PLL Fractional Loop Divider Register bits
 */
#define IMX7_CCM_ANALOG_PLL_DDR_DENOM_B_MASK           (0x7FFFFFFF<<0) /* 30 bit Denominator (B) of fractional loop divider (signed integer) */
#define IMX7_CCM_ANALOG_PLL_DDR_DENOM_B_SHIFT          0


#endif /* IMX7_CCM_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/arm/imx7/imx7_ccm.h $ $Rev: 834524 $")
#endif
