/*
 * $QNXLicenseC:
 * Copyright 2016, Freescale Semiconductor, Inc.
 * Copyright 2017 NXP
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

#ifndef IMX_FLEXSPI_H_
#define IMX_FLEXSPI_H_

/* Register MCR0 - Module Configuration Register 0 */
#define IMX_FLEXSPI_MCR0                            0x0
/* Field AHBGRANTWAIT - Time out wait cycle for AHB command grant */
#define IMX_FLEXSPI_MCR0_AHBGRANTWAIT_SHIFT         24
#define IMX_FLEXSPI_MCR0_AHBGRANTWAIT_MASK          (0xFF << 24)
/* Field SWRSTHD - Time out wait cycle for IP command grant */
#define IMX_FLEXSPI_MCR0_IPGRANTWAIT_SHIFT          16
#define IMX_FLEXSPI_MCR0_IPGRANTWAIT_MASK           (0xFF << 16)
/* Field LEARNEN - Data learning feature */
#define IMX_FLEXSPI_MCR0_LEARNEN_SHIFT              15
#define IMX_FLEXSPI_MCR0_LEARNEN_MASK               (0x1 << 15)
/* Field SCKFREERUNEN - SCK output free-running */
#define IMX_FLEXSPI_MCR0_SCKFREERUNEN_SHIFT         14
#define IMX_FLEXSPI_MCR0_SCKFREERUNEN_MASK          (0x1 << 14)
/* Field COMBINATIONEN - Flash Octal mode */
#define IMX_FLEXSPI_MCR0_COMBINATIONEN_SHIFT        13
#define IMX_FLEXSPI_MCR0_COMBINATIONEN_MASK         (0x1 << 13)
/* Field DOZEEN - Doze mode */
#define IMX_FLEXSPI_MCR0_DOZEEN_SHIFT               12
#define IMX_FLEXSPI_MCR0_DOZEEN_MASK                (0x1 << 12)
/* Field HSEN - Half Speed Serial Flash access */
#define IMX_FLEXSPI_MCR0_HSEN_SHIFT                 11
#define IMX_FLEXSPI_MCR0_HSEN_MASK                  (0x1 << 11)
/* Field SERCLKDIV - Root clock divider */
#define IMX_FLEXSPI_MCR0_SERCLKDIV_SHIFT            8
#define IMX_FLEXSPI_MCR0_SERCLKDIV_MASK             (0x7 << 8)
/* Field ATDFEN - Enable AHB bus Write Access to IP TX FIFO */
#define IMX_FLEXSPI_MCR0_ATDFEN_SHIFT               7
#define IMX_FLEXSPI_MCR0_ATDFEN_MASK                (0x1 << 7)
/* Field ARDFEN - Enable AHB bus Read Access to IP RX FIFO */
#define IMX_FLEXSPI_MCR0_ARDFEN_SHIFT               6
#define IMX_FLEXSPI_MCR0_ARDFEN_MASK                (0x1 << 6)
/* Field RXCLKSRC - Sample Clock source selection for Flash Reading */
#define IMX_FLEXSPI_MCR0_RXCLKSRC_SHIFT             4
#define IMX_FLEXSPI_MCR0_RXCLKSRC_MASK              (0x3 << 4)
/* Field MDIS - Module Disable */
#define IMX_FLEXSPI_MCR0_MDIS_SHIFT                 1
#define IMX_FLEXSPI_MCR0_MDIS_MASK                  (0x1 << 1)
/* Field RESET - Software Reset */
#define IMX_FLEXSPI_MCR0_SWRESET_SHIFT              0
#define IMX_FLEXSPI_MCR0_SWRESET_MASK               (0x1 << 0)

/* Register MCR1 - Module Configuration Register 1 */
#define IMX_FLEXSPI_MCR1                            0x4
/* Field SEQWAIT - Command Sequence Execution timeout */
#define IMX_FLEXSPI_MCR1_SEQWAIT_SHIFT              16
#define IMX_FLEXSPI_MCR1_SEQWAIT_MASK               (0xFFFF << 16)
/* Field AHBBUSWAIT - AHB Read/Write access to Serial Flash Memory timeout */
#define IMX_FLEXSPI_MCR1_AHBBUSWAIT_SHIFT           0
#define IMX_FLEXSPI_MCR1_AHBBUSWAIT_MASK            (0xFFFF << 0)

/* Register MCR2 - Module Configuration Register 2 */
#define IMX_FLEXSPI_MCR2                            0x8
/* Field RESUMEWAIT - Wait cycle (in AHB clock cycle) - idle state before suspended command resumed */
#define IMX_FLEXSPI_MCR2_RESUMEWAIT_SHIFT           24
#define IMX_FLEXSPI_MCR2_RESUMEWAIT_MASK            (0xFF << 24)
/* Field SCKBDIFFOPT - SCKB pad can be used as SCKA differential clock output (inverted clock to SCKA) */
#define IMX_FLEXSPI_MCR2_SCKBDIFFOPT_SHIFT          19
#define IMX_FLEXSPI_MCR2_SCKBDIFFOPT_MASK           (0x1 << 19)
/* Field SAMEDEVICEEN - All external devices are same devices (both in types and size) for A1/A2/B1/B2 */
#define IMX_FLEXSPI_MCR2_SAMEDEVICEEN_SHIFT         15
#define IMX_FLEXSPI_MCR2_SAMEDEVICEEN_MASK          (0x1 << 15)
/* Field CLRLEARNPHASE - Sampling clock phase selection reset */
#define IMX_FLEXSPI_MCR2_CLRLEARNPHASE_SHIFT        14
#define IMX_FLEXSPI_MCR2_CLRLEARNPHASE_MASK         (0x1 << 14)
/* Field CLRAHBBUFOPT - AHB buffers will be cleaned when FlexSPI return from STOP mode */
#define IMX_FLEXSPI_MCR2_CLRAHBBUFOPT_SHIFT         11
#define IMX_FLEXSPI_MCR2_CLRAHBBUFOPT_MASK          (0x1 << 11)

