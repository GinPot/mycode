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


#ifndef IMX7_FLEXCAN_H_
#define IMX7_FLEXCAN_H_

/* FlexCAN registers */
#define IMX7_FLEXCAN0_REG_BASE                       0x30A00000
#define IMX7_FLEXCAN1_REG_BASE                       0x30A10000
#define IMX7_FLEXCAN0_MEM_BASE                       0x30A00080
#define IMX7_FLEXCAN1_MEM_BASE                       0x30A10080
#define IMX7_FLEXCAN_REG_SIZE                        0x40
#define IMX7_FLEXCAN_MEM_SIZE                        0x400
#define IMX7_FLEXCAN_LAM_MEM_SIZE                    0x100
#define IMX7_FLEXCAN_NUM_MAILBOX                     64
#define IMX7_FLEXCAN_MAILBOX_SIZE                    4
#define IMX7_FLEXCAN_SET_MODE_RETRIES                255


/* FlexCAN Register Offsets */
#define IMX7_FLEXCAN_MCR                             0x00
#define IMX7_FLEXCAN_CTRL1                           0x04
#define IMX7_FLEXCAN_TIMER                           0x08
#define IMX7_FLEXCAN_RXGMASK                         0x10
#define IMX7_FLEXCAN_RX14MASK                        0x14
#define IMX7_FLEXCAN_RX15MASK                        0x18
#define IMX7_FLEXCAN_ECR                             0x1C
#define IMX7_FLEXCAN_ESR1                            0x20
#define IMX7_FLEXCAN_IMASK2                          0x24
#define IMX7_FLEXCAN_IMASK1                          0x28
#define IMX7_FLEXCAN_IFLAG2                          0x2C
#define IMX7_FLEXCAN_IFLAG1                          0x30
#define IMX7_FLEXCAN_CTRL2                           0x34

/* Message Buffers MB0-MB15 */
#define IMX7_FLEXCAN_MB0                             0x80
#define IMX7_FLEXCAN_MB1                             0x90
#define IMX7_FLEXCAN_MB2                             0xA0
#define IMX7_FLEXCAN_MB3                             0xB0
#define IMX7_FLEXCAN_MB4                             0xC0
#define IMX7_FLEXCAN_MB5                             0xD0
#define IMX7_FLEXCAN_MB6                             0xE0
#define IMX7_FLEXCAN_MB7                             0xF0
#define IMX7_FLEXCAN_MB8                             0x100
#define IMX7_FLEXCAN_MB9                             0x110
#define IMX7_FLEXCAN_MB10                            0x120
#define IMX7_FLEXCAN_MB11                            0x130
#define IMX7_FLEXCAN_MB12                            0x140
#define IMX7_FLEXCAN_MB13                            0x150
#define IMX7_FLEXCAN_MB14                            0x160
#define IMX7_FLEXCAN_MB15                            0x170

/*Message Buffers MB16-MB31 */
#define IMX7_FLEXCAN_MB16                            0x180
#define IMX7_FLEXCAN_MB17                            0x190
#define IMX7_FLEXCAN_MB18                            0x1A0
#define IMX7_FLEXCAN_MB19                            0x1B0
#define IMX7_FLEXCAN_MB20                            0x1C0
#define IMX7_FLEXCAN_MB21                            0x1D0
#define IMX7_FLEXCAN_MB22                            0x1E0
#define IMX7_FLEXCAN_MB23                            0x1F0
#define IMX7_FLEXCAN_MB24                            0x200
#define IMX7_FLEXCAN_MB25                            0x210
#define IMX7_FLEXCAN_MB26                            0x220
#define IMX7_FLEXCAN_MB27                            0x230
#define IMX7_FLEXCAN_MB28                            0x240
#define IMX7_FLEXCAN_MB29                            0x250
#define IMX7_FLEXCAN_MB30                            0x260
#define IMX7_FLEXCAN_MB31                            0x270

