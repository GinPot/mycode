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

#ifndef IMX_LPSPI_H_
#define IMX_LPSPI_H_

/* Low Power Serial Peripheral Interface (LPSPI) */

/* Register VERID - Version ID Register */
#define IMX_LPSPI_VERID                 0x0
/* Field FEATURE - Module Identification Number */
#define IMX_LPSPI_VERID_FEATURE_SHIFT   0
#define IMX_LPSPI_VERID_FEATURE_MASK    0xFFFF
#define IMX_LPSPI_VERID_FEATURE(v)      (((v) << 0) & 0xFFFF)
/* Field MINOR - Minor Version Number */
#define IMX_LPSPI_VERID_MINOR_SHIFT     16
#define IMX_LPSPI_VERID_MINOR_MASK      0xFF0000
#define IMX_LPSPI_VERID_MINOR(v)        (((v) << 16) & 0xFF0000)
/* Field MAJOR - Major Version Number */
#define IMX_LPSPI_VERID_MAJOR_SHIFT     24
#define IMX_LPSPI_VERID_MAJOR_MASK      0xFF000000
#define IMX_LPSPI_VERID_MAJOR(v)        (((v) << 24) & 0xFF000000)


/* Register PARAM - Parameter Register */
#define IMX_LPSPI_PARAM                 0x4
/* Field TXFIFO - Transmit FIFO Size */
#define IMX_LPSPI_PARAM_TXFIFO_SHIFT    0
#define IMX_LPSPI_PARAM_TXFIFO_MASK     0xFF
#define IMX_LPSPI_PARAM_TXFIFO(v)       (((v) << 0) & 0xFF)
/* Field RXFIFO - Receive FIFO Size */
#define IMX_LPSPI_PARAM_RXFIFO_SHIFT    8
#define IMX_LPSPI_PARAM_RXFIFO_MASK     0xFF00
#define IMX_LPSPI_PARAM_RXFIFO(v)       (((v) << 8) & 0xFF00)


/* Register CR - Control Register */
#define IMX_LPSPI_CR                    0x10
/* Field MEN - Module Enable */
#define IMX_LPSPI_CR_MEN_SHIFT          0
#define IMX_LPSPI_CR_MEN_MASK           0x1
#define IMX_LPSPI_CR_MEN(v)             (((v) << 0) & 0x1)
/* Field RST - Software Reset */
#define IMX_LPSPI_CR_RST_SHIFT          1
#define IMX_LPSPI_CR_RST_MASK           0x2
#define IMX_LPSPI_CR_RST(v)             (((v) << 1) & 0x2)
/* Field DOZEN - Doze mode enable */
#define IMX_LPSPI_CR_DOZEN_SHIFT        2
#define IMX_LPSPI_CR_DOZEN_MASK         0x4
#define IMX_LPSPI_CR_DOZEN(v)           (((v) << 2) & 0x4)
/* Field DBGEN - Debug Enable */
#define IMX_LPSPI_CR_DBGEN_SHIFT        3
#define IMX_LPSPI_CR_DBGEN_MASK         0x8
#define IMX_LPSPI_CR_DBGEN(v)           (((v) << 3) & 0x8)
/* Field RTF - Reset Transmit FIFO */
#define IMX_LPSPI_CR_RTF_SHIFT          8
#define IMX_LPSPI_CR_RTF_MASK           0x100
#define IMX_LPSPI_CR_RTF(v)             (((v) << 8) & 0x100)
/* Field RRF - Reset Receive FIFO */
#define IMX_LPSPI_CR_RRF_SHIFT          9
#define IMX_LPSPI_CR_RRF_MASK           0x200
#define IMX_LPSPI_CR_RRF(v)             (((v) << 9) & 0x200)


