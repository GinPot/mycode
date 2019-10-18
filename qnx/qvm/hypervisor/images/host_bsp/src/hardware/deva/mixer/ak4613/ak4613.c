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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <devctl.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/neutrino.h>
#include <hw/inout.h>
#include <sys/mman.h>
#include <audio_driver.h>
#include "ak4613.h"
#include "i2c2.h"

/* ak4613 registers */
#define AK4613_POWER_MANAGEMENT1        0x00
  #define AK4613_POWER_MANAGEMENT1_RSTN   0x01
  #define AK4613_POWER_MANAGEMENT1_PMDAC  0x02
  #define AK4613_POWER_MANAGEMENT1_PMADC  0x04
  #define AK4613_POWER_MANAGEMENT1_PMVR   0x08
#define AK4613_POWER_MANAGEMENT2        0x01
  #define AK4613_POWER_MANAGEMENT2_AD1    0x01
  #define AK4613_POWER_MANAGEMENT2_AD2    0x02
  #define AK4613_POWER_MANAGEMENT2_AD(adc_num) ((1<<adc_num) - 1)
#define AK4613_POWER_MANAGEMENT3        0x02
  #define AK4613_POWER_MANAGEMENT3_DA1    0x01
  #define AK4613_POWER_MANAGEMENT3_DA2    0x02
  #define AK4613_POWER_MANAGEMENT3_DA3    0x04
  #define AK4613_POWER_MANAGEMENT3_DA4    0x08
  #define AK4613_POWER_MANAGEMENT3_DA5    0x10
  #define AK4613_POWER_MANAGEMENT3_DA6    0x20
  #define AK4613_POWER_MANAGEMENT2_DA(dac_num) ((1<<dac_num) - 1)
#define AK4613_CONTROL1                 0x03
  #define AK4613_CONTROL1_SMUTE_MASK      0x1
  #define AK4613_CONTROL1_TDM_MASK        (0x3 << 6)
  #define AK4613_CONTROL1_STEREO          0x0
  #define AK4613_CONTROL1_TDM512          (0x1 << 6)
  #define AK4613_CONTROL1_TDM256          (0x2 << 6)
  #define AK4613_CONTROL1_TDM128          (0x3 << 6)
#define AK4613_CONTROL2                 0x04
  #define AK4613_CONTROL2_DFS_MASK        (0x03 << 2)
  #define AK4613_CONTROL2_DFS_NORMAL      0x00
  #define AK4613_CONTROL2_DFS_DOUBLE      (0x01 << 2)
  #define AK4613_CONTROL2_DFS_QUAD        (0x02 << 2)
  #define AK4613_CONTROL2_CKS_MASK        (0x03 << 4)
  #define AK4613_CONTROL2_CKS_256FS       0x00
  #define AK4613_CONTROL2_CKS_384FS       (0x01 << 4)
  #define AK4613_CONTROL2_CKS_512FS       (0x02 << 4)
  #define AK4613_CONTROL2_ACKS_MASK       0x02
#define AK4613_DE_EMPHASIS1             0x05
#define AK4613_DE_EMPHASIS2             0x06
#define AK4613_OVERFLOW_DETECT          0x07
#define AK4613_ZERO_DETECT              0x08
#define AK4613_INPUT_CONTROL            0x09
  #define AK4613_INPUT_CONTROL_DIE1       0x01
  #define AK4613_INPUT_CONTROL_DIE2       0x02
#define AK4613_OUTPUT_CONTROL           0x0A
  #define AK4613_OUTPUT_CONTROL_DOE1      0x01
  #define AK4613_OUTPUT_CONTROL_DOE2      0x02
  #define AK4613_OUTPUT_CONTROL_DOE3      0x04
  #define AK4613_OUTPUT_CONTROL_DOE4      0x08
  #define AK4613_OUTPUT_CONTROL_DOE5      0x10
  #define AK4613_OUTPUT_CONTROL_DOE6      0x20
