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

#include <internal.h>

#ifdef SDIO_HC_SDHCI_PCI

#ifdef SDHCI_PCI_VENDOR_INTEL
extern sdio_product_t	sdio_sdhci_intel_products[];
#endif

#ifdef SDHCI_PCI_VENDOR_O2
extern sdio_product_t	sdio_sdhci_o2_products[];
#endif

#ifdef SDHCI_PCI_VENDOR_CMD
extern sdio_product_t	sdio_sdhci_cmd_products[];
#endif

#ifdef SDHCI_PCI_VENDOR_GENERIC
extern sdio_product_t	sdio_sdhci_product[];
#endif

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/devb/sdmmc/sdiodi/hc/sdhci_pci.h $ $Rev: 825921 $")
#endif
