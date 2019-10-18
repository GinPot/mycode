/*
 * $QNXLicenseC:
 * Copyright 2015, 2016 QNX Software Systems.
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

#ifndef _R_Car_ADG_H
#define _R_Car_ADG_H

#include <stdint.h>

typedef enum
{
    AUDIO_CLK_NONE,
    AUDIO_CLKA,
    AUDIO_CLKB,
    AUDIO_CLKC,
    AUDIO_MLPCLK
} adg_clk_src_t;

int adg_init(void);
void adg_deinit(void);
int adg_set_ssi_clksrc(uint32_t ssi_channel, uint32_t clk);
int adg_set_ssi_clkdiv(uint32_t ssi_channel, uint32_t divisor);
int adg_set_src_clksrc(uint32_t src_channel, uint32_t ssi_channel, uint32_t playback);
int adg_set_cmd_clksrc(uint32_t cmd_channel, uint32_t ssi_channel);
void adg_register_dump(void);

#endif /* _R_Car_ADG_H */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/deva/ctrl/rcar/adg.h $ $Rev: 842276 $")
#endif
