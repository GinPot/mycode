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
 *       Used by Hypervisor guests running on a57 boards.
 *       The Hypervisor does not recognize the Cortex A57's
 *       CP15 CPU Extended Control Register, and so we use here the
 *       armv_setup_a15 instead.
 */
const struct armv_chip armv_chip_hyp_a57 = {
	.cpuid		= 0xd070,
	.name		= "Cortex A57 Guest",
	.cycles		= 2,
	.power		= &power_v7_wfi,
	.setup		= armv_setup_a15,
};

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/arm/armv_chip_hyp_a57.c $ $Rev: 829879 $")
#endif

