/*
 * $QNXLicenseC:
 * Copyright 2017, QNX Software Systems.
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

#include <startup.h>
#include <hwinfo_private.h>
#include <drvr/hwinfo.h>
#include <hw/hwinfo_rcar.h>

#include <aarch64/r-car-gen3.h>
#include <rcar_board.h>

void init_hwinfo()
{
    {
        unsigned i, hwi_off;

        hwiattr_uart_t uart_attr = HWIATTR_UART_T_INITIALIZER;
        /* All the UARTs operate from a fixed functional clock of 14.7456 MHz only */
        struct hwi_inputclk uart_clk = {.clk = 14745600, .div = 16};

        /* each UART has an interrupt */
        HWIATTR_UART_SET_NUM_IRQ(&uart_attr, 1);
        HWIATTR_UART_SET_NUM_CLK(&uart_attr, 1);

        dev_info_t dev[] = {
            {.base=RCAR_GEN3_SCIF0_BASE, .irq=RCAR_GEN3_INTCSYS_SCIF0},
            {.base=RCAR_GEN3_SCIF1_BASE, .irq=RCAR_GEN3_INTCSYS_SCIF1},
            {.base=RCAR_GEN3_SCIF2_BASE, .irq=RCAR_GEN3_INTCSYS_SCIF2},
            {.base=RCAR_GEN3_SCIF3_BASE, .irq=RCAR_GEN3_INTCSYS_SCIF3},
            {.base=RCAR_GEN3_SCIF4_BASE, .irq=RCAR_GEN3_INTCSYS_SCIF4},
            {.base=RCAR_GEN3_SCIF5_BASE, .irq=RCAR_GEN3_INTCSYS_SCIF5},
        };

        for(i = 0; i < NUM_ELTS(dev); i++) {
            HWIATTR_UART_SET_LOCATION(&uart_attr, dev[i].base,  RCAR_GEN3_SCIF_SIZE, 0, hwi_find_as(dev[i].base , 1));
            hwi_off = hwidev_add_uart(RCAR_HWI_UART, &uart_attr, HWI_NULL_OFF);
            ASSERT(hwi_find_unit(hwi_off) == i);
            hwitag_set_inputclk(hwi_off, 0, &uart_clk);
            hwitag_set_ivec(hwi_off, 0, dev[i].irq);
        }
    }

    {
        unsigned i, hwi_off;

        hwiattr_uart_t uart_attr = HWIATTR_UART_T_INITIALIZER;
        /* All the UARTs operate from a fixed functional clock of 14.7456 MHz only */
        struct hwi_inputclk uart_clk = {.clk = 14745600, .div = 16};

        /* each UART has an interrupt */
        HWIATTR_UART_SET_NUM_IRQ(&uart_attr, 1);
        HWIATTR_UART_SET_NUM_CLK(&uart_attr, 1);

        dev_info_t dev[] = {
            {.base=RCAR_GEN3_HSCIF0_BASE, .irq=RCAR_GEN3_INTCSYS_HSCIF0},
            {.base=RCAR_GEN3_HSCIF1_BASE, .irq=RCAR_GEN3_INTCSYS_HSCIF1},
            {.base=RCAR_GEN3_HSCIF2_BASE, .irq=RCAR_GEN3_INTCSYS_HSCIF2},
            {.base=RCAR_GEN3_HSCIF3_BASE, .irq=RCAR_GEN3_INTCSYS_HSCIF3},
            {.base=RCAR_GEN3_HSCIF4_BASE, .irq=RCAR_GEN3_INTCSYS_HSCIF4},
        };

        for(i = 0; i < NUM_ELTS(dev); i++) {
            HWIATTR_UART_SET_LOCATION(&uart_attr, dev[i].base,  RCAR_GEN3_SCIF_SIZE, 0, hwi_find_as(dev[i].base , 1));
            hwi_off = hwidev_add_uart(RCAR_HWI_HSUART, &uart_attr, HWI_NULL_OFF);
            ASSERT(hwi_find_unit(hwi_off) == i);
            hwitag_set_inputclk(hwi_off, 0, &uart_clk);
            hwitag_set_ivec(hwi_off, 0, dev[i].irq);
        }
    }

    {
        unsigned i, hwi_off;

        hwiattr_sdio_t sdio_attr = HWIATTR_SDIO_T_INITIALIZER;
        /* RCAR_GEN3_CPG_SD0CKCR = 0x1 SDnH = 800 MHz SDn = 200 Mhz*/
        struct hwi_inputclk sdio_clk = {.clk = 200000000, .div = 1};

        HWIATTR_SDIO_SET_NUM_IRQ(&sdio_attr, 1);
        HWIATTR_SDIO_SET_NUM_CLK(&sdio_attr, 1);
        HWIATTR_SDIO_SET_DLL(&sdio_attr, "rcar_gen3");

        sd_info_t sd[] = {
            {.pwr_base=RCAR_GEN3_GPIO5_BASE, .pwr_offset=RCAR_GEN3_GPIO_OUTDT, .pwr_vdd=2,  .pwr_if=1,  .pfc_base=RCAR_GEN3_PFC_BASE, .pfc_offset=RCAR_GEN3_PFC_POCCTRL0, .pfc_mask=0x3F,  .pfc_shift=0},
            { 0 },
            { 0 },
            {.pwr_base=RCAR_GEN3_GPIO3_BASE, .pwr_offset=RCAR_GEN3_GPIO_OUTDT, .pwr_vdd=15, .pwr_if=14, .pfc_base=RCAR_GEN3_PFC_BASE, .pfc_offset=RCAR_GEN3_PFC_POCCTRL0, .pfc_mask=0x7FF, .pfc_shift=19},
        };

        dev_info_t dev[] = {
            {.base=RCAR_GEN3_SDHI0_BASE, .irq=RCAR_GEN3_INTCSYS_SDHI0},
            {.base=RCAR_GEN3_SDHI1_BASE, .irq=RCAR_GEN3_INTCSYS_SDHI1},
            {.base=RCAR_GEN3_SDHI2_BASE, .irq=RCAR_GEN3_INTCSYS_SDHI2},
            {.base=RCAR_GEN3_SDHI3_BASE, .irq=RCAR_GEN3_INTCSYS_SDHI3},
        };

        for(i = 0; i < NUM_ELTS(dev); i++) {
            HWIATTR_SDIO_SET_LOCATION(&sdio_attr, dev[i].base, RCAR_GEN3_SDHI_SIZE, 0, hwi_find_as(dev[i].base, 1));
            hwi_off = hwibus_add_sdio(HWI_NULL_OFF, &sdio_attr);
            ASSERT(hwi_find_unit(hwi_off) == i);
            hwitag_set_inputclk(hwi_off, 0, &sdio_clk);
            hwitag_set_avail_ivec(hwi_find_bus(HWI_ITEM_BUS_SDIO, i), -1, dev[i].irq);

            hwi_off = hwidev_add(RCAR_HWI_SDHI_PWR, hwi_devclass_NONE, HWI_NULL_OFF);
            if(sd[i].pwr_base != 0) {
                hwitag_add_location(hwi_off, sd[i].pwr_base, RCAR_GEN3_GPIO_SIZE, 0, hwi_find_as(sd[i].pwr_base, 1));
                hwitag_add_regname(hwi_off, RCAR_HWI_SDHI_PWR_REG, sd[i].pwr_offset);
                hwitag_add_regname(hwi_off, RCAR_HWI_SDHI_PWR_VDD, sd[i].pwr_vdd);
                hwitag_add_regname(hwi_off, RCAR_HWI_SDHI_PWR_IF, sd[i].pwr_if);
            }

            hwi_off = hwidev_add(RCAR_HWI_SDHI_PFC, hwi_devclass_NONE, HWI_NULL_OFF);
            if(sd[i].pfc_base != 0) {
                hwitag_add_location(hwi_off, sd[i].pfc_base, RCAR_GEN3_PFC_SIZE, 0, hwi_find_as(sd[i].pfc_base, 1));
                hwitag_add_regname(hwi_off, RCAR_HWI_SDHI_PFC_REG, sd[i].pfc_offset);
                hwitag_add_regname(hwi_off, RCAR_HWI_SDHI_PFC_PMMR, RCAR_GEN3_PFC_PMMR);
                hwitag_add_regname(hwi_off, RCAR_HWI_SDHI_PFC_MASK, sd[i].pfc_mask);
                hwitag_add_regname(hwi_off, RCAR_HWI_SDHI_PFC_SHIFT, sd[i].pfc_shift);
            }
        }
    }

    {
        unsigned hwi_off;
        hwiattr_spi_t attr = HWIATTR_SPI_T_INITIALIZER;
        struct hwi_inputclk rpc_clk = {.clk = 320000000, .div = 1};
        HWIATTR_SPI_SET_NUM_CLK(&attr, 1);

        HWIATTR_SPI_SET_LOCATION(&attr, RCAR_GEN3_RPC_BASE, RCAR_GEN3_RPC_SIZE, 0, hwi_find_as(RCAR_GEN3_RPC_BASE, 1));
        hwi_off = hwibus_add_spi(HWI_NULL_OFF, &attr);
        ASSERT(hwi_find_unit(hwi_off) == 0);
        hwitag_set_inputclk(hwi_off, 0, &rpc_clk);

        hwitag_add_regname(hwi_off, RCAR_HWI_RPC_WBUF, RCAR_GEN3_RPC_BUFFER);
        hwitag_add_regname(hwi_off, RCAR_HWI_RPC_WBUF_SIZE, RCAR_GEN3_RPC_BUFFER_SIZE);
    }

    {
        unsigned i, hwi_off;

        hwiattr_i2c_t i2c_attr = HWIATTR_I2C_T_INITIALIZER;
        /* All I2Cs operate on 133.33 MHz only */
        struct hwi_inputclk i2c_clk = {.clk = 133330000, .div = 1};

        /* each I2C has an interrupt */
        HWIATTR_I2C_SET_NUM_IRQ(&i2c_attr, 1);
        HWIATTR_I2C_SET_NUM_CLK(&i2c_attr, 1);

        dev_info_t dev[] = {
            {.base=RCAR_GEN3_I2C0_BASE, .irq=RCAR_GEN3_INTCSYS_I2C0},
            {.base=RCAR_GEN3_I2C1_BASE, .irq=RCAR_GEN3_INTCSYS_I2C1},
            {.base=RCAR_GEN3_I2C2_BASE, .irq=RCAR_GEN3_INTCSYS_I2C2},
            {.base=RCAR_GEN3_I2C3_BASE, .irq=RCAR_GEN3_INTCSYS_I2C3},
            {.base=RCAR_GEN3_I2C4_BASE, .irq=RCAR_GEN3_INTCSYS_I2C4},
            {.base=RCAR_GEN3_I2C5_BASE, .irq=RCAR_GEN3_INTCSYS_I2C5},
            {.base=RCAR_GEN3_I2C6_BASE, .irq=RCAR_GEN3_INTCSYS_I2C6},
        };

        for(i = 0; i < NUM_ELTS(dev); i++) {
            HWIATTR_I2C_SET_LOCATION(&i2c_attr, dev[i].base, RCAR_GEN3_I2C_SIZE, 0, hwi_find_as(dev[i].base, 1));
            hwi_off = hwibus_add_i2c(HWI_NULL_OFF, &i2c_attr);
            ASSERT(hwi_find_unit(hwi_off) == i);
            hwitag_set_inputclk(hwi_off, 0, &i2c_clk);
            hwitag_set_ivec(hwi_off, 0, dev[i].irq);
        }
    }

    {
        unsigned hwi_off = hwibus_add(RCAR_HWI_IIC, HWI_NULL_OFF);

        hwiattr_i2c_t iic_attr = HWIATTR_I2C_T_INITIALIZER;
        struct hwi_inputclk iic_clk = {.clk = RCAR_GEN3_EXT_CLK, .div = 2};

        HWIATTR_I2C_SET_NUM_IRQ(&iic_attr, 1);
        HWIATTR_I2C_SET_NUM_CLK(&iic_attr, 1);

        HWIATTR_I2C_SET_LOCATION(&iic_attr, RCAR_GEN3_IIC_BASE, RCAR_GEN3_IIC_SIZE, 0, hwi_find_as(RCAR_GEN3_IIC_BASE, 1));
        hwitag_add_common(hwi_off, &iic_attr.common);
        ASSERT(hwi_off != HWI_NULL_OFF);
        hwitag_add_inputclk(hwi_off, iic_clk.clk, iic_clk.div);
        hwitag_set_ivec(hwi_off, 0, RCAR_GEN3_INTCSYS_IIC);
    }

    {
        unsigned i, irq, hwi_off;
        hwiattr_dma_t attr = HWIATTR_DMA_T_INITIALIZER;

        dev_info_t dev[] = {
            {.base=RCAR_GEN3_SYSDMAC0_BASE},
            {.base=RCAR_GEN3_SYSDMAC1_BASE},
            {.base=RCAR_GEN3_SYSDMAC2_BASE},
        };

        uint32_t irq0[] = {
            RCAR_GEN3_INTCSYS_SYSDMAC0_0,  RCAR_GEN3_INTCSYS_SYSDMAC0_1,  RCAR_GEN3_INTCSYS_SYSDMAC0_2,  RCAR_GEN3_INTCSYS_SYSDMAC0_3,
            RCAR_GEN3_INTCSYS_SYSDMAC0_4,  RCAR_GEN3_INTCSYS_SYSDMAC0_5,  RCAR_GEN3_INTCSYS_SYSDMAC0_6,  RCAR_GEN3_INTCSYS_SYSDMAC0_7,
            RCAR_GEN3_INTCSYS_SYSDMAC0_8,  RCAR_GEN3_INTCSYS_SYSDMAC0_9,  RCAR_GEN3_INTCSYS_SYSDMAC0_10, RCAR_GEN3_INTCSYS_SYSDMAC0_11,
            RCAR_GEN3_INTCSYS_SYSDMAC0_12, RCAR_GEN3_INTCSYS_SYSDMAC0_13, RCAR_GEN3_INTCSYS_SYSDMAC0_14, RCAR_GEN3_INTCSYS_SYSDMAC0_15
        };
        uint32_t irq1[] = {
            RCAR_GEN3_INTCSYS_SYSDMAC1_0,  RCAR_GEN3_INTCSYS_SYSDMAC1_1,  RCAR_GEN3_INTCSYS_SYSDMAC1_2,  RCAR_GEN3_INTCSYS_SYSDMAC1_3,
            RCAR_GEN3_INTCSYS_SYSDMAC1_4,  RCAR_GEN3_INTCSYS_SYSDMAC1_5,  RCAR_GEN3_INTCSYS_SYSDMAC1_6,  RCAR_GEN3_INTCSYS_SYSDMAC1_7,
            RCAR_GEN3_INTCSYS_SYSDMAC1_8,  RCAR_GEN3_INTCSYS_SYSDMAC1_9,  RCAR_GEN3_INTCSYS_SYSDMAC1_10, RCAR_GEN3_INTCSYS_SYSDMAC1_11,
            RCAR_GEN3_INTCSYS_SYSDMAC1_12, RCAR_GEN3_INTCSYS_SYSDMAC1_13, RCAR_GEN3_INTCSYS_SYSDMAC1_14, RCAR_GEN3_INTCSYS_SYSDMAC1_15
        };
        uint32_t irq2[] = {
            RCAR_GEN3_INTCSYS_SYSDMAC2_0,  RCAR_GEN3_INTCSYS_SYSDMAC2_1,  RCAR_GEN3_INTCSYS_SYSDMAC2_2,  RCAR_GEN3_INTCSYS_SYSDMAC2_3,
            RCAR_GEN3_INTCSYS_SYSDMAC2_4,  RCAR_GEN3_INTCSYS_SYSDMAC2_5,  RCAR_GEN3_INTCSYS_SYSDMAC2_6,  RCAR_GEN3_INTCSYS_SYSDMAC2_7,
            RCAR_GEN3_INTCSYS_SYSDMAC2_8,  RCAR_GEN3_INTCSYS_SYSDMAC2_9,  RCAR_GEN3_INTCSYS_SYSDMAC2_10, RCAR_GEN3_INTCSYS_SYSDMAC2_11,
            RCAR_GEN3_INTCSYS_SYSDMAC2_12, RCAR_GEN3_INTCSYS_SYSDMAC2_13, RCAR_GEN3_INTCSYS_SYSDMAC2_14, RCAR_GEN3_INTCSYS_SYSDMAC2_15
        };

        uint32_t *irqs[] = { irq0, irq1, irq2 };

        HWIATTR_DMA_SET_NUM_IRQ(&attr, NUM_ELTS(irq0));

        for(i = 0; i < NUM_ELTS(dev); i++) {
            HWIATTR_USB_SET_LOCATION(&attr, dev[i].base, RCAR_GEN3_SYSDMAC_SIZE, 0, hwi_find_as(dev[i].base, 1));
            hwi_off = hwidev_add_dma(RCAR_HWI_SYSDMAC, &attr, HWI_NULL_OFF);
            ASSERT(hwi_find_unit(hwi_off) == i);
            hwitag_add_regname(hwi_off, RCAR_HWI_DMA_REG, RCAR_GEN3_SYSDMAC_REGS);
            hwitag_add_regname(hwi_off, RCAR_HWI_DMA_SIZE, RCAR_GEN3_SYSDMAC_REGSIZE);
            hwitag_add_regname(hwi_off, RCAR_HWI_DMA_DESC, RCAR_GEN3_SYSDMAC_DESC);
            for(irq = 0; irq < NUM_ELTS(irq0); irq++) {
                hwitag_set_avail_ivec(hwi_off, irq, irqs[i][irq]);
            }
        }
    }

    {
        unsigned i, irq, hwi_off;
        hwiattr_dma_t attr = HWIATTR_DMA_T_INITIALIZER;

        dev_info_t dev[] = {
            {.base=RCAR_GEN3_AUDIODMAC0_BASE},
            {.base=RCAR_GEN3_AUDIODMAC1_BASE},
        };

        uint32_t irq0[] = {
            RCAR_GEN3_INTCSYS_AUDIODMAC0_0,  RCAR_GEN3_INTCSYS_AUDIODMAC0_1,  RCAR_GEN3_INTCSYS_AUDIODMAC0_2,  RCAR_GEN3_INTCSYS_AUDIODMAC0_3,
            RCAR_GEN3_INTCSYS_AUDIODMAC0_4,  RCAR_GEN3_INTCSYS_AUDIODMAC0_5,  RCAR_GEN3_INTCSYS_AUDIODMAC0_6,  RCAR_GEN3_INTCSYS_AUDIODMAC0_7,
            RCAR_GEN3_INTCSYS_AUDIODMAC0_8,  RCAR_GEN3_INTCSYS_AUDIODMAC0_9,  RCAR_GEN3_INTCSYS_AUDIODMAC0_10, RCAR_GEN3_INTCSYS_AUDIODMAC0_11,
            RCAR_GEN3_INTCSYS_AUDIODMAC0_12, RCAR_GEN3_INTCSYS_AUDIODMAC0_13, RCAR_GEN3_INTCSYS_AUDIODMAC0_14, RCAR_GEN3_INTCSYS_AUDIODMAC0_15
        };
        uint32_t irq1[] = {
            RCAR_GEN3_INTCSYS_AUDIODMAC1_0,  RCAR_GEN3_INTCSYS_AUDIODMAC1_1,  RCAR_GEN3_INTCSYS_AUDIODMAC1_2,  RCAR_GEN3_INTCSYS_AUDIODMAC1_3,
            RCAR_GEN3_INTCSYS_AUDIODMAC1_4,  RCAR_GEN3_INTCSYS_AUDIODMAC1_5,  RCAR_GEN3_INTCSYS_AUDIODMAC1_6,  RCAR_GEN3_INTCSYS_AUDIODMAC1_7,
            RCAR_GEN3_INTCSYS_AUDIODMAC1_8,  RCAR_GEN3_INTCSYS_AUDIODMAC1_9,  RCAR_GEN3_INTCSYS_AUDIODMAC1_10, RCAR_GEN3_INTCSYS_AUDIODMAC1_11,
            RCAR_GEN3_INTCSYS_AUDIODMAC1_12, RCAR_GEN3_INTCSYS_AUDIODMAC1_13, RCAR_GEN3_INTCSYS_AUDIODMAC1_14, RCAR_GEN3_INTCSYS_AUDIODMAC1_15
        };

        uint32_t *irqs[] = { irq0, irq1 };

        HWIATTR_DMA_SET_NUM_IRQ(&attr, NUM_ELTS(irq0));

        for(i = 0; i < NUM_ELTS(dev); i++) {
            HWIATTR_USB_SET_LOCATION(&attr, dev[i].base, RCAR_GEN3_SYSDMAC_SIZE, 0, hwi_find_as(dev[i].base, 1));
            hwi_off = hwidev_add_dma(RCAR_HWI_AUDIODMAC, &attr, HWI_NULL_OFF);
            ASSERT(hwi_find_unit(hwi_off) == i);
            hwitag_add_regname(hwi_off, RCAR_HWI_DMA_REG, RCAR_GEN3_SYSDMAC_REGS);
            hwitag_add_regname(hwi_off, RCAR_HWI_DMA_SIZE, RCAR_GEN3_SYSDMAC_REGSIZE);
            hwitag_add_regname(hwi_off, RCAR_HWI_DMA_DESC, RCAR_GEN3_SYSDMAC_DESC);
            for(irq = 0; irq < NUM_ELTS(irq0); irq++) {
                hwitag_set_avail_ivec(hwi_off, irq, irqs[i][irq]);
            }
        }
    }

    {
        unsigned hwi_off;
        hwiattr_dma_t attr = HWIATTR_DMA_T_INITIALIZER;

        HWIATTR_USB_SET_LOCATION(&attr, RCAR_GEN3_AUDIODMACPP_BASE, RCAR_GEN3_AUDIODMACPP_SIZE, 0, hwi_find_as(RCAR_GEN3_AUDIODMACPP_BASE, 1));
        hwi_off = hwidev_add_dma(RCAR_HWI_AUDIOPPDMAC, &attr, HWI_NULL_OFF);
        ASSERT(hwi_find_unit(hwi_off) == 0);

        HWIATTR_USB_SET_LOCATION(&attr, RCAR_GEN3_AUDIODMACPP_EXT_BASE, RCAR_GEN3_AUDIODMACPP_SIZE, 0, hwi_find_as(RCAR_GEN3_AUDIODMACPP_EXT_BASE, 1));
        hwi_off = hwidev_add_dma(RCAR_HWI_AUDIOPPDMAC, &attr, HWI_NULL_OFF);
        ASSERT(hwi_find_unit(hwi_off) == 1);
    }

    {
        unsigned hwi_off = hwidev_add(RCAR_HWI_AUDIO_ADG, hwi_devclass_NONE, HWI_NULL_OFF);
        hwiattr_common_t attr = HWIATTR_COMMON_INITIALIZER;
        HWIATTR_SET_LOCATION(&attr, RCAR_GEN3_ADG_BASE, RCAR_GEN3_ADG_SIZE, 0, hwi_find_as(RCAR_GEN3_ADG_BASE, 1));
        hwitag_add_common(hwi_off, &attr);
        ASSERT(hwi_off != HWI_NULL_OFF);
    }

    {
        unsigned hwi_off = hwidev_add(RCAR_HWI_AUDIO_SSI, hwi_devclass_NONE, HWI_NULL_OFF);
        hwiattr_common_t attr = HWIATTR_COMMON_INITIALIZER;
        HWIATTR_SET_LOCATION(&attr, RCAR_GEN3_SSI_BASE, RCAR_GEN3_SSI_SIZE, 0, hwi_find_as(RCAR_GEN3_SSI_BASE, 1));
        hwitag_add_common(hwi_off, &attr);
        ASSERT(hwi_off != HWI_NULL_OFF);
    }

    {
        unsigned hwi_off = hwidev_add(RCAR_HWI_AUDIO_SSIU, hwi_devclass_NONE, HWI_NULL_OFF);
        hwiattr_common_t attr = HWIATTR_COMMON_INITIALIZER;
        HWIATTR_SET_LOCATION(&attr, RCAR_GEN3_SSIU_BASE, RCAR_GEN3_SSIU_SIZE, 0, hwi_find_as(RCAR_GEN3_SSIU_BASE, 1));
        hwitag_add_common(hwi_off, &attr);
        ASSERT(hwi_off != HWI_NULL_OFF);
    }

    {
        unsigned hwi_off = hwidev_add(RCAR_HWI_AUDIO_SCU, hwi_devclass_NONE, HWI_NULL_OFF);
        hwiattr_common_t attr = HWIATTR_COMMON_INITIALIZER;
        HWIATTR_SET_LOCATION(&attr, RCAR_GEN3_SCU_BASE, RCAR_GEN3_SCU_SIZE, 0, hwi_find_as(RCAR_GEN3_SCU_BASE, 1));
        hwitag_add_common(hwi_off, &attr);
        ASSERT(hwi_off != HWI_NULL_OFF);
    }

    {
        unsigned i, hwi_off;
        hwi_tag *tag;

        hwiattr_common_t attr = HWIATTR_COMMON_INITIALIZER;
        HWIATTR_SET_NUM_IRQ(&attr, 3);

        dev_info_t dev[] = {
            { 0 },
            {.base=RCAR_GEN3_THS_TSC1},
            {.base=RCAR_GEN3_THS_TSC2},
            {.base=RCAR_GEN3_THS_TSC3},
        };

        for(i = 0; i < NUM_ELTS(dev); i++) {
            hwi_off = hwidev_add(RCAR_HWI_THERMAL, hwi_devclass_NONE, HWI_NULL_OFF);
            HWIATTR_SET_LOCATION(&attr, dev[i].base, RCAR_GEN3_THS_SIZE, 0, hwi_find_as(dev[i].base, 1));
            hwitag_add_common(hwi_off, &attr);
            ASSERT(hwi_off != HWI_NULL_OFF);

            if(dev[i].base) {
                hwitag_set_ivec(hwi_off, 0, RCAR_GEN3_INTCSYS_THERMAL0);
                hwitag_set_ivec(hwi_off, 1, RCAR_GEN3_INTCSYS_THERMAL1);
                hwitag_set_ivec(hwi_off, 2, RCAR_GEN3_INTCSYS_THERMAL2);
            }
            tag = hwi_alloc_tag(HWI_TAG_INFO(hwversion));
            tag->hwversion.version = 2;
        }
    }

    {
        unsigned i, hwi_off;

        hwiattr_common_t attr = HWIATTR_COMMON_INITIALIZER;
        HWIATTR_SET_NUM_IRQ(&attr, 1);

        dev_info_t dev[] = {
            {.base=RCAR_GEN3_VIN0_BASE, .irq=RCAR_GEN3_INTCSYS_VIN0},
            {.base=RCAR_GEN3_VIN1_BASE, .irq=RCAR_GEN3_INTCSYS_VIN1},
            {.base=RCAR_GEN3_VIN2_BASE, .irq=RCAR_GEN3_INTCSYS_VIN2},
            {.base=RCAR_GEN3_VIN3_BASE, .irq=RCAR_GEN3_INTCSYS_VIN3},
            {.base=RCAR_GEN3_VIN4_BASE, .irq=RCAR_GEN3_INTCSYS_VIN4},
            {.base=RCAR_GEN3_VIN5_BASE, .irq=RCAR_GEN3_INTCSYS_VIN5},
            {.base=RCAR_GEN3_VIN6_BASE, .irq=RCAR_GEN3_INTCSYS_VIN6},
            {.base=RCAR_GEN3_VIN7_BASE, .irq=RCAR_GEN3_INTCSYS_VIN7},
        };

        for(i = 0; i < NUM_ELTS(dev); i++) {
            hwi_off = hwidev_add(RCAR_HWI_VIN, hwi_devclass_NONE, HWI_NULL_OFF);
            HWIATTR_SET_LOCATION(&attr, dev[i].base, RCAR_GEN3_VIN_SIZE, 0, hwi_find_as(dev[i].base, 1));
            hwitag_add_common(hwi_off, &attr);
            ASSERT(hwi_off != HWI_NULL_OFF);
            hwitag_set_ivec(hwi_off, 0, dev[i].irq);
        }
    }

    {
        unsigned i, hwi_off;

        hwiattr_common_t attr = HWIATTR_COMMON_INITIALIZER;

        dev_info_t dev[] = {
            {.base=RCAR_GEN3_CSI40_BASE},
            {.base=RCAR_GEN3_CSI20_BASE},
        };

        for(i = 0; i < NUM_ELTS(dev); i++) {
            hwi_off = hwidev_add(RCAR_HWI_CAMERA, hwi_devclass_NONE, HWI_NULL_OFF);
            HWIATTR_SET_LOCATION(&attr, dev[i].base, RCAR_GEN3_CSI2_SIZE, 0, hwi_find_as(dev[i].base, 1));
            hwitag_add_common(hwi_off, &attr);
            ASSERT(hwi_off != HWI_NULL_OFF);
        }
    }

    {
        unsigned hwi_off;
        hwiattr_timer_t attr = HWIATTR_TIMER_T_INITIALIZER;
        const struct hwi_inputclk clksrc_kick = {.clk = 10000, .div = 1};

        HWIATTR_TIMER_SET_NUM_CLK(&attr, 1);

        HWIATTR_TIMER_SET_LOCATION(&attr, RCAR_GEN3_RWDT_BASE, RCAR_GEN3_WDT_SIZE, 0, hwi_find_as(RCAR_GEN3_RWDT_BASE, 1));
        hwi_off = hwidev_add_timer(RCAR_HWI_WDT, &attr,  HWI_NULL_OFF);
        ASSERT(hwi_off != HWI_NULL_OFF);
        hwitag_set_inputclk(hwi_off, 0, (struct hwi_inputclk *)&clksrc_kick);

        hwi_off = hwidev_add("wdt,options", 0, HWI_NULL_OFF);
        hwitag_add_regname(hwi_off, "enable_width",     16);

        hwitag_add_regname(hwi_off, "enable_mask",      0x080);
        hwitag_add_regname(hwi_off, "enable_condition", 0x080);
        hwitag_add_regname(hwi_off, "enable_offset",    0x4);

        /* use wdtkick cmd param -W to set wdt,regwrite value
         * since value=0x5A5AFF00 but egname.offset is Uint16t */
    }
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/boards/rcar_gen3/rcar_m3/init_hwinfo.c $ $Rev: 837440 $")
#endif