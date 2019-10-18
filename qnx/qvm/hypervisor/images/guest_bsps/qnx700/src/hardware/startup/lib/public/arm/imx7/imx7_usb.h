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

#ifndef IMX7_USB_OTG_H_
#define IMX7_USB_OTG_H_

/*
 * USBOTG1 SOC features *
 */
#define IMX7_USBOTG1_BASE                        0x30B10000
#define IMX7_USBOTG1_SIZE                        0x10000
#define IMX7_USBOTG1_IRQ                         75

/*
 * USBOTG2 SOC features *
 */
#define IMX7_USBOTG2_BASE                        0x30B20000
#define IMX7_USBOTG2_SIZE                        0x10000
#define IMX7_USBOTG2_IRQ                         74

/*
 * USBHOST SOC features *
 */
#define IMX7_USBHOST_BASE                        0x30B30000
#define IMX7_USBHOST_SIZE                        0x10000
#define IMX7_USBHOST_IRQ                         72

/* USB_OTG registers, offset from base address */
#define IMX7_USB_ID                 0x0000  /* Identification register offset */
#define IMX7_USB_HWGENERAL          0x0004  /* General hardware parameters register offset */
#define IMX7_USB_HWHOST             0x0008  /* Host hardware parameter register offset */
#define IMX7_USB_HWDEVICE           0x000C  /* Device hardware parameter register offset */
#define IMX7_USB_HWTXBUF            0x0010  /* TX buffer hardware parameters register offset */
#define IMX7_USB_HWRXBUF            0x0014  /* RX buffer hardware parameters register offset */
#define IMX7_USB_GPTIMER0LD         0x0080  /* General purpose timer #0 Load register offset */
#define IMX7_USB_GPTIMER0CTRL       0x0084  /* General purpose timer #0 controller register offset */
#define IMX7_USB_GPTIMER1LD         0x0088  /* General purpose timer #1 Load register offset */
#define IMX7_USB_GPTIMER1CTRL       0x008C  /* General purpose timer #1 controller register offset */
#define IMX7_USB_SBUSCFG            0x0090  /* System bus config register offset */
#define IMX7_USB_CAPLENGTH          0x0100  /* Capability Registers Length offset */
#define IMX7_USB_HCIVERSION         0x0102  /* Host Controller Interface Version offset */
#define IMX7_USB_HCSPARAMS          0x0104  /* Identification register offset */
#define IMX7_USB_HCCPARAMS          0x0108  /* Identification register offset */
#define IMX7_USB_DCIVERSION         0x0120  /* Device Controller Interface Version offset */
#define IMX7_USB_DCCPARAMS          0x0124  /* Identification register offset */
#define IMX7_USB_USBCMD             0x0140  /* USB Command Register offset */
#define IMX7_USB_USBSTS             0x0144  /* USB Status Register offset */
#define IMX7_USB_USBINTR            0x0148  /* Interrupt Enable Register offset */
#define IMX7_USB_FRINDEX            0x014C  /* USB Frame Index offset */
#define IMX7_USB_PERIODICLISTBASE   0x0154  /* Frame List Base Address offset */
#define IMX7_USB_ASYNCLISTADDR      0x0158  /* Next Asynch. Address offset */
#define IMX7_USB_BURSTSIZE          0x0160  /* Programmable Burst Size offset */
#define IMX7_USB_TXFILLTUNING       0x0164  /* TX FIFO Fill Tuning offset */
#define IMX7_USB_ENDPTNAK           0x0178  /* Endpoint NAK offset */
#define IMX7_USB_ENDPTNAKEN         0x017C  /* Endpoint NAK Enable offset */
#define IMX7_USB_CONFIGFLAG         0x0180  /* Configure Flag Register offset */
#define IMX7_USB_PORTSC1            0x0184  /* Port Status & Control offset */
#define IMX7_USB_OTGSC              0x01A4  /* On-The-Go Status & control offset */
#define IMX7_USB_USBMODE            0x01A8  /* USB Device Mode offset */
#define IMX7_USB_ENDPTSETUPSTAT     0x01AC  /* Endpoint Setup Status offset */
#define IMX7_USB_ENDPTPRIME         0x01B0  /* Endpoint Prime offset */
#define IMX7_USB_ENDPTFLUSH         0x01B4  /* Endpoint Flush offset */
#define IMX7_USB_ENDPTSTAT          0x01B8  /* Endpoint Status offset */
#define IMX7_USB_ENDPTCOMPLETE      0x01BC  /* Endpoint Complete offset */
#define IMX7_USB_ENDPTCTRL0         0x01C0  /* Endpoint 0 settings offset */
#define IMX7_USB_ENDPTCTRL1         0x01C4  /* Endpoint settings offset */
#define IMX7_USB_ENDPTCTRL2         0x01C8  /* Endpoint settings offset */
#define IMX7_USB_ENDPTCTRL3         0x01CC  /* Endpoint settings offset */
#define IMX7_USB_ENDPTCTRL4         0x01D0  /* Endpoint settings offset */
#define IMX7_USB_ENDPTCTRL5         0x01D4  /* Endpoint settings offset */
#define IMX7_USB_ENDPTCTRL6         0x01D8  /* Endpoint settings offset */
#define IMX7_USB_ENDPTCTRL7         0x01DC  /* Endpoint settings offset */
#define IMX7_USBNC_OTG_CTRL1        0x0200  /* USBNC_n_CTRL1 offset */
#define IMX7_USBNC_OTG_CTRL2        0x0204  /* USBNC_n_CTRL2 offset */
#define IMX7_USBNC_OTG_PHY_CFG1     0x0230  /* USB OTG PHY Configuration Register 1 offset */
#define IMX7_USBNC_OTG_PHY_CFG2     0x0234  /* USB OTG PHY Configuration Register 2 offset */
#define IMX7_USBNC_PHY_STATUS       0x023C  /* USB OTG PHY Status Register offset */
#define IMX7_USBNC_UH_HSICPHY_CFG1  0x0240  /* USB Host HSIC PHY configuration register offset */
#define IMX7_USBNC_ADP_CFG1         0x0250  /* USBNC_ADP_CFG1 offset */
#define IMX7_USBNC_ADP_CFG2         0x0254  /* USBNC_ADP_CFG2 offset */
#define IMX7_USBNC_ADP_STATUS       0x0258  /* USBNC_ADP_STATUS offset */

/*
 * USB_ID register bits *
 */
#define IMX7_USB_ID_Revision_MASK                              0x00FF0000
#define IMX7_USB_ID_Revision_SHIFT                             16
#define IMX7_USB_ID_NID_MASK                                   0x00003F00
#define IMX7_USB_ID_NID_SHIFT                                  8
#define IMX7_USB_ID_ID_MASK                                    0x0000003F
#define IMX7_USB_ID_ID_SHIFT                                   0

/*
 * USB_HWGENERAL register bits *
 */
#define IMX7_USB_HWGENERAL_SM_MASK                             0x00000C00
#define IMX7_USB_HWGENERAL_SM_SHIFT                            10
#define IMX7_USB_HWGENERAL_PHYM_MASK                           0x000003C0
#define IMX7_USB_HWGENERAL_PHYM_SHIFT                          6
#define IMX7_USB_HWGENERAL_PHYW_MASK                           0x00000030
#define IMX7_USB_HWGENERAL_PHYW_SHIFT                          4

/*
 * USB_HWHOST register bits *
 */
#define IMX7_USB_HWHOST_NPORT_MASK                             0x0000000E
#define IMX7_USB_HWHOST_NPORT_SHIFT                            1
#define IMX7_USB_HWHOST_HC_MASK                                0x00000001
#define IMX7_USB_HWHOST_HC_SHIFT                               0

/*
 * USB_HWDEVICE register bits *
 */
#define IMX7_USB_HWDEVICE_DEVEP_MASK                           0x0000003E
#define IMX7_USB_HWDEVICE_DEVEP_SHIFT                          1
#define IMX7_USB_HWDEVICE_DC_MASK                              0x00000001
#define IMX7_USB_HWDEVICE_DC_SHIFT                             0

/*
 * USB_HWTXBUF register bits *
 */
#define IMX7_USB_HWTXBUF_TXCHANADD_MASK                        0x00FF0000
#define IMX7_USB_HWTXBUF_TXCHANADD_SHIFT                       16
#define IMX7_USB_HWTXBUF_TXBURST_MASK                          0x000000FF
#define IMX7_USB_HWTXBUF_TXBURST_SHIFT                         0

/*
 * USB_HWRXBUF register bits *
 */
#define IMX7_USB_HWRXBUF_RXADD_MASK                            0x0000FF00
#define IMX7_USB_HWRXBUF_RXADD_SHIFT                           8
#define IMX7_USB_HWRXBUF_RXBURST_MASK                          0x000000FF
#define IMX7_USB_HWRXBUF_RXBURST_SHIFT                         0

/*
 * USB_GPTIMER0LD register bits *
 */
#define IMX7_USB_GPTIMER0LD_GPTLD_MASK                         0x00FFFFFF
#define IMX7_USB_GPTIMER0LD_GPTLD_SHIFT                        0

/*
 * USB_GPTIMER0CTRL register bits *
 */
#define IMX7_USB_GPTIMER0CTRL_GPTRUN_MASK                      0x80000000
#define IMX7_USB_GPTIMER0CTRL_GPTRUN_SHIFT                     31
#define IMX7_USB_GPTIMER0CTRL_GPTRST_MASK                      0x40000000
#define IMX7_USB_GPTIMER0CTRL_GPTRST_SHIFT                     30
#define IMX7_USB_GPTIMER0CTRL_GPTMODE_MASK                     0x01000000
#define IMX7_USB_GPTIMER0CTRL_GPTMODE_SHIFT                    24
#define IMX7_USB_GPTIMER0CTRL_GPTCNT_MASK                      0x00FFFFFF
#define IMX7_USB_GPTIMER0CTRL_GPTCNT_SHIFT                     0

/*
 * USB_GPTIMER1LD register bits *
 */
#define IMX7_USB_GPTIMER1LD_GPTLD_MASK                         0x00FFFFFF
#define IMX7_USB_GPTIMER1LD_GPTLD_SHIFT                        0

/*
 * USB_GPTIMER1CTRL register bits *
 */
#define IMX7_USB_GPTIMER1CTRL_GPTRUN_MASK                      0x80000000
#define IMX7_USB_GPTIMER1CTRL_GPTRUN_SHIFT                     31
#define IMX7_USB_GPTIMER1CTRL_GPTRST_MASK                      0x40000000
#define IMX7_USB_GPTIMER1CTRL_GPTRST_SHIFT                     30
#define IMX7_USB_GPTIMER1CTRL_GPTMODE_MASK                     0x01000000
#define IMX7_USB_GPTIMER1CTRL_GPTMODE_SHIFT                    24
#define IMX7_USB_GPTIMER1CTRL_GPTCNT_MASK                      0x00FFFFFF
#define IMX7_USB_GPTIMER1CTRL_GPTCNT_SHIFT                     0

/*
 * USB_SBUSCFG register bits *
 */
#define IMX7_USB_SBUSCFG_AHBBRST_MASK                          0x00000007
#define IMX7_USB_SBUSCFG_AHBBRST_SHIFT                         0

/*
 * USB_CAPLENGTH register bits *
 */
#define IMX7_USB_CAPLENGTH_CAPLENGTH_MASK                      0x000000FF
#define IMX7_USB_CAPLENGTH_CAPLENGTH_SHIFT                     0

/*
 * USB_HCIVERSION register bits *
 */
#define IMX7_USB_HCIVERSION_HCIVERSION_MASK                    0x0000FFFF
#define IMX7_USB_HCIVERSION_HCIVERSION_SHIFT                   0

/*
 * USB_HCSPARAMS register bits *
 */
#define IMX7_USB_HCSPARAMS_N_TT_MASK                           0x0F000000
#define IMX7_USB_HCSPARAMS_N_TT_SHIFT                          24
#define IMX7_USB_HCSPARAMS_N_PTT_MASK                          0x00800000
#define IMX7_USB_HCSPARAMS_N_PTT_SHIFT                         23
#define IMX7_USB_HCSPARAMS_PI_MASK                             0x00010000
#define IMX7_USB_HCSPARAMS_PI_SHIFT                            16
#define IMX7_USB_HCSPARAMS_N_CC_MASK                           0x0000F000
#define IMX7_USB_HCSPARAMS_N_CC_SHIFT                          12
#define IMX7_USB_HCSPARAMS_N_PCC_MASK                          0x00000800
#define IMX7_USB_HCSPARAMS_N_PCC_SHIFT                         11
#define IMX7_USB_HCSPARAMS_PPC_MASK                            0x00000010
#define IMX7_USB_HCSPARAMS_PPC_SHIFT                           4
#define IMX7_USB_HCSPARAMS_N_PORTS_MASK                        0x0000000F
#define IMX7_USB_HCSPARAMS_N_PORTS_SHIFT                       0

/*
 * USB_HCCPARAMS register bits *
 */
#define IMX7_USB_HCCPARAMS_EECP_MASK                           0x0000FF00
#define IMX7_USB_HCCPARAMS_EECP_SHIFT                          8
#define IMX7_USB_HCCPARAMS_IST_MASK                            0x000000F0
#define IMX7_USB_HCCPARAMS_IST_SHIFT                           4
#define IMX7_USB_HCCPARAMS_ASP_MASK                            0x00000004
#define IMX7_USB_HCCPARAMS_ASP_SHIFT                           2
#define IMX7_USB_HCCPARAMS_PFL_MASK                            0x00000002
#define IMX7_USB_HCCPARAMS_PFL_SHIFT                           1
#define IMX7_USB_HCCPARAMS_ADC_MASK                            0x00000001
#define IMX7_USB_HCCPARAMS_ADC_SHIFT                           0

