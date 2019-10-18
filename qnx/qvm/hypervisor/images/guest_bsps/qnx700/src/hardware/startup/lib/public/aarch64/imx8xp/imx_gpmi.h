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

#ifndef IMX_GPMI_H_
#define IMX_GPMI_H_

#include <stdint.h>

/* IMX_GPMI_CTRL0 - GPMI Control Register 0 */
typedef union {
    uint32_t U;
    struct {
        unsigned XFER_COUNT: 16;
        unsigned ADDRESS_INCREMENT: 1;
        unsigned ADDRESS: 3;
        unsigned CS: 3;
        unsigned WORD_LENGTH: 1;
        unsigned COMMAND_MODE: 2;
        unsigned UDMA: 1;
        unsigned LOCK_CS: 1;
        unsigned DEV_IRQ_EN: 1;
        unsigned RUN: 1;
        unsigned CLKGATE: 1;
        unsigned SFTRST: 1;
    } B;
} imx_gpmi_ctrl0_t;

#define IMX_GPMI_CTRL0_OFFSET                           (0x0)
#define IMX_GPMI_CTRL0_SET_OFFSET                       (0x4)
#define IMX_GPMI_CTRL0_CLR_OFFSET                       (0x8)
#define IMX_GPMI_CTRL0_TOG_OFFSET                       (0xC)

/* IMX_GPMI_CTRL0 bit-fields */
/* SFTRST */
#define IMX_GPMI_CTRL0_SFTRST_SHIFT                     31
#define IMX_GPMI_CTRL0_SFTRST_MASK                      0x80000000
#define IMX_GPMI_CTRL0_SFTRST(v)                        (((v) << 31) & IMX_GPMI_CTRL0_SFTRST_MASK)

#define IMX_GPMI_CTRL0_SFTRST_BV_RUN                    0x0
#define IMX_GPMI_CTRL0_SFTRST_BV_RESET                  0x1

/* CLKGATE */
#define IMX_GPMI_CTRL0_CLKGATE_SHIFT                    30
#define IMX_GPMI_CTRL0_CLKGATE_MASK                     0x40000000
#define IMX_GPMI_CTRL0_CLKGATE(v)                       (((v) << 30) & IMX_GPMI_CTRL0_CLKGATE_MASK)

#define IMX_GPMI_CTRL0_CLKGATE_BV_RUN                   0x0
#define IMX_GPMI_CTRL0_CLKGATE_BV_NO_CLKS               0x1

/* RUN */
#define IMX_GPMI_CTRL0_RUN_SHIFT                        29
#define IMX_GPMI_CTRL0_RUN_MASK                         0x20000000
#define IMX_GPMI_CTRL0_RUN(v)                           (((v) << 29) & IMX_GPMI_CTRL0_RUN_MASK)

#define IMX_GPMI_CTRL0_RUN_BV_IDLE                      0x0
#define IMX_GPMI_CTRL0_RUN_BV_BUSY                      0x1

/* DEV_IRQ_EN */
#define IMX_GPMI_CTRL0_DEV_IRQ_EN_SHIFT                 28
#define IMX_GPMI_CTRL0_DEV_IRQ_EN_MASK                  0x10000000
#define IMX_GPMI_CTRL0_DEV_IRQ_EN(v)                    (((v) << 28) & IMX_GPMI_CTRL0_DEV_IRQ_EN_MASK)

/* LOCK_CS */
#define IMX_GPMI_CTRL0_LOCK_CS_SHIFT                    27
#define IMX_GPMI_CTRL0_LOCK_CS_MASK                     0x08000000
#define IMX_GPMI_CTRL0_LOCK_CS(v)                       (((v) << 27) & IMX_GPMI_CTRL0_LOCK_CS_MASK)

#define IMX_GPMI_CTRL0_LOCK_CS_BV_DISABLED              0x0
#define IMX_GPMI_CTRL0_LOCK_CS_BV_ENABLED               0x1

/* UDMA */
#define IMX_GPMI_CTRL0_UDMA_SHIFT                       26
#define IMX_GPMI_CTRL0_UDMA_MASK                        0x04000000
#define IMX_GPMI_CTRL0_UDMA(v)                          (((v) << 26) & IMX_GPMI_CTRL0_UDMA_MASK)

#define IMX_GPMI_CTRL0_UDMA_BV_DISABLED                 0x0
#define IMX_GPMI_CTRL0_UDMA_BV_ENABLED                  0x1

/* COMMAND_MODE */
#define IMX_GPMI_CTRL0_COMMAND_MODE_SHIFT               24
#define IMX_GPMI_CTRL0_COMMAND_MODE_MASK                0x03000000
#define IMX_GPMI_CTRL0_COMMAND_MODE(v)                  (((v) << 24) & IMX_GPMI_CTRL0_COMMAND_MODE_MASK)

#define IMX_GPMI_CTRL0_COMMAND_MODE_BV_WRITE            0x0
#define IMX_GPMI_CTRL0_COMMAND_MODE_BV_READ             0x1
#define IMX_GPMI_CTRL0_COMMAND_MODE_BV_READ_AND_COMPARE 0x2
#define IMX_GPMI_CTRL0_COMMAND_MODE_BV_WAIT_FOR_READY   0x3

/* WORD_LENGTH */
#define IMX_GPMI_CTRL0_WORD_LENGTH_SHIFT                23
#define IMX_GPMI_CTRL0_WORD_LENGTH_MASK                 0x00800000
#define IMX_GPMI_CTRL0_WORD_LENGTH(v)                   (((v) << 23) & IMX_GPMI_CTRL0_WORD_LENGTH_MASK)

#define IMX_GPMI_CTRL0_WORD_LENGTH_BV_16_BIT            0x0
#define IMX_GPMI_CTRL0_WORD_LENGTH_BV_8_BIT             0x1

/* CS */
#define IMX_GPMI_CTRL0_CS_SHIFT                         20
#define IMX_GPMI_CTRL0_CS_MASK                          0x00700000
#define IMX_GPMI_CTRL0_CS(v)                            (((v) << 20) & IMX_GPMI_CTRL0_CS_MASK)

/* ADDRESS */
#define IMX_GPMI_CTRL0_ADDRESS_SHIFT                    17
#define IMX_GPMI_CTRL0_ADDRESS_MASK                     0x000E0000
#define IMX_GPMI_CTRL0_ADDRESS(v)                       (((v) << 17) & IMX_GPMI_CTRL0_ADDRESS_MASK)

#define IMX_GPMI_CTRL0_ADDRESS_BV_NAND_DATA             0x0
#define IMX_GPMI_CTRL0_ADDRESS_BV_NAND_CLE              0x1
#define IMX_GPMI_CTRL0_ADDRESS_BV_NAND_ALE              0x2

/* ADDRESS_INCREMENT */
#define IMX_GPMI_CTRL0_ADDRESS_INCREMENT_SHIFT          16
#define IMX_GPMI_CTRL0_ADDRESS_INCREMENT_MASK           0x00010000
#define IMX_GPMI_CTRL0_ADDRESS_INCREMENT(v)             (((v) << 16) & IMX_GPMI_CTRL0_ADDRESS_INCREMENT_MASK)

#define IMX_GPMI_CTRL0_ADDRESS_INCREMENT_BV_DISABLED    0x0
#define IMX_GPMI_CTRL0_ADDRESS_INCREMENT_BV_ENABLED     0x1

/* XFER_COUNT */
#define IMX_GPMI_CTRL0_XFER_COUNT_SHIFT                 0
#define IMX_GPMI_CTRL0_XFER_COUNT_MASK                  0x0000FFFF
#define IMX_GPMI_CTRL0_XFER_COUNT(v)                    (((v) << 0) & IMX_GPMI_CTRL0_XFER_COUNT_MASK)

/*
 * IMX_GPMI_COMPARE - GPMI Compare Register
 */
typedef union {
    uint32_t U;
    struct {
        unsigned REFERENCE: 16;
        unsigned MASK: 16;
    } B;
} imx_gpmi_compare_t;

#define IMX_GPMI_COMPARE_OFFSET                         (0x10)

