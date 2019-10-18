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

struct ak4613_context;
#define MIXER_CONTEXT_T struct ak4613_context

#include <audio_driver.h>
#include <mixer/i2s_codec_dll.h>

#include "ak4613.h"

static int32_t pcm_devices[1] = { 0 };

static snd_mixer_voice_t    stereo_voices[2] = {
    {SND_MIXER_VOICE_LEFT,  0}, // left channel
    {SND_MIXER_VOICE_RIGHT, 0}  // right channel
};

/* Output volume range of the AK4613 DAC */
static struct snd_mixer_element_volume1_range ak4613_output_range[2] = {
    {0, AK4613_MAX_DIGITAL_VOL, -12700, 0, 100},    // min, max, min_dB, max_dB, dB_scale_factor (SPEAKER)
    {0, AK4613_MAX_DIGITAL_VOL, -12700, 0, 100}     // min, max, min_dB, max_dB, dB_scale_factor (SPEAKER)
};

static int32_t
ak4613_master_vol_control (MIXER_CONTEXT_T * ak4613, ado_mixer_delement_t * element, uint8_t set,
    uint32_t * vol, void *instance_data)
{
    uint8_t data[12];
    int32_t altered = 0, i;


    /* get output volume from hardware */
    ak4613_output_vol_get( ak4613->params.tx_voices, data );

    if (set) {
        for ( i = 0; i < ak4613->params.tx_voices; i++ ) {
            if ( data[i] != min( vol[i], 0xFF ) ) {
                data[i] = min( vol[i], 0xFF );
                ado_debug( DB_LVL_DRIVER, "AK4613 : changing channel %u output volume to %x", i, data[i] );
                altered = 1;
            }
        }

        /* set output volume to CODEC */
        if ( altered ) {
            ak4613_output_vol_set( ak4613->params.tx_voices, data );
        }
    } else { /* read volume */
        for ( i = 0; i < ak4613->params.tx_voices; i++ ) {
            vol[i] = data[i];
        }
    }

    return altered;
}

static int32_t
ak4613_master_mute_control (MIXER_CONTEXT_T * ak4613, ado_mixer_delement_t * element, uint8_t set,
    uint32_t * mute, void *instance_data)
{
    uint8_t data;
    int32_t altered = 0;

    ak4613_output_mute_get(&data);

    /* implement joint-mute, as the mute bit for L,R channels is common */
    if (set) {
        altered = data != *mute;
        if (altered) {
            ak4613_output_mute_set(*mute);
        }
    } else { /* read mute */
        *mute = data;
    }

    return altered;
}

/*
 *  build_ak4613_mixer
 */
static int32_t
build_ak4613_mixer(MIXER_CONTEXT_T * ak4613)
{
    int     error = 0;
    ado_mixer_delement_t *pre_elem, *vol_elem, *mute_elem, *elem = NULL;

    ado_debug (DB_LVL_DRIVER, "AK4613");

    /* ################# */
    /* the OUTPUT GROUPS */
    /* ################# */
    if (!error && (elem = ado_mixer_element_pcm1 (ak4613->mixer, "DAC PCM",
                SND_MIXER_ETYPE_PLAYBACK1, 1, &pcm_devices[0])) == NULL)
        error++;

    pre_elem = elem;

    if (!error && (vol_elem = ado_mixer_element_volume1 (ak4613->mixer, "DAC Volume",
                2, ak4613_output_range, ak4613_master_vol_control, NULL, NULL)) == NULL)
        error++;

    /* route pcm to volume */
    if (!error && ado_mixer_element_route_add(ak4613->mixer, pre_elem, vol_elem) != 0)
        error++;

    pre_elem = vol_elem;

    if (!error && (mute_elem = ado_mixer_element_sw2 (ak4613->mixer, "DAC Mute",
                ak4613_master_mute_control, NULL, NULL)) == NULL)
        error++;

    /* route volume to mute */
    if (!error && ado_mixer_element_route_add(ak4613->mixer, pre_elem, mute_elem) != 0)
        error++;

    pre_elem = mute_elem;

    if (!error && (elem = ado_mixer_element_io (ak4613->mixer, "DAC Output",
                SND_MIXER_ETYPE_OUTPUT, 0, 2, stereo_voices)) == NULL)
        error++;

    if (!error && ado_mixer_element_route_add (ak4613->mixer, pre_elem, elem) != 0)
        error++;

    if (!error && ado_mixer_playback_group_create(ak4613->mixer, SND_MIXER_MASTER_OUT,
                SND_MIXER_CHN_MASK_STEREO, vol_elem, mute_elem) == NULL)
        error++;

    if (!error) {
        return EOK;
    }

    return ENOMEM;
}

