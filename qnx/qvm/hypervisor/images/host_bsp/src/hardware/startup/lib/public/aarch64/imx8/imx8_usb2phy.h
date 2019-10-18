/*
* $QNXLicenseC:
* Copyright 2016, QNX Software Systems.
* Copyright 2016, Freescale Semiconductor, Inc.
* Copyright 2017 NXP
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

#ifndef IMX_USB2PHY_H_
#define IMX_USB2PHY_H_

/* USB2PHY registers, offset from base address */
#define IMX_USB2PHYx_PWD          0x0000  /* USB2PHYx Power-Down Register offset */
#define IMX_USB2PHYx_PWD_SET      0x0004  /* USB2PHYx Power-Down Register set offset */
#define IMX_USB2PHYx_PWD_CLR      0x0008  /* USB2PHYx Power-Down Register clear offset */
#define IMX_USB2PHYx_PWD_TOG      0x000C  /* USB2PHYx Power-Down Register toggle offset */
#define IMX_USB2PHYx_TX           0x0010  /* USB2PHYx Transmitter Control Register offset */
#define IMX_USB2PHYx_TX_SET       0x0014  /* USB2PHYx Transmitter Control Register set offset */
#define IMX_USB2PHYx_TX_CLR       0x0018  /* USB2PHYx Transmitter Control Register clear offset */
#define IMX_USB2PHYx_TX_TOG       0x001C  /* USB2PHYx Transmitter Control Register toggle offset */
#define IMX_USB2PHYx_RX           0x0020  /* USB2PHYx Receiver Control Register offset */
#define IMX_USB2PHYx_RX_SET       0x0024  /* USB2PHYx Receiver Control Register set offset */
#define IMX_USB2PHYx_RX_CLR       0x0028  /* USB2PHYx Receiver Control Register clear offset */
#define IMX_USB2PHYx_RX_TOG       0x002C  /* USB2PHYx Receiver Control Register toggle offset */
#define IMX_USB2PHYx_CTRL         0x0030  /* USB2PHYx General Control Register offset */
#define IMX_USB2PHYx_CTRL_SET     0x0034  /* USB2PHYx General Control Register set offset */
#define IMX_USB2PHYx_CTRL_CLR     0x0038  /* USB2PHYx General Control Register clear offset */
#define IMX_USB2PHYx_CTRL_TOG     0x003C  /* USB2PHYx General Control Register toggle offset */
#define IMX_USB2PHYx_PLL_SIC      0x00A0  /* USB2PHYx PLL Control Register offset */
#define IMX_USB2PHYx_PLL_SIC_SET  0x00A4  /* USB2PHYx PLL Control Register set offset */
#define IMX_USB2PHYx_PLL_SIC_CLR  0x00A8  /* USB2PHYx PLL Control Register clear offset */
#define IMX_USB2PHYx_PLL_SIC_TOG  0x00AC  /* USB2PHYx PLL Control Register toggle offset */
#define IMX_USB2PHYx_CHRG_DETECT      0x00E0  /* USB2PHYx Charge Detect Control Register offset */
#define IMX_USB2PHYx_CHRG_DETECT_SET  0x00E4  /* USB2PHYx Charge Detect Control Register set offset */
#define IMX_USB2PHYx_CHRG_DETECT_CLR  0x00E8  /* USB2PHYx Charge Detect Control Register clear offset */
#define IMX_USB2PHYx_CHRG_DETECT_TOG  0x00EC  /* USB2PHYx Charge Detect Control Register toggle offset */

/*
 * USB2PHYx_PWD register bits *
 */
#define IMX_USB2PHYx_PWD_RXPWDRX_MASK                 0x00100000
#define IMX_USB2PHYx_PWD_RXPWDRX_SHIFT                20
#define IMX_USB2PHYx_PWD_RXPWDDIFF_MASK               0x00080000
#define IMX_USB2PHYx_PWD_RXPWDDIFF_SHIFT              19
#define IMX_USB2PHYx_PWD_RXPWD1PT1_MASK               0x00040000
#define IMX_USB2PHYx_PWD_RXPWD1PT1_SHIFT              18
#define IMX_USB2PHYx_PWD_RXPWDENV_MASK                0x00020000
#define IMX_USB2PHYx_PWD_RXPWDENV_SHIFT               17
#define IMX_USB2PHYx_PWD_TXPWDV2I_MASK                0x00001000
#define IMX_USB2PHYx_PWD_TXPWDV2I_SHIFT               12
#define IMX_USB2PHYx_PWD_TXPWDIBIAS_MASK              0x00000800
#define IMX_USB2PHYx_PWD_TXPWDIBIAS_SHIFT             11
#define IMX_USB2PHYx_PWD_TXPWDFS_MASK                 0x00000400
#define IMX_USB2PHYx_PWD_TXPWDFS_SHIFT                10