/* Message Buffers MB32-MB63 */
#define IMX7_FLEXCAN_MB32                            0x280
#define IMX7_FLEXCAN_MB33                            0x290
#define IMX7_FLEXCAN_MB34                            0x2A0
#define IMX7_FLEXCAN_MB35                            0x2B0
#define IMX7_FLEXCAN_MB36                            0x2C0
#define IMX7_FLEXCAN_MB37                            0x2D0
#define IMX7_FLEXCAN_MB38                            0x2E0
#define IMX7_FLEXCAN_MB39                            0x2F0
#define IMX7_FLEXCAN_MB40                            0x300
#define IMX7_FLEXCAN_MB41                            0x310
#define IMX7_FLEXCAN_MB42                            0x320
#define IMX7_FLEXCAN_MB43                            0x330
#define IMX7_FLEXCAN_MB44                            0x340
#define IMX7_FLEXCAN_MB45                            0x350
#define IMX7_FLEXCAN_MB46                            0x360
#define IMX7_FLEXCAN_MB47                            0x370
#define IMX7_FLEXCAN_MB48                            0x380
#define IMX7_FLEXCAN_MB49                            0x390
#define IMX7_FLEXCAN_MB50                            0x3A0
#define IMX7_FLEXCAN_MB51                            0x3B0
#define IMX7_FLEXCAN_MB52                            0x3C0
#define IMX7_FLEXCAN_MB53                            0x3D0
#define IMX7_FLEXCAN_MB54                            0x3E0
#define IMX7_FLEXCAN_MB55                            0x3F0
#define IMX7_FLEXCAN_MB56                            0x400
#define IMX7_FLEXCAN_MB57                            0x410
#define IMX7_FLEXCAN_MB58                            0x420
#define IMX7_FLEXCAN_MB59                            0x430
#define IMX7_FLEXCAN_MB60                            0x440
#define IMX7_FLEXCAN_MB61                            0x450
#define IMX7_FLEXCAN_MB62                            0x460
#define IMX7_FLEXCAN_MB63                            0x470

/* Rx Individual Mask Registers RXIMR0-RXIMR15 */
#define IMX7_FLEXCAN_RXIMR0                          0x880
#define IMX7_FLEXCAN_RXIMR1                          0x884
#define IMX7_FLEXCAN_RXIMR2                          0x888
#define IMX7_FLEXCAN_RXIMR3                          0x88C
#define IMX7_FLEXCAN_RXIMR4                          0x890
#define IMX7_FLEXCAN_RXIMR5                          0x894
#define IMX7_FLEXCAN_RXIMR6                          0x898
#define IMX7_FLEXCAN_RXIMR7                          0x89C
#define IMX7_FLEXCAN_RXIMR8                          0x8A0
#define IMX7_FLEXCAN_RXIMR9                          0x8A4
#define IMX7_FLEXCAN_RXIMR10                         0x8A8
#define IMX7_FLEXCAN_RXIMR11                         0x8AC
#define IMX7_FLEXCAN_RXIMR12                         0x8B0
#define IMX7_FLEXCAN_RXIMR13                         0x8B4
#define IMX7_FLEXCAN_RXIMR14                         0x8B8
#define IMX7_FLEXCAN_RXIMR15                         0x8BC

/* Rx Individual Mask Registers RXIMR16-RXIMR31 */
#define IMX7_FLEXCAN_RXIMR16                         0x8C0
#define IMX7_FLEXCAN_RXIMR17                         0x8C4
#define IMX7_FLEXCAN_RXIMR18                         0x8C8
#define IMX7_FLEXCAN_RXIMR19                         0x8CC
#define IMX7_FLEXCAN_RXIMR20                         0x8D0
#define IMX7_FLEXCAN_RXIMR21                         0x8D4
#define IMX7_FLEXCAN_RXIMR22                         0x8D8
#define IMX7_FLEXCAN_RXIMR23                         0x8DC
#define IMX7_FLEXCAN_RXIMR24                         0x8E0
#define IMX7_FLEXCAN_RXIMR25                         0x8E4
#define IMX7_FLEXCAN_RXIMR26                         0x8E8
#define IMX7_FLEXCAN_RXIMR27                         0x8EC
#define IMX7_FLEXCAN_RXIMR28                         0x8F0
#define IMX7_FLEXCAN_RXIMR29                         0x8F4
#define IMX7_FLEXCAN_RXIMR30                         0x8F8
#define IMX7_FLEXCAN_RXIMR31                         0x8FC

