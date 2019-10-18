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

#ifndef __ARM_MX7X_IOMUX_H_INCLUDED
#define __ARM_MX7X_IOMUX_H_INCLUDED

/*
 * There are two IOMUXes in this SOC, IOMUX and IOMUX_LPSR
 */
#define IMX7_IOMUX                                          0x00000000
#define IMX7_IOMUX_LPSR                                     0x80000000

/*
 * Bit definitions for SW_MUX_CTL registers
 */

#define MUX_CTL_SION                                       (0x1 << 4)
#define MUX_CTL_MUX_MODE_ALT0                              0
#define MUX_CTL_MUX_MODE_ALT1                              1
#define MUX_CTL_MUX_MODE_ALT2                              2
#define MUX_CTL_MUX_MODE_ALT3                              3
#define MUX_CTL_MUX_MODE_ALT4                              4
#define MUX_CTL_MUX_MODE_ALT5                              5
#define MUX_CTL_MUX_MODE_ALT6                              6
#define MUX_CTL_MUX_MODE_ALT7                              7
#define MUX_CTL_MUX_MODE_ALT8                              8
#define MUX_CTL_MUX_MODE_ALT9                              9

/*
 * Bit definitions for SW_PAD_CTL registers
 * Note that the various pad registers do not all have identical bit settings,
 * for example the Speed field only exists in certain pad registers.
 */

/* Pull Select */
#define PAD_CTL_PUS_100K_PD                                (0x0 << 5)
#define PAD_CTL_PUS_5K_PU                                  (0x1 << 5)
#define PAD_CTL_PUS_47K_PU                                 (0x2 << 5)
#define PAD_CTL_PUS_100K_PU                                (0x3 << 5)
#define PAD_CTL_PUS_MASK                                   (0x3 << 5)

/* Pull Enable */
#define PAD_CTL_PE_DISABLE                                 (0x0 << 4)
#define PAD_CTL_PE_ENABLE                                  (0x1 << 4)
#define PAD_CTL_PE_MASK                                    (0x1 << 4)

/* Hyst. Enable */
#define PAD_CTL_HYS_DISABLE                                (0x0 << 3)
#define PAD_CTL_HYS_ENABLE                                 (0x1 << 3)
#define PAD_CTL_HYS_MASK                                   (0x1 << 3)

/* Slew Rate */
#define PAD_CTL_SRE_FAST                                   (0x0 << 2)
#define PAD_CTL_SRE_SLOW                                   (0x1 << 2)
#define PAD_CTL_SRE_MASK                                   (0x1 << 2)

/* Drive strength */
#define PAD_CTL_DSE_X1                                     (0x0)
#define PAD_CTL_DSE_X4                                     (0x1)
#define PAD_CTL_DSE_X2                                     (0x2)
#define PAD_CTL_DSE_X6                                     (0x3)

/*
 * offsets of IOMUXC registers from IMX7_IOMUX_SWMUX
 * where IMX7_IOMUX_SWMUX = IMX7_IOMUXC_BASE + 0x0014
 */

