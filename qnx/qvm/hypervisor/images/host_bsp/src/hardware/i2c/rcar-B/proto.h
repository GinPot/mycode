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


#ifndef _PROTO_H_INCLUDED
#define _PROTO_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/neutrino.h>
#include <hw/inout.h>
#include <hw/i2c.h>
#include <drvr/hwinfo.h>
#include <hw/hwinfo_rcar.h>

#define CMODE_SEND          0x00000001
#define CMODE_RECV          0x00000002
#define CMODE_DOSTOP        0x00000004
#define CMODE_SADDR_L       0x00000008
#define CMODE_SADDR_H       0x00000010
#define CMODE_RECV_ONE      0x00000020
#define CMODE_RECV_TWO      0x00000040
#define CMODE_WAIT          0x00000080
#define CMODE_STOPRECV      0x00000100
#define CMODE_NORMAL_SPEED  0x00000200

#define I2C_BUSY            0x80000000

#define MAX_POOL            0x500

/* IIC Controller Specific Registers */
#define RCAR_IIC_SIZE       0x100

/* IIC registers, offset from base */
#define RCAR_IIC_ICDR       0x0000      // Data Register
#define RCAR_IIC_ICCR       0x0004      // Control Register
#define RCAR_IIC_ICSR       0x0008      // Status Register
#define RCAR_IIC_ICIC       0x000C      // Interrupt Control Register
#define RCAR_IIC_ICCL       0x0010      // Clock Control Register Low
#define RCAR_IIC_ICCH       0x0014      // Clock Control Register High

/* ICCR bit definition */
#define RCAR_ICCR_ICE       (1 << 7)    // IIC Interface Enable
#define RCAR_ICCR_RACK      (1 << 6)    // Receive Acknowledge
#define RCAR_ICCR_MTM       (1 << 4)    // Master Transmit Mode
#define RCAR_ICCR_MRM       (0 << 4)    // Master Receive Mode
#define RCAR_ICCR_BBSY      (1 << 2)    // Bus Busy
#define RCAR_ICCR_SCP       (1 << 0)    // START/STOP Condition Prohibit

/* ICSR bit definition */
#define RCAR_ICSR_BUSY      (1 << 4)    // Transmit Busy
#define RCAR_ICSR_AL        (1 << 3)    // Arbitration Lost
#define RCAR_ICSR_TACK      (1 << 2)    // Transmit Acknowledge
#define RCAR_ICSR_WAIT      (1 << 1)    // Wait
#define RCAR_ICSR_DTE       (1 << 0)    // Data Transmit Enable

/* ICIC bit definition */
#define RCAR_ICIC_AL        (1 << 3)    // Arbitration Lost
#define RCAR_ICIC_TACK      (1 << 2)    // Transmit Acknowledge
#define RCAR_ICIC_WAIT      (1 << 1)    // Wait
#define RCAR_ICIC_DTE       (1 << 0)    // Data Transmit Enable


typedef struct {
    uintptr_t       regbase;
    unsigned        physbase;

    uint32_t        pck;        /* peripheral clock */
    uint32_t        slave_addr;
    i2c_addrfmt_t   slave_addr_fmt;
    struct sigevent intrevent;
    int             irq;
    int             iid;

    uint8_t         *buf;
    int             len;
    int             bytes;
    uint32_t        mode;
    uint32_t        iccl;
    uint32_t        icch;
    uint32_t        status;
    int             max_ints;
    int32_t         clockLow;
    int32_t         clockHigh;
    uint32_t        speed;
} rcar_i2c_dev_t;

uint32_t rcar_i2c_poll_busy(rcar_i2c_dev_t *dev, uint8_t mask, uint8_t bit);
uint32_t rcar_i2c_poll_complete(rcar_i2c_dev_t *dev);
int rcar_i2c_options(rcar_i2c_dev_t *dev, int argc, char *argv[]);

void *rcar_i2c_init(int argc, char *argv[]);
void rcar_i2c_fini(void *hdl);

int rcar_i2c_set_slave_addr(void *hdl, unsigned int addr, i2c_addrfmt_t fmt);
int rcar_i2c_set_bus_speed(void *hdl, unsigned int speed, unsigned int *ospeed);

i2c_status_t rcar_i2c_recv(void *hdl, void *buf, unsigned int len, unsigned int stop);
i2c_status_t rcar_i2c_send(void *hdl, void *buf, unsigned int len, unsigned int stop);

const struct sigevent * rcar_i2c_intr(void *area, int id);

int rcar_i2c_version_info(i2c_libversion_t *version);
int rcar_i2c_driver_info(void *hdl, i2c_driver_info_t *info);

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/i2c/rcar-B/proto.h $ $Rev: 837440 $")
#endif
