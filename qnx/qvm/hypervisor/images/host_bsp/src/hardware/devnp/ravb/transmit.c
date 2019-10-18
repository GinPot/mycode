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
#include <netdrvr/avb.h>

#include <bpfilter.h>
#if NBPFILTER > 0
#include <net/bpf.h>
#include <net/bpfdesc.h>
#endif

void ravb_reap_tx (ravb_dev_t *ravb, uint8_t queue)
{
    uint32_t idx;

    idx = ravb->tx_cidx[queue];
    while ((idx != ravb->tx_pidx[queue]) &&
       (ravb->tx_bd[queue][idx].die_dt == DT_FEMPTY) &&
       !(ravb->tx_bd[queue][idx].tagh_tsr & TX_TSR))
    {
        if (ravb->cfg.verbose & VERBOSE_TX)
        {
            slogf(_SLOGC_NETWORK, _SLOG_INFO,
              "ravb: Tx reap queue %d index %d", queue, idx);
        }

        m_freem(ravb->tx_pkts[queue][idx]);
        ravb->tx_pkts[queue][idx] = NULL;
        idx = (idx + 1) % NUM_TX_DESC;
    }

    ravb->tx_cidx[queue] = idx;
}

void ravb_start(struct ifnet *ifp)
{
    ravb_dev_t      *ravb;
    struct mbuf     *m;
    uint32_t        idx, next_idx;

    ravb = ifp->if_softc;

    if (((ifp->if_flags & IFF_RUNNING) == 0) || ((ravb->cfg.flags & NIC_FLAG_LINK_DOWN) != 0)) {
        /* Get rid of any stale traffic */
        IFQ_PURGE(&ifp->if_snd);
        ravb->stats.un.estats.no_carrier++;
        NW_SIGUNLOCK(&ifp->if_snd_ex, ravb->iopkt);
        return;
    }

    ifp->if_flags_tx |= IFF_OACTIVE;

    while (1)
    {
        idx = ravb->tx_pidx[0];
        next_idx = (idx + 1) % NUM_TX_DESC;

        if (next_idx == ravb->tx_cidx[0]) {
            /* Ran out of Tx descriptors, see if we can free some up */
            ravb_reap_tx(ravb, 0);
            if (next_idx == ravb->tx_cidx[0]) {
                /* Out of Tx descriptors, leave IFF_OACTIVE set */
                NW_SIGUNLOCK_P(&ifp->if_snd_ex, ravb->iopkt, WTP);
                return;
            }
        }

        IFQ_DEQUEUE(&ifp->if_snd, m);
        if (m == NULL) {
            /* Done */
            break;
        }

        ravb_tx(ravb, m, 0);
    }

    ifp->if_flags_tx &= ~IFF_OACTIVE;
    NW_SIGUNLOCK_P(&ifp->if_snd_ex, ravb->iopkt, WTP);
}

