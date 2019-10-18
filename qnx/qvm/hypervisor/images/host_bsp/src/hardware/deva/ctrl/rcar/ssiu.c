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

#include <audio_driver.h>
#include "ssiu.h"
#include "rcar_support.h"


/* Helper functions to set SSI register bit fields */

static int ssi_set_role_bits(uint32_t* cr, uint32_t* cre, uint32_t* wsr, uint32_t is_master, uint32_t is_transmit)
{
    if( cr == 0 || wsr == 0 ) {
         return EINVAL;
    }

    (*cr) &= ~( SSICR_TRMD_MASK | SSICR_SWSD_MASK | SSICR_SCKD_MASK );
    (*wsr) &= ~SSIWSR_CONT_MASK;

    if( is_master ) {
        (*cr) |= SSICR_SWSD_OUTPUT | SSICR_SCKD_OUTPUT;
        (*wsr) |= SSIWSR_CONT_ENABLED;
    } else {
        (*cr) |= SSICR_SWSD_INPUT | SSICR_SCKD_INPUT;
        (*wsr) |= SSIWSR_CONT_DISABLED; // for slave the CONT mode must be disabled
    }

    if( is_transmit ) {
        (*cr) |= SSICR_TRMD_TX_MODE;
    } else {
        (*cr) |= SSICR_TRMD_RX_MODE;
    }

    return EOK;
}

static int ssi_set_mode_bits(uint32_t* cr, uint32_t* cre, uint32_t* wsr, ssi_op_mode_t mode, uint32_t voicenum)
{
    if( cr == 0 || cre == 0 || wsr == 0 ) {
         return EINVAL;
    }

    // for most modes, populate the CHNL and SDTA bit fields of SSICR,
    // the MONO and WSMODE bit fields of SSIWSR and if applicable,
    // the CHNL2 bit field of SSICRE
    // for the TDM split modes also overwrite the SWL and DWL bit fields
    // that were populated by ssi_set_config_bits
    (*cr) &= ~( SSICR_CHNL_MASK | SSICR_SDTA_MASK );
    (*wsr) &= ~( SSIWSR_MONO_MASK | SSIWSR_WSMODE_MASK );
    (*cre) &= ~SSICRE_CHNL2_MASK;

    if( mode == SSI_OP_MODE_MONO ) {
        (*cr) &= ~SSICR_SDTA_MASK; // for monaural SDTA must be forced to 0
        (*wsr) |=  SSIWSR_WSMODE_TDM_MONO | SSIWSR_MONO_MONO;
        (*cr) |= SSICR_CHNL_1 | SSICR_SDTA_SDATA_FIRST;
    } else if( mode == SSI_OP_MODE_STEREO || mode == SSI_OP_MODE_MULTICH ) {
        (*cr) |=  SSICR_CHNL_1 | SSICR_SDTA_SDATA_FIRST;
        (*wsr) |= SSIWSR_WSMODE_STEREO_MULTICH | SSIWSR_MONO_TDM ;
    } else if( mode == SSI_OP_MODE_TDM || mode == SSI_OP_MODE_TDMEXT ) {
        (*wsr) |=  SSIWSR_WSMODE_TDM_MONO | SSIWSR_MONO_TDM;
        if( voicenum == 4 ) {
            (*cr) |= SSICR_CHNL_2;
        } else if( voicenum == 6 ) {
            (*cr) |= SSICR_CHNL_3;
        } else if( voicenum == 8 ) {
            (*cr) |= SSICR_CHNL_4;
        } else {
            return EINVAL;
        }
    // for the TDM split modes SWL and DWL have special settings,
    // overwrite the default settings
    } else if( mode == SSI_OP_MODE_TDMSPLIT_4XMONO ) {
        (*cr) &= ~( SSICR_SWL_MASK | SSICR_DWL_MASK );
        (*wsr) |=  SSIWSR_WSMODE_TDM_MONO | SSIWSR_MONO_TDM;
        (*cr) |= SSICR_DWL_32BIT | SSICR_SWL_32BIT | SSICR_CHNL_2;
    } else if( mode == SSI_OP_MODE_TDMSPLIT_4XSTEREO ) {
        (*cr) &= ~( SSICR_SWL_MASK | SSICR_DWL_MASK );
        (*wsr) |=  SSIWSR_WSMODE_TDM_MONO | SSIWSR_MONO_TDM;
        (*cr) |= SSICR_DWL_32BIT | SSICR_SWL_32BIT | SSICR_CHNL_4;
    } else if( mode == SSI_OP_MODE_TDMEXSPLIT_16CHAN ) {
        (*wsr) |=  SSIWSR_WSMODE_TDM_MONO | SSIWSR_MONO_TDM;
        (*cre) |= SSICRE_CHNL2_MASK;
    } else if( mode == SSI_OP_MODE_TDMEXSPLIT_8CHAN ) {
        (*wsr) |=  SSIWSR_WSMODE_TDM_MONO | SSIWSR_MONO_TDM;
        (*cr) |= SSICR_CHNL_4;
    } else if( mode == SSI_OP_MODE_TDM_16CHAN ) {
        (*wsr) |=  SSIWSR_WSMODE_TDM_MONO | SSIWSR_MONO_TDM;
        (*cre) |= SSICRE_CHNL2_MASK;
    }

    return EOK;
}

static int ssi_set_config_bits(uint32_t* cr, ssi_config_t * config)
{
    if( cr == 0 ) {
         return EINVAL;
    }

    (*cr) &= ~(SSICR_DWL_MASK | SSICR_SWL_MASK | SSICR_SCKP_MASK | SSICR_SWSP_MASK |
               SSICR_SPDP_MASK | SSICR_SDTA_MASK | SSICR_DEL_MASK);
    (*cr) |= (config->bit_delay << 8);
    (*cr) |= (config->serial_data_alignment<<10);
    (*cr) |= (config->padding_pol<<11);
    (*cr) |= (config->ws_pol << 12);
    (*cr) |= (config->clk_pol << 13);
    (*cr) |= (config->sys_word_length << 16);
    (*cr) |= (config->data_word_length << 19);

    return EOK;
}

static int ssi_set_clockdiv_bits(uint32_t* cr, uint32_t divisor)
{
    if( cr == 0 ) {
         return EINVAL;
    }

    *cr &= ~SSICR_CKDV_MASK;

    switch (divisor) {
        case 1:
            *cr |= SSICR_CKDV_1;
            break;
        case 2:
            *cr |= SSICR_CKDV_2;
            break;
        case 4:
            *cr |= SSICR_CKDV_4;
            break;
        case 6:
            *cr |= SSICR_CKDV_6;
            break;
        case 8:
            *cr |= SSICR_CKDV_8;
            break;
        case 12:
            *cr |= SSICR_CKDV_12;
            break;
        case 16:
            *cr |= SSICR_CKDV_16;
            break;
        default:
            return EINVAL;
    }
    return EOK;
}

static int ssi_set_control_bits(uint32_t* cr, uint32_t enable, uint32_t mute)
{
    if( cr == 0 ) {
         return EINVAL;
    }

    (*cr) &= ~(SSICR_EN_MASK | SSICR_MUEN_MASK);
    if( enable ) {
        (*cr) |= SSICR_EN_ENABLE;
    } else {
        (*cr) |= SSICR_EN_DISABLE;
    }
    if( mute ) {
        (*cr) |= SSICR_MUEN_MUTED;
    } else {
        (*cr) |= SSICR_MUEN_UNMUTED;
    }

    return EOK;
}

#if 0
static int ssi_set_interrupt_bits(uint32_t* cr, ssi_interrupt_t* interrupts)
{
    if( cr == 0 || interrupts == 0 ) {
        return EINVAL;
    }
    (*cr) &= ~(SSICR_DMEN_MASK | SSICR_UIEN_MASK | SSICR_OIEN_MASK | SSICR_IIEN_MASK | SSICR_DIEN_MASK);
    if( interrupts->dma ) {
        (*cr) |= SSICR_DMEN_ENABLE;
    } else {
        (*cr) |= SSICR_DMEN_DISABLE;
    }
    if( interrupts->underflow ) {
        (*cr) |= SSICR_UIEN_ENABLE;
    } else {
        (*cr) |= SSICR_UIEN_DISABLE;
    }
    if( interrupts->overflow ) {
        (*cr) |= SSICR_OIEN_ENABLE;
    } else {
        (*cr) |= SSICR_OIEN_DISABLE;
    }
    if( interrupts->idle_mode ) {
        (*cr) |= SSICR_IIEN_ENABLE;
    } else {
        (*cr) |= SSICR_IIEN_DISABLE;
    }
    if( interrupts->data ) {
        (*cr) |= SSICR_DIEN_ENABLE;
    } else {
        (*cr) |= SSICR_DIEN_DISABLE;
    }
    return EOK;
}
#endif

/* SSI register configuration functions */

static int ssi_set_control_and_mode
(
    uint32_t ssi_channel,
    uint32_t is_master,
    uint32_t is_transmit,
    uint32_t ssi_op_mode,
    uint32_t voicenum,
    ssi_config_t* config
)
{
    uint32_t cr, cre = 0, wsr;

    if( !rcar_ssi_supported(ssi_channel) ) {
        ado_error_fmt("SSI%d is not supported", ssi_channel);
        return EINVAL;
    }
    if( !SSI(ssi_channel) ) {
        return EFAULT;
    }

    cr = SSI(ssi_channel)->cr;
    if( SSI_EXT(ssi_channel) ) {
        cre = SSI_EXT(ssi_channel)->cre;
    }
    wsr = SSI(ssi_channel)->wsr;

    if( ssi_set_role_bits( &cr, &cre, &wsr, is_master, is_transmit ) != EOK ) {
        return EINVAL;
    }

    if( ssi_set_config_bits( &cr, config) != EOK ) {
        return EINVAL;
    }

    // important: call ssi_set_mode_bits after ssi_set_config_bits, because it
    // overrides the SWL and DWL bit fields with special values for some ssi_op_mode values
    if( ssi_set_mode_bits( &cr, &cre, &wsr, ssi_op_mode, voicenum ) != EOK ) {
        return EINVAL;
    }

    if( ssi_set_control_bits( &cr, 0, 0 ) != EOK ) {
        return EINVAL;
    }

    cr |= SSICR_FORCE_FIXED;

    SSI(ssi_channel)->cr = cr;
    if( SSI_EXT(ssi_channel) ) {
        SSI_EXT(ssi_channel)->cre = cre;
    }
    SSI(ssi_channel)->wsr = wsr;

    return EOK;
}

static int ssi_clear_control_and_mode( uint32_t ssi_channel )
{
    if (!rcar_ssi_supported(ssi_channel) || !SSI(ssi_channel))
    {
        ado_error_fmt("SSI%d is not supported", ssi_channel);
        return EINVAL;
    }
    SSI(ssi_channel)->cr &= ~0x7FFF;
    SSI(ssi_channel)->wsr = 0;

    return EOK;
}

/* Public SSI level functions - start/stop */

