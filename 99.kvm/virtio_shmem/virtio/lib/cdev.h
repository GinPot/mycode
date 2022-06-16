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

/**
 * @file
 * Support routines for virtual character devices
 */

#ifndef _QVM_CDEV_H_INCLUDED
#define _QVM_CDEV_H_INCLUDED

#include <pthread.h>
#include <termios.h>
#include <stdint.h>

#define CDSF_DELAYED		0x0001
#define CDSF_CONFIGURED		0x0002
#define CDSF_NEEDS_RESTORE	0x0004
#define CDSF_OWN_BUFFERS    0x0008
#define CDSF_DRAINED	    0x0010

struct cdev_state {
    struct qvm_callback cb;
	struct qvm_vdev_s	*vdev;
	struct {
		const char 	*name;
		uint8_t		*buff;
		unsigned	size;
		unsigned	avail;
		unsigned	pos;
		int			fd;
	}		in;
	struct {
		const char	*name;
        uint8_t     *buff;
        unsigned    size;
        unsigned    begin;
		unsigned    pos;
		int			fd;
		timer_t		flush_timer;
		long		flush_timeout;
	}		out;
	struct termios	save_termio;
	unsigned		delayed_count;
	unsigned		flags;
	int				priority;
};

#define CDEV_INPUT_AVAILABLE_PULSE_CODE     0
#define CDEV_OUTPUT_AVAILABLE_PULSE_CODE	1
#define CDEV_NUM_PULSE_CODES			    2

enum {
    CDEV_OPT_HOSTDEV_IDX,
    CDEV_OPT_DELAYED_IDX,
    CDEV_OPT_BATCH_IDX,
    CDEV_OPT_NUM_OPTS
};

/* <STAN_MACRO1> */
#define CDEV_OPTIONS 	\
    [CDEV_OPT_HOSTDEV_IDX] = "hostdev",	\
    [CDEV_OPT_DELAYED_IDX] = "delayed", \
    [CDEV_OPT_BATCH_IDX] = "batch"
/* </STAN_MACRO1> */

int		cdev_control(struct cdev_state *cds, unsigned ctrl, const char *arg);
int		cdev_setup(struct cdev_state *cds, struct qvm_vdev_s *vdev, unsigned size);
int		cdev_write(struct cdev_state *cds, const uint8_t *data, unsigned len);
int     cdev_write_buffer(struct cdev_state *cds);
int		cdev_read(struct cdev_state *cds, uint8_t *data, unsigned len);
int		cdev_input_available(struct cdev_state *cds);

#endif


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.0.0/trunk/utils/q/qvm/public/qvm/cdev.h $ $Rev: 868406 $")
#endif