/*
 * USB_DCIVERSION register bits *
 */
#define IMX7_USB_DCIVERSION_DCIVERSION_MASK                    0x0000FFFF
#define IMX7_USB_DCIVERSION_DCIVERSION_SHIFT                   0

/*
 * USB_DCCPARAMS register bits *
 */
#define IMX7_USB_DCCPARAMS_HC_MASK                             0x00000100
#define IMX7_USB_DCCPARAMS_HC_SHIFT                            8
#define IMX7_USB_DCCPARAMS_DC_MASK                             0x00000080
#define IMX7_USB_DCCPARAMS_DC_SHIFT                            7
#define IMX7_USB_DCCPARAMS_DEN_MASK                            0x0000001F
#define IMX7_USB_DCCPARAMS_DEN_SHIFT                           0

/*
 * USB_USBCMD register bits *
 */
#define IMX7_USB_USBCMD_ITC_MASK                               0x00FF0000
#define IMX7_USB_USBCMD_ITC_SHIFT                              16
#define IMX7_USB_USBCMD_FS_2_MASK                              0x00008000
#define IMX7_USB_USBCMD_FS_2_SHIFT                             15
#define IMX7_USB_USBCMD_SUTW_MASK                              0x00002000
#define IMX7_USB_USBCMD_SUTW_SHIFT                             13
#define IMX7_USB_USBCMD_ATDTW_MASK                             0x00001000
#define IMX7_USB_USBCMD_ATDTW_SHIFT                            12
#define IMX7_USB_USBCMD_ASPE_MASK                              0x00000800
#define IMX7_USB_USBCMD_ASPE_SHIFT                             11
#define IMX7_USB_USBCMD_ASP_MASK                               0x00000300
#define IMX7_USB_USBCMD_ASP_SHIFT                              8
#define IMX7_USB_USBCMD_IAA_MASK                               0x00000040
#define IMX7_USB_USBCMD_IAA_SHIFT                              6
#define IMX7_USB_USBCMD_ASE_MASK                               0x00000020
#define IMX7_USB_USBCMD_ASE_SHIFT                              5
#define IMX7_USB_USBCMD_PSE_MASK                               0x00000010
#define IMX7_USB_USBCMD_PSE_SHIFT                              4
#define IMX7_USB_USBCMD_FS_1_MASK                              0x0000000C
#define IMX7_USB_USBCMD_FS_1_SHIFT                             2
#define IMX7_USB_USBCMD_RST_MASK                               0x00000002
#define IMX7_USB_USBCMD_RST_SHIFT                              1
#define IMX7_USB_USBCMD_RS_MASK                                0x00000001
#define IMX7_USB_USBCMD_RS_SHIFT                               0

/*
 * USB_USBSTS register bits *
 */
#define IMX7_USB_USBSTS_TI1_MASK                               0x02000000
#define IMX7_USB_USBSTS_TI1_SHIFT                              25
#define IMX7_USB_USBSTS_TI0_MASK                               0x01000000
#define IMX7_USB_USBSTS_TI0_SHIFT                              24
#define IMX7_USB_USBSTS_NAKI_MASK                              0x00010000
#define IMX7_USB_USBSTS_NAKI_SHIFT                             16
#define IMX7_USB_USBSTS_AS_MASK                                0x00008000
#define IMX7_USB_USBSTS_AS_SHIFT                               15
#define IMX7_USB_USBSTS_PS_MASK                                0x00004000
#define IMX7_USB_USBSTS_PS_SHIFT                               14
#define IMX7_USB_USBSTS_RCL_MASK                               0x00002000
#define IMX7_USB_USBSTS_RCL_SHIFT                              13
#define IMX7_USB_USBSTS_HCH_MASK                               0x00001000
#define IMX7_USB_USBSTS_HCH_SHIFT                              12
#define IMX7_USB_USBSTS_ULPII_MASK                             0x00000400
#define IMX7_USB_USBSTS_ULPII_SHIFT                            10
#define IMX7_USB_USBSTS_SLI_MASK                               0x00000100
#define IMX7_USB_USBSTS_SLI_SHIFT                              8
#define IMX7_USB_USBSTS_SRI_MASK                               0x00000080
#define IMX7_USB_USBSTS_SRI_SHIFT                              7
#define IMX7_USB_USBSTS_URI_MASK                               0x00000040
#define IMX7_USB_USBSTS_URI_SHIFT                              6
#define IMX7_USB_USBSTS_AAI_MASK                               0x00000020
#define IMX7_USB_USBSTS_AAI_SHIFT                              5
#define IMX7_USB_USBSTS_SEI_MASK                               0x00000010
#define IMX7_USB_USBSTS_SEI_SHIFT                              4
#define IMX7_USB_USBSTS_FRI_MASK                               0x00000008
#define IMX7_USB_USBSTS_FRI_SHIFT                              3
#define IMX7_USB_USBSTS_PCI_MASK                               0x00000004
#define IMX7_USB_USBSTS_PCI_SHIFT                              2
#define IMX7_USB_USBSTS_UEI_MASK                               0x00000002
#define IMX7_USB_USBSTS_UEI_SHIFT                              1
#define IMX7_USB_USBSTS_UI_MASK                                0x00000001
#define IMX7_USB_USBSTS_UI_SHIFT                               0

/*
 * USB_USBINTR register bits *
 */
#define IMX7_USB_USBINTR_TIE1_MASK                             0x02000000
#define IMX7_USB_USBINTR_TIE1_SHIFT                            25
#define IMX7_USB_USBINTR_TIE0_MASK                             0x01000000
#define IMX7_USB_USBINTR_TIE0_SHIFT                            24
#define IMX7_USB_USBINTR_UPIE_MASK                             0x00080000
#define IMX7_USB_USBINTR_UPIE_SHIFT                            19
#define IMX7_USB_USBINTR_UAIE_MASK                             0x00040000
#define IMX7_USB_USBINTR_UAIE_SHIFT                            18
#define IMX7_USB_USBINTR_NAKE_MASK                             0x00010000
#define IMX7_USB_USBINTR_NAKE_SHIFT                            16
#define IMX7_USB_USBINTR_ULPIE_MASK                            0x00000400
#define IMX7_USB_USBINTR_ULPIE_SHIFT                           10
#define IMX7_USB_USBINTR_SLE_MASK                              0x00000100
#define IMX7_USB_USBINTR_SLE_SHIFT                             8
#define IMX7_USB_USBINTR_SRE_MASK                              0x00000080
#define IMX7_USB_USBINTR_SRE_SHIFT                             7
#define IMX7_USB_USBINTR_URE_MASK                              0x00000040
#define IMX7_USB_USBINTR_URE_SHIFT                             6
#define IMX7_USB_USBINTR_AAE_MASK                              0x00000020
#define IMX7_USB_USBINTR_AAE_SHIFT                             5
#define IMX7_USB_USBINTR_SEE_MASK                              0x00000010
#define IMX7_USB_USBINTR_SEE_SHIFT                             4
#define IMX7_USB_USBINTR_FRE_MASK                              0x00000008
#define IMX7_USB_USBINTR_FRE_SHIFT                             3
#define IMX7_USB_USBINTR_PCE_MASK                              0x00000004
#define IMX7_USB_USBINTR_PCE_SHIFT                             2
#define IMX7_USB_USBINTR_UEE_MASK                              0x00000002
#define IMX7_USB_USBINTR_UEE_SHIFT                             1
#define IMX7_USB_USBINTR_UE_MASK                               0x00000001
#define IMX7_USB_USBINTR_UE_SHIFT                              0

/*
 * USB_FRINDEX register bits *
 */
#define IMX7_USB_FRINDEX_FRINDEX_MASK                          0x00003FFF
#define IMX7_USB_FRINDEX_FRINDEX_SHIFT                         0

/*
 * USB_PERIODICLISTBASE register bits *
 */
#define IMX7_USB_PERIODICLISTBASE_BASEADR_MASK                 0xFFFFF000
#define IMX7_USB_PERIODICLISTBASE_BASEADR_SHIFT                12

/*
 * USB_ASYNCLISTADDR register bits *
 */
#define IMX7_USB_ASYNCLISTADDR_ASYBAS_MASK                     0xFFFFFFE0
#define IMX7_USB_ASYNCLISTADDR_ASYBAS_SHIFT                    5

/*
 * USB_BURSTSIZE register bits *
 */
#define IMX7_USB_BURSTSIZE_TXPBURST_MASK                       0x0001FF00
#define IMX7_USB_BURSTSIZE_TXPBURST_SHIFT                      8
#define IMX7_USB_BURSTSIZE_RXPBURST_MASK                       0x000000FF
#define IMX7_USB_BURSTSIZE_RXPBURST_SHIFT                      0

/*
 * USB_TXFILLTUNING register bits *
 */
#define IMX7_USB_TXFILLTUNING_TXFIFOTHRES_MASK                 0x003F0000
#define IMX7_USB_TXFILLTUNING_TXFIFOTHRES_SHIFT                16
#define IMX7_USB_TXFILLTUNING_TXSCHHEALTH_MASK                 0x00001F00
#define IMX7_USB_TXFILLTUNING_TXSCHHEALTH_SHIFT                8
#define IMX7_USB_TXFILLTUNING_TXSCHOH_MASK                     0x000000FF
#define IMX7_USB_TXFILLTUNING_TXSCHOH_SHIFT                    0

/*
 * USB_ENDPTNAK register bits *
 */
#define IMX7_USB_ENDPTNAK_EPTN_MASK                            0x00FF0000
#define IMX7_USB_ENDPTNAK_EPTN_SHIFT                           16
#define IMX7_USB_ENDPTNAK_EPRN_MASK                            0x000000FF
#define IMX7_USB_ENDPTNAK_EPRN_SHIFT                           0

/*
 * USB_ENDPTNAKEN register bits *
 */
#define IMX7_USB_ENDPTNAKEN_EPTNE_MASK                         0x00FF0000
#define IMX7_USB_ENDPTNAKEN_EPTNE_SHIFT                        16
#define IMX7_USB_ENDPTNAKEN_EPRNE_MASK                         0x000000FF
#define IMX7_USB_ENDPTNAKEN_EPRNE_SHIFT                        0

/*
 * USB_CONFIGFLAG register bits *
 */
#define IMX7_USB_CONFIGFLAG_CF_MASK                            0x00000001
#define IMX7_USB_CONFIGFLAG_CF_SHIFT                           0

/*
 * USB_PORTSC1 register bits *
 */
#define IMX7_USB_PORTSC1_PTS_1_MASK                            0xC0000000
#define IMX7_USB_PORTSC1_PTS_1_SHIFT                           30
#define IMX7_USB_PORTSC1_STS_MASK                              0x20000000
#define IMX7_USB_PORTSC1_STS_SHIFT                             29
#define IMX7_USB_PORTSC1_PTW_MASK                              0x10000000
#define IMX7_USB_PORTSC1_PTW_SHIFT                             28
#define IMX7_USB_PORTSC1_PSPD_MASK                             0x0C000000
#define IMX7_USB_PORTSC1_PSPD_SHIFT                            26
#define IMX7_USB_PORTSC1_PTS_2_MASK                            0x02000000
#define IMX7_USB_PORTSC1_PTS_2_SHIFT                           25
#define IMX7_USB_PORTSC1_PFSC_MASK                             0x01000000
#define IMX7_USB_PORTSC1_PFSC_SHIFT                            24
#define IMX7_USB_PORTSC1_PHCD_MASK                             0x00800000
#define IMX7_USB_PORTSC1_PHCD_SHIFT                            23
#define IMX7_USB_PORTSC1_WKOC_MASK                             0x00400000
#define IMX7_USB_PORTSC1_WKOC_SHIFT                            22
#define IMX7_USB_PORTSC1_WKDC_MASK                             0x00200000
#define IMX7_USB_PORTSC1_WKDC_SHIFT                            21
#define IMX7_USB_PORTSC1_WKDN_MASK                             0x00100000
#define IMX7_USB_PORTSC1_WKDN_SHIFT                            20
#define IMX7_USB_PORTSC1_PTC_MASK                              0x000F0000
#define IMX7_USB_PORTSC1_PTC_SHIFT                             16
#define IMX7_USB_PORTSC1_PIC_MASK                              0x0000C000
#define IMX7_USB_PORTSC1_PIC_SHIFT                             14
#define IMX7_USB_PORTSC1_PO_MASK                               0x00002000
#define IMX7_USB_PORTSC1_PO_SHIFT                              13
#define IMX7_USB_PORTSC1_PP_MASK                               0x00001000
#define IMX7_USB_PORTSC1_PP_SHIFT                              12
#define IMX7_USB_PORTSC1_LS_MASK                               0x00000C00
#define IMX7_USB_PORTSC1_LS_SHIFT                              10
#define IMX7_USB_PORTSC1_HSP_MASK                              0x00000200
#define IMX7_USB_PORTSC1_HSP_SHIFT                             9
#define IMX7_USB_PORTSC1_PR_MASK                               0x00000100
#define IMX7_USB_PORTSC1_PR_SHIFT                              8
#define IMX7_USB_PORTSC1_SUSP_MASK                             0x00000080
#define IMX7_USB_PORTSC1_SUSP_SHIFT                            7
#define IMX7_USB_PORTSC1_FPR_MASK                              0x00000040
#define IMX7_USB_PORTSC1_FPR_SHIFT                             6
#define IMX7_USB_PORTSC1_OCC_MASK                              0x00000020
#define IMX7_USB_PORTSC1_OCC_SHIFT                             5
#define IMX7_USB_PORTSC1_OCA_MASK                              0x00000010
#define IMX7_USB_PORTSC1_OCA_SHIFT                             4
#define IMX7_USB_PORTSC1_PEC_MASK                              0x00000008
#define IMX7_USB_PORTSC1_PEC_SHIFT                             3
#define IMX7_USB_PORTSC1_PE_MASK                               0x00000004
#define IMX7_USB_PORTSC1_PE_SHIFT                              2
#define IMX7_USB_PORTSC1_CSC_MASK                              0x00000002
#define IMX7_USB_PORTSC1_CSC_SHIFT                             1
#define IMX7_USB_PORTSC1_CCS_MASK                              0x00000001
#define IMX7_USB_PORTSC1_CCS_SHIFT                             0

