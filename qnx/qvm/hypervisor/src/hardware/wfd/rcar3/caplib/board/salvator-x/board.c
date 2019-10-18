/*
 * $QNXLicenseC:
 * Copyright 2012, QNX Software Systems. All Rights Reserved.
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

#include <dlfcn.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>

#include <vcapture/capture.h>
#include <vcapture/capture-rcar3-ext.h>
#include <vcapture/capture-adv-ext.h>
#include <screen/screen.h>

/******************************************************** STRUCTS & STATICS ***/
/* debug control */
#ifdef CAPTURE_DEBUG_ON
    #define TRACE do { int temperrno=errno; slogf(_SLOGC_GRAPHICS, _SLOG_DEBUG2, "TRACE (Salvator-X): %s()", __FUNCTION__); errno=temperrno; } while (0)
    #define LOG(...) do { int temperrno=errno; slogf(_SLOGC_GRAPHICS, _SLOG_DEBUG2, __VA_ARGS__); errno=temperrno; } while(0)
#else
    #define TRACE
    #define LOG(...)
#endif

#define ADV7482_I2C_ADDR		0x70
#define ADV7482_I2C_PATH		"/dev/i2c4"

#define ADV_SOURCE_TYPE_ID	0  // composite CVBS

#define NDEVICES 1

/*
 * There are two input sources on Salvator-X board:
 * ADV7482 HDMI IN --> ADV7482 CSI2_A --> Renesas CSI40
 * ADV7482 AIN8 IN  --> ADV7482 CSI2_B -->Renesas CSI20
 */

enum {
	SRC0_HDMI_CSI40,
	SRC1_CVBS_AIN8_CSI20,
	NSOURCES
};

struct _capture_context {
	void *decoder_ctx;
	void *soc_ctx;
	uint32_t src_index;
	uint32_t device;
	int initialized;
	int pending_enable;
};

struct _capture_api {
	volatile void *dll_handle;
	capture_context_t (*capture_create_context)( uint32_t flags );
	void (*capture_destroy_context)( capture_context_t ctx );
	int (*capture_is_property)( capture_context_t ctx, uint32_t prop );
	int (*capture_update)( capture_context_t ctx, uint32_t flags );
	int (*capture_set_property_i)( capture_context_t ctx, uint32_t prop, int32_t value );
	int (*capture_set_property_p)( capture_context_t ctx, uint32_t prop, void *value );
	int (*capture_get_property_i)( capture_context_t ctx, uint32_t prop, int32_t *value );
	int (*capture_get_property_p)( capture_context_t ctx, uint32_t prop, void **value );
	int (*capture_get_frame)( capture_context_t ctx, uint64_t timeout, uint32_t flags );
	int (*capture_release_frame)( capture_context_t ctx, uint32_t idx );
	int (*capture_create_buffers)( capture_context_t ctx, uint32_t prop );
};

static struct _capture_api soc_api = { 0 };
static struct _capture_api decoder_api = { 0 };
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static char logmsg_prefix[] = "Salvator-X capture:";

/***************************************************************** LOGMSG() ***/
static void
logmsg( const char *fmt, ... )
{
	int err = errno;
	va_list	vargs;
	char fmt2[ strlen(fmt) + strlen(logmsg_prefix) + 2];

	strcpy( fmt2, logmsg_prefix);
	strcat( fmt2, fmt );

	va_start( vargs, fmt );
	vslogf( _SLOG_SETCODE(_SLOGC_GRAPHICS, 0), _SLOG_DEBUG1, fmt2, vargs );
	va_end( vargs );

	va_start( vargs, fmt );
	strcat( fmt2, "\n" );
	vfprintf( stderr, fmt2, vargs );
	va_end( vargs );
	errno = err;
}

/*************************************************************** LOAD_LIB() ***/
/** Open a capture library. Fill up the API table;
 * Returns:
 *   0 -- on success, -1 on error;
 */
