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

/*!
 * @file imx_usb3_non_core.h
 * @date date-of-generation
 * @brief USB3_NON_CORE memory map for QNX USB3_NON_CORE memory map for i.MX
 *
 * QNX USB3_NON_CORE memory map for i.MX
 */

#ifndef IMX_USB3_NON_CORE_H_
#define IMX_USB3_NON_CORE_H_

#include <stdint.h>


/* IMX_USB3_NON_CORE_CORE_CTRL11 - USB3 Core Control Register 11 */
typedef union IMX_USB3_NON_CORE_CORE_CTRL11_union_t {
    uint32_t R;
    struct {
        uint32_t MODE_STRAP  :3;                 /* Can only be changed when the pwr_sw_reset is 1. */
        uint32_t WAKEUP_INT_CLR :1;              /* Wakeup Interrupt Clear */
        uint32_t APP_CLK_125_ALWAYSON :1;        /* Set to always enable the 125MHz Clock */
        uint32_t LPM_CLK_ALWAYSON :1;            /* set to always enable lpm_clk */
        uint32_t XHC_D0_REQ  :1;                 /* Request Host mode transition to D0 state. Once asserted, shall be held high until xhc_d0_ack is asserted. */
        uint32_t MDCTRL_CLK_SEL :1;              /* OTG logic clock select signal */
        uint32_t OVERCURRENT_POLARITY :1;        /* Over Current Polarity */
        uint32_t OVERCURRENT_DISABLE :1;         /* 1 to disable overcurrent, since core is active low, so set to 1 will always set overcurrent_n to core. */
        uint32_t USB2_LOOPBACK_LOCAL :2;         /* usb2phy loopback test port. (NXP internal use only) */
        uint32_t HOST_SYSTEM_ERR_LOCAL :1;       /* write 1 will indicate host system error to usb3 core.( NXP internal use only) */
        uint32_t OTG_ID_WAKEUP_EN :1;            /* id wakeup interrupt enable, used only when usb3 controller is power down and usb2phy is power on. */
        uint32_t OTG_VBUS_WAKEUP_EN :1;          /* vbus/sessvalid wakeup interrupt enable, used only when usb3 controller is power down and usb2phy is power on */
        uint32_t OTG_WKDPDMCHG_EN :1;            /* dpdm wakeup interrupt enable, used only when usb3 controller is power down and usb2phy is power on */
        uint32_t WAKEUP_INT_STATUS :1;           /* Wakeup_int_status is read. (Read only, but can be written) */
        uint32_t _unused17   :9;                 /* Normal RW registers */
        uint32_t PHYAPB_SW_RESET :1;             /* software reset for usb3phy apb bus */
        uint32_t PHY_SW_RESET :1;                /* Softwrae reset for usb3 PHY */
        uint32_t _unused28   :1;                 /* Normal RW registers */
        uint32_t AXI_SW_RESET :1;                /* software reset for usb3 axi bus */
        uint32_t APB_SW_RESET :1;                /* software reset for usb3 core apb bus */
        uint32_t PWR_SW_RESET :1;                /* software reset for usb3 core */
    } B;
} IMX_USB3_NON_CORE_CORE_CTRL11_t;

