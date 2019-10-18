/*
 * $QNXLicenseC:
 * Copyright 2016, QNX Software Systems.
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

#ifndef _AK4613_H
#define _AK4613_H

#include <stdint.h>
#include <mixer/i2s_codec_dll.h>

#define AK4613_MAX_DIGITAL_VOL          254
#define AK4613_NUM_DACS                 6
#define AK4613_NUM_ADCS                 2

typedef struct ak4613_context
{
    ado_mixer_t                  *mixer;
    ado_mixer_dll_codec_params_t params;
    bool                         debug;
    bool                         tdm;
    uint8_t                      differential_in_mask;
    uint8_t                      differential_out_mask;
    uint8_t                      adc_mask;
    uint8_t                      dac_mask;
} ak4613_context_t;

int ak4613_init( ak4613_context_t* ak4613 );
int ak4613_deinit( void );
int ak4613_output_vol_get( unsigned voices, uint8_t* vol );
int ak4613_output_vol_set( unsigned voices, uint8_t* vol );
int ak4613_output_mute_set( uint8_t mute );
int ak4613_output_mute_get( uint8_t* mute );

int ak4613_rate_setting( uint32_t sample_rate );
void ak4613_register_dump( void );

#endif /* _AK4613_H */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/deva/mixer/ak4613/ak4613.h $ $Rev: 842276 $")
#endif