/* Register SR - Status Register */
#define IMX_LPSPI_SR                    0x14
/* Field TDF - Transmit Data Flag */
#define IMX_LPSPI_SR_TDF_SHIFT          0
#define IMX_LPSPI_SR_TDF_MASK           0x1
#define IMX_LPSPI_SR_TDF(v)             (((v) << 0) & 0x1)
/* Field RDF - Receive Data Flag */
#define IMX_LPSPI_SR_RDF_SHIFT          1
#define IMX_LPSPI_SR_RDF_MASK           0x2
#define IMX_LPSPI_SR_RDF(v)             (((v) << 1) & 0x2)
/* Field WCF - Word Complete Flag */
#define IMX_LPSPI_SR_WCF_SHIFT          8
#define IMX_LPSPI_SR_WCF_MASK           0x100
#define IMX_LPSPI_SR_WCF(v)             (((v) << 8) & 0x100)
/* Field FCF - Frame Complete Flag */
#define IMX_LPSPI_SR_FCF_SHIFT          9
#define IMX_LPSPI_SR_FCF_MASK           0x200
#define IMX_LPSPI_SR_FCF(v)             (((v) << 9) & 0x200)
/* Field TCF - Transfer Complete Flag */
#define IMX_LPSPI_SR_TCF_SHIFT          10
#define IMX_LPSPI_SR_TCF_MASK           0x400
#define IMX_LPSPI_SR_TCF(v)             (((v) << 10) & 0x400)
/* Field TEF - Transmit Error Flag */
#define IMX_LPSPI_SR_TEF_SHIFT          11
#define IMX_LPSPI_SR_TEF_MASK           0x800
#define IMX_LPSPI_SR_TEF(v)             (((v) << 11) & 0x800)
/* Field REF - Receive Error Flag */
#define IMX_LPSPI_SR_REF_SHIFT          12
#define IMX_LPSPI_SR_REF_MASK           0x1000
#define IMX_LPSPI_SR_REF(v)             (((v) << 12) & 0x1000)
/* Field DMF - Data Match Flag */
#define IMX_LPSPI_SR_DMF_SHIFT          13
#define IMX_LPSPI_SR_DMF_MASK           0x2000
#define IMX_LPSPI_SR_DMF(v)             (((v) << 13) & 0x2000)
/* Field MBF - Module Busy Flag */
#define IMX_LPSPI_SR_MBF_SHIFT          24
#define IMX_LPSPI_SR_MBF_MASK           0x1000000
#define IMX_LPSPI_SR_MBF(v)             (((v) << 24) & 0x1000000)


/* Register IER - Interrupt Enable Register */
#define IMX_LPSPI_IER                   0x18
/* Field TDIE - Transmit Data Interrupt Enable */
#define IMX_LPSPI_IER_TDIE_SHIFT        0
#define IMX_LPSPI_IER_TDIE_MASK         0x1
#define IMX_LPSPI_IER_TDIE(v)           (((v) << 0) & 0x1)
/* Field RDIE - Receive Data Interrupt Enable */
#define IMX_LPSPI_IER_RDIE_SHIFT        1
#define IMX_LPSPI_IER_RDIE_MASK         0x2
#define IMX_LPSPI_IER_RDIE(v)           (((v) << 1) & 0x2)
/* Field WCIE - Word Complete Interrupt Enable */
#define IMX_LPSPI_IER_WCIE_SHIFT        8
#define IMX_LPSPI_IER_WCIE_MASK         0x100
#define IMX_LPSPI_IER_WCIE(v)           (((v) << 8) & 0x100)
/* Field FCIE - Frame Complete Interrupt Enable */
#define IMX_LPSPI_IER_FCIE_SHIFT        9
#define IMX_LPSPI_IER_FCIE_MASK         0x200
#define IMX_LPSPI_IER_FCIE(v)           (((v) << 9) & 0x200)
/* Field TCIE - Transfer Complete Interrupt Enable */
#define IMX_LPSPI_IER_TCIE_SHIFT        10
#define IMX_LPSPI_IER_TCIE_MASK         0x400
#define IMX_LPSPI_IER_TCIE(v)           (((v) << 10) & 0x400)
/* Field TEIE - Transmit Error Interrupt Enable */
#define IMX_LPSPI_IER_TEIE_SHIFT        11
#define IMX_LPSPI_IER_TEIE_MASK         0x800
#define IMX_LPSPI_IER_TEIE(v)           (((v) << 11) & 0x800)
/* Field REIE - Receive Error Interrupt Enable */
#define IMX_LPSPI_IER_REIE_SHIFT        12
#define IMX_LPSPI_IER_REIE_MASK         0x1000
#define IMX_LPSPI_IER_REIE(v)           (((v) << 12) & 0x1000)
/* Field DMIE - Data Match Interrupt Enable */
#define IMX_LPSPI_IER_DMIE_SHIFT        13
#define IMX_LPSPI_IER_DMIE_MASK         0x2000
#define IMX_LPSPI_IER_DMIE(v)           (((v) << 13) & 0x2000)


