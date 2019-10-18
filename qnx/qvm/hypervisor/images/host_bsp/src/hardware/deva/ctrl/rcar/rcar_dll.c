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

#include <unistd.h>
#include <string.h>

#include "rcar.h"
#include "ssiu.h"
#include "scu.h"
#include "dmac.h"
#include "adg.h"
#include "rcar_support.h"
#include "rcar_rsrc.h"
#include "rcar_mixer.h"

#include "variant.h"
#define CS_MACHINE_LEN (32 + 1)

static inline uint32_t count_set_bits( uint32_t bitmask ) {
    uint32_t count = 0;
    while (bitmask) { bitmask &= (bitmask-1); count++; }
    return count;
}

/* supported_rate_list is a bitmask of RCAR-H2 hardware supported rates */
static const uint32_t supported_rate_list = SND_PCM_RATE_8000 |
                                            SND_PCM_RATE_11025 |
                                            SND_PCM_RATE_16000 |
                                            SND_PCM_RATE_22050 |
                                            SND_PCM_RATE_32000 |
                                            SND_PCM_RATE_44100 |
                                            SND_PCM_RATE_48000 |
                                            SND_PCM_RATE_88200 |
                                            SND_PCM_RATE_96000 |
                                            SND_PCM_RATE_176400 |
                                            SND_PCM_RATE_192000;

#define RCAR_NUM_SUPPORTED_RATES (count_set_bits(supported_rate_list))

/* configured_rate_list is a bitmask of configured rates - has to be a subset of supported rates */
static uint32_t configured_rate_list;

static int rcar_set_clock_rate (rcar_context_t * rcar);
static void rcar_register_dump( HW_CONTEXT_T * rcar );

static int32_t rcar_capabilities(HW_CONTEXT_T* rcar, ado_pcm_t *pcm, snd_pcm_channel_info_t* info)
{
    int chn_avail;

    ado_debug( DB_LVL_DRIVER, "CAPABILITIES" );

    if( rcar->pcm != pcm ) {
        ado_error_fmt( "invalid pcm" );
        return EINVAL;
    }

    chn_avail = 1;

    info->rates = rcar->playback.pcm_caps.rates;
    info->min_rate = rcar->sample_rate_min;
    info->max_rate = rcar->sample_rate_max;

    if (info->channel == SND_PCM_CHANNEL_PLAYBACK) {
        if ( rcar->playback.subchn ) {
            chn_avail = 0;
        } else if (rcar->sample_rate_min != rcar->sample_rate_max) {

            ado_mutex_lock(&rcar->hw_lock);

            /* Playback and Capture are Rate locked, so adjust rate capabilities
             * if the other side has been aquired.
             */
            if ( rcar->sample_rate ) {
                info->min_rate = info->max_rate = rcar->sample_rate;
                info->rates = ado_pcm_rate2flag(rcar->sample_rate);
            }

            ado_mutex_unlock(&rcar->hw_lock);
        }
    } else if (info->channel == SND_PCM_CHANNEL_CAPTURE) {
        if ( rcar->capture.subchn ) {
            chn_avail = 0;
        } else if (rcar->sample_rate_min != rcar->sample_rate_max) {

            ado_mutex_lock(&rcar->hw_lock);

            /* Playback and Capture are Rate locked, so adjust rate capabilities
             * if the other side has been aquired (in this case rate is non-zero)
             */
            if ( rcar->sample_rate ) {
                info->min_rate = info->max_rate = rcar->sample_rate;
                info->rates = ado_pcm_rate2flag(rcar->sample_rate);
            }

            ado_mutex_unlock(&rcar->hw_lock);
        }
    }

    if (chn_avail == 0) {
        info->formats       = 0;
        info->rates         = 0;
        info->min_rate      = 0;
        info->max_rate      = 0;
        info->min_voices    = 0;
        info->max_voices    = 0;
        info->min_fragment_size = 0;
        info->max_fragment_size = 0;
    }

    return EOK;
}

static int32_t rcar_playback_acquire (HW_CONTEXT_T * rcar, PCM_SUBCHN_CONTEXT_T ** pc,
        ado_pcm_config_t * config, ado_pcm_subchn_t * subchn, uint32_t * why_failed)
{
    int status;

    ado_debug ( DB_LVL_DRIVER, "ACQUIRE" );

    ado_mutex_lock( &rcar->hw_lock );

    if( rcar->playback.subchn ) {
        *why_failed = SND_PCM_PARAMS_NO_CHANNEL;
        ado_mutex_unlock( &rcar->hw_lock );
        ado_error_fmt( "no channel available" );
        return EAGAIN;
    }

    rcar->playback.active = 0;

    if( rcar->sample_rate_min == rcar->sample_rate_max ) {
        if( config->format.rate != rcar->sample_rate_min ) {
            ado_mutex_unlock (&rcar->hw_lock);
            ado_error_fmt( "rate not supported: %d", config->format.rate );
            return EINVAL;
        }
    } else {
        if( rcar->sample_rate && config->format.rate != rcar->sample_rate ) {
            ado_mutex_unlock (&rcar->hw_lock);
            ado_error_fmt( "rate is locked by capture session: locked rate: %d, requested rate: %d",
                       rcar->sample_rate, config->format.rate );
            return EBUSY;
        }
    }

    rcar->sample_rate = config->format.rate;

    status = rcar_set_clock_rate( rcar );
    if( status != EOK ) {
        ado_mutex_unlock (&rcar->hw_lock);
        ado_error_fmt( "failed setting the clock rate" );
        return status;
    }

    /* Allocate DMA transfer buffer*/

    // added ADO_BUF_CACHE flag to flags used in old driver
    if( ado_pcm_buf_alloc( config, config->dmabuf.size,
                           ADO_SHM_DMA_SAFE | ADO_BUF_CACHE ) == NULL ) {
        ado_mutex_unlock (&rcar->hw_lock);
        ado_error_fmt( "failed allocating shared memory" );
        return ENOMEM;
    }

    ado_debug( DB_LVL_DRIVER, "dmabuf.size = %X ", config->dmabuf.size );

    if ( !rcar->use_scu ) {
        audio_peripheral_t dest;
        if( rcar->ssi_transfer_mode == SSI_BUSIF_TRANSFER ) {
            dest = AUDIO_PERIPHERAL_SSI_BUSIF(rcar->playback.ssi_chan,0);
        } else {
            dest = AUDIO_PERIPHERAL_SSI(rcar->playback.ssi_chan);
        }
        /* Not to use SCU: DMA transfer from Memory to SSI */
        status = audio_dmac_mp_setup( rcar->playback.dma_context.audiodma_chn,
                                      dest,
                                      config->dmabuf.phys_addr,
                                      config->dmabuf.size );
        if( status != EOK ) {
            ado_error_fmt( "failed setting up dmac mp to peripheral %x", dest );
        }
    } else {
        /* To use SCU: DMA transfer from Memory to SRC */
        status = audio_dmac_mp_setup( rcar->playback.dma_context.audiodma_chn,
                                      AUDIO_PERIPHERAL_SCUSRC(rcar->playback.src_chan),
                                      config->dmabuf.phys_addr,
                                      config->dmabuf.size);

        if( status != EOK ) {
            ado_error_fmt( "failed setting up dmac mp to peripheral %x",
                       AUDIO_PERIPHERAL_SCUSRC(rcar->playback.src_chan) );
        } else {
            /* Setup peripheral-peripheral DMA transfer from SCU-CMD0 to SSI0-0 */
            status = audio_dmac_pp_setup( rcar->playback.dma_context.audiodma_pp_chn,
                                           AUDIO_PERIPHERAL_SCUCMD(rcar->playback.cmd_chan),
                                           AUDIO_PERIPHERAL_SSI_BUSIF(rcar->playback.ssi_chan,0) );
            if( status != EOK ) {
                ado_error_fmt( "failed setting up dmac pp between peripherals %x and %x",
                           AUDIO_PERIPHERAL_SCUCMD(rcar->playback.cmd_chan),
                           AUDIO_PERIPHERAL_SSI_BUSIF(rcar->playback.ssi_chan,0) );
            }
        }
    }

    if( status == EOK ) {
        rcar->playback.subchn = subchn;
        *pc = &rcar->playback;
    } else {
        ado_pcm_buf_free (config);
        config->dmabuf.addr = NULL;
        rcar->sample_rate = 0;
    }

    ado_mutex_unlock (&rcar->hw_lock);

    return EOK;
}

/* */
/*  Playback release*/
/* */
static int32_t rcar_playback_release (HW_CONTEXT_T * rcar, PCM_SUBCHN_CONTEXT_T * pc,
    ado_pcm_config_t * config)
{
    ado_debug( DB_LVL_DRIVER, "RELEASE" );

    ado_mutex_lock (&rcar->hw_lock);

    rcar->playback.subchn = NULL;
    if( !rcar->capture.subchn ) {
        rcar->sample_rate = 0;
    }

    if( config->dmabuf.addr ) {
        (void)ado_pcm_buf_free (config);
        config->dmabuf.addr = NULL;
    }

    ado_mutex_unlock (&rcar->hw_lock);

    return (EOK);
}

static int32_t rcar_capture_acquire (HW_CONTEXT_T * rcar, PCM_SUBCHN_CONTEXT_T ** pc,
    ado_pcm_config_t * config, ado_pcm_subchn_t * subchn, uint32_t * why_failed)
{
    int status;

    ado_debug( DB_LVL_DRIVER, "ACQUIRE" );

    ado_mutex_lock (&rcar->hw_lock);

    if( rcar->capture.subchn ) {
        *why_failed = SND_PCM_PARAMS_NO_CHANNEL;
        ado_mutex_unlock( &rcar->hw_lock );
        ado_error_fmt( "no channel available" );
        return EAGAIN;
    }

    rcar->capture.active = 0;

    if( rcar->sample_rate_min == rcar->sample_rate_max ) {
        if( config->format.rate != rcar->sample_rate_min ) {
            ado_mutex_unlock (&rcar->hw_lock);
            ado_error_fmt( "rate not supported: %d", config->format.rate );
            return EINVAL;
        }
    } else {
        if( rcar->sample_rate && config->format.rate != rcar->sample_rate ) {
            ado_mutex_unlock (&rcar->hw_lock);
            ado_error_fmt( "rate is locked by playback session: locked rate: %d, requested rate: %d",
                       rcar->sample_rate, config->format.rate );
            return EBUSY;
        }
    }

    rcar->sample_rate = config->format.rate;

    status = rcar_set_clock_rate( rcar );
    if( status != EOK ) {
        ado_mutex_unlock (&rcar->hw_lock);
        ado_error_fmt( "failed setting the clock rate" );
        return status;
    }

    /* Allocate DMA transfer buffer*/

    // added ADO_BUF_CACHE flag to flags used in old driver
    if( ado_pcm_buf_alloc( config, config->dmabuf.size,
                           ADO_SHM_DMA_SAFE | ADO_BUF_CACHE ) == NULL ) {
        ado_mutex_unlock (&rcar->hw_lock);
        ado_error_fmt( "failed allocating shared memory" );
        return ENOMEM;
    }

    ado_debug( DB_LVL_DRIVER, "dmabuf.size = %X ", config->dmabuf.size );

    if ( !rcar->use_scu ) {
        audio_peripheral_t src;
        if( rcar->ssi_transfer_mode == SSI_BUSIF_TRANSFER ) {
            src = AUDIO_PERIPHERAL_SSI_BUSIF(rcar->capture.ssi_chan,0);
        } else {
            src = AUDIO_PERIPHERAL_SSI(rcar->capture.ssi_chan);
        }
        /* Not to use SCU: DMA transfer from SSI to Memory */
        status = audio_dmac_pm_setup( rcar->capture.dma_context.audiodma_chn,
                                      src,
                                      config->dmabuf.phys_addr,
                                      config->dmabuf.size );
        if( status != EOK ) {
            ado_error_fmt( "failed setting up dmac pm from peripheral %x", src );
        }
    } else {
        status = audio_dmac_pm_setup( rcar->capture.dma_context.audiodma_chn,
                                      AUDIO_PERIPHERAL_SCUSRC(rcar->capture.src_chan),
                                      config->dmabuf.phys_addr,
                                      config->dmabuf.size );
        if( status != EOK ) {
            ado_error_fmt( "failed setting up dmac pm from peripheral %x",
                       AUDIO_PERIPHERAL_SCUSRC(rcar->capture.src_chan) );
        } else {
            /* Setup peripheral-peripheral DMA transfer from SSIx-0 to SCU-SRCy */
            status = audio_dmac_pp_setup( rcar->capture.dma_context.audiodma_pp_chn,
                                          AUDIO_PERIPHERAL_SSI_BUSIF(rcar->capture.ssi_chan,0),
                                          AUDIO_PERIPHERAL_SCUSRC(rcar->capture.src_chan) );
            if( status != EOK ) {
                ado_error_fmt( "failed setting up dmac pp between peripherals %x and %x",
                           AUDIO_PERIPHERAL_SSI_BUSIF(rcar->capture.ssi_chan,0),
                           AUDIO_PERIPHERAL_SCUSRC(rcar->capture.src_chan) );
            }
        }
    }

    if( status == EOK ) {
        rcar->capture.subchn = subchn;
        *pc = &rcar->capture;
    } else {
        ado_pcm_buf_free (config);
        config->dmabuf.addr = NULL;
        rcar->sample_rate = 0;
    }

    ado_mutex_unlock (&rcar->hw_lock);

    return EOK;
}


/* */
/*  Recording release */
/* */
static int32_t rcar_capture_release (HW_CONTEXT_T * rcar, PCM_SUBCHN_CONTEXT_T * pc,
    ado_pcm_config_t * config)
{
    ado_debug (DB_LVL_DRIVER, "RELEASE");

    ado_mutex_lock (&rcar->hw_lock);

    rcar->capture.subchn = NULL;
    if( !rcar->playback.subchn ) {
        rcar->sample_rate = 0;
    }

    /* Free DMA transfer buffer*/
    if( config->dmabuf.addr ) {
        ado_pcm_buf_free (config);
        config->dmabuf.addr = NULL;
    }

    ado_mutex_unlock (&rcar->hw_lock);

    return (EOK);
}

