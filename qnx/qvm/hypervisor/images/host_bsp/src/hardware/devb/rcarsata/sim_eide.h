/*
 * $QNXLicenseC:
 * Copyright 2016, QNX Software Systems.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

// Module Description: eide header file

#ifndef _EIDE_H_INCLUDED
#define _EIDE_H_INCLUDED

#include <time.h>
#include <dlfcn.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sched.h>
#include <signal.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <gulliver.h>
#include <sys/syspage.h>
#include <sys/dcmd_cam.h>
#include <sys/slogcodes.h>
#include <sys/cam_device.h>

#include <hw/inout.h>
#include <hw/sysinfo.h>
#include <hw/dcmd_sim_eide.h>

#include <ntocam.h>
#include <sim.h>
#include <module.h>

#include <sim_sata.h>
#include <sim_ds.h>


/*****************************************************************************/
#define SATA_DEBUG                          1

#if (SATA_DEBUG == 1)
    #define EIDE_DEBUG
    #define EIDE_ATA_DEBUG
    #define EIDE_ATAPI_DEBUG
    #define EIDE_SATA_DEBUG
    #define EIDE_SATL_DEBUG

    #define UNUSED(_a)
#else
    #define UNUSED(_a)  (_a) = (_a)
#endif

#define SATA_NO_PRINT_DUMP      /* No print debug */

#define SATA_NO_DEVICE_PARAMS   /* Do not parse SATA device parameters */

/* Set TRACE_EXEC to 1 to log execution trace (called functions) */
#define TRACE_EXEC                          0

#if (TRACE_EXEC == 1)
    #define TRACE_LEVEL                     0//10
    #define EXEC_TRACE                                                      \
        eide_slogf(NULL, _SLOGC_SIM_RCAR_SATA,                              \
                            _SLOG_DEBUG2, TRACE_LEVEL, "%s", __FUNCTION__)
#else
    #define EXEC_TRACE
#endif

/* Plug and Play is not supported */
#define SATA_PNP_SUPPORT                    1

/* ATAPI supported */
#define EIDE_ATAPI_SUPPORT

/* Physical memory must be below 4G, for DMA transfers */
#define EIDE_CHECK_BELOW_4G_PHYS_MEM        0

/*****************************************************************************/
#define SIM_EIDE_VERSION                    100
#define SIM_EIDE_LETTER                     'A'

#define EIDE_PCI_VENDOR_ID_WILDCARD         0xffff
#define EIDE_PCI_DEVICE_ID_WILDCARD         0xffff

#define EIDE_ARG_VAL(_o, _v)                                                \
    if ((_v) == NULL || *(_v) == '\0')                                      \
    {                                                                       \
        eide_slogf(NULL, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,              \
                "%s:  Missing argument for '%s'", __FUNCTION__, _o);        \
        break;                                                              \
    }

#define EIDE_ARG_NOVAL(_o, _v) if ((_v) != NULL)                            \
        { eide_slogf(NULL, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,            \
        "%s:  Unexpected argument for '%s'", __FUNCTION__, _o); break; }

#define EIDE_ARG_NUMBER(_o, _n, _f) if ((*(_n) = (_f)) == -1)               \
        { eide_slogf(NULL, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,            \
        "%s:  Invalid numeric argument for '%s'", __FUNCTION__, _o); break; }

#define EIDE_ARG_MISSING(_o, _a) if (!(_a))                                 \
        { eide_slogf(NULL, _SLOGC_SIM_RCAR_SATA, _SLOG_ERROR, 1,            \
        "%s:  Missing argument component for '%s'", __FUNCTION__, _o); break; }

/*****************************************************************************/
#define EIDE_MAX_HBA                                1   /* 8 */ /* Maximum number of EIDE cards */
#define EIDE_MAX_TID                                1   /* 8 */
#define EIDE_MAX_TARGETS                            1   /* 15 */

#define EIDE_TSTATE_CREATING                        0x00
#define EIDE_TSTATE_INITIALIZED                     0x01
#define EIDE_TSTATE_INIT_FAILURE                    0x02

#define EIDE_CONTEXT_PROCESS                        0x01
#define EIDE_CONTEXT_INTR                           0x02
#define EIDE_SCHED_PRIORITY                         21
#define EIDE_DFLT_TIMEOUT                           5
#define EIDE_INIT_NORESET                           0
#define EIDE_INIT_RESET                             1
#define EIDE_STACK_SIZE                             8192
#define EIDE_WS_SIZE                                4096
#define EIDE_PG_SIZE                                4096

#define EIDE_RESET_TIME                             40
#define EIDE_ATA_DEVICE                             0
#define EIDE_ATAPI_DEVICE                           2

#define EIDE_CMDSET_ATA                             0x00
#define EIDE_CMDSET_ATAPI                           0x02

#define CCB_REQSENSE_MAX                            0x24

#define EIDE_SENSE_FIXED                            0x00
#define EIDE_SENSE_DESC                             0x01
#define EIDE_SENSE_EXTEND                           0x02

/* EIDE commands */
#define EIDE_CMD_MEDIA_EJECT                        0xed    /* Eject media */
#define EIDE_CMD_ACK_MEDIA_CHNG                     0xdb    /* Ack. media change */
#define EIDE_CMD_BOOT_POST_BOOT                     0xdc
#define EIDE_CMD_BOOT_PRE_BOOT                      0xdd
#define EIDE_CMD_CHECK_POWER_MODE                   0xe5
#define EIDE_CMD_DOOR_LOCK                          0xde
#define EIDE_CMD_DOOR_UNLOCK                        0xdf
#define EIDE_CMD_EXECUTE_DRV_DIAGS                  0x90
#define EIDE_CMD_DOWNLOAD_MICROCODE                 0x92
 #define EIDE_DM_TEMP                               0x01
 #define EIDE_DM_SAVE                               0x07
#define EIDE_CMD_FORMAT_TRACK                       0x50
#define EIDE_CMD_ATA_IDENTIFY                       0xec
#define EIDE_CMD_IDLE                               0xe3
#define EIDE_CMD_IDLE_IMMEDIATE                     0xe1
#define EIDE_CMD_INIT_DRV_PARMS                     0x91
#define EIDE_CMD_NOP                                0x00
 #define EIDE_CMD_FEAT_FLUSHQUEUE                   0x00
