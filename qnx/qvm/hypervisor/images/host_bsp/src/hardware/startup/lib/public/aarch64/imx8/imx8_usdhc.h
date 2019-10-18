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

#ifndef IMX_USDHC_H_
#define IMX_USDHC_H_



/* Register DS_ADDR - SDMA System Address */
#define IMX_USDHC_DS_ADDR                       0
/* Field DS_ADDR - DMA System Address */
#define IMX_USDHC_DS_ADDR_DS_ADDR_SHIFT         2
#define IMX_USDHC_DS_ADDR_DS_ADDR_MASK          0xFFFFFFFC
#define IMX_USDHC_DS_ADDR_DS_ADDR(v)            (((v) << 0x2) & 0xFFFFFFFC)



/* Register BLK_ATT - Block Attributes */
#define IMX_USDHC_BLK_ATT                       0x4
/* Field BLKSIZE - Transfer Block Size */
#define IMX_USDHC_BLK_ATT_BLKSIZE_SHIFT         0
#define IMX_USDHC_BLK_ATT_BLKSIZE_MASK          0x1FFF
#define IMX_USDHC_BLK_ATT_BLKSIZE(v)            (((v) << 0) & 0x1FFF)
/* Field BLKCNT - Blocks Count For Current Transfer */
#define IMX_USDHC_BLK_ATT_BLKCNT_SHIFT          16
#define IMX_USDHC_BLK_ATT_BLKCNT_MASK           0xFFFF0000
#define IMX_USDHC_BLK_ATT_BLKCNT(v)             (((v) << 0x10) & 0xFFFF0000)



/* Register CMD_ARG - Command Argument */
#define IMX_USDHC_CMD_ARG                       0x8
/* Field CMDARG - Command Argument */
#define IMX_USDHC_CMD_ARG_CMDARG_SHIFT          0
#define IMX_USDHC_CMD_ARG_CMDARG_MASK           0xFFFFFFFF
#define IMX_USDHC_CMD_ARG_CMDARG(v)             (((v) << 0) & 0xFFFFFFFF)



/* Register CMD_XFR_TYP - Command Transfer Type */
#define IMX_USDHC_CMD_XFR_TYP                   0xC
/* Field RSPTYP - Response Type Select */
#define IMX_USDHC_CMD_XFR_TYP_RSPTYP_SHIFT      16
#define IMX_USDHC_CMD_XFR_TYP_RSPTYP_MASK       0x30000
#define IMX_USDHC_CMD_XFR_TYP_RSPTYP(v)         (((v) << 16) & 0x30000)
/* Response types bit values */
/* Response Length 48, check Busy after response */
#define IMX_USDHC_CMD_XFR_TYP_RSPTYP_BV_48B     3
/* Response Length 48 */
#define IMX_USDHC_CMD_XFR_TYP_RSPTYP_BV_48      2
/* Response Length 136 */
#define IMX_USDHC_CMD_XFR_TYP_RSPTYP_BV_136     1
/*No Response */
#define IMX_USDHC_CMD_XFR_TYP_RSPTYP_BV_NORSP   0
/* Field CCCEN - Command CRC Check Enable */
#define IMX_USDHC_CMD_XFR_TYP_CCCEN_SHIFT       19
#define IMX_USDHC_CMD_XFR_TYP_CCCEN_MASK        0x80000
#define IMX_USDHC_CMD_XFR_TYP_CCCEN(v)          (((v) << 19) & 0x80000)
/* Field CICEN - Command Index Check Enable */
#define IMX_USDHC_CMD_XFR_TYP_CICEN_SHIFT       20
#define IMX_USDHC_CMD_XFR_TYP_CICEN_MASK        0x100000
#define IMX_USDHC_CMD_XFR_TYP_CICEN(v)          (((v) << 20) & 0x100000)
/* Field DPSEL - Data Present Select */
#define IMX_USDHC_CMD_XFR_TYP_DPSEL_SHIFT       21
#define IMX_USDHC_CMD_XFR_TYP_DPSEL_MASK        0x200000
#define IMX_USDHC_CMD_XFR_TYP_DPSEL(v)          (((v) << 21) & 0x200000)
/* Field CMDTYP - Command Type */
#define IMX_USDHC_CMD_XFR_TYP_CMDTYP_SHIFT      22
#define IMX_USDHC_CMD_XFR_TYP_CMDTYP_MASK       0xC00000
#define IMX_USDHC_CMD_XFR_TYP_CMDTYP(v)         (((v) << 22) & 0xC00000)
/* Command types */
/* Abort CMD12 or CMD52 for writing I/O Abort in CCCR. */
#define IMX_USDHC_CMD_XFR_TYP_CMDTYP_BV_ABORT   3
/* Resume CMD52 for writing Function Select in CCCR. */
#define IMX_USDHC_CMD_XFR_TYP_CMDTYP_BV_RESUME  2
/* Suspend CMD52 for writing Bus Suspend in CCCR. */
#define IMX_USDHC_CMD_XFR_TYP_CMDTYP_BV_SUSPEND 1
/* Normal Other commands. */
#define IMX_USDHC_CMD_XFR_TYP_CMDTYP_BV_NORMAL  0
/* Field CMDINX - Command Index */
#define IMX_USDHC_CMD_XFR_TYP_CMDINX_SHIFT      24
#define IMX_USDHC_CMD_XFR_TYP_CMDINX_MASK       0x3F000000
#define IMX_USDHC_CMD_XFR_TYP_CMDINX(v)         (((v) << 0x18) & 0x3F000000)



/* Register CMD_RSP0 - Command Response0 */
#define IMX_USDHC_CMD_RSP0                      0x10
/* Field CMDRSP0 - Command Response 0 */
#define IMX_USDHC_CMD_RSP0_CMDRSP0_SHIFT        0
#define IMX_USDHC_CMD_RSP0_CMDRSP0_MASK         0xFFFFFFFF
#define IMX_USDHC_CMD_RSP0_CMDRSP0(v)           (((v) << 0) & 0xFFFFFFFF)



/* Register CMD_RSP1 - Command Response1 */
#define IMX_USDHC_CMD_RSP1                      0x14
/* Field CMDRSP1 - Command Response 1 */
#define IMX_USDHC_CMD_RSP1_CMDRSP1_SHIFT        0
#define IMX_USDHC_CMD_RSP1_CMDRSP1_MASK         0xFFFFFFFF
#define IMX_USDHC_CMD_RSP1_CMDRSP1(v)           (((v) << 0) & 0xFFFFFFFF)



/* Register CMD_RSP2 - Command Response2 */
#define IMX_USDHC_CMD_RSP2                      0x18
/* Field CMDRSP2 - Command Response 2 */
#define IMX_USDHC_CMD_RSP2_CMDRSP2_SHIFT        0
#define IMX_USDHC_CMD_RSP2_CMDRSP2_MASK         0xFFFFFFFF
#define IMX_USDHC_CMD_RSP2_CMDRSP2(v)           (((v) << 0) & 0xFFFFFFFF)



/* Register CMD_RSP3 - Command Response3 */
#define IMX_USDHC_CMD_RSP3                      0x1C
/* Field CMDRSP3 - Command Response 3 */
#define IMX_USDHC_CMD_RSP3_CMDRSP3_SHIFT        0
#define IMX_USDHC_CMD_RSP3_CMDRSP3_MASK         0xFFFFFFFF
#define IMX_USDHC_CMD_RSP3_CMDRSP3(v)           (((v) << 0) & 0xFFFFFFFF)



/* Register DATA_BUFF_ACC_PORT - Data Buffer Access Port */
#define IMX_USDHC_DATA_BUFF_ACC_PORT            0x20
/* Field DATCONT - Data Content */
#define IMX_USDHC_DATA_BUFF_ACC_PORT_DATCONT_SHIFT 0
#define IMX_USDHC_DATA_BUFF_ACC_PORT_DATCONT_MASK  0xFFFFFFFF
#define IMX_USDHC_DATA_BUFF_ACC_PORT_DATCONT(v)    (((v) << 0) & 0xFFFFFFFF)



/* Register PRES_STATE - Present State */
#define IMX_USDHC_PRES_STATE                    0x24
/* Field CIHB - Command Inhibit (CMD) */
#define IMX_USDHC_PRES_STATE_CIHB_SHIFT         0
#define IMX_USDHC_PRES_STATE_CIHB_MASK          0x1
#define IMX_USDHC_PRES_STATE_CIHB(v)            (((v) << 0) & 0x1)
/* Field CDIHB - Command Inhibit (DATA) */
#define IMX_USDHC_PRES_STATE_CDIHB_SHIFT        1
#define IMX_USDHC_PRES_STATE_CDIHB_MASK         0x2
#define IMX_USDHC_PRES_STATE_CDIHB(v)           (((v) << 0x1) & 0x2)
/* Field DLA - Data Line Active */
#define IMX_USDHC_PRES_STATE_DLA_SHIFT          2
#define IMX_USDHC_PRES_STATE_DLA_MASK           0x4
#define IMX_USDHC_PRES_STATE_DLA(v)             (((v) << 0x2) & 0x4)
/* Field SDSTB - SD Clock Stable */
#define IMX_USDHC_PRES_STATE_SDSTB_SHIFT        3
#define IMX_USDHC_PRES_STATE_SDSTB_MASK         0x8
#define IMX_USDHC_PRES_STATE_SDSTB(v)           (((v) << 0x3) & 0x8)
/* Field IPGOFF - IPG_CLK Gated Off Internally */
#define IMX_USDHC_PRES_STATE_IPGOFF_SHIFT       4
#define IMX_USDHC_PRES_STATE_IPGOFF_MASK        0x10
#define IMX_USDHC_PRES_STATE_IPGOFF(v)          (((v) << 0x4) & 0x10)
/* Field HCKOFF - HCLK Gated Off Internally */
#define IMX_USDHC_PRES_STATE_HCKOFF_SHIFT       5
#define IMX_USDHC_PRES_STATE_HCKOFF_MASK        0x20
#define IMX_USDHC_PRES_STATE_HCKOFF(v)          (((v) << 0x5) & 0x20)
/* Field PEROFF - IPG_PERCLK Gated Off Internally */
#define IMX_USDHC_PRES_STATE_PEROFF_SHIFT       6
#define IMX_USDHC_PRES_STATE_PEROFF_MASK        0x40
#define IMX_USDHC_PRES_STATE_PEROFF(v)          (((v) << 0x6) & 0x40)
/* Field SDOFF - SD Clock Gated Off Internally */
#define IMX_USDHC_PRES_STATE_SDOFF_SHIFT        7
#define IMX_USDHC_PRES_STATE_SDOFF_MASK         0x80
#define IMX_USDHC_PRES_STATE_SDOFF(v)           (((v) << 0x7) & 0x80)
/* Field WTA - Write Transfer Active */
#define IMX_USDHC_PRES_STATE_WTA_SHIFT          8
#define IMX_USDHC_PRES_STATE_WTA_MASK           0x100
#define IMX_USDHC_PRES_STATE_WTA(v)             (((v) << 0x8) & 0x100)
/* Field RTA - Read Transfer Active */
#define IMX_USDHC_PRES_STATE_RTA_SHIFT          9
#define IMX_USDHC_PRES_STATE_RTA_MASK           0x200
#define IMX_USDHC_PRES_STATE_RTA(v)             (((v) << 0x9) & 0x200)
/* Field BWEN - Buffer Write Enable */
#define IMX_USDHC_PRES_STATE_BWEN_SHIFT         10
#define IMX_USDHC_PRES_STATE_BWEN_MASK          0x400
#define IMX_USDHC_PRES_STATE_BWEN(v)            (((v) << 0xA) & 0x400)
/* Field BREN - Buffer Read Enable */
#define IMX_USDHC_PRES_STATE_BREN_SHIFT         11
#define IMX_USDHC_PRES_STATE_BREN_MASK          0x800
#define IMX_USDHC_PRES_STATE_BREN(v)            (((v) << 0xB) & 0x800)
/* Field RTR - Re-Tuning Request (only for SD3.0 SDR104 mode) */
#define IMX_USDHC_PRES_STATE_RTR_SHIFT          12
#define IMX_USDHC_PRES_STATE_RTR_MASK           0x1000
#define IMX_USDHC_PRES_STATE_RTR(v)             (((v) << 12) & 0x1000)
/* Field TSCD - Tape Select Change Done */
#define IMX_USDHC_PRES_STATE_TSCD_SHIFT         15
#define IMX_USDHC_PRES_STATE_TSCD_MASK          0x8000
#define IMX_USDHC_PRES_STATE_TSCD(v)            (((v) << 15) & 0x8000)
/* Field CINST - Card Inserted */
#define IMX_USDHC_PRES_STATE_CINST_SHIFT        16
#define IMX_USDHC_PRES_STATE_CINST_MASK         0x10000
#define IMX_USDHC_PRES_STATE_CINST(v)           (((v) << 16) & 0x10000)
/* Field CDPL - Card Detect Pin Level */
#define IMX_USDHC_PRES_STATE_CDPL_SHIFT         18
#define IMX_USDHC_PRES_STATE_CDPL_MASK          0x40000
#define IMX_USDHC_PRES_STATE_CDPL(v)            (((v) << 0x12) & 0x40000)
/* Field WPSPL - Write Protect Switch Pin Level */
#define IMX_USDHC_PRES_STATE_WPSPL_SHIFT        19
#define IMX_USDHC_PRES_STATE_WPSPL_MASK         0x80000
#define IMX_USDHC_PRES_STATE_WPSPL(v)           (((v) << 0x13) & 0x80000)
/* Field CLSL - CMD Line Signal Level */
#define IMX_USDHC_PRES_STATE_CLSL_SHIFT         23
#define IMX_USDHC_PRES_STATE_CLSL_MASK          0x800000
#define IMX_USDHC_PRES_STATE_CLSL(v)            (((v) << 0x17) & 0x800000)
/* Field DLSL - DATA[7:0] Line Signal Level */
#define IMX_USDHC_PRES_STATE_DLSL_SHIFT         24
#define IMX_USDHC_PRES_STATE_DLSL_MASK          0xFF000000
#define IMX_USDHC_PRES_STATE_DLSL(v)            (((v) << 0x18) & 0xFF000000)
#define IMX_USDHC_PRES_STATE_DLSL7_MASK         (1 << 31)
#define IMX_USDHC_PRES_STATE_DLSL6_MASK         (1 << 30)
#define IMX_USDHC_PRES_STATE_DLSL5_MASK         (1 << 29)
#define IMX_USDHC_PRES_STATE_DLSL4_MASK         (1 << 28)
#define IMX_USDHC_PRES_STATE_DLSL3_MASK         (1 << 27)
#define IMX_USDHC_PRES_STATE_DLSL2_MASK         (1 << 26)
#define IMX_USDHC_PRES_STATE_DLSL1_MASK         (1 << 25)
#define IMX_USDHC_PRES_STATE_DLSL0_MASK         (1 << 24)
/* Additional user defines */
#define IMX_USDHC_PRES_STATE_CARD_STABLE (IMX_USDHC_PRES_STATE_CDPL_MASK | \
                                          IMX_USDHC_PRES_STATE_SDSTB_MASK | \
                                          IMX_USDHC_PRES_STATE_CINST_MASK)



