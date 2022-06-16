/**===========================================================================

  @file
        vdev-spmi_virt.c

  @brief
        Traps and emulate SPMI read/write commands

  @details
        Traps and emulate SPMI registers

  @copyright
         Copyright (c) 2012-2018 QUALCOMM Technologies Incorporated.
         All rights reserved. Licensed Material - Restricted Use.
         Qualcomm Confidential and Proprietary.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/plt_vm_vdev.qxa_qa/1.0/vdev-spmi-virt/vdev-spmi_virt.c#9 $
$DateTime: 2019/02/14 12:08:19 $
$Author: zambur $
$Change: 18330804 $

when       who          what, where, why
--------   ---       ------------------------------------------------------
1/22/18   bmatic     Initial Version
==========================================================================*/


#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <qvm/vdev.h>
#include <inttypes.h>
#include <pthread.h>
#include <hw/inout.h>
#include <amss/fdt_utils.h>
#include <amss/vdev_utils.h>
#include <amss/adc_client.h>
#include <amss/core/spmi_client.h>

#define SPMI_TRANSACTION_STATUS_NONE    0x0
#define SPMI_TRANSACTION_STATUS_DONE    0x1
#define SPMI_TRANSACTION_STATUS_FAILURE 0x2
#define SPMI_TRANSACTION_STATUS_DENIED  0x4
#define SPMI_TRANSACTION_STATUS_DROPPED 0x8

#define SPMI_DEV_NODE "/dev/spmi"

#define SPMI_TRANSACTION_MAX_SIZE 8
#define SPMI_VDEV_MAX_PERIPHERALS   256U
#define SPMI_VDEV_GIC   358
#define SPMI_VDEV_CHANNELS_BASE  0x4400000
#define SPMI_VDEV_CORE_BASE              0x400F000
#define SPMI_VDEV_PIC_BASE       0x5800000

/*Maximum size of one peripheral*/
#define SPMI_PERIPH_MAX_SIZE    0x100

#define VSPMI_CFG_REG_PMIC_ARB_REG_CHNLn 0x800

#define ADC_DEV_NODE                "/dev/adc"
#define ADC_MIN_ADDR_RANGE          0x3100
#define ADC_MAX_ADDR_RANGE          0x3200

#define VADC_REVISION2              0x3101
#define VADC_PERPH_TYPE             0x3104
#define VADC_STATUS1                0x3108
#define VADC_MODE_CTL               0x3140
#define VADC_EN_CTL1                0x3146
#define VADC_ADC_CH_SEL_CTL         0x3148
#define VADC_ADC_DIG_PARAM          0x3150
#define VADC_HW_SETTLE_DELAY        0x3151
#define VADC_CONV_REQ               0x3152
#define VADC_FAST_AVG_CTL           0x315A
#define VADC_FAST_AVG_EN            0x315B
#define VADC_DATA0                  0x3160
#define VADC_DATA1                  0x3161
#define VADC_SEC_ACCESS             0x31D0
#define VADC_PERPH_RESET_CTL3       0x31DA
#define VADC_MEAS_INTERVAL_OP_CTL   0x3159

typedef struct _spmi_virt_pic_reg_s {
    uint32_t            acc_enable;
    uint32_t            irq_status;
    uint32_t            irq_clear;
} spmi_virt_pic_reg_s;

typedef struct _spmi_virt_perph_data_s {
    void*                   vdp;
    uint32_t                id;
    spmi_virt_pic_reg_s     pic_reg;
    pthread_mutex_t         mutex;
    int                     msg_ch;
    pthread_t               ch_irq_thread;
} spmi_virt_perph_data_s;

typedef struct _spmi_virt_data_s {
    uint32_t          command;
    uint32_t          status;
    uint32_t          wr_data[2];
    uint32_t          rd_data[2];
} spmi_virt_data_t;

typedef struct _adc_virt_data_s {
    uint8_t             vadc_revision2;
    uint8_t             vadc_perph_type;
    uint8_t             vadc_status1;
    uint8_t             vadc_mode_ctl;
    uint8_t             vadc_en_ctl1;
    uint8_t             vadc_adc_ch_sel_ctl;
    uint8_t             vadc_adc_dig_param;
    uint8_t             vadc_hw_settle_delay;
    uint8_t             vadc_conv_req;
    uint8_t             vadc_fast_avg_ctl;
    uint8_t             vadc_fast_avg_en;
    uint8_t             vadc_data0;
    uint8_t             vadc_data1;
} adc_virt_data_t;

/*Device specific state information*/
typedef struct _spmi_virt_dev_s {
    uint32_t            spmi_virt_base;
    uint32_t            spmi_virt_pic_base;
    uint32_t            spmi_virt_core_base;
    uint64_t            spmi_virt_gic;
    int                 spmi_gic_status;
    int                 spmi_fd;
    spmi_virt_data_t    spmi_data;
    uint8_t             nperipherals;
    const uint32_t      *peripherals;
    spmi_virt_perph_data_s      perph_data[SPMI_VDEV_MAX_PERIPHERALS];
    adc_virt_data_t             adc_data;
    int                         adc_fd;
    AdcInputPropertiesType      adc_input_props;
    AdcResultType               adc_result;
    AdcRequestParametersExType  adc_request_param_ex;
} spmi_virt_dev_t;

#define VSPMI_REG_CMD                   0x00
#define VSPMI_REG_STATUS                0x04
#define VSPMI_REG_DATA0                 0x08
#define VSPMI_REG_DATA1                 0x10

#define VSPMI_PIC_REG_ACC_ENABLE        0x00
#define VSPMI_PIC_REG_IRQ_STATUS        0x04
#define VSPMI_PIC_REG_IRQ_CLEAR         0x08

#define SMPI_CMD_BIT_MASK_OPC 27
#define SMPI_CMD_BIT_MASK_SID 20
#define SMPI_CMD_BIT_MASK_ADDR 4
#define SMPI_CMD_BIT_MASK_SIZE 0

#define SPMI_CMD_OPC_EXT_WRITEL 0
#define SPMI_CMD_OPC_EXT_READL  1

