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

#ifndef IMX_EDMA_REQUESTS_H_
#define IMX_EDMA_REQUESTS_H_

/**
 * @file        src/lib/dma/public/arm/imx/imx_edma_requests.h
 * @addtogroup  edma
 * @{
 */

#define IMX_DMA_COUNT                            4
#define IMX_DMA_DEVICE_SIZE                      0x10000
#define IMX_DMA_CHANNEL_SIZE                     0x10000

#define IMX_DMA0_BASE                            0x5A1F0000
#define IMX_DMA0_IRQ                             98
#define IMX_DMA0_ERROR_IRQ                       99
#define IMX_DMA1_BASE                            0x5A9F0000
#define IMX_DMA1_IRQ                             100
#define IMX_DMA1_ERROR_IRQ                       101
#define IMX_DMA2_BASE                            0x591F0000
#define IMX_DMA2_IRQ                             400
#define IMX_DMA2_ERROR_IRQ                       401
#define IMX_DMA3_BASE                            0x599F0000
#define IMX_DMA3_IRQ                             402
#define IMX_DMA3_ERROR_IRQ                       403

/** DMA request sources definition.
* bits: 31..31 - 1 = SW request, 0 = peripheral request
*       20..11 - eDMA channel irq number
*        6..5  - eDMA controller index
*        4..0  - request source (channel) number
*/
#define IMX_DMA_REQUEST_CHANNEL_IRQ_NUMBER_MASK  0x001FF800
#define IMX_DMA_REQUEST_CHANNEL_IRQ_NUMBER_SHIFT 11
#define IMX_DMA_REQUEST_DMA_NUMBER_MASK          0x00000060
#define IMX_DMA_REQUEST_DMA_NUMBER_SHIFT         5
#define IMX_DMA_REQUEST_CHANNEL_NUMBER_MASK      0x0000001F
#define IMX_DMA_REQUEST_CHANNEL_NUMBER_SHIFT     0

#define imx_edma_define_request(_edma_num_,_channel_num_,_irq_num_) ( \
        ((_irq_num_     << IMX_DMA_REQUEST_CHANNEL_IRQ_NUMBER_SHIFT)  & IMX_DMA_REQUEST_CHANNEL_IRQ_NUMBER_MASK) | \
        ((_edma_num_    << IMX_DMA_REQUEST_DMA_NUMBER_SHIFT)          & IMX_DMA_REQUEST_DMA_NUMBER_MASK) | \
        ((_channel_num_ << IMX_DMA_REQUEST_CHANNEL_NUMBER_SHIFT)      & IMX_DMA_REQUEST_CHANNEL_NUMBER_MASK) \
)
#define imx_edma_request_get_irq_number(_request_)        (((_request_) & IMX_DMA_REQUEST_CHANNEL_IRQ_NUMBER_MASK) >> IMX_DMA_REQUEST_CHANNEL_IRQ_NUMBER_SHIFT)
#define imx_edma_request_get_idx(_request_)                ((_request_) & (IMX_DMA_REQUEST_DMA_NUMBER_MASK | IMX_DMA_REQUEST_CHANNEL_NUMBER_MASK))
#define imx_edma_request_get_dma_number(_request_)        (((_request_) & IMX_DMA_REQUEST_DMA_NUMBER_MASK)         >> IMX_DMA_REQUEST_DMA_NUMBER_SHIFT)
#define imx_edma_request_get_channel_number(_request_)    (((_request_) & IMX_DMA_REQUEST_CHANNEL_NUMBER_MASK)     >> IMX_DMA_REQUEST_CHANNEL_NUMBER_SHIFT)

