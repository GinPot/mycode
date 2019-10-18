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

#ifndef _PROTO_H_INCLUDED
#define _PROTO_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <sys/neutrino.h>
#include <hw/inout.h>
#include <hw/i2c.h>
#include <drvr/hwinfo.h>
#include <hw/hwinfo_rcar.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>
#include <unistd.h>

#define	CMODE_SEND          0x00000001
#define	CMODE_RECV          0x00000002
#define	CMODE_DOSTOP        0x00000010

#define	VERBOSE_QUIET       0
#define	VERBOSE_LEVEL1      1
#define	VERBOSE_LEVEL2      2
#define	VERBOSE_LEVEL4      4
#define	VERBOSE_LEVEL8      8

#define I2C_DEFAULT_PCLK    50000000
#define I2C_DEFAULT_BAUD    50000

/* I2C Controller Specific Registers */
#define RCAR_I2C_SIZE       0x100

/* Offset from I2C base */
#define RCAR_I2C_ICSCR      0x00        // Slave Control Register
#define RCAR_I2C_ICMCR      0x04        // Master Control Register
#define RCAR_I2C_ICSSR      0x08        // Slave Status Register
#define RCAR_I2C_ICMSR      0x0C        // Master Status Register
#define RCAR_I2C_ICSIER     0x10        // Slave Interrupt Enable Register
#define RCAR_I2C_ICMIER     0x14        // Master Interrupt Enable Register
#define RCAR_I2C_ICCCR      0x18        // Clock Control Register
#define RCAR_I2C_ICSAR      0x1C        // Slave Address Register
#define RCAR_I2C_ICMAR      0x20        // Master Address Register
#define RCAR_I2C_ICRXD      0x24        // Receive Data Register
#define RCAR_I2C_ICTXD      0x24        // Transmit Data Register
#define RCAR_I2C_ICCCR2     0x28
#define RCAR_I2C_ICMPR      0x2C
#define RCAR_I2C_ICHPR      0x30
#define RCAR_I2C_ICLPR      0x34

/* ICSCR bit definition */
#define RCAR_ICSCR_SDBS     (1 << 3)
#define RCAR_ICSCR_SIE      (1 << 2)
#define RCAR_ICSCR_GCAE     (1 << 1)
#define RCAR_ICSCR_FNA      (1 << 0)

/* ICSSR bit definition */
#define RCAR_ICSSR_GCAR     (1 << 6)
#define RCAR_ICSSR_STM      (1 << 5)
#define RCAR_ICSSR_SSR      (1 << 4)
#define RCAR_ICSSR_SDE      (1 << 3)
#define RCAR_ICSSR_SDT      (1 << 2)
#define RCAR_ICSSR_SDR      (1 << 1)
#define RCAR_ICSSR_SAR      (1 << 0)

/* ICSIER bit definition */
#define RCAR_ICSIER_SSRE    (1 << 4)
#define RCAR_ICSIER_SDEE    (1 << 3)
#define RCAR_ICSIER_SDTE    (1 << 2)
#define RCAR_ICSIER_SDRE    (1 << 1)
#define RCAR_ICSIER_SARE    (1 << 0)

/* ICMCR bit definition */
#define RCAR_ICMCR_MDBS     (1 << 7)
#define RCAR_ICMCR_FSCL     (1 << 6)
#define RCAR_ICMCR_FSDA     (1 << 5)
#define RCAR_ICMCR_OBPC     (1 << 4)
#define RCAR_ICMCR_MIE      (1 << 3)
#define RCAR_ICMCR_TSBE     (1 << 2)
#define RCAR_ICMCR_FSB      (1 << 1)
#define RCAR_ICMCR_ESG      (1 << 0)

/* ICMSR bit definition */
#define RCAR_ICMSR_MNR      (1 << 6)
#define RCAR_ICMSR_MAL      (1 << 5)
#define RCAR_ICMSR_MST      (1 << 4)
#define RCAR_ICMSR_MDE      (1 << 3)
#define RCAR_ICMSR_MDT      (1 << 2)
#define RCAR_ICMSR_MDR      (1 << 1)
#define RCAR_ICMSR_MAT      (1 << 0)

/* ICMIER bit definition */
#define RCAR_ICMIER_MNRE    (1 << 6)
#define RCAR_ICMIER_MALE    (1 << 5)
#define RCAR_ICMIER_MSTE    (1 << 4)
#define RCAR_ICMIER_MDEE    (1 << 3)
#define RCAR_ICMIER_MDTE    (1 << 2)
#define RCAR_ICMIER_MDRE    (1 << 1)
#define RCAR_ICMIER_MATE    (1 << 0)

/* ICMAR bit definition */
#define RCAR_ICMAR_STM1     (1 << 0)

/* ICCCR bit definition */
#define RCAR_ICCCR_SCGD(x)  ((x) << 3)
#define RCAR_ICCCR_CDF(x)   ((x) & 0x7)

extern int verbose;

#define rcar_i2c_slogf(dev, vl, msg, ...) \
            if (vl == VERBOSE_QUIET) \
                slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, msg, ##__VA_ARGS__); \
            else if (vl <= dev->verbose) \
                slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, msg, ##__VA_ARGS__)

typedef struct {
    uintptr_t           regbase;
    unsigned            physbase;

    uint32_t            busSpeed;
    unsigned            pck;		/* peripheral clock */
    unsigned            scl_freq;
    unsigned long       scl_period;	/* in ns */

    unsigned            slave_addr;
    i2c_addrfmt_t       slave_addr_fmt;

    int                 irq;
    int                 intr;

    uint8_t             *buf;
    int                 len;
    unsigned            mode;
    unsigned            status;
    int                 verbose;

    struct sigevent     intrEvent;
} rcar_i2c_dev_t;

const struct sigevent* rcar_i2c_intr(void* arg);
int rcar_i2c_wait_complete(rcar_i2c_dev_t *dev);
int rcar_i2c_parse_options(rcar_i2c_dev_t *dev, int argc, char *argv[]);
void *rcar_i2c_init(int argc, char *argv[]);
void rcar_i2c_fini(void *hdl);
void rcar_i2c_reset(rcar_i2c_dev_t *dev);
int rcar_i2c_set_slave_addr(void *hdl, unsigned int addr, i2c_addrfmt_t fmt);
int rcar_i2c_set_bus_speed(void *hdl, unsigned int speed, unsigned int *ospeed);
int rcar_i2c_version_info(i2c_libversion_t *version);
int rcar_i2c_driver_info(void *hdl, i2c_driver_info_t *info);
i2c_status_t rcar_i2c_recv(void *hdl, void *buf, unsigned int len, unsigned int stop);
i2c_status_t rcar_i2c_send(void *hdl, void *buf, unsigned int len, unsigned int stop);

#endif /* _PROTO_H_INCLUDED */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/i2c/rcar-A/proto.h $ $Rev: 837440 $")
#endif
