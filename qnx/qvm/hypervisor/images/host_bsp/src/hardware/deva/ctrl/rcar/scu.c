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
#include "scu.h"
#include "rcar_support.h"

/*
 * Bit field mappings for CMDx_ROUTE_SELECT register
 *
 * 19-31:  Not used:     set to 0
 * 16-18:  CMD_CASE_SEL: Select the route in CMD (select audio data 0,1,2,3 or CTU-MIX path)
 * 9-15:   Not used:     set to 0
 * 8:      CMDIN_CTU3:   Select SRC to connect to input of audio data 3
 * 1-7:    Not used:     set to 0
 * 0:      CMDIN_CTU2:   Select SRC to connect to input of audio data 2
 *
 */
#define CMD_ROUTE_SELECT_CMDCASESEL_MASK       (0x7<<16)
#define CMD_ROUTE_SELECT_CMDCASESEL_CTUMIXDVC  0
#define CMD_ROUTE_SELECT_CMDCASESEL_SRC3DVC    (0x1<<16)
#define CMD_ROUTE_SELECT_CMDCASESEL_SRC4DVC    (0x2<<16)
#define CMD_ROUTE_SELECT_CMDCASESEL_SRC01DVC   (0x3<<16)
#define CMD_ROUTE_SELECT_CMDCASESEL_SRC25DVC   (0x4<<16)
#define CMD_ROUTE_SELECT_CMDINCTU3_MASK        (0x1<<8)
#define CMD_ROUTE_SELECT_CMDINCTU3_SRC2        0
#define CMD_ROUTE_SELECT_CMDINCTU3_SRC5        (0x1<<8)
#define CMD_ROUTE_SELECT_CMDINCTU2_MASK        0x1
#define CMD_ROUTE_SELECT_CMDINCTU2_SRC0        0
#define CMD_ROUTE_SELECT_CMDINCTU2_SRC1        0x1

/* structure definitions for memory mapped SRC, CMD and DVC registers */
/* CTU and MIX registers are not currently mapped, CTU/MIX path is
   currently not supported - TBA */
typedef struct
{
    volatile uint32_t in_busif_mode;
    volatile uint32_t out_busif_mode;
    volatile uint32_t busif_dalign;
    volatile uint32_t mode;
    volatile uint32_t control;
    volatile uint32_t status;
    volatile uint32_t int_enable0;
    volatile uint32_t dummy;
} scu_scusrc_reg_t;

typedef struct
{
    volatile uint32_t swrsr;
    volatile uint32_t srcir;
    volatile uint32_t dummy1[3];
    volatile uint32_t adinr;
    volatile uint32_t dummy2;
    volatile uint32_t ifscr;
    volatile uint32_t ifsvr;
    volatile uint32_t srccr;
    volatile uint32_t dummy3;
    volatile uint32_t bsdsr;
    volatile uint32_t dummy4[2];
    volatile uint32_t bsisr;
    volatile uint32_t dummy5;
} scu_src_reg_t;

typedef struct
{
    volatile uint32_t out_busif_mode;
    volatile uint32_t busif_dalign;
    volatile uint32_t route_select;
    volatile uint32_t control;
    volatile uint32_t dummy[4];
} scu_cmd_reg_t;

typedef struct
{
    volatile uint32_t swrsr;
    volatile uint32_t dvuir;
    volatile uint32_t adinr;
    volatile uint32_t dummy1;
    volatile uint32_t dvucr;
    volatile uint32_t zcmcr;
    volatile uint32_t vrctr;
    volatile uint32_t vrpdr;
    volatile uint32_t vrdbr;
    volatile uint32_t vrwtr;
    volatile uint32_t vol[8];
    volatile uint32_t dvuer;
    volatile uint32_t dvusr;
    volatile uint32_t dvier;
    volatile uint32_t dummy2[43];
} scu_dvc_reg_t;


static scu_scusrc_reg_t *rcar_scusrc = MAP_FAILED;
static scu_src_reg_t    *rcar_src = MAP_FAILED;
static scu_cmd_reg_t    *rcar_cmd = MAP_FAILED;
static scu_dvc_reg_t    *rcar_dvc = MAP_FAILED;