#define AK4613_LOUT1_VOLUME_CONTROL     0x0B
#define AK4613_ROUT1_VOLUME_CONTROL     0x0C
#define AK4613_LOUT2_VOLUME_CONTROL     0x0D
#define AK4613_ROUT2_VOLUME_CONTROL     0x0E
#define AK4613_LOUT3_VOLUME_CONTROL     0x0F
#define AK4613_ROUT3_VOLUME_CONTROL     0x10
#define AK4613_LOUT4_VOLUME_CONTROL     0x11
#define AK4613_ROUT4_VOLUME_CONTROL     0x12
#define AK4613_LOUT5_VOLUME_CONTROL     0x13
#define AK4613_ROUT5_VOLUME_CONTROL     0x14
#define AK4613_LOUT6_VOLUME_CONTROL     0x15
#define AK4613_ROUT6_VOLUME_CONTROL     0x16

static ak4613_context_t* ak4613 = NULL;

static int ak4613_open_i2c_fd ( void )
{
    int status = EOK;

    if ( ak4613 ) {
        if ( ak4613->params.codec_open ) {
            status = ak4613->params.codec_open( ak4613->params.hw_context );
        } else {
            char i2c_devname[20];
            sprintf ( i2c_devname, "/dev/i2c%d", ak4613->params.i2c_dev );
            status = open_i2c_fd ( i2c_devname );
        }
        if ( status != EOK ) {
            ado_error_fmt ( "AK4613 codec open failed" );
        }
    } else {
        ado_error_fmt( "AK4613 codec is not init" );
        status = EINVAL;
    }
    return status;
}

static int ak4613_close_i2c_fd ( void )
{
    int status = EOK;

    if ( ak4613 ) {
        if ( ak4613->params.codec_close ) {
            ak4613->params.codec_close( ak4613->params.hw_context );
        } else {
            close_i2c_fd();
        }
    } else {
        ado_error_fmt( "AK4613 codec is not init" );
        status = EINVAL;
    }

    return status;
}

static int ak4613_i2c_write ( uint8_t reg, uint8_t val, int index )
{
    int status = EOK;

    if ( ak4613 ) {
        if ( ak4613->params.codec_write ) {
            status = ak4613->params.codec_write( ak4613->params.hw_context, reg, val, index );
        } else {
            status = i2c_write ( ak4613->params.i2c_addr, reg, val, index );
        }
        if ( status != EOK ) {
            ado_error_fmt ( "AK4613 i2c write failed" );
        }
    } else {
        ado_error_fmt("AK4613 codec is not init");
        status = EINVAL;
    }

    return status;
}

static int ak4613_i2c_read ( uint8_t reg, uint8_t* val, int index )
{
    int status = EOK;
    uint32_t reg_val;

    if ( ak4613 && val ) {
        if ( ak4613->params.codec_read ) {
            status = ak4613->params.codec_read( ak4613->params.hw_context, reg, &reg_val, index );
            if ( status == EOK ) {
                *val = reg_val;
            }
        } else {
            status = i2c_read ( ak4613->params.i2c_addr, reg, val, index );
        }
        if ( status != EOK ) {
            ado_error_fmt ( "AK4613 i2c read failed" );
        }
    } else {
        ado_error_fmt("AK4613 codec is not init or val is NULL");
        status = EINVAL;
    }

    return status;
}

static int ak4613_count_bits ( uint8_t bitmask )
{
    int count = 0;
    while ( bitmask ) {
        bitmask &= (bitmask - 1);
        count++;
    }
    return count;
}

