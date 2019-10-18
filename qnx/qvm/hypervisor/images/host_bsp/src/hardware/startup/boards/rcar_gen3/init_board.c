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
#include <errno.h>
#include <aarch64/r-car-gen3.h>
#include <rcar_board.h>
#include "rcar_startup.h"

//#define PFC_DEBUG

#ifdef PFC_DEBUG
 #define PFC_DEBUG_PRINT(format, ...) kprintf(format, ##__VA_ARGS__)
#else
 #define PFC_DEBUG_PRINT(format, ...)
#endif

extern int rcar_usb3_dl_fw(uint32_t regs);

int read_boot_device(){
    int modemr = in32(RCAR_GEN3_RESET_BASE + RCAR_GEN3_MODEMR);
    return (modemr & RCAR_GEN3_MODEMR_BOOT_MASK);
}

// Power up sequence as per Renesas Manual "Power Resume/Shutoff flowchart"
void power_domain_on(uint8_t domain, uint8_t irq_bit, uint8_t pwroncr_bit)
{
    uint32_t pwroncr_mask = BIT(pwroncr_bit);
    uint32_t irq_mask     = BIT(irq_bit);

    if ((in32(RCAR_GEN3_SYSC_BASE + RCAR_GEN3_SYSC_PWRSRx(domain)) & pwroncr_mask) == 0x0) {
        return;
    }

    // 1. Enable power interrupt
    out32(RCAR_GEN3_SYSC_BASE + RCAR_GEN3_SYSC_IER,
                (in32(RCAR_GEN3_SYSC_BASE + RCAR_GEN3_SYSC_IER) | irq_mask));
    out32(RCAR_GEN3_SYSC_BASE + RCAR_GEN3_SYSC_IMR,
                (in32(RCAR_GEN3_SYSC_BASE + RCAR_GEN3_SYSC_IMR) | irq_mask));
    do {
        //  2. Check to confirm power resume is ready to start
        while ((in32(RCAR_GEN3_SYSC_BASE + RCAR_GEN3_SYSC_SR) & (1 << 1)) == 0) { ; }

        //  3. Set SYSCEXTMASK and power resume/shutdown control register
        if (! (domain < 2 || domain == 6)) {  // Not valid for domains: 0, 1, 6
            // EXTMASK is only valid for V3M
            if (product_id == RCAR_V3) {
                out32(RCAR_GEN3_SYSC_BASE + RCAR_GEN3_SYSC_EXTMASK_V3M, 1);
            }
            out32(RCAR_GEN3_SYSC_BASE + RCAR_GEN3_SYSC_PWRONCRx(domain), pwroncr_mask);
        }

        //  4. Check whether the request has been accepted
    } while ((in32(RCAR_GEN3_SYSC_BASE + RCAR_GEN3_SYSC_PWRERx(domain)) & pwroncr_mask) != 0);

    //  5. Wait for the completion of power sequence
    while ((in32(RCAR_GEN3_SYSC_BASE + RCAR_GEN3_SYSC_ISR) & irq_mask) == 0) { ; }

    out32(RCAR_GEN3_SYSC_BASE + RCAR_GEN3_SYSC_ISCR, irq_mask);
    if (product_id == RCAR_V3) {
        out32(RCAR_GEN3_SYSC_BASE + RCAR_GEN3_SYSC_EXTMASK_V3M, 0);
    }

    //  Check the power-up
    while ((in32(RCAR_GEN3_SYSC_BASE + RCAR_GEN3_SYSC_PWRSRx(domain)) & pwroncr_mask) == 0) { ; }
}

void usec_delay(uint32_t usec)
{
    /* Stop TMU0 before changing settings */
    uint8_t start_reg = in8(RCAR_GEN3_TMU_BASE + RCAR_GEN3_TMU_TSTR0);
    out8(RCAR_GEN3_TMU_BASE + RCAR_GEN3_TMU_TSTR0, start_reg & ~RCAR_GEN3_TMU_START0);

    /*
     * Input-clock for TMU0 is CP = EXTAL/2 = 16.66MHz/2
     * TPSC(RCAR_GEN3_TMU_TCR[2:0]) = 0: clock_div = 4
     * Each cycle = 1/(CP/clock_div) = 480ns
     * 2 cycles = 960ns ~= 1us
     */
    if(usec & (1 << 31)){
        kprintf("usec_delay: warning - invalid delay value\n");
    }
    usec <<= 1;

    out16(RCAR_GEN3_TMU0_BASE + RCAR_GEN3_TMU_TCR, RCAR_GEN3_TMU_UNIE);
    out32(RCAR_GEN3_TMU0_BASE + RCAR_GEN3_TMU_TCOR, usec);
    out32(RCAR_GEN3_TMU0_BASE + RCAR_GEN3_TMU_TCNT, usec);

    /* Start TMU0 */
    out8(RCAR_GEN3_TMU_BASE + RCAR_GEN3_TMU_TSTR0, start_reg | RCAR_GEN3_TMU_START0);

    while(!(in16(RCAR_GEN3_TMU0_BASE + RCAR_GEN3_TMU_TCR) & RCAR_GEN3_TMU_UNF));
    out8(RCAR_GEN3_TMU_BASE + RCAR_GEN3_TMU_TSTR0, start_reg & ~RCAR_GEN3_TMU_START0);
}