int ssi_start(uint32_t ssi_channel, ssi_start_mode_t start_mode)
{
    if( !rcar_ssi_supported(ssi_channel) ) {
        ado_error_fmt("SSI%d is not supported", ssi_channel);
        return EINVAL;
    }
    if( !SSI(ssi_channel) ) {
        return EFAULT;
    }

    // if this SSI uses independent start, set SSICR_EN at this time to start
    // the transfer; otherwise set only the other registers, the actual transfer
    // will start in that case via one of the SSI_CONTROL_SSI1029 or SSI_CONTROL_SSI34
    // bit masks of the SSI_CONTROL register (implemented as part of ssiu_start)
    if( start_mode == SSI_INDEPENDENT_START ) {
        SSI(ssi_channel)->cr |= SSICR_EN_ENABLE | SSICR_DMEN_ENABLE |
                                SSICR_UIEN_ENABLE | SSICR_OIEN_ENABLE | SSICR_FORCE_FIXED;
    } else {
        SSI(ssi_channel)->cr |= SSICR_DMEN_ENABLE |
                                SSICR_UIEN_ENABLE | SSICR_OIEN_ENABLE | SSICR_FORCE_FIXED;
    }

    return EOK;
}

int ssi_wait_status(uint32_t ssi_channel, uint32_t bitmask)
{
    uint32_t i, timeout = 1000;

    if( !rcar_ssi_supported(ssi_channel) ) {
        ado_error_fmt("SSI%d is not supported", ssi_channel);
        return EINVAL;
    }
    if( !SSI(ssi_channel) ) {
        return EFAULT;
    }

    for (i = 0; i < timeout; i++)
    {
        if (SSI(ssi_channel)->sr & bitmask)
        {
            return EOK;
        }
        usleep(1000);
    }

    return ETIMEDOUT;
}

int ssi_stop(uint32_t ssi_channel)
{
    int dirq_status;

    if( !rcar_ssi_supported(ssi_channel) ) {
        ado_error_fmt("SSI%d is not supported", ssi_channel);
        return EINVAL;
    }
    if( !SSI(ssi_channel) ) {
        return EFAULT;
    }

    // old driver was not disabling the OIEN and UIEN interrupts here
    SSI(ssi_channel)->cr &= ~(SSICR_DMEN_MASK | SSICR_UIEN_MASK | SSICR_OIEN_MASK);

    dirq_status = ssi_wait_status(ssi_channel, SSISR_DIRQ_MASK);

    if ( dirq_status != EOK ) {
        ado_error_fmt("SSI%d timeout waiting for DIRQ");
    }

    SSI(ssi_channel)->cr &= ~SSICR_EN_MASK;
    SSI(ssi_channel)->cr |= SSICR_IIEN_ENABLE;

    return EOK;
}

int ssi_stop_idle_int(uint32_t ssi_channel)
{
    if( !rcar_ssi_supported(ssi_channel) ) {
        ado_error_fmt("SSI%d is not supported", ssi_channel);
        return EINVAL;
    }
    if( !SSI(ssi_channel) ) {
        return EFAULT;
    }
    SSI(ssi_channel)->cr &= ~(SSICR_EN_MASK|SSICR_IIEN_MASK);
    return EOK;
}

/* SSIU register configuration functions */
static int ssiu_set_mode_1channel(uint32_t ssi_channel, uint32_t transfer_mode)
{
    if( !rcar_ssi_supported(ssi_channel) ) {
        ado_error_fmt("SSI%d is not supported", ssi_channel);
        return EINVAL;
    }
    if( !SSIU_COMMON || !SSI(ssi_channel)) {
        return EFAULT;
    }

    // MODE0

    // enable or disable the independent mode for the given ssi_channel
    SSIU_COMMON->mode0 &= ~SSI_MODE0_IND_MASK(ssi_channel);
    if( transfer_mode == SSI_INDEPENDENT_TRANSFER ){
        SSIU_COMMON->mode0 |= SSI_MODE0_IND_ENABLE(ssi_channel);
    } else {
        SSIU_COMMON->mode0 |= SSI_MODE0_IND_DISABLE(ssi_channel);
    }

    //MODE1, MODE2, MODE3

    // to set up any of SSI 0,1,2,9 as an independent transfer channel, disable the
    // four channel mode of SSI 0,1,2,9
    if( ssi_channel == SSI_CHANNEL_0 ||
        ssi_channel == SSI_CHANNEL_1 ||
        ssi_channel == SSI_CHANNEL_2 ||
        ssi_channel == SSI_CHANNEL_9 ) {
        SSIU_COMMON->mode2 &= ~SSI_MODE2_SSI0129FOURMODULE_MASK;
        SSIU_COMMON->mode2 |= SSI_MODE2_SSI0129FOURMODULE_DISABLE;
    }
    // to set up any of SSI 0,1,2 as an independent transfer channel, disable the
    // three channel mode of SSI 0,1,2
    if( ssi_channel == SSI_CHANNEL_0 ||
        ssi_channel == SSI_CHANNEL_1 ||
        ssi_channel == SSI_CHANNEL_2 ) {
        SSIU_COMMON->mode1 &= ~SSI_MODE1_SSI012THREEMODULE_MASK;
        SSIU_COMMON->mode1 |= SSI_MODE1_SSI012THREEMODULE_DISABLE;
    }
    // SSI1PIN: to set up any of SSI 0,1 as an independent channel,
    // disable the pin sharing mode of SSI 0,1
    if( ssi_channel == SSI_CHANNEL_0 ||
        ssi_channel == SSI_CHANNEL_1 ) {
        SSIU_COMMON->mode1 &= ~SSI_MODE1_SSI1PIN_MASK;
        SSIU_COMMON->mode1 |= SSI_MODE1_SSI1PIN_SSI01_INDEPENDENT;
    }
    // SSI2PIN: to set up any of SSI 0,2 as an independent channel,
    // disable the pin sharing mode of SSI 0,2
    if( ssi_channel == SSI_CHANNEL_0 ||
        ssi_channel == SSI_CHANNEL_2 ) {
        SSIU_COMMON->mode1 &= ~SSI_MODE1_SSI2PIN_MASK;
        SSIU_COMMON->mode1 |= SSI_MODE1_SSI2PIN_SSI02_INDEPENDENT;
    }
    // SSI9PIN - the story is a bit more complex here:
    // - to set up SSI 9 as an independent channel, disable the pin
    // sharing mode for both the SSI 0,9 and SSI 3,9 pairs;
    // - to set up SSI 0 as an independent channel, disable the pin
    // sharing mode for SSI 0,9 if currently set, don't alter the pin
    // sharing mode for SSI 3,9
    // - to set up SSI 3 as an independent channel, disable the pin
    // sharing mode for SSI 3,9 if currently set, don't alter the pin
    // sharing mode for SSI 0,9
    if( ssi_channel == SSI_CHANNEL_9 ) {
        SSIU_COMMON->mode2 &= ~SSI_MODE2_SSI9PIN_MASK;
        SSIU_COMMON->mode2 |= SSI_MODE2_SSI9PIN_SSI09_39_INDEPENDENT;
    } else if( ssi_channel == SSI_CHANNEL_0 ) {
        if( (SSIU_COMMON->mode2 & SSI_MODE2_SSI9PIN_MASK) ==
            SSI_MODE2_SSI9PIN_SSI09_BOTHSLAVE ||
            (SSIU_COMMON->mode2 & SSI_MODE2_SSI9PIN_MASK) ==
            SSI_MODE2_SSI9PIN_SSI09_MASTERSLAVE ) {
            SSIU_COMMON->mode2 &= ~SSI_MODE2_SSI9PIN_MASK;
            SSIU_COMMON->mode2 |= SSI_MODE2_SSI9PIN_SSI09_39_INDEPENDENT;
        }
    } else if( ssi_channel == SSI_CHANNEL_3 ) {
        if( (SSIU_COMMON->mode2 & SSI_MODE2_SSI9PIN_MASK) ==
            SSI_MODE2_SSI9PIN_SSI39_BOTHSLAVE ||
            (SSIU_COMMON->mode2 & SSI_MODE2_SSI9PIN_MASK) ==
            SSI_MODE2_SSI9PIN_SSI39_MASTERSLAVE ) {
            SSIU_COMMON->mode2 &= ~SSI_MODE2_SSI9PIN_MASK;
            SSIU_COMMON->mode2 |= SSI_MODE2_SSI9PIN_SSI09_39_INDEPENDENT;
        }
    }
    // SSI3PIN: to set up any of SSI 0,3 as an independent channel,
    // disable the pin sharing mode of SSI 0,3
    if( ssi_channel == SSI_CHANNEL_0 ||
        ssi_channel == SSI_CHANNEL_3 ) {
        SSIU_COMMON->mode3 &= ~SSI_MODE3_SSI3PIN_MASK;
        SSIU_COMMON->mode3 |= SSI_MODE3_SSI3PIN_SSI03_INDEPENDENT;
    }
    // SSI4PIN: to set up any of SSI 3,4 as an independent channel,
    // disable the pin sharing mode and the sync mode of SSI 3,4
    if( ssi_channel == SSI_CHANNEL_3 ||
        ssi_channel == SSI_CHANNEL_4 ) {
        SSIU_COMMON->mode1 &= ~(SSI_MODE1_SSI4PIN_MASK|SSI_MODE1_SSI34SYNC_MASK);
        SSIU_COMMON->mode1 |= SSI_MODE1_SSI4PIN_SSI34_INDEPENDENT |
                              SSI_MODE1_SSI34SYNC_DISABLE;
    }

    return EOK;
}

static int ssiu_clear_mode_1channel(uint32_t ssi_channel)
{
    if( !SSIU_COMMON ) {
        return EFAULT;
    }

    SSIU_COMMON->mode0 &= ~SSI_MODE0_IND_MASK(ssi_channel);

    return EOK;
}

