/**===========================================================================

  @file
        vdev-msm.c

  @brief
        Traps and emulate APPS Non-Secure watchdog

  @details
        Traps the write attempts to HMSS WDOG Timer registers by the guest OS watchdog and emulate the hardware behaviour.
        When the timer reaches the value programmed in the bark register, a bark interrupt will be sent to the guest system. and the timer is programmed to the bite register value.
        When the timer reaches the value programmed in the bite register, a SIGUSR2 signal will be sent to qvm to create a guest core file.

  @copyright
         Copyright (c) 2012-2017 QUALCOMM Technologies Incorporated.
         All rights reserved. Licensed Material - Restricted Use.
         Qualcomm Confidential and Proprietary.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header$
$DateTime$
$Author$
$Change$

when       who          what, where, why
--------   ---       ------------------------------------------------------
10/10/17   jsamuel     Initial Version
04/27/18   zambur      Merged Watchdog and SMC Interrupt
05/09/18   zambur      Added ability to intercept when guests enter and exit sleep
10/04/18   jsamuel     Send pulse event to perform the dump
==========================================================================*/


#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <sched.h>
#include <inttypes.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>
#include <libgen.h>
#include <qvm/vdev.h>
#include <qvm/outpipe.h>
#include <aarch64/psci.h>
#include <aarch64/context.h>
#include <sys/neutrino.h>
#include <sys/syspage.h>
#include <amss/core/pdbg.h>
#include <amss/npa_client.h>
#include "dplmp_client.h"
#include "devcfg_utils.h"
#include <fcntl.h>
#include <stdbool.h>
#include "vdev_events.h"
#include "vmm_common.h"
#include <hw/inout.h>
#include <sys/iofunc.h>
#include <amss/vdev_utils.h>
#include <amss/core/libbmetrics.h>
#include <pcie_xport.h>
#include "logger_utils.h"

/* Device specific state information */
typedef struct _msm_dev_s {
    char*               vm_name;
    iofunc_notify_t*    notify;
    int32_t             notify_idx;
    struct qvm_timer_data gtimer_data;
    struct guest_power_state_notify *cpu_core0_notify;
    struct guest_power_state_notify *cpu_core1_notify;
    struct guest_power_state_notify *cpu_core2_notify;
    struct guest_power_state_notify *cpu_core3_notify;
    volatile uint32_t   core_bitmask;
    struct guest_timer  *s_gtimer;        // virtual timer used to fire events
    uint64_t            tmr_hz;           // guest timer resolution in Hz
    uint64_t            wdt_hz;           // wdog sleep resolution in Hz
    paddr_t             wdt_base;
    uint64_t            wdt_bark_irq;
    uint64_t            wdt_bite_irq;
    uint64_t            bark_cnt;
    uint64_t            bite_cnt;
    bool                bark_happened;
    bool                bite_happened;
    uint8_t             dog_enable;
    uint64_t            remaining_wd_cnt;
    uint64_t            tm_ticks;
    char                *shmem_vaddr;
    bool                vm_wants_reset;
    bool                vm_wants_shutdown;
} msm_dev_t;

/* vdev-msm command line options */
enum {
    OPTION_MSM_WDT_BASE,
    OPTION_MSM_WDT_HZ,
    OPTION_MSM_WDT_BARK_IRQ,
    OPTION_MSM_WDT_BITE_IRQ,
    OPTION_MSM_SMC_TRAP,
    OPTION_MSM_PCIE_POWER,
    OPTION_MSM_TRIGGER_HOSTRAMDUMP,
    OPTION_MSM_USB,
    OPTION_MSM_LPASS_SMMU,
    OPTION_MSM_MODEM,
    OPTION_MSM_VERBOSE,
};

/* WDOG registers */
#define APCS_HMSS_WDT_ADDR             (0x09830000u)
#define APCS_WDOG_ACR_OFF              (0x0)
#define APCS_WDOG_RESET_OFF            (0x4)
#define APCS_WDOG_CTL_OFF              (0x8)
#define APCS_WDOG_STATUS_OFF           (0xC)
#define APCS_WDOG_BARK_TIME_OFF        (0x10)
#define APCS_WDOG_BITE_TIME_OFF        (0x14)
#define APCS_WDOG_TEST_LOAD_STATUS_OFF (0x18)
#define APCS_WDOG_TEST_LOAD_OFF        (0x1C)
#define APCS_WDOG_TEST_OFF             (0x20)
#define APCS_WDOG_WR_PEND_STS_OFF      (0x24)

#define SHARED_IMEM_BASE    (0x066BF000u)
#define SHARED_IMEM_SIZE    (0x1000u)

/* Address offset within the virtualize shared memory to store the mpm timer value */
#define BS_KERNEL_ENTRY_ADDROFFSET  (0x6B8u)

/* MPM2 SLEEP COUNTER - Physical Address to read the mpm timer value */
#define MPM_BASE_PHYS          (0x4A3000u)
#define MPM_BASE_SIZE          (0x1000u)
#define MPM_SLEEP_CNT_OFFSET   (0x0)

/* vdev-wdog command line option to specify wdt_hz=<clock val in HZ> to set wdog timer ticks.
 * By default it is set to 32765.
 */
#define WDT_HZ                          (32765u)

/* vdev-wdog command line option to specify .wdt_bark_irq=<vector number> to set wdog bark interrupt
 * By default it is set WDT_slp_barkInt spi[28]
 */
#define WDT_BARK_IRQ                    (60u)

/* vdev-wdog command line option to specify .wdt_bite_irq=<vector number> to set wdog bite interrupt
 * By default it is set APCS_sysWdogBiteInt spi[29]
 */
#define WDT_BITE_IRQ                    (61u)

/* max size of the source id interrupt */
#define MAX_CHAR 50

/* Total number of registers that are returned from any SMC call */
#define SMC_RESULT_REGISTERS 4

/* Parent directory location for vm guest */
#define VM_GUEST_PARENT_DIRECTORY "/dev/qvm/"

#define PLL_SR2_NPA_RES_NAME     "/pmic/client/pll_sr2"
#define LPASS_SMMU_GDHS          "/devcfg/smmu_lpass/1"

#define GVM_INFO_CREATE_MODE (S_IRWXU | S_IRGRP | S_IROTH)
#define GVM_INFO_CREATE_FLAGS (O_CREAT | O_RDWR)

enum {
    USB3_IDX,
    USB2_IDX,
    NUM_USB_PORTS
};

static const char* devcfg_usb[] =
{
    [USB3_IDX] = "/devcfg/usb/1",
    [USB2_IDX] = "/devcfg/usb/2"
};

#define USB_DEVCFG_STATE_PWR_ON  7
#define USB_DEVCFG_STATE_PWR_OFF 8

static npa_client_handle wlan_client = NULL;
static npa_query_handle qpcie0 = NULL;
static int pcie_wlan_coid = -1;
static npa_client_handle usb_client[NUM_USB_PORTS];
static npa_client_handle pll_sr2_client = NULL;
static npa_client_handle lpass_smmu_client = NULL;

#ifdef VDEV_MSM_TPM_SUPPORT
/* tpm resource */
#define TPM_DEV_PATH "/dev/tpm"
#endif

#define WDT_BITE_PULSE_CODE _PULSE_CODE_MINAVAIL+1

/* IO mem region index */
enum {
    REGION_WDT_IDX,
    REGION_IMEM_IDX,
};

