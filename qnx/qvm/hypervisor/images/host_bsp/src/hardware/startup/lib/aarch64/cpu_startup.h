/*
 * $QNXLicenseC:
 * Copyright 2014, QNX Software Systems.
 * Copyright 2016, Freescale Semiconductor, Inc.
 * Copyright 2017, NXP.
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

#include <aarch64/cpu.h>
#include <aarch64/inout.h>
#include <aarch64/inline.h>

/********************************************************************
*********************************************************************

	These definitions are required for the system independent code.

*********************************************************************
********************************************************************/

/*
 * FIXME_AARCH64
 * The 0xe7ffffef encoding is currently in the unassigned space but
 * we need to get a permanently undefined encoding to use here...
 *
 * We also need to figure out how to handle forced kercalls for AArch32
 * mode execution (needs an ARM svc and Thumb2 svc sequence)
 */
#define KERCALL_SEQUENCE(name)	uint32_t name[] = {			\
			0xd4000a21,		/* svc #0x51				*/	\
			0xe7ffffef		/* undefined instruction	*/	\
}

#define CPU_SYSPAGE_TYPE	SYSPAGE_AARCH64

struct cpu_local_syspage {
	SYSPAGE_SECTION(aarch64_gic_map);
};

#define BOOTSTRAPS_RUN_ONE_TO_ONE	0

#define CPU_COMMON_OPTIONS_STRING	""

#define CPU_PADDR_BITS			48

/********************************************************************
*********************************************************************

	Everything below is specific to ARMv8

*********************************************************************
********************************************************************/

#define	PROT_DEVICE		PROT_CALLOUT

struct aarch64_cpuid {
	unsigned	midr;
	const char	*name;
	void		(*cpuinfo)(unsigned cpunum, struct cpuinfo_entry *info);
};
extern struct aarch64_cpuid	*aarch64_cpuid[];
extern _Uint32t				aarch64_cpuspeed(void);
extern unsigned				syspage_roundup;

/*
 * MMU Setup
 */
extern void		aarch64_map_init(void);
extern paddr_t	aarch64_map(uintptr_t va, paddr_t pa, size_t sz, int flags);

extern unsigned			sctlr_clr;
extern unsigned			sctlr_el1;
extern unsigned	long	mair_el1;
extern unsigned	long	tcr_el1;
extern unsigned long	ttbr0;
extern unsigned long	ttbr1[PROCESSORS_MAX];
extern _Uint64t			L0_vaddr;
extern _Uint64t			boot_regs[4];   /* x0-x3 at time of entry to _start */
extern int				have_el2;
extern int				trying_hvc;
extern void				aarch64_enable_mmu(uint64_t tlb);
extern void				aarch64_disable_mmu(void);
extern void				aarch64_alignment_check_enable(void);
extern void				aarch64_alignment_check_disable(void);
extern void				aarch64_dcache_enable(void);
extern void				aarch64_dcache_disable(void);
extern void				aarch64_icache_enable(void);
extern void				aarch64_icache_disable(void);

/*
 * ------------------------------------------------------------------
 * IMX support
 * ------------------------------------------------------------------
 */
extern void imx_init_uart(unsigned channel, const char *init, const char *defaults);
extern void imx_uart_put_char(int);

extern struct callout_rtn	imx_uart_display_char;
extern struct callout_rtn	imx_uart_poll_key;
extern struct callout_rtn	imx_uart_break_detect;

/*
 * ------------------------------------------------------------------
 * ARMv8 Cores
 * ------------------------------------------------------------------
 */

extern struct aarch64_cpuid	cpuid_v8_fm;
extern struct aarch64_cpuid	cpuid_v8_aem;
extern struct aarch64_cpuid	cpuid_a35;
extern struct aarch64_cpuid	cpuid_a53;
extern struct aarch64_cpuid	cpuid_a57;
extern struct aarch64_cpuid	cpuid_a72;
extern struct aarch64_cpuid	cpuid_d15;
extern struct aarch64_cpuid	cpuid_d20;

/*
 * ------------------------------------------------------------------
 * ARMv8 Cache Callouts
 * ------------------------------------------------------------------
 */

extern struct callout_rtn	cache_armv8_icache;
extern struct callout_rtn	cache_armv8_dcache;

/*
 * ------------------------------------------------------------------
 * GIC Support
 * ------------------------------------------------------------------
 */

extern paddr_t				gic_gicd;
extern paddr_t				gic_gicc;
extern void					(*gic_cpu_init)(unsigned);
extern struct callout_rtn	*gic_sendipi;

