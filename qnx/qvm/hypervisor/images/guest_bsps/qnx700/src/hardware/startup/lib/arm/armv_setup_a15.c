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

#include "startup.h"

/*
 * Additional Cortex A15 MPCore specific CPU initialisation
 */

void
armv_setup_a15(struct cpuinfo_entry *cpu, unsigned cpunum, unsigned cpuid)
{
    unsigned   aux;
    unsigned   val;

    aux = arm_actlr_get();
    if (lsp.syspage.p->num_cpu > 1) {
        /*
         * Set SMP (bit 6) to enable coherency requests from other cpus
         */
        aux |= (1u << 6);
    }
    /*
     * ARM errata 801819:  A livelock can occur in the L2 cache arbitration
     * that might prevent a snoop from completing. Under certain conditions this
     * can cause the system to deadlock.
     * Applies to A15 revisions r2p0, r2p1, r2p2, r2p3 or r2p4 and REVIDR[3] is
     * set to 0
     */
    /* Read REVIDR reg */
    __asm__ __volatile__ ("mrc p15, 0, %0, c0, c0, 6" : "=r" (val));

    /* REVIDR[3] indicates erratum fix available on silicon */
    if ((val & (0x1 << 3)) == 0) {
        /* Disables streaming. All write-allocate lines allocate in the L1 or L2 cache.*/
        aux |= (0x3 << 27);
        /* Disables streaming. All write-allocate lines allocate in the L1 cache.*/
        aux |= (0x3 << 25);
        if (debug_flag) {
            kprintf("CPU%d: %s: ARM erratum 801819 fix applied\n", cpunum, __func__);
        }
    }

    arm_actlr_set(aux);
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/arm/armv_setup_a15.c $ $Rev: 835265 $")
#endif