/* Register CORE_CTRL11 - USB3 Core Control Register 11 */
#define IMX_USB3_NON_CORE_CORE_CTRL11_OFFSET     0
/* Field MODE_STRAP - Can only be changed when the pwr_sw_reset is 1. */
#define IMX_USB3_NON_CORE_CORE_CTRL11_MODE_STRAP_SHIFT 0
#define IMX_USB3_NON_CORE_CORE_CTRL11_MODE_STRAP_MASK 0x7
#define IMX_USB3_NON_CORE_CORE_CTRL11_MODE_STRAP(v) (((v) << 0) & 0x7)
/* Field WAKEUP_INT_CLR - Wakeup Interrupt Clear */
#define IMX_USB3_NON_CORE_CORE_CTRL11_WAKEUP_INT_CLR_SHIFT 3
#define IMX_USB3_NON_CORE_CORE_CTRL11_WAKEUP_INT_CLR_MASK 0x8
#define IMX_USB3_NON_CORE_CORE_CTRL11_WAKEUP_INT_CLR(v) (((v) << 0x3) & 0x8)
/* Field APP_CLK_125_ALWAYSON - Set to always enable the 125MHz Clock */
#define IMX_USB3_NON_CORE_CORE_CTRL11_APP_CLK_125_ALWAYSON_SHIFT 4
#define IMX_USB3_NON_CORE_CORE_CTRL11_APP_CLK_125_ALWAYSON_MASK 0x10
#define IMX_USB3_NON_CORE_CORE_CTRL11_APP_CLK_125_ALWAYSON(v) (((v) << 0x4) & 0x10)
/* Field LPM_CLK_ALWAYSON - set to always enable lpm_clk */
#define IMX_USB3_NON_CORE_CORE_CTRL11_LPM_CLK_ALWAYSON_SHIFT 5
#define IMX_USB3_NON_CORE_CORE_CTRL11_LPM_CLK_ALWAYSON_MASK 0x20
#define IMX_USB3_NON_CORE_CORE_CTRL11_LPM_CLK_ALWAYSON(v) (((v) << 0x5) & 0x20)
/* Field XHC_D0_REQ - Request Host mode transition to D0 state. Once asserted, shall be held high until xhc_d0_ack is asserted. */
#define IMX_USB3_NON_CORE_CORE_CTRL11_XHC_D0_REQ_SHIFT 6
#define IMX_USB3_NON_CORE_CORE_CTRL11_XHC_D0_REQ_MASK 0x40
#define IMX_USB3_NON_CORE_CORE_CTRL11_XHC_D0_REQ(v) (((v) << 0x6) & 0x40)
/* Field MDCTRL_CLK_SEL - OTG logic clock select signal */
#define IMX_USB3_NON_CORE_CORE_CTRL11_MDCTRL_CLK_SEL_SHIFT 7
#define IMX_USB3_NON_CORE_CORE_CTRL11_MDCTRL_CLK_SEL_MASK 0x80
#define IMX_USB3_NON_CORE_CORE_CTRL11_MDCTRL_CLK_SEL(v) (((v) << 0x7) & 0x80)
/* Field OVERCURRENT_POLARITY - Over Current Polarity */
#define IMX_USB3_NON_CORE_CORE_CTRL11_OVERCURRENT_POLARITY_SHIFT 8
#define IMX_USB3_NON_CORE_CORE_CTRL11_OVERCURRENT_POLARITY_MASK 0x100
#define IMX_USB3_NON_CORE_CORE_CTRL11_OVERCURRENT_POLARITY(v) (((v) << 0x8) & 0x100)
/* Field OVERCURRENT_DISABLE - 1 to disable overcurrent, since core is active low, so set to 1 will always set overcurrent_n to core. */
#define IMX_USB3_NON_CORE_CORE_CTRL11_OVERCURRENT_DISABLE_SHIFT 9
#define IMX_USB3_NON_CORE_CORE_CTRL11_OVERCURRENT_DISABLE_MASK 0x200
#define IMX_USB3_NON_CORE_CORE_CTRL11_OVERCURRENT_DISABLE(v) (((v) << 0x9) & 0x200)
/* Field USB2_LOOPBACK_LOCAL - usb2phy loopback test port. (NXP internal use only) */
#define IMX_USB3_NON_CORE_CORE_CTRL11_USB2_LOOPBACK_LOCAL_SHIFT 10
#define IMX_USB3_NON_CORE_CORE_CTRL11_USB2_LOOPBACK_LOCAL_MASK 0xC00
#define IMX_USB3_NON_CORE_CORE_CTRL11_USB2_LOOPBACK_LOCAL(v) (((v) << 0xA) & 0xC00)
/* Field HOST_SYSTEM_ERR_LOCAL - write 1 will indicate host system error to usb3 core.( NXP internal use only) */
#define IMX_USB3_NON_CORE_CORE_CTRL11_HOST_SYSTEM_ERR_LOCAL_SHIFT 12
#define IMX_USB3_NON_CORE_CORE_CTRL11_HOST_SYSTEM_ERR_LOCAL_MASK 0x1000
#define IMX_USB3_NON_CORE_CORE_CTRL11_HOST_SYSTEM_ERR_LOCAL(v) (((v) << 0xC) & 0x1000)
/* Field OTG_ID_WAKEUP_EN - id wakeup interrupt enable, used only when usb3 controller is power down and usb2phy is power on. */
#define IMX_USB3_NON_CORE_CORE_CTRL11_OTG_ID_WAKEUP_EN_SHIFT 13
#define IMX_USB3_NON_CORE_CORE_CTRL11_OTG_ID_WAKEUP_EN_MASK 0x2000
#define IMX_USB3_NON_CORE_CORE_CTRL11_OTG_ID_WAKEUP_EN(v) (((v) << 0xD) & 0x2000)
/* Field OTG_VBUS_WAKEUP_EN - vbus/sessvalid wakeup interrupt enable, used only when usb3 controller is power down and usb2phy is power on */
#define IMX_USB3_NON_CORE_CORE_CTRL11_OTG_VBUS_WAKEUP_EN_SHIFT 14
#define IMX_USB3_NON_CORE_CORE_CTRL11_OTG_VBUS_WAKEUP_EN_MASK 0x4000
#define IMX_USB3_NON_CORE_CORE_CTRL11_OTG_VBUS_WAKEUP_EN(v) (((v) << 0xE) & 0x4000)
/* Field OTG_WKDPDMCHG_EN - dpdm wakeup interrupt enable, used only when usb3 controller is power down and usb2phy is power on */
#define IMX_USB3_NON_CORE_CORE_CTRL11_OTG_WKDPDMCHG_EN_SHIFT 15
#define IMX_USB3_NON_CORE_CORE_CTRL11_OTG_WKDPDMCHG_EN_MASK 0x8000
#define IMX_USB3_NON_CORE_CORE_CTRL11_OTG_WKDPDMCHG_EN(v) (((v) << 0xF) & 0x8000)
/* Field WAKEUP_INT_STATUS - Wakeup_int_status is read. (Read only, but can be written) */
#define IMX_USB3_NON_CORE_CORE_CTRL11_WAKEUP_INT_STATUS_SHIFT 16
#define IMX_USB3_NON_CORE_CORE_CTRL11_WAKEUP_INT_STATUS_MASK 0x10000
#define IMX_USB3_NON_CORE_CORE_CTRL11_WAKEUP_INT_STATUS(v) (((v) << 0x10) & 0x10000)
/* Field PHYAPB_SW_RESET - software reset for usb3phy apb bus */
#define IMX_USB3_NON_CORE_CORE_CTRL11_PHYAPB_SW_RESET_SHIFT 26
#define IMX_USB3_NON_CORE_CORE_CTRL11_PHYAPB_SW_RESET_MASK 0x04000000
#define IMX_USB3_NON_CORE_CORE_CTRL11_PHYAPB_SW_RESET(v) (((v) << 0x1A) & 0x4000000)
/* Field PHY_SW_RESET - Softwrae reset for usb3 PHY */
#define IMX_USB3_NON_CORE_CORE_CTRL11_PHY_SW_RESET_SHIFT 27
#define IMX_USB3_NON_CORE_CORE_CTRL11_PHY_SW_RESET_MASK 0x8000000
#define IMX_USB3_NON_CORE_CORE_CTRL11_PHY_SW_RESET(v) (((v) << 0x1B) & 0x8000000)
/* Field AXI_SW_RESET - software reset for usb3 axi bus */
#define IMX_USB3_NON_CORE_CORE_CTRL11_AXI_SW_RESET_SHIFT 29
#define IMX_USB3_NON_CORE_CORE_CTRL11_AXI_SW_RESET_MASK 0x20000000
#define IMX_USB3_NON_CORE_CORE_CTRL11_AXI_SW_RESET(v) (((v) << 0x1D) & 0x20000000)
/* Field APB_SW_RESET - software reset for usb3 core apb bus */
#define IMX_USB3_NON_CORE_CORE_CTRL11_APB_SW_RESET_SHIFT 30
#define IMX_USB3_NON_CORE_CORE_CTRL11_APB_SW_RESET_MASK 0x40000000
#define IMX_USB3_NON_CORE_CORE_CTRL11_APB_SW_RESET(v) (((v) << 0x1E) & 0x40000000)
/* Field PWR_SW_RESET - software reset for usb3 core */
#define IMX_USB3_NON_CORE_CORE_CTRL11_PWR_SW_RESET_SHIFT 31
#define IMX_USB3_NON_CORE_CORE_CTRL11_PWR_SW_RESET_MASK 0x80000000
#define IMX_USB3_NON_CORE_CORE_CTRL11_PWR_SW_RESET(v) (((v) << 0x1F) & 0x80000000)