/* Register PROT_CTRL - Protocol Control */
#define IMX_USDHC_PROT_CTRL                     0x28
/* Field LCTL - LED Control */
#define IMX_USDHC_PROT_CTRL_LCTL_SHIFT          0
#define IMX_USDHC_PROT_CTRL_LCTL_MASK           0x1
#define IMX_USDHC_PROT_CTRL_LCTL(v)             (((v) << 0) & 0x1)
/* Field DTW - Data Transfer Width */
#define IMX_USDHC_PROT_CTRL_DTW_SHIFT           1
#define IMX_USDHC_PROT_CTRL_DTW_MASK            0x6
#define IMX_USDHC_PROT_CTRL_DTW(v)              (((v) << 0x1) & 0x6)
/* Data bus width bit values */
/* Data Bus Width 1 bit */
#define IMX_USDHC_PROT_CTRL_DTW_BV_1            0
/* Data Bus Width 4 bit */
#define IMX_USDHC_PROT_CTRL_DTW_BV_4            1
/* Data Bus Width 8 bit */
#define IMX_USDHC_PROT_CTRL_DTW_BV_8            2
/* Field D3CD - DATA3 as Card Detection Pin */
#define IMX_USDHC_PROT_CTRL_D3CD_SHIFT          3
#define IMX_USDHC_PROT_CTRL_D3CD_MASK           0x8
#define IMX_USDHC_PROT_CTRL_D3CD(v)             (((v) << 0x3) & 0x8)
/* Field EMODE - Endian Mode */
#define IMX_USDHC_PROT_CTRL_EMODE_SHIFT         4
#define IMX_USDHC_PROT_CTRL_EMODE_MASK          0x30
#define IMX_USDHC_PROT_CTRL_EMODE(v)            (((v) << 0x4) & 0x30)
/* Field CDTL - Card Detect Test Level */
#define IMX_USDHC_PROT_CTRL_CDTL_SHIFT          6
#define IMX_USDHC_PROT_CTRL_CDTL_MASK           0x40
#define IMX_USDHC_PROT_CTRL_CDTL(v)             (((v) << 0x6) & 0x40)
/* Field CDSS - Card Detect Signal Selection */
#define IMX_USDHC_PROT_CTRL_CDSS_SHIFT          7
#define IMX_USDHC_PROT_CTRL_CDSS_MASK           0x80
#define IMX_USDHC_PROT_CTRL_CDSS(v)             (((v) << 0x7) & 0x80)
/* Field DMASEL - DMA Select */
#define IMX_USDHC_PROT_CTRL_DMASEL_SHIFT        8
#define IMX_USDHC_PROT_CTRL_DMASEL_MASK         0x300
#define IMX_USDHC_PROT_CTRL_DMASEL(v)           (((v) << 0x8) & 0x300)
/* DMA selection bit values */
#define IMX_USDHC_PROT_CTRL_DMASEL_BV_SDMA      0
#define IMX_USDHC_PROT_CTRL_DMASEL_BV_ADMA1     1
#define IMX_USDHC_PROT_CTRL_DMASEL_BV_ADMA2     2
/* Field SABGREQ - Stop At Block Gap Request */
#define IMX_USDHC_PROT_CTRL_SABGREQ_SHIFT       16
#define IMX_USDHC_PROT_CTRL_SABGREQ_MASK        0x10000
#define IMX_USDHC_PROT_CTRL_SABGREQ(v)          (((v) << 0x10) & 0x10000)
/* Field CREQ - Continue Request */
#define IMX_USDHC_PROT_CTRL_CREQ_SHIFT          17
#define IMX_USDHC_PROT_CTRL_CREQ_MASK           0x20000
#define IMX_USDHC_PROT_CTRL_CREQ(v)             (((v) << 0x11) & 0x20000)
/* Field RWCTL - Read Wait Control */
#define IMX_USDHC_PROT_CTRL_RWCTL_SHIFT         18
#define IMX_USDHC_PROT_CTRL_RWCTL_MASK          0x40000
#define IMX_USDHC_PROT_CTRL_RWCTL(v)            (((v) << 0x12) & 0x40000)
/* Field IABG - Interrupt At Block Gap */
#define IMX_USDHC_PROT_CTRL_IABG_SHIFT          19
#define IMX_USDHC_PROT_CTRL_IABG_MASK           0x80000
#define IMX_USDHC_PROT_CTRL_IABG(v)             (((v) << 0x13) & 0x80000)
/* Field RD_DONE_NO_8CLK -  */
#define IMX_USDHC_PROT_CTRL_RD_DONE_NO_8CLK_SHIFT 20
#define IMX_USDHC_PROT_CTRL_RD_DONE_NO_8CLK_MASK  0x100000
#define IMX_USDHC_PROT_CTRL_RD_DONE_NO_8CLK(v)    (((v) << 0x14) & 0x100000)
/* Field WECINT - Wakeup Event Enable On Card Interrupt */
#define IMX_USDHC_PROT_CTRL_WECINT_SHIFT        24
#define IMX_USDHC_PROT_CTRL_WECINT_MASK         0x1000000
#define IMX_USDHC_PROT_CTRL_WECINT(v)           (((v) << 0x18) & 0x1000000)
/* Field WECINS - Wakeup Event Enable On SD Card Insertion */
#define IMX_USDHC_PROT_CTRL_WECINS_SHIFT        25
#define IMX_USDHC_PROT_CTRL_WECINS_MASK         0x2000000
#define IMX_USDHC_PROT_CTRL_WECINS(v)           (((v) << 0x19) & 0x2000000)
/* Field WECRM - Wakeup Event Enable On SD Card Removal */
#define IMX_USDHC_PROT_CTRL_WECRM_SHIFT         26
#define IMX_USDHC_PROT_CTRL_WECRM_MASK          0x4000000
#define IMX_USDHC_PROT_CTRL_WECRM(v)            (((v) << 0x1A) & 0x4000000)
/* Field BURST_LEN_EN - BURST length enable for:
 * INCR, INCR4 / INCR8 / INCR16, INCR4-WRAP / INCR8-WRAP / INCR16-WRAP */
#define IMX_USDHC_PROT_CTRL_BURST_LEN_EN_SHIFT  27
#define IMX_USDHC_PROT_CTRL_BURST_LEN_EN_MASK   0x38000000
#define IMX_USDHC_PROT_CTRL_BURST_LEN_EN(v)     (((v) << 0x1B) & 0x38000000)
/* Field NON_EXACT_BLK_RD - Current block read is non-exact block read -SDIO */
#define IMX_USDHC_PROT_CTRL_NON_EXACT_BLK_RD_SHIFT 30
#define IMX_USDHC_PROT_CTRL_NON_EXACT_BLK_RD_MASK  0x40000000
#define IMX_USDHC_PROT_CTRL_NON_EXACT_BLK_RD(v)    (((v) << 0x1E) & 0x40000000)



/* Register SYS_CTRL - System Control */
#define IMX_USDHC_SYS_CTRL                      0x2C
/* Field DVS - Divisor */
#define IMX_USDHC_SYS_CTRL_DVS_SHIFT            4
#define IMX_USDHC_SYS_CTRL_DVS_MASK             0xF0
#define IMX_USDHC_SYS_CTRL_DVS(v)               (((v) << 0x4) & 0xF0)
/* Field SDCLKFS - SDCLK Frequency Select */
#define IMX_USDHC_SYS_CTRL_SDCLKFS_SHIFT        8
#define IMX_USDHC_SYS_CTRL_SDCLKFS_MASK         0xFF00
#define IMX_USDHC_SYS_CTRL_SDCLKFS(v)           (((v) << 0x8) & 0xFF00)
/* Field DTOCV - Data Timeout Counter Value */
#define IMX_USDHC_SYS_CTRL_DTOCV_SHIFT          16
#define IMX_USDHC_SYS_CTRL_DTOCV_MASK           0xF0000
#define IMX_USDHC_SYS_CTRL_DTOCV(v)             (((v) << 0x10) & 0xF0000)
/* Field IPP_RST_N - Direct reset pad control. */
#define IMX_USDHC_SYS_CTRL_IPP_RST_N_SHIFT      23
#define IMX_USDHC_SYS_CTRL_IPP_RST_N_MASK       0x800000
#define IMX_USDHC_SYS_CTRL_IPP_RST_N(v)         (((v) << 0x17) & 0x800000)
/* Field RSTA - Software Reset For ALL */
#define IMX_USDHC_SYS_CTRL_RSTA_SHIFT           24
#define IMX_USDHC_SYS_CTRL_RSTA_MASK            0x1000000
#define IMX_USDHC_SYS_CTRL_RSTA(v)              (((v) << 0x18) & 0x1000000)
/* Field RSTC - Software Reset For CMD Line */
#define IMX_USDHC_SYS_CTRL_RSTC_SHIFT           25
#define IMX_USDHC_SYS_CTRL_RSTC_MASK            0x2000000
#define IMX_USDHC_SYS_CTRL_RSTC(v)              (((v) << 0x19) & 0x2000000)
/* Field RSTD - Software Reset For DATA Line */
#define IMX_USDHC_SYS_CTRL_RSTD_SHIFT           26
#define IMX_USDHC_SYS_CTRL_RSTD_MASK            0x4000000
#define IMX_USDHC_SYS_CTRL_RSTD(v)              (((v) << 0x1A) & 0x4000000)
/* Field INITA - Initialization Active */
#define IMX_USDHC_SYS_CTRL_INITA_SHIFT          27
#define IMX_USDHC_SYS_CTRL_INITA_MASK           0x8000000
#define IMX_USDHC_SYS_CTRL_INITA(v)             (((v) << 0x1B) & 0x8000000)
/* Field RSTT - Reset Tuning */
#define IMX_USDHC_SYS_CTRL_RSTT_SHIFT           28
#define IMX_USDHC_SYS_CTRL_RSTT_MASK            0x10000000
#define IMX_USDHC_SYS_CTRL_RSTT(v)              (((v) << 0x1C) & 0x10000000)



