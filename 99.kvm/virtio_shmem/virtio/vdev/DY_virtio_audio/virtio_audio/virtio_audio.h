/*
* Copyright (c) 2017, QNX Software Systems Limited (“QSS”).
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its
*    contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Additional Patent Grant
*
* QSS hereby grants to you a perpetual, worldwide, non-exclusive,
* no-charge, irrevocable (except as stated in this section) patent
* license to make, have made, use, offer to sell, sell, import,
* transfer, and otherwise run, modify and propagate the contents of this
* header file (“Implementation”) , where such license applies
* only to those patent claims, both currently owned by QSS and
* acquired in the future, licensable by QSS that are necessarily
* infringed by this Implementation. This grant does
* not include claims that would be infringed only as a consequence of
* further modification of this Implementation. If you or your agent or
* exclusive licensee institute or order or agree to the institution of
* patent litigation against any entity (including a cross-claim or
* counterclaim in a lawsuit) alleging that this Implementation constitutes
* direct or contributory patent infringement, or inducement of patent
* infringement, then any patent rights granted to you under this license for
* this Implementation shall terminate as of the date such litigation is filed.
*
* Alternatively, this software may be distributed under the terms of the
* GNU General Public License ("GPL") version 2 as published by the Free
* Software Foundation.
*/


/**
 * @file
 * definitions for virtual audio devices
 */

#ifndef __VIRTIO_AUDIO_H
#define __VIRTIO_AUDIO_H

/* Virtq types */
#define VIRTIO_AUDIO_VIRTQ_TYPE_IN     0u
#define VIRTIO_AUDIO_VIRTQ_TYPE_OUT    1u
#define VIRTIO_AUDIO_VIRTQ_TYPE_CTRL   2u
#define VIRTIO_AUDIO_VIRTQ_TYPE_NUM    3u

/* Virtq sizes */
#define VIRTIO_AUDIO_VIRTQ_SIZE_IN     128  /* max is 512 */
#define VIRTIO_AUDIO_VIRTQ_SIZE_OUT    128  /* max is 512 */
#define VIRTIO_AUDIO_VIRTQ_SIZE_CTRL   128

/* Audio features */
#define VIRTIO_AUDIO_F_PCM_AUTO_XRUN_HDL              0
#define VIRTIO_AUDIO_F_PCM_CAPTURE_PAUSE_RESUME       1
#define VIRTIO_AUDIO_F_PCM_POSITION                   2
#define VIRTIO_AUDIO_F_PCM_PLAYBACK_PAUSE_RESUME      3
#define VIRTIO_AUDIO_F_MIXER_CTL                      4

/* Audio command types */
#define VIRTIO_AUDIO_CMD_TYPE_PCM                     0
#define VIRTIO_AUDIO_CMD_TYPE_MIXER                   1

/* Audio PCM commands */
#define VIRTIO_AUDIO_CMD_PCM_ACQUIRE                  0
#define VIRTIO_AUDIO_CMD_PCM_PREPARE                  1
#define VIRTIO_AUDIO_CMD_PCM_DRAIN                    2
#define VIRTIO_AUDIO_CMD_PCM_DROP                     3
#define VIRTIO_AUDIO_CMD_PCM_PAUSE                    4
#define VIRTIO_AUDIO_CMD_PCM_RESUME                   5
#define VIRTIO_AUDIO_CMD_PCM_RELEASE                  6
#define VIRTIO_AUDIO_CMD_PCM_POSITION_GET             7
#define VIRTIO_AUDIO_CMD_PCM_FIRST                    VIRTIO_AUDIO_CMD_PCM_ACQUIRE
#define VIRTIO_AUDIO_CMD_PCM_LAST                     VIRTIO_AUDIO_CMD_PCM_POSITION_GET