/* the volume table below contains the linear volume corresponding
   to dB values from -120 dB to 0 dB in 0.25 dB increments
   for a total of 481 values */
static uint32_t scu_db_to_linear_vol_tbl[] = {
1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,
3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,
4,4,4,5,5,5,5,5,5,5,6,6,6,6,6,6,
7,7,7,7,7,8,8,8,8,9,9,9,9,10,10,10,
10,11,11,11,12,12,12,13,13,14,14,14,
15,15,16,16,17,17,18,18,19,19,20,20,
21,22,22,23,23,24,25,26,26,27,28,29,
30,30,31,32,33,34,35,36,37,38,39,41,
42,43,44,46,47,48,50,51,53,54,56,57,
59,61,62,64,66,68,70,72,74,76,79,81,
83,86,88,91,93,96,99,102,105,108,111,114,
118,121,125,128,132,136,140,144,148,152,
157,161,166,171,176,181,186,192,198,203,
209,215,222,228,235,242,249,256,263,271,
279,287,296,304,313,322,332,341,351,361,
372,383,394,406,417,430,442,455,468,482,
496,511,526,541,557,573,590,607,625,643,
662,681,701,721,742,764,786,809,833,857,
882,908,935,962,990,1019,1049,1079,1111,1143,
1177,1211,1246,1283,1320,1359,1398,1439,
1481,1524,1569,1615,1662,1710,1760,1812,
1865,1919,1975,2033,2092,2153,2216,2281,
2347,2416,2487,2559,2634,2711,2790,2871,
2955,3042,3130,3222,3316,3413,3512,3615,
3720,3829,3941,4056,4174,4296,4422,4551,
4684,4821,4961,5106,5255,5409,5567,5729,
5897,6069,6246,6428,6616,6809,7008,7213,
7423,7640,7863,8093,8329,8572,8823,9080,
9345,9618,9899,10188,10486,10792,11107,11431,
11765,12109,12462,12826,13201,13586,
13983,14391,14812,15244,15689,16147,
16619,17104,17604,18118,18647,19191,
19751,20328,20922,21533,22162,22809,
23475,24160,24866,25592,26339,27108,
27900,28714,29553,30416,31304,32218,
33159,34127,35124,36149,37205,38291,
39409,40560,41745,42964,44218,45509,
46838,48206,49614,51062,52553,54088,
55667,57293,58966,60688,62460,64284,
66161,68093,70081,72127,74233,76401,
78632,80928,83291,85724,88227,90803,
93454,96183,98992,101883,104858,107919,
111071,114314,117652,121088,124623,
128263,132008,135863,139830,143913,
148115,152440,156892,161473,166188,
171041,176035,181176,186466,191911,
197515,203282,209218,215328,221615,
228087,234747,241602,248657,255917,
263390,271082,278997,287144,295529,
304158,313040,322181,331589,341271,
351237,361493,372049,382913,394094,
405602,417446,429635,442181,455093,
468382,482059,496135,510622,525533,
540879,556673,572928,589658,606876,
624597,642836,661607,680926,700809,
721273,742335,764012,786321,809282,
832914,857235,882267,908029,934544,
961834,989920,1018826,1048576 };

uint32_t scu_dvc_get_max_vol( void )
{
    return sizeof(scu_db_to_linear_vol_tbl)/sizeof(scu_db_to_linear_vol_tbl[0]) - 1;
}

int scu_init()
{
    unsigned long base = RCAR_GEN2_SCU_BASE;
    rcar_query_hwinfo(RCAR_HWI_AUDIO_SCU, &base);

    if ((rcar_scusrc = (scu_scusrc_reg_t *)ado_device_mmap (base, sizeof(scu_scusrc_reg_t))) == MAP_FAILED)
    {
        ado_error_fmt ("SCU registers map failed");
        rcar_src = MAP_FAILED;
        rcar_cmd = MAP_FAILED;
        rcar_dvc = MAP_FAILED;
        return ENOMEM;
    }

    /* SCU SRC registers */
    rcar_src = (scu_src_reg_t *)((uintptr_t)rcar_scusrc + 0x200);

    /* SCU CMD registers */
    rcar_cmd = (scu_cmd_reg_t *)((uintptr_t)rcar_scusrc + 0x184);

    /* SCU DVC registers */
    rcar_dvc = (scu_dvc_reg_t *)((uintptr_t)rcar_scusrc + 0xE00);

    return EOK;
}

