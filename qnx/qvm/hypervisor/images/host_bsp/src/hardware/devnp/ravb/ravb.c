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

#include <fcntl.h>
#include <drvr/hwinfo.h>
#include <net/ifdrvcom.h>
#include <sys/sockio.h>

static void ravb_set_mac (ravb_dev_t *ravb);
static void ravb_cleanup (ravb_dev_t *ravb);

int ravb_wait(ravb_dev_t *ravb, int reg, uint32_t mask, uint32_t value)
{
    int i;

    for (i = 0; i < 10000; i++) {
        if ((in32(ravb->base + reg) & mask) == value)
            return EOK;
        nic_delay(10);
    }
    return -ETIMEDOUT;
}

int ravb_operate(ravb_dev_t *ravb)
{
    uint32_t queue;

    if ((in32(ravb->base + CCC) & CCC_OPC_OPERATION) != CCC_OPC_OPERATION) {
        /*
         * When opmode is changed to operation
         * CDAR gets descriptor from DBAT, which stores the head of descriptor queue
         */
        for (queue = 0; queue < NUM_TX_QUEUES; queue++)
        {
            ravb->tx_pidx[queue] = 0;
            ravb->tx_cidx[queue] = 0;
        }
        for (queue = 0; queue < NUM_RX_QUEUES; queue++)
        {
            ravb->rx_idx[queue] = 0;
        }

        out32(ravb->base + CCC, (in32(ravb->base + CCC) & ~CCC_OPC) | CCC_OPC_OPERATION);

        if((in32(ravb->base + CSR) & CSR_OPS) != CSR_OPS_OPERATION)
            slogf(_SLOGC_NETWORK, _SLOG_ERROR,
                "failed to switch device to operation mode CSR=0x%x", in32(ravb->base + CSR));
    }

    return EOK;
}

int ravb_config(ravb_dev_t *ravb)
{
    int error;

    nic_mutex_lock(&ravb->drv_mutex);
    /* Set config mode */
    out32(ravb->base + CCC,
        (in32(ravb->base + CCC) & ~CCC_OPC) | CCC_OPC_CONFIG);
    /* Check if the operating mode is changed to the config mode */
    error = ravb_wait(ravb, CSR, CSR_OPS, CSR_OPS_CONFIG);
    nic_mutex_unlock(&ravb->drv_mutex);
    if (error)
        slogf(_SLOGC_NETWORK, _SLOG_ERROR,
                "failed to switch device to config mode");

    return error;
}

/* Device init function for Ethernet AVB */
static int ravb_dmac_init(ravb_dev_t *ravb)
{
    /* Disable all interrupts */
    out32(ravb->base + RIC0 , 0);
    out32(ravb->base + RIC1 , 0);
    out32(ravb->base + RIC2 , 0);
    out32(ravb->base + TIC , 0);

    /* Little Endian */
    out32(ravb->base + CCC, in32(ravb->base + CCC) & ~CCC_BOC);

    /* Set AVB RX */
    out32(ravb->base + RCR, RCR_EFFS | RCR_ENCF | RCR_ETS0 | 0x18000000);

    /* Set FIFO size */
    out32(ravb->base + TGC, TGC_TQP_AVBMODE1 | 0x00222200);

    /* delay CLK: 2ns */
    out32(ravb->base + APSR, (1 << 14));

    /* Timestamp enable */
    out32(ravb->base + TCCR, TCCR_TFEN);

    /* Enable TX and RX */
    out32(ravb->base + ECMR, in32(ravb->base + ECMR) | ECMR_RE | ECMR_TE);

    return 0;
}

/* E-MAC init function */
static int ravb_emac_init(ravb_dev_t *ravb)
{
    uint32_t ecmr;
    /* Disable MAC Interrupt */
    out32(ravb->base + ECSIPR, 0);

    /* Recv frame limit set register */
    out32(ravb->base + RFLR, RFLR_RFL_MIN);

    /* PAUSE prohibition */
    ecmr =  in32(ravb->base + ECMR);
    ecmr |= ECMR_ZPF | ECMR_TE | ECMR_RE;
    out32(ravb->base + ECMR, ecmr);

    /* Set MAC address */
    ravb_set_mac(ravb);

    out32(ravb->base + MPR, 1);

    /* E-MAC status register clear */
    out32(ravb->base + ECSR, ECSR_ICD | ECSR_MPD);

    /* E-MAC interrupt enable register */
    out32(ravb->base + ECSIPR, ECSIPR_ICDIP | ECSIPR_MPDIP | ECSIPR_LCHNGIP);

    return 0;
}