/* Rx Individual Mask Registers RXIMR32-RXIMR63 */
#define IMX7_FLEXCAN_RXIMR32                         0x900
#define IMX7_FLEXCAN_RXIMR33                         0x904
#define IMX7_FLEXCAN_RXIMR34                         0x908
#define IMX7_FLEXCAN_RXIMR35                         0x90C
#define IMX7_FLEXCAN_RXIMR36                         0x910
#define IMX7_FLEXCAN_RXIMR37                         0x914
#define IMX7_FLEXCAN_RXIMR38                         0x918
#define IMX7_FLEXCAN_RXIMR39                         0x91C
#define IMX7_FLEXCAN_RXIMR40                         0x920
#define IMX7_FLEXCAN_RXIMR41                         0x924
#define IMX7_FLEXCAN_RXIMR42                         0x928
#define IMX7_FLEXCAN_RXIMR43                         0x92C
#define IMX7_FLEXCAN_RXIMR44                         0x930
#define IMX7_FLEXCAN_RXIMR45                         0x934
#define IMX7_FLEXCAN_RXIMR46                         0x938
#define IMX7_FLEXCAN_RXIMR47                         0x93C
#define IMX7_FLEXCAN_RXIMR48                         0x940
#define IMX7_FLEXCAN_RXIMR49                         0x944
#define IMX7_FLEXCAN_RXIMR50                         0x948
#define IMX7_FLEXCAN_RXIMR51                         0x94C
#define IMX7_FLEXCAN_RXIMR52                         0x950
#define IMX7_FLEXCAN_RXIMR53                         0x954
#define IMX7_FLEXCAN_RXIMR54                         0x958
#define IMX7_FLEXCAN_RXIMR55                         0x95C
#define IMX7_FLEXCAN_RXIMR56                         0x960
#define IMX7_FLEXCAN_RXIMR57                         0x964
#define IMX7_FLEXCAN_RXIMR58                         0x968
#define IMX7_FLEXCAN_RXIMR59                         0x96C
#define IMX7_FLEXCAN_RXIMR60                         0x970
#define IMX7_FLEXCAN_RXIMR61                         0x974
#define IMX7_FLEXCAN_RXIMR62                         0x978
#define IMX7_FLEXCAN_RXIMR63                         0x97C

/* Bit definitions for FlexCAN Module Configuration (CANMC) Register */
#define IMX7_FLEXCAN_MCR_MDIS                        (0x01 << 31)
#define IMX7_FLEXCAN_MCR_FRZ                         (0x01 << 30)
#define IMX7_FLEXCAN_MCR_FEN                         (0x01 << 29)
#define IMX7_FLEXCAN_MCR_HALT                        (0x01 << 28)
#define IMX7_FLEXCAN_MCR_NOTRDY                      (0x01 << 27)
#define IMX7_FLEXCAN_MCR_WAKEMSK                     (0x01 << 26)
#define IMX7_FLEXCAN_MCR_SOFTRST                     (0x01 << 25)
#define IMX7_FLEXCAN_MCR_FRZACK                      (0x01 << 24)
#define IMX7_FLEXCAN_MCR_SUPV                        (0x01 << 23)
#define IMX7_FLEXCAN_MCR_SELFWAKE                    (0x01 << 22)
#define IMX7_FLEXCAN_MCR_WRN_EN                      (0x01 << 21)
#define IMX7_FLEXCAN_MCR_LPM_ACK                     (0x01 << 20)
#define IMX7_FLEXCAN_MCR_WAK_SRC                     (0x01 << 19)
#define IMX7_FLEXCAN_MCR_DOZE                        (0x01 << 18)
#define IMX7_FLEXCAN_MCR_SRX_DIS                     (0x01 << 17)
#define IMX7_FLEXCAN_MCR_IRMQ                        (0x01 << 16)
#define IMX7_FLEXCAN_MCR_LPRIO_EN                    (0x01 << 13)
#define IMX7_FLEXCAN_MCR_AEN                         (0x01 << 12)
#define IMX7_FLEXCAN_MCR_IDAM_FormatA                (0x00 << 8)
#define IMX7_FLEXCAN_MCR_IDAM_FormatB                (0x01 << 8)
#define IMX7_FLEXCAN_MCR_IDAM_FormatC                (0x10 << 8)
#define IMX7_FLEXCAN_MCR_IDAM_FormatD                (0x11 << 8)
#define IMX7_FLEXCAN_MCR_MAXMB_MASK                  0x0000003F
#define IMX7_FLEXCAN_MCR_MAXMB_MAXVAL                0x0000003F    /* 64 MBs */

