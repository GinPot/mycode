/*
 * $QNXLicenseC:
 * Copyright 2008, QNX Software Systems. 
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
#include <hw/uefi.h>

EFI_HANDLE			efi_image_handle;
EFI_SYSTEM_TABLE	*efi_system_table;

void *
uefi_find_config_tbl(const EFI_GUID *const uid) {
	EFI_CONFIGURATION_TABLE	*tbl;
	unsigned				i;

	tbl = efi_system_table->ConfigurationTable;
	for(i = 0; i < efi_system_table->NumberOfTableEntries; ++i) {
		if(memcmp(&tbl->VendorGuid, uid, sizeof(*uid)) == 0) {
			return tbl->VendorTable;
		}
		++tbl;
	}
	return NULL;
}

static unsigned uefi_init_cnt = 0;
extern void (*uefi_exit_boot_services_f)(void);

static void _uefi_exit_boot_services(void)
{
	ASSERT(uefi_init_cnt > 0);

	if (--uefi_init_cnt == 0)
	{
		const EFI_BOOT_SERVICES	*const bs = efi_system_table->BootServices;
		_Uintn map_key;
		_Uintn desc_size;
		uint32_t desc_version;
		uint8_t dummy;
		_Uintn map_size = sizeof(dummy);

		if (debug_flag > 0) kprintf("%s, exiting\n", __FUNCTION__);
		uefi_io_flush();

		/* Get size of table */
		bs->GetMemoryMap(&map_size, (EFI_MEMORY_DESCRIPTOR *)&dummy, &map_key, &desc_size, &desc_version);

		EFI_STATUS r = bs->ExitBootServices(efi_image_handle, map_key);
		if(r != 0) {
			kprintf("exit status: %u\n", r);
			crash("Could not exit boot services\n");
		}
		efi_system_table->ConOut = NULL;	/* no more output */
		efi_system_table->ConIn = NULL;	/* no more input */
		uefi_exit_boot_services_f = NULL;
	}
	else if (debug_flag > 0) kprintf("%s, deferred, cnt = %u\n", __FUNCTION__, uefi_init_cnt);
}

void uefi_exit_init(void)
{
	uefi_exit_boot_services_f = _uefi_exit_boot_services;
	++uefi_init_cnt;
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION( "$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/uefi.c $ $Rev: 833536 $" );
#endif
