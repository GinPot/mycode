/*
 * $QNXLicenseC:
 * Copyright 2014, QNX Software Systems.
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

#include "rcar_mixer.h"
#include "scu.h"
#include "variant.h"
#if _NTO_VERSION >= 700
#include <mixer/i2s_codec_dll.h>
#endif


static int32_t pcm_devices[1] = { 0 };

static bool use_scu = false;

/* use for mono, stereo or 3 channel configurations */
static snd_mixer_voice_t default_voices[3] = {
    {SND_MIXER_VOICE_LEFT,  0}, // left channel
    {SND_MIXER_VOICE_RIGHT, 0}, // right channel
    {SND_MIXER_VOICE_CENTER, 0} // center channel
};

static snd_mixer_voice_t quad_voices[4] = {
    {SND_MIXER_VOICE_LEFT, 0},
    {SND_MIXER_VOICE_RIGHT, 0},
    {SND_MIXER_VOICE_REAR_LEFT, 0},
    {SND_MIXER_VOICE_REAR_RIGHT, 0}
};

static snd_mixer_voice_t five_one_voices[6] = {
    {SND_MIXER_VOICE_LEFT, 0},
    {SND_MIXER_VOICE_RIGHT, 0},
    {SND_MIXER_VOICE_CENTER, 0},
    {SND_MIXER_VOICE_REAR_LEFT, 0},
    {SND_MIXER_VOICE_REAR_RIGHT, 0},
    {SND_MIXER_VOICE_WOOFER, 0}
};

static snd_mixer_voice_t seven_one_voices[8] = {
    {SND_MIXER_VOICE_LEFT, 0},
    {SND_MIXER_VOICE_RIGHT, 0},
    {SND_MIXER_VOICE_CENTER, 0},
    {SND_MIXER_VOICE_REAR_LEFT, 0},
    {SND_MIXER_VOICE_REAR_RIGHT, 0},
    {SND_MIXER_VOICE_WOOFER, 0},
    {SND_MIXER_VOICE_SURR_LEFT, 0},
    {SND_MIXER_VOICE_SURR_RIGHT, 0}
};

/* Digital output volume range - the SCU supports max volume of +18 dB,
   but for near to full scale PCM data, any digital volume in excess of 0dB
   will result in distortion, limiting the range to digital volume up to 0 dB */
#if _NTO_VERSION < 700
static struct snd_mixer_element_volume1_range   scu_dig_output_range[8] = {
    {0, 480, -12000, 0},    // min, max, min_dB, max_dB (SPEAKER)
    {0, 480, -12000, 0},    // min, max, min_dB, max_dB (SPEAKER)
    {0, 480, -12000, 0},    // min, max, min_dB, max_dB (SPEAKER)
    {0, 480, -12000, 0},    // min, max, min_dB, max_dB (SPEAKER)
    {0, 480, -12000, 0},    // min, max, min_dB, max_dB (SPEAKER)
    {0, 480, -12000, 0},    // min, max, min_dB, max_dB (SPEAKER)
    {0, 480, -12000, 0},    // min, max, min_dB, max_dB (SPEAKER)
    {0, 480, -12000, 0}     // min, max, min_dB, max_dB (SPEAKER)
};
#else
static struct snd_mixer_element_volume1_range   scu_dig_output_range[8] = {
    {0, 480, -12000, 0, 100},    // min, max, min_dB, max_dB, dB_scale_factor (SPEAKER)
    {0, 480, -12000, 0, 100},    // min, max, min_dB, max_dB, dB_scale_factor (SPEAKER)
    {0, 480, -12000, 0, 100},    // min, max, min_dB, max_dB, dB_scale_factor (SPEAKER)
    {0, 480, -12000, 0, 100},    // min, max, min_dB, max_dB, dB_scale_factor (SPEAKER)
    {0, 480, -12000, 0, 100},    // min, max, min_dB, max_dB, dB_scale_factor (SPEAKER)
    {0, 480, -12000, 0, 100},    // min, max, min_dB, max_dB, dB_scale_factor (SPEAKER)
    {0, 480, -12000, 0, 100},    // min, max, min_dB, max_dB, dB_scale_factor (SPEAKER)
    {0, 480, -12000, 0, 100}     // min, max, min_dB, max_dB, dB_scale_factor (SPEAKER)
};
#endif

