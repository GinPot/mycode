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


#ifndef __ARM_MX6UL_IOMUX_H_INCLUDED
#define __ARM_MX6UL_IOMUX_H_INCLUDED


/*
 * Bit definitions for SW_MUX_CTL registers
 */
#define MUX_CTL_SION                        (0x1 << 4)
#define MUX_CTL_MUX_MODE_ALT0                0
#define MUX_CTL_MUX_MODE_ALT1                1
#define MUX_CTL_MUX_MODE_ALT2                2
#define MUX_CTL_MUX_MODE_ALT3                3
#define MUX_CTL_MUX_MODE_ALT4                4
#define MUX_CTL_MUX_MODE_ALT5                5
#define MUX_CTL_MUX_MODE_ALT6                6
#define MUX_CTL_MUX_MODE_ALT7                7


/*
 * Bit definitions for SW_PAD_CTL registers
 * Note that the various pad registers do not all have identical bit settings,
 * for example the Speed field only exists in certain pad registers.
 */

#define PAD_CTL_HYS_DISABLE             (0x0 << 16)
#define PAD_CTL_HYS_ENABLE              (0x1 << 16)
#define PAD_CTL_HYS_MASK                (0x1 << 16)

#define PAD_CTL_PUS_100K_PD             (0x0 << 14)
#define PAD_CTL_PUS_47K_PU              (0x1 << 14)
#define PAD_CTL_PUS_100K_PU             (0x2 << 14)
#define PAD_CTL_PUS_22K_PU              (0x3 << 14)
#define PAD_CTL_PUS_MASK                (0x3 << 14)

#define PAD_CTL_PUE_PULL                (0x1 << 13)
#define PAD_CTL_PUE_KEEP                (0x0 << 13)
#define PAD_CTL_PUE_MASK                (0x1 << 13)

#define PAD_CTL_PKE_DISABLE             (0x0 << 12)
#define PAD_CTL_PKE_ENABLE              (0x1 << 12)
#define PAD_CTL_PKE_MASK                (0x1 << 12)

#define PAD_CTL_ODE_DISABLE             (0x0 << 11)
#define PAD_CTL_ODE_ENABLE              (0x1 << 11)
#define PAD_CTL_ODE_MASK                (0x1 << 11)

#define PAD_CTL_ODT
#define PAD_CTL_SPEED_TBD               (0x0 << 6)
#define PAD_CTL_SPEED_LOW               (0x1 << 6)
#define PAD_CTL_SPEED_MEDIUM            (0x2 << 6)
#define PAD_CTL_SPEED_MAX               (0x3 << 6)
#define PAD_CTL_SPEED_MASK              (0x3 << 6)

#define PAD_CTL_DSE_0                   (0x0 << 3)
#define PAD_CTL_DSE_1                   (0x1 << 3)
#define PAD_CTL_DSE_2                   (0x2 << 3)
#define PAD_CTL_DSE_3                   (0x3 << 3)
#define PAD_CTL_DSE_4                   (0x4 << 3)
#define PAD_CTL_DSE_5                   (0x5 << 3)
#define PAD_CTL_DSE_6                   (0x6 << 3)
#define PAD_CTL_DSE_7                   (0x7 << 3)
#define PAD_CTL_DSE_MASK                (0x7 << 3)

/* 3.3V NVCC_GPIO GPIO DSE Names */
#define PAD_CTL_DSE_HIZ                 PAD_CTL_DSE_0
#define PAD_CTL_DSE_260_OHM             PAD_CTL_DSE_1
#define PAD_CTL_DSE_130_OHM             PAD_CTL_DSE_2
#define PAD_CTL_DSE_87_OHM              PAD_CTL_DSE_3
#define PAD_CTL_DSE_65_OHM              PAD_CTL_DSE_4
#define PAD_CTL_DSE_52_OHM              PAD_CTL_DSE_5
#define PAD_CTL_DSE_43_OHM              PAD_CTL_DSE_6
#define PAD_CTL_DSE_37_OHM              PAD_CTL_DSE_7

