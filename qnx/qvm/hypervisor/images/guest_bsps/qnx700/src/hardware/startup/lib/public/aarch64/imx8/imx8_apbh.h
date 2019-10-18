/*
 * $QNXLicenseC:
 * Copyright 2016, QNX Software Systems.
 * Copyright 2016, Freescale Semiconductor, Inc.
 * Copyright 2017 NXP
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

#ifndef IMX_APBH_H_
#define IMX_APBH_H_

#include <stdint.h>

/*
 * IMX_APBH_CTRL0 - AHB to APBH Bridge Control and Status Register 0
 */
typedef union {
    uint32_t U;
    struct {
        unsigned CLKGATE_CHANNEL:16;
        unsigned RSVD0:12;
        unsigned APB_BURST_EN:1;
        unsigned AHB_BURST8_EN:1;
        unsigned CLKGATE:1;
        unsigned SFTRST:1;
    } B;
} imx_apbh_ctrl0_t;

/*
 * constants & macros for entire IMX_APBH_CTRL0 register
 */
#define IMX_APBH_CTRL0_OFFSET                           (0x0)
#define IMX_APBH_CTRL0_SET_OFFSET                       (0x4)
#define IMX_APBH_CTRL0_CLR_OFFSET                       (0x8)

/* IMX_APBH_CTRL0 bit-fields */
/* SFTRST */
#define IMX_APBH_CTRL0_SFTRST_SHIFT                     31
#define IMX_APBH_CTRL0_SFTRST_MASK                      0x80000000
#define IMX_APBH_CTRL0_SFTRST(v)                        ((((uint32_t) v) << 31) & IMX_APBH_CTRL0_SFTRST_MASK)

/* CLKGATE */
#define IMX_APBH_CTRL0_CLKGATE_SHIFT                    30
#define IMX_APBH_CTRL0_CLKGATE_MASK                     0x40000000
#define IMX_APBH_CTRL0_CLKGATE(v)                       (((v) << 30) & IMX_APBH_CTRL0_CLKGATE_MASK)

/* AHB_BURST8_EN */
#define IMX_APBH_CTRL0_AHB_BURST8_EN_SHIFT              29
#define IMX_APBH_CTRL0_AHB_BURST8_EN_MASK               0x20000000
#define IMX_APBH_CTRL0_AHB_BURST8_EN(v)                 (((v) << 29) & IMX_APBH_CTRL0_AHB_BURST8_EN_MASK)

/* APB_BURST_EN */
#define IMX_APBH_CTRL0_APB_BURST_EN_SHIFT               28
#define IMX_APBH_CTRL0_APB_BURST_EN_MASK                0x10000000
#define IMX_APBH_CTRL0_APB_BURST_EN(v)                  (((v) << 28) & IMX_APBH_CTRL0_APB_BURST_EN_MASK)

/* RSVD0 */
#define IMX_APBH_CTRL0_RSVD0_SHIFT                      16
#define IMX_APBH_CTRL0_RSVD0_MASK                       0x0FFF0000
#define IMX_APBH_CTRL0_RSVD0(v)                         (((v) << 16) & IMX_APBH_CTRL0_RSVD0_MASK)

/* CLKGATE_CHANNEL */
#define IMX_APBH_CTRL0_CLKGATE_CHANNEL_SHIFT            0
#define IMX_APBH_CTRL0_CLKGATE_CHANNEL_MASK             0x0000FFFF
#define IMX_APBH_CTRL0_CLKGATE_CHANNEL(v)               (((v) << 0) & IMX_APBH_CTRL0_CLKGATE_CHANNEL_MASK)

#define IMX_APBH_CTRL0_CLKGATE_CHANNEL_BV_NAND0         0x0001
#define IMX_APBH_CTRL0_CLKGATE_CHANNEL_BV_NAND1         0x0002
#define IMX_APBH_CTRL0_CLKGATE_CHANNEL_BV_NAND2         0x0004
#define IMX_APBH_CTRL0_CLKGATE_CHANNEL_BV_NAND3         0x0008
#define IMX_APBH_CTRL0_CLKGATE_CHANNEL_BV_NAND4         0x0010
#define IMX_APBH_CTRL0_CLKGATE_CHANNEL_BV_NAND5         0x0020
#define IMX_APBH_CTRL0_CLKGATE_CHANNEL_BV_NAND6         0x0040
#define IMX_APBH_CTRL0_CLKGATE_CHANNEL_BV_NAND7         0x0080
#define IMX_APBH_CTRL0_CLKGATE_CHANNEL_BV_SSP           0x0100
#define NAND_CHANNEL 0

/*
 * IMX_APBH_CTRL1 - AHB to APBH Bridge Control and Status Register 1
 */
typedef union {
    uint32_t U;
    struct {
        unsigned CH0_CMDCMPLT_IRQ:1;
        unsigned CH1_CMDCMPLT_IRQ:1;
        unsigned CH2_CMDCMPLT_IRQ:1;
        unsigned CH3_CMDCMPLT_IRQ:1;
        unsigned CH4_CMDCMPLT_IRQ:1;
        unsigned CH5_CMDCMPLT_IRQ:1;
        unsigned CH6_CMDCMPLT_IRQ:1;
        unsigned CH7_CMDCMPLT_IRQ:1;
        unsigned CH8_CMDCMPLT_IRQ:1;
        unsigned CH9_CMDCMPLT_IRQ:1;
        unsigned CH10_CMDCMPLT_IRQ:1;
        unsigned CH11_CMDCMPLT_IRQ:1;
        unsigned CH12_CMDCMPLT_IRQ:1;
        unsigned CH13_CMDCMPLT_IRQ:1;
        unsigned CH14_CMDCMPLT_IRQ:1;
        unsigned CH15_CMDCMPLT_IRQ:1;
        unsigned CH0_CMDCMPLT_IRQ_EN:1;
        unsigned CH1_CMDCMPLT_IRQ_EN:1;
        unsigned CH2_CMDCMPLT_IRQ_EN:1;
        unsigned CH3_CMDCMPLT_IRQ_EN:1;
        unsigned CH4_CMDCMPLT_IRQ_EN:1;
        unsigned CH5_CMDCMPLT_IRQ_EN:1;
        unsigned CH6_CMDCMPLT_IRQ_EN:1;
        unsigned CH7_CMDCMPLT_IRQ_EN:1;
        unsigned CH8_CMDCMPLT_IRQ_EN:1;
        unsigned CH9_CMDCMPLT_IRQ_EN:1;
        unsigned CH10_CMDCMPLT_IRQ_EN:1;
        unsigned CH11_CMDCMPLT_IRQ_EN:1;
        unsigned CH12_CMDCMPLT_IRQ_EN:1;
        unsigned CH13_CMDCMPLT_IRQ_EN:1;
        unsigned CH14_CMDCMPLT_IRQ_EN:1;
        unsigned CH15_CMDCMPLT_IRQ_EN:1;
    } B;
} imx_apbh_ctrl1_t;

#define IMX_APBH_CTRL1_OFFSET                           (0x10)
#define IMX_APBH_CTRL1_SET_OFFSET                       (0x14)
#define IMX_APBH_CTRL1_CLR_OFFSET                       (0x18)
#define IMX_APBH_CTRL1_TOG_OFFSET                       (0x1C)

/* IMX_APBH_CTRL1 bit-fields */
/* CH15_CMDCMPLT_IRQ_EN */
#define IMX_APBH_CTRL1_CH15_CMDCMPLT_IRQ_EN_SHIFT       31
#define IMX_APBH_CTRL1_CH15_CMDCMPLT_IRQ_EN_MASK        0x80000000
#define IMX_APBH_CTRL1_CH15_CMDCMPLT_IRQ_EN(v)          ((((uint32_t) v) << 31) & IMX_APBH_CTRL1_CH15_CMDCMPLT_IRQ_EN_MASK)

/* CH14_CMDCMPLT_IRQ_EN */
#define IMX_APBH_CTRL1_CH14_CMDCMPLT_IRQ_EN_SHIFT       30
#define IMX_APBH_CTRL1_CH14_CMDCMPLT_IRQ_EN_MASK        0x40000000
#define IMX_APBH_CTRL1_CH14_CMDCMPLT_IRQ_EN(v)          (((v) << 30) & IMX_APBH_CTRL1_CH14_CMDCMPLT_IRQ_EN_MASK)

/* CH13_CMDCMPLT_IRQ_EN */
#define IMX_APBH_CTRL1_CH13_CMDCMPLT_IRQ_EN_SHIFT       29
#define IMX_APBH_CTRL1_CH13_CMDCMPLT_IRQ_EN_MASK        0x20000000
#define IMX_APBH_CTRL1_CH13_CMDCMPLT_IRQ_EN(v)          (((v) << 29) & IMX_APBH_CTRL1_CH13_CMDCMPLT_IRQ_EN_MASK)