static int ssiu_set_mode_2channel
(
    uint32_t ssi_channel1,
    uint32_t ssi_channel2,
    ssi_masterslave_mode_t masterslave_mode,
    ssi_transfer_mode_t transfer_mode,
    ssi_start_mode_t start_mode
)
{
    if( !rcar_ssi_supported(ssi_channel1) ) {
        ado_error_fmt("SSI%d is not supported", ssi_channel1);
        return EINVAL;
    }
    if( !rcar_ssi_supported(ssi_channel2) ) {
        ado_error_fmt("SSI%d is not supported", ssi_channel2);
        return EINVAL;
    }
    if( !SSIU_COMMON ) {
        return EFAULT;
    }

    // MODE0

    // enable or disable the independent mode for the given SSI channels
    SSIU_COMMON->mode0 &= ~(SSI_MODE0_IND_MASK(ssi_channel1) | SSI_MODE0_IND_MASK(ssi_channel2));
    if( transfer_mode == SSI_INDEPENDENT_TRANSFER ){
        SSIU_COMMON->mode0 |= ( SSI_MODE0_IND_ENABLE(ssi_channel1) |
                                SSI_MODE0_IND_ENABLE(ssi_channel2) );
    } else {
        SSIU_COMMON->mode0 |= ( SSI_MODE0_IND_DISABLE(ssi_channel1) |
                                SSI_MODE0_IND_DISABLE(ssi_channel2) );
    }

    //MODE1, MODE2, MODE3

    // if any of SSI 0,1,2,9 are configured in two channel mode, disable
    // the SSI 0,1,2,9 four channel mode
    if( ssi_channel1 == SSI_CHANNEL_0 || ssi_channel1 == SSI_CHANNEL_1 ||
        ssi_channel1 == SSI_CHANNEL_2 || ssi_channel1 == SSI_CHANNEL_9 ||
        ssi_channel2 == SSI_CHANNEL_0 || ssi_channel2 == SSI_CHANNEL_1 ||
        ssi_channel2 == SSI_CHANNEL_2 || ssi_channel2 == SSI_CHANNEL_9 ) {
        SSIU_COMMON->mode2 &= ~SSI_MODE2_SSI0129FOURMODULE_MASK;
        SSIU_COMMON->mode2 |= SSI_MODE2_SSI0129FOURMODULE_DISABLE;
    }
    // if any of SSI 0,1,2 are configured in two channel mode, disable
    // the SSI 0,1,2 three channel mode
    if( ssi_channel1 == SSI_CHANNEL_0 || ssi_channel1 == SSI_CHANNEL_1 ||
        ssi_channel1 == SSI_CHANNEL_2 || ssi_channel2 == SSI_CHANNEL_0 ||
        ssi_channel2 == SSI_CHANNEL_1 || ssi_channel2 == SSI_CHANNEL_2 ) {
        SSIU_COMMON->mode1 &= ~SSI_MODE1_SSI012THREEMODULE_MASK;
        SSIU_COMMON->mode1 |= SSI_MODE1_SSI012THREEMODULE_DISABLE;
    }
    // if SSI 0,1 are configured in two channel mode, enable the pin sharing for
    // SSI 0,1 and disable the pin sharing for SSI 0,2, SSI 0,3, SSI 0,9
    if( ( ssi_channel1 == SSI_CHANNEL_0 && ssi_channel2 == SSI_CHANNEL_1 ) ||
        ( ssi_channel1 == SSI_CHANNEL_1 && ssi_channel2 == SSI_CHANNEL_0 ) ) {
        SSIU_COMMON->mode1 &= ~(SSI_MODE1_SSI1PIN_MASK|SSI_MODE1_SSI2PIN_MASK);
        if( masterslave_mode == SSI_MASTER_SLAVE ) {
            SSIU_COMMON->mode1 |= SSI_MODE1_SSI1PIN_SSI01_MASTERSLAVE |
                                  SSI_MODE1_SSI2PIN_SSI02_INDEPENDENT;
        } else {
            SSIU_COMMON->mode1 |= SSI_MODE1_SSI1PIN_SSI01_BOTHSLAVE |
                                  SSI_MODE1_SSI2PIN_SSI02_INDEPENDENT;;
        }
        if( (SSIU_COMMON->mode2 & SSI_MODE2_SSI9PIN_MASK) ==
            SSI_MODE2_SSI9PIN_SSI09_BOTHSLAVE ||
            (SSIU_COMMON->mode2 & SSI_MODE2_SSI9PIN_MASK) ==
            SSI_MODE2_SSI9PIN_SSI09_MASTERSLAVE ) {
            SSIU_COMMON->mode2 &= ~SSI_MODE2_SSI9PIN_MASK;
            SSIU_COMMON->mode2 |= SSI_MODE2_SSI9PIN_SSI09_39_INDEPENDENT;
        }
        SSIU_COMMON->mode3 &= ~SSI_MODE3_SSI3PIN_MASK;
        SSIU_COMMON->mode3 |= SSI_MODE3_SSI3PIN_SSI03_INDEPENDENT;
    // if SSI 0,2 are configured in two channel mode, enable the pin sharing for
    // SSI 0,2 and disable the pin sharing for SSI 0,2, SSI 0,3, SSI 0,9
    } else if ( ( ssi_channel1 == SSI_CHANNEL_0 && ssi_channel2 == SSI_CHANNEL_2 ) ||
                ( ssi_channel1 == SSI_CHANNEL_2 && ssi_channel2 == SSI_CHANNEL_0 ) ) {
        SSIU_COMMON->mode1 &= ~(SSI_MODE1_SSI1PIN_MASK|SSI_MODE1_SSI2PIN_MASK);
        if( masterslave_mode == SSI_MASTER_SLAVE ) {
            SSIU_COMMON->mode1 |= SSI_MODE1_SSI2PIN_SSI02_MASTERSLAVE |
                                  SSI_MODE1_SSI1PIN_SSI01_INDEPENDENT;
        } else {
            SSIU_COMMON->mode1 |= SSI_MODE1_SSI2PIN_SSI02_BOTHSLAVE |
                                  SSI_MODE1_SSI1PIN_SSI01_INDEPENDENT;
        }
        if( (SSIU_COMMON->mode2 & SSI_MODE2_SSI9PIN_MASK) ==
            SSI_MODE2_SSI9PIN_SSI09_BOTHSLAVE ||
            (SSIU_COMMON->mode2 & SSI_MODE2_SSI9PIN_MASK) ==
            SSI_MODE2_SSI9PIN_SSI09_MASTERSLAVE ) {
            SSIU_COMMON->mode2 &= ~SSI_MODE2_SSI9PIN_MASK;
            SSIU_COMMON->mode2 |= SSI_MODE2_SSI9PIN_SSI09_39_INDEPENDENT;
        }
        SSIU_COMMON->mode3 &= ~SSI_MODE3_SSI3PIN_MASK;
        SSIU_COMMON->mode3 |= SSI_MODE3_SSI3PIN_SSI03_INDEPENDENT;
    // if SSI 0,3 are configured in two channel mode, enable the pin sharing for
    // SSI 0,3 and disable the pin sharing for SSI 0,1, SSI 0,2, SSI 0,9, SSI 3,9
    // and SSI 3,4
    } else if ( ( ssi_channel1 == SSI_CHANNEL_0 && ssi_channel2 == SSI_CHANNEL_3 ) ||
                ( ssi_channel1 == SSI_CHANNEL_3 && ssi_channel2 == SSI_CHANNEL_0 ) ) {
        SSIU_COMMON->mode1 &= ~(SSI_MODE1_SSI1PIN_MASK|SSI_MODE1_SSI2PIN_MASK|
                                SSI_MODE1_SSI4PIN_MASK|SSI_MODE1_SSI34SYNC_MASK);
        SSIU_COMMON->mode1 |= SSI_MODE1_SSI2PIN_SSI02_INDEPENDENT |
                              SSI_MODE1_SSI1PIN_SSI01_INDEPENDENT |
                              SSI_MODE1_SSI4PIN_SSI34_INDEPENDENT |
                              SSI_MODE1_SSI34SYNC_DISABLE;
        SSIU_COMMON->mode2 &= ~SSI_MODE2_SSI9PIN_MASK;
        SSIU_COMMON->mode2 |= SSI_MODE2_SSI9PIN_SSI09_39_INDEPENDENT;
        SSIU_COMMON->mode3 &= ~SSI_MODE3_SSI3PIN_MASK;
        if( masterslave_mode == SSI_MASTER_SLAVE ) {
            SSIU_COMMON->mode3 |= SSI_MODE3_SSI3PIN_SSI03_MASTERSLAVE;
        } else {
            SSIU_COMMON->mode3 |= SSI_MODE3_SSI3PIN_SSI03_BOTHSLAVE;
        }
    // if SSI 0,9 are configured in two channel mode, enable the pin sharing for
    // SSI 0,9 and disable the pin sharing for SSI 0,1, SSI 0,2, SSI 0,3, SSI 3,9
    } else if ( ( ssi_channel1 == SSI_CHANNEL_0 && ssi_channel2 == SSI_CHANNEL_9 ) ||
                ( ssi_channel1 == SSI_CHANNEL_9 && ssi_channel2 == SSI_CHANNEL_0 ) ) {
        SSIU_COMMON->mode1 &= ~(SSI_MODE1_SSI1PIN_MASK|SSI_MODE1_SSI2PIN_MASK);
        SSIU_COMMON->mode1 |= SSI_MODE1_SSI2PIN_SSI02_INDEPENDENT |
                              SSI_MODE1_SSI1PIN_SSI01_INDEPENDENT;
        SSIU_COMMON->mode2 &= ~SSI_MODE2_SSI9PIN_MASK;
        if( masterslave_mode == SSI_MASTER_SLAVE ) {
            SSIU_COMMON->mode2 |= SSI_MODE2_SSI9PIN_SSI09_MASTERSLAVE;
        } else {
            SSIU_COMMON->mode2 |= SSI_MODE2_SSI9PIN_SSI09_BOTHSLAVE;
        }
        SSIU_COMMON->mode3 &= ~SSI_MODE3_SSI3PIN_MASK;
        SSIU_COMMON->mode3 |= SSI_MODE3_SSI3PIN_SSI03_INDEPENDENT;
    // if SSI 3,9 are configured in two channel mode, enable the pin sharing for
    // SSI 3,9 and disable the pin sharing for SSI 0,3, SSI 0,9, SSI 3,4
    } else if ( ( ssi_channel1 == SSI_CHANNEL_3 && ssi_channel2 == SSI_CHANNEL_9 ) ||
                ( ssi_channel1 == SSI_CHANNEL_9 && ssi_channel2 == SSI_CHANNEL_3 ) ) {
        SSIU_COMMON->mode1 &= ~(SSI_MODE1_SSI4PIN_MASK|SSI_MODE1_SSI34SYNC_MASK);
        SSIU_COMMON->mode1 |= SSI_MODE1_SSI4PIN_SSI34_INDEPENDENT |
                              SSI_MODE1_SSI34SYNC_DISABLE;
        SSIU_COMMON->mode2 &= ~SSI_MODE2_SSI9PIN_MASK;
        if( masterslave_mode == SSI_MASTER_SLAVE ) {
            SSIU_COMMON->mode2 |= SSI_MODE2_SSI9PIN_SSI39_MASTERSLAVE;
        } else {
            SSIU_COMMON->mode2 |= SSI_MODE2_SSI9PIN_SSI39_BOTHSLAVE;
        }
        SSIU_COMMON->mode3 &= ~SSI_MODE3_SSI3PIN_MASK;
        SSIU_COMMON->mode3 |= SSI_MODE3_SSI3PIN_SSI03_INDEPENDENT;
    // if SSI 3,4 are configured in two channel mode, enable the pin sharing for
    // SSI 3,4 and disable the pin sharing for SSI 0,3, SSI 3,9, while leaving
    // untouched the pin sharing for SSI 0,9
    } else if ( ( ssi_channel1 == SSI_CHANNEL_3 && ssi_channel2 == SSI_CHANNEL_4 ) ||
                ( ssi_channel1 == SSI_CHANNEL_4 && ssi_channel2 == SSI_CHANNEL_3 ) ) {
        SSIU_COMMON->mode1 &= ~(SSI_MODE1_SSI4PIN_MASK|SSI_MODE1_SSI34SYNC_MASK);
        if( masterslave_mode == SSI_MASTER_SLAVE ) {
             SSIU_COMMON->mode1 |= SSI_MODE1_SSI4PIN_SSI34_MASTERSLAVE;
        } else {
             SSIU_COMMON->mode1 |= SSI_MODE1_SSI4PIN_SSI34_BOTHSLAVE;
        }
        if( start_mode == SSI_SYNC_SSI34_START ) {
             SSIU_COMMON->mode1 |= SSI_MODE1_SSI34SYNC_ENABLE;
        }
        if( (SSIU_COMMON->mode2 & SSI_MODE2_SSI9PIN_MASK) ==
            SSI_MODE2_SSI9PIN_SSI39_BOTHSLAVE ||
            (SSIU_COMMON->mode2 & SSI_MODE2_SSI9PIN_MASK) ==
            SSI_MODE2_SSI9PIN_SSI39_MASTERSLAVE ) {
            SSIU_COMMON->mode2 &= ~SSI_MODE2_SSI9PIN_MASK;
            SSIU_COMMON->mode2 |= SSI_MODE2_SSI9PIN_SSI09_39_INDEPENDENT;
        }
        SSIU_COMMON->mode3 &= ~SSI_MODE3_SSI3PIN_MASK;
        SSIU_COMMON->mode3 |= SSI_MODE3_SSI3PIN_SSI03_INDEPENDENT;
    } else {
        return EINVAL;
    }

    return EOK;
}

