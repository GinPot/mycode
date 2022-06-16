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

#include "DWC_ETH_QOS_ipa.h"

#define NTN_IPA_PIPE_MAX_BW 800
#define NTN_IPA_PIPE_MIN_BW 0
#define NTN_IPA_DBG_MAX_MSG_LEN 3000
static char buf[3000];

static void DWC_ETH_QOS_ipaUcRdy_wq(struct work_struct *work);

/* Description of RX CH Status as per DMA_RXCHSTS register in spec*/
static const char *rx_ch_status_string[] = {
	"Stop State",
	"Descriptor Fetch State",
	"Wait For Status",
	"Writing Data to Memory",
	"Status Update",
	"Suspended State",
	"Closing Descriptor",
	"Reserved",
};

/* Description of TX CH Status as per DMA_TXCHSTS register in spec*/
static const char *tx_ch_status_string[] = {
	"Stop State",
	"Descriptor Fetch State",
	"Wait For Status",
	"Reading Data from Memory",
	"Status Update",
	"Suspended State",
	"Closing Descriptor",
	"Reserved",
};

/* Generic Bit descirption; reset = 0, set = 1*/
static const char *bit_status_string[] = {
	"Reset",
	"Set",
};

/* Generic Bit Mask Description; masked = 0, enabled = 1*/
static const char *bit_mask_string[] = {
	"Masked",
	"Enable",
};

#define IPA_ETH_RX_SOFTIRQ_THRESH	16

int DWC_ETH_QOS_enable_ipa_offload(struct DWC_ETH_QOS_prv_data *pdata)
{
	int ret = Y_SUCCESS;
	struct hw_if_struct *hw_if = &pdata->hw_if;

	/* Enable offload flag in NTN HW */
	hw_if->enable_offload(pdata);

	if (pdata->prv_ipa.ipa_ready  && !pdata->prv_ipa.ipa_offload_init) {
		ret = DWC_ETH_QOS_ipa_offload_init(pdata);
		if (ret) {
			pdata->prv_ipa.ipa_offload_init = false;
			NMSGPR_ERR("IPA Offload Init Failed \n");
			goto fail;
		}
		NMSGPR_INFO("IPA Offload Initialized Successfully \n");
		pdata->prv_ipa.ipa_offload_init = true;
	}

	if (pdata->prv_ipa.ipa_uc_ready && !pdata->prv_ipa.ipa_offload_conn) {
		ret = DWC_ETH_QOS_ipa_offload_connect(pdata);
		if (ret) {
			NMSGPR_ERR("IPA Offload Connect Failed \n");
			pdata->prv_ipa.ipa_offload_conn = false;
			goto fail;
		}
		NMSGPR_INFO("IPA Offload Connect Successfully\n");
		pdata->prv_ipa.ipa_offload_conn = true;
	}

	/*Initialize DMA CHs for offload*/
	ret = hw_if->init_offload(pdata);
	if (ret) {
		NMSGPR_ERR("Offload channel Init Failed \n");
		goto fail;
	}

	if (!pdata->prv_ipa.ipa_debugfs_exists) {
		if (!DWC_ETH_QOS_ipa_create_debugfs(pdata)) {
			NMSGPR_INFO("NTN Debugfs created  \n");
			pdata->prv_ipa.ipa_debugfs_exists = true;
		} else NMSGPR_ERR("NTN Debugfs failed \n");
	}

	NMSGPR_INFO("IPA Offload Enabled successfully\n");
	return ret;

fail:
	if ( pdata->prv_ipa.ipa_offload_conn ) {
		if( DWC_ETH_QOS_ipa_offload_disconnect(pdata) )
			NMSGPR_ERR("IPA Offload Disconnect Failed \n");
		else
			NMSGPR_INFO("IPA Offload Disconnect Successfully \n");
		pdata->prv_ipa.ipa_offload_conn = false;
	}
	if ( pdata->prv_ipa.ipa_offload_init ) {
		if ( DWC_ETH_QOS_ipa_offload_cleanup(pdata ))
			NMSGPR_ERR("IPA Offload Cleanup Failed \n");
		else
			NMSGPR_INFO("IPA Offload Cleanup Success \n");
		pdata->prv_ipa.ipa_offload_init = false;
	}
	/* Disable offload flag in NTN HW */
	hw_if->disable_offload(pdata);

	return ret;
}

int DWC_ETH_QOS_disable_ipa_offload(struct DWC_ETH_QOS_prv_data *pdata)
{
	struct hw_if_struct *hw_if = &pdata->hw_if;
	int ret = Y_SUCCESS;

	/* De-configure IPA Related Stuff */
	if (pdata->prv_ipa.ipa_offload_conn) {
		ret = DWC_ETH_QOS_ipa_offload_suspend(pdata);
		if (ret) {
			NMSGPR_ERR("IPA Offload Disconnect Failed, err:%d\n", ret);
			return ret;
		}
		pdata->prv_ipa.ipa_offload_conn = false;
		NMSGPR_INFO("IPA Offload Disconnect Successfully \n");
	}

	if (pdata->prv_ipa.ipa_offload_init) {
		ret = DWC_ETH_QOS_ipa_offload_cleanup(pdata);
		if (ret) {
			NMSGPR_ERR("IPA Offload Cleanup Failed, err: %d\n", ret);
			return ret;
		}
		NMSGPR_INFO("IPA Offload Cleanup Success \n");
		pdata->prv_ipa.ipa_offload_init = false;
	}
	NMSGPR_INFO("IPA Offload Disabled successfully\n");

	if (pdata->prv_ipa.ipa_debugfs_exists) {
		if (DWC_ETH_QOS_ipa_cleanup_debugfs(pdata))
			NMSGPR_ERR("Unable to delete IPA debugfs\n");
		else
			pdata->prv_ipa.ipa_debugfs_exists = false;
	}

	/* Disable offload flag in NTN HW */
	hw_if->disable_offload(pdata);

	return ret;
}


/* This function is called from IOCTL when new RX/TX properties have to
   be registered with IPA e.g VLAN hdr insertion deletion */
int DWC_ETH_QOS_disable_enable_ipa_offload(struct DWC_ETH_QOS_prv_data *pdata, int chInx_tx_ipa, int chInx_rx_ipa)
{
		struct hw_if_struct *hw_if = &(pdata->hw_if);
		int ret = Y_SUCCESS;

		/*stop the IPA owned DMA channels */
		ret = hw_if->stop_dma_rx(chInx_rx_ipa, pdata);
		if (ret != Y_SUCCESS) {
			   NMSGPR_ERR("%s stop_dma_rx failed %d\n", __func__, ret);
			   return ret;
		}

		ret = hw_if->stop_dma_tx(chInx_tx_ipa, pdata);
		if (ret != Y_SUCCESS) {
			  NMSGPR_ERR("%s stop_dma_tx failed %d\n", __func__, ret);
			  return ret;
		}

		/*disable IPA pipe first*/
		ret = DWC_ETH_QOS_disable_ipa_offload(pdata);
		if ( ret ){
					 NMSGPR_ERR("%s:%d unable to disable ipa offload\n",
							   __func__, __LINE__);
					 return ret;
		}
		else {
			 hw_if->tx_desc_init(pdata, chInx_tx_ipa);
			 hw_if->rx_desc_init(pdata, chInx_rx_ipa);

			 /*if VLAN-id is passed, then make the VLAN+ETH hdr
			   and register the RX/TX properties*/
			 ret = DWC_ETH_QOS_enable_ipa_offload(pdata);
			 if (ret) {
					   NMSGPR_ERR("%s:%d unable to enable ipa offload\n",
									  __func__, __LINE__);
					   return ret;
			 }
			 else {
				ret = hw_if->start_dma_tx(chInx_tx_ipa, pdata);
				if (ret != Y_SUCCESS) {
						NMSGPR_ERR("%s start_dma_tx failed %d\n", __func__, ret);
						return ret;
				}

				ret = hw_if->start_dma_rx(chInx_rx_ipa, pdata);
				if (ret != Y_SUCCESS){
						NMSGPR_ERR("%s start_dma_rx failed %d\n", __func__, ret);
						return ret;
				}
			 }
		}

		return ret;
}