/* CH12_CMDCMPLT_IRQ_EN */
#define IMX_APBH_CTRL1_CH12_CMDCMPLT_IRQ_EN_SHIFT       28
#define IMX_APBH_CTRL1_CH12_CMDCMPLT_IRQ_EN_MASK        0x10000000
#define IMX_APBH_CTRL1_CH12_CMDCMPLT_IRQ_EN(v)          (((v) << 28) & IMX_APBH_CTRL1_CH12_CMDCMPLT_IRQ_EN_MASK)

/* CH11_CMDCMPLT_IRQ_EN */
#define IMX_APBH_CTRL1_CH11_CMDCMPLT_IRQ_EN_SHIFT       27
#define IMX_APBH_CTRL1_CH11_CMDCMPLT_IRQ_EN_MASK        0x08000000
#define IMX_APBH_CTRL1_CH11_CMDCMPLT_IRQ_EN(v)          (((v) << 27) & IMX_APBH_CTRL1_CH11_CMDCMPLT_IRQ_EN_MASK)

/* CH10_CMDCMPLT_IRQ_EN */
#define IMX_APBH_CTRL1_CH10_CMDCMPLT_IRQ_EN_SHIFT       26
#define IMX_APBH_CTRL1_CH10_CMDCMPLT_IRQ_EN_MASK        0x04000000
#define IMX_APBH_CTRL1_CH10_CMDCMPLT_IRQ_EN(v)          (((v) << 26) & IMX_APBH_CTRL1_CH10_CMDCMPLT_IRQ_EN_MASK)

/* CH9_CMDCMPLT_IRQ_EN */
#define IMX_APBH_CTRL1_CH9_CMDCMPLT_IRQ_EN_SHIFT        25
#define IMX_APBH_CTRL1_CH9_CMDCMPLT_IRQ_EN_MASK         0x02000000
#define IMX_APBH_CTRL1_CH9_CMDCMPLT_IRQ_EN(v)           (((v) << 25) & IMX_APBH_CTRL1_CH9_CMDCMPLT_IRQ_EN_MASK)

/* CH8_CMDCMPLT_IRQ_EN */
#define IMX_APBH_CTRL1_CH8_CMDCMPLT_IRQ_EN_SHIFT        24
#define IMX_APBH_CTRL1_CH8_CMDCMPLT_IRQ_EN_MASK         0x01000000
#define IMX_APBH_CTRL1_CH8_CMDCMPLT_IRQ_EN(v)           (((v) << 24) & IMX_APBH_CTRL1_CH8_CMDCMPLT_IRQ_EN_MASK)

/* CH7_CMDCMPLT_IRQ_EN */
#define IMX_APBH_CTRL1_CH7_CMDCMPLT_IRQ_EN_SHIFT        23
#define IMX_APBH_CTRL1_CH7_CMDCMPLT_IRQ_EN_MASK         0x00800000
#define IMX_APBH_CTRL1_CH7_CMDCMPLT_IRQ_EN(v)           (((v) << 23) & IMX_APBH_CTRL1_CH7_CMDCMPLT_IRQ_EN_MASK)

/* CH6_CMDCMPLT_IRQ_EN */
#define IMX_APBH_CTRL1_CH6_CMDCMPLT_IRQ_EN_SHIFT        22
#define IMX_APBH_CTRL1_CH6_CMDCMPLT_IRQ_EN_MASK         0x00400000
#define IMX_APBH_CTRL1_CH6_CMDCMPLT_IRQ_EN(v)           (((v) << 22) & IMX_APBH_CTRL1_CH6_CMDCMPLT_IRQ_EN_MASK)

/* CH5_CMDCMPLT_IRQ_EN */
#define IMX_APBH_CTRL1_CH5_CMDCMPLT_IRQ_EN_SHIFT        21
#define IMX_APBH_CTRL1_CH5_CMDCMPLT_IRQ_EN_MASK         0x00200000
#define IMX_APBH_CTRL1_CH5_CMDCMPLT_IRQ_EN(v)           (((v) << 21) & IMX_APBH_CTRL1_CH5_CMDCMPLT_IRQ_EN_MASK)

/* CH4_CMDCMPLT_IRQ_EN */
#define IMX_APBH_CTRL1_CH4_CMDCMPLT_IRQ_EN_SHIFT        20
#define IMX_APBH_CTRL1_CH4_CMDCMPLT_IRQ_EN_MASK         0x00100000
#define IMX_APBH_CTRL1_CH4_CMDCMPLT_IRQ_EN(v)           (((v) << 20) & IMX_APBH_CTRL1_CH4_CMDCMPLT_IRQ_EN_MASK)

/* CH3_CMDCMPLT_IRQ_EN */
#define IMX_APBH_CTRL1_CH3_CMDCMPLT_IRQ_EN_SHIFT        19
#define IMX_APBH_CTRL1_CH3_CMDCMPLT_IRQ_EN_MASK         0x00080000
#define IMX_APBH_CTRL1_CH3_CMDCMPLT_IRQ_EN(v)           (((v) << 19) & IMX_APBH_CTRL1_CH3_CMDCMPLT_IRQ_EN_MASK)

/* CH2_CMDCMPLT_IRQ_EN */
#define IMX_APBH_CTRL1_CH2_CMDCMPLT_IRQ_EN_SHIFT        18
#define IMX_APBH_CTRL1_CH2_CMDCMPLT_IRQ_EN_MASK         0x00040000
#define IMX_APBH_CTRL1_CH2_CMDCMPLT_IRQ_EN(v)           (((v) << 18) & IMX_APBH_CTRL1_CH2_CMDCMPLT_IRQ_EN_MASK)

/* CH1_CMDCMPLT_IRQ_EN */
#define IMX_APBH_CTRL1_CH1_CMDCMPLT_IRQ_EN_SHIFT        17
#define IMX_APBH_CTRL1_CH1_CMDCMPLT_IRQ_EN_MASK         0x00020000
#define IMX_APBH_CTRL1_CH1_CMDCMPLT_IRQ_EN(v)           (((v) << 17) & IMX_APBH_CTRL1_CH1_CMDCMPLT_IRQ_EN_MASK)

/* CH0_CMDCMPLT_IRQ_EN */
#define IMX_APBH_CTRL1_CH0_CMDCMPLT_IRQ_EN_SHIFT        16
#define IMX_APBH_CTRL1_CH0_CMDCMPLT_IRQ_EN_MASK         0x00010000
#define IMX_APBH_CTRL1_CH0_CMDCMPLT_IRQ_EN(v)           (((v) << 16) & IMX_APBH_CTRL1_CH0_CMDCMPLT_IRQ_EN_MASK)

/* CH15_CMDCMPLT_IRQ */
#define IMX_APBH_CTRL1_CH15_CMDCMPLT_IRQ_SHIFT          15
#define IMX_APBH_CTRL1_CH15_CMDCMPLT_IRQ_MASK           0x00008000
#define IMX_APBH_CTRL1_CH15_CMDCMPLT_IRQ(v)             (((v) << 15) & IMX_APBH_CTRL1_CH15_CMDCMPLT_IRQ_MASK)

/* CH14_CMDCMPLT_IRQ */
#define IMX_APBH_CTRL1_CH14_CMDCMPLT_IRQ_SHIFT          14
#define IMX_APBH_CTRL1_CH14_CMDCMPLT_IRQ_MASK           0x00004000
#define IMX_APBH_CTRL1_CH14_CMDCMPLT_IRQ(v)             (((v) << 14) & IMX_APBH_CTRL1_CH14_CMDCMPLT_IRQ_MASK)

/* CH13_CMDCMPLT_IRQ */
#define IMX_APBH_CTRL1_CH13_CMDCMPLT_IRQ_SHIFT          13
#define IMX_APBH_CTRL1_CH13_CMDCMPLT_IRQ_MASK           0x00002000
#define IMX_APBH_CTRL1_CH13_CMDCMPLT_IRQ(v)             (((v) << 13) & IMX_APBH_CTRL1_CH13_CMDCMPLT_IRQ_MASK)

/* CH12_CMDCMPLT_IRQ */
#define IMX_APBH_CTRL1_CH12_CMDCMPLT_IRQ_SHIFT          12
#define IMX_APBH_CTRL1_CH12_CMDCMPLT_IRQ_MASK           0x00001000
#define IMX_APBH_CTRL1_CH12_CMDCMPLT_IRQ(v)             (((v) << 12) & IMX_APBH_CTRL1_CH12_CMDCMPLT_IRQ_MASK)

/* CH11_CMDCMPLT_IRQ */
#define IMX_APBH_CTRL1_CH11_CMDCMPLT_IRQ_SHIFT          11
#define IMX_APBH_CTRL1_CH11_CMDCMPLT_IRQ_MASK           0x00000800
#define IMX_APBH_CTRL1_CH11_CMDCMPLT_IRQ(v)             (((v) << 11) & IMX_APBH_CTRL1_CH11_CMDCMPLT_IRQ_MASK)

/* CH10_CMDCMPLT_IRQ */
#define IMX_APBH_CTRL1_CH10_CMDCMPLT_IRQ_SHIFT          10
#define IMX_APBH_CTRL1_CH10_CMDCMPLT_IRQ_MASK           0x00000400
#define IMX_APBH_CTRL1_CH10_CMDCMPLT_IRQ(v)             (((v) << 10) & IMX_APBH_CTRL1_CH10_CMDCMPLT_IRQ_MASK)

