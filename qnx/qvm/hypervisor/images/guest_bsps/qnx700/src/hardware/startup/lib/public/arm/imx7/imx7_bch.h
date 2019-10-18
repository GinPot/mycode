/*
 * $QNXLicenseC:
 * Copyright 2017, QNX Software Systems.
 * Copyright 2016, Freescale Semiconductor, Inc.
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

#ifndef IMX7_BCH_H_
#define IMX7_BCH_H_

#include <stdint.h>

#ifndef IMX7_BCH_BASE
#define IMX7_BCH_BASE																		(0x33004000)
#endif

#ifndef IMX7_BCH_SIZE
#define IMX7_BCH_SIZE																		(0x200)
#endif

#ifndef IMX7_BCH_IRQ
#define IMX7_BCH_IRQ																		(46) //14+32
#endif

#define IMX7_BCH_MAX_ECC																62

/*
 * IMX7_BCH_CTRL - Hardware BCH ECC Accelerator Control Register
 */
typedef union {
    uint32_t U;
    struct {
        unsigned COMPLETE_IRQ:1;
        unsigned RSVD0:1;
        unsigned DEBUG_STALL_IRQ:1;
        unsigned BM_ERROR_IRQ:1;
        unsigned RSVD1:4;
        unsigned COMPLETE_IRQ_EN:1;
        unsigned RSVD2:1;
        unsigned DEBUG_STALL_IRQ_EN:1;
        unsigned RSVD3:5;
        unsigned M2M_ENABLE:1;
        unsigned M2M_ENCODE:1;
        unsigned M2M_LAYOUT:2;
        unsigned RSVD4:2;
        unsigned DEBUGSYNDROME:1;
        unsigned RSVD5:7;
        unsigned CLKGATE:1;
        unsigned SFTRST:1;
    } B;
} imx7_bch_ctrl_t;

#define IMX7_BCH_CTRL_OFFSET                             (0x0)
#define IMX7_BCH_CTRL_SET_OFFSET                         (0x4)
#define IMX7_BCH_CTRL_CLR_OFFSET                         (0x8)

/* IMX7_BCH_CTRL bit-fields */
/* SFTRST */
#define IMX7_BCH_CTRL_SFTRST_SHIFT                       31
#define IMX7_BCH_CTRL_SFTRST_MASK                        0x80000000
#define IMX7_BCH_CTRL_SFTRST(v)                          (((uint32_t) v) << 31) & IMX7_BCH_CTRL_SFTRST_MASK)
//
#define IMX7_BCH_CTRL_SFTRST_BV_RUN                      0x0
#define IMX7_BCH_CTRL_SFTRST_BV_RESET                    0x1

/* CLKGATE */
#define IMX7_BCH_CTRL_CLKGATE_SHIFT                      30
#define IMX7_BCH_CTRL_CLKGATE_MASK                       0x40000000
#define IMX7_BCH_CTRL_CLKGATE(v)                         (((v) << 30) & IMX7_BCH_CTRL_CLKGATE_MASK)
//
#define IMX7_BCH_CTRL_CLKGATE_BV_RUN                     0x0
#define IMX7_BCH_CTRL_CLKGATE_BV_NO_CLKS                 0x1

/* RSVD5 */
#define IMX7_BCH_CTRL_RSVD5_SHIFT                        23
#define IMX7_BCH_CTRL_RSVD5_MASK                         0x3F800000
#define IMX7_BCH_CTRL_RSVD5(v)                           (((v) << 23) & IMX7_BCH_CTRL_RSVD5_MASK)

/* DEBUGSYNDROME */
#define IMX7_BCH_CTRL_DEBUGSYNDROME_SHIFT                22
#define IMX7_BCH_CTRL_DEBUGSYNDROME_MASK                 0x00400000
#define IMX7_BCH_CTRL_DEBUGSYNDROME(v)                   (((v) << 22) & IMX7_BCH_CTRL_DEBUGSYNDROME_MASK)

/* RSVD4 */
#define IMX7_BCH_CTRL_RSVD4_SHIFT                        20
#define IMX7_BCH_CTRL_RSVD4_MASK                         0x00300000
#define IMX7_BCH_CTRL_RSVD4(v)                           (((v) << 20) & IMX7_BCH_CTRL_RSVD4_MASK)

/* M2M_LAYOUT */
#define IMX7_BCH_CTRL_M2M_LAYOUT_SHIFT                   18
#define IMX7_BCH_CTRL_M2M_LAYOUT_MASK                    0x000C0000
#define IMX7_BCH_CTRL_M2M_LAYOUT(v)                      (((v) << 18) & IMX7_BCH_CTRL_M2M_LAYOUT_MASK)

/* M2M_ENCODE */
#define IMX7_BCH_CTRL_M2M_ENCODE_SHIFT                   17
#define IMX7_BCH_CTRL_M2M_ENCODE_MASK                    0x00020000
#define IMX7_BCH_CTRL_M2M_ENCODE(v)                      (((v) << 17) & IMX7_BCH_CTRL_M2M_ENCODE_MASK)

/* M2M_ENABLE */
#define IMX7_BCH_CTRL_M2M_ENABLE_SHIFT                   16
#define IMX7_BCH_CTRL_M2M_ENABLE_MASK                    0x00010000
#define IMX7_BCH_CTRL_M2M_ENABLE(v)                      (((v) << 16) & IMX7_BCH_CTRL_M2M_ENABLE_MASK)

/* RSVD3 */
#define IMX7_BCH_CTRL_RSVD3_SHIFT                        11
#define IMX7_BCH_CTRL_RSVD3_MASK                         0x0000F800
#define IMX7_BCH_CTRL_RSVD3(v)                           (((v) << 11) & IMX7_BCH_CTRL_RSVD3_MASK)

/* DEBUG_STALL_IRQ_EN */
#define IMX7_BCH_CTRL_DEBUG_STALL_IRQ_EN_SHIFT           10
#define IMX7_BCH_CTRL_DEBUG_STALL_IRQ_EN_MASK            0x00000400
#define IMX7_BCH_CTRL_DEBUG_STALL_IRQ_EN(v)              (((v) << 10) & IMX7_BCH_CTRL_DEBUG_STALL_IRQ_EN_MASK)

