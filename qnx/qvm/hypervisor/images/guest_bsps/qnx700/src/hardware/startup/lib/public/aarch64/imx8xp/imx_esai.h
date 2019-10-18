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

#ifndef IMX_ESAI_H_
#define IMX_ESAI_H_

#include <stdint.h>

/** ESAI ETDR Register offset */
#define IMX_ESAI_ETDR_OFFSET           0x00
/** ESAI ERDR Register offset */
#define IMX_ESAI_ERDR_OFFSET           0x04
/** ESAI ECR Register macros */
#define IMX_ESAI_ECR_ETI_MASK          (1 << 19)
#define IMX_ESAI_ECR_ETO_MASK          (1 << 18)
#define IMX_ESAI_ECR_ERI_MASK          (1 << 17)
#define IMX_ESAI_ECR_ERO_MASK          (1 << 16)
#define IMX_ESAI_ECR_ERST_MASK         (1 << 1)
#define IMX_ESAI_ECR_ESAIEN_MASK       (1 << 0)
/** ESAI ESR Register macros */
#define IMX_ESAI_ESR_TINIT_MASK        (1 << 10)
#define IMX_ESAI_ESR_RFF_MASK          (1 << 9)
#define IMX_ESAI_ESR_TFE_MASK          (1 << 8)
#define IMX_ESAI_ESR_TLS_MASK          (1 << 7)
#define IMX_ESAI_ESR_TDE_MASK          (1 << 6)
#define IMX_ESAI_ESR_TED_MASK          (1 << 5)
#define IMX_ESAI_ESR_TD_MASK           (1 << 4)
#define IMX_ESAI_ESR_RLS_MASK          (1 << 3)
#define IMX_ESAI_ESR_RDE_MASK          (1 << 2)
#define IMX_ESAI_ESR_RED_MASK          (1 << 1)
#define IMX_ESAI_ESR_RD_MASK           (1 << 0)
/** ESAI TFCR Register macros */
#define IMX_ESAI_TFCR_TIEN_MASK        (1 << 19)
#define IMX_ESAI_TFCR_TWA(x)           ((x & 7) << 16)
#define IMX_ESAI_TFCR_TFWM(x)          ((x & 0xFF) << 8)
#define IMX_ESAI_TFCR_TE_MASK          (0xFC)
#define IMX_ESAI_TFCR_TE(x)            ((1 << (x+2)) & IMX_ESAI_TFCR_TE_MASK)
#define IMX_ESAI_TFCR_TFR_MASK         (1 << 1)
#define IMX_ESAI_TFCR_TFE_MASK         (1 << 0)
/** ESAI TFSR Register macros */
#define IMX_ESAI_TFSR_TFCNT(x)         (x & 0xFF)
/** ESAI RFCR Register macros */
#define IMX_ESAI_RFCR_REXT_MASK        (1 << 19)
#define IMX_ESAI_RFCR_RWA(x)           ((x & 7) << 16)
#define IMX_ESAI_RFCR_RFWM(x)          ((x & 0xFF) << 8)
#define IMX_ESAI_RFCR_RE_MASK          (0x3C)
#define IMX_ESAI_RFCR_RE(x)            ((1 << (x+2)) & IMX_ESAI_RFCR_RE_MASK)
#define IMX_ESAI_RFCR_RFR_MASK         (1 << 1)
#define IMX_ESAI_RFCR_RFE_MASK         (1 << 0)
/** ESAI RFSR Register macros */
#define IMX_ESAI_RFSR_RFCNT(x)         (x & 0xFF)
/** ESAI SAISR Register macros */
#define IMX_ESAI_SAISR_TDE_MASK        (1 << 15)
#define IMX_ESAI_SAISR_TUE_MASK        (1 << 14)
/** ESAI SAICR Register macros */
#define IMX_ESAI_SAICR_ALC_MASK        (1 << 8)
#define IMX_ESAI_SAICR_SYNC_MASK       (1 << 6)
/** ESAI TCR Register macros */
#define IMX_ESAI_TCR_TPR_MASK          (1 << 19)
#define IMX_ESAI_TCR_PADC_MASK         (1 << 17)
#define IMX_ESAI_TCR_TFSR_MASK         (1 << 16)
#define IMX_ESAI_TCR_TFSL_MASK         (1 << 15)
#define IMX_ESAI_TCR_TSWS(x)           ((x & 0x1F) << 10)
#define IMX_ESAI_TCR_TMOD(x)           ((x & 3) << 8)
#define IMX_ESAI_TCR_TE(x)             ((1 << x) & 0x3F)
#define IMX_ESAI_TCR_TE_MASK           0x3F
/** ESAI TCCR Register macros */
#define IMX_ESAI_TCCR_THCKD_MASK       (1 << 23)
#define IMX_ESAI_TCCR_TFSD_MASK        (1 << 22)
#define IMX_ESAI_TCCR_TCKD_MASK        (1 << 21)
#define IMX_ESAI_TCCR_THCKP_MASK       (1 << 20)
#define IMX_ESAI_TCCR_TFSP_MASK        (1 << 19)
#define IMX_ESAI_TCCR_TCKP_MASK        (1 << 18)
#define IMX_ESAI_TCCR_TFP(x)           ((x & 0xF) << 14)
#define IMX_ESAI_TCCR_TDC(x)           ((x & 0x1F) << 9)
#define IMX_ESAI_TCCR_TPSR_MASK        (1 << 8)
#define IMX_ESAI_TCCR_TPM(x)           (x & 0xFF)
/** ESAI RCR Register macros */
#define IMX_ESAI_RCR_RPR_MASK          (1 << 19)
#define IMX_ESAI_RCR_RFSR_MASK         (1 << 16)
#define IMX_ESAI_RCR_RFSL_MASK         (1 << 15)
#define IMX_ESAI_RCR_RSWS(x)           ((x & 0x1F) << 10)
#define IMX_ESAI_RCR_RMOD(x)           ((x & 3) << 8)
#define IMX_ESAI_RCR_RE(x)             ((1 << (x)) & 0xF)
#define IMX_ESAI_RCR_REMASK_MASK       0xF
/** ESAI RCCR Register macros */
#define IMX_ESAI_RCCR_RHCKD_MASK       (1 << 23)
#define IMX_ESAI_RCCR_RFSD_MASK        (1 << 22)
#define IMX_ESAI_RCCR_RCKD_MASK        (1 << 21)
#define IMX_ESAI_RCCR_RHCKP_MASK       (1 << 20)
#define IMX_ESAI_RCCR_RFSP_MASK        (1 << 19)
#define IMX_ESAI_RCCR_RCKP_MASK        (1 << 18)
#define IMX_ESAI_RCCR_RFP(x)           ((x & 0x1F) << 14)
#define IMX_ESAI_RCCR_RDC(x)           ((x & 0x1F) << 9)
#define IMX_ESAI_RCCR_RPSP_MASK        (1 << 8)
#define IMX_ESAI_RCCR_RPM(x)           (x & 0xFF)
/** ESAI PORT Register macros */
#define IMX_ESAI_PORT0_SCKR_MASK       (1 << 0)
#define IMX_ESAI_PORT1_FSR_MASK        (1 << 1)
#define IMX_ESAI_PORT2_HCKR_MASK       (1 << 2)
#define IMX_ESAI_PORT3_SCKT_MASK       (1 << 3)
#define IMX_ESAI_PORT4_FST_MASK        (1 << 4)
#define IMX_ESAI_PORT5_HCKT_MASK       (1 << 5)
#define IMX_ESAI_PORT6_SDO5_SDI0_MASK  (1 << 6)
#define IMX_ESAI_PORT7_SDO4_SDI1_MASK  (1 << 7)
#define IMX_ESAI_PORT8_SDO3_SDI2_MASK  (1 << 8)
#define IMX_ESAI_PORT9_SDO2_SDI3_MASK  (1 << 9)
#define IMX_ESAI_PORT10_SDO1_MASK      (1 << 10)
#define IMX_ESAI_PORT11_SDO0_MASK      (1 << 11)