#define EIDE_CMD_ATAPI_PKT_CMD                      0xa0
#define EIDE_CMD_ATAPI_IDENTIFY                     0xa1
#define EIDE_CMD_SERVICE                            0xa2
#define EIDE_CMD_SMART                              0xb0
 #define EIDE_SMART_SIG                             0xc24f
 #define EIDE_SMART_LBA_MID_SIG                     0x4f
 #define EIDE_SMART_LBA_HI_SIG                      0xc2
 #define EIDE_SMART_LBA_MID_SIG_FAULT               0xf4
 #define EIDE_SMART_LBA_HI_SIG_FAULT                0x2c

 #define EIDE_SMART_READ_DATA                       0xd0
 #define EIDE_SMART_READ_THRESHOLDS                 0xd1
 #define EIDE_SMART_ATTRIBUTE_AUTOSAVE              0xd2
  #define EIDE_SMART_ENABLE_ATTRIBUTE_AUTOSAVE      0x00
  #define EIDE_SMART_DISABLE_ATTRIBUTE_AUTOSAVE     0xf1
 #define EIDE_SMART_ATTRIBUTE_SAVE                  0xd3
 #define EIDE_SMART_EXECUTE_OFFLINE                 0xd4
  #define EIDE_SMART_OFFLINE                        0x00
  #define EIDE_SMART_SHORT_SELFTEST                 0x01
  #define EIDE_SMART_EXTENDED_SELFTEST              0x02
  #define EIDE_SMART_CONVEYANCE_SELFTEST            0x03
  #define EIDE_SMART_SELECTIVE_SELFTEST             0x04
  #define EIDE_SMART_ABORT_OFFLINE                  0x7f
  #define EIDE_SMART_SHORT_SELFTEST_CAPTIVE         0x81
  #define EIDE_SMART_EXTENDED_SELFTEST_CAPTIVE      0x82
  #define EIDE_SMART_CONVEYANCE_SELFTEST_CAPTIVE    0x83
  #define EIDE_SMART_SELECTIVE_SELFTEST_CAPTIVE     0x84
 #define EIDE_SMART_READ_LOG                        0xd5
 #define EIDE_SMART_WRITE_LOG                       0xd6
  #define EIDE_SMART_LA_DIRECTORY_LOG               0x00
  #define EIDE_SMART_LA_SUMMARY_LOG                 0x01
  #define EIDE_SMART_LA_COMPREHENSIVE_LOG           0x02
  #define EIDE_SMART_LA_COMPREHENSIVE_EXT_LOG       0x03
  #define EIDE_SMART_LA_SELF_TEST_LOG               0x06
  #define EIDE_SMART_LA_SELF_TEST_EXT_LOG           0x07
  #define EIDE_SMART_LA_SELECTIVE_SELF_TEST_LOG     0x09
  #define EIDE_SMART_LA_STREAMING_PERF_LOG          0x20
  #define EIDE_SMART_LA_WRITE_STREAM_LOG            0x21
  #define EIDE_SMART_LA_READ_STREAM_LOG             0x22
  #define EIDE_SMART_LA_DELAYED_SECTOR_LOG          0x23
 #define EIDE_SMART_ENABLE_OPERATIONS               0xd8
 #define EIDE_SMART_DISABLE_OPERATIONS              0xd9
 #define EIDE_SMART_STATUS                          0xda
#define EIDE_CMD_READ_LOG_EXT                       0x2f
#define EIDE_CMD_WRITE_LOG_EXT                      0x3f
#define EIDE_CMD_ATAPI_SOFT_RESET                   0x08
#define EIDE_CMD_READ_BUFFER                        0xe4
#define EIDE_CMD_PM_READ                            0xe4    /* PMP read */
#define EIDE_CMD_READ_DMA_QUEUED                    0xc7    /* w/retry */
#define EIDE_CMD_READ_DMA_QUEUED_EXT                0x26
#define EIDE_CMD_READ_DMA                           0xc8    /* w/retry */
#define EIDE_CMD_READ_DMA_R                         0xc9    /* wo/retry */
#define EIDE_CMD_READ_DMA_EXT                       0x25
#define EIDE_CMD_READ_LONG                          0x22    /* w/retry */
#define EIDE_CMD_READ_LONG_R                        0x23    /* wo/retry */
#define EIDE_CMD_READ_MULTIPLE                      0xc4
#define EIDE_CMD_READ_MULTIPLE_EXT                  0x29
#define EIDE_CMD_READ_SECTORS                       0x20    /* w/retry */
#define EIDE_CMD_READ_SECTORS_R                     0x21    /* wo/retry */
#define EIDE_CMD_READ_SECTORS_EXT                   0x24
#define EIDE_CMD_READ_VER_SECTORS                   0x40    /* w/retry */
#define EIDE_CMD_READ_VER_SECTORS_R                 0x41    /* wo/retry */
#define EIDE_CMD_READ_VER_SECTORS_EXT               0x42
#define EIDE_CMD_RECALIBRATE                        0x10
#define EIDE_CMD_SEEK                               0x70
#define EIDE_CMD_SET_FEATURE                        0xef
 #define EIDE_FEAT_8BIT                             0x01    /* 8 bit I/O (retired ATA-4+) */
 #define EIDE_FEAT_EN_WCACHE                        0x02
 #define EIDE_FEAT_XFER_MODE                        0x03
 #define EIDE_FEAT_EN_APM                           0x05
  #define EIDE_APM_LEVEL_MAX                        0xfe
  #define EIDE_APM_LEVEL_STANDBY                    0x7f    /* >= 0x7f puts device in standby */
 #define EIDE_FEAT_POWER_UP_STANDBY                 0x06
 #define EIDE_FEAT_STANDBY_SPINUP                   0x07
 #define EIDE_XFER_MODE_PIO_FLOW                    0x08
 #define EIDE_XFER_MODE_SDMA                        0x10
 #define EIDE_XFER_MODE_MDMA                        0x20
 #define EIDE_XFER_MODE_UDMA                        0x40
 #define EIDE_FEAT_DIS_MEDIA_STATUS                 0x31
 #define EIDE_FEAT_DIS_READ_AHEAD                   0x55
 #define EIDE_FEAT_DIS_WCACHE                       0x82
 #define EIDE_FEAT_DIS_APM                          0x85
 #define EIDE_FEAT_DIS_POWER_UP_STANDBY             0x86
 #define EIDE_FEAT_MEDIA_STATUS                     0x95
 #define EIDE_FEAT_EN_READ_AHEAD                    0xaa
 #define EIDE_FEAT_DIS_RLSE_INTR                    0xdd
 #define EIDE_FEAT_DIS_SERV_INTR                    0xde