#define SWMUX_GPIO1_IO08                                   0
#define SWMUX_GPIO1_IO09                                   1
#define SWMUX_GPIO1_IO10                                   2
#define SWMUX_GPIO1_IO11                                   3
#define SWMUX_GPIO1_IO12                                   4
#define SWMUX_GPIO1_IO13                                   5
#define SWMUX_GPIO1_IO14                                   6
#define SWMUX_GPIO1_IO15                                   7
#define SWMUX_EPDC_DATA00                                  8
#define SWMUX_EPDC_DATA01                                  9
#define SWMUX_EPDC_DATA02                                  10
#define SWMUX_EPDC_DATA03                                  11
#define SWMUX_EPDC_DATA04                                  12
#define SWMUX_EPDC_DATA05                                  13
#define SWMUX_EPDC_DATA06                                  14
#define SWMUX_EPDC_DATA07                                  15
#define SWMUX_EPDC_DATA08                                  16
#define SWMUX_EPDC_DATA09                                  17
#define SWMUX_EPDC_DATA10                                  18
#define SWMUX_EPDC_DATA11                                  19
#define SWMUX_EPDC_DATA12                                  20
#define SWMUX_EPDC_DATA13                                  21
#define SWMUX_EPDC_DATA14                                  22
#define SWMUX_EPDC_DATA15                                  23
#define SWMUX_EPDC_SDCLK                                   24
#define SWMUX_EPDC_SDLE                                    25
#define SWMUX_EPDC_SDOE                                    26
#define SWMUX_EPDC_SDSHR                                   27
#define SWMUX_EPDC_SDCE0                                   28
#define SWMUX_EPDC_SDCE1                                   29
#define SWMUX_EPDC_SDCE2                                   30
#define SWMUX_EPDC_SDCE3                                   31
#define SWMUX_EPDC_GDCLK                                   32
#define SWMUX_EPDC_GDOE                                    33
#define SWMUX_EPDC_GDRL                                    34
#define SWMUX_EPDC_GDSP                                    35
#define SWMUX_EPDC_BDR0                                    36
#define SWMUX_EPDC_BDR1                                    37
#define SWMUX_EPDC_PWR_COM                                 38
#define SWMUX_EPDC_PWR_STAT                                39
#define SWMUX_LCD_CLK                                      40
#define SWMUX_LCD_ENABLE                                   41
#define SWMUX_LCD_HSYNC                                    42
#define SWMUX_LCD_VSYNC                                    43
#define SWMUX_LCD_RESET                                    44
#define SWMUX_LCD_DATA00                                   45
#define SWMUX_LCD_DATA01                                   46
#define SWMUX_LCD_DATA02                                   47
#define SWMUX_LCD_DATA03                                   48
#define SWMUX_LCD_DATA04                                   49
#define SWMUX_LCD_DATA05                                   50
#define SWMUX_LCD_DATA06                                   51
#define SWMUX_LCD_DATA07                                   52
#define SWMUX_LCD_DATA08                                   53
#define SWMUX_LCD_DATA09                                   54
#define SWMUX_LCD_DATA10                                   55
#define SWMUX_LCD_DATA11                                   56
#define SWMUX_LCD_DATA12                                   57
#define SWMUX_LCD_DATA13                                   58
#define SWMUX_LCD_DATA14                                   59
#define SWMUX_LCD_DATA15                                   60
#define SWMUX_LCD_DATA16                                   61
#define SWMUX_LCD_DATA17                                   62
#define SWMUX_LCD_DATA18                                   63
#define SWMUX_LCD_DATA19                                   64
#define SWMUX_LCD_DATA20                                   65
#define SWMUX_LCD_DATA21                                   66
#define SWMUX_LCD_DATA22                                   67
#define SWMUX_LCD_DATA23                                   68
#define SWMUX_UART1_RX_DATA                                69
#define SWMUX_UART1_TX_DATA                                70
#define SWMUX_UART2_RX_DATA                                71
#define SWMUX_UART2_TX_DATA                                72
#define SWMUX_UART3_RX_DATA                                73
#define SWMUX_UART3_TX_DATA                                74
#define SWMUX_UART3_RTS_B                                  75
#define SWMUX_UART3_CTS_B                                  76
#define SWMUX_I2C1_SCL                                     77
#define SWMUX_I2C1_SDA                                     78
#define SWMUX_I2C2_SCL                                     79
#define SWMUX_I2C2_SDA                                     80
#define SWMUX_I2C3_SCL                                     81
#define SWMUX_I2C3_SDA                                     82
#define SWMUX_I2C4_SCL                                     83
#define SWMUX_I2C4_SDA                                     84
#define SWMUX_ECSPI1_SCLK                                  85
#define SWMUX_ECSPI1_MOSI                                  86
#define SWMUX_ECSPI1_MISO                                  87
#define SWMUX_ECSPI1_SS0                                   88
#define SWMUX_ECSPI2_SCLK                                  89
#define SWMUX_ECSPI2_MOSI                                  90
#define SWMUX_ECSPI2_MISO                                  91
#define SWMUX_ECSPI2_SS0                                   92
#define SWMUX_SD1_CD_B                                     93
#define SWMUX_SD1_WP                                       94
#define SWMUX_SD1_RESET_B                                  95
#define SWMUX_SD1_CLK                                      96
#define SWMUX_SD1_CMD                                      97
#define SWMUX_SD1_DATA0                                    98
#define SWMUX_SD1_DATA1                                    99
#define SWMUX_SD1_DATA2                                    100
#define SWMUX_SD1_DATA3                                    101
#define SWMUX_SD2_CD_B                                     102
#define SWMUX_SD2_WP                                       103
#define SWMUX_SD2_RESET_B                                  104
#define SWMUX_SD2_CLK                                      105
#define SWMUX_SD2_CMD                                      106
#define SWMUX_SD2_DATA0                                    107
#define SWMUX_SD2_DATA1                                    108
#define SWMUX_SD2_DATA2                                    109
#define SWMUX_SD2_DATA3                                    110
#define SWMUX_SD3_CLK                                      111
#define SWMUX_SD3_CMD                                      112
#define SWMUX_SD3_DATA0                                    113
#define SWMUX_SD3_DATA1                                    114
#define SWMUX_SD3_DATA2                                    115
#define SWMUX_SD3_DATA3                                    116
#define SWMUX_SD3_DATA4                                    117
#define SWMUX_SD3_DATA5                                    118
#define SWMUX_SD3_DATA6                                    119
#define SWMUX_SD3_DATA7                                    120
#define SWMUX_SD3_STROBE                                   121
#define SWMUX_SD3_RESET_B                                  122
#define SWMUX_SAI1_RX_DATA                                 123
#define SWMUX_SAI1_TX_BCLK                                 124
#define SWMUX_SAI1_TX_SYNC                                 125
#define SWMUX_SAI1_TX_DATA                                 126
#define SWMUX_SAI1_RX_SYNC                                 127
#define SWMUX_SAI1_RX_BCLK                                 128
#define SWMUX_SAI1_MCLK                                    129
#define SWMUX_SAI2_TX_SYNC                                 130
#define SWMUX_SAI2_TX_BCLK                                 131
#define SWMUX_SAI2_RX_DATA                                 132
#define SWMUX_SAI2_TX_DATA                                 133
#define SWMUX_ENET1_RGMII_RD0                              134
#define SWMUX_ENET1_RGMII_RD1                              135
#define SWMUX_ENET1_RGMII_RD2                              136
#define SWMUX_ENET1_RGMII_RD3                              137
#define SWMUX_ENET1_RGMII_RX_CTL                           138
#define SWMUX_ENET1_RGMII_RXC                              139
#define SWMUX_ENET1_RGMII_TD0                              140
#define SWMUX_ENET1_RGMII_TD1                              141
#define SWMUX_ENET1_RGMII_TD2                              142
#define SWMUX_ENET1_RGMII_TD3                              143
#define SWMUX_ENET1_RGMII_TX_CTL                           144
#define SWMUX_ENET1_RGMII_TXC                              145
#define SWMUX_ENET1_TX_CLK                                 146
#define SWMUX_ENET1_RX_CLK                                 147
#define SWMUX_ENET1_CRS                                    148
#define SWMUX_ENET1_COL                                    149