static int ravb_tx_desc_init(ravb_dev_t *ravb)
{
    size_t          size;
    off64_t         offset;
    uint32_t        i, queue;
    RAVB_TX_DESC    *tx_desc;

    for (queue = 0; queue < NUM_TX_QUEUES; queue++)
    {
        /* mbuf pointer array, corresponding to tx descriptor ring */
        size = sizeof(struct mbuf *) * NUM_TX_DESC;
        ravb->tx_pkts[queue] = malloc(size, M_DEVBUF, M_NOWAIT);
        if (ravb->tx_pkts[queue] == NULL) {
            return errno;
        }

        /* No packets in tx ring to start */
        memset(ravb->tx_pkts[queue], 0x00, size);

        /* Allocate all TX descriptors. */
        size = sizeof(RAVB_TX_DESC) * NUM_TX_DESC_WITH_LINK;
        ravb->tx_bd[queue] = mmap (NULL, size, PROT_READ | PROT_WRITE | PROT_NOCACHE,
                            MAP_SHARED, ravb->fd, 0);
        if (ravb->tx_bd[queue] == MAP_FAILED) {
            return errno;
        }
        if (mem_offset64((void *)ravb->tx_bd[queue], NOFD, 1, &offset, 0) == -1) {
            return errno;
        }
        memset((void *)ravb->tx_bd[queue], 0x00, size);

        /* Build TX ring buffer */
        for (i = 0; i < NUM_TX_DESC; i++)
        {
            tx_desc = &ravb->tx_bd[queue][i];
            tx_desc->die_dt = DT_EEMPTY;
        }
        tx_desc = &ravb->tx_bd[queue][i];
        tx_desc->dptr = (uint32_t)offset;
        tx_desc->die_dt = DT_LINKFIX; /* type */

        CACHE_FLUSH(&ravb->cachectl, (void *)ravb->tx_bd[queue], offset, size);
    }

    return EOK;
}

