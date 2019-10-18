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

#include <audio_driver.h>
#include <stdint.h>
#include <errno.h>
#include "i2c2.h"
#include "rcar.h"

extern void cs2000_set(uint8_t enable);
void cs2000_register_dump(void);

int variant_init ( HW_CONTEXT_T *rcar )
{
    int  status;
    char devname[40];

    ado_debug (DB_LVL_DRIVER, "%s", __func__);

    sprintf( devname, "/dev/i2c%d", rcar->i2c_dev );

    if ((status = open_i2c_fd(devname)) != EOK) {
        ado_error_fmt("Failed to open I2C FD");
        return status;
    }

    cs2000_set(0);

    if ( rcar->debug ) {
        cs2000_register_dump();
    }

    return (0);
}

int variant_uninit ( HW_CONTEXT_T *rcar )
{
    ado_debug (DB_LVL_MIXER, "CS2000 UNINIT");

    cs2000_set(0);

    if ( rcar->debug ) {
        cs2000_register_dump();
    }

    close_i2c_fd();

    return EOK;
}

int variant_set_clock_rate ( uint32_t mclk_rate )
{
    ado_debug (DB_LVL_MIXER, "mclk_rate %d", mclk_rate);

    switch( mclk_rate ) {
        case 22579200:
            ado_debug( DB_LVL_DRIVER, "Disabling CS2000 for mclk rate %d", mclk_rate );
            cs2000_set(0);
            break;
        case 24576000:
            ado_debug( DB_LVL_DRIVER, "Enabling CS2000 for mclk rate %d", mclk_rate );
            cs2000_set(1);
            break;
    }

    return EOK;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/deva/ctrl/rcar/nto/aarch64/dll.le/variant.c $ $Rev: 850894 $")
#endif