static int ssiu_clear_mode_2channel
(
    uint32_t ssi_channel1,
    uint32_t ssi_channel2
)
{
    if( !SSIU_COMMON ) {
        return EFAULT;
    }

    //MODE0 - disable independent transfer bits for both ssi channels
    SSIU_COMMON->mode0 &= ~(SSI_MODE0_IND_MASK(ssi_channel1) | SSI_MODE0_IND_MASK(ssi_channel2));

    //MODE1,2,3 - clear applicable pin sharing mask, and the sync3,4 mask for SSI 3,4
    if( ( ssi_channel1 == SSI_CHANNEL_0 && ssi_channel2 == SSI_CHANNEL_1 ) ||
        ( ssi_channel1 == SSI_CHANNEL_1 && ssi_channel2 == SSI_CHANNEL_0 ) ) {
        SSIU_COMMON->mode1 &= ~SSI_MODE1_SSI1PIN_MASK;
    } else if( ( ssi_channel1 == SSI_CHANNEL_0 && ssi_channel2 == SSI_CHANNEL_2 ) ||
               ( ssi_channel1 == SSI_CHANNEL_2 && ssi_channel2 == SSI_CHANNEL_0 ) ) {
        SSIU_COMMON->mode1 &= ~SSI_MODE1_SSI2PIN_MASK;
    } else if( ( ssi_channel1 == SSI_CHANNEL_0 && ssi_channel2 == SSI_CHANNEL_3 ) ||
               ( ssi_channel1 == SSI_CHANNEL_3 && ssi_channel2 == SSI_CHANNEL_0 ) ) {
        SSIU_COMMON->mode3 &= ~SSI_MODE3_SSI3PIN_MASK;
    } else if( ( ssi_channel1 == SSI_CHANNEL_0 && ssi_channel2 == SSI_CHANNEL_9 ) ||
               ( ssi_channel1 == SSI_CHANNEL_9 && ssi_channel2 == SSI_CHANNEL_0 ) ) {
        SSIU_COMMON->mode2 &= ~SSI_MODE2_SSI9PIN_MASK;
    } else if( ( ssi_channel1 == SSI_CHANNEL_3 && ssi_channel2 == SSI_CHANNEL_9 ) ||
               ( ssi_channel1 == SSI_CHANNEL_9 && ssi_channel2 == SSI_CHANNEL_3 ) ) {
        SSIU_COMMON->mode2 &= ~SSI_MODE2_SSI9PIN_MASK;
    } else if( ( ssi_channel1 == SSI_CHANNEL_3 && ssi_channel2 == SSI_CHANNEL_4 ) ||
               ( ssi_channel1 == SSI_CHANNEL_4 && ssi_channel2 == SSI_CHANNEL_3 ) ) {
        SSIU_COMMON->mode1 &= ~(SSI_MODE1_SSI4PIN_MASK|SSI_MODE1_SSI34SYNC_MASK);
    }

    return EOK;
}

static int ssiu_set_mode_3channel(ssi_masterslave_mode_t masterslave_mode)
{
    if( !SSIU_COMMON ) {
        return EFAULT;
    }

    // MODE0

    // disable the independent mode for SSI channels 0,1,2
    SSIU_COMMON->mode0 &= ~( SSI_MODE0_IND_MASK(SSI_CHANNEL_0) |
                             SSI_MODE0_IND_MASK(SSI_CHANNEL_1) |
                             SSI_MODE0_IND_MASK(SSI_CHANNEL_2) );
    SSIU_COMMON->mode0 |= SSI_MODE0_IND_DISABLE(SSI_CHANNEL_0) |
                          SSI_MODE0_IND_DISABLE(SSI_CHANNEL_1) |
                          SSI_MODE0_IND_DISABLE(SSI_CHANNEL_2);

    //MODE1, MODE2, MODE3

    // enable the SSI 0,1,2 three channel mode and the pin sharing mode
    // for SSI 0,1 and SSI 0,2
    SSIU_COMMON->mode1 &= ~( SSI_MODE1_SSI012THREEMODULE_MASK |
                             SSI_MODE1_SSI1PIN_MASK|
                             SSI_MODE1_SSI2PIN_MASK );
    if( masterslave_mode == SSI_MASTER_SLAVE ) {
        SSIU_COMMON->mode1 |= SSI_MODE1_SSI012THREEMODULE_ENABLE |
                              SSI_MODE1_SSI2PIN_SSI02_MASTERSLAVE |
                              SSI_MODE1_SSI1PIN_SSI01_MASTERSLAVE;
    } else {
        SSIU_COMMON->mode1 |= SSI_MODE1_SSI012THREEMODULE_ENABLE |
                              SSI_MODE1_SSI2PIN_SSI02_BOTHSLAVE |
                              SSI_MODE1_SSI1PIN_SSI01_BOTHSLAVE;
    }

    // disable the SSI 0,1,2,9 four channel mode
    SSIU_COMMON->mode2 &= ~SSI_MODE2_SSI0129FOURMODULE_MASK;
    SSIU_COMMON->mode2 |= SSI_MODE2_SSI0129FOURMODULE_DISABLE;

    // disable the pin sharing mode for SSI 0,9
    if( (SSIU_COMMON->mode2 & SSI_MODE2_SSI9PIN_MASK) ==
        SSI_MODE2_SSI9PIN_SSI09_BOTHSLAVE ||
        (SSIU_COMMON->mode2 & SSI_MODE2_SSI9PIN_MASK) ==
        SSI_MODE2_SSI9PIN_SSI09_MASTERSLAVE ) {
        SSIU_COMMON->mode2 &= ~SSI_MODE2_SSI9PIN_MASK;
        SSIU_COMMON->mode2 |= SSI_MODE2_SSI9PIN_SSI09_39_INDEPENDENT;
    }

    // disable the pin sharing for SSI 0,3
    SSIU_COMMON->mode3 &= ~SSI_MODE3_SSI3PIN_MASK;
    SSIU_COMMON->mode3 |= SSI_MODE3_SSI3PIN_SSI03_INDEPENDENT;

    return EOK;
}

static int ssiu_clear_mode_3channel()
{
    if( !SSIU_COMMON ) {
        return EFAULT;
    }

    //MODE0 - leave unchanged - independent mode disabled for SSI 0,1,2 is the default

    //MODE1 - clear the three module bit and the pin1 and pin2 masks
    SSIU_COMMON->mode1 &= ~( SSI_MODE1_SSI012THREEMODULE_MASK |
                             SSI_MODE1_SSI1PIN_MASK|
                             SSI_MODE1_SSI2PIN_MASK );

    //MODE2, MODE3 - leave unchanged

    return EOK;
}

static int ssiu_set_mode_4channel(ssi_masterslave_mode_t masterslave_mode)
{
    if( !SSIU_COMMON ) {
        return EFAULT;
    }

    // MODE0

    // disable the independent mode for SSI channels 0,1,2,9
    SSIU_COMMON->mode0 &= ~( SSI_MODE0_IND_MASK(SSI_CHANNEL_0) |
                             SSI_MODE0_IND_MASK(SSI_CHANNEL_1) |
                             SSI_MODE0_IND_MASK(SSI_CHANNEL_2) |
                             SSI_MODE0_IND_MASK(SSI_CHANNEL_9) );
    SSIU_COMMON->mode0 |= SSI_MODE0_IND_DISABLE(SSI_CHANNEL_0) |
                          SSI_MODE0_IND_DISABLE(SSI_CHANNEL_1) |
                          SSI_MODE0_IND_DISABLE(SSI_CHANNEL_2) |
                          SSI_MODE0_IND_DISABLE(SSI_CHANNEL_9);

    //MODE1, MODE2, MODE3

    // disable the SSI 0,1,2 three channel mode and enable the pin sharing mode
    // for SSI 0,1 and SSI 0,2
    SSIU_COMMON->mode1 &= ~( SSI_MODE1_SSI012THREEMODULE_MASK |
                             SSI_MODE1_SSI1PIN_MASK |
                             SSI_MODE1_SSI2PIN_MASK );
    if( masterslave_mode == SSI_MASTER_SLAVE ) {
        SSIU_COMMON->mode1 |= SSI_MODE1_SSI012THREEMODULE_DISABLE |
                              SSI_MODE1_SSI2PIN_SSI02_MASTERSLAVE |
                              SSI_MODE1_SSI1PIN_SSI01_MASTERSLAVE;
    } else {
        SSIU_COMMON->mode1 |= SSI_MODE1_SSI012THREEMODULE_DISABLE |
                              SSI_MODE1_SSI2PIN_SSI02_BOTHSLAVE |
                              SSI_MODE1_SSI1PIN_SSI01_BOTHSLAVE;
    }

    // enable the SSI 0,1,2,9 four channel mode and enable the
    // pin sharing mode for SSI 0,9, disable pin sharing for SSI 3,9
    SSIU_COMMON->mode2 &= ~(SSI_MODE2_SSI0129FOURMODULE_MASK | SSI_MODE2_SSI9PIN_MASK);
    if( masterslave_mode == SSI_MASTER_SLAVE ) {
        SSIU_COMMON->mode2 |= SSI_MODE2_SSI0129FOURMODULE_ENABLE |
                              SSI_MODE2_SSI9PIN_SSI09_MASTERSLAVE;
    } else {
        SSIU_COMMON->mode2 |= SSI_MODE2_SSI0129FOURMODULE_ENABLE |
                              SSI_MODE2_SSI9PIN_SSI09_BOTHSLAVE;
    }

    // disable the pin sharing for SSI 0,3
    SSIU_COMMON->mode3 &= ~SSI_MODE3_SSI3PIN_MASK;
    SSIU_COMMON->mode3 |= SSI_MODE3_SSI3PIN_SSI03_INDEPENDENT;

    return EOK;
}

static int ssiu_clear_mode_4channel()
{
    if( !SSIU_COMMON ) {
        return EFAULT;
    }

    //MODE0 - leave unchanged - independent mode disabled for SSI 0,1,2,9 is the default

    //MODE1 - clear pin1 and pin2 masks
    SSIU_COMMON->mode1 &= ~( SSI_MODE1_SSI1PIN_MASK | SSI_MODE1_SSI2PIN_MASK );
    //MODE2 - clear the four module bit and the pin9 mask
    SSIU_COMMON->mode2 &= ~(SSI_MODE2_SSI0129FOURMODULE_MASK | SSI_MODE2_SSI9PIN_MASK);

    //MODE3 - leave unchanged

    return EOK;
}

static int ssiu_set_hdmi_mode_1channel(uint32_t master_ssi_channel, uint32_t ssi_channel, uint32_t hdmi_channel)
{
    uint32_t hdmi_sel = SSI_HDMI_SCK_WS_SEL(master_ssi_channel)
                        | SSI_HDMI_SD0_SEL(ssi_channel)
                        | SSI_HDMI_SD1_SEL_MASK
                        | SSI_HDMI_SD2_SEL_MASK
                        | SSI_HDMI_SD3_SEL_MASK;
    if( !SSIU_COMMON_EXT ) {
        return EINVAL;
    }
    if( hdmi_channel == SSI_HDMI_CHANNEL_0 ) {
        SSIU_COMMON_EXT->hdmi0_sel = hdmi_sel;
    } else if( hdmi_channel == SSI_HDMI_CHANNEL_1 ) {
        SSIU_COMMON_EXT->hdmi1_sel = hdmi_sel;
    }
    return EOK;
}