/* 1.8V NVCC_GPIO GPIO DSE */
#define PAD_CTL_DSE_1V8_150_OHM         PAD_CTL_DSE_1
#define PAD_CTL_DSE_1V8_75_OHM          PAD_CTL_DSE_2
#define PAD_CTL_DSE_1V8_50_OHM          PAD_CTL_DSE_3
#define PAD_CTL_DSE_1V8_38_OHM          PAD_CTL_DSE_4
#define PAD_CTL_DSE_1V8_30_OHM          PAD_CTL_DSE_5
#define PAD_CTL_DSE_1V8_25_OHM          PAD_CTL_DSE_6
#define PAD_CTL_DSE_1V8_21_OHM          PAD_CTL_DSE_7

/* DDR GPIO DSE */
#define PAD_CTL_DSE_DDR_240_OHM         PAD_CTL_DSE_1
#define PAD_CTL_DSE_DDR_120_OHM         PAD_CTL_DSE_2
#define PAD_CTL_DSE_DDR_80_OHM          PAD_CTL_DSE_3
#define PAD_CTL_DSE_DDR_60_OHM          PAD_CTL_DSE_4
#define PAD_CTL_DSE_DDR_48_OHM          PAD_CTL_DSE_5
#define PAD_CTL_DSE_DDR_40_OHM          PAD_CTL_DSE_6
#define PAD_CTL_DSE_DDR_34_OHM          PAD_CTL_DSE_7

#define PAD_CTL_SRE_SLOW                (0x0 << 0)
#define PAD_CTL_SRE_FAST                (0x1 << 0)
#define PAD_CTL_SRE_MASK                (0x1 << 0)

/*
 * offsets of IOMUXC registers from MX6UL_IOMUX_SWMUX
 * where MX6UL_IOMUX_SWMUX = MX6UL_IOMUXC_BASE + 0x0014
 */