/* Register INT_STATUS - Interrupt Status */
#define IMX_USDHC_INT_STATUS                    0x30
/* Field CC - Command Complete */
#define IMX_USDHC_INT_STATUS_CC_SHIFT           0
#define IMX_USDHC_INT_STATUS_CC_MASK            0x1
#define IMX_USDHC_INT_STATUS_CC(v)              (((v) << 0) & 0x1)
/* Field TC - Transfer Complete */
#define IMX_USDHC_INT_STATUS_TC_SHIFT           1
#define IMX_USDHC_INT_STATUS_TC_MASK            0x2
#define IMX_USDHC_INT_STATUS_TC(v)              (((v) << 0x1) & 0x2)
/* Field BGE - Block Gap Event */
#define IMX_USDHC_INT_STATUS_BGE_SHIFT          2
#define IMX_USDHC_INT_STATUS_BGE_MASK           0x4
#define IMX_USDHC_INT_STATUS_BGE(v)             (((v) << 0x2) & 0x4)
/* Field DINT - DMA Interrupt */
#define IMX_USDHC_INT_STATUS_DINT_SHIFT         3
#define IMX_USDHC_INT_STATUS_DINT_MASK          0x8
#define IMX_USDHC_INT_STATUS_DINT(v)            (((v) << 0x3) & 0x8)
/* Field BWR - Buffer Write Ready */
#define IMX_USDHC_INT_STATUS_BWR_SHIFT          4
#define IMX_USDHC_INT_STATUS_BWR_MASK           0x10
#define IMX_USDHC_INT_STATUS_BWR(v)             (((v) << 0x4) & 0x10)
/* Field BRR - Buffer Read Ready */
#define IMX_USDHC_INT_STATUS_BRR_SHIFT          5
#define IMX_USDHC_INT_STATUS_BRR_MASK           0x20
#define IMX_USDHC_INT_STATUS_BRR(v)             (((v) << 0x5) & 0x20)
/* Field CINS - Card Insertion */
#define IMX_USDHC_INT_STATUS_CINS_SHIFT         6
#define IMX_USDHC_INT_STATUS_CINS_MASK          0x40
#define IMX_USDHC_INT_STATUS_CINS(v)            (((v) << 0x6) & 0x40)
/* Field CRM - Card Removal */
#define IMX_USDHC_INT_STATUS_CRM_SHIFT          7
#define IMX_USDHC_INT_STATUS_CRM_MASK           0x80
#define IMX_USDHC_INT_STATUS_CRM(v)             (((v) << 0x7) & 0x80)
/* Field CINT - Card Interrupt */
#define IMX_USDHC_INT_STATUS_CINT_SHIFT         8
#define IMX_USDHC_INT_STATUS_CINT_MASK          0x100
#define IMX_USDHC_INT_STATUS_CINT(v)            (((v) << 0x8) & 0x100)
/* Field RTE - Re-Tuning Event: (only for SD3.0 SDR104 mode) */
#define IMX_USDHC_INT_STATUS_RTE_SHIFT          12
#define IMX_USDHC_INT_STATUS_RTE_MASK           0x1000
#define IMX_USDHC_INT_STATUS_RTE(v)             (((v) << 0xC) & 0x1000)
/* Field TP - Tuning Pass:(only for SD3.0 SDR104 mode) */
#define IMX_USDHC_INT_STATUS_TP_SHIFT           14
#define IMX_USDHC_INT_STATUS_TP_MASK            0x4000
#define IMX_USDHC_INT_STATUS_TP(v)              (((v) << 0xE) & 0x4000)
/* Field CTOE - Command Timeout Error */
#define IMX_USDHC_INT_STATUS_CTOE_SHIFT         16
#define IMX_USDHC_INT_STATUS_CTOE_MASK          0x10000
#define IMX_USDHC_INT_STATUS_CTOE(v)            (((v) << 0x10) & 0x10000)
/* Field CCE - Command CRC Error */
#define IMX_USDHC_INT_STATUS_CCE_SHIFT          17
#define IMX_USDHC_INT_STATUS_CCE_MASK           0x20000
#define IMX_USDHC_INT_STATUS_CCE(v)             (((v) << 0x11) & 0x20000)
/* Field CEBE - Command End Bit Error */
#define IMX_USDHC_INT_STATUS_CEBE_SHIFT         18
#define IMX_USDHC_INT_STATUS_CEBE_MASK          0x40000
#define IMX_USDHC_INT_STATUS_CEBE(v)            (((v) << 0x12) & 0x40000)
/* Field CIE - Command Index Error */
#define IMX_USDHC_INT_STATUS_CIE_SHIFT          19
#define IMX_USDHC_INT_STATUS_CIE_MASK           0x80000
#define IMX_USDHC_INT_STATUS_CIE(v)             (((v) << 0x13) & 0x80000)
/* Field DTOE - Data Timeout Error */
#define IMX_USDHC_INT_STATUS_DTOE_SHIFT         20
#define IMX_USDHC_INT_STATUS_DTOE_MASK          0x100000
#define IMX_USDHC_INT_STATUS_DTOE(v)            (((v) << 0x14) & 0x100000)
/* Field DCE - Data CRC Error */
#define IMX_USDHC_INT_STATUS_DCE_SHIFT          21
#define IMX_USDHC_INT_STATUS_DCE_MASK           0x200000
#define IMX_USDHC_INT_STATUS_DCE(v)             (((v) << 0x15) & 0x200000)
/* Field DEBE - Data End Bit Error */
#define IMX_USDHC_INT_STATUS_DEBE_SHIFT         22
#define IMX_USDHC_INT_STATUS_DEBE_MASK          0x400000
#define IMX_USDHC_INT_STATUS_DEBE(v)            (((v) << 0x16) & 0x400000)
/* Field AC12E - Auto CMD12 Error */
#define IMX_USDHC_INT_STATUS_AC12E_SHIFT        24
#define IMX_USDHC_INT_STATUS_AC12E_MASK         0x1000000
#define IMX_USDHC_INT_STATUS_AC12E(v)           (((v) << 0x18) & 0x1000000)
/* Field TNE - Tuning Error: (only for SD3.0 SDR104 mode) */
#define IMX_USDHC_INT_STATUS_TNE_SHIFT          26
#define IMX_USDHC_INT_STATUS_TNE_MASK           0x4000000
#define IMX_USDHC_INT_STATUS_TNE(v)             (((v) << 0x1A) & 0x4000000)
/* Field DMAE - DMA Error */
#define IMX_USDHC_INT_STATUS_DMAE_SHIFT         28
#define IMX_USDHC_INT_STATUS_DMAE_MASK          0x10000000
#define IMX_USDHC_INT_STATUS_DMAE(v)            (((v) << 0x1C) & 0x10000000)
/* Additional user defines */
#define IMX_USDHC_INT_STATUS_INTR_CLR_ALL 0xFFFFFFFF
/* ERRI */
#define IMX_USDHC_INT_STATUS_ERRI         (IMX_USDHC_INT_STATUS_DMAE_MASK | \
                                           IMX_USDHC_INT_STATUS_CCE_MASK | \
                                           IMX_USDHC_INT_STATUS_CTOE_MASK | \
                                           IMX_USDHC_INT_STATUS_CEBE_MASK | \
                                           IMX_USDHC_INT_STATUS_CIE_MASK | \
                                           IMX_USDHC_INT_STATUS_DTOE_MASK | \
                                           IMX_USDHC_INT_STATUS_DCE_MASK | \
                                           IMX_USDHC_INT_STATUS_DEBE_MASK | \
                                           IMX_USDHC_INT_STATUS_AC12E_MASK)



/* Register INT_STATUS_EN - Interrupt Status Enable */
#define IMX_USDHC_INT_STATUS_EN                 0x34
/* Field CCSEN - Command Complete Status Enable */
#define IMX_USDHC_INT_STATUS_EN_CCSEN_SHIFT     0
#define IMX_USDHC_INT_STATUS_EN_CCSEN_MASK      0x1
#define IMX_USDHC_INT_STATUS_EN_CCSEN(v)        (((v) << 0) & 0x1)
/* Field TCSEN - Transfer Complete Status Enable */
#define IMX_USDHC_INT_STATUS_EN_TCSEN_SHIFT     1
#define IMX_USDHC_INT_STATUS_EN_TCSEN_MASK      0x2
#define IMX_USDHC_INT_STATUS_EN_TCSEN(v)        (((v) << 0x1) & 0x2)
/* Field BGESEN - Block Gap Event Status Enable */
#define IMX_USDHC_INT_STATUS_EN_BGESEN_SHIFT    2
#define IMX_USDHC_INT_STATUS_EN_BGESEN_MASK     0x4
#define IMX_USDHC_INT_STATUS_EN_BGESEN(v)       (((v) << 0x2) & 0x4)
/* Field DINTSEN - DMA Interrupt Status Enable */
#define IMX_USDHC_INT_STATUS_EN_DINTSEN_SHIFT   3
#define IMX_USDHC_INT_STATUS_EN_DINTSEN_MASK    0x8
#define IMX_USDHC_INT_STATUS_EN_DINTSEN(v)      (((v) << 0x3) & 0x8)
/* Field BWRSEN - Buffer Write Ready Status Enable */
#define IMX_USDHC_INT_STATUS_EN_BWRSEN_SHIFT    4
#define IMX_USDHC_INT_STATUS_EN_BWRSEN_MASK     0x10
#define IMX_USDHC_INT_STATUS_EN_BWRSEN(v)       (((v) << 0x4) & 0x10)
/* Field BRRSEN - Buffer Read Ready Status Enable */
#define IMX_USDHC_INT_STATUS_EN_BRRSEN_SHIFT    5
#define IMX_USDHC_INT_STATUS_EN_BRRSEN_MASK     0x20
#define IMX_USDHC_INT_STATUS_EN_BRRSEN(v)       (((v) << 0x5) & 0x20)
/* Field CINSSEN - Card Insertion Status Enable */
#define IMX_USDHC_INT_STATUS_EN_CINSSEN_SHIFT   6
#define IMX_USDHC_INT_STATUS_EN_CINSSEN_MASK    0x40
#define IMX_USDHC_INT_STATUS_EN_CINSSEN(v)      (((v) << 0x6) & 0x40)
/* Field CRMSEN - Card Removal Status Enable */
#define IMX_USDHC_INT_STATUS_EN_CRMSEN_SHIFT    7
#define IMX_USDHC_INT_STATUS_EN_CRMSEN_MASK     0x80
#define IMX_USDHC_INT_STATUS_EN_CRMSEN(v)       (((v) << 0x7) & 0x80)
/* Field CINTSEN - Card Interrupt Status Enable */
#define IMX_USDHC_INT_STATUS_EN_CINTSEN_SHIFT   8
#define IMX_USDHC_INT_STATUS_EN_CINTSEN_MASK    0x100
#define IMX_USDHC_INT_STATUS_EN_CINTSEN(v)      (((v) << 0x8) & 0x100)
/* Field RTESEN - Re-Tuning Event Status Enable */
#define IMX_USDHC_INT_STATUS_EN_RTESEN_SHIFT    12
#define IMX_USDHC_INT_STATUS_EN_RTESEN_MASK     0x1000
#define IMX_USDHC_INT_STATUS_EN_RTESEN(v)       (((v) << 0xC) & 0x1000)
/* Field TPSEN - Tuning Pass Status Enable */
#define IMX_USDHC_INT_STATUS_EN_TPSEN_SHIFT     14
#define IMX_USDHC_INT_STATUS_EN_TPSEN_MASK      0x4000
#define IMX_USDHC_INT_STATUS_EN_TPSEN(v)        (((v) << 0xE) & 0x4000)
/* Field CTOESEN - Command Timeout Error Status Enable */
#define IMX_USDHC_INT_STATUS_EN_CTOESEN_SHIFT   16
#define IMX_USDHC_INT_STATUS_EN_CTOESEN_MASK    0x10000
#define IMX_USDHC_INT_STATUS_EN_CTOESEN(v)      (((v) << 0x10) & 0x10000)
/* Field CCESEN - Command CRC Error Status Enable */
#define IMX_USDHC_INT_STATUS_EN_CCESEN_SHIFT    17
#define IMX_USDHC_INT_STATUS_EN_CCESEN_MASK     0x20000
#define IMX_USDHC_INT_STATUS_EN_CCESEN(v)       (((v) << 0x11) & 0x20000)
/* Field CEBESEN - Command End Bit Error Status Enable */
#define IMX_USDHC_INT_STATUS_EN_CEBESEN_SHIFT   18
#define IMX_USDHC_INT_STATUS_EN_CEBESEN_MASK    0x40000
#define IMX_USDHC_INT_STATUS_EN_CEBESEN(v)      (((v) << 0x12) & 0x40000)
/* Field CIESEN - Command Index Error Status Enable */
#define IMX_USDHC_INT_STATUS_EN_CIESEN_SHIFT    19
#define IMX_USDHC_INT_STATUS_EN_CIESEN_MASK     0x80000
#define IMX_USDHC_INT_STATUS_EN_CIESEN(v)       (((v) << 0x13) & 0x80000)
/* Field DTOESEN - Data Timeout Error Status Enable */
#define IMX_USDHC_INT_STATUS_EN_DTOESEN_SHIFT   20
#define IMX_USDHC_INT_STATUS_EN_DTOESEN_MASK    0x100000
#define IMX_USDHC_INT_STATUS_EN_DTOESEN(v)      (((v) << 0x14) & 0x100000)
/* Field DCESEN - Data CRC Error Status Enable */
#define IMX_USDHC_INT_STATUS_EN_DCESEN_SHIFT    21
#define IMX_USDHC_INT_STATUS_EN_DCESEN_MASK     0x200000
#define IMX_USDHC_INT_STATUS_EN_DCESEN(v)       (((v) << 0x15) & 0x200000)
/* Field DEBESEN - Data End Bit Error Status Enable */
#define IMX_USDHC_INT_STATUS_EN_DEBESEN_SHIFT   22
#define IMX_USDHC_INT_STATUS_EN_DEBESEN_MASK    0x400000
#define IMX_USDHC_INT_STATUS_EN_DEBESEN(v)      (((v) << 0x16) & 0x400000)
/* Field AC12ESEN - Auto CMD12 Error Status Enable */
#define IMX_USDHC_INT_STATUS_EN_AC12ESEN_SHIFT  24
#define IMX_USDHC_INT_STATUS_EN_AC12ESEN_MASK   0x1000000
#define IMX_USDHC_INT_STATUS_EN_AC12ESEN(v)     (((v) << 0x18) & 0x1000000)
/* Field TNESEN - Tuning Error Status Enable */
#define IMX_USDHC_INT_STATUS_EN_TNESEN_SHIFT    26
#define IMX_USDHC_INT_STATUS_EN_TNESEN_MASK     0x4000000
#define IMX_USDHC_INT_STATUS_EN_TNESEN(v)       (((v) << 0x1A) & 0x4000000)
/* Field DMAESEN - DMA Error Status Enable */
#define IMX_USDHC_INT_STATUS_EN_DMAESEN_SHIFT   28
#define IMX_USDHC_INT_STATUS_EN_DMAESEN_MASK    0x10000000
#define IMX_USDHC_INT_STATUS_EN_DMAESEN(v)      (((v) << 0x1C) & 0x10000000)
/* Additional user defines */
#define IMX_USDHC_INT_STATUS_EN_DFLTS (IMX_USDHC_INT_STATUS_EN_CTOESEN_MASK | \
                                       IMX_USDHC_INT_STATUS_EN_CCESEN_MASK | \
                                       IMX_USDHC_INT_STATUS_EN_CEBESEN_MASK | \
                                       IMX_USDHC_INT_STATUS_EN_CIESEN_MASK | \
                                       IMX_USDHC_INT_STATUS_EN_AC12ESEN_MASK | \
                                       IMX_USDHC_INT_STATUS_EN_CINSSEN_MASK | \
                                       IMX_USDHC_INT_STATUS_EN_CRMSEN_MASK)