static int
load_lib( struct _capture_api *api_tab, const char *lib_name )
{
	void *handle = dlopen( lib_name, RTLD_LOCAL );
	if( !handle ) {
		logmsg( "dlopen('%s') failed: %s\n", lib_name, dlerror() );
		return -1;
	}

#define	GETSYM( h, t, s )	if( NULL == ((t)->s = dlsym( h, #s )) ) { \
		logmsg( "unresolved symbol '" #s "' in '%s'\n", lib_name );\
		dlclose( h ); \
		return -1; \
	}

	api_tab->dll_handle = handle;
	GETSYM( handle, api_tab, capture_create_context );
	GETSYM( handle, api_tab, capture_destroy_context );
	GETSYM( handle, api_tab, capture_is_property );
	GETSYM( handle, api_tab, capture_update );
	GETSYM( handle, api_tab, capture_set_property_i );
	GETSYM( handle, api_tab, capture_set_property_p );
	GETSYM( handle, api_tab, capture_get_property_i );
	GETSYM( handle, api_tab, capture_get_property_p );
	GETSYM( handle, api_tab, capture_get_frame );
	GETSYM( handle, api_tab, capture_release_frame );

	return 0;
}

/************************************************************* DLL_LOADED() ***/
static void dll_loaded(void) __attribute__ ((constructor));
static void
dll_loaded(void)
{
	if( -1 == load_lib( &decoder_api, "libcapture-decoder-adv7482.so" )
	 || -1 == load_lib( &soc_api, "libcapture-soc-rcar3.so")
	) {
		exit( EXIT_FAILURE );
	}
}

/*********************************************************** DLL_UNLOADED() ***/
static void dll_unloaded(void) __attribute__ ((destructor));
static void
dll_unloaded(void)
{
	pthread_mutex_lock( &mutex );

	if( decoder_api.dll_handle != NULL ) {
		dlclose( (void*)decoder_api.dll_handle );
		dlclose( (void*)soc_api.dll_handle );
		memset( &decoder_api, 0, sizeof(decoder_api) );
		memset( &soc_api, 0, sizeof(soc_api));
	}
	pthread_mutex_unlock( &mutex );
}

/************************************************* CAPTURE_CREATE_CONTEXT() ***/
capture_context_t
capture_create_context( uint32_t flags )
{
	struct _capture_context *ctx = NULL;
	int err = EOK;

	TRACE;
	if((err = pthread_mutex_lock( &mutex )) != EOK) {
		logmsg(  "%s: pthread_mutex_lock failed (%s)", __FUNCTION__, strerror(err));
		errno = err;
		return NULL;
	}

	if( decoder_api.dll_handle == NULL ) {
		dll_loaded();
	}

	errno = 0;
	if( ( ctx = calloc( 1, sizeof( struct _capture_context ) ) ) == NULL ) {
		goto end;
	}

	if ( ( ctx->decoder_ctx = decoder_api.capture_create_context( flags ) ) == NULL
			|| ( ctx->soc_ctx = soc_api.capture_create_context( flags ) ) == NULL) {
		goto fail;
	}

	ctx->device = -1;
	ctx->src_index = -1;

	goto end;
fail:
	if( ctx ) {
		if( ctx->decoder_ctx) {
			decoder_api.capture_destroy_context( ctx->decoder_ctx );
		}
		if( ctx->soc_ctx) {
			soc_api.capture_destroy_context( ctx->soc_ctx );
		}
		free( ctx );
		ctx = NULL;
	}
end:
	pthread_mutex_unlock( &mutex );

	return ctx;
}

/************************************************* CAPTURE_CREATE_BUFFERS() ***/
int
capture_create_buffers( capture_context_t ctx, uint32_t property )
{
	return soc_api.capture_create_buffers( ctx->soc_ctx, property );
}

/************************************************ CAPTURE_DESTROY_CONTEXT() ***/
void
capture_destroy_context( capture_context_t ctx )
{
	TRACE;
	if(ctx == NULL) {
		return;
	}
	pthread_mutex_lock( &mutex );

	soc_api.capture_destroy_context( ctx->soc_ctx );
	decoder_api.capture_destroy_context( ctx->decoder_ctx );

	free( ctx );
	pthread_mutex_unlock( &mutex );

}