/**
 * DWC_ETH_QOS_ipa_offload_suspend() - Suspend IPA offload data
 * path.
 *
 * IN: @pdata: NTN dirver private structure.
 * OUT: 0 on success and -1 on failure
 */
int DWC_ETH_QOS_ipa_offload_suspend(struct DWC_ETH_QOS_prv_data *pdata)
{
	int ret = Y_SUCCESS;
	struct hw_if_struct *hw_if = &(pdata->hw_if);
	struct ipa_rm_perf_profile profile;

	NDBGPR_L1("Suspend/disable IPA offload\n");
	ret = hw_if->stop_dma_rx(NTN_RX_DMA_CH_0, pdata);
	if (ret != Y_SUCCESS) {
		NMSGPR_ERR("%s stop_dma_rx failed %d\n", __func__, ret);
		return ret;
	}

	/* Disconnect IPA offload */
	if (pdata->prv_ipa.ipa_offload_conn) {
		ret = DWC_ETH_QOS_ipa_offload_disconnect(pdata);
		if (ret) {
			NMSGPR_ERR("IPA Offload Disconnect Failed, err:%d\n", ret);
			return ret;
		}
		pdata->prv_ipa.ipa_offload_conn = false;
		NDBGPR_L1("IPA Offload Disconnect Successfully \n");
	}

	ret = hw_if->stop_dma_tx(NTN_TX_DMA_CH_2, pdata);
	if (ret != Y_SUCCESS) {
		NMSGPR_ERR("%s stop_dma_tx failed %d\n", __func__, ret);
		return ret;
	}

	profile.max_supported_bandwidth_mbps = NTN_IPA_PIPE_MIN_BW;
	ret = ipa_rm_set_perf_profile(IPA_RM_RESOURCE_ETHERNET_CONS,
					&profile);
	if (ret)
		NMSGPR_ERR("Err to set BW: IPA_RM_RESOURCE_ETHERNET_CONS err:%d\n",
				ret);

	pdata->prv_ipa.ipa_offload_susp = true;
	return ret;
}

/**
 * DWC_ETH_QOS_ipa_offload_resume() - Resumes IPA offload data
 * path.
 *
 * IN: @pdata: NTN dirver private structure.
 * OUT: 0 on success and -1 on failure
 */
int DWC_ETH_QOS_ipa_offload_resume(struct DWC_ETH_QOS_prv_data *pdata)
{
	struct hw_if_struct *hw_if = &(pdata->hw_if);
	int ret = Y_SUCCESS;
	struct ipa_rm_perf_profile profile;

	NDBGPR_L1("Resume/enable IPA offload\n");
	hw_if->tx_desc_init(pdata, NTN_TX_DMA_CH_2);
	hw_if->rx_desc_init(pdata, NTN_RX_DMA_CH_0);

	NDBGPR_L1("DWC_ETH_QOS_ipa_offload_connect\n");
	ret = DWC_ETH_QOS_ipa_offload_connect(pdata);
	if (ret != Y_SUCCESS)
		return ret;
	else
		pdata->prv_ipa.ipa_offload_conn = true;

	profile.max_supported_bandwidth_mbps = NTN_IPA_PIPE_MAX_BW;
	ret = ipa_rm_set_perf_profile(IPA_RM_RESOURCE_ETHERNET_CONS,
					&profile);
	if (ret)
		NMSGPR_ERR("Err to set BW: IPA_RM_RESOURCE_ETHERNET_CONS err:%d\n",
				ret);
	/*Initialize DMA CHs for offload*/
	ret = hw_if->init_offload(pdata);
	if (ret) {
		NMSGPR_ERR("Offload channel Init Failed \n");
		return ret;
	}

	NDBGPR_L1("start_dma_tx/start_dma_rx\n");
	ret = hw_if->start_dma_tx(NTN_TX_DMA_CH_2, pdata);
	if (ret != Y_SUCCESS) {
		NMSGPR_ERR("%s start_dma_tx failed %d\n", __func__, ret);
		return ret;
	}

	ret = hw_if->start_dma_rx(NTN_RX_DMA_CH_0, pdata);
	if (ret != Y_SUCCESS)
		NMSGPR_ERR("%s start_dma_rx failed %d\n", __func__, ret);

	pdata->prv_ipa.ipa_offload_susp = false;
	return ret;
}

static int DWC_ETH_QOS_ipa_uc_ready(struct DWC_ETH_QOS_prv_data *pdata)
{
	struct ipa_uc_ready_params param;
	int ret;

	param.is_uC_ready = false;
	param.priv = pdata;
	param.notify = DWC_ETH_QOS_ipa_uc_ready_cb;
	param.proto = IPA_UC_NTN;

	ret = ipa_uc_offload_reg_rdyCB(&param);
	if (ret == 0 && param.is_uC_ready) {
		NDBGPR_L1("%s:%d ipa uc ready\n", __func__, __LINE__);
		pdata->prv_ipa.ipa_uc_ready = true;
	}

	return ret;
}

static void DWC_ETH_QOS_ipa_ready_wq(struct work_struct *work)
{
	struct DWC_ETH_QOS_prv_ipa_data *ntn_ipa = container_of(work,
				struct DWC_ETH_QOS_prv_ipa_data, ntn_ipa_rdy_work);
	struct DWC_ETH_QOS_prv_data *pdata = container_of(ntn_ipa,
					struct DWC_ETH_QOS_prv_data, prv_ipa);
	int ret;

	pdata->prv_ipa.ipa_ready = true;
	ret = DWC_ETH_QOS_ipa_offload_init(pdata);
	if (!ret) {
		NMSGPR_INFO("IPA Offload Initialized Successfully \n");
		pdata->prv_ipa.ipa_offload_init = true;
	}

	ret = DWC_ETH_QOS_ipa_uc_ready(pdata);
	if (ret == 0 && pdata->prv_ipa.ipa_uc_ready)
		DWC_ETH_QOS_enable_ipa_offload(pdata);
}