/* Register DER - DMA Enable Register */
#define IMX_LPSPI_DER                   0x1C
/* Field TDDE - Transmit Data DMA Enable */
#define IMX_LPSPI_DER_TDDE_SHIFT        0
#define IMX_LPSPI_DER_TDDE_MASK         0x1
#define IMX_LPSPI_DER_TDDE(v)           (((v) << 0) & 0x1)
/* Field RDDE - Receive Data DMA Enable */
#define IMX_LPSPI_DER_RDDE_SHIFT        1
#define IMX_LPSPI_DER_RDDE_MASK         0x2
#define IMX_LPSPI_DER_RDDE(v)           (((v) << 1) & 0x2)
/* Field FCDE - Frame completion DMA Enable */
#define IMX_LPSPI_DER_FCDE_SHIFT        9
#define IMX_LPSPI_DER_FCDE_MASK         0x200
#define IMX_LPSPI_DER_FCDE(v)           (((v) << 9) & 0x200)


/* Register CFGR0 - Configuration Register 0 */
#define IMX_LPSPI_CFGR0                 0x20
/* Field HREN - Host Request Enable */
#define IMX_LPSPI_CFGR0_HREN_SHIFT      0
#define IMX_LPSPI_CFGR0_HREN_MASK       0x1
#define IMX_LPSPI_CFGR0_HREN(v)         (((v) << 0) & 0x1)
/* Field HRPOL - Host Request Polarity */
#define IMX_LPSPI_CFGR0_HRPOL_SHIFT     1
#define IMX_LPSPI_CFGR0_HRPOL_MASK      0x2
#define IMX_LPSPI_CFGR0_HRPOL(v)        (((v) << 1) & 0x2)
/* Field HRSEL - Host Request Select */
#define IMX_LPSPI_CFGR0_HRSEL_SHIFT     2
#define IMX_LPSPI_CFGR0_HRSEL_MASK      0x4
#define IMX_LPSPI_CFGR0_HRSEL(v)        (((v) << 2) & 0x4)
/* Field CIRFIFO - Circular FIFO Enable */
#define IMX_LPSPI_CFGR0_CIRFIFO_SHIFT   8
#define IMX_LPSPI_CFGR0_CIRFIFO_MASK    0x100
#define IMX_LPSPI_CFGR0_CIRFIFO(v)      (((v) << 8) & 0x100)
/* Field RDMO - Receive Data Match Only */
#define IMX_LPSPI_CFGR0_RDMO_SHIFT      9
#define IMX_LPSPI_CFGR0_RDMO_MASK       0x200
#define IMX_LPSPI_CFGR0_RDMO(v)         (((v) << 9) & 0x200)


