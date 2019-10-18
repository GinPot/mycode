/*
 * $QNXLicenseC:
 * Copyright 2016, Freescale Semiconductor, Inc.
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

#ifndef IMX_GPIO_H_
#define IMX_GPIO_H_


/* GPIO Registers, offset from base address */
#define IMX_GPIO_DR                             0x00
#define IMX_GPIO_GDIR                           0x04
#define IMX_GPIO_PSR                            0x08
#define IMX_GPIO_ICR1                           0x0C
#define IMX_GPIO_ICR2                           0x10
#define IMX_GPIO_IMR                            0x14
#define IMX_GPIO_ISR                            0x18
#define IMX_GPIO_EDR                            0x1C


#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/aarch64/imx8xp/imx_gpio.h $ $Rev: 850159 $")
#endif
