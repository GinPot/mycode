/*
 * $QNXLicenseC:
 * Copyright 2017 NXP
 * Copyright 2018, QNX Software Systems.
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

#ifndef IMX_INTMUX_H_
#define IMX_INTMUX_H_

/* INTMUX registers, offset from base address */

#define IMX_INTMUX_CHAN(n)             (64 * n) /* Channel offset from base address */

#define IMX_INTMUX_CSR                 0x00     /* Control Status Register */
#define IMX_INTMUX_VEC                 0x04     /* Vector Number Register */
#define IMX_INTMUX_IER                 0x10     /* Interrupt Enable Register */
#define IMX_INTMUX_IPR                 0x20     /* Interrupt Pending Register */


#endif /* IMX_INTMUX_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/aarch64/imx8xp/imx_intmux.h $ $Rev: 850159 $")
#endif
