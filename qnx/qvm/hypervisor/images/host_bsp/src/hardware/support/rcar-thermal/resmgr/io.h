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


#ifndef _IO_H_INCLUDED
#define _IO_H_INCLUDED

#include <sys/resmgr.h>
#include <sys/iomsg.h>
#include <sys/iofunc.h>

#define RESMGR_NAME         "thermal"
#define DEV_NAME            "/dev/" RESMGR_NAME     // do not modify

#define VERBOSE_QUIET       0
#define VERBOSE_LEVEL1      1
#define VERBOSE_LEVEL2      2
#define VERBOSE_LEVEL4      4
#define VERBOSE_LEVEL8      8

#define MAX_MSG_SIZE        IOBUF_SIZE              // max size of a message, should be max(IOBUF, devctl_t)
#define IOBUF_SIZE          1024                    // size of io_buffer used to for msg read/write with client
#define ENG                 512                     // Error Code


typedef struct
{
    volatile uint32_t THSIRQSTR;    // 0x00 THS Interrupt Status Register
    volatile uint32_t IRQSTR;       // 0x04 Interrupt Status Register
    volatile uint32_t IRQMSK;       // 0x08 Interrupt Mask Register
    volatile uint32_t IRQCTL;       // 0x0C Threshold Edge/Level Register
    volatile uint32_t IRQEN;        // 0x10 Interrupt Enable Register
    volatile uint32_t IRQTEMP1;     // 0x14 Interrupt Temperature 1 Register
    volatile uint32_t IRQTEMP2;     // 0x18 Interrupt Temperature 2 Register
    volatile uint32_t IRQTEMP3;     // 0x1C Interrupt Temperature 3 Register
    volatile uint32_t THCTR;        // 0x20 Control Status Register
    volatile uint32_t THSTR;        // 0x24 Control Status Register
    volatile uint32_t TEMP;         // 0x28 Temperature Register
    volatile uint32_t VOLT;         // 0x2C Voltage Register
}ths_civm_ver2_t;

typedef struct
{
    volatile uint32_t THSIRQSTR;    // 0x00 THS Interrupt Status Register
    volatile uint32_t IRQSTR;       // 0x04 Interrupt Status Register
    volatile uint32_t IRQMSK;       // 0x08 Interrupt Mask Register
    volatile uint32_t IRQCTL;       // 0x0C Threshold Edge/Level Register
    volatile uint32_t IRQEN;        // 0x10 Interrupt Enable Register
    volatile uint32_t IRQTEMP1;     // 0x14 Interrupt Temperature 1 Register
    volatile uint32_t IRQTEMP2;     // 0x18 Interrupt Temperature 2 Register
    volatile uint32_t IRQTEMP3;     // 0x1C Interrupt Temperature 3 Register
    volatile uint32_t CTSR;         // 0x20 Control Status Register
    volatile uint32_t UNUSED;
    volatile uint32_t TEMP;         // 0x28 Temperature Register
    volatile uint32_t UNUSED2;
    volatile uint32_t VOLT;         // 0x30 Voltage Register
}ths_civm_ver1_t;

typedef struct
{
    volatile uint32_t IRQSTR;           // 0x00 IRQSTR Interrupt Status Register
    volatile uint32_t IRQEN;            // 0x04 IRQEN Interrupt Enable Register
    volatile uint32_t Reserve0;         // 0x08 Reserve
    volatile uint32_t IRQMSK;           // 0x0C IRQMSK Interrupt Mask Register
    volatile uint32_t Reserve1[68];     // 0x10 - 0x11C Reserve
    volatile uint32_t POSNEG;           // 0x120 POSNEG Positive/Negative Logic Select Register
    volatile uint32_t Reserve2[2];      // 0x124 - 0x128 Reserve
    volatile uint32_t THSCR;            // 0x12C THSCR THS Control Register
    volatile uint32_t THSSR;            // 0x130 THSSR THS Status Register
    volatile uint32_t INTCTLR;          // 0x134 INTCTLR Interrupt Control Register
    volatile uint32_t Reserve3[57];     // 0x138 - 0x218 Reserve
    volatile uint32_t CIVM_SR;          // 0x21C CIVM_SR CIVM Status Register
}ths_civm_ver3_t;


/* Structure for all THS/CIVM register */
typedef union
{
    ths_civm_ver1_t ver1;
    ths_civm_ver2_t ver2;
    ths_civm_ver3_t ver3;
}ths_civm_t;


