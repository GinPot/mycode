/*
 * $QNXLicenseC:
 * Copyright 2016, 2017 QNX Software Systems.
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

#include "proto.h"

void query_hwi_device(rcar_i2c_dev_t *dev, unsigned interface)
{
    hwi_tag *tag;
    unsigned hwi_off = hwi_find_bus(RCAR_HWI_IIC, interface);
    if(hwi_off != HWI_NULL_OFF){
        tag = hwi_tag_find(hwi_off, HWI_TAG_NAME_location, 0);
        if(tag){
            dev->physbase = tag->location.base;
        }
        tag = hwi_tag_find(hwi_off, HWI_TAG_NAME_irq, 0);
        if(tag){
            dev->irq = tag->irq.vector;
        }
        tag = hwi_tag_find(hwi_off, HWI_TAG_NAME_inputclk, 0);
        if(tag){
            dev->pck = tag->inputclk.clk/tag->inputclk.div;
        }
    }
}

int rcar_i2c_options(rcar_i2c_dev_t *dev, int argc, char *argv[])
{
    int done = 0;
    int	prev_optind, c;
    unsigned interface = 0;

    dev->physbase  = 0;
    dev->irq       = 0;
    dev->pck       = 0;
    dev->clockLow  = 5;
    dev->clockHigh = 4;
	dev->speed     = 100 * 1000;

    while (!done) {
        prev_optind = optind;
        c = getopt(argc, argv, "I:p:d:i:s:c:");

        switch (c) {
            case 'I':
                interface = strtoul(optarg, &optarg, 0);
                query_hwi_device(dev, interface);
                break;
            case 'i':
                dev->irq = strtol(optarg, &optarg, 0);
                break;
            case 'p':
                dev->physbase = strtoul(optarg, &optarg, 0);
                break;
            case 's':
                dev->slave_addr = strtoul(optarg, &optarg, 0);
                break;
            case 'c':
                dev->pck = strtoul(optarg, &optarg, 0);
                break;
            case 'd':
                c = sscanf(optarg, "%d/%d", &dev->clockLow, &dev->clockHigh);
                if ((2 != c) || ((dev->clockLow <= 0) || (dev->clockHigh <= 0))) {
                    fprintf(stderr, "Invalid duty cycle specified: %s\n", optarg);
                    dev->clockLow = 5;
                    dev->clockHigh = 4;
                }
                break;
            case '?':
                if (optopt == '-') {
                    ++optind;
                    break;
                }
                return -1;

            case -1:
                if (prev_optind < optind) /* -- */
                    return -1;

                if (argv[optind] == NULL) {
                    done = 1;
                    break;
                }
                if (*argv[optind] != '-') {
                    ++optind;
                    break;
                }
                return -1;

            case ':':
            default:
                return -1;
        }
    }

    // Use 5/3 duty for super fast speed.
    if (dev->clockLow == 5 && dev->clockHigh == 4 && dev->speed > 400 * 1000)
        dev->clockHigh = 3;

    if (dev->pck == 0)
        dev->pck = 130000000;

    return 0;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/i2c/rcar-B/options.c $ $Rev: 837440 $")
#endif