#define SPMI_PIC_PERIPH_WIDTH 4096U
/*
   Initialize the mutex to synchronize access to spmi_virt_dev_t variables
   between vdev read requests.
   */
static pthread_mutex_t   vdev_mutex = PTHREAD_MUTEX_INITIALIZER;

/*IO mem region index*/
enum {
    REGION_SPMI_VIRT_IDX,
    REGION_SPMI_PIC_VIRT_IDX,
    REGION_SPMI_CORE_VIRT_IDX,
};

/*vdev-spmi-virt command line options*/
enum {
    OPTION_SPMI_VIRT_BASE,
    OPTION_SPMI_VIRT_PIC_BASE,
    OPTION_SPMI_VIRT_CORE_BASE,
    OPTION_SPMI_VIRT_GIC,
    OPTION_SPMI_VIRT_VERBOSE,
};

/*IO mem regions for trap and emulate*/
static struct qvm_region trap_regions [] = {
    [ REGION_SPMI_VIRT_IDX ] = {
        .flags          = QRF_VIRT_RD | QRF_VIRT_WR ,
        .guest.location = 0,
        .guest.length   = __PAGESIZE,
        .guest.type     = QST_MEMORY,
        .guest.flags    = QSF_NONE,
    },
    [ REGION_SPMI_PIC_VIRT_IDX ] = {
        .flags          = QRF_VIRT_RD | QRF_VIRT_WR ,
        .guest.location = SPMI_VDEV_PIC_BASE,
        .guest.length   = (SPMI_PIC_PERIPH_WIDTH * SPMI_VDEV_MAX_PERIPHERALS),
        .guest.type     = QST_MEMORY,
        .guest.flags    = QSF_NONE,
    },
    [ REGION_SPMI_CORE_VIRT_IDX ] = {
        .flags          = QRF_VIRT_RD | QRF_VIRT_WR ,
        .guest.location = SPMI_VDEV_CORE_BASE,
        .guest.length   = (VSPMI_CFG_REG_PMIC_ARB_REG_CHNLn + (4 * SPMI_VDEV_MAX_PERIPHERALS)),
        .guest.type     = QST_MEMORY,
        .guest.flags    = QSF_NONE,
    },
};

