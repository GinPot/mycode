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

#include "startup.h"

/*
 *       Used by Hypervisor guests running on different flavours of
 *       Qualcomm 820A boards. The entries for a bare-metal system
 *       may have different requirements (if those boards are one day integrated),
 *       which is why they are not in their own file.
 */
const struct armv_chip armv_chip_hyp_qc2 = {
        .cpuid          = 0x2050,
        .name           = "Armv8 QC850-2 Guest",
        .cycles         = 2,
        .power          = &power_v7_wfi,
        .setup          = armv_setup_a15,
};


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/arm/armv_chip_hyp_qc2.c $ $Rev: 829879 $")
#endif