/* Register AHBCR - AHB Bus Control Register */
#define IMX_FLEXSPI_AHBCR                           0xC
/* Field READADDROPT - AHB burst start address alignment */
#define IMX_FLEXSPI_AHBCR_READADDROPT_SHIFT         6
#define IMX_FLEXSPI_AHBCR_READADDROPT_MASK          (0x1 << 6)
/* Field PREFETCHEN - AHB Read Prefetch Enable */
#define IMX_FLEXSPI_AHBCR_PREFETCHEN_SHIFT          5
#define IMX_FLEXSPI_AHBCR_PREFETCHEN_MASK           (0x1 << 5)
/* Field BUFFERABLEEN - Enable AHB bus buffer-able write access support */
#define IMX_FLEXSPI_AHBCR_BUFFERABLEEN_SHIFT        4
#define IMX_FLEXSPI_AHBCR_BUFFERABLEEN_MASK         (0x1 << 4)
/* Field CACHABLEEN - Enable AHB bus cache-able read access support */
#define IMX_FLEXSPI_AHBCR_CACHABLEEN_SHIFT          3
#define IMX_FLEXSPI_AHBCR_CACHABLEEN_MASK           (0x1 << 3)
/* Field APAREN - Parallel mode enabled for AHB triggered Command (both read and write) */
#define IMX_FLEXSPI_AHBCR_APAREN_SHIFT              0
#define IMX_FLEXSPI_AHBCR_APAREN_MASK               (0x1 << 0)

/* Register INTEN - Interrupt Enable Register */
#define IMX_FLEXSPI_INTEN                           0x10
/* Field SEQTIMEOUTEN - Sequence execution timeout interrupt enable */
#define IMX_FLEXSPI_INTEN_SEQTIMEOUTEN_SHIFT        11
#define IMX_FLEXSPI_INTEN_SEQTIMEOUTEN_MASK         (0x1 << 11)
/* Field AHBBUSTIMEOUTEN - AHB Bus timeout interrupt enable */
#define IMX_FLEXSPI_INTEN_AHBBUSTIMEOUTEN_SHIFT     10
#define IMX_FLEXSPI_INTEN_AHBBUSTIMEOUTEN_MASK      (0x1 << 10)
/* Field SCKSTOPBYWREN - SCK is stopped during command sequence because Asynchronous TX FIFO empty interrupt enable */
#define IMX_FLEXSPI_INTEN_SCKSTOPBYWREN_SHIFT       9
#define IMX_FLEXSPI_INTEN_SCKSTOPBYWREN_MASK        (0x1 << 9)
/* Field SCKSTOPBYRDEN - SCK is stopped during command sequence because Asynchronous RX FIFO full interrupt enable */
#define IMX_FLEXSPI_INTEN_SCKSTOPBYRDEN_SHIFT       8
#define IMX_FLEXSPI_INTEN_SCKSTOPBYRDEN_MASK        (0x1 << 8)
/* Field DATALEARNFAILEN - Data Learning failed interrupt enable */
#define IMX_FLEXSPI_INTEN_DATALEARNFAILEN_SHIFT     7
#define IMX_FLEXSPI_INTEN_DATALEARNFAILEN_MASK      (0x1 << 7)
/* Field IPTXWEEN - IP TX FIFO */
#define IMX_FLEXSPI_INTEN_IPTXWEEN_SHIFT            6
#define IMX_FLEXSPI_INTEN_IPTXWEEN_MASK             (0x1 << 6)
/* Field IPRXWAEN - IP RX FIFO */
#define IMX_FLEXSPI_INTEN_IPRXWAEN_SHIFT            5
#define IMX_FLEXSPI_INTEN_IPRXWAEN_MASK             (0x1 << 5)
/* Field AHBCMDERREN - AHB triggered Command Sequences Error Detected interrupt enable */
#define IMX_FLEXSPI_INTEN_AHBCMDERREN_SHIFT         4
#define IMX_FLEXSPI_INTEN_AHBCMDERREN_MASK          (0x1 << 4)
/* Field IPCMDERREN - IP triggered Command Sequences Error Detected interrupt enable */
#define IMX_FLEXSPI_INTEN_IPCMDERREN_SHIFT          3
#define IMX_FLEXSPI_INTEN_IPCMDERREN_MASK           (0x1 << 3)
/* Field AHBCMDGEEN - AHB triggered Command Sequences Grant Timeout interrupt enable */
#define IMX_FLEXSPI_INTEN_AHBCMDGEEN_SHIFT          2
#define IMX_FLEXSPI_INTEN_AHBCMDGEEN_MASK           (0x1 << 2)
/* Field IPCMDGEEN - IP triggered Command Sequences Grant Timeout interrupt enable */
#define IMX_FLEXSPI_INTEN_IPCMDGEEN_SHIFT           1
#define IMX_FLEXSPI_INTEN_IPCMDGEEN_MASK            (0x1 << 1)
/* Field IPCMDDONEEN - IP triggered Command Sequences Execution finished interrupt enable. */
#define IMX_FLEXSPI_INTEN_IPCMDDONEEN_SHIFT         0
#define IMX_FLEXSPI_INTEN_IPCMDDONEEN_MASK          (0x1 << 0)