static int32_t rcar_playback_prepare (HW_CONTEXT_T * rcar, PCM_SUBCHN_CONTEXT_T * pc, ado_pcm_config_t * config)
{
    int status = EOK;

    ado_debug (DB_LVL_DRIVER, "PREPARE");

    ado_mutex_lock (&rcar->hw_lock);

    /* Without rcar_set_clock_rate in the rcar_playback_prepare/rcar_capture_prepare
     * callbacks, resume from pause or underrun/overrun recovery would hang occasionally
     * TODO: investigate with Renesas why rcar_set_clock_rate required in the
     * rcar_playback_prepare/rcar_capture_prepare callbacks
     */
    status = rcar_set_clock_rate( rcar );
    if( status != EOK ) {
        ado_error_fmt( "failed setting the clock rate" );
        ado_mutex_lock (&rcar->hw_lock);
        return status;
    }

    /* TBD: why is memory to peripheral DMA set-up repeated here (already done in acquire)? */
    if ( !rcar->use_scu ) {
        audio_peripheral_t dest;
        if( rcar->ssi_transfer_mode == SSI_BUSIF_TRANSFER ) {
            dest = AUDIO_PERIPHERAL_SSI_BUSIF(rcar->playback.ssi_chan,0);
        } else {
            dest = AUDIO_PERIPHERAL_SSI(rcar->playback.ssi_chan);
        }
        /* Not to use SCU: DMA transfer from Memory to SSI */
        status = audio_dmac_mp_setup( rcar->playback.dma_context.audiodma_chn,
                                      dest,
                                      config->dmabuf.phys_addr,
                                      config->dmabuf.size );
        if( status != EOK ) {
            ado_error_fmt( "failed setting up dmac mp to peripheral %x", dest );
        }
    } else {
        /* To use SCU: DMA transfer from Memory to SRC */
        status = audio_dmac_mp_setup( rcar->playback.dma_context.audiodma_chn,
                                      AUDIO_PERIPHERAL_SCUSRC(rcar->playback.src_chan),
                                      config->dmabuf.phys_addr,
                                      config->dmabuf.size );
        if( status != EOK ) {
            ado_error_fmt( "failed setting up dmac mp to peripheral %x",
                       AUDIO_PERIPHERAL_SCUSRC(rcar->playback.src_chan) );
        }
    }

    ado_mutex_unlock (&rcar->hw_lock);

    return status;
}

static int32_t rcar_capture_prepare (HW_CONTEXT_T * rcar, PCM_SUBCHN_CONTEXT_T * pc, ado_pcm_config_t * config)
{
    int status = EOK;

    ado_debug (DB_LVL_DRIVER, "PREPARE");

    ado_mutex_lock (&rcar->hw_lock);

    /* Without rcar_set_clock_rate in the rcar_playback_prepare/rcar_capture_prepare
     * callbacks, resume from pause or underrun/overrun recovery would hang occasionally
     * TODO: investigate with Renesas why rcar_set_clock_rate required in the
     * rcar_playback_prepare/rcar_capture_prepare callbacks
     */
    status = rcar_set_clock_rate( rcar );
    if( status != EOK ) {
        ado_mutex_unlock (&rcar->hw_lock);
        ado_error_fmt( "failed setting the clock rate" );
        return status;
    }

    if ( !rcar->use_scu ) {
       audio_peripheral_t src;
        if( rcar->ssi_transfer_mode == SSI_BUSIF_TRANSFER ) {
            src = AUDIO_PERIPHERAL_SSI_BUSIF(rcar->capture.ssi_chan,0);
        } else {
            src = AUDIO_PERIPHERAL_SSI(rcar->capture.ssi_chan);
        }
        /* Not to use SCU: DMA transfer from SSI to Memory */
        status = audio_dmac_pm_setup( rcar->capture.dma_context.audiodma_chn,
                                      src,
                                      config->dmabuf.phys_addr,
                                      config->dmabuf.size );
        if( status != EOK ) {
            ado_error_fmt( "failed setting up dmac pm from peripheral %x", src );
        }
    } else {
        /* To use SCU: DMA transfer from SRC to memory */
        status = audio_dmac_pm_setup( rcar->capture.dma_context.audiodma_chn,
                                      AUDIO_PERIPHERAL_SCUSRC(rcar->capture.src_chan),
                                      config->dmabuf.phys_addr,
                                      config->dmabuf.size );
        if( status != EOK ) {
            ado_error_fmt( "failed setting up dmac pm from peripheral %x",
                       AUDIO_PERIPHERAL_SCUSRC(rcar->capture.src_chan) );
        }
    }

    ado_mutex_unlock (&rcar->hw_lock);

    return status;
}

static int32_t rcar_playback_trigger (HW_CONTEXT_T * rcar, PCM_SUBCHN_CONTEXT_T * pc, uint32_t cmd)
{
    int idst_status;

    ado_debug (DB_LVL_DRIVER, "TRIGGER - cmd : %d", cmd);

    if( pc->subchn != rcar->playback.subchn ) {
        ado_debug( DB_LVL_DRIVER, "unknown subchn" );
        return EINVAL;
    }

    if( cmd == ADO_PCM_TRIGGER_GO ) {
        ado_debug( DB_LVL_DRIVER, "ADO_PCM_TRIGGER_GO" );

        /* Start Audio-DMAC */
        ado_debug( DB_LVL_DRIVER, "Start Audio DMAC" );
        audio_dmac_start( rcar->playback.dma_context.audiodma_chn );

        if( rcar->use_scu ) {
            /* Start Peripheral-Peripheral DMAC */
            ado_debug( DB_LVL_DRIVER, "Start Audio DMAC PP" );
            audio_dmac_pp_start( rcar->playback.dma_context.audiodma_pp_chn );
        }

        if( rcar->ssi_start_mode == SSI_SYNC_SSI0129_START ) {
            /* Multichannel SSI */

            /* start the individual SSIs */
            ado_debug( DB_LVL_DRIVER, "Start SSI 0,1,2 (no CR.EN)" );
            ssi_start( SSI_CHANNEL_0, SSI_SYNC_SSI0129_START );
            ssi_start( SSI_CHANNEL_1, SSI_SYNC_SSI0129_START );
            ssi_start( SSI_CHANNEL_2, SSI_SYNC_SSI0129_START );
            if( rcar->voices == 8 ) {
                ado_debug( DB_LVL_DRIVER, "Start SSI 9 (no CR.EN)" );
                ssi_start( SSI_CHANNEL_9, SSI_SYNC_SSI0129_START );
            }

            /* start in a synchronized fashion the SSI0129 or SSI012 channels */
            ado_debug( DB_LVL_DRIVER, "Synchronized start of SSI 0,1,2(,9)" );
            ssiu_start( SSI_SYNC_SSI0129_START );
        } else if( rcar->ssi_start_mode == SSI_SYNC_SSI34_START ) {
            /* SSI 3,4 configured for synchronized start */

            /* start the individual SSIs */
            ado_debug( DB_LVL_DRIVER, "Start SSI 3,4 (no CR.EN)" );
            ssi_start( SSI_CHANNEL_3, SSI_SYNC_SSI34_START );
            ssi_start( SSI_CHANNEL_4, SSI_SYNC_SSI34_START );

            /* start in a synchronized fashion the SSI34 */
            ado_debug( DB_LVL_DRIVER, "Synchronized start of SSI 3,4" );
            ssiu_start( SSI_SYNC_SSI34_START );
        } else {
            /* start SSIx as independent SSI */
            ado_debug( DB_LVL_DRIVER, "Start SSI %d (CR.EN)",
                       rcar->playback.ssi_chan );
            ssi_start( rcar->playback.ssi_chan, SSI_INDEPENDENT_START );
        }

        /* start applicable busif if required */
        if( rcar->ssi_transfer_mode == SSI_BUSIF_TRANSFER ) {
            /* start busif SSIx-0 */
            ado_debug( DB_LVL_DRIVER, "Start BUSIF for SSI %d subchan 0",
                       rcar->playback.ssi_chan );
            ssiu_busif_start(rcar->playback.ssi_chan, 0);

            /* start busif SSIx-1,2,3 if in TDM split mode or ex-split mode */
            if( rcar->ssi_op_mode == SSI_OP_MODE_TDMSPLIT_4XMONO ||
                rcar->ssi_op_mode == SSI_OP_MODE_TDMSPLIT_4XSTEREO ||
                rcar->ssi_op_mode == SSI_OP_MODE_TDMEXSPLIT_8CHAN ||
                rcar->ssi_op_mode == SSI_OP_MODE_TDMEXSPLIT_16CHAN ) {
                ado_debug( DB_LVL_DRIVER, "Start BUSIF for SSI %d subchan 1,2,3",
                           rcar->playback.ssi_chan );
                ssiu_busif_start(rcar->playback.ssi_chan, 1);
                ssiu_busif_start(rcar->playback.ssi_chan, 2);
                ssiu_busif_start(rcar->playback.ssi_chan, 3);
            }

            /* start busif SSIx-4,5,6,7 if in TDM ex-split mode */
            if( rcar->ssi_op_mode == SSI_OP_MODE_TDMEXSPLIT_16CHAN ) {
                ado_debug( DB_LVL_DRIVER, "Start BUSIF for SSI %d subchan 4,5,6,7",
                           rcar->playback.ssi_chan );
                ssiu_busif_start(rcar->playback.ssi_chan, 4);
                ssiu_busif_start(rcar->playback.ssi_chan, 5);
                ssiu_busif_start(rcar->playback.ssi_chan, 6);
                ssiu_busif_start(rcar->playback.ssi_chan, 7);
            }
        }

        if( rcar->use_scu ) {
            /* start src */
            ado_debug( DB_LVL_DRIVER, "Start SRC %d",
                       rcar->playback.src_chan );
            scu_src_start(rcar->playback.src_chan);

            /* start cmd */
            ado_debug( DB_LVL_DRIVER, "Start CMD %d",
                       rcar->playback.cmd_chan );
            scu_cmd_start(rcar->playback.cmd_chan);
        }

        rcar->playback.active = 1;

        ado_debug (DB_LVL_DRIVER, "ADO_PCM_TRIGGER_START complete");

    } else if (cmd == ADO_PCM_TRIGGER_STOP) {
        ado_debug (DB_LVL_DRIVER, "ADO_PCM_TRIGGER_STOP");

        rcar->playback.active = 0;

        /* DMA request disable*/
        ado_debug( DB_LVL_DRIVER, "Stop Audio DMAC" );
        audio_dmac_stop(rcar->playback.dma_context.audiodma_chn);

        if (rcar->use_scu == 1) {

            /* Stop Peripheral-Peripheral DMAC */
            ado_debug( DB_LVL_DRIVER, "Stop Audio DMAC PP" );
            audio_dmac_pp_stop(rcar->playback.dma_context.audiodma_pp_chn);

            /* stop src0 */
            ado_debug( DB_LVL_DRIVER, "Stop SRC %d",
                       rcar->playback.src_chan );
            scu_src_stop(rcar->playback.src_chan);

            /* stop cmd0 */
            ado_debug( DB_LVL_DRIVER, "Stop CMD %d",
                       rcar->playback.cmd_chan );
            scu_cmd_stop(rcar->playback.cmd_chan);
        }

        /* stop applicable busif if required */
        if( rcar->ssi_transfer_mode == SSI_BUSIF_TRANSFER ) {
            /* stop busif SSIx-0 */
            ado_debug( DB_LVL_DRIVER, "Stop BUSIF for SSI %d subchan 0",
                       rcar->playback.ssi_chan );
            /* TODO: for TDM split and ex-split modes stop all BUSIF subchannels
               only when none are used anymore */
            ssiu_busif_stop(rcar->playback.ssi_chan, 0);

            /* stop busif SSIx-1,2,3 if in TDM split mode or ex-split mode */
            if( rcar->ssi_op_mode == SSI_OP_MODE_TDMSPLIT_4XMONO ||
                rcar->ssi_op_mode == SSI_OP_MODE_TDMSPLIT_4XSTEREO ||
                rcar->ssi_op_mode == SSI_OP_MODE_TDMEXSPLIT_8CHAN ||
                rcar->ssi_op_mode == SSI_OP_MODE_TDMEXSPLIT_16CHAN ) {
                ado_debug( DB_LVL_DRIVER, "Stop BUSIF for SSI %d subchan 1,2,3",
                           rcar->playback.ssi_chan );
                ssiu_busif_stop(rcar->playback.ssi_chan, 1);
                ssiu_busif_stop(rcar->playback.ssi_chan, 2);
                ssiu_busif_stop(rcar->playback.ssi_chan, 3);
            }
            /* stop busif SSIx-4,5,6,7 if in TDM ex-split mode */
            if( rcar->ssi_op_mode == SSI_OP_MODE_TDMEXSPLIT_16CHAN ) {
                ado_debug( DB_LVL_DRIVER, "Stop BUSIF for SSI %d subchan 4,5,6,7",
                           rcar->playback.ssi_chan );
                ssiu_busif_stop(rcar->playback.ssi_chan, 4);
                ssiu_busif_stop(rcar->playback.ssi_chan, 5);
                ssiu_busif_stop(rcar->playback.ssi_chan, 6);
                ssiu_busif_stop(rcar->playback.ssi_chan, 7);
            }
        }
        if( rcar->ssi_start_mode == SSI_SYNC_SSI0129_START ) {

            /* stop the individual SSIs */
            ado_debug( DB_LVL_DRIVER, "Stop SSI 0,1,2 (no CR.EN)" );
            ssi_stop( SSI_CHANNEL_0 );
            ssi_stop( SSI_CHANNEL_1 );
            ssi_stop( SSI_CHANNEL_2 );
            if( rcar->voices == 8 ) {
                ado_debug( DB_LVL_DRIVER, "Stop SSI 9 (no CR.EN)" );
                ssi_stop( SSI_CHANNEL_9 );
            }
            /* Multichannel SSI */
            ado_debug( DB_LVL_DRIVER, "Synchronized stop of SSI 0,1,2(,9)" );
            ssiu_stop( SSI_SYNC_SSI0129_START );
        } else if( rcar->ssi_start_mode == SSI_SYNC_SSI34_START ) {

            /* stop the individual SSIs */
            ado_debug( DB_LVL_DRIVER, "Stop SSI 3,4 (no CR.EN)" );
            ssi_stop( SSI_CHANNEL_3 );
            ssi_stop( SSI_CHANNEL_4 );

            /* SSI 3,4 configured for synchronized start */
            ado_debug( DB_LVL_DRIVER, "Synchronized stop of SSI 3,4" );
            ssiu_stop( SSI_SYNC_SSI34_START );
        } else {
            /* stop SSIx */
            ado_debug( DB_LVL_DRIVER, "Stop SSI %d (CR.EN)",
                       rcar->playback.ssi_chan );
            ssi_stop( rcar->playback.ssi_chan );
        }

        /* Wait for idle mode*/
        usleep(1000);
        ado_debug( DB_LVL_DRIVER, "Waiting for IDST on SSI %d",
                   rcar->playback.ssi_chan );
        idst_status = ssi_wait_status(rcar->playback.ssi_chan, SSISR_IDST_MASK);
        if (idst_status != EOK) {
            ado_error_fmt("SSI%d timed out waiting for IDST");
        }
        ssi_stop_idle_int(rcar->playback.ssi_chan);

        ado_debug (DB_LVL_DRIVER, "ADO_PCM_TRIGGER_STOP complete");
    }

    if( rcar->debug ) {
        rcar_register_dump( rcar );
    }

    return EOK;
}