int ak4613_init ( ak4613_context_t* ak4613_ctx )
{
    /* For both playback and capture */

    uint8_t data, dfs, tdm;
    unsigned num_dacs, num_adcs;
    int status, i;

    ado_debug ( DB_LVL_DRIVER, "" );

    if ( ak4613_ctx == NULL ) {
        ado_error_fmt ( "ak4613_ctx is NULL" );
        return EINVAL;
    }

    ak4613 = ak4613_ctx;

    if ( ak4613->params.tx_sample_rate != ak4613->params.rx_sample_rate ) {
        ado_error_fmt ( "tx and rx sample rate must be same" );
        return ENOTSUP;
    }

    if ( ( ak4613->params.tx_voices % 2 ) || ak4613->params.tx_voices > 12 ) {
        ado_error_fmt ( "tx voices %d not supported", ak4613->params.tx_voices );
        return ENOTSUP;
    }

    if ( ( ak4613->params.rx_voices % 2 ) || ak4613->params.rx_voices > 4 ) {
        ado_error_fmt ( "rx voices %d not supported", ak4613->params.rx_voices );
        return ENOTSUP;
    }

    status = ak4613_open_i2c_fd();
    if ( status != EOK ) {
        return status;
    }

    num_dacs = ak4613->params.tx_voices ? 1 : 0;
    num_adcs = ak4613->params.rx_voices ? 1 : 0;

    /* the DFS selection based on sample_rate is from the reference AK4613 codec
     * source code from Renesas; in slave mode it works fine even if the MCKI rate
     * is not a 128/256/512 multiple of the sample rate (MCKI rate is 50 MHz on
     * Renesas Salvator board); in master mode it is expected that for the below
     * determination to work, the MCKI rate should be the corresponding multiple of
     * the sample rate (512 when DFS_NORMAL is used, 256 when DFS_DOUBLE is used,
     * 128 when DFS_QUAD is used) */
    switch ( ak4613->params.tx_sample_rate ) {
        case 32000:
        case 44100:
        case 48000:
            dfs = AK4613_CONTROL2_DFS_NORMAL; //512fs
            tdm = ak4613->tdm ? AK4613_CONTROL1_TDM512 : AK4613_CONTROL1_STEREO;
            break;
        case 88200:
        case 96000:
            dfs = AK4613_CONTROL2_DFS_DOUBLE; //256fs
            tdm = ak4613->tdm ? AK4613_CONTROL1_TDM256 : AK4613_CONTROL1_STEREO;
            break;
        case 176400:
        case 192000:
            dfs = AK4613_CONTROL2_DFS_QUAD; //128fs
            tdm = ak4613->tdm ? AK4613_CONTROL1_TDM128 : AK4613_CONTROL1_STEREO;
            break;
        default:
            return ENOTSUP;
    }

    switch ( tdm ) {
        case AK4613_CONTROL1_TDM512:
            break;
        case AK4613_CONTROL1_TDM256:
            if (ak4613->params.tx_voices > 8) {
                num_dacs = 2;
            }
            break;
        case AK4613_CONTROL1_TDM128:
            if (ak4613->params.tx_voices > 8) {
                num_dacs = 3;
            } else if (ak4613->params.tx_voices > 4) {
                num_dacs = 2;
            }
            break;
        case AK4613_CONTROL1_STEREO:
            num_adcs = (ak4613->params.rx_voices >> 1); /* rx_voices/2 */
            num_dacs = (ak4613->params.tx_voices >> 1); /* tx_voices/2 */
            break;
        default:
            return ENOTSUP;
    }

    if ( num_dacs != ak4613_count_bits ( ak4613->dac_mask ) ) {
        ado_error_fmt ( "Discrepancy between specified DAC outputs %x and intended config: tdm=%d, tx_voices=%d",
                        ak4613->dac_mask, ak4613->tdm, ak4613->params.tx_voices );
        return ENOTSUP;
    }

    if ( num_adcs != ak4613_count_bits ( ak4613->adc_mask ) ) {
        ado_error_fmt ( "Discrepancy between specified ADC inputs %x and intended config: tdm=%d, rx_voices=%d",
                        ak4613->adc_mask, ak4613->tdm, ak4613->params.rx_voices );
        return ENOTSUP;
    }

    /*
     * (0x03) Control 1 (default=0x20)
     *  [7:6] TDM1-0 =00/01/10/11  TDM Format Select                  (00:Stereo, 01:TDM512, 10:TDM256, 11:TDM128)
     *  [5:3] DIF2-0 =100          Audio Data Interface Modes         (100: 24bit I2S)
     *  [2:1] ATS1-0 =00           Digital attenuator transition time (00:4096/fs, 01:2048/fs, 10:512/fs, 11:256/fs)
     *  [0]   SMUTE  =0            Soft Mute Enable                   (0: Normal operation, 1: All DAC outputs soft-muted)
     */
    data = 0x20;
    data |= tdm;
    status = ak4613_i2c_write ( AK4613_CONTROL1, data, 0 );
    if ( status != EOK ) {
        return status;
    }

    /* wait 4096/fs --> 4096 * 1/32kHz = 128ms */
    delay ( 200 );  // wait 200ms

    /* (0x00) Power Management 1 (default=0x0F)
     *  [7:4] fixed =0
     *  [3]   PMVR  =1      Power management of reference voltage (1: Normal operation, 0: Power-down)
     *  [2]   PMADC =0/1    Power management of ADC               (1: Normal operation, 0: Power-down)
     *  [1]   PMDAC =0/1    Power management of DAC               (1: Normal operation, 0: Power-down)
     *  [0]   RSTN  =1      Internal timing reset                 (1: Normal operation, 0: Reset, but registers are not initialized)
     */
    data = AK4613_POWER_MANAGEMENT1_RSTN|AK4613_POWER_MANAGEMENT1_PMVR;
    if ( num_adcs ) {
        /* assume the used ADCs are the first "num_adcs" ADCs */
        data |= AK4613_POWER_MANAGEMENT1_PMADC;
    }
    if ( num_dacs ) {
        /* assume the used DACs are the first "num_dacs" DACs */
        data |= AK4613_POWER_MANAGEMENT1_PMDAC;
    }
    status = ak4613_i2c_write ( AK4613_POWER_MANAGEMENT1, data, 0 );
    if ( status != EOK ) {
        return status;
    }

    /* (0x01) Power Management 2 (default=0x07)
     *  [7:3] fixed =0
     *  [2]   fixed =1
     *  [1]   PMAD2 =0/1    Power management of ADC2 (0: Power-down, 1: Normal operation)
     *  [0]   PMAD1 =0/1    Power management of ADC1 (0: Power-down, 1: Normal operation)
     */
    data = 0x04 | ak4613->adc_mask;
    status = ak4613_i2c_write ( AK4613_POWER_MANAGEMENT2, data, 0 );
    if ( status != EOK ) {
        return status;
    }

    /* (0x02) Power Management 3 (default=0x3F)
     *  [7:6] fixed =0
     *  [5]   PMDA6 =0/1    Power management of DAC6 (0: Power-down, 1: Normal operation)
     *  [4]   PMDA5 =0/1    Power management of DAC5 (0: Power-down, 1: Normal operation)
     *  [3]   PMDA4 =0/1    Power management of DAC4 (0: Power-down, 1: Normal operation)
     *  [2]   PMDA3 =0/1    Power management of DAC3 (0: Power-down, 1: Normal operation)
     *  [1]   PMDA2 =0/1    Power management of DAC2 (0: Power-down, 1: Normal operation)
     *  [0]   PMDA1 =0/1    Power management of DAC1 (0: Power-down, 1: Normal operation)
     */
    data = ak4613->dac_mask;
    status = ak4613_i2c_write ( AK4613_POWER_MANAGEMENT3, data, 0 );
    if ( status != EOK ) {
        return status;
    }

    /* (0x04) Control 2 (default=0x20)
     *  [7]   fixed  =0
     *  [6]   MCKO   =0        Master clock output enable          (0:Output"L", 1:Output "MCKO")
     *  [5:4] CKS1-0 =00/01/10 Master Clock Input Frequency Select (00:Normal Speed=256fs, Double Speed=256fs, Quad Speed=128fs)
     *  [3:2] DFS1-0 =00/01/10 Sampling speed mode                 (00:Normal Speed, 01:Double Speed, 10:Quad Speed, 11:N/A)
     *                         DFS is ignored at ACKS bit=1.
     *  [1]   ACKS   =0        Master Clock Frequency Auto Setting (0:Disable, 1:Enable)
     *  [0]   DIV    =0        Output of Master clock frequency    (0:x1, 1:x1/2) --> do not use MCKO, then set default
     */
    data = 0x00;
    data |= dfs;
    status = ak4613_i2c_write ( AK4613_CONTROL2, data, 0 );
    if ( status != EOK ) {
        return status;
    }

    /* (0x05) De-emphasis1 (default=0x55)
     *  [7:6] DEM41-0 =01   De-emphasis response control for DAC4 data on SDTI1 (01:OFF)
     *  [5:4] DEM31-0 =01   De-emphasis response control for DAC3 data on SDTI1 (01:OFF)
     *  [3:2] DEM21-0 =01   De-emphasis response control for DAC2 data on SDTI1 (01:OFF)
     *  [1:0] DEM11-0 =01   De-emphasis response control for DAC1 data on SDTI1 (01:OFF)
     */
    status = ak4613_i2c_write ( AK4613_DE_EMPHASIS1, 0x55, 0 );
    if ( status != EOK ) {
        return status;
    }

    /* (0x06) De-emphasis2 (default=0x05)
     *  [7:4] fixed   =0
     *  [3:2] DEM61-0 =01   De-emphasis response control for DAC6 data on SDTI1 (01:OFF)
     *  [1:0] DEM51-0 =01   De-emphasis response control for DAC5 data on SDTI1 (01:OFF)
     */
    status = ak4613_i2c_write ( AK4613_DE_EMPHASIS2, 0x05, 0 );
    if ( status != EOK ) {
        return status;
    }

    /* (0x07) Overflow Detect (default=0x07)
     *  [7:4] fixed   =0
     *  [3]   OVFE    =0    Overflow detection enable   (0:Disable, 1:Enable)
     *  [2:0] OVFM2-0 =111  Overflow detect mode select (111:Disable)
     */
    status = ak4613_i2c_write ( AK4613_OVERFLOW_DETECT, 0x07, 0 );
    if ( status != EOK ) {
        return status;
    }

    /* (0x08) Zero Detect (default=0x0F)
     *  [7:6] LOOP1-0 =00   Loopback mode enable (00:Normal, No loop back)
     *  [5:4] fixed   =0
     *  [3:0] DZFM3-0 =1111 Zero detect mode select (1111:Disable)
     */
    status = ak4613_i2c_write ( AK4613_ZERO_DETECT, 0x0F, 0 );
    if ( status != EOK ) {
        return status;
    }

    /* (0x09) Input Control (default=0x07)
     *  [7:3] fixed =0
     *  [2]   fixed =1
     *  [1]   DIE2  =0      ADC2 Differential Input Enable (0:Single-End Input, 1:Differential Input)
     *  [0]   DIE1  =0      ADC1 Differential Input Enable (0:Single-End Input, 1:Differential Input)
     */
    data = 0x04 | ak4613->differential_in_mask;
    status = ak4613_i2c_write ( AK4613_INPUT_CONTROL, data, 0 );
    if ( status != EOK ) {
        return status;
    }

    /* (0x0A) Output Control (default=0x3F)
     *  [7:6] fixed =0
     *  [5]   DOE6  =0      DAC6 Differential Output Enable (0:Single-End Output, 1:Differential Output)
     *  [4]   DOE5  =0      DAC5 Differential Output Enable (0:Single-End Output, 1:Differential Output)
     *  [3]   DOE4  =0      DAC4 Differential Output Enable (0:Single-End Output, 1:Differential Output)
     *  [2]   DOE3  =0      DAC3 Differential Output Enable (0:Single-End Output, 1:Differential Output)
     *  [1]   DOE2  =0      DAC2 Differential Output Enable (0:Single-End Output, 1:Differential Output)
     *  [0]   DOE1  =0      DAC1 Differential Output Enable (0:Single-End Output, 1:Differential Output)
     */
    data = ak4613->differential_out_mask;
    status = ak4613_i2c_write ( AK4613_OUTPUT_CONTROL, data, 0 );
    if ( status != EOK ) {
        return status;
    }

    /* (0x0B-0x16) Volume Control (default=0x00)
     *  [7:0] ATT7-0        Attenuation Level
     *       ==> 0x00:0.0dB, 0x01:-0.5dB, 0x02:-1.0dB, ..., 0xFD:-126.5dB, 0xFE:-127.0dB, 0xFF:MUTE
     */
    for ( i = 0; i < AK4613_NUM_DACS; i++ ) {
        /* set LOUT/ROUT Volume Control to 0x00:0.0dB for used DACs, or 0xFF:MUTE for unused DACs */
        data = ( ak4613->dac_mask & (1 << i) ) ? 0x00 : 0xFF;

        status = ak4613_i2c_write ( AK4613_LOUT1_VOLUME_CONTROL + (i<<1), data, 0 );
        if ( status != EOK ) {
            return status;
        }

        status = ak4613_i2c_write ( AK4613_ROUT1_VOLUME_CONTROL + (i<<1), data, 0 );
        if ( status != EOK ) {
            return status;
        }
    }

    if ( ak4613->debug ) {
        ak4613_register_dump ();
    }

    return EOK;
}

