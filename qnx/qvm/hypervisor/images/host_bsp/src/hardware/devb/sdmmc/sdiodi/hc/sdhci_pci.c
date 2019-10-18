/*
 * $QNXLicenseC:
 * Copyright 2007, 2008, QNX Software Systems.
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

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <unistd.h>

#include <hw/inout.h>

#include <sys/mman.h>

#include <internal.h>
#include <sdhci.h>

#ifdef SDIO_HC_SDHCI_PCI

#include <pci/pci.h>
#include <pci/pci_id.h>
#include <pci/pci_ccode.h>
#include "pci_devices.h"

int sdhci_pci_init( sdio_hc_t *hc )
{
	sdio_hc_cfg_t		*cfg;

	cfg	= &hc->cfg;

	// Timing caps have to be set before sdhci_init() is called,
	switch( cfg->vid ) {
		case PCI_VID_Intel_Corp:
			if( cfg->did == PCI_DEVICE_ID_INTEL_BROXTON_EMMC ||
				cfg->did == PCI_DEVICE_ID_INTEL_DENVERTON_EMMC ) {
				hc->caps |= HC_CAP_HS200 | HC_CAP_HS400;
				hc->caps |= HC_CAP_SLOT_TYPE_EMBEDDED;
				hc->flags |= HC_FLAG_DEV_MMC;
			}
			break;

		default:
			break;
	}

	return sdhci_init( hc );
}

#ifdef SDHCI_PCI_VENDOR_GENERIC
sdio_product_t	sdio_sdhci_product[] = {
	{ SDIO_DEVICE_ID_WILDCARD, PCI_CCODE_PERIPHERAL_SD_HOST_CTRL, 0, "Generic SDHCI", sdhci_pci_init },
	{ 0, 0, 0, NULL, NULL }
};
#endif

#ifdef SDHCI_PCI_VENDOR_O2
sdio_product_t	sdio_sdhci_o2_products[] = {
	{ PCI_DEVICE_ID_O2_8321, 0, 0, "O2 Micro 8321", sdhci_pci_init },
	{ 0, 0, 0, NULL, NULL }
};
#endif

#ifdef SDHCI_PCI_VENDOR_CMD
sdio_product_t	sdio_sdhci_cmd_products[] = {
	{ PCI_DEVICE_ID_CMD_670, 0, 0, "Sil 670", sdhci_pci_init },
	{ 0, 0, 0, NULL, NULL }
};
#endif

#ifdef SDHCI_PCI_VENDOR_INTEL
sdio_product_t	sdio_sdhci_intel_products[] = {
	{ PCI_DEVICE_ID_INTEL_SCH_SDIO1, 0, 0, "Intel SCH", sdhci_pci_init },
	{ PCI_DEVICE_ID_INTEL_SCH_SDIO2, 0, 0, "Intel SCH", sdhci_pci_init },
	{ PCI_DEVICE_ID_INTEL_SCH_SDIO3, 0, 0, "Intel SCH", sdhci_pci_init },
	{ PCI_DEVICE_ID_INTEL_BROXTON_SDCARD, 0, 0, "Intel Broxton", sdhci_pci_init },
	{ PCI_DEVICE_ID_INTEL_BROXTON_EMMC,   0, 0, "Intel Broxton", sdhci_pci_init },
	{ PCI_DEVICE_ID_INTEL_BROXTON_SDIO,   0, 0, "Intel Broxton", sdhci_pci_init },
	{ PCI_DEVICE_ID_INTEL_DENVERTON_EMMC, 0, 0, "Intel Denverton", sdhci_pci_init },
	{ 0, 0, 0, NULL, NULL }
};
#endif

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/devb/sdmmc/sdiodi/hc/sdhci_pci.c $ $Rev: 850035 $")
#endif