/* CH9_CMDCMPLT_IRQ */
#define IMX_APBH_CTRL1_CH9_CMDCMPLT_IRQ_SHIFT           9
#define IMX_APBH_CTRL1_CH9_CMDCMPLT_IRQ_MASK            0x00000200
#define IMX_APBH_CTRL1_CH9_CMDCMPLT_IRQ(v)              (((v) << 9) & IMX_APBH_CTRL1_CH9_CMDCMPLT_IRQ_MASK)

/* CH8_CMDCMPLT_IRQ */
#define IMX_APBH_CTRL1_CH8_CMDCMPLT_IRQ_SHIFT           8
#define IMX_APBH_CTRL1_CH8_CMDCMPLT_IRQ_MASK            0x00000100
#define IMX_APBH_CTRL1_CH8_CMDCMPLT_IRQ(v)              (((v) << 8) & IMX_APBH_CTRL1_CH8_CMDCMPLT_IRQ_MASK)

/* CH7_CMDCMPLT_IRQ */
#define IMX_APBH_CTRL1_CH7_CMDCMPLT_IRQ_SHIFT           7
#define IMX_APBH_CTRL1_CH7_CMDCMPLT_IRQ_MASK            0x00000080
#define IMX_APBH_CTRL1_CH7_CMDCMPLT_IRQ(v)              (((v) << 7) & IMX_APBH_CTRL1_CH7_CMDCMPLT_IRQ_MASK)

/* CH6_CMDCMPLT_IRQ */
#define IMX_APBH_CTRL1_CH6_CMDCMPLT_IRQ_SHIFT           6
#define IMX_APBH_CTRL1_CH6_CMDCMPLT_IRQ_MASK            0x00000040
#define IMX_APBH_CTRL1_CH6_CMDCMPLT_IRQ(v)              (((v) << 6) & IMX_APBH_CTRL1_CH6_CMDCMPLT_IRQ_MASK)

/* CH5_CMDCMPLT_IRQ */
#define IMX_APBH_CTRL1_CH5_CMDCMPLT_IRQ_SHIFT           5
#define IMX_APBH_CTRL1_CH5_CMDCMPLT_IRQ_MASK            0x00000020
#define IMX_APBH_CTRL1_CH5_CMDCMPLT_IRQ(v)              (((v) << 5) & IMX_APBH_CTRL1_CH5_CMDCMPLT_IRQ_MASK)

/* CH4_CMDCMPLT_IRQ */
#define IMX_APBH_CTRL1_CH4_CMDCMPLT_IRQ_SHIFT           4
#define IMX_APBH_CTRL1_CH4_CMDCMPLT_IRQ_MASK            0x00000010
#define IMX_APBH_CTRL1_CH4_CMDCMPLT_IRQ(v)              (((v) << 4) & IMX_APBH_CTRL1_CH4_CMDCMPLT_IRQ_MASK)

/* CH3_CMDCMPLT_IRQ */
#define IMX_APBH_CTRL1_CH3_CMDCMPLT_IRQ_SHIFT           3
#define IMX_APBH_CTRL1_CH3_CMDCMPLT_IRQ_MASK            0x00000008
#define IMX_APBH_CTRL1_CH3_CMDCMPLT_IRQ(v)              (((v) << 3) & IMX_APBH_CTRL1_CH3_CMDCMPLT_IRQ_MASK)

/* CH2_CMDCMPLT_IRQ */
#define IMX_APBH_CTRL1_CH2_CMDCMPLT_IRQ_SHIFT           2
#define IMX_APBH_CTRL1_CH2_CMDCMPLT_IRQ_MASK            0x00000004
#define IMX_APBH_CTRL1_CH2_CMDCMPLT_IRQ(v)              (((v) << 2) & IMX_APBH_CTRL1_CH2_CMDCMPLT_IRQ_MASK)

/* CH1_CMDCMPLT_IRQ */
#define IMX_APBH_CTRL1_CH1_CMDCMPLT_IRQ_SHIFT           1
#define IMX_APBH_CTRL1_CH1_CMDCMPLT_IRQ_MASK            0x00000002
#define IMX_APBH_CTRL1_CH1_CMDCMPLT_IRQ(v)              (((v) << 1) & IMX_APBH_CTRL1_CH1_CMDCMPLT_IRQ_MASK)

/* CH0_CMDCMPLT_IRQ */
#define IMX_APBH_CTRL1_CH0_CMDCMPLT_IRQ_SHIFT           0
#define IMX_APBH_CTRL1_CH0_CMDCMPLT_IRQ_MASK            0x00000001
#define IMX_APBH_CTRL1_CH0_CMDCMPLT_IRQ(v)              (((v) << 0) & IMX_APBH_CTRL1_CH0_CMDCMPLT_IRQ_MASK)

/*
 * IMX_APBH_CTRL2 - AHB to APBH Bridge Control and Status Register 2
 */
typedef union {
    uint32_t U;
    struct {
        unsigned CH0_ERROR_IRQ:1;
        unsigned CH1_ERROR_IRQ:1;
        unsigned CH2_ERROR_IRQ:1;
        unsigned CH3_ERROR_IRQ:1;
        unsigned CH4_ERROR_IRQ:1;
        unsigned CH5_ERROR_IRQ:1;
        unsigned CH6_ERROR_IRQ:1;
        unsigned CH7_ERROR_IRQ:1;
        unsigned CH8_ERROR_IRQ:1;
        unsigned CH9_ERROR_IRQ:1;
        unsigned CH10_ERROR_IRQ:1;
        unsigned CH11_ERROR_IRQ:1;
        unsigned CH12_ERROR_IRQ:1;
        unsigned CH13_ERROR_IRQ:1;
        unsigned CH14_ERROR_IRQ:1;
        unsigned CH15_ERROR_IRQ:1;
        unsigned CH0_ERROR_STATUS:1;
        unsigned CH1_ERROR_STATUS:1;
        unsigned CH2_ERROR_STATUS:1;
        unsigned CH3_ERROR_STATUS:1;
        unsigned CH4_ERROR_STATUS:1;
        unsigned CH5_ERROR_STATUS:1;
        unsigned CH6_ERROR_STATUS:1;
        unsigned CH7_ERROR_STATUS:1;
        unsigned CH8_ERROR_STATUS:1;
        unsigned CH9_ERROR_STATUS:1;
        unsigned CH10_ERROR_STATUS:1;
        unsigned CH11_ERROR_STATUS:1;
        unsigned CH12_ERROR_STATUS:1;
        unsigned CH13_ERROR_STATUS:1;
        unsigned CH14_ERROR_STATUS:1;
        unsigned CH15_ERROR_STATUS:1;
    } B;
} imx_apbh_ctrl2_t;

#define IMX_APBH_CTRL2_OFFSET                           (0x20)
#define IMX_APBH_CTRL2_SET_OFFSET                       (0x24)
#define IMX_APBH_CTRL2_CLR_OFFSET                       (0x28)
#define IMX_APBH_CTRL2_TOG_OFFSET                       (0x2C)

/* IMX_APBH_CTRL2 bit-fields */
/* CH15_ERROR_STATUS */
#define IMX_APBH_CTRL2_CH15_ERROR_STATUS_SHIFT          31
#define IMX_APBH_CTRL2_CH15_ERROR_STATUS_MASK           0x80000000
#define IMX_APBH_CTRL2_CH15_ERROR_STATUS(v)             ((((uint32_t) v) << 31) & IMX_APBH_CTRL2_CH15_ERROR_STATUS_MASK)

#define IMX_APBH_CTRL2_CH15_ERROR_STATUS_BV_TERMINATION 0x0
#define IMX_APBH_CTRL2_CH15_ERROR_STATUS_BV_BUS_ERROR   0x1

/* CH14_ERROR_STATUS */
#define IMX_APBH_CTRL2_CH14_ERROR_STATUS_SHIFT          30
#define IMX_APBH_CTRL2_CH14_ERROR_STATUS_MASK           0x40000000
#define IMX_APBH_CTRL2_CH14_ERROR_STATUS(v)             (((v) << 30) & IMX_APBH_CTRL2_CH14_ERROR_STATUS_MASK)

#define IMX_APBH_CTRL2_CH14_ERROR_STATUS_BV_TERMINATION 0x0
#define IMX_APBH_CTRL2_CH14_ERROR_STATUS_BV_BUS_ERROR   0x1

/* CH13_ERROR_STATUS */
#define IMX_APBH_CTRL2_CH13_ERROR_STATUS_SHIFT          29
#define IMX_APBH_CTRL2_CH13_ERROR_STATUS_MASK           0x20000000
#define IMX_APBH_CTRL2_CH13_ERROR_STATUS(v)             (((v) << 29) & IMX_APBH_CTRL2_CH13_ERROR_STATUS_MASK)