/* Register INT_SIGNAL_EN - Interrupt Signal Enable */
#define IMX_USDHC_INT_SIGNAL_EN                 0x38
/* Field CCIEN - Command Complete Interrupt Enable */
#define IMX_USDHC_INT_SIGNAL_EN_CCIEN_SHIFT     0
#define IMX_USDHC_INT_SIGNAL_EN_CCIEN_MASK      0x1
#define IMX_USDHC_INT_SIGNAL_EN_CCIEN(v)        (((v) << 0) & 0x1)
/* Field TCIEN - Transfer Complete Interrupt Enable */
#define IMX_USDHC_INT_SIGNAL_EN_TCIEN_SHIFT     1
#define IMX_USDHC_INT_SIGNAL_EN_TCIEN_MASK      0x2
#define IMX_USDHC_INT_SIGNAL_EN_TCIEN(v)        (((v) << 0x1) & 0x2)
/* Field BGEIEN - Block Gap Event Interrupt Enable */
#define IMX_USDHC_INT_SIGNAL_EN_BGEIEN_SHIFT    2
#define IMX_USDHC_INT_SIGNAL_EN_BGEIEN_MASK     0x4
#define IMX_USDHC_INT_SIGNAL_EN_BGEIEN(v)       (((v) << 0x2) & 0x4)
/* Field DINTIEN - DMA Interrupt Enable */
#define IMX_USDHC_INT_SIGNAL_EN_DINTIEN_SHIFT   3
#define IMX_USDHC_INT_SIGNAL_EN_DINTIEN_MASK    0x8
#define IMX_USDHC_INT_SIGNAL_EN_DINTIEN(v)      (((v) << 0x3) & 0x8)
/* Field BWRIEN - Buffer Write Ready Interrupt Enable */
#define IMX_USDHC_INT_SIGNAL_EN_BWRIEN_SHIFT    4
#define IMX_USDHC_INT_SIGNAL_EN_BWRIEN_MASK     0x10
#define IMX_USDHC_INT_SIGNAL_EN_BWRIEN(v)       (((v) << 0x4) & 0x10)
/* Field BRRIEN - Buffer Read Ready Interrupt Enable */
#define IMX_USDHC_INT_SIGNAL_EN_BRRIEN_SHIFT    5
#define IMX_USDHC_INT_SIGNAL_EN_BRRIEN_MASK     0x20
#define IMX_USDHC_INT_SIGNAL_EN_BRRIEN(v)       (((v) << 0x5) & 0x20)
/* Field CINSIEN - Card Insertion Interrupt Enable */
#define IMX_USDHC_INT_SIGNAL_EN_CINSIEN_SHIFT   6
#define IMX_USDHC_INT_SIGNAL_EN_CINSIEN_MASK    0x40
#define IMX_USDHC_INT_SIGNAL_EN_CINSIEN(v)      (((v) << 0x6) & 0x40)
/* Field CRMIEN - Card Removal Interrupt Enable */
#define IMX_USDHC_INT_SIGNAL_EN_CRMIEN_SHIFT    7
#define IMX_USDHC_INT_SIGNAL_EN_CRMIEN_MASK     0x80
#define IMX_USDHC_INT_SIGNAL_EN_CRMIEN(v)       (((v) << 0x7) & 0x80)
/* Field CINTIEN - Card Interrupt Interrupt Enable */
#define IMX_USDHC_INT_SIGNAL_EN_CINTIEN_SHIFT   8
#define IMX_USDHC_INT_SIGNAL_EN_CINTIEN_MASK    0x100
#define IMX_USDHC_INT_SIGNAL_EN_CINTIEN(v)      (((v) << 0x8) & 0x100)
/* Field RTEIEN - Re-Tuning Event Interrupt Enable */
#define IMX_USDHC_INT_SIGNAL_EN_RTEIEN_SHIFT    12
#define IMX_USDHC_INT_SIGNAL_EN_RTEIEN_MASK     0x1000
#define IMX_USDHC_INT_SIGNAL_EN_RTEIEN(v)       (((v) << 0xC) & 0x1000)
/* Field TPIEN - Tuning Pass Interrupt Enable */
#define IMX_USDHC_INT_SIGNAL_EN_TPIEN_SHIFT     14
#define IMX_USDHC_INT_SIGNAL_EN_TPIEN_MASK      0x4000
#define IMX_USDHC_INT_SIGNAL_EN_TPIEN(v)        (((v) << 0xE) & 0x4000)
/* Field CTOEIEN - Command Timeout Error Interrupt Enable */
#define IMX_USDHC_INT_SIGNAL_EN_CTOEIEN_SHIFT   16
#define IMX_USDHC_INT_SIGNAL_EN_CTOEIEN_MASK    0x10000
#define IMX_USDHC_INT_SIGNAL_EN_CTOEIEN(v)      (((v) << 0x10) & 0x10000)
/* Field CCEIEN - Command CRC Error Interrupt Enable */
#define IMX_USDHC_INT_SIGNAL_EN_CCEIEN_SHIFT    17
#define IMX_USDHC_INT_SIGNAL_EN_CCEIEN_MASK     0x20000
#define IMX_USDHC_INT_SIGNAL_EN_CCEIEN(v)       (((v) << 0x11) & 0x20000)
/* Field CEBEIEN - Command End Bit Error Interrupt Enable */
#define IMX_USDHC_INT_SIGNAL_EN_CEBEIEN_SHIFT   18
#define IMX_USDHC_INT_SIGNAL_EN_CEBEIEN_MASK    0x40000
#define IMX_USDHC_INT_SIGNAL_EN_CEBEIEN(v)      (((v) << 0x12) & 0x40000)
/* Field CIEIEN - Command Index Error Interrupt Enable */
#define IMX_USDHC_INT_SIGNAL_EN_CIEIEN_SHIFT    19
#define IMX_USDHC_INT_SIGNAL_EN_CIEIEN_MASK     0x80000
#define IMX_USDHC_INT_SIGNAL_EN_CIEIEN(v)       (((v) << 0x13) & 0x80000)
/* Field DTOEIEN - Data Timeout Error Interrupt Enable */
#define IMX_USDHC_INT_SIGNAL_EN_DTOEIEN_SHIFT   20
#define IMX_USDHC_INT_SIGNAL_EN_DTOEIEN_MASK    0x100000
#define IMX_USDHC_INT_SIGNAL_EN_DTOEIEN(v)      (((v) << 0x14) & 0x100000)
/* Field DCEIEN - Data CRC Error Interrupt Enable */
#define IMX_USDHC_INT_SIGNAL_EN_DCEIEN_SHIFT    21
#define IMX_USDHC_INT_SIGNAL_EN_DCEIEN_MASK     0x200000
#define IMX_USDHC_INT_SIGNAL_EN_DCEIEN(v)       (((v) << 0x15) & 0x200000)
/* Field DEBEIEN - Data End Bit Error Interrupt Enable */
#define IMX_USDHC_INT_SIGNAL_EN_DEBEIEN_SHIFT   22
#define IMX_USDHC_INT_SIGNAL_EN_DEBEIEN_MASK    0x400000
#define IMX_USDHC_INT_SIGNAL_EN_DEBEIEN(v)      (((v) << 0x16) & 0x400000)
/* Field AC12EIEN - Auto CMD12 Error Interrupt Enable */
#define IMX_USDHC_INT_SIGNAL_EN_AC12EIEN_SHIFT  24
#define IMX_USDHC_INT_SIGNAL_EN_AC12EIEN_MASK   0x1000000
#define IMX_USDHC_INT_SIGNAL_EN_AC12EIEN(v)     (((v) << 0x18) & 0x1000000)
/* Field TNEIEN - Tuning Error Interrupt Enable */
#define IMX_USDHC_INT_SIGNAL_EN_TNEIEN_SHIFT    26
#define IMX_USDHC_INT_SIGNAL_EN_TNEIEN_MASK     0x4000000
#define IMX_USDHC_INT_SIGNAL_EN_TNEIEN(v)       (((v) << 0x1A) & 0x4000000)
/* Field DMAEIEN - DMA Error Interrupt Enable */
#define IMX_USDHC_INT_SIGNAL_EN_DMAEIEN_SHIFT   28
#define IMX_USDHC_INT_SIGNAL_EN_DMAEIEN_MASK    0x10000000
#define IMX_USDHC_INT_SIGNAL_EN_DMAEIEN(v)      (((v) << 0x1C) & 0x10000000)
/* Additional user defines */
#define IMX_USDHC_INT_SIGNAL_EN_DFLTS           0x117FF1FF



/* Register AUTOCMD12_ERR_STATUS - Auto CMD12 Error Status */
#define IMX_USDHC_AUTOCMD12_ERR_STATUS          0x3C
/* Field AC12NE - Auto CMD12 Not Executed */
#define IMX_USDHC_AUTOCMD12_ERR_STATUS_AC12NE_SHIFT 0
#define IMX_USDHC_AUTOCMD12_ERR_STATUS_AC12NE_MASK  0x1
#define IMX_USDHC_AUTOCMD12_ERR_STATUS_AC12NE(v)    (((v) << 0) & 0x1)
/* Field AC12TOE - Auto CMD12 / 23 Timeout Error */
#define IMX_USDHC_AUTOCMD12_ERR_STATUS_AC12TOE_SHIFT 1
#define IMX_USDHC_AUTOCMD12_ERR_STATUS_AC12TOE_MASK  0x2
#define IMX_USDHC_AUTOCMD12_ERR_STATUS_AC12TOE(v)    (((v) << 0x1) & 0x2)
/* Field AC12EBE - Auto CMD12 / 23 End Bit Error */
#define IMX_USDHC_AUTOCMD12_ERR_STATUS_AC12EBE_SHIFT 2
#define IMX_USDHC_AUTOCMD12_ERR_STATUS_AC12EBE_MASK  0x4
#define IMX_USDHC_AUTOCMD12_ERR_STATUS_AC12EBE(v)    (((v) << 0x2) & 0x4)
/* Field AC12CE - Auto CMD12 / 23 CRC Error */
#define IMX_USDHC_AUTOCMD12_ERR_STATUS_AC12CE_SHIFT 3
#define IMX_USDHC_AUTOCMD12_ERR_STATUS_AC12CE_MASK  0x8
#define IMX_USDHC_AUTOCMD12_ERR_STATUS_AC12CE(v)    (((v) << 0x3) & 0x8)
/* Field AC12IE - Auto CMD12 / 23 Index Error */
#define IMX_USDHC_AUTOCMD12_ERR_STATUS_AC12IE_SHIFT 4
#define IMX_USDHC_AUTOCMD12_ERR_STATUS_AC12IE_MASK  0x10
#define IMX_USDHC_AUTOCMD12_ERR_STATUS_AC12IE(v)    (((v) << 0x4) & 0x10)
/* Field CNIBAC12E - Command Not Issued By Auto CMD12 Error */
#define IMX_USDHC_AUTOCMD12_ERR_STATUS_CNIBAC12E_SHIFT 7
#define IMX_USDHC_AUTOCMD12_ERR_STATUS_CNIBAC12E_MASK  0x80
#define IMX_USDHC_AUTOCMD12_ERR_STATUS_CNIBAC12E(v)    (((v) << 0x7) & 0x80)
/* Field EXECUTE_TUNING - Execute Tuning */
#define IMX_USDHC_AUTOCMD12_ERR_STATUS_EXECUTE_TUNING_SHIFT 22
#define IMX_USDHC_AUTOCMD12_ERR_STATUS_EXECUTE_TUNING_MASK  0x400000
#define IMX_USDHC_AUTOCMD12_ERR_STATUS_EXECUTE_TUNING(v)    (((v) << 0x16) & 0x400000)
/* Field SMP_CLK_SEL - Sample Clock Select */
#define IMX_USDHC_AUTOCMD12_ERR_STATUS_SMP_CLK_SEL_SHIFT 23
#define IMX_USDHC_AUTOCMD12_ERR_STATUS_SMP_CLK_SEL_MASK  0x800000
#define IMX_USDHC_AUTOCMD12_ERR_STATUS_SMP_CLK_SEL(v)    (((v) << 0x17) & 0x800000)



