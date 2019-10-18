/*
* $QNXLicenseC:
* Copyright (c) 2017, QNX Software Systems. All Rights Reserved.
*
* You must obtain a written license from and pay applicable license fees to QNX
* Software Systems before you may reproduce, modify or distribute this software,
* or any work that includes all or part of this software. Free development
* licenses are available for evaluation and non-commercial purposes. For more
* information visit http://licensing.qnx.com or email licensing@qnx.com.
*
* This file may contain contributions from others. Please review this entire
* file for other proprietary rights or license notices, as well as the QNX
* Development Suite License Guide at http://licensing.qnx.com/license-guide/
* for other information.
* $
*/

/*h
 * init_raminfo.c
 *  Tell syspage about our RAM configuration
 */
#include "startup.h"
#include "aarch64/r-car-gen3.h"
#include <rcar_board.h>

extern void rcar_add_ram(rcar_ram *ram, uint8_t detect_ram);

uint8_t detect_ram = 0;

void init_raminfo()
{
    uint32_t reg = in32(RCAR_GEN3_PRODUCT_REGISTER);

    if (RCAR_GEN3_PRODUCT_ID(reg) == PRODUCT_ID_RCAR_M3W)
    {
        /*
         * M3 Salvator-X
         *   ch0 (0x40000000)
         *     rank0: 1GB
         *     rank1: 1GB
         *   ch1 (0x600000000)
         *     rank0: 1GB
         *     rank1: 1GB
         * M3 Starter Kit
         *   ch0 (0x40000000)
         *     rank0: 1GB
         *   ch1 (0x600000000)
         *     rank0: 1GB
         */
        rcar_ram ram[] = {
          {.base=RCAR_GEN3_SDRAM_0, .rank={RCAR_GEN3_DBSC4_DBMEMCONF_0_0, RCAR_GEN3_DBSC4_DBMEMCONF_0_1, RCAR_GEN3_DBSC4_DBMEMCONF_0_2, RCAR_GEN3_DBSC4_DBMEMCONF_0_3}, .size=GIG(1)},
          {.base=RCAR_GEN3_SDRAM_2, .rank={RCAR_GEN3_DBSC4_DBMEMCONF_1_0, RCAR_GEN3_DBSC4_DBMEMCONF_1_1, RCAR_GEN3_DBSC4_DBMEMCONF_1_2, RCAR_GEN3_DBSC4_DBMEMCONF_1_3}, .size=GIG(1)},
          { 0 }
        };
#if 0
        /* M3 Salvator-XS rev1.1(4GB) */
        rcar_ram ram[] = {
          {.base=RCAR_GEN3_SDRAM_0, .rank={RCAR_GEN3_DBSC4_DBMEMCONF_0_0, RCAR_GEN3_DBSC4_DBMEMCONF_0_1, RCAR_GEN3_DBSC4_DBMEMCONF_0_2, RCAR_GEN3_DBSC4_DBMEMCONF_0_3}, .size=GIG(2)},
          {.base=RCAR_GEN3_SDRAM_2, .rank={RCAR_GEN3_DBSC4_DBMEMCONF_1_0, RCAR_GEN3_DBSC4_DBMEMCONF_1_1, RCAR_GEN3_DBSC4_DBMEMCONF_1_2, RCAR_GEN3_DBSC4_DBMEMCONF_1_3}, .size=GIG(2)},
          { 0 }
        };
#endif

        rcar_add_ram(ram, detect_ram);
    }
    else if (RCAR_GEN3_PRODUCT_ID(reg) == PRODUCT_ID_RCAR_M3N) {
        rcar_ram ram[] = {
          {.base=RCAR_GEN3_SDRAM_0, .rank={RCAR_GEN3_DBSC4_DBMEMCONF_0_0, RCAR_GEN3_DBSC4_DBMEMCONF_0_1, RCAR_GEN3_DBSC4_DBMEMCONF_0_2, RCAR_GEN3_DBSC4_DBMEMCONF_0_3}, .size=GIG(1)},
          { 0 }
        };

        rcar_add_ram(ram, detect_ram);
    }


    // Remove Secure Region (0x43F00000-0x47DFFFFF) from sysram
    alloc_ram(0x43F00000, 0x3F00000, 0);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/boards/rcar_gen3/rcar_m3/init_raminfo.c $ $Rev: 846264 $")
#endif