void scu_deinit()
{
    if (rcar_scusrc != MAP_FAILED ) {
        ado_device_munmap(rcar_scusrc, sizeof(scu_scusrc_reg_t));
        rcar_scusrc = MAP_FAILED;
    }
}

int scu_src_reset(uint32_t src_channel)
{
    if( !rcar_src_supported(src_channel) ) {
        ado_error_fmt("SRC %d is not supported", src_channel);
        return EINVAL;
    }
    if (rcar_scusrc == MAP_FAILED ) {
        ado_error_fmt("SCU memory is not mapped");
        return EFAULT;
    }

    rcar_src[src_channel].swrsr = 0;    // reset src
    rcar_src[src_channel].swrsr = 1;    // src operates

    return EOK;
}

int scu_dvc_reset(uint32_t cmd_channel)
{
    if ( !rcar_cmd_supported( cmd_channel ) ) {
        ado_error_fmt("CMD %d is not supported", cmd_channel);
        return EINVAL;
    }
    if (rcar_scusrc == MAP_FAILED ) {
        ado_error_fmt("SCU memory is not mapped");
        return EFAULT;
    }
    rcar_dvc[cmd_channel].swrsr = 0;    // reset src
    rcar_dvc[cmd_channel].swrsr = 1;    // src operates

    return 0;
}


static uint32_t scu_src_calc_bsdsr(uint32_t ratio)
{
    uint32_t val;

    /* check FSO/FSI ratio */
    /*  1/4=25, 1/3=33, 1/2=50, 2/3=66, 1/1=100  */
    if (ratio < 25) {
        val = (0x180);
    } else if (ratio >= 25 && ratio < 33) {
        val = (0x100);
    } else if (ratio >= 33 && ratio < 50) {
        val = (0x0c0);
    } else if (ratio >= 50 && ratio < 66) {
        val = (0x080);
    } else if (ratio >= 66 && ratio < 100) {
        val = (0x060);
    } else { /* ratio >= 100 */
        val = (0x040);
    }
    return val;
}

static uint32_t scu_src_calc_bsisr(uint32_t ratio)
{
    uint32_t val;

    /* check FSO/FSI ratio */
    /* 1/4=25, 1/3=33, 1/2=50, 2/3=66, 1/1=100 */
    if (ratio < 25) {
        val = 0x60;
    } else if (ratio >= 25 && ratio < 33) {
        val = 0x40;
    } else if (ratio >= 33 && ratio < 50) {
        val = 0x30;
    } else if (ratio >= 50 && ratio < 66) {
        val = 0x20;
    } else if (ratio >= 66 && ratio < 100) {
        val = 0x20;
    } else { /* ratio >= 100 */
        val = 0x20;
    }
    return val;
}

int scu_src_setup
(
    uint32_t src_channel,
    uint32_t in_sync,
    uint32_t out_sync,
    uint32_t bitnum,
    uint32_t voicenum,
    uint32_t freq_in,
    uint32_t freq_out
)
{
    uint64_t    val;
    uint32_t    ratio;

    if( !rcar_src_supported(src_channel) ) {
        ado_error_fmt("SRC %d is not supported", src_channel);
        return EINVAL;
    }
    if (rcar_scusrc == MAP_FAILED ) {
        ado_error_fmt("SCU memory is not mapped");
        return EFAULT;
    }

    scu_src_reset(src_channel);

    /* Setup scu src interface */
    rcar_scusrc[src_channel].mode = ((out_sync & 1) << 25)  // output buffer asynchronous
                               | ((in_sync & 1) << 24)  // input buffer synchronous
                               | (0 << 16)  // all 0 when underflow
                               | (1 << 0);  // SRC is used
    rcar_scusrc[src_channel].int_enable0 = 0;   // Disable interrupts

    /* Setup scu src */
    rcar_src[src_channel].srcir = 1;    // src initialization
    rcar_src[src_channel].adinr = (((24 - bitnum) & 0x1F) << 16)    // output:data bit number
                             | ((voicenum & 0xF) << 0);     // channels number
    rcar_src[src_channel].ifscr = 1;    // Enable INTIFS

    val = (uint64_t)0x0400000 * freq_in / freq_out;
    rcar_src[src_channel].ifsvr = (uint32_t)val;

    //val = (uint64_t)val * 98 / 100;
    //rcar_src[src_channel].mnfsr = (uint32_t)val; //mnfsr deprecated

    if (in_sync || out_sync) {
        rcar_src[src_channel].srccr = 0x11111;  // synchronous SRC
    } else {
        rcar_src[src_channel].srccr = 0x11110;  // asynchronous SRC
    }
    ratio = freq_out * 100 / freq_in;
    val = scu_src_calc_bsdsr(ratio);
    rcar_src[src_channel].bsdsr = ((uint32_t)val << 16);

    val = scu_src_calc_bsisr(ratio);
    rcar_src[src_channel].bsisr = (0x10 << 16) | ((uint32_t)val << 0);

    rcar_src[src_channel].srcir = 0;

    return 0;
}