/* Register INTR - Interrupt Register */
#define IMX_FLEXSPI_INTR                            0x14
/* Field SEQTIMEOUT - Sequence execution timeout interrupt enable */
#define IMX_FLEXSPI_INTR_SEQTIMEOUT_SHIFT           11
#define IMX_FLEXSPI_INTR_SEQTIMEOUT_MASK            (0x1 << 11)
/* Field AHBBUSTIMEOUT - AHB Bus timeout interrupt */
#define IMX_FLEXSPI_INTR_AHBBUSTIMEOUT_SHIFT        10
#define IMX_FLEXSPI_INTR_AHBBUSTIMEOUT_MASK         (0x1 << 10)
/* Field SCKSTOPBYWR - SCK is stopped during command sequence because Asynchronous TX FIFO empty interrupt */
#define IMX_FLEXSPI_INTR_SCKSTOPBYWR_SHIFT          9
#define IMX_FLEXSPI_INTR_SCKSTOPBYWR_MASK           (0x1 << 9)
/* Field SCKSTOPBYRD - SCK is stopped during command sequence because Asynchronous RX FIFO full interrupt */
#define IMX_FLEXSPI_INTR_SCKSTOPBYRD_SHIFT          8
#define IMX_FLEXSPI_INTR_SCKSTOPBYRD_MASK           (0x1 << 8)
/* Field DATALEARNFAIL - Data Learning failed interrupt */
#define IMX_FLEXSPI_INTR_DATALEARNFAIL_SHIFT        7
#define IMX_FLEXSPI_INTR_DATALEARNFAIL_MASK         (0x1 << 7)
/* Field IPTXWE - IP TX FIFO */
#define IMX_FLEXSPI_INTR_IPTXWE_SHIFT               6
#define IMX_FLEXSPI_INTR_IPTXWE_MASK                (0x1 << 6)
/* Field IPRXWA - IP RX FIFO */
#define IMX_FLEXSPI_INTR_IPRXWA_SHIFT               5
#define IMX_FLEXSPI_INTR_IPRXWA_MASK                (0x1 << 5)
/* Field AHBCMDERR - AHB triggered Command Sequences Error Detected interrupt */
#define IMX_FLEXSPI_INTR_AHBCMDERR_SHIFT            4
#define IMX_FLEXSPI_INTR_AHBCMDERR_MASK             (0x1 << 4)
/* Field IPCMDERR - IP triggered Command Sequences Error Detected interrupt */
#define IMX_FLEXSPI_INTR_IPCMDERR_SHIFT             3
#define IMX_FLEXSPI_INTR_IPCMDERR_MASK              (0x1 << 3)
/* Field AHBCMDGE - AHB triggered Command Sequences Grant Timeout interrupt */
#define IMX_FLEXSPI_INTR_AHBCMDGE_SHIFT             2
#define IMX_FLEXSPI_INTR_AHBCMDGE_MASK              (0x1 << 2)
/* Field IPCMDGE - IP triggered Command Sequences Grant Timeout interrupt */
#define IMX_FLEXSPI_INTR_IPCMDGE_SHIFT              1
#define IMX_FLEXSPI_INTR_IPCMDGE_MASK               (0x1 << 1)
/* Field IPCMDDONE - IP triggered Command Sequences Execution finished interrupt */
#define IMX_FLEXSPI_INTR_IPCMDDONE_SHIFT            0
#define IMX_FLEXSPI_INTR_IPCMDDONE_MASK             (0x1 << 0)

/* Register LUTKEY - LUT Key Register */
#define IMX_FLEXSPI_LUTKEY                          0x18
#define IMX_FLEXSPI_LUT_KEY_VAL                     0x5AF05AF0U
/* Field KEY - The key is 0x5AF05AF0 */
#define IMX_FLEXSPI_LUTKEY_KEY_SHIFT                0
#define IMX_FLEXSPI_LUTKEY_KEY_MASK                 (0xFFFFFFFF << 0)

/* Register LUTCR - LUT Control Register */
#define IMX_FLEXSPI_LUTCR                           0x1C
/* Field UNLOCK - Unlock LUT */
#define IMX_FLEXSPI_LUTCR_UNLOCK_SHIFT              1
#define IMX_FLEXSPI_LUTCR_UNLOCK_MASK               (0x1 << 1)
/* Field LOCK - Lock LUT */
#define IMX_FLEXSPI_LUTCR_LOCK_SHIFT                0
#define IMX_FLEXSPI_LUTCR_LOCK_MASK                 (0x1 << 0)

/* Register AHBRXBUFaCR0 - AHB RX Buffer a Control Register 0 */
#define IMX_FLEXSPI_AHBRXBUFaCR0_NUM                8
#define IMX_FLEXSPI_AHBRXBUF0CR0                    0x20
#define IMX_FLEXSPI_AHBRXBUFaCR0(index)             (IMX_FLEXSPI_AHBRXBUF0CR0 + (index * 4))
/* Field PRIORITY - This priority for AHB Master Read which this AHB RX Buffer is assigned */
#define IMX_FLEXSPI_AHBRXBUFaCR0_PRIORITY_SHIFT     24
#define IMX_FLEXSPI_AHBRXBUFaCR0_PRIORITY_MASK      (0x7 << 24)
/* Field MSTRID - This AHB RX Buffer is assigned according to AHB Master with ID (MSTR_ID) */
#define IMX_FLEXSPI_AHBRXBUFaCR0_MSTRID_SHIFT       16
#define IMX_FLEXSPI_AHBRXBUFaCR0_MSTRID_MASK        (0xF << 16)
/* Field BUFSZ - AHB RX Buffer Size in 64 bits */
#define IMX_FLEXSPI_AHBRXBUFaCR0_BUFSZ_SHIFT        0
#define IMX_FLEXSPI_AHBRXBUFaCR0_BUFSZ_MASK         (0x1FF << 0)

/* Register FLSHA1CR0 - Flash A1 Control Register 0 */
#define IMX_FLEXSPI_FLSHA1CR0                       0x60
/* Field FLSHSZ - Flash Size in KByte */
#define IMX_FLEXSPI_FLSHA1CR0_FLSHSZ_SHIFT          0
#define IMX_FLEXSPI_FLSHA1CR0_FLSHSZ_MASK           (0x7FFFFF << 0)

/* Register FLSHA2CR0 - Flash A2 Control Register 0 */
#define IMX_FLEXSPI_FLSHA2CR0                       0x64
/* Field FLSHSZ - Flash Size in KByte */
#define IMX_FLEXSPI_FLSHA2CR0_FLSHSZ_SHIFT          0
#define IMX_FLEXSPI_FLSHA2CR0_FLSHSZ_MASK           (0x7FFFFF << 0)

/* Register FLSHB1CR0 - Flash B1 Control Register 0 */
#define IMX_FLEXSPI_FLSHB1CR0                       0x68
/* Field FLSHSZ - Flash Size in KByte */
#define IMX_FLEXSPI_FLSHB1CR0_FLSHSZ_SHIFT          0
#define IMX_FLEXSPI_FLSHB1CR0_FLSHSZ_MASK           (0x7FFFFF << 0)

/* Register FLSHB2CR0 - Flash B2 Control Register 0 */
#define IMX_FLEXSPI_FLSHB2CR0                       0x6C
/* Field FLSHSZ - Flash Size in KByte */
#define IMX_FLEXSPI_FLSHB2CR0_FLSHSZ_SHIFT          0
#define IMX_FLEXSPI_FLSHB2CR0_FLSHSZ_MASK           (0x7FFFFF << 0)

