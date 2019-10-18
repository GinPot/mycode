/*
 * $QNXLicenseC:
 * Copyright 2011, QNX Software Systems.
 * Copyright 2016, Freescale Semiconductor, Inc.
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

#include "startup.h"
#include <aarch64/gic_v3.h>


/*
 * GICv3 support code
 */

static struct startup_intrinfo gic_v3_intr_ppi = {
    .vector_base     = _NTO_INTR_CLASS_EXTERNAL,
    .num_vectors     = 32,
    .cascade_vector  = _NTO_INTR_SPARE,
    .cpu_intr_base   = 0,
    .cpu_intr_stride = 0,
    .flags           = 0,
    .id              = { 0, 0, &interrupt_id_gic_v3_ppi_sr },
    .eoi             = { 0, 0, &interrupt_eoi_gic_v3_ppi_sr },
    .mask            = &interrupt_mask_gic_v3_ppi,
    .unmask          = &interrupt_unmask_gic_v3_ppi,
    .config          = &interrupt_config_gic_v3_ppi,
    .patch_data      = NULL,
#ifdef INTR_CONFIG_FLAG_SYS // we can remove this after the new header are propagated
    .local_stride	 = 0x8000,
#endif
};


static struct startup_intrinfo gic_v3_intr_spi = {
    .vector_base     = _NTO_INTR_CLASS_EXTERNAL + 32,
    .num_vectors     = 0, //patched based on the number supported by the GIC
    .cascade_vector  = _NTO_INTR_SPARE,
    .cpu_intr_base   = 0,
    .cpu_intr_stride = 0,
    .flags           = 0,
    .id              = { 0, 0, &interrupt_id_gic_v3_spi },
    .eoi             = { 0, 0, &interrupt_eoi_gic_v3_spi_sr },
    .mask            = &interrupt_mask_gic_v3_spi,
    .unmask          = &interrupt_unmask_gic_v3_spi,
    .config          = NULL,
    .patch_data      = NULL,
};


/**
 * Wait for a GICR/GICD state update to propagate
 * @param	addr	paddr containing the RWP bit
 * @param	rwp_bit	which bit is RWP indicator
 */
static void
wait_for_rwp(paddr_t const addr, unsigned const rwp_bit) {
	//XXXX: check for never clearing
	do {
		// Wait for the RWP bit to clear
	} while(in32(addr) & rwp_bit);
}


/**
 * Initialize CPU specific parts of the GIC interface
 *
 * @param cpu CPU core index.
 */
