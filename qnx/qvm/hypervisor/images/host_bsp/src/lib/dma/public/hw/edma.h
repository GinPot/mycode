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
#ifndef EDMA_H_
#define EDMA_H_

/**
 * @file        src/lib/dma/public/hw/edma.h
 * @addtogroup  edma
 * @{
 */

#include <stdint.h>
#include <hw/dma.h>
#include <aarch64/imx8/imx8_edma_channel.h>
#include <aarch64/imx8/imx8_edma_mp.h>
#include <aarch64/imx8/imx8_edma_requests.h>

/** Defines maximum number of TCD descriptors. The source or destination buffer consists of one or multiple fragments
 *  of varying lengths. Each fragment configures one TCD descriptor. So maximum number of fragments is equal to maximum
 *  number of TCD descriptors.
 */
#define MAX_DESCRIPTORS 256
/** eDMA driver description string */
#ifndef IMX_DMA_DESCRIPTION_STR
    #define IMX_DMA_DESCRIPTION_STR    "i.MX eDMA Controller driver"
#endif

/** Lowest channel number. Gathers all channels from all eDMA peripherals */
#define IMX_DMA_CH_LO                  0
/** Highest channel number. Gathers all channels from all eDMA peripherals */
#define IMX_DMA_CH_HI                  (4 * 32)
/** Channel count. Gathers all channels from all eDMA peripherals */
#define IMX_DMA_N_CH                   (4 * 32)
/** eDMA channel lowest priority. */
#define IMX_DMA_CH_PRIO_LO             1
/** eDMA channel highest priority. */
#define IMX_DMA_CH_PRIO_HI             7

/* Helper macros defining number of bits */
#define IMX_DMA_NBITS_8                8
#define IMX_DMA_NBITS_16               16
#define IMX_DMA_NBITS_24               24
#define IMX_DMA_NBITS_32               32
#define IMX_DMA_NBITS_64               64
#define IMX_DMA_NBITS_128              128
#define IMX_DMA_NBITS_256              256
#define IMX_DMA_NBITS_512              512

/** eDMA device info structure type */
typedef struct edma_device_info_s {
    uint32_t      base;
    uint32_t      chn_base;
    unsigned int  error_irq;
    unsigned int  irq;
} edma_device_info_t;

extern const edma_device_info_t edma_device_info[IMX_DMA_COUNT];

/** @} */ /* End of edma */

#endif /* EDMA_H_ */




#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/lib/dma/public/hw/edma.h $ $Rev: 842764 $")
#endif
