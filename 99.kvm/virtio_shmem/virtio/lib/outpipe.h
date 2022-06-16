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
 * Definitions for outpipe
 */
#ifndef _QVM_OUTPIPE_H_INCLUDED
#define _QVM_OUTPIPE_H_INCLUDED


#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>


/*

  Attribute
  =========


           +--> Label
          /
         +
         |     +--> Returnable Code
         |     |
         |     |
      +-----+-----+-----+-----+
      |  L  |  R  |  U     C  |  4-byte Attribute
      +-----+-----+-----+-----+
  HI     |     |  |           |   LO
         |     |  +-----+-----+
         |     |        |
         |     |        +--> (Optional) User-defined code
         |     |
         |     |     +---+---+---+---+---------------+
         |     +-->  |   S   R   C   | C   O   D   E |  8-bit Returnable Code
         |           +---+---+---+---+---------------+
         |
         |     +---+---+---+---+---+---+---+---+
         +-->  |   N   F   E   W   I   V   D   |  8-bit Label
               +---+---+---+---+---+---+---+---+


      Label: iNternal, Fatal, Error, Warning, Info, Verbose, Debug

      Returnable Code:
        - Hi 4-bit Source : Internal, External (Configuration)
        - Lo 4-bit Code   : Unexpected, Unsupported, Exists, NoRes, Unable, Failed, Missing, Invalid

 */


/*
 * AL: Attribute - Label
 * Each output message can be labeled with one of the following QVM_OUTAL_* labels
 *
 * Rules for redirecting messages to different outpipes
 * can be defined by code or configuration language in the future
 *
 * e.g.
 *   OUTAL_ERROR | OUTAL_FATAL | OUTAL_INTERNAL  =>  stderr
 *   OUTAL_WARN                                  =>  stdout
 *   OUTAL_INFO  | OUTAL_DEBUG                   =>  slog
 */
#define QVM_OUTAL_OFFSET      24
#define QVM_OUTAL_MASK        (0xffu << QVM_OUTAL_OFFSET)
#define QVM_OUTAL_TRIM(attr)  (((attr) & QVM_OUTAL_MASK) >> QVM_OUTAL_OFFSET)


typedef enum qvm_outlbl qvm_outlbl_t;
/**
 * Label types
 */
enum qvm_outlbl {
    QVM_OUTAL_NONE,      /**< NOTE: Internal use only to indicate no label specified */

    QVM_OUTAL_INTERNAL,
    QVM_OUTAL_FATAL,
    QVM_OUTAL_ERROR,
    QVM_OUTAL_WARN,
    QVM_OUTAL_INFO,
    QVM_OUTAL_VERBOSE,
    QVM_OUTAL_DEBUG,

    QVM_OUT_NUM_LBLS     /**< NOTE: Internal use only to calculate how many members in total */
};


#define QVM_OUTAC_OFFSET      16
#define QVM_OUTAC_MASK        (0xffu << QVM_OUTAC_OFFSET)
#define QVM_OUTAC_TRIM(attr)  (((attr) & QVM_OUTAC_MASK) >> QVM_OUTAC_OFFSET)

#define QVM_OUTAC_SRC_MASK    ((0xf0u << QVM_OUTAC_OFFSET) & QVM_OUTAC_MASK)
#define QVM_OUTAC_INT_MASK    ((0xf0u << QVM_OUTAC_OFFSET) & QVM_OUTAC_SRC_MASK)
#define QVM_OUTAC_EXT_MASK    ((0x70u << QVM_OUTAC_OFFSET) & QVM_OUTAC_SRC_MASK)


/** Source of log entity as a part of return code */
typedef enum qvm_outrs qvm_outrs_t;
/**
 * Source of log entity as a part of return code
 */
enum qvm_outrs {
    /** Internal */
    QVM_OUTAC_INT    = ((0x80u << QVM_OUTAC_OFFSET) & QVM_OUTAC_INT_MASK),
    /** External Configuration */
    QVM_OUTAC_XCONF  = ((0x40u << QVM_OUTAC_OFFSET) & QVM_OUTAC_EXT_MASK),
    QVM_OUTAC_GST    = ((0x20u << QVM_OUTAC_OFFSET) & QVM_OUTAC_EXT_MASK)
};


#define QVM_OUTAC_CODE_MASK  ((0x0fu << QVM_OUTAC_OFFSET) & QVM_OUTAC_MASK)


typedef enum qvm_outrc qvm_outrc_t;
/**
 * Return Code
 */