static void DWC_ETH_QOS_ipaUcRdy_wq(struct work_struct *work)
{
	struct DWC_ETH_QOS_prv_ipa_data *ntn_ipa = container_of(work,
					struct DWC_ETH_QOS_prv_ipa_data, ntn_ipa_rdy_work);
	struct DWC_ETH_QOS_prv_data *pdata = container_of(ntn_ipa,
					struct DWC_ETH_QOS_prv_data, prv_ipa);

	pdata->prv_ipa.ipa_uc_ready = true;
	DWC_ETH_QOS_enable_ipa_offload(pdata);
}
/**
 * DWC_ETH_QOS_ipa_ready_cb() - Callback register with IPA to
 * indicate if IPA is ready to receive configuration commands.
 * If IPA is not ready no IPA configuration commands should be
 * set.
 *
 * IN: @pdata: NTN private structure handle that will be passed by IPA.
 * OUT: NULL
 */
static void DWC_ETH_QOS_ipa_ready_cb(void *user_data)
{
	struct DWC_ETH_QOS_prv_data *pdata = (struct DWC_ETH_QOS_prv_data *)user_data;
	struct DWC_ETH_QOS_prv_ipa_data *ntn_ipa = &pdata->prv_ipa;

	if(!pdata) {
		NMSGPR_ERR("%s Null Param pdata %p \n", __func__, pdata);
		return;
	}

	NDBGPR_L1("%s Received IPA ready callback\n",__func__);
	pdata->prv_ipa.ipa_ready = true;
	INIT_WORK(&ntn_ipa->ntn_ipa_rdy_work, DWC_ETH_QOS_ipa_ready_wq);
	queue_work(system_unbound_wq, &ntn_ipa->ntn_ipa_rdy_work);
	return;
}

/**
 * DWC_ETH_QOS_ipa_uc_ready_cb() - Callback register with IPA to indicate
 * if IPA (and IPA uC) is ready to receive configuration commands.
 * If IPA is not ready no IPA configuration commands should be set.
 *
 * IN: @pdata: NTN private structure handle that will be passed by IPA.
 * OUT: NULL
 */
void DWC_ETH_QOS_ipa_uc_ready_cb(void *user_data)
{
	struct DWC_ETH_QOS_prv_data *pdata = (struct DWC_ETH_QOS_prv_data *)user_data;
	struct DWC_ETH_QOS_prv_ipa_data *ntn_ipa = &pdata->prv_ipa;

	if(!pdata) {
		NMSGPR_ERR("%s Null Param pdata %p \n", __func__, pdata);
		return;
	}

	NDBGPR_L1("%s Received IPA ready callback\n",__func__);
	pdata->prv_ipa.ipa_uc_ready = true;

	INIT_WORK(&ntn_ipa->ntn_ipa_rdy_work, DWC_ETH_QOS_ipaUcRdy_wq);
	queue_work(system_unbound_wq, &ntn_ipa->ntn_ipa_rdy_work);

	return;
}

/**
 * ntn_ipa_notify_cb() - Callback registered with IPA to handle data packets.
 *
 * IN: @priv:             Priv data passed to IPA at the time of registration
 * IN: @ipa_dp_evt_type:  IPA event type
 * IN: @data:             Data to be handled for this event.
 * OUT: NULL
 */
static void ntn_ipa_notify_cb(void *priv, enum ipa_dp_evt_type evt,
				unsigned long data)
{
	struct DWC_ETH_QOS_prv_data *pdata = (struct DWC_ETH_QOS_prv_data *)priv;
	struct DWC_ETH_QOS_prv_ipa_data *ntn_ipa;
	struct sk_buff *skb = (struct sk_buff *)data;
	struct iphdr *ip_hdr = NULL;
	int stat = NET_RX_SUCCESS;

	if(!pdata || !skb) {
		NMSGPR_ERR("Null Param %s pdata %p skb %p \n", __func__, pdata, skb);
		return;
	}

	ntn_ipa = &pdata->prv_ipa;
	if(!ntn_ipa) {
		NMSGPR_ERR( "Null Param %s ntn_ipa %p \n", __func__, ntn_ipa);
		return;
	}

	NDBGPR_L2("%s %d EVT Rcvd %d \n", __func__, __LINE__, evt);

	if (!ntn_ipa->ipa_offload_conn) {
		NMSGPR_ERR("ipa_cb before offload is ready %s ipa_offload_conn %d  \n", __func__, ntn_ipa->ipa_offload_conn);
		return;
	}

	if (evt == IPA_RECEIVE) {
		/*Exception packets to network stack*/
		skb->dev = pdata->dev;
		skb_record_rx_queue(skb, NTN_RX_DMA_CH_0);
		skb->protocol = eth_type_trans(skb, skb->dev);
		ip_hdr = (struct iphdr *)(skb_mac_header(skb) + ETH_HLEN);

		/* Submit packet to network stack */
		/* If its a ping packet submit it via rx_ni else use rx */
		if (ip_hdr->protocol == IPPROTO_ICMP) {
			stat = netif_rx_ni(skb);
		} else if ((pdata->dev->stats.rx_packets %
				IPA_ETH_RX_SOFTIRQ_THRESH) == 0){
			stat = netif_rx_ni(skb);
		} else {
			stat = netif_rx(skb);
		}

		if(stat == NET_RX_DROP) {
			pdata->dev->stats.rx_dropped++;
		} else {
			/* Update Statistics */
			pdata->ipa_stats.ipa_ul_exception++;
			pdata->dev->last_rx = jiffies;
			pdata->dev->stats.rx_packets++;
			pdata->dev->stats.rx_bytes += skb->len;
		}
	} else {
		NMSGPR_ERR("Unhandled Evt %d ",evt);
		dev_kfree_skb_any(skb);
		skb = NULL;
		pdata->dev->stats.tx_dropped++;
	}
}

/**
 * DWC_ETH_QOS_ipa_offload_init() - Called from NTN driver to initialize IPA
 * offload data path.
 * This function will add partial headers and register the interface with IPA.
 *
 * IN: @pdata: NTN private structure handle that will be passed by IPA.
 * OUT: 0 on success and -1 on failure
 */