int ak4613_deinit( void )
{
    /* For both playback and capture */

    /* (0x00) Power Management 1 (default=0x0F)
     *  [7:4] fixed =0
     *  [3]   PMVR  =0      Power management of reference voltage (1: Normal operation, 0: Power-down)
     *  [2]   PMADC =0      Power management of ADC               (1: Normal operation, 0: Power-down)
     *  [1]   PMDAC =0      Power management of DAC               (1: Normal operation, 0: Power-down)
     *  [0]   RSTN  =0      Internal timing reset                 (1: Normal operation, 0: Reset, but registers are not initialized)
     */
    ak4613_i2c_write ( AK4613_POWER_MANAGEMENT1, 0x0, 0 );

    /* (0x01) Power Management 2 (default=0x07)
     *  [7:3] fixed =0
     *  [2]   fixed =1
     *  [1]   PMAD2 =0      Power management of ADC2 (0: Power-down, 1: Normal operation)
     *  [0]   PMAD1 =0      Power management of ADC1 (0: Power-down, 1: Normal operation)
     */
    ak4613_i2c_write ( AK4613_POWER_MANAGEMENT2, 0x04, 0 );

    /* (0x02) Power Management 3 (default=0x3F)
     *  [7:6] fixed =0
     *  [5]   PMDA6 =0      Power management of DAC6 (0: Power-down, 1: Normal operation)
     *  [4]   PMDA5 =0      Power management of DAC5 (0: Power-down, 1: Normal operation)
     *  [3]   PMDA4 =0      Power management of DAC4 (0: Power-down, 1: Normal operation)
     *  [2]   PMDA3 =0      Power management of DAC3 (0: Power-down, 1: Normal operation)
     *  [1]   PMDA2 =0      Power management of DAC2 (0: Power-down, 1: Normal operation)
     *  [0]   PMDA1 =0      Power management of DAC1 (0: Power-down, 1: Normal operation)
     */
    ak4613_i2c_write ( AK4613_POWER_MANAGEMENT3, 0x0, 0 );

    if ( ak4613->debug ) {
        ak4613_register_dump ();
    }

    ak4613_close_i2c_fd ();

    return EOK;
};