/* Register CFGR1 - Configuration Register 1 */
#define IMX_LPSPI_CFGR1                 0x24
/* Field MASTER - Master Mode */
#define IMX_LPSPI_CFGR1_MASTER_SHIFT    0
#define IMX_LPSPI_CFGR1_MASTER_MASK     0x1
#define IMX_LPSPI_CFGR1_MASTER(v)       (((v) << 0) & 0x1)
/* Field SAMPLE - Sample Point */
#define IMX_LPSPI_CFGR1_SAMPLE_SHIFT    1
#define IMX_LPSPI_CFGR1_SAMPLE_MASK     0x2
#define IMX_LPSPI_CFGR1_SAMPLE(v)       (((v) << 1) & 0x2)
/* Field AUTOPCS - Automatic PCS */
#define IMX_LPSPI_CFGR1_AUTOPCS_SHIFT   2
#define IMX_LPSPI_CFGR1_AUTOPCS_MASK    0x4
#define IMX_LPSPI_CFGR1_AUTOPCS(v)      (((v) << 2) & 0x4)
/* Field NOSTALL - No Stall */
#define IMX_LPSPI_CFGR1_NOSTALL_SHIFT   3
#define IMX_LPSPI_CFGR1_NOSTALL_MASK    0x8
#define IMX_LPSPI_CFGR1_NOSTALL(v)      (((v) << 3) & 0x8)
/* Field PCSPOL - Peripheral Chip Select Polarity */
#define IMX_LPSPI_CFGR1_PCSPOL_SHIFT    8
#define IMX_LPSPI_CFGR1_PCSPOL_MASK     0xF00
#define IMX_LPSPI_CFGR1_PCSPOL(v)       (((v) << 8) & 0xF00)
/* Field MATCFG - Match Configuration */
#define IMX_LPSPI_CFGR1_MATCFG_SHIFT    16
#define IMX_LPSPI_CFGR1_MATCFG_MASK     0x70000
#define IMX_LPSPI_CFGR1_MATCFG(v)       (((v) << 16) & 0x70000)
/* Field PINCFG - Pin Configuration */
#define IMX_LPSPI_CFGR1_PINCFG_SHIFT    24
#define IMX_LPSPI_CFGR1_PINCFG_MASK     0x3000000
#define IMX_LPSPI_CFGR1_PINCFG(v)       (((v) << 24) & 0x3000000)
/* Field OUTCFG - Output Configuration */
#define IMX_LPSPI_CFGR1_OUTCFG_SHIFT    26
#define IMX_LPSPI_CFGR1_OUTCFG_MASK     0x4000000
#define IMX_LPSPI_CFGR1_OUTCFG(v)       (((v) << 26) & 0x4000000)
/* Field PCSCFG - Peripheral Chip Select Configuration */
#define IMX_LPSPI_CFGR1_PCSCFG_SHIFT    27
#define IMX_LPSPI_CFGR1_PCSCFG_MASK     0x8000000
#define IMX_LPSPI_CFGR1_PCSCFG(v)       (((v) << 27) & 0x8000000)


/* Register DMR0 - Data Match Register 0 */
#define IMX_LPSPI_DMR0                  0x30
/* Field MATCH0 - Match 0 Value */
#define IMX_LPSPI_DMR0_MATCH0_SHIFT     0
#define IMX_LPSPI_DMR0_MATCH0_MASK      0xFFFFFFFF
#define IMX_LPSPI_DMR0_MATCH0(v)        (((v) << 0) & 0xFFFFFFFF)

/* Register DMR1 - Data Match Register 1 */
#define IMX_LPSPI_DMR1                  0x34
/* Field MATCH1 - Match 1 Value */
#define IMX_LPSPI_DMR1_MATCH1_SHIFT     0
#define IMX_LPSPI_DMR1_MATCH1_MASK      0xFFFFFFFF
#define IMX_LPSPI_DMR1_MATCH1(v)        (((v) << 0) & 0xFFFFFFFF)