int scu_src_cleanup(uint32_t src_channel)
{
    if( !rcar_src_inline_supported(src_channel) ) {
        ado_error_fmt("SRC %d is not supported", src_channel);
        return EINVAL;
    }
    if (rcar_scusrc == MAP_FAILED ) {
        ado_error_fmt("SCU memory is not mapped");
        return EFAULT;
    }

    rcar_scusrc[src_channel].control = 0;
    rcar_src[src_channel].srcir = 1;
    rcar_scusrc[src_channel].mode = 0;
    rcar_scusrc[src_channel].int_enable0 = 0;
    rcar_src[src_channel].adinr = 0;
    rcar_src[src_channel].ifscr = 0;
    rcar_src[src_channel].ifsvr = 0;
    //rcar_src[src_channel].mnfsr = 0; // mnfsr deprecated
    rcar_src[src_channel].srccr = 0x11110;
    rcar_src[src_channel].bsdsr = 0;
    rcar_src[src_channel].bsisr = 0;

    return EOK;
}

/* CMD routing set-up between indicated SRC and CMD channel -
   does not currently support CTU/MIX path */
int scu_cmd_setup(uint32_t cmd_channel, uint32_t src_channel)
{
    if( !rcar_cmd_supported(cmd_channel) ) {
        ado_error_fmt("CMD %d is not supported", cmd_channel);
        return EINVAL;
    }
    if( !rcar_src_inline_supported(src_channel) ) {
        ado_error_fmt("SRC %d is not supported", src_channel);
        return EINVAL;
    }
    if (rcar_scusrc == MAP_FAILED ) {
        ado_error_fmt("SCU memory is not mapped");
        return EFAULT;
    }

    /* Setup scu cmd */
    switch( src_channel ) {
        case 0:
            rcar_cmd[cmd_channel].route_select = CMD_ROUTE_SELECT_CMDCASESEL_SRC01DVC |
                                                 CMD_ROUTE_SELECT_CMDINCTU2_SRC0;
            break;
        case 1:
            rcar_cmd[cmd_channel].route_select = CMD_ROUTE_SELECT_CMDCASESEL_SRC01DVC |
                                                 CMD_ROUTE_SELECT_CMDINCTU2_SRC1;
            break;
        case 2:
            rcar_cmd[cmd_channel].route_select = CMD_ROUTE_SELECT_CMDCASESEL_SRC25DVC |
                                                 CMD_ROUTE_SELECT_CMDINCTU3_SRC2;
            break;
        case 3:
            rcar_cmd[cmd_channel].route_select = CMD_ROUTE_SELECT_CMDCASESEL_SRC3DVC;
            break;
        case 4:
            rcar_cmd[cmd_channel].route_select = CMD_ROUTE_SELECT_CMDCASESEL_SRC4DVC;
            break;
        case 5:
            rcar_cmd[cmd_channel].route_select = CMD_ROUTE_SELECT_CMDCASESEL_SRC25DVC |
                                                 CMD_ROUTE_SELECT_CMDINCTU3_SRC5;
            break;
        default:
            break;
    }

    return EOK;
}