/*
 * USB_OTGSC register bits *
 */
#define IMX7_USB_OTGSC_DPIE_MASK                               0x40000000
#define IMX7_USB_OTGSC_DPIE_SHIFT                              30
#define IMX7_USB_OTGSC_EN_1MS_MASK                             0x20000000
#define IMX7_USB_OTGSC_EN_1MS_SHIFT                            29
#define IMX7_USB_OTGSC_BSEIE_MASK                              0x10000000
#define IMX7_USB_OTGSC_BSEIE_SHIFT                             28
#define IMX7_USB_OTGSC_BSVIE_MASK                              0x08000000
#define IMX7_USB_OTGSC_BSVIE_SHIFT                             27
#define IMX7_USB_OTGSC_ASVIE_MASK                              0x04000000
#define IMX7_USB_OTGSC_ASVIE_SHIFT                             26
#define IMX7_USB_OTGSC_AVVIE_MASK                              0x02000000
#define IMX7_USB_OTGSC_AVVIE_SHIFT                             25
#define IMX7_USB_OTGSC_IDIE_MASK                               0x01000000
#define IMX7_USB_OTGSC_IDIE_SHIFT                              24
#define IMX7_USB_OTGSC_DPIS_MASK                               0x00400000
#define IMX7_USB_OTGSC_DPIS_SHIFT                              22
#define IMX7_USB_OTGSC_STATUS_1MS_MASK                         0x00200000
#define IMX7_USB_OTGSC_STATUS_1MS_SHIFT                        21
#define IMX7_USB_OTGSC_BSEIS_MASK                              0x00100000
#define IMX7_USB_OTGSC_BSEIS_SHIFT                             20
#define IMX7_USB_OTGSC_BSVIS_MASK                              0x00080000
#define IMX7_USB_OTGSC_BSVIS_SHIFT                             19
#define IMX7_USB_OTGSC_ASVIS_MASK                              0x00040000
#define IMX7_USB_OTGSC_ASVIS_SHIFT                             18
#define IMX7_USB_OTGSC_AVVIS_MASK                              0x00020000
#define IMX7_USB_OTGSC_AVVIS_SHIFT                             17
#define IMX7_USB_OTGSC_IDIS_MASK                               0x00010000
#define IMX7_USB_OTGSC_IDIS_SHIFT                              16
#define IMX7_USB_OTGSC_DPS_MASK                                0x00004000
#define IMX7_USB_OTGSC_DPS_SHIFT                               14
#define IMX7_USB_OTGSC_TOG_1MS_MASK                            0x00002000
#define IMX7_USB_OTGSC_TOG_1MS_SHIFT                           13
#define IMX7_USB_OTGSC_BSE_MASK                                0x00001000
#define IMX7_USB_OTGSC_BSE_SHIFT                               12
#define IMX7_USB_OTGSC_BSV_MASK                                0x00000800
#define IMX7_USB_OTGSC_BSV_SHIFT                               11
#define IMX7_USB_OTGSC_ASV_MASK                                0x00000400
#define IMX7_USB_OTGSC_ASV_SHIFT                               10
#define IMX7_USB_OTGSC_AVV_MASK                                0x00000200
#define IMX7_USB_OTGSC_AVV_SHIFT                               9
#define IMX7_USB_OTGSC_ID_MASK                                 0x00000100
#define IMX7_USB_OTGSC_ID_SHIFT                                8
#define IMX7_USB_OTGSC_IDPU_MASK                               0x00000020
#define IMX7_USB_OTGSC_IDPU_SHIFT                              5
#define IMX7_USB_OTGSC_DP_MASK                                 0x00000010
#define IMX7_USB_OTGSC_DP_SHIFT                                4
#define IMX7_USB_OTGSC_OT_MASK                                 0x00000008
#define IMX7_USB_OTGSC_OT_SHIFT                                3
#define IMX7_USB_OTGSC_VC_MASK                                 0x00000002
#define IMX7_USB_OTGSC_VC_SHIFT                                1
#define IMX7_USB_OTGSC_VD_MASK                                 0x00000001
#define IMX7_USB_OTGSC_VD_SHIFT                                0

/*
 * USB_USBMODE register bits *
 */
#define IMX7_USB_USBMODE_SDIS_MASK                             0x00000010
#define IMX7_USB_USBMODE_SDIS_SHIFT                            4
#define IMX7_USB_USBMODE_SLOM_MASK                             0x00000008
#define IMX7_USB_USBMODE_SLOM_SHIFT                            3
#define IMX7_USB_USBMODE_ES_MASK                               0x00000004
#define IMX7_USB_USBMODE_ES_SHIFT                              2
#define IMX7_USB_USBMODE_CM_MASK                               0x00000003
#define IMX7_USB_USBMODE_CM_SHIFT                              0

/*
 * USB_ENDPTSETUPSTAT register bits *
 */
#define IMX7_USB_ENDPTSETUPSTAT_ENDPTSETUPSTAT_MASK            0x0000FFFF
#define IMX7_USB_ENDPTSETUPSTAT_ENDPTSETUPSTAT_SHIFT           0

/*
 * USB_ENDPTPRIME register bits *
 */
#define IMX7_USB_ENDPTPRIME_PETB_MASK                          0x00FF0000
#define IMX7_USB_ENDPTPRIME_PETB_SHIFT                         16
#define IMX7_USB_ENDPTPRIME_PERB_MASK                          0x000000FF
#define IMX7_USB_ENDPTPRIME_PERB_SHIFT                         0

/*
 * USB_ENDPTFLUSH register bits *
 */
#define IMX7_USB_ENDPTFLUSH_FETB_MASK                          0x00FF0000
#define IMX7_USB_ENDPTFLUSH_FETB_SHIFT                         16
#define IMX7_USB_ENDPTFLUSH_FERB_MASK                          0x000000FF
#define IMX7_USB_ENDPTFLUSH_FERB_SHIFT                         0

/*
 * USB_ENDPTSTAT register bits *
 */
#define IMX7_USB_ENDPTSTAT_ETBR_MASK                           0x00FF0000
#define IMX7_USB_ENDPTSTAT_ETBR_SHIFT                          16
#define IMX7_USB_ENDPTSTAT_ERBR_MASK                           0x000000FF
#define IMX7_USB_ENDPTSTAT_ERBR_SHIFT                          0

/*
 * USB_ENDPTCOMPLETE register bits *
 */
#define IMX7_USB_ENDPTCOMPLETE_ETCE_MASK                       0x00FF0000
#define IMX7_USB_ENDPTCOMPLETE_ETCE_SHIFT                      16
#define IMX7_USB_ENDPTCOMPLETE_ERCE_MASK                       0x000000FF
#define IMX7_USB_ENDPTCOMPLETE_ERCE_SHIFT                      0

/*
 * USB_ENDPTCTRL0 register bits *
 */
#define IMX7_USB_ENDPTCTRL0_TXE_MASK                           0x00800000
#define IMX7_USB_ENDPTCTRL0_TXE_SHIFT                          23
#define IMX7_USB_ENDPTCTRL0_TX_MASK                            0x000C0000
#define IMX7_USB_ENDPTCTRL0_TX_SHIFT                           18
#define IMX7_USB_ENDPTCTRL0_TXS_MASK                           0x00010000
#define IMX7_USB_ENDPTCTRL0_TXS_SHIFT                          16
#define IMX7_USB_ENDPTCTRL0_RXE_MASK                           0x00000080
#define IMX7_USB_ENDPTCTRL0_RXE_SHIFT                          7
#define IMX7_USB_ENDPTCTRL0_RX_MASK                            0x0000000C
#define IMX7_USB_ENDPTCTRL0_RX_SHIFT                           2
#define IMX7_USB_ENDPTCTRL0_RXS_MASK                           0x00000001
#define IMX7_USB_ENDPTCTRL0_RXS_SHIFT                          0

/*
 * USB_ENDPTCTRL register bits *
 */
#define IMX7_USB_ENDPTCTRL_TXE_MASK                            0x00800000
#define IMX7_USB_ENDPTCTRL_TXE_SHIFT                           23
#define IMX7_USB_ENDPTCTRL_TXR_MASK                            0x00400000
#define IMX7_USB_ENDPTCTRL_TXR_SHIFT                           22
#define IMX7_USB_ENDPTCTRL_TXI_MASK                            0x00200000
#define IMX7_USB_ENDPTCTRL_TXI_SHIFT                           21
#define IMX7_USB_ENDPTCTRL_TX_MASK                             0x000C0000
#define IMX7_USB_ENDPTCTRL_TX_SHIFT                            18
#define IMX7_USB_ENDPTCTRL_TXS_MASK                            0x00010000
#define IMX7_USB_ENDPTCTRL_TXS_SHIFT                           16
#define IMX7_USB_ENDPTCTRL_RXE_MASK                            0x00000080
#define IMX7_USB_ENDPTCTRL_RXE_SHIFT                           7
#define IMX7_USB_ENDPTCTRL_RXR_MASK                            0x00000040
#define IMX7_USB_ENDPTCTRL_RXR_SHIFT                           6
#define IMX7_USB_ENDPTCTRL_RXI_MASK                            0x00000020
#define IMX7_USB_ENDPTCTRL_RXI_SHIFT                           5
#define IMX7_USB_ENDPTCTRL_RX_MASK                             0x0000000C
#define IMX7_USB_ENDPTCTRL_RX_SHIFT                            2
#define IMX7_USB_ENDPTCTRL_RXS_MASK                            0x00000001
#define IMX7_USB_ENDPTCTRL_RXS_SHIFT                           0

/*
 * USBNC_OTG_CTRL1 register bits *
 */
#define IMX7_USBNC_OTG_CTRL1_WIR_MASK                          0x80000000
#define IMX7_USBNC_OTG_CTRL1_WIR_SHIFT                         31
#define IMX7_USBNC_OTG_CTRL1_WKUP_DPDM_EN_MASK                 0x20000000
#define IMX7_USBNC_OTG_CTRL1_WKUP_DPDM_EN_SHIFT                29
#define IMX7_USBNC_OTG_CTRL1_WKUP_VBUS_EN_MASK                 0x00020000
#define IMX7_USBNC_OTG_CTRL1_WKUP_VBUS_EN_SHIFT                17
#define IMX7_USBNC_OTG_CTRL1_WKUP_ID_EN_MASK                   0x00010000
#define IMX7_USBNC_OTG_CTRL1_WKUP_ID_EN_SHIFT                  16
#define IMX7_USBNC_OTG_CTRL1_WKUP_SW_EN_MASK                   0x00004000
#define IMX7_USBNC_OTG_CTRL1_WKUP_SW_EN_SHIFT                  14
#define IMX7_USBNC_OTG_CTRL1_WIE_MASK                          0x00000400
#define IMX7_USBNC_OTG_CTRL1_WIE_SHIFT                         10
#define IMX7_USBNC_OTG_CTRL1_PWR_POL_MASK                      0x00000200
#define IMX7_USBNC_OTG_CTRL1_PWR_POL_SHIFT                     9
#define IMX7_USBNC_OTG_CTRL1_OVER_CUR_POL_MASK                 0x00000100
#define IMX7_USBNC_OTG_CTRL1_OVER_CUR_POL_SHIFT                8
#define IMX7_USBNC_OTG_CTRL1_OVER_CUR_DIS_MASK                 0x00000080
#define IMX7_USBNC_OTG_CTRL1_OVER_CUR_DIS_SHIFT                7

/*
 * USBNC_OTG_CTRL2 register bits *
 */
#define IMX7_USBNC_OTG_CTRL2_UTMI_CLK_VLD_MASK                 0x80000000
#define IMX7_USBNC_OTG_CTRL2_UTMI_CLK_VLD_SHIFT                31
#define IMX7_USBNC_OTG_CTRL2_DIG_ID_SEL_MASK                   0x00100000
#define IMX7_USBNC_OTG_CTRL2_DIG_ID_SEL_SHIFT                  20
#define IMX7_USBNC_OTG_CTRL2_LOWSPEED_EN_MASK                  0x00000008
#define IMX7_USBNC_OTG_CTRL2_LOWSPEED_EN_SHIFT                 3
#define IMX7_USBNC_OTG_CTRL2_AUTURESUME_EN_MASK                0x00000004
#define IMX7_USBNC_OTG_CTRL2_AUTURESUME_EN_SHIFT               2
#define IMX7_USBNC_OTG_CTRL2_VBUS_SOURCE_SEL_MASK              0x00000003
#define IMX7_USBNC_OTG_CTRL2_VBUS_SOURCE_SEL_SHIFT             0

/*
 * USBNC_OTG_PHY_CFG1 register bits *
 */
