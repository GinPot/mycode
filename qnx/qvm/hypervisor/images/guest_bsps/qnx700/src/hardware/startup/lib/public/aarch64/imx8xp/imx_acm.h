/*
 * $QNXLicenseC:
 * Copyright 2018, QNX Software Systems.
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

#ifndef IMX_ACM_H_
#define IMX_ACM_H_

/* ACM register offsets */
#define IMX_ACM_AUD_CLK0_CTL                             0x000000
#define IMX_ACM_AUD_CLK1_CTL                             0x010000
#define IMX_ACM_MCLKOUT0_CTL                             0x020000
#define IMX_ACM_MCLKOUT1_CTL                             0x030000
#define IMX_ACM_ASRC0_MUX_CLK_CTL                        0x040000
#define IMX_ACM_ASRC1_MUX_CLK_CTL                        0x050000
#define IMX_ACM_ESAI0_MCLK_CTL                           0x060000
/* Reserved                                              0x070000 */
#define IMX_ACM_GPT0_MUX_CLK_CTL                         0x080000
#define IMX_ACM_GPT1_MUX_CLK_CTL                         0x090000
#define IMX_ACM_GPT2_MUX_CLK_CTL                         0x0A0000
#define IMX_ACM_GPT3_MUX_CLK_CTL                         0x0B0000
#define IMX_ACM_GPT4_MUX_CLK_CTL                         0x0C0000
#define IMX_ACM_GPT5_MUX_CLK_CTL                         0x0D0000
#define IMX_ACM_SAI0_MCLK_CTL                            0x0E0000
#define IMX_ACM_SAI1_MCLK_CTL                            0x0F0000
#define IMX_ACM_SAI2_MCLK_CTL                            0x100000
#define IMX_ACM_SAI3_MCLK_CTL                            0x110000
/* Reserved                                              0x120000 */
/* Reserved                                              0x130000 */
#define IMX_ACM_SAI4_MCLK_CTL                            0x140000
#define IMX_ACM_SAI5_MCLK_CTL                            0x150000
/* Reserved                                              0x160000 */
/* Reserved                                              0x170000 */
/* Reserved                                              0x180000 */
/* Reserved                                              0x190000 */
#define IMX_ACM_SPDIF0_TX_CLK_CTL                        0x1A0000
/* Reserved                                              0x1B0000 */
#define IMX_ACM_MQS_TX_CLK_CTL                           0x1C0000

#define IMX_ACM_MUX_RESC_MASK                            0xFFFFFF
#define IMX_ACM_MUX_INIT(mux, sc_resource, sc_clock)     ((mux & 0xFF) | ((sc_resource & IMX_ACM_MUX_RESC_MASK) << 8) | (sc_clock << 24))
#define IMX_ACM_MUX_REG_VAL(enum_val)                    (enum_val & 0xFF)
#define IMX_ACM_MUX_RESC(enum_val)                       ((enum_val >> 8) & IMX_ACM_MUX_RESC_MASK)
#define IMX_ACM_MUX_CLK(enum_val)                        ((enum_val >> 24) & 0xFF)

typedef enum _acm_aud_clk_mux {
    aud_mux_aud_pll_div_clk2 = IMX_ACM_MUX_INIT(0, SC_R_AUDIO_PLL_0, SC_PM_CLK_MISC1),    /**< DSC clock output */
    aud_mux_aud_pll_div_clk3 = IMX_ACM_MUX_INIT(1, SC_R_AUDIO_PLL_1, SC_PM_CLK_MISC1),    /**< DSC clock output */
    aud_mux_hdmi_rx_mclk = IMX_ACM_MUX_INIT(2, SC_R_LAST, 0),
    aud_mux_ext_aud_mclk0,                                                                /**< External MCLK0 audio clock input from IOMUX */
    aud_mux_ext_aud_mclk1,                                                                /**< External MCLK1 audio clock input from IOMUX */
    aud_mux_esai0_rx_clk,                                                                 /**< ESAI0 Receiver external serial bit clock */
    aud_mux_esai0_rx_hf_clk,                                                              /**< ESAI0 Receiver external high frequency clock */
    aud_mux_esai0_tx_clk,                                                                 /**< ESAI0 Transmitter external serial bit clock */
    aud_mux_esai0_tx_hf_clk,                                                              /**< ESAI0 Transmitter external high frequency clock */
    aud_mux_spdif0_rx,                                                                    /**< SPDIF0 Receiver recovered clock */
    aud_mux_sai0_rx_bclk,                                                                 /**< SAI0 Receiver external serial bit clock */
    aud_mux_sai0_tx_bclk,                                                                 /**< SAI0 Transmitter external serial bit clock */
    aud_mux_sai1_rx_bclk,                                                                 /**< SAI1 Receiver external serial bit clock */
    aud_mux_sai1_tx_bclk,                                                                 /**< SAI1 Transmitter external serial bit clock */
    aud_mux_sai2_rx_bclk,                                                                 /**< SAI2 Receiver external serial bit clock */
    aud_mux_sai3_rx_bclk                                                                  /**< SAI3 Receiver external serial bit clock */
} imx_acm_aud_clk_mux_t;