/* Register FLSHA1CR1 - Flash A1 Control Register 1 */
#define IMX_FLEXSPI_FLSHA1CR1                       0x70
/* Field CSINTERVAL - CS interval */
#define IMX_FLEXSPI_FLSHA1CR1_CSINTERVAL_SHIFT      16
#define IMX_FLEXSPI_FLSHA1CR1_CSINTERVAL_MASK       (0xFFFF << 16)
/* Field CSINTERVALUNIT - CS interval unit */
#define IMX_FLEXSPI_FLSHA1CR1_CSINTERVALUNIT_SHIFT  15
#define IMX_FLEXSPI_FLSHA1CR1_CSINTERVALUNIT_MASK   (0x1 << 15)
/* Field CAS - Column Address Size */
#define IMX_FLEXSPI_FLSHA1CR1_CAS_SHIFT             11
#define IMX_FLEXSPI_FLSHA1CR1_CAS_MASK              (0xF << 11)
/* Field WA - Word Addressable */
#define IMX_FLEXSPI_FLSHA1CR1_WA_SHIFT              10
#define IMX_FLEXSPI_FLSHA1CR1_WA_MASK               (0x1 << 10)
/* Field TCSH - Serial Flash CS Hold time */
#define IMX_FLEXSPI_FLSHA1CR1_TCSH_SHIFT            5
#define IMX_FLEXSPI_FLSHA1CR1_TCSH_MASK             (0x1F << 5)
/* Field TCSS - Serial Flash CS setup time */
#define IMX_FLEXSPI_FLSHA1CR1_TCSS_SHIFT            0
#define IMX_FLEXSPI_FLSHA1CR1_TCSS_MASK             (0x1F << 0)

/* Register FLSHA2CR1 - Flash A2 Control Register 1 */
#define IMX_FLEXSPI_FLSHA2CR1                       0x74
/* Field CSINTERVAL - CS interval */
#define IMX_FLEXSPI_FLSHA2CR1_CSINTERVAL_SHIFT      16
#define IMX_FLEXSPI_FLSHA2CR1_CSINTERVAL_MASK       (0xFFFF << 16)
/* Field CSINTERVALUNIT - CS interval unit */
#define IMX_FLEXSPI_FLSHA2CR1_CSINTERVALUNIT_SHIFT  15
#define IMX_FLEXSPI_FLSHA2CR1_CSINTERVALUNIT_MASK   (0x1 << 15)
/* Field CAS - Column Address Size */
#define IMX_FLEXSPI_FLSHA2CR1_CAS_SHIFT             11
#define IMX_FLEXSPI_FLSHA2CR1_CAS_MASK              (0xF << 11)
/* Field WA - Word Addressable */
#define IMX_FLEXSPI_FLSHA2CR1_WA_SHIFT              10
#define IMX_FLEXSPI_FLSHA2CR1_WA_MASK               (0x1 << 10)
/* Field TCSH - Serial Flash CS Hold time */
#define IMX_FLEXSPI_FLSHA2CR1_TCSH_SHIFT            5
#define IMX_FLEXSPI_FLSHA2CR1_TCSH_MASK             (0x1F << 5)
/* Field TCSS - Serial Flash CS setup time */
#define IMX_FLEXSPI_FLSHA2CR1_TCSS_SHIFT            0
#define IMX_FLEXSPI_FLSHA2CR1_TCSS_MASK             (0x1F << 0)

/* Register FLSHB1CR1 - Flash B1 Control Register 1 */
#define IMX_FLEXSPI_FLSHB1CR1                       0x78
/* Field CSINTERVAL - CS interval */
#define IMX_FLEXSPI_FLSHB1CR1_CSINTERVAL_SHIFT      16
#define IMX_FLEXSPI_FLSHB1CR1_CSINTERVAL_MASK       (0xFFFF << 16)
/* Field CSINTERVALUNIT - CS interval unit */
#define IMX_FLEXSPI_FLSHB1CR1_CSINTERVALUNIT_SHIFT  15
#define IMX_FLEXSPI_FLSHB1CR1_CSINTERVALUNIT_MASK   (0x1 << 15)
/* Field CAS - Column Address Size */
#define IMX_FLEXSPI_FLSHB1CR1_CAS_SHIFT             11
#define IMX_FLEXSPI_FLSHB1CR1_CAS_MASK              (0xF << 11)
/* Field WA - Word Addressable */
#define IMX_FLEXSPI_FLSHB1CR1_WA_SHIFT              10
#define IMX_FLEXSPI_FLSHB1CR1_WA_MASK               (0x1 << 10)
/* Field TCSH - Serial Flash CS Hold time */
#define IMX_FLEXSPI_FLSHB1CR1_TCSH_SHIFT            5
#define IMX_FLEXSPI_FLSHB1CR1_TCSH_MASK             (0x1F << 5)
/* Field TCSS - Serial Flash CS setup time */
#define IMX_FLEXSPI_FLSHB1CR1_TCSS_SHIFT            0
#define IMX_FLEXSPI_FLSHB1CR1_TCSS_MASK             (0x1F << 0)

/* Register FLSHB2CR1 - Flash B2 Control Register 1 */
#define IMX_FLEXSPI_FLSHB2CR1                       0x7C
/* Field CSINTERVAL - CS interval */
#define IMX_FLEXSPI_FLSHB2CR1_CSINTERVAL_SHIFT      16
#define IMX_FLEXSPI_FLSHB2CR1_CSINTERVAL_MASK       (0xFFFF << 16)
/* Field CSINTERVALUNIT - CS interval unit */
#define IMX_FLEXSPI_FLSHB2CR1_CSINTERVALUNIT_SHIFT  15
#define IMX_FLEXSPI_FLSHB2CR1_CSINTERVALUNIT_MASK   (0x1 << 15)
/* Field CAS - Column Address Size */
#define IMX_FLEXSPI_FLSHB2CR1_CAS_SHIFT             11
#define IMX_FLEXSPI_FLSHB2CR1_CAS_MASK              (0xF << 11)
/* Field WA - Word Addressable */
#define IMX_FLEXSPI_FLSHB2CR1_WA_SHIFT              10
#define IMX_FLEXSPI_FLSHB2CR1_WA_MASK               (0x1 << 10)
/* Field TCSH - Serial Flash CS Hold time */
#define IMX_FLEXSPI_FLSHB2CR1_TCSH_SHIFT            5
#define IMX_FLEXSPI_FLSHB2CR1_TCSH_MASK             (0x1F << 5)
/* Field TCSS - Serial Flash CS setup time */
#define IMX_FLEXSPI_FLSHB2CR1_TCSS_SHIFT            0
#define IMX_FLEXSPI_FLSHB2CR1_TCSS_MASK             (0x1F << 0)

