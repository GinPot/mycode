/*
 * $QNXLicenseC:
 * Copyright 2013, QNX Software Systems. All Rights Reserved.
 *
 * You must obtain a written license from and pay applicable license fees to QNX
 * Software Systems before you may reproduce, modify or distribute this software,
 * or any work that includes all or part of this software.   Free development
 * licenses are available for evaluation and non-commercial purposes.  For more
 * information visit http://licensing.qnx.com or email licensing@qnx.com.
 *
 * This file may contain contributions from others.  Please review this entire
 * file for other proprietary rights or license notices, as well as the QNX
 * Development Suite License Guide at http://licensing.qnx.com/license-guide/
 * for other information.
 * $
 */

#ifndef ADV748X_REG_H_

#define ADV748X_REG_H_

/* io map */
#define IO_MAP_REG_PDN_CTRL				0x00
#define IO_MAP_HDMI_MHL_RX_EN				(1 << 6)
#define  IO_MAP_HDMI_MHL_RX_PDN				(1 << 5)
#define DPLL_PDN					(1 << 4)
#define IO_MAP_XTAL_PDN					(1 << 2)
#define IO_MAP_CORE_PDN					(1 << 1)
#define IO_MAP_MASTER_PDN				(1 << 0)

#define IO_MAP_REG_01					0x01
#define IO_MAP_PWRDN2B					(1 << 7)
#define IO_MAP_PWRDNB					(1 << 6)


#define IO_MAP_REG_DATAPATH_CTRL			0x03
#define  IO_MAP_AVCODE_INSERT_EN			(1 << 7)
#define  IO_MAP_CP_V_FREQ_MASK				(7 << 4)
#define  IO_MAP_CP_OP_656_RANGE				(1 << 3)
#define  IO_MAP_CP_DATA_BLANK_EN			(1 << 2)
#define  IO_MAP_CP_FREE_RUN_EN				(1 << 1)
#define  IO_MAP_CP_BYPASS				(1 << 0)

#define IO_MAP_REG_04					0x04
#define CP_FORCE_FREERUN_CH1				(1 << 7)
#define CP_ALT_GAMMA					(1 << 3)
#define CP_ALT_DATA_SAT					(1 << 2)
#define CP_RGB_OUT					(1 << 1)

#define IO_MAP_REG_CP_VID_STD				0x05
#define IO_MAP_CP_VID_STD_480P				0x4A
#define IO_MAP_CP_VID_STD_576P				0x4B
#define IO_MAP_CP_VID_STD_720P				0x53
#define IO_MAP_CP_VID_STD_1080I				0x54


#define IO_MAP_REG_0C					0x0C
#define IO_MAP_LLC_DLL_EN				(1 << 7)
#define IO_MAP_LLC_DLL_DOUBLE				(1 << 6)
#define IO_MAP_LLC_DLL_MUX				(1 << 5)
#define IO_MAP_LLC_DLL_PHASE_MASK			(0X1F << 0)

#define IO_MAP_REG_PAD_CTRL				0x0E
#define IO_MAP_TRI_LLC					(1 << 7)
#define IO_MAP_TRI_PIX_OUTPUT				(1 << 6)
#define IO_MAP_TRI_AUDIO				(1 << 5)
#define IO_MAP_TRI_SPI					(1 << 4)
#define PDN_LLC						(1 << 3)
#define IO_MAP_PDN_PIX_OUTPUT				(1 << 2)
#define IO_MAP_PDN_AUDIO				(1 << 1)
#define IO_MAP_PDN_SPI					(1 << 0)

#define IO_MAP_REG_PAD_FILTER_CTRL			0x0F
#define IO_MAP_PDN_VBUS_EN				(1 << 3)
#define IO_MAP_XTAL_FREQ_SEL_MASK			(3 << 0)
#define IO_MAP_XTAL_FREQ_SEL_27MHZ			(0 << 0)
#define IO_MAP_XTAL_FREQ_SEL_28_63636_MHZ		(1 << 0)
#define IO_MAP_XTAL_FREQ_SEL_24_576MHZ			(2 << 0)
#define IO_MAP_XTAL_FREQ_SEL_PROG			(3 << 0)

