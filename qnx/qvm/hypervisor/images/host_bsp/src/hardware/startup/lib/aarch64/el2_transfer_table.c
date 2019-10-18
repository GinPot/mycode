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

#include "startup.h"


static paddr_t	el2_vbar;
int	trying_hvc;

static void
el2_hvc_check(paddr_t dummy) {
	//let el2_alloc_transfer_table() know that the CPU generated
	//the proper HVC exception type by clearing the trying_hvc variable
	trying_hvc = 0;
}


void
el2_transfer_table_allocate(void) {
	if(have_el2) {
		trying_hvc = 1;
		register void (*func) asm ("x1") = el2_hvc_check;
		register uintptr_t parm asm ("x0") = 0;
		asm volatile ("hvc #0" :: "r" (parm), "r" (func) : "x2", "memory");
		if(trying_hvc) {
			// If trying_hvc is still set after the instruction, the HVC
			// opcode is not enabled/supported (e.g. EL3 has it turned off).
			have_el2 = 0;
			trying_hvc = 0;
			return;
		}
		// allocate an area for the EL2 exception vector table
		el2_vbar = alloc_ram(NULL_PADDR, 0x800, 0x800);
		if(el2_vbar == NULL_PADDR) {
			crash("No memory for EL2 exception table.\n");
		}
		// Stash the original EL2 register values in the new vbar
		// region (we're never going to actually take an exception
		// that would transfer to this offset).
		*(struct aarch64_original_el2_regs *)el2_vbar = original_el2_regs;
		//This next line is a temporary change to let procnto know that
		//we've passed the original EL2 register values. It can be removed
		//after a suitable time interval 2016/10/26
		((struct aarch64_original_el2_regs *)el2_vbar)[1].vbar = 0x1122334455667788;
		as_add(el2_vbar, el2_vbar+0x7ff, AS_ATTR_RAM, "hypervisor_vector",
				as_find(AS_NULL_OFF, "memory", NULL));
	}
}


static void
el2_activate(paddr_t vbar) {
	// Switch the EL2 exception table to the transfer location
	aa64_sr_wr64(vbar_el2, vbar);
	// Turn on the instruction and data caches for EL2 execution
	aa64_sr_wr64(sctlr_el2,
			aa64_sr_rd64(sctlr_el2) | (AARCH64_SCTLR_EL1_I|AARCH64_SCTLR_EL1_C));
}



void
el2_transfer_table_activate(void) {
	if(have_el2) {
		register void (*func) asm ("x1") = el2_activate;
		register uintptr_t parm asm ("x0") = el2_vbar;
		asm volatile ("hvc #0" :: "r" (parm), "r" (func) : "x2", "memory");
	}
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/aarch64/el2_transfer_table.c $ $Rev: 817976 $")
#endif
