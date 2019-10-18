/*
 * $QNXLicenseC:
 * Copyright 2016, 2017 QNX Software Systems.
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

#include <stdlib.h>
#include <stdio.h>

#include <stdarg.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>
#include <pthread.h>

#include <stdint.h>
#include <sys/mman.h>
#include <sys/neutrino.h>
#include <hw/inout.h>
#include <sys/resmgr.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <errno.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>
#include <devctl.h>

#include <atomic.h>
#include <sys/procmgr.h>

#include <hw/hwinfo_rcar.h>
#include <drvr/hwinfo.h>
#include <hw/rcar-thermal.h>
#include "io.h"

#define MAX_THRESHOLDS_NUM  3

typedef struct ths_civm_dev{
    iofunc_attr_t       hdr;
    dispatch_t          *dpp;
    dispatch_context_t  *ctp;

    uint32_t            version;    /* ver1: H3 WS1.1; ver2: H3 WS2.0, M3 WS1.0; ver3: V3 */
    paddr_t             base;
    unsigned            num_irq;
    int                 *irq;       /* interrupt number */
    int                 *id;        /* interrupt ID */
    int                 thres[MAX_THRESHOLDS_NUM];     /* temp threshold * 10 (for rounding) */
    int                 tid;
    int                 chid;
    int                 coid;
    struct sigevent     event;

    ths_civm_t          *ths_civm;  /*Structure contain registers if the THS/CIVM */
}ths_civm_dev_t;


/* our connect functions */
resmgr_connect_funcs_t connect_funcs;
resmgr_io_funcs_t io_funcs;
/* our dispatch, resource manager and iofunc variables */
resmgr_attr_t rattr;
ths_civm_dev_t *dev;
int intr_pulse_code;

extern int verbose;
extern int interface;
static volatile unsigned done = 0;

void ths_civm_init(ths_civm_dev_t *dev);
void ths_civm_interrupt_process(ths_civm_dev_t *dev);


//_round_temp takes in temp * 10 to account for rounding
#define CODE_TO_TEMP(ctemp)           ((10L * ((1000L * (ctemp)) - 2536700L)) / 7468L)
#define CODE_TO_TEMP_VER3(ctemp)      (10L * ((5L * (ctemp)) - 65L))
#define TEMP_TO_CODE(celcius)         ((((celcius) * 7468L) + 2536700L) / 1000L)
#define TEMP_TO_CODE_VER3(celcius)    ((int)((((celcius) + 60.0f) / 15) * 3) + 1)
/* TODO: We don't have the formula to convert digital code to voltage yet, returning raw code */
#define CODE_TO_VOLT(volt_code)       (volt_code)
#define CODE_TO_VOLT_VER3(volt_code)  (5L * (volt_code) + 855L)

static int _round_temp(int i)
{
    int result = (abs(i) / 10.0f) + 0.5f;

    return ((i < 0) ? (result*(-1)) : result);
}

void exit_signal(int signo)
{
    atomic_set(&done, 1);
    return;
}

int query_hwinfo(ths_civm_dev_t * dev, int interface)
{
    unsigned    i, irq_cnt, hwi_off;
    hwi_tag     *tag;

    hwi_off = hwi_find_device(RCAR_HWI_THERMAL, interface);
    if(hwi_off != HWI_NULL_OFF){
        tag = hwi_tag_find(hwi_off, HWI_TAG_NAME_location, 0);
        if(tag) {
            dev->base = tag->location.base;

            irq_cnt = 0;
            while(hwitag_find_ivec(hwi_off, &irq_cnt) != HWI_ILLEGAL_VECTOR) {
                dev->num_irq++;
            }

            if(dev->num_irq) {
                dev->irq = malloc(dev->num_irq * sizeof(int));
                dev->id = malloc(dev->num_irq * sizeof(int));

                if((dev->irq == NULL)||(dev->id == NULL)) {
                    thermal_slogf(VERBOSE_QUIET,"%s: irq/id malloc failed\n", __FUNCTION__);
                    return -1;
                }

                for(i = 0;i < dev->num_irq;i++) {
                    irq_cnt = i;
                    dev->irq[i] = hwitag_find_ivec(hwi_off, &irq_cnt);
                }
            }

            tag = hwi_tag_find(hwi_off, HWI_TAG_NAME_hwversion, 0);
            if(tag)
                dev->version = tag->hwversion.version;
        }
    }

    return 0;
}