static int32_t rcar_capture_trigger (HW_CONTEXT_T * rcar, PCM_SUBCHN_CONTEXT_T * pc, uint32_t cmd)
{
    ado_debug (DB_LVL_DRIVER, "TRIGGER - cmd: %d", cmd);

    if( pc->subchn != rcar->capture.subchn ) {
        ado_debug (DB_LVL_DRIVER, "unknown subchn");
        return EINVAL;
    }

    if (cmd == ADO_PCM_TRIGGER_GO) {
        ado_debug (DB_LVL_DRIVER, "ADO_PCM_TRIGGER_GO");

        /* Start Audio-DMAC */
        ado_debug( DB_LVL_DRIVER, "Start Audio DMAC" );
        audio_dmac_start(rcar->capture.dma_context.audiodma_chn);

        if ( rcar->use_scu ) {
            /* Start Peripheral-Peripheral DMAC */
            ado_debug( DB_LVL_DRIVER, "Start Audio DMAC PP" );
            audio_dmac_pp_start(rcar->capture.dma_context.audiodma_pp_chn);

            /* start SRC */
            ado_debug( DB_LVL_DRIVER, "Start SRC %d", rcar->capture.src_chan );
            scu_src_start(rcar->capture.src_chan);
        }

        /* start applicable busif if required */
        if( rcar->ssi_transfer_mode == SSI_BUSIF_TRANSFER &&
            rcar->ssi_op_mode != SSI_OP_MODE_TDMSPLIT_4XMONO &&
            rcar->ssi_op_mode != SSI_OP_MODE_TDMSPLIT_4XSTEREO ) {
            /* start busif SSIx-0 */
            ado_debug( DB_LVL_DRIVER, "Start BUSIF for SSI %d subchan 0",
                       rcar->capture.ssi_chan );
            ssiu_busif_start(rcar->capture.ssi_chan, 0);
        }

        if( rcar->ssi_start_mode == SSI_SYNC_SSI34_START ) {
            /* SSI 3,4 configured for synchronized start */

            /* start the individual SSIs */
            ado_debug( DB_LVL_DRIVER, "Start SSI 3,4 (no CR.EN)" );
            ssi_start( SSI_CHANNEL_3, SSI_SYNC_SSI34_START );
            ssi_start( SSI_CHANNEL_4, SSI_SYNC_SSI34_START );

            /* start in a synchronized fashion the SSI34 */
            ado_debug( DB_LVL_DRIVER, "Synchronized start of SSI 3,4" );
            ssiu_start( SSI_SYNC_SSI34_START );
        } else {
            /* start SSIx as independent SSI */
            ado_debug( DB_LVL_DRIVER, "Start SSI %d (CR.EN)", rcar->capture.ssi_chan );
            ssi_start( rcar->capture.ssi_chan, SSI_INDEPENDENT_START );
        }

        /* start busif SSIx-0,1,2,3 if in TDM split mode or TDM ex-split mode, after starting the SSI channel */
        if( rcar->ssi_op_mode == SSI_OP_MODE_TDMSPLIT_4XMONO ||
            rcar->ssi_op_mode == SSI_OP_MODE_TDMSPLIT_4XSTEREO ||
            rcar->ssi_op_mode == SSI_OP_MODE_TDMEXSPLIT_8CHAN ||
            rcar->ssi_op_mode == SSI_OP_MODE_TDMEXSPLIT_16CHAN ) {
            ado_debug( DB_LVL_DRIVER, "Start BUSIF for SSI %d subchan 0,1,2,3",
                       rcar->capture.ssi_chan );
            ssiu_busif_start(rcar->capture.ssi_chan, 0);
            ssiu_busif_start(rcar->capture.ssi_chan, 1);
            ssiu_busif_start(rcar->capture.ssi_chan, 2);
            ssiu_busif_start(rcar->capture.ssi_chan, 3);
        }
        /* start busif SSIx-4,5,6,7 if in TDM ex-split mode, after starting the SSI channel */
        if( rcar->ssi_op_mode == SSI_OP_MODE_TDMEXSPLIT_16CHAN ) {
            ado_debug( DB_LVL_DRIVER, "Start BUSIF for SSI %d subchan 4,5,6,7",
                       rcar->capture.ssi_chan );
            ssiu_busif_start(rcar->capture.ssi_chan, 4);
            ssiu_busif_start(rcar->capture.ssi_chan, 5);
            ssiu_busif_start(rcar->capture.ssi_chan, 6);
            ssiu_busif_start(rcar->capture.ssi_chan, 7);
        }

        rcar->capture.active = 1;

    } else if (cmd == ADO_PCM_TRIGGER_STOP) {
        ado_debug (DB_LVL_DRIVER, "ADO_PCM_TRIGGER_STOP");

        rcar->capture.active = 0;

        /* DMA request disable */
        ado_debug( DB_LVL_DRIVER, "Stop Audio DMAC" );
        audio_dmac_stop(rcar->capture.dma_context.audiodma_chn);

        if (rcar->use_scu) {
            /* stop pp dmac */
            ado_debug( DB_LVL_DRIVER, "Stop Audio DMAC PP" );
            audio_dmac_pp_stop(rcar->capture.dma_context.audiodma_pp_chn);

            /* stop src */
            ado_debug( DB_LVL_DRIVER, "Stop SRC %d", rcar->capture.src_chan );
            scu_src_stop(rcar->capture.src_chan);
        }

        /* stop applicable busif if required */
        if( rcar->ssi_transfer_mode == SSI_BUSIF_TRANSFER ) {
            /* stop busif SSIx-0 */
            ado_debug( DB_LVL_DRIVER, "Stop BUSIF for SSI %d subchan 0",
                       rcar->capture.ssi_chan );
            /* TODO: for TDM split and ex-split modes stop all BUSIF subchannels
               only when none are used anymore */
            ssiu_busif_stop(rcar->capture.ssi_chan, 0);

            /* stop busif SSIx-1,2,3 if in TDM split mode or ex-split mode */
            if( rcar->ssi_op_mode == SSI_OP_MODE_TDMSPLIT_4XMONO ||
                rcar->ssi_op_mode == SSI_OP_MODE_TDMSPLIT_4XSTEREO ||
                rcar->ssi_op_mode == SSI_OP_MODE_TDMEXSPLIT_8CHAN ||
                rcar->ssi_op_mode == SSI_OP_MODE_TDMEXSPLIT_16CHAN ) {
                ado_debug( DB_LVL_DRIVER, "Stop BUSIF for SSI %d subchan 1,2,3",
                           rcar->capture.ssi_chan );
                ssiu_busif_stop(rcar->playback.ssi_chan, 1);
                ssiu_busif_stop(rcar->playback.ssi_chan, 2);
                ssiu_busif_stop(rcar->playback.ssi_chan, 3);
            }
            /* stop busif SSIx-4,5,6,7 if in TDM ex-split mode */
            if( rcar->ssi_op_mode == SSI_OP_MODE_TDMEXSPLIT_16CHAN ) {
                ado_debug( DB_LVL_DRIVER, "Stop BUSIF for SSI %d subchan 4,5,6,7",
                           rcar->capture.ssi_chan );
                ssiu_busif_stop(rcar->playback.ssi_chan, 4);
                ssiu_busif_stop(rcar->playback.ssi_chan, 5);
                ssiu_busif_stop(rcar->playback.ssi_chan, 6);
                ssiu_busif_stop(rcar->playback.ssi_chan, 7);
            }
        }

        if( rcar->ssi_start_mode == SSI_SYNC_SSI34_START ) {
            /* SSI 3,4 configured for synchronized start */

            /* stop the individual SSIs */
            ado_debug( DB_LVL_DRIVER, "Stop SSI 3,4 (no CR.EN)" );
            ssi_stop( SSI_CHANNEL_3 );
            ssi_stop( SSI_CHANNEL_4 );

            /* stop SSI34 */
            ado_debug( DB_LVL_DRIVER, "Synchronized stop of SSI 3,4" );
            ssiu_stop( SSI_SYNC_SSI34_START );
        } else {
            /* stop SSIx */
            ado_debug( DB_LVL_DRIVER, "Stop SSI %d (CR.EN)", rcar->capture.ssi_chan );
            ssi_stop(rcar->capture.ssi_chan);
        }

        /* Wait for idle mode*/
        usleep(1000);
        ado_debug( DB_LVL_DRIVER, "Waiting for IDST on SSI %d",
                   rcar->capture.ssi_chan );
        ssi_wait_status(rcar->capture.ssi_chan, SSISR_IDST_MASK);
    }

    if( rcar->debug ) {
        rcar_register_dump( rcar );
    }

    return EOK;
}

static uint32_t rcar_position
(
    HW_CONTEXT_T * rcar,
    PCM_SUBCHN_CONTEXT_T * pc,
    ado_pcm_config_t * config,
    uint32_t * hw_buffer_level
)
{
    uint32_t pos = 0;

    if( hw_buffer_level ) {
        *hw_buffer_level = 0;
    }

    ado_mutex_lock (&rcar->hw_lock);

    audio_dmac_count_register_get(pc->dma_context.audiodma_chn, &pos);

    ado_mutex_unlock (&rcar->hw_lock);

    ado_debug (DB_LVL_DRIVER, "position=%x", pos);

    return (config->dmabuf.size - pos);
}

static void rcar_playback_interrupt (HW_CONTEXT_T * rcar, int32_t irq)
{
    ado_debug (DB_LVL_INTERRUPT, "irq=%d", irq);

    /* Clear Interrupt status */
    audio_dmac_cleanup(rcar->playback.dma_context.audiodma_chn);

    /* Signal to io-audio (DMA transfer was completed) */
    dma_interrupt(rcar->playback.subchn);
}

static void rcar_capture_interrupt (HW_CONTEXT_T * rcar, int32_t irq)
{
    ado_debug (DB_LVL_INTERRUPT, "irq=%d", irq);

    /* Clear Interrupt status */
    audio_dmac_cleanup(rcar->capture.dma_context.audiodma_chn);

    /* Signal to io-audio (DMA transfer was completed) */
    dma_interrupt(rcar->capture.subchn);
}

static void rcar_parse_version(char * str)
{
    if( strstr( str, "H2") || strstr( str, "h2") ){
        rcar_version_set( RCAR_VERSION_H2 );
    } else if ( strstr(str, "M2" ) || strstr( str, "m2") ){
        rcar_version_set( RCAR_VERSION_M2 );
    } else if ( strstr(str, "E2" ) || strstr( str, "e2") ){
        rcar_version_set( RCAR_VERSION_E2 );
    } else if ( strstr(str, "V2" ) || strstr( str, "v2") ){
        rcar_version_set( RCAR_VERSION_V2 );
    } else if ( strstr(str, "H3" ) || strstr( str, "h3") ){
        rcar_version_set( RCAR_VERSION_H3 );
    } else if ( strstr(str, "M3" ) || strstr( str, "m3") ){
        rcar_version_set( RCAR_VERSION_M3 );
    } else if ( strstr(str, "D3" ) || strstr( str, "d3") ){
        rcar_version_set( RCAR_VERSION_D3 );
    } else if ( strstr(str, "W2H" ) || strstr( str, "w2h") ){
        rcar_version_set( RCAR_VERSION_W2H );
    } else {
        ado_debug( DB_LVL_DRIVER, "version not set" );
    }
}