/* Audio mixer control commands */
#define VIRTIO_AUDIO_CMD_MIXER_GROUP_VOLUME_GET       100
#define VIRTIO_AUDIO_CMD_MIXER_GROUP_VOLUME_SET       101
#define VIRTIO_AUDIO_CMD_MIXER_GROUP_MUTE_GET         102
#define VIRTIO_AUDIO_CMD_MIXER_GROUP_MUTE_SET         103
#define VIRTIO_AUDIO_CMD_MIXER_GROUP_BALANCE_GET      104
#define VIRTIO_AUDIO_CMD_MIXER_GROUP_BALANCE_SET      105
#define VIRTIO_AUDIO_CMD_MIXER_GROUP_FADE_GET         106
#define VIRTIO_AUDIO_CMD_MIXER_GROUP_FADE_SET         107
#define VIRTIO_AUDIO_CMD_MIXER_GROUP_SWITCH_GET       108
#define VIRTIO_AUDIO_CMD_MIXER_GROUP_SWITCH_SET       109
#define VIRTIO_AUDIO_CMD_MIXER_SWITCH_GET             110
#define VIRTIO_AUDIO_CMD_MIXER_SWITCH_SET             111
#define VIRTIO_AUDIO_CMD_MIXER_GROUP_FIRST            (VIRTIO_AUDIO_CMD_MIXER_GROUP_VOLUME_GET)
#define VIRTIO_AUDIO_CMD_MIXER_GROUP_LAST             (VIRTIO_AUDIO_CMD_MIXER_GROUP_SWITCH_SET)
#define VIRTIO_AUDIO_CMD_MIXER_SWITCH_FIRST           (VIRTIO_AUDIO_CMD_MIXER_SWITCH_GET)
#define VIRTIO_AUDIO_CMD_MIXER_SWITCH_LAST            (VIRTIO_AUDIO_CMD_MIXER_SWITCH_SET)
#define VIRTIO_AUDIO_CMD_MIXER_FIRST                  (VIRTIO_AUDIO_CMD_MIXER_GROUP_FIRST)
#define VIRTIO_AUDIO_CMD_MIXER_LAST                   (VIRTIO_AUDIO_CMD_MIXER_SWITCH_LAST)

#define VIRTIO_AUDIO_CMD_NONE                         0xFFFF

/* Audio status values */
#define VIRTIO_AUDIO_S_OK                             0
#define VIRTIO_AUDIO_S_UNSUPP                         1
#define VIRTIO_AUDIO_S_INVALID                        2
#define VIRTIO_AUDIO_S_FAILURE                        3
#define VIRTIO_AUDIO_S_BADSTATE                       4
#define VIRTIO_AUDIO_S_NOCHANNEL                      5
#define VIRTIO_AUDIO_S_XRUN                           6
#define VIRTIO_AUDIO_S_NOCTL                          7
#define VIRTIO_AUDIO_S_UNAVAIL                        8

#define VIRTIO_AUDIO_MAX_VOICES                       32

/* Audio channel types and max number */
#define VIRTIO_AUDIO_CHANNEL_PLAYBACK                 0
#define VIRTIO_AUDIO_CHANNEL_CAPTURE                  1
#define VIRTIO_AUDIO_CHANNEL_MAX                      2

