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
 * definitions for virtqueues
 * see http://docs.oasis-open.org/virtio/virtio/v1.0/virtio-v1.0.html
 */

#ifndef __VIRTQ_H
#define __VIRTQ_H

#include <stdint.h>


enum virtq_desc_flags {
	VIRTQ_DESC_F_NEXT		= 0x0001, /**< Buffer continues via next field */
	VIRTQ_DESC_F_WRITE		= 0x0002, /**< Buffer is write only */
	VIRTQ_DESC_F_INDIRECT	= 0x0004, /**< Buffer has list of descriptors */
};

enum virtq_avail_flags {
	VIRTQ_AVAIL_F_NO_INTERRUPT = 0x0001, /**< Don't interrupt driver when buffer consumed */
};

enum virtq_used_flags {
	VIRTQ_USED_F_NO_NOTIFY	= 0x00000001, /**< Don't notify device when buffer added */
};

struct virtq_desc {
	uint64_t	addr;	/**< guest physical address (LE) */
	uint32_t	len;	/**< buffer length (LE) */
	uint16_t	flags;	/**< VIRTQ_DESC_F_* bitset (LE) */
	uint16_t	next;	/**< index of next descriptor in chain (LE) */
};

struct virtq_avail {
	uint16_t	flags;	/**< VIRTQ_AVAIL_F_* bitset (LE) */
	uint16_t	idx;	/**< next ring index to use (LE) */
	uint16_t	ring[];	/**< ring of descriptor indexes for device (LE) */
	/* If VIRTIO_F_EVENT_IDX, followed by uint16_t used_event; */
};

struct virtq_used_elem {
	uint32_t	id;		/**< head of descriptor index chain (LE) */
	uint32_t	len;	/**< length of buffer actually written (LE) */
};

struct virtq_used {
	uint16_t	flags; 	/**< VIRTQ_USED_F_* (LE) */
	uint16_t	idx;	/**< next ring index to use (LE) */
	struct virtq_used_elem ring[]; /**< ring of descriptor indexes for driver (LE) */
	/* If VIRTIO_F_EVENT_IDX, followed by uint16_t avail_event; */
};

struct virtq {
	struct virtq_desc	*desc;
	struct virtq_avail	*avail;
	struct virtq_used	*used;
	unsigned			num;
};


static inline uint16_t *
virtq_avail_event(const struct virtq *const vq) {
	return (uint16_t *)&vq->used->ring[vq->num];
}


static inline uint16_t *
virtq_used_event(const struct virtq *const vq) {
	return &vq->avail->ring[vq->num];
}


static inline size_t
virtq_legacy_avail_offset(unsigned const num) {
	return sizeof(struct virtq_desc)*num;
}


static inline size_t
virtq_legacy_used_offset(unsigned const num) {
	 size_t const av_size = sizeof(struct virtq_avail) + (sizeof(uint16_t)*(3+num));
	 return (virtq_legacy_avail_offset(num) + av_size + 0xfff) & ~(size_t)0xfff;
}


static inline size_t
virtq_legacy_ring_size(unsigned const num) {
	return virtq_legacy_used_offset(num)
		+ (sizeof(struct virtq_used_elem)*num)
		+ (sizeof(uint16_t)*3);
}


#define	VIRTQ_IDX_NONE	((uint16_t)0xffffu)

typedef	void	virtq_kick_t(void *cookie, uint16_t	idx);

struct virtq	*virtq_instance(virtq_kick_t *avail_kicker, virtq_kick_t *used_kicker, void *kick_cookie);
int				virtq_setup_virt(struct virtq *vq, unsigned num, int have_events, struct virtq_desc *desc, struct virtq_avail *avail, struct virtq_used *used);
int				virtq_setup_phys(struct virtq *vq, unsigned num, int have_events, uint64_t desc, uint64_t avail, uint64_t used);
void			virtq_shutdown(struct virtq *vq);
uint16_t		virtq_desc_alloc(struct virtq *vq, unsigned readable, unsigned writable);
void			virtq_desc_free(struct virtq *vq, unsigned idx);
void			virtq_avail_put(struct virtq *vq, uint16_t idx, int notify);
unsigned		virtq_avail_get(struct virtq *vq);
int				virtq_avail_empty(const struct virtq *vq);
void			virtq_used_put_notify(struct virtq *vq, uint16_t idx, unsigned len, int notify);
struct virtq_used_elem *virtq_used_get(struct virtq *vq);
int				virtq_used_empty(const struct virtq *vq);

/**
 * put a descriptor list in the used ring
 * @param	vq		virtual queue
 * @param	idx		head descriptor index or VIRTQ_IDX_NONE to not add anything
 * @param	len		number of bytes written into the descriptors
 */
static inline void
virtq_used_put(struct virtq * const vq, uint16_t const idx, unsigned const len)
{
    virtq_used_put_notify(vq, idx, len, 1);
}

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.0.0/trunk/lib/hyp/public/virtq.h $ $Rev: 848164 $")
#endif