/* Register HOST_CTRL_CAP - Host Controller Capabilities */
#define IMX_USDHC_HOST_CTRL_CAP                 0x40
/* Field SDR50_SUPPORT - SDR50 support */
#define IMX_USDHC_HOST_CTRL_CAP_SDR50_SUPPORT_SHIFT 0
#define IMX_USDHC_HOST_CTRL_CAP_SDR50_SUPPORT_MASK  0x1
#define IMX_USDHC_HOST_CTRL_CAP_SDR50_SUPPORT(v)    (((v) << 0) & 0x1)
/* Field SDR104_SUPPORT - SDR104 support */
#define IMX_USDHC_HOST_CTRL_CAP_SDR104_SUPPORT_SHIFT 1
#define IMX_USDHC_HOST_CTRL_CAP_SDR104_SUPPORT_MASK  0x2
#define IMX_USDHC_HOST_CTRL_CAP_SDR104_SUPPORT(v)    (((v) << 0x1) & 0x2)
/* Field DDR50_SUPPORT - DDR50 support */
#define IMX_USDHC_HOST_CTRL_CAP_DDR50_SUPPORT_SHIFT 2
#define IMX_USDHC_HOST_CTRL_CAP_DDR50_SUPPORT_MASK  0x4
#define IMX_USDHC_HOST_CTRL_CAP_DDR50_SUPPORT(v)    (((v) << 0x2) & 0x4)
/* Field TIME_COUNT_RETUNING - Time Counter for Retuning */
#define IMX_USDHC_HOST_CTRL_CAP_TIME_COUNT_RETUNING_SHIFT 8
#define IMX_USDHC_HOST_CTRL_CAP_TIME_COUNT_RETUNING_MASK  0xF00
#define IMX_USDHC_HOST_CTRL_CAP_TIME_COUNT_RETUNING(v)    (((v) << 0x8) & 0xF00)
/* Field USE_TUNING_SDR50 - Use Tuning for SDR50 */
#define IMX_USDHC_HOST_CTRL_CAP_USE_TUNING_SDR50_SHIFT 13
#define IMX_USDHC_HOST_CTRL_CAP_USE_TUNING_SDR50_MASK  0x2000
#define IMX_USDHC_HOST_CTRL_CAP_USE_TUNING_SDR50(v)    (((v) << 0xD) & 0x2000)
/* Field RETUNING_MODE - Retuning Mode */
#define IMX_USDHC_HOST_CTRL_CAP_RETUNING_MODE_SHIFT 14
#define IMX_USDHC_HOST_CTRL_CAP_RETUNING_MODE_MASK  0xC000
#define IMX_USDHC_HOST_CTRL_CAP_RETUNING_MODE(v)    (((v) << 0xE) & 0xC000)
/* Field MBL - Max Block Length */
#define IMX_USDHC_HOST_CTRL_CAP_MBL_SHIFT       16
#define IMX_USDHC_HOST_CTRL_CAP_MBL_MASK        0x70000
#define IMX_USDHC_HOST_CTRL_CAP_MBL(v)          (((v) << 0x10) & 0x70000)
/* Field ADMAS - ADMA Support */
#define IMX_USDHC_HOST_CTRL_CAP_ADMAS_SHIFT     20
#define IMX_USDHC_HOST_CTRL_CAP_ADMAS_MASK      0x100000
#define IMX_USDHC_HOST_CTRL_CAP_ADMAS(v)        (((v) << 0x14) & 0x100000)
/* Field HSS - High Speed Support */
#define IMX_USDHC_HOST_CTRL_CAP_HSS_SHIFT       21
#define IMX_USDHC_HOST_CTRL_CAP_HSS_MASK        0x200000
#define IMX_USDHC_HOST_CTRL_CAP_HSS(v)          (((v) << 0x15) & 0x200000)
/* Field DMAS - DMA Support */
#define IMX_USDHC_HOST_CTRL_CAP_DMAS_SHIFT      22
#define IMX_USDHC_HOST_CTRL_CAP_DMAS_MASK       0x400000
#define IMX_USDHC_HOST_CTRL_CAP_DMAS(v)         (((v) << 0x16) & 0x400000)
/* Field SRS - Suspend / Resume Support */
#define IMX_USDHC_HOST_CTRL_CAP_SRS_SHIFT       23
#define IMX_USDHC_HOST_CTRL_CAP_SRS_MASK        0x800000
#define IMX_USDHC_HOST_CTRL_CAP_SRS(v)          (((v) << 0x17) & 0x800000)
/* Field VS33 - Voltage Support 3.3V */
#define IMX_USDHC_HOST_CTRL_CAP_VS33_SHIFT      24
#define IMX_USDHC_HOST_CTRL_CAP_VS33_MASK       0x1000000
#define IMX_USDHC_HOST_CTRL_CAP_VS33(v)         (((v) << 0x18) & 0x1000000)
/* Field VS30 - Voltage Support 3.0 V */
#define IMX_USDHC_HOST_CTRL_CAP_VS30_SHIFT      25
#define IMX_USDHC_HOST_CTRL_CAP_VS30_MASK       0x2000000
#define IMX_USDHC_HOST_CTRL_CAP_VS30(v)         (((v) << 0x19) & 0x2000000)
/* Field VS18 - Voltage Support 1.8 V */
#define IMX_USDHC_HOST_CTRL_CAP_VS18_SHIFT      26
#define IMX_USDHC_HOST_CTRL_CAP_VS18_MASK       0x4000000
#define IMX_USDHC_HOST_CTRL_CAP_VS18(v)         (((v) << 0x1A) & 0x4000000)



/* Register WTMK_LVL - Watermark Level */
#define IMX_USDHC_WTMK_LVL                      0x44
#define IMX_USDHC_WTMK_LVL_FIFO_SIZE            128
/* Field RD_WML - Read Watermark Level */
#define IMX_USDHC_WTMK_LVL_RD_WML_SHIFT         0
#define IMX_USDHC_WTMK_LVL_RD_WML_MASK          0xFF
#define IMX_USDHC_WTMK_LVL_RD_WML(v)            (((v) << 0) & 0xFF)
/* Maximum watermark level */
#define IMX_USDHC_WTMK_LVL_RD_WML_BV_MAX_VAL    IMX_USDHC_WTMK_LVL_FIFO_SIZE
/* Field RD_BRST_LEN - Read Burst Length Due to system restriction.
 * The actual burst length may not exceed 16. */
#define IMX_USDHC_WTMK_LVL_RD_BRST_LEN_SHIFT    8
#define IMX_USDHC_WTMK_LVL_RD_BRST_LEN_MASK     0x1F00
#define IMX_USDHC_WTMK_LVL_RD_BRST_LEN(v)       (((v) << 0x8) & 0x1F00)
/* Field WR_WML - Write Watermark Level */
#define IMX_USDHC_WTMK_LVL_WR_WML_SHIFT         16
#define IMX_USDHC_WTMK_LVL_WR_WML_MASK          0xFF0000
#define IMX_USDHC_WTMK_LVL_WR_WML(v)            (((v) << 0x10) & 0xFF0000)
/* Maximum watermark level */
#define IMX_USDHC_WTMK_LVL_WR_WML_BV_MAX_VAL    IMX_USDHC_WTMK_LVL_FIFO_SIZE
/* Field WR_BRST_LEN - Write Burst Length Due to system restriction
 * The actual burst length may not exceed 16. */
#define IMX_USDHC_WTMK_LVL_WR_BRST_LEN_SHIFT    24
#define IMX_USDHC_WTMK_LVL_WR_BRST_LEN_MASK     0x1F000000
#define IMX_USDHC_WTMK_LVL_WR_BRST_LEN(v)       (((v) << 0x18) & 0x1F000000)



/* Register MIX_CTRL - Mixer Control */
#define IMX_USDHC_MIX_CTRL                      0x48
/* Field DMAEN - DMA Enable */
#define IMX_USDHC_MIX_CTRL_DMAEN_SHIFT          0
#define IMX_USDHC_MIX_CTRL_DMAEN_MASK           0x1
#define IMX_USDHC_MIX_CTRL_DMAEN(v)             (((v) << 0) & 0x1)
/* Field BCEN - Block Count Enable */
#define IMX_USDHC_MIX_CTRL_BCEN_SHIFT           1
#define IMX_USDHC_MIX_CTRL_BCEN_MASK            0x2
#define IMX_USDHC_MIX_CTRL_BCEN(v)              (((v) << 0x1) & 0x2)
/* Field AC12EN - Auto CMD12 Enable */
#define IMX_USDHC_MIX_CTRL_AC12EN_SHIFT         2
#define IMX_USDHC_MIX_CTRL_AC12EN_MASK          0x4
#define IMX_USDHC_MIX_CTRL_AC12EN(v)            (((v) << 0x2) & 0x4)
/* Field DDR_EN - Dual Data Rate mode selection */
#define IMX_USDHC_MIX_CTRL_DDR_EN_SHIFT         3
#define IMX_USDHC_MIX_CTRL_DDR_EN_MASK          0x8
#define IMX_USDHC_MIX_CTRL_DDR_EN(v)            (((v) << 0x3) & 0x8)
/* Field DTDSEL - Data Transfer Direction Select */
#define IMX_USDHC_MIX_CTRL_DTDSEL_SHIFT         4
#define IMX_USDHC_MIX_CTRL_DTDSEL_MASK          0x10
#define IMX_USDHC_MIX_CTRL_DTDSEL(v)            (((v) << 0x4) & 0x10)
/* Field MSBSEL - Multiple / Single Block Select */
#define IMX_USDHC_MIX_CTRL_MSBSEL_SHIFT         5
#define IMX_USDHC_MIX_CTRL_MSBSEL_MASK          0x20
#define IMX_USDHC_MIX_CTRL_MSBSEL(v)            (((v) << 0x5) & 0x20)
/* Field NIBBLE_POS -  */
#define IMX_USDHC_MIX_CTRL_NIBBLE_POS_SHIFT     6
#define IMX_USDHC_MIX_CTRL_NIBBLE_POS_MASK      0x40
#define IMX_USDHC_MIX_CTRL_NIBBLE_POS(v)        (((v) << 0x6) & 0x40)
/* Field AC23EN - Auto CMD23 Enable */
#define IMX_USDHC_MIX_CTRL_AC23EN_SHIFT         7
#define IMX_USDHC_MIX_CTRL_AC23EN_MASK          0x80
#define IMX_USDHC_MIX_CTRL_AC23EN(v)            (((v) << 0x7) & 0x80)
/* Field EXE_TUNE - Execute Tuning: (Only used for SD3.0, SDR104 mode) */
#define IMX_USDHC_MIX_CTRL_EXE_TUNE_SHIFT       22
#define IMX_USDHC_MIX_CTRL_EXE_TUNE_MASK        0x400000
#define IMX_USDHC_MIX_CTRL_EXE_TUNE(v)          (((v) << 0x16) & 0x400000)
/* Field SMP_CLK_SEL - Select clock to sample data */
#define IMX_USDHC_MIX_CTRL_SMP_CLK_SEL_SHIFT    23
#define IMX_USDHC_MIX_CTRL_SMP_CLK_SEL_MASK     0x800000
#define IMX_USDHC_MIX_CTRL_SMP_CLK_SEL(v)       (((v) << 0x17) & 0x800000)
/* Field AUTO_TUNE_EN - Auto Tuning Enable
 * (Only used for SD3.0, SDR104 mode) */
#define IMX_USDHC_MIX_CTRL_AUTO_TUNE_EN_SHIFT   24
#define IMX_USDHC_MIX_CTRL_AUTO_TUNE_EN_MASK    0x1000000
#define IMX_USDHC_MIX_CTRL_AUTO_TUNE_EN(v)      (((v) << 0x18) & 0x1000000)
/* Field FBCLK_SEL - Feedback Clock Source Selection
 * (Only used for SD3.0, SDR104 mode) */
#define IMX_USDHC_MIX_CTRL_FBCLK_SEL_SHIFT      25
#define IMX_USDHC_MIX_CTRL_FBCLK_SEL_MASK       0x2000000
#define IMX_USDHC_MIX_CTRL_FBCLK_SEL(v)         (((v) << 0x19) & 0x2000000)
/* Field HS400_MODE - Enable HS400 Mode */
#define IMX_USDHC_MIX_CTRL_HS400_MODE_SHIFT     26
#define IMX_USDHC_MIX_CTRL_HS400_MODE_MASK      0x4000000
#define IMX_USDHC_MIX_CTRL_HS400_MODE(v)        (((v) << 0x1A) & 0x4000000)
/* Field HS400ES_MODE - Enable HS400ES mode */
#define IMX_USDHC_MIX_CTRL_HS400ES_EN_SHIFT     27
#define IMX_USDHC_MIX_CTRL_HS400ES_EN_MASK      0x8000000
#define IMX_USDHC_MIX_CTRL_HS400ES_EN(v)        (((v) << 27) & 0x8000000)

/* Register FORCE_EVENT - Force Event */
#define IMX_USDHC_FORCE_EVENT                   0x50
/* Field FEVTAC12NE - Force Event Auto Command 12 Not Executed */
#define IMX_USDHC_FORCE_EVENT_FEVTAC12NE_SHIFT  0
#define IMX_USDHC_FORCE_EVENT_FEVTAC12NE_MASK   0x1
#define IMX_USDHC_FORCE_EVENT_FEVTAC12NE(v)     (((v) << 0) & 0x1)
/* Field FEVTAC12TOE - Force Event Auto Command 12 Time Out Error */
#define IMX_USDHC_FORCE_EVENT_FEVTAC12TOE_SHIFT 1
#define IMX_USDHC_FORCE_EVENT_FEVTAC12TOE_MASK  0x2
#define IMX_USDHC_FORCE_EVENT_FEVTAC12TOE(v)    (((v) << 0x1) & 0x2)
/* Field FEVTAC12CE - Force Event Auto Command 12 CRC Error */
#define IMX_USDHC_FORCE_EVENT_FEVTAC12CE_SHIFT  2
#define IMX_USDHC_FORCE_EVENT_FEVTAC12CE_MASK   0x4
#define IMX_USDHC_FORCE_EVENT_FEVTAC12CE(v)     (((v) << 0x2) & 0x4)
/* Field FEVTAC12EBE - Force Event Auto Command 12 End Bit Error */
#define IMX_USDHC_FORCE_EVENT_FEVTAC12EBE_SHIFT 3
#define IMX_USDHC_FORCE_EVENT_FEVTAC12EBE_MASK  0x8
#define IMX_USDHC_FORCE_EVENT_FEVTAC12EBE(v)    (((v) << 0x3) & 0x8)
/* Field FEVTAC12IE - Force Event Auto Command 12 Index Error */
#define IMX_USDHC_FORCE_EVENT_FEVTAC12IE_SHIFT  4
#define IMX_USDHC_FORCE_EVENT_FEVTAC12IE_MASK   0x10
#define IMX_USDHC_FORCE_EVENT_FEVTAC12IE(v)     (((v) << 0x4) & 0x10)
/* Field FEVTCNIBAC12E -
 * Force Event Command Not Executed By Auto Command 12 Error */
