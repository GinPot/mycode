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
 * qvm.h   QNX Virtual Machine definitions
 *
 */
#ifndef __QVM_H_INCLUDED
#define __QVM_H_INCLUDED

#ifndef __PLATFORM_H_INCLUDED
#include <sys/platform.h>
#endif


#define QVM_VERSION_MAJOR	1u
#define QVM_VERSION_MINOR	0u

#define QVM_GLOBAL_ID_NAME	"QVM global ID"

/**
 * Message sub-types supported by the QVM module.
 */
enum qvm_msg_subtypes {
	/** See ::qvm_version */
	QVM_VERSION,
};


/**
 * Offsets from the start of the control page dictate the operation
 */
enum qvm_control_operations {
	QCO_VC_THREAD,					/**< Mark current thread as a virtual CPU */
	QCO_FORCE_GUEST_EXIT,			/**< Force a guest virtual machine exit */

	QCO_ARCH_SPECIFIC	= 0x100,	/**< Start of architecture specific operations */

	QCO_X86_GET_POST_VECTOR  = QCO_ARCH_SPECIFIC, /**< Return the posted interrupt vector */
	QCO_X86_FLUSH_VMCS,				/**< Flush any cached info to VMCS memory */
	QCO_X86_SEND_POSTED, 			/**< IPI another CPU with the posted interrupt vector  */
};

/**
 * Layout of the ::QVM_VERSION message.
 */
struct qvm_version {
	/** Must be ::_QVM_BASE */
	_Uint16t				type;
	/** Must be ::QVM_VERSION */
	_Uint16t				subtype;
	_Uint32t				zero1;
	_Uint64t				control_page;
	_Uint32t				zero[4];
};

/**
 * Layout of the ::QVM_VERSION reply.
 */
struct qvm_version_reply {
	struct {
		_Uint32t	major;	/**< QVM_VERSION_MAJOR */
		_Uint32t	minor;	/**< QVM_VERSION_MINOR */
	}				version;
	_Uint32t		cert;
	_Uint32t		reason_len;	/**< length of following reason field */
	_Uint64t		reserved[4];
	char			reason[]; 	/**< variably sized: string of failure reason */
};



/**
 * Union of possible message layouts for the type ::_QVM_BASE.
 */
union qvm_op {
	struct qvm_version		 			version;
};


#define QVM_GUEST_ID_NONE	(~(uint64_t)0)
#endif


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.0.0/trunk/services/system/public/sys/qvm.h $ $Rev: 821048 $")
#endif