static int ravb_rx_desc_init(ravb_dev_t *ravb)
{
    size_t          size;
    off64_t         offset;
    uint32_t        i, queue;
    struct mbuf     *m;
    RAVB_RX_DESC    *rx_desc;

    for (queue = 0; queue < NUM_RX_QUEUES; queue++)
    {
        /* mbuf pointer array, corresponding to rx descriptor ring */
        size = sizeof(struct mbuf *) * NUM_RX_DESC;
        ravb->rx_pkts[queue] = malloc(size, M_DEVBUF, M_NOWAIT);
        if (ravb->rx_pkts[queue] == NULL) {
            return errno;
        }
        memset(ravb->rx_pkts[queue], 0, size);

        /* Allocate all RX descriptors. */
        size = sizeof(RAVB_RX_DESC) * NUM_RX_DESC_WITH_LINK;
        ravb->rx_bd[queue] = mmap (NULL, size, PROT_READ | PROT_WRITE | PROT_NOCACHE,
                            MAP_SHARED, ravb->fd, 0);
        if (ravb->rx_bd[queue] == MAP_FAILED) {
            return errno;
        }
        if (mem_offset64((void *)ravb->rx_bd[queue], NOFD, 1, &offset, 0) == -1) {
            return errno;
        }
        memset((void *)ravb->rx_bd[queue], 0, size);

        /* Build RX ring buffer */
        for (i = 0; i < NUM_RX_DESC; i++)
        {
            m = m_getcl(M_NOWAIT, MT_DATA, M_PKTHDR);
            if (m == NULL) {
                return ENOMEM;
            }
            /* RX descriptor */
            rx_desc = &ravb->rx_bd[queue][i];

            /* The size of the buffer should be on 4-byte boundary. */
            rx_desc->ds_cc = ALIGN(PKT_BUF_SZ, DPTR_ALIGN);
            ravb->rx_pkts[queue][i] = m;
            rx_desc->dptr = mbuf_phys(m);
            rx_desc->die_dt = DT_FEMPTY;
            CACHE_FLUSH(&ravb->cachectl, m->m_data, rx_desc->dptr,
                            m->m_ext.ext_size);
        }
        rx_desc = &ravb->rx_bd[queue][i];
        rx_desc->dptr = (uint32_t)offset;
        rx_desc->die_dt = DT_LINKFIX; /* type */

        CACHE_FLUSH(&ravb->cachectl, (void *)ravb->rx_bd[queue], offset, size);
    }

    return EOK;
}
static int ravb_desc_bat_init(ravb_dev_t *ravb)
{
    size_t          size;
    off64_t         offset;
    uint32_t        i;
    RAVB_DESC       *desc;

    /* Allocate descriptor base address table. They should be aligned */
    /* to size of struct ravb_desc. */
    size = sizeof(RAVB_DESC) * DBAT_ENTRY_NUM;
    ravb->desc_bat = mmap (NULL, size, PROT_READ | PROT_WRITE | PROT_NOCACHE,
                        MAP_SHARED, ravb->fd, 0);
    if (ravb->desc_bat == MAP_FAILED) {
        return errno;
    }

    if (mem_offset64((void *)ravb->desc_bat, NOFD, 1, &offset, 0) == -1) {
        return errno;
    }

    /* Initialize all descriptors */
    memset((void *)ravb->desc_bat, 0x0, size);
    for (i = 0; i < DBAT_ENTRY_NUM; i++)
        ravb->desc_bat[i].die_dt = DT_EOS;

    /* Register the descriptor base address table */
    out32(ravb->base + DBAT, offset);

    for(i = 0; i < NUM_RX_QUEUES; i++) {
        /* RX descriptor base address for all queues */
        desc = &ravb->desc_bat[RX_QUEUE_OFFSET + i];
        desc->die_dt = DT_LINKFIX; /* type */
        desc->dptr = ravb->rx_bd[i][NUM_RX_DESC].dptr;
    }

    for(i = 0; i < NUM_TX_QUEUES; i++) {
        /* TX descriptor base address for all queues */
        desc = &ravb->desc_bat[TX_QUEUE_OFFSET + i];
        desc->die_dt = DT_LINKFIX; /* type */
        desc->dptr = ravb->tx_bd[i][NUM_TX_DESC].dptr;
    }

    CACHE_FLUSH(&ravb->cachectl, (void *)ravb->desc_bat, offset, size);

    return EOK;
}
static int ravb_reset (ravb_dev_t *ravb)
{
    int error;

    nic_mutex_lock(&ravb->drv_mutex);
    /* set reset mode */
    out32(ravb->base + CCC, CCC_OPC_RESET);
    error = ravb_wait(ravb, CSR, CSR_OPS, CSR_OPS_RESET);
    nic_mutex_unlock(&ravb->drv_mutex);
    if (error)
        slogf(_SLOGC_NETWORK, _SLOG_ERROR,
                "failed to switch device to reset mode");

    return error;
}

static void ravb_shutdown (void *arg)
{
    ravb_dev_t      *ravb;

    ravb = arg;

    /*
     * Flow for Emergency stop
     * At this point, reads cannot be guranteed,
     * skipping steps to verify CSR register.
     */
    out32(ravb->base + CCC, in32(ravb->base + CCC) | CCC_DTSR);
    out32(ravb->base + CCC, (in32(ravb->base + CCC) & ~(CCC_OPC)) | CCC_OPC_RESET);
}