static int rcar_parse_commandline (rcar_context_t * rcar, char *args)
{
    int      ret = EOK;
    int      opt = 0;
    char     *value;
    int      numvalue = 0;
    uint32_t use_tx = 0;
    uint32_t use_rx = 0;
    uint32_t min_idx, max_idx;
    uint32_t sample_rate_max;
    char     cs_machine_str[CS_MACHINE_LEN];
    char     *opts[] = {
    #define RCAR_OPT_TX_SSI            0
        "tx_ssi",           // e.g. tx_ssi=0, tx_ssi=0129, enumerates the SSI indexes used for transmit
    #define RCAR_OPT_RX_SSI            1
        "rx_ssi",           // e.g. rx_ssi=1, specifies the SSI index used for receive
    #define RCAR_OPT_VOICES            2
        "voices",           // e.g. voices=8, voices=2, specifies number of voices used
    #define RCAR_OPT_CLK_MODE          3
        "clk_mode",         // clk_mode="master" indicates that one of the used SSIs indicated by tx_ssi and rx_ssi is
                            // configured as master, clk_mode="slave" indicates that all SSIs used by this driver
                            // instance are configured as slave
    #define RCAR_OPT_SYNC_START        4
        "sync_start",       // no params, indicates the use of synchronized start of SSI3,4
    #define RCAR_OPT_OP_MODE           5
        "op_mode",          // "op_mode" indicates a special TDM op mode, possible values are "tdm_ext",
                            // "tdm_split", "tdm_exsplit", while other op modes are inferred from the use
                            // of multiple SSI or from the number of voices
    #define RCAR_OPT_SCU               6
        "scu",              // scu=1 indicates that the SCU is used, scu=0 indicates that SCU is not used
    #define RCAR_OPT_MLP               7
        "mlp",              // mlp=1 indicates that the MLP is used, mlp=0 indicates that MLP is not used
    #define RCAR_OPT_DTCP              8
        "dtcp",             // dtcp=1 indicates that the DTCP is used, dtcp=0 indicates that DTCP is not used
    #define RCAR_OPT_SAMPLE_SIZE       9
        "sample_size",
    #define RCAR_OPT_CLK_POL          10
        "clk_pol",
    #define RCAR_OPT_FSYNC_POL        11
        "fsync_pol",
    #define RCAR_OPT_BIT_DELAY        12
        "bit_delay",
    #define RCAR_OPT_SAMPLE_RATE      13
        "sample_rate",
    #define RCAR_OPT_SAMPLE_RATE_LIST 14
        "sample_rate_list",
    #define RCAR_OPT_SLOT_SIZE        15
        "slot_size",
    #define RCAR_OPT_HDMI_CHAN        16
        "hdmi_chan",
    #define RCAR_OPT_VER              17
        "ver",
    #define RCAR_OPT_MCLK1            18
        "mclk_1",
    #define RCAR_OPT_MCLK2            19
        "mclk_2",
    #define RCAR_OPT_MCLK_FS          20
        "mclk_fs",
    #define RCAR_OPT_DEBUG            21
        "debug",            // no params, refers to the use of register dumps at the start and end of
                            // playback or capture sessions
        NULL
    };

    /* Note: sample driver from Renesas illustrates TDM split and ex-split modes by splitting the channels of a
       multi channel PCM device into separate DMA buffers and writing each of these DMA buffers to separate BUSIFs,
       only to re-join the data into the original multi channel format by using the TDM split or ex-split feature;
       while this can be used to test the feature, it is artificial and not at all useful, as the TDM mode could have
       been used in the first place, which is a lot more straightforward.
       TODO: implement support for TDM split and ex-split modes via multiple PCM devices whose data is assembled
       into one multi-channel stream by using this feature; e.g. use TDM ex split to implement two stereo and
       one 4-channel PCM device, whose data is joined into one 8 channel TDM stream */

    ado_debug (DB_LVL_DRIVER, "");

    rcar->ssi_masterslave_mode             = DEFAULT_SSI_MASTERSLAVE_MODE;  /* see variant.h for default */
    rcar->master_ssi_channel               = SSI_CHANNEL_NUM;
    rcar->ssi_start_mode                   = SSI_INDEPENDENT_START;
    rcar->ssi_transfer_mode                = SSI_INDEPENDENT_TRANSFER;
    rcar->ssi_op_mode                      = SSI_OP_MODE_NUM;               /* initialize to invalid value, to detect if specified in cmd line */
    rcar->use_scu                          = DEFAULT_USE_SCU;               /* see variant.h for default */
    rcar->use_mlp                          = 0;                             /* by default, disable mlp */
    rcar->use_dtcp                         = 0;                             /* by default, disable dtcp */
    rcar->sample_rate_min                  = SAMPLE_RATE_MIN;
    rcar->sample_rate_max                  = SAMPLE_RATE_MAX;
    rcar->sample_rate                      = 0;                             /* 0 if no channel acquired, actual rate when a channel is acquired */
    rcar->slot_size                        = DEFAULT_SLOT_SIZE;             /* see variant.h for default */
    rcar->sample_size                      = DEFAULT_SAMPLE_SIZE;           /* see variant.h for default  */
    rcar->voices                           = 0;                             /* initialize to 0, to detect if specified in cmd line */
    rcar->playback.ssi_chan                = DEFAULT_SSI_CHANNEL_PLAYBACK;  /* see variant.h for default */
    rcar->capture.ssi_chan                 = DEFAULT_SSI_CHANNEL_CAPTURE;   /* see variant.h for default */
    rcar->ssi_config.clk_pol               = SSI_BIT_CLK_POL_RISING;
    rcar->ssi_config.ws_pol                = SSI_WS_POL_0;
    rcar->ssi_config.bit_delay             = SSI_BIT_DELAY_ONE;
    rcar->ssi_config.padding_pol           = SSI_PADDING_POL_LOW;
    rcar->ssi_config.serial_data_alignment = SSI_SER_DATA_ALIGN_DATA_FIRST;
    rcar->ssi_config.sys_word_length       = SSI_SYS_WORD_LEN_16BIT_STEREO;
    rcar->ssi_config.data_word_length      = SSI_DATA_WORD_LEN_16BIT;
    rcar->playback.hdmi_chan               = SSI_HDMI_CHANNEL_NUM;
    rcar->mclk_1                           = MCLK_1;
    rcar->mclk_2                           = MCLK_2;
    rcar->mclk_fs                          = MCLK_FS;
    rcar->debug                            = 0;                             /* by default, no register dumps */

    /* Detect R-Car version based on confstr */
    if( confstr(_CS_MACHINE, cs_machine_str, CS_MACHINE_LEN) > 0 ) {
        ado_debug( DB_LVL_DRIVER, "version detected=%s", cs_machine_str );

        rcar_parse_version(cs_machine_str);
    }

    /* Sets options to each values */
    while (args != NULL && args[0] != 0) {
        opt = getsubopt (&args, opts, &value);

        /* Handle in a single place the case where an option that would need a value
           to be specified is missing the value */
        switch ( opt ) {
            case RCAR_OPT_TX_SSI:
            case RCAR_OPT_RX_SSI:
            case RCAR_OPT_VOICES:
            case RCAR_OPT_CLK_MODE:
            case RCAR_OPT_OP_MODE:
            case RCAR_OPT_SCU:
            case RCAR_OPT_MLP:
            case RCAR_OPT_DTCP:
            case RCAR_OPT_SAMPLE_SIZE:
            case RCAR_OPT_CLK_POL:
            case RCAR_OPT_FSYNC_POL:
            case RCAR_OPT_BIT_DELAY:
            case RCAR_OPT_SAMPLE_RATE:
            case RCAR_OPT_SAMPLE_RATE_LIST:
            case RCAR_OPT_SLOT_SIZE:
            case RCAR_OPT_HDMI_CHAN:
            case RCAR_OPT_VER:
            case RCAR_OPT_MCLK1:
            case RCAR_OPT_MCLK2:
            case RCAR_OPT_MCLK_FS:
                if( value == NULL ) {
                    ado_error_fmt( "Option %s requires a value", opts[opt] );
                    return EINVAL;
                }
                break;
            default:
                break;
        }
        switch (opt) {
            case RCAR_OPT_TX_SSI:
                if ( !strcmp(value, "0129") ) {
                    if ( rcar->ssi_op_mode != SSI_OP_MODE_NUM &&
                        rcar->ssi_op_mode != SSI_OP_MODE_MULTICH ) {
                        ado_error_fmt( "Multi SSI config not supported in op mode %d", rcar->ssi_op_mode );
                        return EINVAL;
                    }
                    if ( !rcar_ssi_supported( 0 ) || !rcar_ssi_supported( 1 ) ||
                        !rcar_ssi_supported( 2 ) || !rcar_ssi_supported( 9 ) ) {
                        ado_error_fmt( "Some of SSI 0,1,2,9 not supported" );
                        return EINVAL;
                    }
                    rcar->ssi_op_mode = SSI_OP_MODE_MULTICH;
                    rcar->voices = 8;
                    // set playback ssi_chan as 0, the use of multiple channels is specified by the op mode
                    // and playback voices
                    rcar->playback.ssi_chan = SSI_CHANNEL_0;
                    ado_debug( DB_LVL_DRIVER, "tx uses multiple SSI 0129" );
                } else if ( !strcmp(value, "012") ) {
                    if ( rcar->ssi_op_mode != SSI_OP_MODE_NUM &&
                        rcar->ssi_op_mode != SSI_OP_MODE_MULTICH ) {
                        ado_error_fmt( "Multi SSI config not supported in op mode %d", rcar->ssi_op_mode );
                        return EINVAL;
                    }
                    if ( !rcar_ssi_supported( 0 ) || !rcar_ssi_supported( 1 ) ||
                        !rcar_ssi_supported( 2 ) ) {
                        ado_error_fmt( "Some of SSI 0,1,2 not supported" );
                        return EINVAL;
                    }
                    rcar->ssi_op_mode = SSI_OP_MODE_MULTICH;
                    rcar->voices = 6;
                    // set playback ssi_chan as 0, the use of multiple channels is specified by the op mode
                    rcar->playback.ssi_chan = SSI_CHANNEL_0;
                    ado_debug( DB_LVL_DRIVER, "tx uses multiple SSI 0,1,2" );
                } else {
                    numvalue = strtol( value, NULL, 0 );
                    if ( rcar_ssi_supported( numvalue ) ) {
                        rcar->playback.ssi_chan = numvalue;
                        ado_debug( DB_LVL_DRIVER, "tx uses SSI %d", rcar->playback.ssi_chan );
                    } else {
                        ado_error_fmt( "SSI %d not supported", numvalue );
                        return EINVAL;
                    }
                }
                break;
            case RCAR_OPT_RX_SSI:
                numvalue = strtol( value, NULL, 0 );
                if ( rcar_ssi_supported( numvalue ) ) {
                    rcar->capture.ssi_chan = numvalue;
                    ado_debug( DB_LVL_DRIVER, "rx uses SSI %d", rcar->capture.ssi_chan );
                } else {
                    ado_error_fmt( "SSI %d not supported", numvalue );
                    return EINVAL;
                }
                break;
            case RCAR_OPT_VOICES:
                if ( rcar->voices == 0 ) {
                    numvalue = strtol( value, NULL, 0 );
                    if ( numvalue == 1 || numvalue == 2 || numvalue == 4 ||
                         numvalue == 6 || numvalue == 8 || numvalue == 16) {
                        rcar->voices = numvalue;
                    } else {
                        ado_error_fmt( "Invalid voices %d", numvalue );
                        return EINVAL;
                    }
                    ado_debug( DB_LVL_DRIVER, "voices %d", rcar->voices );
                }
                break;
            case RCAR_OPT_CLK_MODE:
                if ( strcmp( value, "master") == 0 ) {
                    rcar->ssi_masterslave_mode = SSI_MASTER_SLAVE;
                } else if ( strcmp( value, "slave") == 0 ) {
                    rcar->ssi_masterslave_mode = SSI_ALL_SLAVE;
                } else {
                    ado_error_fmt( "Invalid clk_mode %s", value );
                    return EINVAL;
                }
                ado_debug( DB_LVL_DRIVER, "clk_mode %s: %d", value, rcar->ssi_masterslave_mode );
                break;
            case RCAR_OPT_SYNC_START:
                rcar->ssi_start_mode = SSI_SYNC_SSI34_START;
                break;
            case RCAR_OPT_OP_MODE:
                if ( rcar->ssi_op_mode == SSI_OP_MODE_MULTICH ) {
                    ado_error_fmt( "Special TDM modes not supported for multi SSI config" );
                    return EINVAL;
                }
                if ( strcmp( value, "tdm_ext") == 0 ) {
                    rcar->ssi_op_mode = SSI_OP_MODE_TDMEXT;
                } else if ( strcmp( value, "tdm_split") == 0 ) {
                    rcar->ssi_op_mode = SSI_OP_MODE_TDMSPLIT;
                } else if ( strcmp( value, "tdm_exsplit") == 0 ) {
                    rcar->ssi_op_mode = SSI_OP_MODE_TDMEXSPLIT;
                } else {
                    ado_error_fmt( "Invalid op_mode %s", value );
                    return EINVAL;
                }
                ado_debug( DB_LVL_DRIVER, "op_mode %s: %d", value, rcar->ssi_op_mode );
                break;
            case RCAR_OPT_SCU:
                if ( rcar_src_get_supported_range(&min_idx, &max_idx) == EOK ) {
                    rcar->use_scu = strtol( value, NULL, 0 );
                } else {
                    ado_error_fmt( "SCU not supported" );
                    return EINVAL;
                }
                break;
            case RCAR_OPT_MLP:
                if ( rcar_mlm_get_supported_range(&min_idx, &max_idx) ) {
                    rcar->use_mlp = strtol( value, NULL, 0 );
                } else {
                    ado_error_fmt( "MLP/MLM not supported" );
                    return EINVAL;
                }
                break;
            case RCAR_OPT_DTCP:
                if ( rcar_dtcp_get_supported_range(&min_idx, &max_idx) ) {
                    rcar->use_dtcp = strtol( value, NULL, 0 );
                } else {
                    ado_error_fmt( "DTCP not supported" );
                    return EINVAL;
                }
                break;
            case RCAR_OPT_SAMPLE_SIZE:
                numvalue = strtol( value, NULL, 0 );
                if ( numvalue == 16 || numvalue == 24 || numvalue == 32 ) {
                    rcar->sample_size = numvalue;
                } else {
                    ado_error_fmt( "Invalid sample size: %d", numvalue );
                    return EINVAL;
                }
                break;
            case RCAR_OPT_CLK_POL:
                numvalue = strtol( value, NULL, 0 );

                if ( numvalue == 0 ) {
                    rcar->ssi_config.clk_pol = SSI_BIT_CLK_POL_RISING;
                } else if( numvalue == 1 ) {
                    rcar->ssi_config.clk_pol = SSI_BIT_CLK_POL_FALLING;
                } else {
                    ado_error_fmt( "Invalid clk pol: %d", numvalue );
                    return EINVAL;
                }
                ado_debug( DB_LVL_DRIVER, "Set clock polarity %s",
                           numvalue ? "FALLING" : "RISING" );
                break;
            case RCAR_OPT_FSYNC_POL:
                numvalue = strtol( value, NULL, 0 );

                if ( numvalue == 0 ) {
                    rcar->ssi_config.ws_pol = SSI_WS_POL_0;
                } else if ( numvalue == 1 ) {
                    rcar->ssi_config.ws_pol = SSI_WS_POL_1;
                } else {
                    ado_error_fmt( "Invalid ws pol: %d", numvalue );
                    return EINVAL;
                }
                ado_debug( DB_LVL_DRIVER, "Set WS polarity %d", rcar->ssi_config.ws_pol );
                break;
            case RCAR_OPT_BIT_DELAY:
                numvalue = strtol( value, NULL, 0 );

                if ( numvalue == 0 ) {
                    rcar->ssi_config.bit_delay = SSI_BIT_DELAY_NONE;
                } else if ( numvalue == 1 ) {
                    rcar->ssi_config.bit_delay = SSI_BIT_DELAY_ONE;
                } else {
                    ado_error_fmt( "Invalid bit delay: %d", numvalue );
                    return EINVAL;
                }
                ado_debug (DB_LVL_DRIVER, "Set bit delay %s",
                           numvalue ? "ONE" : "NONE");
                break;
            case RCAR_OPT_SAMPLE_RATE: // min and max sample rate, separated by ':'
            {
                uint32_t rates[2];
                uint32_t n = 0;
                uint32_t rate_flag_min;
                uint32_t rate_flag_max;
                uint32_t rate_flag;

                while (value && n < 2) {
                    if (n > 0) value++;  // skip over separator
                    rates[n++] = strtoul(value, &value, 0);
                    value = strchr(value, ':');  // find next separator
                }

                if ( n == 0 ) {
                    ado_error_fmt( "No valid rate in sample_rate: %s", value );
                    return EINVAL;
                }

                if ( ( rate_flag_min = ado_pcm_rate2flag(rates[0]) ) == 0 ) {
                    ado_error_fmt( "Invalid min_rate in sample_rate: %d", rates[0] );
                    return EINVAL;
                }

                if ( ( rate_flag_min & supported_rate_list ) == 0 ) {
                    ado_error_fmt( "Unsupported min_rate in sample_rate: %d", rates[0] );
                    return EINVAL;
                }

                if ( rates[1] < rates[0] ||
                     ( rate_flag_max = ado_pcm_rate2flag(rates[1]) ) == 0 ) {
                    ado_error_fmt( "Invalid max_rate in sample_rate: %d", rates[1] );
                    return EINVAL;
                }

                if ( ( rate_flag_max & supported_rate_list ) == 0 ) {
                    ado_error_fmt( "Unsupported max_rate in sample_rate: %d", rates[0] );
                    return EINVAL;
                }

                rcar->sample_rate_min = rates[0];
                rcar->sample_rate_max = (n > 1) ? rates[1] : rcar->sample_rate_min;

                configured_rate_list = rate_flag_min;
                for ( rate_flag = rate_flag_min; rate_flag <= rate_flag_max; rate_flag <<= 1 ) {
                    if ( rate_flag & supported_rate_list ) {
                        configured_rate_list |= rate_flag;
                    }
                }

                ado_debug( DB_LVL_DRIVER, "sample rate min %d, max %d, rate_list %x",
                           rcar->sample_rate_min, rcar->sample_rate_max, configured_rate_list );
                break;
            }
            case RCAR_OPT_SAMPLE_RATE_LIST: // all supported sample rates, separated by ':'
            {
                uint32_t rates[RCAR_NUM_SUPPORTED_RATES];
                uint32_t n = 0, i = 0;
                uint32_t rate_flag;

                while (value && n < sizeof(rates)/sizeof(uint32_t)) {
                    if (n > 0) value++;  // skip over separator
                    rates[n++] = strtoul(value, &value, 0);
                    value = strchr(value, ':');  // find next separator
                }

                if ( n == 0 ) {
                    ado_error_fmt( "No valid rate in sample_rate_list: %s", value );
                    return EINVAL;
                }

                rcar->sample_rate_min = rates[0];
                rcar->sample_rate_max = rates[0];

                for ( i = 0; i < n; i++ ) {
                    if ( ( rate_flag = ado_pcm_rate2flag(rates[i]) ) == 0 ) {
                        ado_error_fmt( "Invalid rate in sample_rate_list: %d", rates[i] );
                        return EINVAL;
                    }
                    if ( ( rate_flag & supported_rate_list ) == 0 ) {
                        ado_error_fmt( "Unsupported rate in sample_rate_list: %d", rates[i] );
                        return EINVAL;
                    }
                    configured_rate_list |= rate_flag;
                    if ( rates[i] < rcar->sample_rate_min ) {
                        rcar->sample_rate_min = rates[i];
                    } else if( rates[i] > rcar->sample_rate_max ) {
                        rcar->sample_rate_max = rates[i];
                    }
                }

                ado_debug( DB_LVL_DRIVER, "sample rate min %d, max %d, rate_list %x",
                           rcar->sample_rate_min, rcar->sample_rate_max, configured_rate_list );
                break;
            }
            case RCAR_OPT_SLOT_SIZE:
                numvalue = strtol( value, NULL, 0 );
                if ( numvalue == 16 || numvalue == 32 ) {
                    rcar->slot_size = numvalue;
                } else {
                    ado_error_fmt( "Invalid slot_size %d", numvalue );
                    return EINVAL;
                }
                ado_debug( DB_LVL_DRIVER, "slot_size %d", rcar->slot_size );
                break;
            case RCAR_OPT_HDMI_CHAN:
                numvalue = strtol( value, NULL, 0 );
                if ( rcar_hdmi_supported( numvalue ) ) {
                    rcar->playback.hdmi_chan = numvalue;
                } else {
                    ado_error_fmt( "Invalid hdmi_chan %d", numvalue );
                    return EINVAL;
                }
                ado_debug( DB_LVL_DRIVER, "hdmi_chan %d", rcar->playback.hdmi_chan );
                break;
            case RCAR_OPT_VER:
                rcar_parse_version(value);
                ado_debug( DB_LVL_DRIVER, "ver %s", value );
                break;
           case RCAR_OPT_MCLK1:
                numvalue = strtol( value, NULL, 0 );
                rcar->mclk_1 = numvalue;
                ado_debug( DB_LVL_DRIVER, "mclk_1 %d", rcar->mclk_1 );
                break;
           case RCAR_OPT_MCLK2:
                numvalue = strtol( value, NULL, 0 );
                rcar->mclk_2 = numvalue;
                ado_debug( DB_LVL_DRIVER, "mclk_2 %d", rcar->mclk_2 );
                break;
            case RCAR_OPT_MCLK_FS:
                numvalue = strtol( value, NULL, 0 );
                rcar->mclk_fs = numvalue;
                ado_debug( DB_LVL_DRIVER, "mclk_fs %d", rcar->mclk_fs );
                break;
            case RCAR_OPT_DEBUG:
                rcar->debug = 1;
                ado_debug( DB_LVL_DRIVER, "debug mode is on" );
        }
    }

    use_tx = ( rcar->playback.ssi_chan != SSI_CHANNEL_NUM ? 1 : 0 );
    use_rx = ( rcar->capture.ssi_chan != SSI_CHANNEL_NUM ? 1 : 0 );

    /* when adding MLP support change the below validation to check also for MLP use */
    /* when using MLP both transmit and receive could use the MLP port */
    if ( !use_tx && !use_rx ) {
        ado_error_fmt( "No SSI specified for either transmit or receive" );
        return EINVAL;
    }

    ret = EOK;

    /* the only reason to configure a transmit and receive SSI in the same driver
       instance is pin sharing between the SSIs used for receive and transmit;
       pin sharing occurs for SSI pairs {0,1}, {0,2}, {0,3}, {0,9}, {3,9}, {3,4}, {7,8} */
    if( use_tx && use_rx ) {
        switch( rcar->playback.ssi_chan ) {
            case SSI_CHANNEL_0:
                if ( rcar->capture.ssi_chan != SSI_CHANNEL_1 &&
                     rcar->capture.ssi_chan != SSI_CHANNEL_2 &&
                     rcar->capture.ssi_chan != SSI_CHANNEL_3 &&
                     rcar->capture.ssi_chan != SSI_CHANNEL_9 ) {
                    ret = EINVAL;
                }
                if ( rcar->ssi_masterslave_mode == SSI_MASTER_SLAVE ) {
                    rcar->master_ssi_channel = SSI_CHANNEL_0;
                }
                break;
            case SSI_CHANNEL_1:
            case SSI_CHANNEL_2:
                if ( rcar->capture.ssi_chan != SSI_CHANNEL_0 ) {
                    ret = EINVAL;
                }
                if ( rcar->ssi_masterslave_mode == SSI_MASTER_SLAVE ) {
                    rcar->master_ssi_channel = SSI_CHANNEL_0;
                }
                break;
            case SSI_CHANNEL_3:
                if ( rcar->capture.ssi_chan != SSI_CHANNEL_0 &&
                     rcar->capture.ssi_chan != SSI_CHANNEL_4 &&
                     rcar->capture.ssi_chan != SSI_CHANNEL_9 ) {
                    ret = EINVAL;
                }
                if ( rcar->ssi_masterslave_mode == SSI_MASTER_SLAVE ) {
                    if ( rcar->capture.ssi_chan == SSI_CHANNEL_0 ) {
                        rcar->master_ssi_channel = SSI_CHANNEL_0;
                    } else {
                        rcar->master_ssi_channel = SSI_CHANNEL_3;
                    }
                }
                break;
            case SSI_CHANNEL_4:
                if ( rcar->capture.ssi_chan != SSI_CHANNEL_3 ) {
                    ret = EINVAL;
                }
                if ( rcar->ssi_masterslave_mode == SSI_MASTER_SLAVE ) {
                    rcar->master_ssi_channel = SSI_CHANNEL_3;
                }
                break;
            case SSI_CHANNEL_9:
                if ( rcar->capture.ssi_chan != SSI_CHANNEL_0 &&
                    rcar->capture.ssi_chan != SSI_CHANNEL_3 ) {
                    ret = EINVAL;
                }
                if ( rcar->ssi_masterslave_mode == SSI_MASTER_SLAVE ) {
                    if ( rcar->capture.ssi_chan == SSI_CHANNEL_0 ) {
                        rcar->master_ssi_channel = SSI_CHANNEL_0;
                    } else {
                        rcar->master_ssi_channel = SSI_CHANNEL_3;
                    }
                }
                break;
            case SSI_CHANNEL_7:
                if ( rcar->capture.ssi_chan != SSI_CHANNEL_8 ) {
                    ret = EINVAL;
                }
                if ( rcar->ssi_masterslave_mode == SSI_MASTER_SLAVE ) {
                    rcar->master_ssi_channel = SSI_CHANNEL_7;
                }
                break;
            case SSI_CHANNEL_8:
                if ( rcar->capture.ssi_chan != SSI_CHANNEL_7 ) {
                    ret = EINVAL;
                }
                if ( rcar->ssi_masterslave_mode == SSI_MASTER_SLAVE ) {
                    rcar->master_ssi_channel = SSI_CHANNEL_7;
                }
                break;
        }
        if( ret == EINVAL ) {
            ado_error_fmt( "SSI %d and %d can not be configured in pin sharing mode",
                           rcar->playback.ssi_chan, rcar->capture.ssi_chan );
            return ret;
        }
    } else if ( use_tx && rcar->ssi_masterslave_mode == SSI_MASTER_SLAVE ) {
        rcar->master_ssi_channel = rcar->playback.ssi_chan;
    } else if ( use_rx && rcar->ssi_masterslave_mode == SSI_MASTER_SLAVE ) {
        rcar->master_ssi_channel = rcar->capture.ssi_chan;
    }

    /* Refine TDM modes and validate against number of voices if specified */
    if ( rcar->ssi_op_mode == SSI_OP_MODE_TDMEXSPLIT && rcar->voices != 0 ) {
        if ( rcar->voices == 8 ) {
            rcar->ssi_op_mode = SSI_OP_MODE_TDMEXSPLIT_8CHAN;
        } else if ( rcar->voices == 16 ) {
            rcar->ssi_op_mode = SSI_OP_MODE_TDMEXSPLIT_16CHAN;
        } else {
            ado_error_fmt( "%d voices not supported in TDM exsplit mode", rcar->voices );
            return EINVAL;
        }
    } else if ( rcar->ssi_op_mode == SSI_OP_MODE_TDMSPLIT && rcar->voices != 0 ) {
        if ( rcar->voices == 4 ) {
            rcar->ssi_op_mode = SSI_OP_MODE_TDMSPLIT_4XMONO;
        } else if ( rcar->voices == 8 ) {
            rcar->ssi_op_mode = SSI_OP_MODE_TDMSPLIT_4XSTEREO;
        } else {
            ado_error_fmt( "%d voices not supported in TDM split mode", rcar->voices );
            return EINVAL;
        }
    } else if ( rcar->ssi_op_mode == SSI_OP_MODE_TDMEXT && rcar->voices != 0 ) {
        if( rcar->voices != 6 && rcar->voices != 8 ) {
            ado_error_fmt( "%d voices not supported in TDM ext mode", rcar->voices );
            return EINVAL;
        }
    }

    /* if voices not inferred from cmd line params, use variant.h default value,
       except for the case that the op_mode was specified or inferred from cmd line params,
       and it is different from the variant.h default op_mode */
    if ( rcar->voices == 0 ) {
        if ( rcar->ssi_op_mode != SSI_OP_MODE_NUM &&
             DEFAULT_SSI_OP_MODE != rcar->ssi_op_mode ) {
            ado_error_fmt( "Number of voices needs to be specified op_mode %d", rcar->ssi_op_mode );
            return EINVAL;
        } else {
            rcar->voices = DEFAULT_VOICES;
        }
    }

    /* if ssi_op_mode not specified or inferred from cmd line params, use variant.h default value */
    if ( rcar->ssi_op_mode == SSI_OP_MODE_NUM ) {
        if (rcar->voices == DEFAULT_VOICES) {
            rcar->ssi_op_mode = DEFAULT_SSI_OP_MODE;
        } else if (rcar->voices == 1) {
            rcar->ssi_op_mode = SSI_OP_MODE_MONO;
        } else if (rcar->voices == 2) {
            rcar->ssi_op_mode = SSI_OP_MODE_STEREO;
        } else if (rcar->voices == 6 || rcar->voices == 8) {
            rcar->ssi_op_mode = SSI_OP_MODE_TDM;
        } else if (rcar->voices == 16) {
            rcar->ssi_op_mode = SSI_OP_MODE_TDM_16CHAN;
        } else {
            ado_error_fmt( "op_mode can not be determined for voices %d", rcar->voices );
            return EINVAL;
        }
    }

    /* If voices and op_mode were specified or inferred from command line params, they should be fine.
       However, if voices and/or op_mode use variant.h default values, we need to validate
       that voices and op_mode are consistent */
    if ( ( ( rcar->ssi_op_mode == SSI_OP_MODE_MULTICH || rcar->ssi_op_mode == SSI_OP_MODE_TDMEXT ||
             rcar->ssi_op_mode == SSI_OP_MODE_TDM ) && ( rcar->voices != 6 && rcar->voices != 8 ) ) ||
         ( ( rcar->ssi_op_mode == SSI_OP_MODE_TDMSPLIT_4XMONO ) && ( rcar->voices != 4 ) ) ||
         ( ( rcar->ssi_op_mode == SSI_OP_MODE_TDMSPLIT_4XSTEREO ||
             rcar->ssi_op_mode == SSI_OP_MODE_TDMEXSPLIT_8CHAN ) && ( rcar->voices != 8 ) ) ||
         ( ( rcar->ssi_op_mode == SSI_OP_MODE_TDMEXSPLIT_16CHAN ||
             rcar->ssi_op_mode == SSI_OP_MODE_TDM_16CHAN ) && ( rcar->voices != 16 ) ) ||
         ( ( rcar->ssi_op_mode == SSI_OP_MODE_STEREO ) && ( rcar->voices != 2 ) ) ||
         ( ( rcar->ssi_op_mode == SSI_OP_MODE_MONO ) && ( rcar->voices != 1 ) ) ) {
        ado_error_fmt( "op_mode %d and voices %d are inconsistent", rcar->ssi_op_mode, rcar->voices );
        return EINVAL;
    }

    /* in some cases the voices configured on the SSI differs from the client voices */
    if ( rcar->ssi_op_mode == SSI_OP_MODE_MULTICH ) {
        rcar->ssi_voices = 2; // 2 voices on each SSI
    } else if ( rcar->ssi_op_mode == SSI_OP_MODE_TDMEXT ) {
        rcar->ssi_voices = ( rcar->voices == 6 ? 8 : 6 );
    } else {
        rcar->ssi_voices = rcar->voices;
    }

    /* determine the SSI transfer mode */
    if ( rcar->use_scu || rcar->use_mlp || rcar->use_dtcp ||
        rcar->ssi_op_mode == SSI_OP_MODE_MULTICH ||
        rcar->ssi_op_mode == SSI_OP_MODE_TDMEXT ||
        rcar->ssi_op_mode == SSI_OP_MODE_TDMSPLIT_4XMONO ||
        rcar->ssi_op_mode == SSI_OP_MODE_TDMSPLIT_4XSTEREO ||
        rcar->ssi_op_mode == SSI_OP_MODE_TDMEXSPLIT_8CHAN ||
        rcar->ssi_op_mode == SSI_OP_MODE_TDMEXSPLIT_16CHAN )
    {
        rcar->ssi_transfer_mode = SSI_BUSIF_TRANSFER;
    } else {
        rcar->ssi_transfer_mode = SSI_INDEPENDENT_TRANSFER;
    }

    /* set the SSI start mode as synchronized SSI 0129 start if the op mode
       is SSI_OP_MODE_MULTICH */
    if ( rcar->ssi_op_mode == SSI_OP_MODE_MULTICH ) {
        rcar->ssi_start_mode = SSI_SYNC_SSI0129_START;
    }

    if ( rcar->ssi_op_mode == SSI_OP_MODE_MULTICH && use_rx ) {
        ado_error_fmt( "SSI 012(9) and %d can not be configured in the same driver instance",
                       rcar->capture.ssi_chan);
        return EINVAL;
    }

    /* check if the requested SSI channels support the calculated op mode */
    if ( rcar->ssi_op_mode == SSI_OP_MODE_TDMEXT ) {
        if ( use_tx && !rcar_ssi_tdmext_supported( rcar->playback.ssi_chan ) ) {
            ado_error_fmt("SSI %d does not support the TDM EXT mode", rcar->playback.ssi_chan);
            return EINVAL;
        }
        if ( use_rx && !rcar_ssi_tdmext_supported( rcar->capture.ssi_chan ) ) {
            ado_error_fmt("SSI %d does not support the TDM EXT mode", rcar->capture.ssi_chan);
            return EINVAL;
        }
    } else if ( rcar->ssi_op_mode == SSI_OP_MODE_TDMSPLIT_4XMONO ||
                rcar->ssi_op_mode == SSI_OP_MODE_TDMSPLIT_4XSTEREO ) {
        if ( use_tx && !rcar_ssi_tdmsplit_supported( rcar->playback.ssi_chan ) ) {
            ado_error_fmt("SSI %d does not support the TDM SPLIT mode", rcar->playback.ssi_chan);
            return EINVAL;
        }
        if ( use_rx && !rcar_ssi_tdmsplit_supported( rcar->capture.ssi_chan ) ) {
            ado_error_fmt("SSI %d does not support the TDM SPLIT mode", rcar->capture.ssi_chan);
            return EINVAL;
        }
    } else if ( rcar->ssi_op_mode == SSI_OP_MODE_TDMEXSPLIT_8CHAN ||
                rcar->ssi_op_mode == SSI_OP_MODE_TDMEXSPLIT_16CHAN ) {
        if ( use_tx && !rcar_ssi_tdmexsplit_supported( rcar->playback.ssi_chan ) ) {
            ado_error_fmt("SSI %d does not support the TDM EXSPLIT mode", rcar->playback.ssi_chan);
            return EINVAL;
        }
        if ( use_rx && !rcar_ssi_tdmexsplit_supported( rcar->capture.ssi_chan ) ) {
            ado_error_fmt("SSI %d does not support the TDM EXSPLIT mode", rcar->capture.ssi_chan);
            return EINVAL;
        }
    }

    /* check if the sample_size is supported for the calculated op mode */
    if ( rcar->ssi_op_mode == SSI_OP_MODE_MULTICH ||
         rcar->ssi_op_mode == SSI_OP_MODE_TDMEXT ||
         rcar->ssi_op_mode == SSI_OP_MODE_TDMSPLIT_4XMONO ||
         rcar->ssi_op_mode == SSI_OP_MODE_TDMSPLIT_4XSTEREO ||
         rcar->ssi_op_mode == SSI_OP_MODE_TDMEXSPLIT_8CHAN ) {
        if ( rcar->sample_size != 16 && rcar->sample_size != 24 ) {
            ado_error_fmt("Sample size %d is not supported with op mode %d", rcar->sample_size, rcar->ssi_op_mode);
            return EINVAL;
        }
    } else if ( rcar->ssi_op_mode == SSI_OP_MODE_TDMEXSPLIT_16CHAN ||
                rcar->ssi_op_mode == SSI_OP_MODE_TDM_16CHAN ) {
        if ( rcar->sample_size != 16 ) {
            ado_error_fmt("Sample size %d is not supported with op mode %d", rcar->sample_size, rcar->ssi_op_mode);
            return EINVAL;
        }
    }

    /* check if the sample_size is less equal the slot size */
    if ( rcar->sample_size > rcar->slot_size ) {
        ado_error_fmt("Sample size %d is greater than the slot size %d", rcar->sample_size, rcar->slot_size);
        return EINVAL;
    }

    /* check that the sample size is at most 24 bits is the SCU is used */
    if ( rcar->sample_size > 24 && rcar->use_scu ) {
        ado_error_fmt("Sample size %d is not supported by SCU", rcar->sample_size);
        return EINVAL;
    }

    /* if an hdmi_chan is specified, validate that op_mode is either
       stereo or multi channel if a valid HDMI channel is specified */
    if ( rcar->playback.hdmi_chan != SSI_HDMI_CHANNEL_NUM &&
         rcar->ssi_op_mode != SSI_OP_MODE_MULTICH &&
         rcar->ssi_op_mode != SSI_OP_MODE_STEREO ) {
        ado_error_fmt("HDMI interface is not supported in op_mode %d", rcar->ssi_op_mode);
        return EINVAL;
    }

    /* validate that the sample_rate_max or SAMPLE_RATE_SRC, voices and slot_size combination
       can be supported by one of the mclk_1 and mclk_2 specified in the variant.h file */
    sample_rate_max = ( rcar->use_scu ? SAMPLE_RATE_SRC : rcar->sample_rate_max );

    if ( rcar->mclk_1 != 0 &&
         rcar->mclk_1 / sample_rate_max * sample_rate_max == rcar->mclk_1 &&
         sample_rate_max * rcar->ssi_voices * rcar->slot_size > rcar->mclk_1 ) {
        ado_error_fmt("Max sample rate %d, slot_size %d and voices %d not supported by MCLK rate %d",
                      sample_rate_max, rcar->slot_size, rcar->ssi_voices, rcar->mclk_1);
        return EINVAL;
    }

    if ( rcar->mclk_2 != 0 &&
         rcar->mclk_2 / sample_rate_max * sample_rate_max == rcar->mclk_2 &&
         sample_rate_max * rcar->ssi_voices * rcar->slot_size > rcar->mclk_2 ) {
        ado_error_fmt("Max sample rate %d, slot_size %d and voices %d not supported by MCLK rate %d",
                      sample_rate_max, rcar->slot_size, rcar->ssi_voices, rcar->mclk_2);
        return EINVAL;
    }

    /* determine the sys_word_length and data_word_length settings for the SWL, DWL bit fields
       of the SSICR register; note that the override of SWL and DWL with special values for
       the TDM split modes is done in ssi_set_mode_bits */
    switch( rcar->slot_size ) {
        case 16:
            rcar->ssi_config.sys_word_length = ( rcar->ssi_op_mode == SSI_OP_MODE_MONO ?
                                                 SSI_SYS_WORD_LEN_16BIT_MONO : SSI_SYS_WORD_LEN_16BIT_STEREO );
            break;
        case 32:
            rcar->ssi_config.sys_word_length = ( rcar->ssi_op_mode == SSI_OP_MODE_MONO ?
                                                 SSI_SYS_WORD_LEN_32BIT_MONO : SSI_SYS_WORD_LEN_32BIT_STEREO );
            break;
        default:
            ado_error_fmt( "Unsupported slot size %d", rcar->slot_size );
            return EINVAL;
    }

    switch( rcar->sample_size ) {
        case 16:
            rcar->ssi_config.data_word_length      = SSI_DATA_WORD_LEN_16BIT;
            break;
        case 24:
            rcar->ssi_config.data_word_length      = SSI_DATA_WORD_LEN_24BIT;
            break;
        case 32:
            rcar->ssi_config.data_word_length      = SSI_DATA_WORD_LEN_32BIT;
            break;
        default:
            ado_error_fmt( "Unsupported sample size %d", rcar->sample_size );
            return EINVAL;
    }
    return EOK;
}