//  This routine handles the command line options.
int options(ths_civm_dev_t * dev, int argc, char* argv[])
{
    int     c, i, ret=0;
    char    *t;
    double  temp;

    while ((c = getopt(argc, argv, "I:b:i:t:vV:h:")) != -1)
    {
        switch (c)
        {
            case 'I':
                ret = query_hwinfo(dev, strtoul(optarg, NULL, 0));
                break;
            case 'b':
                dev->base = strtoul(optarg, NULL, 0);
                break;
            case 'i':
                t = optarg;

                /* allocate memory for irq if not already done in query_hwinfo */
                if(!dev->num_irq) {
                    dev->num_irq = 1;

                    while(t && *t != '\0') {
                        if((t = strchr(t, ','))) {
                            t++;
                            dev->num_irq++;
                        }
                    }
                    dev->irq = malloc(dev->num_irq * sizeof(int));
                    dev->id = malloc(dev->num_irq * sizeof(int));

                    if((dev->irq == NULL)||(dev->id == NULL)) {
                        thermal_slogf(VERBOSE_QUIET,"%s: irq/id malloc failed\n", __FUNCTION__);
                        return -1;
                    }
                }

                /* set/override irq values */
                i = 0;
                while(optarg && *optarg != '\0' && i < dev->num_irq) {
                    dev->irq[i] = strtoul(optarg, NULL, 0);
                    i++;

                    /* Find next irq */
                    if((optarg = strchr(optarg, ','))) {
                        optarg++;
                    }
                }
                break;
            case 'V':
                dev->version = strtoul(optarg, NULL, 0);
                break;
            case 't':
                i = 0;
                while(optarg && *optarg != '\0' && i < MAX_THRESHOLDS_NUM) {
                    temp = atof(optarg);
                    if(temp >= CTEMP_MIN_TEMP && temp <= CTEMP_MAX_TEMP) {
                        dev->thres[i] = temp * 10;
                        i++;
                    }

                    /* Find next temp */
                    if((optarg = strchr(optarg, ','))) {
                        optarg++;
                    }
                }
                break;
            case 'v':
                verbose++;
                break;
            default:
                thermal_slogf(VERBOSE_QUIET,"%s: Invalid option\n", __FUNCTION__);
                ret = -1;
                break;
        }

    }
    return ret;
}

int
intr_pulse_handler (message_context_t *ctp, int code, unsigned flags, void *handle)
{
    int idx;
    ths_civm_dev_t *dev = (ths_civm_dev_t *)handle;

    if(code == intr_pulse_code) {
        ths_civm_interrupt_process(dev);
        for(idx = 0; idx < dev->num_irq; idx++) {
            InterruptUnmask(dev->irq[idx], dev->id[idx]);
        }
    } else  {
        thermal_slogf(VERBOSE_LEVEL1,"%s: Received an unknown pulse: %d",
                      __FUNCTION__, code);
    }
    return 0;
}

void dinit(ths_civm_dev_t *dev)
{
    if(dev) {
        if(dev->base)
            munmap_device_io(dev->base, sizeof(ths_civm_t));
        if(dev->irq)
            free(dev->irq);
        if(dev->id)
            free(dev->id);
        free(dev);
    }
}

