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

#ifndef IMX_EDMA_CHANNEL_H_
#define IMX_EDMA_CHANNEL_H_

/* eDMA_Channel registers, offset from base address */
#define IMX_DMA_CHN_CSR               0       /* Channel Control and Status offset */
#define IMX_DMA_CHN_ES                0x0004  /* Channel Error Status offset */
#define IMX_DMA_CHN_INT               0x0008  /* Channel Interrupt Status offset */
#define IMX_DMA_CHN_SBR               0x000C  /* Channel System Bus offset */
#define IMX_DMA_CHN_PRI               0x0010  /* Channel Priority offset */
#define IMX_DMA_TCDn_SADDR            0x0020  /* TCD Source Address offset */
#define IMX_DMA_TCDn_SOFF             0x0024  /* TCD Source Address Offset offset */
#define IMX_DMA_TCDn_ATTR             0x0026  /* TCD Transfer Attributes offset */
#define IMX_DMA_TCDn_NBYTES_MLOFFNO   0x0028  /* TCD Signed Minor Loop Offset (Minor Loop Mapping Enabled and Offset Disabled) offset */
#define IMX_DMA_TCDn_NBYTES_MLOFFYES  0x0028  /* TCD Signed Minor Loop Offset (Minor Loop Mapping and Offset Enabled) offset */
#define IMX_DMA_TCDn_SLAST_SDA        0x002C  /* TCD Last Source Address Adjustment / Store DADDR Address offset */
#define IMX_DMA_TCDn_DADDR            0x0030  /* TCD Destination Address offset */
#define IMX_DMA_TCDn_DOFF             0x0034  /* TCD Destination Address Offset offset */
#define IMX_DMA_TCDn_CITER_ELINKYES   0x0036  /* TCD Current Minor Loop Link, Major Loop Count (Channel Linking Enabled) offset */
#define IMX_DMA_TCDn_CITER_ELINKNO    0x0036  /* TCD Current Minor Loop Link, Major Loop Count (Channel Linking Disabled) offset */
#define IMX_DMA_TCDn_DLAST_SGA        0x0038  /* TCD Last Destination Address Adjustment/Scatter Gather Address offset */
#define IMX_DMA_TCDn_CSR              0x003C  /* TCD Control and Status offset */
#define IMX_DMA_TCDn_BITER_ELINKYES   0x003E  /* TCD Beginning Minor Loop Link, Major Loop Count (Channel Linking Enabled) offset */
#define IMX_DMA_TCDn_BITER_ELINKNO    0x003E  /* TCD Beginning Minor Loop Link, Major Loop Count (Channel Linking Disabled)) offset */

/*
 * DMA_CHN_CSR register bits *
 */
#define IMX_DMA_CHN_CSR_ACTIVE_MASK                0x80000000
#define IMX_DMA_CHN_CSR_ACTIVE_SHIFT               31
#define IMX_DMA_CHN_CSR_DONE_MASK                  0x40000000
#define IMX_DMA_CHN_CSR_DONE_SHIFT                 30
#define IMX_DMA_CHN_CSR_EEI_MASK                   0x00000004
#define IMX_DMA_CHN_CSR_EEI_SHIFT                  2
#define IMX_DMA_CHN_CSR_EARQ_MASK                  0x00000002
#define IMX_DMA_CHN_CSR_EARQ_SHIFT                 1
#define IMX_DMA_CHN_CSR_ERQ_MASK                   0x00000001
#define IMX_DMA_CHN_CSR_ERQ_SHIFT                  0

/*
 * DMA_CHN_ES register bits *
 */
#define IMX_DMA_CHN_ES__MASK                       0x80000000
#define IMX_DMA_CHN_ES__SHIFT                      31
#define IMX_DMA_CHN_ES_SAE_MASK                    0x00000080
#define IMX_DMA_CHN_ES_SAE_SHIFT                   7
#define IMX_DMA_CHN_ES_SOE_MASK                    0x00000040
#define IMX_DMA_CHN_ES_SOE_SHIFT                   6
#define IMX_DMA_CHN_ES_DAE_MASK                    0x00000020
#define IMX_DMA_CHN_ES_DAE_SHIFT                   5
#define IMX_DMA_CHN_ES_DOE_MASK                    0x00000010
#define IMX_DMA_CHN_ES_DOE_SHIFT                   4
#define IMX_DMA_CHN_ES_NCE_MASK                    0x00000008
#define IMX_DMA_CHN_ES_NCE_SHIFT                   3
#define IMX_DMA_CHN_ES_SGE_MASK                    0x00000004
#define IMX_DMA_CHN_ES_SGE_SHIFT                   2
#define IMX_DMA_CHN_ES_SBE_MASK                    0x00000002
#define IMX_DMA_CHN_ES_SBE_SHIFT                   1
#define IMX_DMA_CHN_ES_DBE_MASK                    0x00000001
#define IMX_DMA_CHN_ES_DBE_SHIFT                   0