#if 0
static int ssiu_set_hdmi_mode_2channel(uint32_t master_ssi_channel, uint32_t other_ssi_channel, uint32_t hdmi_channel)
{
    uint32_t hdmi_sel = SSI_HDMI_SCK_WS_SEL(master_ssi_channel)
                        | SSI_HDMI_SD0_SEL(master_ssi_channel)
                        | SSI_HDMI_SD1_SEL(other_ssi_channel)
                        | SSI_HDMI_SD2_SEL_MASK
                        | SSI_HDMI_SD3_SEL_MASK;
    if( !SSIU_COMMON_EXT ) {
        return EINVAL;
    }
    if( hdmi_channel == SSI_HDMI_CHANNEL_0 ) {
        SSIU_COMMON_EXT->hdmi0_sel = hdmi_sel;
    } else if( hdmi_channel == SSI_HDMI_CHANNEL_1 ) {
        SSIU_COMMON_EXT->hdmi1_sel = hdmi_sel;
    }
    return EOK;
}
#endif

static int ssiu_set_hdmi_mode_3channel(uint32_t hdmi_channel)
{
    // the three channels are SSI0,1,2
    uint32_t hdmi_sel = SSI_HDMI_SCK_WS_SEL(SSI_CHANNEL_0)
                        | SSI_HDMI_SD0_SEL(SSI_CHANNEL_0)
                        | SSI_HDMI_SD1_SEL(SSI_CHANNEL_1)
                        | SSI_HDMI_SD2_SEL(SSI_CHANNEL_2)
                        | SSI_HDMI_SD3_SEL_MASK;
    if( !SSIU_COMMON_EXT ) {
        return EINVAL;
    }
    if( hdmi_channel == SSI_HDMI_CHANNEL_0 ) {
        SSIU_COMMON_EXT->hdmi0_sel = hdmi_sel;
    } else if( hdmi_channel == SSI_HDMI_CHANNEL_1 ) {
        SSIU_COMMON_EXT->hdmi1_sel = hdmi_sel;
    }
    return EOK;
}

static int ssiu_set_hdmi_mode_4channel(uint32_t hdmi_channel)
{
    // the four channels are SSI0,1,2,9
    uint32_t hdmi_sel = SSI_HDMI_SCK_WS_SEL(SSI_CHANNEL_0)
                        | SSI_HDMI_SD0_SEL(SSI_CHANNEL_0)
                        | SSI_HDMI_SD1_SEL(SSI_CHANNEL_1)
                        | SSI_HDMI_SD2_SEL(SSI_CHANNEL_2)
                        | SSI_HDMI_SD3_SEL(SSI_CHANNEL_9);
    if( !SSIU_COMMON_EXT ) {
        return EINVAL;
    }
    if( hdmi_channel == SSI_HDMI_CHANNEL_0 ) {
        SSIU_COMMON_EXT->hdmi0_sel = hdmi_sel;
    } else if( hdmi_channel == SSI_HDMI_CHANNEL_1 ) {
        SSIU_COMMON_EXT->hdmi1_sel = hdmi_sel;
    }
    return EOK;
}

// make sure no HDMI channel selects the indicated ssi_channel
// as either clock and ws source or as serial data source
// if it does, clear the corresponding HDMIx_SEL register
static int ssiu_clear_hdmi_mode(uint32_t ssi_channel)
{
    uint32_t hdmi_sel_clear = SSI_HDMI_SCK_WS_SEL_MASK
                        | SSI_HDMI_SD0_SEL_MASK
                        | SSI_HDMI_SD1_SEL_MASK
                        | SSI_HDMI_SD2_SEL_MASK
                        | SSI_HDMI_SD3_SEL_MASK;

    if( !SSIU_COMMON_EXT ) {
        return EINVAL;
    }
    if( ( SSIU_COMMON_EXT->hdmi0_sel & SSI_HDMI_SD0_SEL_MASK ) ==
        SSI_HDMI_SD0_SEL(ssi_channel) ||
        ( SSIU_COMMON_EXT->hdmi0_sel & SSI_HDMI_SD1_SEL_MASK ) ==
        SSI_HDMI_SD1_SEL(ssi_channel) ||
        ( SSIU_COMMON_EXT->hdmi0_sel & SSI_HDMI_SD2_SEL_MASK ) ==
        SSI_HDMI_SD2_SEL(ssi_channel) ||
        ( SSIU_COMMON_EXT->hdmi0_sel & SSI_HDMI_SD3_SEL_MASK ) ==
        SSI_HDMI_SD3_SEL(ssi_channel) ||
        ( SSIU_COMMON_EXT->hdmi0_sel & SSI_HDMI_SCK_WS_SEL_MASK ) ==
        SSI_HDMI_SCK_WS_SEL(ssi_channel) ) {
        SSIU_COMMON_EXT->hdmi0_sel = hdmi_sel_clear;
    }

    if( ( SSIU_COMMON_EXT->hdmi1_sel & SSI_HDMI_SD0_SEL_MASK ) ==
        SSI_HDMI_SD0_SEL(ssi_channel) ||
        ( SSIU_COMMON_EXT->hdmi1_sel & SSI_HDMI_SD1_SEL_MASK ) ==
        SSI_HDMI_SD1_SEL(ssi_channel) ||
        ( SSIU_COMMON_EXT->hdmi1_sel & SSI_HDMI_SD2_SEL_MASK ) ==
        SSI_HDMI_SD2_SEL(ssi_channel) ||
        ( SSIU_COMMON_EXT->hdmi1_sel & SSI_HDMI_SD3_SEL_MASK ) ==
        SSI_HDMI_SD3_SEL(ssi_channel) ||
        ( SSIU_COMMON_EXT->hdmi1_sel & SSI_HDMI_SCK_WS_SEL_MASK ) ==
        SSI_HDMI_SCK_WS_SEL(ssi_channel) ) {
        SSIU_COMMON_EXT->hdmi1_sel = hdmi_sel_clear;
    }

    return EOK;
}

static int ssiu_set_ssi_mode(uint32_t ssi_channel, ssi_op_mode_t op_mode)
{
    if( !rcar_ssi_supported( ssi_channel ) ) {
        ado_error_fmt("SSI%d is not supported", ssi_channel);
        return EINVAL;
    }
    if( !SSIU_SSI(ssi_channel) ) {
        return EFAULT;
    }

    SSIU_SSI(ssi_channel)->mode = 0;
    if( SSIU_SSI2(ssi_channel) ) {
        SSIU_SSI2(ssi_channel)->mode2 = 0;
    }

    if( op_mode == SSI_OP_MODE_TDMEXT ) {
        SSIU_SSI(ssi_channel)->mode |= SSI_MODE_TDMEXT_ENABLE;
    } else if( op_mode == SSI_OP_MODE_TDMSPLIT_4XMONO ) {
        SSIU_SSI(ssi_channel)->mode |= SSI_MODE_TDMSPLIT_ENABLE | SSI_MODE_FSMODE_ENABLE;
    } else if( op_mode == SSI_OP_MODE_TDMSPLIT_4XSTEREO ) {
        SSIU_SSI(ssi_channel)->mode |= SSI_MODE_TDMSPLIT_ENABLE;
    } else if( op_mode == SSI_OP_MODE_TDMEXSPLIT_8CHAN ||
               op_mode == SSI_OP_MODE_TDMEXSPLIT_16CHAN ) {
        SSIU_SSI(ssi_channel)->mode |= SSI_MODE_TDMSPLIT_ENABLE;
        SSIU_SSI2(ssi_channel)->mode2 |= SSI_MODE2_EX_FUNC_ENABLE;
    } else if( op_mode == SSI_OP_MODE_TDM_16CHAN ) {
        SSIU_SSI(ssi_channel)->mode |= SSI_MODE_TDMEXT_ENABLE;
        SSIU_SSI2(ssi_channel)->mode2 |= SSI_MODE2_EX_FUNC_ENABLE;
    }
    return EOK;
}

static int ssiu_clear_ssi_mode(uint32_t ssi_channel)
{
    if( !rcar_ssi_supported( ssi_channel ) ) {
        ado_error_fmt("SSI%d is not supported", ssi_channel);
        return EINVAL;
    }
    if( !SSIU_SSI(ssi_channel) ) {
        return EFAULT;
    }

    SSIU_SSI(ssi_channel)->mode = 0;
    if( SSIU_SSI2(ssi_channel) ) {
        SSIU_SSI2(ssi_channel)->mode2 = 0;
    }

    return EOK;
}

static int ssiu_validate_tdm_exsplit_map( uint32_t num_subchan, uint32_t* tdm_exsplit_map ) {
    static uint32_t tdm_exsplit_max_voices_map[] = {10, 2, 4, 2, 8, 2, 4, 2};
    int i, subchan_to_skip = 0;

    for( i = 0; i < SSI_SUB_CHANNEL_NUM; i++ ) {
        // if this subchan must be skipped, check that the entry for it is 0
        if( subchan_to_skip ) {
            if( tdm_exsplit_map[i] != 0 ) {
                return EINVAL;
            }
            subchan_to_skip --;
            continue;
        }
        // check that the subchan entry is even
        if( ( (tdm_exsplit_map[i] >> 1) << 1) != tdm_exsplit_map[i] ) {
            return EINVAL;
        }
        // check that the subchan entry does not exceed the max for this subchan
        if( tdm_exsplit_map[i] > tdm_exsplit_max_voices_map[i] ) {
            return EINVAL;
        }

        // now we know that the entry is valid

        // if the entry for this subchan is greater than two, then the next subchan(s)
        // must be skipped, i.e. their entries must be 0; determine how many subchans to skip
        subchan_to_skip = (tdm_exsplit_map[i] >> 1) - 1;
    }
    return EOK;
}

