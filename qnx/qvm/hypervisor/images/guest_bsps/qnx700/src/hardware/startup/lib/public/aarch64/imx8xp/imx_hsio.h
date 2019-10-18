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

#ifndef IMX_HSIO_H_
#define IMX_HSIO_H_

/** PCIex1_CTRL0 register */
#define IMX_HSIO_PCIEX1_CTRL0                           0x00
#define IMX_HSIO_PCIEX1_CTRL0_DEVICE_TYPE_RC_SHIFT      26
#define IMX_HSIO_PCIEX1_CTRL0_DEVICE_TYPE_SHIFT         24
#define IMX_HSIO_PCIEX1_CTRL0_DEVICE_TYPE_MASK          (0x0F << 24)
#define IMX_HSIO_PCIEX1_CTRL0_PS_REVISION_ID_SHIFT      16
#define IMX_HSIO_PCIEX1_CTRL0_PS_REVISION_ID_MASK       (0xFF << 16)
#define IMX_HSIO_PCIEX1_CTRL0_PS_DEVICE_ID_SHIFT        0
#define IMX_HSIO_PCIEX1_CTRL0_PS_DEVICE_ID_MASK         (0xFFFF << 0)

/** PCIex1_CTRL1 register */
#define IMX_HSIO_PCIEX1_CTRL1                           0x04

/** PCIex1_CTRL2 register */
#define IMX_HSIO_PCIEX1_CTRL2                           0x08
#define IMX_HSIO_PCIEX1_CTRL2_GPR_CRS_CLEAR_SHIFT       26
#define IMX_HSIO_PCIEX1_CTRL2_GPR_CRS_CLEAR_MASK        (0x01 << 26)
#define IMX_HSIO_PCIEX1_CTRL2_POWER_UP_RST_N_SHIFT      23
#define IMX_HSIO_PCIEX1_CTRL2_POWER_UP_RST_N_MASK       (0x01 << 23)
#define IMX_HSIO_PCIEX1_CTRL2_PERST_N_SHIFT             22
#define IMX_HSIO_PCIEX1_CTRL2_PERST_N_MASK              (0x01 << 22)
#define IMX_HSIO_PCIEX1_CTRL2_BUTTON_RST_N_SHIFT        21
#define IMX_HSIO_PCIEX1_CTRL2_BUTTON_RST_N_MASK         (0x01 << 21)
#define IMX_HSIO_PCIEX1_CTRL2_DIAG_CTRL_BUS_SHIFT       17
#define IMX_HSIO_PCIEX1_CTRL2_DIAG_CTRL_BUS_MASK        (0x07 << 17)
#define IMX_HSIO_PCIEX1_CTRL2_DIAG_STATUS_BUS_SELECT_SHIFT 13
#define IMX_HSIO_PCIEX1_CTRL2_DIAG_STATUS_BUS_SELECT_MASK  (0x0F << 13)
#define IMX_HSIO_PCIEX1_CTRL2_APP_XFER_PENDING_SHIFT    11
#define IMX_HSIO_PCIEX1_CTRL2_APP_XFER_PENDING_MASK     (0x01 << 11)
#define IMX_HSIO_PCIEX1_CTRL2_APP_CLK_PM_EN_SHIFT       10
#define IMX_HSIO_PCIEX1_CTRL2_APP_CLK_PM_EN_MASK        (0x01 << 10)
#define IMX_HSIO_PCIEX1_CTRL2_APPS_PM_XMT_TURNOFF_SHIFT 9
#define IMX_HSIO_PCIEX1_CTRL2_APPS_PM_XMT_TURNOFF_MASK  (0x01 << 9)
#define IMX_HSIO_PCIEX1_CTRL2_APPS_PM_XMT_PME_SHIFT     8
#define IMX_HSIO_PCIEX1_CTRL2_APPS_PM_XMT_PME_MASK      (0x01 << 8)
#define IMX_HSIO_PCIEX1_CTRL2_APP_REQ_EXIT_L1_SHIFT     7
#define IMX_HSIO_PCIEX1_CTRL2_APP_REQ_EXIT_L1_MASK      (0x01 << 7)
#define IMX_HSIO_PCIEX1_CTRL2_APP_REQ_ENTR_L1_SHIFT     6
#define IMX_HSIO_PCIEX1_CTRL2_APP_REQ_ENTR_L1_MASK      (0x01 << 6)
#define IMX_HSIO_PCIEX1_CTRL2_APP_READY_ENTR_L23_SHIFT  5
#define IMX_HSIO_PCIEX1_CTRL2_APP_READY_ENTR_L23_MASK   (0x01 << 5)
#define IMX_HSIO_PCIEX1_CTRL2_APP_LTSSM_ENABLE_SHIFT    4
#define IMX_HSIO_PCIEX1_CTRL2_APP_LTSSM_ENABLE_MASK     (0x01 << 4)
#define IMX_HSIO_PCIEX1_CTRL2_APP_INIT_RST_SHIFT        3
#define IMX_HSIO_PCIEX1_CTRL2_APP_INIT_RST_MASK         (0x01 << 3)
#define IMX_HSIO_PCIEX1_CTRL2_APP_CLK_REQ_N_SHIFT       2
#define IMX_HSIO_PCIEX1_CTRL2_APP_CLK_REQ_N_MASK        (0x01 << 2)
#define IMX_HSIO_PCIEX1_CTRL2_SYS_INT_SHIFT             0
#define IMX_HSIO_PCIEX1_CTRL2_SYS_INT_MASK              (0x01 << 0)