static void
gic_v3_gicc_init(unsigned cpu) {
	/*
	 * Initialize CPU GIC redistributor part.
	 */
    if (gic_gicr == 0) {
        crash("gic redistributor not initialised");
    }

    paddr_t gic_core_gicr = gic_gicr;

    /* Get core affinity */
	uint64_t const mpidr = aa64_sr_rd64(mpidr_el1);
    uint64_t const cpu_aff = (uint32_t)((mpidr & 0xFFFFFF) | ((mpidr >> 8) & 0xFF000000));
    do {
        /* Get GICR affinity bits (offset + 4, [32 - 63] bits of TYPER register) */
        uint32_t const gicr_aff = in32(gic_core_gicr + ARM_GICR_TYPER + 4);
        if (cpu_aff == gicr_aff) {
            break;
        }
		//XXXX: Doesn't work for GICv4
        gic_core_gicr += ARM_GICR_SIZE_PER_CORE;
		//XXXX: check for LAST bit
    } while (1);

	//XXXX: Make sure ProcessorSleep bit is off in WAKER?

    /* SGI base address */
    gic_core_gicr += ARM_GICR_SGI_BASE_OFFSET;
    if (debug_flag > 1) {
        kprintf("cpu%d: Core GICR SGI address: 0x%x\n", cpu, gic_core_gicr);
    }

    /* All interrupts are in Group1 */
    out32(gic_core_gicr + ARM_GICR_IGROUPR0, 0xFFFFFFFF);
    /* SGI interrupts are in Group1NS */
    out32(gic_core_gicr + ARM_GICR_IGRPMODR0, 0x00000000);

    /* Set SGI/PPI priority (all 0xA0, except for SGI0 which is 0xC0) */
    out32(gic_core_gicr + ARM_GICR_IPRIORITYR0, 0xA0A0A0C0);
	unsigned i;
	for(i = 1; i < 8; ++i) {
		out32(gic_core_gicr + ARM_GICR_IPRIORITYR0 + (i*4), 0xA0A0A0A0);
	}


    /* Clear/disable all SGI/PPI interrupts */
    out32(gic_core_gicr + ARM_GICR_ICPENDR0, 0xFFFFFFFF);
    out32(gic_core_gicr + ARM_GICR_ICENABLER0, 0xFFFFFFFF);

    /* Enable SGI 0 interrupt (used for IPI's) */
    out32(gic_core_gicr + ARM_GICR_ISENABLER0, 0x0000001);

	wait_for_rwp(gic_core_gicr - ARM_GICR_SGI_BASE_OFFSET + ARM_GICR_CTLR, 0x08);

	/*
	 * Initialize CPU GIC interface part.
	 */

	struct aarch64_gic_map_entry *gic_map = lsp.cpu.aarch64_gic_map.p;
	if(gic_sendipi == &sendipi_gic_v3_mm) {
		// using memory mapped interface */
		//XXXX: doesn't work for GICv4
    	gic_map->gic_cpu[cpu] = (ARM_GICR_SIZE_PER_CORE*cpu)
						+ ARM_GICR_SGI_BASE_OFFSET+ARM_GICR_ISPENDR0;

		// set priority mask to allow all interrupts
		out32(gic_gicc + ARM_GICC_PMR, 0xF0);
		// set binary point value
		out32(gic_gicc + ARM_GICC_BPR, 0);
		// enable group 1 interrupts, disable IRQ/FIQ bypass signals
		out32(gic_gicc + ARM_GICC_CTLR, 0x61);
	} else {
		/* Create value for ARM_GICC_SGI1R_EL1 register used in "sendipi" callout function */
		uint32_t icc_sgi1r = (0x01U << (cpu_aff & 0x0F));
		//XXXX: Looses Aff2/Aff3 values
		icc_sgi1r |= ((cpu_aff & 0xFF00) << 8);
		gic_map->gic_cpu[cpu] =  icc_sgi1r;

		/* Enable the system register interface (icc_sre_el1) */
		aa64_sr_wr32(S3_0_C12_C12_5, 7);
		isb();

		/* Set priority mask to allow all interrupts and enable cpu interface Set ARM_GICC_PMR = 0x0F */
		aa64_sr_wr32(S3_0_C4_C6_0, 0xf0);

		/* Set binary point value, ARM_GICC_BPR1_EL1 */
		aa64_sr_wr32(S3_0_C12_C12_3, 0);

		/* set EOImode to drop/deactivate, ARM_GICC_CTLR_EL1) */
		aa64_sr_wr32(S3_0_C12_C12_4, 0);

		/* Enable signaling of each interrupt group, ARM_GICC_IGRPEN1_ELx = 1 */
		aa64_sr_wr32(S3_0_C12_C12_7, 1);
	}

	if (debug_flag > 1) {
		kprintf("cpu%d: GICv3 cpu interface 0x%x\n", cpu, gic_map->gic_cpu[cpu]);
	}
}


/**
 * Initialize CPU core GIC distributor part.
 *
 * @param gicd  GIC distributor base address.
 * @param gicr  GIC CPU redistributor base address.
 * @param gicc  GIC CPU interface base address.
 * @param nlpi	Minimum number of LPI's to be supported
 * @param use_mm force use of the MMIO interface to GICC registers
 */
