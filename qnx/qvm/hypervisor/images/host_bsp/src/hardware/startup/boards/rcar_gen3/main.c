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


#include "startup.h"
#include <aarch64/r-car-gen3.h>
#include "rcar_startup.h"

extern void init_board(void);
extern void wdt_enable(void);
extern uint32_t aarch64_midr(void);
extern void  init_product_id(void);
extern char* get_soc_name(void);
extern void board_mmu_enable();
extern void board_mmu_disable();

extern uint8_t detect_ram;
extern uint8_t max_cpus_cluster0;

extern struct callout_rtn   reboot_rcarh3;
const struct callout_slot callouts[] = {
    { CALLOUT_SLOT( reboot, _rcarh3 ) },
};

const struct debug_device debug_devices_scif0[] = {
    {   "scif",
        {   "0xE6E60000.115200.14745600.16",    /* SCIF0, use the baud rate set by boot loader */
        },
        init_scif,
        put_scif,
        {   &display_char_scif,
            &poll_key_scif,
            &break_detect_scif,
        }
    },
};

const struct debug_device debug_devices_scif2[] = {
    {   "scif",
        {   "0xE6E88000.115200.14745600.16",    // SCIF2, use the baud rate set by boot loader
        },
        init_scif,
        put_scif,
        {   &display_char_scif,
            &poll_key_scif,
            &break_detect_scif,
        }
    },
};

/*
 * Get CPU frequency
 */
uint32_t
cpu_freq_get(uint32_t midr_cpu_id) {
    uint32_t freq = 0;

    // FRQCRC and PLLxCR are not available for V3M and D3.
    if (product_id == RCAR_V3) {
        // V3m cpu frequency is 800MHz
        return (800 * 1000 * 1000);
    } else if (product_id == RCAR_D3) {
        // D3 cpu frequency is 1000MHz
        return (1000 * 1000 * 1000);
    }

    if (midr_cpu_id == 0) {
        // Get the freq for the current cpu
        midr_cpu_id = MIDR_CPU_ID(aarch64_midr());
    }

    if (midr_cpu_id == MIDR_CPU_A57_ID) {
        if (freq == 0) {
            uint32_t    frqcrc = in32(RCAR_GEN3_CPG_BASE + RCAR_GEN3_CPG_FRQCRC);
            uint32_t    pll0cr = in32(RCAR_GEN3_CPG_BASE + RCAR_GEN3_CPG_PLL0CR);
            if (in32(RCAR_GEN3_CPG_BASE + RCAR_GEN3_CPG_PLLECR) & (1 << 8)) {
                freq = (32 - ((frqcrc >> 8 ) & 0x1F)) / 32 * (((pll0cr >> 24) & 0x7F) + 1);
                freq *= RCAR_GEN3_EXT_CLK;
            } else {
                freq = ((pll0cr >> 24) & 0x7F) * RCAR_GEN3_EXT_CLK;
            }
        }
    } else if (midr_cpu_id == MIDR_CPU_A53_ID) {
        if (freq == 0) {
            uint32_t    frqcrc = in32(RCAR_GEN3_CPG_BASE + RCAR_GEN3_CPG_FRQCRC);
            uint32_t    pll2cr = in32(RCAR_GEN3_CPG_BASE + RCAR_GEN3_CPG_PLL2CR);
            if (in32(RCAR_GEN3_CPG_BASE + RCAR_GEN3_CPG_PLLECR) & (1 << 10)) {
                freq = (32 - (frqcrc & 0x1F)) / 32 * (((pll2cr >> 24) & 0x7F) + 1);
                freq *= RCAR_GEN3_EXT_CLK;
            } else {
                freq = ((pll2cr >> 24) & 0x7F) * RCAR_GEN3_EXT_CLK;
            }
        }
    } else {
        kprintf("cpu_freq_get: Warning: Unknown CPU ID %x. Set cpu_freq = 0\n", midr_cpu_id);
    }
    return freq;
}