typedef enum _acm_mclk_mux {
    mclk_mux_aud_pll_div_clk2 = IMX_ACM_MUX_INIT(0, SC_R_AUDIO_PLL_0, SC_PM_CLK_MISC1),   /**< DSC clock output */
    mclk_mux_aud_pll_div_clk3 = IMX_ACM_MUX_INIT(1, SC_R_AUDIO_PLL_1, SC_PM_CLK_MISC1),   /**< DSC clock output */
    mclk_mux_spdif0_rx = IMX_ACM_MUX_INIT(2, SC_R_LAST, 0),                               /**< SPDIF0 Receiver recovered clock */
    mclk_mux_sai4_rx_bclk                                                                 /**< SAI4 Receiver serial bit clock (Audio Mixer) */
} imx_acm_mclk_mux_t;

typedef enum _acm_esai_mux {
    esai_mux_aud_pll_div_clk0 = IMX_ACM_MUX_INIT(0, SC_R_AUDIO_PLL_0, SC_PM_CLK_MISC0),   /**< DSC clock output */
    esai_mux_aud_pll_div_clk1 = IMX_ACM_MUX_INIT(1, SC_R_AUDIO_PLL_1, SC_PM_CLK_MISC0),   /**< DSC clock output */
    esai_mux_aud_clk0 = IMX_ACM_MUX_INIT(2, SC_R_LAST, 0),                                /**< Audio Clock Multiplexer 0 output */
    esai_mux_aud_clk1                                                                     /**< Audio Clock Multiplexer 1 output */
} imx_acm_esai_mux_t;

typedef enum _acm_sai_mux {
    sai_mux_aud_pll_div_clk0 = IMX_ACM_MUX_INIT(0, SC_R_AUDIO_PLL_0, SC_PM_CLK_MISC0),    /**< DSC clock output */
    sai_mux_aud_pll_div_clk1 = IMX_ACM_MUX_INIT(1, SC_R_AUDIO_PLL_1, SC_PM_CLK_MISC0),    /**< DSC clock output */
    sai_mux_aud_clk0 = IMX_ACM_MUX_INIT(2, SC_R_LAST, 0),                                 /**< Audio Clock Multiplexer 0 output */
    sai_mux_aud_clk1                                                                      /**< Audio Clock Multiplexer 1 output */
} imx_acm_sai_mux_t;

typedef enum _acm_spdif_mux {
    spdif_mux_aud_pll_div_clk0 = IMX_ACM_MUX_INIT(0, SC_R_AUDIO_PLL_0, SC_PM_CLK_MISC0),  /**< DSC clock output */
    spdif_mux_aud_pll_div_clk1 = IMX_ACM_MUX_INIT(1, SC_R_AUDIO_PLL_1, SC_PM_CLK_MISC0),  /**< DSC clock output */
    spdif_mux_aud_clk0 = IMX_ACM_MUX_INIT(2, SC_R_LAST, 0),                               /**< Audio Clock Multiplexer 0 output */
    spdif_mux_aud_clk1                                                                    /**< Audio Clock Multiplexer 1 output */
} imx_acm_spdif_mux_t;

typedef enum _acm_gpt_mux {
    gpt_mux_aud_pll_div_clk0 = IMX_ACM_MUX_INIT(0, SC_R_AUDIO_PLL_0, SC_PM_CLK_MISC0),    /**< DSC clock output */
    gpt_mux_aud_pll_div_clk1 = IMX_ACM_MUX_INIT(1, SC_R_AUDIO_PLL_1, SC_PM_CLK_MISC0),    /**< DSC clock output */
    gpt_mux_aud_clk0 = IMX_ACM_MUX_INIT(2, SC_R_LAST, 0),                                 /**< Audio Clock Multiplexer 0 output */
    gpt_mux_aud_clk1,                                                                     /**< Audio Clock Multiplexer 1 output */
    gpt_mux_ref24m,                                                                       /**< 24 MHz reference */
} imx_acm_gpt_mux_t;

typedef enum _acm_mqs_mux {
    mqs_mux_aud_pll_div_clk0 = IMX_ACM_MUX_INIT(0, SC_R_AUDIO_PLL_0, SC_PM_CLK_MISC0),    /**< DSC clock output */
    mqs_mux_aud_pll_div_clk1 = IMX_ACM_MUX_INIT(1, SC_R_AUDIO_PLL_1, SC_PM_CLK_MISC0),    /**< DSC clock output */
    mqs_mux_aud_clk0 = IMX_ACM_MUX_INIT(2, SC_R_LAST, 0),                                 /**< Audio Clock Multiplexer 0 output */
    mqs_mux_aud_clk1                                                                      /**< Audio Clock Multiplexer 1 output */
} imx_acm_mqs_mux_t;

#endif /* IMX_ACM_H_ */


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/aarch64/imx8xp/imx_acm.h $ $Rev: 850159 $")
#endif