static void ravb_cleanup (ravb_dev_t *ravb)
{
    struct mbuf     *m;
    uint32_t        i, queue;

    if (ravb->iid[0]) {
        InterruptDetach(ravb->iid[0]);
    }
    if (ravb->iid[1]) {
        InterruptDetach(ravb->iid[1]);
    }
    /* Checks and returns EINVAL if not in the interrupt_entry list */
    interrupt_entry_remove(&ravb->ient, NULL);

    if (ravb->base) {
        ravb_reset(ravb);
        munmap_device_io(ravb->base, ravb->cfg.io_window_size[0]);
    }

    shutdownhook_disestablish(ravb->sdhook);

    /* Cleanup Tx allocations */
    for (queue = 0; queue < NUM_TX_QUEUES; queue++) {
        if (ravb->tx_pkts[queue]) {
            for (i = 0; i < NUM_TX_DESC; i++) {
                m = ravb->tx_pkts[queue][i];
                if (m != NULL) {
                    m_freem(m);
                }
            }
            free(ravb->tx_pkts[queue], M_DEVBUF);
        }
        if (ravb->tx_bd[queue]) {
            munmap((void *)ravb->tx_bd[queue], sizeof(RAVB_TX_DESC) * NUM_TX_DESC_WITH_LINK);
        }
    }

    /* Cleanup Rx allocations */
    for (queue = 0; queue < NUM_RX_QUEUES; queue++) {
        if (ravb->rx_pkts[queue]) {
            for (i = 0; i < NUM_RX_DESC; i++) {
                m = ravb->rx_pkts[queue][i];
                if (m != NULL) {
                    m_freem(m);
                }
            }
            free(ravb->rx_pkts[queue], M_DEVBUF);
        }
        if (ravb->rx_bd[queue]) {
            munmap((void *)ravb->rx_bd[queue], sizeof(RAVB_RX_DESC) * NUM_RX_DESC_WITH_LINK);
        }
    }

    if (ravb->desc_bat) {
        munmap((void *)ravb->desc_bat, sizeof(RAVB_DESC) * DBAT_ENTRY_NUM);
    }
    if (ravb->fd) {
        close(ravb->fd);
    }
    cache_fini(&ravb->cachectl);
}

void ravb_hw_stop (ravb_dev_t *ravb)
{
    struct mbuf         *m;
    uint32_t            i, queue;

    nic_mutex_lock(&ravb->drv_mutex);
    /* Wait for stopping the hardware TX process */
    ravb_wait(ravb, TCCR, TCCR_TSRQ0 | TCCR_TSRQ1 | TCCR_TSRQ2 | TCCR_TSRQ3, 0);
    ravb_wait(ravb, CSR, CSR_TPO0 | CSR_TPO1 | CSR_TPO2 | CSR_TPO3, 0);

    /* Stop Tx and Rx */
    out32(ravb->base + ECMR, in32(ravb->base + ECMR) & ~(ECMR_RE | ECMR_TE));

    /* Wait for stopping the RX DMA process */
    ravb_wait(ravb, CSR, CSR_RPO, 0);
    nic_mutex_unlock(&ravb->drv_mutex);

    for (queue = 0; queue < NUM_TX_QUEUES; queue++) {
        ravb_reap_tx(ravb, queue);
        /* Clear any pending Tx buffers */
        for (i = 0; i < NUM_TX_DESC; i++) {
            m = ravb->tx_pkts[queue][i];
            if (m != NULL) {
                m_freem(m);
            }
            ravb->tx_bd[queue][i].die_dt = DT_EEMPTY;
        }
    }
}

static void ravb_stop (struct ifnet *ifp, int disable)
{
    ravb_dev_t          *ravb;
    nic_config_t        *cfg;
    struct nw_work_thread   *wtp;

    ravb = ifp->if_softc;
    cfg = &ravb->cfg;
    wtp = WTP;

    NW_SIGLOCK_P(&ifp->if_snd_ex, ravb->iopkt, wtp);
    callout_stop(&ravb->mii_callout);
    MDI_DisableMonitor(ravb->mdi);
    cfg->flags |= NIC_FLAG_LINK_DOWN;
    if_link_state_change(ifp, LINK_STATE_DOWN);
    ifp->if_flags &= ~(IFF_RUNNING | IFF_OACTIVE);
    NW_SIGUNLOCK_P(&ifp->if_snd_ex, ravb->iopkt, wtp);

    ravb_hw_stop(ravb);

    MDI_PowerdownPhy(ravb->mdi, cfg->phy_addr);
    ravb_clear_stats(ravb);
}