#define IMX_USDHC_FORCE_EVENT_FEVTCNIBAC12E_SHIFT 7
#define IMX_USDHC_FORCE_EVENT_FEVTCNIBAC12E_MASK  0x80
#define IMX_USDHC_FORCE_EVENT_FEVTCNIBAC12E(v)    (((v) << 0x7) & 0x80)
/* Field FEVTCTOE - Force Event Command Time Out Error */
#define IMX_USDHC_FORCE_EVENT_FEVTCTOE_SHIFT    16
#define IMX_USDHC_FORCE_EVENT_FEVTCTOE_MASK     0x10000
#define IMX_USDHC_FORCE_EVENT_FEVTCTOE(v)       (((v) << 0x10) & 0x10000)
/* Field FEVTCCE - Force Event Command CRC Error */
#define IMX_USDHC_FORCE_EVENT_FEVTCCE_SHIFT     17
#define IMX_USDHC_FORCE_EVENT_FEVTCCE_MASK      0x20000
#define IMX_USDHC_FORCE_EVENT_FEVTCCE(v)        (((v) << 0x11) & 0x20000)
/* Field FEVTCEBE - Force Event Command End Bit Error */
#define IMX_USDHC_FORCE_EVENT_FEVTCEBE_SHIFT    18
#define IMX_USDHC_FORCE_EVENT_FEVTCEBE_MASK     0x40000
#define IMX_USDHC_FORCE_EVENT_FEVTCEBE(v)       (((v) << 0x12) & 0x40000)
/* Field FEVTCIE - Force Event Command Index Error */
#define IMX_USDHC_FORCE_EVENT_FEVTCIE_SHIFT     19
#define IMX_USDHC_FORCE_EVENT_FEVTCIE_MASK      0x80000
#define IMX_USDHC_FORCE_EVENT_FEVTCIE(v)        (((v) << 0x13) & 0x80000)
/* Field FEVTDTOE - Force Event Data Time Out Error */
#define IMX_USDHC_FORCE_EVENT_FEVTDTOE_SHIFT    20
#define IMX_USDHC_FORCE_EVENT_FEVTDTOE_MASK     0x100000
#define IMX_USDHC_FORCE_EVENT_FEVTDTOE(v)       (((v) << 0x14) & 0x100000)
/* Field FEVTDCE - Force Event Data CRC Error */
#define IMX_USDHC_FORCE_EVENT_FEVTDCE_SHIFT     21
#define IMX_USDHC_FORCE_EVENT_FEVTDCE_MASK      0x200000
#define IMX_USDHC_FORCE_EVENT_FEVTDCE(v)        (((v) << 0x15) & 0x200000)
/* Field FEVTDEBE - Force Event Data End Bit Error */
#define IMX_USDHC_FORCE_EVENT_FEVTDEBE_SHIFT    22
#define IMX_USDHC_FORCE_EVENT_FEVTDEBE_MASK     0x400000
#define IMX_USDHC_FORCE_EVENT_FEVTDEBE(v)       (((v) << 0x16) & 0x400000)
/* Field FEVTAC12E - Force Event Auto Command 12 Error */
#define IMX_USDHC_FORCE_EVENT_FEVTAC12E_SHIFT   24
#define IMX_USDHC_FORCE_EVENT_FEVTAC12E_MASK    0x1000000
#define IMX_USDHC_FORCE_EVENT_FEVTAC12E(v)      (((v) << 0x18) & 0x1000000)
/* Field FEVTTNE - Force Tuning Error */
#define IMX_USDHC_FORCE_EVENT_FEVTTNE_SHIFT     26
#define IMX_USDHC_FORCE_EVENT_FEVTTNE_MASK      0x4000000
#define IMX_USDHC_FORCE_EVENT_FEVTTNE(v)        (((v) << 0x1A) & 0x4000000)
/* Field FEVTDMAE - Force Event DMA Error */
#define IMX_USDHC_FORCE_EVENT_FEVTDMAE_SHIFT    28
#define IMX_USDHC_FORCE_EVENT_FEVTDMAE_MASK     0x10000000
#define IMX_USDHC_FORCE_EVENT_FEVTDMAE(v)       (((v) << 0x1C) & 0x10000000)
/* Field FEVTCINT - Force Event Card Interrupt */
#define IMX_USDHC_FORCE_EVENT_FEVTCINT_SHIFT    31
#define IMX_USDHC_FORCE_EVENT_FEVTCINT_MASK     0x80000000
#define IMX_USDHC_FORCE_EVENT_FEVTCINT(v)       (((v) << 0x1F) & 0x80000000)



/* Register ADMA_ERR_STATUS - ADMA Error Status Register */
#define IMX_USDHC_ADMA_ERR_STATUS               0x54
/* Field ADMAES - ADMA Error State (when ADMA Error is occurred) */
#define IMX_USDHC_ADMA_ERR_STATUS_ADMAES_SHIFT  0
#define IMX_USDHC_ADMA_ERR_STATUS_ADMAES_MASK   0x3
#define IMX_USDHC_ADMA_ERR_STATUS_ADMAES(v)     (((v) << 0) & 0x3)
/* Field ADMALME - ADMA Length Mismatch Error */
#define IMX_USDHC_ADMA_ERR_STATUS_ADMALME_SHIFT 2
#define IMX_USDHC_ADMA_ERR_STATUS_ADMALME_MASK  0x4
#define IMX_USDHC_ADMA_ERR_STATUS_ADMALME(v)    (((v) << 0x2) & 0x4)
/* Field ADMADCE - ADMA Descriptor Error */
#define IMX_USDHC_ADMA_ERR_STATUS_ADMADCE_SHIFT 3
#define IMX_USDHC_ADMA_ERR_STATUS_ADMADCE_MASK  0x8
#define IMX_USDHC_ADMA_ERR_STATUS_ADMADCE(v)    (((v) << 0x3) & 0x8)



/* Register ADMA_SYS_ADDR - ADMA System Address */
#define IMX_USDHC_ADMA_SYS_ADDR                 0x58
/* Field ADS_ADDR - ADMA System Address */
#define IMX_USDHC_ADMA_SYS_ADDR_ADS_ADDR_SHIFT  2
#define IMX_USDHC_ADMA_SYS_ADDR_ADS_ADDR_MASK   0xFFFFFFFC
#define IMX_USDHC_ADMA_SYS_ADDR_ADS_ADDR(v)     (((v) << 0x2) & 0xFFFFFFFC)



/* Register DLL_CTRL - DLL (Delay Line) Control */
#define IMX_USDHC_DLL_CTRL                      0x60
/* Field DLL_CTRL_ENABLE - Enable/Disable the DLL and delay chain */
#define IMX_USDHC_DLL_CTRL_ENABLE_SHIFT         0
#define IMX_USDHC_DLL_CTRL_ENABLE_MASK          0x1
#define IMX_USDHC_DLL_CTRL_ENABLE(v)            (((v) << 0) & 0x1)
/* Field DLL_CTRL_RESET - Force a reset on DLL */
#define IMX_USDHC_DLL_CTRL_RESET_SHIFT          1
#define IMX_USDHC_DLL_CTRL_RESET_MASK           0x2
#define IMX_USDHC_DLL_CTRL_RESET(v)             (((v) << 0x1) & 0x2)
/* Field DLL_CTRL_SLV_FORCE_UPD -
 * Update to the DLL calibrated value immediately */
#define IMX_USDHC_DLL_CTRL_SLV_FORCE_UPD_SHIFT  2
#define IMX_USDHC_DLL_CTRL_SLV_FORCE_UPD_MASK   0x4
#define IMX_USDHC_DLL_CTRL_SLV_FORCE_UPD(v)     (((v) << 0x2) & 0x4)
/* Field DLL_CTRL_SLV_DLY_TARGET0 - Delay target for the uSDHC loop-back */
#define IMX_USDHC_DLL_CTRL_SLV_DLY_TARGET0_SHIFT 3
#define IMX_USDHC_DLL_CTRL_SLV_DLY_TARGET0_MASK  0x78
#define IMX_USDHC_DLL_CTRL_SLV_DLY_TARGET0(v)    (((v) << 0x3) & 0x78)
/* Field DLL_CTRL_GATE_UPDATE - Prevent the DLL from updating */
#define IMX_USDHC_DLL_CTRL_GATE_UPDATE_SHIFT    7
#define IMX_USDHC_DLL_CTRL_GATE_UPDATE_MASK     0x80
#define IMX_USDHC_DLL_CTRL_GATE_UPDATE(v)       (((v) << 0x7) & 0x80)
/* Field DLL_CTRL_SLV_OVERRIDE - Manual override for slave delay chain */
#define IMX_USDHC_DLL_CTRL_SLV_OVERRIDE_SHIFT   8
#define IMX_USDHC_DLL_CTRL_SLV_OVERRIDE_MASK    0x100
#define IMX_USDHC_DLL_CTRL_SLV_OVERRIDE(v)      (((v) << 0x8) & 0x100)
/* Field DLL_CTRL_SLV_OVERRIDE_VAL - Taps manual selection */
#define IMX_USDHC_DLL_CTRL_SLV_OVERRIDE_VAL_SHIFT 9
#define IMX_USDHC_DLL_CTRL_SLV_OVERRIDE_VAL_MASK  0xFE00
#define IMX_USDHC_DLL_CTRL_SLV_OVERRIDE_VAL(v)    (((v) << 0x9) & 0xFE00)
/* Field DLL_CTRL_SLV_DLY_TARGET1 - Refer to DLL_CTRL_SLV_DLY_TARGET0 */
#define IMX_USDHC_DLL_CTRL_SLV_DLY_TARGET1_SHIFT 16
#define IMX_USDHC_DLL_CTRL_SLV_DLY_TARGET1_MASK  0x70000
#define IMX_USDHC_DLL_CTRL_SLV_DLY_TARGET1(v)    (((v) << 0x10) & 0x70000)
/* Field DLL_CTRL_SLV_UPDATE_INT - Slave delay line update interval */
#define IMX_USDHC_DLL_CTRL_SLV_UPDATE_INT_SHIFT 20
#define IMX_USDHC_DLL_CTRL_SLV_UPDATE_INT_MASK  0xFF00000
#define IMX_USDHC_DLL_CTRL_SLV_UPDATE_INT(v)    (((v) << 0x14) & 0xFF00000)
/* Field DLL_CTRL_REF_UPDATE_INT - DLL control loop update interval */
#define IMX_USDHC_DLL_CTRL_REF_UPDATE_INT_SHIFT 28
#define IMX_USDHC_DLL_CTRL_REF_UPDATE_INT_MASK  0xF0000000
#define IMX_USDHC_DLL_CTRL_REF_UPDATE_INT(v)    (((v) << 0x1C) & 0xF0000000)



/* Register DLL_STATUS - DLL Status */
#define IMX_USDHC_DLL_STATUS                    0x64
/* Field DLL_STS_SLV_LOCK - Slave delay-line lock status */
#define IMX_USDHC_DLL_STATUS_SLV_LOCK_SHIFT     0
#define IMX_USDHC_DLL_STATUS_SLV_LOCK_MASK      0x1
#define IMX_USDHC_DLL_STATUS_SLV_LOCK(v)        (((v) << 0) & 0x1)
/* Field DLL_STS_REF_LOCK - Reference DLL lock status */
#define IMX_USDHC_DLL_STATUS_REF_LOCK_SHIFT     1
#define IMX_USDHC_DLL_STATUS_REF_LOCK_MASK      0x2
#define IMX_USDHC_DLL_STATUS_REF_LOCK(v)        (((v) << 0x1) & 0x2)
/* Field DLL_STS_SLV_SEL - Slave delay line select status */
#define IMX_USDHC_DLL_STATUS_SLV_SEL_SHIFT      2
#define IMX_USDHC_DLL_STATUS_SLV_SEL_MASK       0x1FC
#define IMX_USDHC_DLL_STATUS_SLV_SEL(v)         (((v) << 0x2) & 0x1FC)
/* Field DLL_STS_REF_SEL - Reference delay line select taps */
#define IMX_USDHC_DLL_STATUS_REF_SEL_SHIFT      9
#define IMX_USDHC_DLL_STATUS_REF_SEL_MASK       0xFE00
#define IMX_USDHC_DLL_STATUS_REF_SEL(v)         (((v) << 0x9) & 0xFE00)



