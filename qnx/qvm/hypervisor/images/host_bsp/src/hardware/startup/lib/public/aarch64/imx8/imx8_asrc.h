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

#ifndef IMX_ASRC_H_
#define IMX_ASRC_H_

#include <stdint.h>

#define IMX_ASRC_ASR76K_VAL                          0x6C8
#define IMX_ASRC_ASR56K_VAL                          0x935

/** Offset of ASRC input data register */
#define IMX_ASRC_ASRDI                               0x60
/** Offset of ASRC output data register */
#define IMX_ASRC_ASRDO                               0x64
/** Gets DMA event value for input data register */
#define IMX_ASRC_IN_DMA_EVENT(pair)                  (17 + pair)
/** Gets DMA event value for output data register */
#define IMX_ASRC_OUT_DMA_EVENT(pair)                 (20 + pair)

/** ASRC Control Register macros */
#define IMX_ASRC_ASRCTR_ASRCEN_MASK                  (1)
#define IMX_ASRC_ASRCTR_ASREA_MASK                   (1 << 1)
#define IMX_ASRC_ASRCTR_ASREB_MASK                   (1 << 2)
#define IMX_ASRC_ASRCTR_ASREC_MASK                   (1 << 3)
#define IMX_ASRC_ASRCTR_ASRE_MASK(index)             (1 << (1 + index))
#define IMX_ASRC_ASRCTR_SRST_MASK                    (1 << 4)
#define IMX_ASRC_ASRCTR_IDR_MASK(index)              (1 << (13 + 2 * index))
#define IMX_ASRC_ASRCTR_IDRA_MASK                    (1 << 13)
#define IMX_ASRC_ASRCTR_USR_MASK(index)              (1 << (14 + 2 * index))
#define IMX_ASRC_ASRCTR_USRA_MASK                    (1 << 14)
#define IMX_ASRC_ASRCTR_IDRB_MASK                    (1 << 15)
#define IMX_ASRC_ASRCTR_USRB_MASK                    (1 << 16)
#define IMX_ASRC_ASRCTR_IDRC_MASK                    (1 << 17)
#define IMX_ASRC_ASRCTR_USRC_MASK                    (1 << 18)
#define IMX_ASRC_ASRCTR_ATSA_MASK                    (1 << 20)
#define IMX_ASRC_ASRCTR_ATSB_MASK                    (1 << 21)
#define IMX_ASRC_ASRCTR_ATSC_MASK                    (1 << 22)
/** ASRC Interrupt Enable Register macros */
#define IMX_ASRC_ASRIER_ADIEA_MASK                   (1)
#define IMX_ASRC_ASRIER_ADOEA_MASK                   (1 << 3)
#define IMX_ASRC_ASRIER_AOLIE_MASK                   (1 << 6)
/** ASRC Channel Number Configuration Register macros */
#define IMX_ASRC_ASRCNCR_ANCA_SHIFT                  (0)
#define IMX_ASRC_ASRCNCR_ANCA_MASK                   (0xf << IMX_ASRC_ASRCNCR_ANCA_SHIFT)
#define IMX_ASRC_ASRCNCR_ANCB_SHIFT                  (4)
#define IMX_ASRC_ASRCNCR_ANCB_MASK                   (0xf << IMX_ASRC_ASRCNCR_ANCB_SHIFT)
#define IMX_ASRC_ASRCNCR_ANCC_SHIFT                  (8)
#define IMX_ASRC_ASRCNCR_ANCC_MASK                   (0xf << IMX_ASRC_ASRCNCR_ANCC_SHIFT)
#define IMX_ASRC_ASRCNCR_ANC_SHIFT(index)            (IMX_ASRC_ASRCNCR_ANCA_SHIFT + 4 * index)
#define IMX_ASRC_ASRCNCR_ANC(index)                  (0xf << IMX_ASRC_ASRCNCR_ANC_SHIFT(index))
/*!< ASRC Filter Configuration Status Register */
#define IMX_ASRC_ASRCFG_PREMODA_SHIFT                (6)
#define IMX_ASRC_ASRCFG_PREMOD_SHIFT(index)          (IMX_ASRC_ASRCFG_PREMODA_SHIFT + 4 * index)
#define IMX_ASRC_ASRCFG_PREMODA_MASK                 (3 << IMX_ASRC_ASRCFG_PREMODA_SHIFT)
#define IMX_ASRC_ASRCFG_POSTMODA_SHIFT               (8)
#define IMX_ASRC_ASRCFG_POSTMOD_SHIFT(index)         (IMX_ASRC_ASRCFG_POSTMODA_SHIFT + 4 * index)
#define IMX_ASRC_ASRCFG_POSTMODA_MASK                (3 << IMX_ASRC_ASRCFG_POSTMODA_SHIFT)
#define IMX_ASRC_ASRCFG_PREMODB_SHIFT                (10)
#define IMX_ASRC_ASRCFG_PREMODB_MASK                 (3 << IMX_ASRC_ASRCFG_PREMODB_SHIFT)
#define IMX_ASRC_ASRCFG_POSTMODB_SHIFT               (12)
#define IMX_ASRC_ASRCFG_POSTMODB_MASK                (3 << IMX_ASRC_ASRCFG_POSTMODB_SHIFT)
#define IMX_ASRC_ASRCFG_PREMODC_SHIFT                (14)
#define IMX_ASRC_ASRCFG_PREMODC_MASK                 (3 << IMX_ASRC_ASRCFG_PREMODC_SHIFT)
#define IMX_ASRC_ASRCFG_POSTMODC_SHIFT               (16)
#define IMX_ASRC_ASRCFG_POSTMODC_MASK                (3 << IMX_ASRC_ASRCFG_POSTMODC_SHIFT)
#define IMX_ASRC_ASRCFG_NDPRA_MASK                   (1 << 18)
#define IMX_ASRC_ASRCFG_NDPRB_MASK                   (1 << 19)
#define IMX_ASRC_ASRCFG_NDPRC_MASK                   (1 << 20)
#define IMX_ASRC_ASRCFG_INIRQA_MASK                  (1 << 21)
#define IMX_ASRC_ASRCFG_INIRQB_MASK                  (1 << 22)
#define IMX_ASRC_ASRCFG_INIRQC_MASK                  (1 << 23)
#define IMX_ASRC_ASRCFG_INIRQ_MASK(index)            (1 << (21 + index))
/** ASRC Clock Source Register macros */
#define IMX_ASRC_ASRCSR_AICSA_SHIFT                  (0)
#define IMX_ASRC_ASRCSR_AICSA_MASK                   (0xf)
#define IMX_ASRC_ASRCSR_AICS_SHIFT(index)            (IMX_ASRC_ASRCSR_AICSA_SHIFT + 4 * index)
#define IMX_ASRC_ASRCSR_AICS_MASK(index)             (0xf << IMX_ASRC_ASRCSR_AICS_SHIFT(index))
#define IMX_ASRC_ASRCSR_AICSBS_SHIFT                 (4)
#define IMX_ASRC_ASRCSR_AICSB_MASK                   (0xf << IMX_ASRC_ASRCSR_AICSBS_SHIFT)
#define IMX_ASRC_ASRCSR_AICSC_SHIFT                  (8)
#define IMX_ASRC_ASRCSR_AICSC_MASK                   (0xf << IMX_ASRC_ASRCSR_AICSC_SHIFT)
#define IMX_ASRC_ASRCSR_AOCSA_SHIFT                  (12)
#define IMX_ASRC_ASRCSR_AOCS_SHIFT(index)            (IMX_ASRC_ASRCSR_AOCSA_SHIFT + 4 * index)
#define IMX_ASRC_ASRCSR_AOCSA_MASK                   (0xf << IMX_ASRC_ASRCSR_AOCSA_SHIFT)
#define IMX_ASRC_ASRCSR_AOCS_MASK(index)             (0xf << IMX_ASRC_ASRCSR_AOCS_SHIFT(index))
#define IMX_ASRC_ASRCSR_AOCSB_SHIFT                  (16)
#define IMX_ASRC_ASRCSR_AOCSB_MASK                   (0xf << IMX_ASRC_ASRCSR_AOCSB_SHIFT)
#define IMX_ASRC_ASRCSR_AOCSC_SHIFT                  (20)
#define IMX_ASRC_ASRCSR_AOCSC_MASK                   (0xf << IMX_ASRC_ASRCSR_AOCSC_SHIFT)
/** ASRC Clock Divider Register 1 macros */
#define IMX_ASRC_ASRCDR1_AxCPx_MASK                  (7)
#define IMX_ASRC_ASRCDR1_AICPA_SHIFT                 (0)
#define IMX_ASRC_ASRCDR1_AICPA_MASK                  (IMX_ASRC_ASRCDR1_AxCPx_MASK << IMX_ASRC_ASRCDR1_AICPA_SHIFT)
#define IMX_ASRC_ASRCDR1_AICDA_SHIFT                 (3)
#define IMX_ASRC_ASRCDR1_AICDA_MASK                  (IMX_ASRC_ASRCDR1_AxCPx_MASK << IMX_ASRC_ASRCDR1_AICDA_SHIFT)
#define IMX_ASRC_ASRCDR1_AICPB_SHIFT                 (6)
#define IMX_ASRC_ASRCDR1_AICPB_MASK                  (IMX_ASRC_ASRCDR1_AxCPx_MASK << IMX_ASRC_ASRCDR1_AICPB_SHIFT)
#define IMX_ASRC_ASRCDR1_AICDB_SHIFT                 (9)
#define IMX_ASRC_ASRCDR1_AICDB_MASK                  (IMX_ASRC_ASRCDR1_AxCPx_MASK << IMX_ASRC_ASRCDR1_AICDB_SHIFT)
#define IMX_ASRC_ASRCDR1_AOCPA_SHIFT                 (12)
#define IMX_ASRC_ASRCDR1_AOCPA_MASK                  (IMX_ASRC_ASRCDR1_AxCPx_MASK << IMX_ASRC_ASRCDR1_AOCPA_SHIFT)
#define IMX_ASRC_ASRCDR1_AOCDA_SHIFT                 (15)
#define IMX_ASRC_ASRCDR1_AOCDA_MASK                  (IMX_ASRC_ASRCDR1_AxCPx_MASK << IMX_ASRC_ASRCDR1_AOCDA_SHIFT)
#define IMX_ASRC_ASRCDR1_AOCPB_SHIFT                 (18)
#define IMX_ASRC_ASRCDR1_AOCPB_MASK                  (IMX_ASRC_ASRCDR1_AxCPx_MASK << IMX_ASRC_ASRCDR1_AOCPB_SHIFT)
#define IMX_ASRC_ASRCDR1_AOCDB_SHIFT                 (21)
#define IMX_ASRC_ASRCDR1_AOCDB_MASK                  (IMX_ASRC_ASRCDR1_AxCPx_MASK << IMX_ASRC_ASRCDR1_AOCDB_SHIFT)
/** ASRC Clock Divider Register 2 macros */
#define IMX_ASRC_ASRCDR2_AxCPx_MASK                  (7)
#define IMX_ASRC_ASRCDR2_AICPC_SHIFT                 (0)
#define IMX_ASRC_ASRCDR2_AICPC_MASK                  (IMX_ASRC_ASRCDR2_AxCPx_MASK << IMX_ASRC_ASRCDR2_AICPC_SHIFT)
#define IMX_ASRC_ASRCDR2_AICDC_SHIFT                 (3)
#define IMX_ASRC_ASRCDR2_AICDC_MASK                  (IMX_ASRC_ASRCDR2_AxCPx_MASK << IMX_ASRC_ASRCDR2_AICDC_SHIFT)
#define IMX_ASRC_ASRCDR2_AOCPC_SHIFT                 (6)
#define IMX_ASRC_ASRCDR2_AOCPC_MASK                  (IMX_ASRC_ASRCDR2_AxCPx_MASK << IMX_ASRC_ASRCDR2_AOCPC_SHIFT)
#define IMX_ASRC_ASRCDR2_AOCDC_SHIFT                 (9)
#define IMX_ASRC_ASRCDR2_AOCDC_MASK                  (IMX_ASRC_ASRCDR2_AxCPx_MASK << IMX_ASRC_ASRCDR2_AOCDC_SHIFT)
/** ASRC Status Register macros */
#define IMX_ASRC_ASRSTR_AOLE_MASK                    (1 << 6)
#define IMX_ASRC_ASRSTR_AIDUA_MASK                   (1 << 8)
#define IMX_ASRC_ASRSTR_AIDUB_MASK                   (1 << 9)
#define IMX_ASRC_ASRSTR_AIDUC_MASK                   (1 << 10)
#define IMX_ASRC_ASRSTR_AODOA_MASK                   (1 << 11)
#define IMX_ASRC_ASRSTR_AODOB_MASK                   (1 << 12)
#define IMX_ASRC_ASRSTR_AODOC_MASK                   (1 << 13)
#define IMX_ASRC_ASRSTR_ATQOL_MASK                   (1 << 20)
/** ASRC ASRC Task Queue FIFO Register 1 macros */
#define IMX_ASRC_ASRTFR1_TFBASE_SHIFT                (6)
/** ASRC Data Registers macros */
#define IMX_ASRC_ASRD_IN(index)                      (index * 2)
#define IMX_ASRC_ASRD_OUT(index)                     ((index * 2) + 1)
/** ASRC Ideal Ratio macros */
#define IMX_ASRC_ASRIDR_HIGH(index)                  (index * 2)
#define IMX_ASRC_ASRIDR_LOW(index)                   ((index * 2) + 1)
#define IMX_ASRC_ASRIDR_FRACT_BITS                   (26)
/** ASRC Misc Control Register for Pair A macros */
#define IMX_ASRC_ASRMCRA_RSYNOFA_SHIFT               (10)
#define IMX_ASRC_ASRMCRA_RSYNOFA_MASK                (1 << IMX_ASRC_ASRMCRA_RSYNOFA_SHIFT)
#define IMX_ASRC_ASRMCRA_RSYNIFA_SHIFT               (11)
#define IMX_ASRC_ASRMCRA_RSYNIFA_MASK                (1 << IMX_ASRC_ASRMCRA_RSYNIFA_SHIFT)
#define IMX_ASRC_ASRMCRA_BYPASSPOLYA_SHIFT           (20)
#define IMX_ASRC_ASRMCRA_BYPASSPOLYA_MASK            (1 << IMX_ASRC_ASRMCRA_BYPASSPOLYA_SHIFT)
#define IMX_ASRC_ASRMCRA_BUFSTALLA_SHIFT             (21)
#define IMX_ASRC_ASRMCRA_BUFSTALLA_MASK              (1 << IMX_ASRC_ASRMCRA_BUFSTALLA_SHIFT)
#define IMX_ASRC_ASRMCRA_INFIFO_THRESHOLD_SHIFT      (0)
#define IMX_ASRC_ASRMCRA_INFIFO_THRESHOLD_MASK       (0x3F << IMX_ASRC_ASRMCRA_INFIFO_THRESHOLD_SHIFT)
#define IMX_ASRC_ASRMCRA_OUTFIFO_THRESHOLD_SHIFT     (12)
#define IMX_ASRC_ASRMCRA_OUTFIFO_THRESHOLD_MASK      (0x3F << IMX_ASRC_ASRMCRA_OUTFIFO_THRESHOLD_SHIFT)
/** ASRC Misc Control Register for Pair B macros */
#define IMX_ASRC_ASRMCRB_RSYNOFB_SHIFT               (10)
#define IMX_ASRC_ASRMCRB_RSYNOFB_MASK                (1 << IMX_ASRC_ASRMCRB_RSYNOFB_SHIFT)
#define IMX_ASRC_ASRMCRB_RSYNIFB_SHIFT               (11)
#define IMX_ASRC_ASRMCRB_RSYNIFB_MASK                (1 << IMX_ASRC_ASRMCRB_RSYNIFB_SHIFT)
#define IMX_ASRC_ASRMCRB_BYPASSPOLYB_SHIFT           (20)
#define IMX_ASRC_ASRMCRB_BYPASSPOLYB_MASK            (1 << IMX_ASRC_ASRMCRB_BYPASSPOLYB_SHIFT)
#define IMX_ASRC_ASRMCRB_BUFSTALLB_SHIFT             (21)
#define IMX_ASRC_ASRMCRB_BUFSTALLB_MASK              (1 << IMX_ASRC_ASRMCRB_BUFSTALLB_SHIFT)
#define IMX_ASRC_ASRMCRB_INFIFO_THRESHOLD_SHIFT      (0)
#define IMX_ASRC_ASRMCRB_INFIFO_THRESHOLD_MASK       (0x3F << IMX_ASRC_ASRMCRB_INFIFO_THRESHOLD_SHIFT)
#define IMX_ASRC_ASRMCRB_OUTFIFO_THRESHOLD_SHIFT     (12)
#define IMX_ASRC_ASRMCRB_OUTFIFO_THRESHOLD_MASK      (0x3F << IMX_ASRC_ASRMCRB_OUTFIFO_THRESHOLD_SHIFT)
/** ASRC Misc Control Register for Pair C macros */
#define IMX_ASRC_ASRMCRC_RSYNOFC_SHIFT               (10)
#define IMX_ASRC_ASRMCRC_RSYNOFC_MASK                (1 << IMX_ASRC_ASRMCRC_RSYNOFC_SHIFT)
#define IMX_ASRC_ASRMCRC_RSYNIFC_SHIFT               (11)
#define IMX_ASRC_ASRMCRC_RSYNIFC_MASK                (1 << IMX_ASRC_ASRMCRC_RSYNIFC_SHIFT)
#define IMX_ASRC_ASRMCRC_BYPASSPOLYC_SHIFT           (20)
#define IMX_ASRC_ASRMCRC_BYPASSPOLYC_MASK            (1 << IMX_ASRC_ASRMCRC_BYPASSPOLYC_SHIFT)
#define IMX_ASRC_ASRMCRC_BUFSTALLC_SHIFT             (21)
#define IMX_ASRC_ASRMCRC_BUFSTALLC_MASK              (1 << IMX_ASRC_ASRMCRC_BUFSTALLC_SHIFT)
#define IMX_ASRC_ASRMCRC_INFIFO_THRESHOLD_SHIFT      (0)
#define IMX_ASRC_ASRMCRC_INFIFO_THRESHOLD_MASK       (0x3F << IMX_ASRC_ASRMCRC_INFIFO_THRESHOLD_SHIFT)
#define IMX_ASRC_ASRMCRC_OUTFIFO_THRESHOLD_SHIFT     (12)
#define IMX_ASRC_ASRMCRC_OUTFIFO_THRESHOLD_MASK      (0x3F << IMX_ASRC_ASRMCRC_OUTFIFO_THRESHOLD_SHIFT)
/** ASRC Misc Control Registers macros */
#define IMX_ASRC_ASRMCR1_OW16_MASK                   (1)
#define IMX_ASRC_ASRMCR1_OMSB_SHIFT                  (2)
#define IMX_ASRC_ASRMCR1_OMSB_MASK                   (1 << IMX_ASRC_ASRMCR1_OMSB_SHIFT)
#define IMX_ASRC_ASRMCR1_IMSB_SHIFT                  (8)
#define IMX_ASRC_ASRMCR1_IMSB_MASK                   (1 << IMX_ASRC_ASRMCR1_IMSB_SHIFT)
#define IMX_ASRC_ASRMCR1_IWD_SHIFT                   (9)
#define IMX_ASRC_ASRMCR1_IWD_MASK                    (3 << IMX_ASRC_ASRMCR1_IWD_SHIFT)