/*
 * DMA_CHN_INT register bits *
 */
#define IMX_DMA_CHN_INT_INT_MASK                   0x00000001
#define IMX_DMA_CHN_INT_INT_SHIFT                  0

/*
 * DMA_CHN_SBR register bits *
 */
#define IMX_DMA_CHN_SBR_ATTR_MASK                  0x007E0000
#define IMX_DMA_CHN_SBR_ATTR_SHIFT                 17
#define IMX_DMA_CHN_SBR_PAL_MASK                   0x00008000
#define IMX_DMA_CHN_SBR_PAL_SHIFT                  15
#define IMX_DMA_CHN_SBR_MID_MASK                   0x0000001F
#define IMX_DMA_CHN_SBR_MID_SHIFT                  0

/*
 * DMA_CHN_PRI register bits *
 */
#define IMX_DMA_CHN_PRI_ECP_MASK                   0x80000000
#define IMX_DMA_CHN_PRI_ECP_SHIFT                  31
#define IMX_DMA_CHN_PRI_DPA_MASK                   0x40000000
#define IMX_DMA_CHN_PRI_DPA_SHIFT                  30
#define IMX_DMA_CHN_PRI_ALP_MASK                   0x00000007
#define IMX_DMA_CHN_PRI_ALP_SHIFT                  0

/*
 * DMA_TCDn_SADDR register bits *
 */
#define IMX_DMA_TCDn_SADDR_SADDR_MASK              0x00000000
#define IMX_DMA_TCDn_SADDR_SADDR_SHIFT             0

/*
 * DMA_TCDn_SOFF register bits *
 */
#define IMX_DMA_TCDn_SOFF_SOF_MASK                 0x0000FFFF
#define IMX_DMA_TCDn_SOFF_SOF_SHIFT                0

/*
 * DMA_TCDn_ATTR register bits *
 */
#define IMX_DMA_TCDn_ATTR_SMOD_MASK                0x0000F800
#define IMX_DMA_TCDn_ATTR_SMOD_SHIFT               11
#define IMX_DMA_TCDn_ATTR_SSIZE_MASK               0x00000700
#define IMX_DMA_TCDn_ATTR_SSIZE_SHIFT              8
#define IMX_DMA_TCDn_ATTR_DMOD_MASK                0x000000F8
#define IMX_DMA_TCDn_ATTR_DMOD_SHIFT               3
#define IMX_DMA_TCDn_ATTR_DSIZE_MASK               0x00000007
#define IMX_DMA_TCDn_ATTR_DSIZE_SHIFT              0

/*
 * DMA_TCDn_NBYTES_MLOFFNO register bits *
 */
#define IMX_DMA_TCDn_NBYTES_MLOFFNO_SMLOE_MASK     0x80000000
#define IMX_DMA_TCDn_NBYTES_MLOFFNO_SMLOE_SHIFT    31
#define IMX_DMA_TCDn_NBYTES_MLOFFNO_DMLOE_MASK     0x40000000
#define IMX_DMA_TCDn_NBYTES_MLOFFNO_DMLOE_SHIFT    30
#define IMX_DMA_TCDn_NBYTES_MLOFFNO_NBYTES_MASK    0x3FFFFFFF
#define IMX_DMA_TCDn_NBYTES_MLOFFNO_NBYTES_SHIFT   0

/*
 * DMA_TCDn_NBYTES_MLOFFYES register bits *
 */
#define IMX_DMA_TCDn_NBYTES_MLOFFYES_SMLOE_MASK    0x80000000
#define IMX_DMA_TCDn_NBYTES_MLOFFYES_SMLOE_SHIFT   31
#define IMX_DMA_TCDn_NBYTES_MLOFFYES_DMLOE_MASK    0x40000000
#define IMX_DMA_TCDn_NBYTES_MLOFFYES_DMLOE_SHIFT   30
#define IMX_DMA_TCDn_NBYTES_MLOFFYES_MLOFF_MASK    0x3FFFFC00
#define IMX_DMA_TCDn_NBYTES_MLOFFYES_MLOFF_SHIFT   10
#define IMX_DMA_TCDn_NBYTES_MLOFFYES_NBYTES_MASK   0x000003FF
#define IMX_DMA_TCDn_NBYTES_MLOFFYES_NBYTES_SHIFT  0