/* RSVD2 */
#define IMX7_BCH_CTRL_RSVD2_SHIFT                        9
#define IMX7_BCH_CTRL_RSVD2_MASK                         0x00000200
#define IMX7_BCH_CTRL_RSVD2(v)                           (((v) << 9) & IMX7_BCH_CTRL_RSVD2_MASK)

/* COMPLETE_IRQ_EN */
#define IMX7_BCH_CTRL_COMPLETE_IRQ_EN_SHIFT              8
#define IMX7_BCH_CTRL_COMPLETE_IRQ_EN_MASK               0x00000100
#define IMX7_BCH_CTRL_COMPLETE_IRQ_EN(v)                 (((v) << 8) & IMX7_BCH_CTRL_COMPLETE_IRQ_EN_MASK)

/* RSVD1 */
#define IMX7_BCH_CTRL_RSVD1_SHIFT                        4
#define IMX7_BCH_CTRL_RSVD1_MASK                         0x000000F0
#define IMX7_BCH_CTRL_RSVD1(v)                           (((v) << 4) & IMX7_BCH_CTRL_RSVD1_MASK)

/* IMX7_ERROR_IRQ_MASK */
#define IMX7_BCH_CTRL_IMX7_ERROR_IRQ_SHIFT_MASK           3
#define IMX7_BCH_CTRL_BM_ERROR_IRQ_MASK                  0x00000008
#define IMX7_BCH_CTRL_IMX7_ERROR_IRQ(v_MASK)              (((v) << 3) & IMX7_BCH_CTRL_BM_ERROR_IRQ_MASK)

/* DEBUG_STALL_IRQ */
#define IMX7_BCH_CTRL_DEBUG_STALL_IRQ_SHIFT              2
#define IMX7_BCH_CTRL_DEBUG_STALL_IRQ_MASK               0x00000004
#define IMX7_BCH_CTRL_DEBUG_STALL_IRQ(v)                 (((v) << 2) & IMX7_BCH_CTRL_DEBUG_STALL_IRQ_MASK)

/* RSVD0 */
#define IMX7_BCH_CTRL_RSVD0_SHIFT                        1
#define IMX7_BCH_CTRL_RSVD0_MASK                         0x00000002
#define IMX7_BCH_CTRL_RSVD0(v)                           (((v) << 1) & IMX7_BCH_CTRL_RSVD0_MASK)

/* COMPLETE_IRQ */
#define IMX7_BCH_CTRL_COMPLETE_IRQ_SHIFT                 0
#define IMX7_BCH_CTRL_COMPLETE_IRQ_MASK                  0x00000001
#define IMX7_BCH_CTRL_COMPLETE_IRQ(v)                    (((v) << 0) & IMX7_BCH_CTRL_COMPLETE_IRQ_MASK)

/*
 * IMX7_BCH_STATUS0 - Hardware ECC Accelerator Status Register 0
 */
typedef union {
    uint32_t U;
    struct {
        unsigned RSVD0:2;
        unsigned UNCORRECTABLE:1;
        unsigned CORRECTED:1;
        unsigned ALLONES:1;
        unsigned RSVD1:3;
        unsigned STATUS_BLK0:8;
        unsigned COMPLETED_CE:4;
        unsigned HANDLE:12;
    } B;
} IMX7_bch_status0_t;

#define IMX7_BCH_STATUS0_OFFSET                          (0x10)

/* IMX7_BCH_STATUS0 bit-fields */
/* HANDLE */
#define IMX7_BCH_STATUS0_HANDLE_SHIFT                    20
#define IMX7_BCH_STATUS0_HANDLE_MASK                     0xFFF00000
#define IMX7_BCH_STATUS0_HANDLE(v)                       ((((uint32_t) v) << 20) & IMX7_BCH_STATUS0_HANDLE_MASK)

/* COMPLETED_CE */
#define IMX7_BCH_STATUS0_COMPLETED_CE_SHIFT              16
#define IMX7_BCH_STATUS0_COMPLETED_CE_MASK               0x000F0000
#define IMX7_BCH_STATUS0_COMPLETED_CE(v)                 (((v) << 16) & IMX7_BCH_STATUS0_COMPLETED_CE_MASK)

/* STATUS_BLK0 */
#define IMX7_BCH_STATUS0_STATUS_BLK0_SHIFT               8
#define IMX7_BCH_STATUS0_STATUS_BLK0_MASK                0x0000FF00
#define IMX7_BCH_STATUS0_STATUS_BLK0(v)                  (((v) << 8) & IMX7_BCH_STATUS0_STATUS_BLK0_MASK)
//
#define IMX7_BCH_STATUS0_STATUS_BLK0_BV_ZERO             0x00
#define IMX7_BCH_STATUS0_STATUS_BLK0_BV_ERROR1           0x01
#define IMX7_BCH_STATUS0_STATUS_BLK0_BV_ERROR2           0x02
#define IMX7_BCH_STATUS0_STATUS_BLK0_BV_ERROR3           0x03
#define IMX7_BCH_STATUS0_STATUS_BLK0_BV_ERROR4           0x04
#define IMX7_BCH_STATUS0_STATUS_BLK0_BV_UNCORRECTABLE    0xFE
#define IMX7_BCH_STATUS0_STATUS_BLK0_BV_ERASED           0xFF

/* RSVD1 */
#define IMX7_BCH_STATUS0_RSVD1_SHIFT                     5
#define IMX7_BCH_STATUS0_RSVD1_MASK                      0x000000E0
#define IMX7_BCH_STATUS0_RSVD1(v)                        (((v) << 5) & IMX7_BCH_STATUS0_RSVD1_MASK)