void
gic_v3_init(paddr_t gicd, paddr_t gicr, paddr_t gicc, unsigned const nlpi, int use_mm) {
    struct aarch64_gic_map_entry *gic_map = lsp.cpu.aarch64_gic_map.p;
    unsigned    itn;
    unsigned    i;

	if(nlpi != 0) {
		crash("gic_v3_init: LPI support not yet added");
	}

	if((aa64_sr_rd64(id_aa64pfr0_el1) & (0xful << 24)) == 0) {
		use_mm = 1;
	}

	unsigned const off = as_find(AS_NULL_OFF, "memory", NULL);
	as_add64(gicd, gicd + 0xffffu, AS_ATTR_DEV, "gicd", off);
	//XXXX: Incorrect size for GICv4
	size_t const gicr_size = ARM_GICR_SIZE_PER_CORE * lsp.syspage.p->num_cpu;
	as_add64(gicr, gicr + gicr_size - 1, AS_ATTR_DEV, "gicr", off);
	as_add64(gicc, gicc + 0x1fffu, AS_ATTR_DEV, "gicc", off);

    gic_gicd = gicd;
	gic_gicd_vaddr = callout_io_map(0x10000, gicd);
    gic_gicr = gicr;
	gic_gicr_vaddr = callout_io_map(gicr_size, gicr);
    gic_gicc = gicc;
	if(use_mm) {
		gic_gicc_vaddr = callout_io_map(ARM_GICC_SIZE, gicc);
	}
    gic_cpu_init = gic_v3_gicc_init;

    /* Initialize the GIC cpu map with invalid values. */
    for (i = 0; i < lsp.syspage.p->num_cpu; i++) {
        gic_map->gic_cpu[i] = ~0u;
    }

    /* Disable distributor */
    out32(gicd + ARM_GICD_CTLR, 0);
	wait_for_rwp(gicd + ARM_GICD_CTLR, 0x80000000);

    /* Calculate number of interrupt lines */
    itn = ((in32(gicd + ARM_GICD_TYPER) & ARM_GICD_TYPER_ITLN) + 1) * 32;
    if (debug_flag > 1) {
        kprintf("GICv3: %d interrupts\n", itn);
    }
    gic_v3_intr_spi.num_vectors = min(1020, itn) - 32;

    /* Disable all interrupts and clear pending state */
    for (i = 32; i < itn; i += 32) {
        out32(gicd + ARM_GICD_ICENABLERn + (i * 4 / 32), 0xFFFFFFFF);
        out32(gicd + ARM_GICD_ICPENDRn + (i * 4 / 32), 0xFFFFFFFF);
		/* All interrupts are in Group1 */
        out32(gicd + ARM_GICD_IGROUPRn + (i * 4 / 32), 0xFFFFFFFF);
		/* SGI interrupts are in Group1NS */
        out32(gicd + ARM_GICD_IGRPMODRn + (i * 4 / 32), 0x00000000);
    }

    /* Set default priority of all SPI interrupts to 0xA0 */
    for (i = 8; i < itn; i += 4) {
        out32(gicd + ARM_GICD_IPRIORITYn + i, 0xA0A0A0A0);
    }

    /* Default all SPI interrupts as level triggered */
    for (i = 32; i < itn; i += 16) {
        out32(gicd + ARM_GICD_ICFGRn + (i * 4 / 16), 0);
    }
	wait_for_rwp(gicd + ARM_GICD_CTLR, 0x80000000);

    /* Enable GICD */
    out32(gicd + ARM_GICD_CTLR,
			ARM_GICD_CTLR_ARE_S|ARM_GICD_CTLR_EN_GRP1_NS|ARM_GICD_CTLR_EN_GRP0);

	wait_for_rwp(gicd + ARM_GICD_CTLR, 0x80000000);

    /* Route all SPI interrupts to cpu0 (affinity 0.0.0.0) */
	/* Must be done after ARE is set */
    for (i = 0; i < gic_v3_intr_spi.num_vectors; i++) {
        ((uint64_t *)(gicd + ARM_GICD_IROUTERn))[i+32] = (0ULL << 32) |
                                                      (0ULL << 16) |
                                                      (0ULL << 8)  |
                                                      (0ULL << 0);
    }

    /*
     * Add the interrupt callouts
     */
	if(use_mm) {
    	 gic_sendipi = &sendipi_gic_v3_mm;
		 gic_v3_intr_ppi.id.rtn  = &interrupt_id_gic_v3_ppi_mm;
		 gic_v3_intr_ppi.eoi.rtn = &interrupt_eoi_gic_v3_ppi_mm;
		 gic_v3_intr_spi.eoi.rtn = &interrupt_eoi_gic_v3_spi_mm;
	} else {
    	gic_sendipi = &sendipi_gic_v3_sr;
	}
    add_interrupt(&gic_v3_intr_ppi);
	// The _spi routines MUST be added after the _ppi (id callout depends on it)
    add_interrupt(&gic_v3_intr_spi);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/aarch64/gic_v3.c $ $Rev: 848162 $")
#endif