int ravb_detach (struct device *dev, int flags)
{
    ravb_dev_t      *ravb;
    struct ifnet    *ifp;

    ravb = (ravb_dev_t *)dev;
    ifp = &ravb->ecom.ec_if;

    callout_stop(&ravb->mii_callout);
    MDI_PowerdownPhy(ravb->mdi, ravb->cfg.phy_addr);

    ether_ifdetach(ifp);
    if_detach(ifp);
    ravb_phy_fini(ravb);

    ravb_cleanup(ravb);

    return EOK;
}

static void ravb_set_mac (ravb_dev_t *ravb)
{
    uint32_t mach, macl;
    uint8_t *mac;

    mac = ravb->cfg.current_address;
    mach = (mac[0] << 24) | (mac[1] << 16) | (mac[2] << 8) | mac[3];
    macl = (mac[4] << 8) | mac[5];
    out32(ravb->base + MAHR, mach);
    out32(ravb->base + MALR, macl);
}

static int ravb_init (struct ifnet *ifp)
{
    ravb_dev_t      *ravb;
    nic_config_t    *cfg;
    struct nw_work_thread   *wtp;
    uint32_t        promisc_change, mac_change, hw_stop=0;

    ravb = ifp->if_softc;
    cfg = &ravb->cfg;
    wtp = WTP;

    /* check if configuration is required */
    mac_change = memcmp(cfg->current_address, LLADDR(ifp->if_sadl), ifp->if_addrlen);
    promisc_change = (((ifp->if_flags & IFF_PROMISC) == IFF_PROMISC) != (in32(ravb->base + ECMR) & ECMR_PRM))?1:0;
    if (mac_change | promisc_change) {
        if (in32(ravb->base + ECMR) & (ECMR_RE | ECMR_TE)) {
            hw_stop = 1;
        }
    }
    if (hw_stop) {
        ravb_hw_stop(ravb);
    }

    if (mac_change) {
        memcpy(cfg->current_address, LLADDR(ifp->if_sadl), ifp->if_addrlen);
        ravb_set_mac(ravb);

        if (cfg->verbose) {
            slogf(_SLOGC_NETWORK, _SLOG_ERROR, "ravb: MAC address updated");
        }
    }

    if (promisc_change) {
        out32(ravb->base + ECMR, in32(ravb->base + ECMR) ^ ECMR_PRM);
    }
    if (cfg->verbose) {
        slogf(_SLOGC_NETWORK, _SLOG_ERROR, "ravb: Promiscuous mode %s", (in32(ravb->base + ECMR) & ECMR_PRM)?"enabled":"disabled");
    }

    if (hw_stop) {
        out32(ravb->base + ECMR, in32(ravb->base + ECMR) | ECMR_RE | ECMR_TE);
    }
    if ((ifp->if_flags & IFF_RUNNING) == 0) {
        NW_SIGLOCK_P(&ifp->if_snd_ex, ravb->iopkt, wtp);
        ifp->if_flags |= IFF_RUNNING;
        NW_SIGUNLOCK_P(&ifp->if_snd_ex, ravb->iopkt, wtp);
        ravb_mediachange(ifp);
    }

    ravb_operate(ravb);

    return EOK;
}

