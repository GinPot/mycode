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

#ifndef IMXM_SOC_H_
#define IMXM_SOC_H_

#include <aarch64/mx8x.h>

/*
 * LVDS
 */
#define IMX_DC0_LVDS_BASE                      0x56240000
#define IMX_DC1_LVDS_BASE                      0x57240000

#define IMX_DC0_LVDS_IRQ                       89
#define IMX_DC1_LVDS_IRQ                       90

#define IMX_DC0_LVDS_STEER_IRQ0                800
#define IMX_DC1_LVDS_STEER_IRQ0                (IMX_DC0_LVDS_STEER_IRQ0 + 32)
/*
 * MIPI
 */
#define IMX_DC0_MIPI_BASE                      0x56220000
#define IMX_DC1_MIPI_BASE                      0x57220000

#define IMX_DC0_MIPI_IRQ                       91
#define IMX_DC1_MIPI_IRQ                       92

#define IMX_DC0_MIPI_STEER_IRQ0                (IMX_DC1_LVDS_STEER_IRQ0 + 32)
#define IMX_DC1_MIPI_STEER_IRQ0                (IMX_DC0_MIPI_STEER_IRQ0 + 32)
/*
 * CSI
 */
#define IMX_MIPI_CSI0_BASE                     0x58220000
#define IMX_MIPI_CSI1_BASE                     0x58240000

#define IMX_MIPI_CSI0_IRQ                      352
#define IMX_MIPI_CSI1_IRQ                      353

#define IMX_MIPI_CSI0_STEER_IRQ0               (IMX_DC1_MIPI_STEER_IRQ0 + 32)
#define IMX_MIPI_CSI1_STEER_IRQ0               (IMX_MIPI_CSI0_STEER_IRQ0 + 32)
/*
 * HDMI
 */
#define IMX_HDMI_RX_BASE                       0x58260000
#define IMX_HDMI_TX_BASE                       0x56260000

#define IMX_HDMI_RX_IRQ                        354
#define IMX_HDMI_TX_IRQ                        93

#define IMX_HDMI_RX_STEER_IRQ0                 (IMX_MIPI_CSI1_STEER_IRQ0 + 32)
#define IMX_HDMI_TX_STEER_IRQ0                 (IMX_HDMI_RX_STEER_IRQ0 + 32)

/*
 * LPI2C Controller (I2C)
 */
#define IMX_DC0_LVDS_I2C0_BASE                 0x56246000
#define IMX_DC0_LVDS_I2C1_BASE                 0x56247000
#define IMX_DC0_MIPI_I2C0_BASE                 0x56226000
#define IMX_DC0_MIPI_I2C1_BASE                 0x56227000
#define IMX_DC1_LVDS_I2C0_BASE                 0x57246000
#define IMX_DC1_LVDS_I2C1_BASE                 0x57247000
#define IMX_DC1_MIPI_I2C0_BASE                 0x57226000
#define IMX_DC1_MIPI_I2C1_BASE                 0x57227000
#define IMX_MIPI_CSI0_I2C0_BASE                0x58226000
#define IMX_MIPI_CSI1_I2C0_BASE                0x58246000
#define IMX_HDMI_RX_I2C0_BASE                  0x58266000
#define IMX_HDMI_TX_I2C0_BASE                  0x56266000

#define IMX_DC0_LVDS_I2C0_IRQ                  (IMX_DC0_LVDS_STEER_IRQ0 + 0x08)
#define IMX_DC0_LVDS_I2C1_IRQ                  (IMX_DC0_LVDS_STEER_IRQ0 + 0x09)
#define IMX_DC1_LVDS_I2C0_IRQ                  (IMX_DC1_LVDS_STEER_IRQ0 + 0x08)
#define IMX_DC1_LVDS_I2C1_IRQ                  (IMX_DC1_LVDS_STEER_IRQ0 + 0x09)
#define IMX_DC0_MIPI_I2C0_IRQ                  (IMX_DC0_MIPI_STEER_IRQ0 + 0x08)
#define IMX_DC0_MIPI_I2C1_IRQ                  (IMX_DC0_MIPI_STEER_IRQ0 + 0x09)
#define IMX_DC1_MIPI_I2C0_IRQ                  (IMX_DC1_MIPI_STEER_IRQ0 + 0x08)
#define IMX_DC1_MIPI_I2C1_IRQ                  (IMX_DC1_MIPI_STEER_IRQ0 + 0x09)
#define IMX_MIPI_CSI0_I2C0_IRQ                 (IMX_MIPI_CSI0_STEER_IRQ0 + 0x08)
#define IMX_MIPI_CSI1_I2C0_IRQ                 (IMX_MIPI_CSI1_STEER_IRQ0 + 0x08)
#define IMX_HDMI_RX_I2C0_IRQ                   (IMX_HDMI_RX_STEER_IRQ0 + 0x08)
#define IMX_HDMI_TX_I2C0_IRQ                   (IMX_HDMI_TX_STEER_IRQ0 + 0x08)

#define IMX_LPI2C_COUNT                        17

/*
 * Low Power Universal Asynchronous Receiver/Transmitter (LPUART)
 */
#define IMX_LPUART_COUNT                       5

/*
 * Synchronous Audio Interface (SAI)
 */
#define IMX_SAI0_BASE                          0x59040000
#define IMX_SAI1_BASE                          0x59050000
#define IMX_SAI2_BASE                          0x59060000
#define IMX_SAI3_BASE                          0x59070000
#define IMX_SAI4_BASE                          0x59080000
#define IMX_SAI5_BASE                          0x59090000
#define IMX_SAI6_BASE                          0x59820000
#define IMX_SAI7_BASE                          0x59830000

#define IMX_SAI0_IRQ                           346
#define IMX_SAI1_IRQ                           348
#define IMX_SAI2_IRQ                           350
#define IMX_SAI3_IRQ                           355
#define IMX_SAI4_IRQ                           357
#define IMX_SAI5_IRQ                           359
#define IMX_SAI6_IRQ                           361
#define IMX_SAI7_IRQ                           363

#define IMX_SAI_SIZE                           0x10000
#define IMX_SAI_COUNT                          8

/*
 * Enhanced Serial Audio Interface  (ESAI)
 */
#define IMX_ESAI_COUNT                         2

#endif /* IMXM_SOC_H_ */


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/aarch64/mx8m.h $ $Rev: 851361 $")
#endif
