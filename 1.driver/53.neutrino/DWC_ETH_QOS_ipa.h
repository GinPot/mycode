/* Copyright (c) 2016, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __DWC_ETH_QOS_IPA_H__
#define __DWC_ETH_QOS_IPA_H__

#include <linux/ipa.h>
#include <linux/ipa_uc_offload.h>
#include <asm/io.h>
#include <linux/debugfs.h>
#include <linux/in.h>
#include <linux/ip.h>

#include "DWC_ETH_QOS_yheader.h"
#include "DWC_ETH_QOS_yregacc.h"

extern ULONG dwc_eth_ntn_reg_pci_base_addr;
extern ULONG dwc_eth_ntn_reg_pci_base_addr_phy;
extern ULONG dwc_eth_ntn_SRAM_pci_base_addr_phy;
extern ULONG dwc_eth_ntn_SRAM_pci_base_addr_virt;
extern ULONG dwc_eth_ntn_hostmem_pci_base_addr_virt;

#ifdef DWC_ETH_QOS_ENABLE_IPA

#define NTN_HOST_IPA_CAPABLE	1

/* IPA Ready client callback. Called by IPA when its ready */
void DWC_ETH_QOS_ipa_uc_ready_cb(void *user_data);

int DWC_ETH_QOS_enable_ipa_offload(struct DWC_ETH_QOS_prv_data *pdata);
int DWC_ETH_QOS_disable_ipa_offload(struct DWC_ETH_QOS_prv_data *pdata);
int DWC_ETH_QOS_disable_enable_ipa_offload(struct DWC_ETH_QOS_prv_data *pdata,int chInx_tx_ipa,int chInx_rx_ipa);

/* Initialize Offload data path and add partial headers */
int DWC_ETH_QOS_ipa_offload_init(struct DWC_ETH_QOS_prv_data *pdata);

/* Cleanup Offload data path */
int DWC_ETH_QOS_ipa_offload_cleanup(struct DWC_ETH_QOS_prv_data *pdata);

/* Connect Offload Data path */
int DWC_ETH_QOS_ipa_offload_connect(struct DWC_ETH_QOS_prv_data *pdata);

/* Disconnect Offload Data path */
int DWC_ETH_QOS_ipa_offload_disconnect(struct DWC_ETH_QOS_prv_data *pdata);

/* Create Debugfs Node */
int DWC_ETH_QOS_ipa_create_debugfs(struct DWC_ETH_QOS_prv_data *pdata);

/* Cleanup Debugfs Node */
int DWC_ETH_QOS_ipa_cleanup_debugfs(struct DWC_ETH_QOS_prv_data *pdata);

//void DWC_ETH_QOS_ipa_stats_read(struct DWC_ETH_QOS_prv_data *pdata);
int DWC_ETH_QOS_ipa_offload_suspend(struct DWC_ETH_QOS_prv_data *pdata);
int DWC_ETH_QOS_ipa_offload_resume(struct DWC_ETH_QOS_prv_data *pdata);
int DWC_ETH_QOS_ipa_ready(struct DWC_ETH_QOS_prv_data *pdata);
#else /* DWC_ETH_QOS_ENABLE_IPA */

#define NTN_HOST_IPA_CAPABLE	0

static inline int DWC_ETH_QOS_ipa_offload_init(struct DWC_ETH_QOS_prv_data *pdata)
{
	return -EPERM;
}

static inline void DWC_ETH_QOS_ipa_uc_ready_cb(void *user_data)
{
	return;
}

static inline int DWC_ETH_QOS_enable_ipa_offload(struct DWC_ETH_QOS_prv_data *pdata)
{
	return -EPERM;
}

static inline int DWC_ETH_QOS_disable_ipa_offload(struct DWC_ETH_QOS_prv_data *pdata)
{
	return -EPERM;
}

static inline int DWC_ETH_QOS_disable_enable_ipa_offload(struct DWC_ETH_QOS_prv_data *pdata,int chInx_tx_ipa,int chInx_rx_ipa)
{
	return -EPERM;
}

//static inline void DWC_ETH_QOS_ipa_stats_read(struct DWC_ETH_QOS_prv_data *pdata)
//{
//	return;
//}

static inline int DWC_ETH_QOS_ipa_offload_suspend(struct DWC_ETH_QOS_prv_data *pdata)
{
	return -EPERM;
}
static inline int DWC_ETH_QOS_ipa_offload_resume(struct DWC_ETH_QOS_prv_data *pdata)
{
	return -EPERM;
}

static inline int DWC_ETH_QOS_ipa_ready(struct DWC_ETH_QOS_prv_data *pdata)
{
	return -EPERM;
}



#endif /* DWC_ETH_QOS_ENABLE_IPA */

#endif