int ravb_tx(ravb_dev_t *ravb, struct mbuf *m, uint8_t queue)
{
    struct mbuf     *m2;
    uint32_t        idx, next_idx, tag, req;
    RAVB_TX_DESC    *bd;
    struct ifnet    *ifp = &ravb->ecom.ec_if;

    idx = ravb->tx_pidx[queue];
    next_idx = (idx + 1) % NUM_TX_DESC;

    /* Hardware cannot do gather DMA */
    if (m->m_next != NULL)
    {
        m2 = m_getcl(M_NOWAIT, MT_DATA, M_PKTHDR);
        if (m2 == NULL)
        {
            m_freem(m);
            ravb->stats.tx_failed_allocs++;
            ifp->if_oerrors++;
            return ENOMEM;
        }

        m_copydata(m, 0, m->m_pkthdr.len, mtod(m2, caddr_t));
        m2->m_pkthdr.len = m2->m_len = m->m_pkthdr.len;
        m_freem(m);
        m = m2;
    }
    bd = &ravb->tx_bd[queue][idx];
    ravb->tx_pkts[queue][idx] = m;
    bd->ds_tagl = m->m_len;
    bd->dptr = mbuf_phys(m);
    CACHE_FLUSH(&ravb->cachectl, m->m_data, bd->dptr,
                    m->m_len);

    if (ravb_ptp_is_eventmsg(m, NULL)) {
        /* use desc tag to store queue and idx */
        tag = ((queue << 8) | idx) & 0x3FF;
        bd->ds_tagl |= (tag << 12) & TX_TAGL;
        bd->tagh_tsr = tag >> 4;
        bd->tagh_tsr |= TX_TSR;
    }
    /*
     * Descriptor Type must be set last
     * DMAC will begin transfer if type
     * is not DT_FEMPTY
     */
    mem_barrier();
    bd->die_dt = DT_FSINGLE;

    if (ravb->cfg.verbose & VERBOSE_TX)
    {
        slogf(_SLOGC_NETWORK, _SLOG_INFO,
          "ravb: Transmit packet length %d at queue %d index %d",
          m->m_len, queue, idx);
    }

#if NBPFILTER > 0
    /* Pass the packet to any BPF listeners */
    if (ifp->if_bpf) {
        bpf_mtap(ifp->if_bpf, m);
    }
#endif
    ifp->if_opackets++;
    ravb->stats.txed_ok++;
    ravb->stats.octets_txed_ok += m->m_len;
    if (m->m_flags & M_MCAST) {
        ifp->if_omcasts++;
        ravb->stats.txed_multicast++;
    }
    if (m->m_flags & M_BCAST) {
        ifp->if_omcasts++;
        ravb->stats.txed_broadcast++;
    }

    ravb->tx_pidx[queue] = next_idx;

    /* Request transmit if not already running */
    req = TCCR_TSRQ0 << queue;
    if (!(in32(ravb->base + TCCR) & req))
        out32(ravb->base + TCCR, in32(ravb->base + TCCR) | (req));

    return EOK;
}

int ravb_output (struct ifnet *ifp, struct mbuf *m,
         struct sockaddr *dst, struct rtentry *rt)
{
    ravb_dev_t      *ravb = ifp->if_softc;
    struct m_tag    *tag;
    uint8_t         priority, queue = 0;
    int             error;
    uint32_t        idx, next_idx;

    tag = GET_TXQ_TAG(m);
    if (tag == NULL) {
        priority = 0;
    } else {
        priority = EXTRACT_TXQ_TAG(tag);
    }

    /*
     * Map the eight priorities to the 4 queues
     * According to 802.1Q-2011 Table 34-1
     */
    switch (priority) {
        case 0:
        case 1:
            /* Do a normal if_output via the stack, will be on TXQ 0 */
            return ravb->stack_output(ifp, m, dst, rt);
            break;
        case 2:
            /* Send the mbuf on TXQ 2 */
            queue = 2;
            break;
        case 3:
            /* Send the mbuf on TXQ 3 */
            queue = 3;
            break;
        default:
            /* Send the mbuf on TXQ 1 */
            queue = 1;
            break;
    }

    NW_SIGLOCK_P(&ravb->ecom.ec_if.if_snd_ex, ravb->iopkt, WTP);

    idx = ravb->tx_pidx[queue];
    next_idx = (idx + 1) % NUM_TX_DESC;

    if (next_idx == ravb->tx_cidx[queue]) {
        /* Ran out of Tx descriptors, see if we can free some up */
        ravb_reap_tx(ravb, queue);
        if (next_idx == ravb->tx_cidx[queue]) {
            /* Out of Tx descriptors, leave IFF_OACTIVE set */
            NW_SIGUNLOCK_P(&ifp->if_snd_ex, ravb->iopkt, WTP);
            return ENOBUFS;
        }
    }

    /*
     * The packet already has an ether header on it but is moved down to let
     * ether_output() do the header as well. As this is going direct, just
     * restore the current header.
     */
    m->m_data -= sizeof(struct ether_header);
    m->m_len += sizeof(struct ether_header);
    m->m_pkthdr.len += sizeof(struct ether_header);

    error = ravb_tx(ravb, m, queue);
    NW_SIGUNLOCK_P(&ravb->ecom.ec_if.if_snd_ex, ravb->iopkt, WTP);
    return error;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/devnp/ravb/transmit.c $ $Rev: 853959 $")
#endif