/* IMX_GPMI_COMPARE bit-fields */
/* MASK */
#define IMX_GPMI_COMPARE_MASK_SHIFT                     16
#define IMX_GPMI_COMPARE_MASK_MASK                      0xFFFF0000
#define IMX_GPMI_COMPARE_MASK(v)                        (((v) << 16) & IMX_GPMI_COMPARE_MASK_MASK)

/* REFERENCE */
#define IMX_GPMI_COMPARE_REFERENCE_SHIFT                0
#define IMX_GPMI_COMPARE_REFERENCE_MASK                 0x0000FFFF
#define IMX_GPMI_COMPARE_REFERENCE(v)                   (((v) << 0) & IMX_GPMI_COMPARE_REFERENCE_MASK)

/*
 * IMX_GPMI_ECCCTRL - GPMI Integrated ECC Control Register
 */
typedef union {
    uint32_t U;
    struct {
        unsigned BUFFER_MASK: 9;
        unsigned RSVD1: 3;
        unsigned ENABLE_ECC: 1;
        unsigned ECC_CMD: 2;
        unsigned RSVD2: 1;
        unsigned HANDLE: 16;
    } B;
} imx_gpmi_eccctrl_t;

#define IMX_GPMI_ECCCTRL_OFFSET                         (0x20)
#define IMX_GPMI_ECCCTRL_SET_OFFSET                     (0x24)
#define IMX_GPMI_ECCCTRL_CLR_OFFSET                     (0x28)
#define IMX_GPMI_ECCCTRL_TOG_OFFSET                     (0x2C)

/* IMX_GPMI_ECCCTRL bit-fields */
/* HANDLE */
#define IMX_GPMI_ECCCTRL_HANDLE_SHIFT                   16
#define IMX_GPMI_ECCCTRL_HANDLE_MASK                    0xFFFF0000
#define IMX_GPMI_ECCCTRL_HANDLE(v)                      (((v) << 16) & IMX_GPMI_ECCCTRL_HANDLE_MASK)


/* RSVD2 */
#define IMX_GPMI_ECCCTRL_RSVD2_SHIFT                    15
#define IMX_GPMI_ECCCTRL_RSVD2_MASK                     0x00008000
#define IMX_GPMI_ECCCTRL_RSVD2(v)                       (((v) << 15) & IMX_GPMI_ECCCTRL_RSVD2_MASK)

/* ECC_CMD */
#define IMX_GPMI_ECCCTRL_ECC_CMD_SHIFT                  13
#define IMX_GPMI_ECCCTRL_ECC_CMD_MASK                   0x00006000
#define IMX_GPMI_ECCCTRL_ECC_CMD(v)                     (((v) << 13) & IMX_GPMI_ECCCTRL_ECC_CMD_MASK)

#define IMX_GPMI_ECCCTRL_ECC_CMD_BV_DECODE              0x0
#define IMX_GPMI_ECCCTRL_ECC_CMD_BV_ENCODE              0x1
#define IMX_GPMI_ECCCTRL_ECC_CMD_BV_RESERVE2            0x2
#define IMX_GPMI_ECCCTRL_ECC_CMD_BV_RESERVE3            0x3

/* ENABLE_ECC */
#define IMX_GPMI_ECCCTRL_ENABLE_ECC_SHIFT               12
#define IMX_GPMI_ECCCTRL_ENABLE_ECC_MASK                0x00001000
#define IMX_GPMI_ECCCTRL_ENABLE_ECC(v)                  (((v) << 12) & IMX_GPMI_ECCCTRL_ENABLE_ECC_MASK)

#define IMX_GPMI_ECCCTRL_ENABLE_ECC_BV_ENABLE           0x1
#define IMX_GPMI_ECCCTRL_ENABLE_ECC_BV_DISABLE          0x0

/* RSVD1 */
#define IMX_GPMI_ECCCTRL_RSVD1_SHIFT                    9
#define IMX_GPMI_ECCCTRL_RSVD1_MASK                     0x00000E00
#define IMX_GPMI_ECCCTRL_RSVD1(v)                       (((v) << 9) & IMX_GPMI_ECCCTRL_RSVD1_MASK)

/* BUFFER_MASK */
#define IMX_GPMI_ECCCTRL_BUFFER_MASK_SHIFT              0
#define IMX_GPMI_ECCCTRL_BUFFER_MASK_MASK               0x000001FF
#define IMX_GPMI_ECCCTRL_BUFFER_MASK(v)                 (((v) << 0) & IMX_GPMI_ECCCTRL_BUFFER_MASK_MASK)

#define IMX_GPMI_ECCCTRL_BUFFER_MASK_BV_BCH_AUXONLY     0x100
#define IMX_GPMI_ECCCTRL_BUFFER_MASK_BV_BCH_PAGE        0x1FF

/*
 * IMX_GPMI_ECCCOUNT - GPMI Integrated ECC Transfer Count Register
 */
typedef union {
    uint32_t U;
    struct {
        unsigned COUNT: 16;
        unsigned RSVD2: 16;
    } B;
} imx_gpmi_ecccount_t;

#define IMX_GPMI_ECCCOUNT_OFFSET                        (0x30)

/* IMX_GPMI_ECCCOUNT bit-fields */
/* RSVD2 */
#define IMX_GPMI_ECCCOUNT_RSVD2_SHIFT                   16
#define IMX_GPMI_ECCCOUNT_RSVD2_MASK                    0xFFFF0000
#define IMX_GPMI_ECCCOUNT_RSVD2(v)                      (((v) << 16) & IMX_GPMI_ECCCOUNT_RSVD2_MASK)

/* COUNT */
#define IMX_GPMI_ECCCOUNT_COUNT_SHIFT                   0
#define IMX_GPMI_ECCCOUNT_COUNT_MASK                    0x0000FFFF
#define IMX_GPMI_ECCCOUNT_COUNT(v)                      (((v) << 0) & IMX_GPMI_ECCCOUNT_COUNT_MASK)

/*
 * IMX_GPMI_PAYLOAD - GPMI Payload Address Register
 */
typedef union {
    uint32_t U;
    struct {
        unsigned RSVD0: 2;
        unsigned ADDRESS: 30;
    } B;
} imx_gpmi_payload_t;

#define IMX_GPMI_PAYLOAD_OFFSET                         (0x40)

/* IMX_GPMI_PAYLOAD bit-fields */
/* ADDRESS */
#define IMX_GPMI_PAYLOAD_ADDRESS_SHIFT                  2
#define IMX_GPMI_PAYLOAD_ADDRESS_MASK                   0xFFFFFFFC
#define IMX_GPMI_PAYLOAD_ADDRESS(v)                     (((v) << 2) & IMX_GPMI_PAYLOAD_ADDRESS_MASK)

/* RSVD0 */
#define IMX_GPMI_PAYLOAD_RSVD0_SHIFT                    0
#define IMX_GPMI_PAYLOAD_RSVD0_MASK                     0x00000003
#define IMX_GPMI_PAYLOAD_RSVD0(v)                       (((v) << 0) & IMX_GPMI_PAYLOAD_RSVD0_MASK)

/*
 * IMX_GPMI_AUXILIARY - GPMI Auxiliary Address Register
 */
typedef union {
    uint32_t U;
    struct {
        unsigned RSVD0: 2;
        unsigned ADDRESS: 30;
    } B;
} imx_gpmi_auxiliary_t;

#define IMX_GPMI_AUXILIARY_OFFSET                       (0x50)

/* IMX_GPMI_AUXILIARY bit-fields */
/* ADDRESS */
#define IMX_GPMI_AUXILIARY_ADDRESS_SHIFT                2
#define IMX_GPMI_AUXILIARY_ADDRESS_MASK                 0xFFFFFFFC
#define IMX_GPMI_AUXILIARY_ADDRESS(v)                   (((v) << 2) & IMX_GPMI_AUXILIARY_ADDRESS_MASK)

/* RSVD0 */
#define IMX_GPMI_AUXILIARY_RSVD0_SHIFT                  0
#define IMX_GPMI_AUXILIARY_RSVD0_MASK                   0x00000003
#define IMX_GPMI_AUXILIARY_RSVD0(v)                     (((v) << 0) & IMX_GPMI_AUXILIARY_RSVD0_MASK)