int DWC_ETH_QOS_ipa_offload_init(struct DWC_ETH_QOS_prv_data *pdata)
{
	struct ipa_uc_offload_intf_params in;
	struct ipa_uc_offload_out_params out;
	struct DWC_ETH_QOS_prv_ipa_data *ntn_ipa = &pdata->prv_ipa;
	struct ethhdr ntn_l2_hdr_v4;
	struct ethhdr ntn_l2_hdr_v6;
	struct vlan_ethhdr ntn_vlan_hdr_v4;
	struct vlan_ethhdr ntn_vlan_hdr_v6;
	int ret;

	if(!pdata) {
		NMSGPR_ERR("%s: Null Param\n", __func__);
		return -1;
	}

	memset(&in, 0, sizeof(in));
	memset(&out, 0, sizeof(out));

	/* Building ETH Header */
	if ( !pdata->prv_ipa.vlan_id ) {
		memset(&ntn_l2_hdr_v4, 0, sizeof(ntn_l2_hdr_v4));
		memset(&ntn_l2_hdr_v6, 0, sizeof(ntn_l2_hdr_v6));
		memcpy(&ntn_l2_hdr_v4.h_source, pdata->dev->dev_addr, ETH_ALEN);
		ntn_l2_hdr_v4.h_proto = htons(ETH_P_IP);
		memcpy(&ntn_l2_hdr_v6.h_source, pdata->dev->dev_addr, ETH_ALEN);
		ntn_l2_hdr_v6.h_proto = htons(ETH_P_IPV6);
		in.hdr_info[0].hdr = (u8 *)&ntn_l2_hdr_v4;
		in.hdr_info[0].hdr_len = ETH_HLEN;
		in.hdr_info[1].hdr = (u8 *)&ntn_l2_hdr_v6;
		in.hdr_info[1].hdr_len = ETH_HLEN;
	}
	if ( pdata->prv_ipa.vlan_id > MIN_VLAN_ID && pdata->prv_ipa.vlan_id <= MAX_VLAN_ID ) {
		memset(&ntn_vlan_hdr_v4, 0, sizeof(ntn_vlan_hdr_v4));
		memset(&ntn_vlan_hdr_v6, 0, sizeof(ntn_vlan_hdr_v6));
		memcpy(&ntn_vlan_hdr_v4.h_source, pdata->dev->dev_addr, ETH_ALEN);
		ntn_vlan_hdr_v4.h_vlan_proto = htons(ETH_P_8021Q);
		ntn_vlan_hdr_v4.h_vlan_encapsulated_proto = htons(ETH_P_IP);
		ntn_vlan_hdr_v4.h_vlan_TCI = htons(pdata->prv_ipa.vlan_id);
		in.hdr_info[0].hdr = (u8 *)&ntn_vlan_hdr_v4;
		in.hdr_info[0].hdr_len = VLAN_ETH_HLEN;
		memcpy(&ntn_vlan_hdr_v6.h_source, pdata->dev->dev_addr, ETH_ALEN);
		ntn_vlan_hdr_v6.h_vlan_proto = htons(ETH_P_8021Q);
		ntn_vlan_hdr_v6.h_vlan_encapsulated_proto = htons(ETH_P_IPV6);
		ntn_vlan_hdr_v6.h_vlan_TCI = htons(pdata->prv_ipa.vlan_id);
		in.hdr_info[1].hdr = (u8 *)&ntn_vlan_hdr_v6;
		in.hdr_info[1].hdr_len = VLAN_ETH_HLEN;
	}

	/* Building IN params */
	in.netdev_name = pdata->dev->name;
	in.priv = pdata;
	in.notify = ntn_ipa_notify_cb;
	in.proto = IPA_UC_NTN;
	in.hdr_info[0].dst_mac_addr_offset = 0;
	in.hdr_info[0].hdr_type = IPA_HDR_L2_ETHERNET_II;
	in.hdr_info[1].dst_mac_addr_offset = 0;
	in.hdr_info[1].hdr_type = IPA_HDR_L2_ETHERNET_II;

	ret = ipa_uc_offload_reg_intf(&in, &out);
	if (ret) {
		NMSGPR_ERR("Could not register offload interface ret %d \n",ret);
		return -1;
	}
	ntn_ipa->ipa_client_hndl = out.clnt_hndl;
	NMSGPR_INFO("Recevied IPA Offload Client Handle %d",ntn_ipa->ipa_client_hndl);
	return 0;
}

/**
 * DWC_ETH_QOS_ipa_offload_cleanup() - Called from NTN driver to cleanup IPA
 * offload data path.
 *
 * IN: @pdata: NTN dirver private structure.
 * OUT: 0 on success and -1 on failure
 */
int DWC_ETH_QOS_ipa_offload_cleanup(struct DWC_ETH_QOS_prv_data *pdata)
{
	struct DWC_ETH_QOS_prv_ipa_data *ntn_ipa = &pdata->prv_ipa;
	int ret = 0;

	if(!pdata) {
		NMSGPR_ERR( "Null Param %s \n", __func__);
		return -1;
	}

	if (!ntn_ipa->ipa_client_hndl) {
		NMSGPR_ERR("DWC_ETH_QOS_ipa_offload_cleanup called with NULL"
				" IPA client handle \n");
		return -1;
	}

	ret = ipa_uc_offload_cleanup(ntn_ipa->ipa_client_hndl);
	if (ret) {
		NMSGPR_ERR("Could not cleanup IPA Offload ret %d\n",ret);
		return -1;
	}

	return 0;
}

/**
 * DWC_ETH_QOS_ipa_offload_connect() - Called from NTN driver to connect IPA
 * offload data path. This function should be called from NTN driver after
 * allocation of rings and resources required for offload data path.
 *
 * After this function is called host driver should be ready to receive
 * any packets send by IPA.
 *
 * IN: @pdata: NTN private structure handle that will be passed by IPA.
 * OUT: 0 on success and -1 on failure
 */