/********************************************************* CAPTURE_UPDATE() ***/
int
capture_update( capture_context_t ctx, uint32_t flags )
{
	int enabled = 0, rc1, rc2;
	int errvalue = EOK;

	TRACE;
	if( -1 == soc_api.capture_get_property_i( ctx->soc_ctx, CAPTURE_ENABLE, &enabled ) ) {
		logmsg( "failed to get enable status \n");
		return -1;
	}

	if( enabled ) {
		rc1 = soc_api.capture_update( ctx->soc_ctx, flags );
		if (rc1) {
			errvalue = errno;
			logmsg( "soc_api.capture_update error is %s (errno = %d) \n", strerror( errvalue ), errvalue);
			errno = errvalue;
			return -1;
		}

		rc2 = decoder_api.capture_update( ctx->decoder_ctx, flags );
		if (rc2) {
			errvalue = errno;
			logmsg( "decoder_api.capture_update error is %s (errno = %d) \n", strerror( errvalue ), errvalue);
			errno = errvalue;
			return -1;
		}
	} else {
		if ( !ctx->initialized) {
			// 1-st step: initialze camare MIPI and put it in LowPower state
			decoder_api.capture_set_property_i(ctx->decoder_ctx, CAPTURE_ENABLE, 0);
			rc2 = decoder_api.capture_update( ctx->decoder_ctx, flags );
			if (rc2) {
				errvalue = errno;
				logmsg(  "decoder_api. capture_update error (%s) in 1st step \n" , strerror( errvalue ));
				errno = errvalue;
				return -1;
			}
			//2-nd step: initialize MIPI hardware on the SOC side, but not enable capturing yet
			soc_api.capture_set_property_i(ctx->soc_ctx, CAPTURE_ENABLE, 0);
			rc1 = soc_api.capture_update( ctx->soc_ctx, flags );
			if (rc1) {
				errvalue = errno;
				logmsg(  "soc_api.capture_update error (%s) in 2nd step \n" , strerror( errvalue ));
				errno = errvalue;
				return -1;
			}
			if(ctx->pending_enable) {
				// 3-rd: enable camera MIPI
				decoder_api.capture_set_property_i(ctx->decoder_ctx, CAPTURE_ENABLE, 1);
				rc2 = decoder_api.capture_update( ctx->decoder_ctx, flags );
				if (rc2) {
					errvalue = errno;
					logmsg(  "decoder_api. capture_update error (%s) in 3rd step \n" , strerror( errvalue ));
					errno = errvalue;
					return -1;
				}

				// 4-th: start capture
				soc_api.capture_set_property_i(ctx->soc_ctx, CAPTURE_ENABLE, 1);
				rc1 = soc_api.capture_update( ctx->soc_ctx, flags );
				if (rc1) {
					errvalue = errno;
					logmsg(  "soc_api.capture_update error (%s) in 4th step \n" , strerror( errvalue ));
					errno = errvalue;
					return -1;
				}
			}
			ctx->initialized = 1;
		}
		else {
			rc2 = decoder_api.capture_update( ctx->decoder_ctx, flags );
			if (rc2) {
				errvalue = errno;
				logmsg( "decoder_api.capture_update error is %s (errno = %d) \n", strerror( errvalue ), errvalue);
				errno = errvalue;
				return -1;
			}
			rc1 = soc_api.capture_update( ctx->soc_ctx, flags );
			if (rc1) {
				errvalue = errno;
				logmsg( "soc_api.capture_update error is %s (errno = %d) \n", strerror( errvalue ), errvalue);
				errno = errvalue;
				return -1;
			}
		}
	}

	return (rc1 || rc2 ) ? -1 : 0 ;
}