/*
 * IMX_GPMI_CTRL1 - GPMI Control Register 1
 */
typedef union {
    uint32_t U;
    struct {
        unsigned GPMI_MODE: 1;
        unsigned CAMERA_MODE: 1;
        unsigned ATA_IRQRDY_POLARITY: 1;
        unsigned DEV_RESET: 1;
        unsigned ABORT_WAIT_FOR_READY_CHANNEL: 3;
        unsigned ABORT_WAIT_REQUEST: 1;
        unsigned BURST_EN: 1;
        unsigned TIMEOUT_IRQ: 1;
        unsigned DEV_IRQ: 1;
        unsigned DMA2ECC_MODE: 1;
        unsigned RDN_DELAY: 4;
        unsigned HALF_PERIOD: 1;
        unsigned DLL_ENABLE: 1;
        unsigned BCH_MODE: 1;
        unsigned GANGED_RDYBUSY: 1;
        unsigned TIMEOUT_IRQ_EN: 1;
        unsigned RSVD1: 1;
        unsigned WRN_DLY_SEL: 2;
        unsigned DECOUPLE_CS: 1;
        unsigned SSYNCMODE: 1;
        unsigned UPDATE_CS: 1;
        unsigned GPMI_CLK_DIV2_EN: 1;
        unsigned TOGGLE_MODE: 1;
        unsigned WRITE_CLK_STOP: 1;
        unsigned SSYNC_CLK_STOP: 1;
        unsigned DEV_CLK_STOP: 1;
    } B;
} imx_gpmi_ctrl1_t;

#define IMX_GPMI_CTRL1_OFFSET                             (0x60)
#define IMX_GPMI_CTRL1_SET_OFFSET                         (0x64)
#define IMX_GPMI_CTRL1_CLR_OFFSET                         (0x68)
#define IMX_GPMI_CTRL1_TOG_OFFSET                         (0x6C)

/* IMX_GPMI_CTRL1 bit-fields */
/* DEV_CLK_STOP */
#define IMX_GPMI_CTRL1_DEV_CLK_STOP_SHIFT                 31
#define IMX_GPMI_CTRL1_DEV_CLK_STOP_MASK                  0x80000000
#define IMX_GPMI_CTRL1_DEV_CLK_STOP(v)                    ((((uint32_t) v) << 31) & IMX_GPMI_CTRL1_DEV_CLK_STOP_MASK)

/* SSYNC_CLK_STOP */
#define IMX_GPMI_CTRL1_SSYNC_CLK_STOP_SHIFT               30
#define IMX_GPMI_CTRL1_SSYNC_CLK_STOP_MASK                0x40000000
#define IMX_GPMI_CTRL1_SSYNC_CLK_STOP(v)                  (((v) << 30) & IMX_GPMI_CTRL1_SSYNC_CLK_STOP_MASK)

/* WRITE_CLK_STOP */
#define IMX_GPMI_CTRL1_WRITE_CLK_STOP_SHIFT               29
#define IMX_GPMI_CTRL1_WRITE_CLK_STOP_MASK                0x20000000
#define IMX_GPMI_CTRL1_WRITE_CLK_STOP(v)                  (((v) << 29) & IMX_GPMI_CTRL1_WRITE_CLK_STOP_MASK)

/* TOGGLE_MODE */
#define IMX_GPMI_CTRL1_TOGGLE_MODE_SHIFT                  28
#define IMX_GPMI_CTRL1_TOGGLE_MODE_MASK                   0x10000000
#define IMX_GPMI_CTRL1_TOGGLE_MODE(v)                     (((v) << 28) & IMX_GPMI_CTRL1_TOGGLE_MODE_MASK)

/* GPMI_CLK_DIV2_EN */
#define IMX_GPMI_CTRL1_GPMI_CLK_DIV2_EN_SHIFT             27
#define IMX_GPMI_CTRL1_GPMI_CLK_DIV2_EN_MASK              0x08000000
#define IMX_GPMI_CTRL1_GPMI_CLK_DIV2_EN(v)                (((v) << 27) & IMX_GPMI_CTRL1_GPMI_CLK_DIV2_EN_MASK)

/* UPDATE_CS */
#define IMX_GPMI_CTRL1_UPDATE_CS_SHIFT                    26
#define IMX_GPMI_CTRL1_UPDATE_CS_MASK                     0x04000000
#define IMX_GPMI_CTRL1_UPDATE_CS(v)                       (((v) << 26) & IMX_GPMI_CTRL1_UPDATE_CS_MASK)

/* SSYNCMODE */
#define IMX_GPMI_CTRL1_SSYNCMODE_SHIFT                    25
#define IMX_GPMI_CTRL1_SSYNCMODE_MASK                     0x02000000
#define IMX_GPMI_CTRL1_SSYNCMODE(v)                       (((v) << 25) & IMX_GPMI_CTRL1_SSYNCMODE_MASK)

#define IMX_GPMI_CTRL1_SSYNCMODE_BV_ASYNC                 0x0
#define IMX_GPMI_CTRL1_SSYNCMODE_BV_SSYNC                 0x1

/* DECOUPLE_CS */
#define IMX_GPMI_CTRL1_DECOUPLE_CS_SHIFT                  24
#define IMX_GPMI_CTRL1_DECOUPLE_CS_MASK                   0x01000000
#define IMX_GPMI_CTRL1_DECOUPLE_CS(v)                     (((v) << 24) & IMX_GPMI_CTRL1_DECOUPLE_CS_MASK)

/* WRN_DLY_SEL */
#define IMX_GPMI_CTRL1_WRN_DLY_SEL_SHIFT                  22
#define IMX_GPMI_CTRL1_WRN_DLY_SEL_MASK                   0x00C00000
#define IMX_GPMI_CTRL1_WRN_DLY_SEL(v)                     (((v) << 22) & IMX_GPMI_CTRL1_WRN_DLY_SEL_MASK)

/* RSVD1 */
#define IMX_GPMI_CTRL1_RSVD1_SHIFT                        21
#define IMX_GPMI_CTRL1_RSVD1_MASK                         0x00200000
#define IMX_GPMI_CTRL1_RSVD1(v)                           (((v) << 21) & IMX_GPMI_CTRL1_RSVD1_MASK)

/* TIMEOUT_IRQ_EN */
#define IMX_GPMI_CTRL1_TIMEOUT_IRQ_EN_SHIFT               20
#define IMX_GPMI_CTRL1_TIMEOUT_IRQ_EN_MASK                0x00100000
#define IMX_GPMI_CTRL1_TIMEOUT_IRQ_EN(v)                  (((v) << 20) & IMX_GPMI_CTRL1_TIMEOUT_IRQ_EN_MASK)

/* GANGED_RDYBUSY */
#define IMX_GPMI_CTRL1_GANGED_RDYBUSY_SHIFT               19
#define IMX_GPMI_CTRL1_GANGED_RDYBUSY_MASK                0x00080000
#define IMX_GPMI_CTRL1_GANGED_RDYBUSY(v)                  (((v) << 19) & IMX_GPMI_CTRL1_GANGED_RDYBUSY_MASK)

/* BCH_MODE */
#define IMX_GPMI_CTRL1_BCH_MODE_SHIFT                     18
#define IMX_GPMI_CTRL1_BCH_MODE_MASK                      0x00040000
#define IMX_GPMI_CTRL1_BCH_MODE(v)                        (((v) << 18) & IMX_GPMI_CTRL1_BCH_MODE_MASK)

/* DLL_ENABLE */
#define IMX_GPMI_CTRL1_DLL_ENABLE_SHIFT                   17
#define IMX_GPMI_CTRL1_DLL_ENABLE_MASK                    0x00020000
#define IMX_GPMI_CTRL1_DLL_ENABLE(v)                      (((v) << 17) & IMX_GPMI_CTRL1_DLL_ENABLE_MASK)

/* HALF_PERIOD */
#define IMX_GPMI_CTRL1_HALF_PERIOD_SHIFT                  16
#define IMX_GPMI_CTRL1_HALF_PERIOD_MASK                   0x00010000
#define IMX_GPMI_CTRL1_HALF_PERIOD(v)                     (((v) << 16) & IMX_GPMI_CTRL1_HALF_PERIOD_MASK)