#define EIDE_CMD_SET_MULTIPLE_MODE                  0xc6
#define EIDE_CMD_SLEEP                              0xe6
#define EIDE_CMD_STANDBY                            0xe2
#define EIDE_CMD_STANDBY_IMMEDIATE                  0xe0
#define EIDE_CMD_WRITE_BUFFER                       0xe8
#define EIDE_CMD_PM_WRITE                           0xe8    /* PMP write */
#define EIDE_CMD_WRITE_DMA                          0xca    /* w/retry */
#define EIDE_CMD_WRITE_DMA_R                        0xcb    /* wo/retry */
#define EIDE_CMD_WRITE_DMA_EXT                      0x35
#define EIDE_CMD_WRITE_DMA_QUEUED                   0xcc    /* w/retry */
#define EIDE_CMD_WRITE_DMA_QUEUED_EXT               0x36
#define EIDE_CMD_WRITE_LONG                         0x32    /* w/retry */
#define EIDE_CMD_WRITE_LONG_R                       0x33    /* wo/retry */
#define EIDE_CMD_WRITE_MULTIPLE                     0xc5
#define EIDE_CMD_WRITE_MULTIPLE_EXT                 0x39
#define EIDE_CMD_SAME                               0xe9
#define EIDE_CMD_WRITE_SECTORS                      0x30    /* w/retry */
#define EIDE_CMD_WRITE_SECTORS_R                    0x31    /* wo/retry */
#define EIDE_CMD_WRITE_SECTORS_EXT                  0x34
#define EIDE_CMD_WRITE_VERIFY                       0x3c
#define EIDE_CMD_FLUSH_CACHE                        0xe7
#define EIDE_CMD_FLUSH_CACHE_EXT                    0xea
#define EIDE_CMD_READ_NATIVE_MAX_ADDR               0xf8
#define EIDE_CMD_READ_NATIVE_MAX_ADDR_EXT           0x27
#define EIDE_CMD_REQUEST_SENSE                      0x03    /* CF + Request Sense */
#define EIDE_CMD_GET_MEDIA_STATUS                   0xda
#define EIDE_CMD_DATA_SET_MGNT                      0x06
 #define EIDE_DSM_TRIM                              0x01
 #define EIDE_DSMR_MAX_LBA                          0xffff
 #define EIDE_DSMR_MAX_RNG                          4096 / 8

/*****************************************************************************/
#define ATA_TF_INTR_WAIT        0x01
#define ATA_TF_BUSY_WAIT        0x02
#define ATA_TF_IMMEDIATE        0x04
#define ATA_TF_TYPE_MSK         0x07
#define ATA_TF_EXP              0x08
#define ATA_TF_ATA              0x10
#define ATA_TF_ATAPI            0x20
#define ATA_TF_DIR_IN           0x40
#define ATA_TF_DIR_OUT          0x80
#define ATA_TF_DIR_MSK          0xC0
/*****************************************************************************/
// ATAPI interrupt types
#define ATAPI_STATUS            0x03
#define ATAPI_DATA_OUT          0x04
#define ATAPI_COMMAND           0x05
#define ATAPI_DATA_IN           0x06
#define ATAPI_MESSAGE           0x07

/*****************************************************************************/
/* ATA/ATAPI Register Map */
typedef struct _ata_taskfile {
    uint16_t    tflags;
    uint8_t     did;            /* PATA - 0 or 1, SATA pm 1 - 15 */
    uint8_t     control;        /* Device control           W */
    uint8_t     device;         /* Drive/head               W */
    uint8_t     command;        /* Command                  W */
    uint8_t     error;          /* Error (ATA/ATAPI)        R */
    uint8_t     status;         /* Status                   R */
    uint8_t     astatus;        /* Alternate status         R */
    uint8_t     rsvd;
    uint8_t     feature;        /* Features (ATA) W, intr reason (ATAPI) R */
    uint8_t     hob_feature;    /* Features (ATA) W, intr reason (ATAPI) R */
    uint8_t     seccnt;         /* Sector count (ATA)       R/W 2 */
    uint8_t     hob_seccnt;     /* Sector count (ATA)       R/W 2 */
    uint8_t     lbal;           /* LBA Low (ATA)            R/W 3 */
    uint8_t     hob_lbal;       /* LBA Low (ATA)            R/W 3 */
    uint8_t     lbam;           /* LBA Mid (ATA) - bcount low (ATAPI)   R/W 4 */
    uint8_t     hob_lbam;       /* LBA Mid (ATA) - bcount low (ATAPI)   R/W 4 */
    uint8_t     lbah;           /* LBA High (ATA) - bcount hi (ATAPI)   R/W 5 */
    uint8_t     hob_lbah;       /* LBA High (ATA) - bcount hi (ATAPI)   R/W 5 */
} ATA_TASKFILE;

/* ATA/ATAPI Error Register */
#define EIDE_ERR                                0x01
 /* ATAPI error register bit masks */
 #define EIDE_ERR_ATAPI_ERR_SKEY                0xf0    /* Sense keys           */
 /* ATAPI Sense Key and Sense code definitions continued from above  */
  #define ATAPI_SENSE_NOSENSE                   0x00
  #define ATAPI_SENSE_RECOVERED_ERROR           0x10
  #define ATAPI_SENSE_NOT_READY                 0x20
  #define ATAPI_SENSE_MEDIUM_ERROR              0x30
  #define ATAPI_SENSE_HARDWARE_ERROR            0x40
  #define ATAPI_SENSE_ILLEGAL_REQUEST           0x50
  #define ATAPI_SENSE_UNIT_ATTENTION            0x60
  #define ATAPI_SENSE_DATA_PROTECT              0x70
  #define ATAPI_SENSE_ABORTED_COMMAND           0xB0
  #define ATAPI_SENSE_MISCOMPARE                0xE0
 #define EIDE_ERR_ATAPI_ERR_MCH                 0x20    /* Media changed        */
 #define EIDE_ERR_ATAPI_ERR_MCR                 0x08    /* Media change request */
 #define EIDE_ERR_ATAPI_ERR_ABRT                0x04    /* Aborted command      */
 #define EIDE_ERR_ATAPI_ERR_EOM                 0x02    /* End of media         */
 #define EIDE_ERR_ATAPI_ERR_ILI                 0x01    /* Illegal length       */

 /* ATA error register bit masks */
 #define EIDE_ERR_ATA_BBK                       0x80    /* Bad Block                */
 #define EIDE_ERR_ATA_CRC                       0x80    /* CRC Error - UDMA only    */
 #define EIDE_ERR_ATA_WP                        0x40    /* Write protected          */
 #define EIDE_ERR_ATA_UNC                       0x40    /* Uncorrectable data error */
 #define EIDE_ERR_ATA_MC                        0x20    /* Media Changed            */
 #define EIDE_ERR_ATA_IDNF                      0x10    /* ID mark Not Found        */
 #define EIDE_ERR_ATA_MCR                       0x08    /* Media Change Requested   */
 #define EIDE_ERR_ATA_ABRT                      0x04    /* Command aborted          */
 #define EIDE_ERR_ATA_NM                        0x02    /* No media                 */
 #define EIDE_ERR_ATA_TKONF                     0x02    /* Track 0 Not Found        */
 #define EIDE_ERR_ATA_AMNF                      0x01    /* Address Mark Not Found   */