/** ASRC peripheral register structure. */
typedef struct asrc {
    volatile uint32_t   asrctr;     /*!< ASRC Control Register */
    volatile uint32_t   asrier;     /*!< ASRC Interrupt Enable Register */
    volatile uint8_t    rsvd0[4];   /*!< Reserved */
    volatile uint32_t   asrcncr;    /*!< ASRC Channel Number Configuration Register */
    volatile uint32_t   asrcfg;     /*!< ASRC Filter Configuration Status Register */
    volatile uint32_t   asrcsr;     /*!< ASRC Clock Source Register */
    volatile uint32_t   asrcdr1;    /*!< ASRC Clock Divider Register 1 */
    volatile uint32_t   asrcdr2;    /*!< ASRC Clock Divider Register 2 */
    volatile uint32_t   asrstr;     /*!< ASRC Status Register */
    volatile uint8_t    rsvd1[28];  /*!< Reserved */
    volatile uint32_t   asrpmn1;    /*!< ASRC Parameter Register 1 */
    volatile uint32_t   asrpmn2;    /*!< ASRC Parameter Register 2 */
    volatile uint32_t   asrpmn3;    /*!< ASRC Parameter Register 3 */
    volatile uint32_t   asrpmn4;    /*!< ASRC Parameter Register 4 */
    volatile uint32_t   asrpmn5;    /*!< ASRC Parameter Register 5 */
    volatile uint32_t   asrtfr1;    /*!< ASRC ASRC Task Queue FIFO Register 1 */
    volatile uint8_t    rsvd2[4];   /*!< Reserved */
    volatile uint32_t   asrccr;     /*!< ASRC Channel Counter Register */
    volatile uint32_t   asrd[6];    /*!< ASRC Data Registers */
    volatile uint8_t    rsvd3[8];   /*!< Reserved */
    volatile uint32_t   asridr[6];  /*!< ASRC Ideal Ratio */
    volatile uint32_t   asr76k;     /*!< ASRC 76kHz Period in terms of ASRC processing clock */
    volatile uint32_t   asr56k;     /*!< ASRC 56kHz Period in terms of ASRC processing clock */
    volatile uint32_t   asrmcra;    /*!< ASRC Misc Control Register for Pair A */
    volatile uint32_t   asrfsta;    /*!< ASRC FIFO Status Register for Pair A */
    volatile uint32_t   asrmcrb;    /*!< ASRC Misc Control Register for Pair B */
    volatile uint32_t   asrfstb;    /*!< ASRC FIFO Status Register for Pair B */
    volatile uint32_t   asrmcrc;    /*!< ASRC Misc Control Register for Pair C */
    volatile uint32_t   asrfstc;    /*!< ASRC FIFO Status Register for Pair C */
    volatile uint8_t    rsvd4[8];   /*!< Reserved */
    volatile uint32_t   asrmcr1[3]; /*!< ASRC Misc Control Registers */
} imx_asrc_t;