uint32_t rcar_detect_ram(uint32_t channel)
{
    uint32_t config, capacity;
    uint8_t row, col, density;

    config = in32(RCAR_GEN3_DBSC4_BASE + channel);
    if(!config) {
        // Can't access DBSC, or rank not populated
        return 0;
    }

    row = config >> 24 & 0x1F;
    if(row < 12 || row > 16){
        kprintf("Invalid row address bit width\n");
        return 0;
    }

    col = config >> 8 & 0x0F;
    if(col < 10 || col > 12){
        kprintf("Invalid col address bit width\n");
        return 0;
    }

    density = config >> 31 & 0x03;
    if(density > 1){
        kprintf("Invalid memory density type\n");
        return 0;
    }

    /*
     * R-Car Gen3 SDRAM Calculation
     * (Refer to Section 21.3.5 Address Configuration of R-Car Series, 3rd Gen Hardware Manual)
     * Capacity(bytes) = banks * row * column * 32-bit External Bus / 8
     */
    capacity = 1 << (row + col + 5);

    if(density) {
        capacity >>= 2;
        capacity *= 3;
    }

    // Max 2GB is supported
    if(capacity > 0x80000000){
        kprintf("Invalid size\n");
        return 0;
    }

    return capacity;
}

void rcar_add_ram(rcar_ram *ram, uint8_t detect_ram)
{
    paddr_t size, total = 0;
    int     r, b;

    /*
     * Ram size detection would fail if LIFE_DBSC_PROTECT_ENABLED is set in bootloader.
     * To use ram size detection:
     *   Make sure LIFE_DBSC_PROTECT_ENABLED is unset in bootloader, and use startup -d parameter
     */
    if(detect_ram) {
        // DBSC can be accessed, detect ram size
        for(b = 0; ram[b].base; b++) {
            size = 0;
            for(r = 0; r < RCAR_GEN3_MAX_RANK; r++) {
                size += (paddr_t)rcar_detect_ram(ram[b].rank[r]);
            }

            if(size) {
                add_ram(ram[b].base, size);
                total += size;
            }
        }
        if(!total) {
            kprintf("Unable to detect ram size: try using predefined ram sizes by removing the startup -d parameter\n");
        }
    } else {
        // DBSC cannot be accessed, use predefined ram sizes
        for(b = 0; ram[b].base; b++) {
            add_ram(ram[b].base, ram[b].size);
        }
    }

}

static void rcar_pfc_config(int reg, uint32_t val)
{
    out32(RCAR_GEN3_PFC_BASE + RCAR_GEN3_PFC_PMMR, ~val);
    out32(RCAR_GEN3_PFC_BASE + reg, val);
}

static void rcar_cpg_config(int reg, uint32_t val)
{
    out32(RCAR_GEN3_CPG_BASE + RCAR_GEN3_CPG_CPGWPR, ~val);
    out32(RCAR_GEN3_CPG_BASE + reg, val);
}

/*
 * Power on modules
 */
static void module_pwr(rcar_pwr * pwr)
{
    rcar_pwr *p;

    if(pwr == NULL)
        return;

    p = pwr;
    while(p->base != 0)
    {
        out32(RCAR_GEN3_SYSC_BASE + p->base, p->setting);
        while(in32(RCAR_GEN3_SYSC_BASE + p->status) != p->power_on)
            usec_delay(100);
        p++;
    }
}

/*
 * Set CLK Supply
 *  Enable CLK: set corresponding SMSTPCR bits to 0
 *  Disable CLK: set corresponding SMSTPCR bits to 1
 */