#define IMX_APBH_CTRL2_CH13_ERROR_STATUS_BV_TERMINATION 0x0
#define IMX_APBH_CTRL2_CH13_ERROR_STATUS_BV_BUS_ERROR   0x1

/* CH12_ERROR_STATUS */
#define IMX_APBH_CTRL2_CH12_ERROR_STATUS_SHIFT          28
#define IMX_APBH_CTRL2_CH12_ERROR_STATUS_MASK           0x10000000
#define IMX_APBH_CTRL2_CH12_ERROR_STATUS(v)             (((v) << 28) & IMX_APBH_CTRL2_CH12_ERROR_STATUS_MASK)

#define IMX_APBH_CTRL2_CH12_ERROR_STATUS_BV_TERMINATION 0x0
#define IMX_APBH_CTRL2_CH12_ERROR_STATUS_BV_BUS_ERROR   0x1

/* CH11_ERROR_STATUS */
#define IMX_APBH_CTRL2_CH11_ERROR_STATUS_SHIFT          27
#define IMX_APBH_CTRL2_CH11_ERROR_STATUS_MASK           0x08000000
#define IMX_APBH_CTRL2_CH11_ERROR_STATUS(v)             (((v) << 27) & IMX_APBH_CTRL2_CH11_ERROR_STATUS_MASK)

#define IMX_APBH_CTRL2_CH11_ERROR_STATUS_BV_TERMINATION 0x0
#define IMX_APBH_CTRL2_CH11_ERROR_STATUS_BV_BUS_ERROR   0x1

/* CH10_ERROR_STATUS */
#define IMX_APBH_CTRL2_CH10_ERROR_STATUS_SHIFT          26
#define IMX_APBH_CTRL2_CH10_ERROR_STATUS_MASK           0x04000000
#define IMX_APBH_CTRL2_CH10_ERROR_STATUS(v)             (((v) << 26) & IMX_APBH_CTRL2_CH10_ERROR_STATUS_MASK)

#define IMX_APBH_CTRL2_CH10_ERROR_STATUS_BV_TERMINATION 0x0
#define IMX_APBH_CTRL2_CH10_ERROR_STATUS_BV_BUS_ERROR   0x1

/* CH9_ERROR_STATUS */
#define IMX_APBH_CTRL2_CH9_ERROR_STATUS_SHIFT           25
#define IMX_APBH_CTRL2_CH9_ERROR_STATUS_MASK            0x02000000
#define IMX_APBH_CTRL2_CH9_ERROR_STATUS(v)              (((v) << 25) & IMX_APBH_CTRL2_CH9_ERROR_STATUS_MASK)

#define IMX_APBH_CTRL2_CH9_ERROR_STATUS_BV_TERMINATION  0x0
#define IMX_APBH_CTRL2_CH9_ERROR_STATUS_BV_BUS_ERROR    0x1

/* CH8_ERROR_STATUS */
#define IMX_APBH_CTRL2_CH8_ERROR_STATUS_SHIFT           24
#define IMX_APBH_CTRL2_CH8_ERROR_STATUS_MASK            0x01000000
#define IMX_APBH_CTRL2_CH8_ERROR_STATUS(v)              (((v) << 24) & IMX_APBH_CTRL2_CH8_ERROR_STATUS_MASK)

#define IMX_APBH_CTRL2_CH8_ERROR_STATUS_BV_TERMINATION  0x0
#define IMX_APBH_CTRL2_CH8_ERROR_STATUS_BV_BUS_ERROR    0x1

/* CH7_ERROR_STATUS */
#define IMX_APBH_CTRL2_CH7_ERROR_STATUS_SHIFT           23
#define IMX_APBH_CTRL2_CH7_ERROR_STATUS_MASK            0x00800000
#define IMX_APBH_CTRL2_CH7_ERROR_STATUS(v)              (((v) << 23) & IMX_APBH_CTRL2_CH7_ERROR_STATUS_MASK)

#define IMX_APBH_CTRL2_CH7_ERROR_STATUS_BV_TERMINATION  0x0
#define IMX_APBH_CTRL2_CH7_ERROR_STATUS_BV_BUS_ERROR    0x1

/* CH6_ERROR_STATUS */
#define IMX_APBH_CTRL2_CH6_ERROR_STATUS_SHIFT           22
#define IMX_APBH_CTRL2_CH6_ERROR_STATUS_MASK            0x00400000
#define IMX_APBH_CTRL2_CH6_ERROR_STATUS(v)              (((v) << 22) & IMX_APBH_CTRL2_CH6_ERROR_STATUS_MASK)

#define IMX_APBH_CTRL2_CH6_ERROR_STATUS_BV_TERMINATION  0x0
#define IMX_APBH_CTRL2_CH6_ERROR_STATUS_BV_BUS_ERROR    0x1

/* CH5_ERROR_STATUS */
#define IMX_APBH_CTRL2_CH5_ERROR_STATUS_SHIFT           21
#define IMX_APBH_CTRL2_CH5_ERROR_STATUS_MASK            0x00200000
#define IMX_APBH_CTRL2_CH5_ERROR_STATUS(v)              (((v) << 21) & IMX_APBH_CTRL2_CH5_ERROR_STATUS_MASK)

#define IMX_APBH_CTRL2_CH5_ERROR_STATUS_BV_TERMINATION  0x0
#define IMX_APBH_CTRL2_CH5_ERROR_STATUS_BV_BUS_ERROR    0x1

/* CH4_ERROR_STATUS */
#define IMX_APBH_CTRL2_CH4_ERROR_STATUS_SHIFT           20
#define IMX_APBH_CTRL2_CH4_ERROR_STATUS_MASK            0x00100000
#define IMX_APBH_CTRL2_CH4_ERROR_STATUS(v)              (((v) << 20) & IMX_APBH_CTRL2_CH4_ERROR_STATUS_MASK)

#define IMX_APBH_CTRL2_CH4_ERROR_STATUS_BV_TERMINATION  0x0
#define IMX_APBH_CTRL2_CH4_ERROR_STATUS_BV_BUS_ERROR    0x1

/* CH3_ERROR_STATUS */
#define IMX_APBH_CTRL2_CH3_ERROR_STATUS_SHIFT           19
#define IMX_APBH_CTRL2_CH3_ERROR_STATUS_MASK            0x00080000
#define IMX_APBH_CTRL2_CH3_ERROR_STATUS(v)              (((v) << 19) & IMX_APBH_CTRL2_CH3_ERROR_STATUS_MASK)

#define IMX_APBH_CTRL2_CH3_ERROR_STATUS_BV_TERMINATION  0x0
#define IMX_APBH_CTRL2_CH3_ERROR_STATUS_BV_BUS_ERROR    0x1

/* CH2_ERROR_STATUS */
#define IMX_APBH_CTRL2_CH2_ERROR_STATUS_SHIFT           18
#define IMX_APBH_CTRL2_CH2_ERROR_STATUS_MASK            0x00040000
#define IMX_APBH_CTRL2_CH2_ERROR_STATUS(v)              (((v) << 18) & IMX_APBH_CTRL2_CH2_ERROR_STATUS_MASK)

#define IMX_APBH_CTRL2_CH2_ERROR_STATUS_BV_TERMINATION  0x0
#define IMX_APBH_CTRL2_CH2_ERROR_STATUS_BV_BUS_ERROR    0x1

/* CH1_ERROR_STATUS */
#define IMX_APBH_CTRL2_CH1_ERROR_STATUS_SHIFT           17
#define IMX_APBH_CTRL2_CH1_ERROR_STATUS_MASK            0x00020000
#define IMX_APBH_CTRL2_CH1_ERROR_STATUS(v)              (((v) << 17) & IMX_APBH_CTRL2_CH1_ERROR_STATUS_MASK)

#define IMX_APBH_CTRL2_CH1_ERROR_STATUS_BV_TERMINATION  0x0
#define IMX_APBH_CTRL2_CH1_ERROR_STATUS_BV_BUS_ERROR    0x1

/* CH0_ERROR_STATUS */
#define IMX_APBH_CTRL2_CH0_ERROR_STATUS_SHIFT           16
#define IMX_APBH_CTRL2_CH0_ERROR_STATUS_MASK            0x00010000
#define IMX_APBH_CTRL2_CH0_ERROR_STATUS(v)              (((v) << 16) & IMX_APBH_CTRL2_CH0_ERROR_STATUS_MASK)

#define IMX_APBH_CTRL2_CH0_ERROR_STATUS_BV_TERMINATION  0x0
#define IMX_APBH_CTRL2_CH0_ERROR_STATUS_BV_BUS_ERROR    0x1

/* CH15_ERROR_IRQ */
#define IMX_APBH_CTRL2_CH15_ERROR_IRQ_SHIFT             15
#define IMX_APBH_CTRL2_CH15_ERROR_IRQ_MASK              0x00008000
#define IMX_APBH_CTRL2_CH15_ERROR_IRQ(v)                (((v) << 15) & IMX_APBH_CTRL2_CH15_ERROR_IRQ_MASK)