static void rcar_register_dump( HW_CONTEXT_T * rcar )
{
    uint32_t use_tx = 0, use_rx = 0;

    ado_debug (DB_LVL_DRIVER, "rcar : %s", __func__);

    ssiu_common_register_dump();

    use_tx = rcar->playback.ssi_chan == SSI_CHANNEL_NUM ? 0 : 1;
    use_rx = rcar->capture.ssi_chan == SSI_CHANNEL_NUM ? 0 : 1;

    if( use_tx ) {
        if (rcar->ssi_op_mode == SSI_OP_MODE_MULTICH) { //Multiple SSI
            ssiu_ssi_register_dump( SSI_CHANNEL_0 );
            ssiu_ssi_register_dump( SSI_CHANNEL_1 );
            ssiu_ssi_register_dump( SSI_CHANNEL_2 );
            if( rcar->voices == 8 ) {
                ssiu_ssi_register_dump( SSI_CHANNEL_9 );
            }
        } else {
            ssiu_ssi_register_dump( rcar->playback.ssi_chan );
        }
    }
    if( use_rx ) {
        ssiu_ssi_register_dump( rcar->capture.ssi_chan );
    }
    if (rcar->use_scu) {
        if( use_tx ) {
            scu_src_register_dump( rcar->playback.src_chan );
            scu_cmd_register_dump( rcar->playback.cmd_chan );
            scu_dvc_register_dump( rcar->playback.cmd_chan );
        }
        if( use_rx ) {
            scu_src_register_dump( rcar->capture.src_chan );
        }
    }

    adg_register_dump();

    rcar_mixer_register_dump();
}