int ak4613_output_vol_get( unsigned voices, uint8_t* vol )
{
    int status = EOK, i, dac;
    uint8_t data;

    if ( voices != ak4613->params.tx_voices || vol == NULL ) {
        return EINVAL;
    }

    for ( i = 0, dac = 0; dac < AK4613_NUM_DACS && i < voices; dac++ ) {
        if ( ak4613->dac_mask & (0x1 << dac) ) {
            status = ak4613_i2c_read ( AK4613_LOUT1_VOLUME_CONTROL + (dac<<1), &data, 0 );
            if ( status != EOK ) {
                return status;
            }
            vol[i++] = AK4613_MAX_DIGITAL_VOL - data;

            status = ak4613_i2c_read ( AK4613_ROUT1_VOLUME_CONTROL + (dac<<1), &data, 0 );
            if ( status != EOK ) {
                return status;
            }
            vol[i++] = AK4613_MAX_DIGITAL_VOL - data;
        }
    }
    return EOK;
}

int ak4613_output_vol_set( unsigned voices, uint8_t* vol )
{
    int status = EOK, i, dac;

    if ( voices != ak4613->params.tx_voices || vol == NULL ) {
        return EINVAL;
    }

    for ( i = 0, dac = 0; dac < AK4613_NUM_DACS && i < voices; dac++ ) {
        if ( ak4613->dac_mask & (0x1 << dac) ) {
            status = ak4613_i2c_write ( AK4613_LOUT1_VOLUME_CONTROL + (dac<<1), AK4613_MAX_DIGITAL_VOL - vol[i++], 0 );
            if ( status != EOK ) {
                return status;
            }
            status = ak4613_i2c_write ( AK4613_ROUT1_VOLUME_CONTROL + (dac<<1), AK4613_MAX_DIGITAL_VOL - vol[i++], 0 );
            if ( status != EOK ) {
                return status;
            }
        }
    }

    return EOK;
}