/* IMX_USB3_NON_CORE_CORE_CTRL12 - Core Control Register 12 */
typedef union IMX_USB3_NON_CORE_CORE_CTRL12_union_t {
    uint32_t R;
    struct {
        uint32_t XHCI_DEBUG_SEL :5;              /* Internal Use */
        uint32_t _unused5    :27;                /* Reserved */
    } B;
} IMX_USB3_NON_CORE_CORE_CTRL12_t;

/* Register CORE_CTRL12 - Core Control Register 12 */
#define IMX_USB3_NON_CORE_CORE_CTRL12_OFFSET     0x4
/* Field XHCI_DEBUG_SEL - Internal Use */
#define IMX_USB3_NON_CORE_CORE_CTRL12_XHCI_DEBUG_SEL_SHIFT 0
#define IMX_USB3_NON_CORE_CORE_CTRL12_XHCI_DEBUG_SEL_MASK 0x1F
#define IMX_USB3_NON_CORE_CORE_CTRL12_XHCI_DEBUG_SEL(v) (((v) << 0) & 0x1F)

/* IMX_USB3_NON_CORE_INT - USB3 INT regster */
typedef union IMX_USB3_NON_CORE_INT_union_t {
    uint32_t R;
    struct {
        uint32_t XHCI_INT_EN :8;                 /* xHCI Interrupt Enable */
        uint32_t DEV_INT_EN  :2;                 /* Device Interrupts Enable */
        uint32_t OTG_INT_EN  :1;                 /* OTG Interrupt Enable */
        uint32_t LTM_HOST_EN :1;                 /* LTM(Latency Tolerance messaging) request Interrupt Enable */
        uint32_t OTG_WAKEUP_EN :1;               /* OTG Wakeup Enable */
        uint32_t _unused13   :1;                 /* Reserved */
        uint32_t DEVU3_WAKEUP_EN :1;             /* Device U3 Wakeup Enable */
        uint32_t DEV_WAKEUP  :1;                 /* connect to wakeup of device core */
        uint32_t INTERRUPT_REQ :8;               /* xHCI Interrupts */
        uint32_t DEV_IRQS    :2;                 /* Device interrupts */
        uint32_t OTGIRQ      :1;                 /* OTG interrupts */
        uint32_t LTM_HOST_REQ :1;                /* LTM Request Interrupt */
        uint32_t LPM_HOST_REQ :1;                /* indicate usb3core request lpm_clkc */
        uint32_t CLK_125_REQ :1;                 /* indicate usb3core request 125MHz clock */
        uint32_t PHY_REFCLK_REQ :1;              /* indicate PHY request reference clock */
        uint32_t _unused31   :1;                 /* Reserved */
    } B;
} IMX_USB3_NON_CORE_INT_t;