int scu_dvc_setup
(
    uint32_t cmd_channel,
    uint32_t bitnum,
    uint32_t voicenum,
    uint32_t *vol,
    uint32_t mute
)
{
    int i;

    if( !rcar_cmd_supported(cmd_channel) ) {
        ado_error_fmt("CMD %d is not supported", cmd_channel);
        return EINVAL;
    }
    if (rcar_scusrc == MAP_FAILED ) {
        ado_error_fmt("SCU memory is not mapped");
        return EFAULT;
    }

    scu_dvc_reset(cmd_channel);

    /* Setup scu dvu */
    rcar_dvc[cmd_channel].dvuir = 1;  // src initialization

    rcar_dvc[cmd_channel].adinr =
        (((24 - bitnum) & 0x1F) << 16)    // output:data bit number
        | ((voicenum & 0xF) << 0);        // 1->8 channels

    rcar_dvc[cmd_channel].dvucr = (0 << 16) |   // Disables the DVC_MUTE pin
                              (1 << 8) |    // Use the digital volume value function
                              (0 << 4) |    // No Use the volume ramp function
                              (1 << 0);     // Use the zero cross mute function
    rcar_dvc[cmd_channel].zcmcr = mute;
    rcar_dvc[cmd_channel].vrctr = 0;    // Enable the volume ramp function for all channel
    rcar_dvc[cmd_channel].vrpdr = 0;    // Volume Ramp Period for Volume Up/Down
    rcar_dvc[cmd_channel].vrdbr = 0;    // Control the decibel (gain level) of volume ramp
    rcar_dvc[cmd_channel].vrwtr = 0;    // Control the standby time to start the volume ramp function
    for (i = 0; i < 8; i++) {
        if (vol[i] > SCU_DVC_MAX_VOLUME) {
            vol[i] = SCU_DVC_MAX_VOLUME;
        }
        rcar_dvc[cmd_channel].vol[i] = scu_db_to_linear_vol_tbl[vol[i]];
    }
    rcar_dvc[cmd_channel].dvuir = 0;
    rcar_dvc[cmd_channel].dvuer = 1;    // dvu enable

    return EOK;
}

int scu_dvc_get_vol(uint32_t cmd_channel, uint32_t voice_channel, uint32_t* vol)
{
    if( !rcar_cmd_supported(cmd_channel) ) {
        ado_error_fmt("CMD %d is not supported", cmd_channel);
        return EINVAL;
    }
    if (voice_channel >= 8)
    {
        ado_error_fmt("voice channel %d is not supported", voice_channel);
        return EINVAL;
    }
    if (rcar_scusrc == MAP_FAILED ) {
        ado_error_fmt("SCU memory is not mapped");
        return EFAULT;
    }

    *vol = rcar_dvc[cmd_channel].vol[voice_channel];  // volume

    return EOK;
}

int scu_dvc_set_vol(uint32_t cmd_channel, uint32_t voice_channel, uint32_t vol)
{
    if( !rcar_cmd_supported(cmd_channel) ) {
        ado_error_fmt("CMD %d is not supported", cmd_channel);
        return EINVAL;
    }
    if (voice_channel >= 8)
    {
        ado_error_fmt("voice channel %d is not supported", voice_channel);
        return EINVAL;
    }
    if (rcar_scusrc == MAP_FAILED ) {
        ado_error_fmt("SCU memory is not mapped");
        return EFAULT;
    }
    if (vol >= sizeof(scu_db_to_linear_vol_tbl)/sizeof(scu_db_to_linear_vol_tbl[0])) {
        ado_error_fmt("volume value %d is not supported", vol);
        return EINVAL;
    }

    rcar_dvc[cmd_channel].vol[voice_channel] = scu_db_to_linear_vol_tbl[vol];

    return EOK;
}

int scu_dvc_get_mute(uint32_t cmd_channel, uint32_t* mute)
{
    if (!rcar_cmd_supported(cmd_channel)) {
        ado_error_fmt("CMD %d is not supported", cmd_channel);
        return EINVAL;
    }
    if (rcar_scusrc == MAP_FAILED ) {
        ado_error_fmt("SCU memory is not mapped");
        return EFAULT;
    }

    *mute = rcar_dvc[cmd_channel].zcmcr;

    return EOK;
}