int ak4613_output_mute_set( uint8_t mute )
{
    int status = EOK;
    uint8_t data;

    status = ak4613_i2c_read ( AK4613_CONTROL1, &data, 0 );
    if ( status != EOK ) {
        return status;
    }

    /* Bit 0 of AK4613_CONTROL1 register is the shared mute status of all channels;
     * if mute is non-zero indicating to mute all channels, set bit 0
     * of AK4613_CONTROL1 */
    data &= ~AK4613_CONTROL1_SMUTE_MASK;
    if ( mute ) {
        data |= AK4613_CONTROL1_SMUTE_MASK;
    }

    return ak4613_i2c_write ( AK4613_CONTROL1, data, 0 );
}

int ak4613_output_mute_get( uint8_t* mute )
{
    int status = EOK;
    uint8_t data;

    /* Bit 0 of AK4613_CONTROL1 register is the shared mute status of all channels;
       if non-zero return SND_MIXER_CHN_MASK_STEREO to indicate both channels muted;
       otherwise return 0 to indicate both channels unmuted */
    if ( mute ) {
        status = ak4613_i2c_read ( AK4613_CONTROL1, &data, 0 );
        if ( status == EOK ) {
            *mute = ( (data & AK4613_CONTROL1_SMUTE_MASK) ? 1 : 0 );
        }
    }

    return status;
}