/* Register INT - USB3 INT regster */
#define IMX_USB3_NON_CORE_INT_OFFSET             0x8
/* Field XHCI_INT_EN - xHCI Interrupt Enable */
#define IMX_USB3_NON_CORE_INT_XHCI_INT_EN_SHIFT  0
#define IMX_USB3_NON_CORE_INT_XHCI_INT_EN_MASK   0xFF
#define IMX_USB3_NON_CORE_INT_XHCI_INT_EN(v)     (((v) << 0) & 0xFF)
/* Field DEV_INT_EN - Device Interrupts Enable */
#define IMX_USB3_NON_CORE_INT_DEV_INT_EN_SHIFT   8
#define IMX_USB3_NON_CORE_INT_DEV_INT_EN_MASK    0x300
#define IMX_USB3_NON_CORE_INT_DEV_INT_EN(v)      (((v) << 0x8) & 0x300)
/* Field OTG_INT_EN - OTG Interrupt Enable */
#define IMX_USB3_NON_CORE_INT_OTG_INT_EN_SHIFT   10
#define IMX_USB3_NON_CORE_INT_OTG_INT_EN_MASK    0x400
#define IMX_USB3_NON_CORE_INT_OTG_INT_EN(v)      (((v) << 0xA) & 0x400)
/* Field LTM_HOST_EN - LTM(Latency Tolerance messaging) request Interrupt Enable */
#define IMX_USB3_NON_CORE_INT_LTM_HOST_EN_SHIFT  11
#define IMX_USB3_NON_CORE_INT_LTM_HOST_EN_MASK   0x800
#define IMX_USB3_NON_CORE_INT_LTM_HOST_EN(v)     (((v) << 0xB) & 0x800)
/* Field OTG_WAKEUP_EN - OTG Wakeup Enable */
#define IMX_USB3_NON_CORE_INT_OTG_WAKEUP_EN_SHIFT 12
#define IMX_USB3_NON_CORE_INT_OTG_WAKEUP_EN_MASK 0x1000
#define IMX_USB3_NON_CORE_INT_OTG_WAKEUP_EN(v)   (((v) << 0xC) & 0x1000)
/* Field DEVU3_WAKEUP_EN - Device U3 Wakeup Enable */
#define IMX_USB3_NON_CORE_INT_DEVU3_WAKEUP_EN_SHIFT 14
#define IMX_USB3_NON_CORE_INT_DEVU3_WAKEUP_EN_MASK 0x4000
#define IMX_USB3_NON_CORE_INT_DEVU3_WAKEUP_EN(v) (((v) << 0xE) & 0x4000)
/* Field DEV_WAKEUP - connect to wakeup of device core */
#define IMX_USB3_NON_CORE_INT_DEV_WAKEUP_SHIFT   15
#define IMX_USB3_NON_CORE_INT_DEV_WAKEUP_MASK    0x8000
#define IMX_USB3_NON_CORE_INT_DEV_WAKEUP(v)      (((v) << 0xF) & 0x8000)
/* Field INTERRUPT_REQ - xHCI Interrupts */
#define IMX_USB3_NON_CORE_INT_INTERRUPT_REQ_SHIFT 16
#define IMX_USB3_NON_CORE_INT_INTERRUPT_REQ_MASK 0xFF0000
#define IMX_USB3_NON_CORE_INT_INTERRUPT_REQ(v)   (((v) << 0x10) & 0xFF0000)
/* Field DEV_IRQS - Device interrupts */
#define IMX_USB3_NON_CORE_INT_DEV_IRQS_SHIFT     24
#define IMX_USB3_NON_CORE_INT_DEV_IRQS_MASK      0x3000000
#define IMX_USB3_NON_CORE_INT_DEV_IRQS(v)        (((v) << 0x18) & 0x3000000)
/* Field OTGIRQ - OTG interrupts */
#define IMX_USB3_NON_CORE_INT_OTGIRQ_SHIFT       26
#define IMX_USB3_NON_CORE_INT_OTGIRQ_MASK        0x4000000
#define IMX_USB3_NON_CORE_INT_OTGIRQ(v)          (((v) << 0x1A) & 0x4000000)
/* Field LTM_HOST_REQ - LTM Request Interrupt */
#define IMX_USB3_NON_CORE_INT_LTM_HOST_REQ_SHIFT 27
#define IMX_USB3_NON_CORE_INT_LTM_HOST_REQ_MASK  0x8000000
#define IMX_USB3_NON_CORE_INT_LTM_HOST_REQ(v)    (((v) << 0x1B) & 0x8000000)
/* Field LPM_HOST_REQ - indicate usb3core request lpm_clkc */
#define IMX_USB3_NON_CORE_INT_LPM_HOST_REQ_SHIFT 28
#define IMX_USB3_NON_CORE_INT_LPM_HOST_REQ_MASK  0x10000000
#define IMX_USB3_NON_CORE_INT_LPM_HOST_REQ(v)    (((v) << 0x1C) & 0x10000000)
/* Field CLK_125_REQ - indicate usb3core request 125MHz clock */
#define IMX_USB3_NON_CORE_INT_CLK_125_REQ_SHIFT  29
#define IMX_USB3_NON_CORE_INT_CLK_125_REQ_MASK   0x20000000
#define IMX_USB3_NON_CORE_INT_CLK_125_REQ(v)     (((v) << 0x1D) & 0x20000000)
/* Field PHY_REFCLK_REQ - indicate PHY request reference clock */
#define IMX_USB3_NON_CORE_INT_PHY_REFCLK_REQ_SHIFT 30
#define IMX_USB3_NON_CORE_INT_PHY_REFCLK_REQ_MASK 0x40000000
#define IMX_USB3_NON_CORE_INT_PHY_REFCLK_REQ(v)  (((v) << 0x1E) & 0x40000000)

