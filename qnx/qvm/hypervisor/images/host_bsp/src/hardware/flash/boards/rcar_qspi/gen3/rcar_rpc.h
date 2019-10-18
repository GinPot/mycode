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

#ifndef _RCAR_RPC_H
#define _RCAR_RPC_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/neutrino.h>
#include <hw/inout.h>
#include <hw/dma.h>
#include <drvr/hwinfo.h>
#include <hw/hwinfo_rcar.h>

#include <f3s_spi.h>

#define RCAR_MAX_DEVICE         4

#define RCAR_RPC_WRITE          0
#define RCAR_RPC_READ           1

#define RCAR_RPC_ADDR_MAP       0x08000000

#define RCAR_RPC_EVENT          0x55
#define RCAR_RPC_RDMA_EVENT     0x56
#define RCAR_RPC_TDMA_EVENT     0x57
#define RCAR_RPC_PRIORITY       21

#define RCAR_TxDMA_THRESHOLD    16
#define RCAR_ALIGN_RxBUF(clen)  (4 - (clen & 3))

/* RPC registers, offset from RCAR_RPC_BASE (pbase) */
#define RCAR_RPC_CMNCR          0x00    /* Common control register */
#define RCAR_RPC_SSLDR          0x04    /* SSL delay register  */
#define RCAR_RPC_DRCR           0x0C    /* Data read control register */
#define RCAR_RPC_DRCMR          0x10    /* Data read command setting register */
#define RCAR_RPC_DREAR          0x14    /* Data read extended address setting register */
#define RCAR_RPC_DROPR          0x18    /* Data read option setting register */
#define RCAR_RPC_DRENR          0x1C    /* Data read enable setting register */
#define RCAR_RPC_SMCR           0x20    /* Manual mode control register */
#define RCAR_RPC_SMCMR          0x24    /* Manual mode command setting register */
#define RCAR_RPC_SMADR          0x28    /* Manual mode address setting register */
#define RCAR_RPC_SMOPR          0x2C    /* Manual mode option setting register */
#define RCAR_RPC_SMENR          0x30    /* Manual mode enable setting register */
#define RCAR_RPC_SMRDR0         0x38    /* Manual mode read data register 0 */
#define RCAR_RPC_SMRDR1         0x3C    /* Manual mode read data register 1 */
#define RCAR_RPC_SMWDR0         0x40    /* Manual mode write data register 0 */
#define RCAR_RPC_SMWDR1         0x44    /* Manual mode write data register 1 */
#define RCAR_RPC_CMNSR          0x48    /* Common status register */
#define RCAR_RPC_DRDMCR         0x58    /* Data read dummy cycle setting register */
#define RCAR_RPC_DRDRENR        0x5C    /* Data read DDR enable register */
#define RCAR_RPC_SMDMCR         0x60    /* Manual mode dummy cycle setting register */
#define RCAR_RPC_SMDRENR        0x64    /* Manual mode DDR enable register */
#define RCAR_RPC_PHYCNT         0x7C    /* PHY control register */
#define RCAR_RPC_OFFSET1        0x80    /*  */
#define RCAR_RPC_PHYINT         0x88    /* PHY interrupt register */
#define RCAR_RPC_DIV_REG        0xA8    /* PHY divider register */

/*CMNCR bit definition  */

#define RCAR_RPC_CMNCR_BSZ(x)        (x << 0)
#define RCAR_RPC_CMNCR_IO0FV(x)      (x << 8)
#define RCAR_RPC_CMNCR_IO2FV(x)      (x << 12)
#define RCAR_RPC_CMNCR_IO3FV(x)      (x << 14)
#define RCAR_RPC_CMNCR_MOIIO0(x)     (x << 16)
#define RCAR_RPC_CMNCR_MOIIO1(x)     (x << 18)
#define RCAR_RPC_CMNCR_MOIIO2(x)     (x << 20)
#define RCAR_RPC_CMNCR_MOIIO3(x)     (x << 22)
#define RCAR_RPC_CMNCR_MD            (1 << 31)

/* SSLDR bit definition  */
#define RCAR_RPC_SSLDR_SCKDL(x)      (x << 0)
#define RCAR_RPC_SSLDR_SLNDL(x)      (x << 8)
#define RCAR_RPC_SSLDR_SPNDL(x)      (x << 16)

/* DRCR bit definition*/

#define RCAR_RPC_DRCR_SSLE(x)         (x << 0)
#define RCAR_RPC_DRCR_RBE             (1 << 8)
#define RCAR_RPC_DRCR_RCF             (1 << 9)
#define RCAR_RPC_DRCR_RPURST(x)       (x << 16)
#define RCAR_RPC_DRCR_SSLN(x)         (x << 24)

/* DRENR bit definition */

#define RCAR_RPC_DRENR_OPDE(x)        (x << 4)
#define RCAR_RPC_DRENR_ADE(x)         (x << 8)
#define RCAR_RPC_DRENR_OCDE1          (1 << 12)
#define RCAR_RPC_DRENR_CDE            (1 << 14)
#define RCAR_RPC_DRENR_DME(x)         (x << 15)
#define RCAR_RPC_DRENR_DRDB(x)        (x << 16)
#define RCAR_RPC_DRENR_OPDP(x)        (x << 20)
#define RCAR_RPC_DRENR_ADB(x)         (x << 24)
#define RCAR_RPC_DRENR_OCDB(x)        (x << 28)
#define RCAR_RPC_DRENR_CDB(x)         (x << 30)

/* SMCR bit definition */