/* RDN_DELAY */
#define IMX_GPMI_CTRL1_RDN_DELAY_SHIFT                    12
#define IMX_GPMI_CTRL1_RDN_DELAY_MASK                     0x0000F000
#define IMX_GPMI_CTRL1_RDN_DELAY(v)                       (((v) << 12) & IMX_GPMI_CTRL1_RDN_DELAY_MASK)

/* DMA2ECC_MODE */
#define IMX_GPMI_CTRL1_DMA2ECC_MODE_SHIFT                 11
#define IMX_GPMI_CTRL1_DMA2ECC_MODE_MASK                  0x00000800
#define IMX_GPMI_CTRL1_DMA2ECC_MODE(v)                    (((v) << 11) & IMX_GPMI_CTRL1_DMA2ECC_MODE_MASK)

/* DEV_IRQ */
#define IMX_GPMI_CTRL1_DEV_IRQ_SHIFT                      10
#define IMX_GPMI_CTRL1_DEV_IRQ_MASK                       0x00000400
#define IMX_GPMI_CTRL1_DEV_IRQ(v)                         (((v) << 10) & IMX_GPMI_CTRL1_DEV_IRQ_MASK)

/* TIMEOUT_IRQ */
#define IMX_GPMI_CTRL1_TIMEOUT_IRQ_SHIFT                  9
#define IMX_GPMI_CTRL1_TIMEOUT_IRQ_MASK                   0x00000200
#define IMX_GPMI_CTRL1_TIMEOUT_IRQ(v)                     (((v) << 9) & IMX_GPMI_CTRL1_TIMEOUT_IRQ_MASK)

/* BURST_EN */
#define IMX_GPMI_CTRL1_BURST_EN_SHIFT                     8
#define IMX_GPMI_CTRL1_BURST_EN_MASK                      0x00000100
#define IMX_GPMI_CTRL1_BURST_EN(v)                        (((v) << 8) & IMX_GPMI_CTRL1_BURST_EN_MASK)

/* ABORT_WAIT_REQUEST */
#define IMX_GPMI_CTRL1_ABORT_WAIT_REQUEST_SHIFT           7
#define IMX_GPMI_CTRL1_ABORT_WAIT_REQUEST_MASK            0x00000080
#define IMX_GPMI_CTRL1_ABORT_WAIT_REQUEST(v)              (((v) << 7) & IMX_GPMI_CTRL1_ABORT_WAIT_REQUEST_MASK)

/* ABORT_WAIT_FOR_READY_CHANNEL */
#define IMX_GPMI_CTRL1_ABORT_WAIT_FOR_READY_CHANNEL_SHIFT 4
#define IMX_GPMI_CTRL1_ABORT_WAIT_FOR_READY_CHANNEL_MASK  0x00000070
#define IMX_GPMI_CTRL1_ABORT_WAIT_FOR_READY_CHANNEL(v)    (((v) << 4) & IMX_GPMI_CTRL1_ABORT_WAIT_FOR_READY_CHANNEL_MASK)

/* DEV_RESET */
#define IMX_GPMI_CTRL1_DEV_RESET_SHIFT                    3
#define IMX_GPMI_CTRL1_DEV_RESET_MASK                     0x00000008
#define IMX_GPMI_CTRL1_DEV_RESET(v)                       (((v) << 3) & IMX_GPMI_CTRL1_DEV_RESET_MASK)

#define IMX_GPMI_CTRL1_DEV_RESET_BV_ENABLED               0x0
#define IMX_GPMI_CTRL1_DEV_RESET_BV_DISABLED              0x1

/* ATA_IRQRDY_POLARITY */
#define IMX_GPMI_CTRL1_ATA_IRQRDY_POLARITY_SHIFT          2
#define IMX_GPMI_CTRL1_ATA_IRQRDY_POLARITY_MASK           0x00000004
#define IMX_GPMI_CTRL1_ATA_IRQRDY_POLARITY(v)             (((v) << 2) & IMX_GPMI_CTRL1_ATA_IRQRDY_POLARITY_MASK)

#define IMX_GPMI_CTRL1_ATA_IRQRDY_POLARITY_BV_ACTIVELOW   0x0
#define IMX_GPMI_CTRL1_ATA_IRQRDY_POLARITY_BV_ACTIVEHIGH  0x1

/* CAMERA_MODE */
#define IMX_GPMI_CTRL1_CAMERA_MODE_SHIFT                  1
#define IMX_GPMI_CTRL1_CAMERA_MODE_MASK                   0x00000002
#define IMX_GPMI_CTRL1_CAMERA_MODE(v)                     (((v) << 1) & IMX_GPMI_CTRL1_CAMERA_MODE_MASK)

/* GPMI_MODE */
#define IMX_GPMI_CTRL1_GPMI_MODE_SHIFT                    0
#define IMX_GPMI_CTRL1_GPMI_MODE_MASK                     0x00000001
#define IMX_GPMI_CTRL1_GPMI_MODE(v)                       (((v) << 0) & IMX_GPMI_CTRL1_GPMI_MODE_MASK)

#define IMX_GPMI_CTRL1_GPMI_MODE_BV_NAND                  0x0
#define IMX_GPMI_CTRL1_GPMI_MODE_BV_ATA                   0x1

/*
 * IMX_GPMI_TIMING0 - GPMI Timing Register 0
 */
typedef union {
    uint32_t U;
    struct {
        unsigned DATA_SETUP: 8;
        unsigned DATA_HOLD: 8;
        unsigned ADDRESS_SETUP: 8;
        unsigned RSVD1: 8;
    } B;
} imx_gpmi_timing0_t;

#define IMX_GPMI_TIMING0_OFFSET                             (0x70)

/* IMX_GPMI_TIMING0 bit-fields */
/* RSVD1 */
#define IMX_GPMI_TIMING0_RSVD1_SHIFT                        24
#define IMX_GPMI_TIMING0_RSVD1_MASK                         0xFF000000
#define IMX_GPMI_TIMING0_RSVD1(v)                           ((((uint32_t) v) << 24) & IMX_GPMI_TIMING0_RSVD1_MASK)

/* --- Register IMX_GPMI_TIMING0, field ADDRESS_SETUP */
#define IMX_GPMI_TIMING0_ADDRESS_SETUP_SHIFT                16
#define IMX_GPMI_TIMING0_ADDRESS_SETUP_MASK                 0x00FF0000
#define IMX_GPMI_TIMING0_ADDRESS_SETUP(v)                   (((v) << 16) & IMX_GPMI_TIMING0_ADDRESS_SETUP_MASK)

/* --- Register IMX_GPMI_TIMING0, field DATA_HOLD */
#define IMX_GPMI_TIMING0_DATA_HOLD_SHIFT                    8
#define IMX_GPMI_TIMING0_DATA_HOLD_MASK                     0x0000FF00
#define IMX_GPMI_TIMING0_DATA_HOLD(v)                       (((v) << 8) & IMX_GPMI_TIMING0_DATA_HOLD_MASK)

/* --- Register IMX_GPMI_TIMING0, field DATA_SETUP */
#define IMX_GPMI_TIMING0_DATA_SETUP_SHIFT                   0
#define IMX_GPMI_TIMING0_DATA_SETUP_MASK                    0x000000FF
#define IMX_GPMI_TIMING0_DATA_SETUP(v)                      (((v) << 0) & IMX_GPMI_TIMING0_DATA_SETUP_MASK)

/*
 * IMX_GPMI_TIMING1 - GPMI Timing Register 1
 */
typedef union {
    uint32_t U;
    struct {
        unsigned RSVD1: 16;
        unsigned DEVICE_BUSY_TIMEOUT: 16;
    } B;
} imx_gpmi_timing1_t;

#define IMX_GPMI_TIMING1_OFFSET                            (0x80)

/* IMX_GPMI_TIMING1 bit-fields */
#define IMX_GPMI_TIMING1_DEVICE_BUSY_TIMEOUT_SHIFT         16
#define IMX_GPMI_TIMING1_DEVICE_BUSY_TIMEOUT_MASK          0xFFFF0000
#define IMX_GPMI_TIMING1_DEVICE_BUSY_TIMEOUT(v)            ((((uint32_t) v) << 16) & IMX_GPMI_TIMING1_DEVICE_BUSY_TIMEOUT_MASK)

