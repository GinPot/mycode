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

#ifndef IMX_SAI_H_
#define IMX_SAI_H_

#include <stdint.h>

/** SAI Control Register macros */
#define IMX_SAI_CSR_FRDE_MASK    (1)
#define IMX_SAI_CSR_FWDE_MASK    (1 << 1)
#define IMX_SAI_CSR_FRIE_MASK    (1 << 8)
#define IMX_SAI_CSR_FWIE_MASK    (1 << 9)
#define IMX_SAI_CSR_FEIE_MASK    (1 << 10)
#define IMX_SAI_CSR_SEIE_MASK    (1 << 11)
#define IMX_SAI_CSR_WSIE_MASK    (1 << 12)
#define IMX_SAI_CSR_FRF_MASK     (1 << 16)
#define IMX_SAI_CSR_FWF_MASK     (1 << 17)
#define IMX_SAI_CSR_FEF_MASK     (1 << 18)
#define IMX_SAI_CSR_SEF_MASK     (1 << 19)
#define IMX_SAI_CSR_WSF_MASK     (1 << 20)
#define IMX_SAI_CSR_SR_MASK      (1 << 24)
#define IMX_SAI_CSR_FR_MASK      (1 << 25)
#define IMX_SAI_CSR_BCE_MASK     (1 << 28)
#define IMX_SAI_CSR_DBGE_MASK    (1 << 29)
#define IMX_SAI_CSR_STOPE_MASK   (1 << 30)
#define IMX_SAI_CSR_EN_MASK      (1 << 31)
/** SAI Configuration 1 Register macros */
#define IMX_SAI_CR1_FW_MASK      (0x1F)
/** SAI Configuration 2 Register macros */
#define IMX_SAI_CR2_DIV_MASK     (0xFF)
#define IMX_SAI_CR2_BCD_MASK     (1 << 24)
#define IMX_SAI_CR2_BCP_MASK     (1 << 25)
#define IMX_SAI_CR2_MSEL_SHIFT   (26)
#define IMX_SAI_CR2_MSEL_MASK    (3 << IMX_SAI_CR2_MSEL_SHIFT)
#define IMX_SAI_CR2_BCI_MASK     (1 << 28)
#define IMX_SAI_CR2_BCS_MASK     (1 << 29)
#define IMX_SAI_CR2_SYNC_SHIFT   (30)
#define IMX_SAI_CR2_SYNC_MASK    (3 << IMX_SAI_CR2_SYNC_SHIFT)
/** SAI Configuration 3 Register macros */
#define IMX_SAI_CR3_WDFL_MASK    (0x1F)
#define IMX_SAI_CR3_CE_MASK      (1 << 16)
/** SAI Configuration 4 Register macros */
#define IMX_SAI_CR4_FSD_MASK     (1)
#define IMX_SAI_CR4_FSP_MASK     (1 << 1)
#define IMX_SAI_CR4_FSE_MASK     (1 << 3)
#define IMX_SAI_CR4_MF_MASK      (1 << 4)
#define IMX_SAI_CR4_SYWD_SHIFT   (8)
#define IMX_SAI_CR4_SYWD_MASK    (0x1F << IMX_SAI_CR4_SYWD_SHIFT)
#define IMX_SAI_CR4_FRSZ_SHIFT   (16)
#define IMX_SAI_CR4_FRSZ_MASK    (0x1F << IMX_SAI_CR4_FRSZ_SHIFT)
/** SAI Transmit Configuration 5 Register macros */
#define IMX_SAI_CR5_FBT_SHIFT    (8)
#define IMX_SAI_CR5_FBT_MASK     (0x1F << IMX_SAI_CR5_FBT_SHIFT)
#define IMX_SAI_CR5_W0W_SHIFT    (16)
#define IMX_SAI_CR5_W0W_MASK     (0x1F << IMX_SAI_CR5_W0W_SHIFT)
#define IMX_SAI_CR5_WNW_SHIFT    (24)
#define IMX_SAI_CR5_WNW_MASK     (0x1F << IMX_SAI_CR5_WNW_SHIFT)

/** SAI Data Register offsets */
#define IMX_SAI_TDR_OFFSET       (0x20)
#define IMX_SAI_RDR_OFFSET       (0xA0)

typedef volatile uint32_t vuint32_t;
typedef volatile uint8_t  vuint8_t;

/** SAI peripheral RX/TX register structure. */
typedef struct imx_sai_rxtx {
    vuint32_t csr;                            /**< SAI Control Register */
    vuint32_t cr1;                            /**< SAI Configuration 1 Register */
    vuint32_t cr2;                            /**< SAI Configuration 2 Register */
    vuint32_t cr3;                            /**< SAI Configuration 3 Register */
    vuint32_t cr4;                            /**< SAI Configuration 4 Register */
    vuint32_t cr5;                            /**< SAI Transmit Configuration 5 Register */
    vuint8_t  reserved0[8];                   /**< Reserved Space */
    vuint32_t dr;                             /**< SAI Data Register */
    vuint8_t  reserved1[28];                  /**< Reserved space */
    vuint32_t fr;                             /**< SAI FIFO Register */
    vuint8_t  reserved2[28];                  /**< Reserved Space */
    vuint32_t mr;                             /**< SAI Mask Register */
    vuint8_t  reserved3[28];                  /**< Reserved Space */
} imx_sai_rxtx_t;

/** Enumeration used for indexing SAI register array */
typedef enum imx_txrx_index {
    IMX_SAI_TX = 0,
    IMX_SAI_RX = 1,
    IMX_SAI_RXTX_MAX = IMX_SAI_RX
} imx_txrx_index_t;

#endif /* IMX_SAI_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/aarch64/imx8xp/imx_sai.h $ $Rev: 850159 $")
#endif
