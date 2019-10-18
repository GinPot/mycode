/*
* $QNXLicenseC:
* Copyright 2017, QNX Software Systems.
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

/* For 64KB translation granule
* 512 MB blocks
* TLB level 2 block descriptor format:
*        bit[0]: valid
*        bit[1]: 0-block,1-table
*        bit[5:2]: MemAttr
*        bit[10]: Access flag
*        bit[47:29]: addr
*/

#ifndef __AARCH64_TLB_H__
#define __AARCH64_TLB_H__

#define PAGE_SHIFT          29
#define PAGE_SIZE           (1 << PAGE_SHIFT)
#define TLB_SIZE            0x2000
#define BA_MASK             0xFFFFE0000000

#define VALID               (0x1 << 0)
#define AF                  (0x1 << 10)

typedef struct aarch64_tlb {
    uint64_t start;
    uint64_t len;
    uint64_t attr;
} aarch64_tlb_t;

/* Construct a translation table to cover 42 bit address space
* Please NOTE that the tlb needs to be 64K aligned (0x10000)
*/
static __inline__ void aarch64_setup_tlb(aarch64_tlb_t *table, uint64_t *tlb)
{
    uint64_t base;
    uint64_t offset = 0;

    while (table && (table->start != -1)) {
       base = table->start;

        /* uncacheable */
        for (; offset < (base >> PAGE_SHIFT); offset++) {
            tlb[offset] = (offset << PAGE_SHIFT) | VALID | AF;
        }

        for (; (base >= table->start) && (table->len > base - table->start) && (offset < TLB_SIZE); base += PAGE_SIZE, offset++) {
            tlb[offset] = (base & BA_MASK)
                            | table->attr
                            | VALID | AF;
        }
        table++;
    }

    /* uncacheable */
    for (; offset < TLB_SIZE; offset++) {
        tlb[offset] = (offset << PAGE_SHIFT) | VALID | AF;
    }
}
#endif


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/aarch64/aarch64_tlb.h $ $Rev: 834894 $")
#endif