static int rcar_init_cleanup( HW_CONTEXT_T * rcar )
{
    uint32_t use_tx = rcar->playback.ssi_chan == SSI_CHANNEL_NUM ? 0 : 1;
    uint32_t use_rx = rcar->capture.ssi_chan == SSI_CHANNEL_NUM ? 0 : 1;
    uint32_t use_hdmi = rcar->playback.hdmi_chan == SSI_HDMI_CHANNEL_NUM ? 0 : 1;

    ado_debug (DB_LVL_DRIVER, "rcar : %s", __func__);

    /* perform SSIU cleanup */
    if( use_tx && use_rx ) {
        ssiu_2channel_duplex_cleanup( rcar->playback.ssi_chan,
                                      rcar->capture.ssi_chan );
    } else if( use_tx ) {
        if (rcar->ssi_op_mode == SSI_OP_MODE_MULTICH) {
            if( rcar->voices == 8 ) {
                ssiu_4channel_transmit_cleanup();
            } else {
                ssiu_3channel_transmit_cleanup();
            }
        } else { //Single SSI
            ssiu_1channel_cleanup( rcar->playback.ssi_chan,
                                   rcar->master_ssi_channel );
        }
    } else if( use_rx ) {
        ssiu_1channel_cleanup( rcar->capture.ssi_chan,
                               rcar->master_ssi_channel );
    }

    /* release the reserved SSI and HDMI resources */
    if( use_tx ) {
        if (rcar->ssi_op_mode == SSI_OP_MODE_MULTICH) { //Multiple SSI
            rcar_release_ssi( SSI_CHANNEL_0, SSI_CHANNEL_2 );
            if( rcar->voices == 8 ) {
                rcar_release_ssi( SSI_CHANNEL_9, SSI_CHANNEL_9 );
            }
        } else {
            rcar_release_ssi( rcar->playback.ssi_chan, rcar->playback.ssi_chan );
        }
        if( use_hdmi ) {
            rcar_release_hdmi( rcar->playback.hdmi_chan, rcar->playback.hdmi_chan );
        }
        rcar->playback.ssi_chan = SSI_CHANNEL_NUM;
        rcar->playback.hdmi_chan = SSI_HDMI_CHANNEL_NUM;
    }

    if( use_rx ) {

        /* perform SSIU cleanup - TBA */

        rcar_release_ssi( rcar->capture.ssi_chan, rcar->capture.ssi_chan );
        rcar->capture.ssi_chan = SSI_CHANNEL_NUM;
    }

    /* clean-up and release the reserved SCU resources */
    if (rcar->use_scu) {
        if( use_tx ) {
            /* just in case, stop the SRC and CMD */
            scu_src_stop(rcar->playback.src_chan);
            scu_cmd_stop(rcar->playback.cmd_chan);

            scu_src_cleanup(rcar->playback.src_chan);
            scu_dvc_cleanup(rcar->playback.cmd_chan);

            rcar_release_src( rcar->playback.src_chan );
            rcar_release_cmd( rcar->playback.cmd_chan );
            rcar->playback.src_chan = SCU_SRC_CHANNEL_NUM;
            rcar->playback.cmd_chan = SCU_CMD_CHANNEL_NUM;
        }
        if( use_rx ) {
            /* just in case, stop the SRC */
            scu_src_stop(rcar->capture.src_chan);

            scu_src_cleanup(rcar->capture.src_chan);

            rcar_release_src( rcar->capture.src_chan );
            rcar->capture.src_chan = SCU_SRC_CHANNEL_NUM;
        }
    }
    return EOK;
}