/* ATA/ATAPI Feature Register */
#define EIDE_FEATURES               0x01
 #define ATAPI_TAG_TYPE             0x70
 #define ATAPI_PIO                  0x00
 #define ATAPI_DMA                  0x01
 #define ATAPI_OVL                  0x01

/* ATAPI Interrupt Reason Register (ATA Sector Count Register) */
#define EIDE_SECTOR_COUNT           0x02
 #define EIDE_SECCNT_RELEASE        0x04
 #define EIDE_SECCNT_IO             0x02
 #define EIDE_SECCNT_COD            0x01

#define EIDE_SECTOR_NUM             0x03        /* AKA LBA Low */

/* ATAPI Byte Count Register Low / ATA Cylinder Low Register */
#define EIDE_CYL_LO                 0x04    /* AKA LBA Mid */
#define ATAPI_CYLLO_SIG             0x14    /* Valid after ATA identify or read */

/* ATAPI Byte Count Register High / ATA Cylinder High Register */
#define EIDE_CYL_HI                 0x05    /* AKA LBA High */
#define ATAPI_CYLHI_SIG             0xEB    /* Valid after ATA identify or read */

/* ATA/ATAPI Device/Head Select Register */
#define EIDE_DH                     0x06
 #define EIDE_SELECT_DRIVE(_d)      (0xA0 | ((_d) << 4))
 #define EIDE_CHS_ADDR              0x00
 #define EIDE_LBA_ADDR              0x40
 #define EIDE_HOB                   0x80

/* ATA/ATAPI Command Register */
#define EIDE_CMD                    0x07

/* ATA/ATAPI Status Register */
#define EIDE_STATUS                 0x07
#define EIDE_ALT_STATUS             0x02

#define EIDE_STATUS_BSY             0x80    /* Busy                     */
#define EIDE_STATUS_DRDY            0x40    /* Device ready             */
#define EIDE_STATUS_DMA_RDY         0x20    /* DMA ready                */
#define EIDE_STATUS_DWF             0x20    /* Drive Write Fault        */
#define EIDE_STATUS_SERVICE         0x10    /* Overlap                  */
#define EIDE_STATUS_DSC             0x10    /* Drive Seek Complete      */
#define EIDE_STATUS_DRQ             0x08    /* Data Transfer Requested  */
#define EIDE_STATUS_CORR            0x04    /* Data corrected           */
#define EIDE_STATUS_IDX             0x02    /* Index mark               */
#define EIDE_STATUS_ERR             0x01    /* Error                    */

#define ATA_STATUS_READY            0x40
#define ATA_STATUS_IDLE             0x50
#define ATA_STATUS_BUSY             0x80

#define EIDE_STATUS_ERROR       \
        (EIDE_STATUS_BSY | EIDE_STATUS_ERR | EIDE_STATUS_DRQ)

/* ATA/ATAPI Device Control Register */
#define EIDE_CNTRL                  0x02
 #define EIDE_CNTRL_HOB             0x80
 #define EIDE_CNTRL_RSVD            0x08    /* Reserved bit ATA-1 only */
 #define EIDE_CNTRL_SRST            0x04
 #define EIDE_CNTRL_NIEN            0x02

#define EIDE_DRIVE_ADDR             0x03

struct _eide_regs {
    /* TASKFILE regs */
    uintptr_t   data;               /* Data (ATA/ATAPI)         R/W 0 */
    uintptr_t   error;              /* Error (ATA/ATAPI)        R   1 */
    uintptr_t   features;           /* Features (ATA/ATAPI)     W   1 */
    uintptr_t   seccnt;             /* Sector count (ATA)       R/W 2 */
    uintptr_t   cyllo;              /* Cylinder low (ATA)       R/W 3 */
    uintptr_t   cylmi;              /* Cylinder mid (ATA)       R/W 4 */
    uintptr_t   cylhi;              /* Cylinder high (ATA)      R/W 5 */
    uintptr_t   devhd;              /* Device/head */
    uintptr_t   status;             /* Status */
    uintptr_t   cmd;                /* Command */
    uintptr_t   astatus;            /* Alternate status         R   206 */
    uintptr_t   cntl;               /* Device control           W   206 */
    //uintptr_t daddr;              /* Drive address            R   207 */

    /* SATA regs */
    uintptr_t   sstatus;
    uintptr_t   serror;
    uintptr_t   scontrol;
    uintptr_t   sactive;
    //uintptr_t snotification;
};

typedef struct _ata_pass_thru           ATA_PASS_THRU;
typedef struct _eide_regs               EIDE_REGS;
typedef struct _eide_target             EIDE_TARGET;
typedef struct _eide_geometry           EIDE_GEOMETRY;
typedef struct _sim_eide_data           SIM_EIDE_PDATA;
typedef struct _eide_ctrl               EIDE_CTRL;
typedef struct _eide_identify           EIDE_IDENTIFY;
typedef struct _eide_smart_attribute    EIDE_SMART_ATTRIBUTE;
typedef struct _eide_product            EIDE_PRODUCT;
typedef struct _eide_vendor             EIDE_VENDOR;

/*****************************************************************************/
#define IDENT_MAJOR_VERSION(_v)                                     \
                        (((_v) != 0xffff) ? eide_bsr((_v)) : 0)

#define IDENT_IS_FIELD_VALID(_f)                                        \
                        (((_f) & IDENT_FIELD_VALID) == IDENT_FIELD_VALID)