/* IMX_USB3_NON_CORE_CORE_STATUS - USB3 Core Status Register */
typedef union IMX_USB3_NON_CORE_CORE_STATUS_union_t {
    uint32_t R;
    struct {
        uint32_t LOWEST_BELT :12;                /* lowexst BELT value from xhci core */
        uint32_t XHCI_POWER_ON_READY :1;         /* xhci ready, SW should wait it to be 1 before access any xhci registers */
        uint32_t USBDEV_POWER_ON_READY :1;       /* device ready, SW should wait it to be 1 before access any device registers */
        uint32_t XHC_D0_ACK  :1;                 /* Acknowledge for D0 state entry request indicating that Host entered D0 state. Once asserted, will be held high until xhc_d0_req is de-asserted */
        uint32_t MDCTRL_CLK_STATUS :1;           /* Status from USBSS-DRD to indicate on which clock OTG logic is currently running. Change on this signal can be considered as an acknowledge for the mdctrl_clk_sel. */
        uint32_t _unused16   :16;                /* Reserved */
    } B;
} IMX_USB3_NON_CORE_CORE_STATUS_t;

/* Register CORE_STATUS - USB3 Core Status Regsiter */
#define IMX_USB3_NON_CORE_CORE_STATUS_OFFSET     0xC
/* Field LOWEST_BELT - lowexst BELT value from xhci core */
#define IMX_USB3_NON_CORE_CORE_STATUS_LOWEST_BELT_SHIFT 0
#define IMX_USB3_NON_CORE_CORE_STATUS_LOWEST_BELT_MASK 0xFFF
#define IMX_USB3_NON_CORE_CORE_STATUS_LOWEST_BELT(v) (((v) << 0) & 0xFFF)
/* Field XHCI_POWER_ON_READY - xhci ready, SW should wait it to be 1 before access any xhci registers */
#define IMX_USB3_NON_CORE_CORE_STATUS_XHCI_POWER_ON_READY_SHIFT 12
#define IMX_USB3_NON_CORE_CORE_STATUS_XHCI_POWER_ON_READY_MASK 0x1000
#define IMX_USB3_NON_CORE_CORE_STATUS_XHCI_POWER_ON_READY(v) (((v) << 0xC) & 0x1000)
/* Field USBDEV_POWER_ON_READY - device ready, SW should wait it to be 1 before access any device registers */
#define IMX_USB3_NON_CORE_CORE_STATUS_USBDEV_POWER_ON_READY_SHIFT 13
#define IMX_USB3_NON_CORE_CORE_STATUS_USBDEV_POWER_ON_READY_MASK 0x2000
#define IMX_USB3_NON_CORE_CORE_STATUS_USBDEV_POWER_ON_READY(v) (((v) << 0xD) & 0x2000)
/* Field XHC_D0_ACK - Acknowledge for D0 state entry request indicating that Host entered D0 state. Once asserted, will be held high until xhc_d0_req is de-asserted */
#define IMX_USB3_NON_CORE_CORE_STATUS_XHC_D0_ACK_SHIFT 14
#define IMX_USB3_NON_CORE_CORE_STATUS_XHC_D0_ACK_MASK 0x4000
#define IMX_USB3_NON_CORE_CORE_STATUS_XHC_D0_ACK(v) (((v) << 0xE) & 0x4000)
/* Field MDCTRL_CLK_STATUS - Status from USBSS-DRD to indicate on which clock OTG logic is currently running. Change on this signal can be considered as an acknowledge for the mdctrl_clk_sel. */
#define IMX_USB3_NON_CORE_CORE_STATUS_MDCTRL_CLK_STATUS_SHIFT 15
#define IMX_USB3_NON_CORE_CORE_STATUS_MDCTRL_CLK_STATUS_MASK 0x8000
#define IMX_USB3_NON_CORE_CORE_STATUS_MDCTRL_CLK_STATUS(v) (((v) << 0xF) & 0x8000)