int ak4613_rate_setting( uint32_t sample_rate )
{
    uint8_t data;
    uint8_t tdm = AK4613_CONTROL1_STEREO;
    uint8_t dfs = AK4613_CONTROL2_DFS_NORMAL, new_dfs;
    int status;

    if ( sample_rate == 0 ) {
        return EINVAL;
    }

    /* the DFS selection based on sample_rate is from the reference AK4613 codec
     * source code from Renesas; in slave mode it works fine even if the MCKI rate
     * is not a 128/256/512 multiple of the sample rate (MCKI rate is 50 MHz on
     * Renesas Salvator board); in master mode it is expected that for the below
     * determination to work, the MCKI rate should be the corresponding multiple of
     * the sample rate (512 when DFS_NORMAL is used, 256 when DFS_DOUBLE is used,
     * 128 when DFS_QUAD is used) */
    switch ( sample_rate ) {
        case 32000:
        case 44100:
        case 48000:
            new_dfs = AK4613_CONTROL2_DFS_NORMAL; //512fs
            break;
        case 88200:
        case 96000:
            new_dfs = AK4613_CONTROL2_DFS_DOUBLE; //256fs
            break;
        case 176400:
        case 192000:
            new_dfs = AK4613_CONTROL2_DFS_QUAD; //128fs
            break;
        default:
            return EINVAL;
    }

    status = ak4613_i2c_read ( AK4613_CONTROL1, &data, 0 );
    if ( status == EOK ) {
        tdm = data & AK4613_CONTROL1_TDM_MASK;
    }

    status = ak4613_i2c_read ( AK4613_CONTROL2, &data, 0 );
    if ( status == EOK ) {
        dfs = data & AK4613_CONTROL2_DFS_MASK;
        data &= ~(AK4613_CONTROL2_DFS_MASK|AK4613_CONTROL2_CKS_MASK|AK4613_CONTROL2_ACKS_MASK);
        data |= dfs;
        if ( dfs != new_dfs && tdm != AK4613_CONTROL1_STEREO ) {
            return EINVAL;
        }
        status = ak4613_i2c_write ( AK4613_CONTROL2, data, 0 );
    }

    return status;
}