/* Bit definitions for FlexCAN Control (CTRL) Register */
#define IMX7_FLEXCAN_CTRL1_PRESDIV_MASK              0xFF000000
#define IMX7_FLEXCAN_CTRL1_PRESDIV_MAXVAL            0xFF
#define IMX7_FLEXCAN_CTRL1_PRESDIV_SHIFT             24
#define IMX7_FLEXCAN_CTRL1_PRESDIV_SAM_MIN           5
#define IMX7_FLEXCAN_CTRL1_RJW_MASK                  (0x03 << 22)
#define IMX7_FLEXCAN_CTRL1_RJW_MAXVAL                0x3
#define IMX7_FLEXCAN_CTRL1_RJW_SHIFT                 22
#define IMX7_FLEXCAN_CTRL1_PSEG1_MASK                (0x07 << 19)
#define IMX7_FLEXCAN_CTRL1_PSEG1_MAXVAL              0x7
#define IMX7_FLEXCAN_CTRL1_PSEG1_SHIFT               19
#define IMX7_FLEXCAN_CTRL1_PSEG2_MASK                (0x07 << 16)
#define IMX7_FLEXCAN_CTRL1_PSEG2_MAXVAL              0x7
#define IMX7_FLEXCAN_CTRL1_PSEG2_SHIFT               16
#define IMX7_FLEXCAN_CTRL1_BOFFMSK                   (0x01 << 15)
#define IMX7_FLEXCAN_CTRL1_ERRMASK                   (0x01 << 14)
#define IMX7_FLEXCAN_CTRL1_CLKSRC                    (0x01 << 13)
#define IMX7_FLEXCAN_CTRL1_LPB                       (0x01 << 12)
#define IMX7_FLEXCAN_CTRL1_TWRNMSK                   (0x01 << 11)
#define IMX7_FLEXCAN_CTRL1_RWRNMSK                   (0x01 << 10)
#define IMX7_FLEXCAN_CTRL1_SAMP                      (0x01 << 7)
#define IMX7_FLEXCAN_CTRL1_BOFFREC                   (0x01 << 6)
#define IMX7_FLEXCAN_CTRL1_TSYNC                     (0x01 << 5)
#define IMX7_FLEXCAN_CTRL1_LBUF                      (0x01 << 4)
#define IMX7_FLEXCAN_CTRL1_LOM                       (0x01 << 3)
#define IMX7_FLEXCAN_CTRL1_PROPSEG_MASK              (0x07 << 0)
#define IMX7_FLEXCAN_CTRL1_PROPSEG_SHIFT             0

/* Bit definitions for FlexCAN Control 2 (CTRL2) Register */
#define IMX7_FLEXCAN_CTRL2_EACEN                     (0x01 << 16)


/* FlexCAN Error Counter Register */
#define IMX7_FLEXCAN_ECR_RXECTR                      0xFF00
#define IMX7_FLEXCAN_ECR_TXECTR                      0x00FF

