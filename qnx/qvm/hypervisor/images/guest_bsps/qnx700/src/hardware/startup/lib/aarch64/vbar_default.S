#
# Copyright 2014, QNX Software Systems.
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
 * Stub exception vector table.
 */

#include "asmoff.def"

	.text
	.align	2

	.global	vbar_default

.macro ventry
	.align	7
0:	b	0b
.endm

	.align	11

vbar_default:
	ventry		// 0x000
	ventry		// 0x080
	ventry		// 0x100
	ventry		// 0x180
	.align 7	// 0x200

	// synchronous exceptions with ELx, x > 0
	ldr		w0, trying_hvc
0:	cbz		w0, 0b
	// We were trying to execute an HVC instruction, but it's been disabled
	// somewhere. Just skip it.
	mrs		x0, elr_el1
	add		x0, x0, #4
	msr		elr_el1, x0
	eret

	ventry		// 0x280
	ventry		// 0x300
	ventry		// 0x380

	.align	7	// 0x400
	msr		tpidr_el1,x10 // save x10
	mrs		x10,esr_el2
	lsr		w10,w10,26
	cmp		w10,#AARCH64_ESR_EC_SMC_64	// Check for SMC instruction
	beq		reflecting_smc
	mrs		x10,tpidr_el1 // restore x10

	// This entry is for synchronous exceptions from a lower exception
	// level using aarch64. The only reason we'd start executing here during
	// startup is due to a HVC instruction when we're setting up for a
	// hypervisor. X1 holds a function to execute and X0 holds a parm for it.
	msr		tpidr_el1,x30
	mov		x2,sp
	msr		far_el2,x2
	mrs		x2,sp_el1
	mov		sp,x2
	blr		x1
	mrs		x30,tpidr_el1
	mrs		x2,far_el2
	mov		sp,x2
	eret

	ventry		// 0x480
	ventry		// 0x500
	ventry		// 0x580
	ventry		// 0x600
	ventry		// 0x680
	ventry		// 0x700
	ventry		// 0x780

	// Have to pass an SMC back to original EL2 environment
reflecting_smc:
	msr		far_el2,x11	// save x11 (FAR_EL2 isn't used for SMC processing)
	adr		x11,save_area
	mrs		x10,spsr_el2	// save original exception location/state
	str		x10,[x11, 0]
	mrs		x10,elr_el2
	add		x10,x10,4
	str		x10,[x11, 8]
	mrs		x11,far_el2		// restore X11
	adr		x10,reflect_return	// make original environment ERET return here
	msr		elr_el2,x10
	mov		x10,#0x3c9 // EL2h, DAIF masked
	msr		spsr_el2,x10

	ldr		x10,original_el2_regs + EL2_VTTBR
	msr		vttbr_el2,x10

	mrs		x10,sctlr_el2
	msr		far_el2,x10
	ldr		x10,original_el2_regs + EL2_SCTLR
	msr		sctlr_el2,x10

	ldr		x10,original_el2_regs + EL2_VBAR
	add		x10,x10,0x400		// target vbar synchronous exception vector
	isb
	br		x10

reflect_return:
	nop						// this is where original EL2 thinks SMC is
	msr		vttbr_el2, xzr	// clear VMID
	mrs		x10,far_el2		// restore SCTLR_EL2
	msr		sctlr_el2,x10
	ldr		x10,save_area+0	// restore original exception location/state
	msr		spsr_el2,x10
	ldr		x10,save_area+8
	msr		elr_el2,x10
	mrs		x10,tpidr_el1	// restore X10 value
	eret
	.align	3
save_area:	.xword	0,0

#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/aarch64/vbar_default.S $ $Rev: 817976 $";
.previous
#endif
#endif