enum qvm_outrc {
    /**
     * No error
     */
    QVM_OUTAC_OK           = ((0x00u << QVM_OUTAC_OFFSET) & QVM_OUTAC_CODE_MASK),
    /**
     * Unexpected result or input
     */
    QVM_OUTAC_UNEXPECTED   = ((0x01u << QVM_OUTAC_OFFSET) & QVM_OUTAC_CODE_MASK),
    /**
     * Unsupported object or operation
     */
    QVM_OUTAC_UNSUPPORTED  = ((0x02u << QVM_OUTAC_OFFSET) & QVM_OUTAC_CODE_MASK),

    /**
     * Object exists
     */
    QVM_OUTAC_EXISTS       = ((0x03u << QVM_OUTAC_OFFSET) & QVM_OUTAC_CODE_MASK),
    /**
     * No res or such thing found/present/available/exists (mem, space, etc)
     */
    QVM_OUTAC_NORES        = ((0x04u << QVM_OUTAC_OFFSET) & QVM_OUTAC_CODE_MASK),

    /**
     * Unable to do something, such as unable to determine, obtain, lock, find, load something
     */
    QVM_OUTAC_UNABLE       = ((0x05u << QVM_OUTAC_OFFSET) & QVM_OUTAC_CODE_MASK),
    /**
     * Failed to do something, such as failed to set, create, open, read, write, etc
     */
    QVM_OUTAC_FAILED       = ((0x06u << QVM_OUTAC_OFFSET) & QVM_OUTAC_CODE_MASK),

    /**
     * Missing/expecting something, such as missing argument, file name, data, etc.
     */
    QVM_OUTAC_MISSING      = ((0x07u << QVM_OUTAC_OFFSET) & QVM_OUTAC_CODE_MASK),
    /**
     * Invalid object/state/operation
     */
    QVM_OUTAC_INVALID      = ((0x08u << QVM_OUTAC_OFFSET) & QVM_OUTAC_CODE_MASK)
};

/**
 * User-defined Code
 */
#define QVM_OUTAU_OFFSET  0
#define QVM_OUTAU_MASK    ((0xffffu << QVM_OUTAU_OFFSET))


/**
 * Generates an attribute with given inputs
 * @param   lbl  a QVM_OUTAL_ label, e.g. QVM_OUTAL_ERROR
 * @param   rs   a source type as a part of return code
 * @param   rc   a code type as a part of return code
 * @param   uc   a user-defined code to indicate a specific error or purpose
 * @returns a combined attriubte based on given inputs
 */
static inline unsigned int
qvm_outattr(const qvm_outlbl_t lbl, const qvm_outrs_t rs, const qvm_outrc_t rc, const unsigned int uc) {
    return ((lbl << QVM_OUTAL_OFFSET) & QVM_OUTAL_MASK) | (rs & QVM_OUTAC_SRC_MASK) | (rc & QVM_OUTAC_CODE_MASK) | (uc & QVM_OUTAU_MASK);
}


/** Appender Type */
typedef enum qvm_outapndr qvm_outapndr_t;
/**
 * Appender Type
 * A character which represents what kind of appender it is
 */
enum qvm_outapndr {
    QVM_OUTAT_STDERR,
    QVM_OUTAT_STDOUT,
    QVM_OUTAT_SLOG,

    QVM_OUT_MAX_APPENDERS  /**< internal use only to know how many appenders in total */
};


/**
 * Outpipe Context
 * Which is used to define an outpipe
 * Defines which switches are on/off, where output message goes
 */
typedef struct qvm_outctx qvm_outctx_t;

/**
 * Appender Function Pointer
 * @param  ctx   outpipe context it tells what outpipe that given message goes
 * @param  attr  message-level attribute
 * @param  fmt   printf-like format string
 * @param  args  arguments for format above
 */
typedef void (*qvm_apndrfp_t)(const qvm_outctx_t *const ctx, const unsigned int attr, const char *const fmt, va_list args);

/**
 * Outpipe Context
 * Which is used to define an outpipe
 * Defines which switches are on/off, where output message goes
 */
struct qvm_outctx {
    /** Filter switch, each bit represents a label, 1 means its on, 0 off, see "qvm_outlbl" */
    unsigned        filter;
    /** Appender Type */
    qvm_outapndr_t  appender_type;
    /** An appender (destination) for outputing, like stderr, slog */
    qvm_apndrfp_t   appender;
};

/**
 * initializes given configuration entries.
 * Usually called at beginning of the program to get loggers initialized with default behaviors as expected.
 * @param   conf       a pointer to an configuration entry or entry array, which is preprocessed by configuration language.
 * @param   num_conf   number of elements that pointer conf points to
 * @returns 0 on success, otherwise on failure
 */