#define IDENT_FIELD_VALID_MSK               0xc000
#define IDENT_FIELD_VALID                   0x4000

#define IDENT_GCONFIG                       0
 #define IDENT_GC_RESPONSE_INCOMPLETE       0x0004
 #define IDENT_GC_PMASK                     0xc000
 #define IDENT_GC_DMASK                     0x0f00  /* peripheral drive types mask */
 #define IDENT_GC_REMOVABLE                 0x0080  /* indicates device has removable media */
 #define IDENT_GC_CMASK                     0x0060  /* command packet DRQ type mask */
 #define IDENT_GC_PKTMASK                   0x0003  /* command packet size mask */
  #define CMD_PKT_SIZE_12                   0x0000  /* 12 bytes (all CD-ROM) */
  #define CMD_PKT_SIZE_16                   0x0001  /* 16 bytes (rsvd for SAM) */
 #define IDENT_GC_CF_PCCARD_MODE            0x848a
 #define IDENT_GC_CF_TRUE_IDE_MODE          0x44a

#define IDENT_SCONFIG                       2
 #define IDENT_SC_SFR_SPINUP_INCMPLT        0x37c8
 #define IDENT_SC_SFR_SPINUP_CMPLT          0x738c
 #define IDENT_SC_SPINUP_INCMPLT            0x8c73
 #define IDENT_SC_SPINUP_CMPLT              0xc837

#define IDENT_CAPABILITY                    49
 #define IDENT_CAP_STANDBY                  0x4000
 #define IDENT_CAP_IORDY_SUP                0x0800
 #define IDENT_CAP_IORDY_DIS                0x0400
 #define IDENT_CAP_LBA_SUP                  0x0200
 #define IDENT_CAP_DMA_SUP                  0x0100
 #define IDENT_CAP_VENDOR                   0x00ff

#define IDENT_FVALID                        53
 #define IDENT_FVALID_88                    0x0004
 #define IDENT_FVALID_64_70                 0x0002
 #define IDENT_FVALID_54_58                 0x0001

#define IDENT_ADDITIONAL_SUP                69
 #define IDENT_AS_DRAT                      0x4000  /* Deterministic read after TRIM */
 #define IDENT_AS_RZAT                      0x0020  /* Read zero after TRIM */

#define IDENT_CMDSET_SUP                    82
 #define IDENT_CMDSET_SERVICE_INTR          0x0100
 #define IDENT_CMDSET_RELEASE_INTR          0x0080
 #define IDENT_CMDSET_WRITE_CACHE           0x0020
 #define IDENT_CMDSET_SMART                 0x0001

#define IDENT_CMDSETS_SUP                   83
 #define IDENT_CMDSETS_FLUSH_EXT_SUP        0x2000
 #define IDENT_CMDSETS_FLUSH_SUP            0x1000
 #define IDENT_CMDSETS_ADDR48_SUP           0x0400
 #define IDENT_CMDSETS_SFR_SPINUP_SUP       0x0040
 #define IDENT_CMDSETS_PWRUP_STANDBY_SUP    0x0020
 #define IDENT_CMDSETS_MEDIA_STATUS_SUP     0x0010
 #define IDENT_CMDSETS_APM_SUP              0x0008
 #define IDENT_CMDSETS_CFA_SUP              0x0004
 #define IDENT_CMDSETS_QUEUED_SUP           0x0002
 #define IDENT_CMDSETS_DNLD_SUP             0x0001

#define IDENT_CMDSET_FEATURES_ENBABLED      85
 #define IDENT_CFE_SERVICE                  0x0100
 #define IDENT_CFE_RELEASE_INTERRUPT        0x0080
 #define IDENT_CFE_LOOK_AHEAD               0x0040
 #define IDENT_CFE_WRITE_CACHE              0x0020
 #define IDENT_CFE_POWER_MANAGEMENT         0x0008
 #define IDENT_CFE_REMOVABLE_MEDIA          0x0004
 #define IDENT_CFE_SECURITY                 0x0002
 #define IDENT_CFE_SMART                    0x0001

#define IDENT_CMDSET_FEATURES_ENABLED2      86
    #define IDENT_CFE_PWRUP_STANDBY         0x0040
    #define IDENT_CFE_SFR_SPINUP            0x0020

#define IDENT_CMDSET_FEATURE_DFLT           87
 #define IDENT_CFD_VALID                    0x4000
 #define IDENT_CFD_WWN                      0x0100
 #define IDENT_CFD_MEDIA_SERIAL_NUM         0x0004
 #define IDENT_CFD_SMART_SELF_TEST          0x0002
 #define IDENT_CFD_SMART_ERR_LOGGING        0x0001

#define IDENT_MAX_CHS_SECTORS               16514064
#define IDENT_MAX_USER_SECTORS              268435456

#define IDENT_RESET_RESULT                  93
 #define IDENT_RESET_RESULT_CBLID           0x2000

#define IDENT_SATA_CAPABILITIES             76
 #define IDENT_SATA_CAP_GEN1                0x0002
 #define IDENT_SATA_CAP_GEN2                0x0004
 #define IDENT_SATA_CAP_GEN3                0x0008
 #define IDENT_SATA_CAP_PM                  0x0200      /* Partial and slumber */
 #define IDENT_SATA_CAP_PHY_EV              0x0400      /* PHY event counters */

#define IDENT_SATA_FEATURES                 78
#define IDENT_SATA_FEATURES_ENB             79
 #define IDENT_SATA_FEATURE_FIS_OFFSETS     0x0002      /* DMA setup FIS non-zero buffer offsets */
 #define IDENT_SATA_FEATURE_FIS_OPT         0x0004      /* DMA setup FIS auto-active optimization */
 #define IDENT_SATA_FEATURE_DEV_PM          0x0008      /* Device initiated PM */
 #define IDENT_SATA_FEATURE_ORDERED         0x0010      /* Guaranteed in-order delivery */

#define IDENT_PHYS_SCTR_SIZE                106
 #define IDENT_PSS_VALID                    0x4000
 #define IDENT_PSS_MLP                      0x2000      /* Multiple logical per physical sector */

#define IDENT_DATA_SET_MGNT                 169
 #define IDENT_DSM_TRIM                     0x0001

