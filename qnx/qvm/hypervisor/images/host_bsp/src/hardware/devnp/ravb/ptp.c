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

#include "ravb.h"
#include <netinet/ip.h>
#include <netinet/udp.h>

#define NSEC_PER_SEC        1000000000
#define PTP_INCR            0x780000 /* 1/(3.2 GHz/24)*1E9*2^20 */

ptp_extts_t rx_ts[RAVB_RX_TIMESTAMP_BUF_SZ];
uint32_t    rx_ts_cnt;
ptp_extts_t tx_ts[RAVB_TX_TIMESTAMP_BUF_SZ];
uint32_t    tx_ts_cnt;

void ravb_ptp_init(ravb_dev_t *ravb)
{
    nic_mutex_lock(&ravb->ptp_mutex);

    /* Default offset and increment */
    out32(ravb->base + GTI, PTP_INCR & GTI_TIV);
    out32(ravb->base + GTO0, 0);
    out32(ravb->base + GTO1, 0);
    out32(ravb->base + GTO2, 0);
    out32(ravb->base + GCCR, in32(ravb->base + GCCR) | GCCR_LTI | GCCR_LTO);

    /* Capture adjusted gPTP timer timestamps */
    ravb_wait(ravb, GCCR, GCCR_TCR_M, GCCR_TCR_NOP);
    out32(ravb->base + GCCR, (in32(ravb->base + GCCR) & ~(GCCR_TCSS_M)) | GCCR_TCSS_APTP);

    nic_mutex_unlock(&ravb->ptp_mutex);
}

int ravb_ptp_request(ravb_dev_t *ravb, uint32_t request)
{
    int error;

    error = ravb_wait(ravb, GCCR, GCCR_TCR_M, GCCR_TCR_NOP);
    if(error){
        return error;
    }

    out32(ravb->base + GCCR, (in32(ravb->base + GCCR) & ~GCCR_TCR_M) | request);
    return ravb_wait(ravb, GCCR, GCCR_TCR_M, GCCR_TCR_NOP);
}

int ravb_ptp_is_eventmsg(struct mbuf *m, ptpv2hdr_t **ph)
{
    int                 remain, pktlen;
    struct ether_header *eh;
    struct ip           *iph;
    struct udphdr       *udph;

    if (m == NULL) {
        slogf(_SLOGC_NETWORK, _SLOG_ERROR, "%s: NULL mbuf", __FUNCTION__);
        return 0;
    }

    if (m->m_pkthdr.len <
        sizeof(struct ether_header) + sizeof(ptpv2hdr_t)) {
        return 0;
    }

    eh = (struct ether_header *)m->m_data;

    switch(ntohs(eh->ether_type)) {
        case ETHERTYPE_PTP:
            /* This is a native ethernet frame
             * defined for IEEE1588v2
             */
            remain = sizeof(struct ether_header);
            while (remain >= m->m_len) {
                remain -= m->m_len;
                m = m->m_next;
            }
            if ((m->m_len - remain) < sizeof(ptpv2hdr_t)) {
                return 0;
            }
            if (ph != NULL) {
                *ph = (ptpv2hdr_t *)(m->m_data + remain);
            }
            if((((ptpv2hdr_t *)(m->m_data + remain))->messageId & 0x0f)>7) {
                return 0;
            }
            return 1;
            break;
        case ETHERTYPE_IP:
            pktlen = m->m_pkthdr.len;

            remain = sizeof(struct ether_header);
            while (remain >= m->m_len) {
                remain -= m->m_len;
                m = m->m_next;
            }
            iph = (struct ip*)(m->m_data + remain);

            if (iph->ip_p != IPPROTO_UDP) {
                return 0;
            }

            if (pktlen < sizeof(struct ether_header) + (iph->ip_hl << 2) +
                sizeof(struct udphdr) + sizeof (ptpv2hdr_t)) {
                return 0;
            }

            remain += iph->ip_hl << 2;
            while (remain >= m->m_len) {
                remain -= m->m_len;
                m = m->m_next;
            }
            udph = (struct udphdr*)(m->m_data + remain);

            if (ntohs(udph->uh_dport) != PTP_UDP_PORT) {
                return 0;
            }

            remain += sizeof(struct udphdr);
            while (remain >= m->m_len) {
                remain -= m->m_len;
                m = m->m_next;
            }
            if ((m->m_len - remain) < sizeof(ptpv2hdr_t)) {
                return 0;
            }
            if (ph != NULL) {
                *ph = (ptpv2hdr_t*)(m->m_data + remain);
            }
            return 1;
            break;
        default:
            // VLAN & IPv6 protocols are not supported
            break;
    }
    return 0;
}

