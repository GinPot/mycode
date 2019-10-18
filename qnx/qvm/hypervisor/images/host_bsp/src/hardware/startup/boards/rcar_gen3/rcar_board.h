/*
* $QNXLicenseC:
* Copyright (c) 2017, QNX Software Systems. All Rights Reserved.
*
* You must obtain a written license from and pay applicable license fees to QNX
* Software Systems before you may reproduce, modify or distribute this software,
* or any work that includes all or part of this software. Free development
* licenses are available for evaluation and non-commercial purposes. For more
* information visit http://licensing.qnx.com or email licensing@qnx.com.
*
* This file may contain contributions from others. Please review this entire
* file for other proprietary rights or license notices, as well as the QNX
* Development Suite License Guide at http://licensing.qnx.com/license-guide/
* for other information.
* $
*/

#ifndef __RCAR_BOARD_H_INCLUDED
#define __RCAR_BOARD_H_INCLUDED

#define BIT(x)              (1 << x)
#define BITS(x,y)           (((2 << (y-x)) - 1) << x)

#define DISABLE             0
#define ENABLE              1

#define GPSR_IO             0 /* GPIO */
#define GPSR_IP             1 /* Peripheral Function, mux with IPSR */
#define GPSR_PF             2 /* Peripheral Function, no mux */

#define GPIO_IN             0
#define GPIO_OUT            1
#define GPIO_INT            2

#define PUEN_DISABLE        0
#define PUEN_ENABLE         0xFFFFFFFF

#define DRV(mask,shift)     (mask << shift)
#define DRV3                0x7
#define DRV2                0x3

#define DRVCAP(val, shift)  (val << shift)
#define DRV3_1_8            0x0
#define DRV3_2_8            0x1
#define DRV3_3_8            0x2
#define DRV3_1_2            0x3
#define DRV3_5_8            0x4
#define DRV3_6_8            0x5
#define DRV3_7_8            0x6
#define DRV3_FULL           0x7

#define DRV2_1_4            0x0
#define DRV2_1_2            0x1
#define DRV2_3_4            0x2
#define DRV2_FULL           0x3

#define POC_1_8V            0x0
#define POC_3_3V            0xFFFFFFFF

#define TDSEL(val, shift)   (val << shift)
#define TDSEL_10PF          0x0
#define TDSEL_20PF          0x2
#define TDSEL_30PF          0x1
#define TDSEL_40PF          0x3

/* Register Macros */
#define SMSTPCR(x)          RCAR_GEN3_CPG_SMSTPCR##x
#define SRCR(x)             RCAR_GEN3_CPG_SRCR##x
#define SRSTCLR(x)          RCAR_GEN3_CPG_SRSTCLR##x
#define GPSR(x)             RCAR_GEN3_PFC_GPSR##x
#define GPIO(x)             RCAR_GEN3_GPIO##x##_BASE
#define IPSR(x)             RCAR_GEN3_PFC_IPSR##x
#define PUEN(x)             RCAR_GEN3_PFC_PUEN##x
#define MODSEL(x)           RCAR_GEN3_PFC_MODSEL##x
#define DRVCTRL(x)          RCAR_GEN3_PFC_DRVCTRL##x
#define TDSELCTRL(x)        RCAR_GEN3_PFC_TDSELCTRL##x
#define POCCTRL(x)          RCAR_GEN3_PFC_POCCTRL##x
#define PWRONCR(x)          RCAR_GEN3_SYSC_PWRONCR##x
#define PWRSR(x)            RCAR_GEN3_SYSC_PWRSR##x
#define PWRER(x)            RCAR_GEN3_SYSC_PWRER##x

typedef enum {
    DEV_TIMER,
    DEV_SCIF,
    DEV_HSCIF,
    DEV_ETHERNET,
    DEV_USBH,
    DEV_USBD,
    DEV_USB3H,
    DEV_USB3D,
    DEV_MSIOF,
    DEV_I2C,
    DEV_DMAC,
    DEV_AUDIO,
    DEV_SDHI,
    DEV_MMCIF,
    DEV_PCI,
    DEV_SATA,
    DEV_THERMAL,
    DEV_DISPLAY,
    DEV_GRAPHICS,
    DEV_MLB,
    DEV_CAN,
    DEV_PWM,
    DEV_LAST,
} rcar_gen3_dev;

typedef struct rcar_ram_t {
    paddr_t base;                     /* memory channel base */
    paddr_t rank[RCAR_GEN3_MAX_RANK]; /* memory rank bases */
    paddr_t size;                     /* memory channel default size */
} rcar_ram;

typedef struct rcar_pin_t {
    uint32_t reg;
    uint32_t bits;
    uint32_t val;
} rcar_pin;

typedef struct rcar_gpio_t {
    uint32_t type;
    uint32_t reg;
    uint32_t val;
} rcar_gpio;

typedef struct rcar_pinmux_t {
    rcar_pin gpsr;
    union {
        rcar_pin    ipsr;
        rcar_gpio   gpio;
    };
} rcar_pinmux;

typedef struct rcar_pwr_t {
    uint32_t base;
    uint32_t setting;
    uint32_t status;
    uint32_t power_on;
} rcar_pwr;

typedef struct rcar_module_t {
    rcar_pin clk;
    uint32_t base;
} rcar_module;

typedef struct rcar_config_t
{
    rcar_pwr    *pwr;
    rcar_module *mod;
    rcar_pin    *cpg;
    rcar_pinmux *pinmux;   /* pinmux: gpsr, ipsr/gpio */
    rcar_pin    *settings; /* settings: puen, drvctrl, modsel */
    void        (*blk_config)(rcar_module *mod);
} rcar_config;

typedef struct {
    uint32_t base;
    unsigned irq;
} dev_info_t;

typedef struct {
    uint32_t pwr_base;      /* GPIO Module Base */
    uint16_t pwr_offset;    /* OUTDT Offset */
    uint16_t pwr_vdd;       /* Vdd pin */
    uint16_t pwr_if;        /* Vddqva pin */
    uint32_t pfc_base;      /* PFC Module Base */
    uint16_t pfc_offset;    /* POCCTRL Offset */
    uint16_t pfc_mask;      /* Pin mask */
    uint16_t pfc_shift;     /* Mask shift */
} sd_info_t;

extern rcar_config *rcar_get_board_config(size_t *num_devs);

#endif //__RCAR_BOARD_H_INCLUDED

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/boards/rcar_gen3/rcar_board.h $ $Rev: 847588 $")
#endif