/* Register FLSHA1CR2 - Flash A1 Control Register 2 */
#define IMX_FLEXSPI_FLSHA1CR2                       0x80
/* Field CLRINSTRPTR - Clear the instruction pointer */
#define IMX_FLEXSPI_FLSHA1CR2_CLRINSTRPTR_SHIFT     31
#define IMX_FLEXSPI_FLSHA1CR2_CLRINSTRPTR_MASK      (0x1 << 31)
/* Field AWRWAITUNIT - Wait unit */
#define IMX_FLEXSPI_FLSHA1CR2_AWRWAITUNIT_SHIFT     28
#define IMX_FLEXSPI_FLSHA1CR2_AWRWAITUNIT_MASK      (0x7 << 28)
/* Field AWRWAIT - Wait */
#define IMX_FLEXSPI_FLSHA1CR2_AWRWAIT_SHIFT         16
#define IMX_FLEXSPI_FLSHA1CR2_AWRWAIT_MASK          (0xFFF << 16)
/* Field AWRSEQNUM - Sequence Number for AHB Write triggered Command */
#define IMX_FLEXSPI_FLSHA1CR2_AWRSEQNUM_SHIFT       13
#define IMX_FLEXSPI_FLSHA1CR2_AWRSEQNUM_MASK        (0x7 << 13)
/* Field AWRSEQID - Sequence Index for AHB Write triggered Command */
#define IMX_FLEXSPI_FLSHA1CR2_AWRSEQID_SHIFT        8
#define IMX_FLEXSPI_FLSHA1CR2_AWRSEQID_MASK         (0x1F << 8)
/* Field ARDSEQNUM - Sequence Number for AHB Read triggered Command in LUT */
#define IMX_FLEXSPI_FLSHA1CR2_ARDSEQNUM_SHIFT       5
#define IMX_FLEXSPI_FLSHA1CR2_ARDSEQNUM_MASK        (0x7 << 5)
/* Field ARDSEQID - Sequence Index for AHB Read triggered Command in LUT */
#define IMX_FLEXSPI_FLSHA1CR2_ARDSEQID_SHIFT        0
#define IMX_FLEXSPI_FLSHA1CR2_ARDSEQID_MASK         (0x1F << 0)

/* Register FLSHA2CR2 - Flash A2 Control Register 2 */
#define IMX_FLEXSPI_FLSHA2CR2                       0x84
/* Field CLRINSTRPTR - Clear the instruction pointer */
#define IMX_FLEXSPI_FLSHA2CR2_CLRINSTRPTR_SHIFT     31
#define IMX_FLEXSPI_FLSHA2CR2_CLRINSTRPTR_MASK      (0x1 << 31)
/* Field AWRWAITUNIT - Wait unit */
#define IMX_FLEXSPI_FLSHA2CR2_AWRWAITUNIT_SHIFT     28
#define IMX_FLEXSPI_FLSHA2CR2_AWRWAITUNIT_MASK      (0x7 << 28)
/* Field AWRWAIT - Wait */
#define IMX_FLEXSPI_FLSHA2CR2_AWRWAIT_SHIFT         16
#define IMX_FLEXSPI_FLSHA2CR2_AWRWAIT_MASK          (0xFFF << 16)
/* Field AWRSEQNUM - Sequence Number for AHB Write triggered Command */
#define IMX_FLEXSPI_FLSHA2CR2_AWRSEQNUM_SHIFT       13
#define IMX_FLEXSPI_FLSHA2CR2_AWRSEQNUM_MASK        (0x7 << 13)
/* Field AWRSEQID - Sequence Index for AHB Write triggered Command */
#define IMX_FLEXSPI_FLSHA2CR2_AWRSEQID_SHIFT        8
#define IMX_FLEXSPI_FLSHA2CR2_AWRSEQID_MASK         (0x1F << 8)
/* Field ARDSEQNUM - Sequence Number for AHB Read triggered Command in LUT */
#define IMX_FLEXSPI_FLSHA2CR2_ARDSEQNUM_SHIFT       5
#define IMX_FLEXSPI_FLSHA2CR2_ARDSEQNUM_MASK        (0x7 << 5)
/* Field ARDSEQID - Sequence Index for AHB Read triggered Command in LUT */
#define IMX_FLEXSPI_FLSHA2CR2_ARDSEQID_SHIFT        0
#define IMX_FLEXSPI_FLSHA2CR2_ARDSEQID_MASK         (0x1F << 0)

/* Register FLSHB1CR2 - Flash B1 Control Register 2 */
#define IMX_FLEXSPI_FLSHB1CR2                       0x88
/* Field CLRINSTRPTR - Clear the instruction pointer */
#define IMX_FLEXSPI_FLSHB1CR2_CLRINSTRPTR_SHIFT     31
#define IMX_FLEXSPI_FLSHB1CR2_CLRINSTRPTR_MASK      (0x1 << 31)
/* Field AWRWAITUNIT - Wait unit */
#define IMX_FLEXSPI_FLSHB1CR2_AWRWAITUNIT_SHIFT     28
#define IMX_FLEXSPI_FLSHB1CR2_AWRWAITUNIT_MASK      (0x7 << 28)
/* Field AWRWAIT - Wait */
#define IMX_FLEXSPI_FLSHB1CR2_AWRWAIT_SHIFT         16
#define IMX_FLEXSPI_FLSHB1CR2_AWRWAIT_MASK          (0xFFF << 16)
/* Field AWRSEQNUM - Sequence Number for AHB Write triggered Command */
#define IMX_FLEXSPI_FLSHB1CR2_AWRSEQNUM_SHIFT       13
#define IMX_FLEXSPI_FLSHB1CR2_AWRSEQNUM_MASK        (0x7 << 13)
/* Field AWRSEQID - Sequence Index for AHB Write triggered Command */
#define IMX_FLEXSPI_FLSHB1CR2_AWRSEQID_SHIFT        8
#define IMX_FLEXSPI_FLSHB1CR2_AWRSEQID_MASK         (0x1F << 8)
/* Field ARDSEQNUM - Sequence Number for AHB Read triggered Command in LUT */
#define IMX_FLEXSPI_FLSHB1CR2_ARDSEQNUM_SHIFT       5
#define IMX_FLEXSPI_FLSHB1CR2_ARDSEQNUM_MASK        (0x7 << 5)
/* Field ARDSEQID - Sequence Index for AHB Read triggered Command in LUT */
#define IMX_FLEXSPI_FLSHB1CR2_ARDSEQID_SHIFT        0
#define IMX_FLEXSPI_FLSHB1CR2_ARDSEQID_MASK         (0x1F << 0)