/* Register CCR - Clock Configuration Register */
#define IMX_LPSPI_CCR                   0x40
/* Field SCKDIV - SCK Divider */
#define IMX_LPSPI_CCR_SCKDIV_SHIFT      0
#define IMX_LPSPI_CCR_SCKDIV_MASK       0xFF
#define IMX_LPSPI_CCR_SCKDIV(v)         (((v) << 0) & 0xFF)
/* Field DBT - Delay Between Transfers */
#define IMX_LPSPI_CCR_DBT_SHIFT         8
#define IMX_LPSPI_CCR_DBT_MASK          0xFF00
#define IMX_LPSPI_CCR_DBT(v)            (((v) << 8) & 0xFF00)
/* Field PCSSCK - PCS to SCK Delay */
#define IMX_LPSPI_CCR_PCSSCK_SHIFT      16
#define IMX_LPSPI_CCR_PCSSCK_MASK       0xFF0000
#define IMX_LPSPI_CCR_PCSSCK(v)         (((v) << 16) & 0xFF0000)
/* Field SCKPCS - SCK to PCS Delay */
#define IMX_LPSPI_CCR_SCKPCS_SHIFT      24
#define IMX_LPSPI_CCR_SCKPCS_MASK       0xFF000000
#define IMX_LPSPI_CCR_SCKPCS(v)         (((v) << 24) & 0xFF000000)


/* Register FCR - FIFO Control Register */
#define IMX_LPSPI_FCR                   0x58
/* Field TXWATER - Transmit FIFO Watermark */
#define IMX_LPSPI_FCR_TXWATER_SHIFT     0
#define IMX_LPSPI_FCR_TXWATER_MASK      0xFF
#define IMX_LPSPI_FCR_TXWATER(v)        (((v) << 0) & 0xFF)
/* Field RXWATER - Receive FIFO Watermark */
#define IMX_LPSPI_FCR_RXWATER_SHIFT     16
#define IMX_LPSPI_FCR_RXWATER_MASK      0xFF0000
#define IMX_LPSPI_FCR_RXWATER(v)        (((v) << 16) & 0xFF0000)


/* Register FSR - FIFO Status Register */
#define IMX_LPSPI_FSR                   0x5C
/* Field TXCOUNT - Transmit FIFO Count */
#define IMX_LPSPI_FSR_TXCOUNT_SHIFT     0
#define IMX_LPSPI_FSR_TXCOUNT_MASK      0xFF
#define IMX_LPSPI_FSR_TXCOUNT(v)        (((v) << 0) & 0xFF)
/* Field RXCOUNT - Receive FIFO Count */
#define IMX_LPSPI_FSR_RXCOUNT_SHIFT     16
#define IMX_LPSPI_FSR_RXCOUNT_MASK      0xFF0000
#define IMX_LPSPI_FSR_RXCOUNT(v)        (((v) << 16) & 0xFF0000)


