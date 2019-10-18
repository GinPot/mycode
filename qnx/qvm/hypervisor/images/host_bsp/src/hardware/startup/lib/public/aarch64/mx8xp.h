/*
 * $QNXLicenseC:
 * Copyright 2016, Freescale Semiconductor, Inc.
 * Copyright 2017 NXP
 * Copyright 2018, QNX Software Systems.
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

#ifndef IMXXP_SOC_H_
#define IMXXP_SOC_H_

#include <aarch64/mx8x.h>

/*
 * MIPI
 */
#define IMX_DC0_MIPI0_BASE                      0x56220000
#define IMX_DC0_MIPI1_BASE                      0x56240000

#define IMX_DC0_MIPI0_IRQ                       91
#define IMX_DC0_MIPI1_IRQ                       92

#define IMX_DC0_MIPI0_STEER_IRQ0                800
#define IMX_DC0_MIPI1_STEER_IRQ0                (IMX_DC0_MIPI0_STEER_IRQ0 + 32)
/*
 * CSI
 */
#define IMX_MIPI_CSI0_BASE                     0x58220000

#define IMX_MIPI_CSI0_IRQ                      352

#define IMX_MIPI_CSI0_STEER_IRQ0               (IMX_DC0_MIPI1_STEER_IRQ0 + 32)

/*
 * PI
 */
#define IMX_PI_BASE                            0x58240000

#define IMX_PI_IRQ                             354

#define IMX_PI_STEER_IRQ0                      (IMX_MIPI_CSI0_STEER_IRQ0 + 32)

/*
 * INTMUX from Cortex-M4 subsystem
 */
#define IMX_M40_INTMUX_BASE                    0x37400000
#define IMX_M40_INTMUX_SIZE                    0x1000
#define IMX_M40_INTMUX_IRQ0                    48
#define IMX_M40_INTMUX_BASE_IRQ0               (IMX_PI_STEER_IRQ0 + 32)
#define IMX_M40_INTMUX_IRQ1                    49
#define IMX_M40_INTMUX_BASE_IRQ1               (IMX_M40_INTMUX_BASE_IRQ0 + 32)
#define IMX_M40_INTMUX_IRQ2                    50
#define IMX_M40_INTMUX_BASE_IRQ2               (IMX_M40_INTMUX_BASE_IRQ1 + 32)
#define IMX_M40_INTMUX_IRQ3                    51
#define IMX_M40_INTMUX_BASE_IRQ3               (IMX_M40_INTMUX_BASE_IRQ2 + 32)
#define IMX_M40_INTMUX_IRQ4                    52
#define IMX_M40_INTMUX_BASE_IRQ4               (IMX_M40_INTMUX_BASE_IRQ3 + 32)
#define IMX_M40_INTMUX_IRQ5                    53
#define IMX_M40_INTMUX_BASE_IRQ5               (IMX_M40_INTMUX_BASE_IRQ4 + 32)
#define IMX_M40_INTMUX_IRQ6                    54
#define IMX_M40_INTMUX_BASE_IRQ6               (IMX_M40_INTMUX_BASE_IRQ5 + 32)
#define IMX_M40_INTMUX_IRQ7                    55
#define IMX_M40_INTMUX_BASE_IRQ7               (IMX_M40_INTMUX_BASE_IRQ6 + 32)

/*
 * General Purpose Timer (GPT)
 */
#define IMX_GPT6_BASE                           0x590B0000
#define IMX_GPT7_BASE                           0x590C0000
#define IMX_GPT8_BASE                           0x590D0000
#define IMX_GPT9_BASE                           0x590E0000
#define IMX_GPT10_BASE                          0x590F0000
#define IMX_GPT11_BASE                          0x59100000

/*
 * LPI2C Controller (I2C)
 */
#define IMX_DC0_MIPI0_I2C0_BASE                0x56226000
#define IMX_DC0_MIPI0_I2C1_BASE                0x56227000
#define IMX_DC0_MIPI1_I2C0_BASE                0x56246000
#define IMX_DC0_MIPI1_I2C1_BASE                0x56247000
#define IMX_MIPI_CSI0_I2C0_BASE                0x58226000
#define IMX_PARALEL_I2C0_BASE                  0x58246000
#define IMX_M40_I2C0_BASE                      0x37230000

#define IMX_DC0_MIPI0_I2C0_IRQ                 (IMX_DC0_MIPI0_STEER_IRQ0 + 0x08)
#define IMX_DC0_MIPI0_I2C1_IRQ                 (IMX_DC0_MIPI0_STEER_IRQ0 + 0x09)
#define IMX_DC0_MIPI1_I2C0_IRQ                 (IMX_DC0_MIPI1_STEER_IRQ0 + 0x08)
#define IMX_DC0_MIPI1_I2C1_IRQ                 (IMX_DC0_MIPI1_STEER_IRQ0 + 0x09)
#define IMX_MIPI_CSI0_I2C0_IRQ                 (IMX_MIPI_CSI0_STEER_IRQ0 + 0x08)
#define IMX_PARALEL_I2C0_IRQ                   (IMX_PI_STEER_IRQ0 + 0x08)
#define IMX_M40_I2C0_IRQ                       (IMX_M40_INTMUX_BASE_IRQ0 + 0x9)


#define IMX_LPI2C_COUNT                        11

/*
 * Low Power Universal Asynchronous Receiver/Transmitter (LPUART)
 */
#define IMX_LPUART_COUNT                       4

/*
 * Flexible SPI (FlexSPI)
 */
#define IMX_FLEXSPI0_EXTMEM_MAP_ADDR           0x8000000

/*
 * Synchronous Audio Interface (SAI)
 */
#define IMX_SAI0_BASE                          0x59040000
#define IMX_SAI1_BASE                          0x59050000
#define IMX_SAI2_BASE                          0x59060000
#define IMX_SAI3_BASE                          0x59070000
#define IMX_SAI4_BASE                          0x59820000
#define IMX_SAI5_BASE                          0x59830000

#define IMX_SAI0_IRQ                           346
#define IMX_SAI1_IRQ                           348
#define IMX_SAI2_IRQ                           350
#define IMX_SAI3_IRQ                           355
#define IMX_SAI4_IRQ                           361
#define IMX_SAI5_IRQ                           363

#define IMX_SAI_SIZE                           0x10000
#define IMX_SAI_COUNT                          6

/*
 * Enhanced Serial Audio Interface  (ESAI)
 */
#define IMX_ESAI_COUNT                         1

#endif /* IMXXP_SOC_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/aarch64/mx8xp.h $ $Rev: 851269 $")
#endif
