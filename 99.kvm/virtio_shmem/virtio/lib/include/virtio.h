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
 * definitions common to all virtio devices
 * see http://docs.oasis-open.org/virtio/virtio/v1.0/virtio-v1.0.html
 */

#ifndef __VIRTIO_H
#define __VIRTIO_H

#include <virtq.h>

enum virtio_device_type {
	VIRTIO_DT_RESERVED,
	VIRTIO_DT_NETWORK,
	VIRTIO_DT_BLOCK,
	VIRTIO_DT_CONSOLE,
	VIRTIO_DT_ENTROPY,
	VIRTIO_DT_MEMORY_BALLOON_TRADITIONAL,
	VIRTIO_DT_IOMEM,
	VIRTIO_DT_RPMSG,
	VIRTIO_DT_SCSI_HOST,
	VIRTIO_DT_9P,
	VIRTIO_DT_MAC80211_WLAN,
	VIRTIO_DT_RPROC_SERIAL,
	VIRTIO_DT_CAIF,
	VIRTIO_DT_MEMORY_BALLOON,
	VIRTIO_DT_UNUSED_ID14,
	VIRTIO_DT_UNUSED_ID15,
	VIRTIO_DT_GPU,
	VIRTIO_DT_TIMER,
	VIRTIO_DT_INPUT,
	VIRTIO_DT_NUM,

	VIRTIO_DT_QNX_PRIVATE=0xf000,
	VIRTIO_DT_QNX_AUDIO=VIRTIO_DT_QNX_PRIVATE,
};

extern uint16_t const viod_pci_did_legacy[VIRTIO_DT_NUM];

#define VIRTIO_PCI_VID			((uint16_t)0x1af4u)
#define VIRTIO_PCI_DID(type)	((uint16_t)(0x1040u+(type)))
#define VIRTIO_PCI_DID_LEGACY(type)	(viod_pci_did_legacy[(type)])


enum virtio_device_status {
	VIRTIO_DS_ACKNOWLEDGE	= 0x01,
	VIRTIO_DS_DRIVER		= 0x02,
	VIRTIO_DS_DRIVER_OK		= 0x04,
	VIRTIO_DS_FEATURES_OK	= 0x08,
	VIRTIO_DS_DEVICE_NEEDS_RESET= 0x20,
	VIRTIO_DS_FAILED		= 0x80,
};

enum virtio_pci_cap_type {
	VIRTIO_PCI_CAP_COMMON_CFG	= 1,
	VIRTIO_PCI_CAP_NOTIFY_CFG,
	VIRTIO_PCI_CAP_ISR_CFG,
	VIRTIO_PCI_CAP_DEVICE_CFG,
	VIRTIO_PCI_CAP_PCI_CFG,
};

struct virtio_pci_cap {
	uint8_t 	cap_vndr; /* PCI_CAP_ID_VNDR (9) */
	uint8_t 	cap_next;
	uint8_t 	cap_len;
	uint8_t 	cfg_type;	/* VIRTIO_PCI_CAP_* */
	uint8_t 	bar;
	uint8_t 	padding[3];
	uint32_t	offset;
	uint32_t	length;
};

struct virtio_virtq_data {
	uint16_t    queue_size;
	uint16_t    queue_msix_vector;
	uint16_t    queue_enable;
	uint16_t    queue_notify_off;
	uint64_t    queue_desc;
	uint64_t    queue_avail;
	uint64_t    queue_used;
};

struct virtio_pci_common_cfg {
	uint32_t	device_feature_select;
	uint32_t	device_feature;	/* which set controlled by 'device_feature_select' */
	uint32_t	driver_feature_select;
	uint32_t	driver_feature;	/* which set controlled by 'driver_feature_select */
	uint16_t	msix_config;
	uint16_t	num_queues;
	uint8_t		device_status;
	uint8_t		config_generation;
	uint16_t	queue_select;
	/* which queue the following refer to are controlled by 'queue_select' */
    struct virtio_virtq_data	queue_data;
};

struct virtio_pci_notify_cap {
	struct virtio_pci_cap 	cap;
	uint32_t				notify_off_multiplier;
};