#define IO_MAP_REG_10					0x10
#define IO_MAP_CSIA_EN					(1 << 7)
#define IO_MAP_CSIB_EN					(1 << 6)
#define IO_MAP_PIX_OUT_EN				(1 << 5)
#define IO_MAP_SD_THRU_PIX_OUT				(1 << 4)
#define  IO_MAP_CSIA_IN_SEL_MASK			(3 << 2)
#define  IO_MAP_CSIA_IN_SEL_CP_CORE			(0 << 2)
#define  IO_MAP_CSIA_IN_SEL_8BIT_DIGITAL_INPUT		(1 << 2)
#define  IO_MAP_CSIA_IN_SEL_SD_CORE			(2 << 2)


#define IO_MAP_REG_11					0x11
#define SD_DDR_OUT					(1 << 2)

#define IO_MAP_REG_12					0x12
#define IO_MAP_CP_INP_COLOR_SPACE_MASK			(0xF << 4)
#define IO_MAP_CP_INP_COLOR_SPACE_AUTO			(0xF << 4)
#define IO_MAP_CP_OUT_MODE_MASK				(3 << 1)
#define IO_MAP_CP_OUT_MODE_SDR444			(0 << 1)
#define IO_MAP_CP_OUT_MODE_SDR422_2CH			(1 << 1)
#define IO_MAP_CP_OUT_BIT_MASK				(1 << 0)
#define IO_MAP_CP_OUT_8BIT				(0 << 0)
#define IO_MAP_CP_OUT_10BIT				(1 << 0)

#define IO_MAP_REG_17					0x17
#define IO_MAP_BR_DITH_CCIR601_B			(1 << 7)
#define IO_MAP_BR_DITH_YUV422_MODE			(1 << 6)
#define IO_MAP_BR_DITHER_MODE				(1 << 5)
#define IO_MAP_BR_DITHER_EN				(1 << 3)

#define IO_MAP_REG_CHIP_REV_ID_1			0xDF
#define IO_MAP_REG_CHIP_REV_ID_2			0xE0

#define IO_MAP_REG_F2					0xF2
#define IO_MAP_READ_AUTO_INC_EN				(1 << 0)

#define IO_MAP_REG_I2C_SLAVE_ADDR_1			0xF3
#define IO_MAP_REG_I2C_SLAVE_ADDR_2			0xF4
#define IO_MAP_REG_I2C_SLAVE_ADDR_3			0xF5
#define IO_MAP_REG_I2C_SLAVE_ADDR_4			0xF6
#define IO_MAP_REG_I2C_SLAVE_ADDR_5			0xF7
#define IO_MAP_REG_I2C_SLAVE_ADDR_6			0xF8
#define IO_MAP_REG_I2C_SLAVE_ADDR_7			0xF9
#define IO_MAP_REG_I2C_SLAVE_ADDR_8			0xFA
#define IO_MAP_REG_I2C_SLAVE_ADDR_9			0xFB
#define IO_MAP_REG_I2C_SLAVE_ADDR_10			0xFC
#define IO_MAP_REG_I2C_SLAVE_ADDR_11			0xFD

#define IO_MAP_REG_FF					0xFF
#define IO_MAP_MAIN_RESET				(1 << 7)

/* sdp main map */
#define SDP_MAIN_REG_INPUT_SEL				0x00
#define INPUT_CVBS(i)					(0x00 + i)
#define INPUT_YC(i)					(0x08 + i)
#define INPUT_YPBPR(i)					(0x0C + i)
#define INPUT_DIFFERENTIAL_CVBS(i)			(0x0E + i)

#define SDP_MAIN_REG_03					0x03
#define SDP_MAIN_VBI_EN					(1 << 7)

#define SDP_MAIN_REG_04					0x04
#define SDP_MAIN_BT656_4				(1 << 7)
#define SDP_MAIN_BL_C_VBI				(1 << 2)
#define SDP_MAIN_BL_RANGE				(1 << 0)

#define SDP_MAIN_REG_0E					0x0E
#define SDP_MAIN_SUB_USR_EN_MASK			(3 << 5)
#define SDP_MAIN_R_ONLY_MAPS_SEL_MASK			(7 << 0)


#define SDP_MAIN_REG_0F					0x0F
#define SDP_MAIN_RESET					(1 << 7)
#define SDP_MAIN_PWRDN					(1 << 5)

#define SDP_MAIN_REG_17					0x17
#define SDP_MAIN_CSFM_MASK				(7 << 5)