int main(int argc, char *argv[])
{
    int idx, status, error = EXIT_FAILURE;

    verbose = 0;

    dev=(ths_civm_dev_t *) malloc(sizeof(ths_civm_dev_t));
    if(dev == NULL)
    {
        thermal_slogf(VERBOSE_QUIET,"%s: ths_civm malloc failed %s\n", __FUNCTION__, strerror(errno));
        goto fail;
    }

    // parse the command-line options
    status = options(dev, argc, argv);
    if(status)
    {
        goto fail;
    }

    if(!dev->base)
    {
        thermal_slogf(VERBOSE_QUIET,"%s: Invalid base=0x%x\n", __FUNCTION__, dev->base);
        goto fail;
    }
    thermal_slogf(VERBOSE_LEVEL2,"%s: base=0x%x num_irq=%d", __FUNCTION__, dev->base, dev->num_irq);

    if (-1 == ThreadCtl(_NTO_TCTL_IO, 0))
    {
        thermal_slogf(VERBOSE_QUIET,"%s: ThreadCtl failed: %s\n", __FUNCTION__, strerror(errno));
        goto fail;
    }

    /*
     * Single_instance
     * Ensure that the driver is not already running
     * Request a minor number of 0
     * If we get it, we're the first instantiation.
     */
    if (name_attach(NULL, RESMGR_NAME, 0) == NULL)
    {
        thermal_slogf(VERBOSE_QUIET,"\nError: Is '%s' already started? \n", RESMGR_NAME);
        goto fail;
    }

    // allocate and initialize a dispatch structure for use by our main loop
    dev->dpp = dispatch_create();
    if (dev->dpp == NULL)
    {
        thermal_slogf(VERBOSE_QUIET,"%s:  couldn't dispatch_create: %s\n",__FUNCTION__, strerror (errno));
        goto fail;
    }

    /*
     * Set up the resource manager attributes structure, we'll
     * use this as a way of passing information to resmgr_attach().
     * For now, we just use defaults.
     */

    memset(&rattr, 0, sizeof (rattr));
    rattr.msg_max_size = MAX_MSG_SIZE;  // max size of a message

    /*
     * Intialize the connect functions and I/O functions tables to
     * their defaults and then override the defaults with the
     * functions that we are providing.
     */
    iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs,
            _RESMGR_IO_NFUNCS, &io_funcs);

    // use our own devctl
    io_funcs.devctl = io_devctl;

    /*
     * Set the mode to 0666 (R/W)
     * Note leading 0 for octal -------v
     */
    iofunc_attr_init(&dev->hdr, S_IFCHR | 0666, NULL, NULL);

    /*
     * Call resmgr_attach to register our prefix with the
     * process manager, and also to let it know about our connect and I/O functions.
     * On error, returns -1 and errno is set.
     */
    if (resmgr_attach(dev->dpp, &rattr, DEV_NAME, _FTYPE_ANY, 0, &connect_funcs, &io_funcs, &dev->hdr) == -1)
    {
        thermal_slogf(VERBOSE_QUIET,"%s:  couldn't attach pathname: %s\n", __FUNCTION__, strerror(errno));
        goto fail;
    }

    /* THS_CIVM mapping */
    if ((dev->ths_civm = (ths_civm_t *)mmap_device_io(sizeof(ths_civm_t), dev->base)) == MAP_FAILED) {
        thermal_slogf(VERBOSE_QUIET,"%s:  couldn't mapping for ths 0: %s\n", __FUNCTION__, strerror(errno));
        goto fail;
    }
    ths_civm_init(dev);

    intr_pulse_code = pulse_attach(dev->dpp, MSG_FLAG_ALLOC_PULSE, 0, intr_pulse_handler, dev);
    /* create a connection to the channel that our resource manager is receiving on */
    dev->coid = message_connect (dev->dpp, MSG_FLAG_SIDE_CHANNEL);
    if (dev->coid == -1) {
        thermal_slogf(VERBOSE_QUIET,"%s: unable to connect to side channel", __FUNCTION__);
        goto fail;
    }

    SIGEV_PULSE_INIT(&dev->event, dev->coid, 21, intr_pulse_code, NULL);

    for(idx = 0; idx < dev->num_irq; idx++) {
        thermal_slogf(VERBOSE_LEVEL2,"%s: irq[%d]=%d", __FUNCTION__, idx, dev->irq[idx]);
        /* Interrupt attach */
        if ((dev->id[idx] = InterruptAttachEvent(dev->irq[idx], &dev->event,
                                            _NTO_INTR_FLAGS_TRK_MSK|_NTO_INTR_FLAGS_END)) == -1)
            thermal_slogf(VERBOSE_QUIET,"%s: Interrupt attach irq=%d failed:%s \n", __FUNCTION__, dev->irq[idx], strerror(errno));
    }

    /* allocate a context structure */

    dev->ctp = dispatch_context_alloc(dev->dpp);
    /*  Start the resource manager loop */
    while (!done)
    {
        if (dev->ctp == dispatch_block(dev->ctp))
            dispatch_handler(dev->ctp);
        else if (errno != EFAULT)
            atomic_set(&done, 1);
    }

    error = EXIT_SUCCESS;

