/*
 * $QNXLicenseC:
 * Copyright 2016, QNX Software Systems. All Rights Reserved.
 *
 * You must obtain a written license from and pay applicable license fees to QNX
 * Software Systems before you may reproduce, modify or distribute this software,
 * or any work that includes all or part of this software.   Free development
 * licenses are available for evaluation and non-commercial purposes.  For more
 * information visit http://licensing.qnx.com or email licensing@qnx.com.
 *
 * This file may contain contributions from others.  Please review this entire
 * file for other proprietary rights or license notices, as well as the QNX
 * Development Suite License Guide at http://licensing.qnx.com/license-guide/
 * for other information.
 * $
 */

#include <hw/i2c.h>
#include "adv748x.h"

/***************************************************************** I2C_RD() ***/
SYM_INTERNAL_ONLY int
i2c_rd(int fd, uint32_t dev_addr, uint8_t reg_addr, uint8_t *data)
{
	struct send_recv
	{
		i2c_sendrecv_t	hdr;
		uint8_t				buf[2];
	} i2c_data;

	int error = EOK;

	i2c_data.buf[0] = reg_addr;
	i2c_data.hdr.send_len = 1;
	i2c_data.hdr.recv_len = 1;
	i2c_data.hdr.slave.addr = dev_addr;
	i2c_data.hdr.slave.fmt = I2C_ADDRFMT_7BIT;
	i2c_data.hdr.stop = 1;

	error = devctl(fd, DCMD_I2C_SENDRECV, &i2c_data, sizeof (i2c_data), NULL);
	if(error != EOK) {
		logmsg( "%s failed,  dev_addr: 0x%2x, reg_addr: 0x%2x, %s", __FUNCTION__, dev_addr, reg_addr, strerror(error));
	}
	else {
		*data = i2c_data.buf[0];
	}
	return error;


}

/***************************************************************** I2C_WR() ***/
SYM_INTERNAL_ONLY int
i2c_wr(int fd, uint32_t dev_addr, uint8_t reg_addr, uint8_t data)
{
	struct send_recv
	{
		i2c_send_t	hdr;
		uint8_t			buf[2];
	} i2c_data;

	int error = EOK;

	i2c_data.buf[0] = reg_addr;
	i2c_data.buf[1] = data;
	i2c_data.hdr.len = 2;
	i2c_data.hdr.slave.addr = dev_addr;
	i2c_data.hdr.slave.fmt = I2C_ADDRFMT_7BIT;
	i2c_data.hdr.stop = 1;

	error = devctl(fd, DCMD_I2C_SEND, &i2c_data, sizeof (i2c_data), NULL);
	if(error != EOK) {
		logmsg( "%s failed,  dev_addr: 0x%2x, reg_addr: 0x%2x, %s", __FUNCTION__, dev_addr, reg_addr, strerror(error));
	}
	return error;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/graphics/branches/release-2.1.x/hardware/capture/adv748x/i2c.c $ $Rev: 867357 $")
#endif
