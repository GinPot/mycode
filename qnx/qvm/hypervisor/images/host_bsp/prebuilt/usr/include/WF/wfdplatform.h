/*********************************************************************
 *                                                                   *
 * Sample implementation of wfdplatform.h, version 1.0               *
 *                                                                   *
 * Copyright © 2008-2009 The Khronos Group                           *
 *                                                                   *
 *********************************************************************/

#ifndef _WFDPLATFORM_H_
#define _WFDPLATFORM_H_

#include <KHR/khrplatform.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__QNX__) && defined(__GNUC__)
# ifndef WFD_API_CALL
#   define WFD_API_CALL __attribute__((visibility("default")))
# endif
#endif

#ifndef WFD_API_CALL
#define WFD_API_CALL KHRONOS_APICALL
#endif
#ifndef WFD_APIENTRY
#define WFD_APIENTRY KHRONOS_APIENTRY
#endif
#ifndef WFD_APIEXIT
#define WFD_APIEXIT KHRONOS_APIATTRIBUTES
#endif

typedef enum
{ WFD_FALSE = KHRONOS_FALSE,
  WFD_TRUE  = KHRONOS_TRUE
} WFDboolean;

typedef khronos_uint8_t             WFDuint8;
typedef khronos_int32_t             WFDint;
typedef khronos_float_t             WFDfloat;
typedef khronos_uint32_t            WFDbitfield;
typedef khronos_uint32_t            WFDHandle;
typedef khronos_utime_nanoseconds_t WFDtime;

#define WFD_FOREVER                 (0xFFFFFFFFFFFFFFFFLL)

#if defined (__QNXNTO__)
typedef void*  WFDEGLDisplay; /* An opaque handle to an EGLDisplay */
typedef void*  WFDEGLSync;    /* An opaque handle to an EGLSyncKHR */
typedef struct win_image*         WFDEGLImage;   /* An opaque handle to an EGLImage */
typedef struct win_native_stream* WFDNativeStreamType;
#else
#error "Platform not recognized"
#endif

#define WFD_INVALID_SYNC            ((WFDEGLSync)0)

#ifdef __cplusplus
}
#endif

#endif /* _WFDPLATFORM_H_ */



#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/graphics/branches/dev-renesas-hypervisor/lib/khronos/public/WF/wfdplatform.h $ $Rev: 844939 $")
#endif