/*
 * USB2PHYx_TX register bits *
 */
#define IMX_USB2PHYx_TX_USB2PHY_TX_EDGECTRL_MASK      0x1C000000
#define IMX_USB2PHYx_TX_USB2PHY_TX_EDGECTRL_SHIFT     26
#define IMX_USB2PHYx_TX_TXCAL45DP_MASK                0x000F0000
#define IMX_USB2PHYx_TX_TXCAL45DP_SHIFT               16
#define IMX_USB2PHYx_TX_TXCAL45DN_MASK                0x00000F00
#define IMX_USB2PHYx_TX_TXCAL45DN_SHIFT               8
#define IMX_USB2PHYx_TX_D_CAL_MASK                    0x0000000F
#define IMX_USB2PHYx_TX_D_CAL_SHIFT                   0

/*
 * USB2PHYx_RX register bits *
 */
#define IMX_USB2PHYx_RX_RXDBYPASS_MASK                0x00400000
#define IMX_USB2PHYx_RX_RXDBYPASS_SHIFT               22
#define IMX_USB2PHYx_RX_DISCONADJ_MASK                0x00000070
#define IMX_USB2PHYx_RX_DISCONADJ_SHIFT               4
#define IMX_USB2PHYx_RX_ENVADJ_MASK                   0x00000007
#define IMX_USB2PHYx_RX_ENVADJ_SHIFT                  0

/*
 * USB2PHYx_CTRL register bits *
 */

#define IMX_USB2PHYx_CTRL_SFTRST_MASK                 0x80000000
#define IMX_USB2PHYx_CTRL_SFTRST_SHIFT                31
#define IMX_USB2PHYx_CTRL_CLKGATE_MASK                0x40000000
#define IMX_USB2PHYx_CTRL_CLKGATE_SHIFT               30
#define IMX_USB2PHYx_CTRL_UTMI_SUSPENDM_MASK          0x20000000
#define IMX_USB2PHYx_CTRL_UTMI_SUSPENDM_SHIFT         29
#define IMX_USB2PHYx_CTRL_HOST_FORCE_LS_SE0_MASK      0x10000000
#define IMX_USB2PHYx_CTRL_HOST_FORCE_LS_SE0_SHIFT     28
#define IMX_USB2PHYx_CTRL_OTG_ID_VALUE_MASK           0x08000000
#define IMX_USB2PHYx_CTRL_OTG_ID_VALUE_SHIFT          27
#define IMX_USB2PHYx_CTRL_ENAUTOSET_USBCLKS_MASK      0x04000000
#define IMX_USB2PHYx_CTRL_ENAUTOSET_USBCLKS_SHIFT     26
#define IMX_USB2PHYx_CTRL_FSDLL_RST_EN_MASK           0x01000000
#define IMX_USB2PHYx_CTRL_FSDLL_RST_EN_SHIFT          24
#define IMX_USB2PHYx_CTRL_ENVBUSCHG_WKUP_MASK         0x00800000
#define IMX_USB2PHYx_CTRL_ENVBUSCHG_WKUP_SHIFT        23
#define IMX_USB2PHYx_CTRL_ENIDCHG_WKUP_MASK           0x00400000
#define IMX_USB2PHYx_CTRL_ENIDCHG_WKUP_SHIFT          22
#define IMX_USB2PHYx_CTRL_ENDPDMCHG_WKUP_MASK         0x00200000
#define IMX_USB2PHYx_CTRL_ENDPDMCHG_WKUP_SHIFT        21
#define IMX_USB2PHYx_CTRL_ENAUTOCLR_PHY_PWD_MASK      0x00100000
#define IMX_USB2PHYx_CTRL_ENAUTOCLR_PHY_PWD_SHIFT     20
#define IMX_USB2PHYx_CTRL_ENAUTOCLR_CLKGATE_MASK      0x00080000
#define IMX_USB2PHYx_CTRL_ENAUTOCLR_CLKGATE_SHIFT     19
#define IMX_USB2PHYx_CTRL_ENAUTO_PWRON_PLL_MASK       0x00040000
#define IMX_USB2PHYx_CTRL_ENAUTO_PWRON_PLL_SHIFT      18
#define IMX_USB2PHYx_CTRL_WAKEUP_IRQ_MASK             0x00020000
#define IMX_USB2PHYx_CTRL_WAKEUP_IRQ_SHIFT            17
#define IMX_USB2PHYx_CTRL_ENIRQWAKEUP_MASK            0x00010000
#define IMX_USB2PHYx_CTRL_ENIRQWAKEUP_SHIFT           16
#define IMX_USB2PHYx_CTRL_ENUTMILEVEL3_MASK           0x00008000
#define IMX_USB2PHYx_CTRL_ENUTMILEVEL3_SHIFT          15
#define IMX_USB2PHYx_CTRL_ENUTMILEVEL2_MASK           0x00004000
#define IMX_USB2PHYx_CTRL_ENUTMILEVEL2_SHIFT          14
#define IMX_USB2PHYx_CTRL_DATA_ON_LRADC_MASK          0x00002000
#define IMX_USB2PHYx_CTRL_DATA_ON_LRADC_SHIFT         13
#define IMX_USB2PHYx_CTRL_DEVPLUGIN_IRQ_MASK          0x00001000
#define IMX_USB2PHYx_CTRL_DEVPLUGIN_IRQ_SHIFT         12
#define IMX_USB2PHYx_CTRL_ENIRQDEVPLUGIN_MASK         0x00000800
#define IMX_USB2PHYx_CTRL_ENIRQDEVPLUGIN_SHIFT        11
#define IMX_USB2PHYx_CTRL_RESUME_IRQ_MASK             0x00000400
#define IMX_USB2PHYx_CTRL_RESUME_IRQ_SHIFT            10
#define IMX_USB2PHYx_CTRL_ENIRQRESUMEDETECT_MASK      0x00000200
#define IMX_USB2PHYx_CTRL_ENIRQRESUMEDETECT_SHIFT     9
#define IMX_USB2PHYx_CTRL_RESUMEIRQSTICKY_MASK        0x00000100
#define IMX_USB2PHYx_CTRL_RESUMEIRQSTICKY_SHIFT       8
#define IMX_USB2PHYx_CTRL_ENOTGIDDETECT_MASK          0x00000080
#define IMX_USB2PHYx_CTRL_ENOTGIDDETECT_SHIFT         7
#define IMX_USB2PHYx_CTRL_OTG_ID_CHG_IRQ_MASK         0x00000040
#define IMX_USB2PHYx_CTRL_OTG_ID_CHG_IRQ_SHIFT        6
#define IMX_USB2PHYx_CTRL_DEVPLUGIN_POLARITY_MASK     0x00000020
#define IMX_USB2PHYx_CTRL_DEVPLUGIN_POLARITY_SHIFT    5
#define IMX_USB2PHYx_CTRL_ENDEVPLUGINDETECT_MASK      0x00000010
#define IMX_USB2PHYx_CTRL_ENDEVPLUGINDETECT_SHIFT     4
#define IMX_USB2PHYx_CTRL_HOSTDISCONDETECT_IRQ_MASK   0x00000008
#define IMX_USB2PHYx_CTRL_HOSTDISCONDETECT_IRQ_SHIFT  3
#define IMX_USB2PHYx_CTRL_ENIRQHOSTDISCON_MASK        0x00000004
#define IMX_USB2PHYx_CTRL_ENIRQHOSTDISCON_SHIFT       2
#define IMX_USB2PHYx_CTRL_ENHOSTDISCONDETECT_MASK     0x00000002
#define IMX_USB2PHYx_CTRL_ENHOSTDISCONDETECT_SHIFT    1
#define IMX_USB2PHYx_CTRL_ENOTG_ID_CHG_IRQ_MASK       0x00000001
#define IMX_USB2PHYx_CTRL_ENOTG_ID_CHG_IRQ_SHIFT      0

