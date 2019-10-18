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

#include "rcar_rpc.h"

/*
 * setup DMA transfer
 */
int rpc_setup_dma(rpc_dev_t *dev, int len, int dir, int offset)
{
    rcar_rpc_t     *rpc = dev->rpc;
    dma_transfer_t  tinfo;
    dma_addr_t      saddr, daddr;

    memset(&tinfo, 0, sizeof (tinfo));

    tinfo.xfer_unit_size = 4;
    tinfo.xfer_bytes     = len;

    if (dir == RCAR_RPC_WRITE) {
        saddr.paddr     = rpc->dbuf.paddr + offset;
        tinfo.src_addrs = &saddr;
        daddr.paddr     = rpc->buffer;
        tinfo.dst_addrs = &daddr;

        rpc->dmafuncs.setup_xfer(rpc->txdma, &tinfo);
        rpc->dmafuncs.xfer_start(rpc->txdma);
    } else {
        saddr.paddr     = RCAR_RPC_ADDR_MAP + offset;
        tinfo.src_addrs = &saddr;
        daddr.paddr     = rpc->dbuf.paddr;
        tinfo.dst_addrs = &daddr;

        rpc->dmafuncs.setup_xfer(rpc->rxdma, &tinfo);
        rpc->dmafuncs.xfer_start(rpc->rxdma);
    }

    return (len);
}

int rpc_wait(rpc_dev_t *dev, int len, int dir)
{
    rcar_rpc_t     *rpc = dev->rpc;
    struct _pulse   pulse;
    uint64_t        to = len;

    to *= 2048 * 2048;

    while (1) {
       TimerTimeout(CLOCK_REALTIME, _NTO_TIMEOUT_RECEIVE, NULL, &to, NULL);

        if (MsgReceivePulse(rpc->chid, &pulse, sizeof(pulse), NULL) == -1) {
            fprintf(stderr, "RCAR_RPC: XFER error! sts = %#x\n", in32(rpc->vbase + RCAR_RPC_CMNSR));
            return (errno = EIO);
        }

        switch (pulse.code) {
            case RCAR_RPC_RDMA_EVENT:
                if (rpc->dmafuncs.bytes_left(rpc->rxdma) != 0) {
                    fprintf(stderr, "RCAR_RPC: Rx DMA is not completed properly\n");
                    return (EIO);
                }
                rpc->dmafuncs.xfer_complete(rpc->rxdma);
                if (dir == RCAR_RPC_READ)
                    return (EOK);
                break;
            case RCAR_RPC_TDMA_EVENT:
                if (rpc->dmafuncs.bytes_left(rpc->txdma) != 0) {
                    fprintf(stderr, "RCAR_RPC: Tx DMA is not completed properly\n");
                    return (EIO);
                }
                rpc->dmafuncs.xfer_complete(rpc->txdma);
                if (dir == RCAR_RPC_WRITE)
                    return (EOK);
                break;
        }
    }
}

/*
 * DMA transfer
 */
int rpc_dma_xfer(rpc_dev_t *dev, int len, int dir)
{
    rcar_rpc_t *rpc = dev->rpc;
    int         rc;

    if ((rc = rpc_wait(dev, len, dir)) != EOK)
        rpc->dmafuncs.xfer_abort(rpc->rxdma);

    return (rc);
}

int rcar_dma_init(rcar_rpc_t *rpc)
{
    struct sigevent     event;

    if (get_dmafuncs(&rpc->dmafuncs, sizeof(rpc->dmafuncs)) == -1) {
        fprintf(stderr, "rpc: init_dma: failed to get DMA lib functions\n");
        return (-1);
    }

    rpc->dmafuncs.init(NULL);

    event.sigev_notify   = SIGEV_PULSE;
    event.sigev_coid     = rpc->coid;
    event.sigev_code     = RCAR_RPC_TDMA_EVENT;
    event.sigev_priority = RCAR_RPC_PRIORITY;

    if ((rpc->txdma = rpc->dmafuncs.channel_attach("dma=sys", &event, NULL, 0,
                                    DMA_ATTACH_PRIORITY_STRICT | DMA_ATTACH_ANY_CHANNEL | DMA_ATTACH_EVENT_ON_COMPLETE)) == NULL) {
        fprintf(stderr, "rpc: Unable to attach to DMA Channel");
        return (-1);
    }

    if (rpc->dmafuncs.alloc_buffer(rpc->txdma, &rpc->dbuf, RCAR_DMABUF_SIZE, DMA_BUF_FLAG_NOCACHE) != 0) {
        fprintf(stderr, "rpc: Unable to allocate DMA buffer");
        return (-1);
    }

    event.sigev_notify   = SIGEV_PULSE;
    event.sigev_coid     = rpc->coid;
    event.sigev_code     = RCAR_RPC_RDMA_EVENT;
    event.sigev_priority = RCAR_RPC_PRIORITY;

    if ((rpc->rxdma = rpc->dmafuncs.channel_attach("dma=sys", &event, NULL, 1,
                                    DMA_ATTACH_PRIORITY_STRICT | DMA_ATTACH_ANY_CHANNEL | DMA_ATTACH_EVENT_ON_COMPLETE)) == NULL) {
        fprintf(stderr, "rpc: Unable to attach to DMA Channel");
        return (-1);
    }

    return (EOK);
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/flash/boards/rcar_qspi/gen3/rcar_rpc_dma.c $ $Rev: 851112 $")
#endif