/* CH14_ERROR_IRQ */
#define IMX_APBH_CTRL2_CH14_ERROR_IRQ_SHIFT             14
#define IMX_APBH_CTRL2_CH14_ERROR_IRQ_MASK              0x00004000
#define IMX_APBH_CTRL2_CH14_ERROR_IRQ(v)                (((v) << 14) & IMX_APBH_CTRL2_CH14_ERROR_IRQ_MASK)

/* CH13_ERROR_IRQ */
#define IMX_APBH_CTRL2_CH13_ERROR_IRQ_SHIFT             13
#define IMX_APBH_CTRL2_CH13_ERROR_IRQ_MASK              0x00002000
#define IMX_APBH_CTRL2_CH13_ERROR_IRQ(v)                (((v) << 13) & IMX_APBH_CTRL2_CH13_ERROR_IRQ_MASK)
#define BW_APBH_CTRL2_CH13_ERROR_IRQ(v)                 BF_CS1(APBH_CTRL2, CH13_ERROR_IRQ, v)

/* CH12_ERROR_IRQ */
#define IMX_APBH_CTRL2_CH12_ERROR_IRQ_SHIFT             12
#define IMX_APBH_CTRL2_CH12_ERROR_IRQ_MASK              0x00001000
#define IMX_APBH_CTRL2_CH12_ERROR_IRQ(v)                (((v) << 12) & IMX_APBH_CTRL2_CH12_ERROR_IRQ_MASK)

/* CH11_ERROR_IRQ */
#define IMX_APBH_CTRL2_CH11_ERROR_IRQ_SHIFT             11
#define IMX_APBH_CTRL2_CH11_ERROR_IRQ_MASK              0x00000800
#define IMX_APBH_CTRL2_CH11_ERROR_IRQ(v)                (((v) << 11) & IMX_APBH_CTRL2_CH11_ERROR_IRQ_MASK)

/* CH10_ERROR_IRQ */
#define IMX_APBH_CTRL2_CH10_ERROR_IRQ_SHIFT             10
#define IMX_APBH_CTRL2_CH10_ERROR_IRQ_MASK              0x00000400
#define IMX_APBH_CTRL2_CH10_ERROR_IRQ(v)                (((v) << 10) & IMX_APBH_CTRL2_CH10_ERROR_IRQ_MASK)

/* CH9_ERROR_IRQ */
#define IMX_APBH_CTRL2_CH9_ERROR_IRQ_SHIFT              9
#define IMX_APBH_CTRL2_CH9_ERROR_IRQ_MASK               0x00000200
#define IMX_APBH_CTRL2_CH9_ERROR_IRQ(v)                 (((v) << 9) & IMX_APBH_CTRL2_CH9_ERROR_IRQ_MASK)

/* CH8_ERROR_IRQ */
#define IMX_APBH_CTRL2_CH8_ERROR_IRQ_SHIFT              8
#define IMX_APBH_CTRL2_CH8_ERROR_IRQ_MASK               0x00000100
#define IMX_APBH_CTRL2_CH8_ERROR_IRQ(v)                 (((v) << 8) & IMX_APBH_CTRL2_CH8_ERROR_IRQ_MASK)

/* CH7_ERROR_IRQ */
#define IMX_APBH_CTRL2_CH7_ERROR_IRQ_SHIFT              7
#define IMX_APBH_CTRL2_CH7_ERROR_IRQ_MASK               0x00000080
#define IMX_APBH_CTRL2_CH7_ERROR_IRQ(v)                 (((v) << 7) & IMX_APBH_CTRL2_CH7_ERROR_IRQ_MASK)

/* CH6_ERROR_IRQ */
#define IMX_APBH_CTRL2_CH6_ERROR_IRQ_SHIFT              6
#define IMX_APBH_CTRL2_CH6_ERROR_IRQ_MASK               0x00000040
#define IMX_APBH_CTRL2_CH6_ERROR_IRQ(v)                 (((v) << 6) & IMX_APBH_CTRL2_CH6_ERROR_IRQ_MASK)

/* CH5_ERROR_IRQ */
#define IMX_APBH_CTRL2_CH5_ERROR_IRQ_SHIFT              5
#define IMX_APBH_CTRL2_CH5_ERROR_IRQ_MASK               0x00000020
#define IMX_APBH_CTRL2_CH5_ERROR_IRQ(v)                 (((v) << 5) & IMX_APBH_CTRL2_CH5_ERROR_IRQ_MASK)

/* CH4_ERROR_IRQ */
#define IMX_APBH_CTRL2_CH4_ERROR_IRQ_SHIFT              4
#define IMX_APBH_CTRL2_CH4_ERROR_IRQ_MASK               0x00000010
#define IMX_APBH_CTRL2_CH4_ERROR_IRQ(v)                 (((v) << 4) & IMX_APBH_CTRL2_CH4_ERROR_IRQ_MASK)

/* CH3_ERROR_IRQ */
#define IMX_APBH_CTRL2_CH3_ERROR_IRQ_SHIFT              3
#define IMX_APBH_CTRL2_CH3_ERROR_IRQ_MASK               0x00000008
#define IMX_APBH_CTRL2_CH3_ERROR_IRQ(v)                 (((v) << 3) & IMX_APBH_CTRL2_CH3_ERROR_IRQ_MASK)

/* CH2_ERROR_IRQ */
#define IMX_APBH_CTRL2_CH2_ERROR_IRQ_SHIFT              2
#define IMX_APBH_CTRL2_CH2_ERROR_IRQ_MASK               0x00000004
#define IMX_APBH_CTRL2_CH2_ERROR_IRQ(v)                 (((v) << 2) & IMX_APBH_CTRL2_CH2_ERROR_IRQ_MASK)

/* CH1_ERROR_IRQ */
#define IMX_APBH_CTRL2_CH1_ERROR_IRQ_SHIFT              1
#define IMX_APBH_CTRL2_CH1_ERROR_IRQ_MASK               0x00000002
#define IMX_APBH_CTRL2_CH1_ERROR_IRQ(v)                 (((v) << 1) & IMX_APBH_CTRL2_CH1_ERROR_IRQ_MASK)

/* CH0_ERROR_IRQ */
#define IMX_APBH_CTRL2_CH0_ERROR_IRQ_SHIFT              0
#define IMX_APBH_CTRL2_CH0_ERROR_IRQ_MASK               0x00000001
#define IMX_APBH_CTRL2_CH0_ERROR_IRQ(v)                 (((v) << 0) & IMX_APBH_CTRL2_CH0_ERROR_IRQ_MASK)

/*
 * IMX_APBH_CHANNEL_CTRL - AHB to APBH Bridge Channel Register
 */
typedef union {
    uint32_t U;
    struct {
        unsigned FREEZE_CHANNEL:16;
        unsigned RESET_CHANNEL:16;
    } B;
} imx_apbh_channel_ctrl_t;

#define IMX_APBH_CHANNEL_CTRL_OFFSET                    (0x30)
#define IMX_APBH_CHANNEL_CTRL_SET_OFFSET                (0x34)
#define IMX_APBH_CHANNEL_CTRL_CLR_OFFSET                (0x38)
#define IMX_APBH_CHANNEL_CTRL_TOG_OFFSET                (0x3C)

/* IMX_APBH_CHANNEL_CTRL bit-fields */
/* RESET_CHANNEL */
#define IMX_APBH_CHANNEL_CTRL_RESET_CHANNEL_SHIFT       16
#define IMX_APBH_CHANNEL_CTRL_RESET_CHANNEL_MASK        0xFFFF0000
#define IMX_APBH_CHANNEL_CTRL_RESET_CHANNEL(v)          ((((uint32_t) v) << 16) & IMX_APBH_CHANNEL_CTRL_RESET_CHANNEL_MASK)

#define IMX_APBH_CHANNEL_CTRL_RESET_CHANNEL_BV_NAND0    0x0001
#define IMX_APBH_CHANNEL_CTRL_RESET_CHANNEL_BV_NAND1    0x0002
#define IMX_APBH_CHANNEL_CTRL_RESET_CHANNEL_BV_NAND2    0x0004
#define IMX_APBH_CHANNEL_CTRL_RESET_CHANNEL_BV_NAND3    0x0008
#define IMX_APBH_CHANNEL_CTRL_RESET_CHANNEL_BV_NAND4    0x0010
#define IMX_APBH_CHANNEL_CTRL_RESET_CHANNEL_BV_NAND5    0x0020
#define IMX_APBH_CHANNEL_CTRL_RESET_CHANNEL_BV_NAND6    0x0040
#define IMX_APBH_CHANNEL_CTRL_RESET_CHANNEL_BV_NAND7    0x0080
#define IMX_APBH_CHANNEL_CTRL_RESET_CHANNEL_BV_SSP      0x0100

/* FREEZE_CHANNEL */
#define IMX_APBH_CHANNEL_CTRL_FREEZE_CHANNEL_SHIFT      0
#define IMX_APBH_CHANNEL_CTRL_FREEZE_CHANNEL_MASK       0x0000FFFF
#define IMX_APBH_CHANNEL_CTRL_FREEZE_CHANNEL(v)         (((v) << 0) & IMX_APBH_CHANNEL_CTRL_FREEZE_CHANNEL_MASK)

