/*
 * $QNXLicenseC:
 * Copyright 2017, QNX Software Systems.
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

#ifndef __HWINFO_RCAR_H_INCLUDED
#define __HWINFO_RCAR_H_INCLUDED

#include <hw/sysinfo.h>

/*
 * =============================================================================
 *
 *                                B U S E S
 *
 * =============================================================================
*/
#define RCAR_HWI_I2C                        HWI_ITEM_BUS_I2C
#define RCAR_HWI_IIC                        "iic"

#define RCAR_HWI_SPI                        HWI_ITEM_BUS_SPI
 #define RCAR_HWI_RPC_WBUF                  "wbuf"
 #define RCAR_HWI_RPC_WBUF_SIZE             "wbuf_size"

#define RCAR_HWI_MSIOF                      "msiof_spi"

#define RCAR_HWI_SDHI                       HWI_ITEM_BUS_SDIO
 #define RCAR_HWI_SDHI_PWR                  "sdio,pwr"
 #define RCAR_HWI_SDHI_PWR_REG              "pwr_offset"
 #define RCAR_HWI_SDHI_PWR_VDD              "pwr_vdd"
 #define RCAR_HWI_SDHI_PWR_IF               "pwr_if"
 #define RCAR_HWI_SDHI_PFC                  "sdio,pfc"
 #define RCAR_HWI_SDHI_PFC_REG              "pfc_offset"
 #define RCAR_HWI_SDHI_PFC_PMMR             "pfc_pmmr"
 #define RCAR_HWI_SDHI_PFC_MASK             "pfc_mask"
 #define RCAR_HWI_SDHI_PFC_SHIFT            "pfc_shift"

/*
 * =============================================================================
 *
 *                              D E V I C E S
 *
 * =============================================================================
*/
#define RCAR_HWI_UART                       "scif"

#define RCAR_HWI_HSUART                     "hscif"

#define RCAR_HWI_SYSDMAC                    "sys"
#define RCAR_HWI_AUDIODMAC                  "audio"
 #define RCAR_HWI_DMA_REG                   "dma_reg"
 #define RCAR_HWI_DMA_SIZE                  "dma_size"
 #define RCAR_HWI_DMA_DESC                  "dma_desc"
#define RCAR_HWI_AUDIOPPDMAC                "audiopp"

#define RCAR_HWI_AUDIO_ADG                  "adg"
#define RCAR_HWI_AUDIO_SSI                  "ssi"
#define RCAR_HWI_AUDIO_SSIU                 "ssiu"
#define RCAR_HWI_AUDIO_SCU                  "scu"

#define RCAR_HWI_VIN                        "vin"
#define RCAR_HWI_CAMERA                     "csi"

#define RCAR_HWI_WDT                        "wdog"

#define RCAR_HWI_THERMAL                    "thermal"

#endif //__HWINFO_RCAR_H_INCLUDED

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/hw/hwinfo_rcar.h $ $Rev: 842614 $")
#endif