/* Audio formats */
#define        VIRTIO_AUDIO_FMT_U8                       0
#define        VIRTIO_AUDIO_FMT_S8                       1
#define        VIRTIO_AUDIO_FMT_U16_LE                   2
#define        VIRTIO_AUDIO_FMT_U16_BE                   3
#define        VIRTIO_AUDIO_FMT_S16_LE                   4
#define        VIRTIO_AUDIO_FMT_S16_BE                   5
/* 24 bit PCM formats using lower three bytes in 32-bit word */
#define        VIRTIO_AUDIO_FMT_U24_LE                   6
#define        VIRTIO_AUDIO_FMT_U24_BE                   7
#define        VIRTIO_AUDIO_FMT_S24_LE                   8
#define        VIRTIO_AUDIO_FMT_S24_BE                   9
#define        VIRTIO_AUDIO_FMT_U32_LE                   10
#define        VIRTIO_AUDIO_FMT_U32_BE                   11
#define        VIRTIO_AUDIO_FMT_S32_LE                   12
#define        VIRTIO_AUDIO_FMT_S32_BE                   13
#define        VIRTIO_AUDIO_FMT_A_LAW                    14
#define        VIRTIO_AUDIO_FMT_MU_LAW                   15
#define        VIRTIO_AUDIO_FMT_IEC958_SUBFRAME_LE       16
#define        VIRTIO_AUDIO_FMT_IEC958_SUBFRAME_BE       17
#define        VIRTIO_AUDIO_FMT_AC3                      18
#define        VIRTIO_AUDIO_FMT_FLOAT_LE                 19
#define        VIRTIO_AUDIO_FMT_FLOAT_BE                 20
#define        VIRTIO_AUDIO_FMT_FLOAT64_LE               21
#define        VIRTIO_AUDIO_FMT_FLOAT64_BE               22
#define        VIRTIO_AUDIO_FMT_IMA_ADPCM                23
#define        VIRTIO_AUDIO_FMT_GSM                      24
#define        VIRTIO_AUDIO_FMT_MPEG                     25
#define        VIRTIO_AUDIO_FMT_SPECIAL                  26
/* 24 bit PCM formats using three bytes */
#define        VIRTIO_AUDIO_FMT_U24_3LE                  27
#define        VIRTIO_AUDIO_FMT_U24_3BE                  28
#define        VIRTIO_AUDIO_FMT_S24_3LE                  29
#define        VIRTIO_AUDIO_FMT_S24_3BE                  30

/* Audio format masks */
#define        VIRTIO_AUDIO_FMT_MSK_U8                   (1 << VIRTIO_AUDIO_FMT_U8)
#define        VIRTIO_AUDIO_FMT_MSK_S8                   (1 << VIRTIO_AUDIO_FMT_S8)
#define        VIRTIO_AUDIO_FMT_MSK_U16_LE               (1 << VIRTIO_AUDIO_FMT_U16_LE)
#define        VIRTIO_AUDIO_FMT_MSK_U16_BE               (1 << VIRTIO_AUDIO_FMT_U16_BE)
#define        VIRTIO_AUDIO_FMT_MSK_S16_LE               (1 << VIRTIO_AUDIO_FMT_S16_LE)
#define        VIRTIO_AUDIO_FMT_MSK_S16_BE               (1 << VIRTIO_AUDIO_FMT_S16_BE)
/* 24 bit PCM formats using lower three bytes in 32-bit word */
#define        VIRTIO_AUDIO_FMT_MSK_U24_LE               (1 << VIRTIO_AUDIO_FMT_U24_LE)
#define        VIRTIO_AUDIO_FMT_MSK_U24_BE               (1 << VIRTIO_AUDIO_FMT_U24_BE)
#define        VIRTIO_AUDIO_FMT_MSK_S24_LE               (1 << VIRTIO_AUDIO_FMT_S24_LE)
#define        VIRTIO_AUDIO_FMT_MSK_S24_BE               (1 << VIRTIO_AUDIO_FMT_S24_BE)
#define        VIRTIO_AUDIO_FMT_MSK_U32_LE               (1 << VIRTIO_AUDIO_FMT_U32_LE)
#define        VIRTIO_AUDIO_FMT_MSK_U32_BE               (1 << VIRTIO_AUDIO_FMT_U32_BE)
#define        VIRTIO_AUDIO_FMT_MSK_S32_LE               (1 << VIRTIO_AUDIO_FMT_S32_LE)
#define        VIRTIO_AUDIO_FMT_MSK_S32_BE               (1 << VIRTIO_AUDIO_FMT_S32_BE)
#define        VIRTIO_AUDIO_FMT_MSK_A_LAW                (1 << VIRTIO_AUDIO_FMT_A_LAW)
#define        VIRTIO_AUDIO_FMT_MSK_MU_LAW               (1 << VIRTIO_AUDIO_FMT_MU_LAW)
#define        VIRTIO_AUDIO_FMT_MSK_IEC958_SUBFRAME_LE   (1 << VIRTIO_AUDIO_FMT_IEC958_SUBFRAME_LE)
#define        VIRTIO_AUDIO_FMT_MSK_IEC958_SUBFRAME_BE   (1 << VIRTIO_AUDIO_FMT_IEC958_SUBFRAME_BE)
#define        VIRTIO_AUDIO_FMT_MSK_AC3                  (1 << VIRTIO_AUDIO_FMT_AC3)
#define        VIRTIO_AUDIO_FMT_MSK_FLOAT_LE             (1 << VIRTIO_AUDIO_FMT_FLOAT_LE)
#define        VIRTIO_AUDIO_FMT_MSK_FLOAT_BE             (1 << VIRTIO_AUDIO_FMT_FLOAT_BE)
#define        VIRTIO_AUDIO_FMT_MSK_FLOAT64_LE           (1 << VIRTIO_AUDIO_FMT_FLOAT64_LE)
#define        VIRTIO_AUDIO_FMT_MSK_FLOAT64_BE           (1 << VIRTIO_AUDIO_FMT_FLOAT64_BE)
#define        VIRTIO_AUDIO_FMT_MSK_IMA_ADPCM            (1 << VIRTIO_AUDIO_FMT_IMA_ADPCM)
#define        VIRTIO_AUDIO_FMT_MSK_GSM                  (1 << VIRTIO_AUDIO_FMT_GSM)
#define        VIRTIO_AUDIO_FMT_MSK_MPEG                 (1 << VIRTIO_AUDIO_FMT_MPEG)
#define        VIRTIO_AUDIO_FMT_MSK_SPECIAL              (1 << VIRTIO_AUDIO_FMT_SPECIAL)
/* 24 bit PCM formats using three bytes */
#define        VIRTIO_AUDIO_FMT_MSK_U24_3LE              (1 << VIRTIO_AUDIO_FMT_U24_3LE)
#define        VIRTIO_AUDIO_FMT_MSK_U24_3BE              (1 << VIRTIO_AUDIO_FMT_U24_3BE)
#define        VIRTIO_AUDIO_FMT_MSK_S24_3LE              (1 << VIRTIO_AUDIO_FMT_S24_3LE)
#define        VIRTIO_AUDIO_FMT_MSK_S24_3BE              (1 << VIRTIO_AUDIO_FMT_S24_3BE)