int ravb_attach (struct device *parent, struct device *self, void *aux)
{
    ravb_dev_t      *ravb;
    attach_args_t   *attach_args;
    struct  ifnet   *ifp;
    nic_config_t    *cfg;
    int             rc;

    ravb = (ravb_dev_t*)self;
    attach_args = aux;

    /* Copy the config across */
    cfg = &ravb->cfg;
    ravb->drv_mutex = NIC_MUTEX_UNLOCKED_VALUE;
    ravb->ptp_mutex = NIC_MUTEX_UNLOCKED_VALUE;
    memcpy(cfg, &attach_args->cfg, sizeof(*cfg));
    ravb->set_flow = attach_args->set_flow;

    cfg->connector = NIC_CONNECTOR_MII;

    ravb->iopkt = attach_args->iopkt;
    ravb->dev.dv_dll_hdl = attach_args->dll_hdl;
    callout_init(&ravb->mii_callout);

    ravb->cachectl.fd = NOFD;
    if (cache_init(0, &ravb->cachectl, NULL) == -1) {
    rc = errno;
        slogf(_SLOGC_NETWORK, _SLOG_ERROR,
          "ravb: Failed to init cache");
        ravb_cleanup(ravb);
        return rc;
    }

    ravb->base = mmap_device_io(cfg->io_window_size[0],
                 cfg->io_window_base[0]);
    if (ravb->base == (uintptr_t)MAP_FAILED) {
        rc = errno;
        slogf(_SLOGC_NETWORK, _SLOG_ERROR, "ravb: Failed to mmap device");
        ravb_cleanup(ravb);
        return rc;
    }

    /* Set AVB reset mode */
    ravb_reset(ravb);
    /* Set AVB config mode */
    ravb_config(ravb);
    /* gPTP Clock Select = High Speed Peripheral Bus Clock */
    out32(ravb->base + CCC, (in32(ravb->base + CCC) & ~CCC_CSEL) | CCC_CSEL_HPB);
    /* clk setting */
    out32(ravb->base + APSR, in32(ravb->base + APSR) | 0x00004000);

    /* Use typed mem for descriptors and packets since
     * ethernet DMA has 32-bit boundary */
    ravb->fd = posix_typed_mem_open( "below4G", O_RDWR,
        POSIX_TYPED_MEM_ALLOCATE_CONTIG);
    if (ravb->fd == -1) {
        return ENOMEM;
    }

    rc = ravb_tx_desc_init(ravb);
    if (rc != EOK) {
        slogf(_SLOGC_NETWORK, _SLOG_ERROR,
              "ravb: Failed to init TX Descriptors: %d", rc);
        ravb_cleanup(ravb);
        return rc;
    }

    rc = ravb_rx_desc_init(ravb);
    if (rc != EOK) {
        slogf(_SLOGC_NETWORK, _SLOG_ERROR,
              "ravb: Failed to init RX Descriptors: %d", rc);
        ravb_cleanup(ravb);
        return rc;
    }

    rc = ravb_desc_bat_init(ravb);
    if (rc != EOK) {
        slogf(_SLOGC_NETWORK, _SLOG_ERROR,
              "ravb: Failed to init BAT Descriptors: %d", rc);
        ravb_cleanup(ravb);
        return rc;
    }

    ravb->sdhook = shutdownhook_establish(ravb_shutdown, ravb);

    ravb_emac_init(ravb);
    ravb_dmac_init(ravb);
    ravb_ptp_init(ravb);
    rc = ravb_phy_init(ravb);

    ravb_dmac_enable_interrupt(ravb);

    if (rc != EOK) {
        slogf(_SLOGC_NETWORK, _SLOG_ERROR,
              "ravb: Failed to find PHY: %d", rc);
        ravb_cleanup(ravb);
        return rc;
    }

    ravb->ient.func = ravb_process_interrupt;
    ravb->ient.enable = ravb_dmac_enable_interrupt;
    ravb->ient.arg = ravb;

    if ((rc = interrupt_entry_init(&ravb->ient, 0, NULL,
        cfg->priority)) != EOK) {
        slogf(_SLOGC_NETWORK, _SLOG_ERROR,
              "ravb: Failed to init the interrupt entry: %d", rc);
        ravb_cleanup(ravb);
        return rc;
    }

    rc = InterruptAttach_r(cfg->irq[0], ravb_dmac_isr, ravb, sizeof(ravb),
               _NTO_INTR_FLAGS_TRK_MSK);
    if (rc < 0) {
        rc = -rc;
        slogf(_SLOGC_NETWORK, _SLOG_ERROR,
              "ravb: Failed to attach to the interrupt: %d", rc);
        ravb_cleanup(ravb);
        return rc;
    }
    ravb->iid[0] = rc;

    rc = InterruptAttach_r(cfg->irq[1], ravb_emac_isr, ravb, sizeof(ravb),
               _NTO_INTR_FLAGS_TRK_MSK);
    if (rc < 0) {
        rc = -rc;
        slogf(_SLOGC_NETWORK, _SLOG_ERROR,
              "ravb: Failed to attach to the interrupt: %d", rc);
        ravb_cleanup(ravb);
        return rc;
    }
    ravb->iid[1] = rc;

    ravb_clear_stats(ravb);

    strcpy((char *)cfg->device_description, "Renesas AVB Network Device");

    ifp = &ravb->ecom.ec_if;
    strcpy(ifp->if_xname, ravb->dev.dv_xname);
    ifp->if_softc = ravb;
    /* No multicast filtering in hardware so set IFF_ALLMULTI */
    ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX | IFF_MULTICAST | IFF_ALLMULTI;
    ifp->if_ioctl = ravb_ioctl;
    ifp->if_start = ravb_start;
    ifp->if_init  = ravb_init;
    ifp->if_stop  = ravb_stop;
    IFQ_SET_READY(&ifp->if_snd);
    ravb->ecom.ec_capabilities |= ETHERCAP_VLAN_MTU;

    if_attach(ifp);
    ether_ifattach(ifp, cfg->current_address);

    /* Intercept if_output for pulling off AVB packets */
    ravb->stack_output = ravb->ecom.ec_if.if_output;
    ravb->ecom.ec_if.if_output = ravb_output;

    return EOK;
}

