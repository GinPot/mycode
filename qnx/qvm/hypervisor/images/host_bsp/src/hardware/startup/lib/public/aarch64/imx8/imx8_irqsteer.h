/*
 * $QNXLicenseC:
 * Copyright 2017 NXP
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

#ifndef IMX_IRQ_STEER_H_
#define IMX_IRQ_STEER_H_

/* IRQ STEER registers, offset from base address */
#define IMX_IRQ_STEER_CHN_CTL          0x00     /* Channel Control Register */
#define IMX_IRQ_STEER_MASK             0x04     /* Channel n Interrupt Mask */
#define IMX_IRQ_STEER_SET              0x08     /* Channel n Interrupt Set */
#define IMX_IRQ_STEER_STAT             0x0C     /* Channel n Interrupt Status */
#define IMX_IRQ_STEER_DIS              0x10     /* Channel Master Interrupt Disable Register */
#define IMX_IRQ_STEER_MSTR_STAT        0x14     /* Channel n Master Status Register */

#endif /* IMX_IRQ_STEER_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/aarch64/imx8/imx8_irqsteer.h $ $Rev: 837679 $")
#endif