extern void 				gicv_asinfo(paddr_t gicv, paddr_t banked_gich, paddr_t direct_gich, unsigned direct_num, unsigned direct_spacing);
extern void					gic_v2_init(paddr_t gicd, paddr_t gicc);
extern struct callout_rtn	interrupt_id_gic_v2;
extern struct callout_rtn	interrupt_eoi_gic_v2;
extern struct callout_rtn	interrupt_mask_gic_v2;
extern struct callout_rtn	interrupt_unmask_gic_v2;
extern struct callout_rtn	interrupt_config_gic_v2;

// GIC ver.3
extern paddr_t				gic_gicr;
extern uintptr_t			gic_gicc_vaddr;
extern uintptr_t			gic_gicr_vaddr;
extern uintptr_t			gic_gicd_vaddr;
extern void					gic_v3_init(paddr_t gicd, paddr_t gicr, paddr_t gicc, unsigned nlpi, int use_mm);
extern struct callout_rtn	interrupt_id_gic_v3_ppi_sr;
extern struct callout_rtn	interrupt_id_gic_v3_ppi_mm;
extern struct callout_rtn	interrupt_eoi_gic_v3_ppi_sr;
extern struct callout_rtn	interrupt_eoi_gic_v3_ppi_mm;

extern struct callout_rtn	interrupt_mask_gic_v3_ppi;
extern struct callout_rtn	interrupt_unmask_gic_v3_ppi;
extern struct callout_rtn	interrupt_config_gic_v3_ppi;

extern struct callout_rtn	interrupt_id_gic_v3_spi;
extern struct callout_rtn	interrupt_eoi_gic_v3_spi_sr;
extern struct callout_rtn	interrupt_eoi_gic_v3_spi_mm;
extern struct callout_rtn	interrupt_mask_gic_v3_spi;
extern struct callout_rtn	interrupt_unmask_gic_v3_spi;
extern struct callout_rtn	sendipi_gic_v3_sr;
extern struct callout_rtn	sendipi_gic_v3_mm;

/*
 * ------------------------------------------------------------------
 * ARMv8 Generic Timer
 * ------------------------------------------------------------------
 */

extern struct callout_rtn	timer_load_armv8;
extern struct callout_rtn	timer_value_armv8;
extern struct callout_rtn	timer_reload_armv8;

/*
 * ------------------------------------------------------------------
 * PrimeCell PL011 UART support
 * ------------------------------------------------------------------
 */

extern void init_pl011(unsigned, const char *, const char *);
extern void put_pl011(int);

extern struct callout_rtn	display_char_pl011;
extern struct callout_rtn	poll_key_pl011;
extern struct callout_rtn	break_detect_pl011;

/*
 * ------------------------------------------------------------------
 * nVidia Tegra UART support
 * ------------------------------------------------------------------
 */

extern struct callout_rtn	display_char_tegra;
extern struct callout_rtn	poll_key_tegra;
extern struct callout_rtn	break_detect_tegra;

/*
 * ------------------------------------------------------------------
 * Renesas support
 * ------------------------------------------------------------------
 */
extern void init_scif(unsigned, const char *, const char *);
extern void put_scif(int);

extern struct callout_rtn   display_char_scif;
extern struct callout_rtn   poll_key_scif;
extern struct callout_rtn   break_detect_scif;

extern struct callout_rtn   interrupt_id_rcar_gpio;
extern struct callout_rtn   interrupt_eoi_rcar_gpio;
extern struct callout_rtn   interrupt_mask_rcar_gpio;
extern struct callout_rtn   interrupt_unmask_rcar_gpio;

/*
 * Temporary hack - will use definition in shared location once
 * hypervisor migrated to main branch.
 */
#if !defined(HAVE_ORIGINAL_EL2_REGS)
#define HAVE_ORIGINAL_EL2_REGS
struct aarch64_original_el2_regs {
	_Uint64t	vbar;
	_Uint64t	sctlr;
	_Uint64t	hcr;
	_Uint64t	vttbr;
	_Uint64t	vtcr;
	_Uint64t	tcr;
	_Uint64t	ttbr0;
	_Uint64t	tpidr;
	_Uint64t	sp;
};
#endif

extern struct aarch64_original_el2_regs	original_el2_regs;
extern void el2_transfer_table_allocate(void);
extern void el2_transfer_table_activate(void);

#include "common_arm/armboth_startup.h"

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/aarch64/cpu_startup.h $ $Rev: 850140 $")
#endif
