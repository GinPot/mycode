/*
* $QNXLicenseC:
* Copyright (c) 2017, QNX Software Systems. All Rights Reserved.
*
* You must obtain a written license from and pay applicable license fees to QNX
* Software Systems before you may reproduce, modify or distribute this software,
* or any work that includes all or part of this software. Free development
* licenses are available for evaluation and non-commercial purposes. For more
* information visit http://licensing.qnx.com or email licensing@qnx.com.
*
* This file may contain contributions from others. Please review this entire
* file for other proprietary rights or license notices, as well as the QNX
* Development Suite License Guide at http://licensing.qnx.com/license-guide/
* for other information.
* $
*/

#ifndef __RCAR_SOC_BOARD_M3_H_INCLUDED
#define __RCAR_SOC_BOARD_M3_H_INCLUDED

extern void config_usbh(rcar_module *module);
extern void config_usbd(rcar_module *module);
extern void config_usb3h(rcar_module *module);
extern void config_dmac(rcar_module *module);
extern void config_audio(rcar_module *module);

rcar_config m3_ws10_config[] = {
    [DEV_TIMER] = {
        .mod = (rcar_module[])
        {
            { .clk.val = ENABLE,  .clk.reg = SMSTPCR(1), .clk.bits = BIT(25) }, /* TMU0 */
            { .clk.val = DISABLE, .clk.reg = SMSTPCR(1), .clk.bits = BIT(24) }, /* TMU1 */
            { .clk.val = DISABLE, .clk.reg = SMSTPCR(1), .clk.bits = BIT(23) }, /* TMU2 */
            { .clk.val = DISABLE, .clk.reg = SMSTPCR(1), .clk.bits = BIT(22) }, /* TMU3 */
            { .clk.val = DISABLE, .clk.reg = SMSTPCR(1), .clk.bits = BIT(21) }, /* TMU4 */
            { { 0 } }
        }
    },
#define USE_HSCIF1
#if defined USE_HSCIF1
    [DEV_HSCIF] = {
        .mod = (rcar_module[])
        {
            { .clk.val = DISABLE, .clk.reg = SMSTPCR(5), .clk.bits = BIT(20) }, /* HSCIF0 */
            { .clk.val = ENABLE,  .clk.reg = SMSTPCR(5), .clk.bits = BIT(19) }, /* HSCIF1 */
            { .clk.val = DISABLE, .clk.reg = SMSTPCR(5), .clk.bits = BIT(18) }, /* HSCIF2 */
            { .clk.val = DISABLE, .clk.reg = SMSTPCR(5), .clk.bits = BIT(17) }, /* HSCIF3 */
            { .clk.val = DISABLE, .clk.reg = SMSTPCR(5), .clk.bits = BIT(16) }, /* HSCIF4 */
            { { 0 } }
        },
        .pinmux = (rcar_pinmux[])
        {
            { .gpsr.reg = GPSR(5), .gpsr.bits = BIT(5),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(12), .ipsr.bits = 12,  .ipsr.val = 1 }, /* RX1_A/HRX1_A */
            { .gpsr.reg = GPSR(5), .gpsr.bits = BIT(6),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(12), .ipsr.bits = 16,  .ipsr.val = 1 }, /* TX1_A/HTX1_A */
            { .gpsr.reg = GPSR(5), .gpsr.bits = BIT(7),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(12), .ipsr.bits = 20,  .ipsr.val = 1 }, /* CTS1#/HCTS1#_A */
            { .gpsr.reg = GPSR(5), .gpsr.bits = BIT(8),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(12), .ipsr.bits = 24,  .ipsr.val = 1 }, /* RTS1#/HRTS1#_A */
            { { 0 } }
        }
    },
#endif
    [DEV_ETHERNET] = {
        .mod = (rcar_module[])
        {
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(8), .clk.bits = BIT(12) }, /* EAVB-IF */
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(9), .clk.bits = BIT(10) }, /* GPIO2 */
            { { 0 } }
        },
        .pinmux = (rcar_pinmux[])
        {
            { .gpsr.reg = GPSR(2), .gpsr.bits = BIT(9),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(0), .ipsr.bits = 0,        .ipsr.val = 0 }, /* AVB_MDC */
            { .gpsr.reg = GPSR(2), .gpsr.bits = BIT(10), .gpsr.val = GPSR_IO, .gpio.reg = GPIO(2), .gpio.type = GPIO_OUT, .gpio.val = 1 }, /* AVB_MAGIC */
            { .gpsr.reg = GPSR(2), .gpsr.bits = BIT(11), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(0), .ipsr.bits = 8,        .ipsr.val = 0 }, /* AVB_PHY_INT */
            { .gpsr.reg = GPSR(2), .gpsr.bits = BIT(12), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(0), .ipsr.bits = 12,       .ipsr.val = 0 }, /* AVB_LINK */
            { .gpsr.reg = GPSR(2), .gpsr.bits = BIT(13), .gpsr.val = GPSR_PF },                                                            /* AVB_AVTP_MATCH_A */
            { .gpsr.reg = GPSR(2), .gpsr.bits = BIT(14), .gpsr.val = GPSR_PF },                                                            /* AVB_AVTP_CAPTURE_A */
            { { 0 } }
        },
        .settings = (rcar_pin[])
        {
            /* disable all eavb pins pull-up/down */
            { .reg = PUEN(0), .bits = BITS(15,31), .val = PUEN_DISABLE },
            { .reg = PUEN(1), .bits = BIT(0),      .val = PUEN_DISABLE },
            { .reg = DRVCTRL(2), .bits = DRV(DRV3, 0),  .val = DRVCAP(DRV3_1_2, 0)  }, /* AVB_TD0 */
            { .reg = DRVCTRL(2), .bits = DRV(DRV3, 4),  .val = DRVCAP(DRV3_1_2, 4)  }, /* AVB_TXC */
            { .reg = DRVCTRL(2), .bits = DRV(DRV3, 8),  .val = DRVCAP(DRV3_1_2, 8)  }, /* AVB_TX_CTL */
            { .reg = DRVCTRL(3), .bits = DRV(DRV3, 20), .val = DRVCAP(DRV3_1_2, 20) }, /* AVB_TD3 */
            { .reg = DRVCTRL(3), .bits = DRV(DRV3, 24), .val = DRVCAP(DRV3_1_2, 24) }, /* AVB_TD2 */
            { .reg = DRVCTRL(3), .bits = DRV(DRV3, 28), .val = DRVCAP(DRV3_1_2, 28) }, /* AVB_TD1 */
            { 0 }
        }
    },
    [DEV_USBH] = {
        .mod = (rcar_module[])
        {
            { .clk.val = ENABLE,  .clk.reg = SMSTPCR(7), .clk.bits = BIT(3), .base = RCAR_GEN3_USB0_BASE }, /* EHCI0/OHCI0 */
            { .clk.val = ENABLE,  .clk.reg = SMSTPCR(7), .clk.bits = BIT(2), .base = RCAR_GEN3_USB1_BASE }, /* EHCI0/OHCI1 */
            { { 0 } }
        },
        .blk_config = config_usbh,
    },
    [DEV_USBD] = {
        .mod = (rcar_module[])
        {
            { .clk.val = ENABLE,  .clk.reg = SMSTPCR(7), .clk.bits = BIT(4), .base = RCAR_GEN3_HSUSB_BASE }, /* HS-USB-IF */
            { .clk.val = ENABLE,  .clk.reg = SMSTPCR(3), .clk.bits = BIT(30) },                              /* USB_DMAC0-0*/
            { .clk.val = ENABLE,  .clk.reg = SMSTPCR(3), .clk.bits = BIT(31) },                              /* USB_DMAC0-1*/
            { .clk.val = ENABLE,  .clk.reg = SMSTPCR(9), .clk.bits = BIT(6)  },                              /* GPIO6 */
            { { 0 } }
        },
        .pinmux = (rcar_pinmux[])
        {
            { .gpsr.reg = GPSR(6), .gpsr.bits = BIT(24), .gpsr.val = GPSR_IO, .gpio.reg = GPIO(6), .gpio.type = GPIO_OUT, .gpio.val = 1 }, /* USB0_PWEN */
            { .gpsr.reg = GPSR(6), .gpsr.bits = BIT(25), .gpsr.val = GPSR_IO, .gpio.reg = GPIO(6), .gpio.type = GPIO_OUT, .gpio.val = 1 }, /* USB0_OVC */
            { { 0 } }
        },
        .blk_config = config_usbd,
    },
    [DEV_USB3H] = {
        .mod = (rcar_module[])
        {
            { .clk.val = ENABLE,  .clk.reg = SMSTPCR(3), .clk.bits = BIT(28), .base = RCAR_GEN3_USB30_BASE }, /* USB3.0-IF0 */
            { { 0 } }
        },
        .blk_config = config_usb3h,
    },
    [DEV_MSIOF] = {
        .mod = (rcar_module[])
        {
            { .clk.val = ENABLE,  .clk.reg = SMSTPCR(2), .clk.bits = BIT(11) }, /* MSIOF0 */
#if defined USE_MSIOF1A || defined USE_MSIOF1C
            { .clk.val = ENABLE,  .clk.reg = SMSTPCR(2), .clk.bits = BIT(10) }, /* MSIOF1 */
#else
            { .clk.val = DISABLE, .clk.reg = SMSTPCR(2), .clk.bits = BIT(10) }, /* MSIOF1 */
#endif
            { .clk.val = DISABLE, .clk.reg = SMSTPCR(2), .clk.bits = BIT(9)  }, /* MSIOF2 */
            { .clk.val = DISABLE, .clk.reg = SMSTPCR(2), .clk.bits = BIT(8)  }, /* MSIOF3 */
            { { 0 } }
        },
        .pinmux = (rcar_pinmux[])
        {
            { .gpsr.reg = GPSR(5), .gpsr.bits = BIT(17), .gpsr.val = GPSR_PF }, /* MSIOF0_SCK */
            { .gpsr.reg = GPSR(5), .gpsr.bits = BIT(18), .gpsr.val = GPSR_PF }, /* MSIOF0_SYNC */
            { .gpsr.reg = GPSR(5), .gpsr.bits = BIT(20), .gpsr.val = GPSR_PF }, /* MSIOF0_TXD */
            { .gpsr.reg = GPSR(5), .gpsr.bits = BIT(22), .gpsr.val = GPSR_PF }, /* MSIOF0_RXD */
#if defined USE_MSIOF1A
            { .gpsr.reg = GPSR(6), .gpsr.bits = BIT(7),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(15), .ipsr.bits = 16, .ipsr.val = 2 }, /* MSIOF1_TXD_A/SDATA3 */
            { .gpsr.reg = GPSR(6), .gpsr.bits = BIT(8),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(15), .ipsr.bits = 20, .ipsr.val = 2 }, /* MSIOF1_SCK_A/SSI_SCK4 */
            { .gpsr.reg = GPSR(6), .gpsr.bits = BIT(9),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(15), .ipsr.bits = 24, .ipsr.val = 2 }, /* MSIOF1_SYNC_A/SSI_WS4 */
            { .gpsr.reg = GPSR(6), .gpsr.bits = BIT(10), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(15), .ipsr.bits = 28, .ipsr.val = 2 }, /* MSIOF1_RXD_A/SSI_SDATA4 */
#elif defined USE_MSIOF1C
            { .gpsr.reg = GPSR(6), .gpsr.bits = BIT(17), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(16), .ipsr.bits = 12, .ipsr.val = 2 }, /* MSIOF1_SCK_C/SSI_SCK78 */
            { .gpsr.reg = GPSR(6), .gpsr.bits = BIT(18), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(16), .ipsr.bits = 16, .ipsr.val = 2 }, /* MSIOF1_SYNC_C/SSI_WS78 */
            { .gpsr.reg = GPSR(6), .gpsr.bits = BIT(19), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(16), .ipsr.bits = 20, .ipsr.val = 2 }, /* MSIOF1_RXD_C/SSI_SDATA7 */
            { .gpsr.reg = GPSR(6), .gpsr.bits = BIT(20), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(16), .ipsr.bits = 24, .ipsr.val = 2 }, /* MSIOF1_TXD_C/SSI_SDATA8 */
#endif
            { { 0 } }
        },
        .settings = (rcar_pin[])
        {
#if defined USE_MSIOF1A
            { .reg = MODSEL(0), .bits = BITS(24,26), .val = (0 << 24) }, /* SEL_MSIOF1 */
#elif defined USE_MSIOF1C
            { .reg = MODSEL(0), .bits = BITS(24,26), .val = (2 << 24) },  /* SEL_MSIOF1 */
#endif
            { 0 }
        }
    },
    [DEV_I2C] = {
        .mod = (rcar_module[])
        {
            { .clk.val = DISABLE, .clk.reg = SMSTPCR(9), .clk.bits = BIT(31) }, /* I2C-IF0 */
            { .clk.val = DISABLE, .clk.reg = SMSTPCR(9), .clk.bits = BIT(30) }, /* I2C-IF1 */
            { .clk.val = ENABLE,  .clk.reg = SMSTPCR(9), .clk.bits = BIT(29) }, /* I2C-IF2 */
            { .clk.val = DISABLE, .clk.reg = SMSTPCR(9), .clk.bits = BIT(28) }, /* I2C-IF3 */
            { .clk.val = ENABLE,  .clk.reg = SMSTPCR(9), .clk.bits = BIT(27) }, /* I2C-IF4 */
            { .clk.val = DISABLE, .clk.reg = SMSTPCR(9), .clk.bits = BIT(19) }, /* I2C-IF5 */
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(9), .clk.bits = BIT(18) }, /* I2C-IF6 */
            { { 0 } }
        },
        .pinmux = (rcar_pinmux[])
        {
            { .gpsr.reg = GPSR(5), .gpsr.bits = BIT(0), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(11), .ipsr.bits = 24, .ipsr.val = 4 }, /* SDA2_A/SCK0 */
            { .gpsr.reg = GPSR(5), .gpsr.bits = BIT(4), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(12), .ipsr.bits = 8,  .ipsr.val = 4 }, /* SCL2_A/RTS0#/TANS */
            { .gpsr.reg = GPSR(1), .gpsr.bits = BIT(25), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(5), .ipsr.bits = 0, .ipsr.val = 7 }, /* SCL6_B */
            { .gpsr.reg = GPSR(1), .gpsr.bits = BIT(26), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(5), .ipsr.bits = 4,  .ipsr.val = 7 }, /* SDA6_B */
			{ { 0 } }
        },
        .settings = (rcar_pin[])
        {
            { .reg = MODSEL(0), .bits = BIT(21), .val = 0 }, /* SEL_I2C2 */
			{ .reg = MODSEL(2), .bits = BIT(24)|BIT(23)|BIT(25), .val = (1 << 23) }, /* SEL_I2C6_B */
            { 0 }
        }
    },
    [DEV_AUDIO] = {
        .mod = (rcar_module[])
        {
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(9),  .clk.bits = BIT(22)    }, /* ADG */
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(10), .clk.bits = BITS(5,15) }, /* SSI */
            { { 0 } }
        },
        .pinmux = (rcar_pinmux[])
        {
            { .gpsr.reg = GPSR(5), .gpsr.bits = BIT(12), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(13), .ipsr.bits = 8,  .ipsr.val = 3 }, /* AUDIO_CLKB_A/HSCK0 */
            { .gpsr.reg = GPSR(5), .gpsr.bits = BIT(18), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(13), .ipsr.bits = 28, .ipsr.val = 8 }, /* AUDIO_CLKOUT_A/MSIOF0_SYNC */
            { .gpsr.reg = GPSR(5), .gpsr.bits = BIT(19), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(14), .ipsr.bits = 0,  .ipsr.val = 8 }, /* AUDIO_CLKOUT3_A/MSIOF0_SS1 */
            { .gpsr.reg = GPSR(5), .gpsr.bits = BIT(21), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(14), .ipsr.bits = 4,  .ipsr.val = 3 }, /* AUDIO_CLKC_A/MSIOF0_SS2 */
            { .gpsr.reg = GPSR(6), .gpsr.bits = BIT(22), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(17), .ipsr.bits = 0,  .ipsr.val = 0 }, /* AUDIO_CLKA_A */
            { .gpsr.reg = GPSR(6), .gpsr.bits = BIT(0),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(14), .ipsr.bits = 20, .ipsr.val = 0 }, /* SSI_SCK0129 */
            { .gpsr.reg = GPSR(6), .gpsr.bits = BIT(1),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(14), .ipsr.bits = 24, .ipsr.val = 0 }, /* SSI_WS0129 */
            { .gpsr.reg = GPSR(6), .gpsr.bits = BIT(2),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(14), .ipsr.bits = 28, .ipsr.val = 0 }, /* SSI_SDATA0 */
            { .gpsr.reg = GPSR(6), .gpsr.bits = BIT(3),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(15), .ipsr.bits = 0,  .ipsr.val = 0 }, /* SSI_SDATA1_A */
            { .gpsr.reg = GPSR(6), .gpsr.bits = BIT(8),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(15), .ipsr.bits = 20, .ipsr.val = 0 }, /* SSI_SCK4 */
            { .gpsr.reg = GPSR(6), .gpsr.bits = BIT(9),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(15), .ipsr.bits = 24, .ipsr.val = 0 }, /* SSI_WS4 */
            { .gpsr.reg = GPSR(6), .gpsr.bits = BIT(10), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(15), .ipsr.bits = 28, .ipsr.val = 0 }, /* SSI_SDATA4 */
            { .gpsr.reg = GPSR(6), .gpsr.bits = BIT(17), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(16), .ipsr.bits = 12, .ipsr.val = 0 }, /* SSI_SCK78 */
            { .gpsr.reg = GPSR(6), .gpsr.bits = BIT(18), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(16), .ipsr.bits = 16, .ipsr.val = 0 }, /* SSI_WS78 */
            { .gpsr.reg = GPSR(6), .gpsr.bits = BIT(19), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(16), .ipsr.bits = 20, .ipsr.val = 0 }, /* SSI_SDATA7 */
            { .gpsr.reg = GPSR(6), .gpsr.bits = BIT(20), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(16), .ipsr.bits = 24, .ipsr.val = 0 }, /* SSI_SDATA8 */
            { { 0 } }
        },
        .settings = (rcar_pin[])
        {
            { .reg = MODSEL(0), .bits = BITS(3,4),   .val = 0 }, /* SEL_ADGA */
            { .reg = MODSEL(1), .bits = BIT(20),     .val = 0 }, /* SEL_SSI1 */
            { .reg = MODSEL(2), .bits = BITS(17,18), .val = 0 }, /* SEL_ADG_B SEL_ADG_C */
            { 0 }
        },
        .blk_config = config_audio,
    },
    [DEV_SDHI] = {
        .mod = (rcar_module[])
        {
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(3), .clk.bits = BIT(14) }, /* SD-IF0 */
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(3), .clk.bits = BIT(11) }, /* SD-IF3 */
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(9), .clk.bits = BIT(9)  }, /* GPIO3 */
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(9), .clk.bits = BIT(7)  }, /* GPIO5 */
            { { 0 } }
        },
        .cpg = (rcar_pin[])
        {
            { .reg = RCAR_GEN3_CPG_SD0CKCR, .bits = 0xFFFFFFFF, .val = 1 }, /* SD0 clk */
            { .reg = RCAR_GEN3_CPG_SD3CKCR, .bits = 0xFFFFFFFF, .val = 1 }, /* SD3 clk */
            { 0 }
        },
        .pinmux = (rcar_pinmux[])
        {
            { .gpsr.reg = GPSR(3), .gpsr.bits = BIT(0),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(7),  .ipsr.bits = 16, .ipsr.val = 0 }, /* SD0_CLK */
            { .gpsr.reg = GPSR(3), .gpsr.bits = BIT(1),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(7),  .ipsr.bits = 20, .ipsr.val = 0 }, /* SD0_CMD */
            { .gpsr.reg = GPSR(3), .gpsr.bits = BIT(2),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(7),  .ipsr.bits = 24, .ipsr.val = 0 }, /* SD0_DAT0 */
            { .gpsr.reg = GPSR(3), .gpsr.bits = BIT(3),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(7),  .ipsr.bits = 28, .ipsr.val = 0 }, /* SD0_DAT1 */
            { .gpsr.reg = GPSR(3), .gpsr.bits = BIT(4),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(8),  .ipsr.bits = 0,  .ipsr.val = 0 }, /* SD0_DAT2 */
            { .gpsr.reg = GPSR(3), .gpsr.bits = BIT(5),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(8),  .ipsr.bits = 4,  .ipsr.val = 0 }, /* SD0_DAT3 */
            { .gpsr.reg = GPSR(3), .gpsr.bits = BIT(12), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(11), .ipsr.bits = 16, .ipsr.val = 0 }, /* SD0_CD */
            { .gpsr.reg = GPSR(3), .gpsr.bits = BIT(13), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(11), .ipsr.bits = 12, .ipsr.val = 0 }, /* SD0_WP */
            { .gpsr.reg = GPSR(5), .gpsr.bits = BIT(1),  .gpsr.val = GPSR_IO, .gpio.reg = GPIO(5),  .gpio.type = GPIO_OUT, .gpio.val = 0 }, /* SD0_PWR */
            { .gpsr.reg = GPSR(5), .gpsr.bits = BIT(2),  .gpsr.val = GPSR_IO, .gpio.reg = GPIO(5),  .gpio.type = GPIO_OUT, .gpio.val = 0 }, /* SD0_SIG_PWR */
            { .gpsr.reg = GPSR(4), .gpsr.bits = BIT(7),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(9),  .ipsr.bits = 28, .ipsr.val = 0 }, /* SD3_CLK */
            { .gpsr.reg = GPSR(4), .gpsr.bits = BIT(8),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(10), .ipsr.bits = 0,  .ipsr.val = 0 }, /* SD3_CMD */
            { .gpsr.reg = GPSR(4), .gpsr.bits = BIT(9),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(10), .ipsr.bits = 4,  .ipsr.val = 0 }, /* SD3_DAT0 */
            { .gpsr.reg = GPSR(4), .gpsr.bits = BIT(10), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(10), .ipsr.bits = 8,  .ipsr.val = 0 }, /* SD3_DAT1 */
            { .gpsr.reg = GPSR(4), .gpsr.bits = BIT(11), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(10), .ipsr.bits = 12, .ipsr.val = 0 }, /* SD3_DAT2 */
            { .gpsr.reg = GPSR(4), .gpsr.bits = BIT(12), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(10), .ipsr.bits = 16, .ipsr.val = 0 }, /* SD3_DAT3 */
            { .gpsr.reg = GPSR(4), .gpsr.bits = BIT(15), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(10), .ipsr.bits = 28, .ipsr.val = 1 }, /* SD3_CD/SD3_DAT6 */
            { .gpsr.reg = GPSR(4), .gpsr.bits = BIT(16), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(11), .ipsr.bits = 0,  .ipsr.val = 1 }, /* SD3_WP/SD3_DAT7 */
            { .gpsr.reg = GPSR(3), .gpsr.bits = BIT(14), .gpsr.val = GPSR_IO, .gpio.reg = GPIO(3),  .gpio.type = GPIO_OUT, .gpio.val = 0 }, /* SD3_PWR */
            { .gpsr.reg = GPSR(3), .gpsr.bits = BIT(15), .gpsr.val = GPSR_IO, .gpio.reg = GPIO(3),  .gpio.type = GPIO_OUT, .gpio.val = 0 }, /* SD3_SIG_PWR */
            { { 0 } }
        },
        .settings = (rcar_pin[])
        {
            { .reg = PUEN(3), .bits = BITS(10,15)|BITS(29,31),       .val = PUEN_DISABLE },
            { .reg = PUEN(4), .bits = BITS(0,2)|BITS(5,6)|BITS(8,9), .val = PUEN_DISABLE },
            { 0 }
        },
    },
    [DEV_MMCIF] = {
        .mod = (rcar_module[])
        {
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(3), .clk.bits = BIT(13) }, /* SD-IF1 */
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(3), .clk.bits = BIT(12) }, /* SD-IF2 */
            { { 0 } }
        },
        .cpg = (rcar_pin[])
        {
            { .reg = RCAR_GEN3_CPG_SD1CKCR, .bits = 0xFFFFFFFF, .val = 1 }, /* SD1 clk */
            { .reg = RCAR_GEN3_CPG_SD2CKCR, .bits = 0xFFFFFFFF, .val = 1 }, /* SD2 clk */
            { 0 }
        },
        .pinmux = (rcar_pinmux[])
        {
            { .gpsr.reg = GPSR(3), .gpsr.bits = BIT(8),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(8), .ipsr.bits = 16, .ipsr.val = 1 }, /* SD2_DAT4/SD1_DAT0 */
            { .gpsr.reg = GPSR(3), .gpsr.bits = BIT(9),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(8), .ipsr.bits = 20, .ipsr.val = 1 }, /* SD2_DAT5/SD1_DAT1 */
            { .gpsr.reg = GPSR(3), .gpsr.bits = BIT(10), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(8), .ipsr.bits = 24, .ipsr.val = 1 }, /* SD2_DAT6/SD1_DAT2 */
            { .gpsr.reg = GPSR(3), .gpsr.bits = BIT(11), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(8), .ipsr.bits = 28, .ipsr.val = 1 }, /* SD2_DAT7/SD1_DAT3 */
            { .gpsr.reg = GPSR(4), .gpsr.bits = BIT(0),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(9), .ipsr.bits = 0,  .ipsr.val = 0 }, /* SD2_CLK */
            { .gpsr.reg = GPSR(4), .gpsr.bits = BIT(1),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(9), .ipsr.bits = 4,  .ipsr.val = 0 }, /* SD2_CMD */
            { .gpsr.reg = GPSR(4), .gpsr.bits = BIT(2),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(9), .ipsr.bits = 8,  .ipsr.val = 0 }, /* SD2_DAT0 */
            { .gpsr.reg = GPSR(4), .gpsr.bits = BIT(3),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(9), .ipsr.bits = 12, .ipsr.val = 0 }, /* SD2_DAT1 */
            { .gpsr.reg = GPSR(4), .gpsr.bits = BIT(4),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(9), .ipsr.bits = 16, .ipsr.val = 0 }, /* SD2_DAT2 */
            { .gpsr.reg = GPSR(4), .gpsr.bits = BIT(5),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(9), .ipsr.bits = 20, .ipsr.val = 0 }, /* SD2_DAT3 */
            { .gpsr.reg = GPSR(4), .gpsr.bits = BIT(6),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(9), .ipsr.bits = 24, .ipsr.val = 0 }, /* SD2_DS */
            { { 0 } }
        },
        .settings = (rcar_pin[])
        {
            { .reg = PUEN(3), .bits = BITS(18,28), .val = PUEN_DISABLE },               /* Disable all SD1, SD2 Pins Pull-up/down */
            { .reg = DRVCTRL(14), .bits = DRV(DRV3, 0),  .val = DRVCAP(DRV3_1_2, 0)  }, /* SD2_CMD */
            { .reg = DRVCTRL(14), .bits = DRV(DRV3, 4),  .val = DRVCAP(DRV3_1_2, 4)  }, /* SD2_CLK */
            { .reg = DRVCTRL(14), .bits = DRV(DRV3, 8),  .val = DRVCAP(DRV3_1_2, 8)  }, /* SD1_DAT3 */
            { .reg = DRVCTRL(14), .bits = DRV(DRV3, 12), .val = DRVCAP(DRV3_1_2, 12) }, /* SD1_DAT2 */
            { .reg = DRVCTRL(14), .bits = DRV(DRV3, 16), .val = DRVCAP(DRV3_1_2, 16) }, /* SD1_DAT1 */
            { .reg = DRVCTRL(14), .bits = DRV(DRV3, 20), .val = DRVCAP(DRV3_1_2, 20) }, /* SD1_DAT0 */
            { .reg = DRVCTRL(15), .bits = DRV(DRV3, 28), .val = DRVCAP(DRV3_1_2, 28) }, /* SD2_DAT0 */
            { .reg = DRVCTRL(15), .bits = DRV(DRV3, 24), .val = DRVCAP(DRV3_1_2, 24) }, /* SD2_DAT1 */
            { .reg = DRVCTRL(15), .bits = DRV(DRV3, 20), .val = DRVCAP(DRV3_1_2, 20) }, /* SD2_DAT2 */
            { .reg = DRVCTRL(15), .bits = DRV(DRV3, 16), .val = DRVCAP(DRV3_1_2, 16) }, /* SD2_DAT3 */
            { .reg = DRVCTRL(15), .bits = DRV(DRV3, 12), .val = DRVCAP(DRV3_1_2, 12) }, /* SD2_DS */
            { .reg = POCCTRL(0), .bits = BITS(6,18), .val = POC_1_8V },                 /* Set SD1, SD2 POCCTRL to 1.8V */
            { .reg = TDSELCTRL(0), .bits = BITS(2,5), .val = TDSEL(TDSEL_30PF, 2)|TDSEL(TDSEL_30PF, 4) }, /* Set SD1, SD2 TDSEL to 30 pF */
            { 0 }
        }
    },
    [DEV_DMAC] = {
        .mod = (rcar_module[])
        {
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(5), .clk.bits = BIT(2),  .base = RCAR_GEN3_AUDIODMAC0_BASE }, /* Audio-DMAC0 */
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(5), .clk.bits = BIT(1),  .base = RCAR_GEN3_AUDIODMAC1_BASE }, /* Audio-DMAC1 */
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(2), .clk.bits = BIT(19), .base = RCAR_GEN3_SYSDMAC0_BASE },   /* SYS-DMAC0 */
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(2), .clk.bits = BIT(18), .base = RCAR_GEN3_SYSDMAC1_BASE },   /* SYS-DMAC1 */
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(2), .clk.bits = BIT(17), .base = RCAR_GEN3_SYSDMAC2_BASE },   /* SYS-DMAC2 */
            { { 0 } }
        },
        .blk_config = config_dmac,
    },
    [DEV_PCI] = {
        .mod = (rcar_module[])
        {
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(3), .clk.bits = BIT(19) }, /* PCIEC0 */
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(3), .clk.bits = BIT(18) }, /* PCIEC1 */
            { {0} }
        }
    },
    [DEV_THERMAL] = {
        .mod = (rcar_module[])
        {
            { .clk.val = ENABLE,  .clk.reg = SMSTPCR(5), .clk.bits = BIT(22) }, /* THS */
            { {0} }
        }
    },
    [DEV_GRAPHICS] = {
        .pwr = (rcar_pwr[])
        {
            { .base = PWRONCR(2), .setting = 0x1F, .status = PWRSR(2), .power_on = 0x3E0 }, /* 3DG */
            { 0 }
        },
        .mod = (rcar_module[])
        {
            { .clk.val = ENABLE,  .clk.reg = SMSTPCR(1), .clk.bits = BIT(12) }, /* 3DGE */
            { { 0 } }
        }
    },
    [DEV_DISPLAY] = {
        .pwr = (rcar_pwr[])
        {
            { .base = PWRONCR(8), .setting = 0x1, .status = PWRSR(8), .power_on = (1 << 4) }, /* A3VP */
            { .base = PWRONCR(9), .setting = 0x1, .status = PWRSR(9), .power_on = (1 << 4) }, /* A3VC */
            { 0 }
        },
        .mod = (rcar_module[])
        {
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(7), .clk.bits = BIT(29) }, /* HDMI-IF0 */
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(7), .clk.bits = BIT(27) }, /* LVDS-IF */
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(7), .clk.bits = BIT(24) }, /* DU0 */
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(7), .clk.bits = BIT(23) }, /* DU1 */
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(7), .clk.bits = BIT(22) }, /* DU2 */
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(7), .clk.bits = BIT(21) }, /* DU3, only used on M3-N.  No effect on M3-W */
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(6), .clk.bits = BIT(31) }, /* VSPI0 */
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(6), .clk.bits = BIT(26) }, /* VSPBD */
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(6), .clk.bits = BIT(24) }, /* VSPBC */
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(6), .clk.bits = BIT(23) }, /* VSPD0 */
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(6), .clk.bits = BIT(22) }, /* VSPD1 */
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(6), .clk.bits = BIT(21) }, /* VSPD2 */
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(6), .clk.bits = BIT(19) }, /* FCPCS */
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(6), .clk.bits = BIT(17) }, /* FCPCI0 */
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(6), .clk.bits = BIT(11) }, /* FCPVI0 */
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(6), .clk.bits = BIT(7)  },  /* FCPVB0 */
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(6), .clk.bits = BIT(3)  },  /* FCPVD0 */
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(6), .clk.bits = BIT(2)  },  /* FCPVD1 */
            { .clk.val = ENABLE, .clk.reg = SMSTPCR(6), .clk.bits = BIT(1)  },  /* FCPVD2 */
            { { 0 } }
        },
        .cpg = (rcar_pin[])
        {
            { .reg = RCAR_GEN3_CPG_HDMICKCR, .bits = 0xFFFFFFFF, .val = 0x1F },
            { 0 }
        },
        .pinmux = (rcar_pinmux[])
        {
            { .gpsr.reg = GPSR(0), .gpsr.bits = BIT(8),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(6), .ipsr.bits = 12, .ipsr.val = 6 }, /* DU_DR0/D8 */
            { .gpsr.reg = GPSR(0), .gpsr.bits = BIT(9),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(6), .ipsr.bits = 16, .ipsr.val = 6 }, /* DU_DR1/D9 */
            { .gpsr.reg = GPSR(0), .gpsr.bits = BIT(10), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(6), .ipsr.bits = 20, .ipsr.val = 6 }, /* DU_DR2/D10 */
            { .gpsr.reg = GPSR(0), .gpsr.bits = BIT(11), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(6), .ipsr.bits = 24, .ipsr.val = 6 }, /* DU_DR3/D11 */
            { .gpsr.reg = GPSR(0), .gpsr.bits = BIT(12), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(6), .ipsr.bits = 28, .ipsr.val = 6 }, /* DU_DR4/D12 */
            { .gpsr.reg = GPSR(0), .gpsr.bits = BIT(13), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(7), .ipsr.bits = 0,  .ipsr.val = 6 }, /* DU_DR5/D13 */
            { .gpsr.reg = GPSR(0), .gpsr.bits = BIT(14), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(7), .ipsr.bits = 4,  .ipsr.val = 6 }, /* DU_DR6/D14 */
            { .gpsr.reg = GPSR(0), .gpsr.bits = BIT(15), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(7), .ipsr.bits = 8,  .ipsr.val = 6 }, /* DU_DR7/D15 */
            { .gpsr.reg = GPSR(1), .gpsr.bits = BIT(0),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(1), .ipsr.bits = 28, .ipsr.val = 6 }, /* DU_DB0/A0 */
            { .gpsr.reg = GPSR(1), .gpsr.bits = BIT(1),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(2), .ipsr.bits = 0,  .ipsr.val = 6 }, /* DU_DB1/A1 */
            { .gpsr.reg = GPSR(1), .gpsr.bits = BIT(2),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(2), .ipsr.bits = 4,  .ipsr.val = 6 }, /* DU_DB2/A2 */
            { .gpsr.reg = GPSR(1), .gpsr.bits = BIT(3),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(2), .ipsr.bits = 8,  .ipsr.val = 6 }, /* DU_DB3/A3 */
            { .gpsr.reg = GPSR(1), .gpsr.bits = BIT(4),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(2), .ipsr.bits = 12, .ipsr.val = 6 }, /* DU_DB4/A4 */
            { .gpsr.reg = GPSR(1), .gpsr.bits = BIT(5),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(2), .ipsr.bits = 16, .ipsr.val = 6 }, /* DU_DB5/A5 */
            { .gpsr.reg = GPSR(1), .gpsr.bits = BIT(6),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(2), .ipsr.bits = 20, .ipsr.val = 6 }, /* DU_DB6/A6 */
            { .gpsr.reg = GPSR(1), .gpsr.bits = BIT(7),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(2), .ipsr.bits = 24, .ipsr.val = 6 }, /* DU_DB7/A7 */
            { .gpsr.reg = GPSR(1), .gpsr.bits = BIT(12), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(3), .ipsr.bits = 12, .ipsr.val = 6 }, /* DU_DG4/A12 */
            { .gpsr.reg = GPSR(1), .gpsr.bits = BIT(13), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(3), .ipsr.bits = 16, .ipsr.val = 6 }, /* DU_DG5/A13 */
            { .gpsr.reg = GPSR(1), .gpsr.bits = BIT(14), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(3), .ipsr.bits = 20, .ipsr.val = 6 }, /* DU_DG6/A14 */
            { .gpsr.reg = GPSR(1), .gpsr.bits = BIT(15), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(3), .ipsr.bits = 24, .ipsr.val = 6 }, /* DU_DG7/A15 */
            { .gpsr.reg = GPSR(1), .gpsr.bits = BIT(16), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(3), .ipsr.bits = 28, .ipsr.val = 6 }, /* DU_DG0/A16 */
            { .gpsr.reg = GPSR(1), .gpsr.bits = BIT(17), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(4), .ipsr.bits = 0,  .ipsr.val = 6 }, /* DU_DG1/A17 */
            { .gpsr.reg = GPSR(1), .gpsr.bits = BIT(18), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(4), .ipsr.bits = 4,  .ipsr.val = 6 }, /* DU_DG2/A18 */
            { .gpsr.reg = GPSR(1), .gpsr.bits = BIT(19), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(4), .ipsr.bits = 8,  .ipsr.val = 6 }, /* DU_DG3/A19 */
            { .gpsr.reg = GPSR(1), .gpsr.bits = BIT(27), .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(5), .ipsr.bits = 8,  .ipsr.val = 6 }, /* DU_DOTCLKOUT0/EX_WAIT0_A */
            { .gpsr.reg = GPSR(2), .gpsr.bits = BIT(1),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(0), .ipsr.bits = 28, .ipsr.val = 3 }, /* DU_DISP/IRQ1 */
            { .gpsr.reg = GPSR(2), .gpsr.bits = BIT(3),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(1), .ipsr.bits = 4,  .ipsr.val = 3 }, /* DU_DOTCLKOUT1/IRQ3 */
            { .gpsr.reg = GPSR(2), .gpsr.bits = BIT(4),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(1), .ipsr.bits = 8,  .ipsr.val = 3 }, /* DU_EXHSYNC/DU_HSYNC/IRQ4 */
            { .gpsr.reg = GPSR(2), .gpsr.bits = BIT(5),  .gpsr.val = GPSR_IP, .ipsr.reg = IPSR(1), .ipsr.bits = 12, .ipsr.val = 3 }, /* DU_EXVSYNC/DU_VSYNC/IRQ5 */
            { .gpsr.reg = GPSR(7), .gpsr.bits = BIT(2),  .gpsr.val = GPSR_PF }, /* HDMI0_CEC */
            { { 0 } }
        },
        .settings = (rcar_pin[])
        {
            { .reg = DRVCTRL(10), .bits = DRV(DRV3, 0)|DRV(DRV3, 4)|DRV(DRV3, 8)|DRV(DRV3, 12)|DRV(DRV3, 16)|DRV(DRV3, 20),
                                  .val = DRVCAP(DRV3_1_2, 0)|DRVCAP(DRV3_1_2, 4)|DRVCAP(DRV3_1_2, 8)|DRVCAP(DRV3_1_2, 12)|DRVCAP(DRV3_1_2, 16)|DRVCAP(DRV3_1_2, 20) }, /* DR0-5 */
            { .reg = DRVCTRL(11), .bits = DRV(DRV3, 24)|DRV(DRV3, 28),
                                  .val = DRVCAP(DRV3_1_2, 24)|DRVCAP(DRV3_1_2, 28) }, /* DR6-7 */
            { .reg = DRVCTRL(5),  .bits = DRV(DRV3, 0)|DRV(DRV3, 4)|DRV(DRV3, 8)|DRV(DRV3, 12),
                                  .val = DRVCAP(DRV3_1_2, 0)|DRVCAP(DRV3_1_2, 4)|DRVCAP(DRV3_1_2, 8)|DRVCAP(DRV3_1_2, 12) }, /* DB0-3 */
            { .reg = DRVCTRL(6),  .bits = DRV(DRV3, 16)|DRV(DRV3, 20)|DRV(DRV3, 24)|DRV(DRV3, 28),
                                  .val = DRVCAP(DRV3_1_2, 16)|DRVCAP(DRV3_1_2, 20)|DRVCAP(DRV3_1_2, 24)|DRVCAP(DRV3_1_2, 28) }, /* DB4-7 */
            { .reg = DRVCTRL(7),  .bits = DRV(DRV3, 0)|DRV(DRV3, 4)|DRV(DRV3, 8)|DRV(DRV3, 12)|DRV(DRV3, 16)|DRV(DRV3, 20)|DRV(DRV3, 24)|DRV(DRV3, 28),
                                  .val = DRVCAP(DRV3_1_2, 0)|DRVCAP(DRV3_1_2, 4)|DRVCAP(DRV3_1_2, 8)|DRVCAP(DRV3_1_2, 12)|DRVCAP(DRV3_1_2, 16)|DRVCAP(DRV3_1_2, 20)|DRVCAP(DRV3_1_2, 24)|DRVCAP(DRV3_1_2, 28) }, /* DG0-7 */
            { 0 }
        }
    },
};

#endif //__RCAR_SOC_BOARD_M3_H_INCLUDED

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/boards/rcar_gen3/rcar_m3/rcar_soc_board.h $ $Rev: 851259 $")
#endif