/************************* VER 1 - 2 **********************/
/* IRQSTR bit definition */
#define THSCIVM_IRQSTR_TEMPD3_STR   (1<<5)    // Masking of TEMPD3 interrupt requests
#define THSCIVM_IRQSTR_TEMPD2_STR   (1<<4)    // Masking of TEMPD2 interrupt requests
#define THSCIVM_IRQSTR_TEMPD1_STR   (1<<3)    // Masking of TEMPD1 interrupt requests
#define THSCIVM_IRQSTR_TEMP3_STR    (1<<2)    // Masking of TEMP3 interrupt requests
#define THSCIVM_IRQSTR_TEMP2_STR    (1<<1)    // Masking of TEMP2 interrupt requests
#define THSCIVM_IRQSTR_TEMP1_STR    (1<<0)    // Masking of TEMP1 interrupt requests

/* IRQMSK bit definition */
#define THSCIVM_IRQMSK_TEMPD3_MSK   (1<<5)    // Masking of TEMPD3 interrupt requests
#define THSCIVM_IRQMSK_TEMPD2_MSK   (1<<4)    // Masking of TEMPD2 interrupt requests
#define THSCIVM_IRQMSK_TEMPD1_MSK   (1<<3)    // Masking of TEMPD1 interrupt requests
#define THSCIVM_IRQMSK_TEMP3_MSK    (1<<2)    // Masking of TEMP3 interrupt requests
#define THSCIVM_IRQMSK_TEMP2_MSK    (1<<1)    // Masking of TEMP2 interrupt requests
#define THSCIVM_IRQMSK_TEMP1_MSK    (1<<0)    // Masking of TEMP1 interrupt requests

/* CTSR bit definition */
#define THSCIVM_CTSR_IRQ3           (1<<31)   // interrupt signal that is detected by IRQTEMP3
#define THSCIVM_CTSR_IRQ2           (1<<30)   // interrupt signal that is detected by IRQTEMP2
#define THSCIVM_CTSR_IRQ1           (1<<29)   // interrupt signal that is detected by IRQTEMP1
#define THSCIVM_CTSR_VMCNTOV_MASK   (3<<22)   // detection of an overflow of the counters for chip internal voltage monitor
#define THSCIVM_CTSR_VMCNTOV_0      (0<<22)   // Normal operation
#define THSCIVM_CTSR_VMCNTOV_1      (1<<22)   // Counter 0 has overflowed
#define THSCIVM_CTSR_VMCNTOV_2      (2<<22)   // Counter 1 has overflowed
#define THSCIVM_CTSR_VMCNTOV_3      (3<<22)   // Both counters 0 and 1 have overflowed
#define THSCIVM_CTSR_THCNTOV_MASK   (3<<20)   // Detection of an overflow of the counters for the thermal sensor
#define THSCIVM_CTSR_THCNTOV_0      (0<<20)   // Normal operation
#define THSCIVM_CTSR_THCNTOV_1      (1<<20)   // Counter 0 has overflowed
#define THSCIVM_CTSR_THCNTOV_2      (2<<20)   // Counter 1 has overflowed
#define THSCIVM_CTSR_THCNTOV_3      (3<<20)   // Both counters 0 and 1 have overflowed
#define THSCIVM_CTSR_THFAIL1        (1<<17)   // all TEMP_CODE [11:0] bits are 1
#define THSCIVM_CTSR_THFAIL0        (1<<16)   // all TEMP_CODE [11:0] bits are 0
#define THSCIVM_CTSR_PONM           (1<<8)    // Mode switching signal
#define THSCIVM_CTSR_PONM_PON       (0<<8)    // Power-on Mode
#define THSCIVM_CTSR_PONM_NORMAL    (1<<8)    // Normal Mode
#define THSCIVM_CTSR_AOUT           (1<<7)    // Enabling/disabling external output of THS
#define THSCIVM_CTSR_THBGR          (1<<5)    // Enabling/disabling of THS
#define THSCIVM_CTSR_VMEN           (1<<4)    // Enabling/disabling of the CIVM
#define THSCIVM_CTSR_VMST           (1<<1)    // Enabling/disabling of the A/D converter for the CIVM
#define THSCIVM_CTSR_THSST          (1<<0)    // Enabling/disabling of the A/D converter for the THS

/* THCTR bit definition */
#define THSCIVM_THCTR_PONM          (1<<6)
#define THSCIVM_THCTR_PONM_NORMAL   (0<<6)
#define THSCIVM_THCTR_PONM_PON      (1<<6)
#define THSCIVM_THCTR_THSST         (1<<0)

#define CTEMP_MIN_TEMP              -40
#define CTEMP_MAX_TEMP              125

