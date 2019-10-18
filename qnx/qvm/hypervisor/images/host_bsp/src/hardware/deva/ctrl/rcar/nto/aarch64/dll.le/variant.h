/*
 * $QNXLicenseC:
 * Copyright 2016 QNX Software Systems.
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

extern int variant_init ( HW_CONTEXT_T * rcar );
extern int variant_uninit ( HW_CONTEXT_T *rcar );
extern int variant_set_clock_rate( uint32_t mclk_rate );
extern void cs2000_register_dump();

#define DEFAULT_SSI_MASTERSLAVE_MODE    SSI_MASTER_SLAVE
#define DEFAULT_SSI_OP_MODE             SSI_OP_MODE_STEREO
#define DEFAULT_USE_SCU                 1               /* enable scu */
#define DEFAULT_SLOT_SIZE               32              /* 32-bit */
#define DEFAULT_SAMPLE_SIZE             16              /* 16-bit */
#define DEFAULT_VOICES                  2
#define DEFAULT_SSI_CHANNEL_PLAYBACK    SSI_CHANNEL_NUM /* do not specify a default value, to allow for a capture only use case */
#define DEFAULT_SSI_CHANNEL_CAPTURE     SSI_CHANNEL_NUM /* do not specify a default value, to allow for a playback only use case */
#define SAMPLE_RATE_SRC                 48000
#define SAMPLE_RATE_MIN                 8000
#define SAMPLE_RATE_MAX                 48000
#define MCLK_1                          22579200
#define MCLK_SOURCE_1                   AUDIO_CLKA
#define MCLK_2                          24576000
#define MCLK_SOURCE_2                   AUDIO_CLKB
#define MCLK_FS                         0
#define MCLK_SOURCE_FS                  AUDIO_CLK_NONE

#define VARIANT_SET_CLOCK_RATE(mclk_rate, sample_rate) variant_set_clock_rate(mclk_rate)
#define VARIANT_REGISTER_DUMP()                        cs2000_register_dump()
#define VARIANT_INIT(rcar)                             variant_init(rcar)
#define VARIANT_UNINIT(rcar)                           variant_uninit(rcar)

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/deva/ctrl/rcar/nto/aarch64/dll.le/variant.h $ $Rev: 842276 $")
#endif