/* ALLONES */
#define IMX7_BCH_STATUS0_ALLONES_SHIFT                   4
#define IMX7_BCH_STATUS0_ALLONES_MASK                    0x00000010
#define IMX7_BCH_STATUS0_ALLONES(v)                      (((v) << 4) & IMX7_BCH_STATUS0_ALLONES_MASK)

/* CORRECTED */
#define IMX7_BCH_STATUS0_CORRECTED_SHIFT                 3
#define IMX7_BCH_STATUS0_CORRECTED_MASK                  0x00000008
#define IMX7_BCH_STATUS0_CORRECTED(v)                    (((v) << 3) & IMX7_BCH_STATUS0_CORRECTED_MASK)

/* UNCORRECTABLE */
#define IMX7_BCH_STATUS0_UNCORRECTABLE_SHIFT             2
#define IMX7_BCH_STATUS0_UNCORRECTABLE_MASK              0x00000004
#define IMX7_BCH_STATUS0_UNCORRECTABLE(v)                (((v) << 2) & IMX7_BCH_STATUS0_UNCORRECTABLE_MASK)

/* RSVD0 */
#define IMX7_BCH_STATUS0_RSVD0_SHIFT                     0
#define IMX7_BCH_STATUS0_RSVD0_MASK                      0x00000003
#define IMX7_BCH_STATUS0_RSVD0(v)                        (((v) << 0) & IMX7_BCH_STATUS0_RSVD0_MASK)

/*
 * IMX7_BCH_MODE - Hardware ECC Accelerator Mode Register
 */
typedef union {
    uint32_t U;
    struct {
        unsigned ERASE_THRESHOLD:8;
        unsigned RSVD:24;
    } B;
} imx7_bch_mode_t;

#define IMX7_BCH_MODE_OFFSET														(0x20)

/* IMX7_BCH_MODE bit-fields */
/* RSVD */
#define IMX7_BCH_MODE_RSVD_SHIFT								8
#define IMX7_BCH_MODE_RSVD_MASK									0xFFFFFF00
#define IMX7_BCH_MODE_RSVD(v)									((((uint32_t) v) << 8) & IMX7_BCH_MODE_RSVD_MASK)

/* ERASE_THRESHOLD */
#define IMX7_BCH_MODE_ERASE_THRESHOLD_SHIFT						0
#define IMX7_BCH_MODE_ERASE_THRESHOLD_MASK						0x000000FF
#define IMX7_BCH_MODE_ERASE_THRESHOLD(v)						(((v) << 0) & IMX7_BCH_MODE_ERASE_THRESHOLD_MASK)

/*
 * IMX7_BCH_ENCODEPTR - Hardware BCH ECC Loop-back Encode Buffer Register
 */
typedef union {
    uint32_t U;
    struct {
        unsigned ADDR:32;
    } B;
} imx7_bch_encodeptr_t;

#define IMX7_BCH_ENCODEPTR_OFFSET										(0x30)

/* IMX7_BCH_ENCODEPTR bit-fields */
/* ADDR */
#define IMX7_BCH_ENCODEPTR_ADDR_SHIFT									0
#define IMX7_BCH_ENCODEPTR_ADDR_MASK									0xFFFFFFFF
#define IMX7_BCH_ENCODEPTR_ADDR(v)										((uint32_t) v)

/*
 * IMX7_BCH_DATAPTR - Hardware BCH ECC Loop-back Data Buffer Register
 */
typedef union {
    uint32_t U;
    struct {
        unsigned ADDR:32;
    } B;
} imx7_bch_dataptr_t;

/*
 * constants & macros for entire IMX7_BCH_DATAPTR register
 */
#define IMX7_BCH_DATAPTR_OFFSET											(0x40)

/* IMX7_BCH_DATAPTR bit-fields */
/* ADDR */
#define IMX7_BCH_DATAPTR_ADDR_SHIFT										0
#define IMX7_BCH_DATAPTR_ADDR_MASK										0xFFFFFFFF
#define IMX7_BCH_DATAPTR_ADDR(v)										((uint32_t) v)

/*
 * IMX7_BCH_METAPTR - Hardware BCH ECC Loop-back Meta-data Buffer Register
 */
typedef union {
    uint32_t U;
    struct {
        unsigned ADDR:32;
    } B;
} imx7_bch_metaptr_t;

#define IMX7_BCH_METAPTR_OFFSET											(0x50)

/* IMX7_BCH_METAPTR bit-fields */
/* ADDR */
#define IMX7_BCH_METAPTR_ADDR_SHIFT										0
#define IMX7_BCH_METAPTR_ADDR_MASK										0xFFFFFFFF
#define IMX7_BCH_METAPTR_ADDR(v)										((uint32_t) v)

/*
 * IMX7_BCH_LAYOUTSELECT - Hardware ECC Accelerator Layout Select Register
 */
typedef union {
    uint32_t U;
    struct {
        unsigned CS0_SELECT:2;
        unsigned CS1_SELECT:2;
        unsigned CS2_SELECT:2;
        unsigned CS3_SELECT:2;
        unsigned CS4_SELECT:2;
        unsigned CS5_SELECT:2;
        unsigned CS6_SELECT:2;
        unsigned CS7_SELECT:2;
        unsigned CS8_SELECT:2;
        unsigned CS9_SELECT:2;
        unsigned CS10_SELECT:2;
        unsigned CS11_SELECT:2;
        unsigned CS12_SELECT:2;
        unsigned CS13_SELECT:2;
        unsigned CS14_SELECT:2;
        unsigned CS15_SELECT:2;
    } B;
} imx7_bch_layoutselect_t;

#define IMX7_BCH_LAYOUTSELECT_OFFSET							(0x70)

/* IMX7_BCH_LAYOUTSELECT bit-fields */
/* CS15_SELECT */
#define IMX7_BCH_LAYOUTSELECT_CS15_SELECT_SHIFT					30
#define IMX7_BCH_LAYOUTSELECT_CS15_SELECT_MASK					0xC0000000
#define IMX7_BCH_LAYOUTSELECT_CS15_SELECT(v)					((((uint32_t) v) << 30) & IMX7_BCH_LAYOUTSELECT_CS15_SELECT_MASK)