/* Audio sample rate masks */
#define        VIRTIO_AUDIO_RATE_MSK_8000                (1<<1)
#define        VIRTIO_AUDIO_RATE_MSK_11025               (1<<2)
#define        VIRTIO_AUDIO_RATE_MSK_12000               (1<<14)
#define        VIRTIO_AUDIO_RATE_MSK_16000               (1<<3)
#define        VIRTIO_AUDIO_RATE_MSK_18900               (1<<13)
#define        VIRTIO_AUDIO_RATE_MSK_22050               (1<<4)
#define        VIRTIO_AUDIO_RATE_MSK_24000               (1<<12)
#define        VIRTIO_AUDIO_RATE_MSK_32000               (1<<5)
#define        VIRTIO_AUDIO_RATE_MSK_44100               (1<<6)
#define        VIRTIO_AUDIO_RATE_MSK_48000               (1<<7)
#define        VIRTIO_AUDIO_RATE_MSK_64000               (1<<15)
#define        VIRTIO_AUDIO_RATE_MSK_88200               (1<<8)
#define        VIRTIO_AUDIO_RATE_MSK_96000               (1<<9)
#define        VIRTIO_AUDIO_RATE_MSK_176400              (1<<10)
#define        VIRTIO_AUDIO_RATE_MSK_192000              (1<<11)