struct _eide_identify {
    uint16_t    general_config;         /* General Configuration (0) */
    uint16_t    dflt_logical_cylinders; /* Number of cylinders (1) */
    uint16_t    specific_config;        /* ATA specific/ATAPI unique configuration (2) */
    uint16_t    dflt_logical_heads;     /* Number of heads (3) */
    uint16_t    unformated_bpt;         /* Un-formated bytes per track ATA-1 (4) */
    uint16_t    unformated_bps;         /* Un-fomarted bytes per sector ATA-1 (5) */
    uint16_t    dflt_logical_sec_trk;   /* Sectors per track (6) */
    uint16_t    vendor_word7;           /* Was bytes inter-sector gap */
    uint16_t    vendor_word8;           /* Was bytes in sync */
    uint16_t    vendor_word9;           /* Was words unique status */
    uint8_t     serial_number[20];      /* Serial number (10-19) */
    uint16_t    buffer_type;            /* Controller type ATA-1 (20) */
    uint16_t    buffer_size;            /* Buffer size in 512 byte blocks ATA-1 (21) */
    uint16_t    ecc_bytes;              /* # extra bytes in read/write long (22) */
    uint8_t     firmware_revision[8];   /* Firmware revision (23-26) */
    uint8_t     model_number[40];       /* Model number (27-46) */
    uint16_t    rw_multiple;            /* Read/write multiple support (47) */
    uint16_t    doubleword_support;     /* Double word I/O support ATA-1 (48) */
    uint16_t    capabilities;           /* Capabilities LBA/DMA (49) */
    uint16_t    capabilities2;          /* Capabilites ATA-4+ (security mode ATA-3) (50) */
    uint16_t    opio_mode;              /* PIO mode ATA-1,4 (51) */
    uint16_t    odma_mode;              /* DMA mode ATA-1,3 (52) */
    uint16_t    field_validity;         /* Field validity (53) */
    uint16_t    cur_logical_cylinders;  /* Current logical cylinders (54) */
    uint16_t    cur_logical_heads;      /* Current logical heads  (55) */
    uint16_t    cur_logical_sec_trk;    /* Current sectors per track (56) */
    uint16_t    capacity_low;           /* Current capacity in sectors (57) */
    uint16_t    capacity_hi;            /* Current capacity in sectors (58) */
    uint16_t    cur_rw_multiple;        /* Current multiple sector setting (59) */
    uint16_t    usectors_low;           /* Total number of user addressable sectors (60) */
    uint16_t    usectors_hi;            /* Total number of user addressable sectors (61) */
    uint16_t    single_word_dma;        /* Single DMA config ATA-1,3 (62) */
    uint16_t    multiword_dma;          /* Multi-word DMA transfer (63) */
    uint16_t    pio_modes;              /* PIO transfer modes supported ATA-2+ (64) */
    uint16_t    dma_min_cycle;          /* Min multi-word DMA transfer cycle time ATA-2+ (65) */
    uint16_t    dma_ideal_cycle;        /* Recommended multi-word DMA transfer cycle time ATA-2+ (66) */
    uint16_t    pio_burst_cycle;        /* Min PIO transfer cycle time without flow control ATA-2+ (67) */
    uint16_t    dma_iordy_cycle;        /* Min PIO transfer cycle time with iordy ATA-2+ (68) */
    uint16_t    additional_sup;         /* Additional support (69) */
    uint16_t    rsvd_70;                /* Rsvd (70) */
    uint16_t    interface_type;         /* ATA-3 only (ATA-4+ ns from receipt of PACKET command to bus release) (71) */
    uint16_t    major_version_ata3;     /* ATA-3 only (ATA-4+ ns from receipt of SERVICE command to BSY cleared) (72) */
    uint16_t    minor_version_ata3;     /* ATA-3 only (73) */
    uint16_t    rsvd_74;                /* Reserved (74) */
    uint16_t    queue_depth;            /* Queue depth ATA-4+ (75) */
    uint16_t    sata_capabilities;      /* SATA capabilities (76) */
    uint16_t    rsvd_77;                /* Reserved (77) */
    uint16_t    sata_feature_sup;       /* SATA features supported (78) */
    uint16_t    sata_feature_enb;       /* SATA features enabled (79) */
    uint16_t    major_version;          /* ATA-4+ (80) */
    uint16_t    minor_version;          /* ATA-4+ (81) */
    uint16_t    cmdset_sup;             /* Command set supported ata-4+ (82) */
    uint16_t    cmdsets_sup;            /* Command sets supported ata-4+ (83) */
    uint16_t    cmd_setfeat_sup_ext;    /* Command set/feature suppported ext ata-4+ (84) */
    uint16_t    cmd_feature_enb;        /* Command set/feature enabled (85) */
    uint16_t    cmd_feature_enb2;       /* Command set/feature enabled (86) */
    uint16_t    cmd_setfeat_dflt;       /* Command set/feature default ata-4+ (87) */
    uint16_t    udma_modes;             /* Ultra DMA modes (88) */
    uint16_t    secure_erase_time;      /* Time required for security erase unit completion ATA-5+ (89) */
    uint16_t    secure_enhanced_erase_time; /* Time required for enhanced security erase unit completion  ATA-5+ (90) */
    uint16_t    apm;                    /* Current advanced power management value  ATA-4+ (91) */
    uint16_t    master_passwd_revision; /* Master password revision code  ATA-5+ (92) */
    uint16_t    reset_result;           /* Hardware reset result ATA-5+ (93) */
    uint16_t    acoustic;               /* Acoustic management ATA-6 (94) */
    uint16_t    rsvd_95[5];             /* Reserved (95-99) */
    uint16_t    lba_max0;               /* Max 48-bit LBA ATA-6 (100) */
    uint16_t    lba_max1;               /* Max 48-bit LBA ATA-6 (101) */
    uint16_t    lba_max2;               /* Max 48-bit LBA ATA-6 (102) */
    uint16_t    lba_max3;               /* Max 48-bit LBA ATA-6 (103) */
    uint16_t    rsvd_104[2];            /* Reserved (104-105) */
    uint16_t    phys_sctr_size;         /* 106 */
    uint16_t    rsvd_107;               /* Reserved (107) */
    uint16_t    wwn[4];                 /* (108-111) */
    uint16_t    rsvd_112[5];            /* Reserved (112-116) */
    uint16_t    word_per_logical_sctr0; /* (117) */
    uint16_t    word_per_logical_sctr1; /* (118) */
    uint16_t    rsvd_119[7];            /* Reserved (119-125) */
    uint16_t    atapi_byte_count;       /* ATAPI byte count behavior (126) */
    uint16_t    media_notification;     /* Removable media status notification feature set support ATA-4+ (127) */
    uint16_t    security_status;        /* Security status ATA-4+ (128) */
    uint16_t    vendor_specific[31];    /* (129-159) */
    uint16_t    cfa_power;              /* CFA power mode ATA-6 (160) */
    uint16_t    rsvd_161[7];            /* (161-168) */
    uint16_t    nominal_form_factor;    /* Nominal form factor */
    uint16_t    data_set_mgnt;          /* 169 */
    uint16_t    product_identifier[4];
    uint16_t    rsvd_174[2];            /* (174-175) */
    uint16_t    media_serial_number[30];/* (176-205) */
    uint16_t    sct_cmd_xport;          /* SCT Command Transport (206) */
    uint16_t    rsvd_207[2];            /* (207-208) */
    uint16_t    lba_alignment;          /* Alignment of Logical blocks within physical block (209) */
    uint16_t    rsvd_210[7];            /* (210-216) */
    uint16_t    rotation_rate;          /* 217 */
    uint16_t    rsvd_218[37];           /* (218-254) */
    uint16_t    integrity;              /* ATA-5+ (255) */
};