/* IMX_USB3_NON_CORE_RESERVED - RESERVED */
typedef union IMX_USB3_NON_CORE_RESERVED_union_t {
    uint32_t R;
    struct {
        uint32_t _unused0    :32;                /* Reserved */
    } B;
} IMX_USB3_NON_CORE_RESERVED_t;

/* Register SSPHY_STATUS - USB3 SSPHY Status Register */
#define IMX_USB3_NON_CORE_SSPHY_STATUS_OFFSET               0x4C
#define IMX_USB3_NON_CORE_SSPHY_STATUS_ALLCLKS_VLD_MASK     0xF0000000


/* Register RESERVED - RESERVED */
#define IMX_USB3_NON_CORE_RESERVED_OFFSET        0x10

/* USB3_NON_CORE peripheral register structure */
typedef struct IMX_USB3_NON_CORE_struct_t {
    volatile IMX_USB3_NON_CORE_CORE_CTRL11_t CORE_CTRL11;   /* USB3 Core Control Register 11 */
    volatile IMX_USB3_NON_CORE_CORE_CTRL12_t CORE_CTRL12;   /* Core Control Register 12 */
    volatile IMX_USB3_NON_CORE_INT_t INT;                   /* USB3 INT regster */
    volatile IMX_USB3_NON_CORE_CORE_STATUS_t CORE_STATUS;   /* USB3 Core Status Regsiter */
    volatile IMX_USB3_NON_CORE_RESERVED_t RESERVED;         /* RESERVED */
} IMX_USB3_NON_CORE_t;



#endif /* IMX_USB3_NON_CORE_H */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/aarch64/imx8xp/imx_usb3_non_core.h $ $Rev: 850159 $")
#endif
