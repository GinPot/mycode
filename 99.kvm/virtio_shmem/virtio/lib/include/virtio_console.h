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
 * definitions for virtio-console devices
 * see http://docs.oasis-open.org/virtio/virtio/v1.0/virtio-v1.0.html
 */

#ifndef __VIRTIO_CONSOLE_H
#define __VIRTIO_CONSOLE_H

#include <virtio.h>

#define VIRTIO_CONSOLE_F_SIZE 			0u
#define VIRTIO_CONSOLE_F_MULTIPORT		1u
#define VIRTIO_CONSOLE_F_EMERG_WRITE	2u

enum {
	VIRTIO_CONSOLE_RECV0_QIDX,
	VIRTIO_CONSOLE_TRAN0_QIDX,
	/* following are only present if MULTIPORT feature negotiated */
	VIRTIO_CONSOLE_RECV_CTRL_QIDX,
	VIRTIO_CONSOLE_TRAN_CTRL_QIDX,
	VIRTIO_CONSOLE_RECV1_QIDX,
	VIRTIO_CONSOLE_TRAN1_QIDX,
	/* queue id's added as needed for additional ports */
};

#define VIRTIO_CONSOLE_RECV_QIDX(port) (((port) == 0)	\
					? VIRTIO_CONSOLE_RECV0_QIDX			\
					: VIRTIO_CONSOLE_RECV1_QIDX+((port)-1))
#define VIRTIO_CONSOLE_TRAN_QIDX(port) (((port) == 0)	\
					? VIRTIO_CONSOLE_TRAN0_QIDX			\
					: VIRTIO_CONSOLE_TRAN1_QIDX+((port)-1))

struct virtio_console_config {
	uint16_t cols;
	uint16_t rows;
	uint32_t max_nr_ports;
	uint32_t emerg_wr;
};


enum virtio_console_events {
	VIRTIO_CONSOLE_DEVICE_READY,
	VIRTIO_CONSOLE_DEVICE_ADD,
	VIRTIO_CONSOLE_DEVICE_REMOVE,
	VIRTIO_CONSOLE_PORT_READY,
	VIRTIO_CONSOLE_CONSOLE_PORT,
	VIRTIO_CONSOLE_RESIZE,
	VIRTIO_CONSOLE_PORT_OPEN,
	VIRTIO_CONSOLE_PORT_NAME,
};

struct virtio_console_control {
	uint32_t id;
	uint16_t event;
	uint16_t value;
};

struct virtio_console_resize {
	uint16_t cols;
	uint16_t rows;
};

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.0.0/trunk/lib/hyp/public/virtio_console.h $ $Rev: 793126 $")
#endif
