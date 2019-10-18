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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/neutrino.h>
#include <sys/resmgr.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>
#include <assert.h>
#include <devctl.h>
#include <hw/rcar-thermal.h>

#include "io.h"

static pthread_mutex_t g_csIOCTL= PTHREAD_MUTEX_INITIALIZER;
int io_devctl(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb)
{
    int                status = 0;
    uint32_t           dcmd;
    ths_civm_packet_t *packet;

    if ((status = iofunc_devctl_default(ctp, msg, ocb)) != _RESMGR_DEFAULT) {
        return(status);
    }

    if (msg->i.nbytes < sizeof(ths_civm_packet_t))
        return EINVAL;

    dcmd = msg->i.dcmd;

    SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(ths_civm_packet_t));
    memset(&msg->o, 0, sizeof(msg->o));
    packet = (ths_civm_packet_t *) _DEVCTL_DATA(msg->o);

    if((status = pthread_mutex_lock( &g_csIOCTL )) != EOK)
        return status;

    switch (dcmd) {
        case DCMD_THSCIVM_GET_TEMPERATURE:
            status = ths_civm_get_temperature(&packet->temperature);
            break;
        case DCMD_THSCIVM_GET_VOLTAGE:
            status = ths_civm_get_voltage(&packet->voltage);
            break;
        case DCMD_THSCIVM_GET_TEMP_VOLT:
            status = ths_civm_get_temperature(&packet->temperature);
            if(status != EOK)
                break;
            status = ths_civm_get_voltage(&packet->voltage);
            break;
        default:
            status = ENOSYS;
    }

    if(pthread_mutex_unlock( &g_csIOCTL ) != EOK)
        return ENG;

    if(status != EOK)
        return status;

    return(_RESMGR_NPARTS(1));
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/support/rcar-thermal/resmgr/io_devctl.c $ $Rev: 841525 $")
#endif