static void
ak4613_mixer_set_default_group( ado_pcm_t *pcm, ado_mixer_t *mixer, int channel, int index )
{
    switch (channel)
    {
        case ADO_PCM_CHANNEL_PLAYBACK:
            ado_pcm_chn_mixer (pcm, ADO_PCM_CHANNEL_PLAYBACK, mixer,
                ado_mixer_find_element (mixer, SND_MIXER_ETYPE_PLAYBACK1,
                    SND_MIXER_ELEMENT_PLAYBACK, index), ado_mixer_find_group (mixer,
                    SND_MIXER_MASTER_OUT, index));
            break;
        case ADO_PCM_CHANNEL_CAPTURE: /* no input group */
        default:
            break;
    }
}

void ak4613_mixer_register_dump()
{
    ak4613_register_dump();
}

static ado_mixer_reset_t ak4613_mixer_reset;
static int32_t ak4613_mixer_reset (MIXER_CONTEXT_T *ak4613)
{
    int32_t status;

    ado_debug (DB_LVL_MIXER, "MIXER RESET");

    status = ak4613_init ( ak4613 );

    return status;
}

static ado_mixer_destroy_t ak4613_mixer_destroy;
static int32_t ak4613_mixer_destroy (MIXER_CONTEXT_T *ak4613)
{
    ado_debug ( DB_LVL_MIXER, "MIXER DESTROY" );

    ak4613_deinit();

    return EOK;
}

static void ak4613_mixer_set_rate (ado_mixer_t *mixer, uint32_t sample_rate, int channel)
{
    ado_debug ( DB_LVL_DRIVER, "Setting AK4613 sample rate to %d", sample_rate );

    ak4613_rate_setting ( sample_rate );
}

/* Option names are prefixed with codec name to prevent name collisions
 * with other options handled at higher levels in io-audio;
 * TODO: add option for tdm when needed
 */
static const char* opts[] = {
#define I2C_DEV              0
    "ak4613_i2c_dev",
#define I2C_ADDR             1
    "ak4613_i2c_addr",
#define DEBUG                2
    "ak4613_debug",
#define OUT1                 3
    "ak4613_out1",
#define OUT2                 4
    "ak4613_out2",
#define OUT3                 5
    "ak4613_out3",
#define OUT4                 6
    "ak4613_out4",
#define OUT5                 7
    "ak4613_out5",
#define OUT6                 8
    "ak4613_out6",
#define IN1                  9
    "ak4613_in1",
#define IN2                  10
    "ak4613_in2",
    NULL
};

#define N_OPTS ((sizeof(opts)/sizeof(opts[0])) - 1U)