int ssiu_set_ssi_busif(uint32_t ssi_channel, ssi_op_mode_t op_mode, uint32_t voicenum, uint32_t bitnum, uint32_t* tdm_exsplit_map)
{
    uint32_t otbl, chnum;
    uint32_t num_subchan = 1;
    uint32_t i;
    int status = EOK;

    if( !rcar_ssi_supported( ssi_channel ) ) {
        ado_error_fmt("SSI%d is not supported", ssi_channel);
        return EINVAL;
    }

    switch(bitnum) {
        case 8: otbl = SSI_BUSIF_ADINR_OTBL_8BITS; break;
        case 16: otbl = SSI_BUSIF_ADINR_OTBL_16BITS; break;
        case 18: otbl = SSI_BUSIF_ADINR_OTBL_18BITS; break;
        case 20: otbl = SSI_BUSIF_ADINR_OTBL_20BITS; break;
        case 22: otbl = SSI_BUSIF_ADINR_OTBL_22BITS; break;
        case 24: otbl = SSI_BUSIF_ADINR_OTBL_24BITS; break;
        default: return EINVAL;
    }
    switch(voicenum) {
        case 0: chnum = SSI_BUSIF_ADINR_CHNUM_NONE; break;
        case 1: chnum = SSI_BUSIF_ADINR_CHNUM_ONE; break;
        case 2: chnum = SSI_BUSIF_ADINR_CHNUM_TWO; break;
        case 4: chnum = SSI_BUSIF_ADINR_CHNUM_FOUR; break;
        case 6: chnum = SSI_BUSIF_ADINR_CHNUM_SIX; break;
        case 8: chnum = SSI_BUSIF_ADINR_CHNUM_EIGHT; break;
        case 16: chnum = SSI_BUSIF_ADINR_CHNUM_SIXTEEN; break;
        default: return EINVAL;
    }

    // validate bitnum for all modes except for the simple modes
    if( op_mode == SSI_OP_MODE_TDMEXSPLIT_8CHAN ||
        op_mode == SSI_OP_MODE_TDMEXSPLIT_16CHAN ) {
        if( bitnum != 16 ) {
            return EINVAL;
        }
        if( tdm_exsplit_map == NULL ) {
            return EINVAL;
        }
    } else if( op_mode != SSI_OP_MODE_MONO && op_mode != SSI_OP_MODE_STEREO &&
        op_mode != SSI_OP_MODE_MULTICH ) {
        if( bitnum != 16 && bitnum != 24 ) {
            return EINVAL;
        }
    }
    // force chnum to 2 for TDM split modes
    if( op_mode == SSI_OP_MODE_TDMSPLIT_4XMONO || op_mode == SSI_OP_MODE_TDMSPLIT_4XSTEREO ) {
        chnum = SSI_BUSIF_ADINR_CHNUM_TWO;
        num_subchan = 4;
    } else if( op_mode == SSI_OP_MODE_TDMEXSPLIT_8CHAN ) {
        // chnum can be different for each subchannel
        num_subchan = 4;
    } else if( op_mode == SSI_OP_MODE_TDMEXSPLIT_16CHAN ) {
        // chnum can be different for each subchannel
        num_subchan = 8;
    }

    if( tdm_exsplit_map && ssiu_validate_tdm_exsplit_map( num_subchan, tdm_exsplit_map ) != EOK ) {
        return EINVAL;
    }

    for( i = 0; i < num_subchan; i++ ) {

        if( !SSIU_BUSIF(ssi_channel, i) ) {
            status = EFAULT;
            break;
        }
        if( op_mode == SSI_OP_MODE_TDMEXSPLIT_8CHAN ||
            op_mode == SSI_OP_MODE_TDMEXSPLIT_16CHAN ) {
            chnum = tdm_exsplit_map[i];
        }
        SSIU_BUSIF(ssi_channel, i)->busif_adinr &= ~( SSI_BUSIF_ADINR_OTBL_MASK |
                                                      SSI_BUSIF_ADINR_CHNUM_MASK );
        SSIU_BUSIF(ssi_channel, i)->busif_adinr |= ( otbl | chnum );
    }
    return status;
}

int ssiu_clear_ssi_busif( uint32_t ssi_channel )
{
    // placeholder - not sure at this point if anything needs to be done here
    uint32_t min_subchan, max_subchan;
    int i;
    int status;

    status = rcar_ssi_subchan_get_supported_range( ssi_channel, &min_subchan, &max_subchan );

    if( status != EOK ) return status;

    for( i = min_subchan; i <= max_subchan; i++ ) {

        if( !SSIU_BUSIF( ssi_channel, i ) ) {
            status = EFAULT;
            break;
        }

        SSIU_BUSIF(ssi_channel, i)->busif_adinr &= ~( SSI_BUSIF_ADINR_OTBL_MASK |
                                                      SSI_BUSIF_ADINR_CHNUM_MASK );
    }

    return status;
}

/* Public SSIU level functions */

int ssiu_init( void )
{
    return ssiu_mem_map();
}

int ssiu_deinit(void)
{
    return ssiu_mem_unmap();
}

int ssiu_1channel_setup
(
    uint32_t ssi_channel,
    uint32_t master_ssi_channel,
    uint32_t is_transmit,
    ssi_op_mode_t op_mode,
    ssi_transfer_mode_t transfer_mode,
    uint32_t bitnum,
    uint32_t voicenum,
    uint32_t voicenum_ext,
    uint32_t hdmi_channel,
    ssi_config_t* config,
    uint32_t* tdm_exsplit_map
)
{
    ssi_masterslave_mode_t masterslave_mode = SSI_ALL_SLAVE;
    uint32_t chn_is_master = 0;
    int status;

    if( !rcar_ssi_supported( ssi_channel ) ) {
        ado_error_fmt("SSI%d is not supported", ssi_channel);
        return EINVAL;
    }
    if( !rcar_ssi_supported( master_ssi_channel ) && master_ssi_channel != SSI_CHANNEL_NUM ) {
        ado_error_fmt("SSI%d is not supported", ssi_channel);
        return EINVAL;
    }
    if( master_ssi_channel == SSI_CHANNEL_8 ) {
        ado_error_fmt("SSI8 is not supported in master mode");
        return EINVAL;
    }
    if( master_ssi_channel == SSI_CHANNEL_NUM && hdmi_channel != SSI_HDMI_CHANNEL_NUM) {
        ado_error_fmt("HDMI output is only supported in master mode");
        return EINVAL;
    }
    if( ssi_channel != master_ssi_channel && master_ssi_channel != SSI_CHANNEL_NUM &&
         master_ssi_channel != SSI_CHANNEL_0 && master_ssi_channel != SSI_CHANNEL_3 ) {
        ado_error_fmt("SSI%d is not supported in master mode with pin sharing", ssi_channel);
        return EINVAL;
    }
    if( op_mode == SSI_OP_MODE_TDM ) {
        if( !rcar_ssi_tdm_supported( ssi_channel) ) {
            ado_error_fmt("SSI%d is not supported in TDM mode", ssi_channel);
            return EINVAL;
        }
        if( !rcar_ssi_tdm_voices_supported( voicenum ) ) {
            ado_error_fmt("%d voices not supported in TDM mode", voicenum);
            return EINVAL;
        }
        if( voicenum == 16 && bitnum != 16 ) {
            ado_error_fmt("bitnum %d not supported in 16 channel TDM mode", voicenum);
            return EINVAL;
        } else if( bitnum != 24 ) {
            ado_error_fmt("bitnum %d is not supported in TDM mode", bitnum);
            return EINVAL;
        }
    }
    if( op_mode == SSI_OP_MODE_TDMEXT ) {
        if( !rcar_ssi_tdmext_supported( ssi_channel) ) {
            ado_error_fmt("SSI%d is not supported in TDM mode", ssi_channel);
            return EINVAL;
        }
    }
    if( op_mode == SSI_OP_MODE_TDMSPLIT_4XMONO || op_mode == SSI_OP_MODE_TDMSPLIT_4XSTEREO ) {
        if( !rcar_ssi_tdmsplit_supported( ssi_channel ) ) {
            ado_error_fmt("SSI%d is not supported in TDM split mode", ssi_channel);
            return EINVAL;
        }
    }
    if( op_mode == SSI_OP_MODE_TDMEXSPLIT_8CHAN || op_mode == SSI_OP_MODE_TDMEXSPLIT_16CHAN ) {
        if( !rcar_ssi_tdmexsplit_supported( ssi_channel ) ) {
            ado_error_fmt("SSI%d is not supported in TDM exsplit mode", ssi_channel);
            return EINVAL;
        }
        if( bitnum != 16 ) {
            ado_error_fmt("bitnum %d is not supported in TDM exsplit mode", bitnum);
            return EINVAL;
        }
    }
    if( op_mode == SSI_OP_MODE_TDMEXT ||
        op_mode == SSI_OP_MODE_TDMSPLIT_4XMONO || op_mode == SSI_OP_MODE_TDMSPLIT_4XSTEREO ) {
        if( bitnum != 16 && bitnum != 24 ) {
            ado_error_fmt("bitnum %d is not supported in TDM mode", bitnum);
            return EINVAL;
        }
        if( voicenum <= 2 ) {
            ado_error_fmt("extended and split TDM modes not supported for voicenum=%d", voicenum);
            return EINVAL;
        }
    }

    if( master_ssi_channel != SSI_CHANNEL_NUM ) {
        masterslave_mode = SSI_MASTER_SLAVE;
    }

    do {
        chn_is_master = (masterslave_mode == SSI_MASTER_SLAVE && ssi_channel == master_ssi_channel ? 1 : 0);
        status = ssi_set_control_and_mode( ssi_channel, chn_is_master, is_transmit, op_mode, voicenum_ext, config );

        if( status != EOK ) break;

        if( masterslave_mode == SSI_MASTER_SLAVE && ssi_channel != master_ssi_channel ) {
            status = ssi_set_control_and_mode( master_ssi_channel, 1, is_transmit, op_mode, voicenum_ext, config );

            if( status != EOK ) break;

            ssiu_set_mode_2channel( master_ssi_channel, ssi_channel, masterslave_mode, transfer_mode, SSI_INDEPENDENT_START );

            if( status != EOK ) break;
        } else {
            status = ssiu_set_mode_1channel( ssi_channel, transfer_mode );

            if( status != EOK ) break;
        }

        status = ssiu_set_ssi_mode(ssi_channel, op_mode);

        if( status != EOK ) break;

        if( transfer_mode == SSI_BUSIF_TRANSFER ) {
            status = ssiu_set_ssi_busif( ssi_channel, op_mode, voicenum, bitnum, tdm_exsplit_map );
        }

        if( status != EOK ) break;

        if( hdmi_channel != SSI_HDMI_CHANNEL_NUM ) {
            status = ssiu_set_hdmi_mode_1channel( master_ssi_channel, ssi_channel, hdmi_channel );
        } else {
            (void)ssiu_clear_hdmi_mode( master_ssi_channel );
            (void)ssiu_clear_hdmi_mode( ssi_channel );
        }
    } while( false );

    if( status != EOK ) {
        (void)ssiu_1channel_cleanup( ssi_channel, master_ssi_channel );
    }

    return status;
}

int ssiu_1channel_cleanup( uint32_t ssi_channel, uint32_t master_ssi_channel )
{
    if( master_ssi_channel != ssi_channel ) {
        (void)ssi_clear_control_and_mode( master_ssi_channel );
        (void)ssiu_clear_mode_2channel( master_ssi_channel, ssi_channel );
    } else {
        (void)ssiu_clear_mode_1channel( ssi_channel );
    }

    (void)ssi_clear_control_and_mode( ssi_channel );
    (void)ssiu_clear_ssi_mode( ssi_channel );

    (void)ssiu_busif_cleanup( ssi_channel );
    (void)ssiu_clear_ssi_busif( ssi_channel );

    (void)ssiu_clear_hdmi_mode( master_ssi_channel );
    (void)ssiu_clear_hdmi_mode( ssi_channel );

    return EOK;
}

