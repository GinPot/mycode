/*
 * $QNXLicenseC:
 * Copyright 2014,2017 QNX Software Systems.
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <devctl.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/neutrino.h>
#include <hw/inout.h>
#include <hw/rcar-thermal.h>

const char	*devname = "/dev/thermal";

//	Print Usage options and exit
void usage(char* pcProgname)
{
	printf( "Usage: %s [options]\n"
			"\t-h        Prints this info\n"
			"\n",
			pcProgname);
}

int
main(int argc, char *argv[])
{
	int 	tpsFd;
	int 	status;
	int c;

    while ((c = getopt(argc, argv, "h:")) != -1)
    {
        switch (c)
        {
            case 'h':
                usage(argv[0]);
                exit(1);
            default:
                usage(argv[0]);
                exit(-1);
                break;
        }

    }

	printf("\n");
	printf("************************************\n");
	printf("*   Thermal test start             *\n");
	printf("************************************\n");

	tpsFd = open( devname, O_RDWR);
	if (tpsFd < 0)
	{
		fprintf(stderr, "open(devname failed errno=%d -  %s\n", errno, strerror(errno));
		return -1;
	}
	ths_civm_packet_t data;

	/* Test getting temperature from thermal */
	printf("\n");
	printf("1. Get LSI temperature\n");
	data.temperature = 0;
	status = devctl( tpsFd, DCMD_THSCIVM_GET_TEMPERATURE, &data, sizeof(data), NULL);
	if (status != EOK)
	{
		errno = status;
		perror("devctl(DCMD_THSCIVM_GET_TEMPERATURE)");
		exit(-1);
	}
	else
	{
		printf("   THS temperature = %d oC\n", data.temperature);
		printf("   Test is done\n");
	}

	/* Test getting voltage from thermal */
	 printf("\n");
	 printf("2. Get LSI core voltage \n");
	 data.voltage = 0;
	 status = devctl( tpsFd, DCMD_THSCIVM_GET_VOLTAGE, &data, sizeof(data), NULL);
	 if (status != EOK)
	 {
		 errno = status;
		 perror("devctl(DCMD_THSCIVM_GET_VOLTAGE)");
		 exit(-1);
	 }
	 else
	 {
		 printf("   LSI voltage = %d mV\n", data.voltage);
		 printf("   Test is done\n");
	 }


	close(tpsFd);

    exit(0);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/support/rcar-thermal/test/thermal_test.c $ $Rev: 841525 $")
#endif
