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
#include "rcar_startup.h"
#include <aarch64/r-car-gen3.h>

static uintptr_t rcar_gpio_vbase[8];

/*
 * Initialize interrupt controller hardware & intrinfo structure in the
 * system page.
 * This code is hardware dependant and may have to be changed
 * changed by end users.
 */

const static struct startup_intrinfo intrs[] =
{
    // GPIO0 interrupt
    {   512,                            // vector base
        32,                             // number of vectors
        RCAR_GEN3_INTCSYS_GPIO0,             // cascade vector
        0,                              // CPU vector base
        0,                              // CPU vector stride
        0,                              // flags
        { 0, 0, &interrupt_id_rcar_gpio},
        { INTR_GENFLAG_LOAD_INTRMASK, 0, &interrupt_eoi_rcar_gpio},
        &interrupt_mask_rcar_gpio,      // mask   callout
        &interrupt_unmask_rcar_gpio,    // unmask callout
        0,                              // config callout
        &rcar_gpio_vbase[0]
    },
    // GPIO1 interrupt
    {   512 + 32,                       // vector base
        32,                             // number of vectors
        RCAR_GEN3_INTCSYS_GPIO1,             // cascade vector
        0,                              // CPU vector base
        0,                              // CPU vector stride
        0,                              // flags
        { 0, 0, &interrupt_id_rcar_gpio},
        { INTR_GENFLAG_LOAD_INTRMASK, 0, &interrupt_eoi_rcar_gpio},
        &interrupt_mask_rcar_gpio,      // mask   callout
        &interrupt_unmask_rcar_gpio,    // unmask callout
        0,                              // config callout
        &rcar_gpio_vbase[1]
    },
    // GPIO2 interrupt
    {   512 + 64,                       // vector base
        32,                             // number of vectors
        RCAR_GEN3_INTCSYS_GPIO2,             // cascade vector
        0,                              // CPU vector base
        0,                              // CPU vector stride
        0,                              // flags
        { 0, 0, &interrupt_id_rcar_gpio},
        { INTR_GENFLAG_LOAD_INTRMASK, 0, &interrupt_eoi_rcar_gpio},
        &interrupt_mask_rcar_gpio,      // mask   callout
        &interrupt_unmask_rcar_gpio,    // unmask callout
        0,                              // config callout
        &rcar_gpio_vbase[2]
    },
    // GPIO3 interrupt
    {   512 + 96,                       // vector base
        32,                             // number of vectors
        RCAR_GEN3_INTCSYS_GPIO3,             // cascade vector
        0,                              // CPU vector base
        0,                              // CPU vector stride
        0,                              // flags
        { 0, 0, &interrupt_id_rcar_gpio},
        { INTR_GENFLAG_LOAD_INTRMASK, 0, &interrupt_eoi_rcar_gpio},
        &interrupt_mask_rcar_gpio,      // mask   callout
        &interrupt_unmask_rcar_gpio,    // unmask callout
        0,                              // config callout
        &rcar_gpio_vbase[3]
    },
    // GPIO4 interrupt
    {   512 + 128,                      // vector base
        32,                             // number of vectors
        RCAR_GEN3_INTCSYS_GPIO4,             // cascade vector
        0,                              // CPU vector base
        0,                              // CPU vector stride
        0,                              // flags
        { 0, 0, &interrupt_id_rcar_gpio},
        { INTR_GENFLAG_LOAD_INTRMASK, 0, &interrupt_eoi_rcar_gpio},
        &interrupt_mask_rcar_gpio,      // mask   callout
        &interrupt_unmask_rcar_gpio,    // unmask callout
        0,                              // config callout
        &rcar_gpio_vbase[4]
    },
    // GPIO5 interrupt
    {   512 + 160,                      // vector base
        32,                             // number of vectors
        RCAR_GEN3_INTCSYS_GPIO5,             // cascade vector
        0,                              // CPU vector base
        0,                              // CPU vector stride
        0,                              // flags
        { 0, 0, &interrupt_id_rcar_gpio},
        { INTR_GENFLAG_LOAD_INTRMASK, 0, &interrupt_eoi_rcar_gpio},
        &interrupt_mask_rcar_gpio,      // mask   callout
        &interrupt_unmask_rcar_gpio,    // unmask callout
        0,                              // config callout
        &rcar_gpio_vbase[5]
    },
};