static char *ravb_opts[] = {
    "flow",
#define OPT_FLOW    0
    NULL
};

static int ravb_parse_options (const char *optstring,
                 attach_args_t *attach_args)
{
    nic_config_t    *cfg;
    char        *options, *freeptr, *value;
    int         opt;

    if (optstring == NULL) {
    return 0;
    }

    cfg = &attach_args->cfg;

    /* getsubopt() is destructive */
    freeptr = options = strdup(optstring);

    while (options && *options != '\0') {
    opt = getsubopt(&options, ravb_opts, &value);
    switch (opt) {

    case OPT_FLOW:
        attach_args->set_flow = strtoul(value, 0, 0);
        if (attach_args->set_flow > 3) {
        slogf(_SLOGC_NETWORK, _SLOG_ERROR,
              "ravb: Unknown flow value %d, forcing off",
              attach_args->set_flow);
        attach_args->set_flow = 0;
        }
        break;

    default:
        if (nic_parse_options(cfg, value) != EOK) {
        slogf(_SLOGC_NETWORK, _SLOG_ERROR,
              "ravb: Unknown option %s", value);
        (free)(freeptr);
        return EINVAL;
        }
    }
    }
    (free)(freeptr);
    return EOK;
}

CFATTACH_DECL(ravb,
    sizeof(ravb_dev_t),
    NULL,
    ravb_attach,
    ravb_detach,
    NULL);

static void ravb_parse_syspage (nic_config_t *cfg)
{
    hwi_tag     *tag;
    unsigned        item;

    /* Find network hardware information. */
    item = hwi_find_device("ravb", 0);
    if (item == HWI_NULL_OFF) {
    return;
    }


    if (cfg->num_io_windows == 0) {
    /* Find base address */
    if ((tag = hwi_tag_find(item, HWI_TAG_NAME_location, NULL)) != NULL) {
        cfg->io_window_base[0] = (paddr_t)tag->location.base;
        cfg->num_io_windows = 1;
    }
    }

    if (cfg->num_irqs == 0) {
    /* Find IRQ */
    cfg->irq[0] = hwitag_find_ivec(item, NULL);
    if (cfg->irq[0] != HWI_ILLEGAL_VECTOR) {
        cfg->num_irqs = 1;
    }
    }

    /* Find MAC address */
    hwitag_find_nicaddr(item, NULL, cfg->current_address);
}

