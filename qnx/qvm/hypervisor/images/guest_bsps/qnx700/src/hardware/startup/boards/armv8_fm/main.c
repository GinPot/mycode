/*
 * $QNXLicenseC:
 * Copyright 2017, QNX Software Systems. All Rights Reserved.
 *
 * You must obtain a written license from and pay applicable license fees to QNX
 * Software Systems before you may reproduce, modify or distribute this software,
 * or any work that includes all or part of this software.   Free development
 * licenses are available for evaluation and non-commercial purposes.  For more
 * information visit http://licensing.qnx.com or email licensing@qnx.com.
 *
 * This file may contain contributions from others.  Please review this entire
 * file for other proprietary rights or license notices, as well as the QNX
 * Development Suite License Guide at http://licensing.qnx.com/license-guide/
 * for other information.
 * $
 */

/*
 * ARMv8 Foundation Model
 */

#include "startup.h"

// Foundation Model has a specific interrupt init
extern void init_intrinfo_fm(unsigned gic_version, int use_mm);


#define DEFAULT_CPU_FREQ 100000000

extern struct callout_rtn reboot;
const struct callout_slot callouts_hvc[] = {
    { CALLOUT_SLOT( reboot, _psci_smc ) },
};
const struct callout_slot callouts[] = {
    { CALLOUT_SLOT( reboot, ) },
};

const struct debug_device debug_devices[] = {
    {   "pl011",
        {
            "0x1c090000^0.38400.24000000",
            "0x1c0A0000^0.38400.24000000",
        },
        init_pl011,
        put_pl011,
        {
            &display_char_pl011,
            &poll_key_pl011,
            &break_detect_pl011,
        }
    },
};

#if defined(__aarch64__)
    #define FDT_REG     0
#else
    #define FDT_REG     2
#endif


/**
 * Adjust bootstrap executable command lines based on "/chosen" node's "bootargs" property
 * @param   bap         argument and environment list
 * @param   name    string associated to list
 */
void
tweak_cmdline(struct bootargs_entry *bap, const char *name) {
    if(boot_regs[FDT_REG]) {
        fdt_tweak_cmdline(bap, name, boot_regs[FDT_REG]);
    }
}


/*
 * main()
 *  Startup program executing out of RAM
 *
 * 1. It gathers information about the system and places it in a structure
 *    called the system page. The kernel references this structure to
 *    determine everything it needs to know about the system. This structure
 *    is also available to user programs (read only if protection is on)
 *    via _syspage->.
 *
 * 2. It (optionally) turns on the MMU and starts the next program
 *    in the image file system.
 */
int
main(int argc, char **argv, char **envv)
{
    int         opt;

    // Check for and initialize flattened device tree
    if(boot_regs[FDT_REG]) {
        fdt_init(boot_regs[FDT_REG]);
        fdt_psci_configure();
    }

    /*
     * FIXME_AARCH64: need to figure out how to get RAM size
     */
    paddr_t     memsize = MEG(1024);

    // Allow to specify specific GIC version
    unsigned gic_version = 0;

	int use_mm = 0;
    while ((opt = getopt(argc, argv, COMMON_OPTIONS_STRING "m:23b")) != -1) {
        switch(opt) {
        case 'm':
            memsize = getsize(optarg, NULL);
            break;

        case '2':
            gic_version = 2;
            break;

        case '3':
            gic_version = 3;
            break;

        case 'b':
            use_mm = 1;
            break;

        default:
            handle_common_option(opt);
            break;
        }
    }

    if(in_hvc) {
        // Need to decide if smc or hvc interface
        // If fdt present, scan for psci indicator
        add_callout_array(callouts_hvc, sizeof(callouts_hvc));
    } else {
        add_callout_array(callouts, sizeof(callouts));
    }

    /*
     * Initialise debugging output
     */
    select_debug(debug_devices, sizeof(debug_devices));

    /*
     * Set the default cpu frequency of 100MHz unless override by -f option or if provided by FDT
     */
    if (cpu_freq == 0) {
        if (fdt_size != 0) {
            cpu_freq = fdt_get_cpu_freq();
        }

        if ( cpu_freq == 0 ) {
            cpu_freq = DEFAULT_CPU_FREQ;
        }
    }

#if defined(__aarch64__)
    if(timer_freq == 0) {
        timer_freq = aa64_sr_rd32(cntfrq_el0);
        if(timer_freq == 0) {
            timer_freq = DEFAULT_CPU_FREQ;
        }
    }
#endif

    /*
     * Collect information on all free RAM in the system
     */
    if(fdt_size != 0) {
        init_raminfo_fdt();
        fdt_asinfo();
    } else {
        add_ram(0x80000000, memsize);
    }

    /*
     * Remove RAM used by modules in the image
     */
    alloc_ram(shdr->ram_paddr, shdr->ram_size, 1);

	/* Reserve RAM for graphics */
#if 1
	{
		paddr_t address, size;

		address = 0x60000000;
		size = (32*1024*1024) + 0x100000;
		alloc_ram(address, size, 1);
		as_add_containing(address, address + size - 1, AS_ATTR_RAM, "gpu-g-fw", "memory");

		address = 0x62100000;
		size = (96*1024*1024);
		alloc_ram(address, size, 1);
		as_add_containing(address, address + size - 1, AS_ATTR_RAM, "qnx-screen-fbuffer", "memory");

		address = 0x620000000;
		size = (96*1024*1024);
		alloc_ram(address, size, 1);
		as_add_containing(address, address + size - 1, AS_ATTR_RAM, "gpu-g", "memory");
	}
#endif

    /*
     * Initialise SMP
     */
    init_smp();

    if (shdr->flags1 & STARTUP_HDR_FLAGS1_VIRTUAL) {
        init_mmu();
    }
    init_intrinfo_fm(gic_version, use_mm);
    init_qtime();
    init_cacheattr();
    init_cpuinfo();
    init_hwinfo();
    add_typed_string(_CS_MACHINE, "ARMv8 Foundation Model");

    /*
     * Load bootstrap executables in the image file system and Initialise
     * various syspage pointers. This must be the _last_ initialisation done
     * before transferring control to the next program.
     */
    init_system_private();

    /*
     * This is handy for debugging a new version of the startup program.
     * Commenting this line out will save a great deal of code.
     */
    print_syspage();

    return 0;
}



#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/boards/armv8_fm/main.c $ $Rev: 845888 $")
#endif
