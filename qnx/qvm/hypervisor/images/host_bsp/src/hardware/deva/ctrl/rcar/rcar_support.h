/*
 * $QNXLicenseC:
 * Copyright 2014, 2016 QNX Software Systems.
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

#ifndef _R_Car_SUPPORT_H
#define _R_Car_SUPPORT_H

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <drvr/hwinfo.h>
#include <hw/hwinfo_rcar.h>
#include <arm/r-car-gen2.h>

/* uncomment to enable TDM split mode support - currently disabled, given that we need
   a concrete use case with the associated routing in order to meaningfully
   support this feature */
/* #define RCAR_TDMSPLIT_SUPPORT */

/* uncomment to enable TDM ex-split mode support - currently disabled, given that we need
   a concrete use case with the associated routing in order to meaningfully
   support this feature */
/* #define RCAR_TDMEXSPLIT_SUPPORT */

/* uncomment to enable support for MLP routing of audio data; more work still required
   to fully support this feature */
/* #define RCAR_MLP_SUPPORT */

/* uncomment to enable support for DTCP - digital content protection of audio data;
   more work still required to fully support this feature; dependency on MLP support */
/* #define RCAR_DTCP_SUPPORT */

typedef enum {
   RCAR_VERSION_H2,
   RCAR_VERSION_E2,
   RCAR_VERSION_M2,
   RCAR_VERSION_V2,
   RCAR_VERSION_H3,
   RCAR_VERSION_M3,
   RCAR_VERSION_D3,
   RCAR_VERSION_W2H
} rcar_version_t;

rcar_version_t rcar_version_get(void);
int rcar_version_set( rcar_version_t version );
void rcar_query_hwinfo( const char* dev, unsigned long * base );

/* SSI/SSIU features */
bool rcar_ssi_supported( uint32_t ssi_channel );
bool rcar_ssi_tdm_supported( uint32_t ssi_channel );
bool rcar_ssi_tdm_voices_supported( uint32_t voices );
bool rcar_ssi_tdmext_supported( uint32_t ssi_channel );
bool rcar_ssi_tdmsplit_supported( uint32_t ssi_channel );
bool rcar_ssi_tdmexsplit_supported( uint32_t ssi_channel );
int rcar_ssi_get_supported_range( uint32_t* min_ssi_channel, uint32_t* max_ssi_channel );
bool rcar_ssi_subchan_supported( uint32_t ssi_channel, uint32_t subchannel );
int rcar_ssi_subchan_get_supported_range( uint32_t ssi_channel, uint32_t* min_subchannel, uint32_t* max_subchannel );
bool rcar_ssi_ext_reg_supported( void );
bool rcar_ssi_common_ext_reg_supported( void );
bool rcar_ssi_busif2_reg_supported( uint32_t ssi_channel );
bool rcar_adg_ext_reg_supported( void );

/* HDMI features - refer to HDMI interface module on Rcar-H3 */
bool rcar_hdmi_supported( uint32_t hdmi_channel );
int rcar_hdmi_get_supported_range( uint32_t* min_hdmi_channel, uint32_t* max_hdmi_channel );

/* SCU features */
bool rcar_src_supported( uint32_t src_channel );
bool rcar_src_multichan_supported( uint32_t src_channel );
bool rcar_src_highsound_supported( uint32_t src_channel );
bool rcar_src_inline_supported( uint32_t src_channel );
int rcar_src_get_supported_range( uint32_t* min_src_channel, uint32_t* max_src_channel );
bool rcar_cmd_supported( uint32_t cmd_channel );
int rcar_cmd_get_supported_range( uint32_t* min_cmd_channel, uint32_t* max_cmd_channel );
/* MLP/MLM features */
bool rcar_mlm_supported( uint32_t mlm_channel );
int rcar_mlm_get_supported_range( uint32_t* min_mlm_channel, uint32_t* max_mlm_channel );
/* DTCP features */
bool rcar_dtcp_supported( uint32_t dtcp );
int rcar_dtcp_get_supported_range( uint32_t* min_dtcp, uint32_t* max_dtcp);
/* DMAC channels */
bool rcar_dmac_channel_supported( uint32_t dma_channel );
int rcar_dmac_get_supported_range( uint32_t* min_dma_channel, uint32_t* max_dma_channel);
int rcar_get_i2c_bus_num(int *i2c_bus);
#endif /* _R_Car_SUPPORT_H */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/deva/ctrl/rcar/rcar_support.h $ $Rev: 850894 $")
#endif