/*
 * DMA_TCDn_SLAST_SDA register bits *
 */
#define IMX_DMA_TCDn_SLAST_SDA_SLAST_MASK          0x00000000
#define IMX_DMA_TCDn_SLAST_SDA_SLAST_SHIFT         0

/*
 * DMA_TCDn_DADDR register bits *
 */
#define IMX_DMA_TCDn_DADDR_DADDR_MASK              0x00000000
#define IMX_DMA_TCDn_DADDR_DADDR_SHIFT             0

/*
 * DMA_TCDn_DOFF register bits *
 */
#define IMX_DMA_TCDn_DOFF_DOFF_MASK                0x0000FFFF
#define IMX_DMA_TCDn_DOFF_DOFF_SHIFT               0

/*
 * DMA_TCDn_CITER_ELINKYES register bits *
 */
#define IMX_DMA_TCDn_CITER_ELINKYES_ELINK_MASK     0x00008000
#define IMX_DMA_TCDn_CITER_ELINKYES_ELINK_SHIFT    15
#define IMX_DMA_TCDn_CITER_ELINKYES_LINKCH_MASK    0x00003E00
#define IMX_DMA_TCDn_CITER_ELINKYES_LINKCH_SHIFT   9
#define IMX_DMA_TCDn_CITER_ELINKYES_CITER_MASK     0x000001FF
#define IMX_DMA_TCDn_CITER_ELINKYES_CITER_SHIFT    0

/*
 * DMA_TCDn_CITER_ELINKNO register bits *
 */
#define IMX_DMA_TCDn_CITER_ELINKNO_ELINK_MASK      0x00008000
#define IMX_DMA_TCDn_CITER_ELINKNO_ELINK_SHIFT     15
#define IMX_DMA_TCDn_CITER_ELINKNO_CITER_MASK      0x00007FFF
#define IMX_DMA_TCDn_CITER_ELINKNO_CITER_SHIFT     0

/*
 * DMA_TCDn_DLAST_SGA register bits *
 */
#define IMX_DMA_TCDn_DLAST_SGA_DLAST_SGA_MASK      0x00000000
#define IMX_DMA_TCDn_DLAST_SGA_DLAST_SGA_SHIFT     0

/*
 * DMA_TCDn_CSR register bits *
 */
#define IMX_DMA_TCDn_CSR_BWC_MASK                  0x0000C000
#define IMX_DMA_TCDn_CSR_BWC_SHIFT                 14
#define IMX_DMA_TCDn_CSR_MAJORLINKCH_MASK          0x00001F00
#define IMX_DMA_TCDn_CSR_MAJORLINKCH_SHIFT         8
#define IMX_DMA_TCDn_CSR_ESDA_MASK                 0x00000080
#define IMX_DMA_TCDn_CSR_ESDA_SHIFT                7
#define IMX_DMA_TCDn_CSR_EEOP_MASK                 0x00000040
#define IMX_DMA_TCDn_CSR_EEOP_SHIFT                6
#define IMX_DMA_TCDn_CSR_MAJORELINK_MASK           0x00000020
#define IMX_DMA_TCDn_CSR_MAJORELINK_SHIFT          5
#define IMX_DMA_TCDn_CSR_ESG_MASK                  0x00000010
#define IMX_DMA_TCDn_CSR_ESG_SHIFT                 4
#define IMX_DMA_TCDn_CSR_DREQ_MASK                 0x00000008
#define IMX_DMA_TCDn_CSR_DREQ_SHIFT                3
#define IMX_DMA_TCDn_CSR_INTHALF_MASK              0x00000004
#define IMX_DMA_TCDn_CSR_INTHALF_SHIFT             2
#define IMX_DMA_TCDn_CSR_INTMAJOR_MASK             0x00000002
#define IMX_DMA_TCDn_CSR_INTMAJOR_SHIFT            1
#define IMX_DMA_TCDn_CSR_START_MASK                0x00000001
#define IMX_DMA_TCDn_CSR_START_SHIFT               0

/*
 * DMA_TCDn_BITER_ELINKYES register bits *
 */
