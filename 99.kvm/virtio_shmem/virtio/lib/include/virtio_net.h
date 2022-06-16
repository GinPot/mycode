/*
 * $QNXLicenseC:
 * Copyright 2015, QNX Software Systems. All Rights Reserved.
 *
 * You must obtain a written license from and pay applicable license fees to QNX
 * Software Systems before you may reproduce, modify or distribute this software,
 * or any work that includes all or part of this software.   Free development
 * licenses are available for evaluation and non-commercial purposes.  For more
 * information visit http://licensing.qnx.com or email licensing@qnx.com.
 *
 * This file may contain contributions from others.  Please review this entire
 * file for other proprietary rights or license notices, as well as the QNX
 * Development Suite License Guide at http://licensing.qnx.com/license-guide/
 * for other information.
 * $
 */

/**
 * @file
 * definitions for virtio-net devices
 * see http://docs.oasis-open.org/virtio/virtio/v1.0/virtio-v1.0.html
 */

#ifndef __VIRTIO_NET_H
#define __VIRTIO_NET_H

#include <virtio.h>

#define VIRTIO_NET_F_CSUM			0
#define VIRTIO_NET_F_GUEST_CSUM		1
#define VIRTIO_NET_F_CTRL_GUEST_OFFLOADS 2
#define VIRTIO_NET_F_MAC			5
#define VIRTIO_NET_F_GSO			6	/* legacy */
#define VIRTIO_NET_F_GUEST_TSO4		7
#define VIRTIO_NET_F_GUEST_TSO6		8
#define VIRTIO_NET_F_GUEST_ECN		9
#define VIRTIO_NET_F_GUEST_UFO		10
#define VIRTIO_NET_F_HOST_TSO4		11
#define VIRTIO_NET_F_HOST_TSO6		12
#define VIRTIO_NET_F_HOST_ECN		13
#define VIRTIO_NET_F_HOST_UFO		14
#define VIRTIO_NET_F_MRG_RXBUF		15
#define VIRTIO_NET_F_STATUS			16
#define VIRTIO_NET_F_CTRL_VQ		17
#define VIRTIO_NET_F_CTRL_RX		18
#define VIRTIO_NET_F_CTRL_VLAN		19
#define VIRTIO_NET_F_GUEST_ANNOUNCE	21
#define VIRTIO_NET_F_MQ				22
#define VIRTIO_NET_F_CTRL_MAC_ADDR	23

#define VIRTIO_NET_RECV_QIDX(port)	(2*(port)+0)
#define VIRTIO_NET_TRAN_QIDX(port)	(2*(port)+1)
#define VIRTIO_NET_CTRL_QIDX(maxports)	(2*((maxports)+1))

#define VIRTIO_NET_S_LINK_UP		1u
#define VIRTIO_NET_S_ANNOUNCE		2u

struct virtio_net_config {
	uint8_t	 	mac[6];
	uint16_t	status;
	uint16_t	max_virtqueue_pairs;
};

#define VIRTIO_NET_HDR_F_NEEDS_CSUM 1
#define VIRTIO_NET_HDR_F_DATA_VALID 2

#define VIRTIO_NET_HDR_GSO_NONE 0
#define VIRTIO_NET_HDR_GSO_TCPV4 1
#define VIRTIO_NET_HDR_GSO_UDP 3
#define VIRTIO_NET_HDR_GSO_TCPV6 4
#define VIRTIO_NET_HDR_GSO_ECN 0x80

struct virtio_net_hdr {
	uint8_t		flags;		/* VIRTIO_NET_HDR_F_* */
	uint8_t		gso_type;	/* VIRTIO_NET_HDR_GSO_* */
	uint16_t	hdr_len;
	uint16_t	gso_size;
	uint16_t	csum_start;
	uint16_t	csum_offset;
	uint16_t	num_buffers;
};


#define VIRTIO_NET_CTRL_RX			0u
 #define VIRTIO_NET_CTRL_RX_PROMISC		0u
 #define VIRTIO_NET_CTRL_RX_ALLMULTI	1u
 #define VIRTIO_NET_CTRL_RX_ALLUNI		2u
 #define VIRTIO_NET_CTRL_RX_NOMULTI		3u
 #define VIRTIO_NET_CTRL_RX_NOUNI		4u
 #define VIRTIO_NET_CTRL_RX_NOBCAST		5u

#define VIRTIO_NET_CTRL_MAC		1u
 #define VIRTIO_NET_CTRL_MAC_TABLE_SET	0u
 #define VIRTIO_NET_CTRL_MAC_ADDR_SET	1u

#define VIRTIO_NET_CTRL_VLAN	2u
 #define VIRTIO_NET_CTRL_VLAN_ADD		0u
 #define VIRTIO_NET_CTRL_VLAN_DEL		1u

#define VIRTIO_NET_CTRL_ANNOUNCE	3u
 #define VIRTIO_NET_CTRL_ANNOUNCE_ACK	0u

#define VIRTIO_NET_CTRL_MQ		4u
 #define VIRTIO_NET_CTRL_MQ_VQ_PAIRS_SET	0u
 #define VIRTIO_NET_CTRL_MQ_VQ_PAIRS_MIN	1u
 #define VIRTIO_NET_CTRL_MQ_VQ_PAIRS_MAX	0x8000u

#define VIRTIO_NET_CTRL_GUEST_OFFLOADS	5u
 #define VIRTIO_NET_CTRL_GUEST_OFFLOADS_SET	0u

#define VIRTIO_NET_OK	0u
#define VIRTIO_NET_ERR	1u

struct virtio_net_ctrl {
	uint8_t	class;
	uint8_t	command;
	uint8_t	command_specific_data[];
	/* uint8_t	ack; */	/* VIRTIO_NET_OK/ERR */
};

struct virtio_net_ctrl_mac {
	uint32_t	entries;
	uint8_t		macs[/*entries*/][6];
};

struct virtio_net_ctrl_mq {
	uint16_t	virtqueue_pairs;
};

struct virtio_net_ctrl_offloads {
	uint64_t	offloads;
};

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.0.0/trunk/lib/hyp/public/virtio_net.h $ $Rev: 816581 $")
#endif
