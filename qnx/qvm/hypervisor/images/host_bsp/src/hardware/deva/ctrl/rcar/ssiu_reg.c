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

#include <audio_driver.h>
#include "rcar_support.h"
#include "ssiu_reg.h"
#include "ssiu.h"

static ssiu_reg_map_t* ssiu_reg_map = MAP_FAILED;
static ssi_reg_t* ssi_reg = MAP_FAILED;

int ssiu_mem_map(void)
{
    unsigned long base;

    base = RCAR_GEN2_SSIU_BASE;
    rcar_query_hwinfo(RCAR_HWI_AUDIO_SSIU, &base);
    /* SSIU registers */
    ssiu_reg_map = ado_device_mmap (base, sizeof(ssiu_reg_map_t));
    if (ssiu_reg_map == MAP_FAILED )
    {
        ado_error_fmt ("SSIU register map failed");
        return ENOMEM;
    }

    base = RCAR_GEN2_SSI_BASE;
    rcar_query_hwinfo(RCAR_HWI_AUDIO_SSI, &base);
    ssi_reg = (ssi_reg_t *)ado_device_mmap (base, SSI_CHANNEL_NUM * sizeof(ssi_reg_t));
    if (ssi_reg == MAP_FAILED )
    {
        ado_error_fmt ("SSI register map failed");
        return ENOMEM;
    }

    return EOK;
}

int ssiu_mem_unmap(void)
{
    ado_device_munmap((void*)ssiu_reg_map, sizeof(ssiu_reg_map_t));
    ssiu_reg_map = MAP_FAILED;

    ado_device_munmap((void*)ssi_reg, SSI_CHANNEL_NUM * sizeof(ssi_reg_t));
    ssi_reg = MAP_FAILED;

    return EOK;
}

ssi_reg_t* get_ssi_reg(uint32_t ssi_idx)
{
    if( ssi_reg == MAP_FAILED ) {
        return NULL;
    }

    if( ssi_idx >= SSI_CHANNEL_NUM ) {
        return NULL;
    }

    return ssi_reg + ssi_idx;
}

ssi_ext_reg_t* get_ssi_ext_reg(uint32_t ssi_idx)
{
    if( ssi_reg == MAP_FAILED ) {
        return NULL;
    }

    if( ssi_idx >= SSI_CHANNEL_NUM ) {
        return NULL;
    }

    if( !rcar_ssi_ext_reg_supported() ) {
        return NULL;
    }

    return (ssi_ext_reg_t*)ssi_reg + ssi_idx;
}


ssiu_busif_reg_t* get_ssiu_busif_reg(uint32_t ssi_idx, uint32_t ssi_subchan_idx)
{
    ssiu_reg_t* ssiu_reg = NULL;

    if( ssiu_reg_map == MAP_FAILED ) {
        return NULL;
    }

    if( !rcar_ssi_subchan_supported(ssi_idx, ssi_subchan_idx) ) {
        return NULL;
    }

    if( ssi_subchan_idx <= 3 ) {
        ssiu_reg = &ssiu_reg_map->ssiu_reg_ssi_0_9_busif_0_3[ssi_idx][ssi_subchan_idx];
    } else if (ssi_idx <= 5) {
        ssiu_reg = &ssiu_reg_map->ssiu_reg_ssi_0_5_busif_4_7[ssi_idx][ssi_subchan_idx-4];
    } else {
        ssiu_reg = &ssiu_reg_map->ssiu_reg_ssi_9_busif_4_7[ssi_subchan_idx-4];
    }

    return &ssiu_reg->busif;
}

ssiu_busif2_reg_t* get_ssiu_busif2_reg(uint32_t ssi_idx)
{
    if( ssiu_reg_map == MAP_FAILED ) {
        return NULL;
    }

    if( !rcar_ssi_supported(ssi_idx) ) {
        return NULL;
    }

    if( !rcar_ssi_busif2_reg_supported(ssi_idx) ) {
        return NULL;
    }

    return &ssiu_reg_map->ssiu_reg_ssi_0_9_busif2[ssi_idx].busif2;
}

ssiu_ssi_reg_t* get_ssiu_ssi_reg(uint32_t ssi_idx)
{
    if( ssiu_reg_map == MAP_FAILED ) {
        return NULL;
    }

    if( !rcar_ssi_supported(ssi_idx) ) {
        return NULL;
    }

    return &ssiu_reg_map->ssiu_reg_ssi_0_9_busif_0_3[ssi_idx][0].ssi;
}

ssiu_ssi2_reg_t* get_ssiu_ssi2_reg(uint32_t ssi_idx)
{
    if( ssiu_reg_map == MAP_FAILED ) {
        return NULL;
    }

    if( !rcar_ssi_supported(ssi_idx) ) {
        return NULL;
    }

    return &ssiu_reg_map->ssiu_reg_ssi_0_9_busif2[ssi_idx].ssi2;
}

ssiu_common_reg_t* get_ssiu_common_reg(void)
{
    if( ssiu_reg_map == MAP_FAILED ) {
        return NULL;
    }

    return &ssiu_reg_map->common;
}

ssiu_common_ext_reg_t* get_ssiu_common_ext_reg(void)
{
    if( ssiu_reg_map == MAP_FAILED ) {
        return NULL;
    }

    return (ssiu_common_ext_reg_t*) &ssiu_reg_map->common;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/deva/ctrl/rcar/ssiu_reg.c $ $Rev: 837440 $")
#endif