static void module_clk(rcar_module * mod, rcar_pin * cpg)
{
    rcar_pin    tmp = { 0 };
    rcar_module *p;

    if(mod == NULL)
        return;

    /* Module cpg settings */
    if(cpg != NULL)
    {
        /* disable all related submodules */
        p = mod;
        while(p->clk.reg != 0)
        {
            if (tmp.reg != p->clk.reg)
            {
                if (tmp.reg != 0)
                {
                    rcar_cpg_config(tmp.reg, (in32(RCAR_GEN3_CPG_BASE + tmp.reg) & ~(tmp.bits)) | tmp.val);
                    PFC_DEBUG_PRINT("  module_clk disable clk reg=0x%x val=0x%x\n", RCAR_GEN3_CPG_BASE + tmp.reg, in32(RCAR_GEN3_CPG_BASE + tmp.reg));
                }
                tmp.reg = p->clk.reg;
                tmp.bits = tmp.val = 0;
            }

            tmp.bits |= p->clk.bits;
            tmp.val |= p->clk.bits;

            p++;
        }
        if (tmp.reg != 0)
        {
            rcar_cpg_config(tmp.reg, (in32(RCAR_GEN3_CPG_BASE + tmp.reg) & ~(tmp.bits)) | tmp.val);
            PFC_DEBUG_PRINT("  module_clk disable clk modules reg=0x%x val=0x%x\n", RCAR_GEN3_CPG_BASE + tmp.reg, in32(RCAR_GEN3_CPG_BASE + tmp.reg));
        }

        /* make CPG settings */
        while(cpg->reg != 0)
        {
            rcar_cpg_config(cpg->reg, (in32(RCAR_GEN3_CPG_BASE + cpg->reg) & ~(cpg->bits)) | cpg->val);
            PFC_DEBUG_PRINT("  module_clk cpg reg=0x%x val=0x%x\n", RCAR_GEN3_CPG_BASE + cpg->reg, in32(RCAR_GEN3_CPG_BASE + cpg->reg));
            cpg++;
        }
    }

    /* Module clock settings */
    tmp.reg = tmp.bits = tmp.val = 0;
    p = mod;
    while(p->clk.reg != 0)
    {
        if (tmp.reg != p->clk.reg)
        {
            if (tmp.reg != 0)
            {
                rcar_cpg_config(tmp.reg, (in32(RCAR_GEN3_CPG_BASE + tmp.reg) & ~(tmp.bits)) | tmp.val);
                PFC_DEBUG_PRINT("  module_clk clk reg=0x%x val=0x%x\n", RCAR_GEN3_CPG_BASE + tmp.reg, in32(RCAR_GEN3_CPG_BASE + tmp.reg));
            }
            tmp.reg = p->clk.reg;
            tmp.bits = tmp.val = 0;
        }

        tmp.bits |= p->clk.bits;
        if (DISABLE == p->clk.val)
        {
            tmp.val |= p->clk.bits;
        }

        p++;
    }
    if (tmp.reg != 0)
    {
        rcar_cpg_config(tmp.reg, (in32(RCAR_GEN3_CPG_BASE + tmp.reg) & ~(tmp.bits)) | tmp.val);
        PFC_DEBUG_PRINT("  module_clk clk reg=0x%x val=0x%x\n", RCAR_GEN3_CPG_BASE + tmp.reg, in32(RCAR_GEN3_CPG_BASE + tmp.reg));
    }
}

static void module_gpio(rcar_pinmux * pin, uint32_t reg, uint32_t set)
{
    if (set)
    {
        out32(pin->gpio.reg + reg, in32(pin->gpio.reg + reg) | pin->gpsr.bits);
    }
    else
    {
        out32(pin->gpio.reg + reg, in32(pin->gpio.reg + reg) & ~(pin->gpsr.bits));
    }
    PFC_DEBUG_PRINT("  module_gpio reg=0x%x val=0x%x\n", pin->gpio.reg + reg, in32(pin->gpio.reg + reg));

}