/*
 * offsets of IOMUXC registers from IMX7_IOMUX_SWPAD
 * where IMX7_IOMUX_SWPAD = IMX7_IOMUXC_BASE + 0x026C
 */

#define SWPAD_GPIO1_IO08                                   0
#define SWPAD_GPIO1_IO09                                   1
#define SWPAD_GPIO1_IO10                                   2
#define SWPAD_GPIO1_IO11                                   3
#define SWPAD_GPIO1_IO12                                   4
#define SWPAD_GPIO1_IO13                                   5
#define SWPAD_GPIO1_IO14                                   6
#define SWPAD_GPIO1_IO15                                   7
#define SWPAD_JTAG_MOD                                     8
#define SWPAD_JTAG_TCK                                     9
#define SWPAD_JTAG_TDI                                     10
#define SWPAD_JTAG_TDO                                     11
#define SWPAD_JTAG_TMS                                     12
#define SWPAD_JTAG_TRST_B                                  13
#define SWPAD_EPDC_DATA00                                  14
#define SWPAD_EPDC_DATA01                                  15
#define SWPAD_EPDC_DATA02                                  16
#define SWPAD_EPDC_DATA03                                  17
#define SWPAD_EPDC_DATA04                                  18
#define SWPAD_EPDC_DATA05                                  19
#define SWPAD_EPDC_DATA06                                  20
#define SWPAD_EPDC_DATA07                                  21
#define SWPAD_EPDC_DATA08                                  22
#define SWPAD_EPDC_DATA09                                  23
#define SWPAD_EPDC_DATA10                                  24
#define SWPAD_EPDC_DATA11                                  25
#define SWPAD_EPDC_DATA12                                  26
#define SWPAD_EPDC_DATA13                                  27
#define SWPAD_EPDC_DATA14                                  28
#define SWPAD_EPDC_DATA15                                  29
#define SWPAD_EPDC_SDCLK                                   30
#define SWPAD_EPDC_SDLE                                    31
#define SWPAD_EPDC_SDOE                                    32
#define SWPAD_EPDC_SDSHR                                   33
#define SWPAD_EPDC_SDCE0                                   34
#define SWPAD_EPDC_SDCE1                                   35
#define SWPAD_EPDC_SDCE2                                   36
#define SWPAD_EPDC_SDCE3                                   37
#define SWPAD_EPDC_GDCLK                                   38
#define SWPAD_EPDC_GDOE                                    39
#define SWPAD_EPDC_GDRL                                    40
#define SWPAD_EPDC_GDSP                                    41
#define SWPAD_EPDC_BDR0                                    42
#define SWPAD_EPDC_BDR1                                    43
#define SWPAD_EPDC_PWR_COM                                 44
#define SWPAD_EPDC_PWR_STAT                                45
#define SWPAD_LCD_CLK                                      46
#define SWPAD_LCD_ENABLE                                   47
#define SWPAD_LCD_HSYNC                                    48
#define SWPAD_LCD_VSYNC                                    49
#define SWPAD_LCD_RESET                                    50
#define SWPAD_LCD_DATA00                                   51
#define SWPAD_LCD_DATA01                                   52
#define SWPAD_LCD_DATA02                                   53
#define SWPAD_LCD_DATA03                                   54
#define SWPAD_LCD_DATA04                                   55
#define SWPAD_LCD_DATA05                                   56
#define SWPAD_LCD_DATA06                                   57
#define SWPAD_LCD_DATA07                                   58
#define SWPAD_LCD_DATA08                                   59
#define SWPAD_LCD_DATA09                                   60
#define SWPAD_LCD_DATA10                                   61
#define SWPAD_LCD_DATA11                                   62
#define SWPAD_LCD_DATA12                                   63
#define SWPAD_LCD_DATA13                                   64
#define SWPAD_LCD_DATA14                                   65
#define SWPAD_LCD_DATA15                                   66
#define SWPAD_LCD_DATA16                                   67
#define SWPAD_LCD_DATA17                                   68
#define SWPAD_LCD_DATA18                                   69
#define SWPAD_LCD_DATA19                                   70
#define SWPAD_LCD_DATA20                                   71
#define SWPAD_LCD_DATA21                                   72
#define SWPAD_LCD_DATA22                                   73
#define SWPAD_LCD_DATA23                                   74
#define SWPAD_UART1_RX_DATA                                75
#define SWPAD_UART1_TX_DATA                                76
#define SWPAD_UART2_RX_DATA                                77
#define SWPAD_UART2_TX_DATA                                78
#define SWPAD_UART3_RX_DATA                                79
#define SWPAD_UART3_TX_DATA                                80
#define SWPAD_UART3_RTS_B                                  81
#define SWPAD_UART3_CTS_B                                  82
#define SWPAD_I2C1_SCL                                     83
#define SWPAD_I2C1_SDA                                     84
#define SWPAD_I2C2_SCL                                     85
#define SWPAD_I2C2_SDA                                     86
#define SWPAD_I2C3_SCL                                     87
#define SWPAD_I2C3_SDA                                     88
#define SWPAD_I2C4_SCL                                     89
#define SWPAD_I2C4_SDA                                     90
#define SWPAD_ECSPI1_SCLK                                  91
#define SWPAD_ECSPI1_MOSI                                  92
#define SWPAD_ECSPI1_MISO                                  93
#define SWPAD_ECSPI1_SS0                                   94
#define SWPAD_ECSPI2_SCLK                                  95
#define SWPAD_ECSPI2_MOSI                                  96
#define SWPAD_ECSPI2_MISO                                  97
#define SWPAD_ECSPI2_SS0                                   98
#define SWPAD_SD1_CD_B                                     99
#define SWPAD_SD1_WP                                       100
#define SWPAD_SD1_RESET_B                                  101
#define SWPAD_SD1_CLK                                      102
#define SWPAD_SD1_CMD                                      103
#define SWPAD_SD1_DATA0                                    104
#define SWPAD_SD1_DATA1                                    105
#define SWPAD_SD1_DATA2                                    106
#define SWPAD_SD1_DATA3                                    107
#define SWPAD_SD2_CD_B                                     108
#define SWPAD_SD2_WP                                       109
#define SWPAD_SD2_RESET_B                                  110
#define SWPAD_SD2_CLK                                      111
#define SWPAD_SD2_CMD                                      112
#define SWPAD_SD2_DATA0                                    113
#define SWPAD_SD2_DATA1                                    114
#define SWPAD_SD2_DATA2                                    115
#define SWPAD_SD2_DATA3                                    116
#define SWPAD_SD3_CLK                                      117
#define SWPAD_SD3_CMD                                      118
#define SWPAD_SD3_DATA0                                    119
#define SWPAD_SD3_DATA1                                    120
#define SWPAD_SD3_DATA2                                    121
#define SWPAD_SD3_DATA3                                    122
#define SWPAD_SD3_DATA4                                    123
#define SWPAD_SD3_DATA5                                    124
#define SWPAD_SD3_DATA6                                    125
#define SWPAD_SD3_DATA7                                    126
#define SWPAD_SD3_STROBE                                   127
#define SWPAD_SD3_RESET_B                                  128
#define SWPAD_SAI1_RX_DATA                                 129
#define SWPAD_SAI1_TX_BCLK                                 130
#define SWPAD_SAI1_TX_SYNC                                 131
#define SWPAD_SAI1_TX_DATA                                 132
#define SWPAD_SAI1_RX_SYNC                                 133
#define SWPAD_SAI1_RX_BCLK                                 134
#define SWPAD_SAI1_MCLK                                    135
#define SWPAD_SAI2_TX_SYNC                                 136
#define SWPAD_SAI2_TX_BCLK                                 137
#define SWPAD_SAI2_RX_DATA                                 138
#define SWPAD_SAI2_TX_DATA                                 139
#define SWPAD_ENET1_RGMII_RD0                              140
#define SWPAD_ENET1_RGMII_RD1                              141
#define SWPAD_ENET1_RGMII_RD2                              142
#define SWPAD_ENET1_RGMII_RD3                              143
#define SWPAD_ENET1_RGMII_RX_CTL                           144
#define SWPAD_ENET1_RGMII_RXC                              145
#define SWPAD_ENET1_RGMII_TD0                              146
#define SWPAD_ENET1_RGMII_TD1                              147
#define SWPAD_ENET1_RGMII_TD2                              148
#define SWPAD_ENET1_RGMII_TD3                              149
#define SWPAD_ENET1_RGMII_TX_CTL                           150
#define SWPAD_ENET1_RGMII_TXC                              151
#define SWPAD_ENET1_TX_CLK                                 152
#define SWPAD_ENET1_RX_CLK                                 153
#define SWPAD_ENET1_CRS                                    154
#define SWPAD_ENET1_COL                                    155

