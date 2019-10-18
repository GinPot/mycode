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

#ifndef IMX_PCIE_H_
#define IMX_PCIE_H_

/* PCIe CTRL RC Mode, offset from base address */
#define IMX_PCIE_RC_DeviceID            0x000     /* Device ID and Vendor ID Register */
#define IMX_PCIE_RC_Command             0x004     /* Command and Status Register */
#define IMX_PCIE_RC_RevID               0x008     /* Revision ID and Class Code Register */
#define IMX_PCIE_RC_BIST                0x00C     /* BIST Register */
#define IMX_PCIE_RC_BAR0                0x010     /* Base Address 0 */
#define IMX_PCIE_RC_BAR1                0x014     /* Base Address 1 */
#define IMX_PCIE_RC_BNR                 0x018     /* Bus Number Registers */
#define IMX_PCIE_RC_IOBLSSR             0x01C     /* I/O Base Limit Secondary Status Register */
#define IMX_PCIE_RC_MEM_BLR             0x020     /* Memory Base and Memory Limit Register */
#define IMX_PCIE_RC_PREF_MEM_BLR        0x024     /* Prefetchable Memory Base and Limit Register */
#define IMX_PCIE_RC_PREF_BASE_U32       0x028     /* Prefetchable Base Upper 32 Bits Register */
#define IMX_PCIE_RC_PREF_LIM_U32        0x02C     /* Prefetchable Limit Upper 32 Bits Register */
#define IMX_PCIE_RC_IO_BASE_LIM_U16     0x030     /* I/O Base and Limit Upper 16 Bits Register */
#define IMX_PCIE_RC_CAPPR               0x034     /* Capability Pointer Register */
#define IMX_PCIE_RC_EROMBAR             0x038     /* Expansion ROM Base Address Register */
#define IMX_PCIE_RC_EROMMASK            0x038     /* Expansion ROM BAR Mask Register */
#define IMX_PCIE_RC_PMCR                0x040     /* Power Management Capability Register */
#define IMX_PCIE_RC_PMCSR               0x044     /* Power Management Control and Status Register */
#define IMX_PCIE_RC_CIDR                0x070     /* PCI Express Capability ID Register */
#define IMX_PCIE_RC_DCR                 0x074     /* Device Capabilities Register */
#define IMX_PCIE_RC_DConR               0x078     /* Device Control Register */
#define IMX_PCIE_RC_LCR                 0x07C     /* Link Capabilities Register */
#define IMX_PCIE_RC_LCSR                0x080     /* Link Control and Status Register */
#define IMX_PCIE_RC_SCR                 0x084     /* Slot Capabilities Register */
#define IMX_PCIE_RC_SCSR                0x088     /* Slot Control and Status Register */
#define IMX_PCIE_RC_RCCR                0x08C     /* Root Control and Capabilities Register */
#define IMX_PCIE_RC_RSR                 0x090     /* Root Status Register */
#define IMX_PCIE_RC_DCR2                0x094     /* Device Capabilities 2 Register */
#define IMX_PCIE_RC_DCSR2               0x098     /* Device Control and Status 2 Register */
#define IMX_PCIE_RC_LCR2                0x09C     /* Link Capabilities 2 Register */
#define IMX_PCIE_RC_LCSR2               0x0A0     /* Link Control and Status 2 Register */
#define IMX_PCIE_RC_AER                 0x100     /* AER Capability Header */
#define IMX_PCIE_RC_UESR                0x104     /* Uncorrectable Error Status Register */
#define IMX_PCIE_RC_UEMR                0x108     /* Uncorrectable Error Mask Register */
#define IMX_PCIE_RC_UESevR              0x10C     /* Uncorrectable Error Severity Register */
#define IMX_PCIE_RC_CESR                0x110     /* Correctable Error Status Register */
#define IMX_PCIE_RC_CEMR                0x114     /* Correctable Error Mask Register */
#define IMX_PCIE_RC_ACCR                0x118     /* Advanced Capabilities and Control Register */
#define IMX_PCIE_RC_HLR                 0x11C     /* Header Log Register */
#define IMX_PCIE_RC_RECR                0x12C     /* Root Error Command Register */
#define IMX_PCIE_RC_RESR                0x130     /* Root Error Status Register */
#define IMX_PCIE_RC_ESIR                0x134     /* Error Source Identification Register */
#define IMX_PCIE_RC_VCECHR              0x140     /* VC Extended Capability Header */
#define IMX_PCIE_RC_PVCCR1              0x144     /* Port VC Capability Register 1 */
#define IMX_PCIE_RC_PVCCR2              0x148     /* Port VC Capability Register 2 */
#define IMX_PCIE_RC_PVCCSR              0x14C     /* Port VC Control and Status Register */
#define IMX_PCIE_RC_VCRCR               0x150     /* VC Resource Capability Register n */
#define IMX_PCIE_RC_VCRConR             0x154     /* VC Resource Control Register n */
#define IMX_PCIE_RC_VCRSR               0x158     /* VC Resource Status Register n */

