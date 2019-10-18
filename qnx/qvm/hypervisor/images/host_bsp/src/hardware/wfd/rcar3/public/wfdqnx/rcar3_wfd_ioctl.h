/*
 * $QNXLicenseC:
 * Copyright 2015, QNX Software Systems.
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

#ifndef __RCAR3_WFD_IOCTL_H__
#define __RCAR3_WFD_IOCTL_H__

#include <sys/ioctl.h>
#include <sys/types.h>

/* CMM control devctl group */

#define RCAR3_CMM_GET_BRIGHTNESS_R      __DIOF('R', 'C' + 0, int)
#define RCAR3_CMM_GET_BRIGHTNESS_G      __DIOF('R', 'C' + 1, int)
#define RCAR3_CMM_GET_BRIGHTNESS_B      __DIOF('R', 'C' + 2, int)
#define RCAR3_CMM_GET_CONTRAST_R        __DIOF('R', 'C' + 3, int)
#define RCAR3_CMM_GET_CONTRAST_G        __DIOF('R', 'C' + 4, int)
#define RCAR3_CMM_GET_CONTRAST_B        __DIOF('R', 'C' + 5, int)
#define RCAR3_CMM_GET_HUE               __DIOF('R', 'C' + 6, int)
#define RCAR3_CMM_GET_SATURATION        __DIOF('R', 'C' + 7, int)
#define RCAR3_CMM_GET_GAMMA             __DIOF('R', 'C' + 8, int)
#define RCAR3_CMM_GET_COLOR_TEMPERATURE __DIOF('R', 'C' + 9, int)

/* range -255 to 255, int (default is 0) */
#define RCAR3_CMM_SET_BRIGHTNESS_R      __DIOT('R', 'C' + 20, int)
/* range -255 to 255, int (default is 0) */
#define RCAR3_CMM_SET_BRIGHTNESS_G      __DIOT('R', 'C' + 21, int)
/* range -255 to 255, int (default is 0) */
#define RCAR3_CMM_SET_BRIGHTNESS_B      __DIOT('R', 'C' + 22, int)
/* range -128 to 127, int (default is 0) */
#define RCAR3_CMM_SET_CONTRAST_R        __DIOT('R', 'C' + 23, int)
/* range -128 to 127, int (default is 0) */
#define RCAR3_CMM_SET_CONTRAST_G        __DIOT('R', 'C' + 24, int)
/* range -128 to 127, int (default is 0) */
#define RCAR3_CMM_SET_CONTRAST_B        __DIOT('R', 'C' + 25, int)
/* range -180 to 180, int (default is 0) */
#define RCAR3_CMM_SET_HUE               __DIOT('R', 'C' + 26, int)
/* range -128 to 127, int (default is 0) */
#define RCAR3_CMM_SET_SATURATION        __DIOT('R', 'C' + 27, int)
/* range 0.01 to 7.99, float (default is 1.0) */
#define RCAR3_CMM_SET_GAMMA             __DIOT('R', 'C' + 28, int)
/* range 1000 to 40000, int (default is 0 - disabled, 6500 is neutral value) */
#define RCAR3_CMM_SET_COLOR_TEMPERATURE __DIOT('R', 'C' + 29, int)

/* This ioctl() takes an int argument which means 0 - do not wait on VSYNC */
/* any other value - wait for VSYNC before apply latest settings. */
#define RCAR3_CMM_APPLY_SETTINGS        __DIOT('R', 'C' + 40, int)

#endif /* __RCAR3_WFD_IOCTL_H__ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/graphics/branches/dev-renesas-hypervisor/hardware/wfd/rcar3/public/wfdqnx/rcar3_wfd_ioctl.h $ $Rev: 842609 $")
#endif
