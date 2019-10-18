/*
 * $QNXLicenseC:
 * Copyright 2016, QNX Software Systems.
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

// Module Description: device specific header file

#ifndef _DS_H_INCLUDED
#define _DS_H_INCLUDED


#define ds_devctl( _hba, _ccb )                 CAM_REQ_CMP
#define ds_temperature( __tgt, __ccb )          CAM_REQ_CMP_ERR
#define ds_ata_init_device( __tgt )             CAM_SUCCESS
#define ds_ata_dinit_device( __tgt )            CAM_SUCCESS
#define ds_atapi_sense( __tgt, __ccb )          CAM_SUCCESS
#define ds_atapi_init_device( __tgt )           CAM_SUCCESS
#define ds_atapi_dinit_device( __tgt )          CAM_SUCCESS

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/devb/rcarsata/sim_ds.h $ $Rev: 814232 $")
#endif
