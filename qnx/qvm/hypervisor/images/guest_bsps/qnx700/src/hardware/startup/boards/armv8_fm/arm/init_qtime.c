/*
 * $QNXLicenseC:
 * Copyright 2014, QNX Software Systems. All Rights Reserved.
 *
 * This source code may contain confidential information of QNX Software
 * Systems (QSS) and its licensors.  Any use, reproduction, modification,
 * disclosure, distribution or transfer of this software, or any software
 * that includes or is based upon any of this code, is prohibited unless
 * expressly authorized by QSS by written agreement.  For more information
 * (including whether this source code file has been published) please
 * email licensing@qnx.com. $
*/

#include "startup.h"

void
init_qtime()
{
	if (timer_freq == 0) {
		__asm__ __volatile__("mrc   p15,0,%0,c14,c0,0" : "=r"(timer_freq));
	}

	init_qtime_v7gt(timer_freq, 0);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/boards/armv8_fm/arm/init_qtime.c $ $Rev: 835011 $")
#endif