/** channel positions */
typedef enum virtio_audio_chmap_position {
    VIRTIO_AUDIO_CHMAP_UNKNOWN = 0,  /** unspecified */
    VIRTIO_AUDIO_CHMAP_NA,           /** N/A, silent */
    VIRTIO_AUDIO_CHMAP_MONO,         /** mono stream */
    VIRTIO_AUDIO_CHMAP_FL,           /** front left */
    VIRTIO_AUDIO_CHMAP_FR,           /** front right */
    VIRTIO_AUDIO_CHMAP_RL,           /** rear left */
    VIRTIO_AUDIO_CHMAP_RR,           /** rear right */
    VIRTIO_AUDIO_CHMAP_FC,           /** front center */
    VIRTIO_AUDIO_CHMAP_LFE,          /** LFE */
    VIRTIO_AUDIO_CHMAP_SL,           /** side left */
    VIRTIO_AUDIO_CHMAP_SR,           /** side right */
    VIRTIO_AUDIO_CHMAP_RC,           /** rear center */
    VIRTIO_AUDIO_CHMAP_FLC,          /** front left center */
    VIRTIO_AUDIO_CHMAP_FRC,          /** front right center */
    VIRTIO_AUDIO_CHMAP_RLC,          /** rear left center */
    VIRTIO_AUDIO_CHMAP_RRC,          /** rear right center */
    VIRTIO_AUDIO_CHMAP_FLW,          /** front left wide */
    VIRTIO_AUDIO_CHMAP_FRW,          /** front right wide */
    VIRTIO_AUDIO_CHMAP_FLH,          /** front left high */
    VIRTIO_AUDIO_CHMAP_FCH,          /** front center high */
    VIRTIO_AUDIO_CHMAP_FRH,          /** front right high */
    VIRTIO_AUDIO_CHMAP_TC,           /** top center */
    VIRTIO_AUDIO_CHMAP_TFL,          /** top front left */
    VIRTIO_AUDIO_CHMAP_TFR,          /** top front right */
    VIRTIO_AUDIO_CHMAP_TFC,          /** top front center */
    VIRTIO_AUDIO_CHMAP_TRL,          /** top rear left */
    VIRTIO_AUDIO_CHMAP_TRR,          /** top rear right */
    VIRTIO_AUDIO_CHMAP_TRC,          /** top rear center */
    VIRTIO_AUDIO_CHMAP_TFLC,         /** top front left center */
    VIRTIO_AUDIO_CHMAP_TFRC,         /** top front right center */
    VIRTIO_AUDIO_CHMAP_TSL,          /** top side left */
    VIRTIO_AUDIO_CHMAP_TSR,          /** top side right */
    VIRTIO_AUDIO_CHMAP_LLFE,         /** left LFE */
    VIRTIO_AUDIO_CHMAP_RLFE,         /** right LFE */
    VIRTIO_AUDIO_CHMAP_BC,           /** bottom center */
    VIRTIO_AUDIO_CHMAP_BLC,          /** bottom left center */
    VIRTIO_AUDIO_CHMAP_BRC,          /** bottom right center */
    VIRTIO_AUDIO_CHMAP_LAST = VIRTIO_AUDIO_CHMAP_BRC,
} virtio_audio_chmap_position_t;

/* Audio control definitions */
#define        VIRTIO_AUDIO_CTL_MAX_NAME_LEN                32

/* Mixer control max number */
#define        VIRTIO_AUDIO_MIXER_CTL_MAX                   10

/* Mixer control types */
#define        VIRTIO_AUDIO_MIXER_CTL_PLAYBACK_GROUP        0
#define        VIRTIO_AUDIO_MIXER_CTL_CAPTURE_GROUP         1
#define        VIRTIO_AUDIO_MIXER_CTL_SWITCH                2
#define        VIRTIO_AUDIO_MIXER_CTL_NONE                  0xFFFF

/* Mixer group control capabilities */
#define        VIRTIO_AUDIO_MIXER_CAP_MSK_VOLUME            (1 << 0)
#define        VIRTIO_AUDIO_MIXER_CAP_MSK_JOINTLY_VOLUME    (1 << 1)
#define        VIRTIO_AUDIO_MIXER_CAP_MSK_MUTE              (1 << 2)
#define        VIRTIO_AUDIO_MIXER_CAP_MSK_JOINTLY_MUTE      (1 << 3)
#define        VIRTIO_AUDIO_MIXER_CAP_MSK_BALANCE           (1 << 4)
#define        VIRTIO_AUDIO_MIXER_CAP_MSK_FADE              (1 << 5)
#define        VIRTIO_AUDIO_MIXER_CAP_MSK_SWITCH            (1 << 6)
#define        VIRTIO_AUDIO_MIXER_CAP_MSK_JOINTLY_SWITCH    (1 << 7)
#define        VIRTIO_AUDIO_MIXER_CAP_MSK_EXCL_SWITCH       (1 << 8)

