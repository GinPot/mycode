#
# Copyright 2016, QNX Software Systems.
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
 * cstart.S
 *
 *	Initial code to get Neutrino started
 *
 */

	.globl	_start

_start:
	/* NOTE:
	   	Do NOT modify registers R0-R3 before jumping to the cstart label.
		These registers may contain information provided by the IPL and
		cstart will save them in the boot_regs variable for later perusal
		by other portions of startup.
	*/
	bl		boot_verify_load_addr

	bl		spin_smp_init

	b		cstart

#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/boards/armv8_fm/arm/cstart.S $ $Rev: 805440 $";
.previous
#endif
#endif
