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
 * Definitions for virtio devices
 */

#ifndef _QVM_VIO_H_INCLUDED
#define _QVM_VIO_H_INCLUDED

#include <pthread.h>
#include <virtio.h>
#include <qvm/vpci.h>

/** Maximum number of feature words that we support */
#define VIO_MAX_FEATURE_WORDS	4

/**
 * Information describing the virtio device that we're creating
 */
struct vio_description {
	/** What type of device: VIRTIO_DT_*/
	unsigned	device_type;
	/** Maximum number of virtqueues for the device */
	unsigned	num_queues;
	/** Number of bytes of device specific configuraton data */
	unsigned	device_config_size;
	/** Device feature bits to be offered to the guest driver */
	uint32_t	device_features[VIO_MAX_FEATURE_WORDS];
};

typedef uint32_t	vio_notify_t;


/**
 * Information for one virtqueue on the device
 */
struct vio_queue_info {
	/** Pointer to virtqueue support routine control structure */
	struct virtq				*vq;
	/** Backpointer to the virtio state for this queue */
	struct vio_state			*vsp;
	/** Basic virtio data about the queue */
	struct virtio_virtq_data	data;
	/** Maximum number of entries the device allows in the queue */
	unsigned					max_size;
};

/**
 * Structure for converting a virtqueue descriptor list into an iov_t array
 */
struct vio_iov {
	iov_t		*vi_iov;		///<	Actual I/O vector
	int			vi_iov_len;		///<	Size of allocated array in #vi_iov
	int			vi_count;		///<	Number of entries mapped in #vi_iov
	int			vi_payload;		///<	Index at which payload starts
};


/**
 *  Version of the virtio standard the device is implementing
 */
enum vio_version {
	/** legacy */
	VIOVER_095,
	/** 1.0 standard */
	VIOVER_100,
};


/**
 * State of a virtio device
 */
struct vio_state {
	/** Pointer to internal state information */
	struct vio_state_internal	*vsip;
	/** Pointer to array of virtqueue information */
	struct vio_queue_info		*qinfo;
	/** Feature bits accepted by the guest driver */
	uint32_t					driver_features[VIO_MAX_FEATURE_WORDS];
};

void 		vio_init(qvm_vdev_t *vdp, enum vio_version ver, const struct vio_description *vdesc, struct vio_state *vsp);
void		vio_reset(struct vio_state *vsp);
int 		vio_init_queue(struct vio_state *vsp, unsigned qidx, unsigned max_size);
void 		vio_setup_queues(struct vio_state *vsp);
void 		vio_config_update(struct vio_state *vsp);
unsigned	vio_device_status(struct vio_state *vsp, unsigned on, unsigned off);

/**
 * Return values from vio_read/write routines
 */
#define VIOREF_CLASS_NOP		0x00000u /**< Nothing to do */
#define VIOREF_CLASS_NOTIFY		0x10000u /**< Driver has notified device about a queue update: idx in low bits ，对应linux端的#define VIRTIO_MMIO_QUEUE_NOTIFY	0x050的地址*/
#define VIOREF_CLASS_ENABLED	0x20000u /**< Driver has enabled a queue: idx in low bits */
#define VIOREF_CLASS_RESET		0x30000u /**< Driver has requested a device reset */
#define VIOREF_CLASS_STATUS		0x40000u /**< Driver has turned on new device status bits: new flags in low bits */
#define VIOREF_CLASS_DEVCFG		0x50000u /**< Driver has read/written device specific configation: cfg offset in low bits，对应linux端#define VIRTIO_MMIO_CONFIG		0x100的地址 */
#define VIOREF_CLASS_MASK		0xf0000u /**< Mask to isolate return class type */

unsigned	vio_read(struct vio_state *vsp, unsigned cookie, const struct qvm_state_block *vopnd,
                        const struct qvm_state_block *oopnd, struct guest_cpu *gcp);
unsigned	vio_write(struct vio_state *vsp, unsigned cookie, const struct qvm_state_block *vopnd,
                        const struct qvm_state_block *oopnd, struct guest_cpu *gcp);
int         vio_ctrl_pci_bars(struct vio_state *vsp, const char *arg);

int			vio_get_iov(struct guest_system *gsp, struct virtq * vq, uint16_t didx,
                                struct vio_iov *iov, unsigned hdr_size);
void		vio_put_iov(struct guest_system *gsp, struct vio_iov *iov);

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.0.0/trunk/utils/q/qvm/public/qvm/vio.h $ $Rev: 846963 $")
#endif
