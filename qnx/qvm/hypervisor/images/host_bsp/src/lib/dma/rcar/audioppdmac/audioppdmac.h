/*
 * $QNXLicenseC:
 * Copyright 2015, QNX Software Systems.
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

#ifndef __RCAR_AUDIOPPDMAC_H__
#define __RCAR_AUDIOPPDMAC_H__

#include <inttypes.h>
#include <sys/neutrino.h>
#include <sys/mman.h>

typedef struct
{
    uint32_t        chan_idx;
    uint32_t        grp_idx;
    paddr_t         pbase;
    paddr_t         vbase;
    uint32_t        size;
    uintptr_t       regs;
    uint32_t        channels;
    uint32_t        channel_groups;
} dma_channel_t;

typedef struct
{
    paddr_t         paddr;
    uintptr_t       vaddr;
    unsigned        num;    // attach count
} audioppdmac_ctrl_t;

typedef enum {
   RCAR_H2,
   RCAR_E2,
   RCAR_M2,
   RCAR_V2,
   RCAR_W2H,
   RCAR_H3,
   RCAR_M3,
   RCAR_V3,
   RCAR_D3,
   RCAR_UNKNOWN
} rcar_soc_t;


#endif /* #ifndef __RCAR_AUDIOPPDMAC_H__ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/lib/dma/rcar/audioppdmac/audioppdmac.h $ $Rev: 850778 $")
#endif