#define IMX7_USBNC_OTG_PHY_CFG1_CHRGDET_Megamix_MASK           0x80000000
#define IMX7_USBNC_OTG_PHY_CFG1_CHRGDET_Megamix_SHIFT          31
#define IMX7_USBNC_OTG_PHY_CFG1_TXPREEMPPULSETUNE0_MASK        0x40000000
#define IMX7_USBNC_OTG_PHY_CFG1_TXPREEMPPULSETUNE0_SHIFT       30
#define IMX7_USBNC_OTG_PHY_CFG1_TXPREEMPAMPTUNE0_MASK          0x30000000
#define IMX7_USBNC_OTG_PHY_CFG1_TXPREEMPAMPTUNE0_SHIFT         28
#define IMX7_USBNC_OTG_PHY_CFG1_TXRESTUNE0_MASK                0x0C000000
#define IMX7_USBNC_OTG_PHY_CFG1_TXRESTUNE0_SHIFT               26
#define IMX7_USBNC_OTG_PHY_CFG1_TXRISETUNE0_MASK               0x03000000
#define IMX7_USBNC_OTG_PHY_CFG1_TXRISETUNE0_SHIFT              24
#define IMX7_USBNC_OTG_PHY_CFG1_TXVREFTUNE0_MASK               0x00F00000
#define IMX7_USBNC_OTG_PHY_CFG1_TXVREFTUNE0_SHIFT              20
#define IMX7_USBNC_OTG_PHY_CFG1_TXFSLSTUNE0_MASK               0x000F0000
#define IMX7_USBNC_OTG_PHY_CFG1_TXFSLSTUNE0_SHIFT              16
#define IMX7_USBNC_OTG_PHY_CFG1_TXHSXVTUNE0_MASK               0x00006000
#define IMX7_USBNC_OTG_PHY_CFG1_TXHSXVTUNE0_SHIFT              13
#define IMX7_USBNC_OTG_PHY_CFG1_OTGTUNE0_MASK                  0x00001C00
#define IMX7_USBNC_OTG_PHY_CFG1_OTGTUNE0_SHIFT                 10
#define IMX7_USBNC_OTG_PHY_CFG1_SQRXTUNE0_MASK                 0x00000380
#define IMX7_USBNC_OTG_PHY_CFG1_SQRXTUNE0_SHIFT                7
#define IMX7_USBNC_OTG_PHY_CFG1_COMPDISTUNE0_MASK              0x00000070
#define IMX7_USBNC_OTG_PHY_CFG1_COMPDISTUNE0_SHIFT             4
#define IMX7_USBNC_OTG_PHY_CFG1_FSEL_MASK                      0x0000000E
#define IMX7_USBNC_OTG_PHY_CFG1_FSEL_SHIFT                     1
#define IMX7_USBNC_OTG_PHY_CFG1_COMMONONN_MASK                 0x00000001
#define IMX7_USBNC_OTG_PHY_CFG1_COMMONONN_SHIFT                0

/*
 * USBNC_OTG_PHY_CFG2 register bits *
 */
#define IMX7_USBNC_OTG_PHY_CFG2_DRVVBUS0_MASK                  0x00010000
#define IMX7_USBNC_OTG_PHY_CFG2_DRVVBUS0_SHIFT                 16
#define IMX7_USBNC_OTG_PHY_CFG2_VBUSVLDEXT_MASK                0x00008000
#define IMX7_USBNC_OTG_PHY_CFG2_VBUSVLDEXT_SHIFT               15
#define IMX7_USBNC_OTG_PHY_CFG2_VBUSVLDEXTSEL0_MASK            0x00004000
#define IMX7_USBNC_OTG_PHY_CFG2_VBUSVLDEXTSEL0_SHIFT           14
#define IMX7_USBNC_OTG_PHY_CFG2_ADPPRBENB0_MASK                0x00002000
#define IMX7_USBNC_OTG_PHY_CFG2_ADPPRBENB0_SHIFT               13
#define IMX7_USBNC_OTG_PHY_CFG2_ADPDISCHRG0_MASK               0x00001000
#define IMX7_USBNC_OTG_PHY_CFG2_ADPDISCHRG0_SHIFT              12
#define IMX7_USBNC_OTG_PHY_CFG2_ADPCHRG0_MASK                  0x00000800
#define IMX7_USBNC_OTG_PHY_CFG2_ADPCHRG0_SHIFT                 11
#define IMX7_USBNC_OTG_PHY_CFG2_OTGDISABLE0_MASK               0x00000400
#define IMX7_USBNC_OTG_PHY_CFG2_OTGDISABLE0_SHIFT              10
#define IMX7_USBNC_OTG_PHY_CFG2_TXBITSTUFFENH0_MASK            0x00000200
#define IMX7_USBNC_OTG_PHY_CFG2_TXBITSTUFFENH0_SHIFT           9
#define IMX7_USBNC_OTG_PHY_CFG2_TXBITSTUFFEN0_MASK             0x00000100
#define IMX7_USBNC_OTG_PHY_CFG2_TXBITSTUFFEN0_SHIFT            8
#define IMX7_USBNC_OTG_PHY_CFG2_LOOPBACKENB0_MASK              0x00000040
#define IMX7_USBNC_OTG_PHY_CFG2_LOOPBACKENB0_SHIFT             6
#define IMX7_USBNC_OTG_PHY_CFG2_SLEEPM0_MASK                   0x00000020
#define IMX7_USBNC_OTG_PHY_CFG2_SLEEPM0_SHIFT                  5
#define IMX7_USBNC_OTG_PHY_CFG2_ACAENB0_MASK                   0x00000010
#define IMX7_USBNC_OTG_PHY_CFG2_ACAENB0_SHIFT                  4
#define IMX7_USBNC_OTG_PHY_CFG2_DCDENB_MASK                    0x00000008
#define IMX7_USBNC_OTG_PHY_CFG2_DCDENB_SHIFT                   3
#define IMX7_USBNC_OTG_PHY_CFG2_VDATSRCENB0_MASK               0x00000004
#define IMX7_USBNC_OTG_PHY_CFG2_VDATSRCENB0_SHIFT              2
#define IMX7_USBNC_OTG_PHY_CFG2_VDATDETENB0_MASK               0x00000002
#define IMX7_USBNC_OTG_PHY_CFG2_VDATDETENB0_SHIFT              1
#define IMX7_USBNC_OTG_PHY_CFG2_CHRGSEL_MASK                   0x00000001
#define IMX7_USBNC_OTG_PHY_CFG2_CHRGSEL_SHIFT                  0

/*
 * USBNC_PHY_STATUS register bits *
 */
#define IMX7_USBNC_PHY_STATUS_ADPSNS0_MASK                     0x80000000
#define IMX7_USBNC_PHY_STATUS_ADPSNS0_SHIFT                    31
#define IMX7_USBNC_PHY_STATUS_ADPPRB0_MASK                     0x40000000
#define IMX7_USBNC_PHY_STATUS_ADPPRB0_SHIFT                    30
#define IMX7_USBNC_PHY_STATUS_CHRGDET_MASK                     0x20000000
#define IMX7_USBNC_PHY_STATUS_CHRGDET_SHIFT                    29
#define IMX7_USBNC_PHY_STATUS_RIDFLOAT0_MASK                   0x10000000
#define IMX7_USBNC_PHY_STATUS_RIDFLOAT0_SHIFT                  28
#define IMX7_USBNC_PHY_STATUS_RIDGND0_MASK                     0x08000000
#define IMX7_USBNC_PHY_STATUS_RIDGND0_SHIFT                    27
#define IMX7_USBNC_PHY_STATUS_RIDA0_MASK                       0x04000000
#define IMX7_USBNC_PHY_STATUS_RIDA0_SHIFT                      26
#define IMX7_USBNC_PHY_STATUS_RIDB0_MASK                       0x02000000
#define IMX7_USBNC_PHY_STATUS_RIDB0_SHIFT                      25
#define IMX7_USBNC_PHY_STATUS_RIDC0_MASK                       0x01000000
#define IMX7_USBNC_PHY_STATUS_RIDC0_SHIFT                      24
#define IMX7_USBNC_PHY_STATUS_HOST_DISCONNECT_MASK             0x00000020
#define IMX7_USBNC_PHY_STATUS_HOST_DISCONNECT_SHIFT            5
#define IMX7_USBNC_PHY_STATUS_ID_DIG_MASK                      0x00000010
#define IMX7_USBNC_PHY_STATUS_ID_DIG_SHIFT                     4
#define IMX7_USBNC_PHY_STATUS_VBUS_VLD_MASK                    0x00000008
#define IMX7_USBNC_PHY_STATUS_VBUS_VLD_SHIFT                   3
#define IMX7_USBNC_PHY_STATUS_SESS_VLD_MASK                    0x00000004
#define IMX7_USBNC_PHY_STATUS_SESS_VLD_SHIFT                   2
#define IMX7_USBNC_PHY_STATUS_LINE_STATE_MASK                  0x00000003
#define IMX7_USBNC_PHY_STATUS_LINE_STATE_SHIFT                 0

/*
 * USBNC_UH_HSICPHY_CFG1 register bits *
 */
#define IMX7_USBNC_UH_HSICPHY_CFG1_REFCLKSEL_MASK              0x03000000
#define IMX7_USBNC_UH_HSICPHY_CFG1_REFCLKSEL_SHIFT             24
#define IMX7_USBNC_UH_HSICPHY_CFG1_REFCLKDIV_MASK              0x007F0000
#define IMX7_USBNC_UH_HSICPHY_CFG1_REFCLKDIV_SHIFT             16
#define IMX7_USBNC_UH_HSICPHY_CFG1_TXSRTUNE_MASK               0x0000F000
#define IMX7_USBNC_UH_HSICPHY_CFG1_TXSRTUNE_SHIFT              12
#define IMX7_USBNC_UH_HSICPHY_CFG1_TXRPDTUNE_MASK              0x00000C00
#define IMX7_USBNC_UH_HSICPHY_CFG1_TXRPDTUNE_SHIFT             10
#define IMX7_USBNC_UH_HSICPHY_CFG1_TXRPUTUNE_MASK              0x00000300
#define IMX7_USBNC_UH_HSICPHY_CFG1_TXRPUTUNE_SHIFT             8
#define IMX7_USBNC_UH_HSICPHY_CFG1_TXBITSTUFFENH_MASK          0x00000040
#define IMX7_USBNC_UH_HSICPHY_CFG1_TXBITSTUFFENH_SHIFT         6
#define IMX7_USBNC_UH_HSICPHY_CFG1_TXBITSTUFFEN_MASK           0x00000020
#define IMX7_USBNC_UH_HSICPHY_CFG1_TXBITSTUFFEN_SHIFT          5
#define IMX7_USBNC_UH_HSICPHY_CFG1_SLEEPM_MASK                 0x00000010
#define IMX7_USBNC_UH_HSICPHY_CFG1_SLEEPM_SHIFT                4
#define IMX7_USBNC_UH_HSICPHY_CFG1_DMPULLDOWN_MASK             0x00000008
#define IMX7_USBNC_UH_HSICPHY_CFG1_DMPULLDOWN_SHIFT            3
#define IMX7_USBNC_UH_HSICPHY_CFG1_DPPULLDOWN_MASK             0x00000004
#define IMX7_USBNC_UH_HSICPHY_CFG1_DPPULLDOWN_SHIFT            2
#define IMX7_USBNC_UH_HSICPHY_CFG1_LOOPBACKENB_MASK            0x00000002
#define IMX7_USBNC_UH_HSICPHY_CFG1_LOOPBACKENB_SHIFT           1
#define IMX7_USBNC_UH_HSICPHY_CFG1_COMMONONN_MASK              0x00000001
#define IMX7_USBNC_UH_HSICPHY_CFG1_COMMONONN_SHIFT             0

/*
 * USBNC_ADP_CFG1 register bits *
 */
#define IMX7_USBNC_ADP_CFG1_ADP_PRB_EN_MASK                    0x00800000
#define IMX7_USBNC_ADP_CFG1_ADP_PRB_EN_SHIFT                   23
#define IMX7_USBNC_ADP_CFG1_ADP_PRB_INT_EN_MASK                0x00400000
#define IMX7_USBNC_ADP_CFG1_ADP_PRB_INT_EN_SHIFT               22
#define IMX7_USBNC_ADP_CFG1_ADP_SNS_INT_EN_MASK                0x00200000
#define IMX7_USBNC_ADP_CFG1_ADP_SNS_INT_EN_SHIFT               21
#define IMX7_USBNC_ADP_CFG1_TIMER_EN_MASK                      0x00100000
#define IMX7_USBNC_ADP_CFG1_TIMER_EN_SHIFT                     20
#define IMX7_USBNC_ADP_CFG1_ADP_WAIT_MASK                      0x0003FFFF
#define IMX7_USBNC_ADP_CFG1_ADP_WAIT_SHIFT                     0

/*
 * USBNC_ADP_CFG2 register bits *
 */
#define IMX7_USBNC_ADP_CFG2_ADP_DISCHG_TIME_MASK               0x00FF0000
#define IMX7_USBNC_ADP_CFG2_ADP_DISCHG_TIME_SHIFT              16
#define IMX7_USBNC_ADP_CFG2_ADP_CHRG_SWTIME_MASK               0x0000FF00
#define IMX7_USBNC_ADP_CFG2_ADP_CHRG_SWTIME_SHIFT              8
#define IMX7_USBNC_ADP_CFG2_ADP_CHRG_SWCMP_MASK                0x00000080
#define IMX7_USBNC_ADP_CFG2_ADP_CHRG_SWCMP_SHIFT               7
#define IMX7_USBNC_ADP_CFG2_ADP_CHRG_DELTA_MASK                0x0000007F
#define IMX7_USBNC_ADP_CFG2_ADP_CHRG_DELTA_SHIFT               0

/*
 * USBNC_ADP_STATUS register bits *
 */