void ak4613_register_dump( void )
{
    int status = EOK, i;
    uint8_t reg_val = 0;

    status = ak4613_i2c_read ( AK4613_POWER_MANAGEMENT1, &reg_val, 0 );
    if ( status == EOK ) {
        ado_debug ( DB_LVL_DRIVER, "AK4613_POWER_MANAGEMENT1 - 0x%02x", reg_val );
    }

    status = ak4613_i2c_read ( AK4613_POWER_MANAGEMENT2, &reg_val, 0 );
    if ( status == EOK ) {
        ado_debug ( DB_LVL_DRIVER, "AK4613_POWER_MANAGEMENT2 - 0x%02x", reg_val );
    }

    status = ak4613_i2c_read ( AK4613_POWER_MANAGEMENT3, &reg_val, 0 );
    if ( status == EOK ) {
        ado_debug ( DB_LVL_DRIVER, "AK4613_POWER_MANAGEMENT3 - 0x%02x", reg_val );
    }

    status = ak4613_i2c_read ( AK4613_CONTROL1, &reg_val, 0 );
    if ( status == EOK ) {
        ado_debug ( DB_LVL_DRIVER, "AK4613_CONTROL1 - 0x%02x", reg_val );
    }

    status = ak4613_i2c_read ( AK4613_CONTROL2, &reg_val, 0 );
    if ( status == EOK ) {
        ado_debug ( DB_LVL_DRIVER, "AK4613_CONTROL2 - 0x%02x", reg_val );
    }

    status = ak4613_i2c_read ( AK4613_DE_EMPHASIS1, &reg_val, 0 );
    if ( status == EOK ) {
        ado_debug ( DB_LVL_DRIVER, "AK4613_DE_EMPHASIS1 - 0x%02x", reg_val );
    }

    status = ak4613_i2c_read ( AK4613_DE_EMPHASIS2, &reg_val, 0 );
    if ( status == EOK ) {
        ado_debug ( DB_LVL_DRIVER, "AK4613_DE_EMPHASIS2 - 0x%02x", reg_val );
    }

    status = ak4613_i2c_read ( AK4613_OVERFLOW_DETECT, &reg_val, 0 );
    if ( status == EOK ) {
        ado_debug ( DB_LVL_DRIVER, "AK4613_OVERFLOW_DETECT - 0x%02x", reg_val );
    }

    status = ak4613_i2c_read ( AK4613_ZERO_DETECT, &reg_val, 0 );
    if ( status == EOK ) {
        ado_debug ( DB_LVL_DRIVER, "AK4613_ZERO_DETECT - 0x%02x", reg_val );
    }

    status = ak4613_i2c_read ( AK4613_INPUT_CONTROL, &reg_val, 0 );
    if ( status == EOK ) {
        ado_debug ( DB_LVL_DRIVER, "AK4613_INPUT_CONTROL - 0x%08x", reg_val );
    }

    status = ak4613_i2c_read ( AK4613_OUTPUT_CONTROL, &reg_val, 0 );
    if ( status == EOK ) {
        ado_debug ( DB_LVL_DRIVER, "AK4613_OUTPUT_CONTROL - 0x%08x", reg_val );
    }

    for (i = 0; i < AK4613_NUM_DACS; i++) {
        status = ak4613_i2c_read ( AK4613_LOUT1_VOLUME_CONTROL + (i<<1), &reg_val, 0 );
        if ( status == EOK ) {
            ado_debug ( DB_LVL_DRIVER, "AK4613_LOUT%d_VOLUME_CONTROL - 0x%08x", 1 + i, reg_val );
        }
        status = ak4613_i2c_read ( AK4613_ROUT1_VOLUME_CONTROL + (i<<1), &reg_val, 0 );
        if ( status == EOK ) {
            ado_debug ( DB_LVL_DRIVER, "AK4613_ROUT%d_VOLUME_CONTROL - 0x%08x", 1 + i, reg_val );
        }
    }
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/deva/mixer/ak4613/ak4613.c $ $Rev: 842276 $")
#endif