/* CS14_SELECT */
#define IMX7_BCH_LAYOUTSELECT_CS14_SELECT_SHIFT					28
#define IMX7_BCH_LAYOUTSELECT_CS14_SELECT_MASK					0x30000000
#define IMX7_BCH_LAYOUTSELECT_CS14_SELECT(v)					(((v) << 28) & IMX7_BCH_LAYOUTSELECT_CS14_SELECT_MASK)

/* CS13_SELECT */
#define IMX7_BCH_LAYOUTSELECT_CS13_SELECT_SHIFT					26
#define IMX7_BCH_LAYOUTSELECT_CS13_SELECT_MASK					0x0C000000
#define IMX7_BCH_LAYOUTSELECT_CS13_SELECT(v)					(((v) << 26) & IMX7_BCH_LAYOUTSELECT_CS13_SELECT_MASK)

/* CS12_SELECT */
#define IMX7_BCH_LAYOUTSELECT_CS12_SELECT_SHIFT					24
#define IMX7_BCH_LAYOUTSELECT_CS12_SELECT_MASK					0x03000000
#define IMX7_BCH_LAYOUTSELECT_CS12_SELECT(v)					(((v) << 24) & IMX7_BCH_LAYOUTSELECT_CS12_SELECT_MASK)

/* CS11_SELECT */
#define IMX7_BCH_LAYOUTSELECT_CS11_SELECT_SHIFT					22
#define IMX7_BCH_LAYOUTSELECT_CS11_SELECT_MASK					0x00C00000
#define IMX7_BCH_LAYOUTSELECT_CS11_SELECT(v)					(((v) << 22) & IMX7_BCH_LAYOUTSELECT_CS11_SELECT_MASK)

/* CS10_SELECT */
#define IMX7_BCH_LAYOUTSELECT_CS10_SELECT_SHIFT					20
#define IMX7_BCH_LAYOUTSELECT_CS10_SELECT_MASK					0x00300000
#define IMX7_BCH_LAYOUTSELECT_CS10_SELECT(v)					(((v) << 20) & IMX7_BCH_LAYOUTSELECT_CS10_SELECT_MASK)

/* CS9_SELECT */
#define IMX7_BCH_LAYOUTSELECT_CS9_SELECT_SHIFT					18
#define IMX7_BCH_LAYOUTSELECT_CS9_SELECT_MASK					0x000C0000
#define IMX7_BCH_LAYOUTSELECT_CS9_SELECT(v)						(((v) << 18) & IMX7_BCH_LAYOUTSELECT_CS9_SELECT_MASK)

/* CS8_SELECT */
#define IMX7_BCH_LAYOUTSELECT_CS8_SELECT_SHIFT					16
#define IMX7_BCH_LAYOUTSELECT_CS8_SELECT_MASK					0x00030000
#define IMX7_BCH_LAYOUTSELECT_CS8_SELECT(v)						(((v) << 16) & IMX7_BCH_LAYOUTSELECT_CS8_SELECT_MASK)

/* CS7_SELECT */
#define IMX7_BCH_LAYOUTSELECT_CS7_SELECT_SHIFT					14
#define IMX7_BCH_LAYOUTSELECT_CS7_SELECT_MASK					0x0000C000
#define IMX7_BCH_LAYOUTSELECT_CS7_SELECT(v)						(((v) << 14) & IMX7_BCH_LAYOUTSELECT_CS7_SELECT_MASK)

/* CS6_SELECT */
#define IMX7_BCH_LAYOUTSELECT_CS6_SELECT_SHIFT					12
#define IMX7_BCH_LAYOUTSELECT_CS6_SELECT_MASK					0x00003000
#define IMX7_BCH_LAYOUTSELECT_CS6_SELECT(v)						(((v) << 12) & IMX7_BCH_LAYOUTSELECT_CS6_SELECT_MASK)

/* CS5_SELECT */
#define IMX7_BCH_LAYOUTSELECT_CS5_SELECT_SHIFT					10
#define IMX7_BCH_LAYOUTSELECT_CS5_SELECT_MASK					0x00000C00
#define IMX7_BCH_LAYOUTSELECT_CS5_SELECT(v)						(((v) << 10) & IMX7_BCH_LAYOUTSELECT_CS5_SELECT_MASK)

/* CS4_SELECT */
#define IMX7_BCH_LAYOUTSELECT_CS4_SELECT_SHIFT					8
#define IMX7_BCH_LAYOUTSELECT_CS4_SELECT_MASK					0x00000300
#define IMX7_BCH_LAYOUTSELECT_CS4_SELECT(v)						(((v) << 8) & IMX7_BCH_LAYOUTSELECT_CS4_SELECT_MASK)

/* CS3_SELECT */
#define IMX7_BCH_LAYOUTSELECT_CS3_SELECT_SHIFT					6
#define IMX7_BCH_LAYOUTSELECT_CS3_SELECT_MASK					0x000000C0
#define IMX7_BCH_LAYOUTSELECT_CS3_SELECT(v)						(((v) << 6) & IMX7_BCH_LAYOUTSELECT_CS3_SELECT_MASK)

/* CS2_SELECT */
#define IMX7_BCH_LAYOUTSELECT_CS2_SELECT_SHIFT					4
#define IMX7_BCH_LAYOUTSELECT_CS2_SELECT_MASK					0x00000030
#define IMX7_BCH_LAYOUTSELECT_CS2_SELECT(v)						(((v) << 4) & IMX7_BCH_LAYOUTSELECT_CS2_SELECT_MASK)

/* CS1_SELECT */
#define IMX7_BCH_LAYOUTSELECT_CS1_SELECT_SHIFT					2
#define IMX7_BCH_LAYOUTSELECT_CS1_SELECT_MASK					0x0000000C
#define IMX7_BCH_LAYOUTSELECT_CS1_SELECT(v)						(((v) << 2) & IMX7_BCH_LAYOUTSELECT_CS1_SELECT_MASK)

