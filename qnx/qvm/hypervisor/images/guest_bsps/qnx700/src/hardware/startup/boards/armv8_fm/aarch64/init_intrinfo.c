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

#define     GICD_V2PADDR    0x2c001000
#define     GICC_V2PADDR    0x2c002000

#define     GICD_V3PADDR    0x2f000000
#define     GICC_V3PADDR    0x2c000000
#define     GICR_V3PADDR    0x2f100000

#define GET_ID(off) (*(uint32_t *)(GICD_V2PADDR+(off)))


/**
 * init_intrinfo_fm
 *
 * @param gic_version  Which version of the GIC to use
 * 					   0 - automatically determine
 *                     2 - GICv2
 *                     3 - GICv3
 * @param use_mm       force use of the MMIO interface to the GICC registers
 */
void
init_intrinfo_fm(unsigned gic_version, int const use_mm)
{
	if(gic_version == 0) {
        if((GET_ID(0xfe0) == 0x90) && (GET_ID(0xfe4) == 0xb4)) {
			gic_version = 2;
		} else {
			gic_version = 3;
		}
	}

	if(gic_version == 2) {
		gic_v2_init(GICD_V2PADDR, GICC_V2PADDR);
		gicv_asinfo(0x2c006000, 0x2c004000, 0x2c005000, 0, 0x200);
	} else {
		gic_v3_init(GICD_V3PADDR, GICR_V3PADDR, GICC_V3PADDR, 0, use_mm);
		gicv_asinfo(0x2c02f000, 0x2c010000, NULL_PADDR, 0, 0);
		struct smp_entry *const smp  = lsp.smp.p;
		if(smp != NULL) {
			// We didn't know the correct IPI routine to use before this point
			smp->send_ipi = (void *)gic_sendipi;
		}
	}
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/boards/armv8_fm/aarch64/init_intrinfo.c $ $Rev: 845888 $")
#endif