fail:
    dinit(dev);
    exit(error);

    return error;
}

int ths_temp_to_code(ths_civm_dev_t *dev, int temp) {
    if (temp == 0)
        return 0;

    temp = _round_temp(temp);

    if (dev->version == 3)
        return TEMP_TO_CODE_VER3(temp);
    else
        return TEMP_TO_CODE(temp);
}

/*
 * Initialize THS/CIVM module
 */
void ths_civm_init(ths_civm_dev_t *dev)
{
    thermal_slogf(VERBOSE_LEVEL1,"%s", __FUNCTION__);
    ths_civm_ver1_t *ths_ver1 = &dev->ths_civm->ver1;
    ths_civm_ver2_t *ths_ver2 = &dev->ths_civm->ver2;
    ths_civm_ver3_t *ths_ver3 = &dev->ths_civm->ver3;

    /* Power-on mode
     * This mode operates automatically at start up
     */

     thermal_slogf(VERBOSE_LEVEL2, "%s: thres0=%d thres1=%d thres2=%d", __FUNCTION__,
             dev->thres[0], dev->thres[1], dev->thres[2]);

    /* Correction of THS & CIVM
     * TODO: Refer to Application Notes
     */

    /* Setting for Normal mode  */
    thermal_slogf(VERBOSE_LEVEL1, "%s: version %d", __FUNCTION__, dev->version);
    switch(dev->version) {
        case 1:
            /* Initialization sequence from 10A.3.1.2 */
            ths_ver1->CTSR = THSCIVM_CTSR_THBGR;
            ths_ver1->CTSR = 0;
            delay(1);  /* Wait 1ms */
            /* Switch mode */
            ths_ver1->CTSR = THSCIVM_CTSR_PONM_NORMAL;
            /*Edge detection */
            ths_ver1->IRQCTL = 0x3F; // Detect all status (TEMP_CODE [11:0] falls/exceeds IRQTEMP1,2,3)

            /* Set mask register */
            ths_ver1->IRQMSK = 0x3F; // All mask clear (pass all interrupt)
            /* Set IRQTEMP# */
            ths_ver1->IRQTEMP1 = ths_temp_to_code(dev, dev->thres[0]);
            ths_ver1->IRQTEMP2 = ths_temp_to_code(dev, dev->thres[1]);
            ths_ver1->IRQTEMP3 = ths_temp_to_code(dev, dev->thres[2]);
            thermal_slogf(VERBOSE_LEVEL2, "%s: IRQTEMP1=%d IRQTEMP2=%d IRQTEMP3=%d", __FUNCTION__,
                    ths_ver1->IRQTEMP1, ths_ver1->IRQTEMP2, ths_ver1->IRQTEMP3);
            /* Set IQREN */
            ths_ver1->IRQEN = 0x3F; // Enable all interrupt

            /*Enable analog circuit*/
            ths_ver1->CTSR |= (THSCIVM_CTSR_VMEN | THSCIVM_CTSR_THBGR | THSCIVM_CTSR_AOUT);
            /* Wait 100us */
            usleep(100);
            /*Enable digital circuit*/
            ths_ver1->CTSR |= (THSCIVM_CTSR_THSST | THSCIVM_CTSR_VMST);

            /* Clear Interrupt Status */
            ths_ver1->IRQSTR = 0;

            break;
        case 2:
        default:
            /* Initialization sequence from 10A.3 */
            /* Switch mode */
            ths_ver2->THCTR &= ~THSCIVM_THCTR_PONM;
            delay(1); /* Wait 1ms */
            /*Edge detection */
            ths_ver2->IRQCTL = 0x0;

            /* Set mask register */
            ths_ver2->IRQMSK = 0x3F; // All mask clear (pass all interrupt)
            /* Set IRQTEMP# */
            ths_ver2->IRQTEMP1 = ths_temp_to_code(dev, dev->thres[0]);
            ths_ver2->IRQTEMP2 = ths_temp_to_code(dev, dev->thres[1]);
            ths_ver2->IRQTEMP3 = ths_temp_to_code(dev, dev->thres[2]);
            thermal_slogf(VERBOSE_LEVEL2, "%s: IRQTEMP1=%d IRQTEMP2=%d IRQTEMP3=%d", __FUNCTION__,
                    ths_ver2->IRQTEMP1, ths_ver2->IRQTEMP2, ths_ver2->IRQTEMP3);
            /* Set IQREN */
            ths_ver2->IRQEN = 0x3F; // Enable all interrupt

            /* Enable circuit */
            ths_ver2->THCTR |= THSCIVM_THCTR_THSST;

            /* Clear Interrupt Status */
            ths_ver2->IRQSTR = 0;
            break;

        case 3:
            // POSNEG = 0 -> interrupt when the temp goes above threshold
            // POSNEG = 1 -> interrupt when the temp goes below threshold
            ths_ver3->POSNEG &= ~(THSCIVM_POSNEG1 | THSCIVM_POSNEG2 | THSCIVM_POSNEG0);

            /* Set operation mode for THS0: Normal 1 mode
             * Setting THSCR - TAP value is determined automatically by hardware
             */
            ths_ver3->THSCR &= ~ THSCIVM_THSCR_THIDLE_MSK;
            ths_ver3->THSCR |= THSCIVM_THSCR_CPCTL;

            usleep(300);

            /* Set Tj00/Tj01/Tj02INT_MSK */
            ths_ver3->IRQMSK |= THSCIVM_INT_TJ00_MSK | THSCIVM_INT_TJ01_MSK | THSCIVM_INT_TJ02_MSK;

            /* Set Tj00/Tj01/Tj02_EN */
            ths_ver3->IRQEN |= THSCIVM_ENR_TJ00_EN | THSCIVM_ENR_TJ01_EN | THSCIVM_ENR_TJ02_EN;

            /* reset threshold interrupts INTDT2/1/0 interrupts for THS0 */
            ths_ver3->INTCTLR = (
                ((ths_temp_to_code(dev, dev->thres[0])) << THSCIVM_INTCTLR_CTEMP0_POS) |
                ((ths_temp_to_code(dev, dev->thres[1])) << THSCIVM_INTCTLR_CTEMP1_POS) |
                ((ths_temp_to_code(dev, dev->thres[2])) << THSCIVM_INTCTLR_CTEMP2_POS) );

            thermal_slogf(VERBOSE_LEVEL2, "%s: CTEMP0=%d CTEMP1=%d CTEMP2=%d", __FUNCTION__,
                (ths_ver3->INTCTLR & THSCIVM_INTCTLR_CTEMP0_MSK) >> THSCIVM_INTCTLR_CTEMP0_POS,
                (ths_ver3->INTCTLR & THSCIVM_INTCTLR_CTEMP1_MSK) >> THSCIVM_INTCTLR_CTEMP1_POS,
                (ths_ver3->INTCTLR & THSCIVM_INTCTLR_CTEMP2_MSK) >> THSCIVM_INTCTLR_CTEMP2_POS);

            /* Unmask Tj00/Tj01/Tj02*/
            ths_ver3->IRQMSK &= ~(THSCIVM_INT_TJ00_MSK | THSCIVM_INT_TJ01_MSK | THSCIVM_INT_TJ02_MSK);

            /* Clear Interrupt Status */
            ths_ver3->IRQSTR &= ~(THSCIVM_STR_TJ00 | THSCIVM_STR_TJ01 | THSCIVM_STR_TJ02);
            break;
    }

    /* Note: temperature is not immediately available after initialization
     * Some delay is required*/
    //delay(1);
}

