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

#ifndef IMX_EDMA_MP_H_
#define IMX_EDMA_MP_H_

/* eDMA_MP registers, offset from base address */
#define IMX_DMA_MP_CSR                         0       /* Management Page Control offset */
#define IMX_DMA_MP_ES                          0x0004  /* Management Page Error Status offset */
#define IMX_DMA_MP_HRS                         0x000C  /* Management Page Hardware Request Status offset */

/*
 * DMA_MP_CSR register bits *
 */
#define IMX_DMA_MP_CSR_ACTIVE_SHIFT            31
#define IMX_DMA_MP_CSR_ACTIVE_MASK             (1 << IMX_DMA_MP_CSR_ACTIVE_SHIFT)
#define IMX_DMA_MP_CSR_ACTIVE_ID_SHIFT         24
#define IMX_DMA_MP_CSR_ACTIVE_ID_MASK          (0x1F << IMX_DMA_MP_CSR_ACTIVE_ID_SHIFT)
#define IMX_DMA_MP_CSR_CX_SHIFT                9
#define IMX_DMA_MP_CSR_CX_MASK                 (1 << IMX_DMA_MP_CSR_CX_SHIFT)
#define IMX_DMA_MP_CSR_ECX_SHIFT               8
#define IMX_DMA_MP_CSR_ECX_MASK                (1 << IMX_DMA_MP_CSR_ECX_SHIFT)
#define IMX_DMA_MP_CSR_EMI_SHIFT               7
#define IMX_DMA_MP_CSR_EMI_MASK                (1 << IMX_DMA_MP_CSR_EMI_SHIFT)
#define IMX_DMA_MP_CSR_ECL_SHIFT               6
#define IMX_DMA_MP_CSR_ECL_MASK                (1 << IMX_DMA_MP_CSR_ECL_SHIFT)
#define IMX_DMA_MP_CSR_HALT_SHIFT              5
#define IMX_DMA_MP_CSR_HALT_MASK               (1 << IMX_DMA_MP_CSR_HALT_SHIFT)
#define IMX_DMA_MP_CSR_HAE_SHIFT               4
#define IMX_DMA_MP_CSR_HAE_MASK                (1 << IMX_DMA_MP_CSR_HAE_SHIFT)
#define IMX_DMA_MP_CSR_ERCA_SHIFT              2
#define IMX_DMA_MP_CSR_ERCA_MASK               (1 << IMX_DMA_MP_CSR_ERCA_SHIFT)
#define IMX_DMA_MP_CSR_EDBG_SHIFT              1
#define IMX_DMA_MP_CSR_EDBG_MASK               (1 << IMX_DMA_MP_CSR_EDBG_SHIFT)
#define IMX_DMA_MP_CSR_EBW_SHIFT               0
#define IMX_DMA_MP_CSR_EBW_MASK                (1 << IMX_DMA_MP_CSR_EBW_SHIFT)

/*
 * DMA_MP_ES register bits *
 */
#define IMX_DMA_MP_ES_VLD_SHIFT                31
#define IMX_DMA_MP_ES_VLD_MASK                 (1 << IMX_DMA_MP_ES_VLD_SHIFT)
#define IMX_DMA_MP_ES_ERRCHN_SHIFT             24
#define IMX_DMA_MP_ES_ERRCHN_MASK              (0x1F << IMX_DMA_MP_ES_ERRCHN_SHIFT)
#define IMX_DMA_MP_ES_ECX_SHIFT                8
#define IMX_DMA_MP_ES_ECX_MASK                 (1 << IMX_DMA_MP_ES_ECX_SHIFT)
#define IMX_DMA_MP_ES_SAE_SHIFT                7
#define IMX_DMA_MP_ES_SAE_MASK                 (1 << IMX_DMA_MP_ES_SAE_SHIFT)
#define IMX_DMA_MP_ES_SOE_SHIFT                6
#define IMX_DMA_MP_ES_SOE_MASK                 (1 << IMX_DMA_MP_ES_SOE_SHIFT)
#define IMX_DMA_MP_ES_DAE_SHIFT                5
#define IMX_DMA_MP_ES_DAE_MASK                 (1 << IMX_DMA_MP_ES_DAE_SHIFT)
#define IMX_DMA_MP_ES_DOE_SHIFT                4
#define IMX_DMA_MP_ES_DOE_MASK                 (1 << IMX_DMA_MP_ES_DOE_SHIFT)
#define IMX_DMA_MP_ES_NCE_SHIFT                3
#define IMX_DMA_MP_ES_NCE_MASK                 (1 << IMX_DMA_MP_ES_NCE_SHIFT)
#define IMX_DMA_MP_ES_SGE_SHIFT                2
#define IMX_DMA_MP_ES_SGE_MASK                 (1 << IMX_DMA_MP_ES_SGE_SHIFT)
#define IMX_DMA_MP_ES_SBE_SHIFT                1
#define IMX_DMA_MP_ES_SBE_MASK                 (1 << IMX_DMA_MP_ES_SBE_SHIFT)
#define IMX_DMA_MP_ES_DBE_SHIFT                1
#define IMX_DMA_MP_ES_DBE_MASK                 (1 << IMX_DMA_MP_ES_DBE_SHIFT)