int scu_dvc_set_mute(uint32_t cmd_channel, uint32_t mute)
{
    if (!rcar_cmd_supported(cmd_channel)) {
        ado_error_fmt("CMD %d is not supported", cmd_channel);
        return EINVAL;
    }
    if (rcar_scusrc == MAP_FAILED ) {
        ado_error_fmt("SCU memory is not mapped");
        return EFAULT;
    }
    if (mute & ~0xFF) {
        ado_error_fmt("Invalid mute setting 0x%x, DVC only supports 8 voices", mute);
    }
    rcar_dvc[cmd_channel].zcmcr = mute;

    return EOK;
}

int scu_dvc_cleanup(uint32_t cmd_channel)
{
    int i;

    if( !rcar_cmd_supported(cmd_channel) ) {
        ado_error_fmt("CMD %d is not supported", cmd_channel);
        return EINVAL;
    }
    if (rcar_scusrc == MAP_FAILED ) {
        ado_error_fmt("SCU memory is not mapped");
        return EFAULT;
    }

    rcar_dvc[cmd_channel].dvuir = 1;
    rcar_dvc[cmd_channel].adinr = 0;
    rcar_dvc[cmd_channel].dvucr = 0;
    rcar_dvc[cmd_channel].zcmcr = 0;
    rcar_dvc[cmd_channel].vrctr = 0;
    rcar_dvc[cmd_channel].vrpdr = 0;
    rcar_dvc[cmd_channel].vrdbr = 0;
    rcar_dvc[cmd_channel].vrwtr = 0;
    for (i = 0; i < 8; i++)
    {
        rcar_dvc[cmd_channel].vol[i] = 0;
    }
    rcar_dvc[cmd_channel].dvuir = 0;
    rcar_dvc[cmd_channel].dvuer = 0;

    return EOK;
}

int scu_src_start(uint32_t src_channel)
{
    if( !rcar_src_supported(src_channel) ) {
        ado_error_fmt("SRC %d is not supported", src_channel);
        return EINVAL;
    }
    if (rcar_scusrc == MAP_FAILED ) {
        ado_error_fmt("SCU memory is not mapped");
        return EFAULT;
    }

    /* for inline SRCs it might be enough to use 0x10
       for non-inline SRCs both bits must be used */
    rcar_scusrc[src_channel].control = 0x11;

    return EOK;
}

int scu_src_stop(uint32_t src_channel)
{
    if( !rcar_src_supported(src_channel) ) {
        ado_error_fmt("SRC %d is not supported", src_channel);
        return EINVAL;
    }
    if (rcar_scusrc == MAP_FAILED ) {
        ado_error_fmt("SCU memory is not mapped");
        return EFAULT;
    }

    rcar_scusrc[src_channel].control = 0;

    return EOK;
}

int scu_cmd_start(uint32_t cmd_channel)
{
    if( !rcar_cmd_supported(cmd_channel) ) {
        ado_error_fmt("CMD %d is not supported", cmd_channel);
        return EINVAL;
    }
    if (rcar_scusrc == MAP_FAILED ) {
        ado_error_fmt("SCU memory is not mapped");
        return EFAULT;
    }

    rcar_cmd[cmd_channel].control = 0x10;

    return EOK;
}

int scu_cmd_stop(uint32_t cmd_channel)
{
    if( !rcar_cmd_supported(cmd_channel) )
    {
        ado_error_fmt("CMD%d is not supported", cmd_channel);
        return EINVAL;
    }
    if (rcar_scusrc == MAP_FAILED ) {
        ado_error_fmt("SCU memory is not mapped");
        return EFAULT;
    }

    rcar_cmd[cmd_channel].control = 0;

    return EOK;
}