/*
 * USBPHY_PLL_SICn register bits *
 */
#define IMX_USB2PHYx_PLL_LOCK_MASK                     0x80000000
#define IMX_USB2PHYx_PLL_LOCK_SHIFT                    31
#define IMX_USB2PHYx_PLL_DIV_SELECT_MASK               0x01C00000
#define IMX_USB2PHYx_PLL_DIV_SELECT_SHIFT              22
#define IMX_USB2PHYx_PLL_REG_ENABLE_MASK               0x00200000
#define IMX_USB2PHYx_PLL_REG_ENABLE_SHIFT              21
#define IMX_USB2PHYx_PLL_BYPASS_MASK                   0x00010000
#define IMX_USB2PHYx_PLL_BYPASS_SHIFT                  16
#define IMX_USB2PHYx_PLL_ENABLE_MASK                   0x00002000
#define IMX_USB2PHYx_PLL_ENABLE_SHIFT                  13
#define IMX_USB2PHYx_PLL_POWER_MASK                    0x00001000
#define IMX_USB2PHYx_PLL_POWER_SHIFT                   12
#define IMX_USB2PHYx_PLL_EN_USB_CLKS_MASK              0x00000040
#define IMX_USB2PHYx_PLL_EN_USB_CLKS_SHIFT             6

/*
 * USB_ANALOG_CHRG_DETECT register bits *
 */
#define IMX_USB2PHYx_CHRG_DETECT_PULLUP_DP_MASK        0x00000004
#define IMX_USB2PHYx_CHRG_DETECT_PULLUP_DP_SHIFT       2

#endif /* IMX_USB2PHY_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/aarch64/imx8/imx8_usb2phy.h $ $Rev: 837679 $")
#endif