/* Register CLK_TUNE_CTRL_STATUS - CLK Tuning Control and Status */
#define IMX_USDHC_CLK_TUNE_CTRL_STATUS          0x68
/* Field DLY_CELL_SET_POST - Number of delay cells on the feedback clock */
#define IMX_USDHC_CLK_TUNE_CTRL_STATUS_DLY_CELL_SET_POST_SHIFT 0
#define IMX_USDHC_CLK_TUNE_CTRL_STATUS_DLY_CELL_SET_POST_MASK  0xF
#define IMX_USDHC_CLK_TUNE_CTRL_STATUS_DLY_CELL_SET_POST(v)    (((v) << 0) & 0xF)
/* Field DLY_CELL_SET_OUT - Number of delay cells on the feedback clock */
#define IMX_USDHC_CLK_TUNE_CTRL_STATUS_DLY_CELL_SET_OUT_SHIFT 4
#define IMX_USDHC_CLK_TUNE_CTRL_STATUS_DLY_CELL_SET_OUT_MASK  0xF0
#define IMX_USDHC_CLK_TUNE_CTRL_STATUS_DLY_CELL_SET_OUT(v)    (((v) << 0x4) & 0xF0)
/* Field DLY_CELL_SET_PRE - Number of delay cells on the feedback clock */
#define IMX_USDHC_CLK_TUNE_CTRL_STATUS_DLY_CELL_SET_PRE_SHIFT 8
#define IMX_USDHC_CLK_TUNE_CTRL_STATUS_DLY_CELL_SET_PRE_MASK  0x7F00
#define IMX_USDHC_CLK_TUNE_CTRL_STATUS_DLY_CELL_SET_PRE(v)    (((v) << 0x8) & 0x7F00)
/* Field DLY_CELL_SET_PRE bit field values */
#define IMX_USDHC_CLK_TUNE_CTRL_STATUS_PRE_MIN  0x0
#define IMX_USDHC_CLK_TUNE_CTRL_STATUS_PRE_MAX  ((1 << 7) - 1)
#define IMX_USDHC_CLK_TUNE_CTRL_STATUS_PRE_STEP 1
/* Field NXT_ERR -
 * Number of delay cells added on the feedback clock is too large */
#define IMX_USDHC_CLK_TUNE_CTRL_STATUS_NXT_ERR_SHIFT 15
#define IMX_USDHC_CLK_TUNE_CTRL_STATUS_NXT_ERR_MASK  0x8000
#define IMX_USDHC_CLK_TUNE_CTRL_STATUS_NXT_ERR(v)    (((v) << 0xF) & 0x8000)
/* Field TAP_SEL_POST - Number of delay cells added on the feedback clock */
#define IMX_USDHC_CLK_TUNE_CTRL_STATUS_TAP_SEL_POST_SHIFT 16
#define IMX_USDHC_CLK_TUNE_CTRL_STATUS_TAP_SEL_POST_MASK  0xF0000
#define IMX_USDHC_CLK_TUNE_CTRL_STATUS_TAP_SEL_POST(v)    (((v) << 0x10) & 0xF0000)
/* Field TAP_SEL_OUT - Number of delay cells added on the feedback clock */
#define IMX_USDHC_CLK_TUNE_CTRL_STATUS_TAP_SEL_OUT_SHIFT 20
#define IMX_USDHC_CLK_TUNE_CTRL_STATUS_TAP_SEL_OUT_MASK  0xF00000
#define IMX_USDHC_CLK_TUNE_CTRL_STATUS_TAP_SEL_OUT(v)    (((v) << 0x14) & 0xF00000)
/* Field TAP_SEL_PRE - Number of delay cells added on the feedback clock */
#define IMX_USDHC_CLK_TUNE_CTRL_STATUS_TAP_SEL_PRE_SHIFT 24
#define IMX_USDHC_CLK_TUNE_CTRL_STATUS_TAP_SEL_PRE_MASK  0x7F000000
#define IMX_USDHC_CLK_TUNE_CTRL_STATUS_TAP_SEL_PRE(v)    (((v) << 0x18) & 0x7F000000)
/* Field PRE_ERR -
 * Number of delay cells added on the feedback clock is too small */
#define IMX_USDHC_CLK_TUNE_CTRL_STATUS_PRE_ERR_SHIFT 31
#define IMX_USDHC_CLK_TUNE_CTRL_STATUS_PRE_ERR_MASK  0x80000000
#define IMX_USDHC_CLK_TUNE_CTRL_STATUS_PRE_ERR(v)    (((v) << 0x1F) & 0x80000000)



/* Register STROBE_DLL_CTRL - Strobe DLL Control */
#define IMX_USDHC_STROBE_DLL_CTRL               0x70
/* Field STROBE_DLL_CTRL_ENABLE - Strobe DLL Control Enable */
#define IMX_USDHC_STROBE_DLL_CTRL_ENABLE_SHIFT  0
#define IMX_USDHC_STROBE_DLL_CTRL_ENABLE_MASK   0x1
#define IMX_USDHC_STROBE_DLL_CTRL_ENABLE(v)     (((v) << 0) & 0x1)
/* Field STROBE_DLL_CTRL_RESET - Strobe DLL Control Reset */
#define IMX_USDHC_STROBE_DLL_CTRL_RESET_SHIFT   1
#define IMX_USDHC_STROBE_DLL_CTRL_RESET_MASK    0x2
#define IMX_USDHC_STROBE_DLL_CTRL_RESET(v)      (((v) << 0x1) & 0x2)
/* Field STROBE_DLL_CTRL_SLV_FORCE_UPD -
 * Strobe DLL Control Slave Force Updated */
#define IMX_USDHC_STROBE_DLL_CTRL_SLV_FORCE_UPD_SHIFT 2
#define IMX_USDHC_STROBE_DLL_CTRL_SLV_FORCE_UPD_MASK  0x4
#define IMX_USDHC_STROBE_DLL_CTRL_SLV_FORCE_UPD(v)    (((v) << 0x2) & 0x4)
/* Field STROBE_DLL_CTRL_SLV_DLY_TARGET -
 * Strobe DLL Control Slave Delay Target */
#define IMX_USDHC_STROBE_DLL_CTRL_SLV_DLY_TARGET_SHIFT 3
#define IMX_USDHC_STROBE_DLL_CTRL_SLV_DLY_TARGET_MASK  0x38
#define IMX_USDHC_STROBE_DLL_CTRL_SLV_DLY_TARGET(v)    (((v) << 0x3) & 0x38)
/* Field STROBE_DLL_CTRL_GATE_UPDATE_0 - Strobe DLL Control Gate Update */
#define IMX_USDHC_STROBE_DLL_CTRL_GATE_UPDATE_0_SHIFT 6
#define IMX_USDHC_STROBE_DLL_CTRL_GATE_UPDATE_0_MASK  0x40
#define IMX_USDHC_STROBE_DLL_CTRL_GATE_UPDATE_0(v)    (((v) << 0x6) & 0x40)
/* Field STROBE_DLL_CTRL_GATE_UPDATE_1 - Strobe DLL Control Gate Update */
#define IMX_USDHC_STROBE_DLL_CTRL_GATE_UPDATE_1_SHIFT 7
#define IMX_USDHC_STROBE_DLL_CTRL_GATE_UPDATE_1_MASK  0x80
#define IMX_USDHC_STROBE_DLL_CTRL_GATE_UPDATE_1(v)    (((v) << 0x7) & 0x80)
/* Field STROBE_DLL_CTRL_SLV_OVERRIDE - Strobe DLL Control Slave Override */
#define IMX_USDHC_STROBE_DLL_CTRL_SLV_OVERRIDE_SHIFT 8
#define IMX_USDHC_STROBE_DLL_CTRL_SLV_OVERRIDE_MASK  0x100
#define IMX_USDHC_STROBE_DLL_CTRL_SLV_OVERRIDE(v)    (((v) << 0x8) & 0x100)
/* Field STROBE_DLL_CTRL_SLV_OVERRIDE_VAL -
 * Strobe DLL Control Slave Override Value */
#define IMX_USDHC_STROBE_DLL_CTRL_SLV_OVERRIDE_VAL_SHIFT 9
#define IMX_USDHC_STROBE_DLL_CTRL_SLV_OVERRIDE_VAL_MASK  0xFE00
#define IMX_USDHC_STROBE_DLL_CTRL_SLV_OVERRIDE_VAL(v)    (((v) << 0x9) & 0xFE00)
/* Field STROBE_DLL_CTRL_SLV_UPDATE_INT -
 * Strobe DLL Control Slave Update Interval */
#define IMX_USDHC_STROBE_DLL_CTRL_SLV_UPDATE_INT_SHIFT 20
#define IMX_USDHC_STROBE_DLL_CTRL_SLV_UPDATE_INT_MASK  0xFF00000
#define IMX_USDHC_STROBE_DLL_CTRL_SLV_UPDATE_INT(v)    (((v) << 0x14) & 0xFF00000)
/* Field STROBE_DLL_CTRL_REF_UPDATE_INT -
 * Strobe DLL Control Reference Update Interval */
#define IMX_USDHC_STROBE_DLL_CTRL_REF_UPDATE_INT_SHIFT 28
#define IMX_USDHC_STROBE_DLL_CTRL_REF_UPDATE_INT_MASK  0xF0000000
#define IMX_USDHC_STROBE_DLL_CTRL_REF_UPDATE_INT(v)    (((v) << 0x1C) & 0xF0000000)



/* Register STROBE_DLL_STATUS - Strobe DLL Status */
#define IMX_USDHC_STROBE_DLL_STATUS             0x74
/* Field STROBE_DLL_STS_SLV_LOCK - Strobe DLL Status Slave Lock */
#define IMX_USDHC_STROBE_DLL_STATUS_SLV_LOCK_SHIFT 0
#define IMX_USDHC_STROBE_DLL_STATUS_SLV_LOCK_MASK  0x1
#define IMX_USDHC_STROBE_DLL_STATUS_SLV_LOCK(v)    (((v) << 0) & 0x1)
/* Field STROBE_DLL_STS_REF_LOCK - Strobe DLL Status Reference Lock */
#define IMX_USDHC_STROBE_DLL_STATUS_REF_LOCK_SHIFT 1
#define IMX_USDHC_STROBE_DLL_STATUS_REF_LOCK_MASK  0x2
#define IMX_USDHC_STROBE_DLL_STATUS_REF_LOCK(v)    (((v) << 0x1) & 0x2)
/* Field STROBE_DLL_STS_SLV_SEL - Strobe DLL Status Slave Select */
#define IMX_USDHC_STROBE_DLL_STATUS_SLV_SEL_SHIFT 2
#define IMX_USDHC_STROBE_DLL_STATUS_SLV_SEL_MASK  0x1FC
#define IMX_USDHC_STROBE_DLL_STATUS_SLV_SEL(v)    (((v) << 0x2) & 0x1FC)
/* Field STROBE_DLL_STS_REF_SEL - Strobe DLL Status Reference Select */
#define IMX_USDHC_STROBE_DLL_STATUS_REF_SEL_SHIFT 9
#define IMX_USDHC_STROBE_DLL_STATUS_REF_SEL_MASK  0xFE00
#define IMX_USDHC_STROBE_DLL_STATUS_REF_SEL(v)    (((v) << 0x9) & 0xFE00)



/* Register VEND_SPEC - Vendor Specific Register */
#define IMX_USDHC_VEND_SPEC                     0xC0
/* Field EXT_DMA_EN - External DMA Request Enable */
#define IMX_USDHC_VEND_SPEC_EXT_DMA_EN_SHIFT    0
#define IMX_USDHC_VEND_SPEC_EXT_DMA_EN_MASK     0x1
#define IMX_USDHC_VEND_SPEC_EXT_DMA_EN(v)       (((v) << 0) & 0x1)
/* Field VSELECT - Voltage Selection */
#define IMX_USDHC_VEND_SPEC_VSELECT_SHIFT       1
#define IMX_USDHC_VEND_SPEC_VSELECT_MASK        0x2
#define IMX_USDHC_VEND_SPEC_VSELECT(v)          (((v) << 0x1) & 0x2)
/* Voltage Selection bit values */
#define IMX_USDHC_VEND_SPEC_VSELECT_BV_1V8      1
#define IMX_USDHC_VEND_SPEC_VSELECT_BV_3V0      0
/* Field CONFLICT_CHK_EN - Conflict check enable. */
#define IMX_USDHC_VEND_SPEC_CONFLICT_CHK_EN_SHIFT 2
#define IMX_USDHC_VEND_SPEC_CONFLICT_CHK_EN_MASK  0x4
#define IMX_USDHC_VEND_SPEC_CONFLICT_CHK_EN(v)    (((v) << 0x2) & 0x4)
/* Field AC12_WR_CHKBUSY_EN -
 * Check busy enable after auto CMD12 for write data packet */