/* PCIe CTRL Port Logic, offset from base address */
#define IMX_PCIE_PL_ALTRTR              0x700     /* Ack Latency Timer and Replay Timer Register */
#define IMX_PCIE_PL_VSDR                0x704     /* Vendor Specific DLLP Register */
#define IMX_PCIE_PL_PFLR                0x708     /* Port Force Link Register */
#define IMX_PCIE_PL_AFLACR              0x70C     /* Ack Frequency and L0-L1 ASPM Control Register */
#define IMX_PCIE_PL_PLCR                0x710     /* Port Link Control Register */
#define IMX_PCIE_PL_LSR                 0x714     /* Lane Skew Register */
#define IMX_PCIE_PL_SNR                 0x718     /* Symbol Number Register */
#define IMX_PCIE_PL_STRFM1              0x71C     /* Symbol Timer Register and Filter Mask Register 1 */
#define IMX_PCIE_PL_STRFM2              0x720     /* Filter Mask Register 2 */
#define IMX_PCIE_PL_AMODNPSR            0x724     /* AMBA Multiple Outbound Decomposed NP Sub-Requests Control Register */
#define IMX_PCIE_PL_DEBUG0              0x728     /* Debug Register 0 */
#define IMX_PCIE_PL_DEBUG1              0x72C     /* Debug Register 1 */
#define IMX_PCIE_PL_TPFCSR              0x730     /* Transmit Posted FC Credit Status Register */
#define IMX_PCIE_PL_TNFCSR              0x734     /* Transmit Non-Posted FC Credit Status Register */
#define IMX_PCIE_PL_TCFCSR              0x738     /* Transmit Completion FC Credit Status Register */
#define IMX_PCIE_PL_QSR                 0x73C     /* Queue Status Register */
#define IMX_PCIE_PL_VCTAR1              0x740     /* VC Transmit Arbitration Register 1 */
#define IMX_PCIE_PL_VCTAR2              0x744     /* VC Transmit Arbitration Register 2 */
#define IMX_PCIE_PL_VC0PRQC             0x748     /* VC0 Posted Receive Queue Control */
#define IMX_PCIE_PL_VC0NRQC             0x74C     /* VC0 Non-Posted Recleive Queue Control */
#define IMX_PCIE_PL_VC0CRQC             0x750     /* VC0 Completion Receive Queue Control */
#define IMX_PCIE_PL_VCnPRQC             0x754     /* VCn Posted Receive Queue Control */
#define IMX_PCIE_PL_VCnNRQC             0x758     /* VCn Non-Posted Receive Queue Control */
#define IMX_PCIE_PL_VCnCRQC             0x75C     /* VCn Completion Receive Queue Control */
#define IMX_PCIE_PL_VC0PBD              0x7A8     /* VC0 Posted Buffer Depth */
#define IMX_PCIE_PL_VC0NPBD             0x7AC     /* VC0 Non-Posted Buffer Depth */
#define IMX_PCIE_PL_VC0CBD              0x7B0     /* VC0 Completion Buffer Depth */
#define IMX_PCIE_PL_VCnPBD              0x7B4     /* VCn Posted Buffer Depth */
#define IMX_PCIE_PL_VCnNPBD             0x7B8     /* VCn Non-Posted Buffer Depth */
#define IMX_PCIE_PL_VCnCBD              0x7BC     /* VCn Completion Buffer Depth */
#define IMX_PCIE_PL_G2CR                0x80C     /* Gen2 Control Register */
#define IMX_PCIE_PL_PHY_STATUS          0x810     /* PHY Status */
#define IMX_PCIE_PL_PHY_CTRL            0x814     /* PHY Control */
#define IMX_PCIE_PL_MRCCR0              0x818     /* Master Response Composer Control Register 0 */
#define IMX_PCIE_PL_MRCCR1              0x81C     /* Master Response Composer Control Register 1 */
#define IMX_PCIE_PL_MSICA               0x820     /* MSI Controller Address */
#define IMX_PCIE_PL_MSICUA              0x824     /* MSI Controller Upper Address */
#define IMX_PCIE_PL_MSICIn_ENB          0x828     /* MSI Controller Interrupt n Enable */
#define IMX_PCIE_PL_MSICIn_MASK         0x82C     /* MSI Controller Interrupt n Mask */
#define IMX_PCIE_PL_MSICIn_STATUS       0x830     /* MSI Controller Interrupt n Status */
#define IMX_PCIE_PL_MSICGPIO            0x888     /* MSI Controller General Purpose IO Register */
#define IMX_PCIE_PL_MISC_CTRL1_OFF      0x8BC     /* DBI Read-Only Write Enable Register. */
#define IMX_PCIE_PL_iATUVR              0x900     /* iATU Viewport Register */
#define IMX_PCIE_PL_iATURC1             0x904     /* iATU Region Control 1 Register */
#define IMX_PCIE_PL_iATURC2             0x908     /* iATU Region Control 2 Register */
#define IMX_PCIE_PL_iATURLBA            0x90C     /* iATU Region Lower Base Address Register */
#define IMX_PCIE_PL_iATURUBA            0x910     /* iATU Region Upper Base Address Register */
#define IMX_PCIE_PL_iATURLA             0x914     /* iATU Region Limit Address Register */
#define IMX_PCIE_PL_iATURLTA            0x918     /* iATU Region Lower Target Address Register */
#define IMX_PCIE_PL_iATURUTA            0x91C     /* iATU Region Upper Target Address Register */
#define IMX_PCIE_PL_iATURULA            0x924     /* iATU Region Limit Upper Address Register */

/*
 * Control Register 2 bits
 */
#define IMX_PCIE_PL_DEBUG1_LINK_UP                  (0x01 << 4)      /* xmlh_link_up LTSSM reports PHY link up */

/*
 * iATU Viewport Register
 */
#define IMX_PCIE_PL_iATUVR_Region_Direction_MASK    (0x01 << 31)    /* Region Direction mask */
#define IMX_PCIE_PL_iATUVR_Region_Direction_SHIFT   31              /* Region Direction bit offset */

#define IMX_PCIE_PL_iATUVR_Region_Index_MASK        (0x0F << 0)     /* Region Index mask */
#define IMX_PCIE_PL_iATUVR_Region_Index_SHIFT       0               /* Region Index group offset*/

/*
 * iATU Region Control 1 Register
 */
#define IMX_PCIE_PL_iATURC1_INC_REGION_SIZE_MASK    (0x01 << 13)     /* Increase Region Size mask */
#define IMX_PCIE_PL_iATURC1_INC_REGION_SIZE_SHIFT   13               /* Increase Region Size bit offset*/

#endif /* IMX_PCIE_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/aarch64/imx8/imx8_pcie.h $ $Rev: 837679 $")
#endif