#define IMX7_USBNC_ADP_STATUS_ADP_PRB_INT_MASK                 0x08000000
#define IMX7_USBNC_ADP_STATUS_ADP_PRB_INT_SHIFT                27
#define IMX7_USBNC_ADP_STATUS_ADP_SNS_INT_MASK                 0x04000000
#define IMX7_USBNC_ADP_STATUS_ADP_SNS_INT_SHIFT                26
#define IMX7_USBNC_ADP_STATUS_ADP_CNT_MASK                     0x03FFFF00
#define IMX7_USBNC_ADP_STATUS_ADP_CNT_SHIFT                    8
#define IMX7_USBNC_ADP_STATUS_ADP_PRB_TIMR_MASK                0x000000FF
#define IMX7_USBNC_ADP_STATUS_ADP_PRB_TIMR_SHIFT               0

/*
 * USB_ID register bits definition *
 */
typedef union IMX7_USB_ID_union_t {
    uint32_t R;
    struct {
        uint32_t ID           :  6;  /* Configuration number. */
        uint32_t Reserved_6   :  2;  /* Reserved */
        uint32_t NID          :  6;  /* Complement version of ID. */
        uint32_t Reserved_14  :  2;  /* Reserved */
        uint32_t Revision     :  8;  /* Revision number of the controller core. */
        uint32_t Reserved_24  :  8;  /* Reserved */
    } B;
} IMX7_USB_ID_t;

/*
 * USB_HWGENERAL register bits definition *
 */
typedef union IMX7_USB_HWGENERAL_union_t {
    uint32_t R;
    struct {
        uint32_t Reserved_3   :  4;  /* Reserved */
        uint32_t PHYW         :  2;  /* Data width of the transciever connected to the controller core */
        uint32_t PHYM         :  4;  /* Transceiver type */
        uint32_t SM           :  2;  /* Serial interface mode capability. */
        uint32_t Reserved_12  : 20;  /* Reserved */
    } B;
} IMX7_USB_HWGENERAL_t;

/*
 * USB_HWHOST register bits definition *
 */
typedef union IMX7_USB_HWHOST_union_t {
    uint32_t R;
    struct {
        uint32_t HC          :  1;  /* Host Capable. Indicating whether host operation mode is supported or not. */
        uint32_t NPORT       :  3;  /* The Number of downstream ports supported by the host controller is (NPORT+1). */
        uint32_t Reserved_4  : 28;  /* Reserved */
    } B;
} IMX7_USB_HWHOST_t;

/*
 * USB_HWDEVICE register bits definition *
 */
typedef union IMX7_USB_HWDEVICE_union_t {
    uint32_t R;
    struct {
        uint32_t DC          :  1;  /* Device Capable. Indicating whether device operation mode is supported or not. */
        uint32_t DEVEP       :  5;  /* Device Endpoint Number. */
        uint32_t Reserved_6  : 26;  /* Reserved */
    } B;
} IMX7_USB_HWDEVICE_t;

/*
 * USB_HWTXBUF register bits definition *
 */
typedef union IMX7_USB_HWTXBUF_union_t {
    uint32_t R;
    struct {
        uint32_t TXBURST      :  8;  /* Default burst size for memory to TX buffer transfer. */
        uint32_t Reserved_8   :  8;  /* Reserved */
        uint32_t TXCHANADD    :  8;  /* TX FIFO Buffer size is: (2^TXCHANADD) * 4 Bytes. */
        uint32_t Reserved_24  :  8;  /* Reserved */
    } B;
} IMX7_USB_HWTXBUF_t;

/*
 * USB_HWRXBUF register bits definition *
 */
typedef union IMX7_USB_HWRXBUF_union_t {
    uint32_t R;
    struct {
        uint32_t RXBURST      :  8;  /* Default burst size for memory to RX buffer transfer. */
        uint32_t RXADD        :  8;  /* Buffer total size for all receive endpoints is (2^RXADD). */
        uint32_t Reserved_16  : 16;  /* Reserved */
    } B;
} IMX7_USB_HWRXBUF_t;

/*
 * USB_GPTIMER0LD register bits definition *
 */
typedef union IMX7_USB_GPTIMER0LD_union_t {
    uint32_t R;
    struct {
        uint32_t GPTLD        : 24;  /* General purpose timer load value */
        uint32_t Reserved_24  :  8;  /* Reserved */
    } B;
} IMX7_USB_GPTIMER0LD_t;

/*
 * USB_GPTIMER0CTRL register bits definition *
 */
typedef union IMX7_USB_GPTIMER0CTRL_union_t {
    uint32_t R;
    struct {
        uint32_t GPTCNT       : 24;  /* General purpose timer counter */
        uint32_t GPTMODE      :  1;  /* General purpose timer mode */
        uint32_t Reserved_25  :  5;  /* Reserved */
        uint32_t GPTRST       :  1;  /* General purpose timer reset */
        uint32_t GPTRUN       :  1;  /* General purpose timer run */
    } B;
} IMX7_USB_GPTIMER0CTRL_t;

/*
 * USB_GPTIMER1LD register bits definition *
 */
typedef union IMX7_USB_GPTIMER1LD_union_t {
    uint32_t R;
    struct {
        uint32_t GPTLD        : 24;  /* General purpose timer load value */
        uint32_t Reserved_24  :  8;  /* Reserved */
    } B;
} IMX7_USB_GPTIMER1LD_t;

/*
 * USB_GPTIMER1CTRL register bits definition *
 */
typedef union IMX7_USB_GPTIMER1CTRL_union_t {
    uint32_t R;
    struct {
        uint32_t GPTCNT       : 24;  /* General purpose timer counter */
        uint32_t GPTMODE      :  1;  /* General purpose timer mode */
        uint32_t Reserved_25  :  5;  /* Reserved */
        uint32_t GPTRST       :  1;  /* General purpose timer reset */
        uint32_t GPTRUN       :  1;  /* General purpose timer run */
    } B;
} IMX7_USB_GPTIMER1CTRL_t;

/*
 * USB_SBUSCFG register bits definition *
 */
typedef union IMX7_USB_SBUSCFG_union_t {
    uint32_t R;
    struct {
        uint32_t AHBBRST     :  3;  /* AHB master interface Burst configuration. */
        uint32_t Reserved_3  : 29;  /* Reserved */
    } B;
} IMX7_USB_SBUSCFG_t;

/*
 * USB_CAPLENGTH register bits definition *
 */
typedef union IMX7_USB_CAPLENGTH_union_t {
    uint8_t R;
    struct {
        uint8_t CAPLENGTH  :  8;  /* These bits are used as an offset to add to register base to find the beginning of the Operational Register */
    } B;
} IMX7_USB_CAPLENGTH_t;

/*
 * USB_HCIVERSION register bits definition *
 */
typedef union IMX7_USB_HCIVERSION_union_t {
    uint16_t R;
    struct {
        uint16_t HCIVERSION  : 16;  /* Host Controller Interface Version Number */
    } B;
} IMX7_USB_HCIVERSION_t;

/*
 * USB_HCSPARAMS register bits definition *
 */
typedef union IMX7_USB_HCSPARAMS_union_t {
    uint32_t R;
    struct {
        uint32_t N_PORTS      :  4;  /* Number of downstream ports */
        uint32_t PPC          :  1;  /* Port Power Control */
        uint32_t Reserved_5   :  6;  /* Reserved */
        uint32_t N_PCC        :  1;  /* Number of Ports per Companion Controller */
        uint32_t N_CC         :  4;  /* Number of Companion Controller */
        uint32_t PI           :  1;  /* Port Indicators */
        uint32_t Reserved_17  :  6;  /* Reserved */
        uint32_t N_PTT        :  1;  /* Number of Ports per Transaction Translator (N_PTT). Default value '0000b'D */
        uint32_t N_TT         :  4;  /* Number of Transaction Translators (N_TT). Default value '0000b' */
        uint32_t Reserved_28  :  4;  /* Reserved */
    } B;
} IMX7_USB_HCSPARAMS_t;

/*
 * USB_HCCPARAMS register bits definition *
 */
typedef union IMX7_USB_HCCPARAMS_union_t {
    uint32_t R;
    struct {
        uint32_t ADC          :  1;  /* 64-bit Addressing Capability */
        uint32_t PFL          :  1;  /* Programmable Frame List Flag */
        uint32_t ASP          :  1;  /* Asynchronous Schedule Park Capability */
        uint32_t Reserved_3   :  1;  /* Reserved */
        uint32_t IST          :  4;  /* Isochronous Scheduling Threshold */
        uint32_t EECP         :  8;  /* EHCI Extended Capabilities Pointer. */
        uint32_t Reserved_16  : 16;  /* Reserved */
    } B;
} IMX7_USB_HCCPARAMS_t;

/*
 * USB_DCIVERSION register bits definition *
 */
typedef union IMX7_USB_DCIVERSION_union_t {
    uint16_t R;
    struct {
        uint16_t DCIVERSION  : 16;  /* Device Controller Interface Version Number */
    } B;
} IMX7_USB_DCIVERSION_t;

/*
 * USB_DCCPARAMS register bits definition *
 */
typedef union IMX7_USB_DCCPARAMS_union_t {
    uint32_t R;
    struct {
        uint32_t DEN         :  5;  /* The number of endpoints built into the device controller. */
        uint32_t Reserved_5  :  2;  /* Reserved */
        uint32_t DC          :  1;  /* Device Capable. */
        uint32_t HC          :  1;  /* Host Capable. */
        uint32_t Reserved_9  : 23;  /* Reserved */
    } B;
} IMX7_USB_DCCPARAMS_t;

/*
 * USB_USBCMD register bits definition *
 */
typedef union IMX7_USB_USBCMD_union_t {
    uint32_t R;
    struct {
        uint32_t RS           :  1;  /* Run/Stop (RS) */
        uint32_t RST          :  1;  /* Controller Reset */
        uint32_t FS_1         :  2;  /* See description at bit 15, FS_2 */
        uint32_t PSE          :  1;  /* Periodic Schedule Enable- */
        uint32_t ASE          :  1;  /* Asynchronous Schedule Enable */
        uint32_t IAA          :  1;  /* Interrupt on Async Advance Doorbell */
        uint32_t Reserved_7   :  1;  /* Reserved */
        uint32_t ASP          :  2;  /* Asynchronous Schedule Park Mode Count */
        uint32_t Reserved_10  :  1;  /* Reserved */
        uint32_t ASPE         :  1;  /* Asynchronous Schedule Park Mode Enable */
        uint32_t ATDTW        :  1;  /* Add dTD TripWire */
        uint32_t SUTW         :  1;  /* Setup TripWire */
        uint32_t Reserved_14  :  1;  /* Reserved */
        uint32_t FS_2         :  1;  /* Frame List Size. See also bits 3-2 */
        uint32_t ITC          :  8;  /* Interrupt Threshold Control */
        uint32_t Reserved_24  :  8;  /* Reserved */
    } B;
} IMX7_USB_USBCMD_t;

/*
 * USB_USBSTS register bits definition *
 */
typedef union IMX7_USB_USBSTS_union_t {
    uint32_t R;
    struct {
        uint32_t UI           :  1;  /* USB Interrupt (USBINT) */
        uint32_t UEI          :  1;  /* USB Error Interrupt (USBERRINT) */
        uint32_t PCI          :  1;  /* Port Change Detect */
        uint32_t FRI          :  1;  /* Frame List Rollover */
        uint32_t SEI          :  1;  /* System Error */
        uint32_t AAI          :  1;  /* Interrupt on Async Advance */
        uint32_t URI          :  1;  /* USB Reset Received */
        uint32_t SRI          :  1;  /* SOF Received */
        uint32_t SLI          :  1;  /* DCSuspend */
        uint32_t Reserved_9   :  1;  /* Reserved */
        uint32_t ULPII        :  1;  /* ULPI Interrupt */
        uint32_t Reserved_11  :  1;  /* Reserved */
        uint32_t HCH          :  1;  /* HCHaIted */
        uint32_t RCL          :  1;  /* Reclamation */
        uint32_t PS           :  1;  /* Periodic Schedule Status */
        uint32_t AS           :  1;  /* Asynchronous Schedule Status */
        uint32_t NAKI         :  1;  /* NAK Interrupt Bit */
        uint32_t Reserved_17  :  7;  /* Reserved */
        uint32_t TI0          :  1;  /* General Purpose Timer Interrupt 0(GPTINT0) */
        uint32_t TI1          :  1;  /* General Purpose Timer Interrupt 1(GPTINT1) */
        uint32_t Reserved_26  :  6;  /* Reserved */
    } B;
} IMX7_USB_USBSTS_t;

/*
 * USB_USBINTR register bits definition *
 */
typedef union IMX7_USB_USBINTR_union_t {
    uint32_t R;
    struct {
        uint32_t UE           :  1;  /* USB Interrupt Enable */
        uint32_t UEE          :  1;  /* USB Error Interrupt Enable */
        uint32_t PCE          :  1;  /* Port Change Detect Interrupt Enable */
        uint32_t FRE          :  1;  /* Frame List Rollover Interrupt Enable */
        uint32_t SEE          :  1;  /* System Error Interrupt Enable */
        uint32_t AAE          :  1;  /* Async Advance Interrupt Enable */
        uint32_t URE          :  1;  /* USB Reset Interrupt Enable */
        uint32_t SRE          :  1;  /* SOF Received Interrupt Enable */
        uint32_t SLE          :  1;  /* Sleep Interrupt Enable */
        uint32_t Reserved_9   :  1;  /* Reserved */
        uint32_t ULPIE        :  1;  /* ULPI Interrupt Enable */
        uint32_t Reserved_11  :  5;  /* Reserved */
        uint32_t NAKE         :  1;  /* NAK Interrupt Enable */
        uint32_t Reserved_17  :  1;  /* Reserved */
        uint32_t UAIE         :  1;  /* USB Host Asynchronous Interrupt Enable */
        uint32_t UPIE         :  1;  /* USB Host Periodic Interrupt Enable */
        uint32_t Reserved_20  :  4;  /* Reserved */
        uint32_t TIE0         :  1;  /* General Purpose Timer #0 Interrupt Enable */
        uint32_t TIE1         :  1;  /* General Purpose Timer #1 Interrupt Enable */
        uint32_t Reserved_26  :  6;  /* Reserved */
    } B;
} IMX7_USB_USBINTR_t;