/* RSVD1 */
#define IMX_GPMI_TIMING1_RSVD1_SHIFT                       0
#define IMX_GPMI_TIMING1_RSVD1_MASK                        0x0000FFFF
#define IMX_GPMI_TIMING1_RSVD1(v)                          (((v) << 0) & IMX_GPMI_TIMING1_RSVD1_MASK)

/*
 * IMX_GPMI_TIMING2 - GPMI Timing Register 2
 */
typedef union {
    uint32_t U;
    struct {
        unsigned DATA_PAUSE: 4;
        unsigned CMDADD_PAUSE: 4;
        unsigned POSTAMBLE_DELAY: 4;
        unsigned PREAMBLE_DELAY: 4;
        unsigned CE_DELAY: 5;
        unsigned RSVD0: 3;
        unsigned READ_LATENCY: 3;
        unsigned RSVD1: 5;
    } B;
} imx_gpmi_timing2_t;

#define IMX_GPMI_TIMING2_OFFSET                 (0x90)

/* IMX_GPMI_TIMING2 bit-fields */
/* RSVD1 */
#define IMX_GPMI_TIMING2_RSVD1_SHIFT            27
#define IMX_GPMI_TIMING2_RSVD1_MASK             0xF8000000
#define IMX_GPMI_TIMING2_RSVD1(v)               ((((uint32_t) v) << 27) & IMX_GPMI_TIMING2_RSVD1_MASK)

/* READ_LATENCY */
#define IMX_GPMI_TIMING2_READ_LATENCY_SHIFT     24
#define IMX_GPMI_TIMING2_READ_LATENCY_MASK      0x07000000
#define IMX_GPMI_TIMING2_READ_LATENCY(v)        (((v) << 24) & IMX_GPMI_TIMING2_READ_LATENCY_MASK)

/* RSVD0 */
#define IMX_GPMI_TIMING2_RSVD0_SHIFT            21
#define IMX_GPMI_TIMING2_RSVD0_MASK             0x00E00000
#define IMX_GPMI_TIMING2_RSVD0(v)               (((v) << 21) & IMX_GPMI_TIMING2_RSVD0_MASK)

/* CE_DELAY */
#define IMX_GPMI_TIMING2_CE_DELAY_SHIFT         16
#define IMX_GPMI_TIMING2_CE_DELAY_MASK          0x001F0000
#define IMX_GPMI_TIMING2_CE_DELAY(v)            (((v) << 16) & IMX_GPMI_TIMING2_CE_DELAY_MASK)

/* PREAMBLE_DELAY */
#define IMX_GPMI_TIMING2_PREAMBLE_DELAY_SHIFT   12
#define IMX_GPMI_TIMING2_PREAMBLE_DELAY_MASK    0x0000F000
#define IMX_GPMI_TIMING2_PREAMBLE_DELAY(v)      (((v) << 12) & IMX_GPMI_TIMING2_PREAMBLE_DELAY_MASK)

/* POSTAMBLE_DELAY */
#define IMX_GPMI_TIMING2_POSTAMBLE_DELAY_SHIFT  8
#define IMX_GPMI_TIMING2_POSTAMBLE_DELAY_MASK   0x00000F00
#define IMX_GPMI_TIMING2_POSTAMBLE_DELAY(v)     (((v) << 8) & IMX_GPMI_TIMING2_POSTAMBLE_DELAY_MASK)

/* CMDADD_PAUSE */
#define IMX_GPMI_TIMING2_CMDADD_PAUSE_SHIFT     4
#define IMX_GPMI_TIMING2_CMDADD_PAUSE_MASK      0x000000F0
#define IMX_GPMI_TIMING2_CMDADD_PAUSE(v)        (((v) << 4) & IMX_GPMI_TIMING2_CMDADD_PAUSE_MASK)

/* DATA_PAUSE */
#define IMX_GPMI_TIMING2_DATA_PAUSE_SHIFT       0
#define IMX_GPMI_TIMING2_DATA_PAUSE_MASK        0x0000000F
#define IMX_GPMI_TIMING2_DATA_PAUSE(v)          (((v) << 0) & IMX_GPMI_TIMING2_DATA_PAUSE_MASK)

/*
 * IMX_GPMI_DATA - GPMI DMA Data Transfer Register
 */
typedef union {
    uint32_t U;
    struct {
        unsigned DATA: 32;
    } B;
} imx_gpmi_data_t;

#define IMX_GPMI_DATA_OFFSET            (0xA0)

/* IMX_GPMI_DATA bit-fields */
/* DATA */
#define IMX_GPMI_DATA_DATA_SHIFT        0
#define IMX_GPMI_DATA_DATA_MASK         0xFFFFFFFF
#define IMX_GPMI_DATA_DATA(v)           ((uint32_t) v)

/*
 * IMX_GPMI_STAT - GPMI Status Register
 */
typedef union {
    uint32_t U;
    struct {
        unsigned PRESENT: 1;
        unsigned FIFO_FULL: 1;
        unsigned FIFO_EMPTY: 1;
        unsigned INVALID_BUFFER_MASK: 1;
        unsigned ATA_IRQ: 1;
        unsigned RSVD1: 3;
        unsigned DEV0_ERROR: 1;
        unsigned DEV1_ERROR: 1;
        unsigned DEV2_ERROR: 1;
        unsigned DEV3_ERROR: 1;
        unsigned DEV4_ERROR: 1;
        unsigned DEV5_ERROR: 1;
        unsigned DEV6_ERROR: 1;
        unsigned DEV7_ERROR: 1;
        unsigned RDY_TIMEOUT: 8;
        unsigned READY_BUSY: 8;
    } B;
} imx_gpmi_stat_t;

#define IMX_GPMI_STAT_OFFSET                    (0xB0)

/* IMX_GPMI_STAT bit-fields */
/* READY_BUSY */
#define IMX_GPMI_STAT_READY_BUSY_SHIFT          24
#define IMX_GPMI_STAT_READY_BUSY_MASK           0xFF000000
#define IMX_GPMI_STAT_READY_BUSY(v)             ((((uint32_t) v) << 24) & IMX_GPMI_STAT_READY_BUSY_MASK)

/* RDY_TIMEOUT */
#define IMX_GPMI_STAT_RDY_TIMEOUT_SHIFT         16
#define IMX_GPMI_STAT_RDY_TIMEOUT_MASK          0x00FF0000
#define IMX_GPMI_STAT_RDY_TIMEOUT(v)            (((v) << 16) & IMX_GPMI_STAT_RDY_TIMEOUT_MASK)

/* DEV7_ERROR */
#define IMX_GPMI_STAT_DEV7_ERROR_SHIFT          15
#define IMX_GPMI_STAT_DEV7_ERROR_MASK           0x00008000
#define IMX_GPMI_STAT_DEV7_ERROR(v)             (((v) << 15) & IMX_GPMI_STAT_DEV7_ERROR_MASK)

/* DEV6_ERROR */
#define IMX_GPMI_STAT_DEV6_ERROR_SHIFT          14
#define IMX_GPMI_STAT_DEV6_ERROR_MASK           0x00004000
#define IMX_GPMI_STAT_DEV6_ERROR(v)             (((v) << 14) & IMX_GPMI_STAT_DEV6_ERROR_MASK)

/* DEV5_ERROR */
#define IMX_GPMI_STAT_DEV5_ERROR_SHIFT          13
#define IMX_GPMI_STAT_DEV5_ERROR_MASK           0x00002000
#define IMX_GPMI_STAT_DEV5_ERROR(v)             (((v) << 13) & IMX_GPMI_STAT_DEV5_ERROR_MASK)

/* DEV4_ERROR */
#define IMX_GPMI_STAT_DEV4_ERROR_SHIFT          12
#define IMX_GPMI_STAT_DEV4_ERROR_MASK           0x00001000
#define IMX_GPMI_STAT_DEV4_ERROR(v)             (((v) << 12) & IMX_GPMI_STAT_DEV4_ERROR_MASK)