#define IMX_DMA_TCDn_BITER_ELINKYES_ELINK_MASK     0x00008000
#define IMX_DMA_TCDn_BITER_ELINKYES_ELINK_SHIFT    15
#define IMX_DMA_TCDn_BITER_ELINKYES_LINKCH_MASK    0x00003E00
#define IMX_DMA_TCDn_BITER_ELINKYES_LINKCH_SHIFT   9
#define IMX_DMA_TCDn_BITER_ELINKYES_BITER_MASK     0x000001FF
#define IMX_DMA_TCDn_BITER_ELINKYES_BITER_SHIFT    0

/*
 * DMA_TCDn_BITER_ELINKNO register bits *
 */
#define IMX_DMA_TCDn_BITER_ELINKNO_ELINK_MASK      0x00008000
#define IMX_DMA_TCDn_BITER_ELINKNO_ELINK_SHIFT     15
#define IMX_DMA_TCDn_BITER_ELINKNO_BITER_MASK      0x00007FFF
#define IMX_DMA_TCDn_BITER_ELINKNO_BITER_SHIFT     0


typedef volatile uint8_t vuint8_t;
typedef volatile uint16_t vuint16_t;
typedef volatile uint32_t vuint32_t;
/*
 * DMA_CHN_CSR register bits definition *
 */
typedef union IMX_DMA_CHN_CSR_union_t {
    vuint32_t R;
    struct {
        vuint32_t ERQ         :  1;  /* Enable DMA Request. */
        vuint32_t EARQ        :  1;  /* Enable Asynchronous DMA Request in stop mode for channel */
        vuint32_t EEI         :  1;  /* Enable Error Interrupt */
        vuint32_t Reserved_3  : 27;  /* Reserved */
        vuint32_t DONE        :  1;  /* Channel Done. */
        vuint32_t ACTIVE      :  1;  /* Channel Active. */
    } B;
} IMX_DMA_CHN_CSR_t;

/*
 * DMA_CHN_ES register bits definition *
 */
typedef union IMX_DMA_CHN_ES_union_t {
    vuint32_t R;
    struct {
        vuint32_t DBE         :  1;  /* Destination Bus Error. */
        vuint32_t SBE         :  1;  /* Source Bus Error. */
        vuint32_t SGE         :  1;  /* Scatter/Gather Configuration Error. */
        vuint32_t NCE         :  1;  /* NBYTES/CITER Configuration Error. */
        vuint32_t DOE         :  1;  /* Destination Offset Error. */
        vuint32_t DAE         :  1;  /* Destination Address Error. */
        vuint32_t SOE         :  1;  /* Source Offset Error. */
        vuint32_t SAE         :  1;  /* Source Address Error. */
        vuint32_t Reserved_8  : 23;  /* Reserved */
        vuint32_t ERR         :  1;  /* Error In Channel */
    } B;
} IMX_DMA_CHN_ES_t;

/*
 * DMA_CHN_INT register bits definition *
 */
typedef union IMX_DMA_CHN_INT_union_t {
    vuint32_t R;
    struct {
        vuint32_t INT         :  1;  /* Interrupt Request. */
        vuint32_t Reserved_1  : 31;  /* Reserved */
    } B;
} IMX_DMA_CHN_INT_t;

/*
 * DMA_CHN_SBR register bits definition *
 */
typedef union IMX_DMA_CHN_SBR_union_t {
    vuint32_t R;
    struct {
        vuint32_t MID          :  5;  /* Master ID. */
        vuint32_t Reserved_5   : 10;  /* Reserved */
        vuint32_t PAL          :  1;  /* Privileged Access Level. */
        vuint32_t Reserved_16  :  1;  /* Reserved */
        vuint32_t ATTR         :  6;  /* Attribute Outpu */
        vuint32_t Reserved_23  :  9;  /* Reserved */
    } B;
} IMX_DMA_CHN_SBR_t;

/*
 * DMA_CHN_PRI register bits definition *
 */
typedef union IMX_DMA_CHN_PRI_union_t {
    vuint32_t R;
    struct {
        vuint32_t ALP         :  3;  /* Channel n Arbitration Priority Level */
        vuint32_t Reserved_3  : 27;  /* Reserved */
        vuint32_t DPA         :  1;  /* Disable Preempt Ability. */
        vuint32_t ECP         :  1;  /* Enable Channel Preemption. */
    } B;
} IMX_DMA_CHN_PRI_t;

/*
 * DMA_TCDn_SADDR register bits definition *
 */
typedef union IMX_DMA_TCDn_SADDR_union_t {
    vuint32_t R;
    struct {
        vuint32_t SADDR  : 32;  /* Source Address */
    } B;
} IMX_DMA_TCDn_SADDR_t;