/*
 * scu_master_vol_control using the SCU DVC block
 */
static int32_t
scu_master_vol_control
(
    MIXER_CONTEXT_T * rcar,
    ado_mixer_delement_t * element,
    uint8_t set,
    uint32_t * vol,
    void *instance_data
)
{
    int32_t altered = 0;
    uint32_t data[8];
    int i;

    for (i = 0; i < rcar->voices; i++) {
        rcar_get_dvc_output_volume(rcar, i, &data[i]);
    }

    if (set) {
        for (i = 0; i < rcar->voices; i++) {
            altered = altered || ( vol[i] != data[i] );
            data[i] = vol[i] < SCU_DVC_MAX_VOLUME ? vol[i] : SCU_DVC_MAX_VOLUME;
            ado_debug( DB_LVL_DRIVER, "rcar : setting DVC volume[%d]=0x%x", i, data[i] );
        }

        ado_mutex_lock (&rcar->hw_lock);

        for (i = 0; i < rcar->voices; i++) {
            rcar_set_dvc_output_volume(rcar, i, data[i]);
        }

        ado_mutex_unlock (&rcar->hw_lock);
    } else {
        for (i = 0; i < rcar->voices; i++) {
            vol[i] = data[i];
        }
    }
    return altered;
}

/*
 * scu_master_mute_control using the SCU DVC block
 */
static int32_t
scu_master_mute_control
(
    MIXER_CONTEXT_T * rcar,
    ado_mixer_delement_t * element,
    uint8_t set,
    uint32_t * mute,
    void *instance_data
)
{
    int32_t altered = 0;
    uint32_t data;

    rcar_get_dvc_output_mute(rcar, &data);

    if (set) {
        altered = ( (*mute & 0xFF) != data );
        data = *mute & 0xFF;

        ado_debug( DB_LVL_DRIVER, "rcar : setting DVC mute to 0x%x", data );

        ado_mutex_lock (&rcar->hw_lock);

        rcar_set_dvc_output_mute(rcar, data);

        ado_mutex_unlock (&rcar->hw_lock);
    } else {
        *mute = data;
    }
    return altered;
}

/*
 *  build_scu_mixer
 */