/* Register TCR - Transmit Command Register */
#define IMX_LPSPI_TCR                   0x60
/* Field FRAMESZ - Frame Size */
#define IMX_LPSPI_TCR_FRAMESZ_SHIFT     0
#define IMX_LPSPI_TCR_FRAMESZ_MASK      0xFFF
#define IMX_LPSPI_TCR_FRAMESZ(v)        (((v) << 0) & 0xFFF)
/* Field WIDTH - Transfer Width */
#define IMX_LPSPI_TCR_WIDTH_SHIFT       16
#define IMX_LPSPI_TCR_WIDTH_MASK        0x30000
#define IMX_LPSPI_TCR_WIDTH(v)          (((v) << 16) & 0x30000)
/* Field TXMSK - Transmit Data Mask */
#define IMX_LPSPI_TCR_TXMSK_SHIFT       18
#define IMX_LPSPI_TCR_TXMSK_MASK        0x40000
#define IMX_LPSPI_TCR_TXMSK(v)          (((v) << 18) & 0x40000)
/* Field RXMSK - Receive Data Mask */
#define IMX_LPSPI_TCR_RXMSK_SHIFT       19
#define IMX_LPSPI_TCR_RXMSK_MASK        0x80000
#define IMX_LPSPI_TCR_RXMSK(v)          (((v) << 19) & 0x80000)
/* Field CONTC - Continuing Command */
#define IMX_LPSPI_TCR_CONTC_SHIFT       20
#define IMX_LPSPI_TCR_CONTC_MASK        0x100000
#define IMX_LPSPI_TCR_CONTC(v)          (((v) << 20) & 0x100000)
/* Field CONT - Continuous Transfer */
#define IMX_LPSPI_TCR_CONT_SHIFT        21
#define IMX_LPSPI_TCR_CONT_MASK         0x200000
#define IMX_LPSPI_TCR_CONT(v)           (((v) << 21) & 0x200000)
/* Field BYSW - Byte Swap */
#define IMX_LPSPI_TCR_BYSW_SHIFT        22
#define IMX_LPSPI_TCR_BYSW_MASK         0x400000
#define IMX_LPSPI_TCR_BYSW(v)           (((v) << 22) & 0x400000)
/* Field LSBF - LSB First */
#define IMX_LPSPI_TCR_LSBF_SHIFT        23
#define IMX_LPSPI_TCR_LSBF_MASK         0x800000
#define IMX_LPSPI_TCR_LSBF(v)           (((v) << 23) & 0x800000)
/* Field PCS - Peripheral Chip Select */
#define IMX_LPSPI_TCR_PCS_SHIFT         24
#define IMX_LPSPI_TCR_PCS_MASK          0x3000000
#define IMX_LPSPI_TCR_PCS(v)            (((v) << 24) & 0x3000000)
/* Field PRESCALE - Prescaler Value */
#define IMX_LPSPI_TCR_PRESCALE_SHIFT    27
#define IMX_LPSPI_TCR_PRESCALE_MASK     0x38000000
#define IMX_LPSPI_TCR_PRESCALE(v)       (((v) << 27) & 0x38000000)
/* Field CPHA - Clock Phase */
#define IMX_LPSPI_TCR_CPHA_SHIFT        30
#define IMX_LPSPI_TCR_CPHA_MASK         0x40000000
#define IMX_LPSPI_TCR_CPHA(v)           (((v) << 30) & 0x40000000)
/* Field CPOL - Clock Polarity */
#define IMX_LPSPI_TCR_CPOL_SHIFT        31
#define IMX_LPSPI_TCR_CPOL_MASK         0x80000000
#define IMX_LPSPI_TCR_CPOL(v)           (((v) << 31) & 0x80000000)


/* Register TDR - Transmit Data Register */
#define IMX_LPSPI_TDR                   0x64
/* Field DATA - Transmit Data */
#define IMX_LPSPI_TDR_DATA_SHIFT        0
#define IMX_LPSPI_TDR_DATA_MASK         0xFFFFFFFF
#define IMX_LPSPI_TDR_DATA(v)           (((v) << 0) & 0xFFFFFFFF)


/* Register RSR - Receive Status Register */
#define IMX_LPSPI_RSR                   0x70
/* Field SOF - Start Of Frame */
#define IMX_LPSPI_RSR_SOF_SHIFT         0
#define IMX_LPSPI_RSR_SOF_MASK          0x1
#define IMX_LPSPI_RSR_SOF(v)            (((v) << 0) & 0x1)
/* Field RXEMPTY - RX FIFO Empty */
#define IMX_LPSPI_RSR_RXEMPTY_SHIFT     1
#define IMX_LPSPI_RSR_RXEMPTY_MASK      0x2
#define IMX_LPSPI_RSR_RXEMPTY(v)        (((v) << 1) & 0x2)


/* Register RDR - Receive Data Register */
#define IMX_LPSPI_RDR                   0x74
/* Field DATA - Receive Data */
#define IMX_LPSPI_RDR_DATA_SHIFT        0
#define IMX_LPSPI_RDR_DATA_MASK         0xFFFFFFFF
#define IMX_LPSPI_RDR_DATA(v)           (((v) << 0) & 0xFFFFFFFF)


#endif /* IMX_LPSPI_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/aarch64/imx8/imx8_lpspi.h $ $Rev: 837679 $")
#endif