/**************************************************** CAPTURE_IS_PROPERTY() ***/
int
capture_is_property( capture_context_t ctx, uint32_t prop )
{
	if( decoder_api.capture_is_property( ctx->decoder_ctx, prop ) ) {
		return 1;
	}
	if( soc_api.capture_is_property( ctx->soc_ctx, prop ) ) {
		return 1;
	}
	return 0;
}

/************************************************* CAPTURE_GET_PROPERTY_I() ***/
int
capture_get_property_i( capture_context_t  ctx, uint32_t property, int32_t *value )
{
	int rc1, rc2;
	int32_t value1, value2;

	TRACE;
	if(property != CAPTURE_PROPERTY_NDEVICES) {
		// "device" must be set first.
		if(ctx->device == -1) {
			errno = ENODEV;
			return -1;
		}
	}

	switch (property) {
	case CAPTURE_PROPERTY_NSOURCES:
		*value = NSOURCES;
		return 0;
	case CAPTURE_PROPERTY_NDEVICES:
		*value = NDEVICES;
		return 0;
	case CAPTURE_PROPERTY_SRC_INDEX:
		*value = ctx->src_index;
		return 0;
	case CAPTURE_PROPERTY_DEVICE:
		*value = ctx->device;
		return 0;
	case CAPTURE_PROPERTY_VERBOSITY:
		rc1 = decoder_api.capture_get_property_i( ctx->decoder_ctx, property, &value1 );
		rc2 = soc_api.capture_get_property_i( ctx->soc_ctx, property, &value2 );
		if( rc1 == 0 && rc2 == 0) {
			*value = (value1 > value2)? value2: value1;
			return 0;
		}
		else {
			return -1;
		}
	case CAPTURE_PROPERTY_SRC_WIDTH:
	case CAPTURE_PROPERTY_SRC_HEIGHT:
		rc1 = decoder_api.capture_get_property_i( ctx->decoder_ctx, property, &value1 );
		rc2 = soc_api.capture_get_property_i( ctx->soc_ctx, property, &value2 );
		// return the value returned by the decoder, and print a warning message
		if( rc1 == 0 && rc2 == 0) {
			*value = value1;
			if(value2 != value1) {
				logmsg(  "%s: the source size (%d) on soc side doesn't match the source"
						" size (%d) on camera side", __FUNCTION__, value2, value1);
			}
			return 0;
		}
		else {
			return -1;
		}
	default:
		if(decoder_api.capture_get_property_i( ctx->decoder_ctx, property, value ) == 0) {
			return 0;
		}
		else if(errno == ENOENT) {
			return soc_api.capture_get_property_i( ctx->soc_ctx, property, value );
		}
		else
			return -1;
	}
}

