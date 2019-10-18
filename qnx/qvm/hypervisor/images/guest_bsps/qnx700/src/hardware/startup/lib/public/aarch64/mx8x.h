/*
 * $QNXLicenseC:
 * Copyright 2016, QNX Software Systems.
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

#ifndef IMX_SOC_H_
#define IMX_SOC_H_

/*
 * Cache Coherent Interconnect 400 (CCI400)
 */
#define IMX_CCI400_BASE                        0x52000000   /* CCCI400 BASE address, CBAR register value */

/*
 * Generic interrupt controller (GIC)
 */
#define IMX_GIC_GICD_BASE                      0x51A00000   /* CPU distributor BASE address */
#define IMX_GIC_GICR_BASE                      0x51B00000   /* CPU redistributor BASE address */
#define IMX_GIC_GICC_BASE                      0x52000000   /* CPU interface, CBAR register value */


/*
 * General Purpose Input/Output (GPIO)
 */
#define IMX_GPIO0_BASE                         0x5D080000
#define IMX_GPIO1_BASE                         0x5D090000
#define IMX_GPIO2_BASE                         0x5D0A0000
#define IMX_GPIO3_BASE                         0x5D0B0000
#define IMX_GPIO4_BASE                         0x5D0C0000
#define IMX_GPIO5_BASE                         0x5D0D0000
#define IMX_GPIO6_BASE                         0x5D0E0000
#define IMX_GPIO7_BASE                         0x5D0F0000

#define IMX_GPIO_SIZE                          0x4000

#define IMX_GPIO0_IRQ                          168
#define IMX_GPIO1_IRQ                          169
#define IMX_GPIO2_IRQ                          170
#define IMX_GPIO3_IRQ                          171
#define IMX_GPIO4_IRQ                          172
#define IMX_GPIO5_IRQ                          173
#define IMX_GPIO6_IRQ                          174
#define IMX_GPIO7_IRQ                          175

/*
 * General Purpose Timer (GPT)
 */
#define IMX_GPT0_BASE                          0x5D140000
#define IMX_GPT1_BASE                          0x5D150000
#define IMX_GPT2_BASE                          0x5D160000
#define IMX_GPT3_BASE                          0x5D170000
#define IMX_GPT4_BASE                          0x5D180000

#define IMX_GPT_SIZE                           0x10000

#define IMX_GPT0_IRQ                           112
#define IMX_GPT1_IRQ                           113
#define IMX_GPT2_IRQ                           114
#define IMX_GPT3_IRQ                           115
#define IMX_GPT4_IRQ                           116


/*
 * High Speed I/O Subsystem (HSIO)
 */
#define IMX_HSIO_BASE                           0x5F000000
/* GPIO */
#define IMX_HSIO_GPIO_BASE                      0x5F170000
#define IMX_HSIO_GPIO_SIZE                      0x10000
/* SYS.CSR */
#define IMX_HSIO_CRR_MISC_BASE                  0x5F160000
#define IMX_HSIO_CRR_MISC_SIZE                  0x10000
#define IMX_HSIO_CRR_SATA0_BASE                 0x5F150000
#define IMX_HSIO_CRR_SATA0_SIZE                 0x10000
#define IMX_HSIO_CRR_PCIEX1_BASE                0x5F140000
#define IMX_HSIO_CRR_PCIEX1_SIZE                0x10000
#define IMX_HSIO_CRR_PCIEX2_BASE                0x5F130000
#define IMX_HSIO_CRR_PCIEX2_SIZE                0x10000
#define IMX_HSIO_CRR_PHYX1_BASE                 0x5F120000
#define IMX_HSIO_CRR_PHYX1_SIZE                 0x10000
#define IMX_HSIO_CRR_PHYX2_BASE                 0x5F110000
#define IMX_HSIO_CRR_PHYX2_SIZE                 0x10000
/* LPCG */
#define IMX_HSIO_LPCG_GPIO_BASE                 0x5F100000
#define IMX_HSIO_LPCG_GPIO_SIZE                 0x10000
#define IMX_HSIO_LPCG_CRR_5_BASE                0x5F0F0000
#define IMX_HSIO_LPCG_CRR_5_SIZE                0x10000
#define IMX_HSIO_LPCG_CRR_4_BASE                0x5F0E0000
#define IMX_HSIO_LPCG_CRR_4_SIZE                0x10000
#define IMX_HSIO_LPCG_CRR_3_BASE                0x5F0D0000
#define IMX_HSIO_LPCG_CRR_3_SIZE                0x10000
#define IMX_HSIO_LPCG_CRR_2_BASE                0x5F0C0000
#define IMX_HSIO_LPCG_CRR_2_SIZE                0x10000
#define IMX_HSIO_LPCG_CRR_1_BASE                0x5F0B0000
#define IMX_HSIO_LPCG_CRR_1_SIZE                0x10000
#define IMX_HSIO_LPCG_CRR_0_BASE                0x5F0A0000
#define IMX_HSIO_LPCG_CRR_0_SIZE                0x10000
#define IMX_HSIO_LPCG_PHYX1_BASE                0x5F090000
#define IMX_HSIO_LPCG_PHYX1_SIZE                0x10000
#define IMX_HSIO_LPCG_PHYX2_BASE                0x5F080000
#define IMX_HSIO_LPCG_PHYX2_SIZE                0x10000
#define IMX_HSIO_LPCG_PCIEB_BASE                0x5F060000
#define IMX_HSIO_LPCG_PCIEB_SIZE                0x10000
#define IMX_HSIO_LPCG_PCIEA_BASE                0x5F050000
#define IMX_HSIO_LPCG_PCIEA_SIZE                0x10000
/* SATA AHCI */
#define IMX_SATA_BASE                           0x5F020000
#define IMX_SATA_SIZE                           0x10000
#define IMX_SATA_IRQ0                           120
#define IMX_SATA_IRQ1                           121