/************************* VER 3 **********************/
/* IRQSTR bit definition */
#define THSCIVM_STR_TJ00            (1 << 0)    /* Tj00 Interrupt Detection Status */
#define THSCIVM_STR_TJ01            (1 << 1)    /* Tj01 Interrupt Detection Status */
#define THSCIVM_STR_TJ02            (1 << 2)    /* Tj02 Interrupt Detection Status */

/* IRQEN bit definition */
#define THSCIVM_ENR_TJ00_EN         (1 << 0)    /* Enables comparing the CTEMP0[5:0] bits in the INTCTLR register with the CTEMP[5:0] bits in the THSSR register */
#define THSCIVM_ENR_TJ01_EN         (1 << 1)    /* Enables comparing the CTEMP1[5:0] bits in the INTCTLR register with the CTEMP[5:0] bits in the THSSR register */
#define THSCIVM_ENR_TJ02_EN         (1 << 2)    /* Enables comparing the CTEMP2[5:0] bits in the INTCTLR register with the CTEMP[5:0] bits in the THSSR register */

/* IRQMSK bit definition */
#define THSCIVM_INT_TJ00_MSK        (1 << 0)    /* Selects whether or not the Tj00 interrupt request is masked when Tj00ST bit in the IRQSTR register is 1 */
#define THSCIVM_INT_TJ01_MSK        (1 << 1)    /* Selects whether or not the Tj01 interrupt request is masked when Tj00ST bit in the IRQSTR register is 1 */
#define THSCIVM_INT_TJ02_MSK        (1 << 2)    /* Selects whether or not the Tj02 interrupt request is masked when Tj00ST bit in the IRQSTR register is 1 */

/* POSNEG bit definition */
#define THSCIVM_POSNEG0             (1 << 0)    /* Selects the edge polarity of the Tj00 interrupt input signal */
#define THSCIVM_POSNEG1             (1 << 1)    /* Selects the edge polarity of the Tj01 interrupt input signal */
#define THSCIVM_POSNEG2             (1 << 2)    /* Selects the edge polarity of the Tj02 interrupt input signal */

/* THSCR bit definition */
#define THSCIVM_THSCR_THIDLE_POS    (8)         /* Selects either the normal operating state or the idle state of the THS */
#define THSCIVM_THSCR_THIDLE0       (1 << THSCIVM_THSCR_THIDLE_POS)
#define THSCIVM_THSCR_THIDLE1       (1 << (THSCIVM_THSCR_THIDLE_POS + 1))
#define THSCIVM_THSCR_THIDLE_MSK    (0x3 << THSCIVM_THSCR_THIDLE_POS)
#define THSCIVM_THSCR_CPCTL         (1 << 12)   /* TAP value select method */

/* THSSR bit definition */
#define THSCIVM_THSSR_CTEMP_POS     (0)   /* indicates the current temperature */
#define THSCIVM_THSSR_CTEMP_MSK     (0x3F << THSCIVM_THSSR_CTEMP_POS)

/* INTCTLR bit definition */
#define THSCIVM_INTCTLR_CTEMP0_POS  (0)   /* indicates the temperature that causes an INTDT0 interrupt */
#define THSCIVM_INTCTLR_CTEMP0_MSK  (0x3F << THSCIVM_INTCTLR_CTEMP0_POS)
#define THSCIVM_INTCTLR_CTEMP1_POS  (8)   /* indicates the temperature that causes an INTDT1 interrupt */
#define THSCIVM_INTCTLR_CTEMP1_MSK  (0x3F << THSCIVM_INTCTLR_CTEMP1_POS)
#define THSCIVM_INTCTLR_CTEMP2_POS  (16)  /* indicates the temperature that causes an INTDT2 interrupt */
#define THSCIVM_INTCTLR_CTEMP2_MSK  (0x3F << THSCIVM_INTCTLR_CTEMP2_POS)

/* CIVM_SR bit definition */
#define THSCIVM_CIVM_SR_VOLT_POS    (0)   /* Indicates the current chip internal voltage */
#define THSCIVM_CIVM_SR_VOLT_MSK    (0x3F << THSCIVM_CIVM_SR_VOLT_POS)


int io_devctl(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb);
int thermal_slogf(const int Verbose, const char *fmt, ...);


int ths_civm_get_temperature(uint16_t *temperature);
int ths_civm_get_voltage(uint16_t *voltage);

#endif /* _IO_H_INCLUDED */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/support/rcar-thermal/resmgr/io.h $ $Rev: 841525 $")
#endif