#define IMX_APBH_CHANNEL_CTRL_FREEZE_CHANNEL_BV_NAND0   0x0001
#define IMX_APBH_CHANNEL_CTRL_FREEZE_CHANNEL_BV_NAND1   0x0002
#define IMX_APBH_CHANNEL_CTRL_FREEZE_CHANNEL_BV_NAND2   0x0004
#define IMX_APBH_CHANNEL_CTRL_FREEZE_CHANNEL_BV_NAND3   0x0008
#define IMX_APBH_CHANNEL_CTRL_FREEZE_CHANNEL_BV_NAND4   0x0010
#define IMX_APBH_CHANNEL_CTRL_FREEZE_CHANNEL_BV_NAND5   0x0020
#define IMX_APBH_CHANNEL_CTRL_FREEZE_CHANNEL_BV_NAND6   0x0040
#define IMX_APBH_CHANNEL_CTRL_FREEZE_CHANNEL_BV_NAND7   0x0080
#define IMX_APBH_CHANNEL_CTRL_FREEZE_CHANNEL_BV_SSP     0x0100

/*
 * IMX_APBH_DEVSEL - AHB to APBH DMA Device Assignment Register
 */
typedef union {
    uint32_t U;
    struct {
        unsigned CH0:2;
        unsigned CH1:2;
        unsigned CH2:2;
        unsigned CH3:2;
        unsigned CH4:2;
        unsigned CH5:2;
        unsigned CH6:2;
        unsigned CH7:2;
        unsigned CH8:2;
        unsigned CH9:2;
        unsigned CH10:2;
        unsigned CH11:2;
        unsigned CH12:2;
        unsigned CH13:2;
        unsigned CH14:2;
        unsigned CH15:2;
    } B;
} imx_apbh_devsel_t;

#define IMX_APBH_DEVSEL_OFFSET                          (0x40)

/* IMX_APBH_DEVSEL bit-fields */
/* CH15 */
#define IMX_APBH_DEVSEL_CH15_SHIFT                      30
#define IMX_APBH_DEVSEL_CH15_MASK                       0xC0000000
#define IMX_APBH_DEVSEL_CH15(v)                         ((((uint32_t) v) << 30) & IMX_APBH_DEVSEL_CH15_MASK)

/* CH14 */
#define IMX_APBH_DEVSEL_CH14_SHIFT                      28
#define IMX_APBH_DEVSEL_CH14_MASK                       0x30000000
#define IMX_APBH_DEVSEL_CH14(v)                         (((v) << 28) & IMX_APBH_DEVSEL_CH14_MASK)

/* CH13 */
#define IMX_APBH_DEVSEL_CH13_SHIFT                      26
#define IMX_APBH_DEVSEL_CH13_MASK                       0x0C000000
#define IMX_APBH_DEVSEL_CH13(v)                         (((v) << 26) & IMX_APBH_DEVSEL_CH13_MASK)

/* CH12 */
#define IMX_APBH_DEVSEL_CH12_SHIFT                      24
#define IMX_APBH_DEVSEL_CH12_MASK                       0x03000000
#define IMX_APBH_DEVSEL_CH12(v)                         (((v) << 24) & IMX_APBH_DEVSEL_CH12_MASK)

/* CH11 */
#define IMX_APBH_DEVSEL_CH11_SHIFT                      22
#define IMX_APBH_DEVSEL_CH11_MASK                       0x00C00000
#define IMX_APBH_DEVSEL_CH11(v)                         (((v) << 22) & IMX_APBH_DEVSEL_CH11_MASK)

/* CH10 */
#define IMX_APBH_DEVSEL_CH10_SHIFT                      20
#define IMX_APBH_DEVSEL_CH10_MASK                       0x00300000
#define IMX_APBH_DEVSEL_CH10(v)                         (((v) << 20) & IMX_APBH_DEVSEL_CH10_MASK)

/* CH9 */
#define IMX_APBH_DEVSEL_CH9_SHIFT                       18
#define IMX_APBH_DEVSEL_CH9_MASK                        0x000C0000
#define IMX_APBH_DEVSEL_CH9(v)                          (((v) << 18) & IMX_APBH_DEVSEL_CH9_MASK)

/* CH8 */
#define IMX_APBH_DEVSEL_CH8_SHIFT                       16
#define IMX_APBH_DEVSEL_CH8_MASK                        0x00030000
#define IMX_APBH_DEVSEL_CH8(v)                          (((v) << 16) & IMX_APBH_DEVSEL_CH8_MASK)

/* CH7 */
#define IMX_APBH_DEVSEL_CH7_SHIFT                       14
#define IMX_APBH_DEVSEL_CH7_MASK                        0x0000C000
#define IMX_APBH_DEVSEL_CH7(v)                          (((v) << 14) & IMX_APBH_DEVSEL_CH7_MASK)

/* CH6 */
#define IMX_APBH_DEVSEL_CH6_SHIFT                       12
#define IMX_APBH_DEVSEL_CH6_MASK                        0x00003000
#define IMX_APBH_DEVSEL_CH6(v)                          (((v) << 12) & IMX_APBH_DEVSEL_CH6_MASK)

/* CH5 */
#define IMX_APBH_DEVSEL_CH5_SHIFT                       10
#define IMX_APBH_DEVSEL_CH5_MASK                        0x00000C00
#define IMX_APBH_DEVSEL_CH5(v)                          (((v) << 10) & IMX_APBH_DEVSEL_CH5_MASK)

/* CH4 */
#define IMX_APBH_DEVSEL_CH4_SHIFT                       8
#define IMX_APBH_DEVSEL_CH4_MASK                        0x00000300
#define IMX_APBH_DEVSEL_CH4(v)                          (((v) << 8) & IMX_APBH_DEVSEL_CH4_MASK)

/* CH3 */
#define IMX_APBH_DEVSEL_CH3_SHIFT                       6
#define IMX_APBH_DEVSEL_CH3_MASK                        0x000000C0
#define IMX_APBH_DEVSEL_CH3(v)                          (((v) << 6) & IMX_APBH_DEVSEL_CH3_MASK)

/* CH2 */
#define IMX_APBH_DEVSEL_CH2_SHIFT                       4
#define IMX_APBH_DEVSEL_CH2_MASK                        0x00000030
#define IMX_APBH_DEVSEL_CH2(v)                          (((v) << 4) & IMX_APBH_DEVSEL_CH2_MASK)

/* CH1 */
#define IMX_APBH_DEVSEL_CH1_SHIFT                       2
#define IMX_APBH_DEVSEL_CH1_MASK                        0x0000000C
#define IMX_APBH_DEVSEL_CH1(v)                          (((v) << 2) & IMX_APBH_DEVSEL_CH1_MASK)

/* CH0 */
#define IMX_APBH_DEVSEL_CH0_SHIFT                       0
#define IMX_APBH_DEVSEL_CH0_MASK                        0x00000003
#define IMX_APBH_DEVSEL_CH0(v)                          (((v) << 0) & IMX_APBH_DEVSEL_CH0_MASK)

/*
 * IMX_APBH_DMA_BURST_SIZE - AHB to APBH DMA burst size
 */
typedef union {
    uint32_t U;
    struct {
        unsigned CH0:2;
        unsigned CH1:2;
        unsigned CH2:2;
        unsigned CH3:2;
        unsigned CH4:2;
        unsigned CH5:2;
        unsigned CH6:2;
        unsigned CH7:2;
        unsigned CH8:2;
        unsigned CH9:2;
        unsigned CH10:2;
        unsigned CH11:2;
        unsigned CH12:2;
        unsigned CH13:2;
        unsigned CH14:2;
        unsigned CH15:2;
    } B;
} imx_apbh_dma_burst_size_t;

#define IMX_APBH_DMA_BURST_SIZE_OFFSET          (0x50)

/* IMX_APBH_DMA_BURST_SIZE bit-fields */
/* CH15 */
#define IMX_APBH_DMA_BURST_SIZE_CH15_SHIFT      30
#define IMX_APBH_DMA_BURST_SIZE_CH15_MASK       0xC0000000
#define IMX_APBH_DMA_BURST_SIZE_CH15(v)         ((((uint32_t) v) << 30) & IMX_APBH_DMA_BURST_SIZE_CH15_MASK)

/* CH14 */
#define IMX_APBH_DMA_BURST_SIZE_CH14_SHIFT      28
#define IMX_APBH_DMA_BURST_SIZE_CH14_MASK       0x30000000
#define IMX_APBH_DMA_BURST_SIZE_CH14(v)         (((v) << 28) & IMX_APBH_DMA_BURST_SIZE_CH14_MASK)

/* CH13 */
#define IMX_APBH_DMA_BURST_SIZE_CH13_SHIFT      26
#define IMX_APBH_DMA_BURST_SIZE_CH13_MASK       0x0C000000
#define IMX_APBH_DMA_BURST_SIZE_CH13(v)         (((v) << 26) & IMX_APBH_DMA_BURST_SIZE_CH13_MASK)