/* Bit definitions for FlexCAN Error and Status (CANES) Register */
#define IMX7_FLEXCAN_ESR1_TWRNINT                    (0x01 << 17)
#define IMX7_FLEXCAN_ESR1_RWRNINT                    (0x01 << 16)
#define IMX7_FLEXCAN_ESR1_BITERR_RECESSIVE_DOMINANT  (0x01 << 15)
#define IMX7_FLEXCAN_ESR1_BITERR_DOMINANT_RECESSIVE  (0x01 << 14)
#define IMX7_FLEXCAN_ESR1_ACKERR                     (0x01 << 13)
#define IMX7_FLEXCAN_ESR1_CRCERR                     (0x01 << 12)
#define IMX7_FLEXCAN_ESR1_FORMERR                    (0x01 << 11)
#define IMX7_FLEXCAN_ESR1_STUFFERR                   (0x01 << 10)
#define IMX7_FLEXCAN_ESR1_TXWARN                     (0x01 << 9)
#define IMX7_FLEXCAN_ESR1_RXWARN                     (0x01 << 8)
#define IMX7_FLEXCAN_ESR1_IDLE                       (0x01 << 7)
#define IMX7_FLEXCAN_ESR1_TX_RX                      (0x01 << 6)
#define IMX7_FLEXCAN_ESR1_FCS_MASK                   (0x03 << 4)
#define IMX7_FLEXCAN_ESR1_FCS_SHIFT                   4
#define IMX7_FLEXCAN_ESR1_FCS_ERROR_ACTIVE           (0x00 << 4)
#define IMX7_FLEXCAN_ESR1_FCS_ERROR_PASSIVE          (0x01 << 4)
#define IMX7_FLEXCAN_ESR1_BOFFINT                    (0x01 << 2)
#define IMX7_FLEXCAN_ESR1_ERRINT                     (0x01 << 1)
#define IMX7_FLEXCAN_ESR1_WAKEINT                    (0x01 << 0)

/* Interrupt Mask Register */
#define IMX7_FLEXCAN_IMASK_BUFnM(x)                  (0x1<<(x))
#define IMX7_FLEXCAN_IMASK_BUFF_ENABLE_ALL           (0xFFFFFFFF)
#define IMX7_FLEXCAN_IMASK_BUFF_DISABLE_ALL          (0x00000000)

/* Interrupt Flag Register */
#define IMX7_FLEXCAN_IFLAG_BUFF_SET_ALL              (0xFFFFFFFF)
#define IMX7_FLEXCAN_IFLAG_BUFF_CLEAR_ALL            (0x00000000)
#define IMX7_FLEXCAN_IFLAG_BUFnM(x)                  (0x1<<(x))

/* Message Buffers */
#define IMX7_FLEXCAN_MB_CNT_CODE(x)                  (((x)&0x0F)<<24)
#define IMX7_FLEXCAN_MB_CNT_SRR                      (0x00400000)
#define IMX7_FLEXCAN_MB_CNT_IDE                      (0x00200000)
#define IMX7_FLEXCAN_MB_CNT_RTR                      (0x00100000)
#define IMX7_FLEXCAN_MB_CNT_LENGTH(x)                (((x)&0x0F)<<16)
#define IMX7_FLEXCAN_MB_CNT_TIMESTAMP(x)             ((x)&0xFFFF)

/* Bit definitions for FlexCAN Rx Global Mask (CANRXGMASK) Register */
#define IMX7_FLEXCAN_RXGMASK_MASK                    0xFFFFFFFF

#define IMX7_FLEXCAN_LAM_MASK                        0xFFFFFFFF

/* Data Length Code */
#define IMX7_FLEXCAN_MSG_BUF_DLC_MASK                0x000F0000
#define IMX7_FLEXCAN_MSG_BUF_DLC_SHIFT               16


#endif /* IMX7_FLEXCAN_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/arm/imx7/imx7_flexcan.h $ $Rev: 834524 $")
#endif