int ravb_entry (void *dll_hdl,  struct _iopkt_self *iopkt, char *options)
{
    attach_args_t           *attach_args;
    nic_config_t            *cfg;
    struct device           *dev;
    int                     rc, single;
    struct ifnet            *ifp;
    struct drvcom_config    *dcon;

    /* Check if it is already mounted by doing a "nicinfo" on each interface */
    dcon = (malloc)(sizeof(*dcon));
    if (dcon == NULL) {
        slogf(_SLOGC_NETWORK, _SLOG_ERROR,
              "%s:%d - malloc failed", __FUNCTION__, __LINE__);
        return ENOMEM;
    }
    IFNET_FOREACH(ifp) {
        dcon->dcom_cmd.ifdc_cmd = DRVCOM_CONFIG;
        dcon->dcom_cmd.ifdc_len = sizeof(dcon->dcom_config);
        rc = ifp->if_ioctl(ifp, SIOCGDRVCOM, (caddr_t)dcon);
        if ((rc == EOK) && (dcon->dcom_config.num_io_windows > 0) &&
                (dcon->dcom_config.io_window_base[0] == AVB_REG_BASE) ) {
            slogf(_SLOGC_NETWORK, _SLOG_ERROR,
                  "%s:%d - Driver already loaded for %s",
                  __FUNCTION__, __LINE__, ifp->if_xname);
            return EBUSY;
        }
    }
    (free)(dcon);

    attach_args = calloc(1,sizeof(*attach_args));
    attach_args->iopkt = iopkt;
    attach_args->dll_hdl = dll_hdl;

    /* Set defaults before parsing the command line */
    cfg = &attach_args->cfg;
    cfg->media_rate = -1;
    cfg->duplex = -1;
    attach_args->set_flow = -1;
    cfg->mtu = ETHERMTU;
    cfg->mru = ETHERMTU;
    cfg->flags = NIC_FLAG_MULTICAST;
    cfg->mac_length = ETHER_ADDR_LEN;
    cfg->priority = stk_ctl.rx_prio;

    if (ravb_parse_options(options, attach_args) != EOK) {
        (free)(attach_args);
        return EINVAL;
    }

    /* If setting speed, must set duplex and vice versa */
    if (((cfg->media_rate != -1) && (cfg->duplex == -1)) ||
        ((cfg->media_rate == -1) && (cfg->duplex != -1))) {
        slogf(_SLOGC_NETWORK, _SLOG_ERROR,
              "ravb: Must set both speed and duplex if setting either");
        return EINVAL;
    }
    /* If speed set then turn flow off if not set */
    if ((cfg->media_rate != -1) && (attach_args->set_flow == -1)) {
        attach_args->set_flow = 0;
    }

    /*
     * If base address, IRQ, MAC address are not set from the command line,
     * set them from the syspage info and if not there then set to
     * default values.
     */
    ravb_parse_syspage(cfg);

    if (cfg->num_io_windows == 0) {
        cfg->io_window_base[0] = AVB_REG_BASE;
        cfg->num_io_windows = 1;
    }
    cfg->io_window_size[0] = AVB_REG_SIZE;

    if (cfg->num_irqs == 0) {
        cfg->irq[0] = AVB_DMAC_IRQ;
        cfg->irq[1] = AVB_EMAC_IRQ;
        cfg->num_irqs = 2;
    }

    if ((memcmp(cfg->current_address, "\0\0\0\0\0\0", ETHER_ADDR_LEN) == 0) ||
    ETHER_IS_MULTICAST(cfg->current_address)) {
    slogf(_SLOGC_NETWORK, _SLOG_ERROR,
          "ravb: Invalid MAC address %02x%02x%02x%02x%02x%02x",
          cfg->current_address[0], cfg->current_address[1],
          cfg->current_address[2], cfg->current_address[3],
          cfg->current_address[4], cfg->current_address[5]);
    return EINVAL;
    }

    memcpy(cfg->permanent_address, cfg->current_address, ETHER_ADDR_LEN);

    dev = NULL;
    rc = dev_attach("ravb", options, &ravb_ca, attach_args, &single,
            &dev, NULL);
    (free)(attach_args);
    return rc;
}

struct _iopkt_drvr_entry IOPKT_DRVR_ENTRY_SYM(ravb) = IOPKT_DRVR_ENTRY_SYM_INIT(ravb_entry);

#ifdef VARIANT_a
#include <nw_dl.h>
/* This is what gets specified in the stack's dl.c */
struct nw_dll_syms ravb_syms[] = {
    {"iopkt_drvr_entry", &IOPKT_DRVR_ENTRY_SYM(ravb)},
    {NULL, NULL}
};
#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/devnp/ravb/ravb.c $ $Rev: 853959 $")
#endif