#define SWMUX_UL_BOOT_MODE0                                   0
#define SWMUX_UL_BOOT_MODE1                                   1
#define SWMUX_UL_SNVS_TAMPER0                                 2
#define SWMUX_UL_SNVS_TAMPER1                                 3
#define SWMUX_UL_SNVS_TAMPER2                                 4
#define SWMUX_UL_SNVS_TAMPER3                                 5
#define SWMUX_UL_SNVS_TAMPER4                                 6
#define SWMUX_UL_SNVS_TAMPER5                                 7
#define SWMUX_UL_SNVS_TAMPER6                                 8
#define SWMUX_UL_SNVS_TAMPER7                                 9
#define SWMUX_UL_SNVS_TAMPER8                                 10
#define SWMUX_UL_SNVS_TAMPER9                                 11
#define SWMUX_UL_JTAG_MOD                                     12
#define SWMUX_UL_JTAG_TMS                                     13
#define SWMUX_UL_JTAG_TD0                                     14
#define SWMUX_UL_JTAG_TDI                                     15
#define SWMUX_UL_JTAG_TCK                                     16
#define SWMUX_UL_JTAG_TRST_B                                  17
#define SWMUX_UL_GPIO1_IO00                                   18
#define SWMUX_UL_GPIO1_IO01                                   19
#define SWMUX_UL_GPIO1_IO02                                   20
#define SWMUX_UL_GPIO1_IO03                                   21
#define SWMUX_UL_GPIO1_IO04                                   22
#define SWMUX_UL_GPIO1_IO05                                   23
#define SWMUX_UL_GPIO1_IO06                                   24
#define SWMUX_UL_GPIO1_IO07                                   25
#define SWMUX_UL_GPIO1_IO08                                   26
#define SWMUX_UL_GPIO1_IO09                                   27
#define SWMUX_UL_UART1_TX_DATA                                28
#define SWMUX_UL_UART1_RX_DATA                                29
#define SWMUX_UL_UART1_CTS_B                                  30
#define SWMUX_UL_UART1_RTS_B                                  31
#define SWMUX_UL_UART2_TX_DATA                                32
#define SWMUX_UL_UART2_RX_DATA                                33
#define SWMUX_UL_UART2_RX_CTS_B                               34
#define SWMUX_UL_UART2_RX_RTS_B                               35
#define SWMUX_UL_UART3_TX_DATA                                36
#define SWMUX_UL_UART3_RX_DATA                                37
#define SWMUX_UL_UART3_RX_CTS_B                               38
#define SWMUX_UL_UART3_RX_RTS_B                               39
#define SWMUX_UL_UART4_TX_DATA                                40
#define SWMUX_UL_UART4_RX_DATA                                41
#define SWMUX_UL_UART5_TX_DATA                                42
#define SWMUX_UL_UART5_RX_DATA                                43
#define SWMUX_UL_ENET1_RX_DATA0                               44
#define SWMUX_UL_ENET1_RX_DATA1                               45
#define SWMUX_UL_ENET1_RX_EN                                  46
#define SWMUX_UL_ENET1_TX_DATA0                               47
#define SWMUX_UL_ENET1_TX_DATA1                               48
#define SWMUX_UL_ENET1_TX_EN                                  49
#define SWMUX_UL_ENET1_TX_CLK                                 50
#define SWMUX_UL_ENET1_RX_ER                                  51
#define SWMUX_UL_ENET2_RX_DATA0                               52
#define SWMUX_UL_ENET2_RX_DATA1                               53
#define SWMUX_UL_ENET2_RX_EN                                  54
#define SWMUX_UL_ENET2_TX_DATA0                               55
#define SWMUX_UL_ENET2_TX_DATA1                               56
#define SWMUX_UL_ENET2_TX_EN                                  57
#define SWMUX_UL_ENET2_TX_CLK                                 58
#define SWMUX_UL_ENET2_RX_ER                                  59
#define SWMUX_UL_LCD_CLK                                      60
#define SWMUX_UL_LCD_ENABLE                                   61
#define SWMUX_UL_LCD_HSYNC                                    62
#define SWMUX_UL_LCD_VSYNC                                    63
#define SWMUX_UL_LCD_RESET                                    64
#define SWMUX_UL_LCD_DATA00                                   65
#define SWMUX_UL_LCD_DATA01                                   66
#define SWMUX_UL_LCD_DATA02                                   67
#define SWMUX_UL_LCD_DATA03                                   68
#define SWMUX_UL_LCD_DATA04                                   69
#define SWMUX_UL_LCD_DATA05                                   70
#define SWMUX_UL_LCD_DATA06                                   71
#define SWMUX_UL_LCD_DATA07                                   72
#define SWMUX_UL_LCD_DATA08                                   73
#define SWMUX_UL_LCD_DATA09                                   74
#define SWMUX_UL_LCD_DATA10                                   75
#define SWMUX_UL_LCD_DATA11                                   76
#define SWMUX_UL_LCD_DATA12                                   77
#define SWMUX_UL_LCD_DATA13                                   78
#define SWMUX_UL_LCD_DATA14                                   79
#define SWMUX_UL_LCD_DATA15                                   80
#define SWMUX_UL_LCD_DATA16                                   81
#define SWMUX_UL_LCD_DATA17                                   82
#define SWMUX_UL_LCD_DATA18                                   83
#define SWMUX_UL_LCD_DATA19                                   84
#define SWMUX_UL_LCD_DATA20                                   85
#define SWMUX_UL_LCD_DATA21                                   86
#define SWMUX_UL_LCD_DATA22                                   87
#define SWMUX_UL_LCD_DATA23                                   88
#define SWMUX_UL_NAND_RE_B                                    89
#define SWMUX_UL_NAND_WE_B                                    90
#define SWMUX_UL_NAND_DATA00                                  91
#define SWMUX_UL_NAND_DATA01                                  92
#define SWMUX_UL_NAND_DATA02                                  93
#define SWMUX_UL_NAND_DATA03                                  94
#define SWMUX_UL_NAND_DATA04                                  95
#define SWMUX_UL_NAND_DATA05                                  96
#define SWMUX_UL_NAND_DATA06                                  97
#define SWMUX_UL_NAND_DATA07                                  98
#define SWMUX_UL_NAND_ALE                                     99
#define SWMUX_UL_NAND_WP_B                                    100
#define SWMUX_UL_NAND_READY_B                                 101
#define SWMUX_UL_NAND_CE0_B                                   102
#define SWMUX_UL_NAND_CE1_B                                   103
#define SWMUX_UL_NAND_CLE                                     104
#define SWMUX_UL_NAND_DQS                                     105
#define SWMUX_UL_SD1_CMD                                      106
#define SWMUX_UL_SD1_CLK                                      107
#define SWMUX_UL_SD1_DATA0                                    108
#define SWMUX_UL_SD1_DATA1                                    109
#define SWMUX_UL_SD1_DATA2                                    110
#define SWMUX_UL_SD1_DATA3                                    111
#define SWMUX_UL_CSI_MCLK                                     112
#define SWMUX_UL_CSI_PIXCLK                                   113
#define SWMUX_UL_CSI_VSYNC                                    114
#define SWMUX_UL_CSI_HSYNC                                    115
#define SWMUX_UL_CSI_DATA00                                   116
#define SWMUX_UL_CSI_DATA01                                   117
#define SWMUX_UL_CSI_DATA02                                   118
#define SWMUX_UL_CSI_DATA03                                   119
#define SWMUX_UL_CSI_DATA04                                   120
#define SWMUX_UL_CSI_DATA05                                   121
#define SWMUX_UL_CSI_DATA06                                   122
#define SWMUX_UL_CSI_DATA07                                   123
/*
 * offsets of IOMUXC registers from MX6UL_IOMUX_SWPAD
 * where MX6UL_IOMUX_SWPAD = MX6UL_IOMUXC_BASE + 0x0204
 */