/*
 * DMA_TCDn_SOFF register bits definition *
 */
typedef union IMX_DMA_TCDn_SOFF_union_t {
    vuint16_t R;
    struct {
        vuint16_t SOF  : 16;  /* Source address signed offset. */
    } B;
} IMX_DMA_TCDn_SOFF_t;

/*
 * DMA_TCDn_ATTR register bits definition *
 */
typedef union IMX_DMA_TCDn_ATTR_union_t {
    vuint16_t R;
    struct {
        vuint16_t DSIZE  :  3;  /* Destination data transfer size. */
        vuint16_t DMOD   :  5;  /* Destination Address Modulo. */
        vuint16_t SSIZE  :  3;  /* Source data transfer size. */
        vuint16_t SMOD   :  5;  /* Source Address Modulo. */
    } B;
} IMX_DMA_TCDn_ATTR_t;

/*
 * DMA_TCDn_NBYTES_MLOFFNO register bits definition *
 */
typedef union IMX_DMA_TCDn_NBYTES_MLOFFNO_union_t {
    vuint32_t R;
    struct {
        vuint32_t NBYTES  : 30;  /* Minor Byte Transfer Count. */
        vuint32_t DMLOE   :  1;  /* Destination Minor Loop Offset enable. */
        vuint32_t SMLOE   :  1;  /* Source Minor Loop Offset Enable. */
    } B;
} IMX_DMA_TCDn_NBYTES_MLOFFNO_t;

/*
 * DMA_TCDn_NBYTES_MLOFFYES register bits definition *
 */
typedef union IMX_DMA_TCDn_NBYTES_MLOFFYES_union_t {
    vuint32_t R;
    struct {
        vuint32_t NBYTES  : 10;  /* Minor Byte Transfer Count. */
        vuint32_t MLOFF   : 20;  /* Minor Loop Offset. */
        vuint32_t DMLOE   :  1;  /* Destination Minor Loop Offset enable. */
        vuint32_t SMLOE   :  1;  /* Source Minor Loop Offset Enable. */
    } B;
} IMX_DMA_TCDn_NBYTES_MLOFFYES_t;

/*
 * DMA_TCDn_SLAST_SDA register bits definition *
 */
typedef union IMX_DMA_TCDn_SLAST_SDA_union_t {
    vuint32_t R;
    struct {
        vuint32_t SLAST  : 32;  /* Last Source Address Adjustment / Store DADDR Address */
    } B;
} IMX_DMA_TCDn_SLAST_SDA_t;

/*
 * DMA_TCDn_DADDR register bits definition *
 */
typedef union IMX_DMA_TCDn_DADDR_union_t {
    vuint32_t R;
    struct {
        vuint32_t DADDR  : 32;  /* Source Address */
    } B;
} IMX_DMA_TCDn_DADDR_t;

/*
 * DMA_TCDn_DOFF register bits definition *
 */
typedef union IMX_DMA_TCDn_DOFF_union_t {
    vuint16_t R;
    struct {
        vuint16_t DOFF  : 16;  /* Source address signed offset. */
    } B;
} IMX_DMA_TCDn_DOFF_t;

/*
 * DMA_TCDn_CITER_ELINKYES register bits definition *
 */
typedef union IMX_DMA_TCDn_CITER_ELINKYES_union_t {
    vuint16_t R;
    struct {
        vuint16_t CITER        :  9;  /* Current Major Iteration Count. */
        vuint16_t LINKCH       :  5;  /* Link Channel Number. */
        vuint16_t Reserved_14  :  1;  /* Reserved */
        vuint16_t ELINK        :  1;  /* Enable channel-to-channel linking on minor-loop complete. */
    } B;
} IMX_DMA_TCDn_CITER_ELINKYES_t;

/*
 * DMA_TCDn_CITER_ELINKNO register bits definition *
 */
typedef union IMX_DMA_TCDn_CITER_ELINKNO_union_t {
    vuint16_t R;
    struct {
        vuint16_t CITER  : 15;  /* Current Major Iteration Count. */
        vuint16_t ELINK  :  1;  /* Enable channel-to-channel linking on minor-loop complete. */
    } B;
} IMX_DMA_TCDn_CITER_ELINKNO_t;

/*
 * DMA_TCDn_DLAST_SGA register bits definition *
 */