enum virtio_pci_isr_status_bits {
	VIRTIO_PCI_ISR_STATUS_QUEUE		= 0x01,
	VIRTIO_PCI_ISR_STATUS_CONFIG	= 0x02,
};

struct virtio_pci_isr_cfg {
	uint8_t		isr_status; /* VIRTIO_PCI_ISR_STATUS_* */
	uint8_t		padding[3];
};

struct virtio_pci_cfg_cap {
	struct virtio_pci_cap 	cap;
	uint8_t					pci_cfg_data[4];
};


struct virtio_pci_legacy_bar {
	uint32_t		device_feature;
	uint32_t		driver_feature;
	uint32_t		queue_address;
	uint16_t		queue_size;
	uint16_t		queue_select;
	uint16_t		queue_notify;
	uint8_t			device_status;
	uint8_t			isr_status;
	/* followed by device specific configuration */
};

struct virtio_pci_legacy_bar_msix {
	struct virtio_pci_legacy_bar	base;
	uint16_t						config_msix_vector;
	uint16_t						queue_msix_vector;
	/* followed by device specific configuration */
};


#define VIRTIO_MMIO_MAGIC_NUMBER	0x74726976u
#define VIRTIO_MMIO_VERSION_LEGACY	1u
#define VIRTIO_MMIO_VERSION			2u

union virtio_ring_paddr {
	struct {
		uint32_t	low;
		uint32_t	high;
	};
	uint64_t		full;
};

struct virtio_mmio_registers {
	uint32_t			magic;
	uint32_t			version;
	uint32_t			device_id;
	uint32_t			vendor_id;
	uint32_t			device_feature;
	uint32_t			device_feature_select;
	uint32_t			unused1[2];
	uint32_t			driver_feature;
	uint32_t			driver_feature_select;
	uint32_t			guest_page_size; /* legacy only */
	uint32_t			unused2[1];
	uint32_t			queue_select;
	uint32_t			queue_num_max;
	uint32_t			queue_num;
	uint32_t			queue_align; /* legacy only */
	uint32_t			queue_pfn; /* legacy only */
	uint32_t			queue_ready; /* 1.0 only */
	uint32_t			unused4[2];
	uint32_t			queue_notify;
	uint32_t			unused5[3];
	uint32_t			interrupt_status;
	uint32_t			interrupt_ack;
	uint32_t			unused6[2];
	uint32_t			device_status;
	uint32_t			unused7[3];
	union virtio_ring_paddr	queue_desc; /* 1.0 only */
	uint32_t			unused8[2];
	union virtio_ring_paddr	queue_avail; /* 1.0 only */
	uint32_t			unused9[2];
	union virtio_ring_paddr	queue_used; /* 1.0 only */
	uint32_t			unuseda[0x15];
	uint32_t			config_generation;	/* 1.0 only */
	/* followed by device specific configuration */
};


#define VIRTIO_MSI_NO_VECTOR	0xffffu


#define VIRTIO_F_NOTIFY_ON_EMPTY	24u	/* transitional */
#define VIRTIO_F_ANY_LAYOUT			27u /* transitional */
#define VIRTIO_F_RING_INDIRECT_DESC	28u
#define VIRTIO_F_RING_EVENT_IDX		29u
#define VIRTIO_F_UNUSED				30u /* transitional */
#define VIRTIO_F_VERSION_1			32u

#define VIO_F_BIT(bit)	(1u << ((bit) & (((unsigned)sizeof(uint32_t)*8)-1)))
#define VIO_F_IDX(bit)	((bit) / ((unsigned)sizeof(uint32_t)*8))
#define VIO_HAS_FEATURE(array, bit)	(((array)[VIO_F_IDX((bit))] & VIO_F_BIT((bit))) != 0)
#define VIO_SET_FEATURE(array, bit)	((array)[VIO_F_IDX((bit))] |= VIO_F_BIT((bit)))

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.0.0/trunk/lib/hyp/public/virtio.h $ $Rev: 848164 $")
#endif