void ravb_ptp_add_rx_timestamp(ravb_dev_t *ravb, ptpv2hdr_t *ph, RAVB_RX_DESC *bd)
{
    if ((ph == NULL) || (bd == NULL) ||
        ((ph->version & 0x0f) != 0x2) ) {
        /* Only PTPv2 currently supported */
        return;
    }

    /* Add the details */
    rx_ts[rx_ts_cnt].msg_type = ph->messageId & 0x0f;
    rx_ts[rx_ts_cnt].sequence_id = ntohs(ph->sequenceId);
    memcpy(rx_ts[rx_ts_cnt].clock_identity, ph->clockIdentity,
       sizeof(rx_ts[rx_ts_cnt].clock_identity));
    rx_ts[rx_ts_cnt].sport_id = ntohs(ph->sportId);
    rx_ts[rx_ts_cnt].ts.nsec = bd->ts_n;
    rx_ts[rx_ts_cnt].ts.sec = bd->ts_sl;

    if(bd->ts_sh){
        slogf(_SLOGC_NETWORK, _SLOG_ERROR, "%s upper seconds detected", __FUNCTION__);
    }

    if(ravb->cfg.verbose & VERBOSE_PTP)
        slogf(_SLOGC_NETWORK, _SLOG_INFO, "%s msgid=0x%x sequence_id=%u ts=%u.%u", __FUNCTION__, rx_ts[rx_ts_cnt].msg_type, rx_ts[rx_ts_cnt].sequence_id, rx_ts[rx_ts_cnt].ts.sec, rx_ts[rx_ts_cnt].ts.nsec);

    /* Advance the counter including wrapping */
    rx_ts_cnt = (rx_ts_cnt + 1) % RAVB_RX_TIMESTAMP_BUF_SZ;
}

void ravb_ptp_add_tx_timestamp(ravb_dev_t *ravb)
{
    uint32_t    count, tag, idx, queue;
    ptpv2hdr_t  *ph;

    while((count = ((in32(ravb->base + TSR) & TSR_TFFL) >> 8)) > 0) {
        tag = (in32(ravb->base + TFA2) >> 16) & 0x3FF;
        queue = tag >> 8;
        idx = tag & 0xFF;

        /* Check tx desc for transfer complete */
        if(ravb->tx_bd[queue][idx].die_dt != DT_FEMPTY){
            slogf(_SLOGC_NETWORK, _SLOG_ERROR, "%s queue=%d idx=%d die_dt=0x%x error tx not complete/desc overwritten", __FUNCTION__, queue, idx, ravb->tx_bd[queue][idx].die_dt);
        } else if(ravb->tx_pkts[queue][idx] == NULL) {
            slogf(_SLOGC_NETWORK, _SLOG_ERROR, "%s queue=%d idx=%d tx_pkts empty", __FUNCTION__, queue, idx);
        } else {
            /* Only add timestamp for supported eventmsgs */
            if(ravb_ptp_is_eventmsg(ravb->tx_pkts[queue][idx], &ph) && (ph != NULL) && ((ph->version & 0x0f) == 0x2)){
                /* Add the details */
                tx_ts[tx_ts_cnt].msg_type = ph->messageId & 0x0f;
                tx_ts[tx_ts_cnt].sequence_id = ntohs(ph->sequenceId);
                memcpy(tx_ts[tx_ts_cnt].clock_identity, ph->clockIdentity,
                   sizeof(tx_ts[tx_ts_cnt].clock_identity));
                tx_ts[tx_ts_cnt].sport_id = ntohs(ph->sportId);
                tx_ts[tx_ts_cnt].ts.nsec = in32(ravb->base + TFA0);
                tx_ts[tx_ts_cnt].ts.sec = in32(ravb->base + TFA1);
                if(in32(ravb->base + TFA2) & 0xFF){
                    slogf(_SLOGC_NETWORK, _SLOG_ERROR, "%s upper seconds detected", __FUNCTION__);
                }

                if(ravb->cfg.verbose & VERBOSE_PTP)
                    slogf(_SLOGC_NETWORK, _SLOG_INFO, "%s queue=%d idx=%d msgid=0x%x sequence_id=%u ts=%u.%u", __FUNCTION__, queue, idx, tx_ts[tx_ts_cnt].msg_type, tx_ts[tx_ts_cnt].sequence_id, tx_ts[tx_ts_cnt].ts.sec, tx_ts[tx_ts_cnt].ts.nsec);

                /* Advance the counter including wrapping */
                tx_ts_cnt = (tx_ts_cnt + 1) % RAVB_TX_TIMESTAMP_BUF_SZ;
            }
            /* Clear timestamp request flag */
            ravb->tx_bd[queue][idx].tagh_tsr &= ~TX_TSR;
        }
        /* release timestamp entry */
        out32(ravb->base + TCCR, in32(ravb->base + TCCR) | TCCR_TFR);
    }
}