static int
parse_config_opts ( MIXER_CONTEXT_T *ak4613 )
{
    const ado_dict_t *dict = NULL;
    const char* opt_str[N_OPTS] = {NULL};
    const char* sub_opt_str = NULL;
    int opt;
    long value;
    ado_card_t *card = ado_mixer_get_card ( ak4613->mixer );

    dict = ado_get_card_dict (card);

    ado_config_load_key_values(dict, opts, opt_str, N_OPTS, 0, -1);

    if ( opt_str[I2C_DEV] != NULL ) {
        if ( ( value = strtol ( opt_str[I2C_DEV], NULL, 0 ) ) >= 0 ) {
            ak4613->params.i2c_dev = value;
        } else {
            ado_error_fmt( "Incorrect value for option %s: %s", opts[I2C_DEV], opt_str[I2C_DEV] );
            return EINVAL;
        }
    }

    if ( opt_str[I2C_ADDR] != NULL ) {
        if ( ( value = strtol ( opt_str[I2C_ADDR], NULL, 0 ) ) != 0 ) {
            ak4613->params.i2c_addr = value;
        } else {
            ado_error_fmt( "Incorrect value for option %s: %s", opts[I2C_ADDR], opt_str[I2C_ADDR] );
            return EINVAL;
        }
    }

    if (opt_str[DEBUG] != NULL) {
        if ( !strncasecmp ( opt_str[DEBUG], "enable", strlen("enable") ) ) {
            ak4613->debug = true;
        }
    }

    for ( opt = OUT1; opt <= OUT6; opt++ ) {
        if ( opt_str[opt] ) {
            if ( !strncasecmp ( opt_str[opt], "enable", strlen("enable") ) ) {
                ak4613->dac_mask |= (1 << (opt-OUT1));
                sub_opt_str = strpbrk ( opt_str[opt], ":" );
                if ( sub_opt_str ) {
                    sub_opt_str++;
                    if ( !strncasecmp ( sub_opt_str, "differential", strlen("differential") ) ) {
                        ak4613->differential_out_mask |= (1 << (opt-OUT1));
                    } else if ( strncasecmp ( sub_opt_str, "single-ended", strlen("single-ended") ) ) {
                        ado_error_fmt ( "Incorrect sub-option for %s: %s", opts[opt], sub_opt_str );
                        return EINVAL;
                    }
                }
            } else if ( strncasecmp ( opt_str[opt], "disable", strlen("disable") ) ) {
                ado_error_fmt ( "Incorrect option for %s: %s", opts[opt], opt_str[opt] );
                return EINVAL;
            }
        }
    }

    for ( opt = IN1; opt <= IN2; opt++ ) {
        if ( opt_str[opt] ) {
            if ( !strncasecmp ( opt_str[opt], "enable", strlen("enable") ) ) {
                ak4613->adc_mask |= (1 << (opt-IN1));
                sub_opt_str = strpbrk ( opt_str[opt], ":" );
                if ( sub_opt_str ) {
                    sub_opt_str++;
                    if ( !strncasecmp ( sub_opt_str, "differential", strlen("differential") ) ) {
                        ak4613->differential_in_mask |= (1 << (opt-IN1));
                    } else if ( strncasecmp ( sub_opt_str, "single-ended", strlen("single-ended") ) ) {
                        ado_error_fmt ( "Incorrect sub-option for %s: %s", opts[opt], sub_opt_str );
                        return EINVAL;
                    }
                }
            } else if ( strncasecmp ( opt_str[opt], "disable", strlen("disable") ) ) {
                ado_error_fmt ( "Incorrect option for %s: %s", opts[opt], opt_str[opt] );
                return EINVAL;
            }
        }
    }

    /* do a basic validation of the options:
     * i2c_dev and i2c_addr must be >= 0 (should have been initialized to -1 in the deva ctrl driver) */
    if ( ak4613->params.i2c_dev < 0 || ak4613->params.i2c_addr < 0 ) {
        ado_error_fmt ( "Missing or incorrect I2C params: i2c_dev %d, i2c_addr %d",
                        ak4613->params.i2c_dev, ak4613->params.i2c_addr );
        return EINVAL;
    }

    return EOK;
}

ado_mixer_dll_init_t mixer_dll_init;
int mixer_dll_init (MIXER_CONTEXT_T ** context, ado_mixer_t * mixer, void *params, void *raw_callbacks, int version)
{
    ak4613_context_t *ak4613;
    ado_mixer_dll_codec_callbacks_t *callbacks = raw_callbacks;

    ado_debug(DB_LVL_MIXER, "Initializing AK4613 Codec");

    if ( (ak4613 = (ak4613_context_t *) ado_calloc(1, sizeof (ak4613_context_t))) == NULL ) {
        ado_error_fmt( "AK4613: Failed to allocate device structure - %s", strerror(errno) );
        return (-1);
    }
    *context = ak4613;
    ado_mixer_set_name ( mixer, "AK4613" );

    ak4613->mixer = mixer;
    memcpy ( &ak4613->params, params, sizeof (ak4613->params) );

    if ( ak4613->params.is_active == NULL ) {
        ado_error_fmt ( "AK4613: is_active routine missing" );
        ado_free( ak4613 );
        return (-1);
    }

    if ( parse_config_opts(ak4613) != EOK ) {
        ado_error_fmt("Failed parsing AK4613 options");
        ado_free(ak4613);
        return -1;
    }

    if (callbacks) {
        callbacks->codec_set_default_group = ak4613_mixer_set_default_group;
        callbacks->codec_set_rate = ak4613_mixer_set_rate;
    }

    if ( build_ak4613_mixer(ak4613) != EOK ) {
        ado_error_fmt("Failed building AK4613 mixer");
        ado_free(ak4613);
        return -1;
    }

    if ( ak4613_mixer_reset(ak4613) != EOK ) {
        ado_error_fmt("Failed to reset AK4613");
        ado_free(ak4613);
        return -1;
    }

    ado_mixer_set_reset_func ( ak4613->mixer, ak4613_mixer_reset );
    ado_mixer_set_destroy_func ( ak4613->mixer, ak4613_mixer_destroy );

    return 0;
}

ado_dll_version_t version;
void
version (int *major, int *minor, char *date)
{
	*major = ADO_MAJOR_VERSION;
	*minor = I2S_CODEC_MINOR_VERSION;
	date = __DATE__;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/deva/mixer/ak4613/ak4613_mixer.c $ $Rev: 850894 $")
#endif