#define RCAR_RPC_CMNCR_SPIE           (1 << 0)
#define RCAR_RPC_CMNCR_SPIWE          (1 << 1)
#define RCAR_RPC_CMNCR_SPIRE          (1 << 2)
#define RCAR_RPC_CMNCR_SSLKP          (1 << 8)

/*SMENR bit definition */

#define RCAR_RPC_SMDRENR_SPIDE(x)     (x << 0)
#define RCAR_RPC_SMDRENR_OPDE(x)      (x << 4)
#define RCAR_RPC_SMDRENR_ADE(x)       (x << 8)
#define RCAR_RPC_SMDRENR_OCDE         (1 << 12)
#define RCAR_RPC_SMDRENR_CDE          (1 << 14)
#define RCAR_RPC_SMDRENR_DME          (1 << 15)
#define RCAR_RPC_SMDRENR_SPIDB(x)     (x << 16)
#define RCAR_RPC_SMDRENR_OPDB(x)      (x << 20)
#define RCAR_RPC_SMDRENR_ADB(x)       (x << 24)
#define RCAR_RPC_SMDRENR_OCDB(x)      (x << 28)
#define RCAR_RPC_SMDRENR_CDB(x)       (x << 30)

#define RCAR_RPC_SINGLE_8BIT          0x8
#define RCAR_RPC_SINGLE_16BIT         0xC
#define RCAR_RPC_SINGLE_32BIT         0xF

/* CMNSR bit definition*/

#define RCAR_RPC_CMNSR_TEND           (1 << 0)
#define RCAR_RPC_CMNSR_SSLF           (1 << 1)

/* DRDMCR  bit definition */

#define RCAR_RPC_DRDMCR_DMCYC(x)      (x << 0)
#define RCAR_RPC_DRDMCR_DMDB(x)       (x << 16)

/*DRDRENR bit definition */

#define RCAR_RPC_DRDRENR_DRDRE(x)     (x << 0)
#define RCAR_RPC_DRDRENR_OPDRE(x)     (x << 4)
#define RCAR_RPC_DRDRENR_ADDRE(x)     (x << 8)
#define RCAR_RPC_DRDRENR_HYPE(x)      (x << 12)

/* SMDMCR bit definition */

#define RCAR_RPC_SMDMCR_DCMYC(x)      (x << 0)
#define RCAR_RPC_SMDMCR_DMDB(x)       (x << 16)

/* SMDRENR bit definition*/

#define RCAR_RPC_SMDRENR_SPIDRE(x)    (x << 0)
#define RCAR_RPC_SMDRENR_OPDRE(x)     (x << 4)
#define RCAR_RPC_SMDRENR_ADDRE(x)     (x << 8)
#define RCAR_RPC_SMDRENR_HYPE(x)      (x << 12)

/*PHYCNT bit definition */

#define RCAR_RPC_PHYCNT_PHYMEM(x)     (x << 0)
#define RCAR_RPC_PHYCNT_WBUF          (1 << 2)
#define RCAR_RPC_PHYCNT_WBUF2         (1 << 4)
#define RCAR_RPC_PHYCNT_HS            (1 << 18)
//#define RCAR_RPC_PHYCNT_OCTA         (1 << 20)
#define RCAR_RPC_PHYCNT_EXDS          (1 << 21)
#define RCAR_RPC_PHYCNT_OCTA(x)       (x << 22)
#define RCAR_RPC_PHYCNT_CAL           (1 << 31)

/* PHYINT BIT Definitions */
#define RCAR_RPC_PHYINT_RSTEN         (1<<18)         /* RPC_RESET# Pin Enable */
#define RCAR_RPC_PHYINT_WPEN          (1<<17)         /* RPC_WP# Pin Enable */
#define RCAR_RPC_PHYINT_INTEN         (1<<16)         /* RPC_INT# Pin Enable */
#define RCAR_RPC_PHYINT_RSTVAL        (1<<2)         /* RPC_RESET# Pin Output Value */
#define RCAR_RPC_PHYINT_WPVAL         (1<<1)         /* RPC_WP# Pin Output Value */
#define RCAR_RPC_PHYINT_INT           (1<<0)         /* Interrupt Status */

/* SPI controller specific */
typedef struct {
    unsigned        pbase;
    unsigned        size;
    uintptr_t       vbase;
    unsigned        buffer;
    unsigned        buffer_size;
    int             chid, coid;
    int             irq, iid;
    uint32_t        ctrl;
    uint32_t        clock;
    uint32_t        drate;
    int             dtime;
    struct sigevent spievent;

    dma_functions_t dmafuncs;
    void            *txdma;
    dma_addr_t      dbuf;
    void            *rxdma;
#define RCAR_DMABUF_SIZE        (16 * 1024)     /* DMA buffer size, set to F3S_DATA_MAX */
#define RCAR_TXDMA_OFF          ( 4 * 1024)     /* Tx DMA buffer offset */

    pthread_mutex_t mutex;
    int             ndev;
} rcar_rpc_t;

/* device specific structure */
typedef struct {
    spi_flash_t     spic;
    uint32_t        mode;
    uint32_t        drate;
    uint32_t        csel;
    uint32_t        bus;
    uint32_t        flags;
#define RCAR_FLAG_MUTEX         (1 << 0)
    rcar_rpc_t      *rpc;
} rpc_dev_t;

int rpc_setup_dma(rpc_dev_t *dev, int len, int dir, int offset);
int rpc_dma_xfer(rpc_dev_t *dev, int len, int dir);
int rcar_dma_init(rcar_rpc_t *rpc);

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/flash/boards/rcar_qspi/gen3/rcar_rpc.h $ $Rev: 853179 $")
#endif