/* CS0_SELECT */
#define IMX7_BCH_LAYOUTSELECT_CS0_SELECT_SHIFT					0
#define IMX7_BCH_LAYOUTSELECT_CS0_SELECT_MASK					0x00000003
#define IMX7_BCH_LAYOUTSELECT_CS0_SELECT(v)						(((v) << 0) & IMX7_BCH_LAYOUTSELECT_CS0_SELECT_MASK)

/*
 * IMX7_BCH_FLASH0LAYOUT0 - Hardware BCH ECC Flash 0 Layout 0 Register
 */
typedef union {
    uint32_t U;
    struct {
        unsigned DATA0_SIZE:10;
        unsigned GF13_0_GF14_1:1;
        unsigned ECC0:5;
        unsigned META_SIZE:8;
        unsigned NBLOCKS:8;
    } B;
} imx7_bch_flash0layout0_t;

#define IMX7_BCH_FLASH0LAYOUT0_OFFSET							(0x80)

/* IMX7_BCH_FLASH0LAYOUT0 bit-fields */
/* NBLOCKS */
#define IMX7_BCH_FLASH0LAYOUT0_NBLOCKS_SHIFT					24
#define IMX7_BCH_FLASH0LAYOUT0_NBLOCKS_MASK						0xFF000000
#define IMX7_BCH_FLASH0LAYOUT0_NBLOCKS(v)						((((uint32_t) v) << 24) & IMX7_BCH_FLASH0LAYOUT0_NBLOCKS_MASK)
#define BW_BCH_FLASH0LAYOUT0_NBLOCKS(v)							(IMX7_BCH_FLASH0LAYOUT0.B.NBLOCKS = (v))

/* META_SIZE */
#define IMX7_BCH_FLASH0LAYOUT0_META_SIZE_SHIFT					16
#define IMX7_BCH_FLASH0LAYOUT0_META_SIZE_MASK					0x00FF0000
#define IMX7_BCH_FLASH0LAYOUT0_META_SIZE(v)						(((v) << 16) & IMX7_BCH_FLASH0LAYOUT0_META_SIZE_MASK)
#define BW_BCH_FLASH0LAYOUT0_META_SIZE(v)						(IMX7_BCH_FLASH0LAYOUT0.B.META_SIZE = (v))

/* ECC0 */
#define IMX7_BCH_FLASH0LAYOUT0_ECC0_SHIFT						11
#define IMX7_BCH_FLASH0LAYOUT0_ECC0_MASK						0x0000F800
#define IMX7_BCH_FLASH0LAYOUT0_ECC0(v)							(((v) << 11) & IMX7_BCH_FLASH0LAYOUT0_ECC0_MASK)

/* GF13_0_GF14_1 */
#define IMX7_BCH_FLASH0LAYOUT0_GF13_0_GF14_1_SHIFT				10
#define IMX7_BCH_FLASH0LAYOUT0_GF13_0_GF14_1_MASK				0x00000400
#define IMX7_BCH_FLASH0LAYOUT0_GF13_0_GF14_1(v)					(((v) << 10) & IMX7_BCH_FLASH0LAYOUT0_GF13_0_GF14_1_MASK)

/* DATA0_SIZE */
#define IMX7_BCH_FLASH0LAYOUT0_DATA0_SIZE_SHIFT					0
#define IMX7_BCH_FLASH0LAYOUT0_DATA0_SIZE_MASK					0x000003FF
#define IMX7_BCH_FLASH0LAYOUT0_DATA0_SIZE(v)					(((v) << 0) & IMX7_BCH_FLASH0LAYOUT0_DATA0_SIZE_MASK)

/*
 * IMX7_BCH_FLASH0LAYOUT1 - Hardware BCH ECC Flash 0 Layout 1 Register
 */
typedef union {
    uint32_t U;
    struct {
        unsigned DATAN_SIZE:10;
        unsigned GF13_0_GF14_1:1;
        unsigned ECCN:5;
        unsigned PAGE_SIZE:16;
    } B;
} imx7_bch_flash0layout1_t;

#define IMX7_BCH_FLASH0LAYOUT1_OFFSET                    (0x90)

/* IMX7_BCH_FLASH0LAYOUT1 bit-fields */
/* PAGE_SIZE */
#define IMX7_BCH_FLASH0LAYOUT1_PAGE_SIZE_SHIFT           16
#define IMX7_BCH_FLASH0LAYOUT1_PAGE_SIZE_MASK            0xFFFF0000
#define IMX7_BCH_FLASH0LAYOUT1_PAGE_SIZE(v)              ((((uint32_t) v) << 16) & IMX7_BCH_FLASH0LAYOUT1_PAGE_SIZE_MASK)

/* ECCN */
#define IMX7_BCH_FLASH0LAYOUT1_ECCN_SHIFT                11
#define IMX7_BCH_FLASH0LAYOUT1_ECCN_MASK                 0x0000F800
#define IMX7_BCH_FLASH0LAYOUT1_ECCN(v)                   (((v) << 11) & IMX7_BCH_FLASH0LAYOUT1_ECCN_MASK)

/* GF13_0_GF14_1 */
#define IMX7_BCH_FLASH0LAYOUT1_GF13_0_GF14_1_SHIFT       10
#define IMX7_BCH_FLASH0LAYOUT1_GF13_0_GF14_1_MASK        0x00000400
#define IMX7_BCH_FLASH0LAYOUT1_GF13_0_GF14_1(v)          (((v) << 10) & IMX7_BCH_FLASH0LAYOUT1_GF13_0_GF14_1_MASK)

/* DATAN_SIZE */
#define IMX7_BCH_FLASH0LAYOUT1_DATAN_SIZE_SHIFT          0
#define IMX7_BCH_FLASH0LAYOUT1_DATAN_SIZE_MASK           0x000003FF
#define IMX7_BCH_FLASH0LAYOUT1_DATAN_SIZE(v)             (((v) << 0) & IMX7_BCH_FLASH0LAYOUT1_DATAN_SIZE_MASK)