const static struct startup_intrinfo gpio6_intr[] =
{
    // GPIO6 interrupt
    {   512 + 192,                      // vector base
        32,                             // number of vectors
        RCAR_GEN3_INTCSYS_GPIO6,        // cascade vector
        0,                              // CPU vector base
        0,                              // CPU vector stride
        0,                              // flags
        { 0, 0, &interrupt_id_rcar_gpio},
        { INTR_GENFLAG_LOAD_INTRMASK, 0, &interrupt_eoi_rcar_gpio},
        &interrupt_mask_rcar_gpio,      // mask   callout
        &interrupt_unmask_rcar_gpio,    // unmask callout
        0,                              // config callout
        &rcar_gpio_vbase[6]
    }
};
const static struct startup_intrinfo gpio7_intr[] =
{
    // GPIO7 interrupt
    {   512 + 224,                      // vector base
        32,                             // number of vectors
        RCAR_GEN3_INTCSYS_GPIO7,        // cascade vector
        0,                              // CPU vector base
        0,                              // CPU vector stride
        0,                              // flags
        { 0, 0, &interrupt_id_rcar_gpio},
        { INTR_GENFLAG_LOAD_INTRMASK, 0, &interrupt_eoi_rcar_gpio},
        &interrupt_mask_rcar_gpio,      // mask   callout
        &interrupt_unmask_rcar_gpio,    // unmask callout
        0,                              // config callout
        &rcar_gpio_vbase[7]
    }
};

static void irqc_irq_unmask(uint32_t irq)
{
    uint32_t base;

    base = RCAR_GEN3_IRQC0_BASE;
    out32(base + RCAR_GEN3_INTEN_SET0, in32(base + RCAR_GEN3_INTEN_SET0) | (1 << (irq & 0x1F)));
}

void init_intrinfo()
{
    unsigned    i;
    paddr_t     irqc_base0 = RCAR_GEN3_IRQC0_BASE;

    gic_v2_init(RCAR_GEN3_GIC_DIST_BASE, RCAR_GEN3_GIC_CPU_BASE);

    /* Enable IRQC & INTC-SYS clock */
    out32(RCAR_GEN3_CPG_BASE + RCAR_GEN3_CPG_SMSTPCR4,
            in32(RCAR_GEN3_CPG_BASE + RCAR_GEN3_CPG_SMSTPCR4) & ~((1 << 7) | (1 << 8)));

    /* Mask IRQC interrupts */
    out32(irqc_base0 + RCAR_GEN3_INTEN_STS0, 0x0000001F);

    /* Clear IRQC interrupts */
    out32(irqc_base0 + RCAR_GEN3_DETECT_STATUS, 0x0000001F);

    /* Enable IRQC 0: PMIC */
    irqc_irq_unmask(0);

    /* All interrupts high-level */
    for (i = 0; i <= 5; i += 1) {
        out32(irqc_base0 + RCAR_GEN3_CONFIG_00 + i*4, (in32(irqc_base0 + RCAR_GEN3_CONFIG_00 + i*4) & ~0x3F) | 0x2);
    }

    rcar_gpio_vbase[0] = callout_io_map(0x100, RCAR_GEN3_GPIO0_BASE);
    rcar_gpio_vbase[1] = callout_io_map(0x100, RCAR_GEN3_GPIO1_BASE);
    rcar_gpio_vbase[2] = callout_io_map(0x100, RCAR_GEN3_GPIO2_BASE);
    rcar_gpio_vbase[3] = callout_io_map(0x100, RCAR_GEN3_GPIO3_BASE);
    rcar_gpio_vbase[4] = callout_io_map(0x100, RCAR_GEN3_GPIO4_BASE);
    rcar_gpio_vbase[5] = callout_io_map(0x100, RCAR_GEN3_GPIO5_BASE);

    add_interrupt_array(intrs, sizeof(intrs));

    switch(product_id) {
        case RCAR_H3:
        case RCAR_M3W:
        case RCAR_M3N:
        case RCAR_D3:
            /* M3W, M3N, H3, D3, and E3 have GPIO 6 */
            rcar_gpio_vbase[6] = callout_io_map(0x100, RCAR_GEN3_GPIO6_BASE);
            add_interrupt_array(gpio6_intr, sizeof(gpio6_intr));

            if (product_id == RCAR_D3) {
                break;
            }

            /* M3W, M3N, and H3 have GPIO 7 */
            rcar_gpio_vbase[7] = callout_io_map(0x100, RCAR_GEN3_GPIO7_BASE);
            add_interrupt_array(gpio7_intr, sizeof(gpio7_intr));
            break;
        case RCAR_V3:
        default:
            break;
    }


}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/boards/rcar_gen3/init_intrinfo.c $ $Rev: 850459 $")
#endif
