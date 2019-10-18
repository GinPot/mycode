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

#include "ravb.h"

#include <bpfilter.h>
#if NBPFILTER > 0
#include <net/bpf.h>
#include <net/bpfdesc.h>
#endif

#define IS_BROADCAST(dptr) \
    ((dptr)[0] == 0xff && (dptr)[1] == 0xff && \
    (dptr)[2] == 0xff && (dptr)[3] == 0xff && \
    (dptr)[4] == 0xff && (dptr)[5] == 0xff)

void ravb_receive (ravb_dev_t *ravb, struct nw_work_thread *wtp, uint8_t queue)
{
    struct mbuf     *m, *m2;
    struct ifnet    *ifp;
    uint32_t        idx;
    uint8_t         *dptr = 0;
    ptpv2hdr_t      *ph;
    RAVB_RX_DESC    *bd;

    ifp = &ravb->ecom.ec_if;

    ravb->pkts_received = 1;

    for (;;)
    {
        idx = ravb->rx_idx[queue];
        bd = &ravb->rx_bd[queue][idx];

        if(bd->die_dt == DT_FEMPTY) {
            /* If it was out of Rx descriptors and stopped, restart it */
            return;
        }

        if (bd->msc & MSC_MC)
        {
            ravb->stats.rxed_multicast++;
        }

        if (bd->msc & (MSC_CRC | MSC_RFE | MSC_RTSF | MSC_RTLF | MSC_CEEF))
        {
            bd->die_dt = DT_FEMPTY;
            ifp->if_ierrors++;
            ravb->rx_idx[queue] = (idx + 1) % NUM_RX_DESC;
            continue;
        }

        m2 = m_getcl_wtp(M_DONTWAIT, MT_DATA, M_PKTHDR, wtp);
        if (m2 == NULL) {
            /* Failed to get new mbuf, return the old one */
            slogf(_SLOGC_NETWORK, _SLOG_ERROR,
              "ravb: Rx queue %d index %d failed to retrieve new mbuf", queue, idx);
            bd->die_dt = DT_FEMPTY;
            ravb->stats.rx_failed_allocs++;
            ifp->if_ierrors++;
            ravb->rx_idx[queue] = (idx + 1) % NUM_RX_DESC;
            continue;
        }

        m = ravb->rx_pkts[queue][idx];
        m->m_pkthdr.len = m->m_len = bd->ds_cc & RX_DS;
        m->m_pkthdr.rcvif = ifp;
        CACHE_INVAL(&ravb->cachectl, ravb->rx_pkts[queue][idx]->m_data,
                bd->dptr, m->m_len);

        ravb->rx_pkts[queue][idx] = m2;
        bd->dptr = mbuf_phys(m2);
        bd->ds_cc = ALIGN(PKT_BUF_SZ, DPTR_ALIGN);
        bd->die_dt = DT_FEMPTY;
        CACHE_FLUSH(&ravb->cachectl, m2->m_data, bd->dptr,
                        m2->m_ext.ext_size);

        if (ravb->cfg.verbose & VERBOSE_RX)
        {
            slogf(_SLOGC_NETWORK, _SLOG_INFO,
              "ravb: Receive packet length %d at queue %d index %d",
              m->m_len, queue, idx);
        }

        if (ravb_ptp_is_eventmsg(m, &ph)) {
            ravb_ptp_add_rx_timestamp(ravb, ph, bd);
        }

#if NBPFILTER > 0
        /* Pass this up to any BPF listeners. */
        if (ifp->if_bpf) {
            bpf_mtap(ifp->if_bpf, m);
        }
#endif

        /* Pass the packet in to the stack */
        ifp->if_ipackets++;
        ravb->stats.rxed_ok++;
        ravb->stats.octets_rxed_ok += m->m_len;
        dptr = mtod (m, uint8_t *);
        if(IS_BROADCAST(dptr))
            ravb->stats.rxed_broadcast++;
        (*ifp->if_input)(ifp, m);

        ravb->rx_idx[queue] = (idx + 1) % NUM_RX_DESC;
    }
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/devnp/ravb/receive.c $ $Rev: 853959 $")
#endif