/* IO mem regions for trap and emulate */
static struct qvm_region trap_regions [] = {
    [ REGION_WDT_IDX ] = {
        .flags          = QRF_VIRT_RD | QRF_VIRT_WR ,
        .guest.location = APCS_HMSS_WDT_ADDR,
        .guest.length   = __PAGESIZE,
        .guest.type     = QST_MEMORY,
        .guest.flags    = QSF_NONE,
    },
    [ REGION_IMEM_IDX ] = {
        .flags          = QRF_PASS_ANY | QRF_NOCACHE ,
        .guest.location = SHARED_IMEM_BASE,
        .guest.length   = SHARED_IMEM_SIZE,
        .guest.type     = QST_MEMORY,
        .guest.flags    = QSF_NONE,
    },    
};

/* wdog timer pulse */
typedef union {
        struct _pulse   pulse;
        /* your other message structures would go
           here too */
} wdog_message_t;

/* Wrapper definitions for severity levels */
enum __LOGGER_SEVERITY {
    MSMLOG_ERROR,          //unexpected recoverable error
    MSMLOG_WARNING,        //expected error
    MSMLOG_INFO,           //information
    MSMLOG_DEBUG,          //debug messages (e.g., normal detail).
};

static int slogger_codes[] =  {
    QCLOG_ERROR,
    QCLOG_WARNING,
    QCLOG_INFO,
    QCLOG_DEBUG1
};

#define QNX_BUG_00170918_UNRESOLVED
#ifdef QNX_BUG_00170918_UNRESOLVED
#define VDEV_LOG(log_type,fmt,...) { \
            logger_log(QCLOG_AMSS_QNP_PLATFORM_VDEV, __LINE__, log_type, \
                        "[pid:%d] " fmt, getpid(), ##__VA_ARGS__); \
        }
#else
#define VDEV_LOG(log_type,fmt,...)
#endif