typedef enum _imx_asrc0_mux {
    asrc0_mux_aud_pll_div_clk0 = 0,   /**< DSC clock output */
    asrc0_mux_aud_pll_div_clk1,       /**< DSC clock output */
    asrc0_mux_aud_clk0,               /**< Audio Clock Multiplexer 0 output */
    asrc0_mux_aud_clk1,               /**< Audio Clock Multiplexer 1 output */
    asrc0_mux_esai0_rx_clk,           /**< ESAI0 Receiver serial bit clock */
    asrc0_mux_esai0_tx_clk,           /**< ESAI0 Transmitter serial bit clock */
    asrc0_mux_spdif0_rx,              /**< SPDIF0 Receiver recovered clock */
    asrc0_mux_spdif1_rx,              /**< SPDIF1 Receiver recovered clock */
    asrc0_mux_sai0_rx_bclk,           /**< SAI0 Receiver serial bit clock */
    asrc0_mux_sai0_tx_bclk,           /**< SAI0 Transmitter serial bit clock */
    asrc0_mux_sai1_rx_bclk,           /**< SAI1 Receiver serial bit clock */
    asrc0_mux_sai1_tx_bclk,           /**< SAI1 Transmitter serial bit clock */
    asrc0_mux_sai2_rx_bclk,           /**< SAI2 Receiver serial bit clock */
    asrc0_mux_sai3_rx_bclk,           /**< SAI3 Receiver serial bit clock */
    asrc0_mux_asrc_presel             /**< ASRC0 pre-selector */
} imx_asrc0_mux_t;