int ssiu_2channel_duplex_setup
(
    uint32_t ssi_channel_tx,
    uint32_t ssi_channel_rx,
    ssi_op_mode_t op_mode,
    ssi_masterslave_mode_t masterslave_mode,
    ssi_transfer_mode_t transfer_mode,
    ssi_start_mode_t start_mode,
    uint32_t bitnum,
    uint32_t voicenum,
    uint32_t voicenum_ext,
    uint32_t hdmi_channel,
    ssi_config_t* config,
    uint32_t* tdm_exsplit_map_tx,
    uint32_t* tdm_exsplit_map_rx
)
{
    int status;
    uint32_t ssi_master_channel = SSI_CHANNEL_NUM;
    uint32_t chn_is_master = 0;

    if( !rcar_ssi_supported( ssi_channel_tx ) ) {
        ado_error_fmt("SSI%d is not supported", ssi_channel_tx);
        return EINVAL;
    }
    if( !rcar_ssi_supported( ssi_channel_rx ) ) {
        ado_error_fmt("SSI%d is not supported", ssi_channel_rx);
        return EINVAL;
    }
    if( op_mode == SSI_OP_MODE_TDM || op_mode == SSI_OP_MODE_TDMEXT ) {
        if( !( ( ssi_channel_tx >= SSI_CHANNEL_0 && ssi_channel_tx <= SSI_CHANNEL_4 ) ||
               ssi_channel_tx == SSI_CHANNEL_9 ) ) {
            ado_error_fmt("SSI%d is not supported in TDM mode", ssi_channel_tx);
            return EINVAL;
        }
        if( !( ( ssi_channel_rx >= SSI_CHANNEL_0 && ssi_channel_rx <= SSI_CHANNEL_4 ) ||
               ssi_channel_rx == SSI_CHANNEL_9 ) ) {
            ado_error_fmt("SSI%d is not supported in TDM mode", ssi_channel_rx);
            return EINVAL;
        }
        if( bitnum != 16 && bitnum != 24 ) {
            ado_error_fmt("bitnum %d is not supported in TDM mode",
                      bitnum);
            return EINVAL;
        }
        if( voicenum <= 2 ) {
            ado_error_fmt("TDM modes not supported for voicenum=%d", voicenum);
            return EINVAL;
        }
    }
    if( op_mode == SSI_OP_MODE_TDMSPLIT_4XMONO || op_mode == SSI_OP_MODE_TDMSPLIT_4XSTEREO ) {
        return EINVAL;
    }

    if( masterslave_mode == SSI_MASTER_SLAVE ) {
        if( ssi_channel_tx == SSI_CHANNEL_0 ) {
            ssi_master_channel = SSI_CHANNEL_0;
        } else if( ssi_channel_rx == SSI_CHANNEL_0 ) {
            ssi_master_channel = SSI_CHANNEL_0;
        } else if( ssi_channel_tx == SSI_CHANNEL_3 ) {
            ssi_master_channel = SSI_CHANNEL_3;
        } else if( ssi_channel_rx == SSI_CHANNEL_3 ) {
            ssi_master_channel = SSI_CHANNEL_3;
        } else {
            ado_error_fmt("none of SSI%d and SSI%d is supported as master",
                      ssi_channel_tx, ssi_channel_rx);
            return EINVAL;
        }
    }

    do {
        chn_is_master = (ssi_channel_tx == ssi_master_channel ? 1 : 0);

        status = ssi_set_control_and_mode( ssi_channel_tx, chn_is_master, 1, op_mode, voicenum_ext, config );

        if( status != EOK ) break;

        chn_is_master = (ssi_channel_rx == ssi_master_channel ? 1 : 0 );

        status = ssi_set_control_and_mode( ssi_channel_rx, chn_is_master, 0, op_mode, voicenum_ext, config );

        if( status != EOK ) break;

        if( !( ssi_channel_tx == SSI_CHANNEL_3 && ssi_channel_rx == SSI_CHANNEL_4 ) &&
            !( ssi_channel_tx == SSI_CHANNEL_4 && ssi_channel_rx == SSI_CHANNEL_3 ) ) {
            start_mode = SSI_INDEPENDENT_START;
        }

        status = ssiu_set_mode_2channel( ssi_channel_tx, ssi_channel_rx, masterslave_mode, transfer_mode, start_mode );

        if( status != EOK ) break;

        status = ssiu_set_ssi_mode( ssi_channel_tx, op_mode );

        if( status != EOK ) break;

        status = ssiu_set_ssi_mode( ssi_channel_rx, op_mode );

        if( status != EOK ) break;

        if( transfer_mode == SSI_BUSIF_TRANSFER ) {
            status = ssiu_set_ssi_busif( ssi_channel_tx, op_mode, voicenum, bitnum, tdm_exsplit_map_tx );

            if( status != EOK ) break;

            status = ssiu_set_ssi_busif( ssi_channel_rx, op_mode, voicenum, bitnum, tdm_exsplit_map_rx );

            if( status != EOK ) break;
        }

        if( hdmi_channel != SSI_HDMI_CHANNEL_NUM ) {
            status = ssiu_set_hdmi_mode_1channel( ssi_master_channel, ssi_channel_tx, hdmi_channel );
        } else {
            (void)ssiu_clear_hdmi_mode( ssi_master_channel );
            (void)ssiu_clear_hdmi_mode( ssi_channel_tx );
        }
    } while( false );

    if( status != EOK ) {
        ssiu_2channel_duplex_cleanup( ssi_channel_tx, ssi_channel_rx );
    }

    return EOK;
}

int ssiu_2channel_duplex_cleanup
(
    uint32_t ssi_channel_tx,
    uint32_t ssi_channel_rx
)
{
    uint32_t ssi_master_channel = SSI_CHANNEL_0;

    if( ssi_channel_tx == SSI_CHANNEL_0 ) {
        ssi_master_channel = SSI_CHANNEL_0;
    } else if( ssi_channel_rx == SSI_CHANNEL_0 ) {
        ssi_master_channel = SSI_CHANNEL_0;
    } else if( ssi_channel_tx == SSI_CHANNEL_3 ) {
        ssi_master_channel = SSI_CHANNEL_3;
    } else if( ssi_channel_rx == SSI_CHANNEL_3 ) {
        ssi_master_channel = SSI_CHANNEL_3;
    }

    if( ( ssi_channel_tx == SSI_CHANNEL_3 && ssi_channel_rx == SSI_CHANNEL_4 ) ||
        ( ssi_channel_tx == SSI_CHANNEL_4 && ssi_channel_rx == SSI_CHANNEL_3 ) ) {
        (void)ssiu_stop( SSI_SYNC_SSI34_START );
    }

    (void)ssi_clear_control_and_mode( ssi_channel_tx );
    (void)ssi_clear_control_and_mode( ssi_channel_rx );

    (void)ssiu_clear_mode_2channel( ssi_channel_tx, ssi_channel_rx );

    (void)ssiu_clear_ssi_mode( ssi_channel_tx );
    (void)ssiu_clear_ssi_mode( ssi_channel_rx );

    (void)ssiu_busif_cleanup( ssi_channel_tx );
    (void)ssiu_busif_cleanup( ssi_channel_rx );
    (void)ssiu_clear_ssi_busif( ssi_channel_tx );
    (void)ssiu_clear_ssi_busif( ssi_channel_rx );

    (void)ssiu_clear_hdmi_mode( ssi_master_channel );
    (void)ssiu_clear_hdmi_mode( ssi_channel_tx );

    return EOK;
}

int ssiu_3channel_transmit_setup
(
    ssi_masterslave_mode_t masterslave_mode,
    uint32_t bitnum,
    uint32_t hdmi_channel,
    ssi_config_t* config
)
{
    int status;

    if( bitnum != 16 && bitnum != 24 ) {
        return EINVAL;
    }
    // TBD: existing driver sets up all three SSIs as master, while the doc seems to indicate that only
    // SSI 0 should be set up as master

    do {
        status = ssi_set_control_and_mode( SSI_CHANNEL_0,
                                           masterslave_mode == SSI_MASTER_SLAVE ? 1 : 0,
                                           1, SSI_OP_MODE_MULTICH, 6, config );
        if( status != EOK ) break;

        status = ssi_set_control_and_mode( SSI_CHANNEL_1, 0, 1, SSI_OP_MODE_MULTICH, 6, config );

        if( status != EOK ) break;

        status = ssi_set_control_and_mode( SSI_CHANNEL_2, 0, 1, SSI_OP_MODE_MULTICH, 6, config );

        if( status != EOK ) break;

        status = ssiu_set_mode_3channel( masterslave_mode );

        if( status != EOK ) break;

        status = ssiu_set_ssi_mode( SSI_CHANNEL_0, SSI_OP_MODE_MULTICH );

        if( status != EOK ) break;

        status = ssiu_set_ssi_mode( SSI_CHANNEL_1, SSI_OP_MODE_MULTICH );

        if( status != EOK ) break;

        status = ssiu_set_ssi_mode( SSI_CHANNEL_2, SSI_OP_MODE_MULTICH );

        if( status != EOK ) break;

        status = ssiu_set_ssi_busif( SSI_CHANNEL_0, SSI_OP_MODE_MULTICH, 6, bitnum, NULL );

        if( status != EOK ) break;

        if( hdmi_channel != SSI_HDMI_CHANNEL_NUM ) {
            status = ssiu_set_hdmi_mode_3channel( hdmi_channel );
        } else {
            (void)ssiu_clear_hdmi_mode( SSI_CHANNEL_0 );
            (void)ssiu_clear_hdmi_mode( SSI_CHANNEL_1 );
            (void)ssiu_clear_hdmi_mode( SSI_CHANNEL_2 );
        }
    } while( false );

    if( status != EOK ) {
        (void)ssiu_3channel_transmit_cleanup();
    }

    return status;
}

int ssiu_3channel_transmit_cleanup()
{
    (void)ssiu_stop( SSI_SYNC_SSI0129_START );

    (void)ssi_clear_control_and_mode( SSI_CHANNEL_0 );
    (void)ssi_clear_control_and_mode( SSI_CHANNEL_1 );
    (void)ssi_clear_control_and_mode( SSI_CHANNEL_2 );

    (void)ssiu_clear_mode_3channel();

    (void)ssiu_clear_ssi_mode( SSI_CHANNEL_0 );
    (void)ssiu_clear_ssi_mode( SSI_CHANNEL_1 );
    (void)ssiu_clear_ssi_mode( SSI_CHANNEL_2 );

    (void)ssiu_busif_cleanup( SSI_CHANNEL_0 );
    (void)ssiu_clear_ssi_busif( SSI_CHANNEL_0 );

    (void)ssiu_clear_hdmi_mode( SSI_CHANNEL_0 );
    (void)ssiu_clear_hdmi_mode( SSI_CHANNEL_1 );
    (void)ssiu_clear_hdmi_mode( SSI_CHANNEL_2 );

    return EOK;
}

int ssiu_4channel_transmit_setup
(
    ssi_masterslave_mode_t masterslave_mode,
    uint32_t bitnum,
    uint32_t hdmi_channel,
    ssi_config_t* config
)
{
    int status;

    if( bitnum != 16 && bitnum != 24 ) {
        return EINVAL;
    }

    // TBD: existing driver sets up all four SSIs as master, while the doc seems to indicate that only
    // SSI 0 should be set up as master

    do {
        status = ssi_set_control_and_mode( SSI_CHANNEL_0,
                                           masterslave_mode == SSI_MASTER_SLAVE ? 1 : 0,
                                           1, SSI_OP_MODE_MULTICH, 8, config );
        if( status != EOK ) break;

        status = ssi_set_control_and_mode( SSI_CHANNEL_1, 0, 1, SSI_OP_MODE_MULTICH, 8, config );

        if( status != EOK ) break;

        status = ssi_set_control_and_mode( SSI_CHANNEL_2, 0, 1, SSI_OP_MODE_MULTICH, 8, config );

        if( status != EOK ) break;

        status = ssi_set_control_and_mode( SSI_CHANNEL_9, 0, 1, SSI_OP_MODE_MULTICH, 8, config );

        if( status != EOK ) break;

        status = ssiu_set_mode_4channel( masterslave_mode );

        if( status != EOK ) break;

        status = ssiu_set_ssi_mode( SSI_CHANNEL_0, SSI_OP_MODE_MULTICH );

        if( status != EOK ) break;

        status = ssiu_set_ssi_mode( SSI_CHANNEL_1, SSI_OP_MODE_MULTICH );

        if( status != EOK ) break;

        status = ssiu_set_ssi_mode( SSI_CHANNEL_2, SSI_OP_MODE_MULTICH );

        if( status != EOK ) break;

        status = ssiu_set_ssi_mode( SSI_CHANNEL_9, SSI_OP_MODE_MULTICH );

        if( status != EOK ) break;

        status = ssiu_set_ssi_busif( SSI_CHANNEL_0, SSI_OP_MODE_MULTICH, 8, bitnum, NULL );

        if( status != EOK ) break;

        if( hdmi_channel != SSI_HDMI_CHANNEL_NUM ) {
            status = ssiu_set_hdmi_mode_4channel( hdmi_channel );
        } else {
            (void)ssiu_clear_hdmi_mode( SSI_CHANNEL_0 );
            (void)ssiu_clear_hdmi_mode( SSI_CHANNEL_1 );
            (void)ssiu_clear_hdmi_mode( SSI_CHANNEL_2 );
            (void)ssiu_clear_hdmi_mode( SSI_CHANNEL_9 );
        }

    } while (false);

    if( status != EOK ) {
        (void)ssiu_4channel_transmit_cleanup();
    }

    return status;
}