/*
 * Processs thermal interrupt
 */
void ths_civm_interrupt_process(ths_civm_dev_t *dev)
{
    volatile uint32_t *irq_status;
    uint8_t   num_irq;
    uint32_t  mask;
    uint16_t  temp;

    thermal_slogf(VERBOSE_LEVEL1,"%s", __FUNCTION__);

    switch(dev->version) {
        case 1:
            irq_status = &dev->ths_civm->ver1.IRQSTR;
            num_irq = 6; // 3 raising, 3 falling
            break;
        case 3:
            irq_status = &dev->ths_civm->ver3.IRQSTR;
            num_irq = 3; // 3 raising or falling
            break;
        case 2:
        default:
            irq_status = &dev->ths_civm->ver2.IRQSTR;
            num_irq = 6; // 3 raising, 3 falling
            break;
    }

    ths_civm_get_temperature(&temp);

    for (int i = 0; i < num_irq; i++){
        mask = (1 << i);
        if ( mask == (*irq_status & mask)) {
            thermal_slogf(VERBOSE_LEVEL1, "Threshold crossed, IRQSTR[%d] triggered. Temp %d", i, temp);
            *irq_status &= ~ mask; // Clear the interrupt
        }
    }
}

/*
 * Get current temperature of thermal:
 */