#define        VIRTIO_AUDIO_MIXER_CAP_MSK_VOLUME_ALL        (VIRTIO_AUDIO_MIXER_CAP_MSK_VOLUME|VIRTIO_AUDIO_MIXER_CAP_MSK_JOINTLY_VOLUME)
#define        VIRTIO_AUDIO_MIXER_CAP_MSK_MUTE_ALL          (VIRTIO_AUDIO_MIXER_CAP_MSK_MUTE|VIRTIO_AUDIO_MIXER_CAP_MSK_JOINTLY_MUTE)
#define        VIRTIO_AUDIO_MIXER_CAP_MSK_SWITCH_ALL        (VIRTIO_AUDIO_MIXER_CAP_MSK_SWITCH|VIRTIO_AUDIO_MIXER_CAP_MSK_JOINTLY_SWITCH|\
                                                             VIRTIO_AUDIO_MIXER_CAP_MSK_EXCL_SWITCH)
#define        VIRTIO_AUDIO_MIXER_CAP_MSK_BALANCE_FADE_ALL  (VIRTIO_AUDIO_MIXER_CAP_MSK_BALANCE|VIRTIO_AUDIO_MIXER_CAP_MSK_FADE)
#define        VIRTIO_AUDIO_MIXER_CAP_MSK_ALL               (VIRTIO_AUDIO_MIXER_CAP_MSK_VOLUME_ALL|VIRTIO_AUDIO_MIXER_CAP_MSK_MUTE_ALL|\
                                                             VIRTIO_AUDIO_MIXER_CAP_MSK_BALANCE_FADE_ALL|VIRTIO_AUDIO_MIXER_CAP_MSK_SWITCH_ALL)

/* Mixer switch control definitions */
#define VIRTIO_AUDIO_MIXER_SWITCH_MAX_ENUM_ITEMS         12
#define VIRTIO_AUDIO_MIXER_SWITCH_MAX_ENUM_ITEM_LEN      32
#define VIRTIO_AUDIO_MIXER_SWITCH_TYPE_INTEGER           0
#define VIRTIO_AUDIO_MIXER_SWITCH_TYPE_INTEGER64         1
#define VIRTIO_AUDIO_MIXER_SWITCH_TYPE_ENUMERATED        2

/*
 * Define device specific configuration - for the virtio_audio case the device
 * capabilities, restricted in range via the qvm config file, are treated as device config
 */
typedef struct virtio_audio_chan_config {
    uint32_t    channel_type;      /* VIRTIO_AUDIO_CHANNEL_PLAYBACK or VIRTIO_AUDIO_CHANNEL_CAPTURE */
    uint32_t    rates;             /* bit mask of VIRTIO_AUDIO_RATE_MSK_* values */
    uint32_t    formats;           /* bit mask of VIRTIO_AUDIO_FMT_MSK_* values */
    uint32_t    min_fragsize;      /* min fragment size in bytes */
    uint32_t    max_fragsize;      /* max fragment size in bytes */
    uint32_t    min_voices;
    uint32_t    max_voices;
    uint32_t    min_frags;
    uint32_t    fragment_align;    /* fragment alignment in bytes */
    uint32_t    max_buffersize;    /* max buffer size in bytes */
    /* the first max_voices array values are populated with actual channel positions
     * the remaining array values are populated with VIRTIO_AUDIO_CHMAP_UNKNOWN
     */
    uint32_t    chmap_pos[VIRTIO_AUDIO_MAX_VOICES];
} virtio_audio_chan_config_t;

