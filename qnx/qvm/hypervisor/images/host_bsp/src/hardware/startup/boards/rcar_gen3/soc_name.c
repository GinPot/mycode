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

#include <hw/inout.h>
#include <inttypes.h>
#include <aarch64/r-car-gen3.h>
#include "rcar_startup.h"

product_id_t  product_id  = ID_NUM;
product_rev_t product_rev = REV_NUM;

char * soc_name[] = {
    "R-Car product unknown",
    "R-Car H3 rev 1.0",
    "R-Car H3 rev 1.1",
    "R-Car H3 rev 2.0",
    "R-Car H3 rev 3.0",
    "R-Car H3 rev unknown",
    "R-Car M3W rev 1.0",
    "R-Car M3W rev 1.1",
    "R-Car M3W rev 2.0",
    "R-Car M3W rev 3.0",
    "R-Car M3W rev unknown",
    "R-Car M3N rev 1.0",
    "R-Car M3N rev 1.1",
    "R-Car M3N rev 2.0",
    "R-Car M3N rev 3.0",
    "R-Car M3N rev unknown",
    "R-Car V3 rev 1.0",
    "R-Car V3 rev 1.1",
    "R-Car V3 rev 2.0",
    "R-Car V3 rev 3.0",
    "R-Car V3 rev unknown",
    "R-Car D3 rev 1.0",
    "R-Car D3 rev 1.1",
    "R-Car D3 rev 2.0",
    "R-Car D3 rev 3.0",
    "R-Car D3 rev unknown"
};

void init_product_id()
{
    uint32_t reg = in32(RCAR_GEN3_PRODUCT_REGISTER);

    switch(RCAR_GEN3_PRODUCT_ID(reg)) {
        case PRODUCT_ID_RCAR_H3:
            product_id = RCAR_H3;
            switch (RCAR_GEN3_PRODUCT_REV(reg)) {
                case PRODUCT_H3_REV_1_0:
                    product_rev = REV_1_0;
                    break;
                case PRODUCT_H3_REV_1_1:
                    product_rev = REV_1_1;
                    break;
                case PRODUCT_H3_REV_2_0:
                    product_rev = REV_2_0;
                    break;
                case PRODUCT_H3_REV_3_0:
                    product_rev = REV_3_0;
                    break;
                default:
                    product_rev = REV_UNKNOWN;
                    break;
            }
            break;

        case PRODUCT_ID_RCAR_M3W:
            product_id = RCAR_M3W;
            switch(RCAR_GEN3_PRODUCT_REV(reg)) {
                case PRODUCT_M3_REV_1_0:
                    product_rev = REV_1_0;
                    break;
                case PRODUCT_M3_REV_1_1:
                    product_rev = REV_1_1;
                    break;
                default:
                    product_rev = REV_UNKNOWN;
                    break;
            }
            break;

        case PRODUCT_ID_RCAR_M3N:
            product_id = RCAR_M3N;
            switch(RCAR_GEN3_PRODUCT_REV(reg)) {
                case PRODUCT_M3N_REV_1_0:
                    product_rev = REV_1_0;
                    break;
                case PRODUCT_M3N_REV_1_1:
                    product_rev = REV_1_1;
                    break;
                default:
                    product_rev = REV_UNKNOWN;
                    break;
            }
            break;

        case PRODUCT_ID_RCAR_V3:
            product_id = RCAR_V3;
            switch (RCAR_GEN3_PRODUCT_REV(reg)) {
                case PRODUCT_V3M_REV_1_0:
                    product_rev = REV_1_0;
                    break;
                case PRODUCT_V3M_REV_2_0:
                    product_rev = REV_2_0;
                    break;
                default:
                    product_rev = REV_UNKNOWN;
                    break;
            }
            break;

        case PRODUCT_ID_RCAR_D3:
            product_id = RCAR_D3;
            switch (RCAR_GEN3_PRODUCT_REV(reg)) {
                case PRODUCT_D3_REV_1_0:
                    product_rev = REV_1_0;
                    break;
                default:
                    product_rev = REV_UNKNOWN;
                    break;
            }
            break;
        default:
            break;
    }
}

char *get_soc_name()
{
    char *name;
    if (product_id != ID_NUM) {
        name = soc_name[REV_NUM*(product_id) + (product_rev) + 1];
    } else {
        name = soc_name[0];
    }

    return name;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/boards/rcar_gen3/soc_name.c $ $Rev: 853070 $")
#endif