typedef enum {
    IMX_DMA_REQ_LPSPI0_RX                    = imx_edma_define_request(0, 0, 448),
    IMX_DMA_REQ_LPSPI0_TX                    = imx_edma_define_request(0, 1, 449),
    IMX_DMA_REQ_LPSPI1_RX                    = imx_edma_define_request(0, 2, 450),
    IMX_DMA_REQ_LPSPI1_TX                    = imx_edma_define_request(0, 3, 451),
    IMX_DMA_REQ_LPSPI2_RX                    = imx_edma_define_request(0, 4, 452),
    IMX_DMA_REQ_LPSPI2_TX                    = imx_edma_define_request(0, 5, 453),
    IMX_DMA_REQ_LPSPI3_RX                    = imx_edma_define_request(0, 6, 454),
    IMX_DMA_REQ_LPSPI3_TX                    = imx_edma_define_request(0, 7, 455),
    IMX_DMA_REQ_LPUART0_RX                   = imx_edma_define_request(0, 12, 466),
    IMX_DMA_REQ_LPUART0_TX                   = imx_edma_define_request(0, 13, 467),
    IMX_DMA_REQ_LPUART1_RX                   = imx_edma_define_request(0, 14, 468),
    IMX_DMA_REQ_LPUART1_TX                   = imx_edma_define_request(0, 15, 469),
    IMX_DMA_REQ_LPUART2_RX                   = imx_edma_define_request(0, 16, 470),
    IMX_DMA_REQ_LPUART2_TX                   = imx_edma_define_request(0, 17, 471),
    IMX_DMA_REQ_LPUART3_RX                   = imx_edma_define_request(0, 18, 472),
    IMX_DMA_REQ_LPUART3_TX                   = imx_edma_define_request(0, 19, 473),
    IMX_DMA_REQ_LPUART4_RX                   = imx_edma_define_request(0, 20, 474),
    IMX_DMA_REQ_LPUART4_TX                   = imx_edma_define_request(0, 21, 475),
    IMX_DMA_REQ_EMVSIM0_RX                   = imx_edma_define_request(0, 26, 476),
    IMX_DMA_REQ_EMVSIM0_TX                   = imx_edma_define_request(0, 27, 477),
    IMX_DMA_REQ_EMVSIM1_RX                   = imx_edma_define_request(0, 28, 478),
    IMX_DMA_REQ_EMVSIM1_TX                   = imx_edma_define_request(0, 29, 479),
    IMX_DMA_REQ_EDMA0_EXT_INT_0              = imx_edma_define_request(0, 30, 274),
    IMX_DMA_REQ_EDMA0_EXT_INT_1              = imx_edma_define_request(0, 31, 275),
    IMX_DMA_REQ_LPI2C0_RX                    = imx_edma_define_request(1, 0, 456),
    IMX_DMA_REQ_LPI2C0_TX                    = imx_edma_define_request(1, 1, 457),
    IMX_DMA_REQ_LPI2C1_RX                    = imx_edma_define_request(1, 2, 458),
    IMX_DMA_REQ_LPI2C1_TX                    = imx_edma_define_request(1, 3, 459),
    IMX_DMA_REQ_LPI2C2_RX                    = imx_edma_define_request(1, 4, 460),
    IMX_DMA_REQ_LPI2C2_TX                    = imx_edma_define_request(1, 5, 461),
    IMX_DMA_REQ_LPI2C3_RX                    = imx_edma_define_request(1, 6, 462),
    IMX_DMA_REQ_LPI2C3_TX                    = imx_edma_define_request(1, 7, 463),
    IMX_DMA_REQ_LPI2C4_RX                    = imx_edma_define_request(1, 8, 464),
    IMX_DMA_REQ_LPI2C4_TX                    = imx_edma_define_request(1, 9, 465),
    IMX_DMA_REQ_FLEXSPI0_RX                  = imx_edma_define_request(1, 12, 276),
    IMX_DMA_REQ_FLEXSPI1_RX                  = imx_edma_define_request(1, 13, 277),
    IMX_DMA_REQ_FLEXSPI0_TX                  = imx_edma_define_request(1, 14, 278),
    IMX_DMA_REQ_FLEXSPI1_TX                  = imx_edma_define_request(1, 15, 279),
    IMX_DMA_REQ_ADC0                         = imx_edma_define_request(1, 16, 396),
    IMX_DMA_REQ_ADC1                         = imx_edma_define_request(1, 18, 397),
    IMX_DMA_REQ_FTM0                         = imx_edma_define_request(1, 20, 394),
    IMX_DMA_REQ_FTM1                         = imx_edma_define_request(1, 22, 395),
    IMX_DMA_REQ_FLEXCAN0                     = imx_edma_define_request(1, 26, 391),
    IMX_DMA_REQ_FLEXCAN1                     = imx_edma_define_request(1, 28, 392),
    IMX_DMA_REQ_FLEXCAN2                     = imx_edma_define_request(1, 30, 393),
    IMX_DMA_REQ_ASRC0_PAIR_A_INPUT           = imx_edma_define_request(2, 0, 406),
    IMX_DMA_REQ_ASRC0_PAIR_B_INPUT           = imx_edma_define_request(2, 1, 407),
    IMX_DMA_REQ_ASRC0_PAIR_C_INPUT           = imx_edma_define_request(2, 2, 408),
    IMX_DMA_REQ_ASRC0_PAIR_A_OUTPUT          = imx_edma_define_request(2, 3, 409),
    IMX_DMA_REQ_ASRC0_PAIR_B_OUTPUT          = imx_edma_define_request(2, 4, 410),
    IMX_DMA_REQ_ASRC0_PAIR_C_OUTPUT          = imx_edma_define_request(2, 5, 411),
    IMX_DMA_REQ_ESAI0_RX                     = imx_edma_define_request(2, 6, 442),
    IMX_DMA_REQ_ESAI0_TX                     = imx_edma_define_request(2, 7, 442),
    IMX_DMA_REQ_SPDIF0_RX                    = imx_edma_define_request(2, 8, 489),
    IMX_DMA_REQ_SPDIF0_TX                    = imx_edma_define_request(2, 9, 491),
    IMX_DMA_REQ_SPDIF1_RX                    = imx_edma_define_request(2, 10, 493),
    IMX_DMA_REQ_SPDIF1_TX                    = imx_edma_define_request(2, 11, 495),
    IMX_DMA_REQ_SAI0_RX                      = imx_edma_define_request(2, 12, 347),
    IMX_DMA_REQ_SAI0_TX                      = imx_edma_define_request(2, 13, 347),
    IMX_DMA_REQ_SAI1_RX                      = imx_edma_define_request(2, 14, 349),
    IMX_DMA_REQ_SAI1_TX                      = imx_edma_define_request(2, 15, 349),
    IMX_DMA_REQ_SAI2_RX                      = imx_edma_define_request(2, 16, 351),
    IMX_DMA_REQ_SAI3_RX                      = imx_edma_define_request(2, 17, 356),
    IMX_DMA_REQ_HDMI_RX_SAI0_RX              = imx_edma_define_request(2, 18, 358),
    IMX_DMA_REQ_HDMI_TX_SAI0_TX              = imx_edma_define_request(2, 19, 360),
    IMX_DMA_REQ_GPT0_EC                      = imx_edma_define_request(2, 21, 423),
    IMX_DMA_REQ_GPT1_EC                      = imx_edma_define_request(2, 22, 424),
    IMX_DMA_REQ_GPT2_EC                      = imx_edma_define_request(2, 23, 425),
    IMX_DMA_REQ_GPT3_EC                      = imx_edma_define_request(2, 24, 426),
    IMX_DMA_REQ_GPT4_EC                      = imx_edma_define_request(2, 25, 427),
    IMX_DMA_REQ_GPT5_EC                      = imx_edma_define_request(2, 26, 428),
    IMX_DMA_REQ_ASRC1_PAIR_A_INPUT           = imx_edma_define_request(3, 0, 414),
    IMX_DMA_REQ_ASRC1_PAIR_B_INPUT           = imx_edma_define_request(3, 1, 415),
    IMX_DMA_REQ_ASRC1_PAIR_C_INPUT           = imx_edma_define_request(3, 2, 416),
    IMX_DMA_REQ_ASRC1_PAIR_A_OUTPUT          = imx_edma_define_request(3, 3, 417),
    IMX_DMA_REQ_ASRC1_PAIR_B_OUTPUT          = imx_edma_define_request(3, 4, 418),
    IMX_DMA_REQ_ASRC1_PAIR_C_OUTPUT          = imx_edma_define_request(3, 5, 419),
    IMX_DMA_REQ_ESAI1_RX                     = imx_edma_define_request(3, 6, 444),
    IMX_DMA_REQ_ESAI1_TX                     = imx_edma_define_request(3, 7, 444),
    IMX_DMA_REQ_SAI6_RX                      = imx_edma_define_request(3, 8, 362),
    IMX_DMA_REQ_SAI6_TX                      = imx_edma_define_request(3, 9, 362),
    IMX_DMA_REQ_SAI7_TX                      = imx_edma_define_request(3, 10, 364),
    IMX_DMA_REQ_SW                           = 0x80000000,
    IMX_DMA_REQ_NONE                         = 0xFFFFFFFF
} imx_edma_request_source_t;

/** @} */ /* End of edma */

#endif /* IMX_EDMA_REQUESTS_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/lib/dma/public/aarch64/imx8/imx8_edma_requests.h $ $Rev: 837687 $")
#endif
