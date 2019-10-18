/*
 * $QNXLicenseC:
 * Copyright 2016, QNX Software Systems.
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

unsigned
board_smp_num_cpu()
{
	unsigned num;

	num = fdt_num_cpu();
	if(num == 0) {
		// No FDT available and the Foundation Model doesn't have a mechanism
		// to determine how many cpu's it's been started with :-(.
		if(!in_hvc) {
			// not a hypervisor guest, see how many cores showed up
			num = spin_smp_num_cpu();
		} else if(max_cpus != ~0u) {
			// In hypervisor guest, secondary cores are not started, use the -P
			// option to determine how many CPU's we've got
			num = max_cpus;
		} else {
			// In hypervisor, -P not specified, assume 1 core
			num = 1;
		}
	}
//	kprintf("board_smp_num_cpu: %d cores\n", num);
	return num;
}

void
board_smp_init(struct smp_entry *smp, unsigned num_cpus)
{
	smp->send_ipi = (void *)&sendipi_gic_v2;
}

int
board_smp_start(unsigned cpu, void (*start)(void))
{
	if(in_hvc) {
		// Guest under the hypervisor, use PSCI emulation to get a secondary
		// CPU running
		return psci_smp_start(cpu, start);
	}
	return spin_smp_start(cpu, start);
}

unsigned
board_smp_adjust_num(unsigned cpu)
{
	return cpu;
}



#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/boards/armv8_fm/board_smp.c $ $Rev: 845710 $")
#endif