typedef union IMX_DMA_TCDn_DLAST_SGA_union_t {
    vuint32_t R;
    struct {
        vuint32_t DLAST_SGA  : 32;  /* Destination last address adjustment or the memory address for the next transfer control descriptor to be loaded into this channel (scatter/gather). */
    } B;
} IMX_DMA_TCDn_DLAST_SGA_t;

/*
 * DMA_TCDn_CSR register bits definition *
 */
typedef union IMX_DMA_TCDn_CSR_union_t {
    vuint16_t R;
    struct {
        vuint16_t START        :  1;  /* Channel Start */
        vuint16_t INTMAJOR     :  1;  /* Enable an interrupt when major iteration count completes */
        vuint16_t INTHALF      :  1;  /* Enable an interrupt when major counter is half complete. */
        vuint16_t DREQ         :  1;  /* Disable Request */
        vuint16_t ESG          :  1;  /* Enable Scatter/Gather Processing */
        vuint16_t MAJORELINK   :  1;  /* Enable channel-to-channel linking on major loop complete */
        vuint16_t EEOP         :  1;  /* Enable end-of-packet processing */
        vuint16_t ESDA         :  1;  /* Enable store destination address */
        vuint16_t MAJORLINKCH  :  5;  /* Link Channel Number */
        vuint16_t Reserved_13  :  1;  /* Reserved */
        vuint16_t BWC          :  2;  /* Bandwidth Control */
    } B;
} IMX_DMA_TCDn_CSR_t;

/*
 * DMA_TCDn_BITER_ELINKYES register bits definition *
 */
typedef union IMX_DMA_TCDn_BITER_ELINKYES_union_t {
    vuint16_t R;
    struct {
        vuint16_t BITER        :  9;  /* Starting Major Iteration Count. */
        vuint16_t LINKCH       :  5;  /* Link Channel Number. */
        vuint16_t Reserved_14  :  1;  /* Reserved */
        vuint16_t ELINK        :  1;  /* Enable channel-to-channel linking on minor-loop complete. */
    } B;
} IMX_DMA_TCDn_BITER_ELINKYES_t;

/*
 * DMA_TCDn_BITER_ELINKNO register bits definition *
 */
typedef union IMX_DMA_TCDn_BITER_ELINKNO_union_t {
    vuint16_t R;
    struct {
        vuint16_t BITER  : 15;  /* Starting Major Iteration Count. */
        vuint16_t ELINK  :  1;  /* Enable channel-to-channel linking on minor-loop complete. */
    } B;
} IMX_DMA_TCDn_BITER_ELINKNO_t;

typedef struct IMX_eDMA_TCD_struct_t {
    IMX_DMA_TCDn_SADDR_t                DMA_TCDn_SADDR;            /* 0x00000020 TCD Source Address */
    IMX_DMA_TCDn_SOFF_t                 DMA_TCDn_SOFF;             /* 0x00000024 TCD Source Address Offset */
    IMX_DMA_TCDn_ATTR_t                 DMA_TCDn_ATTR;             /* 0x00000026 TCD Transfer Attributes */
    union {
        IMX_DMA_TCDn_NBYTES_MLOFFNO_t   DMA_TCDn_NBYTES_MLOFFNO;   /* 0x00000028 TCD Signed Minor Loop Offset (Minor Loop Mapping Enabled and Offset Disabled) */
        IMX_DMA_TCDn_NBYTES_MLOFFYES_t  DMA_TCDn_NBYTES_MLOFFYES;  /* 0x00000028 TCD Signed Minor Loop Offset (Minor Loop Mapping and Offset Enabled) */
    };
    IMX_DMA_TCDn_SLAST_SDA_t            DMA_TCDn_SLAST_SDA;        /* 0x0000002C TCD Last Source Address Adjustment / Store DADDR Address */
    IMX_DMA_TCDn_DADDR_t                DMA_TCDn_DADDR;            /* 0x00000030 TCD Destination Address */
    IMX_DMA_TCDn_DOFF_t                 DMA_TCDn_DOFF;             /* 0x00000034 TCD Destination Address Offset */
    union {
        IMX_DMA_TCDn_CITER_ELINKYES_t   DMA_TCDn_CITER_ELINKYES;   /* 0x00000036 TCD Current Minor Loop Link, Major Loop Count (Channel Linking Enabled) */
        IMX_DMA_TCDn_CITER_ELINKNO_t    DMA_TCDn_CITER_ELINKNO;    /* 0x00000036 TCD Current Minor Loop Link, Major Loop Count (Channel Linking Disabled) */
    };
    IMX_DMA_TCDn_DLAST_SGA_t            DMA_TCDn_DLAST_SGA;        /* 0x00000038 TCD Last Destination Address Adjustment/Scatter Gather Address */
    IMX_DMA_TCDn_CSR_t                  DMA_TCDn_CSR;              /* 0x0000003C TCD Control and Status */
    union {
        IMX_DMA_TCDn_BITER_ELINKYES_t   DMA_TCDn_BITER_ELINKYES;   /* 0x0000003E TCD Beginning Minor Loop Link, Major Loop Count (Channel Linking Enabled) */
        IMX_DMA_TCDn_BITER_ELINKNO_t    DMA_TCDn_BITER_ELINKNO;    /* 0x0000003E TCD Beginning Minor Loop Link, Major Loop Count (Channel Linking Disabled)) */
    };
} IMX_eDMA_TCD_t;