#define IMX_USDHC_VEND_SPEC_AC12_WR_CHKBUSY_EN_SHIFT 3
#define IMX_USDHC_VEND_SPEC_AC12_WR_CHKBUSY_EN_MASK  0x8
#define IMX_USDHC_VEND_SPEC_AC12_WR_CHKBUSY_EN(v)    (((v) << 0x3) & 0x8)
/* Field DAT3_CD_POL - Only for debug */
#define IMX_USDHC_VEND_SPEC_DAT3_CD_POL_SHIFT   4
#define IMX_USDHC_VEND_SPEC_DAT3_CD_POL_MASK    0x10
#define IMX_USDHC_VEND_SPEC_DAT3_CD_POL(v)      (((v) << 0x4) & 0x10)
/* Field CD_POL - Only for debug */
#define IMX_USDHC_VEND_SPEC_CD_POL_SHIFT        5
#define IMX_USDHC_VEND_SPEC_CD_POL_MASK         0x20
#define IMX_USDHC_VEND_SPEC_CD_POL(v)           (((v) << 0x5) & 0x20)
/* Field WP_POL - Only for debug */
#define IMX_USDHC_VEND_SPEC_WP_POL_SHIFT        6
#define IMX_USDHC_VEND_SPEC_WP_POL_MASK         0x40
#define IMX_USDHC_VEND_SPEC_WP_POL(v)           (((v) << 0x6) & 0x40)
/* Field CLKONJ_IN_ABORT - Only for debug */
#define IMX_USDHC_VEND_SPEC_CLKONJ_IN_ABORT_SHIFT 7
#define IMX_USDHC_VEND_SPEC_CLKONJ_IN_ABORT_MASK  0x80
#define IMX_USDHC_VEND_SPEC_CLKONJ_IN_ABORT(v)    (((v) << 0x7) & 0x80)
/* Field FRC_SDCLK_ON - Force CLK output active */
#define IMX_USDHC_VEND_SPEC_FRC_SDCLK_ON_SHIFT  8
#define IMX_USDHC_VEND_SPEC_FRC_SDCLK_ON_MASK   0x100
#define IMX_USDHC_VEND_SPEC_FRC_SDCLK_ON(v)     (((v) << 0x8) & 0x100)
/* Field IPG_CLK_SOFT_EN - IPG_CLK Software Enable */
#define IMX_USDHC_VEND_SPEC_IPG_CLK_SOFT_EN_SHIFT 11
#define IMX_USDHC_VEND_SPEC_IPG_CLK_SOFT_EN_MASK  0x800
#define IMX_USDHC_VEND_SPEC_IPG_CLK_SOFT_EN(v)    (((v) << 0xB) & 0x800)
/* Field HCLK_SOFT_EN - AHB Clock Software Enable */
#define IMX_USDHC_VEND_SPEC_HCLK_SOFT_EN_SHIFT  12
#define IMX_USDHC_VEND_SPEC_HCLK_SOFT_EN_MASK   0x1000
#define IMX_USDHC_VEND_SPEC_HCLK_SOFT_EN(v)     (((v) << 0xC) & 0x1000)
/* Field IPG_PERCLK_SOFT_EN - IPG_PERCLK Software Enable */
#define IMX_USDHC_VEND_SPEC_IPG_PERCLK_SOFT_EN_SHIFT 13
#define IMX_USDHC_VEND_SPEC_IPG_PERCLK_SOFT_EN_MASK  0x2000
#define IMX_USDHC_VEND_SPEC_IPG_PERCLK_SOFT_EN(v)    (((v) << 0xD) & 0x2000)
/* Field CARD_CLK_SOFT_EN - Card Clock Software Enable */
#define IMX_USDHC_VEND_SPEC_CARD_CLK_SOFT_EN_SHIFT 14
#define IMX_USDHC_VEND_SPEC_CARD_CLK_SOFT_EN_MASK  0x4000
#define IMX_USDHC_VEND_SPEC_CARD_CLK_SOFT_EN(v)    (((v) << 0xE) & 0x4000)
/* Field CRC_CHK_DIS - CRC Check Disable */
#define IMX_USDHC_VEND_SPEC_CRC_CHK_DIS_SHIFT   15
#define IMX_USDHC_VEND_SPEC_CRC_CHK_DIS_MASK    0x8000
#define IMX_USDHC_VEND_SPEC_CRC_CHK_DIS(v)      (((v) << 0xF) & 0x8000)
/* Field INT_ST_VAL - Internal State Value */
#define IMX_USDHC_VEND_SPEC_INT_ST_VAL_SHIFT    16
#define IMX_USDHC_VEND_SPEC_INT_ST_VAL_MASK     0xFF0000
#define IMX_USDHC_VEND_SPEC_INT_ST_VAL(v)       (((v) << 0x10) & 0xFF0000)
/* Field CMD_BYTE_EN - Byte access */
#define IMX_USDHC_VEND_SPEC_CMD_BYTE_EN_SHIFT   31
#define IMX_USDHC_VEND_SPEC_CMD_BYTE_EN_MASK    0x80000000
#define IMX_USDHC_VEND_SPEC_CMD_BYTE_EN(v)      (((v) << 0x1F) & 0x80000000)



/* Register MMC_BOOT - MMC Boot Register */
#define IMX_USDHC_MMC_BOOT                      0xC4
/* Field DTOCV_ACK - Boot ACK time out counter value */
#define IMX_USDHC_MMC_BOOT_DTOCV_ACK_SHIFT      0
#define IMX_USDHC_MMC_BOOT_DTOCV_ACK_MASK       0xF
#define IMX_USDHC_MMC_BOOT_DTOCV_ACK(v)         (((v) << 0) & 0xF)
/* Field BOOT_ACK - Boot ACK mode select */
#define IMX_USDHC_MMC_BOOT_BOOT_ACK_SHIFT       4
#define IMX_USDHC_MMC_BOOT_BOOT_ACK_MASK        0x10
#define IMX_USDHC_MMC_BOOT_BOOT_ACK(v)          (((v) << 0x4) & 0x10)
/* Field BOOT_MODE - Boot mode select */
#define IMX_USDHC_MMC_BOOT_BOOT_MODE_SHIFT      5
#define IMX_USDHC_MMC_BOOT_BOOT_MODE_MASK       0x20
#define IMX_USDHC_MMC_BOOT_BOOT_MODE(v)         (((v) << 0x5) & 0x20)
/* Field BOOT_EN - Boot mode enable */
#define IMX_USDHC_MMC_BOOT_BOOT_EN_SHIFT        6
#define IMX_USDHC_MMC_BOOT_BOOT_EN_MASK         0x40
#define IMX_USDHC_MMC_BOOT_BOOT_EN(v)           (((v) << 0x6) & 0x40)
/* Field AUTO_SABG_EN - During boot, enable auto stop at block gap function */
#define IMX_USDHC_MMC_BOOT_AUTO_SABG_EN_SHIFT   7
#define IMX_USDHC_MMC_BOOT_AUTO_SABG_EN_MASK    0x80
#define IMX_USDHC_MMC_BOOT_AUTO_SABG_EN(v)      (((v) << 0x7) & 0x80)
/* Field DISABLE_TIME_OUT - Disable Time Out */
#define IMX_USDHC_MMC_BOOT_DISABLE_TIME_OUT_SHIFT 8
#define IMX_USDHC_MMC_BOOT_DISABLE_TIME_OUT_MASK  0x100
#define IMX_USDHC_MMC_BOOT_DISABLE_TIME_OUT(v)    (((v) << 0x8) & 0x100)
/* Field BOOT_BLK_CNT -
 * The value defines the Stop At Block Gap value of automatic mode. */
#define IMX_USDHC_MMC_BOOT_BOOT_BLK_CNT_SHIFT   16
#define IMX_USDHC_MMC_BOOT_BOOT_BLK_CNT_MASK    0xFFFF0000
#define IMX_USDHC_MMC_BOOT_BOOT_BLK_CNT(v)      (((v) << 0x10) & 0xFFFF0000)



/* Register VEND_SPEC2 - Vendor Specific 2 Register */
#define IMX_USDHC_VEND_SPEC2                    0xC8
/* Field SDR104_TIMING_DIS - This bit only uses for debugging */
#define IMX_USDHC_VEND_SPEC2_SDR104_TIMING_DIS_SHIFT 0
#define IMX_USDHC_VEND_SPEC2_SDR104_TIMING_DIS_MASK  0x1
#define IMX_USDHC_VEND_SPEC2_SDR104_TIMING_DIS(v)    (((v) << 0) & 0x1)
/* Field SDR104_OE_DIS - This bit only uses for debugging */
#define IMX_USDHC_VEND_SPEC2_SDR104_OE_DIS_SHIFT 1
#define IMX_USDHC_VEND_SPEC2_SDR104_OE_DIS_MASK  0x2
#define IMX_USDHC_VEND_SPEC2_SDR104_OE_DIS(v)    (((v) << 0x1) & 0x2)
/* Field SDR104_NSD_DIS - This bit only uses for debugging */
#define IMX_USDHC_VEND_SPEC2_SDR104_NSD_DIS_SHIFT 2
#define IMX_USDHC_VEND_SPEC2_SDR104_NSD_DIS_MASK  0x4
#define IMX_USDHC_VEND_SPEC2_SDR104_NSD_DIS(v)    (((v) << 0x2) & 0x4)
/* Field CARD_INT_D3_TEST - Card Interrupt Detection Test */
#define IMX_USDHC_VEND_SPEC2_CARD_INT_D3_TEST_SHIFT 3
#define IMX_USDHC_VEND_SPEC2_CARD_INT_D3_TEST_MASK  0x8
#define IMX_USDHC_VEND_SPEC2_CARD_INT_D3_TEST(v)    (((v) << 0x3) & 0x8)
/* Field TUNING_8bit_EN -
 * Enable the auto tuning circuit to check the DATA[7:0]. */
#define IMX_USDHC_VEND_SPEC2_TUNING_8bit_EN_SHIFT 4
#define IMX_USDHC_VEND_SPEC2_TUNING_8bit_EN_MASK  0x10
#define IMX_USDHC_VEND_SPEC2_TUNING_8bit_EN(v)    (((v) << 0x4) & 0x10)
/* Field TUNING_1bit_EN -
 * Enable the auto tuning circuit to check the DATA0 only */
#define IMX_USDHC_VEND_SPEC2_TUNING_1bit_EN_SHIFT 5
#define IMX_USDHC_VEND_SPEC2_TUNING_1bit_EN_MASK  0x20
#define IMX_USDHC_VEND_SPEC2_TUNING_1bit_EN(v)    (((v) << 0x5) & 0x20)
/* Field TUNING_CMD_EN - Enable the auto tuning circuit to check the CMD line */
#define IMX_USDHC_VEND_SPEC2_TUNING_CMD_EN_SHIFT 6
#define IMX_USDHC_VEND_SPEC2_TUNING_CMD_EN_MASK  0x40
#define IMX_USDHC_VEND_SPEC2_TUNING_CMD_EN(v)    (((v) << 0x6) & 0x40)
/* Field CARD_INT_AUTO_CLR_DIS - Only for debug */
#define IMX_USDHC_VEND_SPEC2_CARD_INT_AUTO_CLR_DIS_SHIFT 7
#define IMX_USDHC_VEND_SPEC2_CARD_INT_AUTO_CLR_DIS_MASK  0x80
#define IMX_USDHC_VEND_SPEC2_CARD_INT_AUTO_CLR_DIS(v)    (((v) << 0x7) & 0x80)
/* Field HS400_WR_CLK_STOP_EN - HS400 Write Clock Stop Enable */
#define IMX_USDHC_VEND_SPEC2_HS400_WR_CLK_STOP_EN_SHIFT 10
#define IMX_USDHC_VEND_SPEC2_HS400_WR_CLK_STOP_EN_MASK  0x400
#define IMX_USDHC_VEND_SPEC2_HS400_WR_CLK_STOP_EN(v)    (((v) << 0xA) & 0x400)
/* Field HS400_RD_CLK_STOP_EN - HS400 Read Clock Stop Enable */
#define IMX_USDHC_VEND_SPEC2_HS400_RD_CLK_STOP_EN_SHIFT 11
#define IMX_USDHC_VEND_SPEC2_HS400_RD_CLK_STOP_EN_MASK  0x800
#define IMX_USDHC_VEND_SPEC2_HS400_RD_CLK_STOP_EN(v)    (((v) << 0xB) & 0x800)



/* Register TUNING_CTRL - Tuning Control Register */
#define IMX_USDHC_TUNING_CTRL                   0xCC
/* Field TUNING_START_TAP -
 * The start delay cell point when send first CMD19 in tuning procedure */
#define IMX_USDHC_TUNING_CTRL_TUNING_START_TAP_SHIFT 0
#define IMX_USDHC_TUNING_CTRL_TUNING_START_TAP_MASK  0xFF
#define IMX_USDHC_TUNING_CTRL_TUNING_START_TAP(v)    (((v) << 0) & 0xFF)
/* Field TUNING_COUNTER - The MAX repeat CMD19 times in tuning procedure */
#define IMX_USDHC_TUNING_CTRL_TUNING_COUNTER_SHIFT 8
#define IMX_USDHC_TUNING_CTRL_TUNING_COUNTER_MASK  0xFF00
#define IMX_USDHC_TUNING_CTRL_TUNING_COUNTER(v)    (((v) << 0x8) & 0xFF00)
/* Field TUNING_STEP - The increasing delay cell steps in tuning procedure */
#define IMX_USDHC_TUNING_CTRL_TUNING_STEP_SHIFT 16
#define IMX_USDHC_TUNING_CTRL_TUNING_STEP_MASK  0x70000
#define IMX_USDHC_TUNING_CTRL_TUNING_STEP(v)    (((v) << 0x10) & 0x70000)
/* Field TUNING_WINDOW - Select data window value for auto tuning */
#define IMX_USDHC_TUNING_CTRL_TUNING_WINDOW_SHIFT 20
#define IMX_USDHC_TUNING_CTRL_TUNING_WINDOW_MASK  0x700000
#define IMX_USDHC_TUNING_CTRL_TUNING_WINDOW(v)    (((v) << 0x14) & 0x700000)
/* Field STD_TUNING_EN - Standard tuning circuit and procedure enable */
#define IMX_USDHC_TUNING_CTRL_STD_TUNING_EN_SHIFT 24
#define IMX_USDHC_TUNING_CTRL_STD_TUNING_EN_MASK  0x1000000
#define IMX_USDHC_TUNING_CTRL_STD_TUNING_EN(v)    (((v) << 0x18) & 0x1000000)



/* Register HCTL */
#define IMX_USDHC_HCTL_VERSION                  0xFC
/* Host controller version mask*/
#define IMX_USDHC_HCTL_SPEC_VER_MASK            0xFF
/* Host controller version */
#define IMX_USDHC_HCTL_SPEC_VER_3               0x2
#define IMX_USDHC_HCTL_SPEC_VER_1               0x0



#endif /* IMX_USDHC_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/aarch64/imx8/imx8_usdhc.h $ $Rev: 842080 $")
#endif
