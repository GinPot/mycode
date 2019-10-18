/*
 * $QNXLicenseC:
 * Copyright 2016, 2017 QNX Software Systems.
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

#include "startup.h"
#include "aarch64/psci.h"
#include <aarch64/r-car-gen3.h>
#include "rcar_startup.h"

extern uint32_t aarch64_midr(void);
extern uint32_t aarch64_mpidr(void);

typedef struct cluster_s {
    uint32_t       id;       // A57 or A53
    uint8_t        idx;      // Cluster idx in the mpidr register
    uint8_t        num_cpu;  // Number of available cores in the cluster
    uint32_t       cpu_freq; // The frequency of the cpus in the cluster
} cluster;

static cluster clusters[CLUSTERS_NUM_MAX];

/*
 * max_cpus: Maximum number of cpus in both clusters.
 * max_cpus_cluster0: Maximum number of cpus in the boot cluster (cluster 0)
 */
uint8_t max_cpus_cluster0 = ~0;

/*
 * psci calls are supported if the board boots first into the Arm Trusted firmware, which sets up the
 * PSCI lib. When booting without ATF, no support for PSCI is available.
 */
uint32_t psci_support = 1;

/*
 * Secondary cpus entry point is _start. the cpu then waits for the secondary_core_start to be set
 * before jumping to that adress to continue the execution.
 */
uintptr_t secondary_core_start = 0;


/*
 * R-CAR Gen3 CPUs
 * H3:  4 A53 cores, 4 A57 cores
 * M3W: 4 A53 cores, 2 A57 cores
 * V3M: 2 A53 cores
 */

unsigned num_cpus_in_cluster(uint32_t cluster_id){
    uint32_t prr   = in32(RCAR_GEN3_PRODUCT_REGISTER);
    uint32_t cpu_mask;
    uint8_t  num_cpu = 0;

    if (cluster_id == MIDR_CPU_A53_ID) {
        cpu_mask = RCAR_GEN3_PRODUCT_CA53EN(prr);
    } else if (cluster_id == MIDR_CPU_A57_ID) {
        cpu_mask = RCAR_GEN3_PRODUCT_CA57EN(prr);
    } else {
         // Unknown cluster. Disable all cores.
         cpu_mask = 0xF;
    }

    for (int i = 0 ; i < 4; i++) {
        // cpu_mask bits: 0 cpu available, 1 cpu not available
        if ((cpu_mask & (1 << i)) == 0)
            num_cpu++;
    }

    return num_cpu;
}

unsigned
board_smp_num_cpu()
{
    unsigned num_cpu;

    // Boot cluster
    clusters[0].idx      = MPIDR_CLUSTER_ID(aarch64_mpidr());
    clusters[0].id       = MIDR_CPU_ID(aarch64_midr());
    clusters[0].num_cpu  = min(num_cpus_in_cluster(clusters[0].id), max_cpus_cluster0);
    clusters[0].cpu_freq = cpu_freq;

    // Secondary cluster
    clusters[1].idx      = (clusters[0].idx == 0) ? 1 : 0;
    clusters[1].id       = (clusters[0].id == MIDR_CPU_A53_ID) ? MIDR_CPU_A57_ID : MIDR_CPU_A53_ID;
    clusters[1].num_cpu  = num_cpus_in_cluster(clusters[1].id);
    clusters[1].cpu_freq = cpu_freq_get(clusters[1].id);

    num_cpu = clusters[0].num_cpu + clusters[1].num_cpu;

    kprintf("board_smp_num_cpu: %d cores, max allowed %d\n", num_cpu, max_cpus);
    return num_cpu;
}