/*
 * USB_FRINDEX register bits definition *
 */
typedef union IMX7_USB_FRINDEX_union_t {
    uint32_t R;
    struct {
        uint32_t FRINDEX      : 14;  /* Frame Index */
        uint32_t Reserved_14  : 18;  /* Reserved */
    } B;
} IMX7_USB_FRINDEX_t;

/*
 * USB_PERIODICLISTBASE register bits definition *
 */
typedef union IMX7_USB_PERIODICLISTBASE_union_t {
    uint32_t R;
    struct {
        uint32_t Reserved_11  : 12;  /* Reserved */
        uint32_t BASEADR      : 20;  /* Base Address (Low) */
    } B;
} IMX7_USB_PERIODICLISTBASE_t;

/*
 * USB_ASYNCLISTADDR register bits definition *
 */
typedef union IMX7_USB_ASYNCLISTADDR_union_t {
    uint32_t R;
    struct {
        uint32_t Reserved_4  :  5;  /* Reserved */
        uint32_t ASYBAS      : 27;  /* Link Pointer Low (LPL) */
    } B;
} IMX7_USB_ASYNCLISTADDR_t;

/*
 * USB_BURSTSIZE register bits definition *
 */
typedef union IMX7_USB_BURSTSIZE_union_t {
    uint32_t R;
    struct {
        uint32_t RXPBURST     :  8;  /* Programmable RX Burst Size */
        uint32_t TXPBURST     :  9;  /* Programmable TX Burst Size */
        uint32_t Reserved_17  : 15;  /* Reserved */
    } B;
} IMX7_USB_BURSTSIZE_t;

/*
 * USB_TXFILLTUNING register bits definition *
 */
typedef union IMX7_USB_TXFILLTUNING_union_t {
    uint32_t R;
    struct {
        uint32_t TXSCHOH      :  8;  /* Scheduler Overhead */
        uint32_t TXSCHHEALTH  :  5;  /* Scheduler Health Counter */
        uint32_t Reserved_13  :  3;  /* Reserved */
        uint32_t TXFIFOTHRES  :  6;  /* FIFO Burst Threshold */
        uint32_t Reserved_22  : 10;  /* Reserved */
    } B;
} IMX7_USB_TXFILLTUNING_t;

/*
 * USB_ENDPTNAK register bits definition *
 */
typedef union IMX7_USB_ENDPTNAK_union_t {
    uint32_t R;
    struct {
        uint32_t EPRN         :  8;  /* RX Endpoint NAK */
        uint32_t Reserved_8   :  8;  /* Reserved */
        uint32_t EPTN         :  8;  /* TX Endpoint NAK */
        uint32_t Reserved_24  :  8;  /* Reserved */
    } B;
} IMX7_USB_ENDPTNAK_t;

/*
 * USB_ENDPTNAKEN register bits definition *
 */
typedef union IMX7_USB_ENDPTNAKEN_union_t {
    uint32_t R;
    struct {
        uint32_t EPRNE        :  8;  /* RX Endpoint NAK Enable */
        uint32_t Reserved_8   :  8;  /* Reserved */
        uint32_t EPTNE        :  8;  /* TX Endpoint NAK Enable */
        uint32_t Reserved_24  :  8;  /* Reserved */
    } B;
} IMX7_USB_ENDPTNAKEN_t;

/*
 * USB_CONFIGFLAG register bits definition *
 */
typedef union IMX7_USB_CONFIGFLAG_union_t {
    uint32_t R;
    struct {
        uint32_t CF          :  1;  /* Configure Flag */
        uint32_t Reserved_1  : 31;  /* Reserved */
    } B;
} IMX7_USB_CONFIGFLAG_t;

/*
 * USB_PORTSC1 register bits definition *
 */
typedef union IMX7_USB_PORTSC1_union_t {
    uint32_t R;
    struct {
        uint32_t CCS    :  1;  /* Current Connect Status */
        uint32_t CSC    :  1;  /* Connect Status Change */
        uint32_t PE     :  1;  /* Port Enabled/Disabled */
        uint32_t PEC    :  1;  /* Port Enable/Disable Change */
        uint32_t OCA    :  1;  /* Over-current Active */
        uint32_t OCC    :  1;  /* Over-current Change */
        uint32_t FPR    :  1;  /* Force Port Resume */
        uint32_t SUSP   :  1;  /* Suspend */
        uint32_t PR     :  1;  /* Port Reset */
        uint32_t HSP    :  1;  /* High-Speed Port */
        uint32_t LS     :  2;  /* Line Status */
        uint32_t PP     :  1;  /* Port Power */
        uint32_t PO     :  1;  /* Port Owner */
        uint32_t PIC    :  2;  /* Port Indicator Control */
        uint32_t PTC    :  4;  /* Port Test Control */
        uint32_t WKDN   :  1;  /* Wake on Connect Enable */
        uint32_t WKDC   :  1;  /* Wake on Disconnect Enable */
        uint32_t WKOC   :  1;  /* Wake on Over-current Enable */
        uint32_t PHCD   :  1;  /* PHY Low Power Suspend - Clock Disable (PLPSCD) */
        uint32_t PFSC   :  1;  /* Port Force Full Speed Connect */
        uint32_t PTS_2  :  1;  /* See description at PTS_1 bits */
        uint32_t PSPD   :  2;  /* Port Speed */
        uint32_t PTW    :  1;  /* Parallel Transceiver Width */
        uint32_t STS    :  1;  /* Serial Transceiver Select - Read Only */
        uint32_t PTS_1  :  2;  /* Parallel Transceiver Select */
    } B;
} IMX7_USB_PORTSC1_t;

/*
 * USB_OTGSC register bits definition *
 */
typedef union IMX7_USB_OTGSC_union_t {
    uint32_t R;
    struct {
        uint32_t VD           :  1;  /* VBUS_Discharge */
        uint32_t VC           :  1;  /* VBUS Charge */
        uint32_t Reserved_2   :  1;  /* Reserved */
        uint32_t OT           :  1;  /* OTG Termination */
        uint32_t DP           :  1;  /* Data Pulsing */
        uint32_t IDPU         :  1;  /* ID Pullup */
        uint32_t Reserved_6   :  2;  /* Reserved */
        uint32_t ID           :  1;  /* USB ID */
        uint32_t AVV          :  1;  /* A VBus Valid */
        uint32_t ASV          :  1;  /* A Session Valid */
        uint32_t BSV          :  1;  /* B Session Valid */
        uint32_t BSE          :  1;  /* B Session End */
        uint32_t TOG_1MS      :  1;  /* 1 millisecond timer toggle */
        uint32_t DPS          :  1;  /* Data Bus Pulsing Status */
        uint32_t Reserved_15  :  1;  /* Reserved */
        uint32_t IDIS         :  1;  /* USB ID Interrupt Status */
        uint32_t AVVIS        :  1;  /* A VBus Valid Interrupt Status */
        uint32_t ASVIS        :  1;  /* A Session Valid Interrupt Status */
        uint32_t BSVIS        :  1;  /* B Session Valid Interrupt Status */
        uint32_t BSEIS        :  1;  /* B Session End Interrupt Status */
        uint32_t STATUS_1MS   :  1;  /* 1 millisecond timer Interrupt Status */
        uint32_t DPIS         :  1;  /* Data Pulse Interrupt Status */
        uint32_t Reserved_23  :  1;  /* Reserved */
        uint32_t IDIE         :  1;  /* USB ID Interrupt Enable */
        uint32_t AVVIE        :  1;  /* A VBus Valid Interrupt Enable */
        uint32_t ASVIE        :  1;  /* A Session Valid Interrupt Enable */
        uint32_t BSVIE        :  1;  /* B Session Valid Interrupt Enable */
        uint32_t BSEIE        :  1;  /* B Session End Interrupt Enable */
        uint32_t EN_1MS       :  1;  /* 1 millisecond timer Interrupt Enable */
        uint32_t DPIE         :  1;  /* Data Pulse Interrupt Enable */
        uint32_t Reserved_31  :  1;  /* Reserved */
    } B;
} IMX7_USB_OTGSC_t;

/*
 * USB_USBMODE register bits definition *
 */
typedef union IMX7_USB_USBMODE_union_t {
    uint32_t R;
    struct {
        uint32_t CM          :  2;  /* Controller Mode */
        uint32_t ES          :  1;  /* Endian Select */
        uint32_t SLOM        :  1;  /* Setup Lockout Mode */
        uint32_t SDIS        :  1;  /* Stream Disable Mode */
        uint32_t Reserved_5  : 27;  /* Reserved */
    } B;
} IMX7_USB_USBMODE_t;

/*
 * USB_ENDPTSETUPSTAT register bits definition *
 */
typedef union IMX7_USB_ENDPTSETUPSTAT_union_t {
    uint32_t R;
    struct {
        uint32_t ENDPTSETUPSTAT  : 16;  /* Setup Endpoint Status */
        uint32_t Reserved_16     : 16;  /* Reserved */
    } B;
} IMX7_USB_ENDPTSETUPSTAT_t;

/*
 * USB_ENDPTPRIME register bits definition *
 */
typedef union IMX7_USB_ENDPTPRIME_union_t {
    uint32_t R;
    struct {
        uint32_t PERB         :  8;  /* Prime Endpoint Receive Buffer */
        uint32_t Reserved_8   :  8;  /* Reserved */
        uint32_t PETB         :  8;  /* Prime Endpoint Transmit Buffer */
        uint32_t Reserved_24  :  8;  /* Reserved */
    } B;
} IMX7_USB_ENDPTPRIME_t;

/*
 * USB_ENDPTFLUSH register bits definition *
 */
typedef union IMX7_USB_ENDPTFLUSH_union_t {
    uint32_t R;
    struct {
        uint32_t FERB         :  8;  /* Flush Endpoint Receive Buffer */
        uint32_t Reserved_8   :  8;  /* Reserved */
        uint32_t FETB         :  8;  /* Flush Endpoint Transmit Buffer */
        uint32_t Reserved_24  :  8;  /* Reserved */
    } B;
} IMX7_USB_ENDPTFLUSH_t;

/*
 * USB_ENDPTSTAT register bits definition *
 */
typedef union IMX7_USB_ENDPTSTAT_union_t {
    uint32_t R;
    struct {
        uint32_t ERBR         :  8;  /* Endpoint Receive Buffer Ready */
        uint32_t Reserved_8   :  8;  /* Reserved */
        uint32_t ETBR         :  8;  /* Endpoint Transmit Buffer Ready */
        uint32_t Reserved_24  :  8;  /* Reserved */
    } B;
} IMX7_USB_ENDPTSTAT_t;

/*
 * USB_ENDPTCOMPLETE register bits definition *
 */
typedef union IMX7_USB_ENDPTCOMPLETE_union_t {
    uint32_t R;
    struct {
        uint32_t ERCE         :  8;  /* Endpoint Receive Complete Event */
        uint32_t Reserved_8   :  8;  /* Reserved */
        uint32_t ETCE         :  8;  /* Endpoint Transmit Complete Event */
        uint32_t Reserved_24  :  8;  /* Reserved */
    } B;
} IMX7_USB_ENDPTCOMPLETE_t;

/*
 * USB_ENDPTCTRL0 register bits definition *
 */
typedef union IMX7_USB_ENDPTCTRL0_union_t {
    uint32_t R;
    struct {
        uint32_t RXS          :  1;  /* RX Endpoint Stall */
        uint32_t Reserved_1   :  1;  /* Reserved */
        uint32_t RX           :  2;  /* RX Endpoint Type */
        uint32_t Reserved_4   :  3;  /* Reserved */
        uint32_t RXE          :  1;  /* RX Endpoint Enable */
        uint32_t Reserved_8   :  8;  /* Reserved */
        uint32_t TXS          :  1;  /* TX Endpoint Stall */
        uint32_t Reserved_17  :  1;  /* Reserved */
        uint32_t TX           :  2;  /* TX Endpoint Type */
        uint32_t Reserved_20  :  3;  /* Reserved */
        uint32_t TXE          :  1;  /* TX Endpoint Enable */
        uint32_t Reserved_24  :  8;  /* Reserved */
    } B;
} IMX7_USB_ENDPTCTRL0_t;

/*
 * USB_ENDPTCTRL register bits definition *
 */
typedef union IMX7_USB_ENDPTCTRL_union_t {
    uint32_t R;
    struct {
        uint32_t RXS          :  1;  /* RX Endpoint Stall */
        uint32_t Reserved_1   :  1;  /* Reserved */
        uint32_t RX           :  2;  /* RX Endpoint Type */
        uint32_t Reserved_4   :  1;  /* Reserved */
        uint32_t RXI          :  1;  /* RX Data Toggle Inhibit */
        uint32_t RXR          :  1;  /* RX Data Toggle Reset (WS) */
        uint32_t RXE          :  1;  /* RX Endpoint Enable */
        uint32_t Reserved_8   :  8;  /* Reserved */
        uint32_t TXS          :  1;  /* TX Endpoint Stall */
        uint32_t Reserved_17  :  1;  /* Reserved */
        uint32_t TX           :  2;  /* TX Endpoint Type */
        uint32_t Reserved_20  :  1;  /* Reserved */
        uint32_t TXI          :  1;  /* TX Data Toggle Inhibit */
        uint32_t TXR          :  1;  /* TX Data Toggle Reset (WS) */
        uint32_t TXE          :  1;  /* TX Endpoint Enable */
        uint32_t Reserved_24  :  8;  /* Reserved */
    } B;
} IMX7_USB_ENDPTCTRL_t;