int ths_civm_get_temperature(uint16_t *temperature)
{
    uint32_t ret = 0;
    int code;

    thermal_slogf(VERBOSE_LEVEL1, "%s", __FUNCTION__);

    switch(dev->version) {
        case 1:
            code = dev->ths_civm->ver1.TEMP & 0xFFF; // Read TEMP_CODE[11:0]
            *temperature = CODE_TO_TEMP(code);
            break;
        case 3:
            code = dev->ths_civm->ver3.THSSR & THSCIVM_THSSR_CTEMP_MSK; // Read CTEMP[5:0]
            *temperature = CODE_TO_TEMP_VER3(code);
            break;
        case 2:
        default:
            code = dev->ths_civm->ver2.TEMP & 0xFFF; // Read TEMP_CODE[11:0]
            *temperature = CODE_TO_TEMP(code);
            break;
    }

    *temperature = _round_temp(*temperature);

    thermal_slogf(VERBOSE_LEVEL1, "Temperature: %d (code: %d)", *temperature, code);

    return ret;
}

/*
 * Get current voltage of LSI:
 */
int ths_civm_get_voltage(uint16_t *voltage)
{
    uint32_t ret = 0;
    uint32_t code;

    thermal_slogf(VERBOSE_LEVEL1, "%s", __FUNCTION__);

    switch(dev->version) {
        case 1:
            code = dev->ths_civm->ver1.VOLT & 0x3FF; //Read VOLT_CODE[9:0]
            *voltage = CODE_TO_VOLT(code);
            break;
        case 3:
            code = dev->ths_civm->ver3.CIVM_SR & THSCIVM_CIVM_SR_VOLT_MSK; //Read VOLT_CODE[5:0]
            *voltage = CODE_TO_VOLT_VER3(code);
            break;
        case 2:
        default:
            code = dev->ths_civm->ver2.VOLT & 0x3FF; //Read VOLT_CODE[9:0]
            *voltage = CODE_TO_VOLT(code);
            break;
    }

    thermal_slogf(VERBOSE_LEVEL1, "Voltage: %d (code: %d)", *voltage, code);

    return ret;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/support/rcar-thermal/resmgr/thermal.c $ $Rev: 841525 $")
#endif