int rcar_init( HW_CONTEXT_T * hw )
{
    int status = EOK;
    int i;

    rcar_context_t* rcar = (rcar_context_t*) hw;

    uint32_t use_tx = 0, use_rx = 0, use_hdmi = 0;

    use_tx = rcar->playback.ssi_chan == SSI_CHANNEL_NUM ? 0 : 1;
    use_rx = rcar->capture.ssi_chan == SSI_CHANNEL_NUM ? 0 : 1;
    use_hdmi = rcar->playback.hdmi_chan == SSI_HDMI_CHANNEL_NUM ? 0 : 1;

    ado_debug (DB_LVL_DRIVER, "INIT");

    /* seed the resources if not already done */
    rcar_create_resources();

    /* reserve the required resources */
    if( use_tx ) {
        if (rcar->ssi_op_mode == SSI_OP_MODE_MULTICH) { //Multiple SSI
            if( rcar_reserve_ssi( SSI_CHANNEL_0, SSI_CHANNEL_2 ) != EOK ) {
                //rcar_init_cleanup( rcar ); /* nothing to clean up yet */
                ado_error_fmt("failed reserving SSI 0,1,2");
                return EAGAIN;
            }
            if( rcar->voices == 8 &&
                rcar_reserve_ssi( SSI_CHANNEL_9, SSI_CHANNEL_9 ) != EOK ) {
                ado_error_fmt("failed reserving SSI 9");
                rcar_init_cleanup( rcar );
                return EAGAIN;
            }
        } else {
            if( rcar_reserve_ssi( rcar->playback.ssi_chan, rcar->playback.ssi_chan ) != EOK ) {
                //rcar_init_cleanup( rcar ); /* nothing to clean up yet */
                ado_error_fmt("failed reserving SSI %d", rcar->playback.ssi_chan);
                return EAGAIN;
            }
        }
        if( use_hdmi ) {
            if( rcar_reserve_hdmi( rcar->playback.hdmi_chan, rcar->playback.hdmi_chan ) != EOK ) {
                rcar_init_cleanup( rcar );
                ado_error_fmt("failed reserving HDMI %d", rcar->playback.hdmi_chan);
                return EAGAIN;
            }
        }
    }
    if( use_rx ) {
        if( rcar_reserve_ssi( rcar->capture.ssi_chan, rcar->capture.ssi_chan ) != EOK ) {
            ado_error_fmt("failed reserving SSI %d", rcar->capture.ssi_chan);
            rcar_init_cleanup( rcar );
            return EAGAIN;
        }
    }
    if (rcar->use_scu) {
        if( use_tx ) {
            if( rcar_reserve_src( rcar->voices > 2 ? 1 : 0, 0, 1,
                                  &rcar->playback.src_chan ) != EOK ||
                rcar_reserve_cmd( &rcar->playback.cmd_chan ) != EOK ) {
                ado_error_fmt("failed reserving playback SRC");
                rcar_init_cleanup( rcar );
                return EAGAIN;
            }
        }
        if( use_rx ) {
            if( rcar_reserve_src( rcar->voices > 2 ? 1 : 0, 0, 1,
                                  &rcar->capture.src_chan ) != EOK ) {
                ado_error_fmt("failed reserving capture SRC");
                rcar_init_cleanup( rcar );
                return EAGAIN;
            }
        }
    }

    /* set-up the SSIU */
    if( use_tx && use_rx ) {
        /* set-up for duplex - playback and capture */
        status = ssiu_2channel_duplex_setup( rcar->playback.ssi_chan,
                                             rcar->capture.ssi_chan,
                                             rcar->ssi_op_mode,
                                             rcar->ssi_masterslave_mode,
                                             rcar->ssi_transfer_mode,
                                             rcar->ssi_start_mode,
                                             rcar->sample_size,
                                             rcar->voices,
                                             rcar->ssi_voices,
                                             rcar->playback.hdmi_chan,
                                             &rcar->ssi_config,
                                             NULL, NULL );
    } else if( use_tx ) {
        /* set-up for playback */
        if (rcar->ssi_op_mode == SSI_OP_MODE_MULTICH) { //Multiple SSI
            if( rcar->voices == 8 ) {
                status = ssiu_4channel_transmit_setup( rcar->ssi_masterslave_mode,
                                                       16,
                                                       rcar->playback.hdmi_chan,
                                                       &rcar->ssi_config);
            } else {
                status = ssiu_3channel_transmit_setup(rcar->ssi_masterslave_mode,
                                                      16,
                                                      rcar->playback.hdmi_chan,
                                                      &rcar->ssi_config);
            }
        } else { //Single SSI
            status = ssiu_1channel_setup( rcar->playback.ssi_chan,
                                          rcar->master_ssi_channel,
                                          1, /* transmit */
                                          rcar->ssi_op_mode,
                                          rcar->ssi_transfer_mode,
                                          rcar->sample_size,
                                          rcar->voices,
                                          rcar->ssi_voices,
                                          rcar->playback.hdmi_chan,
                                          &rcar->ssi_config,
                                          NULL );
        }
    } else if( use_rx ) {
        status = ssiu_1channel_setup( rcar->capture.ssi_chan,
                                      rcar->master_ssi_channel,
                                      0, /* receive */
                                      rcar->ssi_op_mode,
                                      rcar->ssi_transfer_mode,
                                      rcar->sample_size,
                                      rcar->voices,
                                      rcar->ssi_voices,
                                      rcar->playback.hdmi_chan,
                                      &rcar->ssi_config,
                                      NULL );
    }

    if( status != EOK ) {
        ado_error_fmt("failed setting up SSIU");
        rcar_init_cleanup( rcar );
        return status;
    }

    /* Set-up the SCU */
    if (rcar->use_scu == 1) {
        if( use_tx ) {
            /* Initialize the DVC volume variables to 0dB for all channels */
            for( i = 0; i < sizeof(rcar->playback.dvc_volume)/sizeof(rcar->playback.dvc_volume[0]); i++ ) {
                rcar->playback.dvc_volume[i] = SCU_DVC_MAX_VOLUME; /* SCU_DVC_MAX_VOLUME corresponds to 0 dB */
            }

            /* Initialize the DVC mute to unmuted for all channels */
            rcar->playback.dvc_mute = 0;

            /* Setup syncronous SRC0 */
            status = scu_src_setup( rcar->playback.src_chan,
                                    1,
                                    0,
                                    rcar->sample_size,
                                    rcar->voices,
                                    rcar->sample_rate_max,
                                    SAMPLE_RATE_SRC );
            /* use SRC0 -> CMD0 */
            if( status == EOK ) {
                status = scu_cmd_setup( rcar->playback.cmd_chan,
                                        rcar->playback.src_chan );
            }

            /* setup DVC0 */
            if( status == EOK ) {
                status = scu_dvc_setup( rcar->playback.cmd_chan,
                                        rcar->sample_size,
                                        rcar->voices,
                                        rcar->playback.dvc_volume,
                                        rcar->playback.dvc_mute );
            }
        }
        if( use_rx ) {
            /* Setup SRC */
            status = scu_src_setup( rcar->capture.src_chan,
                                     0,
                                     1,
                                     rcar->sample_size,
                                     rcar->voices,
                                     SAMPLE_RATE_SRC,
                                     rcar->sample_rate_max );
        }
    }

    if( status != EOK ) {
        ado_error_fmt("failed setting up SCU");
        rcar_init_cleanup( rcar );
        return status;
    }

    status = rcar_set_clock_rate( rcar );

    if( status != EOK ) {
        ado_error_fmt("failed setting up the clock rate");
        rcar_init_cleanup( rcar );
        return status;
    }

    if( rcar->debug ) {
        rcar_register_dump( rcar );
    }

    return EOK;
}

static int rcar_set_clock_rate( rcar_context_t * rcar )
{
    int ret = EOK;
    uint32_t divisor = 0;
    uint32_t ssi_divisor = 0;
    uint32_t adg_divisor = 0;

    uint32_t sample_rate;
    uint32_t ssi_sample_rate;
    uint32_t scu_clk_src = 0; // SSI 0
    uint32_t mclk_src = AUDIO_CLKA;
    uint32_t mclk_rate;

    ado_debug( DB_LVL_DRIVER, "sample_rate=%d, sample_rate_max=%d",
               rcar->sample_rate, rcar->sample_rate_max );

    if( !rcar->sample_rate ) {
        sample_rate = rcar->sample_rate_max;
    } else {
        sample_rate = rcar->sample_rate;
    }
    if( rcar->use_scu ) {
        ssi_sample_rate = SAMPLE_RATE_SRC;
    } else {
        ssi_sample_rate = sample_rate;
    }
    ado_debug( DB_LVL_DRIVER, "ssi_sample_rate=%d", ssi_sample_rate );

    if ( rcar->mclk_1 != 0 && rcar->mclk_1 / ssi_sample_rate * ssi_sample_rate == rcar->mclk_1 ) {
        mclk_rate = rcar->mclk_1;
        mclk_src = MCLK_SOURCE_1;
    } else if ( rcar->mclk_2 != 0 && rcar->mclk_2 / ssi_sample_rate * ssi_sample_rate == rcar->mclk_2 ) {
        mclk_rate = rcar->mclk_2;
        mclk_src = MCLK_SOURCE_2;
    } else if ( rcar->mclk_fs != 0 ) {
        mclk_rate = ssi_sample_rate * rcar->mclk_fs;
        mclk_src = MCLK_SOURCE_FS;
    } else {
        ado_error_fmt("Could not find a valid mclk source for sample_rate %d", ssi_sample_rate);
	    return EINVAL;
    }

    ado_debug( DB_LVL_DRIVER, "Selected clock source %s with mclk rate %d",
               ( mclk_src == AUDIO_CLKA ? "AUDIO_CLKA" : "AUDIO_CLKB" ), mclk_rate );

    // divisor = mclk_rate / slot_size / ssi_voices / ssi_sample_rate
    divisor = mclk_rate / rcar->slot_size / rcar->ssi_voices / ssi_sample_rate;

    // Divisors in two different audio blocks (ADG and SSI) must be used to
    // achieve the desired range of clock rates. A second, more subtle
    // restriction dictates that the SSI divisor must be a value other than
    // '1' if a continuous frame clock (i.e. even without data streaming) is
    // configured.
    // ADG divisor possible values are 1, 2, 4, 8, 16, 32
    // SSI divisor possible values are 1, 2, 4, 8, 16, 6, 12
    switch ( divisor ) {
        case 1:
        case 2:
        case 4:
        case 6:
        case 8:
        case 12:
        case 16:
            ssi_divisor = divisor;
            adg_divisor = 1;
            break;
        case 24:
        case 32:
            ssi_divisor = divisor>>1; // divisor / 2
            adg_divisor = 2;
            break;
        case 48:
        case 64:
            ssi_divisor = divisor>>2; // divisor / 4
            adg_divisor = 4;
            break;
        case 96:
        case 128:
            ssi_divisor = divisor>>3; // divisor / 8
            adg_divisor = 8;
            break;
        case 192:
            ssi_divisor = divisor>>4; // divisor / 16
            adg_divisor = 16;
            break;
        default:
            ado_error_fmt("Unsupported divisor %d", divisor);
            return EINVAL;
    }

    ado_debug( DB_LVL_DRIVER, "Set SSI divisor to %d, ADG divisor to %d",
               ssi_divisor, adg_divisor );

    if( rcar->ssi_op_mode == SSI_OP_MODE_MULTICH ) {
        ret = ssi_set_divisor( SSI_CHANNEL_0, ssi_divisor );
        if( ret == EOK ) ret = ssi_set_divisor( SSI_CHANNEL_1, ssi_divisor );
        if( ret == EOK ) ret = ssi_set_divisor( SSI_CHANNEL_2, ssi_divisor );
        if( ret == EOK && rcar->voices == 8 ) {
            ret = ssi_set_divisor( SSI_CHANNEL_9, ssi_divisor );
        }
    } else {
        if( rcar->playback.ssi_chan != SSI_CHANNEL_NUM ) {
            ret = ssi_set_divisor( rcar->playback.ssi_chan, ssi_divisor );
        }
        if( ret == EOK && rcar->capture.ssi_chan != SSI_CHANNEL_NUM ) {
            ret = ssi_set_divisor( rcar->capture.ssi_chan, ssi_divisor );
        }
    }
    if (ret != EOK) {
        ado_error_fmt( "failed setting the SSI divisor" );
        return ret;
    }

    if( rcar->playback.ssi_chan != SSI_CHANNEL_NUM ) {
        ret = adg_set_ssi_clksrc(rcar->playback.ssi_chan, mclk_src);
        if (ret != EOK) {
            ado_error_fmt( "failed setting the ADG Clock Select for SSI %d", rcar->playback.ssi_chan );
            return ret;
        }
        ret = adg_set_ssi_clkdiv(rcar->playback.ssi_chan, adg_divisor);
        if (ret != EOK) {
            ado_error_fmt( "failed setting the ADG divisor for SSI %d", rcar->playback.ssi_chan );
            return ret;
        }
    }

    if( rcar->capture.ssi_chan != SSI_CHANNEL_NUM ) {
        ret = adg_set_ssi_clksrc(rcar->capture.ssi_chan, mclk_src);
        if (ret != EOK) {
            ado_error_fmt( "failed setting the ADG Clock Select for SSI %d", rcar->playback.ssi_chan );
            return ret;
        }
        ret = adg_set_ssi_clkdiv(rcar->capture.ssi_chan, adg_divisor);
        if (ret != EOK) {
            ado_error_fmt( "failed setting the ADG divisor for SSI %d", rcar->capture.ssi_chan );
            return ret;
        }
    }

    /* set the mixer/codec clock rate if applicable */
    ret = rcar_mixer_set_clock_rate( rcar, ssi_sample_rate );
    if (ret != EOK) {
        ado_error_fmt( "failed setting the mixer clock rate" );
        return ret;
    }

#if _NTO_VERSION >= 700
    ret = VARIANT_SET_CLOCK_RATE( mclk_rate, sample_rate );
    if( ret != EOK ) {
        ado_error_fmt ("Failed to set variant clock rate");
        return ret;
    }
#endif

    if( rcar->use_scu ) {
        if( rcar->playback.ssi_chan != SSI_CHANNEL_NUM ) {
            scu_clk_src = ( rcar->master_ssi_channel != SSI_CHANNEL_NUM ?
                            rcar->master_ssi_channel : rcar->playback.ssi_chan );

            /* Re-setup SRC */
            ret = scu_src_setup( rcar->playback.src_chan,
                                 1,
                                 0,
                                 rcar->sample_size,
                                 rcar->voices,
                                 sample_rate,
                                 ssi_sample_rate );
            if( ret == EOK ) {
                ret = adg_set_src_clksrc( rcar->playback.src_chan, scu_clk_src, 1 );
            }
            /* Re-setup DVC  */
            if( ret == EOK ) {
                ret = scu_dvc_setup( rcar->playback.cmd_chan,
                                     rcar->sample_size,
                                     rcar->voices,
                                     rcar->playback.dvc_volume,
                                     rcar->playback.dvc_mute );
            }
            if( ret == EOK ) {
                ret = adg_set_cmd_clksrc( rcar->playback.cmd_chan, scu_clk_src );
            }
        }
        if( rcar->capture.ssi_chan != SSI_CHANNEL_NUM ) {
            scu_clk_src = ( rcar->master_ssi_channel != SSI_CHANNEL_NUM ?
                            rcar->master_ssi_channel : rcar->capture.ssi_chan );

            /* Re-configure scu-src with new sample rate */
            ret = scu_src_setup( rcar->capture.src_chan,
                                 0,
                                 1,
                                 rcar->sample_size,
                                 rcar->voices,
                                 ssi_sample_rate,
                                 sample_rate );
            if( ret == EOK ) {
                ret = adg_set_src_clksrc( rcar->capture.src_chan, scu_clk_src, 0 );
            }
        }

        if( ret != EOK) {
            ado_error_fmt( "failed setting the SCU rate" );
        }
    }

    return EOK;
}

