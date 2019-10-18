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
 * _start.S
 *
 * This code runs as the first instructions after control has transferred
 * from any bootup monitor.  This module defines a minimal bootup stack,
 * and sets SP to this stack, and then starts running C code in _main().
 *
 * For Renesas R-Car Gen3, only the master boot processor cpu0 is started
 * from cold reset. Master boot processor (CA57/CA53/CR7) can be selected
 * with Mode pin MD[6:7].
 */

#include <aarch64/r-car-gen3.h>
#include <aarch64/gic_v2.h>

#define CPUECTLR_EL1            S3_1_C15_C2_1
#define CPUECTLR_SMP_BIT        (1 << 6)

    .text
    .align 2

/*
 * EL3 needs to perform some initial GIC configuration
 */
    .extern cstart
    .extern secondary_core_start
    .extern psci_support
    .global _start

_start:
    /* NOTE:
     *  Do NOT modify registers X0-X3 before jumping to the cstart label.
     *  These registers may contain information provided by the IPL and
     *  cstart will save them in the boot_regs variable for later perusal
     *  by other portions of startup.
     */

    /*
     * Set up EL3 state if necessary
     */
    mrs     x9, CurrentEL
    cmp     x9, #0xc                // EL3
    bne     0f
    bl      setup_el3

0:  b       cstart

/*
 * Core is in EL3, this happens when no secure code was running, e.g. no ARM trusted firmware
 * before jumping to the startup/ipl.
 */
setup_el3:
    /*
     * Set the GIC Priority Mask to enable forwarding of interrupts to cpu interface.
     * Configure interrupts as insecure Group1 interrupts.
     */
    mov     x4, #RCAR_GEN3_GIC_CPU_BASE
    mov     w5, #0xf0
    str     w5, [x4, #ARM_GICC_PMR]

    mov     x4, #RCAR_GEN3_GIC_DIST_BASE
    mov     w5, #0xffffffff
    str     w5, [x4, #ARM_GICD_IGROUPRn+0x00]
    str     w5, [x4, #ARM_GICD_IGROUPRn+0x04]
    str     w5, [x4, #ARM_GICD_IGROUPRn+0x08]
    str     w5, [x4, #ARM_GICD_IGROUPRn+0x0c]
    str     w5, [x4, #ARM_GICD_IGROUPRn+0x10]
    str     w5, [x4, #ARM_GICD_IGROUPRn+0x14]
    str     w5, [x4, #ARM_GICD_IGROUPRn+0x18]
    str     w5, [x4, #ARM_GICD_IGROUPRn+0x1c]

    /*
     * Disable interrupts
     */
    msr        DAIFset, #0x0F

    /*
     * Disable MMU and Data cache, Enable I cache
     */
    mrs     x4, SCTLR_EL3
    bic     x4, x4, #(1 << 0)    /* Clear bit M(0) - EL3 stage 1 address translation disabled. */
    orr     x4, x4, #(1 << 1)    /* Set bit A(1)   - Alignment fault checking enabled. */
    bic     x4, x4, #(1 << 2)    /* Disable D Cache */
    orr     x4, x4, #(1 << 12)   /* Enable I Cache */
    msr     SCTLR_EL3, x4
    dsb     sy
    isb
    ic      ialluis
    isb     sy
    mov     x4, x4
    mov     x4, x4
    mov     x4, x4

    /*
     * Enable data coherency with other cores in the cluster
     *
     */
    mrs     x4, CPUECTLR_EL1
    orr     x4, x4, #CPUECTLR_SMP_BIT
    msr     CPUECTLR_EL1, x4
    isb

    /*
     * Set default generic timer frequency, if not set already
     */
    mrs     x4, cntfrq_el0
    cbnz    x4, 1f
    ldr     w4, cntfrq_default
    msr     cntfrq_el0, x4

    /*
     *  Check the core id
     */
1:  mrs     x4, MPIDR_EL1
    and     x4, x4, #0xffff
    cbnz    x4, secondary_core

    /*
     *  Master core
     *  Set the entry point of the secondary cores to _start
     */
    mov     x4, #RCAR_GEN3_RESET_BASE
    adr     x5, _start
    str     w5, [x4, #RCAR_GEN3_RESET_CA53CPUxBARL(0)]
    str     w5, [x4, #RCAR_GEN3_RESET_CA53CPUxBARL(1)]
    str     w5, [x4, #RCAR_GEN3_RESET_CA53CPUxBARL(2)]
    str     w5, [x4, #RCAR_GEN3_RESET_CA53CPUxBARL(3)]
    str     w5, [x4, #RCAR_GEN3_RESET_CA57CPUxBARL(0)]
    str     w5, [x4, #RCAR_GEN3_RESET_CA57CPUxBARL(1)]
    str     w5, [x4, #RCAR_GEN3_RESET_CA57CPUxBARL(2)]
    str     w5, [x4, #RCAR_GEN3_RESET_CA57CPUxBARL(3)]

    /*
     *  PSCI not supported
     */
    ldr	    x4, =psci_support
    mov     w5, #0
    str     w5, [x4]

    ret


secondary_core:
    ldr	    x4, =secondary_core_start
0:  wfe
    ldr     x5, [x4]
    cbz     x5, 0b
    br      x5


cntfrq_default: .word   100000000   // bogus default cntfrq_el0 value

#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/boards/rcar_gen3/_start.S $ $Rev: 848016 $";
.previous
#endif
#endif
