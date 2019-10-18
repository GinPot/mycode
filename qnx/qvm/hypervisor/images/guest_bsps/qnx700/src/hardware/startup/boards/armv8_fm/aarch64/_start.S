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
 * _start.S
 *
 * This code runs as the first instructions after control has transferred
 * from any bootup monitor.  This module defines a minimal bootup stack,
 * and sets SP to this stack, and then starts running C code in _main().
 *
 * For the ARMv8 Fast Model, all processors execute from cold reset in EL3.
 * This code sets up minimal EL2/EL3 state for all cpus and switches to EL1
 * to continue execution:
 * - cpu0 jumps to cstart to perform the startup initialisation
 * - other cpus spin until they are woken up by board_smp_start
 */

	.text
	.align 2

/*
 * EL3 needs to perform some initial GIC configuration
 */
#define	GICCV2_BASE			0x2c002000
#define	GICDV2_BASE			0x2c001000

#define V2_ID0_OFF			0xfe0
#define V2_ID0_VAL			0x90
#define V2_ID1_OFF			0xfe4
#define V2_ID1_VAL			0xb4
#define V2_ID0_VAL			0x90

#define	GICCV3_BASE			0x2c000000
#define	GICDV3_BASE			0x2f000000
#define	GICRV3_BASE			0x2f100000

#include <aarch64/gic_v3.h>

	.extern	cstart
	.global	_start

_start:
	/* NOTE:
	   	Do NOT modify registers X0-X3 before jumping to the cstart label.
		These registers may contain information provided by the IPL and
		cstart will save them in the boot_regs variable for later perusal
		by other portions of startup.
	*/

	/*
	 * Set up EL3 state if necessary
	 */
	mrs		x9, CurrentEL
	cmp		x9, #0xc				// EL3
	bne		0f
	bl		setup_el3
0:

	bl		spin_smp_init

	/*
	 * call cstart
	 */
	b		cstart

	/*
	 * Setup minimal GIC state
	 */
setup_el3:
	ldr		x1, =GICCV2_BASE
	ldr		x2, =GICDV2_BASE
	ldr		w3, [x2, #V2_ID0_OFF]
	cmp		w3, V2_ID0_VAL
	bne		1f
	ldr		w3, [x2, #V2_ID1_OFF]
	cmp		w3, V2_ID1_VAL
	beq		2f

1:
	// Foundation model was run with --gicv3

	mov		x1, #GICCV3_BASE
	mov		x2, #GICDV3_BASE
	mov		x3, #GICRV3_BASE

	// find the correct redistributor
	mrs		x4,mpidr_el1
	and		x5,x4,#0xffffff
	lsr		x4,x4,#32
	bfi		w5,w4,#24,#8
3:
	ldr		w4,[x3, #ARM_GICR_TYPER+4]
	cmp		w5,w4
	beq		4f
	add		x3,x3,#ARM_GICR_SIZE_PER_CORE
	b		3b

	// Have to wake up the CPU interface
4:
	ldr		w4, [x3, #ARM_GICR_WAKER]
	and		w4,	w4, #~0x2	// turn off ProcessorSleep
	str		w4, [x3, #ARM_GICR_WAKER]
	dsb		sy
99:
	yield
	ldr		w4, [x3, #ARM_GICR_WAKER]
	tst		w4, #0x4	// wait for ChildrenAsleep bit to clear
	bne		99b

	mrs     x4,S3_6_C12_C12_5	// icc_sre_el3
	orr		x4,x4,#0xf 			// turn on SRE, DIB, DFB, and Enable bits
	msr     S3_6_C12_C12_5,x4	// icc_sre_el3
	isb

2:

	/*
	 * Set Priority Mask to enable forwarding of interrupts to cpu interface.
	 * Configure interrupts as insecure Group1 interrupts.
	 */
	mov		w3, #0xf0
	str		w3, [x1, #ARM_GICC_PMR]
	mov		w3, #((1<<0)|(1<<5)|(1<<6)|(1<<7)|(1<<8))
	str		w3, [x1, #ARM_GICC_CTLR]
	dsb		sy

	mov		w4, #0xffffffff
	str		w4, [x2, #ARM_GICD_IGROUPRn+0x00]
	str		w4, [x2, #ARM_GICD_IGROUPRn+0x04]
	str		w4, [x2, #ARM_GICD_IGROUPRn+0x08]
	str		w4, [x2, #ARM_GICD_IGROUPRn+0x0c]
	str		w4, [x2, #ARM_GICD_IGROUPRn+0x10]
	str		w4, [x2, #ARM_GICD_IGROUPRn+0x14]
	str		w4, [x2, #ARM_GICD_IGROUPRn+0x18]
	str		w4, [x2, #ARM_GICD_IGROUPRn+0x1c]

	// See if CNTFRQ has been set. If not, set it to 1000000
	mrs		x3,cntfrq_el0
	cmp		x3,xzr
	bne		1f
	ldr		x3,=100000000
	msr		cntfrq_el0,x3
1:
	ret

#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/boards/armv8_fm/aarch64/_start.S $ $Rev: 844523 $";
.previous
#endif
#endif
