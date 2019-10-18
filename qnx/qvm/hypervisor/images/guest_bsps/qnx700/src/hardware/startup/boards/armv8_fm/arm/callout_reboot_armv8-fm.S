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

/*
 * -----------------------------------------------------------------------
 * Routine to patch callout code
 *
 * On entry:
 *  r0 - physical address of syspage
 *  r1 - virtual  address of syspage
 *  r2 - offset from start of syspage to start of the callout routine
 *  r3 - offset from start of syspage to read/write data used by callout
 * -----------------------------------------------------------------------
 */

patch_reboot:
    stmdb          sp!, {r4,lr}
    add            r4, r0, r2

    /*
     * Map System Register Block registers
     */
    mov            r0, #0x100
    adr            r1, Lpaddr
    bl             callout_io_map_indirect

    /*
     * Patch the callout routine
     */
    CALLOUT_PATCH  r4, r0, r1, r2, ip
    ldmia          sp!, {r4,pc}

        .align  3
Lpaddr: .quad   0x1C010000

CALLOUT_START(reboot, 0, patch_reboot)
    mov     ip,     #0x000000ff
    orr     ip, ip, #0x0000ff00
    orr     ip, ip, #0x00ff0000
    orr     ip, ip, #0xff000000

    teq     r1, #0                              // r1 = abnormal parameter flag
    beq     do_reboot

    ldrh    r2, [r0, #SYSPAGE_SYSTEM_PRIVATE]   // offset to system_private
    add     r0, r0, r2                          // system_private address
    ldr     r2, [r0, #SP_PRIVATE_FLAGS]
    tst     r2, #SYSTEM_PRIVATE_FLAG_ABNORMAL_REBOOT
    beq     0f                                  // spin if ABNORMAL_REBOOT is NOT set

do_reboot:
    /*
     * Write to System configuration control to trigger reset
     */
    ldr     r1, setreboot
    str     r1, [ip, #0xA4]
    dsb     sy
0:  b       0b

            .align  2
setreboot:  .word   0xC0800000
CALLOUT_END(reboot)

#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/boards/armv8_fm/arm/callout_reboot_armv8-fm.S $ $Rev: 824292 $";
.previous
#endif
#endif