/* Register FLSHB2CR2 - Flash B2 Control Register 2 */
#define IMX_FLEXSPI_FLSHB2CR2                       0x8C
/* Field CLRINSTRPTR - Clear the instruction pointer */
#define IMX_FLEXSPI_FLSHB2CR2_CLRINSTRPTR_SHIFT     31
#define IMX_FLEXSPI_FLSHB2CR2_CLRINSTRPTR_MASK      (0x1 << 31)
/* Field AWRWAITUNIT - Wait unit */
#define IMX_FLEXSPI_FLSHB2CR2_AWRWAITUNIT_SHIFT     28
#define IMX_FLEXSPI_FLSHB2CR2_AWRWAITUNIT_MASK      (0x7 << 28)
/* Field AWRWAIT - Wait */
#define IMX_FLEXSPI_FLSHB2CR2_AWRWAIT_SHIFT         16
#define IMX_FLEXSPI_FLSHB2CR2_AWRWAIT_MASK          (0xFFF << 16)
/* Field AWRSEQNUM - Sequence Number for AHB Write triggered Command */
#define IMX_FLEXSPI_FLSHB2CR2_AWRSEQNUM_SHIFT       13
#define IMX_FLEXSPI_FLSHB2CR2_AWRSEQNUM_MASK        (0x7 << 13)
/* Field AWRSEQID - Sequence Index for AHB Write triggered Command */
#define IMX_FLEXSPI_FLSHB2CR2_AWRSEQID_SHIFT        8
#define IMX_FLEXSPI_FLSHB2CR2_AWRSEQID_MASK         (0x1F << 8)
/* Field ARDSEQNUM - Sequence Number for AHB Read triggered Command in LUT */
#define IMX_FLEXSPI_FLSHB2CR2_ARDSEQNUM_SHIFT       5
#define IMX_FLEXSPI_FLSHB2CR2_ARDSEQNUM_MASK        (0x7 << 5)
/* Field ARDSEQID - Sequence Index for AHB Read triggered Command in LUT */
#define IMX_FLEXSPI_FLSHB2CR2_ARDSEQID_SHIFT        0
#define IMX_FLEXSPI_FLSHB2CR2_ARDSEQID_MASK         (0x1F << 0)

/* Register IPCR0 - IP Control Register 0 */
#define IMX_FLEXSPI_IPCR0                           0xA0
/* Field SFAR - Serial Flash Address for IP command */
#define IMX_FLEXSPI_IPCR0_SFAR_SHIFT                0
#define IMX_FLEXSPI_IPCR0_SFAR_MASK                 (0xFFFFFFFF << 0)

/* Register IPCR1 - IP Control Register 1 */
#define IMX_FLEXSPI_IPCR1                           0xA4
/* Field IPAREN - Parallel mode Enabled for IP command */
#define IMX_FLEXSPI_IPCR1_IPAREN_SHIFT              31
#define IMX_FLEXSPI_IPCR1_IPAREN_MASK               (0x1 << 31)
/* Field ISEQNUM - Sequence Number for IP command */
#define IMX_FLEXSPI_IPCR1_ISEQNUM_SHIFT             24
#define IMX_FLEXSPI_IPCR1_ISEQNUM_MASK              (0x7 << 24)
/* Field ISEQID - Sequence Index in LUT for IP command */
#define IMX_FLEXSPI_IPCR1_ISEQID_SHIFT              16
#define IMX_FLEXSPI_IPCR1_ISEQID_MASK               (0x1F << 16)
/* Field IDATSZ - Flash Read/Program Data Size (in Byte) for IP command */
#define IMX_FLEXSPI_IPCR1_IDATSZ_SHIFT              0
#define IMX_FLEXSPI_IPCR1_IDATSZ_MASK               (0xFFFF << 0)

/* Register IPCMD - IP Command Register */
#define IMX_FLEXSPI_IPCMD                           0xB0
/* Field TRG - IP command trigger */
#define IMX_FLEXSPI_IPCMD_TRG_SHIFT                 0
#define IMX_FLEXSPI_IPCMD_TRG_MASK                  (0x1 << 0)

/* Register DLPR - Data Learn Pattern Register */
#define IMX_FLEXSPI_DLPR                            0xB4
/* Field DLP - Data Learn Pattern */
#define IMX_FLEXSPI_DLPR_DLP_SHIFT                  0
#define IMX_FLEXSPI_DLPR_DLP_MASK                   (0xFFFFFFFF << 0)

/* Register IPRXFCR - IP RX FIFO Control Register */
#define IMX_FLEXSPI_IPRXFCR                         0xB8
/* Field RXWMRK - Water-mark level is (RXWMRK+1)*64 Bits */
#define IMX_FLEXSPI_IPRXFCR_RXWMRK_SHIFT            2
#define IMX_FLEXSPI_IPRXFCR_RXWMRK_MASK             (0x3F << 2)
/* Field RXDMAEN - IP RX FIFO reading by DMA enabled */
#define IMX_FLEXSPI_IPRXFCR_RXDMAEN_SHIFT           1
#define IMX_FLEXSPI_IPRXFCR_RXDMAEN_MASK            (0x1 << 1)
/* Field CLRIPRXF - Clear all valid data entries in IP RX FIFO */
#define IMX_FLEXSPI_IPRXFCR_CLRIPRXF_SHIFT          0
#define IMX_FLEXSPI_IPRXFCR_CLRIPRXF_MASK           (0x1 << 0)

/* Register IPTXFCR - IP TX FIFO Control Register */
#define IMX_FLEXSPI_IPTXFCR                         0xBC
/* Field TXWMRK - Water-mark level is (TXWMRK+1)*64 Bits */
#define IMX_FLEXSPI_IPTXFCR_TXWMRK_SHIFT            2
#define IMX_FLEXSPI_IPTXFCR_TXWMRK_MASK             (0x7F << 2)
/* Field TXDMAEN - IP TX FIFO reading by DMA enabled */
#define IMX_FLEXSPI_IPTXFCR_TXDMAEN_SHIFT           1
#define IMX_FLEXSPI_IPTXFCR_TXDMAEN_MASK            (0x1 << 1)
/* Field CLRIPTXF - Clear all valid data entries in IP TX FIFO */
#define IMX_FLEXSPI_IPTXFCR_CLRIPTXF_SHIFT          0
#define IMX_FLEXSPI_IPTXFCR_CLRIPTXF_MASK           (0x1 << 0)