/* DEV3_ERROR */
#define IMX_GPMI_STAT_DEV3_ERROR_SHIFT          11
#define IMX_GPMI_STAT_DEV3_ERROR_MASK           0x00000800
#define IMX_GPMI_STAT_DEV3_ERROR(v)             (((v) << 11) & IMX_GPMI_STAT_DEV3_ERROR_MASK)

/* DEV2_ERROR */
#define IMX_GPMI_STAT_DEV2_ERROR_SHIFT          10
#define IMX_GPMI_STAT_DEV2_ERROR_MASK           0x00000400
#define IMX_GPMI_STAT_DEV2_ERROR(v)             (((v) << 10) & IMX_GPMI_STAT_DEV2_ERROR_MASK)

/* DEV1_ERROR */
#define IMX_GPMI_STAT_DEV1_ERROR_SHIFT          9
#define IMX_GPMI_STAT_DEV1_ERROR_MASK           0x00000200
#define IMX_GPMI_STAT_DEV1_ERROR(v)             (((v) << 9) & IMX_GPMI_STAT_DEV1_ERROR_MASK)

/* DEV0_ERROR */
#define IMX_GPMI_STAT_DEV0_ERROR_SHIFT          8
#define IMX_GPMI_STAT_DEV0_ERROR_MASK           0x00000100
#define IMX_GPMI_STAT_DEV0_ERROR(v)             (((v) << 8) & IMX_GPMI_STAT_DEV0_ERROR_MASK)

/* RSVD1 */
#define IMX_GPMI_STAT_RSVD1_SHIFT               5
#define IMX_GPMI_STAT_RSVD1_MASK                0x000000E0
#define IMX_GPMI_STAT_RSVD1(v)                  (((v) << 5) & IMX_GPMI_STAT_RSVD1_MASK)

/* ATA_IRQ */
#define IMX_GPMI_STAT_ATA_IRQ_SHIFT             4
#define IMX_GPMI_STAT_ATA_IRQ_MASK              0x00000010
#define IMX_GPMI_STAT_ATA_IRQ(v)                (((v) << 4) & IMX_GPMI_STAT_ATA_IRQ_MASK)

/* INVALID_BUFFER_MASK */
#define IMX_GPMI_STAT_INVALID_BUFFER_MASK_SHIFT 3
#define IMX_GPMI_STAT_INVALID_BUFFER_MASK_MASK  0x00000008
#define IMX_GPMI_STAT_INVALID_BUFFER_MASK(v)    (((v) << 3) & IMX_GPMI_STAT_INVALID_BUFFER_MASK_MASK)

/* FIFO_EMPTY */
#define IMX_GPMI_STAT_FIFO_EMPTY_SHIFT          2
#define IMX_GPMI_STAT_FIFO_EMPTY_MASK           0x00000004

#define IMX_GPMI_STAT_FIFO_EMPTY(v)             (((v) << 2) & IMX_GPMI_STAT_FIFO_EMPTY_MASK)

#define IMX_GPMI_STAT_FIFO_EMPTY_BV_NOT_EMPTY   0x0
#define IMX_GPMI_STAT_FIFO_EMPTY_BV_EMPTY       0x1

/* FIFO_FULL */
#define IMX_GPMI_STAT_FIFO_FULL_SHIFT           1
#define IMX_GPMI_STAT_FIFO_FULL_MASK            0x00000002

#define IMX_GPMI_STAT_FIFO_FULL(v)              (((v) << 1) & IMX_GPMI_STAT_FIFO_FULL_MASK)

#define IMX_GPMI_STAT_FIFO_FULL_BV_NOT_FULL     0x0
#define IMX_GPMI_STAT_FIFO_FULL_BV_FULL         0x1

/* PRESENT */
#define IMX_GPMI_STAT_PRESENT_SHIFT             0
#define IMX_GPMI_STAT_PRESENT_MASK              0x00000001
#define IMX_GPMI_STAT_PRESENT(v)                (((v) << 0) & IMX_GPMI_STAT_PRESENT_MASK)

#define IMX_GPMI_STAT_PRESENT_BV_UNAVAILABLE    0x0
#define IMX_GPMI_STAT_PRESENT_BV_AVAILABLE      0x1

/*
 * IMX_GPMI_VERSION - GPMI version register
 */
#define IMX_GPMI_VERSION_OFFSET                 (0xD0)

/*
 * IMX_GPMI_READ_DDR_DLL_CTRL - GPMI Double Rate Read DLL Control Register
 */
typedef union {
    uint32_t U;
    struct {
        unsigned ENABLE: 1;
        unsigned RESET: 1;
        unsigned SLV_FORCE_UPD: 1;
        unsigned SLV_DLY_TARGET: 4;
        unsigned GATE_UPDATE: 1;
        unsigned REFCLK_ON: 1;
        unsigned SLV_OVERRIDE: 1;
        unsigned SLV_OVERRIDE_VAL: 8;
        unsigned RSVD1: 2;
        unsigned SLV_UPDATE_INT: 8;
        unsigned REF_UPDATE_INT: 4;
    } B;
} imx_gpmi_read_ddr_dll_ctrl_t;

/* IMX_GPMI_READ_DDR_DLL_CTRL register */
#define IMX_GPMI_READ_DDR_DLL_CTRL_OFFSET                       (0x100)

/* IMX_GPMI_READ_DDR_DLL_CTRL bit-fields */
/* REF_UPDATE_INT */
#define IMX_GPMI_READ_DDR_DLL_CTRL_REF_UPDATE_INT_SHIFT         28
#define IMX_GPMI_READ_DDR_DLL_CTRL_REF_UPDATE_INT_MASK          0xF0000000
#define IMX_GPMI_READ_DDR_DLL_CTRL_REF_UPDATE_INT(v)            ((((uint32_t) v) << 28) & IMX_GPMI_READ_DDR_DLL_CTRL_REF_UPDATE_INT_MASK)

/* SLV_UPDATE_INT */
#define IMX_GPMI_READ_DDR_DLL_CTRL_SLV_UPDATE_INT_SHIFT         20
#define IMX_GPMI_READ_DDR_DLL_CTRL_SLV_UPDATE_INT_MASK          0x0FF00000
#define IMX_GPMI_READ_DDR_DLL_CTRL_SLV_UPDATE_INT(v)            (((v) << 20) & IMX_GPMI_READ_DDR_DLL_CTRL_SLV_UPDATE_INT_MASK)

/* RSVD1 */
#define IMX_GPMI_READ_DDR_DLL_CTRL_RSVD1_SHIFT                  18
#define IMX_GPMI_READ_DDR_DLL_CTRL_RSVD1_MASK                   0x000C0000
#define IMX_GPMI_READ_DDR_DLL_CTRL_RSVD1(v)                     (((v) << 18) & IMX_GPMI_READ_DDR_DLL_CTRL_RSVD1_MASK)

/* SLV_OVERRIDE_VAL */
#define IMX_GPMI_READ_DDR_DLL_CTRL_SLV_OVERRIDE_VAL_SHIFT       10
#define IMX_GPMI_READ_DDR_DLL_CTRL_SLV_OVERRIDE_VAL_MASK        0x0003FC00
#define IMX_GPMI_READ_DDR_DLL_CTRL_SLV_OVERRIDE_VAL(v)          (((v) << 10) & IMX_GPMI_READ_DDR_DLL_CTRL_SLV_OVERRIDE_VAL_MASK)

/* SLV_OVERRIDE */
#define IMX_GPMI_READ_DDR_DLL_CTRL_SLV_OVERRIDE_SHIFT           9
#define IMX_GPMI_READ_DDR_DLL_CTRL_SLV_OVERRIDE_MASK            0x00000200
#define IMX_GPMI_READ_DDR_DLL_CTRL_SLV_OVERRIDE(v)              (((v) << 9) & IMX_GPMI_READ_DDR_DLL_CTRL_SLV_OVERRIDE_MASK)

