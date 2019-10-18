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

//#include <audio_driver.h>

#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <hw/i2c.h>

static int          i2c_fd = -1;

struct OMSG
{
    i2c_send_t  hdr;        /* Message header */
    uint8_t     bytes[8];   /* Transmit buffer with (optional) extra space */
} omsg_t;

struct IMSG
{
    i2c_recv_t  hdr;        /* Message header */
    uint8_t     bytes[16];  /* Receive buffer */
} imsg_t;


int open_i2c_fd (const char* i2c_devname)
{
    if( i2c_fd < 0 ) {
        i2c_fd = open(i2c_devname, O_RDWR);
        if (i2c_fd < 0) {
            return ENODEV;
        }
        return EOK;
    }
    return EALREADY;
}

int close_i2c_fd(void)
{
    if( i2c_fd < 0 ) {
        return EBADF;
    }

    close(i2c_fd);
    i2c_fd = -1;

    return EOK;
}

int i2c_write(uint32_t addr, uint8_t reg, uint8_t valmsb, uint8_t vallab, int index)
{
    int         status;
    struct OMSG omsg;

    if( i2c_fd < 0 ) {
        return EBADF;
    }

    omsg.hdr.slave.addr = addr + index;
    omsg.hdr.slave.fmt = I2C_ADDRFMT_7BIT;
    omsg.hdr.len = 3;
    omsg.hdr.stop = 1;
    omsg.bytes[0] = reg;
    omsg.bytes[1] = valmsb;
	omsg.bytes[2] = vallab;

    status = devctl(i2c_fd, DCMD_I2C_SEND, &omsg, sizeof(omsg.hdr) + omsg.hdr.len, NULL);

    return status;
}

int i2c_read(uint32_t addr, uint8_t reg, unsigned short* val, int index)
{
    int             status;
    iov_t           siov[2], riov[3];
    unsigned char   bdata1;
    unsigned char   bdata2;
    i2c_sendrecv_t  hdr;    /* Message header */

    if( i2c_fd < 0 ) {
        return EBADF;
    }

    //Write register address which is need to be read.
    hdr.slave.addr = addr + index;
    hdr.slave.fmt = I2C_ADDRFMT_7BIT;
    hdr.stop = 1;

    hdr.send_len = 1;
    hdr.recv_len = 2 ;

    SETIOV(&siov[0], &hdr, sizeof(hdr) );
    SETIOV(&siov[1], &reg, 1 );

    SETIOV(&riov[0], &hdr, sizeof(hdr) );
    SETIOV(&riov[1], &bdata1, 1 );
    SETIOV(&riov[2], &bdata2, 1 );
    status = devctlv(i2c_fd, DCMD_I2C_SENDRECV, 2, 3, siov, riov, NULL);

    if( status == EOK ) {
        *val = (bdata1 << 8) | bdata2;
    }

    return status;
}

int i2c_devrdy(void)
{
    if( i2c_fd < 0) {
        return EBADF;
    }

    return EOK;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/deva/mixer/ak4613/i2c2.c $ $Rev: 840692 $")
#endif