#define SDP_MAIN_REG_31					0x31
#define SDP_MAIN_NEWAVMODE				(1 << 4)

/* hdmi rx map */
#define HDMI_RX_REG_00					0x00
#define HDCP_A0						(1 << 7)
#define HDCP_ONLY_MODE					(1 << 6)

#define HDMI_RX_REG_AUDIO_MUTE_SPEED			0x0F
#define MAN_AUDIO_DL_BYPASS				(1 << 7)
#define AUDIO_DELAY_LINE_BYPASS				(1 << 6)
#define AUDIO_MUTE_SPEED_MASK				(0X1F << 0)
#define AUDIO_MUTE_SPEED_FASTEST			(0 << 0)

#define HDMI_RX_REG_6C					0x6C
#define HPA_DELAY_SEL_MASK				(0xF << 4)
#define HPA_OVR_TERM					(1 << 3)
#define HPA_AUTO_INT_EDID_MASK				(3 << 1)
#define HPA_MANUAL					(1 << 0)

#define HDMI_RX_REG_F8					0xF8
#define HPA_MAN_VALUE_PORT_A				(1 << 0)

#define HDMI_RX_REG_83					0x83
#define CLOCK_TERM_DISABLE				(1 << 0)

/* hdmi rx repeater map */
#define REPEATER_REG_BCAPS				0x40
#define BCAPS_DEFAULT					0x83

#define REPEATER_REG_70					0x70
#define PRIMARY_EDID_SIZE_MASK				(0xF << 4)
#define PRIMARY_EDID_SIZE_128B				(1 << 4)
#define PRIMARY_EDID_SIZE_256B				(2 << 4)
#define PRIMARY_EDID_SIZE_384B				(3 << 4)
#define PRIMARY_EDID_SIZE_512B				(4 << 4)

#define REPEATER_REG_HDCP_EDID_CTRL			0x74
#define MAN_EIDI_A_ENABLE				(1 << 0)

/* cp map */
#define REPEATER_REG_SYNC_CTRL_1			0x7C
#define START_HS_PART_2_MASK				(3 << 2)
#define END_HS_PART_2_MASK				(3 << 0)

/* csi-txa, csi-txb */
#define CSI_TX_TOP_REG_00				0x00
#define CSI_TX_PWRDN					(1 << 7)
#define CSI_EN_AUTOCALC_DPHY_PARAMS			(1 << 5)
#define CSI_NUM_DATA_LANES_MASK				(7 << 0)
#define CSI_NUM_DATA_LANES_SHIFT			0

#define CSI_TX_TOP_REG_1D				0x1D
#define CSI_INTERLACE_OVERRIDE				(1 << 7)
#define CSI_INTERLACE_MODE				(1 << 6)

#define CSI_TX_TOP_REG_1E				0x1E
#define CSI_INTERPRET_FS_AS_LS				(1 << 7)

#define CSI_TX_TOP_REG_21				0x21

#define CSI_TX_TOP_REG_C1				0xC1
#define CSI_CLKLN_IS_CLOCK_LANE				(1 << 5)
#define CSI_CLKLN_ZERO_CLK_LANE				(1 << 4)
#define CSI_CLKLN_LANE_IS_MASTER			(1 << 3)

#define CSI_TX_TOP_REG_C4				0xC4
#define CSI_D0LN_IS_CLOCK_LANE				(1 << 5)
#define CSI_D0LN_ZERO_CLK_LANE				(1 << 4)
#define CSI_D0LN_LANE_IS_MASTER				(1 << 3)

#define CSI_TX_TOP_REG_CA				0xCA
#define CSI_D2LN_IS_CLOCK_LANE				(1 << 5)
#define CSI_D2LN_ZERO_CLK_LANE				(1 << 4)
#define CSI_D2LN_LANE_IS_MASTER				(1 << 3)


#define CSI_TX_TOP_REG_DA				0xDA
#define CSI_MIPI_PLL_LOCK_FLAG				(1 << 2)
#define CSI_MIPI_PLL_CLK_DETECT				(1 << 1)
#define CSI_MIPI_PLL_EN					(1 << 0)

#define CSI_TX_TOP_REG_F0				0xF0
#define CSI_DPHY_PWDN					(1 << 0)

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/graphics/branches/release-2.1.x/hardware/capture/adv748x/adv748x_reg.h $ $Rev: 867357 $")
#endif