/*spmi_virt log macro*/
#define QVM_LOG(severity,fmt,...)\
    do { \
        if ((severity) <= g_verbose) { \
            (void) qvm_report("%s:%d " fmt, __func__, __LINE__, ##__VA_ARGS__);\
        } \
    } while (0)

/* Macro to flip byte order */
#define FLIP_UINT32_BYTE_ORDER(data)\
    do {\
        int i, sz=sizeof(uint32_t);\
        uint8_t temp;\
        union {\
            uint32_t num;\
            uint8_t arr[4];\
        } bytes;\
        bytes.num = data;\
        for(i=0;i<sz/2;i++) {\
            temp = bytes.arr[i];\
            bytes.arr[i] = bytes.arr[sz-i-1];\
            bytes.arr[sz-i-1] = temp;\
        }\
        data = bytes.num;\
    } while(0);

/* Wrapper definitions for severity levels*/
enum _LOGGER_SEVERITY{
    SPMIVIRTLOG_ERROR,  //unexpected recoverable error
    SPMIVIRTLOG_WARNING,  //expected error
    SPMIVIRTLOG_INFO,  //information
    SPMIVIRTLOG_DEBUG,  //debug messages (e.g., normal detail).
};

/*log verbosity for qvm_report*/
static uint8_t g_verbose = SPMIVIRTLOG_ERROR;

/**
 * Perform a interrupt setup and wait for each channel
 * thread exists
 * @param       args    reference to peripheral data
 * @returns     none
 */
static void* spmi_virt_int_wait ( void * args )
{
    int rc = 0;
    uint32_t peripheral_id;
    uint32_t irq_mask;
    int acc_enabled = 0;
    spmi_virt_perph_data_s* perph_data = (spmi_virt_perph_data_s*)args;
    qvm_vdev_t* vdp = (qvm_vdev_t*)perph_data->vdp;
    spmi_virt_dev_t* dev = vdp->v_device;
    pthread_setname_np ( pthread_self () , __func__ );
    if (perph_data->msg_ch == -1)
    {
        if ((rc = spmi_channel_create(dev->spmi_fd, perph_data->id, 0xff, &perph_data->msg_ch)) != 0)
        {
            QVM_LOG(SPMIVIRTLOG_ERROR,"failed  spmi_channel_create \n" );
            goto pthread_exit_label;
        }
    }

    for (;;) {
        // start waiting for interrupt
        QVM_LOG(SPMIVIRTLOG_DEBUG,"waiting for interrupt on perph id %d \n", perph_data->id);
        rc = spmi_interrupt_wait(dev->spmi_fd, perph_data->msg_ch, &peripheral_id, &irq_mask);
        if (rc != 0)
        {
            QVM_LOG(SPMIVIRTLOG_ERROR,"failed spmi_interrupt_wait \n");
            goto pthread_exit_label;
        }
        if (peripheral_id != perph_data->id)
        {
            QVM_LOG(SPMIVIRTLOG_ERROR,"wrong spmi_interrupt_wait for perph id %d instead %d\n", peripheral_id, perph_data->id);
            continue;
        }
        QVM_LOG(SPMIVIRTLOG_DEBUG,"Interrupt detected for perph id %d irq mask %x\n", perph_data->id, irq_mask);
        // interrupt is received for the right peripheral
        // lock base mutex
        pthread_mutex_lock(&vdev_mutex);

        // check if the GIC signal is fired up
        if (!dev->spmi_gic_status)
        {
            // if not fire up GIC signal
            QVM_LOG(SPMIVIRTLOG_DEBUG,"spmi_gic_status is 0 - fire GIC \n");
            rc = qvm_signal_intr(&vdp->v_intr, QIS_ON);
            if (EOK != rc)
            {
                QVM_LOG(SPMIVIRTLOG_ERROR,"Couldn't assert the interrupt to the guest system, err=%s \n", strerror ( errno));
            }
            dev->spmi_gic_status = 1;
        }
        // unlock base mutex
        pthread_mutex_unlock(&vdev_mutex);

        // lock peripheral mutex
        pthread_mutex_lock(&perph_data->mutex);

        // update peripheral irq status
        perph_data->pic_reg.irq_status = perph_data->pic_reg.irq_status | irq_mask;

        // read if the acc en is set
        acc_enabled = perph_data->pic_reg.acc_enable;

        // unlock peripheral mutex
        pthread_mutex_unlock(&perph_data->mutex);

        // if acc enable go back and wait for another interrupt
        if (acc_enabled)
        {
            continue;
        }
    }
    QVM_LOG(SPMIVIRTLOG_DEBUG,"acc_enabled set 0 - cleanup and exit thread \n");

    if(perph_data->msg_ch != -1)
    {
        pthread_mutex_lock(&perph_data->mutex);
        // clear listening channel
        rc = spmi_channel_clear (dev->spmi_fd, perph_data->id, 0xff, perph_data->msg_ch);
        QVM_LOG(SPMIVIRTLOG_DEBUG,"spmi_channel_clear() requested response %d \n", rc);

        // reset peripheral channel id
        perph_data->msg_ch = -1;
        QVM_LOG(SPMIVIRTLOG_DEBUG,"msg_ch set -1  \n");
        pthread_mutex_unlock(&perph_data->mutex);
    }

pthread_exit_label:
    // exit thread
    pthread_exit ( NULL ) ;
}


/**
 * Perform a vdev read operation for the device
 * @param       vdev    virtual device
 * @param       vopnd   state block for virtual device reference operand
 * @param       oopnd   state block for the 'other' operand
 * @param       gcp     guest cpu performing the reference
 * @returns QVRS_* value
 */
static enum qvm_vdev_ref_status spmi_virt_vread(qvm_vdev_t *const vdev,
        unsigned cookie , const struct qvm_state_block *const vopnd,
        const struct qvm_state_block *const oopnd,
        struct guest_cpu *const gcp)
{

    spmi_virt_dev_t *dev = ( spmi_virt_dev_t* ) vdev->v_device;
    pthread_mutex_lock(&vdev_mutex);

    const uint64_t base =  vopnd->location & (~(__PAGESIZE - 1));
    const uint32_t off  =  vopnd->location & (__PAGESIZE-1);

    QVM_LOG(SPMIVIRTLOG_DEBUG,"read on loc=%lx , cookie = %d \n", vopnd->location, cookie ) ;
    QVM_LOG(SPMIVIRTLOG_DEBUG,"base =%lx :: off =%08x \n", base, off) ;
    void* pData = NULL;
    int nLen = 0;

    if(base == dev->spmi_virt_base)
    {
        if (off == VSPMI_REG_STATUS) {
            // check the command register
            pData = (void*)&(dev->spmi_data.status);
            nLen = sizeof(dev->spmi_data.status);
        } else if (off == VSPMI_REG_DATA0) {
            pData = (void*)&(dev->spmi_data.rd_data[0]);
            nLen = sizeof(dev->spmi_data.rd_data[0]);
        } else if (off == VSPMI_REG_DATA1) {
            pData = (void*)&(dev->spmi_data.rd_data[1]);
            nLen = sizeof(dev->spmi_data.rd_data[1]);
        }
    }
    else if(base == dev->spmi_virt_core_base)
    {
        int periph_idx = (off - VSPMI_CFG_REG_PMIC_ARB_REG_CHNLn) / 4;
        uint32_t cfg_reg_offset = off;
        QVM_LOG(SPMIVIRTLOG_DEBUG,"periph_idx %x  reg offset %x\n", periph_idx, off);
        if(periph_idx < SPMI_VDEV_MAX_PERIPHERALS) {
            // read back acc_enable
            QVM_LOG(SPMIVIRTLOG_DEBUG,"Trapped PMIC_ARB_REG_CHNLn peripheral %d \n", periph_idx);
            pData = (void*)&(dev->perph_data[periph_idx].id);
            nLen = sizeof(dev->perph_data[periph_idx].id);
            QVM_LOG(SPMIVIRTLOG_DEBUG,"reading back PPID %d\n", dev->perph_data[periph_idx].id);
        }
    }
    else if(base >= dev->spmi_virt_pic_base)
    {
        int periph_idx = (base - dev->spmi_virt_pic_base) / SPMI_PIC_PERIPH_WIDTH;
        uint32_t pic_reg_offset = off;
        QVM_LOG(SPMIVIRTLOG_DEBUG,"periph_idx %x  reg offset %x\n", periph_idx, pic_reg_offset);
        if(pic_reg_offset == VSPMI_PIC_REG_ACC_ENABLE) {
            // read back acc_enable
            QVM_LOG(SPMIVIRTLOG_DEBUG,"Trapped VSPMI_PIC_REG_ACC_ENABLE peripheral %d \n", periph_idx);
            pData = (void*)&(dev->perph_data[periph_idx].pic_reg.acc_enable);
            nLen = sizeof(dev->perph_data[periph_idx].pic_reg.acc_enable);
            QVM_LOG(SPMIVIRTLOG_DEBUG,"reading back from acc_enable register value %d\n", dev->perph_data[periph_idx].pic_reg.acc_enable);
        } else if (pic_reg_offset == VSPMI_PIC_REG_IRQ_STATUS) {
            // read back irq status
            QVM_LOG(SPMIVIRTLOG_DEBUG,"Trapped VSPMI_PIC_REG_IRQ_STATUS peripheral %d \n", periph_idx);
            pData = (void*)&(dev->perph_data[periph_idx].pic_reg.irq_status);
            nLen = sizeof(dev->perph_data[periph_idx].pic_reg.irq_status);
            QVM_LOG(SPMIVIRTLOG_DEBUG,"reading back from irq_status register value %d\n", dev->perph_data[periph_idx].pic_reg.irq_status);
        }
    }

    if (pData && nLen) {
        qvm_cpu_write(gcp, QXF_NONE,  oopnd, 1, pData, nLen);
    }
    pthread_mutex_unlock(&vdev_mutex);
    return QVRS_NORMAL;
}

static int spmi_periph_check_access ( const spmi_virt_dev_t *dev , uint16_t addr, uint8_t sid ) 
{
    const uint32_t theone = (sid << 16 ) | ( addr & ~( SPMI_PERIPH_MAX_SIZE - 1 )) ;

    for ( int i = 0 ; i < dev->nperipherals ; i++ ) {
        if ( dev->peripherals[i] == theone ) {
            return EOK;
        }
    }

    return EPERM;
}

/**
 * Perform a vdev read operation for the device
 * @param       vdev    virtual device
 * @param       vopnd   state block for virtual device reference operand
 * @param       oopnd   state block for the 'other' operand
 * @param       gcp     guest cpu performing the reference
 * @returns QVRS_* value
 */
static enum qvm_vdev_ref_status spmi_virt_vwrite(qvm_vdev_t *const vdev,
        unsigned cookie , const struct qvm_state_block *const vopnd,
        const struct qvm_state_block *const oopnd,
        struct guest_cpu *const gcp)
{
    spmi_virt_dev_t *dev = ( spmi_virt_dev_t* ) vdev->v_device;
    int rc = EOK;
    void* pData = NULL;
    int nLen = 0;

    pthread_mutex_lock(&vdev_mutex);

    const uint64_t base =  vopnd->location & (~(__PAGESIZE - 1));
    const uint32_t off  =  vopnd->location & (__PAGESIZE-1);

    QVM_LOG(SPMIVIRTLOG_DEBUG,"read on loc=%lx , cookie = %d \n", vopnd->location, cookie ) ;
    QVM_LOG(SPMIVIRTLOG_DEBUG,"base =%lx :: off =%08x \n", base, off) ;

    if(base == dev->spmi_virt_base)
    {
        if (off == VSPMI_REG_CMD)
        {
            // check the command register
            rc = qvm_cpu_read(gcp, QXF_NONE,  oopnd, 1, &(dev->spmi_data.command), sizeof(dev->spmi_data.command));
            if (rc != EOK) {
                pthread_mutex_unlock(&vdev_mutex);
                return QVRS_ERRNO + rc;
            }

            uint8_t opc = ((uint32_t)(dev->spmi_data.command)>> SMPI_CMD_BIT_MASK_OPC) & 0x0f;
            uint8_t sid = ((uint32_t)(dev->spmi_data.command)>> SMPI_CMD_BIT_MASK_SID) & 0x7f;
            uint16_t addr = ((uint32_t)(dev->spmi_data.command)>> SMPI_CMD_BIT_MASK_ADDR) & 0xffff;
            uint8_t len = ((uint32_t)(dev->spmi_data.command)>> SMPI_CMD_BIT_MASK_SIZE) & 0xf;

            rc = spmi_periph_check_access ( dev, addr , sid ) ;
            if ( rc != EOK ) {
                pthread_mutex_unlock(&vdev_mutex);
                QVM_LOG(SPMIVIRTLOG_ERROR,"spmi access to addr=0x%x, sid=%d not allowed, terminating guest", 
                        addr, sid);
                return QVRS_TERMINATE;
            }

            int spmi_transaction = 0;
            dev->spmi_data.status = SPMI_TRANSACTION_STATUS_NONE;

            QVM_LOG(SPMIVIRTLOG_DEBUG,"spmi_command 0x%08x / opc 0x%02x / sid 0x%02x / addr 0x%04x / len 0x%02x\n", dev->spmi_data.command, opc, sid, addr, len);
            if (len <= SPMI_TRANSACTION_MAX_SIZE) {
                if (opc == SPMI_CMD_OPC_EXT_WRITEL){
                    if (addr >= ADC_MIN_ADDR_RANGE && addr < ADC_MAX_ADDR_RANGE) {
                        switch (addr) {
                            case VADC_MODE_CTL:
                                dev->adc_data.vadc_mode_ctl = dev->spmi_data.wr_data[0];
                                dev->spmi_data.status = SPMI_TRANSACTION_STATUS_DONE;
                                break;

                            case VADC_EN_CTL1:
                                if (dev->adc_fd == -1) {
                                    if ((dev->adc_fd = spmi_open(ADC_DEV_NODE)) == -1) {
                                        QVM_LOG(SPMIVIRTLOG_WARNING, "Can't open device %s for RDWR, errno %d", ADC_DEV_NODE, errno);
                                    }
                                }
                                dev->adc_data.vadc_en_ctl1 = dev->spmi_data.wr_data[0];
                                dev->spmi_data.status = SPMI_TRANSACTION_STATUS_DONE;
                                break;

                            case VADC_ADC_CH_SEL_CTL:
                                dev->adc_data.vadc_adc_ch_sel_ctl = dev->spmi_data.wr_data[0];
                                dev->spmi_data.status = SPMI_TRANSACTION_STATUS_DONE;
                                break;

                            case VADC_ADC_DIG_PARAM:
                                dev->adc_data.vadc_adc_dig_param = dev->spmi_data.wr_data[0];
                                dev->spmi_data.status = SPMI_TRANSACTION_STATUS_DONE;
                                break;

                            case VADC_HW_SETTLE_DELAY:
                                dev->adc_data.vadc_hw_settle_delay = dev->spmi_data.wr_data[0];
                                dev->spmi_data.status = SPMI_TRANSACTION_STATUS_DONE;
                                break;

                            case VADC_CONV_REQ:
                                dev->adc_data.vadc_conv_req = dev->spmi_data.wr_data[0];
                                
                                dev->adc_input_props.nChannelIdx = 0;
                                dev->adc_input_props.nDeviceIdx = 0;
                                rc = GetAdcInputPropByHwChannel(dev->adc_fd, dev->adc_data.vadc_adc_ch_sel_ctl, &dev->adc_input_props);
                                
                                QVM_LOG(SPMIVIRTLOG_DEBUG, "GetInput Props By HW Channel (rc:%d)- channel:%u dev:%u\n", rc, dev->adc_input_props.nChannelIdx, dev->adc_input_props.nDeviceIdx);

                                dev->adc_request_param_ex.nDeviceIdx = dev->adc_input_props.nDeviceIdx;
                                dev->adc_request_param_ex.nChannelIdx = dev->adc_input_props.nChannelIdx;
                                dev->adc_request_param_ex.RegChSel = dev->adc_data.vadc_adc_ch_sel_ctl;
                                dev->adc_request_param_ex.nModeControl = dev->adc_data.vadc_mode_ctl;
                                dev->adc_request_param_ex.RegDigParam = dev->adc_data.vadc_adc_dig_param;
                                dev->adc_request_param_ex.RegSettleDelay = dev->adc_data.vadc_hw_settle_delay;
                                dev->adc_request_param_ex.RegFastAvgCtl = dev->adc_data.vadc_fast_avg_ctl;

                                memset(&dev->adc_result, 0, sizeof(dev->adc_result));
                                rc = RequestConversionEx(dev->adc_fd, &dev->adc_request_param_ex, &dev->adc_result);
                                QVM_LOG(SPMIVIRTLOG_DEBUG, "Request Conversion (rc:%d)- eStatus:%d nToken:%u nDeviceIdx:%u nChannelIdx:%u \n\t nPhysical:%u millivolts \n\t nPercent:%u \n\t nMicrovolts:%u \n\t nCode:%x \n\n",
                                    rc, dev->adc_result.eStatus, dev->adc_result.nToken, dev->adc_result.nDeviceIdx, dev->adc_result.nChannelIdx, dev->adc_result.nPhysical, dev->adc_result.nPercent,
                                    dev->adc_result.nMicrovolts, dev->adc_result.nCode);

                                dev->adc_data.vadc_data1 = dev->adc_result.nCode >> 8;
                                dev->adc_data.vadc_data0 = (dev->adc_result.nCode & 0xFF);

                                dev->spmi_data.status = SPMI_TRANSACTION_STATUS_DONE;
                                break;

                            case VADC_FAST_AVG_CTL:
                                dev->adc_data.vadc_fast_avg_ctl = dev->spmi_data.wr_data[0];
                                dev->spmi_data.status = SPMI_TRANSACTION_STATUS_DONE;
                                break;

                            case VADC_FAST_AVG_EN:
                                dev->adc_data.vadc_fast_avg_en = dev->spmi_data.wr_data[0];
                                dev->spmi_data.status = SPMI_TRANSACTION_STATUS_DONE;
                                break;

                            default: 
                                dev->spmi_data.status = SPMI_TRANSACTION_STATUS_DONE;
                                break;
                        }
                    } else {
                        spmi_transaction = spmi_write(dev->spmi_fd, sid, SPMI_ACCESS_PRIORITY_HIGH, addr, &(dev->spmi_data.wr_data[0]), len);
                        dev->spmi_data.status = SPMI_TRANSACTION_STATUS_DONE | ((spmi_transaction == len)? 0 : SPMI_TRANSACTION_STATUS_FAILURE);
                        QVM_LOG(SPMIVIRTLOG_DEBUG,"spmi_write transaction result: 0x%02x / arbiter status 0x%02x \n", spmi_transaction, dev->spmi_data.status);   
                    }
                } else if (opc == SPMI_CMD_OPC_EXT_READL) {
                    switch (addr) {
                        case VADC_REVISION2:
                            dev->spmi_data.rd_data[0] = dev->adc_data.vadc_revision2;
                            dev->spmi_data.status = SPMI_TRANSACTION_STATUS_DONE;
                            break;

                        case VADC_PERPH_TYPE:
                            dev->spmi_data.rd_data[0] = dev->adc_data.vadc_perph_type;
                            dev->spmi_data.status = SPMI_TRANSACTION_STATUS_DONE;
                            break;

                        case VADC_EN_CTL1:
                            dev->spmi_data.rd_data[0] = dev->adc_data.vadc_en_ctl1;
                            dev->spmi_data.status = SPMI_TRANSACTION_STATUS_DONE;
                            break;

                        case VADC_STATUS1:
                            dev->spmi_data.rd_data[0] = dev->adc_data.vadc_status1;
                            dev->spmi_data.status = SPMI_TRANSACTION_STATUS_DONE;
                            break;

                        case VADC_DATA0:
                            dev->spmi_data.rd_data[0] = dev->adc_data.vadc_data0;
                            dev->spmi_data.status = SPMI_TRANSACTION_STATUS_DONE;
                            break;

                        case VADC_DATA1:
                            dev->spmi_data.rd_data[0] = dev->adc_data.vadc_data1;
                            dev->spmi_data.status = SPMI_TRANSACTION_STATUS_DONE;
                            break;

                        default:
                            spmi_transaction = spmi_read (dev->spmi_fd, sid, SPMI_ACCESS_PRIORITY_HIGH, addr, &(dev->spmi_data.rd_data[0]), len);
                            dev->spmi_data.status = SPMI_TRANSACTION_STATUS_DONE | ((spmi_transaction == len)? 0 : SPMI_TRANSACTION_STATUS_FAILURE);
                            QVM_LOG(SPMIVIRTLOG_DEBUG,"spmi_read / transaction result: 0x%02x / arbiter status 0x%02x \n", spmi_transaction, dev->spmi_data.status);
                            break;
                    }
                }
            }
        } else if (off == VSPMI_REG_DATA0) {
            qvm_cpu_read(gcp, QXF_NONE,  oopnd, 1, (void*)&(dev->spmi_data.wr_data[0]), sizeof(dev->spmi_data.wr_data[0]));
        } else if (off == VSPMI_REG_DATA1) {
            qvm_cpu_read(gcp, QXF_NONE,  oopnd, 1, (void*)&(dev->spmi_data.wr_data[1]), sizeof(dev->spmi_data.wr_data[1]));
        }
    }
    else if(base == dev->spmi_virt_core_base)
    {
        int periph_idx = (off - VSPMI_CFG_REG_PMIC_ARB_REG_CHNLn) / 4;
        uint32_t cfg_reg_offset = off;
        QVM_LOG(SPMIVIRTLOG_DEBUG,"periph_idx %x  reg offset %x\n", periph_idx, off);
        if(periph_idx < SPMI_VDEV_MAX_PERIPHERALS) {
            // read back acc_enable
            QVM_LOG(SPMIVIRTLOG_DEBUG,"Trapped PMIC_ARB_REG_CHNLn peripheral %d \n", periph_idx);
            qvm_cpu_read(gcp, QXF_NONE,  oopnd, 1, (void*)&(dev->perph_data[periph_idx].id), sizeof(dev->perph_data[periph_idx].id));
            QVM_LOG(SPMIVIRTLOG_DEBUG,"writing PPID %d\n", dev->perph_data[periph_idx].id);
        }
    }
    else if (base >= dev->spmi_virt_pic_base)
    {
        int periph_idx = (base - dev->spmi_virt_pic_base) / SPMI_PIC_PERIPH_WIDTH;
        uint32_t pic_reg_offset = off;
        QVM_LOG(SPMIVIRTLOG_DEBUG,"periph_idx %x  reg offset %x\n", periph_idx, pic_reg_offset);
        uint32_t reg_val = 0;
        qvm_cpu_read(gcp, QXF_NONE,  oopnd, 1, (void*)&(reg_val), 2*sizeof(reg_val));


        if(pic_reg_offset == VSPMI_PIC_REG_ACC_ENABLE) {
            // update acc_enable
            QVM_LOG(SPMIVIRTLOG_DEBUG,"Trapped VSPMI_PIC_REG_ACC_ENABLE peripheral %d val %d \n", periph_idx, reg_val);
            if (reg_val != dev->perph_data[periph_idx].pic_reg.acc_enable)
            {
                QVM_LOG(SPMIVIRTLOG_DEBUG,"current acc_en %d is different from requested %d\n", dev->perph_data[periph_idx].pic_reg.acc_enable, reg_val);
                // value for the ACC ENABLE was changed signal the interrupt
                if(reg_val)
                {
                    // clear acc enable register
                    dev->perph_data[periph_idx].pic_reg.acc_enable = 1;

                    // enable interrupt
                    rc = spmi_interrupt_enable (dev->spmi_fd, periph_idx);
                    QVM_LOG(SPMIVIRTLOG_DEBUG,"spmi_interrupt_enable() requested response %d\n", rc);
                    if (dev->perph_data[periph_idx].msg_ch == -1)
                    {
                        // channel is not created -> start thread
                        QVM_LOG(SPMIVIRTLOG_DEBUG,"start interrupt wait thread() \n");
                        pthread_create(&(dev->perph_data->ch_irq_thread), NULL, (void*)spmi_virt_int_wait, (void*)&(dev->perph_data[periph_idx]));
                    }
                }
                else
                {
                    // disable interrupt service
                    rc = spmi_interrupt_disable (dev->spmi_fd, periph_idx);
                    QVM_LOG(SPMIVIRTLOG_DEBUG,"spmi_interrupt_disable() requested response %d \n", rc);

                    // clear acc enable register
                    dev->perph_data[periph_idx].pic_reg.acc_enable = 0;
                    QVM_LOG(SPMIVIRTLOG_DEBUG,"reset acc_enable local register \n");

                    // lock peripheral mutex
                    pthread_mutex_lock(&dev->perph_data[periph_idx].mutex);

                    // clear listening channel
                    rc = spmi_channel_clear (dev->spmi_fd, periph_idx, 0xff, dev->perph_data[periph_idx].msg_ch);
                    QVM_LOG(SPMIVIRTLOG_DEBUG,"spmi_channel_clear() - requested response %d\n", rc);

                    // reset peripheral channel id
                    dev->perph_data[periph_idx].msg_ch = -1;
                    QVM_LOG(SPMIVIRTLOG_DEBUG,"reset msg_ch \n");

                    // unlock peripheral mutex
                    pthread_mutex_unlock(&dev->perph_data[periph_idx].mutex);
                }
            }
        } else if (pic_reg_offset == VSPMI_PIC_REG_IRQ_CLEAR) {
            // update back irq clear
            // send interrupt done for the processed interrupt(s)
            QVM_LOG(SPMIVIRTLOG_DEBUG,"Trapped VSPMI_PIC_REG_IRQ_CLEAR peripheral %d mask val %x \n", periph_idx, reg_val);
            rc = spmi_interrupt_done (dev->spmi_fd, periph_idx, reg_val);
            QVM_LOG(SPMIVIRTLOG_DEBUG,"spmi_interrupt_done() irq_mas %x - response %d\n", reg_val, rc);

            // clear local irq status with processed irq mask
            //dev->perph_data[periph_idx].pic_reg.irq_clear |= reg_val;
            pthread_mutex_lock(&dev->perph_data[periph_idx].mutex);
            dev->perph_data[periph_idx].pic_reg.irq_status = dev->perph_data[periph_idx].pic_reg.irq_status ^ reg_val;
            pthread_mutex_unlock(&dev->perph_data[periph_idx].mutex);
            QVM_LOG(SPMIVIRTLOG_DEBUG,"remaining irq_status () %x\n", dev->perph_data[periph_idx].pic_reg.irq_status);

            int irq_status_cnt = 1;
            if (!dev->perph_data[periph_idx].pic_reg.irq_status)
            {
                // if this status register is clear
                QVM_LOG(SPMIVIRTLOG_DEBUG,"current irq_status is 0 check all others\n");
                irq_status_cnt = 0;
                // go over all peripherals
                for (int idx = 0; idx < SPMI_VDEV_MAX_PERIPHERALS; idx++)
                {
                    // check each status register
                    irq_status_cnt += (dev->perph_data[idx].pic_reg.irq_status == 0)? 0 : 1;
                }
            }
            // after loop if all are zeros - issue GIC fire down
            QVM_LOG(SPMIVIRTLOG_DEBUG,"irq_status_cnt is %d\n", irq_status_cnt);
            if(irq_status_cnt == 0)
            {
                qvm_vdev_t* vdp = (qvm_vdev_t*)dev->perph_data[periph_idx].vdp;
                qvm_signal_intr(&vdp->v_intr, QIS_OFF);
                QVM_LOG(SPMIVIRTLOG_DEBUG,"set GIC signal down \n");
                dev->spmi_gic_status = 0;
            }
        }
    }

    pthread_mutex_unlock(&vdev_mutex);

    return QVRS_NORMAL;
}


/**
 * Perform a spmi_virt device specific state initialization
 * @param           dev     spmi_virt device
 * @returns         0 if spmi virt addr is set, else -1
 */
static int init_spmi_virt_device ( spmi_virt_dev_t *dev ) {
    if(!dev->spmi_virt_base)
    {
        dev->spmi_virt_base = SPMI_VDEV_CHANNELS_BASE;
        QVM_LOG(SPMIVIRTLOG_ERROR,"spmi_virt_addr not set - defaulted to %x", dev->spmi_virt_base);
    }

    if(!dev->spmi_virt_pic_base)
    {
        dev->spmi_virt_pic_base = SPMI_VDEV_PIC_BASE;
        QVM_LOG(SPMIVIRTLOG_ERROR,"spmi_pic_virt_addr not set - default to %x", dev->spmi_virt_pic_base);
    }

    if(!dev->spmi_virt_core_base)
    {
        dev->spmi_virt_core_base = SPMI_VDEV_CORE_BASE;
        QVM_LOG(SPMIVIRTLOG_ERROR,"spmi_core_virt_addr not set - default to %x", dev->spmi_virt_core_base);
    }

    if ((dev->spmi_fd = spmi_open(SPMI_DEV_NODE)) == -1) {
        QVM_LOG(SPMIVIRTLOG_ERROR,"spmi_fd not set");
        return -1;
    }

    if(!dev->spmi_virt_gic)
    {
        dev->spmi_virt_gic = SPMI_VDEV_GIC;
        QVM_LOG(SPMIVIRTLOG_WARNING, "spmi GIC is not set - default to %d", (int)dev->spmi_virt_gic);
    }
    dev->spmi_gic_status = 0;

    if ((dev->adc_fd = spmi_open(ADC_DEV_NODE)) == -1) {
        QVM_LOG(SPMIVIRTLOG_ERROR, "Can't open device %s for RDWR, errno %d", ADC_DEV_NODE, errno);
    }

    dev->adc_data.vadc_revision2 = 0x4;
    dev->adc_data.vadc_perph_type = 0x8;
    dev->adc_data.vadc_en_ctl1 = 0x0;
    dev->adc_data.vadc_status1 = 0x1;

    trap_regions[REGION_SPMI_VIRT_IDX].guest.location = dev->spmi_virt_base;
    trap_regions[REGION_SPMI_PIC_VIRT_IDX].guest.location = dev->spmi_virt_pic_base;
    trap_regions[REGION_SPMI_CORE_VIRT_IDX].guest.location = dev->spmi_virt_core_base;


    return 0;
}


static uint32_t get_vmid  ( const char *system_name )
{
    for ( int i = 0 ; i < fdt_get_num_vm ( ) ; i++ ) {
        uint32_t vmid; 
        const int rc = fdt_get_vmid ( i , &vmid ) ;
        if ( rc != EOK ) {
            return rc ;
        }

        const char* a_vm_name = fdt_get_vm_name ( vmid ) ;
        if ( ! a_vm_name ) {
            return ENOENT;
        }

        if ( 0 == strncmp ( system_name , a_vm_name , strlen ( system_name ))) {
            return vmid ; 
        }
    }

    return -1;
}

static int spmi_parse_dt ( qvm_vdev_t *const vdp ) 
{
    int rc ;
    spmi_virt_dev_t *dev = vdp->v_device;

    const void * fdt = fdt_get_root ( ) ;
    if ( ! fdt ) {
        QVM_LOG(SPMIVIRTLOG_ERROR,"No DT soup..");
        return ENOENT;
    }

    rc = fdt_init_vm_desc( fdt ) ;
    if ( rc != EOK ) {
        QVM_LOG(SPMIVIRTLOG_ERROR,"fdt_init_vm_desc() failed");
        return rc;
    }

    char* system_name = get_system_name (vdp ) ;
    if ( ! system_name ) {
        QVM_LOG(SPMIVIRTLOG_ERROR,"get_system_name() failed");
        return EINVAL;
    }

    const uint32_t vmid = get_vmid ( system_name ) ;
    if ( vmid == -1 ) {
        QVM_LOG(SPMIVIRTLOG_ERROR,"get_vmid(%s) failed", system_name );
        free ( system_name );
        return EINVAL;
    }

    //we are done with system_name, free it.
    free ( system_name );

    dev->peripherals = fdt_get_allowed_spmi_peripherals ( vmid , &dev->nperipherals  ) ;
    if ( !dev->peripherals || !dev->nperipherals ) {
        QVM_LOG(SPMIVIRTLOG_ERROR,"DT prop \"allowed-spmi-peripherals\" missing for vmid=%d", 
                vmid );
        return ENOENT;
    }

    return EOK ; 
}

/**
 * Handle process-container operations.
 * @param       vdp               virtual device
 * @param       ctrl                operation
 * @param       arg                parameters for the operation
 * @return          EOK on success, an error otherwise.
 */
static int spmi_virt_control(qvm_vdev_t *const vdp, unsigned const ctrl, const char *const arg)
{

    int rc = EOK;
    spmi_virt_dev_t *dev = vdp->v_device;
    int parent;
    int node;
    char src_intr[128];
    char *ret_intr;

    switch (ctrl) {
        case VDEV_CTRL_OPTIONS_START:
            /*initialize vdev state*/
            vdp->v_block.type = QST_DEFAULT_TYPE_IO;
            break;

        case VDEV_CTRL_OPTIONS_END:
            if ( EOK != ( rc = init_spmi_virt_device  ( dev )) )
            {
                QVM_LOG(SPMIVIRTLOG_ERROR,"Couldn't initialize spmi_virt_device");
                break;
            }

            for (int i = 0; i < SPMI_VDEV_MAX_PERIPHERALS; i++)
            {
                // initialize info for all peripheral data
                memset(&dev->perph_data[i], 0, sizeof(spmi_virt_perph_data_s));
                dev->perph_data[i].vdp = (void*)vdp;
                dev->perph_data[i].id = 0; // instead or pre initialized
                pthread_mutex_init(&dev->perph_data[i].mutex, NULL);
                dev->perph_data[i].msg_ch = -1;
                //dev->perph_data[i].ch_irq_thread;
            }

            // reset gic status
            dev->spmi_gic_status = 0;
            //construct a qvm_intr instance which ties to bark IRQ
            if(NULL == vdp->v_intr.pic)
            {
                snprintf(src_intr, sizeof(src_intr),"gic:%lu",dev->spmi_virt_gic);
                ret_intr = parse_intr(src_intr,&vdp->v_intr);
                if(&vdp->v_intr)
                {
                    QVM_LOG(SPMIVIRTLOG_DEBUG,"print the vdp->v_intr.pic == 0x%" PRIXPTR " :: vdp->v_intr.param = %u \n" , (uintptr_t)vdp->v_intr.pic,vdp->v_intr.param);
                    QVM_LOG(SPMIVIRTLOG_DEBUG,"print the vdp->v_intr.src_id = %u :: src_intr = %s \n" , vdp->v_intr.src_id,src_intr);
                }
                else
                {
                    QVM_LOG(SPMIVIRTLOG_ERROR,"coudn't construct qvm_intr instance \n" );
                }
            }

            rc = spmi_parse_dt ( vdp ) ;
            if ( rc != EOK ) {
                return rc;
            }

            trap_regions[REGION_SPMI_VIRT_IDX].vdev = vdp;
            //setup 'cookie' to array index for quick lookup in vread/vwrite
            trap_regions[REGION_SPMI_VIRT_IDX].cookie = REGION_SPMI_VIRT_IDX;

            trap_regions[REGION_SPMI_PIC_VIRT_IDX].vdev = vdp;
            //setup 'cookie' to array index for quick lookup in vread/vwrite
            trap_regions[REGION_SPMI_PIC_VIRT_IDX].cookie = REGION_SPMI_PIC_VIRT_IDX;

            trap_regions[REGION_SPMI_CORE_VIRT_IDX].vdev = vdp;
            //setup 'cookie' to array index for quick lookup in vread/vwrite
            trap_regions[REGION_SPMI_CORE_VIRT_IDX].cookie = REGION_SPMI_CORE_VIRT_IDX;

            qvm_region_set(vdp->v_gsp, 3, &trap_regions[0]);
            break;

        case (VDEV_CTRL_FIRST_OPTIDX + OPTION_SPMI_VIRT_BASE):
            dev->spmi_virt_base = strtoul(arg, NULL, 16);
            QVM_LOG(SPMIVIRTLOG_DEBUG,"spmi_virt_base = 0x%08x\n", dev->spmi_virt_base);
            break;

        case (VDEV_CTRL_FIRST_OPTIDX + OPTION_SPMI_VIRT_PIC_BASE):
            dev->spmi_virt_pic_base = strtoul(arg, NULL, 16);
            QVM_LOG(SPMIVIRTLOG_DEBUG,"spmi_virt_pic_base = 0x%08x\n", dev->spmi_virt_pic_base);
            break;

        case (VDEV_CTRL_FIRST_OPTIDX + OPTION_SPMI_VIRT_CORE_BASE):
            dev->spmi_virt_core_base = strtoul(arg, NULL, 16);
            QVM_LOG(SPMIVIRTLOG_DEBUG,"spmi_virt_core_base = 0x%08x\n", dev->spmi_virt_core_base);
            break;

        case (VDEV_CTRL_FIRST_OPTIDX + OPTION_SPMI_VIRT_GIC):
            dev->spmi_virt_gic = strtoul(arg, NULL, 16);
            QVM_LOG(SPMIVIRTLOG_DEBUG,"spmi_pic_virt_gic = %d\n", (int)dev->spmi_virt_gic);
            break;

        case (VDEV_CTRL_FIRST_OPTIDX + OPTION_SPMI_VIRT_VERBOSE):
            g_verbose = strtoul(arg, NULL, 16);

            if(SPMIVIRTLOG_DEBUG < g_verbose)
            {
                g_verbose = SPMIVIRTLOG_DEBUG;
            }
            QVM_LOG(SPMIVIRTLOG_DEBUG,"g_verbose = %u \n", g_verbose);
            break;

        case VDEV_CTRL_GEN_FDT:
            parent = (int)(uintptr_t)arg;
            node = gfdt_update_node(parent, "spmi_virtual", vdp->v_block.location);

            if(node > 0) {
                uint32_t dtprop = dev->spmi_virt_base;
                FLIP_UINT32_BYTE_ORDER(dtprop);
                gfdt_update_prop_strings(node, "compatible", "qcom,spmi_virtual", NULL);
                gfdt_update_property(node, "qcom,spmi-vdev-addr", &dtprop, sizeof(dtprop));

                dtprop = dev->spmi_virt_pic_base;
                FLIP_UINT32_BYTE_ORDER(dtprop);
                gfdt_update_prop_strings(node, "compatible", "qcom,spmi_virtual", NULL);
                gfdt_update_property(node, "qcom,spmi-pic-vdev-addr", &dtprop, sizeof(dtprop));
            }
            else
            {
                QVM_LOG(SPMIVIRTLOG_ERROR,"spmi_virtual guest device tree not present\n");
            }
            break;

        case VDEV_CTRL_BOOTUP:
        case VDEV_CTRL_RESET:
        case VDEV_CTRL_TERMINATE:
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
static void __attribute__((constructor)) spmi_virt_register(void)
{
    static const char * const spmi_virt_options[] = { "spmi_virt_base", "spmi_virt_pic_base", "spmi_virt_core_base", "spmi_virt_gic", "verbose", NULL };


    static struct qvm_vdev_factory spmi_virt_factory = {
        .next = NULL, // patched
        .control = spmi_virt_control,
        .vread = spmi_virt_vread,
        .vwrite = spmi_virt_vwrite,
        .option_list = spmi_virt_options,
        .name = NULL, // patched
        .factory_flags = QVFF_NONE,
        .acc_sizes = 1u << sizeof(uint32_t),
        .extra_space = sizeof(spmi_virt_dev_t),
    };
    qvm_register_vdev_factory(&spmi_virt_factory, QVM_VDEV_ABI);
}