/************************************************* CAPTURE_SET_PROPERTY_I() ***/
int
capture_set_property_i( capture_context_t  ctx, uint32_t property, int32_t value )
{
	TRACE;

	if(property != CAPTURE_PROPERTY_DEVICE) {
		// "device" must be set first.
		if(ctx->device == -1) {
			errno = ENODEV;
			return -1;
		}
	}

	switch (property) {
	case CAPTURE_PROPERTY_DEVICE:
		if(value < NDEVICES) {
			 if(value == 0) {
				if(( decoder_api.capture_set_property_p( ctx->decoder_ctx, CAPTURE_PROPERTY_DECODER_I2C_PATH, ADV7482_I2C_PATH ) == -1)
					|| ( decoder_api.capture_set_property_i( ctx->decoder_ctx, CAPTURE_PROPERTY_DECODER_I2C_ADDR, ADV7482_I2C_ADDR ) == -1)
					|| decoder_api.capture_update( ctx->decoder_ctx, 0 ) == -1 ) {
					return -1;
				}
				if(soc_api.capture_set_property_i( ctx->soc_ctx, property, value ) == -1) {
					return -1;
				}
				ctx->device = value;
				return 0;
			 }
		}
		errno = EINVAL;
		return -1;
	case CAPTURE_PROPERTY_SRC_INDEX:
		if (value < NSOURCES) {
			switch (value) {
			case SRC0_HDMI_CSI40:
				if((decoder_api.capture_set_property_i(ctx->decoder_ctx, CAPTURE_PROPERTY_DEVICE, 0) == -1)// select CP core
					|| (decoder_api.capture_set_property_i(ctx->decoder_ctx, property, 0) == -1)
					|| (decoder_api.capture_set_property_i(ctx->decoder_ctx, CAPTURE_PROPERTY_CSI2_NUM_DATA_LANES, 4)  == -1)
					|| (soc_api.capture_set_property_i(ctx->soc_ctx, property, 0) == -1)
					|| (soc_api.capture_set_property_i(ctx->soc_ctx, CAPTURE_PROPERTY_INTERFACE_TYPE, CAPTURE_IF_MIPI_CSI2)  == -1)
					|| (soc_api.capture_set_property_i(ctx->soc_ctx, CAPTURE_PROPERTY_CSI2_NUM_DATA_LANES, 4)  == -1)
					|| (soc_api.capture_set_property_i(ctx->soc_ctx, CAPTURE_PROPERTY_CSI2_DDR_CLOCK_MHZ, 200)  == -1)
					||  (soc_api.capture_set_property_i(ctx->soc_ctx, CAPTURE_PROPERTY_SRC_FORMAT, SCREEN_FORMAT_RGB888) == -1)
					|| (soc_api.capture_set_property_i(ctx->soc_ctx, CAPTURE_PROPERTY_RCAR3_VIN_INPUT_IFACE_FORMAT, RCAR3_VIN_INF_BT601_BT709_24BIT) == -1)
					|| ( soc_api.capture_set_property_i(ctx->soc_ctx, CAPTURE_PROPERTY_INVERT_VSYNC_POL, 1 ) == -1)
					|| ( soc_api.capture_set_property_i(ctx->soc_ctx, CAPTURE_PROPERTY_INVERT_HSYNC_POL, 1 ) == -1)
					|| (soc_api.capture_set_property_i(ctx->soc_ctx, CAPTURE_PROPERTY_RCAR3_VSYNC_TOGGLE_MODE, 1) == -1)) {
					return -1;
				}
				break;
			case SRC1_CVBS_AIN8_CSI20:
				if((decoder_api.capture_set_property_i(ctx->decoder_ctx, CAPTURE_PROPERTY_DEVICE, 1) == -1)// select SDP core
					|| (decoder_api.capture_set_property_i(ctx->decoder_ctx, CAPTURE_PROPERTY_ADV_SRC_TYPE, ADV_SOURCE_TYPE_ID) == -1)
					|| (decoder_api.capture_set_property_i(ctx->decoder_ctx, property, 7) == -1)
					|| (decoder_api.capture_set_property_i(ctx->decoder_ctx, CAPTURE_PROPERTY_CSI2_NUM_DATA_LANES, 1)  == -1)
					|| (soc_api.capture_set_property_i(ctx->soc_ctx, property, 1) == -1) // CSI20
					|| (soc_api.capture_set_property_i(ctx->soc_ctx, CAPTURE_PROPERTY_INTERFACE_TYPE, CAPTURE_IF_MIPI_CSI2)  == -1)
					|| (soc_api.capture_set_property_i(ctx->soc_ctx, CAPTURE_PROPERTY_CSI2_NUM_DATA_LANES, 1)  == -1)
					|| (soc_api.capture_set_property_i(ctx->soc_ctx, CAPTURE_PROPERTY_CSI2_DDR_CLOCK_MHZ, 200)  == -1)
					|| (soc_api.capture_set_property_i(ctx->soc_ctx, CAPTURE_PROPERTY_RCAR3_VIN_INPUT_IFACE_FORMAT, RCAR3_VIN_INF_BT656_8BIT) == -1)
					|| (soc_api.capture_set_property_i(ctx->soc_ctx, CAPTURE_PROPERTY_RCAR3_VSYNC_TOGGLE_MODE, 1) == -1)) {
					return -1;
				}
				break;
			default:
				logmsg(  "%s() call with invalid device (%d) set!", __FUNCTION__, value);
				errno=EINVAL;
				return -1;
			}
			ctx->src_index = value;
			return 0;
		}
		errno = EINVAL;
		return -1;
	case CAPTURE_PROPERTY_NSOURCES:
	case CAPTURE_PROPERTY_NDEVICES:
		errno = EROFS;
		return -1;
	case CAPTURE_ENABLE:
		// keep a copy of the "enable" status
		ctx->pending_enable = value;
		if((decoder_api.capture_set_property_i( ctx->decoder_ctx, property, value ) == -1)
			|| soc_api.capture_set_property_i( ctx->soc_ctx, property, value ) == -1) {
			return -1;
		}
		return 0;
	case CAPTURE_PROPERTY_SRC_FORMAT:
	case CAPTURE_PROPERTY_CSI2_NUM_DATA_LANES:
	case CAPTURE_PROPERTY_VERBOSITY:
		if((decoder_api.capture_set_property_i( ctx->decoder_ctx, property, value ) == -1)
			|| soc_api.capture_set_property_i( ctx->soc_ctx, property, value ) == -1) {
			return -1;
		}
		return 0;
	default:
		if( decoder_api.capture_set_property_i( ctx->decoder_ctx, property, value ) == 0) {
			return 0;
		}
		else if( errno == ENOENT ){
			return soc_api.capture_set_property_i( ctx->soc_ctx, property, value );
		}
		else {
			return -1;
		}
	}
}

