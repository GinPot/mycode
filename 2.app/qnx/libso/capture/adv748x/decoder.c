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
#include <stdarg.h>

#include "adv748x.h"

/****************************************************************** STATICS ***/
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static char logmsg_prefix[] = "adv748x capture:";


/***************************************************************** LOGMSG() ***/
SYM_INTERNAL_ONLY void
logmsg( const char *fmt, ... )
{
	int err = errno;
	va_list	vargs;
	char fmt2[ strlen(fmt) + strlen(logmsg_prefix) + 2];

	strcpy( fmt2, logmsg_prefix );
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

/************************************************* CAPTURE_CREATE_CONTEXT() ***/
capture_context_t
capture_create_context( uint32_t flags )
{
	capture_context_t ctx = NULL;
	int err = EOK;

	TRACE;
	if((err = pthread_mutex_lock( &mutex )) != EOK) {
		logmsg("%s: pthread_mutex_lock failed:%s", __FUNCTION__, strerror(err));
		errno = err;
		return NULL;
	}

	if((ctx = ( capture_context_t )calloc( 1, sizeof(struct _capture_context) ) ) == NULL) {
		logmsg( "%s:calloc failed:%s", __FUNCTION__, strerror(errno) );
		pthread_mutex_unlock( &mutex );
		errno = ENOMEM;
		return NULL;
	}

	if((err = decoder_init(ctx)) != EOK) {
		logmsg( "%s:decoder_init failed:%s", __FUNCTION__, strerror(errno) );
		free(ctx);
		ctx = NULL;
		errno = err;
	}

	pthread_mutex_unlock( &mutex );
	return ctx;
}

/************************************************* CAPTURE_CREATE_BUFFERS() ***/
int
capture_create_buffers( capture_context_t ctx, uint32_t property )
{
	errno = ENOTSUP;
	return -1;
}
/*********************************************** CAPTURE_DESTROY_CONTEXT() ***/
void
capture_destroy_context( capture_context_t ctx )
{
	TRACE;
	if(ctx == NULL) {
		return;
	}
	pthread_mutex_lock( &mutex );
	decoder_fini(ctx);
	free( ctx );
	ctx = NULL;
	pthread_mutex_unlock( &mutex );
}

/******************************************************** CAPTURE_UPDATE() ***/
int
capture_update( capture_context_t ctx, uint32_t flags )
{
	int err = EOK;
	TRACE;

	pthread_mutex_lock( &mutex );
	if(ctx->active.verbosity != ctx->pending.verbosity) {
		ctx->active.verbosity = ctx->pending.verbosity;
	}
	if(ctx->changes){
		if((err = decoder_update(ctx, flags)) != EOK) {
			logmsg("decoder_update failed: %s", strerror(err));
		}
	}
	pthread_mutex_unlock( &mutex );
	errno = err;
	return err? -1:0;
}

/**************************************************** CAPTURE_IS_PROPERTY() ***/
int
capture_is_property( capture_context_t ctx, uint32_t prop )
{
	switch (prop) {
	case CAPTURE_ENABLE:
	case CAPTURE_PROPERTY_DEVICE_INFO:
	case CAPTURE_PROPERTY_DEVICE:
	case CAPTURE_PROPERTY_SRC_INDEX:
	case CAPTURE_PROPERTY_ADV_SRC_TYPE:
	case CAPTURE_PROPERTY_NDEVICES:
	case CAPTURE_PROPERTY_NSOURCES:
	case CAPTURE_PROPERTY_VERBOSITY:
	case CAPTURE_PROPERTY_DECODER_I2C_PATH:
	case CAPTURE_PROPERTY_DECODER_I2C_ADDR:
	case CAPTURE_PROPERTY_CSI2_NUM_DATA_LANES:
	case CAPTURE_PROPERTY_SRC_FORMAT:
		return 1;
	case CAPTURE_PROPERTY_BRIGHTNESS:
	case CAPTURE_PROPERTY_CONTRAST:
	case CAPTURE_PROPERTY_SATURATION:
	case CAPTURE_PROPERTY_HUE:
	case CAPTURE_PROPERTY_NORM:
	case CAPTURE_PROPERTY_CURRENT_NORM:
		errno = ENOTSUP;
		return 0;
	default:
		errno = ENOENT;
		return 0;
	}
}

/************************************************* CAPTURE_SET_PROPERTY_I() ***/
int
capture_set_property_i( capture_context_t ctx, uint32_t property, int32_t value )
{
	int rc = 0;

	TRACE;
	LOG("       ctx=%p, property=%08X ('%c', '%c', '%c', '%c'), value=%08X", ctx, property,
		(property>>24) & 0x000000FF, (property>>16) & 0x000000FF, (property>>8) & 0x000000FF,
		property & 0x000000FF, value);

	if((property != CAPTURE_PROPERTY_DEVICE) && ( property != CAPTURE_PROPERTY_DECODER_I2C_ADDR)) {
		// "device" must be set first.
		if(ctx->pending.device == -1) {
			errno = ENODEV;
			return -1;
		}
	}

	pthread_mutex_lock( &mutex );
	switch (property) {
	case CAPTURE_PROPERTY_DECODER_I2C_ADDR:
		ctx->dev_addr = value;
		ctx->changes |= CAPTURE_CHANGE_DEVICE_ADDR;
		break;
	case CAPTURE_PROPERTY_DEVICE:
		if(value >= 0 && value < ndevices) {
			ctx->pending.device = value;
			ctx->changes |= CAPTURE_CHANGE_DEVICE;
		}
		else {
			errno = EINVAL;
			rc = -1;
		}
		break;
	case CAPTURE_ENABLE:
		ctx->pending.output_enable = value?1:0;
		ctx->changes |= CAPTURE_CHANGE_ENABLE;
		break;
	case CAPTURE_PROPERTY_SRC_INDEX:
		if(ctx->pending.device == DEV1_SDP_CORE && ctx->pending.src_type >= 0 && ctx->pending.src_type < CAPTURE_ADV_SRC_TYPE_NUM) {
			if(value >= 0 && value < nsources_sdp[ctx->pending.src_type]) {
				ctx->pending.src_index = value;
				ctx->changes |= CAPTURE_CHANGE_SOURCE;
			}
		}
		else if(ctx->pending.device == DEV0_CP_CORE) {
			if(value >=0 && value < nsources_cp) {
				ctx->pending.src_index = value;
				ctx->changes |= CAPTURE_CHANGE_SOURCE;
			}
		}
		else {
			errno = EINVAL;
			rc = -1;
		}
		break;
	case CAPTURE_PROPERTY_ADV_SRC_TYPE:
		if(ctx->pending.device == DEV1_SDP_CORE && value >= 0 && value < CAPTURE_ADV_SRC_TYPE_NUM) {
			ctx->pending.src_type = value;
			ctx->changes |= CAPTURE_CHANGE_SOURCE;
		}
		else {
			errno = EINVAL;
			rc = -1;
		}
		break;
	case CAPTURE_PROPERTY_VERBOSITY:
		ctx->pending.verbosity = value;
		break;

	case CAPTURE_PROPERTY_CSI2_NUM_DATA_LANES:
		if((value == -1) || (value >= CSI2_MIN_NUM_DATA_LANES && value <= CSI2_MAX_NUM_DATA_LANES)) {
			ctx->pending.num_data_lanes = value;
			ctx->changes |= CAPTURE_CHANGE_SOURCE;
		}
		else {
			errno = EINVAL;
			rc = -1;
		}
		break;
	case CAPTURE_PROPERTY_SRC_FORMAT:
		ctx->pending.output_format = value;
		ctx->changes |= CAPTURE_CHANGE_SOURCE;
		break;
	case CAPTURE_PROPERTY_NSOURCES:
	case CAPTURE_PROPERTY_NDEVICES:
		errno = EROFS;
		rc = -1;
		break;
	case CAPTURE_PROPERTY_BRIGHTNESS:
	case CAPTURE_PROPERTY_CONTRAST:
	case CAPTURE_PROPERTY_SATURATION:
	case CAPTURE_PROPERTY_HUE:
		errno = ENOTSUP;
		rc = -1;
		break;
	default:
		errno = ENOENT;
		rc = -1;
		break;
	}
	pthread_mutex_unlock( &mutex );
	return rc;
}

/************************************************* CAPTURE_GET_PROPERTY_I() ***/
int
capture_get_property_i( capture_context_t ctx, uint32_t property, int32_t *value )
{
	int rc = 0;

	TRACE;
	LOG("       ctx=%p, property=%08X ('%c', '%c', '%c', '%c'), value=%p", ctx, property,
		(property>>24) & 0x000000FF, (property>>16) & 0x000000FF, (property>>8) & 0x000000FF,
		property & 0x000000FF, value);

	if(property != CAPTURE_PROPERTY_NDEVICES) {
		// "device" must be set first.
		if(ctx->pending.device == -1) {
			errno = ENODEV;
			return -1;
		}
	}

	pthread_mutex_lock( &mutex );

	switch (property) {
	case CAPTURE_ENABLE:
		*value = ctx->active.output_enable;
		break;
	case CAPTURE_PROPERTY_DEVICE:
		*value = ctx->active.device;
		break;
	case CAPTURE_PROPERTY_SRC_INDEX:
		*value = ctx->active.src_index;
		break;
	case CAPTURE_PROPERTY_ADV_SRC_TYPE:
		if(ctx->pending.device == DEV1_SDP_CORE) {
			*value = ctx->active.src_type;
		}
		else {
			errno = ENOTSUP;
			rc = -1;
		}
		break;
	case CAPTURE_PROPERTY_NDEVICES:
		*value = ndevices;
		break;
	case CAPTURE_PROPERTY_NSOURCES:
		*value = (ctx->pending.device == DEV0_CP_CORE)? nsources_cp:  nsources_sdp[ctx->pending.src_type];
		break;
	case CAPTURE_PROPERTY_VERBOSITY:
		*value = ctx->active.verbosity;
		break;
	case CAPTURE_PROPERTY_DECODER_I2C_ADDR:
		*value = ctx->dev_addr;
		break;
	case CAPTURE_PROPERTY_CSI2_NUM_DATA_LANES:
		*value = ctx->active.num_data_lanes;
		break;
	case CAPTURE_PROPERTY_SRC_FORMAT:
		 *value = ctx->active.output_format;
		break;
	case CAPTURE_PROPERTY_BRIGHTNESS:
	case CAPTURE_PROPERTY_CONTRAST:
	case CAPTURE_PROPERTY_SATURATION:
	case CAPTURE_PROPERTY_HUE:
		errno = ENOTSUP;
		rc = -1;
		break;
	default:
		errno = ENOENT;
		rc = -1;
		break;
	}
	pthread_mutex_unlock( &mutex );
	return rc;
}

/************************************************* CAPTURE_SET_PROPERTY_P() ***/
int
capture_set_property_p( capture_context_t ctx, uint32_t property, void *value )
{
	int rc = 0;
	TRACE;
	LOG("       ctx=%p, property=%08X ('%c', '%c', '%c', '%c'), value=%p", ctx, property,
		(property>>24) & 0x000000FF, (property>>16) & 0x000000FF, (property>>8) & 0x000000FF,
		property & 0x000000FF, value);

	if(property != CAPTURE_PROPERTY_DECODER_I2C_PATH ) {
		// "device" must be set first.
		if(ctx->pending.device == -1) {
			errno = ENODEV;
			return -1;
		}
	}

	pthread_mutex_lock( &mutex );
	switch (property) {
	case CAPTURE_PROPERTY_DECODER_I2C_PATH:
		LOG("       set: '%s' (CAPTURE_PROPERTY_DECODER_I2C_PATH)", (char*)value);
		if(strlen(value) < _POSIX_PATH_MAX) {
			strncpy(ctx->dev_path, value, _POSIX_PATH_MAX - 1);
			ctx->changes |= CAPTURE_CHANGE_DEVICE_ADDR;

		}
		else {
			errno = EINVAL;
			rc = -1;
		}
		break;
	case CAPTURE_PROPERTY_DEVICE_INFO:
	case CAPTURE_PROPERTY_CURRENT_NORM:
		errno = EROFS;
		rc = -1;
		break;
	case CAPTURE_PROPERTY_NORM:
		errno = ENOTSUP;
		rc = -1;
		break;
	default:
		errno = ENOENT;
		rc = -1;
		break;
	}
	pthread_mutex_unlock( &mutex );
	return rc;
}

/************************************************* CAPTURE_GET_PROPERTY_P() ***/
int
capture_get_property_p( capture_context_t ctx, uint32_t property, void **value )
{
	int rc = 0;

	TRACE;
	LOG("       ctx=%p, property=%08X ('%c', '%c', '%c', '%c'), value=%p", ctx, property,
		(property>>24) & 0x000000FF, (property>>16) & 0x000000FF, (property>>8) & 0x000000FF,
		property & 0x000000FF, value);

	if((property != CAPTURE_PROPERTY_DECODER_I2C_PATH) && (property != CAPTURE_PROPERTY_DEVICE_INFO) ) {
		// "device" must be set first.
		if(ctx->pending.device == -1) {
			errno = ENODEV;
			return -1;
		}
	}

	pthread_mutex_lock( &mutex );
	switch (property) {
	case CAPTURE_PROPERTY_DEVICE_INFO:
		*value = (char *)ctx->device_info;
		break;
	case CAPTURE_PROPERTY_DECODER_I2C_PATH:
		*value = (char *)ctx->dev_path;
		break;
	case CAPTURE_PROPERTY_CURRENT_NORM:
	case CAPTURE_PROPERTY_NORM:
		errno = ENOTSUP;
		rc = -1;
		break;
	default:
		errno = ENOENT;
		rc = -1;
		break;
	}
	pthread_mutex_unlock( &mutex );
	return rc;
}

/****************************************************** CAPTURE_GET_FRAME() ***/
int
capture_get_frame( capture_context_t ctx, uint64_t timeout, uint32_t flags )
{
	errno = ENOTSUP;
	return -1;
}

/************************************************** CAPTURE_RELEASE_FRAME() ***/
int
capture_release_frame( capture_context_t ctx, uint32_t idx )
{
	errno = ENOTSUP;
	return -1;
}

/***************************************************** CAPTURE_PUT_BUFFER() ***/
int
capture_put_buffer( capture_context_t ctx, uint32_t idx, uint32_t flags )
{
	errno = ENOTSUP;
	return -1;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/graphics/branches/release-2.1.x/hardware/capture/adv748x/decoder.c $ $Rev: 867357 $")
#endif
