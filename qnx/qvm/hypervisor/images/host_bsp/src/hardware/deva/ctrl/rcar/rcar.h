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

#ifndef _R_Car_H
#define _R_Car_H

struct  rcar_context;
struct  rcar_audio_channel;

#define  HW_CONTEXT_T           struct rcar_context
#define  PCM_SUBCHN_CONTEXT_T   struct rcar_audio_channel

#include <audio_driver.h>
#include <errno.h>
#include <stdint.h>
#include "dmac.h"
#include "ssiu.h"

typedef struct rcar_audio_channel
{
    ado_pcm_cap_t         pcm_caps;
    ado_pcm_hw_t          pcm_funcs;
    ado_pcm_subchn_t      *subchn;
    audio_dmac_context_t  dma_context;
    uint32_t              ssi_chan;
    uint32_t              src_chan;
    uint32_t              cmd_chan;      /* only applicable to playback channels */
    uint32_t              hdmi_chan;
    uint32_t              dvc_volume[8]; /* only applicable to playback channels */
    uint32_t              dvc_mute;      /* only applicable to playback channels */
    uint32_t              active;
} rcar_audio_channel_t;

typedef struct rcar_context
{
    ado_mutex_t              hw_lock;
    ado_pcm_t                *pcm;
    ado_mixer_t              *mixer;
    rcar_audio_channel_t     playback;  /* Settings for one playback channel.*/
    rcar_audio_channel_t     capture;   /* Settings for one capture channel.*/
    uint32_t                 use_scu;   /* use the Sampling Rate Convertor Unit (SCU) */
    uint32_t                 use_mlp;   /* use the Media LB Port (MLP) */
    uint32_t                 use_dtcp;  /* use Digital Transmission Content Protection (available in conjunction with MLP) */
    uint32_t                 voices;
    uint32_t                 sample_rate_min;
    uint32_t                 sample_rate_max;
    uint32_t                 sample_rate;
    uint32_t                 sample_size;    /* number of bits per sample */
    uint32_t                 slot_size;      /* number of bits in a frame includes both phases */
    ssi_masterslave_mode_t   ssi_masterslave_mode;
    uint32_t                 master_ssi_channel;
    ssi_op_mode_t            ssi_op_mode;
    ssi_transfer_mode_t      ssi_transfer_mode;
    ssi_start_mode_t         ssi_start_mode;
    ssi_pin_mode_t           ssi_pin_mode;
    ssi_config_t             ssi_config;
    uint32_t                 ssi_voices; /* if the TDM split feature is used, this can be different from the host voices */
    /* MCLK rates: can be supplied as fixed rates or multiples of the sample rate; in the cases where MCLK rates are supplied as
     * fixed rates, two MCLK sources are required, in order to support for 48 kHz and 44.1 kHz based sample rates - mclk_1 and
     * mclk_2 are used; in the case when MCLK is supplied as a multiple of the sample rate, a single clock source is required -
     * mclk_fs is used */
    uint32_t                 mclk_1; /* MCLK rate for MCLK source 1 when MCLK supplied as fixed rate */
    uint32_t                 mclk_2; /* MCLK rate for MCLK source 2 when MCLK supplied as fixed rate */
    uint32_t                 mclk_fs; /* MCLK rate for the case when MCLK is a multiple of the sample rate */
    uint32_t                 debug;
    int                      i2c_dev;
} rcar_context_t;

int rcar_set_dvc_output_volume(rcar_context_t * rcar, uint32_t voice_channel, uint32_t vol);
int rcar_get_dvc_output_volume(rcar_context_t * rcar, uint32_t voice_channel, uint32_t *vol);

int rcar_set_dvc_output_mute(rcar_context_t * rcar, uint32_t mute);
int rcar_get_dvc_output_mute(rcar_context_t * rcar, uint32_t *mute);

#endif /* _R_Car_H */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/deva/ctrl/rcar/rcar.h $ $Rev: 850894 $")
#endif