typedef volatile uint32_t vuint32_t;
typedef volatile uint16_t vuint16_t;
typedef volatile uint8_t  vuint8_t;

/** ESAI peripheral register structure. */
typedef struct esai {
    vuint32_t   etdr;           /**< ESAI Transmit Data Register */
    vuint32_t   erdr;           /**< ESAI Receive Data Register */
    vuint32_t   ecr;            /**< ESAI Control Register */
    vuint32_t   esr;            /**< ESAI Status Register */
    vuint32_t   tfcr;           /**< Transmit FIFO Configuration Register */
    vuint32_t   tfsr;           /**< Transmit FIFO Status Register */
    vuint32_t   rfcr;           /**< Receive FIFO Configuration Register */
    vuint32_t   rfsr;           /**< Receive FIFO Status Register */
    vuint32_t   reserved0[24];  /**< Reserved Space */
    vuint32_t   tx0;            /**< Transmit Data register 0 */
    vuint32_t   tx1;            /**< Transmit Data register 1 */
    vuint32_t   tx2;            /**< Transmit Data register 2 */
    vuint32_t   tx3;            /**< Transmit Data register 3 */
    vuint32_t   tx4;            /**< Transmit Data register 4 */
    vuint32_t   tx5;            /**< Transmit Data register 5 */
    vuint32_t   tsr;            /**< ESAI Transmit Slot Register */
    vuint32_t   reserved1;      /**< Reserved Space */
    vuint32_t   rx0;            /**< Receive Data Register 0 */
    vuint32_t   rx1;            /**< Receive Data Register 1 */
    vuint32_t   rx2;            /**< Receive Data Register 2 */
    vuint32_t   rx3;            /**< Receive Data Register 3 */
    vuint32_t   reserved2[7];   /**< Reserved Space */
    vuint32_t   saisr;          /**< Serial Audio Interface Status Register */
    vuint32_t   saicr;          /**< Serial Audio Interface Control Register */
    vuint32_t   tcr;            /**< Transmit Control Register */
    vuint32_t   tccr;           /**< Transmit Clock Control Register */
    vuint32_t   rcr;            /**< Receive Control Register */
    vuint32_t   rccr;           /**< Receive Clock Control Register */
    vuint32_t   tsma;           /**< Transmit Slot Mask Register A */
    vuint32_t   tsmb;           /**< Transmit Slot Mask Register B */
    vuint32_t   rsma;           /**< Receive Slot Mask Register A */
    vuint32_t   rsmb;           /**< Receive Slot Mask Register B */
    vuint32_t   reserved3;      /**< Reserved Space */
    vuint32_t   prrc;           /**< Port C Direction Register */
    vuint32_t   pcrc;           /**< Port C Control Register */
} esai_t;

#endif /* IMX_ESAI_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/aarch64/imx8xp/imx_esai.h $ $Rev: 850159 $")
#endif
