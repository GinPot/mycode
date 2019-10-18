/*
 * $QNXLicenseC:
 * Copyright 2011, QNX Software Systems. All Rights Reserved.
 *
 * You must obtain a written license from and pay applicable
 * license fees to QNX Software Systems before you may reproduce,
 * modify or distribute this software, or any work that includes
 * all or part of this software.   Free development licenses are
 * available for evaluation and non-commercial purposes.  For more
 * information visit http://licensing.qnx.com or email
 * licensing@qnx.com.
 *
 * This file may contain contributions from others.  Please review
 * this entire file for other proprietary rights or license notices,
 * as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 * */

#ifndef WFD_COMMON_H_
#define WFD_COMMON_H_

#ifndef GCCATTR
# if __GNUC__ >= 4
#  define GCCATTR(x) __attribute__ ((x))
# else
#  define GCCATTR(x)
# endif
#endif

// SYM_PRIVATE indicates that the marked symbol won't be visible outside
// the compiled library.  It can be exported via a pointer, or used by
// other source files that are part of the same library.
#ifndef SYM_PRIVATE
# define SYM_PRIVATE GCCATTR(visibility ("hidden"))
#endif

// SYM_INTERNAL_ONLY indicates the symbol won't be used outside the library,
// even via a function pointer.
#ifndef SYM_INTERNAL_ONLY
# define SYM_INTERNAL_ONLY GCCATTR(visibility ("internal"))
#endif

SYM_INTERNAL_ONLY int create_temp_shm_object(void);

#endif /* WFD_COMMON_H_ */


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/graphics/branches/dev-renesas-hypervisor/hardware/wfd/common/wfd_common.h $ $Rev: 782316 $")
#endif
