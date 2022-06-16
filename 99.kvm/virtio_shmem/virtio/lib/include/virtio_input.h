/*
 * $QNXLicenseC:
 * Copyright 2017, QNX Software Systems. All Rights Reserved.
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
#ifndef __VIRTIO_INPUT_H
#define __VIRTIO_INPUT_H

/**
 * @file
 * definitions for virtio-input devices
 * see http://docs.oasis-open.org/virtio/virtio/v1.0/cs03-virtio-input/virtio-v1.0-cs03-virtio-input.html
 */

#include <virtio.h>

#define VIRTIO_INPUT_EVENT_QIDX		0
#define VIRTIO_INPUT_STATUS_QIDX	1

enum virtio_input_config_select {
	VIRTIO_INPUT_CFG_UNSET		= 0x00,
	VIRTIO_INPUT_CFG_ID_NAME	= 0x01,
	VIRTIO_INPUT_CFG_ID_SERIAL	= 0x02,
	VIRTIO_INPUT_CFG_PROP_BITS	= 0x10,
	VIRTIO_INPUT_CFG_EV_BITS	= 0x11,
	VIRTIO_INPUT_CFG_ABS_INFO	= 0x12,
};

struct virtio_input_absinfo {
	uint32_t	min;
	uint32_t	max;
	uint32_t	fuzz;
	uint32_t	flat;
};

struct virtio_input_config {
	uint8_t		select;
	uint8_t		subsel;
	uint8_t		size;
	uint8_t		reserved[5];
	union {
		char	string[128];
		uint8_t	bitmap[128];
		struct virtio_input_absinfo	abs;
	}			u;
};

struct virtio_input_event {
	uint16_t	type;
	uint16_t	code;
	uint32_t	value;
};

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.0.0/trunk/lib/hyp/public/virtio_input.h $ $Rev: 848164 $")
#endif