/* msm log macro */
#define QVM_LOG(severity,fmt,...) \
    do { \
        struct timespec tp; \
        if (severity == MSMLOG_ERROR) { \
            clock_gettime( CLOCK_REALTIME, &tp); \
            (void) qvm_report("%02d:%02d:%02d :: [pid:%d] %s: " fmt, (int)(tp.tv_sec / (60 * 60)), (int)((tp.tv_sec % (60 * 60)) / 60), (int)((tp.tv_sec % (60 * 60) % 60)), getpid(), __func__, ##__VA_ARGS__); \
        } else if ((severity) <= g_verbose) { \
            VDEV_LOG(slogger_codes[severity],fmt, ##__VA_ARGS__); \
        } \
    } while (0)

/* log verbosity for qvm_report */
static uint8_t g_verbose = MSMLOG_ERROR;

/* Command line parameter for trapping smc calls*/
static uint8_t smc_trap = 0;

/* Command line parameter for PCIe */
static uint8_t pcie = 0;

/* Command line parameter for triggering a Host RAM dump*/
static uint8_t trigger_hostramdump = 0;

/* Command line parameter for usb */
static uint8_t usb = 0;

/* Command line parameter for */
static uint8_t lpass_smmu = 0;

/* Command line parameter for */
static uint8_t modem = 0;

/* cpu bitmask for power state */
enum {
    CORE0_MASK = 0x1,
    CORE1_MASK = 0x2,
    CORE2_MASK = 0x4,
    CORE3_MASK = 0x8,
};

enum {
    CORE_OFF,
    CORE_ON,
};

/* pdbg tracking which file to edit */
enum {
    runmask_file,
    priority_file,
};

/* Function declaration */
int wdog_timer_settime(qvm_vdev_t * const vdp , uint64_t wd_ticks);
static void monitor_virtual_core_wakeup(void *data, _Uint64t curr, unsigned base);
static unsigned msm_pdbg_read(char *reply_msg, size_t size, off64_t *offset, void *file_ctx);
static unsigned msm_pdbg_write(char *reply_msg, size_t size, off64_t *offset, void *file_ctx);
static void msm_pdbg_init(msm_dev_t *dev, uint8_t cpu_id);
static int init_msm_device (qvm_vdev_t *vdp);
static int pcie_power(int power_status);
static int usb_power(int usb_idx,int power_status);
static int lpass_smmu_power(int power_status);
static int modem_power(int power_status);

#ifdef VDEV_MSM_TPM_SUPPORT
/**
Send tpm event to store the debug information along with the guest dump
* @param       cmd          pointer to tpm cmd event
* @param       cmd_size     size of the tpm cmd event
**/
void send_tpm_command(const char *cmd, size_t cmd_size)
{
    int fd, ret;

    fd = open(TPM_DEV_PATH, O_WRONLY);
    if (fd < 0) {
       QVM_LOG(MSMLOG_ERROR,"Failed to open tpm device, err=%s", strerror (errno));
    } else {
       ret = (int)write(fd, cmd, cmd_size);
       if (ret < 0) {
           QVM_LOG(MSMLOG_ERROR,"Failed to send event to tpm, err=%s", strerror (errno));
       }
       close(fd);
       
    }
}
#endif

/**
Set the expiration time for a wdog timer to a the one-shot value in number of seconds
wrap to posix timer_settime
* @param       dev      pointer to wdog device specific state information
* @param       wd_ticks   the number of ticks.the timer should go off
* @return          EOK on success, an error otherwise.
**/
int wdog_timer_settime(qvm_vdev_t * const vdp , uint64_t wd_ticks)
{
    int rc = EOK;
    msm_dev_t *dev = vdp->v_device;

    pthread_mutex_lock(&vdp->v_mtx);
    dev->tm_ticks = 0;

    if (dev->tmr_hz > dev->wdt_hz) {
        // QVM timer resolution is greater than the watchdog module's interface clock
        dev->tm_ticks = wd_ticks * (dev->tmr_hz / dev->wdt_hz);
    } else {
        dev->tm_ticks = wd_ticks / (dev->wdt_hz / dev->tmr_hz);
    }
    rc = qvm_gtimer_notify(dev->s_gtimer, NULL, dev->tm_ticks);

    QVM_LOG(MSMLOG_DEBUG, "[%s] Set the expiration :: timer %lu :: wdog %lu ticks", dev->vm_name, dev->tm_ticks, wd_ticks);

    pthread_mutex_unlock(&vdp->v_mtx);
    return rc;
}

/**
 * Handle a pulse delivery for the vdev.
 * @param   vdev     virtual device
 * @param   code     pulse code
 */
static int msm_pulse_handler(qvm_vdev_t * const vdev, int8_t code)
{
    msm_dev_t *dev = vdev->v_device;
    int rc = EOK;
#ifdef VDEV_MSM_TPM_SUPPORT 
    char tpm_guestdumpevent[_POSIX_PATH_MAX] = { 0 };
#endif
    

    if (code == WDT_BITE_PULSE_CODE)
    {
        QVM_LOG(MSMLOG_WARNING,"Received the pulse to generate guest core dump." );
        //Call the qvm function to dump a guest core file. Blocking call
        rc = qvm_write_core_file(vdev->v_gsp);

        if (EOK != rc) {
            QVM_LOG(MSMLOG_ERROR,"Couldn't generate guest core dump file , err=%s", strerror (errno));
            exit(EXIT_FAILURE); // exit qvm. invoke VDEV_CTRL_TERMINATE
        } else {
            QVM_LOG(MSMLOG_WARNING, "QVM function to dump a guest core file returned successfully." );

#ifdef VDEV_MSM_TPM_SUPPORT 
            snprintf(tpm_guestdumpevent, sizeof(tpm_guestdumpevent),"%s_%s",dev->vm_name,"guestdump");
            QVM_LOG(MSMLOG_WARNING, "Sending TPM event to collect QNX host side debug information\n");
            send_tpm_command(tpm_guestdumpevent,sizeof(tpm_guestdumpevent));
#endif
            //Directly call ramdump utility if the option was set to trigger a Host RAM dump
            if(trigger_hostramdump)
            {
                QVM_LOG(MSMLOG_WARNING,"Calling the ramdump utility to trigger a Host RAM dump." );
                rc = system( "sync" );
                rc = system( "ramdump -x 10" );
                if( rc == -1 ) {
                    QVM_LOG(MSMLOG_WARNING,"system(ramdump): shell could not be run");
                }
                else {
                    QVM_LOG(MSMLOG_WARNING,"system(ramdump): result of running command is %d", WEXITSTATUS( rc ));
                }
            }
            exit(EXIT_SUCCESS); // exit qvm. invoke VDEV_CTRL_TERMINATE
        }    
    }

    return EOK;
}

/**
 * A virtual timer has expired.
 * @param   vdp     virtual device
 * @param   data    cookie value passed back to us
 * @param   tdp     timer data
 * @param   gcp     cpu handle
 */
static void msm_timer(qvm_vdev_t * const vdev, void *const data, const struct qvm_timer_data *const tdp, struct guest_cpu *const gcp)
{
    msm_dev_t *dev = vdev->v_device;
    int rc = EOK;
    struct sigevent wdt_bite_pulseevent;
    struct sched_param  params;

    pthread_mutex_lock(&vdev->v_mtx);

    QVM_LOG(MSMLOG_DEBUG, "[%s] We got a pulse from our wdog timer", dev->vm_name);
    if (!dev->bark_happened) {
        QVM_LOG(MSMLOG_DEBUG, "[%s] Bark happened", dev->vm_name);

        /*
         * When the WDOG counter value reaches the value programmed in the bark time register,
         * the WDOG_bark interrupt is asserted.
         * Send the BARK IRQ to the guest vm
         */
        if (NULL != vdev->v_intr.pic) {
            rc = qvm_signal_intr(&vdev->v_intr, QIS_ON);
            if (EOK != rc) {
                QVM_LOG(MSMLOG_ERROR, "[%s] Couldn't assert the interrupt to the guest system, err=%s", dev->vm_name, strerror (errno));
            } else {
                QVM_LOG(MSMLOG_DEBUG, "[%s] Asserted the bark interrupt to the guest system", dev->vm_name);
            }
        }

        if ((dev->bite_cnt > 0) && (dev->bite_cnt > dev->bark_cnt)) {
            QVM_LOG(MSMLOG_DEBUG, "[%s] Reset wdog timer to :: (dev->bite_cnt - dev->bark_cnt) =  %lu", dev->vm_name, (dev->bite_cnt -dev->bark_cnt));
            //Setup the wdog timer with a (bite-bark) counts
            wdog_timer_settime(vdev, (dev->bite_cnt - dev->bark_cnt));
        } else {
            QVM_LOG(MSMLOG_WARNING, "[%s] Trigger the Bite pulse immediately ::  dev->bite_cnt =  %lu", dev->vm_name, dev->bite_cnt);
            //Setup the wdog timer with a 1 seconds to trigger a bite pulse immediately
            wdog_timer_settime(vdev, 1);
        }

        dev->bark_happened = 1;
    } else {
        QVM_LOG(MSMLOG_WARNING, "[%s] Bite happened : Sending off the pulse to generate guest core dump", dev->vm_name);

        //Sending off the pulse to perform the dump and to throw tpm events. 
        rc = pthread_getschedparam(pthread_self(), NULL,&params);
        if(rc != 0) {
            QVM_LOG(MSMLOG_ERROR, "[%s] pthread_getschedparam() failed %s", dev->vm_name, strerror ( errno));
            exit(EXIT_FAILURE); // exit qvm. invoke VDEV_CTRL_TERMINATE
        }
        QVM_LOG(MSMLOG_DEBUG, "[%s] pri %d (cur %d)", dev->vm_name, params.sched_priority, params.sched_curpriority);
        qvm_vdev_pulse (vdev,WDT_BITE_PULSE_CODE,params.sched_curpriority+1,&wdt_bite_pulseevent);
        MsgDeliverEvent(0, &wdt_bite_pulseevent);

        dev->bite_happened =1;
    }
    pthread_mutex_unlock(&vdev->v_mtx);
}


/**
 * Perform a vdev read operation for the device
 * @param       vdev     virtual device
 * @param       vopnd    state block for virtual device reference operand
 * @param       oopnd    state block for the 'other' operand
 * @param       gcp      guest cpu performing the reference
 * @returns     QVRS_* value
 */
static enum qvm_vdev_ref_status msm_vread(qvm_vdev_t *const vdev,
        unsigned cookie , const struct qvm_state_block *const vopnd,
        const struct qvm_state_block *const oopnd,
        struct guest_cpu *const gcp)
{
    int rc = EOK;
    uint32_t rval = ~0u;
    const msm_dev_t *dev = ( msm_dev_t* ) vdev->v_device;
    pthread_mutex_lock(&vdev->v_mtx);

    const uint64_t base =  vopnd->location & (~(__PAGESIZE - 1));
    const uint32_t off  =  vopnd->location & (__PAGESIZE-1);


    QVM_LOG(MSMLOG_DEBUG, "[%s] read on loc=%lx , cookie = %d", dev->vm_name, vopnd->location, cookie ) ;
    QVM_LOG(MSMLOG_DEBUG, "[%s] base =%lx :: off =%08x", dev->vm_name, base, off) ;

    pthread_mutex_unlock(&vdev->v_mtx);

    qvm_cpu_write(gcp, QXF_NONE,  oopnd, 1, &rval, sizeof(rval));
    return QVRS_NORMAL;
}


/**
 * Perform a vdev write operation for the device
 * @param       vdev     virtual device
 * @param       vopnd    state block for virtual device reference operand
 * @param       oopnd    state block for the 'other' operand
 * @param       gcp      guest cpu performing the reference
 * @returns     QVRS_* value
 */
static enum qvm_vdev_ref_status msm_vwrite(qvm_vdev_t *const vdev,
        unsigned cookie, const struct qvm_state_block *const vopnd,
        const struct qvm_state_block *const oopnd,
        struct guest_cpu *const gcp)
{
    int rc = EOK;
    uint64_t wval = 0;
    msm_dev_t *dev = ( msm_dev_t* ) vdev->v_device;

    pthread_mutex_lock(&vdev->v_mtx);

    rc = qvm_cpu_read(gcp, QXF_NONE,  oopnd, 1, &wval, sizeof(wval));
    if (rc != EOK) {
        pthread_mutex_unlock(&vdev->v_mtx);
        return QVRS_ERRNO + rc;
    }
    const uint64_t base =  vopnd->location & (~(__PAGESIZE - 1));
    const uint32_t off  =  vopnd->location & (__PAGESIZE - 1);

    QVM_LOG(MSMLOG_DEBUG, "[%s] write on loc=%lx , cookie = %d value = %lx", dev->vm_name, vopnd->location, cookie,wval ) ;
    QVM_LOG(MSMLOG_DEBUG, "[%s] base =%lx :: off =%08x", dev->vm_name, base, off) ;

    switch (off) {
        //wdog control
        case APCS_WDOG_CTL_OFF:
            if (wval) {
                /*Enable the wdog*/
                QVM_LOG(MSMLOG_DEBUG, "[%s] Enable the wdog", dev->vm_name);

                QVM_LOG(MSMLOG_DEBUG, "[%s] BEFORE ENABLE: remaining wd cnt left :: %lu", dev->vm_name, dev->remaining_wd_cnt);

                if (!dev->bark_cnt) {
                    QVM_LOG(MSMLOG_ERROR, "[%s] dev->bark_cnt should never be 0", dev->vm_name);
                    pthread_mutex_unlock(&vdev->v_mtx);
                    return QVRS_ERRNO + 1;
                }

                /*Start the vdev wdog timer*/
                rc = wdog_timer_settime(vdev, dev->bark_cnt);
                if (rc != EOK) {
                    pthread_mutex_unlock(&vdev->v_mtx);
                    return QVRS_ERRNO + rc;
                }

                dev->dog_enable = 1;
            } else {
                /*Disable the wdog*/
                QVM_LOG(MSMLOG_DEBUG, "[%s] Disable the wdog", dev->vm_name);

                //Disarm the timer by setting the it_value to zero
                qvm_gtimer_notify(dev->s_gtimer, NULL, 0);

                //WDOG_bark interrupt is de-asserted when the watchdog in the guest system reset the WDOG by writing to the WDOG OFF register.
                if (NULL != vdev->v_intr.pic) {
                    rc = qvm_signal_intr(&vdev->v_intr, QIS_OFF);
                    if (EOK != rc) {
                        QVM_LOG(MSMLOG_ERROR, "[%s] Couldn't de-assert the interrupt to the guest system, err=%s", dev->vm_name, strerror ( errno));
                    } else {
                        QVM_LOG(MSMLOG_DEBUG, "[%s] de-assert the bark interrupt to the guest system", dev->vm_name);
                    }
                }

                dev->dog_enable = 0;
            }
            break;

        case APCS_WDOG_BARK_TIME_OFF:
            //WDOG bark time
            //Store the bark time count
            dev->bark_cnt = wval;
            QVM_LOG(MSMLOG_DEBUG, "[%s] Set wdog bark: %lu counts / %lu seconds", dev->vm_name, dev->bark_cnt, (dev->bark_cnt/dev->wdt_hz));

            //Setup the wdog timer with a bark seonds
            //rc = wdog_timer_settime(dev, dev->bark_cnt);
            //if (rc != EOK) {
            //    return QVRS_ERRNO + rc;
            //}
            break;

        case APCS_WDOG_BITE_TIME_OFF:
            //WDOG bite time
            //Store the bite time value (not used anywhere)
            dev->bite_cnt = wval;
            QVM_LOG(MSMLOG_DEBUG, "[%s] Set wdog bite count :: dev->bite_cnt = %lu", dev->vm_name, dev->bite_cnt);
            break;

        case APCS_WDOG_RESET_OFF:
            // WDOG Reset
            QVM_LOG(MSMLOG_DEBUG, "[%s] Reset the wdog", dev->vm_name);

            //WDOG_bark interrupt is de-asserted when the watchdog in the guest system reset the WDOG by writing to the WDOG RESET register.
            if (NULL != vdev->v_intr.pic) {
                rc = qvm_signal_intr(&vdev->v_intr, QIS_OFF);
                if (EOK != rc) {
                    QVM_LOG(MSMLOG_ERROR, "[%s] Couldn't de-assert the interrupt to the guest system, err=%s", dev->vm_name, strerror ( errno));
                } else {
                    QVM_LOG(MSMLOG_DEBUG, "[%s] de-assert the bark interrupt to the guest system", dev->vm_name);
                }
            }

            if (dev->dog_enable) {
                //Setup the wdog timer with a bark counts
                rc = wdog_timer_settime(vdev, dev->bark_cnt);
                if (rc != EOK) {
                    pthread_mutex_unlock(&vdev->v_mtx);
                    return QVRS_ERRNO + rc;
                }
            }

            dev->bark_happened = 0;
            break;

        case APCS_WDOG_STATUS_OFF:
        case APCS_WDOG_ACR_OFF:
        case APCS_WDOG_TEST_LOAD_STATUS_OFF:
        case APCS_WDOG_TEST_LOAD_OFF:
        case APCS_WDOG_TEST_OFF:
        case APCS_WDOG_WR_PEND_STS_OFF:
        default:
            break;
    }

    pthread_mutex_unlock(&vdev->v_mtx);
    return QVRS_NORMAL;
}


/**
 * Monitor when a each virtual core wakes up and mark it as active in
 * the internal bitmask. When a core has woken up from LPM state handle
 * device wake up 
 * @param       data       pointer to vdev virtual device
 * @param       curr       current power state
 * @param       base       base offset for more than 64 cpus
 * @return      None
 */
static void monitor_virtual_core_wakeup(void *data, _Uint64t curr, unsigned base)
{
    int rc = EOK;
    qvm_vdev_t *const vdp = (qvm_vdev_t *)data;
    msm_dev_t *dev = vdp->v_device;
    uint64_t qvm_core_status;

    pthread_mutex_lock(&vdp->v_mtx);

    if (dev->core_bitmask == 0x0) {
        QVM_LOG(MSMLOG_DEBUG,"[%s] Coming out of LPM", dev->vm_name);

        dev->core_bitmask |= (1u << (gettid()-2));

        if (dev->notify)
            pdbg_io_notify_trigger(dev->notify, dev->notify_idx);

        if (dev->dog_enable)
            wdog_timer_settime(vdp, dev->bark_cnt);

        if (pcie) {
            do {
                rc = pcie_power(PCIE_PWR_ACTIVE);
                rc = pcie_power(PCIE_PWR_RESTORE_REG);
                delay(10);
            }while (rc != EOK);
        }

        if (usb)
            usb_power(usb,PWR_ACTIVE);

        if (lpass_smmu)
            lpass_smmu_power(PWR_ACTIVE);
        
        if (modem)
            modem_power(PWR_ACTIVE);
    } else {
        dev->core_bitmask |= (1u << (gettid()-2));
    }


    pthread_mutex_unlock(&vdp->v_mtx);
}


/**
 * Handle parsing SMC call information.
 * @param       vdp             virtual device
 * @param       gcp             guest cpu
 * @param       git             guest instuction type
 * @param       qualifier       instuction call qualifier
 * @return      QVRS_* value
 */
static enum qvm_vdev_ref_status psci_interrupt_callback(qvm_vdev_t *vdp, struct guest_cpu *gcp, enum qvm_guest_instr_type git, unsigned qualifier)
{
    int rc = EOK;
    msm_dev_t *dev = vdp->v_device;
    struct qvm_state_block smc_reg_blk;
    uint64_t smc_type, reg_value;
    npa_resource_state usb_on_vec[] = { USB_DEVCFG_STATE_PWR_ON, 1 };

    /* Reads the first smc return register to get the type of smc call */
    smc_reg_blk.type = QST_CPUREGS;
    smc_reg_blk.location = offsetof(AARCH64_CPU_REGISTERS, gpr[0]);
    smc_reg_blk.flags = QSF_NONE;
    smc_reg_blk.length = sizeof(uint64_t);
    rc = qvm_cpu_read(gcp, QXF_NONE, &smc_reg_blk, 1, &smc_type, sizeof(smc_type));

    switch (smc_type) {
        case PSCI_CPU_OFF:
        case PSCI_CPU_SUSPEND:
            if (smc_trap) {
                pthread_mutex_lock(&vdp->v_mtx);

                dev->core_bitmask &= ~(1u << (gettid()-2));

                if (dev->core_bitmask == 0x0) {
                    QVM_LOG(MSMLOG_DEBUG,"[%s] Going into LPM", dev->vm_name);

                    if (dev->notify)
                        pdbg_io_notify_trigger(dev->notify, dev->notify_idx);

                    // Disarm the timer by setting the it_value to zero
                    if (dev->dog_enable)
                        qvm_gtimer_notify(dev->s_gtimer, NULL, 0);

                    if (pcie) {
                        do {
                            rc = pcie_power(PCIE_PWR_BACKUP_REG);
                            rc = pcie_power(PCIE_PWR_OFF);
                            delay(10);
                        }while (rc != EOK);
                    }

                    if (usb)
                        usb_power(usb,PWR_OFF);

                    if (lpass_smmu)
                        lpass_smmu_power(PWR_OFF);
                    
                    if (modem)
                        modem_power(PWR_OFF);
                }

                pthread_mutex_unlock(&vdp->v_mtx);
            }
            break;

        case PSCI_CPU_ON:
            smc_reg_blk.type = QST_CPUREGS;
            smc_reg_blk.location = offsetof(AARCH64_CPU_REGISTERS, gpr[1]);
            smc_reg_blk.flags = QSF_NONE;
            smc_reg_blk.length = sizeof(uint64_t);
            rc = qvm_cpu_read(gcp, QXF_NONE, &smc_reg_blk, 1, &reg_value, sizeof(reg_value));

            msm_pdbg_init(dev, reg_value);
            break;

        case PSCI_SYSTEM_OFF:
            dev->vm_wants_shutdown = true;
            break;

        case PSCI_SYSTEM_RESET:
            dev->vm_wants_reset = true;
            break;

        default:
            break;
    }

    return QVRS_CONTINUE;
}

/**
Send power message to pcie-server 
* @param       power_event         power operation
**/
static int pcie_power(int power_status)
{
    union pcie_msg msg;
    int ret = EOK;

    if (pcie_wlan_coid == -1) {
        QVM_LOG(MSMLOG_INFO, "name_open(%s)", PCIE0_PWR_ATTACH_POINT);
        pcie_wlan_coid = name_open(PCIE0_PWR_ATTACH_POINT, 0);
        if (pcie_wlan_coid == -1) {
            QVM_LOG(MSMLOG_ERROR,"failed to connect %s", PCIE0_PWR_ATTACH_POINT);
            return -1;
        }
    }
    memset ( &msg , 0x0, sizeof (msg));
    msg.type = power_status;
    QVM_LOG(MSMLOG_INFO, "sending power_status(%d) to pci-server", msg.type);
    ret = MsgSend(pcie_wlan_coid, &msg, sizeof(msg), NULL, 0);

    if (ret != EOK) {
        QVM_LOG(MSMLOG_ERROR, "changing pcie power status(%d) failed: ret=%d", power_status, ret);
    }
    
    return ret;
}


static int usb_power(int usb_idx,int power_status) 
{
    int rc = EOK;
    npa_resource_state usb_on_vec[] = { USB_DEVCFG_STATE_PWR_ON, 1 };
    npa_resource_state usb_off_vec[] = { USB_DEVCFG_STATE_PWR_OFF, 1 };

    if (!usb_client[usb_idx-1]) {
        usb_client[usb_idx-1] = npa_create_sync_client(devcfg_usb[usb_idx-1], "vdev-msm", NPA_CLIENT_VECTOR);
        if (!usb_client[usb_idx-1]) {
            QVM_LOG(MSMLOG_ERROR, "%s fail to create usb_client npa node for /devcfg/usb/%d", __func__,usb);
            return -1;
        }
    }

    if (power_status == PWR_ACTIVE) {
        npa_issue_vector_request(usb_client[usb_idx-1], 2, usb_on_vec);
    } else {
        npa_issue_vector_request(usb_client[usb_idx-1], 2, usb_off_vec);
    }

    return rc;
}

static int lpass_smmu_power(int power_status) 
{
    int rc = EOK;
    if (!lpass_smmu_client) {
        lpass_smmu_client = npa_create_sync_client(LPASS_SMMU_GDHS, "vdev-msm", NPA_CLIENT_REQUIRED);
        if (!lpass_smmu_client) {
            QVM_LOG(MSMLOG_ERROR, "%s fail to create lpass_smmu_client npa node", __func__);
            return -1;
        }
    } 

    npa_issue_required_request(lpass_smmu_client, power_status);
    return rc;
}

static int modem_power(int power_status) 
{
    int rc = EOK;
    if (!pll_sr2_client) {
        pll_sr2_client = npa_create_sync_client(PLL_SR2_NPA_RES_NAME, "vdev-msm", NPA_CLIENT_REQUIRED);
        if (!pll_sr2_client) {
            QVM_LOG(MSMLOG_ERROR, "%s fail to create pll_sr2_client npa node", __func__);
            return -1;
        }
    } 

    npa_issue_required_request(pll_sr2_client, power_status);
    return rc;
}


/**
 * Callback for notifying when the system power status of the specified device changes states
 * @param          file_ctx       addition file context
 * @param          notifyCtx      notify resource manager
 * @param          notifyIdx      index of notify
 * @returns        unsigned       index of notify
 */
static unsigned msm_pdbg_system_notify(void *file_ctx, void *notifyCtx, int32_t notifyIdx)
{
    msm_dev_t* dev = (msm_dev_t*)file_ctx;

    dev->notify = (iofunc_notify_t*)notifyCtx;
    dev->notify_idx = notifyIdx;

    return notifyIdx;
}


/**
 * Callback for reading the system power status of the specified device
 * @param          recv_msg       data writen to the file
 * @param          size           size of msg
 * @param          offset         offset of msg
 * @param          file_ctx       addition file context
 * @returns        unsigned       size of writen message
 */
static unsigned msm_pdbg_system_read(char *reply_msg, size_t size, off64_t *offset, void *file_ctx)
{
    unsigned nbytes = 0;
    msm_dev_t* dev = (msm_dev_t*)file_ctx;

    if (*offset > 0) {
        return 0;
    }

    nbytes = snprintf(reply_msg, size, "%d\n", (dev->core_bitmask & CORE0_MASK));

    (*offset)++;

    return min(size, nbytes);
}


/**
 * Callback for reading the runmask or priority of the specified device cpu
 * @param          recv_msg       data writen to the file
 * @param          size           size of msg
 * @param          offset         offset of msg
 * @param          file_ctx       addition file context
 * @returns        unsigned       size of writen message
 */
static unsigned msm_pdbg_cpu_read(char *reply_msg, size_t size, off64_t *offset, void *file_ctx)
{
    unsigned nbytes = 0;
    uint8_t vcpu_id, file_id;
    int *rsizep, rsize, size_tot;
    unsigned *rmaskp;
    struct sched_param sp = { 0 };

    if (*offset > 0) {
        return 0;
    }

    vcpu_id = (uint8_t)(((unsigned long)file_ctx)>>16);
    file_id = (uint8_t)(((unsigned long)file_ctx)&0xFF);

    if (file_id) {
      SchedGet(0, (vcpu_id + 2), &sp);
      nbytes = snprintf(reply_msg, size, "%d\n", sp.sched_priority);
    } else {
        //rsize = RMSK_SIZE(num_cpus);
        rsize = 1;

        size_tot = sizeof(*rsizep) + (sizeof(*rmaskp) * rsize);

        if (NULL == (rsizep = malloc(size_tot))) {
            QVM_LOG(MSMLOG_ERROR, "Failed to allocate memory for CPU Runmask");
            return 0;
        }

        memset(rsizep, 0x00, size_tot);

        *rsizep = rsize;
        rmaskp = (unsigned *)(rsizep + 1);

        ThreadCtlExt(0, (vcpu_id + 2), _NTO_TCTL_RUNMASK_GET_AND_SET_INHERIT, rsizep);
        nbytes = snprintf(reply_msg, size, "%d\n", *rmaskp);
    }

    (*offset)++;

    return min(size, nbytes);
}


/**
 * Callback for changing the runmask or priority of the specified device cpu
 * @param          recv_msg       data writen to the file
 * @param          size           size of msg
 * @param          offset         offset of msg
 * @param          file_ctx       addition file context
 * @returns        unsigned       size of writen message
 */
static unsigned msm_pdbg_cpu_write(char *recv_msg, size_t size, off64_t *offset, void *file_ctx)
{
    uint64_t data;
    uint8_t vcpu_id, file_id;
    struct sched_param sp = { 0 };

    if (*offset != 0) {
        return 0;
    }

    vcpu_id = (uint8_t)(((unsigned long)file_ctx)>>16);
    file_id = (uint8_t)(((unsigned long)file_ctx)&0xFF);
    data = (uint64_t)strtoul(recv_msg, NULL, 0);

    if (file_id) {
        sp.sched_priority = data;
        SchedSet(0, (vcpu_id + 2), SCHED_NOCHANGE, &sp);
    } else {
        ThreadCtlExt(0, (vcpu_id + 2), _NTO_TCTL_RUNMASK, (void *)data);
    }

    return strlen(recv_msg);
}


/**
 * Create a pdbg status node for each system to:
 *     1) Change runmask and priority of each cpu
 *     2) Read the system power status
 * @param          dev       msm device
 * @returns        None
 */
static void msm_pdbg_init(msm_dev_t *dev, uint8_t cpu_id)
{
    void* file_ctx;

    //Runmask PDBG
    struct pdbg_ops_s ops_runmask = { 0 };
    char pdbg_runmask_dir[64];
    memset(&ops_runmask, 0x0, sizeof(struct pdbg_ops_s));
    ops_runmask.read = msm_pdbg_cpu_read;
    ops_runmask.write = msm_pdbg_cpu_write;

    file_ctx = (void *)(unsigned long)(cpu_id<<16 | runmask_file);
    snprintf(pdbg_runmask_dir, sizeof(pdbg_runmask_dir), "cpu_%d/runmask", cpu_id);
    pdbg_create_file(pdbg_runmask_dir, 0666, ops_runmask, file_ctx);

    //Priority PDBG
    struct  pdbg_ops_s ops_priority = { 0 };
    char pdbg_priority_dir[64];
    memset(&ops_priority, 0x0, sizeof(struct pdbg_ops_s));
    ops_priority.read = msm_pdbg_cpu_read;
    ops_priority.write = msm_pdbg_cpu_write;

    file_ctx = (void *)(unsigned long)(cpu_id<<16 | priority_file);
    snprintf(pdbg_priority_dir, sizeof(pdbg_priority_dir), "cpu_%d/priority", cpu_id);
    pdbg_create_file(pdbg_priority_dir, 0666, ops_priority, file_ctx);

    //System Power Status PDBG
    struct  pdbg_ops_s ops_power_status = { 0 };
    memset(&ops_power_status, 0x0, sizeof(struct pdbg_ops_s));
    ops_power_status.read = msm_pdbg_system_read;
    ops_power_status.notify = msm_pdbg_system_notify;

    pdbg_create_file("power_status", 0444, ops_power_status, (void*)dev);
}


/**
 * Perform a wdog device specific state initialization
 * @param          vdp       virtual device
 * @returns        errno     error variable
 */
static int init_msm_device (qvm_vdev_t *vdp)
{
    int rc = EOK;
    struct sigevent event;
    msm_dev_t *dev = vdp->v_device;
    off64_t shmem_phys;

    if (NULL == (dev->vm_name = get_system_name(vdp))) {
        QVM_LOG(MSMLOG_ERROR,"Couldn't parse system name");
        return -1;
    }

    //Create qvm gtimer
    if (NULL == (dev->s_gtimer = qvm_gtimer_create(NULL, vdp, NULL, &dev->tmr_hz))) {
        QVM_LOG(MSMLOG_ERROR,"Couldn't create qvm_gtimer");
        return -1;
    }

    //Check if the base address was passed through vdev options. If not, restore to default APCS_*.
    if (!dev->wdt_base) {
        dev->wdt_base = APCS_HMSS_WDT_ADDR;
    }

    //Set the wdt base address
    trap_regions[REGION_WDT_IDX].guest.location = dev->wdt_base;

    //Check if the clock val in HZ was passed through vdev options. If not, restore to default.
    if (!dev->wdt_hz) {
        dev->wdt_hz = WDT_HZ;
    }

    //Check if the bark IRQ number was passed through vdev options. If not, restore to default.
    if (!dev->wdt_bark_irq) {
        dev->wdt_bark_irq = WDT_BARK_IRQ;
    }

    //Check if the bite IRQ number was passed through vdev options. If not, restore to default.
    if (!dev->wdt_bite_irq) {
        dev->wdt_bite_irq = WDT_BITE_IRQ;
    }

    //Initialize a power state notify struct for each virtual core
    dev->cpu_core0_notify = qvm_power_state_notify_create(vdp->v_gsp);
    dev->cpu_core1_notify = qvm_power_state_notify_create(vdp->v_gsp);
    dev->cpu_core2_notify = qvm_power_state_notify_create(vdp->v_gsp);
    dev->cpu_core3_notify = qvm_power_state_notify_create(vdp->v_gsp);

    //Enter the monitor_core_status callback when the specified core goes from OFF state to RUNNING state
    qvm_power_state_notify_register(dev->cpu_core0_notify, (QPSC_TO|QPSC_OFF|QPSC_WAIT), 0, CORE0_MASK, 0, *monitor_virtual_core_wakeup, vdp);
    qvm_power_state_notify_register(dev->cpu_core1_notify, (QPSC_TO|QPSC_OFF|QPSC_WAIT), 0, CORE1_MASK, 0, *monitor_virtual_core_wakeup, vdp);
    qvm_power_state_notify_register(dev->cpu_core2_notify, (QPSC_TO|QPSC_OFF|QPSC_WAIT), 0, CORE2_MASK, 0, *monitor_virtual_core_wakeup, vdp);
    qvm_power_state_notify_register(dev->cpu_core3_notify, (QPSC_TO|QPSC_OFF|QPSC_WAIT), 0, CORE3_MASK, 0, *monitor_virtual_core_wakeup, vdp);

    dev->core_bitmask = 0x0;
    dev->bark_cnt = 0;
    dev->bite_cnt = 0;
    dev->bark_happened = 0;
    dev->bite_happened = 0;
    dev->dog_enable = 0;
    dev->remaining_wd_cnt = 0;

    dev->shmem_vaddr = MAP_FAILED;
    //Create a page-level memory allocator for Shared IMEM
    dev->shmem_vaddr = mmap64( (void *)NULL, SHARED_IMEM_SIZE, 
                            PROT_READ | PROT_WRITE | PROT_NOCACHE,
                            MAP_ANON | MAP_PHYS | MAP_SHARED, NOFD, 0);
    
    if( dev->shmem_vaddr == MAP_FAILED )
    {        
        QVM_LOG(MSMLOG_ERROR,"dev->shmem_vaddr : mmap failed to map with error %s",strerror( errno ) );
        return -1;
    }

    if(mem_offset64( (void*)dev->shmem_vaddr, NOFD, sizeof(uint32_t),&shmem_phys,NULL))
    {
        QVM_LOG(MSMLOG_ERROR,"mem_offset64 failed for VA 0x%"PRIxPTR"",(uintptr_t)(dev->shmem_vaddr));
        return -1;
    }

    trap_regions[REGION_IMEM_IDX].host_location = shmem_phys;
    trap_regions[REGION_IMEM_IDX].cookie = (int)REGION_IMEM_IDX;
    QVM_LOG(MSMLOG_DEBUG,"Guest Shared IMEM page :: dev->shmem_vaddr 0x%"PRIxPTR" :: shmem_phys 0x%"PRIxPTR" ",
                (uintptr_t)(dev->shmem_vaddr),(uintptr_t)shmem_phys);

    return rc;
}

int populate_gvm_info_for_vmm(const char *name, int state)
{
    int fd, ret;
    char buf[128];
    pid_t pid;

    (void)strlcpy(buf, "/tmp/", 128);

    ret = strlcat(buf, name, 128);
    if (ret >= 128) {
        QVM_LOG(MSMLOG_ERROR, "gvm info file path trucated\n");
        return -1;
    }
    fd = open(buf, GVM_INFO_CREATE_FLAGS, GVM_INFO_CREATE_MODE);
    if (fd == -1) {
        QVM_LOG(MSMLOG_ERROR, "Failed to open/create file %s: %s\n",
            buf, strerror(errno));
        return -1;
    }
    pid = getpid();
    snprintf(buf, sizeof(buf), "%d %d\n", pid, state);
    ret = write(fd, buf, strlen(buf));
    if (ret == -1) {
        QVM_LOG(MSMLOG_ERROR, "Failed to write gvm_info: %s", strerror(errno));
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}


/**
 * Handle process-container operations.
 * @param       vdp          virtual device
 * @param       ctrl         operation
 * @param       arg          parameters for the operation
 * @return      EOK on success, an error otherwise.
 */
static int msm_control(qvm_vdev_t *const vdp, unsigned const ctrl, const char *const arg)
{
    int i;
    int rc = EOK;
    msm_dev_t *dev = vdp->v_device;
    char src_intr[MAX_CHAR];
    char *ret_intr;
    int parent;
    int node;
    char buf[50]={0};
    int vm_event;
    int fd;
    int ret;
    uintptr_t mpmbase_vaddr = MAP_DEVICE_FAILED;
    char bmet_str[MAX_CHAR];

    QVM_LOG(MSMLOG_DEBUG,"%s, %u, for %s, factory=%p ", __func__, ctrl, vdp->v_factory->name, vdp->v_factory);

    switch (ctrl) {
        case VDEV_CTRL_OPTIONS_START:
            /*initialize vdev state*/
            vdp->v_block.type = QST_DEFAULT_TYPE_IO;
            break;

        case VDEV_CTRL_OPTIONS_END:
            if (EOK != init_msm_device(vdp)) {
                QVM_LOG(MSMLOG_ERROR,"Couldn't initialize init_msm_device");
                break;
            }

            ret = populate_gvm_info_for_vmm(dev->vm_name, 0);
            if (ret != 0)
                QVM_LOG(MSMLOG_ERROR, "Failed to populate gvm info."
                    " NON FATAL error ... Continuiuing");

            //Register for a callback function for all SMC calls
            qvm_register_for_instr(vdp, QGIT_ARM_SMC, 0, *psci_interrupt_callback);

            //Register a power state handler for vdev-msm
            qvm_register_for_power(vdp);

            //construct a qvm_intr instance which ties to bark IRQ
            if (NULL == vdp->v_intr.pic) {
                snprintf(src_intr, sizeof(src_intr),"gic:%lu",dev->wdt_bark_irq);
                ret_intr = parse_intr(src_intr,&vdp->v_intr);
                if (ret_intr) {
                    QVM_LOG(MSMLOG_DEBUG,"print the vdp->v_intr.pic == 0x%" PRIXPTR " :: vdp->v_intr.param = %u" , (uintptr_t)vdp->v_intr.pic,vdp->v_intr.param);
                    QVM_LOG(MSMLOG_DEBUG,"print the vdp->v_intr.src_id = %u :: src_intr = %s" , vdp->v_intr.src_id,src_intr);
                } else {
                    QVM_LOG(MSMLOG_ERROR,"coudn't construct qvm_intr instance" );
                }
            }

            // Initialize the trap mem region
            //setup 'cookie' to array index for quick lookup in vread/vwrite
            trap_regions[REGION_WDT_IDX].vdev = vdp;
            //setup 'cookie' to array index for quick lookup in vread/vwrite
            trap_regions[REGION_WDT_IDX].cookie = (int)REGION_WDT_IDX;

            qvm_region_set(vdp->v_gsp, 2, trap_regions);

            char dirBuffer[128] = VM_GUEST_PARENT_DIRECTORY;
            strlcat(dirBuffer, dev->vm_name, sizeof(dirBuffer));
            pdbg_init(&dirBuffer[0]);
            waitfor(&dirBuffer[0], 1000, 100);
            msm_pdbg_init(dev, 0);
            break;

        case (VDEV_CTRL_FIRST_OPTIDX + OPTION_MSM_WDT_BASE):
            dev->wdt_base = strtoul(arg, NULL, 16);
            QVM_LOG(MSMLOG_DEBUG,"dev->wdt_base= 0x%" PRIXPTR,dev->wdt_base);
            break;

        case (VDEV_CTRL_FIRST_OPTIDX + OPTION_MSM_WDT_HZ):
            dev->wdt_hz = strtoul(arg, NULL, 0);
            QVM_LOG(MSMLOG_DEBUG,"dev->wdt_hz= %lu",dev->wdt_hz);
            break;

        case (VDEV_CTRL_FIRST_OPTIDX + OPTION_MSM_WDT_BARK_IRQ):
            dev->wdt_bark_irq = strtoul(arg, NULL, 0);
            QVM_LOG(MSMLOG_DEBUG,"dev->wdt_bark_irq= %lu",dev->wdt_bark_irq);
            break;

        case (VDEV_CTRL_FIRST_OPTIDX + OPTION_MSM_WDT_BITE_IRQ):
            dev->wdt_bite_irq = strtoul(arg, NULL, 0);
            QVM_LOG(MSMLOG_DEBUG,"dev->wdt_bite_irq= %lu",dev->wdt_bite_irq);
            break;

        case (VDEV_CTRL_FIRST_OPTIDX + OPTION_MSM_SMC_TRAP):
            smc_trap = strtoul(arg, NULL, 0);
            QVM_LOG(MSMLOG_DEBUG, "smc_trap = %u", smc_trap);
            break;

        case (VDEV_CTRL_FIRST_OPTIDX + OPTION_MSM_PCIE_POWER):
            pcie = strtoul(arg, NULL, 0);
            QVM_LOG(MSMLOG_DEBUG, "pcie = %u", pcie);

            if (pcie) {
                pcie_power(PCIE_PWR_ACTIVE);
            }
            break;

        case (VDEV_CTRL_FIRST_OPTIDX + OPTION_MSM_TRIGGER_HOSTRAMDUMP):
            trigger_hostramdump = strtoul(arg, NULL, 0);
            QVM_LOG(MSMLOG_DEBUG, "trigger_hostramdump = %u", trigger_hostramdump);
            break;

        case (VDEV_CTRL_FIRST_OPTIDX + OPTION_MSM_USB):
            usb = strtoul(arg, NULL, 0);
            QVM_LOG(MSMLOG_DEBUG, "usb = %u", usb);

            if (usb) {
                usb_power(usb,PWR_ACTIVE);
            }
            break;

        case (VDEV_CTRL_FIRST_OPTIDX + OPTION_MSM_LPASS_SMMU):
            lpass_smmu = strtoul(arg, NULL, 0);
            QVM_LOG(MSMLOG_DEBUG, "lpass_smmu = %u",lpass_smmu);

            if (lpass_smmu) {
                lpass_smmu_power(PWR_ACTIVE);
            }
            break;

        case (VDEV_CTRL_FIRST_OPTIDX + OPTION_MSM_MODEM):
            modem = strtoul(arg, NULL, 0);
            QVM_LOG(MSMLOG_DEBUG, "modem = %u", modem);

            if (modem) {
                modem_power(PWR_ACTIVE);
            }
            break;

        case (VDEV_CTRL_FIRST_OPTIDX + OPTION_MSM_VERBOSE):
            g_verbose = strtoul(arg, NULL, 0);

            if (MSMLOG_DEBUG < g_verbose) {
                g_verbose = MSMLOG_DEBUG;
            }
            QVM_LOG(MSMLOG_DEBUG,"g_verbose = %u",g_verbose);
            break;

        case VDEV_CTRL_GEN_FDT:
            parent = (int)(uintptr_t)arg;
            node = gfdt_update_node(parent, "msm", vdp->v_block.location);
            if (node > 0) {
                QVM_LOG(MSMLOG_DEBUG,"msm guest device tree present");
                //FIXME: add more to compatible list?
                gfdt_update_prop_strings(node, "compatible", "qcom", "msm-watchdog", NULL);
                //FIXME: command line option to set bark-time, pet-time, ipi-ping and wakeup-enable
                //QVM_LOG(MSMLOG_DEBUG,"vdp->v_block.location == 0x%" PRIXPTR " :: vdp->v_block.length = %u", vdp->v_block.location, vdp->v_block.length);
                //gfdt_update_reg(parent, node, vdp->v_block.location, vdp->v_block.length, QSL_NO_LOCATION);
                //gfdt_update_prop_cells(node, "bark-time", &dev->wdt_bark_time, sizeof(dev->wdt_bark_time));
                //gfdt_update_interrupts(node, &vdp->v_intr, NULL);
            } else {
                QVM_LOG(MSMLOG_ERROR,"msm guest device tree not present");
            }
            break;

       case VDEV_CTRL_TERMINATE:
            pthread_mutex_lock(&vdp->v_mtx);

            //Disarm the timer by setting the it_value to zero
            qvm_gtimer_notify(dev->s_gtimer, NULL, 0);

            if (dev->bite_happened == 1 ) {
                vm_event = VM_EVENT_WDOG_BITE;
                QVM_LOG(MSMLOG_ERROR, "[%s] VDEV_CTRL_TERMINATE wdog bite", dev->vm_name);
                dev->bite_happened = 0;
            } else if ( dev->vm_wants_shutdown ) {
                vm_event = VM_EVENT_TERMINATE_SELF;
                QVM_LOG(MSMLOG_ERROR, "[%s] VDEV_CTRL_TERMINATE shutdown (requested)", dev->vm_name);
                dev->vm_wants_shutdown = false;
            } else if ( dev->vm_wants_reset ) {
                vm_event = VM_EVENT_RESTART_SELF;
                QVM_LOG(MSMLOG_ERROR, "[%s] VDEV_CTRL_TERMINATE reboot (requested)", dev->vm_name);
                dev->vm_wants_reset = false;
            } else {
                vm_event = VM_EVENT_HANDLED_CONTAINER_CRASH;
                QVM_LOG(MSMLOG_ERROR, "[%s] VDEV_CTRL_TERMINATE reboot (unexpected)", dev->vm_name);
            }

            ret = populate_gvm_info_for_vmm(dev->vm_name, vm_event);
            if (ret != 0)
                QVM_LOG(MSMLOG_ERROR, "[%s] Failed to populate gvm info."
                    " NON FATAL error ... Continuiuing", dev->vm_name);

            fd = open(vmm_devname, O_WRONLY);
            if (fd == -1) {
                QVM_LOG(MSMLOG_ERROR, "[%s] Failed to open %s with error: %s. Ignoring this error \
                    and continuing.....", dev->vm_name, vmm_devname, strerror(errno));
            } else {
                snprintf(buf, sizeof(buf), "event %d", vm_event);
                ret = write(fd, buf, strlen(buf));
                if (ret == -1)
                    QVM_LOG(MSMLOG_ERROR, "[%s] Failed to write event to vmm", dev->vm_name);

                close(fd);
            }
            pthread_mutex_unlock(&vdp->v_mtx);
            break;

        /* function being invoked to say guest system is being started */
        case VDEV_CTRL_BOOTUP:

            pthread_mutex_lock(&vdp->v_mtx);
            if( (MAP_DEVICE_FAILED == mpmbase_vaddr) && ( dev->shmem_vaddr != MAP_FAILED) )
            {
                mpmbase_vaddr =  mmap_device_io(MPM_BASE_SIZE,MPM_BASE_PHYS);
                if(MAP_DEVICE_FAILED==mpmbase_vaddr)
                {
                    QVM_LOG(MSMLOG_ERROR, "Couldn't map MPM base : %s",strerror(errno));
                    pthread_mutex_unlock(&vdp->v_mtx);
                    break;
                }

                /* read and store mpm timer value to shared memory */
                out32 ( (uintptr_t)(dev->shmem_vaddr+BS_KERNEL_ENTRY_ADDROFFSET) , in32 (mpmbase_vaddr + MPM_SLEEP_CNT_OFFSET) ) ; 

                /* Record the guest system start in bmetrics*/
                snprintf(bmet_str, sizeof(bmet_str),"%s %s",dev->vm_name,"Guest system ready");
                bmetrics_log_subcomponent_done(bmet_str);

                QVM_LOG(MSMLOG_DEBUG,"shmem_phys :: 0x%lx :: dev->shmem_vaddr= 0x%lx",trap_regions[REGION_IMEM_IDX].host_location, (uintptr_t)dev->shmem_vaddr);
                QVM_LOG(MSMLOG_DEBUG,"mpm timer value stored in shared memory =%x",*((uint32_t*)(dev->shmem_vaddr+BS_KERNEL_ENTRY_ADDROFFSET))) ;
                munmap_device_io(mpmbase_vaddr,MPM_BASE_SIZE);
            }
            pthread_mutex_unlock(&vdp->v_mtx);

            break;
        case VDEV_CTRL_RESET:
        case VDEV_CTRL_STATUS:
        case VDEV_CTRL_GEN_ACPI:
        case VDEV_CTRL_POWER:
        default:
            break;
    }

    return rc ;
}


/**
 * Register the virtual device into the framework.
 */
static void __attribute__((constructor)) msm_register(void)
{
    static const char * const msm_options[] = { "wdt_base", "wdt_hz", "wdt_bark_irq", "wdt_bite_irq", "smc_trap", "pcie", "trigger_hostramdump", "usb","lpass_smmu","modem", "verbose", NULL };

    static struct qvm_vdev_factory msm_factory = {
        .next = NULL, // patched
        .control = msm_control,
        .vread = msm_vread,
        .vwrite = msm_vwrite,
        .timer = msm_timer,
        .pulse = msm_pulse_handler,
        .option_list = msm_options,
        .name = NULL, // patched
        .factory_flags = QVFF_NONE,
        .acc_sizes = 1u << sizeof(uint32_t),
        .extra_space = sizeof(msm_dev_t),
    };
    qvm_register_vdev_factory(&msm_factory, QVM_VDEV_ABI);
}