typedef struct virtio_audio_range {
    int32_t min, max;
} virtio_audio_range_t;

typedef struct virtio_audio_mixer_ctl_config {
    char      name[VIRTIO_AUDIO_CTL_MAX_NAME_LEN]; /* name identifying uniquely this mixer control to the guest */
    uint32_t  mixer_ctl_type;                      /* VIRTIO_AUDIO_MIXER_CTL_* value */
    uint32_t  pad;                                 /* pad for 64 bit alignment of union containing struct with 64 bit fields */
    union {
        struct {
            uint32_t             voices;
            uint32_t             capabilities;
            virtio_audio_range_t vol_range;
            virtio_audio_range_t balance_range;
            virtio_audio_range_t fade_range;
            virtio_audio_range_t db_vol_range;
            uint32_t             db_scale_factor;
            uint32_t             chmap_pos[VIRTIO_AUDIO_MAX_VOICES];
        } group;
        struct {
            uint32_t type;                        /* one of the VIRTIO_AUDIO_MIXER_SWITCH_TYPE_* values */
            uint32_t pad;                         /* pad for 64 bit alignment of union containing struct with 64 bit fields */
            union {
                struct {
                    int32_t min, max, step;
                } integer;
                struct {
                    int64_t min, max, step;
                } integer64;
                struct {
                    uint32_t item_num;
                    uint32_t pad;                 /* pad for 64 bit alignment of two dimensional array */
                    char     item_list[VIRTIO_AUDIO_MIXER_SWITCH_MAX_ENUM_ITEMS][VIRTIO_AUDIO_MIXER_SWITCH_MAX_ENUM_ITEM_LEN];
                } enumerated;
            };
        } sw;
    };
} virtio_audio_mixer_ctl_config_t;

typedef struct virtio_audio_device_config {
    uint32_t                        channel_num;
    uint32_t                        mixer_ctl_num;
    virtio_audio_chan_config_t      channel_config[VIRTIO_AUDIO_CHANNEL_MAX];
    virtio_audio_mixer_ctl_config_t mixer_ctl_config[VIRTIO_AUDIO_MIXER_CTL_MAX];
} virtio_audio_device_config_t;

/* data type for acquire command params */
typedef struct virtio_audio_acquire_params {
    uint32_t    rate;           /* sample rate in Hz */
    uint32_t    voices;
    uint32_t    format;         /* VIRTIO_AUDIO_FMT_* value */
    uint32_t    frag_size;      /* requested fragment size in bytes */
    uint32_t    frags;          /* requested total number of fragments */
} virtio_audio_acquire_params_t;

/* data type for acquire command status */
typedef struct virtio_audio_acquire_status {
    uint32_t    frag_size;
    uint32_t    frags;
} virtio_audio_acquire_status_t;

typedef struct virtio_audio_position_status {
    uint32_t frag_cnt;          /* completed fragment count, reset when preparing the channel */
    uint32_t byte_cnt;          /* processed byte count for current fragment */
} virtio_audio_position_status_t;

typedef struct virtio_audio_pcm_cmd_params {
    uint32_t    channel_type;   /* VIRTIO_AUDIO_CHANNEL_PLAYBACK or VIRTIO_AUDIO_CHANNEL_CAPTURE */
    uint32_t    pad;            /* pad for 64 bit alignment of union that could be extended to contain struct with 64 bit fields */
    union {
        virtio_audio_acquire_params_t acquire;
    };
} virtio_audio_pcm_cmd_params_t;

typedef struct virtio_audio_pcm_cmd_status {
    uint32_t    channel_type;   /* VIRTIO_AUDIO_CHANNEL_PLAYBACK or VIRTIO_AUDIO_CHANNEL_CAPTURE */
    uint32_t    pad;            /* pad for 64 bit alignment of union that could be extended to contain struct with 64 bit fields */
    union {
        virtio_audio_acquire_status_t  acquire;
        virtio_audio_position_status_t position;
    };
} virtio_audio_pcm_cmd_status_t;