#define SWPAD_UL_DRAM_ADDR00                                  0
#define SWPAD_UL_DRAM_ADDR01                                  1
#define SWPAD_UL_DRAM_ADDR02                                  2
#define SWPAD_UL_DRAM_ADDR03                                  3
#define SWPAD_UL_DRAM_ADDR04                                  4
#define SWPAD_UL_DRAM_ADDR05                                  5
#define SWPAD_UL_DRAM_ADDR06                                  6
#define SWPAD_UL_DRAM_ADDR07                                  7
#define SWPAD_UL_DRAM_ADDR08                                  8
#define SWPAD_UL_DRAM_ADDR09                                  9
#define SWPAD_UL_DRAM_ADDR10                                  10
#define SWPAD_UL_DRAM_ADDR11                                  11
#define SWPAD_UL_DRAM_ADDR12                                  12
#define SWPAD_UL_DRAM_ADDR13                                  13
#define SWPAD_UL_DRAM_ADDR14                                  14
#define SWPAD_UL_DRAM_ADDR15                                  15
#define SWPAD_UL_DRAM_DQM0                                    16
#define SWPAD_UL_DRAM_DQM1                                    17
#define SWPAD_UL_DRAM_RAS_B                                   18
#define SWPAD_UL_DRAM_CAS_B                                   19
#define SWPAD_UL_DRAM_CS0_B                                   20
#define SWPAD_UL_DRAM_CS1_B                                   21
#define SWPAD_UL_DRAM_SDWE_B                                  22
#define SWPAD_UL_DRAM_ODT0                                    23
#define SWPAD_UL_DRAM_ODT1                                    24
#define SWPAD_UL_DRAM_SDBA0                                   25
#define SWPAD_UL_DRAM_SDBA1                                   26
#define SWPAD_UL_DRAM_SDBA2                                   27
#define SWPAD_UL_DRAM_SDCKE0                                  28
#define SWPAD_UL_DRAM_SDCKE1                                  29
#define SWPAD_UL_DRAM_SDCLK0_P                                30
#define SWPAD_UL_DRAM_SDQS0_P                                 31
#define SWPAD_UL_DRAM_SDQS1_P                                 32
#define SWPAD_UL_DRAM_RESET                                   33
#define SWPAD_UL_TEST_MODE                                    34
#define SWPAD_UL_POR_B                                        35
#define SWPAD_UL_ONOFF                                        36
#define SWPAD_UL_SNVS_PMIC_ON_REQ                             37
#define SWPAD_UL_CCM_PMIC_STBY_REQ                            38
#define SWPAD_UL_BOOT_MODE0                                   39
#define SWPAD_UL_BOOT_MODE1                                   40
#define SWPAD_UL_SNVS_TAMPER0                                 41
#define SWPAD_UL_SNVS_TAMPER1                                 42
#define SWPAD_UL_SNVS_TAMPER2                                 43
#define SWPAD_UL_SNVS_TAMPER3                                 44
#define SWPAD_UL_SNVS_TAMPER4                                 45
#define SWPAD_UL_SNVS_TAMPER5                                 46
#define SWPAD_UL_SNVS_TAMPER6                                 47
#define SWPAD_UL_SNVS_TAMPER7                                 48
#define SWPAD_UL_SNVS_TAMPER8                                 49
#define SWPAD_UL_SNVS_TAMPER9                                 50
#define SWPAD_UL_JTAG_MOD                                     51
#define SWPAD_UL_JTAG_TMS                                     52
#define SWPAD_UL_JTAG_TDO                                     53
#define SWPAD_UL_JTAG_TDI                                     54
#define SWPAD_UL_JTAG_TCK                                     55
#define SWPAD_UL_JTAG_TRST_B                                  56
#define SWPAD_UL_GPIO1_IO00                                   57
#define SWPAD_UL_GPIO1_IO01                                   58
#define SWPAD_UL_GPIO1_IO02                                   59
#define SWPAD_UL_GPIO1_IO03                                   60
#define SWPAD_UL_GPIO1_IO04                                   61
#define SWPAD_UL_GPIO1_IO05                                   62
#define SWPAD_UL_GPIO1_IO06                                   63
#define SWPAD_UL_GPIO1_IO07                                   64
#define SWPAD_UL_GPIO1_IO08                                   65
#define SWPAD_UL_GPIO1_IO09                                   66
#define SWPAD_UL_UART1_TX_DATA                                67
#define SWPAD_UL_UART1_RX_DATA                                68
#define SWPAD_UL_UART1_CTS_B                                  69
#define SWPAD_UL_UART1_RTS_B                                  70
#define SWPAD_UL_UART2_TX_DATA                                71
#define SWPAD_UL_UART2_RX_DATA                                72
#define SWPAD_UL_UART2_CTS_B                                  73
#define SWPAD_UL_UART2_RTS_B                                  74
#define SWPAD_UL_UART3_TX_DATA                                75
#define SWPAD_UL_UART3_RX_DATA                                76
#define SWPAD_UL_UART3_CTS_B                                  77
#define SWPAD_UL_UART3_RTS_B                                  78
#define SWPAD_UL_UART4_TX_DATA                                79
#define SWPAD_UL_UART4_RX_DATA                                80
#define SWPAD_UL_UART5_TX_DATA                                81
#define SWPAD_UL_UART5_RX_DATA                                82
#define SWPAD_UL_ENET1_RX_DATA0                               83
#define SWPAD_UL_ENET1_RX_DATA1                               84
#define SWPAD_UL_ENET1_RX_EN                                  85
#define SWPAD_UL_ENET1_TX_DATA0                               86
#define SWPAD_UL_ENET1_TX_DATA1                               87
#define SWPAD_UL_ENET1_TX_EN                                  88
#define SWPAD_UL_ENET1_TX_CLK                                 89
#define SWPAD_UL_ENET1_RX_ER                                  90
#define SWPAD_UL_ENET2_RX_DATA0                               91
#define SWPAD_UL_ENET2_RX_DATA1                               92
#define SWPAD_UL_ENET2_RX_EN                                  93
#define SWPAD_UL_ENET2_TX_DATA0                               94
#define SWPAD_UL_ENET2_TX_DATA1                               95
#define SWPAD_UL_ENET2_TX_EN                                  96
#define SWPAD_UL_ENET2_TX_CLK                                 97
#define SWPAD_UL_ENET2_RX_ER                                  98
#define SWPAD_UL_LCD_CLK                                      99
#define SWPAD_UL_LCD_ENABLE                                   100
#define SWPAD_UL_LCD_HSYNC                                    101
#define SWPAD_UL_LCD_VSYNC                                    102
#define SWPAD_UL_LCD_RESET                                    103
#define SWPAD_UL_LCD_DATA00                                   104
#define SWPAD_UL_LCD_DATA01                                   105
#define SWPAD_UL_LCD_DATA02                                   106
#define SWPAD_UL_LCD_DATA03                                   107
#define SWPAD_UL_LCD_DATA04                                   108
#define SWPAD_UL_LCD_DATA05                                   109
#define SWPAD_UL_LCD_DATA06                                   110
#define SWPAD_UL_LCD_DATA07                                   111
#define SWPAD_UL_LCD_DATA08                                   112
#define SWPAD_UL_LCD_DATA09                                   113
#define SWPAD_UL_LCD_DATA10                                   114
#define SWPAD_UL_LCD_DATA11                                   115
#define SWPAD_UL_LCD_DATA12                                   116
#define SWPAD_UL_LCD_DATA13                                   117
#define SWPAD_UL_LCD_DATA14                                   118
#define SWPAD_UL_LCD_DATA15                                   119
#define SWPAD_UL_LCD_DATA16                                   120
#define SWPAD_UL_LCD_DATA17                                   121
#define SWPAD_UL_LCD_DATA18                                   122
#define SWPAD_UL_LCD_DATA19                                   123
#define SWPAD_UL_LCD_DATA20                                   124
#define SWPAD_UL_LCD_DATA21                                   125
#define SWPAD_UL_LCD_DATA22                                   126
#define SWPAD_UL_LCD_DATA23                                   127
#define SWPAD_UL_NAND_RE_B                                    128
#define SWPAD_UL_NAND_WE_B                                    129
#define SWPAD_UL_NAND_DATA00                                  130
#define SWPAD_UL_NAND_DATA01                                  131
#define SWPAD_UL_NAND_DATA02                                  132
#define SWPAD_UL_NAND_DATA03                                  133
#define SWPAD_UL_NAND_DATA04                                  134
#define SWPAD_UL_NAND_DATA05                                  135
#define SWPAD_UL_NAND_DATA06                                  136
#define SWPAD_UL_NAND_DATA07                                  137
#define SWPAD_UL_NAND_ALE                                     138
#define SWPAD_UL_NAND_WP_B                                    139
#define SWPAD_UL_NAND_READY_B                                 140
#define SWPAD_UL_NAND_CE0_B                                   141
#define SWPAD_UL_NAND_CE1_B                                   142
#define SWPAD_UL_NAND_CLE                                     143
#define SWPAD_UL_NAND_DQS                                     144
#define SWPAD_UL_SD1_CMD                                      145
#define SWPAD_UL_SD1_CLK                                      146
#define SWPAD_UL_SD1_DATA0                                    147
#define SWPAD_UL_SD1_DATA1                                    148
#define SWPAD_UL_SD1_DATA2                                    149
#define SWPAD_UL_SD1_DATA3                                    150
#define SWPAD_UL_CSI_MCLK                                     151
#define SWPAD_UL_CSI_PIXCLK                                   152
#define SWPAD_UL_CSI_VSYNC                                    153
#define SWPAD_UL_CSI_HSYNC                                    154
#define SWPAD_UL_CSI_DATA00                                   155
#define SWPAD_UL_CSI_DATA01                                   156
#define SWPAD_UL_CSI_DATA02                                   157
#define SWPAD_UL_CSI_DATA03                                   158
#define SWPAD_UL_CSI_DATA04                                   159
#define SWPAD_UL_CSI_DATA05                                   160
#define SWPAD_UL_CSI_DATA06                                   161
#define SWPAD_UL_CSI_DATA07                                   162