int rcar_set_dvc_output_volume(rcar_context_t * rcar, uint32_t voice_channel, uint32_t vol)
{
    int status = scu_dvc_set_vol( rcar->playback.cmd_chan, voice_channel, vol );
    if ( status == EOK ) {
        rcar->playback.dvc_volume[voice_channel] = vol;
    }
    return status;
}

int rcar_get_dvc_output_volume(rcar_context_t * rcar, uint32_t voice_channel, uint32_t *vol)
{
    if ( vol == NULL ) {
        return EINVAL;
    }
    if ( voice_channel >= sizeof(rcar->playback.dvc_volume)/sizeof(rcar->playback.dvc_volume[0]) ) {
        return EINVAL;
    }
    *vol = rcar->playback.dvc_volume[voice_channel];
    return EOK;
}

int rcar_set_dvc_output_mute(rcar_context_t * rcar, uint32_t mute)
{
    int status = scu_dvc_set_mute( rcar->playback.cmd_chan, mute );
	if (status == EOK) {
        rcar->playback.dvc_mute = mute;
    }
    return status;
}

int rcar_get_dvc_output_mute(rcar_context_t * rcar, uint32_t *mute)
{
    return scu_dvc_get_mute( rcar->playback.cmd_chan, mute );
}

ado_dll_version_t ctrl_version;
void ctrl_version (int *major, int *minor, char *date)
{
    *major = ADO_MAJOR_VERSION;
#if _NTO_VERSION < 700
    *minor = 2;
#else
    *minor = 1;
#endif
    date = __DATE__;
}

static void ctrl_init_cleanup(rcar_context_t * rcar)
{
    ado_debug (DB_LVL_DRIVER, "rcar : %s", __func__);

    rcar_init_cleanup (rcar);
    ado_mutex_destroy (&rcar->hw_lock);

    ssiu_deinit();
    scu_deinit();
    adg_deinit();
    audio_dmac_deinit(&rcar->playback.dma_context, &rcar->capture.dma_context);

#if _NTO_VERSION >= 700
    (void)VARIANT_UNINIT(rcar);
#endif

    ado_free (rcar);

    rcar = NULL;
}

/* */
/*  Initialize */
/* */

int ctrl_init (HW_CONTEXT_T ** hw_context, ado_card_t * card, char *args)
{
    rcar_context_t *rcar;
    int status;

    ado_debug (DB_LVL_DRIVER, "rcar : CTRL_DLL_INIT");

    if ((rcar = (rcar_context_t *) ado_calloc (1, sizeof (rcar_context_t))) == NULL) {
        ado_error_fmt ("Unable to allocate memory (%s)", strerror (errno));
        return ENOMEM;
    }

    memset(rcar, 0, sizeof(rcar_context_t));

    *hw_context = rcar;

    if ((status = rcar_parse_commandline (rcar, args)) != EOK) {
        ado_free (rcar);
        return status;
    }

    ado_card_set_shortname (card, "rcar");
    ado_card_set_longname (card, "rcar", RCAR_GEN2_SSI_BASE);

    /* Map Common SSIU base register */
    if ((status = ssiu_init()) != EOK) {
        ado_error_fmt ("rcar: SSIU init failed");
        ado_free (rcar);
        rcar = NULL;
        return status;
    }

    /* Map SCU */
    if ((status = scu_init()) != EOK) {
        ado_error_fmt ("rcar: SCU init failed");
        ssiu_deinit();
        ado_free (rcar);
        rcar = NULL;
        return status;
    }

    /* Map ADG */
    if ((status = adg_init()) != EOK) {
        ado_error_fmt ("rcar: ADG init failed");
        ssiu_deinit();
        scu_deinit();
        ado_free (rcar);
        rcar = NULL;
        return status;
    }

    /* Map DMAC */
    if ((status = audio_dmac_init(&rcar->playback.dma_context, &rcar->capture.dma_context)) != EOK) {
        ado_error_fmt ("rcar: Audio DMAC init failed (%s)", strerror (errno));
        ssiu_deinit();
        scu_deinit();
        adg_deinit();
        ado_free (rcar);
        rcar = NULL;
        return status;
    }

    ado_mutex_init (&rcar->hw_lock);

    if( (status = rcar_init(rcar)) != EOK ) {
        ado_error_fmt ("rcar: RCAR hw init failed");
        ctrl_init_cleanup(rcar);
        return status;
    }

    ado_debug (DB_LVL_DRIVER, "Attaching interrupts - playback: %d, capture %d",
               rcar->playback.dma_context.audiodma_irq, rcar->capture.dma_context.audiodma_irq);

    if ((status = ado_attach_interrupt (card, rcar->playback.dma_context.audiodma_irq, rcar_playback_interrupt, rcar)) != EOK ||
        (status = ado_attach_interrupt (card, rcar->capture.dma_context.audiodma_irq, rcar_capture_interrupt, rcar)) != EOK) {
        ado_error_fmt ("rcar: Unable to attach interrupt (%s)", strerror (errno));
        ctrl_init_cleanup(rcar);
        return status;
    }

    rcar->playback.pcm_caps.chn_flags = SND_PCM_CHNINFO_BLOCK | SND_PCM_CHNINFO_STREAM |
        SND_PCM_CHNINFO_INTERLEAVE | SND_PCM_CHNINFO_BLOCK_TRANSFER |
        SND_PCM_CHNINFO_MMAP | SND_PCM_CHNINFO_MMAP_VALID;

    if( rcar->sample_size == 16 ) {
        rcar->playback.pcm_caps.formats = SND_PCM_FMT_S16_LE;
    } else {
        /* for sample size 24 bit, advertise S32_LE to io-audio, as on Rcar the 24 bit
         * need to occupy the three most significant bytes of a 32 bit word */
        rcar->playback.pcm_caps.formats = SND_PCM_FMT_S32_LE;
    }

    if( configured_rate_list ) {
        rcar->playback.pcm_caps.rates = configured_rate_list;
    } else {
        rcar->playback.pcm_caps.rates = supported_rate_list;
    }
    rcar->playback.pcm_caps.min_rate = rcar->sample_rate_min;
    rcar->playback.pcm_caps.max_rate = rcar->sample_rate_max;
    rcar->playback.pcm_caps.min_voices = rcar->voices;
    rcar->playback.pcm_caps.max_voices = rcar->voices;
    rcar->playback.pcm_caps.min_fragsize = 64;
    rcar->playback.pcm_caps.max_fragsize = 64 * 1024;
    rcar->playback.pcm_caps.max_frags = 2;

    /* Set capabilities of recording */
    memcpy (&rcar->capture.pcm_caps, &rcar->playback.pcm_caps,
        sizeof (rcar->playback.pcm_caps));
    rcar->capture.pcm_caps.max_voices = rcar->voices;

    /* Set functions for playback */
    rcar->playback.pcm_funcs.aquire = rcar_playback_acquire;
    rcar->playback.pcm_funcs.release = rcar_playback_release;
    rcar->playback.pcm_funcs.prepare = rcar_playback_prepare;
    rcar->playback.pcm_funcs.trigger = rcar_playback_trigger;
#if _NTO_VERSION < 700
    rcar->playback.pcm_funcs.position3 = rcar_position;
    rcar->playback.pcm_funcs.capabilities2 = rcar_capabilities;
#else
    rcar->playback.pcm_funcs.position = rcar_position;
    rcar->playback.pcm_funcs.capabilities = rcar_capabilities;
#endif

    /* Set functions for recording */
    rcar->capture.pcm_funcs.aquire = rcar_capture_acquire;
    rcar->capture.pcm_funcs.release = rcar_capture_release;
    rcar->capture.pcm_funcs.prepare = rcar_capture_prepare;
    rcar->capture.pcm_funcs.trigger = rcar_capture_trigger;
#if _NTO_VERSION < 700
    rcar->capture.pcm_funcs.position3 = rcar_position;
    rcar->capture.pcm_funcs.capabilities2 = rcar_capabilities;
#else
    rcar->capture.pcm_funcs.position = rcar_position;
    rcar->capture.pcm_funcs.capabilities = rcar_capabilities;
#endif

#if _NTO_VERSION >= 700
    if ( (status = rcar_get_i2c_bus_num(&rcar->i2c_dev)) != EOK ) {
        ado_error_fmt ("rcar: Invalid i2c dev");
        return status;
    }

    if( (status = VARIANT_INIT(rcar) != EOK ) ) {
        ado_error_fmt ("rcar: Unable to perform variant specific init");
        ctrl_init_cleanup(rcar);
        return status;
    }
#endif

    if( (status = rcar_mixer_init(card, rcar)) != EOK ) {
        ado_error_fmt ("rcar: Unable to init a mixer");
        ctrl_init_cleanup(rcar);
        return status;
    }

    /* Create a PCM audio device */
#if _NTO_VERSION < 700
    if( (status = ado_pcm_create (card, "R-Car SSI", 0, "rcar",
            1, &rcar->playback.pcm_caps, &rcar->playback.pcm_funcs,
            1, &rcar->capture.pcm_caps, &rcar->capture.pcm_funcs, &rcar->pcm)) != EOK ) {
        ado_error_fmt ("rcar: Unable to create pcm devices (%s)", strerror (errno));
        ctrl_init_cleanup(rcar);
        return status;
    }
#else
    if( (status = ado_pcm_create (card, "R-Car SSI", 0, "rcar",
            1, &rcar->playback.pcm_caps, &rcar->playback.pcm_funcs,
            1, &rcar->capture.pcm_caps, &rcar->capture.pcm_funcs, rcar->mixer, &rcar->pcm)) != EOK ) {
        ado_error_fmt ("rcar: Unable to create pcm devices (%s)", strerror (errno));
        ctrl_init_cleanup(rcar);
        return status;
    }
#endif
    rcar_mixer_set_default_group( rcar->pcm, rcar->mixer, ADO_PCM_CHANNEL_PLAYBACK, 0 );
    rcar_mixer_set_default_group( rcar->pcm, rcar->mixer, ADO_PCM_CHANNEL_CAPTURE, 0 );

    ado_debug (DB_LVL_DRIVER, "rcar initialization complete.....");

    return EOK;
}

ado_ctrl_dll_destroy_t ctrl_destroy;
int ctrl_destroy (HW_CONTEXT_T * rcar)
{
    ado_debug (DB_LVL_DRIVER, "rcar : CTRL_DLL_DESTROY");

    ctrl_init_cleanup(rcar);

    rcar = NULL;

    return EOK;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/deva/ctrl/rcar/rcar_dll.c $ $Rev: 852847 $")
#endif