/* register dump functionality */
void scu_src_register_dump( uint32_t src_channel )
{
    if( !rcar_src_supported(src_channel) ) {
        ado_error_fmt("SRC%d is not supported", src_channel);
        return;
    }
    if (rcar_scusrc == MAP_FAILED ) {
        ado_error_fmt("SCU memory is not mapped");
        return;
    }

    ado_debug( DB_LVL_DRIVER, "SCU SRC%d reg dump: SWRSR=%x SRCIR=%x ADINR=%x IFSCR=%x",
               src_channel, rcar_src[src_channel].swrsr, rcar_src[src_channel].srcir,
               rcar_src[src_channel].adinr, rcar_src[src_channel].ifscr);
    ado_debug( DB_LVL_DRIVER, "SCU SRC%d reg dump: IFSVR=%x SRCCR=%x BSDSR=%x BSISR=%x",
               src_channel, rcar_src[src_channel].ifsvr, rcar_src[src_channel].srccr,
               rcar_src[src_channel].bsdsr, rcar_src[src_channel].bsisr);

    ado_debug( DB_LVL_DRIVER, "SCU SCUSRC%d reg dump: IN_BUSIF_MODE=%x OUT_BUSIF_MODE=%x BUSIF_DALIGN=%x",
               src_channel, rcar_scusrc[src_channel].in_busif_mode,
               rcar_scusrc[src_channel].out_busif_mode, rcar_scusrc[src_channel].busif_dalign );
    ado_debug( DB_LVL_DRIVER, "SCU SCUSRC%d reg dump: MODE=%x CONTROL=%x STATUS=%x INT_ENABLE0=%x",
               src_channel, rcar_scusrc[src_channel].mode, rcar_scusrc[src_channel].control,
               rcar_scusrc[src_channel].status, rcar_scusrc[src_channel].int_enable0 );
}

void scu_cmd_register_dump( uint32_t cmd_channel )
{
    if( !rcar_cmd_supported(cmd_channel) ) {
        ado_error_fmt("CMD%d is not supported", cmd_channel);
        return;
    }
    ado_debug( DB_LVL_DRIVER, "SCU CMD%d reg dump: OUT_BUSIF_MODE=%x BUSIF_DALIGN=%x",
               cmd_channel, rcar_cmd[cmd_channel].out_busif_mode, rcar_cmd[cmd_channel].busif_dalign );
    ado_debug( DB_LVL_DRIVER, "SCU CMD%d reg dump: ROUTE_SELECT=%x CONTROL=%x",
               cmd_channel, rcar_cmd[cmd_channel].route_select, rcar_cmd[cmd_channel].control );
}

void scu_dvc_register_dump( uint32_t cmd_channel )
{
    /* the DVC is part of the CMD, use the cmd_channel index */
    if( !rcar_cmd_supported(cmd_channel) ) {
        ado_error_fmt("CMD%d is not supported", cmd_channel);
        return;
    }

    ado_debug( DB_LVL_DRIVER, "SCU DVC%d reg dump: SWRSR=%x DVUIR=%x ADINR=%x DVUCR=%x",
               cmd_channel, rcar_dvc[cmd_channel].swrsr, rcar_dvc[cmd_channel].dvuir,
               rcar_dvc[cmd_channel].adinr, rcar_dvc[cmd_channel].dvucr );

    ado_debug( DB_LVL_DRIVER, "SCU DVC%d reg dump: ZCMCR=%x VRCTR=%x VRPDR=%x VRDBR=%x VRWTR=%x",
               cmd_channel, rcar_dvc[cmd_channel].zcmcr,
               rcar_dvc[cmd_channel].vrctr, rcar_dvc[cmd_channel].vrpdr,
               rcar_dvc[cmd_channel].vrdbr, rcar_dvc[cmd_channel].vrwtr );

    ado_debug( DB_LVL_DRIVER, "SCU DVC%d reg dump: VOL=%x %x %x %x %x %x %x %x",
               cmd_channel, rcar_dvc[cmd_channel].vol[0], rcar_dvc[cmd_channel].vol[1],
               rcar_dvc[cmd_channel].vol[2], rcar_dvc[cmd_channel].vol[3],
               rcar_dvc[cmd_channel].vol[4], rcar_dvc[cmd_channel].vol[5],
               rcar_dvc[cmd_channel].vol[6], rcar_dvc[cmd_channel].vol[7] );

    ado_debug( DB_LVL_DRIVER, "SCU DVC%d reg dump: DVUER=%x DVUSR=%x DVIER=%x",
               cmd_channel, rcar_dvc[cmd_channel].dvuer,
               rcar_dvc[cmd_channel].dvusr, rcar_dvc[cmd_channel].dvier );
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/deva/ctrl/rcar/scu.c $ $Rev: 837440 $")
#endif