/*
 * LPI2C Controller (I2C)
 */
#define IMX_LSIO_I2C0_BASE                     0x5A800000
#define IMX_LSIO_I2C1_BASE                     0x5A810000
#define IMX_LSIO_I2C2_BASE                     0x5A820000
#define IMX_LSIO_I2C3_BASE                     0x5A830000
#define IMX_LSIO_I2C4_BASE                     0x5A840000

#define IMX_LSIO_I2C0_IRQ                      252
#define IMX_LSIO_I2C1_IRQ                      253
#define IMX_LSIO_I2C2_IRQ                      254
#define IMX_LSIO_I2C3_IRQ                      255
#define IMX_LSIO_I2C4_IRQ                      256

#define IMX_LPI2C_SIZE                         0x10000
#define IMX_LPI2C_FIFO_SIZE                    0x4

/*
 * Low Power Universal Asynchronous Receiver/Transmitter (LPUART)
 */
#define IMX_LPUART0_BASE                       0x5A060000
#define IMX_LPUART1_BASE                       0x5A070000
#define IMX_LPUART2_BASE                       0x5A080000
#define IMX_LPUART3_BASE                       0x5A090000
#define IMX_LPUART4_BASE                       0x5A0A0000

#define IMX_LPUART0_IRQ                        257
#define IMX_LPUART1_IRQ                        258
#define IMX_LPUART2_IRQ                        259
#define IMX_LPUART3_IRQ                        260
#define IMX_LPUART4_IRQ                        261

#define IMX_LPUART_SIZE                        0x10000

/*
 * Messaging unit (MU)
 */
#define IMX_MU0_BASE                           0x5D1B0000
#define IMX_MU1_BASE                           0x5D1C0000
#define IMX_MU2_BASE                           0x5D1D0000
#define IMX_MU3_BASE                           0x5D1E0000
#define IMX_MU4_BASE                           0x5D1F0000

/*
 * PCI Express (PCIe)
 */
#define IMX_PCIEA_BASE                         0x5F000000
#define IMX_PCIEB_BASE                         0x5F010000
#define IMX_PCIE_SIZE                          0x10000

/* PCIeA interrupts */
#define IMX_PCIEA_IRQ_A                        105
#define IMX_PCIEA_IRQ_B                        106
#define IMX_PCIEA_IRQ_C                        107
#define IMX_PCIEA_IRQ_D                        108
#define IMX_PCIEA_MSI                          102

/* PCIeB interrupts */
#define IMX_PCIEB_IRQ_A                        137
#define IMX_PCIEB_IRQ_B                        138
#define IMX_PCIEB_IRQ_C                        139
#define IMX_PCIEB_IRQ_D                        140
#define IMX_PCIEB_MSI                          134

/*
 * Universal Serial Bus Controller (USB)
 */
#define IMX_USB2OTG1_BASE                       0x5B0D0000
#define IMX_USB2PHY1_BASE                       0x5B100000
#define IMX_USB2_SIZE                           0x10000
#define IMX_USB2PHY1_SIZE                       0x10000
#define IMX_USB2OTG2_IRQ                        299