typedef struct IMX_eDMA_Channel_struct_t {
    IMX_DMA_CHN_CSR_t                   DMA_CHN_CSR;               /* 0x00000000 Channel Control and Status */
    IMX_DMA_CHN_ES_t                    DMA_CHN_ES;                /* 0x00000004 Channel Error Status */
    IMX_DMA_CHN_INT_t                   DMA_CHN_INT;               /* 0x00000008 Channel Interrupt Status */
    IMX_DMA_CHN_SBR_t                   DMA_CHN_SBR;               /* 0x0000000C Channel System Bus */
    IMX_DMA_CHN_PRI_t                   DMA_CHN_PRI;               /* 0x00000010 Channel Priority */
    vuint8_t                            Reserved_20[12];           /* 0x00000014 Reserved */
    IMX_eDMA_TCD_t                      TCD;                       /* 0x20000000 TCD */
} IMX_eDMA_Channel_t;

/*
 * eDMA_Channel peripheral register pointers structure *
 */
typedef struct IMX_eDMA_Channel_Ptr_struct_t {
    IMX_DMA_CHN_CSR_t               *pDMA_CHN_CSR;               /* 0x00000000 Channel Control and Status */
    IMX_DMA_CHN_ES_t                *pDMA_CHN_ES;                /* 0x00000004 Channel Error Status */
    IMX_DMA_CHN_INT_t               *pDMA_CHN_INT;               /* 0x00000008 Channel Interrupt Status */
    IMX_DMA_CHN_SBR_t               *pDMA_CHN_SBR;               /* 0x0000000C Channel System Bus */
    IMX_DMA_CHN_PRI_t               *pDMA_CHN_PRI;               /* 0x00000010 Channel Priority */
    IMX_DMA_TCDn_SADDR_t            *pDMA_TCDn_SADDR;            /* 0x00000020 TCD Source Address */
    IMX_DMA_TCDn_SOFF_t             *pDMA_TCDn_SOFF;             /* 0x00000024 TCD Source Address Offset */
    IMX_DMA_TCDn_ATTR_t             *pDMA_TCDn_ATTR;             /* 0x00000026 TCD Transfer Attributes */
    IMX_DMA_TCDn_NBYTES_MLOFFNO_t   *pDMA_TCDn_NBYTES_MLOFFNO;   /* 0x00000028 TCD Signed Minor Loop Offset (Minor Loop Mapping Enabled and Offset Disabled) */
    IMX_DMA_TCDn_NBYTES_MLOFFYES_t  *pDMA_TCDn_NBYTES_MLOFFYES;  /* 0x00000028 TCD Signed Minor Loop Offset (Minor Loop Mapping and Offset Enabled) */
    IMX_DMA_TCDn_SLAST_SDA_t        *pDMA_TCDn_SLAST_SDA;        /* 0x0000002C TCD Last Source Address Adjustment / Store DADDR Address */
    IMX_DMA_TCDn_DADDR_t            *pDMA_TCDn_DADDR;            /* 0x00000030 TCD Destination Address */
    IMX_DMA_TCDn_DOFF_t             *pDMA_TCDn_DOFF;             /* 0x00000034 TCD Destination Address Offset */
    IMX_DMA_TCDn_CITER_ELINKYES_t   *pDMA_TCDn_CITER_ELINKYES;   /* 0x00000036 TCD Current Minor Loop Link, Major Loop Count (Channel Linking Enabled) */
    IMX_DMA_TCDn_CITER_ELINKNO_t    *pDMA_TCDn_CITER_ELINKNO;    /* 0x00000036 TCD Current Minor Loop Link, Major Loop Count (Channel Linking Disabled) */
    IMX_DMA_TCDn_DLAST_SGA_t        *pDMA_TCDn_DLAST_SGA;        /* 0x00000038 TCD Last Destination Address Adjustment/Scatter Gather Address */
    IMX_DMA_TCDn_CSR_t              *pDMA_TCDn_CSR;              /* 0x0000003C TCD Control and Status */
    IMX_DMA_TCDn_BITER_ELINKYES_t   *pDMA_TCDn_BITER_ELINKYES;   /* 0x0000003E TCD Beginning Minor Loop Link, Major Loop Count (Channel Linking Enabled) */
    IMX_DMA_TCDn_BITER_ELINKNO_t    *pDMA_TCDn_BITER_ELINKNO;    /* 0x0000003E TCD Beginning Minor Loop Link, Major Loop Count (Channel Linking Disabled)) */
} IMX_eDMA_Channel_Ptr_t;