/* REFCLK_ON */
#define IMX_GPMI_READ_DDR_DLL_CTRL_REFCLK_ON_SHIFT              8
#define IMX_GPMI_READ_DDR_DLL_CTRL_REFCLK_ON_MASK               0x00000100
#define IMX_GPMI_READ_DDR_DLL_CTRL_REFCLK_ON(v)                 (((v) << 8) & IMX_GPMI_READ_DDR_DLL_CTRL_REFCLK_ON_MASK)

/* GATE_UPDATE */
#define IMX_GPMI_READ_DDR_DLL_CTRL_GATE_UPDATE_SHIFT            7
#define IMX_GPMI_READ_DDR_DLL_CTRL_GATE_UPDATE_MASK             0x00000080
#define IMX_GPMI_READ_DDR_DLL_CTRL_GATE_UPDATE(v)               (((v) << 7) & IMX_GPMI_READ_DDR_DLL_CTRL_GATE_UPDATE_MASK)

/* SLV_DLY_TARGET */
#define IMX_GPMI_READ_DDR_DLL_CTRL_SLV_DLY_TARGET_SHIFT         3
#define IMX_GPMI_READ_DDR_DLL_CTRL_SLV_DLY_TARGET_MASK          0x00000078
#define IMX_GPMI_READ_DDR_DLL_CTRL_SLV_DLY_TARGET(v)            (((v) << 3) & IMX_GPMI_READ_DDR_DLL_CTRL_SLV_DLY_TARGET_MASK)

/* SLV_FORCE_UPD */
#define IMX_GPMI_READ_DDR_DLL_CTRL_SLV_FORCE_UPD_SHIFT          2
#define IMX_GPMI_READ_DDR_DLL_CTRL_SLV_FORCE_UPD_MASK           0x00000004
#define IMX_GPMI_READ_DDR_DLL_CTRL_SLV_FORCE_UPD(v)             (((v) << 2) & IMX_GPMI_READ_DDR_DLL_CTRL_SLV_FORCE_UPD_MASK)

/* RESET */
#define IMX_GPMI_READ_DDR_DLL_CTRL_RESET_SHIFT                  1
#define IMX_GPMI_READ_DDR_DLL_CTRL_RESET_MASK                   0x00000002
#define IMX_GPMI_READ_DDR_DLL_CTRL_RESET(v)                     (((v) << 1) & IMX_GPMI_READ_DDR_DLL_CTRL_RESET_MASK)

/* ENABLE */
#define IMX_GPMI_READ_DDR_DLL_CTRL_ENABLE_SHIFT                 0
#define IMX_GPMI_READ_DDR_DLL_CTRL_ENABLE_MASK                  0x00000001
#define IMX_GPMI_READ_DDR_DLL_CTRL_ENABLE(v)                    (((v) << 0) & IMX_GPMI_READ_DDR_DLL_CTRL_ENABLE_MASK)

/*
 * IMX_GPMI_WRITE_DDR_DLL_CTRL - GPMI Double Rate Write DLL Control Register
 */
typedef union {
    uint32_t U;
    struct {
        unsigned ENABLE: 1;
        unsigned RESET: 1;
        unsigned SLV_FORCE_UPD: 1;
        unsigned SLV_DLY_TARGET: 4;
        unsigned GATE_UPDATE: 1;
        unsigned REFCLK_ON: 1;
        unsigned SLV_OVERRIDE: 1;
        unsigned SLV_OVERRIDE_VAL: 8;
        unsigned RSVD1: 2;
        unsigned SLV_UPDATE_INT: 8;
        unsigned REF_UPDATE_INT: 4;
    } B;
} imx_gpmi_write_ddr_dll_ctrl_t;

#define IMX_GPMI_WRITE_DDR_DLL_CTRL_OFFSET                      (0x110)

/* REF_UPDATE_INT */
#define IMX_GPMI_WRITE_DDR_DLL_CTRL_REF_UPDATE_INT_SHIFT        28
#define IMX_GPMI_WRITE_DDR_DLL_CTRL_REF_UPDATE_INT_MASK         0xF0000000
#define IMX_GPMI_WRITE_DDR_DLL_CTRL_REF_UPDATE_INT(v)           ((((uint32_t) v) << 28) & IMX_GPMI_WRITE_DDR_DLL_CTRL_REF_UPDATE_INT_MASK)

/* SLV_UPDATE_INT */
#define IMX_GPMI_WRITE_DDR_DLL_CTRL_SLV_UPDATE_INT_SHIFT        20
#define IMX_GPMI_WRITE_DDR_DLL_CTRL_SLV_UPDATE_INT_MASK         0x0FF00000
#define IMX_GPMI_WRITE_DDR_DLL_CTRL_SLV_UPDATE_INT(v)           (((v) << 20) & IMX_GPMI_WRITE_DDR_DLL_CTRL_SLV_UPDATE_INT_MASK)

/* RSVD1 */
#define IMX_GPMI_WRITE_DDR_DLL_CTRL_RSVD1_SHIFT                 18
#define IMX_GPMI_WRITE_DDR_DLL_CTRL_RSVD1_MASK                  0x000C0000
#define IMX_GPMI_WRITE_DDR_DLL_CTRL_RSVD1(v)                    (((v) << 18) & IMX_GPMI_WRITE_DDR_DLL_CTRL_RSVD1_MASK)

/* SLV_OVERRIDE_VAL */
#define IMX_GPMI_WRITE_DDR_DLL_CTRL_SLV_OVERRIDE_VAL_SHIFT      10
#define IMX_GPMI_WRITE_DDR_DLL_CTRL_SLV_OVERRIDE_VAL_MASK       0x0003FC00
#define IMX_GPMI_WRITE_DDR_DLL_CTRL_SLV_OVERRIDE_VAL(v)         (((v) << 10) & IMX_GPMI_WRITE_DDR_DLL_CTRL_SLV_OVERRIDE_VAL_MASK)

/* SLV_OVERRIDE */
#define IMX_GPMI_WRITE_DDR_DLL_CTRL_SLV_OVERRIDE_SHIFT          9
#define IMX_GPMI_WRITE_DDR_DLL_CTRL_SLV_OVERRIDE_MASK           0x00000200
#define IMX_GPMI_WRITE_DDR_DLL_CTRL_SLV_OVERRIDE(v)             (((v) << 9) & IMX_GPMI_WRITE_DDR_DLL_CTRL_SLV_OVERRIDE_MASK)

/* REFCLK_ON */
#define IMX_GPMI_WRITE_DDR_DLL_CTRL_REFCLK_ON_SHIFT             8
#define IMX_GPMI_WRITE_DDR_DLL_CTRL_REFCLK_ON_MASK              0x00000100
#define IMX_GPMI_WRITE_DDR_DLL_CTRL_REFCLK_ON(v)                (((v) << 8) & IMX_GPMI_WRITE_DDR_DLL_CTRL_REFCLK_ON_MASK)

/* GATE_UPDATE */
#define IMX_GPMI_WRITE_DDR_DLL_CTRL_GATE_UPDATE_SHIFT           7
#define IMX_GPMI_WRITE_DDR_DLL_CTRL_GATE_UPDATE_MASK            0x00000080
#define IMX_GPMI_WRITE_DDR_DLL_CTRL_GATE_UPDATE(v)              (((v) << 7) & IMX_GPMI_WRITE_DDR_DLL_CTRL_GATE_UPDATE_MASK)

/* SLV_DLY_TARGET */
#define IMX_GPMI_WRITE_DDR_DLL_CTRL_SLV_DLY_TARGET_SHIFT        3
#define IMX_GPMI_WRITE_DDR_DLL_CTRL_SLV_DLY_TARGET_MASK         0x00000078
#define IMX_GPMI_WRITE_DDR_DLL_CTRL_SLV_DLY_TARGET(v)           (((v) << 3) & IMX_GPMI_WRITE_DDR_DLL_CTRL_SLV_DLY_TARGET_MASK)

/* SLV_FORCE_UPD */
#define IMX_GPMI_WRITE_DDR_DLL_CTRL_SLV_FORCE_UPD_SHIFT         2
#define IMX_GPMI_WRITE_DDR_DLL_CTRL_SLV_FORCE_UPD_MASK          0x00000004
#define IMX_GPMI_WRITE_DDR_DLL_CTRL_SLV_FORCE_UPD(v)            (((v) << 2) & IMX_GPMI_WRITE_DDR_DLL_CTRL_SLV_FORCE_UPD_MASK)