int ravb_ptp_get_rx_timestamp (ravb_dev_t *ravb, ptp_extts_t *ts)
{
    int i;

    if (ts == NULL) {
        return 0;
    }

    for (i = 0; i < RAVB_RX_TIMESTAMP_BUF_SZ; i++) {
        if ((ts->msg_type == rx_ts[i].msg_type) &&
            (ts->sequence_id == rx_ts[i].sequence_id) &&
            (ts->sport_id == rx_ts[i].sport_id) &&
            !memcmp(ts->clock_identity, rx_ts[i].clock_identity,
            sizeof(ts->clock_identity))) {
            ts->ts.nsec =  rx_ts[i].ts.nsec;
            ts->ts.sec =  rx_ts[i].ts.sec;

            if(ravb->cfg.verbose & VERBOSE_PTP)
                slogf(_SLOGC_NETWORK, _SLOG_INFO, "%s msgid=0x%x sequence_id=%u ts=%u.%u", __FUNCTION__, ts->msg_type, ts->sequence_id, ts->ts.sec, ts->ts.nsec);
            return 1;
        }
    }
    return 0;
}

int ravb_ptp_get_tx_timestamp (ravb_dev_t *ravb, ptp_extts_t *ts)
{
    int i;

    if (ts == NULL) {
        return 0;
    }

    for (i = 0; i < RAVB_TX_TIMESTAMP_BUF_SZ; i++) {
        if ((ts->msg_type == tx_ts[i].msg_type) &&
            (ts->sequence_id == tx_ts[i].sequence_id) &&
            (ts->sport_id == tx_ts[i].sport_id) &&
            !memcmp(ts->clock_identity, tx_ts[i].clock_identity,
                sizeof(ts->clock_identity))) {
            ts->ts.nsec = tx_ts[i].ts.nsec;
            ts->ts.sec = tx_ts[i].ts.sec;
            if(ravb->cfg.verbose & VERBOSE_PTP)
                slogf(_SLOGC_NETWORK, _SLOG_INFO, "%s msgid=0x%x sequence_id=%u ts=%u.%u", __FUNCTION__, ts->msg_type, ts->sequence_id, ts->ts.sec, ts->ts.nsec);
            return 1;
        }
    }
    return 0;
}

void ravb_ptp_get_cnt(ravb_dev_t *ravb, ptp_time_t *cnt)
{
    int error;

    if(cnt != NULL) {
        /* Request timestamp capture */
        error = ravb_ptp_request(ravb, GCCR_TCR_CAPTURE);
        if(error){
            return;
        }
        /* Get captured timestamp */
        cnt->nsec = in32(ravb->base + GCT0);
        cnt->sec = in32(ravb->base + GCT1);

        if(ravb->cfg.verbose & VERBOSE_PTP)
            slogf(_SLOGC_NETWORK, _SLOG_INFO, "%s: ts=%u.%u", __FUNCTION__, cnt->sec, cnt->nsec);
    }
}

void ravb_ptp_set_cnt(ravb_dev_t *ravb, ptp_time_t *cnt)
{
    int error;
    uint32_t gccr;

    /* Request timestamp reset */
    error = ravb_ptp_request(ravb, GCCR_TCR_RESET);
    if(error){
        return;
    }
    /* Set timestamp offset */
    gccr = in32(ravb->base + GCCR);
    if(gccr & GCCR_LTO){
        return;
    }
    out32(ravb->base + GTO0, cnt->nsec);
    out32(ravb->base + GTO1, cnt->sec);
    out32(ravb->base + GCCR, gccr | GCCR_LTO);

    if(ravb->cfg.verbose & VERBOSE_PTP)
        slogf(_SLOGC_NETWORK, _SLOG_INFO, "%s: ts=%u.%u", __FUNCTION__, cnt->sec, cnt->nsec);
}