int DWC_ETH_QOS_ipa_offload_connect(struct DWC_ETH_QOS_prv_data *pdata)
{
	struct DWC_ETH_QOS_prv_ipa_data *ntn_ipa = &pdata->prv_ipa;
	struct ipa_uc_offload_conn_in_params in;
	struct ipa_uc_offload_conn_out_params out;
	struct ipa_ntn_setup_info ul;
	struct ipa_ntn_setup_info dl;
	struct ipa_rm_perf_profile profile;
	int ret = 0;

	if(!pdata) {
		NMSGPR_ERR( "Null Param %s \n", __func__);
		return -1;
	}

	memset(&in, 0, sizeof(in));
	memset(&out, 0, sizeof(out));
	memset(&profile, 0, sizeof(profile));

	in.clnt_hndl = ntn_ipa->ipa_client_hndl;
	/* Uplink Setup */
	ul.client = IPA_CLIENT_ETHERNET_PROD;
	ul.ring_base_pa = (phys_addr_t)GET_RX_DESC_DMA_ADDR(NTN_RX_DMA_CH_0, 0);
	ul.ntn_ring_size = pdata->rx_dma_ch[NTN_RX_DMA_CH_0].desc_cnt;
	ul.buff_pool_base_pa = virt_to_phys(GET_RX_BUFF_POOL_BASE_ADRR(NTN_RX_DMA_CH_0));
	ul.num_buffers = pdata->rx_dma_ch[NTN_RX_DMA_CH_0].desc_cnt - 1;
	ul.data_buff_size = DWC_ETH_QOS_ETH_FRAME_LEN_IPA;
	ul.ntn_reg_base_ptr_pa = (phys_addr_t)NTN_REG_PHY_BASE_ADRS;

	/* Downlink Setup */
	dl.client = IPA_CLIENT_ETHERNET_CONS;
	dl.ring_base_pa = (phys_addr_t)GET_TX_DESC_DMA_ADDR(NTN_TX_DMA_CH_2, 0);
	dl.ntn_ring_size = pdata->tx_dma_ch[NTN_TX_DMA_CH_2].desc_cnt;
	dl.buff_pool_base_pa = virt_to_phys(GET_TX_BUFF_DMA_POOL_BASE_ADRR(NTN_TX_DMA_CH_2));
	dl.num_buffers = pdata->tx_dma_ch[NTN_TX_DMA_CH_2].desc_cnt - 1;
	dl.data_buff_size = DWC_ETH_QOS_ETH_FRAME_LEN_IPA;
	dl.ntn_reg_base_ptr_pa = (phys_addr_t)NTN_REG_PHY_BASE_ADRS;

	/* Dump UL and DL Setups */
	NMSGPR_INFO("IPA Offload UL client %d ring_base_pa %x ntn_ring_size %d buff_pool_base_pa %x num_buffers %d data_buff_size %d ntn_reg_base_ptr_pa %x ntn_reg_base_ptr %p \n",
		ul.client, ul.ring_base_pa, ul.ntn_ring_size, ul.buff_pool_base_pa, ul.num_buffers, ul.data_buff_size, ul.ntn_reg_base_ptr_pa, DMA_RXCH_DESC_TAILPTR_RgOffAddr(NTN_RX_DMA_CH_0));
	NMSGPR_INFO("IPA Offload DL client %d ring_base_pa %x ntn_ring_size %d buff_pool_base_pa %x num_buffers %d data_buff_size %d ntn_reg_base_ptr_pa %x ntn_reg_base_ptr %p \n",
		dl.client, dl.ring_base_pa, dl.ntn_ring_size, dl.buff_pool_base_pa, dl.num_buffers, dl.data_buff_size, dl.ntn_reg_base_ptr_pa, DMA_TXCH_DESC_TAILPTR_RgOffAddr(NTN_TX_DMA_CH_2));

	in.u.ntn.ul = ul;
	in.u.ntn.dl = dl;

	ret = ipa_uc_offload_conn_pipes(&in, &out);
	if (ret) {
		NMSGPR_ERR("Could not connect IPA Offload Pipes %d\n", ret);
		return -1;
	}

        ntn_ipa->uc_db_rx_addr = out.u.ntn.ul_uc_db_pa;
        ntn_ipa->uc_db_tx_addr = out.u.ntn.dl_uc_db_pa;

	/* Set Perf Profile For PROD/CONS Pipes */
	profile.max_supported_bandwidth_mbps = NTN_IPA_PIPE_MAX_BW;
	ret = ipa_rm_set_perf_profile (IPA_RM_RESOURCE_ETHERNET_PROD,
					&profile);
	if (ret) {
		NMSGPR_ERR("Err to set BW: IPA_RM_RESOURCE_ETHERNET_PROD err:%d\n",
				ret);
		return -1;
	}
	ret = ipa_rm_set_perf_profile (IPA_RM_RESOURCE_ETHERNET_CONS,
					&profile);
	if (ret) {
		NMSGPR_ERR("Err to set BW: IPA_RM_RESOURCE_ETHERNET_CONS err:%d\n",
				ret);
		return -1;
	}
	__pm_stay_awake(&pdata->prv_ipa.wlock);

	return 0;
}

/**
 * DWC_ETH_QOS_ipa_offload_disconnect() - Called from NTN driver to disconnect IPA
 * offload data path. This function should be called from NTN driver before
 * de-allocation of any ring resources.
 *
 * After this function is successful, NTN is free to de-allocate the IPA controlled
 * DMA rings.
 *
 * IN: @pdata: NTN dirver private structure.
 * OUT: 0 on success and -1 on failure
 */
int DWC_ETH_QOS_ipa_offload_disconnect(struct DWC_ETH_QOS_prv_data *pdata)
{
	struct DWC_ETH_QOS_prv_ipa_data *ntn_ipa = &pdata->prv_ipa;
	int ret = 0;

	if(!pdata) {
		NMSGPR_ERR( "Null Param %s \n", __func__);
		return -1;
	}

	NMSGPR_INFO("%s - begin \n",__func__);
	ret = ipa_uc_offload_disconn_pipes(ntn_ipa->ipa_client_hndl);
	if (ret) {
		NMSGPR_ERR("Could not cleanup IPA Offload ret %d\n",ret);
		return ret;
	}
	__pm_relax(&pdata->prv_ipa.wlock);

	NMSGPR_INFO("%s - end \n",__func__);
	return 0;
}

/**
 * read_ipa_stats() - Debugfs read command for IPA statistics
 *
 */