static void module_pinmux(rcar_pinmux * pinmux)
{
    rcar_pin    gpsr = { 0 }, ipsr = { 0 };
    rcar_pinmux *p;
    int i = 0;

    if (pinmux == NULL)
        return;

    /* process gpsr and ipsr */
    p = pinmux;
    i = 0;
    while(p->gpsr.reg != 0)
    {
        if ((p->gpsr.val == GPSR_IP && ipsr.reg != p->ipsr.reg) || (gpsr.reg != p->gpsr.reg))
        {
            if (ipsr.reg != 0)
            {
                /* You must set corresponding GPSR bits to GPIO, before changing IPSR */
                rcar_pfc_config(gpsr.reg, in32(RCAR_GEN3_PFC_BASE + gpsr.reg) & ~(gpsr.bits));
                rcar_pfc_config(ipsr.reg, (in32(RCAR_GEN3_PFC_BASE + ipsr.reg) & ~(ipsr.bits)) | ipsr.val);
                PFC_DEBUG_PRINT("  module_pinmux[%d] IPSR reg=0x%x val=0x%x\n", i, RCAR_GEN3_PFC_BASE + ipsr.reg, in32(RCAR_GEN3_PFC_BASE + ipsr.reg));
                ipsr.val = ipsr.bits = 0;
            }

            if (gpsr.reg != 0)
            {
                rcar_pfc_config(gpsr.reg, (in32(RCAR_GEN3_PFC_BASE + gpsr.reg) & ~(gpsr.bits)) | gpsr.val);
                PFC_DEBUG_PRINT("  module_pinmux[%d] GPSR reg=0x%x val=0x%x\n", i, RCAR_GEN3_PFC_BASE + gpsr.reg, in32(RCAR_GEN3_PFC_BASE + gpsr.reg));
                gpsr.val = gpsr.bits = 0;
            }
            gpsr.reg = p->gpsr.reg;
        }

        if (p->gpsr.val == GPSR_IP)
        {
            ipsr.reg = p->ipsr.reg;
            ipsr.bits |= 0xF << p->ipsr.bits;
            ipsr.val |= p->ipsr.val << p->ipsr.bits;
        }

        gpsr.bits |= p->gpsr.bits;
        if (p->gpsr.val != GPSR_IO )
        {
            gpsr.val |= p->gpsr.bits;
        }

        p++; i++;
    }

    if (ipsr.reg != 0)
    {
        rcar_pfc_config(gpsr.reg, in32(RCAR_GEN3_PFC_BASE + gpsr.reg) & ~(gpsr.bits));
        rcar_pfc_config(ipsr.reg, (in32(RCAR_GEN3_PFC_BASE + ipsr.reg) & ~(ipsr.bits)) | ipsr.val);
        PFC_DEBUG_PRINT("  module_pinmux[%d] IPSR reg=0x%x val=0x%x\n", i, RCAR_GEN3_PFC_BASE + ipsr.reg, in32(RCAR_GEN3_PFC_BASE + ipsr.reg));
    }
    if (gpsr.reg != 0)
    {
        rcar_pfc_config(gpsr.reg, (in32(RCAR_GEN3_PFC_BASE + gpsr.reg) & ~(gpsr.bits)) | gpsr.val);
        PFC_DEBUG_PRINT("  module_pinmux[%d] GPSR reg=0x%x val=0x%x\n", i, RCAR_GEN3_PFC_BASE + gpsr.reg, in32(RCAR_GEN3_PFC_BASE + gpsr.reg));
    }

    /* process gpio */
    p = pinmux;
    while(p->gpsr.reg != 0)
    {
        if (p->gpsr.val != GPSR_IO)
        {
            p++;
            continue;
        }

        /*
         * IOINTSEL: 0=GPIO, 1=INT
         * INOUTSEL: 0=INPUT, 1=OUTPUT (IO only)
         */
        switch (p->gpio.type)
        {
            case GPIO_IN:
                module_gpio(p, RCAR_GEN3_GPIO_IOINTSEL, 0);
                module_gpio(p, RCAR_GEN3_GPIO_INOUTSEL, 0);
                break;
            case GPIO_OUT:
                module_gpio(p, RCAR_GEN3_GPIO_IOINTSEL, 0);
                module_gpio(p, RCAR_GEN3_GPIO_INOUTSEL, 1);
                module_gpio(p, RCAR_GEN3_GPIO_OUTDT, p->gpio.val);
                break;
            case GPIO_INT:
                module_gpio(p, RCAR_GEN3_GPIO_IOINTSEL, 1);
                break;
            default:
                PFC_DEBUG_PRINT("  module_pinmux invalid gpio type\n");
                break;
        }
        p++;
    }
}

