#
# Copyright 2015, QNX Software Systems.
#
# Licensed under the Apache License, Version 2.0 (the "License"). You
# may not reproduce, modify or distribute this software except in
# compliance with the License. You may obtain a copy of the License
# at: http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" basis,
# WITHOUT WARRANTIES OF ANY KIND, either express or implied.
#
# This file may contain contributions from others, either as
# contributors under the License or as licensors under other terms.
# Please review this entire file for other proprietary rights or license
# notices, as well as the QNX Development Suite License Guide at
# http://licensing.qnx.com/license-guide/ for other information.
#

/*
 * _start_el1.S
 *
 * Helper function for _start() to switch to EL1 if we began execution at EL2.
 *
 * This code can be overridden by a board specific startup if necessary.
 */

#include "asmoff.def"

	.text
	.align 2


	.global	_start_el1
_start_el1:
	mrs		x0, CurrentEL
	bic		x0, x0, #1				// clear SPSel bit
	cmp		x0, #0xc				// check for EL3
	beq		from_el3
	cmp		x0, #0x8				// check for EL2
	beq		from_el2

	// At EL1 already
	msr		daifset, #0xf			// disable interrupts etc.
	ret


from_el3:
	/*
	 * Set up minimal EL3 state required for EL1 execution
	 */


	mrs		x1,id_pfr1_el1
	tst		x1,0xf << 28
	beq		1f
	// Chip supports system register access to GIC, enable in ICC_SRE_EL3
	mov		x1,0xf
	msr		S3_6_C12_C12_5,x1
1:

	mov		x1, #((1<<10)|(1<<8)|(1<<0))	// SCR_EL3.RW, HCE and NS set
	msr		scr_el3, x1				// Set EL2/EL1/EL0 to AArch64
	isb

	mrs		x1,scr_el3
	tst		x1,#(1<<8)			// check to see if SCR_EL3.HCE actually got set
	bne		1f

	// SCR_EL3.HCE is RES0, which means that EL2 isn't supported. Just
	// drop down to EL1

	/*
	 * set up for return to caller, executing at EL1
	 */
	msr		elr_el3, x30
	mov		x0, #0x3c5				// DAIF + EL1h
	msr		spsr_el3, x0
	eret

1:
	// If we entered in EL3 and EL2 is supported, we're going to assume
	// that HCR_EL2 is random. Set it to a known good value so that we
	// don't try to reflect things into an uninitialized environment later
	// on.
	mov		x0, #(1 << 31)			// HCR_EL2.RW - execute EL1 in AArch64
	msr		hcr_el2, x0				// HVC instruction is enabled

	// Drop down to EL2 for that portion of the setup so we can
	// reference the ICC_SRE_EL2 if needed.
	adr		x0,from_el2
	msr		elr_el3, x0
	mov		x0, #0x3c9				// DAIF + EL2h
	msr		spsr_el3, x0
	eret


from_el2:
	/*
	 * Set up minimal EL2 state required for EL1 execution
	 */

	/*
	 * set up for return to caller, executing at EL1
	 */
	msr		elr_el2, x30
	mov		x0, #0x3c5				// DAIF + EL1h
	msr		spsr_el2, x0

	mrs		x1,id_pfr1_el1
	tst		x1,0xf << 28
	beq		1f
	// Chip supports system register access to GIC, enable in ICC_SRE_EL2
	mov		x1,0xf
	msr		S3_4_C12_C9_5,x1
1:

	mrs		x0, sctlr_el2
	mov		x1, #((1<<0)|(1<<2)|(1<<12))
	tst		x0, x1
	beq		1f
	// if MMU, data or instruction cache are on, turn them off
	bic		x0, x0, x1
	msr		sctlr_el2, x0
	isb
	bl		aarch64_cache_flush
1:
	adr		x0,	have_el2			// indicate that the chip supports EL2
	mov		w1, #1
	str		w1, [x0]

	adr 	x0,original_el2_regs
	// We don't save the TPIDR_EL2 and SP_EL2 registers in the structure
	// because they're likely per-cpu. We just make sure we don't damage
	// them in startup and procnto will take care of them itself.
	mrs 	x1,vbar_el2
	str 	x1,[x0, EL2_VBAR]
	mrs 	x1,sctlr_el2
	str 	x1,[x0, EL2_SCTLR]
	mrs 	x1,vttbr_el2
	str 	x1,[x0, EL2_VTTBR]
	mrs 	x1,vtcr_el2
	str 	x1,[x0, EL2_VTCR]
	mrs 	x1,tcr_el2
	str 	x1,[x0, EL2_TCR]
	mrs 	x1,ttbr0_el2
	str 	x1,[x0, EL2_TTBR0]
	mrs 	x1,hcr_el2
	str 	x1,[x0, EL2_HCR]

	// Do NOT disturb the X1 (HCR_EL2) value until we update the system register

	mov		x0,#(1 << 19)			// isolate the HCR_EL2.TSC bit
	and		x0,x0,x1
	orr		x0,x0,#(1 << 31)		// turn on HCR_EL2.RW - execute EL1 in AArch64
	msr		hcr_el2,x0				// HVC instruction is enabled

	adr		x0, vbar_default		// initialize an EL2 exception table so
	msr		vbar_el2, x0			// we can set up for a hypervisor later


	mrs		x0, midr_el1
	msr		vpidr_el2, x0			// set MIDR_EL1 value read by EL1

	mrs		x0, mpidr_el1
	msr		vmpidr_el2, x0			// set MPIDR_EL1 value read by EL1

	msr		cntvoff_el2, xzr		// reset CNTVOFF_EL2

	mov		x0, #0x3
	msr		cnthctl_el2, x0			// enable access to physical timer by EL0/1

	msr		vttbr_el2, xzr			// clear VMID

	eret

#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/aarch64/_start_el1.S $ $Rev: 844523 $";
.previous
#endif
#endif