/* CH12 */
#define IMX_APBH_DMA_BURST_SIZE_CH12_SHIFT      24
#define IMX_APBH_DMA_BURST_SIZE_CH12_MASK       0x03000000
#define IMX_APBH_DMA_BURST_SIZE_CH12(v)         (((v) << 24) & IMX_APBH_DMA_BURST_SIZE_CH12_MASK)

/* CH11 */
#define IMX_APBH_DMA_BURST_SIZE_CH11_SHIFT      22
#define IMX_APBH_DMA_BURST_SIZE_CH11_MASK       0x00C00000
#define IMX_APBH_DMA_BURST_SIZE_CH11(v)         (((v) << 22) & IMX_APBH_DMA_BURST_SIZE_CH11_MASK)

/* CH10 */
#define IMX_APBH_DMA_BURST_SIZE_CH10_SHIFT      20
#define IMX_APBH_DMA_BURST_SIZE_CH10_MASK       0x00300000
#define IMX_APBH_DMA_BURST_SIZE_CH10(v)         (((v) << 20) & IMX_APBH_DMA_BURST_SIZE_CH10_MASK)

/* CH9 */
#define IMX_APBH_DMA_BURST_SIZE_CH9_SHIFT       18
#define IMX_APBH_DMA_BURST_SIZE_CH9_MASK        0x000C0000
#define IMX_APBH_DMA_BURST_SIZE_CH9(v)          (((v) << 18) & IMX_APBH_DMA_BURST_SIZE_CH9_MASK)

/* CH8 */
#define IMX_APBH_DMA_BURST_SIZE_CH8_SHIFT       16
#define IMX_APBH_DMA_BURST_SIZE_CH8_MASK        0x00030000
#define IMX_APBH_DMA_BURST_SIZE_CH8(v)          (((v) << 16) & IMX_APBH_DMA_BURST_SIZE_CH8_MASK)

#define IMX_APBH_DMA_BURST_SIZE_CH8_BV_BURST0   0x0
#define IMX_APBH_DMA_BURST_SIZE_CH8_BV_BURST4   0x1
#define IMX_APBH_DMA_BURST_SIZE_CH8_BV_BURST8   0x2

/* CH7 */
#define IMX_APBH_DMA_BURST_SIZE_CH7_SHIFT       14
#define IMX_APBH_DMA_BURST_SIZE_CH7_MASK        0x0000C000
#define IMX_APBH_DMA_BURST_SIZE_CH7(v)          (((v) << 14) & IMX_APBH_DMA_BURST_SIZE_CH7_MASK)

/* CH6 */
#define IMX_APBH_DMA_BURST_SIZE_CH6_SHIFT       12
#define IMX_APBH_DMA_BURST_SIZE_CH6_MASK        0x00003000
#define IMX_APBH_DMA_BURST_SIZE_CH6(v)          (((v) << 12) & IMX_APBH_DMA_BURST_SIZE_CH6_MASK)

/* CH5 */
#define IMX_APBH_DMA_BURST_SIZE_CH5_SHIFT       10
#define IMX_APBH_DMA_BURST_SIZE_CH5_MASK        0x00000C00
#define IMX_APBH_DMA_BURST_SIZE_CH5(v)          (((v) << 10) & IMX_APBH_DMA_BURST_SIZE_CH5_MASK)

/* CH4 */
#define IMX_APBH_DMA_BURST_SIZE_CH4_SHIFT       8
#define IMX_APBH_DMA_BURST_SIZE_CH4_MASK        0x00000300
#define IMX_APBH_DMA_BURST_SIZE_CH4(v)          (((v) << 8) & IMX_APBH_DMA_BURST_SIZE_CH4_MASK)

/* CH3 */
#define IMX_APBH_DMA_BURST_SIZE_CH3_SHIFT       6
#define IMX_APBH_DMA_BURST_SIZE_CH3_MASK        0x000000C0
#define IMX_APBH_DMA_BURST_SIZE_CH3(v)          (((v) << 6) & IMX_APBH_DMA_BURST_SIZE_CH3_MASK)

/* CH2 */
#define IMX_APBH_DMA_BURST_SIZE_CH2_SHIFT       4
#define IMX_APBH_DMA_BURST_SIZE_CH2_MASK        0x00000030
#define IMX_APBH_DMA_BURST_SIZE_CH2(v)          (((v) << 4) & IMX_APBH_DMA_BURST_SIZE_CH2_MASK)

/* CH1 */
#define IMX_APBH_DMA_BURST_SIZE_CH1_SHIFT       2
#define IMX_APBH_DMA_BURST_SIZE_CH1_MASK        0x0000000C
#define IMX_APBH_DMA_BURST_SIZE_CH1(v)          (((v) << 2) & IMX_APBH_DMA_BURST_SIZE_CH1_MASK)

/* CH0 */
#define IMX_APBH_DMA_BURST_SIZE_CH0_SHIFT       0
#define IMX_APBH_DMA_BURST_SIZE_CH0_MASK        0x00000003
#define IMX_APBH_DMA_BURST_SIZE_CH0(v)          (((v) << 0) & IMX_APBH_DMA_BURST_SIZE_CH0_MASK)

/*
 * IMX_APBH_DEBUG - AHB to APBH DMA Debug Register
 */
typedef union {
    uint32_t U;
    struct {
        unsigned GPMI_ONE_FIFO:1;
        unsigned RSVD:31;
    } B;
} imx_apbh_debug_t;

#define IMX_APBH_DEBUG_OFFSET                   (0x60)

/* IMX_APBH_DEBUG bit-fields */
/* RSVD */
#define IMX_APBH_DEBUG_RSVD_SHIFT               1
#define IMX_APBH_DEBUG_RSVD_MASK                0xFFFFFFFE
#define IMX_APBH_DEBUG_RSVD(v)                  ((((uint32_t) v) << 1) & IMX_APBH_DEBUG_RSVD_MASK)

/* GPMI_ONE_FIFO */
#define IMX_APBH_DEBUG_GPMI_ONE_FIFO_SHIFT      0
#define IMX_APBH_DEBUG_GPMI_ONE_FIFO_MASK       0x00000001
#define IMX_APBH_DEBUG_GPMI_ONE_FIFO(v)         (((v) << 0) & IMX_APBH_DEBUG_GPMI_ONE_FIFO_MASK)

/*
 *  multi-register-define name IMX_APBH_CHn_CURCMDAR
 *              base 0x00000100
 *              count 16
 *              offset 0x70
 */
typedef union {
    uint32_t U;
    struct {
        unsigned CMD_ADDR:32;
    } B;
} imx_apbh_chn_curcmdar_t;

#define IMX_APBH_CHn_CURCMDAR_COUNT             16
#define IMX_APBH_CHn_CURCMDAR_OFFSET(n)         (0x100 + ((n) * 0x70))

/* IMX_APBH_CHn_CURCMDAR multi-register bit-fields */
/* CMD_ADDR */
#define IMX_APBH_CHn_CURCMDAR_CMD_ADDR_SHIFT    0
#define IMX_APBH_CHn_CURCMDAR_CMD_ADDR_MASK     0xFFFFFFFF
#define IMX_APBH_CHn_CURCMDAR_CMD_ADDR(v)       ((uint32_t) v)

/*
 *  multi-register-define name IMX_APBH_CHn_NXTCMDAR
 *              base 0x00000110
 *              count 16
 *              offset 0x70
 */
typedef union {
    uint32_t U;
    struct {
        unsigned CMD_ADDR:32;
    } B;
} imx_apbh_chn_nxtcmdar_t;

#define IMX_APBH_CHn_NXTCMDAR_COUNT             16
#define IMX_APBH_CHn_NXTCMDAR_OFFSET(n)         (0x110 + ((n) * 0x70))

/* IMX_APBH_CHn_NXTCMDAR multi-register bit-fields */
/* CMD_ADDR */
#define IMX_APBH_CHn_NXTCMDAR_CMD_ADDR_SHIFT    0
#define IMX_APBH_CHn_NXTCMDAR_CMD_ADDR_MASK     0xFFFFFFFF
#define IMX_APBH_CHn_NXTCMDAR_CMD_ADDR(v)       ((uint32_t) v)

/*
 *  multi-register-define name IMX_APBH_CHn_CMD
 *              base 0x00000120
 *              count 16
 *              offset 0x70
 */
typedef union {
    uint32_t U;
    struct {
        unsigned COMMAND:2;
        unsigned CHAIN:1;
        unsigned IRQONCMPLT:1;
        unsigned NANDLOCK:1;
        unsigned NANDWAIT4READY:1;
        unsigned SEMAPHORE:1;
        unsigned WAIT4ENDCMD:1;
        unsigned HALTONTERMINATE:1;
        unsigned RSVD1:3;
        unsigned CMDWORDS:4;
        unsigned XFER_COUNT:16;
    } B;
} imx_apbh_chn_cmd_t;

#define IMX_APBH_CHn_CMD_COUNT                  16
#define IMX_APBH_CHn_CMD_OFFSET(n)              (0x120 + ((n) * 0x70))