static void module_settings(rcar_pin * settings)
{
    rcar_pin    tmp;

    if (settings == NULL)
        return;

    while(settings->reg != 0)
    {
        tmp.reg = settings->reg;
        tmp.bits = settings->bits;
        tmp.val = settings->val & settings->bits;

        rcar_pfc_config(tmp.reg, (in32(RCAR_GEN3_PFC_BASE + tmp.reg) & ~(tmp.bits)) | tmp.val);
        PFC_DEBUG_PRINT("  module_settings reg=0x%x val=0x%x\n", RCAR_GEN3_PFC_BASE + tmp.reg, in32(RCAR_GEN3_PFC_BASE + tmp.reg));
        settings++;
    }
}

static void module_setup(rcar_config * config)
{
    if (!config)
        return;

    module_pwr(config->pwr);
    module_clk(config->mod, config->cpg);
    module_pinmux(config->pinmux);
    module_settings(config->settings);
    if(config->blk_config)
        config->blk_config(config->mod);
}

void config_usbh(rcar_module *module)
{
    uint32_t base;

    PFC_DEBUG_PRINT("  config_usbh\n");
    /* setup usb channel if clk is enabled */
    while(module->clk.reg != 0) {
        if ((DISABLE == module->clk.val)||(!module->base)) {
            module++;
            continue;
        }

        base = module->base;
        PFC_DEBUG_PRINT("    init_usb: base=0x%x\n", base);
        usec_delay(5);
        out32(base + RCAR_GEN3_USB_AHB_INT_ENABLE, in32(base + RCAR_GEN3_USB_AHB_INT_ENABLE) | RCAR_GEN3_USB_AHB_USBH_INTBEN | RCAR_GEN3_USB_AHB_USBH_INTAEN);

        out32(base + RCAR_GEN3_USB_CORE_SPD_RSM_TIMSET, 0x014e029b);
        out32(base + RCAR_GEN3_USB_CORE_OC_TIMSET, 0x000209ab);

        /* Clock & Reset */
        out32(base + RCAR_GEN3_USB_AHB_USBCTR, in32(base + RCAR_GEN3_USB_AHB_USBCTR) & ~RCAR_GEN3_USB_AHB_PLL_RST);

        module++;
    }
}

void config_usbd(rcar_module *module)
{
    uint32_t base;

    PFC_DEBUG_PRINT("  config_usbd\n");
    while(module->clk.reg != 0) {
        if ((DISABLE == module->clk.val)||(!module->base)) {
            module++;
            continue;
        }

        base = module->base;
        /* Choice USB0SEL */
        out32(base + RCAR_GEN3_HSUSB_UGCTRL2, in32(base + RCAR_GEN3_HSUSB_UGCTRL2) & ~RCAR_GEN3_HSUSB_USB0SEL);
        out32(base + RCAR_GEN3_HSUSB_UGCTRL2, in32(base + RCAR_GEN3_HSUSB_UGCTRL2) | RCAR_GEN3_HSUSB_USB0SEL_EHCI);

        /* low power status */
        out16(base + RCAR_GEN3_HSUSB_LPSTS, in16(base + RCAR_GEN3_HSUSB_LPSTS) & ~RCAR_GEN3_HSUSB_SUSPM);
        out16(base + RCAR_GEN3_HSUSB_LPSTS, in16(base + RCAR_GEN3_HSUSB_LPSTS) | RCAR_GEN3_HSUSB_SUSPM_NORMAL);

        module++;
    }
}

void config_usb3h(rcar_module *module)
{
    int rc;
    uint32_t base;

    PFC_DEBUG_PRINT("  config_usb3\n");
    /* Wait some time for usb3 module to be ready */
    usec_delay(40);

    /* setup usb channel if clk is enabled */
    while(module->clk.reg != 0) {
        if ((DISABLE == module->clk.val)||(!module->base)) {
            module++;
            continue;
        }

        base = module->base;
        PFC_DEBUG_PRINT("    init_usb3: base=0x%x\n", base);
        rc = rcar_usb3_dl_fw(base);
        if ( rc == EOK )
        {
            usec_delay(50);
            /* Interrupt Enable */
            out32(base + RCAR_GEN3_USB3_INT_ENA, in32(base + RCAR_GEN3_USB3_INT_ENA) | RCAR_GEN3_USB3_INT_ENA_VAL);
            /* LCLK Select */
            out32(base + RCAR_GEN3_USB3_LCLK, RCAR_GEN3_USB3_PTPWER_CTRL | RCAR_GEN3_USB3_RENESAS_PRIV | RCAR_GEN3_USB3_BC_MODE);
            /* USB3.0 Configuration */
            out32(base + RCAR_GEN3_USB3_CONF1, RCAR_GEN3_USB3_CONF1_VAL);
            out32(base + RCAR_GEN3_USB3_CONF2, RCAR_GEN3_USB3_CONF2_VAL);
            out32(base + RCAR_GEN3_USB3_CONF3, RCAR_GEN3_USB3_CONF3_VAL);
            /* USB3.0 Polarity */
            out32(base + RCAR_GEN3_USB3_RX_POL, RCAR_GEN3_USB3_RX_POL_VAL);
            out32(base + RCAR_GEN3_USB3_TX_POL, RCAR_GEN3_USB3_TX_POL_VAL);
        }

        module++;
    }
}