void ravb_ptp_set_compensation(ravb_dev_t *ravb, ptp_comp_t ptc)
{
    uint32_t deviation, comp, gccr;

    if(ptc.comp == 0) {
        /* Reset increment to default */
        comp = PTP_INCR;
        deviation = 0;
    } else {
        comp = PTP_INCR;
        deviation = (uint64_t)comp * ptc.comp / NSEC_PER_SEC;
        if (ptc.positive) {
            comp += deviation;
        } else {
            comp -= deviation;
        }
    }
    nic_mutex_lock(&ravb->ptp_mutex);
    gccr = in32(ravb->base + GCCR);
    if(gccr & GCCR_LTI){
        nic_mutex_unlock(&ravb->ptp_mutex);
        return;
    }
    out32(ravb->base + GTI, comp & GTI_TIV);
    out32(ravb->base + GCCR, gccr | GCCR_LTI);
    nic_mutex_unlock(&ravb->ptp_mutex);

    if(ravb->cfg.verbose & VERBOSE_PTP)
        slogf(_SLOGC_NETWORK, _SLOG_INFO, "%s ppb=%u dir=%u deviation=%u current_gti=%u", __FUNCTION__, ptc.comp, ptc.positive, deviation, comp);
}

int ravb_ptp_ioctl(ravb_dev_t *ravb, struct ifdrv *ifd)
{
    ptp_time_t      time;
    ptp_comp_t      comp;
    ptp_extts_t     ts;
    uint8_t     found;

    if (ifd != NULL) {
        switch(ifd->ifd_cmd) {

            case PTP_GET_RX_TIMESTAMP:
            case PTP_GET_TX_TIMESTAMP:
                if (ifd->ifd_len != sizeof(ts)) {
                    return EINVAL;
                }

                if (ISSTACK) {
                    if (copyin((((uint8_t *)ifd) + sizeof(*ifd)),
                           &ts, sizeof(ts))) {
                        return EINVAL;
                    }
                } else {
                    memcpy(&ts, (((uint8_t *)ifd) + sizeof(*ifd)), sizeof(ts));
                }

                if (ifd->ifd_cmd == PTP_GET_RX_TIMESTAMP) {
                    found = ravb_ptp_get_rx_timestamp(ravb, &ts);
                } else {
                    found = ravb_ptp_get_tx_timestamp(ravb, &ts);
                }

                if (found) {
                    if (ISSTACK) {
                        return (copyout(&ts, (((uint8_t *)ifd) + sizeof(*ifd)),
                                sizeof(ts)));
                    } else {
                        memcpy((((uint8_t *)ifd) + sizeof(*ifd)), &ts, sizeof(ts));
                        return EOK;
                    }
                }
                return ENOENT;
                break;

        case PTP_GET_TIME:
            if (ifd->ifd_len != sizeof(time)) {
                return EINVAL;
            }
            nic_mutex_lock(&ravb->ptp_mutex);
            ravb_ptp_get_cnt(ravb, &time);
            nic_mutex_unlock(&ravb->ptp_mutex);
            if (ISSTACK) {
                return (copyout(&time, (((uint8_t *)ifd) + sizeof(*ifd)),
                        sizeof(time)));
            } else {
                memcpy((((uint8_t *)ifd) + sizeof(*ifd)), &time, sizeof(time));
                return EOK;
            }
            break;

        case PTP_SET_TIME:
            if (ifd->ifd_len != sizeof(time)) {
                return EINVAL;
            }
            if (ISSTACK) {
                if (copyin((((uint8_t *)ifd) + sizeof(*ifd)),
                       &time, sizeof(time))) {
                    return EINVAL;
                }
            } else {
                memcpy(&time, (((uint8_t *)ifd) + sizeof(*ifd)), sizeof(time));
            }
            nic_mutex_lock(&ravb->ptp_mutex);
            ravb_ptp_set_cnt(ravb, &time);
            nic_mutex_unlock(&ravb->ptp_mutex);
            /* Clock has changed so all old ts are invalid */
            memset(rx_ts, 0, sizeof(rx_ts));
            memset(tx_ts, 0, sizeof(tx_ts));
            return EOK;
            break;

        case PTP_SET_COMPENSATION:
            if (ifd->ifd_len != sizeof(comp)) {
            return EINVAL;
            }
            if (ISSTACK) {
                if (copyin((((uint8_t *)ifd) + sizeof(*ifd)),
                       &comp, sizeof(comp))) {
                    return EINVAL;
                }
            } else {
                memcpy(&comp, (((uint8_t *)ifd) + sizeof(*ifd)), sizeof(comp));
            }
            ravb_ptp_set_compensation(ravb, comp);
            return EOK;
            break;

        case PTP_GET_COMPENSATION:
            return ENOTTY;

        default:
            break;
        }
    }
    return ENOTTY;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/devnp/ravb/ptp.c $ $Rev: 837133 $")
#endif