#define IMX_USB3_BASE                          0x5B110000
#define IMX_USB3_CTRL_BASE                     0x5B110000
#define IMX_USB3_CORE_BASE                     0x5B120000
#define IMX_USB3_PHY_BASE                      0x5B160000

#define IMX_USB3OTG1_IRQ                       303

/*
 * Ultra Secured Digital Host Controller (uSDHC)
 */
#define IMX_USDHC0_BASE                        0x5B010000
#define IMX_USDHC1_BASE                        0x5B020000
#define IMX_USDHC2_BASE                        0x5B030000

#define IMX_USDHC_SIZE                         0x10000
#define IMX_USDHC_MAP_SIZE                     0x100

#define IMX_USDHC0_IRQ                         264
#define IMX_USDHC1_IRQ                         265
#define IMX_USDHC2_IRQ                         266

/*
 * 10/100/1000-Mbps Ethernet MAC (ENET)
 */
/* ENET - base addresses */
#define IMX_ENET0_BASE                         0x5B040000
#define IMX_ENET1_BASE                         0x5B050000

/* ENET - memory size */
#define IMX_ENET0_MEM_SIZE                     0x10000
#define IMX_ENET1_MEM_SIZE                     0x10000

/* ENET - interrupt vector numbers */
#define IMX_ENET0_IRQ                          290U
#define IMX_ENET1_IRQ                          294U


/*
 * Low Power Serial Peripheral Interface (LPSPI)
 */
#define IMX_LPSPI0_BASE                        0x5A000000
#define IMX_LPSPI1_BASE                        0x5A010000
#define IMX_LPSPI2_BASE                        0x5A020000
#define IMX_LPSPI3_BASE                        0x5A030000

#define IMX_LPSPI0_IRQ                         248
#define IMX_LPSPI1_IRQ                         249
#define IMX_LPSPI2_IRQ                         250
#define IMX_LPSPI3_IRQ                         251

#define IMX_LPSPI_SIZE                         0x10000
#define IMX_LPSPI_COUNT                        4

/*
 * Flexible SPI (FlexSPI)
 */
#define IMX_FLEXSPI0_BASE                      0x5D120000
#define IMX_FLEXSPI1_BASE                      0x5D130000

#define IMX_FLEXSPI_SIZE                       0x10000

#define IMX_FLEXSPI0_IRQ                       124
#define IMX_FLEXSPI1_IRQ                       125

/*
 * Asynchronous Sample Rate Converter (ASRC)
 */
#define IMX_ASRC0_BASE                         0x59000000
#define IMX_ASRC1_BASE                         0x59800000
#define IMX_ASRC0_IRQ                          405
#define IMX_ASRC1_IRQ                          412

#define IMX_ASRC_SIZE                          0x10000

/*
 * Enhanced Serial Audio Interface  (ESAI)
 */
#define IMX_ESAI0_BASE                         0x59010000
#define IMX_ESAI1_BASE                         0x59810000

#define IMX_ESAI0_IRQ                          420
#define IMX_ESAI1_IRQ                          421

#define IMX_ESAI_SIZE                          0x10000

/*
 * Audio Clock Mux (ACM)
 */
#define IMX_ACM_BASE                           0x59E00000

#define IMX_ACM_SIZE                           0x10000

/*
 * Flexible CAN (FlexCAN)
 */
#define IMX_FLEXCAN0_REG_BASE                  0x5A8D0000
#define IMX_FLEXCAN1_REG_BASE                  0x5A8E0000
#define IMX_FLEXCAN2_REG_BASE                  0x5A8F0000
#define IMX_FLEXCAN0_MEM_BASE                  0x5A8D0080
#define IMX_FLEXCAN1_MEM_BASE                  0x5A8E0080
#define IMX_FLEXCAN2_MEM_BASE                  0x5A8F0080
#define IMX_FLEXCAN0_IRQ                       267
#define IMX_FLEXCAN1_IRQ                       268
#define IMX_FLEXCAN2_IRQ                       269
#define IMX_FLEXCAN_COUNT                      3

/*
 * NAND
 */
#define IMX_APBH_BASE                          0x5B810000
#define IMX_APBH_SIZE                          0x1000
#define IMX_APBH_IRQ                           306

#define IMX_BCH_BASE                           0x5B814000
#define IMX_BCH_SIZE                           0x200
#define IMX_BCH_IRQ                            304
#define IMX_BCH_MAX_ECC                        62

#define IMX_GPMI_BASE                          0x5B812000
#define IMX_GPMI_SIZE                          0x200
#define IMX_GPMI_IRQ                           305

#endif /* IMX_SOC_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/aarch64/mx8x.h $ $Rev: 852684 $")
#endif
