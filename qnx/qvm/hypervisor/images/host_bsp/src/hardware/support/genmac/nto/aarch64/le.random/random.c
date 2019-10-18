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

#include <stdint.h>
#include <fcntl.h>
#include "variant.h"

unsigned get_uid(chip_uid_info *chipuid)
{
    uint64_t    data;
    int         fd;

    chipuid->uid_size = sizeof(data);
    chipuid->uid = (uint8_t*) malloc(chipuid->uid_size);
    if (chipuid->uid == NULL)
    {
        fprintf(stderr, "genmac: malloc failed: %s\n", strerror(errno));
        return false;
    }

    fd = open("/dev/random", O_RDONLY);
    if( fd == -1 )
    {
        return false;
    }

    read(fd, &data, sizeof(data));
    close(fd);

    /* Retrieve 8 bytes from random */
    memcpy(chipuid->uid, &data, sizeof(data));

    return true;
}


void free_uid_resources(chip_uid_info *chipuid)
{
    free(chipuid->uid);
    return;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/support/genmac/nto/aarch64/le.random/random.c $ $Rev: 824148 $")
#endif