typedef volatile uint8_t vuint8_t;
typedef volatile uint16_t vuint16_t;
typedef volatile uint32_t vuint32_t;
/*
 * DMA_MP_CSR register bits definition *
 */
typedef union IMX_DMA_MP_CSR_union_t {
    vuint32_t R;
    struct {
        vuint32_t EBW         :  1;  /* Enable Buffered Writes. */
        vuint32_t EDBG        :  1;  /* Enable Debug. */
        vuint32_t ERCA        :  1;  /* Enable Round Robin Channel Arbitration. */
        vuint32_t Reserved_1  :  1;  /* Reserved. */
        vuint32_t HAE         :  1;  /* Halt After Error. */
        vuint32_t HALT        :  1;  /* Halt DMA Operations. */
        vuint32_t ECL         :  1;  /* Enable Channel Linking. */
        vuint32_t EMI         :  1;  /* Enable Master ID replication. */
        vuint32_t ECX         :  1;  /* Cancel Transfer with Error. */
        vuint32_t CX          :  1;  /* Cancel Transfer. */
        vuint32_t Reserved_2  : 14;  /* Reserved. */
        vuint32_t ACTIVE_ID   :  5;  /* Active Channel ID. */
        vuint32_t Reserved_3  :  2;  /* Reserved. */
        vuint32_t ACTIVE      :  1;  /* Channel Active. */
    } B;
} IMX_DMA_MP_CSR_t;

/*
 * DMA_MP_ES register bits definition *
 */
typedef union IMX_DMA_MP_ES_union_t {
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
        vuint32_t ECX         :  1;  /* Transfer Canceled. */
        vuint32_t Reserved_1  : 15;  /* Reserved. */
        vuint32_t ERRCHN      :  5;  /* Error Channel Number or Canceled Channel Number. */
        vuint32_t Reserved_2  :  2;  /* Reserved. */
        vuint32_t VLD         :  1;  /* Logical OR of all ERR status bits. */
    } B;
} IMX_DMA_MP_ES_t;

/*
 * DMA_MP_HRS register bits definition *
 */
typedef union IMX_DMA_MP_HRS_union_t {
    vuint32_t R;
    struct {
        vuint32_t HRS         :  32;  /* Hardware Request Status. */
    } B;
} IMX_DMA_MP_HRS_t;

/*
 * DMA Management Page Control structure
 */
typedef struct IMX_eDMA_MP_struct_t {
    IMX_DMA_MP_CSR_t          DMA_MP_CSR;                /* 0x00000000 Management Page Control */
    IMX_DMA_MP_ES_t           DMA_MP_ES;                 /* 0x00000004 Management Page Error Status */
    vuint8_t                  Reserved[4];               /* 0x00000008 Reserved */
    IMX_DMA_MP_HRS_t          DMA_MP_HRS;                /* 0x0000000C Management Page Hardware Request Status */
} IMX_eDMA_MP_t;

#endif /* IMX_EDMA_MP_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/lib/dma/public/aarch64/imx8/imx8_edma_mp.h $ $Rev: 837687 $")
#endif