void config_dmac(rcar_module *module)
{
    PFC_DEBUG_PRINT("  config_dmac\n");

    /* setup dmac channel if clk is enabled */
    while(module->clk.reg != 0) {
        if ((DISABLE == module->clk.val)||(!module->base)) {
            module++;
            continue;
        }

        out16(module->base + RCAR_GEN3_SYSDMAC_DMAOR, 1);           // Enable DMA master
        out16(module->base + RCAR_GEN3_SYSDMAC_DMACHCLR, 0xFFFF);   // Clear all channels

        module++;
    }
}

void config_impx5(rcar_module *module)
{
    uint32_t tmp;
    int i;

    /* A2IR3-0 and A2SC1-0 power up */
    /* 1. Enable power interrupt */
    for(i = 23; i <= 31 ; i++) {
        tmp = in32(RCAR_GEN3_SYSC_BASE + RCAR_GEN3_SYSC_IER);
        out32 (RCAR_GEN3_SYSC_BASE + RCAR_GEN3_SYSC_IER, tmp | (1 << i));
    }

    for(i = 0; i < 6; i++) {
    again:
        /* 2. Check to confirm power resum is ready to start  */
        while((in32(RCAR_GEN3_SYSC_BASE + RCAR_GEN3_SYSC_SR) & (1 << 1)) == 0);

        /* 3. Set SYSCEXTMASK and power resume/shutdown control register*/
        out32(RCAR_GEN3_SYSC_BASE + RCAR_GEN3_SYSC_EXTMASK_V3M, 1);

        tmp = in32(RCAR_GEN3_SYSC_BASE + PWRONCR(11));
        out32 (RCAR_GEN3_SYSC_BASE + PWRONCR(11), tmp | (1 << i));

        /* 4. Check whether the request has been accepted */
         if ((in32(RCAR_GEN3_SYSC_BASE + PWRER(11)) & 0x0000003F) != 0)
             goto again;

        /* 5. Wait for the completion of power sequence  */
        while((in32(RCAR_GEN3_SYSC_BASE + RCAR_GEN3_SYSC_SR) & (1 << 1)) == 0);

        /* Finally. Clear SYSCEXTMASK */
         out32(RCAR_GEN3_SYSC_BASE + RCAR_GEN3_SYSC_EXTMASK_V3M, 0);
    }
}