/*
 * USBNC_OTG_CTRL1 register bits definition *
 */
typedef union IMX7_USBNC_OTG_CTRL1_union_t {
    uint32_t R;
    struct {
        uint32_t Reserved_6    :  7;  /* Reserved */
        uint32_t OVER_CUR_DIS  :  1;  /*  */
        uint32_t OVER_CUR_POL  :  1;  /*  */
        uint32_t PWR_POL       :  1;  /*  */
        uint32_t WIE           :  1;  /*  */
        uint32_t Reserved_11   :  3;  /* Reserved */
        uint32_t WKUP_SW_EN    :  1;  /*  */
        uint32_t Reserved_15   :  1;  /* Reserved */
        uint32_t WKUP_ID_EN    :  1;  /*  */
        uint32_t WKUP_VBUS_EN  :  1;  /*  */
        uint32_t Reserved_18   : 11;  /* Reserved */
        uint32_t WKUP_DPDM_EN  :  1;  /*  */
        uint32_t Reserved_30   :  1;  /* Reserved */
        uint32_t WIR           :  1;  /*  */
    } B;
} IMX7_USBNC_OTG_CTRL1_t;

/*
 * USBNC_OTG_CTRL2 register bits definition *
 */
typedef union IMX7_USBNC_OTG_CTRL2_union_t {
    uint32_t R;
    struct {
        uint32_t VBUS_SOURCE_SEL  :  2;  /* VBUS source select when detect VBUS wakeup event, it is for UTMI PHY only (UH core has no such feature) */
        uint32_t AUTURESUME_EN    :  1;  /* Auto Resume Enable */
        uint32_t LOWSPEED_EN      :  1;  /* Set if AUTURESUME_EN is set and works on low speed */
        uint32_t Reserved_4       : 16;  /* Reserved */
        uint32_t DIG_ID_SEL       :  1;  /* Set to select the ID from one GPIO input, it also affects bit4 of USB_UTMIPHY_STATUS */
        uint32_t Reserved_21      : 10;  /* Reserved */
        uint32_t UTMI_CLK_VLD     :  1;  /* Indicate whether PHY clock is valid */
    } B;
} IMX7_USBNC_OTG_CTRL2_t;

/*
 * USBNC_OTG_PHY_CFG1 register bits definition *
 */
typedef union IMX7_USBNC_OTG_PHY_CFG1_union_t {
    uint32_t R;
    struct {
        uint32_t COMMONONN           :  1;  /* Common Block Power-Down Control */
        uint32_t FSEL                :  3;  /* Reference Clock Frequency Select */
        uint32_t COMPDISTUNE0        :  3;  /* Disconnect Threshold Adjustment */
        uint32_t SQRXTUNE0           :  3;  /* Squelch Threshold Adjustment */
        uint32_t OTGTUNE0            :  3;  /* VBUS Valid Threshold Adjustment */
        uint32_t TXHSXVTUNE0         :  2;  /* Transmitter High-Speed Crossover Adjustment */
        uint32_t Reserved_15         :  1;  /* Reserved */
        uint32_t TXFSLSTUNE0         :  4;  /* FS/LS Source Impedance Adjustment */
        uint32_t TXVREFTUNE0         :  4;  /* HS DC Voltage Level Adjustment */
        uint32_t TXRISETUNE0         :  2;  /* HS Transmitter Rise/Fall Time Adjustment */
        uint32_t TXRESTUNE0          :  2;  /* USB Source Impedance Adjustment */
        uint32_t TXPREEMPAMPTUNE0    :  2;  /* HS Treansmitter Pre-Emphasis Current Control */
        uint32_t TXPREEMPPULSETUNE0  :  1;  /* HS Transmitter Pre-Emphasis Duration Control */
        uint32_t CHRGDET_Megamix     :  1;  /* USB_OTG1_CHD_B output control */
    } B;
} IMX7_USBNC_OTG_PHY_CFG1_t;

/*
 * USBNC_OTG_PHY_CFG2 register bits definition *
 */
typedef union IMX7_USBNC_OTG_PHY_CFG2_union_t {
    uint32_t R;
    struct {
        uint32_t CHRGSEL         :  1;  /* Battery Charging Source Select */
        uint32_t VDATDETENB0     :  1;  /* Battery Charging Attach / Connect Detection Enable */
        uint32_t VDATSRCENB0     :  1;  /* Battery Charging Source Select */
        uint32_t DCDENB          :  1;  /* Data Contact Detection Enable */
        uint32_t ACAENB0         :  1;  /* ACA USB_OTG*_ID Pin Resistance Detection Enable */
        uint32_t SLEEPM0         :  1;  /* Sleep Mode Assertion */
        uint32_t LOOPBACKENB0    :  1;  /* Loopback Test Enable */
        uint32_t Reserved_7      :  1;  /* Reserved */
        uint32_t TXBITSTUFFEN0   :  1;  /* Low-Byte Transmit Bit-Stuffing Enable */
        uint32_t TXBITSTUFFENH0  :  1;  /* High-Byte Transmit Bit-Stuffing Enable */
        uint32_t OTGDISABLE0     :  1;  /* OTG Block Disable */
        uint32_t ADPCHRG0        :  1;  /* VBUS Input ADP Charge Enable */
        uint32_t ADPDISCHRG0     :  1;  /* VBUS Input ADP Discharge Enable */
        uint32_t ADPPRBENB0      :  1;  /* ADP Probe Enable */
        uint32_t VBUSVLDEXTSEL0  :  1;  /* External VBUS Valid Select */
        uint32_t VBUSVLDEXT      :  1;  /* External VBUS Valid Indicator */
        uint32_t DRVVBUS0        :  1;  /* VBUS Valid Comparator Enable */
        uint32_t Reserved_17     : 15;  /* Reserved */
    } B;
} IMX7_USBNC_OTG_PHY_CFG2_t;

/*
 * USBNC_PHY_STATUS register bits definition *
 */
typedef union IMX7_USBNC_PHY_STATUS_union_t {
    uint32_t R;
    struct {
        uint32_t LINE_STATE       :  2;  /* Line State Indicator outputs from USB OTG PHY */
        uint32_t SESS_VLD         :  1;  /* OTG Device Session Valid Indicator from USB OTG PHY */
        uint32_t VBUS_VLD         :  1;  /* VBUS Valid Indicator from USB OTG PHY */
        uint32_t ID_DIG           :  1;  /* Micro- or Mini- A/B Plug Indicator */
        uint32_t HOST_DISCONNECT  :  1;  /* Peripheral Disconnect Indicator */
        uint32_t Reserved_6       : 18;  /* Reserved */
        uint32_t RIDC0            :  1;  /* ACA USB_OTG*_ID Pin Resistance Indicator */
        uint32_t RIDB0            :  1;  /* Wake */
        uint32_t RIDA0            :  1;  /* Wake */
        uint32_t RIDGND0          :  1;  /* Wake */
        uint32_t RIDFLOAT0        :  1;  /* Wake */
        uint32_t CHRGDET          :  1;  /* Battery Charger Detection Output */
        uint32_t ADPPRB0          :  1;  /* ADP Probe Indicator */
        uint32_t ADPSNS0          :  1;  /* ADP Sense Indicator */
    } B;
} IMX7_USBNC_PHY_STATUS_t;

/*
 * USBNC_UH_HSICPHY_CFG1 register bits definition *
 */
typedef union IMX7_USBNC_UH_HSICPHY_CFG1_union_t {
    uint32_t R;
    struct {
        uint32_t COMMONONN      :  1;  /* This clock configuration setting should not be changed and must remain at 1'b1. */
        uint32_t LOOPBACKENB    :  1;  /* Loopback test enable */
        uint32_t DPPULLDOWN     :  1;  /* Bus keeper resistors enable */
        uint32_t DMPULLDOWN     :  1;  /* Bus keeper resistors enable */
        uint32_t SLEEPM         :  1;  /* Sleep mode assertion */
        uint32_t TXBITSTUFFEN   :  1;  /* Low-Byte Transmit Bit-Stuffing Enable */
        uint32_t TXBITSTUFFENH  :  1;  /* High-Byte Transmit Bit-Stuffing Enable */
        uint32_t Reserved_7     :  1;  /* Reserved */
        uint32_t TXRPUTUNE      :  2;  /* Driver pull-up single-ended source impedance adjustment for USB_H_DATA and USB_H_STROBE pins when driving low. */
        uint32_t TXRPDTUNE      :  2;  /* Driver pull-down single-ended source impedance adjustment for USB_H_DATA and USB_H_STROBE pins when driving low. */
        uint32_t TXSRTUNE       :  4;  /* Driver slew rate adjustment for USB_H_DATA and USB_H_STROBE pins. */
        uint32_t REFCLKDIV      :  7;  /* Reference clock frequency select. */
        uint32_t Reserved_23    :  1;  /* Reserved */
        uint32_t REFCLKSEL      :  2;  /* Reference clock select. */
        uint32_t Reserved_26    :  6;  /* Reserved */
    } B;
} IMX7_USBNC_UH_HSICPHY_CFG1_t;

/*
 * USBNC_ADP_CFG1 register bits definition *
 */
typedef union IMX7_USBNC_ADP_CFG1_union_t {
    uint32_t R;
    struct {
        uint32_t ADP_WAIT        : 18;  /* Delay between 2 ADP probe, default value is about 1.5 s in 32 KHz. */
        uint32_t Reserved_18     :  2;  /* Reserved */
        uint32_t TIMER_EN        :  1;  /* ADP Timer Test Enable */
        uint32_t ADP_SNS_INT_EN  :  1;  /* ADP Sense Interrupt Enable */
        uint32_t ADP_PRB_INT_EN  :  1;  /* ADP probe interrupt enable */
        uint32_t ADP_PRB_EN      :  1;  /* Set to enable the ADP probe sequence */
        uint32_t Reserved_24     :  8;  /* Reserved */
    } B;
} IMX7_USBNC_ADP_CFG1_t;

/*
 * USBNC_ADP_CFG2 register bits definition *
 */
typedef union IMX7_USBNC_ADP_CFG2_union_t {
    uint32_t R;
    struct {
        uint32_t ADP_CHRG_DELTA   :  7;  /* ADP charge time compare */
        uint32_t ADP_CHRG_SWCMP   :  1;  /* If set, HW uses ADP_CHRG_SWTIME to compare. If clear, HW compares current charge time with n-2 */
        uint32_t ADP_CHRG_SWTIME  :  8;  /* ADP charge time assigned by software */
        uint32_t ADP_DISCHG_TIME  :  8;  /* ADP Discharge time */
        uint32_t Reserved_24      :  8;  /* Reserved */
    } B;
} IMX7_USBNC_ADP_CFG2_t;

/*
 * USBNC_ADP_STATUS register bits definition *
 */
typedef union IMX7_USBNC_ADP_STATUS_union_t {
    uint32_t R;
    struct {
        uint32_t ADP_PRB_TIMR  :  8;  /* ADP Probe Time */
        uint32_t ADP_CNT       : 18;  /* ADP Internal 18-bit Counter */
        uint32_t ADP_SNS_INT   :  1;  /* ADP Sense Interrupt Status */
        uint32_t ADP_PRB_INT   :  1;  /* ADP Probe Interrupt Status */
        uint32_t Reserved_28   :  4;  /* Reserved */
    } B;
} IMX7_USBNC_ADP_STATUS_t;

/*
 * USB_OTG peripheral register structure *
 */
