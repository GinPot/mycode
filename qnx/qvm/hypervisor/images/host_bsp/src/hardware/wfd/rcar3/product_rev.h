/*
 * $QNXLicenseC:
 * Copyright 2014-2015, QNX Software Systems.
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

#ifndef _PRODUCT_REV_H_
#define _PRODUCT_REV_H_
#include "wfd_common.h"

/* Product Register */
#define RCAR_PRR                0xFFF00044
#define RCAR_PRR_REGSIZE        0x04
#define RCAR_PRODUCT_SHIFT      8
#define RCAR_PRODUCT_MASK       0x7F00

/* Product Register */
#define RCAR_PRR                0xFFF00044
#define RCAR_PRR_REGSIZE        0x04

enum rcar_product {
	RCAR_PRODUCT_H3  = 0x4F00u,
	RCAR_PRODUCT_M3W = 0x5200u,
	RCAR_PRODUCT_V3M = 0x5400u,
	RCAR_PRODUCT_M3N = 0x5500u,
	RCAR_PRODUCT_D3  = 0x5800u,
	RCAR_PRODUCT_V3H = 0x5600u, /* This SOC is the same as that of V3M. */
};
#define RCAR_PRODUCT_ID(x) (enum rcar_product)((x) & 0x7F00u)

enum rcar_product_rev {
	RCAR_PRODUCT_H3_CUT10  = RCAR_PRODUCT_H3  | 0x00u,  /* H3 Ver1.0 */
	RCAR_PRODUCT_H3_CUT11  = RCAR_PRODUCT_H3  | 0x01u,  /* H3 Ver1.1 */
	RCAR_PRODUCT_H3_CUT20  = RCAR_PRODUCT_H3  | 0x10u,  /* H3 Ver2.0 */
	RCAR_PRODUCT_H3_CUT30  = RCAR_PRODUCT_H3  | 0x20u,  /* H3 Ver3.0 */
	RCAR_PRODUCT_M3W_CUT10 = RCAR_PRODUCT_M3W | 0x00u,  /* M3-W Ver1.0 */
	RCAR_PRODUCT_M3W_CUT11 = RCAR_PRODUCT_M3W | 0x10u,  /* M3-W Ver1.1 */
	RCAR_PRODUCT_M3N_CUT10 = RCAR_PRODUCT_M3N | 0x00u,  /* M3-N Ver1.0 */
	RCAR_PRODUCT_M3N_CUT11 = RCAR_PRODUCT_M3N | 0x01u,  /* M3-N Ver1.1 */
	RCAR_PRODUCT_V3M_CUT10 = RCAR_PRODUCT_V3M | 0x00u,  /* V3M Ver1.0 */
	RCAR_PRODUCT_V3M_CUT20 = RCAR_PRODUCT_V3M | 0x10u,  /* V3M Ver2.0 */
	RCAR_PRODUCT_D3_CUT10  = RCAR_PRODUCT_D3  | 0x00u,  /* D3 Ver1.0 */
	RCAR_PRODUCT_D3_CUT11  = RCAR_PRODUCT_D3  | 0x10u,  /* D3 Ver1.1 */
	RCAR_PRODUCT_V3H_CUT10 = RCAR_PRODUCT_V3H | 0x00u,  /* V3H Ver1.0 */
};
#define RCAR_PRODUCT_REV(x) (enum rcar_product_rev)((x) & 0x7FFFu)

SYM_INTERNAL_ONLY uint32_t get_prr (void);
SYM_INTERNAL_ONLY const char *get_product_name (int chip_type);
SYM_INTERNAL_ONLY const char *get_revision_name (int chip_revision);

#endif /* #ifndef _PRODUCT_REV_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/graphics/branches/dev-renesas-hypervisor/hardware/wfd/rcar3/product_rev.h $Rev")
#endif