/* Register DLLACR - DLL A Control Register */
#define IMX_FLEXSPI_DLLACR                          0xC0
/* Field OVRDVAL - Slave clock delay line delay cell number selection override value */
#define IMX_FLEXSPI_DLLACR_OVRDVAL_SHIFT            9
#define IMX_FLEXSPI_DLLACR_OVRDVAL_MASK             (0x3F << 9)
/* Field OVRDEN - Slave clock delay line delay cell number selection override enable */
#define IMX_FLEXSPI_DLLACR_OVRDEN_SHIFT             8
#define IMX_FLEXSPI_DLLACR_OVRDEN_MASK              (0x1 << 8)
/* Field SLVDLYTARGET - The delay target for slave delay line */
#define IMX_FLEXSPI_DLLACR_SLVDLYTARGET_SHIFT       3
#define IMX_FLEXSPI_DLLACR_SLVDLYTARGET_MASK        (0xF << 3)
/* Field DLLRESET - Software could force a reset on DLL */
#define IMX_FLEXSPI_DLLACR_DLLRESET_SHIFT           1
#define IMX_FLEXSPI_DLLACR_DLLRESET_MASK            (0x1 << 1)
/* Field DLLEN - DLL calibration enable */
#define IMX_FLEXSPI_DLLACR_DLLEN_SHIFT              0
#define IMX_FLEXSPI_DLLACR_DLLEN_MASK               (0x1 << 0)

/* Register DLLBCR - DLL B Control Register */
#define IMX_FLEXSPI_DLLBCR                          0xC4
/* Field OVRDVAL - Slave clock delay line delay cell number selection override value */
#define IMX_FLEXSPI_DLLBCR_OVRDVAL_SHIFT            9
#define IMX_FLEXSPI_DLLBCR_OVRDVAL_MASK             (0x3F << 9)
/* Field OVRDEN - Slave clock delay line delay cell number selection override enable */
#define IMX_FLEXSPI_DLLBCR_OVRDEN_SHIFT             8
#define IMX_FLEXSPI_DLLBCR_OVRDEN_MASK              (0x1 << 8)
/* Field SLVDLYTARGET - The delay target for slave delay line */
#define IMX_FLEXSPI_DLLBCR_SLVDLYTARGET_SHIFT       3
#define IMX_FLEXSPI_DLLBCR_SLVDLYTARGET_MASK        (0xF << 3)
/* Field DLLRESET - Software could force a reset on DLL */
#define IMX_FLEXSPI_DLLBCR_DLLRESET_SHIFT           1
#define IMX_FLEXSPI_DLLBCR_DLLRESET_MASK            (0x1 << 1)
/* Field DLLEN - DLL calibration enable */
#define IMX_FLEXSPI_DLLBCR_DLLEN_SHIFT              0
#define IMX_FLEXSPI_DLLBCR_DLLEN_MASK               (0x1 << 0)

/* Register STS0 - Status Register 0 */
#define IMX_FLEXSPI_STS0                            0xE0
/* Field DATALEARNPHASEB - Indicate the sampling clock phase selection on Port B after Data Learning */
#define IMX_FLEXSPI_STS0_DATALEARNPHASEB_SHIFT      8
#define IMX_FLEXSPI_STS0_DATALEARNPHASEB_MASK       (0xF << 8)
/* Field DATALEARNPHASEA - Indicate the sampling clock phase selection on Port A after Data Learning */
#define IMX_FLEXSPI_STS0_DATALEARNPHASEA_SHIFT      4
#define IMX_FLEXSPI_STS0_DATALEARNPHASEA_MASK       (0xF << 4)
/* Field ARBCMDSRC - Indicates the trigger source of current command sequence granted by arbitrator */
#define IMX_FLEXSPI_STS0_ARBCMDSRC_SHIFT            2
#define IMX_FLEXSPI_STS0_ARBCMDSRC_MASK             (0x7 << 2)
/* Field ARBIDLE - Indicates the state machine in ARB_CTL */
#define IMX_FLEXSPI_STS0_ARBIDLE_SHIFT              1
#define IMX_FLEXSPI_STS0_ARBIDLE_MASK               (0x1 << 1)
/* Field SEQIDLE - Indicates the state machine in SEQ_CTL */
#define IMX_FLEXSPI_STS0_SEQIDLE_SHIFT              0
#define IMX_FLEXSPI_STS0_SEQIDLE_MASK               (0x1 << 0)

/* Register STS1 - Status Register 1 */
#define IMX_FLEXSPI_STS1                            0xE4
/* Field IPCMDERRCODE - Indicate the Error Code when IP command Error detected */
#define IMX_FLEXSPI_STS1_IPCMDERRCODE_SHIFT         24
#define IMX_FLEXSPI_STS1_IPCMDERRCODE_MASK          (0xF << 24)
/* Field IPCMDERRID - Indicate the sequences Index when IP command error detected */
#define IMX_FLEXSPI_STS1_IPCMDERRID_SHIFT           16
#define IMX_FLEXSPI_STS1_IPCMDERRID_MASK            (0x1F << 16)
/* Field AHBCMDERRCODE - Indicate the Error Code when AHB command Error detected */
#define IMX_FLEXSPI_STS1_AHBCMDERRCODE_SHIFT        8
#define IMX_FLEXSPI_STS1_AHBCMDERRCODE_MASK         (0xF << 8)
/* Field AHBCMDERRID - Indicate the sequences index when AHB command error detected */
#define IMX_FLEXSPI_STS1_AHBCMDERRID_SHIFT          0
#define IMX_FLEXSPI_STS1_AHBCMDERRID_MASK           (0x1F << 8)

