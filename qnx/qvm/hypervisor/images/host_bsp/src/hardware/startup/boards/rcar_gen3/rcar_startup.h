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

#ifndef __RCAR_STARTUP_H_INCLUDED
#define __RCAR_STARTUP_H_INCLUDED

#define CLUSTERS_NUM_MAX    2
#define MPIDR_CLUSTER_ID(x) (((x) >> 8) & 0xFF)
#define MIDR_CPU_ID(x)      (((x) >> 4) & 0xFFF)
#define MIDR_CPU_A53_ID     0xD03
#define MIDR_CPU_A57_ID     0xD07

typedef enum {
    RCAR_H3,
    RCAR_M3W,
    RCAR_M3N,
    RCAR_V3,
    RCAR_D3,
    ID_NUM
} product_id_t;

typedef enum {
    REV_1_0,
    REV_1_1,
    REV_2_0,
    REV_3_0,
    REV_UNKNOWN,
    REV_NUM
} product_rev_t;

extern product_id_t  product_id;
extern product_rev_t product_rev;

uint32_t cpu_freq_get(uint32_t midr_cpu_id);
void power_domain_on(uint8_t domain, uint8_t irq_bit, uint8_t pwroncr_bit);

#endif //__RCAR_STARTUP_H_INCLUDED

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/boards/rcar_gen3/rcar_startup.h $ $Rev: 850459 $")
#endif