static ssize_t read_ipa_stats(struct file *file,
	char __user *user_buf, size_t count, loff_t *ppos)
{
	struct DWC_ETH_QOS_prv_data *pdata = file->private_data;
	struct DWC_ETH_QOS_prv_ipa_data *ntn_ipa = &pdata->prv_ipa;
	char *buf;
	unsigned int len = 0, buf_len = 2000;
	ssize_t ret_cnt;

	if (!pdata || !ntn_ipa) {
		NMSGPR_ERR(" %s NULL Pointer \n",__func__);
		return -EINVAL;
	}

	buf = kzalloc(buf_len, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	len += scnprintf(buf + len, buf_len - len, "\n \n");
	len += scnprintf(buf + len, buf_len - len, "%25s\n",
		"NTN IPA Stats");
	len += scnprintf(buf + len, buf_len - len, "%25s\n\n",
		"==================================================");

	len += scnprintf(buf + len, buf_len - len, "%-25s %10llu\n",
		"IPA RX Packets: ", pdata->ipa_stats.ipa_ul_exception);
	len += scnprintf(buf + len, buf_len - len, "\n");

	len += scnprintf(buf + len, buf_len - len, "%-25s %10d\n",
		"NTN FW Support IPA: ", pdata->hw_if.ntn_fw_ipa_supported(pdata));
	len += scnprintf(buf + len, buf_len - len, "\n");

	len += scnprintf(buf + len, buf_len - len, "%-25s 0x%10x\n",
		"FW Version: ", pdata->hw_if.read_fw_ver_features(pdata));
	len += scnprintf(buf + len, buf_len - len, "\n");

	if (len > buf_len)
		len = buf_len;

	ret_cnt = simple_read_from_buffer(user_buf, count, ppos, buf, len);
	kfree(buf);
	return ret_cnt;
}

void DWC_ETH_QOS_ipa_stats_read(struct DWC_ETH_QOS_prv_data *pdata)
{
	struct hw_if_struct *hw_if = &pdata->hw_if;
	struct DWC_ETH_QOS_ipa_stats *dma_stats = &pdata->ipa_stats;
	UINT data;

	dma_stats->ipa_rx_Desc_Ring_Base = GET_RX_DESC_DMA_ADDR(NTN_RX_DMA_CH_0, 0);
	dma_stats->ipa_rx_Desc_Ring_Size = pdata->rx_dma_ch[NTN_RX_DMA_CH_0].desc_cnt;
	dma_stats->ipa_rx_Buff_Ring_Base =
		virt_to_phys(GET_RX_BUFF_POOL_BASE_ADRR(NTN_RX_DMA_CH_0));
	dma_stats->ipa_rx_Buff_Ring_Size = pdata->rx_dma_ch[NTN_RX_DMA_CH_0].desc_cnt - 1;
	dma_stats->ipa_rx_Db_Int_Raised =
		hw_if->ntn_reg_rd((NTN_M3_DBG_CNT_START + NTN_M3_DBG_DMA_RXCH0_DB_OFST),
						 PCIE_SRAM_BAR_NUM, pdata);

	DMA_RXCH_CUR_DESC_RgRd(NTN_RX_DMA_CH_0, data);
	dma_stats->ipa_rx_Cur_Desc_Ptr_Indx = GET_RX_DESC_IDX(NTN_RX_DMA_CH_0, data);
	DMA_RXCH_DESC_TAILPTR_RgRd(NTN_RX_DMA_CH_0, data);
	dma_stats->ipa_rx_Tail_Ptr_Indx = GET_RX_DESC_IDX(NTN_RX_DMA_CH_0, data);

	DMA_RXCHSTS_RgRd(NTN_RX_DMA_CH_0, data);
	dma_stats->ipa_rx_DMA_Status = data;
	dma_stats->ipa_rx_DMA_Ch_Status =
		GET_VALUE(data, DMA_RXCHSTS_CHSTS_LPOS, DMA_RXCHSTS_CHSTS_HPOS);
	dma_stats->ipa_rx_DMA_Ch_underflow =
		GET_VALUE(data, DMA_RXCHSTS_UNF_LPOS, DMA_RXCHSTS_UNF_HPOS);
	dma_stats->ipa_rx_DMA_Ch_stopped =
		GET_VALUE(data, DMA_RXCHSTS_RS_LPOS, DMA_RXCHSTS_RS_HPOS);
	dma_stats->ipa_rx_DMA_Ch_complete =
		GET_VALUE(data, DMA_RXCHSTS_RC_LPOS, DMA_RXCHSTS_RC_HPOS);

	DMA_RXCHINTMASK_RgRd(NTN_RX_DMA_CH_0, dma_stats->ipa_rx_Int_Mask);
	DMA_RXCHINTMASK_RCEN_UdfRd(NTN_RX_DMA_CH_0,
				dma_stats->ipa_rx_Transfer_Complete_irq);
	DMA_RXCHINTMASK_RSEN_UdfRd(NTN_RX_DMA_CH_0,
				dma_stats->ipa_rx_Transfer_Stopped_irq);
	DMA_RXCHINTMASK_UNFEN_UdfRd(NTN_RX_DMA_CH_0, dma_stats->ipa_rx_Underflow_irq);
	DMA_RXCHINTMASK_FEEN_UdfRd(NTN_RX_DMA_CH_0,
				dma_stats->ipa_rx_Early_Trans_Comp_irq);

	dma_stats->ipa_tx_Desc_Ring_Base = GET_TX_DESC_DMA_ADDR(NTN_TX_DMA_CH_2, 0);
	dma_stats->ipa_tx_Desc_Ring_Size = pdata->tx_dma_ch[NTN_TX_DMA_CH_2].desc_cnt;
	dma_stats->ipa_tx_Buff_Ring_Base =
		virt_to_phys(GET_TX_BUFF_DMA_POOL_BASE_ADRR(NTN_TX_DMA_CH_2));
	dma_stats->ipa_tx_Buff_Ring_Size = pdata->tx_dma_ch[NTN_TX_DMA_CH_2].desc_cnt - 1;
	dma_stats->ipa_tx_Db_Int_Raised =
		hw_if->ntn_reg_rd((NTN_M3_DBG_CNT_START + NTN_M3_DBG_DMA_TXCH2_DB_OFST),
						  PCIE_SRAM_BAR_NUM, pdata);
	DMA_TXCH_CUR_DESC_RgRd(NTN_TX_DMA_CH_2, data);
	dma_stats->ipa_tx_Curr_Desc_Ptr_Indx = GET_TX_DESC_IDX(NTN_TX_DMA_CH_2, data);
	DMA_TXCH_DESC_TAILPTR_RgRd(NTN_TX_DMA_CH_2, data);
	dma_stats->ipa_tx_Tail_Ptr_Indx = GET_TX_DESC_IDX(NTN_TX_DMA_CH_2, data);


	DMA_TXCHSTS_RgRd(NTN_TX_DMA_CH_2, data);
	dma_stats->ipa_tx_DMA_Status = data;
	dma_stats->ipa_tx_DMA_Ch_Status =
		GET_VALUE(data, DMA_TXCHSTS_CHSTS_LPOS, DMA_TXCHSTS_CHSTS_HPOS);
	dma_stats->ipa_tx_DMA_Ch_underflow =
		GET_VALUE(data, DMA_TXCHSTS_UNF_LPOS, DMA_TXCHSTS_UNF_HPOS);
	dma_stats->ipa_tx_DMA_Transfer_stopped =
		GET_VALUE(data, DMA_TXCHSTS_TS_LPOS, DMA_TXCHSTS_TS_HPOS);
	dma_stats->ipa_tx_DMA_Transfer_complete =
		GET_VALUE(data, DMA_TXCHSTS_TC_LPOS, DMA_TXCHSTS_TC_HPOS);

	DMA_TXCHINTMASK_RgRd(NTN_TX_DMA_CH_2, dma_stats->ipa_tx_Int_Mask);
	DMA_TXCHINTMASK_TCEN_UdfRd(NTN_TX_DMA_CH_2,
				dma_stats->ipa_tx_Transfer_Complete_irq);
	DMA_TXCHINTMASK_TSEN_UdfRd(NTN_TX_DMA_CH_2,
				dma_stats->ipa_tx_Transfer_Stopped_irq);
	DMA_TXCHINTMASK_UNFEN_UdfRd(NTN_TX_DMA_CH_2,
				dma_stats->ipa_tx_Underflow_irq);
	DMA_TXCHINTMASK_ETCEN_UdfRd(NTN_TX_DMA_CH_2,
				dma_stats->ipa_tx_Early_Trans_Cmp_irq);
	DMA_TXCHINTMASK_FEEN_UdfRd(NTN_TX_DMA_CH_2, dma_stats->ipa_tx_Fatal_err_irq);
	DMA_TXCHINTMASK_CDEE_UdfRd(NTN_TX_DMA_CH_2, dma_stats->ipa_tx_Desc_Err_irq);
}

/**
 * read_ntn_dma_stats() - Debugfs read command for NTN DMA statistics
 * Only read DMA Stats for IPA Control Channels
 *
 */
static ssize_t read_ntn_dma_stats(struct file *file,
	char __user *user_buf, size_t count, loff_t *ppos)
{
	struct DWC_ETH_QOS_prv_data *pdata = file->private_data;
	struct DWC_ETH_QOS_prv_ipa_data *ntn_ipa = &pdata->prv_ipa;
	struct DWC_ETH_QOS_ipa_stats *dma_stats = &pdata->ipa_stats;
	char *buf;
	unsigned int len = 0, buf_len = 3000;
	ssize_t ret_cnt;

	buf = kzalloc(buf_len, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	DWC_ETH_QOS_ipa_stats_read(pdata);

	len += scnprintf(buf + len, buf_len - len, "\n \n");
	len += scnprintf(buf + len, buf_len - len, "%25s\n",
		"NTN DMA Stats");
	len += scnprintf(buf + len, buf_len - len, "%25s\n\n",
		"==================================================");

	len += scnprintf(buf + len, buf_len - len, "%-50s 0x%x\n",
		"RX Desc Ring Base: ", dma_stats->ipa_rx_Desc_Ring_Base);
	len += scnprintf(buf + len, buf_len - len, "%-50s %10d\n",
		"RX Desc Ring Size: ", dma_stats->ipa_rx_Desc_Ring_Size);
	len += scnprintf(buf + len, buf_len - len, "%-50s 0x%x\n",
		"RX Buff Ring Base: ", dma_stats->ipa_rx_Buff_Ring_Base);
	len += scnprintf(buf + len, buf_len - len, "%-50s %10d\n",
		"RX Buff Ring Size: ", dma_stats->ipa_rx_Buff_Ring_Size);
	len += scnprintf(buf + len, buf_len - len, "%-50s %10u\n",
		"RX Doorbell Interrupts Raised: ", dma_stats->ipa_rx_Db_Int_Raised);
	len += scnprintf(buf + len, buf_len - len, "%-50s %10d\n",
		"RX Current Desc Pointer Index: ", dma_stats->ipa_rx_Cur_Desc_Ptr_Indx);
	len += scnprintf(buf + len, buf_len - len, "%-50s %10d\n",
		"RX Tail Pointer Index: ", dma_stats->ipa_rx_Tail_Ptr_Indx);
	len += scnprintf(buf + len, buf_len - len, "%-50s 0x%x\n",
		"RX Doorbell Address: ", ntn_ipa->uc_db_rx_addr);
	len += scnprintf(buf + len, buf_len - len, "\n");

	len += scnprintf(buf + len, buf_len - len, "%-50s 0x%x\n",
		"RX DMA Status: ", dma_stats->ipa_rx_DMA_Status);
	len += scnprintf(buf + len, buf_len - len, "%-50s %10s\n",
		"RX DMA Status - RX DMA CH Status : ",
			rx_ch_status_string[dma_stats->ipa_rx_DMA_Ch_Status]);
	len += scnprintf(buf + len, buf_len - len, "%-50s %10s\n",
		"RX DMA Status - RX DMA Underflow : ",
			bit_status_string[dma_stats->ipa_rx_DMA_Ch_underflow]);
	len += scnprintf(buf + len, buf_len - len, "%-50s %10s\n",
		"RX DMA Status - RX DMA Stopped : ",
			bit_status_string[dma_stats->ipa_rx_DMA_Ch_stopped]);
	len += scnprintf(buf + len, buf_len - len, "%-50s %10s\n",
		"RX DMA Status - RX DMA Complete : ",
			bit_status_string[dma_stats->ipa_rx_DMA_Ch_complete]);
	len += scnprintf(buf + len, buf_len - len, "\n");


	len += scnprintf(buf + len, buf_len - len, "%-50s 0x%x\n",
		"RX DMA CH0 INT Mask: ", dma_stats->ipa_rx_Int_Mask);
	len += scnprintf(buf + len, buf_len - len, "%-50s %10s\n",
		"RXDMACH0 INTMASK - Transfer Complete IRQ : ",
			bit_mask_string[dma_stats->ipa_rx_Transfer_Complete_irq]);
	len += scnprintf(buf + len, buf_len - len, "%-50s %10s\n",
		"RXDMACH0 INTMASK - Transfer Stopped IRQ : ",
			bit_mask_string[dma_stats->ipa_rx_Transfer_Stopped_irq]);
	len += scnprintf(buf + len, buf_len - len, "%-50s %10s\n",
		"RXDMACH0 INTMASK - Underflow IRQ : ",
			bit_mask_string[dma_stats->ipa_rx_Underflow_irq]);
	len += scnprintf(buf + len, buf_len - len, "%-50s %10s\n",
		"RXDMACH0 INTMASK - Early Transmit Complete IRQ : ",
			bit_mask_string[dma_stats->ipa_rx_Early_Trans_Comp_irq]);
	len += scnprintf(buf + len, buf_len - len, "\n");

	len += scnprintf(buf + len, buf_len - len, "%-50s 0x%x\n",
		"TX Desc Ring Base: ", dma_stats->ipa_tx_Desc_Ring_Base);
	len += scnprintf(buf + len, buf_len - len, "%-50s %10d\n",
		"TX Desc Ring Size: ", dma_stats->ipa_tx_Desc_Ring_Size);
	len += scnprintf(buf + len, buf_len - len, "%-50s 0x%x\n",
		"TX Buff Ring Base: ", dma_stats->ipa_tx_Buff_Ring_Base);
	len += scnprintf(buf + len, buf_len - len, "%-50s %10d\n",
		"TX Buff Ring Size: ", dma_stats->ipa_tx_Buff_Ring_Size);
	len += scnprintf(buf + len, buf_len - len, "%-50s %10u\n",
		"TX Doorbell Interrupts Raised: ", dma_stats->ipa_tx_Db_Int_Raised);
	len += scnprintf(buf + len, buf_len - len, "%-50s %10lu\n",
		"TX Current Desc Pointer Index: ", dma_stats->ipa_tx_Curr_Desc_Ptr_Indx);

	len += scnprintf(buf + len, buf_len - len, "%-50s %10lu\n",
		"TX Tail Pointer Index: ", dma_stats->ipa_tx_Tail_Ptr_Indx);
	len += scnprintf(buf + len, buf_len - len, "%-50s 0x%x\n",
		"TX Doorbell Address: ", ntn_ipa->uc_db_tx_addr);
	len += scnprintf(buf + len, buf_len - len, "\n");

	len += scnprintf(buf + len, buf_len - len, "%-50s 0x%x\n",
		"TX DMA Status: ", dma_stats->ipa_tx_DMA_Status);

	len += scnprintf(buf + len, buf_len - len, "%-50s %10s\n",
		"TX DMA Status - TX DMA CH Status : ",
			tx_ch_status_string[dma_stats->ipa_tx_DMA_Ch_Status]);
	len += scnprintf(buf + len, buf_len - len, "%-50s %10s\n",
		"TX DMA Status - TX DMA Underflow : ",
				bit_status_string[dma_stats->ipa_tx_DMA_Ch_underflow]);
	len += scnprintf(buf + len, buf_len - len, "%-50s %10s\n",
		"TX DMA Status - TX DMA Transfer Stopped : ",
				bit_status_string[dma_stats->ipa_tx_DMA_Transfer_stopped]);
	len += scnprintf(buf + len, buf_len - len, "%-50s %10s\n",
		"TX DMA Status - TX DMA Transfer Complete : ",
				bit_status_string[dma_stats->ipa_tx_DMA_Transfer_complete]);
	len += scnprintf(buf + len, buf_len - len, "\n");

	len += scnprintf(buf + len, buf_len - len, "%-50s 0x%x\n",
		"TX DMA CH2 INT Mask: ", dma_stats->ipa_tx_Int_Mask);
	len += scnprintf(buf + len, buf_len - len, "%-50s %10s\n",
		"TXDMACH2 INTMASK - Transfer Complete IRQ : ",
				bit_mask_string[dma_stats->ipa_tx_Transfer_Complete_irq]);
	len += scnprintf(buf + len, buf_len - len, "%-50s %10s\n",
		"TXDMACH2 INTMASK - Transfer Stopped IRQ : ",
				bit_mask_string[dma_stats->ipa_tx_Transfer_Stopped_irq]);
	len += scnprintf(buf + len, buf_len - len, "%-50s %10s\n",
		"TXDMACH2 INTMASK - Underflow IRQ : ", bit_mask_string[dma_stats->ipa_tx_Underflow_irq]);
	len += scnprintf(buf + len, buf_len - len, "%-50s %10s\n",
		"TXDMACH2 INTMASK - Early Transmit Complete IRQ : ",
				bit_mask_string[dma_stats->ipa_tx_Early_Trans_Cmp_irq]);
	len += scnprintf(buf + len, buf_len - len, "%-50s %10s\n",
		"TXDMACH2 INTMASK - Fatal Bus Error IRQ : ",
				bit_mask_string[dma_stats->ipa_tx_Fatal_err_irq]);
	len += scnprintf(buf + len, buf_len - len, "%-50s %10s\n",
		"TXDMACH2 INTMASK - CNTX Desc Error IRQ : ",
				bit_mask_string[dma_stats->ipa_tx_Desc_Err_irq]);

	if (len > buf_len)
		len = buf_len;

	ret_cnt = simple_read_from_buffer(user_buf, count, ppos, buf, len);
	kfree(buf);
	return ret_cnt;
}

static ssize_t read_ipa_offload_status(struct file *file,
	char __user *user_buf, size_t count, loff_t *ppos)
{
	unsigned int len = 0, buf_len = NTN_IPA_DBG_MAX_MSG_LEN;
	struct DWC_ETH_QOS_prv_data *pdata = file->private_data;

	if (pdata->prv_ipa.ipa_offload_susp)
		len += scnprintf(buf + len, buf_len - len, "IPA Offload suspended\n");
	else
		len += scnprintf(buf + len, buf_len - len, "IPA Offload enabled\n");

	if (len > buf_len)
		len = buf_len;

	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
}

static ssize_t suspend_resume_ipa_offload(struct file *file,
	const char __user *user_buf, size_t count, loff_t *ppos)
{
	s8 option = 0;
	char in_buf[2];
	unsigned long ret;
	struct DWC_ETH_QOS_prv_data *pdata = file->private_data;

	if (sizeof(in_buf) < 2)
		return -EFAULT;

	ret = copy_from_user(in_buf, user_buf, 1);
	if (ret)
		return -EFAULT;

	in_buf[1] = '\0';
	if (kstrtos8(in_buf, 0, &option))
		return -EFAULT;

	if (option == 1 && !pdata->prv_ipa.ipa_offload_susp)
		DWC_ETH_QOS_ipa_offload_suspend(pdata);
	else if (option == 0 && pdata->prv_ipa.ipa_offload_susp)
		DWC_ETH_QOS_ipa_offload_resume(pdata);

	return count;
}


static const struct file_operations fops_ipa_stats = {
	.read = read_ipa_stats,
	.open = simple_open,
	.owner = THIS_MODULE,
	.llseek = default_llseek,
};

static const struct file_operations fops_ntn_dma_stats = {
	.read = read_ntn_dma_stats,
	.open = simple_open,
	.owner = THIS_MODULE,
	.llseek = default_llseek,
};

static const struct file_operations fops_ntn_ipa_offload_en = {
	.read = read_ipa_offload_status,
	.write = suspend_resume_ipa_offload,
	.open = simple_open,
	.owner = THIS_MODULE,
	.llseek = default_llseek,
};
/**
 * DWC_ETH_QOS_ipa_create_debugfs() - Called from NTN driver to create debugfs node
 * for offload data path debugging.
 *
 * IN: @pdata: NTN dirver private structure.
 * OUT: 0 on success and -1 on failure
 */
int DWC_ETH_QOS_ipa_create_debugfs(struct DWC_ETH_QOS_prv_data *pdata)
{
	struct DWC_ETH_QOS_prv_ipa_data *ntn_ipa = &pdata->prv_ipa;
	static struct dentry *stats = NULL;

	if(!pdata) {
		NMSGPR_ERR( "Null Param %s \n", __func__);
		return -1;
	}

	ntn_ipa->debugfs_dir = debugfs_create_dir("eth", NULL);
	if (!ntn_ipa->debugfs_dir) {
		NMSGPR_ERR( "Cannot create debugfs dir %d \n", (int)ntn_ipa->debugfs_dir);
		return -ENOMEM;
	}

	stats = debugfs_create_file("ipa_stats", S_IRUSR, ntn_ipa->debugfs_dir,
				pdata, &fops_ipa_stats);
	if (!stats || IS_ERR(stats)) {
		NMSGPR_ERR( "Cannot create debugfs ipa_stats %d \n", (int)stats);
		goto fail;
	}

	stats = debugfs_create_file("dma_stats", S_IRUSR, ntn_ipa->debugfs_dir,
				pdata, &fops_ntn_dma_stats);
	if (!stats || IS_ERR(stats)) {
		NMSGPR_ERR( "Cannot create debugfs dma_stats %d \n", (int)stats);
		goto fail;
	}

	stats = debugfs_create_file("suspend_ipa_offload", (S_IRUSR|S_IWUSR),
				ntn_ipa->debugfs_dir, pdata, &fops_ntn_ipa_offload_en);
	if (!stats || IS_ERR(stats)) {
		NMSGPR_ERR( "Cannot create debugfs ipa_offload_en %d \n", (int)stats);
		goto fail;
	}
	return 0;

fail:
	debugfs_remove_recursive(ntn_ipa->debugfs_dir);
	return -ENOMEM;
}

/**
 * DWC_ETH_QOS_ipa_cleanup_debugfs() - Called from NTN driver to cleanup debugfs node
 * for offload data path debugging.
 *
 * IN: @pdata: NTN dirver private structure.
 * OUT: 0 on success and -1 on failure
 */
int DWC_ETH_QOS_ipa_cleanup_debugfs(struct DWC_ETH_QOS_prv_data *pdata)
{
	struct DWC_ETH_QOS_prv_ipa_data *ntn_ipa;

	if(!pdata) {
		NMSGPR_ERR("Null Param %s \n", __func__);
		return -1;
	}

	ntn_ipa = &pdata->prv_ipa;
	if (ntn_ipa->debugfs_dir)
		debugfs_remove_recursive(ntn_ipa->debugfs_dir);

	NDBGPR_L1("IPA debugfs Deleted Successfully \n");
	return 0;
}
int DWC_ETH_QOS_ipa_ready(struct DWC_ETH_QOS_prv_data *pdata)
{
	int ret = 0;

	if (pdata->prv_ipa.ipa_ver >= IPA_HW_v3_0) {
		ret = ipa_register_ipa_ready_cb(DWC_ETH_QOS_ipa_ready_cb,
										(void *)pdata);
		if (ret == -ENXIO) {
			NMSGPR_ERR("%s: IPA driver context is not even ready\n", __func__);
			pdata->prv_ipa.ipa_ready = false;
			return ret;
		}

		if (ret != -EEXIST) {
			NDBGPR_L1("%s:%d register ipa ready cb\n", __func__, __LINE__);
			return ret;
		}
	}

	pdata->prv_ipa.ipa_ready = true;
	NDBGPR_L1("%s:%d ipa ready\n", __func__, __LINE__);
	ret = DWC_ETH_QOS_ipa_uc_ready(pdata);

	return ret;
}
