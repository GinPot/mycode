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
#ifndef __VIRTIO_BLK_H
#define __VIRTIO_BLK_H

/**
 * @file
 * definitions for virtio-blk devices
 * see http://docs.oasis-open.org/virtio/virtio/v1.0/virtio-v1.0.html
 */

#include <virtio.h>

#define VIRTIO_BLK_F_BARRIER	0	/* legacy */
#define VIRTIO_BLK_F_SIZE_MAX 	1
#define VIRTIO_BLK_F_SEG_MAX 	2
#define VIRTIO_BLK_F_GEOMETRY 	4
#define VIRTIO_BLK_F_RO 		5
#define VIRTIO_BLK_F_BLK_SIZE 	6
#define VIRTIO_BLK_F_SCSI		7	/* legacy */
#define VIRTIO_BLK_F_FLUSH		9	/* legacy */
#define VIRTIO_BLK_F_TOPOLOGY 	10
#define VIRTIO_BLK_F_CONFIG_WCE	11	/* legacy */

#define VIRTIO_BLK_REQ_QIDX	0

struct virtio_blk_config {
	uint64_t	capacity;
	uint32_t	size_max;
	uint32_t	seg_max;
	struct virtio_blk_geometry {
		uint16_t	cylinders;
		uint8_t		heads;
		uint8_t		sectors;
	} 			geometry;
	uint32_t	blk_size;
	struct virtio_blk_topology {
		uint8_t		physical_block_exp;
		uint8_t		alignment_offset;
		uint16_t	min_io_size;
		uint32_t	opt_io_size;
	} 			topology;
	uint8_t		reserved;
};


#define VIRTIO_BLK_T_IN		0u
#define VIRTIO_BLK_T_OUT	1u
#define VIRTIO_BLK_T_FLUSH	4u

#define VIRTIO_BLK_S_OK		0u
#define VIRTIO_BLK_S_IOERR	1u
#define VIRTIO_BLK_S_UNSUPP	2u

struct virtio_blk_req {
	uint32_t	type;		/* VIRTIO_BLK_T_* */
	uint32_t	reserved;
	uint64_t	sector;
	uint8_t		data[][512];
	/* uint8_t		status; */ /* VIRTIO_BLK_S_* */
};

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.0.0/trunk/lib/hyp/public/virtio_blk.h $ $Rev: 793126 $")
#endif