typedef enum _imx_asrc1_mux {
    asrc1_mux_aud_pll_div_clk0 = 0,   /**< DSC clock output */
    asrc1_mux_aud_pll_div_clk1,       /**< DSC clock output */
    asrc1_mux_aud_clk0,               /**< Audio Clock Multiplexer 0 output */
    asrc1_mux_aud_clk1,               /**< Audio Clock Multiplexer 1 output */
    asrc1_mux_esai1_rx_clk,           /**< ESAI1 Receiver serial bit clock */
    asrc1_mux_esai1_tx_clk,           /**< ESAI1 Receiver serial bit clock */
    asrc1_mux_sai6_tx_bclk,           /**< SAI6 Transmitter serial bit clock */
    asrc1_mux_mlb_clk,                /**< Media Local Bus clock */
    asrc1_mux_hdmi_rx_sai0_rx_bclk,   /**< HDMI_RX_SAI0 Receiver serial bit clock (HDMI Rx) */
    asrc1_mux_hdmi_tx_sai0_tx_bclk,   /**< HDMI_TX_SAI0 Transmitter serial bit clock (HDMI Tx) */
    asrc1_mux_reserved,
    asrc1_mux_esai0_rx_clk,           /**< ESAI0 Receiver external serial bit clock */
    asrc1_mux_esai0_tx_clk,           /**< ESAI0 Transmitter serial bit clock */
    asrc1_mux_sai0_rx_bclk,           /**< SAI0 Receiver serial bit clock */
    asrc1_mux_sai0_tx_bclk,           /**< SAI0 Transmitter serial bit clock */
} imx_asrc1_mux_t;

#endif /* IMX_ASRC_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/aarch64/imx8/imx8_asrc.h $ $Rev: 842080 $")
#endif
