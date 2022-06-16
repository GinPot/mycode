/*
 * $QNXLicenseC:
 * Copyright 2016, QNX Software Systems. All Rights Reserved.
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
#ifndef __VIRTIO_DRVR_H
#define __VIRTIO_DRVR_H

/**
 * @file
 * virtio driver support routines
 */

#include <virtio.h>

/** virtio interrupt information */
struct viod_intr_info {
	/** Next interrupt information structure in the list */
	struct viod_intr_info	*next;
	/** Pointer available to user - library makes no reference to it */
	void					*user;
	/** InterruptAttach() vector value */
	unsigned				vector;
	/** Number of entries in the 'queues' array */
	unsigned				num_queues;
	/** Each entry indicates a queue index (or config change) that
	 * should be checked when this interrupt comes in */
	_Uint16t				queues[]; /* variably sized */
};

/** Maximum number of feature words we support */
#define VIOD_MAX_FEATURE_WORDS	4

/** Structure controlling access to one virtio device */
struct viod {
	/** Linked list of interrupt information */
	struct viod_intr_info	*intrs;
	/** One of the VFF_VERSION_* flags, indicating version of connection */
	unsigned				version;
	/** Maximum number of virtques for the device */
	unsigned				num_queues;
	/** Feature set for the device. Driver fills this in orignally and then
	 * calls viod_features() to negotiate with the device for final bitset. */
	unsigned				feature[VIOD_MAX_FEATURE_WORDS];
};

/** Flags for the viod_find_*() routines */
enum viod_find_flags {
	/** Driver can handle a 0.9.5 version device */
	VFF_VERSION_095		= 0x00001,
	/** Driver can handle a 1.0.0 version device */
	VFF_VERSION_100		= 0x00002,
	/** Enable MSI-X support, if present */
	VFF_MSIX			= 0x00004,
};

struct viod	*viod_find_pci(enum virtio_device_type type, unsigned instance, enum viod_find_flags flags);
struct viod	*viod_find_mmio(enum virtio_device_type type, _Uint64t location, enum viod_find_flags flags, _Sizet cfg_size, unsigned intr);

void viod_shutdown(struct viod *vdp);

int viod_reset(const struct viod *vdp);
int viod_features(struct viod *vdp);
int viod_start(const struct viod *vdp);
int viod_fail(const struct viod *vdp);

_Uint16t viod_queue_max_size(const struct viod *vdp, unsigned qidx);
struct virtq *viod_queue_create(struct viod *vdp, unsigned qidx, _Uint16t entries);
int viod_queue_set(struct viod *vdp, unsigned qidx, _Uint16t entries, _Uint64t desc, _Uint64t avail, _Uint64t used);

int viod_read_device_cfg(struct viod *vdp, _Sizet off, void *data, _Sizet elsize);
int viod_write_device_cfg(struct viod *vdp, _Sizet off, const void *data, _Sizet elsize);

/** description for a portion of an atomic device configuration read */
struct device_read_section {
	/** Starting offset in the device config section */
	unsigned		offset;
	/** Size of element to read (number of bytes) */
	unsigned		elsize;
};

int viod_read_atomic_device_cfg(struct viod *vdp, unsigned nentries, const struct device_read_section *src, void *data);

int viod_notify(const struct viod *vdp, unsigned qidx);

unsigned viod_isr_status(const struct viod *vdp, const struct viod_intr_info *intr);
void viod_isr_ack(const struct viod *vdp, const struct viod_intr_info *intr, unsigned ack);

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.0.0/trunk/lib/hyp/public/virtio_drvr.h $ $Rev: 807273 $")
#endif