int
qvm_outpipe_init(qvm_outctx_t *const conf, const unsigned int num_conf);

/**
 * releases all outpipe related resources
 * @returns 0 on success, otherwise on failure
 */
int
qvm_outpipe_destroy(void);

/**
 * loads given configuration entries.
 * Usually called whenever a new logger option parsed.
 * @param   conf       a pointer to an configuration entry or entry array, which is preprocessed by configuration language.
 * @param   num_conf   number of elements that pointer conf points to
 * @returns 0 on success, otherwise on failure
 */
int
qvm_outpipe_load(qvm_outctx_t *const conf, unsigned int num_conf);


/**
 * Outputs a printf-like string to outpipes defined in configuration
 * @param  attr  message-level attribute
 *               NOTE: For QVM_OUTAL_FATAL or QVM_OUTAL_INTERNAL, use qvm_outff() instead
 * @param  fmt   a printf-like formatted string
 * @param  ...   optional arguments for format string above
 */
void
qvm_outf(const unsigned int attr, const char *const fmt, ...);


/**
 * Outputs a printf-like string to outpipes defined in configuration
 * It's similar to the qvm_outf(), but designed for FATAL/INTERNAL levels only, program will be terminated right after
 * And it should only be called for these levels, all others should go with qvm_outf()
 * Also, this one declares as noreturn for avoding klocwork warnings, or
 * klocwork may complain "something" after qvm_outf() call as "something" may be dereferenced
 * @param  attr  message-level attribute
 * @param  fmt   a printf-like formatted string
 * @param  ...   optional arguments for format string above
 */
void
qvm_outff(const unsigned int attr, const char *const fmt, ...) __attribute__((noreturn));


/**
 * Outputs a printf-like string to outpipes defined in configuration
 * @param  attr  message-level attribute
 *               NOTE: If QVM_OUTAL_FATAL flag is on/passed, program will exit right after output done
 * @param  fmt   a printf-like formatted string
 * @param  ap    a va_list of aditional arguments required by format string fmt
 */
void
qvm_voutf(const unsigned int attr, const char *const fmt, va_list ap);


/*
 * Predefined error/logging attributes for configuration related stuff
 */
#define QVM_OUTF_CNF          (qvm_outattr(QVM_OUTAL_FATAL,    QVM_OUTAC_XCONF, QVM_OUTAC_UNEXPECTED,  0))

#define QVM_OUTE_CNF          (qvm_outattr(QVM_OUTAL_ERROR,    QVM_OUTAC_XCONF, QVM_OUTAC_UNEXPECTED,  0))

#define QVM_OUTW_CNF          (qvm_outattr(QVM_OUTAL_WARN,     QVM_OUTAC_XCONF, QVM_OUTAC_UNEXPECTED,  0))

#define QVM_OUTN_QVM          (qvm_outattr(QVM_OUTAL_INTERNAL, QVM_OUTAC_INT,   QVM_OUTAC_UNEXPECTED,  0))

#define QVM_OUTF_QVM          (qvm_outattr(QVM_OUTAL_FATAL,    QVM_OUTAC_INT,   QVM_OUTAC_UNEXPECTED,  0))

#define QVM_OUTE_QVM          (qvm_outattr(QVM_OUTAL_ERROR,    QVM_OUTAC_INT,   QVM_OUTAC_UNEXPECTED,  0))

#define QVM_OUTW_QVM          (qvm_outattr(QVM_OUTAL_WARN,     QVM_OUTAC_INT,   QVM_OUTAC_UNEXPECTED,  0))

#define QVM_OUTI_QVM          (qvm_outattr(QVM_OUTAL_INFO,     QVM_OUTAC_INT,   QVM_OUTAC_OK,          0))

#define QVM_OUTD_QVM          (qvm_outattr(QVM_OUTAL_DEBUG,    QVM_OUTAC_INT,   QVM_OUTAC_OK,          0))

#define QVM_OUTV_QVM          (qvm_outattr(QVM_OUTAL_VERBOSE,  0,               QVM_OUTAC_OK,          0))

#define QVM_OUTE_GST          (qvm_outattr(QVM_OUTAL_ERROR,    QVM_OUTAC_GST,   QVM_OUTAC_UNEXPECTED,  0))



#endif


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.0.0/trunk/utils/q/qvm/public/qvm/outpipe.h $ $Rev: 849478 $")
#endif
