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

/*
 * This source file isn't included in the startup library.
 * It's only used to build the asmoff.def file.
 *
 * CPU specific definitions
*/

#include "asmoff.h"
#include <aarch64/cpu.h>

VALUE(AARCH64_PSTATE_I,		AARCH64_PSTATE_I);

VALUE(EL2_VBAR,		offsetof(struct aarch64_original_el2_regs, vbar));
VALUE(EL2_SCTLR,	offsetof(struct aarch64_original_el2_regs, sctlr));
VALUE(EL2_HCR,		offsetof(struct aarch64_original_el2_regs, hcr));
VALUE(EL2_VTTBR,	offsetof(struct aarch64_original_el2_regs, vttbr));
VALUE(EL2_VTCR,		offsetof(struct aarch64_original_el2_regs, vtcr));
VALUE(EL2_TCR,		offsetof(struct aarch64_original_el2_regs, tcr));
VALUE(EL2_TTBR0,	offsetof(struct aarch64_original_el2_regs, ttbr0));
VALUE(EL2_TPIDR,	offsetof(struct aarch64_original_el2_regs, tpidr));
VALUE(EL2_SP,		offsetof(struct aarch64_original_el2_regs, sp));

VALUE(AARCH64_ESR_EC_SMC_64, AARCH64_ESR_EC_SMC_64);


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/aarch64/asmoff.c $ $Rev: 817976 $")
#endif
