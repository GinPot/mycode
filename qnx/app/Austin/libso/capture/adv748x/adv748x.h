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

#ifndef ADV748X_H_
#define ADV748X_H_

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <limits.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>
#include <vcapture/capture.h>
#include <screen/screen.h>
#include <vcapture/capture-adv-ext.h>

#if __GNUC__ >= 4
#define GCCATTR(x) __attribute__ ((x))
#else
#define GCCATTR(x)
#endif
// SYM_INTERNAL_ONLY indicates a function won't be called from outside this
// library, even via a function pointer.
#define SYM_INTERNAL_ONLY GCCATTR(visibility ("internal"))
#define SYM_PRIVATE GCCATTR(visibility ("hidden"))

/* debug control */
#ifdef ADV748X_DEBUG_ON
    #define TRACE do { int temperrno=errno; slogf(_SLOGC_GRAPHICS, _SLOG_DEBUG2, "TRACE (ADV748X): %s()", __FUNCTION__); errno=temperrno; } while (0)
    #define LOG(...) do { int temperrno=errno; slogf(_SLOGC_GRAPHICS, _SLOG_DEBUG2, __VA_ARGS__); errno=temperrno; } while(0)
#else
    #define TRACE
    #define LOG(...)
#endif

// bit definition for ctx->changes, indicating the properties to be applied to the hardware
#define CAPTURE_CHANGE_ENABLE		(1 << 0)
#define CAPTURE_CHANGE_DEVICE		(1 << 1)
#define CAPTURE_CHANGE_SOURCE		(1 << 2)
#define CAPTURE_CHANGE_MIPI_VCHANNEL	(1 << 3)
#define CAPTURE_CHANGE_CONTRAST		(1 << 4)
#define CAPTURE_CHANGE_BRIGHTNESS	(1 << 5)
#define CAPTURE_CHANGE_SATURATION	(1 << 6)
#define CAPTURE_CHANGE_NORM		(1 << 7)
#define CAPTURE_CHANGE_HUE		(1 << 8)
#define CAPTURE_CHANGE_DEVICE_ADDR	(1 << 9)

#define CSI2_MIN_NUM_DATA_LANES		1
#define CSI2_MAX_NUM_DATA_LANES		4

#define CSI_TXA_NUM_DATA_LANES_MAX 4
#define CSI_TXB_NUM_DATA_LANES_MAX 1

enum {
	DEV0_CP_CORE = 0,
	DEV1_SDP_CORE
};

enum adv748x_output_iface {
	CSI_TXA_OUT, // 4 data lanes
	CSI_TXB_OUT, // 1 data lane
	INVALID_OUT = -1
};

struct _ctx_state
{
	int device;
	int src_index;
	enum adv_src_type src_type;
	int mipi_vchannel;
	int output_enable;
	enum adv748x_output_iface output_iface;
	int num_data_lanes;
	int output_format;
	int contrast;		/* contrast, -128 to 127 */
	int brightness;		/* brightness, -128 to 127 */
	int saturation;		/* color saturation, -128 to 127 */
	int hue;		/* color hue, -128 to 127 */
	const char *norm;
	int verbosity;
};

struct _capture_context {
	const char	*device_info;        /* information about this decoder */
	char		dev_path[_POSIX_PATH_MAX];
	uint32_t	dev_addr;
	int		device_fd;
	struct _ctx_state active;
	struct _ctx_state pending;
	uint32_t	changes;
	int norm_index;         /* the  index into set_norms[] */
};

#ifdef VARIANT_adv7480
// TODO redefine nsources
static const int ndevices = 1;  /* CP core */
#else
static const int ndevices = 2;  /* SD core and CP core */
#endif
static const int nsources_cp = 1;
static const int nsources_sdp[CAPTURE_ADV_SRC_TYPE_NUM] = {8, 4, 2, 4};


static const int num_data_lanes_max[2] = {CSI_TXA_NUM_DATA_LANES_MAX, CSI_TXB_NUM_DATA_LANES_MAX};  // 4 data lanes for CSI_TXA; 1 data lane for CSI_TXB

SYM_INTERNAL_ONLY void logmsg( const char *fmt, ... ) __attribute__((__format__(__printf__, 1, 2)));;
SYM_INTERNAL_ONLY int decoder_init(capture_context_t ctx);
SYM_INTERNAL_ONLY void decoder_fini(capture_context_t ctx);
SYM_INTERNAL_ONLY int decoder_update( capture_context_t ctx, uint32_t flags);
SYM_INTERNAL_ONLY int decoder_get_property(capture_context_t ctx, uint32_t property, void **value);
SYM_INTERNAL_ONLY int get_norm_index(char *norm);
SYM_INTERNAL_ONLY int i2c_rd(int fd, uint32_t dev_addr, uint8_t reg_addr, uint8_t *data);
SYM_INTERNAL_ONLY int i2c_wr(int fd, uint32_t dev_addr, uint8_t reg_addr, uint8_t data);

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/graphics/branches/release-2.1.x/hardware/capture/adv748x/adv748x.h $ $Rev: 867357 $")
#endif