int ssiu_4channel_transmit_cleanup()
{
    (void)ssiu_stop( SSI_SYNC_SSI0129_START );

    (void)ssi_clear_control_and_mode( SSI_CHANNEL_0 );
    (void)ssi_clear_control_and_mode( SSI_CHANNEL_1 );
    (void)ssi_clear_control_and_mode( SSI_CHANNEL_2 );
    (void)ssi_clear_control_and_mode( SSI_CHANNEL_9 );

    (void)ssiu_clear_mode_4channel();

    (void)ssiu_clear_ssi_mode( SSI_CHANNEL_0 );
    (void)ssiu_clear_ssi_mode( SSI_CHANNEL_1 );
    (void)ssiu_clear_ssi_mode( SSI_CHANNEL_2 );
    (void)ssiu_clear_ssi_mode( SSI_CHANNEL_9 );

    (void)ssiu_busif_cleanup( SSI_CHANNEL_0 );
    (void)ssiu_clear_ssi_busif( SSI_CHANNEL_0 );

    (void)ssiu_clear_hdmi_mode( SSI_CHANNEL_0 );
    (void)ssiu_clear_hdmi_mode( SSI_CHANNEL_1 );
    (void)ssiu_clear_hdmi_mode( SSI_CHANNEL_2 );
    (void)ssiu_clear_hdmi_mode( SSI_CHANNEL_9 );

    return EOK;
}


int ssiu_start( ssi_start_mode_t start_mode )
{
    if( !SSIU_COMMON ) {
        return EFAULT;
    }
    if( start_mode == SSI_SYNC_SSI0129_START ) {
        SSIU_COMMON->control &= ~SSI_CONTROL_SSI0129_MASK;
        SSIU_COMMON->control |= SSI_CONTROL_SSI0129_ENABLE;
    } else if ( start_mode == SSI_SYNC_SSI34_START ) {
        SSIU_COMMON->control &= ~SSI_CONTROL_SSI34_MASK;
        SSIU_COMMON->control |= SSI_CONTROL_SSI34_ENABLE;
    }
    return EOK;
}

int ssiu_stop( ssi_start_mode_t start_mode )
{
    if( !SSIU_COMMON ) {
        return EFAULT;
    }
    if( start_mode == SSI_SYNC_SSI0129_START ) {
        SSIU_COMMON->control &= ~SSI_CONTROL_SSI0129_MASK;
    } else if ( start_mode == SSI_SYNC_SSI34_START ) {
        SSIU_COMMON->control &= ~SSI_CONTROL_SSI34_MASK;
    }
    return EOK;
}

int ssiu_busif_start( uint32_t ssi_channel, uint32_t ssi_sub_channel )
{
    if( !rcar_ssi_supported(ssi_channel) ) {
        return EINVAL;
    }
    if( !rcar_ssi_subchan_supported(ssi_channel, ssi_sub_channel) ) {
        return EINVAL;
    }
    if( !SSIU_SSI( ssi_channel ) ) {
        return EFAULT;
    }

    SSIU_SSI( ssi_channel )->control |= SSI_CONTROL_START_START(ssi_sub_channel);

    return EOK;
}

int ssiu_busif_stop( uint32_t ssi_channel, uint32_t ssi_sub_channel )
{
    if( !rcar_ssi_supported(ssi_channel) ) {
        return EINVAL;
    }
    if( !rcar_ssi_subchan_supported(ssi_channel, ssi_sub_channel) ) {
        return EINVAL;
    }
    if( !SSIU_SSI( ssi_channel ) ) {
        return EFAULT;
    }

    SSIU_SSI( ssi_channel )->control &= ~SSI_CONTROL_START_MASK(ssi_sub_channel);

    return EOK;
}

int ssiu_busif_cleanup( uint32_t ssi_channel )
{
    if( !rcar_ssi_supported(ssi_channel) ) {
        return EINVAL;
    }
    if( !SSIU_SSI( ssi_channel ) ) {
        return EFAULT;
    }

    SSIU_SSI( ssi_channel )->control = 0;

    return EOK;
}

int ssi_set_divisor(uint32_t ssi_channel, uint32_t divisor)
{
    int status;
    uint32_t cr;

    if( !rcar_ssi_supported(ssi_channel) ) {
        return EINVAL;
    }

    if( !SSI(ssi_channel) ) {
        return EFAULT;
    }

    cr = SSI(ssi_channel)->cr;

    status = ssi_set_clockdiv_bits( &cr, divisor );
    cr |= SSICR_FORCE_FIXED;

    if( status == EOK ) {
        ado_debug (DB_LVL_DRIVER, "Setting SSI%d divisor to %u", ssi_channel, divisor);

        SSI(ssi_channel)->cr = cr;
    }

    return status;
}

/* SSIU register dump functions */

void ssiu_common_register_dump()
{
    if( !SSIU_COMMON ) {
        ado_error_fmt("SSIU memory not mapped");
        return;
    }
    ado_debug( DB_LVL_DRIVER, "SSIU COMMON reg dump: MODE[0:3]=%x %x %x %x CONTROL=%x",
               SSIU_COMMON->mode0, SSIU_COMMON->mode1, SSIU_COMMON->mode2,
               SSIU_COMMON->mode3, SSIU_COMMON->control );
    ado_debug( DB_LVL_DRIVER, "SSIU COMMON reg dump: SYSTEM_STATUS[0:3]=%x %x %x %x",
               SSIU_COMMON->system_status0, SSIU_COMMON->system_status1,
               SSIU_COMMON->system_status2, SSIU_COMMON->system_status3 );
    ado_debug( DB_LVL_DRIVER, "SSIU COMMON reg dump: SYSTEM_INT_ENABLE[0:3]=%x %x %x %x",
               SSIU_COMMON->system_int_enable0, SSIU_COMMON->system_int_enable1,
               SSIU_COMMON->system_int_enable2, SSIU_COMMON->system_int_enable3 );
    if( SSIU_COMMON_EXT ) {
        ado_debug( DB_LVL_DRIVER, "SSIU COMMON EXT reg dump: CONTROL2=%x HDMI0_SEL=%x HDMI1_SEL=%x",
                   SSIU_COMMON_EXT->control2, SSIU_COMMON_EXT->hdmi0_sel, SSIU_COMMON_EXT->hdmi1_sel );
        ado_debug( DB_LVL_DRIVER, "SSIU COMMON EXT reg dump: SYSTEM_STATUS[4:7]=%x %x %x %x",
                   SSIU_COMMON_EXT->system_status4, SSIU_COMMON_EXT->system_status5,
                   SSIU_COMMON_EXT->system_status6, SSIU_COMMON_EXT->system_status7 );
        ado_debug( DB_LVL_DRIVER, "SSIU COMMON EXT reg dump: SYSTEM_INT_ENABLE[4:7]=%x %x %x %x",
                   SSIU_COMMON_EXT->system_int_enable4, SSIU_COMMON_EXT->system_int_enable5,
                   SSIU_COMMON_EXT->system_int_enable6, SSIU_COMMON_EXT->system_int_enable7 );
    }
}

void ssiu_ssi_register_dump( uint32_t ssi_channel )
{
    uint32_t min_subchannel = 0;
    uint32_t max_subchannel = 0;
    int i;
    if( !rcar_ssi_supported(ssi_channel) ) {
        ado_error_fmt("SSI%d is not supported", ssi_channel);
        return;
    }
    if( SSI(ssi_channel) ) {
        ado_debug( DB_LVL_DRIVER, "SSI%d reg dump: CR=%x SR=%x WSR=%x FMR=%x FSR=%x",
                   ssi_channel, SSI(ssi_channel)->cr, SSI(ssi_channel)->sr,
                   SSI(ssi_channel)->wsr, SSI(ssi_channel)->fmr, SSI(ssi_channel)->fsr );
    } else {
        ado_error_fmt("SSI memory not mapped");
    }
    if( SSI_EXT(ssi_channel) ) {
        ado_debug( DB_LVL_DRIVER, "SSI%d EXT reg dump: CRE=%x", ssi_channel, SSI_EXT(ssi_channel)->cre );
    }
    if( SSIU_SSI(ssi_channel) ) {
        ado_debug( DB_LVL_DRIVER, "SSIU SSI%d reg dump: MODE=%x CONTROL=%x STATUS=%x, INT_ENABLE_MAIN=%x",
                   ssi_channel, SSIU_SSI(ssi_channel)->mode, SSIU_SSI(ssi_channel)->control,
                   SSIU_SSI(ssi_channel)->status, SSIU_SSI(ssi_channel)->int_enable_main );
    } else {
        ado_error_fmt("SSIU memory not mapped");
    }
    if( SSIU_SSI2(ssi_channel) ) {
        ado_debug( DB_LVL_DRIVER, "SSIU SSI%d EXT reg dump: MODE2=%x STATUS2=%x INT_ENABLE_MAIN2=%x",
                   ssi_channel, SSIU_SSI2(ssi_channel)->mode2, SSIU_SSI2(ssi_channel)->status2,
                   SSIU_SSI2(ssi_channel)->int_enable_main2 );
    }

    rcar_ssi_subchan_get_supported_range( ssi_channel, &min_subchannel, &max_subchannel );

    for( i = min_subchannel; i <= max_subchannel; i++ ) {
        if( SSIU_BUSIF(ssi_channel, i) ) {
            ado_debug( DB_LVL_DRIVER, "SSIU BUSIF%d-%d reg dump: BUSIF_MODE=%x BUSIF_ADINR=%x BUSIF_DALIGN=%x",
                       ssi_channel, i,
                       SSIU_BUSIF(ssi_channel, i)->busif_mode,
                       SSIU_BUSIF(ssi_channel, i)->busif_adinr,
                       SSIU_BUSIF(ssi_channel, i)->busif_dalign );
        } else {
            ado_error_fmt("SSIU memory not mapped");
        }
    }

    if( SSIU_BUSIF2(ssi_channel) ) {
        ado_debug( DB_LVL_DRIVER, "SSIU BUSIF%d EXT reg dump: BUSIF_DALIGN2=%x",
                   ssi_channel, SSIU_BUSIF2(ssi_channel)->busif_dalign2 );
    }
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/deva/ctrl/rcar/ssiu.c $ $Rev: 833552 $")
#endif