/** PCIex1_STTS0 register */
#define IMX_HSIO_PCIEX1_STTS0                           0x0C
#define IMX_HSIO_PCIEX1_STTS0_PM_REQ_CORE_RST_MASK      (0x01 << 19)
#define IMX_HSIO_PCIEX1_STTS0_PM_REQ_CORE_RST_SHIFT     19

/** PCIex1_CTRL0 register */
#define IMX_HSIO_PHYX1_CTRL0                            0x00
#define IMX_HSIO_PHYX1_CTRL0_PIPE_LN2LK_SHIFT           13
#define IMX_HSIO_PHYX1_CTRL0_PIPE_LN2LK_MASK            (0x0F << 13)
#define IMX_HSIO_PHYX1_CTRL0_APB_RSTN_SHIFT             0
#define IMX_HSIO_PHYX1_CTRL0_APB_RSTN_MASK              (0x01 << 0)

/** PCIex1_PHYX1_STTS0 register */
#define IMX_HSIO_PHYX1_STTS0                            0x04
#define IMX_HSIO_PHYX1_STTS0_LANE0_RX_PLL_LOCK_SHIFT    5
#define IMX_HSIO_PHYX1_STTS0_LANE0_RX_PLL_LOCK_MASK     (0x01 << 5)
#define IMX_HSIO_PHYX1_STTS0_LANE0_TX_PLL_LOCK_SHIFT    4
#define IMX_HSIO_PHYX1_STTS0_LANE0_TX_PLL_LOCK_MASK     (0x01 << 4)

/** MISC_CTRL0 register */
#define IMX_HSIO_MISC_CTRL0                             0x00
#define IMX_HSIO_MISC_CTRL0_FAST_INIT_MASK              (0x01 << 11)
#define IMX_HSIO_MISC_CTRL0_FAST_INIT_SHIFT             11
#define IMX_HSIO_MISC_CTRL0_IOB_A_0_M1M0_MASK           (0x03 << 3)
#define IMX_HSIO_MISC_CTRL0_IOB_A_0_M1M0_SHIFT          3
#define IMX_HSIO_MISC_CTRL0_IOB_A_0_TXOE_MASK           (0x01 << 2)
#define IMX_HSIO_MISC_CTRL0_IOB_A_0_TXOE_SHIFT          2
#define IMX_HSIO_MISC_CTRL0_IOB_TXENA_MASK              (0x01 << 1)
#define IMX_HSIO_MISC_CTRL0_IOB_TXENA_SHIFT             1
#define IMX_HSIO_MISC_CTRL0_IOB_RXENA_MASK              (0x01 << 0)
#define IMX_HSIO_MISC_CTRL0_IOB_RXENA_SHIFT             0

#endif /* IMX_HSIO_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/aarch64/imx8xp/imx_hsio.h $ $Rev: 850159 $")
#endif
