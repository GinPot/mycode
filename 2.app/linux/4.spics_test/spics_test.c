// SPDX-License-Identifier: GPL-2.0-only
/*
 * SPI testing utility (using spidev driver)
 *
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * Cross-compile with cross-gcc -I/path/to/cross-kernel/include
 */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <sys/stat.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

int main(int argc, char *argv[])
{
	int fd;
	char buf[] = "0";

	fd= open("/sys/devices/platform/soc/a80000.spi/spi_master/spi0/spi0.0/cs_si47961", O_WRONLY);

	if (-1 == write(fd, buf, strlen(buf))) {
	    printf("write power key fail.\n");
	}

	printf("%d %s: %c.\n",argc,argv[0], buf[0]);
	
	close(fd);
    return 0;
}