static void power_scu_on(uint32_t cluster_id)
{
    if (MIDR_CPU_A57_ID == cluster_id) {
        if (in32(RCAR_GEN3_APMU_BASE  + RCAR_GEN3_APMU_CA57CPUCMCR) != 0) {
            out32(RCAR_GEN3_APMU_BASE  + RCAR_GEN3_APMU_CA57CPUCMCR, 0x0);
        }
        // CA57 sysc power domain 5, IER bit 12, PWRONCR bit 0
        power_domain_on(5, 12, 0);
    } else {
        if (in32(RCAR_GEN3_APMU_BASE  + RCAR_GEN3_APMU_CA53CPUCMCR) != 0) {
            out32(RCAR_GEN3_APMU_BASE  + RCAR_GEN3_APMU_CA53CPUCMCR, 0x0);
        }

        // CA53 sysc power domain 3, IER bit 21, PWRONCR bit 0
        power_domain_on(3, 21, 0);
    }
}

static void power_cpu_on(uint8_t cpu_no, uint32_t cluster_id)
{
    uintptr_t cpg_wupcr;
    uintptr_t reset_rescnt;
    uint32_t  reset_rescnt_val;

    if (MIDR_CPU_A53_ID == cluster_id) {
        // A53 side
        reset_rescnt = RCAR_GEN3_RESET_CA53RESCNT;
        cpg_wupcr  = RCAR_GEN3_APMU_CA53WUPCR;
        reset_rescnt_val = 0x5A5A0000U;
    } else {
        // A57 side
        reset_rescnt = RCAR_GEN3_RESET_CA57RESCNT;
        cpg_wupcr  = RCAR_GEN3_APMU_CA57WUPCR;
        reset_rescnt_val = 0xA5A50000U;
    }

    power_scu_on(cluster_id);

    // Wakeup CPUx
    out32(RCAR_GEN3_CPG_BASE + RCAR_GEN3_CPG_CPGWPR, ~(1 << cpu_no));
    out32(RCAR_GEN3_CPG_BASE + cpg_wupcr, (1 << cpu_no));
    while ((in32(RCAR_GEN3_CPG_BASE + cpg_wupcr) & (1 << cpu_no)) == (1 << cpu_no)) { ; }

    // Dessert CPUx reset
    reset_rescnt_val |= in32(RCAR_GEN3_RESET_BASE + reset_rescnt);
    reset_rescnt_val &= ~(1 << (3 - cpu_no));
    out32(RCAR_GEN3_RESET_BASE + reset_rescnt, reset_rescnt_val);
}

void
board_smp_init(struct smp_entry *smp, unsigned num_cpus)
{
    smp->send_ipi = (void *)&sendipi_gic_v2;
}

int
board_smp_start(unsigned cpu, void (*start)(void))
{
    int32_t rval;
    uint8_t cluster;

    if (debug_flag) {
        kprintf("board_smp_start: cpu%d @ %v\n", cpu, start);
    }

    board_mmu_disable();

    if (cpu < clusters[0].num_cpu) {
        // cpu is in the boot cluster
        cluster = 0;
    } else {
        // cpu is in the secondary cluster
        cluster = 1;
        cpu = cpu - clusters[0].num_cpu;
    }

    // Set the frequency of the target cpu
    cpu_freq = clusters[cluster].cpu_freq;

    if (psci_support) {
        // Set the cluster idx
        cpu |= (clusters[cluster].idx << 8);

        rval = psci_cpu_on(cpu, (uint64_t)start, 0);

        if (rval != PSCI_SUCCESS) {
            // Secondary cpu start failed.
            // If the boot cluster is the A57, make sure the "PSCI_DISABLE_BIGLITTLE_IN_CA57BOOT" in
            // the ARM trusted firmware, BL31, is disabled.
            crash("psci_cpu_on returned %x, cpu: %x\n", rval, cpu);
        }
    } else {
        // Turn on secondary cpu. Entry point is _start. the cpu waits for the secondary_core_start
        // to be set.
        power_cpu_on(cpu, clusters[cluster].id);
        secondary_core_start = (uintptr_t)start;

        // Get the secondary cpu out of the wfe
        __asm__ __volatile__(
            "dsb sy\n"
            "sev\n"
            : : : "memory"
        );
    }

    return 1;
}

unsigned
board_smp_adjust_num(unsigned cpu)
{
    return cpu;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/boards/rcar_gen3/board_smp.c $ $Rev: 850328 $")
#endif