/*
 * offsets of IOMUXC registers from IMX7_IOMUX_INPUT
 * where IMX7_IOMUX_INPUT = IMX7_IOMUXC_BASE + 0x04DC
 */

#define SWINPUT_FLEXCAN1_RX                                0
#define SWINPUT_FLEXCAN2_RX                                1
#define SWINPUT_CCM_EXT_CLK_1                              2
#define SWINPUT_CCM_EXT_CLK_2                              3
#define SWINPUT_CCM_EXT_CLK_3                              4
#define SWINPUT_CCM_EXT_CLK_4                              5
#define SWINPUT_CCM_PMIC_READY                             6
#define SWINPUT_CSI_DATA2                                  7
#define SWINPUT_CSI_DATA3                                  8
#define SWINPUT_CSI_DATA4                                  9
#define SWINPUT_CSI_DATA5                                  10
#define SWINPUT_CSI_DATA6                                  11
#define SWINPUT_CSI_DATA7                                  12
#define SWINPUT_CSI_DATA8                                  13
#define SWINPUT_CSI_DATA9                                  14
#define SWINPUT_CSI_HSYNC                                  15
#define SWINPUT_CSI_PIXCLK                                 16
#define SWINPUT_CSI_VSYNC                                  17
#define SWINPUT_ECSPI1_SCLK                                18
#define SWINPUT_ECSPI1_MISO                                19
#define SWINPUT_ECSPI1_MOSI                                20
#define SWINPUT_ECSPI1_SS0_B                               21
#define SWINPUT_ECSPI2_SCLK                                22
#define SWINPUT_ECSPI2_MISO                                23
#define SWINPUT_ECSPI2_MOSI                                24
#define SWINPUT_ECSPI2_SS0_B                               25
#define SWINPUT_ECSPI3_SCLK                                26
#define SWINPUT_ECSPI3_MISO                                27
#define SWINPUT_ECSPI3_MOSI                                28
#define SWINPUT_ECSPI3_SS0_B                               29
#define SWINPUT_ECSPI4_SCLK                                30
#define SWINPUT_ECSPI4_MISO                                31
#define SWINPUT_ECSPI4_MOSI                                32
#define SWINPUT_ECSPI4_SS0_B                               33
#define SWINPUT_CCM_ENET1_REF_CLK                          34
#define SWINPUT_ENET1_MDIO                                 35
#define SWINPUT_ENET1_RX_CLK                               36
#define SWINPUT_CCM_ENET2_REF_CLK                          37
#define SWINPUT_ENET2_MDIO                                 38
#define SWINPUT_ENET2_RX_CLK                               39
#define SWINPUT_EPDC_PWR_IRQ                               40
#define SWINPUT_EPDC_PWR_STAT                              41
#define SWINPUT_FLEXTIMER1_CH0                             42
#define SWINPUT_FLEXTIMER1_CH1                             43
#define SWINPUT_FLEXTIMER1_CH2                             44
#define SWINPUT_FLEXTIMER1_CH3                             45
#define SWINPUT_FLEXTIMER1_CH4                             46
#define SWINPUT_FLEXTIMER1_CH5                             47
#define SWINPUT_FLEXTIMER1_CH6                             48
#define SWINPUT_FLEXTIMER1_CH7                             49
#define SWINPUT_FLEXTIMER1_PHA                             50
#define SWINPUT_FLEXTIMER1_PHB                             51
#define SWINPUT_FLEXTIMER2_CH0                             52
#define SWINPUT_FLEXTIMER2_CH1                             53
#define SWINPUT_FLEXTIMER2_CH2                             54
#define SWINPUT_FLEXTIMER2_CH3                             55
#define SWINPUT_FLEXTIMER2_CH4                             56
#define SWINPUT_FLEXTIMER2_CH5                             57
#define SWINPUT_FLEXTIMER2_CH6                             58
#define SWINPUT_FLEXTIMER2_CH7                             59
#define SWINPUT_FLEXTIMER2_PHA                             60
#define SWINPUT_FLEXTIMER2_PHB                             61
#define SWINPUT_I2C1_SCL                                   62
#define SWINPUT_I2C1_SDA                                   63
#define SWINPUT_I2C2_SCL                                   64
#define SWINPUT_I2C2_SDA                                   65
#define SWINPUT_I2C3_SCL                                   66
#define SWINPUT_I2C3_SDA                                   67
#define SWINPUT_I2C4_SCL                                   68
#define SWINPUT_I2C4_SDA                                   69
#define SWINPUT_KPP_COL0                                   70
#define SWINPUT_KPP_COL1                                   71
#define SWINPUT_KPP_COL2                                   72
#define SWINPUT_KPP_COL3                                   73
#define SWINPUT_KPP_COL4                                   74
#define SWINPUT_KPP_COL5                                   75
#define SWINPUT_KPP_COL6                                   76
#define SWINPUT_KPP_COL7                                   77
#define SWINPUT_KPP_ROW0                                   78
#define SWINPUT_KPP_ROW1                                   79
#define SWINPUT_KPP_ROW2                                   80
#define SWINPUT_KPP_ROW3                                   81
#define SWINPUT_KPP_ROW4                                   82
#define SWINPUT_KPP_ROW5                                   83
#define SWINPUT_KPP_ROW6                                   84
#define SWINPUT_KPP_ROW7                                   85
#define SWINPUT_LCD_BUSY                                   86
#define SWINPUT_LCD_DATA00                                 87
#define SWINPUT_LCD_DATA01                                 88
#define SWINPUT_LCD_DATA02                                 89
#define SWINPUT_LCD_DATA03                                 90
#define SWINPUT_LCD_DATA04                                 91
#define SWINPUT_LCD_DATA05                                 92
#define SWINPUT_LCD_DATA06                                 93
#define SWINPUT_LCD_DATA07                                 94
#define SWINPUT_LCD_DATA08                                 95
#define SWINPUT_LCD_DATA09                                 96
#define SWINPUT_LCD_DATA10                                 97
#define SWINPUT_LCD_DATA11                                 98
#define SWINPUT_LCD_DATA12                                 99
#define SWINPUT_LCD_DATA13                                 100
#define SWINPUT_LCD_DATA14                                 101
#define SWINPUT_LCD_DATA15                                 102
#define SWINPUT_LCD_DATA16                                 103
#define SWINPUT_LCD_DATA17                                 104
#define SWINPUT_LCD_DATA18                                 105
#define SWINPUT_LCD_DATA19                                 106
#define SWINPUT_LCD_DATA20                                 107
#define SWINPUT_LCD_DATA21                                 108
#define SWINPUT_LCD_DATA22                                 109
#define SWINPUT_LCD_DATA23                                 110
#define SWINPUT_LCD_VSYNC                                  111
#define SWINPUT_SAI1_RX_BCLK                               112
#define SWINPUT_SAI1_RX_DATA                               113
#define SWINPUT_SAI1_RX_SYNC                               114
#define SWINPUT_SAI1_TX_BCLK                               115
#define SWINPUT_SAI1_TX_SYNC                               116
#define SWINPUT_SAI2_RX_BCLK                               117
#define SWINPUT_SAI2_RX_DATA                               118
#define SWINPUT_SAI2_RX_SYNC                               119
#define SWINPUT_SAI2_TX_BCLK                               120
#define SWINPUT_SAI2_TX_SYNC                               121
#define SWINPUT_SAI3_RX_BCLK                               122
#define SWINPUT_SAI3_RX_DATA                               123
#define SWINPUT_SAI3_RX_SYNC                               124
#define SWINPUT_SAI3_TX_BCLK                               125
#define SWINPUT_SAI3_TX_SYNC                               126
#define SWINPUT_SDMA_EVENTS0                               127
#define SWINPUT_SDMA_EVENTS1                               128
#define SWINPUT_SIM1_PORT1_PD                              129
#define SWINPUT_SIM1_PORT1_TRXD                            130
#define SWINPUT_SIM2_PORT1_PD                              131
#define SWINPUT_SIM2_PORT1_TRXD                            132
#define SWINPUT_UART1_RTS_B                                133
#define SWINPUT_UART1_RX_DATA                              134
#define SWINPUT_UART2_RTS_B                                135
#define SWINPUT_UART2_RX_DATA                              136
#define SWINPUT_UART3_RTS_B                                137
#define SWINPUT_UART3_RX_DATA                              138
#define SWINPUT_UART4_RTS_B                                139
#define SWINPUT_UART4_RX_DATA                              140
#define SWINPUT_UART5_RTS_B                                141
#define SWINPUT_UART5_RX_DATA                              142
#define SWINPUT_UART6_RTS_B                                143
#define SWINPUT_UART6_RX_DATA                              144
#define SWINPUT_UART7_RTS_B                                145
#define SWINPUT_UART7_RX_DATA                              146
#define SWINPUT_USB_OTG2_OC                                147
#define SWINPUT_USB_OTG1_OC                                148
#define SWINPUT_USB_OTG2_ID                                149
#define SWINPUT_USB_OTG1_ID                                150
#define SWINPUT_SD3_CD_B                                   151
#define SWINPUT_SD3_WP                                     152


