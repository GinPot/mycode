/*
 * $QNXLicenseC:
 * Copyright 2016, Freescale Semiconductor, Inc.
 * Copyright 2017 NXP
 * Copyright 2018, QNX Software Systems.
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

#ifndef IMX_GPT_H_
#define IMX_GPT_H_

/* GPT registers, offset from base address */
#define IMX_GPT_CR                     0x00     /* Control Register */
#define IMX_GPT_PR                     0x04     /* Prescaler Register */
#define IMX_GPT_SR                     0x08     /* Status Register */
#define IMX_GPT_IR                     0x0C     /* Interrupt Register */
#define IMX_GPT_OCR1                   0x10     /* Output Compare Register 1 */
#define IMX_GPT_OCR2                   0x14     /* Output Compare Register 2 */
#define IMX_GPT_OCR3                   0x18     /* Output Compare Register 3 */
#define IMX_GPT_ICR1                   0x1C     /* Input Capture Register 1 */
#define IMX_GPT_ICR2                   0x20     /* Input Capture Register 2 */
#define IMX_GPT_CNT                    0x24     /* Counter Register */

/*
 *  Control Register bits
 */
#define IMX_GPT_CR_FO3                 (1<<31)  /* Force Output Compare Channel 3 */
#define IMX_GPT_CR_FO2                 (1<<30)  /* Force Output Compare Channel 2 */
#define IMX_GPT_CR_FO1                 (1<<29)  /* Force Output Compare Channel 1 */
#define IMX_GPT_CR_OM3_MASK            (7<<26)  /* Output Compare Channel 3 operating mode */
#define IMX_GPT_CR_OM3_SHIFT           26
#define IMX_GPT_CR_OM2_MASK            (7<<23)  /* Output Compare Channel 2 operating mode */
#define IMX_GPT_CR_OM2_SHIFT           23
#define IMX_GPT_CR_OM1_MASK            (7<<20)  /* Output Compare Channel 1 operating mode */
#define IMX_GPT_CR_OM1_SHIFT           20
#define IMX_GPT_CR_IM2_MASK            (3<<18)  /* Input Capture Channel 2 operating mode */
#define IMX_GPT_CR_IM2_SHIFT           18
#define IMX_GPT_CR_IM1_MASK            (3<<16)  /* Input Capture Channel 1 operating mode */
#define IMX_GPT_CR_IM1_SHIFT           16
#define IMX_GPT_CR_SWR                 (1<<15)  /* Software reset */
#define IMX_GPT_CR_SWR_SHIFT           15
#define IMX_GPT_CR_EN_24M              (1<<10)  /* Enable 24MHz clock input from crystal */
#define IMX_GPT_CR_EN_24M_SHIFT        10
#define IMX_GPT_CR_FRR                 (1<<9)   /* Free-Run or Restart mode */
#define IMX_GPT_CR_FRR_SHIFT           9
#define IMX_GPT_CR_CLKSRC_MASK         (7<<6)   /* Clock Source select */
#define IMX_GPT_CR_CLKSRC_SHIFT        6
#define IMX_GPT_CR_CLKSRC_NO_CLK       0        /* Clock Source select - No clock */
#define IMX_GPT_CR_CLKSRC_PERIPH_CLK   1        /* Clock Source select - Peripheral clock */
#define IMX_GPT_CR_CLKSRC_HFR_CLK      2        /* Clock Source select - High Frequency Reference clock */
#define IMX_GPT_CR_CLKSRC_EXT_CLK      3        /* Clock Source select - External clock */
#define IMX_GPT_CR_CLKSRC_LFR_CLK      4        /* Clock Source select - Low Frequency Reference clock */
#define IMX_GPT_CR_CLKSRC_COR_CLK      5        /* Clock Source select - Crystal Oscillator Reference clock */
#define IMX_GPT_CR_STOPEN              (1<<5)   /* GPT Stop Mode enable */
#define IMX_GPT_CR_STOPEN_SHIFT        5
#define IMX_GPT_CR_DOZEEN              (1<<4)   /* GPT Doze Mode enable */
#define IMX_GPT_CR_DOZEEN_SHIFT        4
#define IMX_GPT_CR_WAITEN              (1<<3)   /* GPT Wait Mode enable */
#define IMX_GPT_CR_WAITEN_SHIFT        3
#define IMX_GPT_CR_DBGEN               (1<<2)   /* GPT Debug Mode enable */
#define IMX_GPT_CR_DBGEN_SHIFT         2
#define IMX_GPT_CR_ENMOD               (1<<1)   /* GPT Enable mode */
#define IMX_GPT_CR_ENMOD_SHIFT         1
#define IMX_GPT_CR_EN                  (1<<0)   /* GPT Enable */
#define IMX_GPT_CR_EN_SHIFT            0

/*
 *  Prescaler Register bits
 */
#define IMX_GPT_PR_PRESCALER24M_MASK   (0x0F<<12)   /* Prescaler bits */
#define IMX_GPT_PR_PRESCALER24M_SHIFT  12
#define IMX_GPT_PR_PRESCALER_MASK      (0xFFF<<0)   /* Prescaler bits */
#define IMX_GPT_PR_PRESCALER_SHIFT     0

/*
 *  Status Register bits
 */
#define IMX_GPT_SR_ROV                 (1<<5)       /* Rollover Flag */
#define IMX_GPT_SR_ROV_SHIFT           5
#define IMX_GPT_SR_IF2                 (1<<4)       /* Input capture 2 Flag */
#define IMX_GPT_SR_IF2_SHIFT           4
#define IMX_GPT_SR_IF1                 (1<<3)       /* Input capture 1 Flag */
#define IMX_GPT_SR_IF1_SHIFT           3
#define IMX_GPT_SR_OF3                 (1<<2)       /* Output Compare 3 Flag */
#define IMX_GPT_SR_OF3_SHIFT           2
#define IMX_GPT_SR_OF2                 (1<<1)       /* Output Compare 3 Flag */
#define IMX_GPT_SR_OF2_SHIFT           1
#define IMX_GPT_SR_OF1                 (1<<0)       /* Output Compare 3 Flag */
#define IMX_GPT_SR_OF1_SHIFT           0

/*
 *  Interrupt Register bits
 */
#define IMX_GPT_IR_ROVIE               (1<<5)       /* Rollover Interrupt Enable */
#define IMX_GPT_IR_ROVIE_SHIFT         5
#define IMX_GPT_IR_IF2IE               (1<<4)       /* Input capture 2 Interrupt Enable */
#define IMX_GPT_IR_IF2IE_SHIFT         4
#define IMX_GPT_IR_IF1IE               (1<<3)       /* Input capture 1 Interrupt Enable */
#define IMX_GPT_IR_IF1IE_SHIFT         3
#define IMX_GPT_IR_OF3IE               (1<<2)       /* Output Compare 3 Interrupt Enable */
#define IMX_GPT_IR_OF3IE_SHIFT         2
#define IMX_GPT_IR_OF2IE               (1<<1)       /* Output Compare 2 Interrupt Enable */
#define IMX_GPT_IR_OF2IE_SHIFT         1
#define IMX_GPT_IR_OF1IE               (1<<0)       /* Output Compare 1 Interrupt Enable */
#define IMX_GPT_IR_OF1IE_SHIFT         0


#endif /* IMX_GPT_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/aarch64/imx8xp/imx_gpt.h $ $Rev: 850159 $")
#endif