/*
 * offsets of IOMUXC registers from MX6UL_IOMUX_INPUT
 * where MX6UL_IOMUX_INPUT = MX6UL_IOMUXC_BASE + 0x04B8
 */
#define SWINPUT_UL_USB_OTG1_ID                                0
#define SWINPUT_UL_USB_OTG2_ID                                1
#define SWINPUT_UL_PMIC_READY                                 2
#define SWINPUT_UL_CSI_DATA02                                 3
#define SWINPUT_UL_CSI_DATA03                                 4
#define SWINPUT_UL_CSI_DATA05                                 5
#define SWINPUT_UL_CSI_DATA00                                 6
#define SWINPUT_UL_CSI_DATA01                                 7
#define SWINPUT_UL_CSI_DATA04                                 8
#define SWINPUT_UL_CSI_DATA06                                 9
#define SWINPUT_UL_CSI_DATA07                                 10
#define SWINPUT_UL_CSI_DATA08                                 11
#define SWINPUT_UL_CSI_DATA09                                 12
#define SWINPUT_UL_CSI_DATA10                                 13
#define SWINPUT_UL_CSI_DATA11                                 14
#define SWINPUT_UL_CSI_DATA12                                 15
#define SWINPUT_UL_CSI_DATA13                                 16
#define SWINPUT_UL_CSI_DATA14                                 17
#define SWINPUT_UL_CSI_DATA15                                 18
#define SWINPUT_UL_CSI_DATA16                                 19
#define SWINPUT_UL_CSI_DATA17                                 20
#define SWINPUT_UL_CSI_DATA18                                 21
#define SWINPUT_UL_CSI_DATA19                                 22
#define SWINPUT_UL_CSI_DATA20                                 23
#define SWINPUT_UL_CSI_DATA21                                 24
#define SWINPUT_UL_CSI_DATA22                                 25
#define SWINPUT_UL_CSI_DATA23                                 26
#define SWINPUT_UL_CSI_HSYNC                                  27
#define SWINPUT_UL_CSI_PIXCLK                                 28
#define SWINPUT_UL_CSI_VSYNC                                  29
#define SWINPUT_UL_CSI_FIELD                                  30
#define SWINPUT_UL_ECSPI1_SCLK                                31
#define SWINPUT_UL_ECSPI1_MISO                                32
#define SWINPUT_UL_ECSPI1_M0SI                                33
#define SWINPUT_UL_ECSPI1_SS0_B                               34
#define SWINPUT_UL_ECSPI2_SCLK                                35
#define SWINPUT_UL_ECSPI2_MISO                                36
#define SWINPUT_UL_ECSPI2_M0SI                                37
#define SWINPUT_UL_ECSPI2_SS0_B                               38
#define SWINPUT_UL_ECSPI3_SCLK                                39
#define SWINPUT_UL_ECSPI3_MISO                                40
#define SWINPUT_UL_ECSPI3_M0SI                                41
#define SWINPUT_UL_ECSPI3_SS0_B                               42
#define SWINPUT_UL_ECSPI4_SCLK                                43
#define SWINPUT_UL_ECSPI4_MISO                                44
#define SWINPUT_UL_ECSPI4_M0SI                                45
#define SWINPUT_UL_ECSPI4_SS0_B                               46
#define SWINPUT_UL_ENET1_REF_CLK1                             47
#define SWINPUT_UL_ENET1_MAC0_MDIO                            48
#define SWINPUT_UL_ENET2_REF_CLK2                             49
#define SWINPUT_UL_ENET2_MAC0_MDIO                            50
#define SWINPUT_UL_FLEXCAN1_RX                                51
#define SWINPUT_UL_FLEXCAN2_RX                                52
#define SWINPUT_UL_GPT1_CAPTURE1                              53
#define SWINPUT_UL_GPT1_CAPTURE2                              54
#define SWINPUT_UL_GPT1_CLK                                   55
#define SWINPUT_UL_GPT2_CAPTURE1                              56
#define SWINPUT_UL_GPT2_CAPTURE2                              57
#define SWINPUT_UL_GPT2_CLK                                   58
#define SWINPUT_UL_I2C1_SCL                                   59
#define SWINPUT_UL_I2C1_SDA                                   60
#define SWINPUT_UL_I2C2_SCL                                   61
#define SWINPUT_UL_I2C2_SDA                                   62
#define SWINPUT_UL_I2C3_SCL                                   63
#define SWINPUT_UL_I2C3_SDA                                   64
#define SWINPUT_UL_I2C4_SCL                                   65
#define SWINPUT_UL_I2C4_SDA                                   66
#define SWINPUT_UL_KPP_COL0                                   67
#define SWINPUT_UL_KPP_COL1                                   68
#define SWINPUT_UL_KPP_COL2                                   69
#define SWINPUT_UL_KPP_ROW0                                   70
#define SWINPUT_UL_KPP_ROW1                                   71
#define SWINPUT_UL_KPP_ROW2                                   72
#define SWINPUT_UL_LCD_BUSY                                   73
#define SWINPUT_UL_SAI1_MCLK                                  74
#define SWINPUT_UL_SAI1_RX_DATA                               75
#define SWINPUT_UL_SAI1_TX_BCLK                               76
#define SWINPUT_UL_SAI1_TX_SYNC                               77
#define SWINPUT_UL_SAI2_MCLK                                  78
#define SWINPUT_UL_SAI2_RX_DATA                               79
#define SWINPUT_UL_SAI2_TX_BCLK                               80
#define SWINPUT_UL_SAI2_TX_SYNC                               81
#define SWINPUT_UL_SAI3_MCLK                                  82
#define SWINPUT_UL_SAI3_RX_DATA                               83
#define SWINPUT_UL_SAI3_TX_BCLK                               84
#define SWINPUT_UL_SAI3_TX_SYNC                               85
#define SWINPUT_UL_SDMA_EVENT0                                86
#define SWINPUT_UL_SDMA_EVENT1                                87
#define SWINPUT_UL_SPDIF_IN                                   88
#define SWINPUT_UL_SPDIF_EXT                                  89
#define SWINPUT_UL_UART1_RTS                                  90
#define SWINPUT_UL_UART1_RX_DATA                              91
#define SWINPUT_UL_UART2_RTS                                  92
#define SWINPUT_UL_UART2_RX_DATA                              93
#define SWINPUT_UL_UART3_RTS                                  94
#define SWINPUT_UL_UART3_RX_DATA                              95
#define SWINPUT_UL_UART4_RTS                                  96
#define SWINPUT_UL_UART4_RX_DATA                              97
#define SWINPUT_UL_UART5_RTS                                  98
#define SWINPUT_UL_UART5_RX_DATA                              99
#define SWINPUT_UL_UART6_RTS                                  100
#define SWINPUT_UL_UART6_RX_DATA                              101
#define SWINPUT_UL_UART7_RTS                                  102
#define SWINPUT_UL_UART7_RX_DATA                              103
#define SWINPUT_UL_UART8_RTS                                  104
#define SWINPUT_UL_UART8_RX_DATA                              105
#define SWINPUT_UL_USB_OTG2_OC                                106
#define SWINPUT_UL_USB_OTG1_OC                                107
#define SWINPUT_UL_USDHC1_CD_B                                108
#define SWINPUT_UL_USDHC1_WP                                  109
#define SWINPUT_UL_USDHC2_CLK                                 110
#define SWINPUT_UL_USDHC2_CD_B                                111
#define SWINPUT_UL_USDHC2_CMD                                 112
#define SWINPUT_UL_USDHC2_DATA0                               113
#define SWINPUT_UL_USDHC2_DATA1                               114
#define SWINPUT_UL_USDHC2_DATA2                               115
#define SWINPUT_UL_USDHC2_DATA3                               116
#define SWINPUT_UL_USDHC2_DATA4                               117
#define SWINPUT_UL_USDHC2_DATA5                               118
#define SWINPUT_UL_USDHC2_DATA6                               119
#define SWINPUT_UL_USDHC2_DATA7                               120
#define SWINPUT_UL_USDHC2_WP                                  121

/*
 * Function prototypes
 */
void pinmux_set_swmux(int pin, int mux_config);
void pinmux_set_padcfg(int pin, int pad_config);
void pinmux_set_input(int pin, int input_config);


#endif  /* __ARM_MX6UL_IOMUX_H_INCLUDED */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/arm/mx6ul_iomux.h $ $Rev: 847677 $")
#endif