/* Register STS2 - Status Register 2 */
#define IMX_FLEXSPI_STS2                            0xE8
/* Field BREFSEL - Flash B sample clock reference delay line delay cell number selection */
#define IMX_FLEXSPI_STS2_BREFSEL_SHIFT              24
#define IMX_FLEXSPI_STS2_BREFSEL_MASK               (0x3F << 24)
/* Field BSLVSEL - Flash B sample clock slave delay line delay cell number selection */
#define IMX_FLEXSPI_STS2_BSLVSEL_SHIFT              18
#define IMX_FLEXSPI_STS2_BSLVSEL_MASK               (0x3F << 18)
/* Field BREFLOCK - Flash B sample clock reference delay line locked */
#define IMX_FLEXSPI_STS2_BREFLOCK_SHIFT             17
#define IMX_FLEXSPI_STS2_BREFLOCK_MASK              (0x1 << 17)
/* Field BSLVLOCK - Flash B sample clock slave delay line locked */
#define IMX_FLEXSPI_STS2_BSLVLOCK_SHIFT             16
#define IMX_FLEXSPI_STS2_BSLVLOCK_MASK              (0x1 << 16)
/* Field BREFSEL - Flash A sample clock reference delay line delay cell number selection */
#define IMX_FLEXSPI_STS2_AREFSEL_SHIFT              8
#define IMX_FLEXSPI_STS2_AREFSEL_MASK               (0x3F << 24)
/* Field BSLVSEL - Flash A sample clock slave delay line delay cell number selection */
#define IMX_FLEXSPI_STS2_ASLVSEL_SHIFT              2
#define IMX_FLEXSPI_STS2_ASLVSEL_MASK               (0x3F << 18)
/* Field BREFLOCK - Flash A sample clock reference delay line locked */
#define IMX_FLEXSPI_STS2_AREFLOCK_SHIFT             1
#define IMX_FLEXSPI_STS2_AREFLOCK_MASK              (0x1 << 17)
/* Field BSLVLOCK - Flash A sample clock slave delay line locked */
#define IMX_FLEXSPI_STS2_ASLVLOCK_SHIFT             0
#define IMX_FLEXSPI_STS2_ASLVLOCK_MASK              (0x1 << 16)

/* Register AHBSPNDSTS - AHB Suspend Status Register */
#define IMX_FLEXSPI_AHBSPNDSTS                      0xEC
/* Field DATLFT - Left Data size for suspended command sequence (in byte) */
#define IMX_FLEXSPI_AHBSPNDSTS_DATLFT_SHIFT         16
#define IMX_FLEXSPI_AHBSPNDSTS_DATLFT_MASK          (0xFFFF << 16)
/* Field BUFID - AHB RX BUF ID for suspended command sequence */
#define IMX_FLEXSPI_AHBSPNDSTS_BUFID_SHIFT          1
#define IMX_FLEXSPI_AHBSPNDSTS_BUFID_MASK           (0x7 << 1)
/* Field ACTIVE - Indicating whether there is a AHB Read Prefetch command sequence suspended */
#define IMX_FLEXSPI_AHBSPNDSTS_ACTIVE_SHIFT         0
#define IMX_FLEXSPI_AHBSPNDSTS_ACTIVE_MASK          (0x1 << 0)

/* Register IPRXFSTS - IP RX FIFO Status Register */
#define IMX_FLEXSPI_IPRXFSTS                        0xF0
/* Field RDCNTR - Total Read Data Counter: RDCNTR * 64 Bits */
#define IMX_FLEXSPI_IPRXFSTS_RDCNTR_SHIFT           16
#define IMX_FLEXSPI_IPRXFSTS_RDCNTR_MASK            (0xFFFF << 16)
/* Field FILL - Fill level of IP RX FIFO */
#define IMX_FLEXSPI_IPRXFSTS_FILL_SHIFT             0
#define IMX_FLEXSPI_IPRXFSTS_FILL_MASK              (0xFF << 16)

/* Register IPTXFSTS - IP TX FIFO Status Register */
#define IMX_FLEXSPI_IPTXFSTS                        0xF4
/* Field WRCNTR - Total Write Data Counter: WRCNTR * 64 Bits */
#define IMX_FLEXSPI_IPTXFSTS_WRCNTR_SHIFT           16
#define IMX_FLEXSPI_IPTXFSTS_WRCNTR_MASK            (0xFFFF << 16)
/* Field FILL - Fill level of IP TX FIFO */
#define IMX_FLEXSPI_IPTXFSTS_FILL_SHIFT             0
#define IMX_FLEXSPI_IPTXFSTS_FILL_MASK              (0xFF << 16)

/* Register RFDRa - IP RX FIFO Data Register a */
#define IMX_FLEXSPI_RFDRa_NUM                       32
#define IMX_FLEXSPI_RFDR0                           0x100
#define IMX_FLEXSPI_RFDRa(index)                    (IMX_FLEXSPI_RFDR0 + (index * 4))

/* Register TFDRa - IP TX FIFO Data Register a */
#define IMX_FLEXSPI_TFDRa_NUM                       32
#define IMX_FLEXSPI_TFDR0                           0x180
#define IMX_FLEXSPI_TFDRa(index)                    (IMX_FLEXSPI_TFDR0 + (index * 4))

/* Register LUTa - Look-up table Register a */
#define IMX_FLEXSPI_LUTa_NUM                        128  /* 128 / 4 = 128 LUT records */
#define IMX_FLEXSPI_LUT0                            0x200
#define IMX_FLEXSPI_LUTa(index)                     (IMX_FLEXSPI_LUT0 + (index * 4))
/* Field OPCODE1 */
#define IMX_FLEXSPI_LUT_OPCODE1_SHIFT               26
#define IMX_FLEXSPI_LUT_OPCODE1_MASK                (0x3F << 26)
/* Field NUM_PADS1 */
#define IMX_FLEXSPI_LUT_NUM_PADS1_SHIFT             24
#define IMX_FLEXSPI_LUT_NUM_PADS1_MASK              (0x3 << 24)
/* Field OPERAND1 */
#define IMX_FLEXSPI_LUT_OPERAND1_SHIFT              16
#define IMX_FLEXSPI_LUT_OPERAND1_MASK               (0xFF << 16)
/* Field OPCODE0 */
#define IMX_FLEXSPI_LUT_OPCODE0_SHIFT               10
#define IMX_FLEXSPI_LUT_OPCODE0_MASK                (0x3F << 10)
/* Field NUM_PADS0 */
#define IMX_FLEXSPI_LUT_NUM_PADS0_SHIFT             8
#define IMX_FLEXSPI_LUT_NUM_PADS0_MASK              (0x3 << 8)
/* Field OPERAND0 */
#define IMX_FLEXSPI_LUT_OPERAND0_SHIFT              0
#define IMX_FLEXSPI_LUT_OPERAND0_MASK               (0xFF << 0)

#endif /* IMX_FLEXSPI_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/aarch64/imx8xp/imx_flexspi.h $ $Rev: 850159 $")
#endif