/*
 * IMX7_BCH_FLASH1LAYOUT0 - Hardware BCH ECC Flash 1 Layout 0 Register
 */
typedef union {
    uint32_t U;
    struct {
        unsigned DATA0_SIZE:10;
        unsigned GF13_0_GF14_1:1;
        unsigned ECC0:5;
        unsigned META_SIZE:8;
        unsigned NBLOCKS:8;
    } B;
} imx7_bch_flash1layout0_t;

#define IMX7_BCH_FLASH1LAYOUT0_OFFSET                    (0xA0)

/* IMX7_BCH_FLASH1LAYOUT0 bit-fields */
/* NBLOCKS */
#define IMX7_BCH_FLASH1LAYOUT0_NBLOCKS_SHIFT             24
#define IMX7_BCH_FLASH1LAYOUT0_NBLOCKS_MASK              0xFF000000
#define IMX7_BCH_FLASH1LAYOUT0_NBLOCKS(v)                ((((uint32_t) v) << 24) & IMX7_BCH_FLASH1LAYOUT0_NBLOCKS_MASK)

/* META_SIZE */
#define IMX7_BCH_FLASH1LAYOUT0_META_SIZE_SHIFT           16
#define IMX7_BCH_FLASH1LAYOUT0_META_SIZE_MASK            0x00FF0000
#define IMX7_BCH_FLASH1LAYOUT0_META_SIZE(v)              (((v) << 16) & IMX7_BCH_FLASH1LAYOUT0_META_SIZE_MASK)

/* ECC0 */
#define IMX7_BCH_FLASH1LAYOUT0_ECC0_SHIFT                11
#define IMX7_BCH_FLASH1LAYOUT0_ECC0_MASK                 0x0000F800
#define IMX7_BCH_FLASH1LAYOUT0_ECC0(v)                   (((v) << 11) & IMX7_BCH_FLASH1LAYOUT0_ECC0_MASK)

/* GF13_0_GF14_1 */
#define IMX7_BCH_FLASH1LAYOUT0_GF13_0_GF14_1_SHIFT       10
#define IMX7_BCH_FLASH1LAYOUT0_GF13_0_GF14_1_MASK        0x00000400
#define IMX7_BCH_FLASH1LAYOUT0_GF13_0_GF14_1(v)          (((v) << 10) & IMX7_BCH_FLASH1LAYOUT0_GF13_0_GF14_1_MASK)

/* DATA0_SIZE */
#define IMX7_BCH_FLASH1LAYOUT0_DATA0_SIZE_SHIFT          0
#define IMX7_BCH_FLASH1LAYOUT0_DATA0_SIZE_MASK           0x000003FF
#define IMX7_BCH_FLASH1LAYOUT0_DATA0_SIZE(v)             (((v) << 0) & IMX7_BCH_FLASH1LAYOUT0_DATA0_SIZE_MASK)

/*
 * IMX7_BCH_FLASH1LAYOUT1 - Hardware BCH ECC Flash 1 Layout 1 Register
 */
typedef union {
    uint32_t U;
    struct {
        unsigned DATAN_SIZE:10;
        unsigned GF13_0_GF14_1:1;
        unsigned ECCN:5;
        unsigned PAGE_SIZE:16;
    } B;
} imx7_bch_flash1layout1_t;

#define IMX7_BCH_FLASH1LAYOUT1_OFFSET                    (0xB0)

/* IMX7_BCH_FLASH1LAYOUT1 bit-fields */
/* PAGE_SIZE */
#define IMX7_BCH_FLASH1LAYOUT1_PAGE_SIZE_SHIFT           16
#define IMX7_BCH_FLASH1LAYOUT1_PAGE_SIZE_MASK            0xFFFF0000
#define IMX7_BCH_FLASH1LAYOUT1_PAGE_SIZE(v)              ((((uint32_t) v) << 16) & IMX7_BCH_FLASH1LAYOUT1_PAGE_SIZE_MASK)

/* ECCN */
#define IMX7_BCH_FLASH1LAYOUT1_ECCN_SHIFT                11
#define IMX7_BCH_FLASH1LAYOUT1_ECCN_MASK                 0x0000F800
#define IMX7_BCH_FLASH1LAYOUT1_ECCN(v)                   (((v) << 11) & IMX7_BCH_FLASH1LAYOUT1_ECCN_MASK)

/* GF13_0_GF14_1 */
#define IMX7_BCH_FLASH1LAYOUT1_GF13_0_GF14_1_SHIFT       10
#define IMX7_BCH_FLASH1LAYOUT1_GF13_0_GF14_1_MASK        0x00000400
#define IMX7_BCH_FLASH1LAYOUT1_GF13_0_GF14_1(v)          (((v) << 10) & IMX7_BCH_FLASH1LAYOUT1_GF13_0_GF14_1_MASK)

/* DATAN_SIZE */
#define IMX7_BCH_FLASH1LAYOUT1_DATAN_SIZE_SHIFT          0
#define IMX7_BCH_FLASH1LAYOUT1_DATAN_SIZE_MASK           0x000003FF
#define IMX7_BCH_FLASH1LAYOUT1_DATAN_SIZE(v)             (((v) << 0) & IMX7_BCH_FLASH1LAYOUT1_DATAN_SIZE_MASK)

/*
 * IMX7_BCH_FLASH2LAYOUT0 - Hardware BCH ECC Flash 2 Layout 0 Register
 */
typedef union {
    uint32_t U;
    struct {
        unsigned DATA0_SIZE:10;
        unsigned GF13_0_GF14_1:1;
        unsigned ECC0:5;
        unsigned META_SIZE:8;
        unsigned NBLOCKS:8;
    } B;
} imx7_bch_flash2layout0_t;

#define IMX7_BCH_FLASH2LAYOUT0_OFFSET                    (0xC0)