/* RESET */
#define IMX_GPMI_WRITE_DDR_DLL_CTRL_RESET_SHIFT                 1
#define IMX_GPMI_WRITE_DDR_DLL_CTRL_RESET_MASK                  0x00000002
#define IMX_GPMI_WRITE_DDR_DLL_CTRL_RESET(v)                    (((v) << 1) & IMX_GPMI_WRITE_DDR_DLL_CTRL_RESET_MASK)

/* ENABLE */
#define IMX_GPMI_WRITE_DDR_DLL_CTRL_ENABLE_SHIFT                0
#define IMX_GPMI_WRITE_DDR_DLL_CTRL_ENABLE_MASK                 0x00000001
#define IMX_GPMI_WRITE_DDR_DLL_CTRL_ENABLE(v)                   (((v) << 0) & IMX_GPMI_WRITE_DDR_DLL_CTRL_ENABLE_MASK)

/*
 * IMX_GPMI_READ_DDR_DLL_STS - GPMI Double Rate Read DLL Status Register
 */
typedef union {
    uint32_t U;
    struct {
        unsigned SLV_LOCK: 1;
        unsigned SLV_SEL: 8;
        unsigned RSVD0: 7;
        unsigned REF_LOCK: 1;
        unsigned REF_SEL: 8;
        unsigned RSVD1: 7;
    } B;
} imx_gpmi_read_ddr_dll_sts_t;

#define IMX_GPMI_READ_DDR_DLL_STS_ADDR                  (0x120)

/* IMX_GPMI_READ_DDR_DLL_STS bit-fields */
/* RSVD1 */
#define IMX_GPMI_READ_DDR_DLL_STS_RSVD1_SHIFT           25
#define IMX_GPMI_READ_DDR_DLL_STS_RSVD1_MASK            0xFE000000
#define IMX_GPMI_READ_DDR_DLL_STS_RSVD1(v)              (((v) << 25) & IMX_GPMI_READ_DDR_DLL_STS_RSVD1_MASK)

/* REF_SEL */
#define IMX_GPMI_READ_DDR_DLL_STS_REF_SEL_SHIFT         17
#define IMX_GPMI_READ_DDR_DLL_STS_REF_SEL_MASK          0x01FE0000
#define IMX_GPMI_READ_DDR_DLL_STS_REF_SEL(v)            (((v) << 17) & IMX_GPMI_READ_DDR_DLL_STS_REF_SEL_MASK)

/* REF_LOCK */
#define IMX_GPMI_READ_DDR_DLL_STS_REF_LOCK_SHIFT        16
#define IMX_GPMI_READ_DDR_DLL_STS_REF_LOCK_MASK         0x00010000
#define IMX_GPMI_READ_DDR_DLL_STS_REF_LOCK(v)           (((v) << 16) & IMX_GPMI_READ_DDR_DLL_STS_REF_LOCK_MASK)

/* RSVD0 */
#define IMX_GPMI_READ_DDR_DLL_STS_RSVD0_SHIFT           9
#define IMX_GPMI_READ_DDR_DLL_STS_RSVD0_MASK            0x0000FE00
#define IMX_GPMI_READ_DDR_DLL_STS_RSVD0(v)              (((v) << 9) & IMX_GPMI_READ_DDR_DLL_STS_RSVD0_MASK)

/* SLV_SEL */
#define IMX_GPMI_READ_DDR_DLL_STS_SLV_SEL_SHIFT         1
#define IMX_GPMI_READ_DDR_DLL_STS_SLV_SEL_MASK          0x000001FE
#define IMX_GPMI_READ_DDR_DLL_STS_SLV_SEL(v)            (((v) << 1) & IMX_GPMI_READ_DDR_DLL_STS_SLV_SEL_MASK)

/* SLV_LOCK */
#define IMX_GPMI_READ_DDR_DLL_STS_SLV_LOCK_SHIFT        0
#define IMX_GPMI_READ_DDR_DLL_STS_SLV_LOCK_MASK         0x00000001
#define IMX_GPMI_READ_DDR_DLL_STS_SLV_LOCK(v)           (((v) << 0) & IMX_GPMI_READ_DDR_DLL_STS_SLV_LOCK_MASK)

/*
 * IMX_GPMI_WRITE_DDR_DLL_STS - GPMI Double Rate Write DLL Status Register
 */
typedef union {
    uint32_t U;
    struct {
        unsigned SLV_LOCK: 1;
        unsigned SLV_SEL: 8;
        unsigned RSVD0: 7;
        unsigned REF_LOCK: 1;
        unsigned REF_SEL: 8;
        unsigned RSVD1: 7;
    } B;
} imx_gpmi_write_ddr_dll_sts_t;

#define IMX_GPMI_WRITE_DDR_DLL_STS_OFFSET               (0x130)

/* IMX_GPMI_WRITE_DDR_DLL_STS bit-fields */
/* RSVD1 */
#define IMX_GPMI_WRITE_DDR_DLL_STS_RSVD1_SHIFT          25
#define IMX_GPMI_WRITE_DDR_DLL_STS_RSVD1_MASK           0xFE000000
#define IMX_GPMI_WRITE_DDR_DLL_STS_RSVD1(v)             (((v) << IMX_GPMI_WRITE_DDR_DLL_STS_RSVD1)_SHIFT & IMX_GPMI_WRITE_DDR_DLL_STS_RSVD1_MASK)

/* REF_SEL */
#define IMX_GPMI_WRITE_DDR_DLL_STS_REF_SEL_SHIFT        17
#define IMX_GPMI_WRITE_DDR_DLL_STS_REF_SEL_MASK         0x01FE0000
#define IMX_GPMI_WRITE_DDR_DLL_STS_REF_SEL(v)           (((v) << IMX_GPMI_WRITE_DDR_DLL_STS_REF_SEL)_SHIFT & IMX_GPMI_WRITE_DDR_DLL_STS_REF_SEL_MASK)

/* REF_LOCK */
#define IMX_GPMI_WRITE_DDR_DLL_STS_REF_LOCK_SHIFT       16
#define IMX_GPMI_WRITE_DDR_DLL_STS_REF_LOCK_MASK        0x00010000
#define IMX_GPMI_WRITE_DDR_DLL_STS_REF_LOCK(v)          (((v) << IMX_GPMI_WRITE_DDR_DLL_STS_REF_LOCK)_SHIFT & IMX_GPMI_WRITE_DDR_DLL_STS_REF_LOCK_MASK)

/* RSVD0 */
#define IMX_GPMI_WRITE_DDR_DLL_STS_RSVD0_SHIFT          9
#define IMX_GPMI_WRITE_DDR_DLL_STS_RSVD0_MASK           0x0000FE00
#define IMX_GPMI_WRITE_DDR_DLL_STS_RSVD0(v)             (((v) << IMX_GPMI_WRITE_DDR_DLL_STS_RSVD0)_SHIFT & IMX_GPMI_WRITE_DDR_DLL_STS_RSVD0_MASK)

/* SLV_SEL */
#define IMX_GPMI_WRITE_DDR_DLL_STS_SLV_SEL_SHIFT        1
#define IMX_GPMI_WRITE_DDR_DLL_STS_SLV_SEL_MASK         0x000001FE
#define IMX_GPMI_WRITE_DDR_DLL_STS_SLV_SEL(v)           ((v) << 1) & IMX_GPMI_WRITE_DDR_DLL_STS_SLV_SEL_MASK)

/* SLV_LOCK */
#define IMX_GPMI_WRITE_DDR_DLL_STS_SLV_LOCK_SHIFT       0
#define IMX_GPMI_WRITE_DDR_DLL_STS_SLV_LOCK_MASK        0x00000001
#define IMX_GPMI_WRITE_DDR_DLL_STS_SLV_LOCK(v)          (((v) << 0) & IMX_GPMI_WRITE_DDR_DLL_STS_SLV_LOCK_MASK)

#endif /* IMX_GPMI_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/aarch64/imx8xp/imx_gpmi.h $ $Rev: 850159 $")
#endif