typedef struct IMX7_USB_OTG_struct_t {
    IMX7_USB_ID_t                 USB_ID;                /* 0x00000000 Identification register */
    IMX7_USB_HWGENERAL_t          USB_HWGENERAL;         /* 0x00000004 General hardware parameters register */
    IMX7_USB_HWHOST_t             USB_HWHOST;            /* 0x00000008 Host hardware parameter register */
    IMX7_USB_HWDEVICE_t           USB_HWDEVICE;          /* 0x0000000C Device hardware parameter register */
    IMX7_USB_HWTXBUF_t            USB_HWTXBUF;           /* 0x00000010 TX buffer hardware parameters register */
    IMX7_USB_HWRXBUF_t            USB_HWRXBUF;           /* 0x00000014 RX buffer hardware parameters register */
    uint8_t                      Reserved_24[104];      /* 0x00000018 Reserved */
    IMX7_USB_GPTIMER0LD_t         USB_GPTIMER0LD;        /* 0x00000080 General purpose timer #0 Load register */
    IMX7_USB_GPTIMER0CTRL_t       USB_GPTIMER0CTRL;      /* 0x00000084 General purpose timer #0 controller register */
    IMX7_USB_GPTIMER1LD_t         USB_GPTIMER1LD;        /* 0x00000088 General purpose timer #1 Load register */
    IMX7_USB_GPTIMER1CTRL_t       USB_GPTIMER1CTRL;      /* 0x0000008C General purpose timer #1 controller register */
    IMX7_USB_SBUSCFG_t            USB_SBUSCFG;           /* 0x00000090 System bus config register */
    uint8_t                      Reserved_148[108];     /* 0x00000094 Reserved */
    IMX7_USB_CAPLENGTH_t          USB_CAPLENGTH;         /* 0x00000100 Capability Registers Length */
    uint8_t                      Reserved_257[1];       /* 0x00000101 Reserved */
    IMX7_USB_HCIVERSION_t         USB_HCIVERSION;        /* 0x00000102 Host Controller Interface Version */
    IMX7_USB_HCSPARAMS_t          USB_HCSPARAMS;         /* 0x00000104 Identification register */
    IMX7_USB_HCCPARAMS_t          USB_HCCPARAMS;         /* 0x00000108 Identification register */
    uint8_t                      Reserved_268[20];      /* 0x0000010C Reserved */
    IMX7_USB_DCIVERSION_t         USB_DCIVERSION;        /* 0x00000120 Device Controller Interface Version */
    uint8_t                      Reserved_290[2];       /* 0x00000122 Reserved */
    IMX7_USB_DCCPARAMS_t          USB_DCCPARAMS;         /* 0x00000124 Identification register */
    uint8_t                      Reserved_296[24];      /* 0x00000128 Reserved */
    IMX7_USB_USBCMD_t             USB_USBCMD;            /* 0x00000140 USB Command Register */
    IMX7_USB_USBSTS_t             USB_USBSTS;            /* 0x00000144 USB Status Register */
    IMX7_USB_USBINTR_t            USB_USBINTR;           /* 0x00000148 Interrupt Enable Register */
    IMX7_USB_FRINDEX_t            USB_FRINDEX;           /* 0x0000014C USB Frame Index */
    uint8_t                      Reserved_336[4];       /* 0x00000150 Reserved */
    IMX7_USB_PERIODICLISTBASE_t   USB_PERIODICLISTBASE;  /* 0x00000154 Frame List Base Address */
    IMX7_USB_ASYNCLISTADDR_t      USB_ASYNCLISTADDR;     /* 0x00000158 Next Asynch. Address */
    uint8_t                      Reserved_348[4];       /* 0x0000015C Reserved */
    IMX7_USB_BURSTSIZE_t          USB_BURSTSIZE;         /* 0x00000160 Programmable Burst Size */
    IMX7_USB_TXFILLTUNING_t       USB_TXFILLTUNING;      /* 0x00000164 TX FIFO Fill Tuning */
    uint8_t                      Reserved_360[16];      /* 0x00000168 Reserved */
    IMX7_USB_ENDPTNAK_t           USB_ENDPTNAK;          /* 0x00000178 Endpoint NAK */
    IMX7_USB_ENDPTNAKEN_t         USB_ENDPTNAKEN;        /* 0x0000017C Endpoint NAK Enable */
    IMX7_USB_CONFIGFLAG_t         USB_CONFIGFLAG;        /* 0x00000180 Configure Flag Register */
    IMX7_USB_PORTSC1_t            USB_PORTSC1;           /* 0x00000184 Port Status & Control */
    uint8_t                      Reserved_392[28];      /* 0x00000188 Reserved */
    IMX7_USB_OTGSC_t              USB_OTGSC;             /* 0x000001A4 On-The-Go Status & control */
    IMX7_USB_USBMODE_t            USB_USBMODE;           /* 0x000001A8 USB Device Mode */
    IMX7_USB_ENDPTSETUPSTAT_t     USB_ENDPTSETUPSTAT;    /* 0x000001AC Endpoint Setup Status */
    IMX7_USB_ENDPTPRIME_t         USB_ENDPTPRIME;        /* 0x000001B0 Endpoint Prime */
    IMX7_USB_ENDPTFLUSH_t         USB_ENDPTFLUSH;        /* 0x000001B4 Endpoint Flush */
    IMX7_USB_ENDPTSTAT_t          USB_ENDPTSTAT;         /* 0x000001B8 Endpoint Status */
    IMX7_USB_ENDPTCOMPLETE_t      USB_ENDPTCOMPLETE;     /* 0x000001BC Endpoint Complete */
    IMX7_USB_ENDPTCTRL0_t         USB_ENDPTCTRL0;        /* 0x000001C0 Endpoint 0 settings */
    IMX7_USB_ENDPTCTRL_t          USB_ENDPTCTRL[7];      /* 0x000001C4 Endpoint settings array */
    uint8_t                      Reserved_459[53];      /* 0x000001CB Reserved */
    IMX7_USBNC_OTG_CTRL1_t        USBNC_OTG_CTRL1;       /* 0x00000200 USBNC_n_CTRL1 */
    IMX7_USBNC_OTG_CTRL2_t        USBNC_OTG_CTRL2;       /* 0x00000204 USBNC_n_CTRL2 */
    uint8_t                      Reserved_520[40];      /* 0x00000208 Reserved */
    IMX7_USBNC_OTG_PHY_CFG1_t     USBNC_OTG_PHY_CFG1;    /* 0x00000230 USB OTG PHY Configuration Register 1 */
    IMX7_USBNC_OTG_PHY_CFG2_t     USBNC_OTG_PHY_CFG2;    /* 0x00000234 USB OTG PHY Configuration Register 2 */
    uint8_t                      Reserved_568[4];       /* 0x00000238 Reserved */
    IMX7_USBNC_PHY_STATUS_t       USBNC_PHY_STATUS;      /* 0x0000023C USB OTG PHY Status Register */
    IMX7_USBNC_UH_HSICPHY_CFG1_t  USBNC_UH_HSICPHY_CFG1; /* 0x00000240 USB Host HSIC PHY configuration register */
    uint8_t                      Reserved_580[12];      /* 0x00000244 Reserved */
    IMX7_USBNC_ADP_CFG1_t         USBNC_ADP_CFG1;        /* 0x00000250 USBNC_ADP_CFG1 */
    IMX7_USBNC_ADP_CFG2_t         USBNC_ADP_CFG2;        /* 0x00000254 USBNC_ADP_CFG2 */
    IMX7_USBNC_ADP_STATUS_t       USBNC_ADP_STATUS;      /* 0x00000258 USBNC_ADP_STATUS */
} IMX7_USB_OTG_t;

/*
 * USB_OTG peripheral register pointers structure *
 */
typedef struct IMX7_USB_OTG_Ptr_struct_t {
    IMX7_USB_ID_t                 *pUSB_ID;                /* 0x00000000 Identification register */
    IMX7_USB_HWGENERAL_t          *pUSB_HWGENERAL;         /* 0x00000004 General hardware parameters register */
    IMX7_USB_HWHOST_t             *pUSB_HWHOST;            /* 0x00000008 Host hardware parameter register */
    IMX7_USB_HWDEVICE_t           *pUSB_HWDEVICE;          /* 0x0000000C Device hardware parameter register */
    IMX7_USB_HWTXBUF_t            *pUSB_HWTXBUF;           /* 0x00000010 TX buffer hardware parameters register */
    IMX7_USB_HWRXBUF_t            *pUSB_HWRXBUF;           /* 0x00000014 RX buffer hardware parameters register */
    IMX7_USB_GPTIMER0LD_t         *pUSB_GPTIMER0LD;        /* 0x00000080 General purpose timer #0 Load register */
    IMX7_USB_GPTIMER0CTRL_t       *pUSB_GPTIMER0CTRL;      /* 0x00000084 General purpose timer #0 controller register */
    IMX7_USB_GPTIMER1LD_t         *pUSB_GPTIMER1LD;        /* 0x00000088 General purpose timer #1 Load register */
    IMX7_USB_GPTIMER1CTRL_t       *pUSB_GPTIMER1CTRL;      /* 0x0000008C General purpose timer #1 controller register */
    IMX7_USB_SBUSCFG_t            *pUSB_SBUSCFG;           /* 0x00000090 System bus config register */
    IMX7_USB_CAPLENGTH_t          *pUSB_CAPLENGTH;         /* 0x00000100 Capability Registers Length */
    IMX7_USB_HCIVERSION_t         *pUSB_HCIVERSION;        /* 0x00000102 Host Controller Interface Version */
    IMX7_USB_HCSPARAMS_t          *pUSB_HCSPARAMS;         /* 0x00000104 Identification register */
    IMX7_USB_HCCPARAMS_t          *pUSB_HCCPARAMS;         /* 0x00000108 Identification register */
    IMX7_USB_DCIVERSION_t         *pUSB_DCIVERSION;        /* 0x00000120 Device Controller Interface Version */
    IMX7_USB_DCCPARAMS_t          *pUSB_DCCPARAMS;         /* 0x00000124 Identification register */
    IMX7_USB_USBCMD_t             *pUSB_USBCMD;            /* 0x00000140 USB Command Register */
    IMX7_USB_USBSTS_t             *pUSB_USBSTS;            /* 0x00000144 USB Status Register */
    IMX7_USB_USBINTR_t            *pUSB_USBINTR;           /* 0x00000148 Interrupt Enable Register */
    IMX7_USB_FRINDEX_t            *pUSB_FRINDEX;           /* 0x0000014C USB Frame Index */
    IMX7_USB_PERIODICLISTBASE_t   *pUSB_PERIODICLISTBASE;  /* 0x00000154 Frame List Base Address */
    IMX7_USB_ASYNCLISTADDR_t      *pUSB_ASYNCLISTADDR;     /* 0x00000158 Next Asynch. Address */
    IMX7_USB_BURSTSIZE_t          *pUSB_BURSTSIZE;         /* 0x00000160 Programmable Burst Size */
    IMX7_USB_TXFILLTUNING_t       *pUSB_TXFILLTUNING;      /* 0x00000164 TX FIFO Fill Tuning */
    IMX7_USB_ENDPTNAK_t           *pUSB_ENDPTNAK;          /* 0x00000178 Endpoint NAK */
    IMX7_USB_ENDPTNAKEN_t         *pUSB_ENDPTNAKEN;        /* 0x0000017C Endpoint NAK Enable */
    IMX7_USB_CONFIGFLAG_t         *pUSB_CONFIGFLAG;        /* 0x00000180 Configure Flag Register */
    IMX7_USB_PORTSC1_t            *pUSB_PORTSC1;           /* 0x00000184 Port Status & Control */
    IMX7_USB_OTGSC_t              *pUSB_OTGSC;             /* 0x000001A4 On-The-Go Status & control */
    IMX7_USB_USBMODE_t            *pUSB_USBMODE;           /* 0x000001A8 USB Device Mode */
    IMX7_USB_ENDPTSETUPSTAT_t     *pUSB_ENDPTSETUPSTAT;    /* 0x000001AC Endpoint Setup Status */
    IMX7_USB_ENDPTPRIME_t         *pUSB_ENDPTPRIME;        /* 0x000001B0 Endpoint Prime */
    IMX7_USB_ENDPTFLUSH_t         *pUSB_ENDPTFLUSH;        /* 0x000001B4 Endpoint Flush */
    IMX7_USB_ENDPTSTAT_t          *pUSB_ENDPTSTAT;         /* 0x000001B8 Endpoint Status */
    IMX7_USB_ENDPTCOMPLETE_t      *pUSB_ENDPTCOMPLETE;     /* 0x000001BC Endpoint Complete */
    IMX7_USB_ENDPTCTRL0_t         *pUSB_ENDPTCTRL0;        /* 0x000001C0 Endpoint 0 settings */
    IMX7_USB_ENDPTCTRL_t          *pUSB_ENDPTCTRL1;        /* 0x000001C4 Endpoint settings */
    IMX7_USB_ENDPTCTRL_t          *pUSB_ENDPTCTRL2;        /* 0x000001C8 Endpoint settings */
    IMX7_USB_ENDPTCTRL_t          *pUSB_ENDPTCTRL3;        /* 0x000001CC Endpoint settings */
    IMX7_USB_ENDPTCTRL_t          *pUSB_ENDPTCTRL4;        /* 0x000001D0 Endpoint settings */
    IMX7_USB_ENDPTCTRL_t          *pUSB_ENDPTCTRL5;        /* 0x000001D4 Endpoint settings */
    IMX7_USB_ENDPTCTRL_t          *pUSB_ENDPTCTRL6;        /* 0x000001D8 Endpoint settings */
    IMX7_USB_ENDPTCTRL_t          *pUSB_ENDPTCTRL7;        /* 0x000001DC Endpoint settings */
    IMX7_USBNC_OTG_CTRL1_t        *pUSBNC_OTG_CTRL1;       /* 0x00000200 USBNC_n_CTRL1 */
    IMX7_USBNC_OTG_CTRL2_t        *pUSBNC_OTG_CTRL2;       /* 0x00000204 USBNC_n_CTRL2 */
    IMX7_USBNC_OTG_PHY_CFG1_t     *pUSBNC_OTG_PHY_CFG1;    /* 0x00000230 USB OTG PHY Configuration Register 1 */
    IMX7_USBNC_OTG_PHY_CFG2_t     *pUSBNC_OTG_PHY_CFG2;    /* 0x00000234 USB OTG PHY Configuration Register 2 */
    IMX7_USBNC_PHY_STATUS_t       *pUSBNC_PHY_STATUS;      /* 0x0000023C USB OTG PHY Status Register */
    IMX7_USBNC_UH_HSICPHY_CFG1_t  *pUSBNC_UH_HSICPHY_CFG1; /* 0x00000240 USB Host HSIC PHY configuration register */
    IMX7_USBNC_ADP_CFG1_t         *pUSBNC_ADP_CFG1;        /* 0x00000250 USBNC_ADP_CFG1 */
    IMX7_USBNC_ADP_CFG2_t         *pUSBNC_ADP_CFG2;        /* 0x00000254 USBNC_ADP_CFG2 */
    IMX7_USBNC_ADP_STATUS_t       *pUSBNC_ADP_STATUS;      /* 0x00000258 USBNC_ADP_STATUS */
} IMX7_USB_OTG_Ptr_t;

#endif /* IMX7_USB_OTG_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/arm/imx7/imx7_usb.h $ $Rev: 834524 $")
#endif
