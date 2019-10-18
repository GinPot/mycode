
# Copyright 2014, QNX Software Systems.
# Copyright 2016, Freescale Semiconductor, Inc.
# Copyright 2017 NXP
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
 * GICv3 interrupt controller callouts.
 *
 * See callout.ah for description of special registers for interrupt callouts.
 */

#include "callout.ah"
#include "aarch64/gic_v3.h"

/*
 *
 * Patcher routines take the following arguments:
 *  x0 - syspage paddr
 *  x1 - vaddr of callout
 *  x2 - offset from start of syspage to start of callout routine
 *  x3 - offset from start of syspage to rw storage
 *  x4 - patch data
 *  x5 - callout_rtn
 */


/*
 * Get access to the MMIO GICC registers
 */
gicc_patch:
    add     x2, x0, x2                  // x2 = address of callout routine
	ldr		x0, gic_gicc_vaddr

    CALLOUT_PATCH   x2, w4, w5
    ret

/*
 * Patch PPI/SGI interrupt callouts to access GICR regs
*/
ppi_patch:
    add     x2, x0, x2                  // x2 = address of callout routine
	ldr		x0, gic_gicr_vaddr
	// All the registers we want to access are in the SGI region
	add		x0, x0, #ARM_GICR_SGI_BASE_OFFSET

    CALLOUT_PATCH   x2, w4, w5
	// X2 is now pointing to instruction past the GICR address load
	ldr		w4,[x2]
	//XXXX: Need to get proper GICR spacing value (V4 is different)
	mov		w5,#17
	bfi		w4,w5, #5,#16		// insert shift value in immed field
	str		w4,[x2]

    ret


/*
 * Patch SPI interrupt callouts to access GICD regs
 */
spi_patch:
    add     x2, x0, x2                  // x2 = address of callout routine
	ldr		x0, gic_gicd_vaddr

    /*
     * Patch callout with RW offset in w3
     */
    CALLOUT_PATCH   x2, w4, w5

    ret

/*
 * -----------------------------------------------------------------------
 * Identify interrupt source
 *
 * -----------------------------------------------------------------------
 */
CALLOUT_START(interrupt_id_gic_v3_ppi_sr, 0, 0)
    /*
     * Get interrupt ID and handle special cases:
     */
    mrs     x24, ARM_GICC_IAR1_EL1
    cmp     x24, #31				// check for non-PPI/SGI value
    mov     x0, #-1
	// Return intid if it's <= 31, -1 otherwise
    csel    x19, x0, x24, hi
    /*
     * Done - interrupt id is in x19
     */
CALLOUT_END(interrupt_id_gic_v3_ppi_sr)

CALLOUT_START(interrupt_id_gic_v3_ppi_mm, 0, gicc_patch)
	mov		x7, #1234				// vaddr for GICC (patched)
	movk	x7, #1234, lsl #16		// ...
	movk	x7, #1234, lsl #32		// ...
	movk	x7, #1234, lsl #48		// ...
    /*
     * Get interrupt ID and handle special cases:
     */
	ldr		w24, [x7, ARM_GICC_IAR]
    cmp     w24, #31				// check for non-PPI/SGI value
    mov     x0, #-1
	// Return intid if it's <= 31, -1 otherwise
    csel    x19, x0, x24, hi
    /*
     * Done - interrupt id is in x19
     */
CALLOUT_END(interrupt_id_gic_v3_ppi_mm)


/*
 * -----------------------------------------------------------------------
 * Acknowledge specified interrupt
 *
 *  x19 - contains interrupt id
 *  x24 - contains original IAR read value from interrupt_id_gic_v3_ppi_sr
 * -----------------------------------------------------------------------
 */
CALLOUT_START(interrupt_eoi_gic_v3_ppi_sr, 0, 0)
    /*
     * Send EOI
     */
    msr     ARM_GICC_EOIR1_EL1, x24
CALLOUT_END(interrupt_eoi_gic_v3_ppi_sr)

CALLOUT_START(interrupt_eoi_gic_v3_ppi_mm, 0, gicc_patch)
	mov		x7, #1234				// vaddr for GICC (patched)
	movk	x7, #1234, lsl #16		// ...
	movk	x7, #1234, lsl #32		// ...
	movk	x7, #1234, lsl #48		// ...

	str		w24, [x7, ARM_GICC_EOIR]
CALLOUT_END(interrupt_eoi_gic_v3_ppi_mm)


/*
 * -----------------------------------------------------------------------
 * Mask specified interrupt
 *
 *  x0 - _syspage_ptr
 *  x1 - vector number
 * -----------------------------------------------------------------------
 */
