/*
 * $QNXLicenseC:
 * Copyright 2014, QNX Software Systems.
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

#define GICD_PADDR  0x2c001000
#define GICC_PADDR  0x2c002000

const static struct startup_intrinfo intrs[] =
{
  {
    .vector_base      = _NTO_INTR_CLASS_EXTERNAL,
    .num_vectors      = 160,
    .cascade_vector   = _NTO_INTR_SPARE,
    .cpu_intr_base    = 0,
    .cpu_intr_stride  = 0,
    .flags            = 0,
    .id  = { INTR_GENFLAG_LOAD_SYSPAGE, 0, &interrupt_id_gic},
    .eoi = { INTR_GENFLAG_LOAD_SYSPAGE | INTR_GENFLAG_LOAD_INTRMASK, 0, &interrupt_eoi_gic},
    .mask             = &interrupt_mask_gic,
    .unmask           = &interrupt_unmask_gic,
    .config           = &interrupt_config_gic,
#ifdef INTR_CONFIG_FLAG_SYS // we can remove this after the new header are propagated
    .local_stride     = 0x8000
#endif
  }
};

void
init_intrinfo_fm(const unsigned gic_version, int const use_mm)
{
    if (3 == gic_version) {
        crash("GICv3 is unsupported\n");
    }

    /*
     * Initialise GIC
     */
    arm_gic_init(GICD_PADDR, GICC_PADDR);
    add_interrupt_array(intrs, sizeof(intrs));
}



#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/boards/armv8_fm/arm/init_intrinfo.c $ $Rev: 845888 $")
#endif
