/*
 * $QNXLicenseC:
 * Copyright 2018, QNX Software Systems.
 * Copyright 2018, NVIDIA CORPORATION.
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

#include "startup.h"

static void
d20_cpuinfo(unsigned cpunum, struct cpuinfo_entry *info)
{
    /*
     * Handle D20 Errata workarounds
     */

    /* Keep callouts and cpusyspage in separate pages */
    syspage_roundup = 4*1024;
}

struct aarch64_cpuid    cpuid_d20 = {
    .midr = 0x4e000040,
    .name = "Denver-D20",
    .cpuinfo = d20_cpuinfo
};


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/aarch64/cpuid_d20.c $ $Rev: 850216 $")
#endif