/*
 * eDMA_Channel peripheral register pointers structure initialization example *
 */
#ifdef EDMA_CHANNEL_PTR_ARRAY_INIT

IMX_eDMA_Channel_t *heDMA0 = (IMX_eDMA_Channel_t *)IMX_eDMA0_BASE;

IMX_eDMA_Channel_Ptr_t eDMA0_RegArr = {
    (IMX_DMA_CHN_CSR_t *)             (IMX_eDMA0_BASE + IMX_DMA_CHN_CSR),
    (IMX_DMA_CHN_ES_t *)              (IMX_eDMA0_BASE + IMX_DMA_CHN_ES),
    (IMX_DMA_CHN_INT_t *)             (IMX_eDMA0_BASE + IMX_DMA_CHN_INT),
    (IMX_DMA_CHN_SBR_t *)             (IMX_eDMA0_BASE + IMX_DMA_CHN_SBR),
    (IMX_DMA_CHN_PRI_t *)             (IMX_eDMA0_BASE + IMX_DMA_CHN_PRI),
    (IMX_DMA_TCDn_SADDR_t *)          (IMX_eDMA0_BASE + IMX_DMA_TCDn_SADDR),
    (IMX_DMA_TCDn_SOFF_t *)           (IMX_eDMA0_BASE + IMX_DMA_TCDn_SOFF),
    (IMX_DMA_TCDn_ATTR_t *)           (IMX_eDMA0_BASE + IMX_DMA_TCDn_ATTR),
    (IMX_DMA_TCDn_NBYTES_MLOFFNO_t *) (IMX_eDMA0_BASE + IMX_DMA_TCDn_NBYTES_MLOFFNO),
    (IMX_DMA_TCDn_NBYTES_MLOFFYES_t *)(IMX_eDMA0_BASE + IMX_DMA_TCDn_NBYTES_MLOFFYES),
    (IMX_DMA_TCDn_SLAST_SDA_t *)      (IMX_eDMA0_BASE + IMX_DMA_TCDn_SLAST_SDA),
    (IMX_DMA_TCDn_DADDR_t *)          (IMX_eDMA0_BASE + IMX_DMA_TCDn_DADDR),
    (IMX_DMA_TCDn_DOFF_t *)           (IMX_eDMA0_BASE + IMX_DMA_TCDn_DOFF),
    (IMX_DMA_TCDn_CITER_ELINKYES_t *) (IMX_eDMA0_BASE + IMX_DMA_TCDn_CITER_ELINKYES),
    (IMX_DMA_TCDn_CITER_ELINKNO_t *)  (IMX_eDMA0_BASE + IMX_DMA_TCDn_CITER_ELINKNO),
    (IMX_DMA_TCDn_DLAST_SGA_t *)      (IMX_eDMA0_BASE + IMX_DMA_TCDn_DLAST_SGA),
    (IMX_DMA_TCDn_CSR_t *)            (IMX_eDMA0_BASE + IMX_DMA_TCDn_CSR),
    (IMX_DMA_TCDn_BITER_ELINKYES_t *) (IMX_eDMA0_BASE + IMX_DMA_TCDn_BITER_ELINKYES),
    (IMX_DMA_TCDn_BITER_ELINKNO_t *)  (IMX_eDMA0_BASE + IMX_DMA_TCDn_BITER_ELINKNO)
};

#endif /* EDMA_CHANNEL_PTR_ARRAY_INIT */

#endif /* IMX_EDMA_CHANNEL_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/lib/dma/public/aarch64/imx8/imx8_edma_channel.h $ $Rev: 837687 $")
#endif