/* IMX7_BCH_FLASH2LAYOUT0 bit-fields */
/* NBLOCKS */
#define IMX7_BCH_FLASH2LAYOUT0_NBLOCKS_SHIFT             24
#define IMX7_BCH_FLASH2LAYOUT0_NBLOCKS_MASK              0xFF000000
#define IMX7_BCH_FLASH2LAYOUT0_NBLOCKS(v)                ((((uint32_t) v) << 24) & IMX7_BCH_FLASH2LAYOUT0_NBLOCKS_MASK)

/* META_SIZE */
#define IMX7_BCH_FLASH2LAYOUT0_META_SIZE_SHIFT           16
#define IMX7_BCH_FLASH2LAYOUT0_META_SIZE_MASK            0x00FF0000
#define IMX7_BCH_FLASH2LAYOUT0_META_SIZE(v)              (((v) << 16) & IMX7_BCH_FLASH2LAYOUT0_META_SIZE_MASK)

/* ECC0 */
#define IMX7_BCH_FLASH2LAYOUT0_ECC0_SHIFT                11
#define IMX7_BCH_FLASH2LAYOUT0_ECC0_MASK                 0x0000F800
#define IMX7_BCH_FLASH2LAYOUT0_ECC0(v)                   (((v) << 11) & IMX7_BCH_FLASH2LAYOUT0_ECC0_MASK)

/* GF13_0_GF14_1 */
#define IMX7_BCH_FLASH2LAYOUT0_GF13_0_GF14_1_SHIFT       10
#define IMX7_BCH_FLASH2LAYOUT0_GF13_0_GF14_1_MASK        0x00000400
#define IMX7_BCH_FLASH2LAYOUT0_GF13_0_GF14_1(v)          (((v) << 10) & IMX7_BCH_FLASH2LAYOUT0_GF13_0_GF14_1_MASK)

/* DATA0_SIZE */
#define IMX7_BCH_FLASH2LAYOUT0_DATA0_SIZE_SHIFT          0
#define IMX7_BCH_FLASH2LAYOUT0_DATA0_SIZE_MASK           0x000003FF
#define IMX7_BCH_FLASH2LAYOUT0_DATA0_SIZE(v)             (((v) << 0) & IMX7_BCH_FLASH2LAYOUT0_DATA0_SIZE_MASK)

/*
 * IMX7_BCH_FLASH2LAYOUT1 - Hardware BCH ECC Flash 2 Layout 1 Register
 */
typedef union {
    uint32_t U;
    struct {
        unsigned DATAN_SIZE:10;
        unsigned GF13_0_GF14_1:1;
        unsigned ECCN:5;
        unsigned PAGE_SIZE:16;
    } B;
} imx7_bch_flash2layout1_t;

#define IMX7_BCH_FLASH2LAYOUT1_OFFSET    (0xD0)

/* IMX7_BCH_FLASH2LAYOUT1 bit-fields */
/* PAGE_SIZE */
#define IMX7_BCH_FLASH2LAYOUT1_PAGE_SIZE_SHIFT           16
#define IMX7_BCH_FLASH2LAYOUT1_PAGE_SIZE_MASK            0xFFFF0000
#define IMX7_BCH_FLASH2LAYOUT1_PAGE_SIZE(v)              ((((uint32_t) v) << 16) & IMX7_BCH_FLASH2LAYOUT1_PAGE_SIZE_MASK)

/* ECCN */
#define IMX7_BCH_FLASH2LAYOUT1_ECCN_SHIFT                11
#define IMX7_BCH_FLASH2LAYOUT1_ECCN_MASK                 0x0000F800
#define IMX7_BCH_FLASH2LAYOUT1_ECCN(v)                   (((v) << 11) & IMX7_BCH_FLASH2LAYOUT1_ECCN_MASK)

/* GF13_0_GF14_1 */
#define IMX7_BCH_FLASH2LAYOUT1_GF13_0_GF14_1_SHIFT       10
#define IMX7_BCH_FLASH2LAYOUT1_GF13_0_GF14_1_MASK        0x00000400
#define IMX7_BCH_FLASH2LAYOUT1_GF13_0_GF14_1(v)          (((v) << 10) & IMX7_BCH_FLASH2LAYOUT1_GF13_0_GF14_1_MASK)

/* DATAN_SIZE */
#define IMX7_BCH_FLASH2LAYOUT1_DATAN_SIZE_SHIFT          0
#define IMX7_BCH_FLASH2LAYOUT1_DATAN_SIZE_MASK           0x000003FF
#define IMX7_BCH_FLASH2LAYOUT1_DATAN_SIZE(v)             (((v) << 0) & IMX7_BCH_FLASH2LAYOUT1_DATAN_SIZE_MASK)

/*
 * IMX7_BCH_FLASH3LAYOUT0 - Hardware BCH ECC Flash 3 Layout 0 Register
 */
typedef union {
    uint32_t U;
    struct {
        unsigned DATA0_SIZE:10;
        unsigned GF13_0_GF14_1:1;
        unsigned ECC0:5;
        unsigned META_SIZE:8;
        unsigned NBLOCKS:8;
    } B;
} imx7_bch_flash3layout0_t;

/* IMX7_BCH_FLASH3LAYOUT0 register */
#define IMX7_BCH_FLASH3LAYOUT0_OFFSET                    (0xE0)

/* IMX7_BCH_FLASH3LAYOUT0 bit-fields */
/* NBLOCKS */
#define IMX7_BCH_FLASH3LAYOUT0_NBLOCKS_SHIFT             24
#define IMX7_BCH_FLASH3LAYOUT0_NBLOCKS_MASK              0xFF000000
#define IMX7_BCH_FLASH3LAYOUT0_NBLOCKS(v)                ((((uint32_t) v) << 24) & IMX7_BCH_FLASH3LAYOUT0_NBLOCKS_MASK)

/* META_SIZE */
#define IMX7_BCH_FLASH3LAYOUT0_META_SIZE_SHIFT           16
#define IMX7_BCH_FLASH3LAYOUT0_META_SIZE_MASK            0x00FF0000
#define IMX7_BCH_FLASH3LAYOUT0_META_SIZE(v)              (((v) << 16) & IMX7_BCH_FLASH3LAYOUT0_META_SIZE_MASK)