/************************************************* CAPTURE_SET_PROPERTY_P() ***/
int
capture_set_property_p( capture_context_t  ctx, uint32_t property, void *value )
{
	TRACE;

	// "device" must be set first.
	if(ctx->device == -1) {
		errno = ENODEV;
		return -1;
	}

	if( decoder_api.capture_set_property_p( ctx->decoder_ctx, property, value ) == 0) {
		return 0;
	}
	else if( errno == ENOENT ){
		return soc_api.capture_set_property_p( ctx->soc_ctx, property, value );
	}
	else {
		return -1;
	}
}

/************************************************* CAPTURE_GET_PROPERTY_P() ***/
int
capture_get_property_p( capture_context_t  ctx, uint32_t property, void **value )
{
	static char devinfo[ 1024 ] = "";
	TRACE;

	if(ctx->device == -1) {
		errno = ENODEV;
		return -1;
	}

	switch (property) {
		case CAPTURE_PROPERTY_DEVICE_INFO:
			if( !devinfo[0] ) {
				pthread_mutex_lock(&mutex);
				const char *soc_info, *decoder_info;
				soc_api.capture_get_property_p( ctx->soc_ctx, property, (void **)&soc_info );
				decoder_api.capture_get_property_p( ctx->decoder_ctx, property, (void **)&decoder_info );
				snprintf( devinfo, sizeof(devinfo),
						"board:   capture driver for R-CarH3-SiP Salvator-X\n"
						"soc:     %s\ndecoder: %s\n", soc_info, decoder_info );
				pthread_mutex_unlock(&mutex);
			}
			*value = devinfo;
			return 0;
		default:
			if( decoder_api.capture_get_property_p( ctx->decoder_ctx, property, value ) == 0) {
				return 0;
			}
			else if( errno == ENOENT ){
				return soc_api.capture_get_property_p( ctx->soc_ctx, property, value );
			}
			else {
				return -1;
			}
	}

}

/****************************************************** CAPTURE_GET_FRAME() ***/
int
capture_get_frame( capture_context_t ctx, uint64_t timeout, uint32_t flags )
{
	return soc_api.capture_get_frame( ctx->soc_ctx, timeout, flags );
}

/************************************************** CAPTURE_RELEASE_FRAME() ***/
int
capture_release_frame( capture_context_t ctx, uint32_t idx )
{
	return soc_api.capture_release_frame( ctx->soc_ctx, idx );
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/graphics/branches/release-2.1.x/hardware/wfd/rcar3/caplib/board/salvator-x/board.c $ $Rev: 842613 $")
#endif