void config_audio_d3(rcar_module *module)
{
    uint32_t tmp;
    PFC_DEBUG_PRINT("  config_audio\n");

    /* Set clock for ZA2: 1GHz -> 200 MHz */
    rcar_cpg_config(RCAR_GEN3_CPG_ZA2CKCR, 0x0C);

    /* ZA2clock (200 MHz)  --|     BRGA    |--+-->???
     *
     * Clock Select Register(SSICKR)
     *  [31]   =1'B1           : BRGB output clock
     *  [30:23]=8'B01000110    : Reserved (Fixed value)
     *  [22:20]=3'B101         : BRGA input clock = Fixed at 0
     *  [18:16]=3'B001         : BRGB input clock = lckB (22.5792MHz)
     */
    out32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_SSICKR, 0xA3510000);

    /* BRGB Baud Rate Setting Register (BRRB)
     *  [9:8]=2'B00 : ACLK_B
     *  [7:0]=8'Hxx : division ratio
     *  div ratio = 1/(2(N+1))=1/(2(0+1))= 22.5792MHz/2 = 11.2896MHz */
    out32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_BRRA, 0);
    out32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_BRRB, 0);

    /* Clock Select Register1 (SSICKR1)
        [27:24]=4'b0000 : AUDIO_CLKOUT3 = BRGA/BRGB select
        [19:16]=4'b0000 : AUDIO_CLKOUT2 = BRGA/BRGB select
        [11:08]=4'b0000 : AUDIO_CLKOUT1 = BRGA/BRGB select
        [03:00]=4'b0000 : AUDIO_CLKOUT  = BRGA/BRGB select */
    out32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_SSICKR1, 0x00000000);

    /* AUDIO_CLK_B(22.5792MHz) ---->SSIx
       AUDIO_CLK select data
       No divided, AUDIO_CLK_B output clock */

    /* SSI3 select AUDIO_CLK_B clock(22.5792MHz) / 32 */
    tmp  = in32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_CKSEL0) & ~(0xFF << 24);
    out32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_CKSEL0, tmp | (2 << 24));

    /* SSI4 select AUDIO_CLK_B clock(22.5792MHz) / 32 */
    tmp  = in32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_CKSEL1) & ~(0xFF << 0);
    out32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_CKSEL1, tmp | (2 << 0));

    /* TIM_EN */
    out32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_TIM_EN,
        in32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_TIM_EN) | 0x3f);

    /* SCU SRC0->9,CMD0->1,DVC0->1 */
    out32(RCAR_GEN3_CPG_BASE + RCAR_GEN3_CPG_SMSTPCR10,
        in32(RCAR_GEN3_CPG_BASE + RCAR_GEN3_CPG_SMSTPCR10) & ~((3 << 25) | (0x1F << 17)));

    /* SCU timing */
    tmp  = in32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_SRCIN_TIMSEL2);
    tmp &= ~((0x1F << 24) | (0x1F << 16));
    tmp |= (9 << 24); //SRC5 in timing: Use SW3
    out32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_SRCIN_TIMSEL2,  tmp);

    tmp  = in32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_SRCIN_TIMSEL3);
    tmp &= ~((0x1F << 8) | (0x1F << 0));
    tmp |= (9 << 8); //SRC6 in timing: Use SW3
    out32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_SRCIN_TIMSEL3,  tmp);

    tmp  = in32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_SRCOUT_TIMSEL2);
    tmp &= ~((0x1F << 24) | (0x1F << 16));
    tmp |= (9 << 24); //SRC5 in timing: Use SW3
    out32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_SRCOUT_TIMSEL2, tmp);

    tmp  = in32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_SRCOUT_TIMSEL3);
    tmp &= ~((0x1F << 8) | (0x1F << 0));
    tmp |= (9 << 8); //SRC6 in timing: Use SW3
    out32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_SRCOUT_TIMSEL3, tmp);

    tmp  = in32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_CMDOUT_TIMSEL);
    tmp &= ~((0x1F << 24) | (0x1F << 16) | (0x1F << 8) | (0x1F << 0));
    tmp |= (9 << 24) |  // CMD1 out timing: Use SW3
           (9 << 8);    // CMD0 out timing: Use SW3
    out32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_CMDOUT_TIMSEL,  tmp);

    /* SSI mode register 0 */
    out32(RCAR_GEN3_SSIU_BASE + RCAR_GEN3_SSIU_MODE0, 0);
    /* SSI mode register 1 */
    out32(RCAR_GEN3_SSIU_BASE + RCAR_GEN3_SSIU_MODE1, 0);
    /* SSI mode register 2 */
    out32(RCAR_GEN3_SSIU_BASE + RCAR_GEN3_SSIU_MODE2, 0);
    /* SSI control register */
    out32(RCAR_GEN3_SSIU_BASE + RCAR_GEN3_SSIU_CONTROL, 0);
}