/* ECC0 */
#define IMX7_BCH_FLASH3LAYOUT0_ECC0_SHIFT                11
#define IMX7_BCH_FLASH3LAYOUT0_ECC0_MASK                 0x0000F800
#define IMX7_BCH_FLASH3LAYOUT0_ECC0(v)                   (((v) << 11) & IMX7_BCH_FLASH3LAYOUT0_ECC0_MASK)

/* GF13_0_GF14_1 */
#define IMX7_BCH_FLASH3LAYOUT0_GF13_0_GF14_1_SHIFT       10
#define IMX7_BCH_FLASH3LAYOUT0_GF13_0_GF14_1_MASK        0x00000400
#define IMX7_BCH_FLASH3LAYOUT0_GF13_0_GF14_1(v)          (((v) << 10) & IMX7_BCH_FLASH3LAYOUT0_GF13_0_GF14_1_MASK)

/* DATA0_SIZE */
#define IMX7_BCH_FLASH3LAYOUT0_DATA0_SIZE_SHIFT          0
#define IMX7_BCH_FLASH3LAYOUT0_DATA0_SIZE_MASK           0x000003FF
#define IMX7_BCH_FLASH3LAYOUT0_DATA0_SIZE(v)             (((v) << 0) & IMX7_BCH_FLASH3LAYOUT0_DATA0_SIZE_MASK)

/*
 * IMX7_BCH_FLASH3LAYOUT1 - Hardware BCH ECC Flash 3 Layout 1 Register
 */
typedef union {
    uint32_t U;
    struct {
        unsigned DATAN_SIZE:10;
        unsigned GF13_0_GF14_1:1;
        unsigned ECCN:5;
        unsigned PAGE_SIZE:16;
    } B;
} imx7_bch_flash3layout1_t;

#define IMX7_BCH_FLASH3LAYOUT1_OFFSET                    (0xF0)

/* IMX7_BCH_FLASH3LAYOUT1 bit-fields */
/* PAGE_SIZE */
#define IMX7_BCH_FLASH3LAYOUT1_PAGE_SIZE_SHIFT           16
#define IMX7_BCH_FLASH3LAYOUT1_PAGE_SIZE_MASK            0xFFFF0000
#define IMX7_BCH_FLASH3LAYOUT1_PAGE_SIZE(v)              ((((uint32_t) v) << 16) & IMX7_BCH_FLASH3LAYOUT1_PAGE_SIZE_MASK)

/* ECCN */
#define IMX7_BCH_FLASH3LAYOUT1_ECCN_SHIFT                11
#define IMX7_BCH_FLASH3LAYOUT1_ECCN_MASK                 0x0000F800
#define IMX7_BCH_FLASH3LAYOUT1_ECCN(v)                   (((v) << 11) & IMX7_BCH_FLASH3LAYOUT1_ECCN_MASK)

/* GF13_0_GF14_1 */
#define IMX7_BCH_FLASH3LAYOUT1_GF13_0_GF14_1_SHIFT       10
#define IMX7_BCH_FLASH3LAYOUT1_GF13_0_GF14_1_MASK        0x00000400
#define IMX7_BCH_FLASH3LAYOUT1_GF13_0_GF14_1(v)          (((v) << 10) & IMX7_BCH_FLASH3LAYOUT1_GF13_0_GF14_1_MASK)

/* DATAN_SIZE */
#define IMX7_BCH_FLASH3LAYOUT1_DATAN_SIZE_SHIFT          0
#define IMX7_BCH_FLASH3LAYOUT1_DATAN_SIZE_MASK           0x000003FF
#define IMX7_BCH_FLASH3LAYOUT1_DATAN_SIZE(v)             (((v) << 0) & IMX7_BCH_FLASH3LAYOUT1_DATAN_SIZE_MASK)

/*
 * IMX7_BCH_BLOCKNAME - Block Name Register
 */
typedef union {
    uint32_t U;
    struct {
        unsigned NAME:32;
    } B;
} imx7_bch_blockname_t;

/* IMX7_BCH_BLOCKNAME register */
#define IMX7_BCH_BLOCKNAME_ADDR							(0x150)

/* IMX7_BCH_BLOCKNAME bit-fields */
/* NAME */
#define IMX7_BCH_BLOCKNAME_NAME_SHIFT                    0
#define IMX7_BCH_BLOCKNAME_NAME_MASK                     0xFFFFFFFF
#define IMX7_BCH_BLOCKNAME_NAME(v)                       ((uint32_t) v)

/*
 * IMX7_BCH_VERSION - BCH Version Register
 */
typedef union {
    uint32_t U;
    struct {
        unsigned STEP:16;
        unsigned MINOR:8;
        unsigned MAJOR:8;
    } B;
} imx7_bch_version_t;

#define IMX7_BCH_VERSION_OFFSET							(0x160)

/* IMX7_BCH_VERSION bit-fields */
/* MAJOR */
#define IMX7_BCH_VERSION_MAJOR_SHIFT                     24
#define IMX7_BCH_VERSION_MAJOR_MASK                      0xFF000000
#define IMX7_BCH_VERSION_MAJOR(v)                        ((((uint32_t) v) << 24) & IMX7_BCH_VERSION_MAJOR_MASK)

/* MINOR */
#define IMX7_BCH_VERSION_MINOR_SHIFT                     16
#define IMX7_BCH_VERSION_MINOR_MASK                      0x00FF0000
#define IMX7_BCH_VERSION_MINOR(v)                        (((v) << 16) & IMX7_BCH_VERSION_MINOR_MASK)

/* STEP */
#define IMX7_BCH_VERSION_STEP_SHIFT                      0
#define IMX7_BCH_VERSION_STEP_MASK                       0x0000FFFF
#define IMX7_BCH_VERSION_STEP(v)                         (((v) << 0) & IMX7_BCH_VERSION_STEP_MASK)


#endif /* IMX7_BCH_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/arm/imx7/imx7_bch.h $ $Rev: 834524 $")
#endif
