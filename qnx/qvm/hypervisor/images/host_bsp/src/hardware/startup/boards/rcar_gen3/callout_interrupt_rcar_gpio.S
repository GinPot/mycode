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


#include "callout.ah"
#include <aarch64/r-car-gen3.h>

/*
 * -----------------------------------------------------------------------
 * Patch interrupt callouts to access rw data.
 * The first call will also map the uart.
 *
 * Patcher routine takes the following arguments:
 *
 *  x0 - syspage paddr
 *  x1 - vaddr of callout
 *  x2 - offset from start of syspage to start of callout routine
 *  x3 - offset from start of syspage to rw storage
 *  x4 - patch data
 *  x5 - callout_rtn
 * -----------------------------------------------------------------------
 */
.text
.align 4

patch_intr:
    sub     sp, sp, #16
    stp     x19, x30, [sp]

    add     x19, x0, x2             /* x19 = address of callout routine */

    ldr     x0, [x4]

    /*
     * Patch the callout routine
     * - patch argument in x0
     * - location to patch specified as first parameter
     * - remaining 2 parameters are scratch registers
     */
    CALLOUT_PATCH   x19, w4, w7

    ldp     x19, x30, [sp]
    add     sp, sp, #16
    ret

/*
 * -----------------------------------------------------------------------
 * Identify interrupt source
 *
 * On entry
 * - syspage pointer in x20
 *
 * On exit
 * - interrupt id returned in x19
 * -----------------------------------------------------------------------
 */
CALLOUT_START(interrupt_id_rcar_gpio, 0, patch_intr)
    /*
     * Get the GPIO base address (patched)
     */
    mov     x7, #0xabcd
    movk    x7, #0xabcd, lsl #16
    movk    x7, #0xabcd, lsl #32
    movk    x7, #0xabcd, lsl #48

    ldr     w0, [x7, #RCAR_GEN3_GPIO_INTDT]
    clz     w19, w0
    mov     w2, #31
    subs    w19, w2, w19
    blt     0f

    /*
     * Mask the interrupt
     */
    mov     w2, #1
    lslv    w1, w2, w19
    mvn     w2, w1
    str     w2, [x7, #RCAR_GEN3_GPIO_INTMSK]

    /*
     * Clear the interrupt
     */
    str     w1, [x7, #RCAR_GEN3_GPIO_INTCLR]

0:
CALLOUT_END(interrupt_id_rcar_gpio)

/*
 * -----------------------------------------------------------------------
 * Acknowledge specified interrupt
 *
 * On entry
 * - x19 - contains interrupt id
 * - x20 - contains syspage pointer (INTR_GENFLAG_LOAD_SYSPAGE was used)
 * - x22 - contains the interrupt mask count (INTR_GENFLAG_LOAD_INTRMASK was used)
 * -----------------------------------------------------------------------
 */
CALLOUT_START(interrupt_eoi_rcar_gpio, 0, patch_intr)
    /*
     * Get the GPIO base address (patched)
     */
    mov     x7, #0xabcd
    movk    x7, #0xabcd, lsl #16
    movk    x7, #0xabcd, lsl #32
    movk    x7, #0xabcd, lsl #48

    /* Only unmask interrupt if mask count is zero */
    cbnz    x22, 0f

    /*
     * Unmask the interrupt
     */
    mov     w0, #1
    lslv    w1, w0, w19
    str     w1, [x7, #RCAR_GEN3_GPIO_MSKCLR]

0:
CALLOUT_END(interrupt_eoi_rcar_gpio)

/*
 * -----------------------------------------------------------------------
 * Mask specified interrupt
 *
 * On entry:
 * - x0 - _syspage_ptr
 * - x1 - vector number (relative to controller)
 *
 * Returns:
 * - x0 - error status
 * -----------------------------------------------------------------------
 */
CALLOUT_START(interrupt_mask_rcar_gpio, 0, patch_intr)
    /*
     * Get the GPIO base address (patched)
     */
    mov     x7, #0xabcd
    movk    x7, #0xabcd, lsl #16
    movk    x7, #0xabcd, lsl #32
    movk    x7, #0xabcd, lsl #48

    /*
     * Mask the interrupt
     */
    mov     w0, #1
    lslv    w0, w0, w1
    mvn     w0, w0
    str     w0, [x7, #RCAR_GEN3_GPIO_INTMSK]

    mov     x0, #0

    ret
CALLOUT_END(interrupt_mask_rcar_gpio)

/*
 * -----------------------------------------------------------------------
 * Unmask specified interrupt
 *
 * On entry:
 * - x0 - _syspage_ptr
 * - x1 - vector number (relative to controller)
 *
 * Returns:
 * - x0 - error status
 * -----------------------------------------------------------------------
 */
CALLOUT_START(interrupt_unmask_rcar_gpio, 0, patch_intr)
    /*
     * Get the GPIO base address (patched)
     */
    mov     x7, #0xabcd
    movk    x7, #0xabcd, lsl #16
    movk    x7, #0xabcd, lsl #32
    movk    x7, #0xabcd, lsl #48

    /*
     * Unmask the interrupt
     */
    mov     w0, #1
    lslv    w0, w0, w1
    str     w0, [x7, #RCAR_GEN3_GPIO_MSKCLR]

    mov     x0, #0

    ret
CALLOUT_END(interrupt_unmask_rcar_gpio)

#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/boards/rcar_gen3/callout_interrupt_rcar_gpio.S $ $Rev: 849205 $";
.previous
#endif
#endif