static int32_t
build_scu_mixer( ado_card_t * card, HW_CONTEXT_T * rcar )
{
    int32_t error = 0, status;
    ado_mixer_delement_t *pre_elem = NULL, *elem = NULL, *vol_elem = NULL, *mute_elem = NULL;
    snd_mixer_voice_t* voices = NULL;
    intptr_t chn_mask = 0;

    ado_debug (DB_LVL_MIXER, "Creating mixer");

    status = ado_mixer_create( card, rcar->use_scu ? "scu" : "mixer", &rcar->mixer, rcar );

    if ( status != EOK ) {
        ado_error_fmt ("Failed to create default mixer");
        return status;
    }

    if ( rcar->use_scu ) {
        use_scu = true;

        switch (rcar->voices) {
            case 1:
                chn_mask = SND_MIXER_CHN_MASK_MONO;
                voices = default_voices;
                break;
            case 2:
            default:
                chn_mask = SND_MIXER_CHN_MASK_STEREO;
                voices = default_voices;
                break;
            case 3:
                chn_mask = SND_MIXER_CHN_MASK_STEREO | SND_MIXER_CHN_MASK_FRONT_CENTER;
                voices = default_voices;
                break;
            case 4:
                chn_mask = SND_MIXER_CHN_MASK_4;
                voices = quad_voices;
                break;
            case 6:
                chn_mask = SND_MIXER_CHN_MASK_5_1;
                voices = five_one_voices;
                break;
            case 8:
                chn_mask = SND_MIXER_CHN_MASK_7_1;
                voices = seven_one_voices;
                break;
        }

        if (!error && (elem = ado_mixer_element_pcm1 (rcar->mixer, SND_MIXER_ELEMENT_PLAYBACK,
                                                      SND_MIXER_ETYPE_PLAYBACK1, 1, &pcm_devices[0])) == NULL) {
            error++;
        }
        pre_elem = elem;
        if (!error && (elem = ado_mixer_element_io (rcar->mixer, "PCM Out", SND_MIXER_ETYPE_OUTPUT,
                                                    0, rcar->voices, voices)) == NULL) {
            error++;
        }
        if (!error && ado_mixer_element_route_add (rcar->mixer, pre_elem, elem) != 0) {
            error++;
        }
        pre_elem = elem;
        /* #################### */
        /* the DIGITAL CONTROLS */
        /* #################### */
        /* change volume by SCU-DVC */
        if (!error && (vol_elem = ado_mixer_element_volume1 (rcar->mixer, "Digital Output Gain",
                                                             rcar->voices, scu_dig_output_range,
                                                             scu_master_vol_control,
                                                             NULL, NULL)) == NULL) {
            error++;
        }
        if (!error && ado_mixer_element_route_add (rcar->mixer, pre_elem, vol_elem) != 0) {
            error++;
        }
        pre_elem = vol_elem;
        /* change mute by SCU DVC */
        if (!error && (mute_elem = ado_mixer_element_sw1 (rcar->mixer, "DAC Mute",
                                                          rcar->voices, scu_master_mute_control,
                                                          (void*) chn_mask, NULL)) == NULL) {
            error++;
        }
        if (!error && ado_mixer_element_route_add(rcar->mixer, pre_elem, mute_elem) != 0) {
            error++;
        }
        if (!error && (ado_mixer_playback_group_create (rcar->mixer, SND_MIXER_MASTER_OUT,
                                                        chn_mask, vol_elem, mute_elem)) == NULL) {
            error++;
        }
    }

    if ( !error ) {
        return EOK;
    }

    return ENOMEM;
}

#if _NTO_VERSION >= 700
static bool rcar_mixer_is_active ( HW_CONTEXT_T *rcar, int channel )
{
    if ( channel == ADO_PCM_CHANNEL_CAPTURE ) {
        return (rcar->capture.active ? true : false);
    } else if ( channel == ADO_PCM_CHANNEL_PLAYBACK ) {
        return (rcar->playback.active ? true : false);
    }
    return (false);
}

static ado_mixer_dll_codec_callbacks_t rcar_mixer_callbacks;

static int32_t
build_codec_mixer( ado_card_t * card, HW_CONTEXT_T * rcar )
{
    int32_t status;

    memset ( &rcar_mixer_callbacks, 0, sizeof (rcar_mixer_callbacks) );

    ado_mixer_dll_codec_params_t params = {0};

    params.hw_context = rcar;

    params.tx_sample_rate = ( rcar->sample_rate ? rcar->sample_rate : rcar->sample_rate_max );
    params.rx_sample_rate = params.tx_sample_rate;

    /* The codec clock mode will be the opposite of the Audio Controllers clock mode */
    if ( rcar->ssi_masterslave_mode == SSI_MASTER_SLAVE ) {
        params.clk_master = false;
    } else {
        params.clk_master = true;
    }

    params.tx_voices = rcar->voices;
    params.rx_voices = rcar->voices;

    /* set i2c_dev and i2c_addr to invalid values, these should be passed as codec params */
    params.i2c_dev = -1;
    params.i2c_addr = -1;

    /* mixer -> ctrl callbacks */
    params.is_active = rcar_mixer_is_active;

    /* If NULL is passed in as the DLL name, this will cause the ado_mixer_dll() call
     * to look in the audio configuration file for the dll name (mixer_dll=xxxx)
     */
    if ((status = ado_mixer_dll (card, NULL, I2S_CODEC_MINOR_VERSION, &params, &rcar_mixer_callbacks, &rcar->mixer))) {
        status = errno;
    } else if ( rcar_mixer_callbacks.codec_set_default_group == NULL ) {
        ado_error_fmt("Mixer DLL missing codec_set_default_group callback");
        status = ENOTSUP;
        ado_mixer_destroy ( rcar->mixer );
    }

    return status;
}
#else
static int32_t
build_codec_mixer( ado_card_t * card, HW_CONTEXT_T * rcar )
{
    int32_t status;
#ifdef USE_VARIANT_CODEC
    if ( USE_VARIANT_CODEC ) {
        status = CODEC_MIXER_INIT( card, rcar );
        if ( status != EOK ) {
            ado_error_fmt ("Failed to init variant codec mixer");
            return status;
        }
    } else {
        status = ENOTSUP;
    }
#else
    status = ENOTSUP;
#endif
    return status;
}
#endif