typedef struct virtio_audio_mixergrp_mute_params {
    uint32_t mute;              /* channel mute bitmask (channel order based on chmap) */
} virtio_audio_mixergrp_mute_params_t;
typedef virtio_audio_mixergrp_mute_params_t virtio_audio_mixergrp_mute_status_t;

typedef struct virtio_audio_mixergrp_volume_params {
    uint32_t voices;
    int32_t  vol[VIRTIO_AUDIO_MAX_VOICES]; /* contiguous array of channel volume values (channel order based on chmap) */
} virtio_audio_mixergrp_volume_params_t;
typedef virtio_audio_mixergrp_volume_params_t virtio_audio_mixergrp_volume_status_t;

typedef struct virtio_audio_mixergrp_balance_params {
    int32_t  balance;
} virtio_audio_mixergrp_balance_params_t;
typedef virtio_audio_mixergrp_balance_params_t virtio_audio_mixergrp_balance_status_t;

typedef struct virtio_audio_mixergrp_fade_params {
    int32_t  fade;
} virtio_audio_mixergrp_fade_params_t;
typedef virtio_audio_mixergrp_fade_params_t virtio_audio_mixergrp_fade_status_t;

typedef struct virtio_audio_mixergrp_switch_params {
    uint32_t switch_on;         /* channel switch bitmask (channel order based on chmap) */
} virtio_audio_mixergrp_switch_params_t;
typedef virtio_audio_mixergrp_switch_params_t virtio_audio_mixergrp_switch_status_t;

typedef struct virtio_audio_mixersw_params {
    union {
        int32_t  integer;
        int64_t  integer64;
        uint32_t item;          /* index of selected enumeration item */
    };
} virtio_audio_mixersw_params_t;
typedef virtio_audio_mixersw_params_t virtio_audio_mixersw_status_t;

typedef struct virtio_audio_ctl_cmd_params {
    char ctl_name[VIRTIO_AUDIO_CTL_MAX_NAME_LEN];
    union {
        virtio_audio_mixergrp_volume_params_t   mixergrp_volume;
        virtio_audio_mixergrp_mute_params_t     mixergrp_mute;
        virtio_audio_mixergrp_balance_params_t  mixergrp_balance;
        virtio_audio_mixergrp_fade_params_t     mixergrp_fade;
        virtio_audio_mixergrp_switch_params_t   mixergrp_switch;
        virtio_audio_mixersw_params_t           mixersw;
    };
} virtio_audio_ctl_cmd_params_t;

typedef struct virtio_audio_ctl_cmd_status {
    char ctl_name[VIRTIO_AUDIO_CTL_MAX_NAME_LEN];
    union {
        virtio_audio_mixergrp_volume_status_t       mixergrp_volume;
        virtio_audio_mixergrp_mute_status_t         mixergrp_mute;
        virtio_audio_mixergrp_balance_status_t      mixergrp_balance;
        virtio_audio_mixergrp_fade_status_t         mixergrp_fade;
        virtio_audio_mixergrp_switch_status_t       mixergrp_switch;
        virtio_audio_mixersw_status_t               mixersw;
    };
} virtio_audio_ctl_cmd_status_t;

typedef struct virtio_audio_cmd {
    uint32_t    id;
    uint32_t    pad;            /* pad for 64 bit alignment of union containing struct with 64 bit fields */
    union {
        virtio_audio_pcm_cmd_params_t pcm;
        virtio_audio_ctl_cmd_params_t ctl;
    };
} virtio_audio_cmd_t;

typedef struct virtio_audio_cmd_status {
    uint32_t    id;
    uint32_t    status;
    union {
        virtio_audio_pcm_cmd_status_t pcm;
        virtio_audio_ctl_cmd_status_t ctl;
    };
} virtio_audio_cmd_status_t;

typedef struct virtio_audio_io_status {
    uint32_t    status;
} virtio_audio_io_status_t;

#endif /* __VIRTIO_AUDIO_H */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/qpdc/mainline/vdev/virtio-audio/virtio_audio.h $ $Rev: 948180 $")
#endif