void config_audio(rcar_module *module)
{
    PFC_DEBUG_PRINT("  config_audio\n");
    /* S0D4clock (200 MHz)  --|     BRGA    |--+-->???
     *
     * Clock Select Register(SSICKR)
     *  [31]   =0'B!           : BRGA output clock
     *  [30:23]=8'B01000110    : Reserved (Fixed value)
     *  [22:20]=3'B010         : BRGA input clock = S0D4(200MHz)
     *  [18:16]=3'B101         : BRGB input clock = Fixed at 0
     */
    out32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_SSICKR, 0x23250000);

    /* BRGA Baud Rate Setting Register (BRRA)
     *  [9:8]=2'B00 : ACLK_A
     *  [7:0]=8'Hxx : division ratio
     *  div ratio = 1/(2(N+1))=1/(2(1+1))= 200MHz/4 = 50MHz */
    out32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_BRRA, 0);

    /* Clock Select Register1 (SSICKR1)
        [27:24]=4'b0000 : AUDIO_CLKOUT3 = BRGA/BRGB select
        [19:16]=4'b0000 : AUDIO_CLKOUT2 = BRGA/BRGB select
        [11:08]=4'b0000 : AUDIO_CLKOUT1 = BRGA/BRGB select
        [03:00]=4'b0000 : AUDIO_CLKOUT  = BRGA/BRGB select */
    out32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_SSICKR1, 0x00000000);

    /* AUDIO_CLK_A(22.5792MHz) ---->SSIx
       AUDIO_CLK select data
       No divided, AUDIO_CLK_A output clock */

    /* SSI0,1, 2, 3 select AUDIO_CLK_A clock(22.5792MHz) / 32 */
    out32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_CKSEL0, 0x01010101);

    /* SSI4,5, 6, 7 select AUDIO_CLK_A clock(22.5792MHz) / 32 */
    out32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_CKSEL1, 0x01010101);

    /* SSI9 select AUDIO_CLK_A clock(22.5792MHz) / 32 */
    out32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_CKSEL2, 0x00000100);

    /* TIM_EN */
    out32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_TIM_EN, 0x3f);

    /* SCU SRC0->9,CMD0->1,DVC0->1 */
    out32(RCAR_GEN3_CPG_BASE + RCAR_GEN3_CPG_SMSTPCR10, in32(RCAR_GEN3_CPG_BASE + RCAR_GEN3_CPG_SMSTPCR10) & ~((0x3FF << 22) | (7 << 17)));

    /* SCU timing */
    out32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_SRCIN_TIMSEL0,   (6 << 24) | // SRC1 in timing: Use SW0
                                                    (6 << 8));  // SRC0 in timing: Use SW0

    out32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_SRCIN_TIMSEL1,   (6 << 24) | // SRC3 in timing: Use SW0
                                                    (6 << 8));  // SRC2 in timing: Use SW0

    out32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_SRCIN_TIMSEL2,   (6 << 24) | // SRC5 in timing: Use SW0
                                                    (6 << 8));  // SRC4 in timing: Use SW0

    out32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_SRCIN_TIMSEL3,   (6 << 24) | // SRC7 in timing: Use SW0
                                                    (6 << 8));  // SRC6 in timing: Use SW0

    out32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_SRCIN_TIMSEL4,   (6 << 24) | // SRC9 in timing: Use SW0
                                                    (6 << 8));  // SRC8 in timing: Use SW0

    out32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_SRCOUT_TIMSEL0,  (6 << 24) | // SRC1 out timing: Use SW0
                                                    (6 << 8));  // SRC0 out timing: Use SW0

    out32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_SRCOUT_TIMSEL1,  (6 << 24) | // SRC3 out timing: Use SW0
                                                    (6 << 8));  // SRC2 out timing: Use SW0

    out32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_SRCOUT_TIMSEL2,  (6 << 24) | // SRC5 out timing: Use SW0
                                                    (6 << 8));  // SRC4 out timing: Use SW0

    out32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_SRCOUT_TIMSEL3,  (6 << 24) | // SRC7 out timing: Use SW0
                                                    (6 << 8));  // SRC6 out timing: Use SW0

    out32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_SRCOUT_TIMSEL4,  (6 << 24) | // SRC9 out timing: Use SW0
                                                    (6 << 8));  // SRC8 out timing: Use SW0

    out32(RCAR_GEN3_ADG_BASE + RCAR_GEN3_ADG_CMDOUT_TIMSEL,   (6 << 24) | // CMD1 out timing: Use SW0
                                                    (6 << 8));  // CMD0 out timing: Use SW0

    /* SSI mode register 0 */
    out32(RCAR_GEN3_SSIU_BASE + RCAR_GEN3_SSIU_MODE0, 0);
    /* SSI mode register 1 */
    out32(RCAR_GEN3_SSIU_BASE + RCAR_GEN3_SSIU_MODE1, 0);
    /* SSI mode register 2 */
    out32(RCAR_GEN3_SSIU_BASE + RCAR_GEN3_SSIU_MODE2, 0);
    /* SSI control register */
    out32(RCAR_GEN3_SSIU_BASE + RCAR_GEN3_SSIU_CONTROL, 0);
}

void init_board(void)
{
    rcar_config *config = NULL;
    size_t num_devs = 0;
    int i;

    config = rcar_get_board_config(&num_devs);

    if(config) {
        for(i = 0; i < num_devs; i++) {
            PFC_DEBUG_PRINT("init module %d\n", i);
            module_setup(&config[i]);
        }
    } else {
        crash("No R-Car PFC config found!!\n");
    }
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/boards/rcar_gen3/init_board.c $ $Rev: 850944 $")
#endif