int rcar_mixer_set_clock_rate( HW_CONTEXT_T * rcar, uint32_t sample_rate )
{
    int status = EOK;

#if _NTO_VERSION >= 700
    if ( rcar_mixer_callbacks.codec_set_rate ) {
        rcar_mixer_callbacks.codec_set_rate ( rcar->mixer, sample_rate, ADO_PCM_CHANNEL_PLAYBACK );
    }
#elif defined(CODEC_SET_CLOCK_RATE)
    status = CODEC_SET_CLOCK_RATE ( sample_rate );
#endif

    if ( status != EOK ) {
        ado_error_fmt ("Failed to set codec clock rate");
        return status;
    }

    return EOK;
}

/* if the driver variant supports a hardware codec mixer, build and use
 * the corresponding mixer; otherwise use a simple default mixer based
 * on SCU DVC volume control */
int rcar_mixer_init(ado_card_t* card, HW_CONTEXT_T* rcar)
{
    int status;

    if ( !card || !rcar ) {
        return EINVAL;
    }

    status = build_codec_mixer ( card, rcar );
    if ( status != EOK ) {
        status = build_scu_mixer ( card, rcar );
    }

    if ( status != EOK ) {
        ado_error_fmt ( "Failed building a mixer" );
        return status;
    }

    return EOK;
}

void rcar_mixer_register_dump()
{
#if _NTO_VERSION >= 700
    // TODO: Ideally if using the new mixer dll approach, there would be a register_dump callback
    // to invoke here
    VARIANT_REGISTER_DUMP();
#else
    CODEC_REGISTER_DUMP();
#endif
}

void
rcar_mixer_set_default_group( ado_pcm_t *pcm, ado_mixer_t *mixer, int channel, int index )
{
#if _NTO_VERSION >= 700
    /* if a mixer dll is used, use its default group handling */
    if ( rcar_mixer_callbacks.codec_set_default_group ) {
        rcar_mixer_callbacks.codec_set_default_group ( pcm, mixer, channel, index );
        return;
    }
#elif defined( USE_VARIANT_CODEC )
    /* if a variant codec mixer is available, use its default group handling */
    if ( USE_VARIANT_CODEC ) {
        CODEC_SET_DEFAULT_GROUP( pcm, mixer, channel, index );
        return;
    }
#endif

    /* handling for the default SCU mixer */
    switch (channel)
    {
        case ADO_PCM_CHANNEL_PLAYBACK:
            if (use_scu) {
                ado_pcm_chn_mixer (pcm, ADO_PCM_CHANNEL_PLAYBACK, mixer,
                    ado_mixer_find_element (mixer, SND_MIXER_ETYPE_PLAYBACK1,
                        SND_MIXER_ELEMENT_PLAYBACK, index), ado_mixer_find_group (mixer,
                        SND_MIXER_MASTER_OUT, index));
            }
            break;
        case ADO_PCM_CHANNEL_CAPTURE:
        default:
            break;
    }
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/deva/ctrl/rcar/rcar_mixer.c $ $Rev: 842276 $")
#endif