#define EIDE_MVER_ATA5                      0x20

#define EIDE_IS_SATA(_ident)                                                \
    (!(_ident)->reset_result && ((short)(_ident)->major_version) >= EIDE_MVER_ATA5)

#define SMART_TEMP8(__temp)                                             \
    (((int8_t)(__temp) & 0x80) ? -((~(int8_t)(__temp)) + 1) : (int8_t)(__temp))

#define SMART_TEMP16(__temp)                                                \
    (((int16_t)(__temp) & 0x8000) ? -((~(int16_t)(__temp)) + 1) : (int16_t)(__temp))

/*****************************************************************************/
struct _eide_smart_attribute {
    uint8_t         id;             /* Attibute id number (1 - 0xff) */
    uint8_t         status[2];
    uint8_t         value;
    uint8_t         wvalue;         /* Worst ever normalized */
    uint8_t         rvalue[6];
    uint8_t         rsvd;
};

struct _eide_geometry {
    uint64_t        sectors;
    uint16_t        heads;
    uint16_t        cylinders;
    uint16_t        sec_trk;
    uint16_t        rsvd;
};

struct _eide_product {
    uint32_t        did;            /* Device id */
    uint32_t        class;
    uint32_t        pflags;
    uint32_t        aflags;
    char            *name;          /* Chip set init (map mem, set caps etc...) */
    int             (*init)(SIM_HBA *);
};

struct _eide_vendor {
    uint16_t        vid;
    uint16_t        rsvd;
    EIDE_PRODUCT    *chipsets;
};

typedef struct _eide_command {
    uint8_t         cmd;
    char            *desc;
} EIDE_COMMAND;

typedef struct _eide_workspace {
    char            *buf;
    int             sz;
    ioreq_t         ioreq;
    SG_ELEM         sgl[256];
    mdl_t           mdl[256];
} EIDE_WORKSPACE;

/*****************************************************************************/
/* tflags (target flags) */
#define EIDE_TFLG_ENUMERATING       0x00000001
#define EIDE_TFLG_PRESENT           0x00000002
#define EIDE_TFLG_REMOVABLE         0x00000004
#define EIDE_TFLG_APM_SLEEP         0x00000008
#define EIDE_TFLG_APM_STANDBY_MEDIA 0x00000010
#define EIDE_TFLG_DIS_IORDY         0x00000020
#define EIDE_TFLG_IORDY             0x00000040
#define EIDE_TFLG_USE_LBA           0x00000080
#define EIDE_TFLG_USE_PIO           0x00000100
#define EIDE_TFLG_USE_SDMA          0x00000200
#define EIDE_TFLG_USE_MDMA          0x00000400
#define EIDE_TFLG_USE_UDMA          0x00000800
#define EIDE_TFLG_USE_BMSTR         0x00001000
#define EIDE_TFLG_USE_QUEUED        0x00002000
#define EIDE_TFLG_USE_MBLK          0x00004000
#define EIDE_TFLG_USE_LBA48         0x00008000
#define EIDE_TFLG_USE_BIOS          0x00010000

#define EIDE_TFLG_ATA               0x00020000
#define EIDE_TFLG_ATAPI             0x00040000

#define EIDE_TFLG_CF                0x00080000  /* CompactFlash/PCCard */
#define EIDE_TFLG_APM               0x00100000
#define EIDE_TFLG_SMART             0x00200000
#define EIDE_TFLG_WRITE_CACHE_ON    0x00400000
#define EIDE_TFLG_WRITE_CACHE_OFF   0x00800000
#define EIDE_TFLG_READ_AHEAD_ON     0x01000000
#define EIDE_TFLG_READ_AHEAD_OFF    0x02000000
#define EIDE_TFLG_PWRDOWN           0x04000000  /* Power done at exit */
#define EIDE_TFLG_SFR_SPINUP        0x08000000
#define EIDE_TFLG_MEDIA_NOT_PRESENT 0x10000000

/*****************************************************************************/
struct _eide_target {
    SIM_HBA         *hba;
    uint32_t        tflags;
    uint32_t        verbosity;
    CCB_SCSIIO      *nexus;
    int             (*nfcnp)(EIDE_TARGET *, uint8_t);
    void            *ts;            /* Target specific pointer  */
    uint8_t         port;
    uint8_t         pm_port;        /* PMP */
    uint8_t         vendor;
    uint8_t         model;
    char            mdl[44];
    char            sn[24];
    char            fw[12];
    uint8_t         cmd;            /* Last ATA command */
    uint8_t         ctl;            /* Last ATA control */
    uint8_t         status;
    uint8_t         error;
    uint8_t         device_type;
    uint8_t         rw_multiple;
    uint8_t         queue_depth;
    int8_t          pio_mode;       /* Current pio mode */
    int8_t          mdma_mode;      /* Current mdma mode */
    int8_t          udma_mode;      /* Current udma mode */

    uint32_t        num_prde;
    uint32_t        sataintstat;
    rcar_sata_drv   sata_drv;
    rcar_sata_prd   *prd;
    sem_t           *sata_sem;
    EIDE_REGS       regs;
    EIDE_GEOMETRY   geometry;
    EIDE_GEOMETRY   translation;
    EIDE_IDENTIFY   identify;
    EIDE_WORKSPACE  ws;
};