/*
 * offsets of IOMUXC_LPSR registers from IMX7_IOMUX_SWMUX
 * where IMX7_IOMUX_SWMUX = IMX7_IOMUXC_BASE + 0x0014
 */

#define SWMUX_GPIO1_IO00                                   (IMX7_IOMUX_LPSR + 0)
#define SWMUX_GPIO1_IO01                                   (IMX7_IOMUX_LPSR + 1)
#define SWMUX_GPIO1_IO02                                   (IMX7_IOMUX_LPSR + 2)
#define SWMUX_GPIO1_IO03                                   (IMX7_IOMUX_LPSR + 3)
#define SWMUX_GPIO1_IO04                                   (IMX7_IOMUX_LPSR + 4)
#define SWMUX_GPIO1_IO05                                   (IMX7_IOMUX_LPSR + 5)
#define SWMUX_GPIO1_IO06                                   (IMX7_IOMUX_LPSR + 6)
#define SWMUX_GPIO1_IO07                                   (IMX7_IOMUX_LPSR + 7)

#define SWPAD_GPIO1_IO00                                   (IMX7_IOMUX_LPSR + 0)
#define SWPAD_GPIO1_IO01                                   (IMX7_IOMUX_LPSR + 1)
#define SWPAD_GPIO1_IO02                                   (IMX7_IOMUX_LPSR + 2)
#define SWPAD_GPIO1_IO03                                   (IMX7_IOMUX_LPSR + 3)
#define SWPAD_GPIO1_IO04                                   (IMX7_IOMUX_LPSR + 4)
#define SWPAD_GPIO1_IO05                                   (IMX7_IOMUX_LPSR + 5)
#define SWPAD_GPIO1_IO06                                   (IMX7_IOMUX_LPSR + 6)
#define SWPAD_GPIO1_IO07                                   (IMX7_IOMUX_LPSR + 7)

/*
 * Function prototypes
 */
void pinmux_set_swmux (int pin, int mux_config);
void pinmux_set_padcfg (int pin, int pad_config);
void pinmux_set_input (int pin, int input_config);

#endif  /* __ARM_MX7X_IOMUX_H_INCLUDED */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/arm/mx7x_iomux.h $ $Rev: 834524 $")
#endif