/* IMX_APBH_CHn_CMD multi-register bit-fields */
/* XFER_COUNT */
#define IMX_APBH_CHn_CMD_XFER_COUNT_SHIFT       16
#define IMX_APBH_CHn_CMD_XFER_COUNT_MASK        0xFFFF0000
#define IMX_APBH_CHn_CMD_XFER_COUNT(v)          ((((uint32_t) v) << 16) & IMX_APBH_CHn_CMD_XFER_COUNT_MASK)

/* CMDWORDS */
#define IMX_APBH_CHn_CMD_CMDWORDS_SHIFT         12
#define IMX_APBH_CHn_CMD_CMDWORDS_MASK          0x0000F000
#define IMX_APBH_CHn_CMD_CMDWORDS(v)            (((v) << 12) & IMX_APBH_CHn_CMD_CMDWORDS_MASK)

/* RSVD1 */
#define IMX_APBH_CHn_CMD_RSVD1_SHIFT            9
#define IMX_APBH_CHn_CMD_RSVD1_MASK             0x00000E00
#define IMX_APBH_CHn_CMD_RSVD1(v)               (((v) << 9) & IMX_APBH_CHn_CMD_RSVD1_MASK)

/* HALTONTERMINATE */
#define IMX_APBH_CHn_CMD_HALTONTERMINATE_SHIFT  8
#define IMX_APBH_CHn_CMD_HALTONTERMINATE_MASK   0x00000100
#define IMX_APBH_CHn_CMD_HALTONTERMINATE(v)     (((v) << 8) & IMX_APBH_CHn_CMD_HALTONTERMINATE_MASK)

/* WAIT4ENDCMD */
#define IMX_APBH_CHn_CMD_WAIT4ENDCMD_SHIFT      7
#define IMX_APBH_CHn_CMD_WAIT4ENDCMD_MASK       0x00000080
#define IMX_APBH_CHn_CMD_WAIT4ENDCMD(v)         (((v) << 7) & IMX_APBH_CHn_CMD_WAIT4ENDCMD_MASK)

/* SEMAPHORE */
#define IMX_APBH_CHn_CMD_SEMAPHORE_SHIFT        6
#define IMX_APBH_CHn_CMD_SEMAPHORE_MASK         0x00000040
#define IMX_APBH_CHn_CMD_SEMAPHORE(v)           (((v) << 6) & IMX_APBH_CHn_CMD_SEMAPHORE_MASK)

/* NANDWAIT4READY */
#define IMX_APBH_CHn_CMD_NANDWAIT4READY_SHIFT   5
#define IMX_APBH_CHn_CMD_NANDWAIT4READY_MASK    0x00000020
#define IMX_APBH_CHn_CMD_NANDWAIT4READY(v)      (((v) << 5) & IMX_APBH_CHn_CMD_NANDWAIT4READY_MASK)

/* NANDLOCK */
#define IMX_APBH_CHn_CMD_NANDLOCK_SHIFT         4
#define IMX_APBH_CHn_CMD_NANDLOCK_MASK          0x00000010
#define IMX_APBH_CHn_CMD_NANDLOCK(v)            (((v) << 4) & IMX_APBH_CHn_CMD_NANDLOCK_MASK)

/* IRQONCMPLT */
#define IMX_APBH_CHn_CMD_IRQONCMPLT_SHIFT       3
#define IMX_APBH_CHn_CMD_IRQONCMPLT_MASK        0x00000008
#define IMX_APBH_CHn_CMD_IRQONCMPLT(v)          (((v) << 3) & IMX_APBH_CHn_CMD_IRQONCMPLT_MASK)

/* CHAIN */
#define IMX_APBH_CHn_CMD_CHAIN_SHIFT            2
#define IMX_APBH_CHn_CMD_CHAIN_MASK             0x00000004
#define IMX_APBH_CHn_CMD_CHAIN(v)               (((v) << 2) & IMX_APBH_CHn_CMD_CHAIN_MASK)

/* COMMAND */
#define IMX_APBH_CHn_CMD_COMMAND_SHIFT          0
#define IMX_APBH_CHn_CMD_COMMAND_MASK           0x00000003
#define IMX_APBH_CHn_CMD_COMMAND(v)             (((v) << 0) & IMX_APBH_CHn_CMD_COMMAND_MASK)

#define IMX_APBH_CHn_CMD_COMMAND_BV_NO_DMA_XFER 0x0
#define IMX_APBH_CHn_CMD_COMMAND_BV_DMA_WRITE   0x1
#define IMX_APBH_CHn_CMD_COMMAND_BV_DMA_READ    0x2
#define IMX_APBH_CHn_CMD_COMMAND_BV_DMA_SENSE   0x3

/*
 *  multi-register-define name IMX_APBH_CHn_BAR
 *              base 0x00000130
 *              count 16
 *              offset 0x70
 */
typedef union {
    uint32_t U;
    struct {
        unsigned ADDRESS:32;
    } B;
} imx_apbh_chn_bar_t;

#define IMX_APBH_CHn_BAR_COUNT          16
#define IMX_APBH_CHn_BAR_OFFSET(n)      (0x130 + ((n) * 0x70))

/* IMX_APBH_CHn_BAR multi-register bit-fields */
/* ADDRESS */
#define IMX_APBH_CHn_BAR_ADDRESS_SHIFT  0
#define IMX_APBH_CHn_BAR_ADDRESS_MASK   0xFFFFFFFF
#define IMX_APBH_CHn_BAR_ADDRESS(v)     ((uint32_t) v)

/*
 *  multi-register-define name IMX_APBH_CHn_SEMA
 *              base 0x00000140
 *              count 16
 *              offset 0x70
 */
typedef union {
    uint32_t U;
    struct {
        unsigned INCREMENT_SEMA:8;
        unsigned RSVD1:8;
        unsigned PHORE:8;
        unsigned RSVD2:8;
    } B;
} imx_apbh_chn_sema_t;

#define IMX_APBH_CHn_SEMA_COUNT                 16
#define IMX_APBH_CHn_SEMA_OFFSET(n)             (0x140 + ((n) * 0x70))

/* IMX_APBH_CHn_SEMA multi-register bit-fields */
/* RSVD2 */
#define IMX_APBH_CHn_SEMA_RSVD2_SHIFT           24
#define IMX_APBH_CHn_SEMA_RSVD2_MASK            0xFF000000
#define IMX_APBH_CHn_SEMA_RSVD2(v)              ((((uint32_t) v) << 24) & IMX_APBH_CHn_SEMA_RSVD2_MASK)

/* PHORE */
#define IMX_APBH_CHn_SEMA_PHORE_SHIFT           16
#define IMX_APBH_CHn_SEMA_PHORE_MASK            0x00FF0000
#define IMX_APBH_CHn_SEMA_PHORE(v)              (((v) << 16) & IMX_APBH_CHn_SEMA_PHORE_MASK)

/* RSVD1 */
#define IMX_APBH_CHn_SEMA_RSVD1_SHIFT           8
#define IMX_APBH_CHn_SEMA_RSVD1_MASK            0x0000FF00
#define IMX_APBH_CHn_SEMA_RSVD1(v)              (((v) << 8) & IMX_APBH_CHn_SEMA_RSVD1_MASK)

/* INCREMENT_SEMA */
#define IMX_APBH_CHn_SEMA_INCREMENT_SEMA_SHIFT  0
#define IMX_APBH_CHn_SEMA_INCREMENT_SEMA_MASK   0x000000FF
#define IMX_APBH_CHn_SEMA_INCREMENT_SEMA(v)     (((v) << 0) & IMX_APBH_CHn_SEMA_INCREMENT_SEMA_MASK)

/*
 * IMX_APBH_VERSION - APBH Bridge Version Register
 */
typedef union {
    uint32_t U;
    struct {
        unsigned STEP:16;
        unsigned MINOR:8;
        unsigned MAJOR:8;
    } B;
} imx_apbh_version_t;

#define IMX_APBH_VERSION_OFFSET         (0x800)

/* IMX_APBH_VERSION bit-fields */
/* MAJOR */
#define IMX_APBH_VERSION_MAJOR_SHIFT    24
#define IMX_APBH_VERSION_MAJOR_MASK     0xFF000000
#define IMX_APBH_VERSION_MAJOR(v)       ((((uint32_t) v) << 24) & IMX_APBH_VERSION_MAJOR_MASK)

/* MINOR */
#define IMX_APBH_VERSION_MINOR_SHIFT    16
#define IMX_APBH_VERSION_MINOR_MASK     0x00FF0000
#define IMX_APBH_VERSION_MINOR(v)       (((v) << 16) & IMX_APBH_VERSION_MINOR_MASK)

/* STEP */
#define IMX_APBH_VERSION_STEP_SHIFT     0
#define IMX_APBH_VERSION_STEP_MASK      0x0000FFFF
#define IMX_APBH_VERSION_STEP(v)        (((v) << 0) & IMX_APBH_VERSION_STEP_MASK)

#endif /* IMX_APBH_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/aarch64/imx8/imx8_apbh.h $ $Rev: 837679 $")
#endif
