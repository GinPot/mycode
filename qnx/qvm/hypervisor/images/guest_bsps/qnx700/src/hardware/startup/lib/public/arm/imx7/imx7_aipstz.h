/*
 * $QNXLicenseC:
 * Copyright 2017, QNX Software Systems.
 * Copyright 2016, Freescale Semiconductor, Inc.
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

#ifndef IMX7_AIPS_H_
#define IMX7_AIPS_H_

/*
 * AHB to IP Bridge Trust Zone (AIPSTZ)
 */
#define IMX7_AIPS1_CONFIG               0x301F0000
#define IMX7_AIPS2_CONFIG               0x305F0000
#define IMX7_AIPS3_CONFIG               0x309F0000


#define IMX7_AIPS_MPR0                  0x00
#define IMX7_AIPS_MPR1                  0x04
#define IMX7_AIPS_OPACR0                0x40
#define IMX7_AIPS_OPACR1                0x44
#define IMX7_AIPS_OPACR2                0x48
#define IMX7_AIPS_OPACR3                0x4C
#define IMX7_AIPS_OPACR4                0x50


#endif /* IMX7_AIPS_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/arm/imx7/imx7_aipstz.h $ $Rev: 834524 $")
#endif