void
allocate_codec_memory(paddr_t addr, size_t size)
{
    alloc_ram(addr, size, 1);
    hwi_add_device(HWI_ITEM_BUS_UNKNOWN, HWI_ITEM_DEVCLASS_DMA, "misc", 0);
    hwi_add_location(addr, size, 0, hwi_find_as(addr, 0));
    as_add_containing(addr, addr + size - 1, AS_ATTR_RAM, "codec", "ram");
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
int main(int argc, char **argv, char **envv)
{
    int     opt;
    char    *p;
    paddr_t codecmem_addr = 0;
    size_t  codecmem_size = 0;

    init_product_id();
    add_callout_array(callouts, sizeof(callouts));

    while ((opt = getopt(argc, argv, COMMON_OPTIONS_STRING "L:Wdp:")) != -1) {
        switch (opt) {
            case 'W':
                /* Enable WDT */
                wdt_enable();
                break;
            case 'L':
                codecmem_addr = getsize(optarg, &p);
                if (*p == ',')
                    codecmem_size = getsize(p + 1, &p);
                break;
            case 'd':
                detect_ram = 1;
                break;
            case 'p':
                max_cpus_cluster0 = strtoul(optarg, &optarg, 0);
                break;
            default:
                handle_common_option(opt);
                break;
        }
    }

    /*
     * Initialize debugging output
     */
    switch(product_id) {
        case RCAR_H3:
        case RCAR_M3W:
        case RCAR_M3N:
        case RCAR_D3:
            select_debug(debug_devices_scif2, sizeof(debug_devices_scif2));
            break;
        case RCAR_V3:
            select_debug(debug_devices_scif0, sizeof(debug_devices_scif0));
            break;
        default:
            break;
    }


    /*
     * Collect information on all free RAM in the system
     */
    init_board();
    init_raminfo();

    /*
     * Set Timer frequency
     */
    if (product_id == RCAR_D3) {
        // Xtal = 48MHz
        timer_freq = RCAR_GEN3_EXT_CLK_D3 / 2;
    } else {
        // Xtal = 16.6666MHz
        timer_freq = RCAR_GEN3_EXT_CLK / 2;
    }

    cpu_freq = cpu_freq_get(0); // 0 to get the freq of the current cpu

    /*
     * Remove RAM used by modules in the image
     */
    alloc_ram(shdr->ram_paddr, shdr->ram_size, 1);

    /* Reserve RAM for codec */
    if (codecmem_size != 0)
        allocate_codec_memory(codecmem_addr, codecmem_size);

    /* Reserve RAM for hypervisor demo */
#if 0
    {
        paddr_t address, size;

        address = 0x58000000;
        size = (32*1024*1024);
        alloc_ram(address, size, 1);
        as_add_containing(address, address + size - 1, AS_ATTR_RAM, "gpu-h-fw", "memory");

        address = 0x5a000000;
        size = (96*1024*1024);
        alloc_ram(address, size, 1);
        as_add_containing(address, address + size - 1, AS_ATTR_RAM, "qnx-screen-fbuffer", "memory");

        address = 0x60000000;
        size = (512*1024*1024);
        alloc_ram(address, size, 1);
        as_add_containing(address, address + size - 1, AS_ATTR_RAM, "guests", "memory");

        address = 0x600000000;
        size = (96*1024*1024);
        alloc_ram(address, size, 1);
        as_add_containing(address, address + size - 1, AS_ATTR_RAM, "gpu-h", "memory");

        address = 0x620000000;
        size = (512*1024*1024);
        alloc_ram(address, size, 1);
        as_add_containing(address, address + size - 1, AS_ATTR_RAM, "guests", "memory");
    }
#else    
    {
        paddr_t address, size;

        address = 0x58000000;
        size = (32*1024*1024);
        alloc_ram(address, size, 1);
        as_add_containing(address, address + size - 1, AS_ATTR_RAM, "gpu-h-fw", "memory");

        address = 0x5a000000;
        size = (96*1024*1024);
        alloc_ram(address, size, 1);
        as_add_containing(address, address + size - 1, AS_ATTR_RAM, "qnx-screen-fbuffer", "memory");

        address = 0x60000000;
        size = (512*1024*1024);
        alloc_ram(address, size, 1);
        as_add_containing(address, address + size - 1, AS_ATTR_RAM, "guests", "memory");

        address = 0x80000000;
        size = (192*1024*1024);
        alloc_ram(address, size, 1);
        as_add_containing(address, address + size - 1, AS_ATTR_RAM, "gpu-h", "memory");
		
        address = 0x90000000;
        size = (768*1024*1024);
        alloc_ram(address, size, 1);
        as_add_containing(address, address + size - 1, AS_ATTR_RAM, "guests", "memory");

        address = 0x600000000;
        size = (32*1024*1024);
        alloc_ram(address, size, 1);
        as_add_containing(address, address + size - 1, AS_ATTR_RAM, "gpu-fw-g1", "memory");

        address = 0x602000000;
        size = (1760*1024*1024);
        alloc_ram(address, size, 1);
        as_add_containing(address, address + size - 1, AS_ATTR_RAM, "guests", "memory");
    }
#endif

    /*
    * Initialise SMP
    */
    init_smp();

    if (shdr->flags1 & STARTUP_HDR_FLAGS1_VIRTUAL){
        board_mmu_enable();
        init_mmu();
    }

    init_intrinfo();
    init_qtime();
    init_cacheattr();
    init_cpuinfo();
    init_hwinfo();

    add_typed_string(_CS_MACHINE, get_soc_name());

    /*
     * Load bootstrap executables in the image file system and Initialise
     * various syspage pointers. This must be the _last_ initialisatnion done
     * before transferring control to the next program.
     */
    init_system_private();
    board_mmu_disable();
    /*
     * This is handy for debugging a new version of the startup program.
     * Commenting this line out will save a great deal of code.
     */
    print_syspage();

    return 0;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/boards/rcar_gen3/main.c $ $Rev: 850459 $")
#endif