/*****************************************************************************/
/* eflags (run time flags) */
#define EIDE_EFLG_USE_BMSTR         0x0004  /* Use DMA */

typedef struct _sim_eide_ext {
    SIM_HBA         *hba;
    int             eflags;

    uint8_t         priority;
    uint8_t         rsvd[3];

    uint32_t        s_cap;

    uint32_t        hw_version;     /* Vendor unique HW version */
    uintptr_t       bar;            /* Memory maped base address */
    uint64_t        IOPort_Base;    /* I/O port base on 64 bits */
    EIDE_TARGET     targets[EIDE_MAX_TARGETS];
} SIM_EIDE_EXT;

#define EIDE_RTYPE_READ             0x1
#define EIDE_RTYPE_WRITE            0x2
#define EIDE_RTYPE_DMA              0x4
#define EIDE_RTYPE_MULTI            0x8

/* NOTE: this struct (max 50 bytes) */
struct _sim_eide_data {
    SIMQ_DATA       simq;           /* SIM queue private data (16 bytes) */
    uint32_t        rtype;          /* Request type (PIO/DMA) */
};

#define EIDE_GFLG_ENUMERATION       0x00000001
#define EIDE_GFLG_BMSTR             0x00000004
#define EIDE_GFLG_DBG               0x80000000

struct _eide_ctrl {
    TAILQ_HEAD(,_sim_hba)   hlist;      /* Linked list of HBA's */
    uint32_t                gflags;
    uint8_t                 priority;
    uint8_t                 verbosity;
    uint8_t                 rsvd[2];
    uint32_t                dmode;      /* Debug mode */
    uint32_t                nhba;       /* Number of HBA's */
    uint32_t                pathid_max; /* Max path id */
    int                     pagesize;
};

/*****************************************************************************/
extern EIDE_CTRL eide_ctrl;

/*****************************************************************************/
/* sim_eide.c */
extern int eide_sim_detach(void);
extern int eide_sim_args(char *options);
extern int eide_sim_attach(CAM_ENTRY *centry);
extern int eide_sim_init(SIM_HBA *hba, int path);
extern int eide_sim_action(SIM_HBA *hba, CCB *ccb_ptr);

extern int eide_free(void *addr, size_t size);
extern void *eide_alloc(size_t size);

extern void *eide_dma_mem_alloc(size_t size, size_t *psize);
extern int eide_dma_mem_free(void *addr, size_t sz);

extern int eide_bsr(int cmode);
extern int eide_init(SIM_HBA *hba);
extern int eide_bus_register(SIM_HBA *hba);
extern int eide_sim_idetach(SIM_HBA *hba);
extern SIM_HBA *eide_alloc_hba();
extern int eide_free_hba(SIM_HBA *hba);
extern int eide_dump_regs(EIDE_TARGET *tgt);
extern void eide_start_ccb(SIM_HBA *hba);
extern int eide_reset(EIDE_TARGET *tgt);
extern int eide_init_device(EIDE_TARGET *tgt, int reset);
extern int eide_identify(EIDE_TARGET *tgt);
extern void eide_post_ccb(EIDE_TARGET *tgt, CCB_SCSIIO *ccb);
extern int eide_process_interrupt(SIM_HBA *hba, uint32_t flags);
extern int eide_error(EIDE_TARGET *tgt, int scsi_status, int cam_status);
extern ssize_t eide_slogf(EIDE_TARGET *tgt, int opcode, int severity, int verbosity, const char *fmt, ...);
extern int eide_command(EIDE_TARGET *tgt, uint8_t cmd, uint8_t head, uint32_t cyl,
                        uint16_t sect, uint16_t feat, uint16_t count, uint32_t flags, uint32_t timeo);
extern int eide_create_thread(pthread_t *tid, pthread_attr_t *aattr, void *(*func)(void *), void *arg, int priority, uint32_t *tstate, char *name);
extern int eide_workspace_init(EIDE_TARGET *tgt, int sz);
extern int eide_sense(EIDE_TARGET *tgt, int sflgs, int sense, int asc, int ascq, int info);

/* sim_ata.c */
extern void ata_io(EIDE_TARGET *tgt);
extern int ata_init(EIDE_TARGET *tgt);
extern int ata_dinit(EIDE_TARGET *tgt);
extern int ata_init_device(EIDE_TARGET *tgt);
extern int ata_parse_identify(EIDE_TARGET *tgt, EIDE_IDENTIFY *ident);
extern int ata_command(EIDE_TARGET *tgt, int cmd, int dev, int feat, int sc,
                int lbal, int lbam, int lbah, void *data, int dlen,
                int flgs, int (*cbf)(EIDE_TARGET *, uint8_t), int timeo);
extern uint8_t get_tf_status_reg(EIDE_TARGET *tgt);
extern uint8_t get_tf_error_reg(EIDE_TARGET *tgt);

/* sim_atapi.c */
extern void atapi_io(EIDE_TARGET *tgt);
extern int atapi_init(EIDE_TARGET *tgt);
extern int atapi_dinit(EIDE_TARGET *tgt);
extern int atapi_init_device(EIDE_TARGET *tgt);
extern int atapi_parse_identify(EIDE_TARGET *tgt, EIDE_IDENTIFY *ident);
extern int atapi_power_mode(EIDE_TARGET *tgt, int mode);

/* sim_satl.c */
extern int eide_satl(EIDE_TARGET *tgt, CCB_SCSIIO *ccb);

/* sim_sata.c */
extern int sata_init(SIM_HBA *hba);
extern int sata_dinit(SIM_HBA *hba);
extern int sata_port_reset(EIDE_TARGET *tgt, uint8_t intr);
extern int sata_interrupt(EIDE_TARGET *tgt);
extern int sata_command(EIDE_TARGET *tgt, ATA_TASKFILE *tf,
        uint8_t *acmd, uint32_t alen, SG_ELEM *sge, int sgc, int timeo);
extern void sata_tf_read(EIDE_TARGET *tgt, ATA_TASKFILE *tf);
extern void sata_data_pio(EIDE_TARGET *tgt, uint32_t dir);
extern unsigned int sata_data_pio_rw(EIDE_TARGET *tgt, void *buffer, int count, uint32_t dir);

/* cam.c */
extern int cam_configure(const MODULE_ENTRY *sim_entry,
                            int nsims, int argc, char *argv[]);
extern void scsi_display_ccb(CCB *ccb_ptr, int verbosity);

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/devb/rcarsata/sim_eide.h $ $Rev: 853240 $")
#endif
