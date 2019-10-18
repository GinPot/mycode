/*
 * $QNXLicenseC:
 * Copyright 2017 QNX Software Systems.
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


/*
 * Renesas R-Car processor
 * E2X specific definitions
 */

#ifndef __ARM_RCAR_E2X_H_INCLUDED
#define __ARM_RCAR_E2X_H_INCLUDED

#include <arm/r-car-gen2.h>

/*
 * Pin Function Control
 */
#define RCAR_E2X_PFC_BASE               0xE6060000

 /* PFC registers, offset from RCAR_E2X_PFC_BASE */
 #define RCAR_E2X_PFC_PMMR              0x000
 #define RCAR_E2X_PFC_GPSR0             0x004
 #define RCAR_E2X_PFC_GPSR1             0x008
 #define RCAR_E2X_PFC_GPSR2             0x00C
 #define RCAR_E2X_PFC_GPSR3             0x010
 #define RCAR_E2X_PFC_GPSR4             0x014
 #define RCAR_E2X_PFC_GPSR5             0x018

 #define RCAR_E2X_PFC_IPSR0             0x040
 #define RCAR_E2X_PFC_IPSR1             0x044
 #define RCAR_E2X_PFC_IPSR2             0x048
 #define RCAR_E2X_PFC_IPSR3             0x04C
 #define RCAR_E2X_PFC_IPSR4             0x050
 #define RCAR_E2X_PFC_IPSR5             0x054
 #define RCAR_E2X_PFC_IPSR6             0x058
 #define RCAR_E2X_PFC_IPSR7             0x05C
 #define RCAR_E2X_PFC_IPSR8             0x060
 #define RCAR_E2X_PFC_IPSR9             0x064
 #define RCAR_E2X_PFC_IPSR10            0x068
 #define RCAR_E2X_PFC_IPSR11            0x06C
 #define RCAR_E2X_PFC_IPSR12            0x070
 #define RCAR_E2X_PFC_IPSR13            0x074
 #define RCAR_E2X_PFC_IPSR14            0x078
 #define RCAR_E2X_PFC_IPSR15            0x07C
 #define RCAR_E2X_PFC_IPSR16            0x080
 #define RCAR_E2X_PFC_IPSR17            0x084

 #define RCAR_E2X_PFC_MODSEL0           0x0C0
 #define RCAR_E2X_PFC_MODSEL1           0x0C4
 #define RCAR_E2X_PFC_MODSEL2           0x0C8

 #define RCAR_E2X_PFC_PUPR0             0x100
 #define RCAR_E2X_PFC_PUPR1             0x104
 #define RCAR_E2X_PFC_PUPR2             0x108
 #define RCAR_E2X_PFC_PUPR3             0x10C
 #define RCAR_E2X_PFC_PUPR4             0x110
 #define RCAR_E2X_PFC_PUPR5             0x114

 #define RCAR_E2X_PFC_IOCTRL0           0x94
 #define RCAR_E2X_PFC_IOCTRL1           0x98
 #define RCAR_E2X_PFC_IOCTRL2           0x9C
 #define RCAR_E2X_PFC_IOCTRL3           0xA0
 #define RCAR_E2X_PFC_IOCTRL4           0xA8
 #define RCAR_E2X_PFC_IOCTRL5           0xAC
 #define RCAR_E2X_PFC_IOCTRL6           0xB0

/*
 * SDH interface
 */
#define RCAR_E2X_SDHI0_BASE             0xEE100000
#define RCAR_E2X_SDHI1_BASE             0xEE300000
#define RCAR_E2X_SDHI2_BASE             0xEE160000

/*
 * MMCIF
 */
#define RCAR_E2X_MMCIF_BASE             0xEE300000

/*
 * QSPI
 */
#define RCAR_E2X_QSPI_BASE0             0xE6B10000
#define RCAR_E2X_QSPI_BASE1             0xEE200000

/*
 * HSUSB
 */
#define RCAR_E2X_HSUSB_BASE0            0xE6590000
#define RCAR_E2X_HSUSB_BASE1            0xE6598000

/*
 * DMA request ID
 */
#define RCAR_E2X_DREQ_SDHI2_TXI         0xD3
#define RCAR_E2X_DREQ_SDHI2_RXI         0xD4

#endif /* __ARM_RCAR_E2X_H_INCLUDED */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/arm/r-car-e2x.h $ $Rev: 837290 $")
#endif