CALLOUT_START(interrupt_mask_gic_v3_ppi, 0, ppi_patch)
	mov		x7, #1234				// vaddr for GICR (patched)
	movk	x7, #1234, lsl #16		// ...
	movk	x7, #1234, lsl #32		// ...
	movk	x7, #1234, lsl #48		// ...
	mov		x8, #1234				// GICR spacing shift value
	mrs		x9,tpidr_el1			// adjust GICR to proper CPU
	lsl		x9,x9,x8				//...
	add		x7,x7,x9				//...

    /*
     * Mask interrupt
     */
    mov     w2, #1
    lsl     w1, w2, w1                      // Bit to set = 1 << id
    str     w1, [x7, #ARM_GICR_ICENABLER0]  // ICENABLER0 = bit

    mov     x0, #0
    ret
CALLOUT_END(interrupt_mask_gic_v3_ppi)


/*
 * -----------------------------------------------------------------------
 * Unmask specified interrupt
 *
 *  x0 - _syspage_ptr
 *  x1 - vector number
 * -----------------------------------------------------------------------
 */
CALLOUT_START(interrupt_unmask_gic_v3_ppi, 0, ppi_patch)
	mov		x7, #1234				// vaddr for GICR (patched)
	movk	x7, #1234, lsl #16		// ...
	movk	x7, #1234, lsl #32		// ...
	movk	x7, #1234, lsl #48		// ...
	mov		x8, #1234				// GICR spacing shift value
	mrs		x9,tpidr_el1			// adjust GICR to proper CPU
	lsl		x9,x9,x8				//...
	add		x7,x7,x9				//...

    /*
     * unmask interrupt
     */
    mov     w2, #1
    lsl     w1, w2, w1                      // Bit to set = 1 << id
    str     w1, [x7, #ARM_GICR_ISENABLER0]  // ISENABLER0 = bit

    mov     x0, #0
    ret
CALLOUT_END(interrupt_unmask_gic_v3_ppi)

/*
 * -----------------------------------------------------------------------
 * Configure interrupt flags for a specified interrupt vector
 *
 *  x0 - syspage pointer
 *  x1 - intrinfo_entry pointer
 *  x2 - vector number (relative to controller)
 * -----------------------------------------------------------------------
 */
CALLOUT_START(interrupt_config_gic_v3_ppi, 0, 0)
    /*
     * Use ID0 for IPI
     */
    cmp     x2, #0
    mov     w3, #INTR_CONFIG_FLAG_IPI
    csel    w0, w3, wzr, eq
    ret
CALLOUT_END(interrupt_config_gic_v3_ppi)

/*
 * -----------------------------------------------------------------------
 * Identify interrupt source
 *
 * X24 - read of IAR from interrupt_id_gic_v3_ppi_[sr,mr]
 * -----------------------------------------------------------------------
 */
CALLOUT_START(interrupt_id_gic_v3_spi, 0, 0)
    /*
     * Get interrupt ID and handle special cases:
     * ID1022 - spurious interrupt
     * ID1023 - spurious interrupt
     */
	sub		x0, x24, #32				// make return value relative to SPI's
    cmp     x24, #1021
    mov     x1, #-1
	// Return intid if it's <= 1021, -1 otherwise
    csel    x19, x1, x0, hi
CALLOUT_END(interrupt_id_gic_v3_spi)


/*
 * -----------------------------------------------------------------------
 * Acknowledge specified interrupt
 *
 *  x19 - contains interrupt id
 *  x24 - contains original IAR read value from interrupt_id_gic_v3_ppi_sr
 * -----------------------------------------------------------------------
 */
CALLOUT_START(interrupt_eoi_gic_v3_spi_sr, 0, 0)
    /*
     * Send EOI
     */
    msr     ARM_GICC_EOIR1_EL1, x24
CALLOUT_END(interrupt_eoi_gic_v3_spi_sr)

CALLOUT_START(interrupt_eoi_gic_v3_spi_mm, 0, gicc_patch)
	mov		x7, #1234				// vaddr for GICC (patched)
	movk	x7, #1234, lsl #16		// ...
	movk	x7, #1234, lsl #32		// ...
	movk	x7, #1234, lsl #48		// ...

	str		w24, [x7, ARM_GICC_EOIR]
CALLOUT_END(interrupt_eoi_gic_v3_spi_mm)


/*
 * -----------------------------------------------------------------------
 * Mask specified interrupt
 *l
 *  x0 - _syspage_ptr
 *  x1 - vector number (relative to SPI's)
 * -----------------------------------------------------------------------
 */
CALLOUT_START(interrupt_mask_gic_v3_spi, 0, spi_patch)
	mov		x7, #1234				// vaddr for GICD (patched)
	movk	x7, #1234, lsl #16		// ...
	movk	x7, #1234, lsl #32		// ...
	movk	x7, #1234, lsl #48		// ...

    /*
     * Mask interrupt
     */
    add     x5, x7, #ARM_GICD_ICENABLERn + 4
    and     w0, w1, #0x1f
    mov     w2, #1
    lsl     w2, w2, w0                      // Bit to set = 1 << (id % 32)
    lsr     w0, w1, #5                      // Index = id / 32
    str     w2, [x5, x0, lsl #2]            // ICENABLERn[index] = bit

    mov     x0, #0
    ret
CALLOUT_END(interrupt_mask_gic_v3_spi)


/*
 * -----------------------------------------------------------------------
 * Unmask specified interrupt
 *
 *  x0 - _syspage_ptr
 *  x1 - vector number (relative to SPI's)
 * -----------------------------------------------------------------------
 */
CALLOUT_START(interrupt_unmask_gic_v3_spi, 0, spi_patch)
	mov		x7, #1234				// vaddr for GICD (patched)
	movk	x7, #1234, lsl #16		// ...
	movk	x7, #1234, lsl #32		// ...
	movk	x7, #1234, lsl #48		// ...

    /*
     * Unmask interrupt
     */
    add     x5, x7, #ARM_GICD_ISENABLERn + 4
    and     w0, w1, #0x1f
    mov     w2, #1
    lsl     w2, w2, w0                      // Bit to set = 1 << (id % 32)
    lsr     w0, w1, #5                      // Index = id / 32
    str     w2, [x5, x0, lsl #2]            // ISENABLERn[index] = bit

    mov     x0, #0
    ret
CALLOUT_END(interrupt_unmask_gic_v3_spi)

#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/aarch64/callout_interrupt_gic_v3.S $ $Rev: 845888 $";
.previous
#endif
#endif
