/*
 * $QNXLicenseC:
 * Copyright 2016, QNX Software Systems. All Rights Reserved.
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

#include <fcntl.h>
#include <assert.h>
#include <sys/trace.h>

#include "adv748x.h"
#include "adv748x_reg.h"

static const char device_info[] = "QNX video capture driver for ADV748x decoder";

/******************************************************** MACROS & STATICS ***/
#define ADV748X_IO_MAP_ADDR_0		(0xE0>>1)  // ALSB is low
#define ADV748X_IO_MAP_ADDR_1		(0xE2>>1)  // ALSB is high
#define ADV748X_DLL_ADDR		(0x4C>>1)
#define ADV748X_CP_ADDR			(0x44>>1)
#define ADV748X_HDMI_RX_ADDR		(0x68>>1)
#define ADV748X_EDID_ADDR		(0x6C>>1)
#define ADV748X_HDMI_RX_REPEATER_ADDR	(0x64>>1)
#define ADV748X_INFOFRAME_ADDR		(0x62>>1)
#define ADV748X_CBUS_ADDR		(0xF0>>1)
#define ADV748X_CEC_ADDR		(0x82>>1)
#define ADV748X_SDP_MAIN_ADDR		(0xF2>>1)
#define ADV748X_CSI_TXB_ADDR		(0x90>>1)
#define ADV748X_CSI_TXA_ADDR		(0x94>>1)

/****************************************************************** DELAY() ***/
static void
_delay( int msec )
{
  do{
    msec = delay(msec);
  } while( msec );
}

/************************************************** ADV748X_POWERDOWN_CSI() ***/
static int
adv748x_powerdown_csi(capture_context_t ctx)
{
	int error = 0;
	uint8_t slave_addr, reg_val;
	int fd = ctx->device_fd;

	assert(ctx->active.output_iface == CSI_TXA_OUT || ctx->active.output_iface == CSI_TXB_OUT);
	if(ctx->active.output_iface == CSI_TXA_OUT) {
		slave_addr = ADV748X_CSI_TXA_ADDR;
	}
	else {
		slave_addr = ADV748X_CSI_TXB_ADDR;
	}
	error |= i2c_rd(fd, slave_addr, CSI_TX_TOP_REG_00, &reg_val);
	error |= i2c_wr(fd, slave_addr, CSI_TX_TOP_REG_00,  reg_val | CSI_TX_PWRDN);

	if( error ) {
	  logmsg( "%s(): error=%d", __FUNCTION__, error );
	}

	return error? EIO: EOK;
}

/**************************************************** ADV748X_POWERUP_CSI() ***/
static int
adv748x_powerup_csi(capture_context_t ctx)
{
	int error = 0;
	uint8_t slave_addr, reg_val;
	int fd = ctx->device_fd;

	assert(ctx->active.output_iface == CSI_TXA_OUT || ctx->active.output_iface == CSI_TXB_OUT);
	if(ctx->active.output_iface == CSI_TXA_OUT) {
		slave_addr = ADV748X_CSI_TXA_ADDR;
	}
	else {
		slave_addr = ADV748X_CSI_TXB_ADDR;
	}
	error |= i2c_rd(fd, slave_addr, CSI_TX_TOP_REG_00, &reg_val);
	error |= i2c_wr(fd, slave_addr, CSI_TX_TOP_REG_00,  reg_val & (~CSI_TX_PWRDN));
	_delay(1);

	return error? EIO: EOK;
}

/************************************************* ADV748X_HDMI_CSIA_INIT() ***/
static int
adv748x_hdmi_csia_init(capture_context_t ctx)
{
	int error = 0;
	uint8_t reg_val;
	int fd = ctx->device_fd;

	TRACE;

	// Setting Vid_Std to 1280x720p
	error |= i2c_wr(fd, ctx->dev_addr, IO_MAP_REG_CP_VID_STD, IO_MAP_CP_VID_STD_1080I);

	// Disable chip powerdown & Enable HDMI/MHL Rx block
	error |= i2c_rd(fd, ctx->dev_addr, IO_MAP_REG_PDN_CTRL, &reg_val);
	error |= i2c_wr(fd, ctx->dev_addr, IO_MAP_REG_PDN_CTRL, reg_val | IO_MAP_HDMI_MHL_RX_EN);
	//  Enable HDCP 1.1
	error |= i2c_wr(fd, ADV748X_HDMI_RX_REPEATER_ADDR, REPEATER_REG_BCAPS, BCAPS_DEFAULT);

	// Foreground Channel = A
	error |= i2c_wr(fd, ADV748X_HDMI_RX_ADDR, HDMI_RX_REG_00, 0x08);
	// ADI Required Writes
	error |= i2c_wr(fd, ADV748X_HDMI_RX_ADDR, 0x98, 0xFF);
	error |= i2c_wr(fd, ADV748X_HDMI_RX_ADDR, 0x99, 0xA3);
	error |= i2c_wr(fd, ADV748X_HDMI_RX_ADDR, 0x9A, 0x00);
	error |= i2c_wr(fd, ADV748X_HDMI_RX_ADDR, 0x9B, 0x0A);
	error |= i2c_wr(fd, ADV748X_HDMI_RX_ADDR, 0x9D, 0x40);
	error |= i2c_wr(fd, ADV748X_HDMI_RX_ADDR, 0xCB, 0x09);

	error |= i2c_wr(fd, ADV748X_HDMI_RX_ADDR, 0x3D, 0x10);
	error |= i2c_wr(fd, ADV748X_HDMI_RX_ADDR, 0x3E, 0x7B);
	error |= i2c_wr(fd, ADV748X_HDMI_RX_ADDR, 0x3F, 0x5E);
	error |= i2c_wr(fd, ADV748X_HDMI_RX_ADDR, 0x4E, 0xFE);
	error |= i2c_wr(fd, ADV748X_HDMI_RX_ADDR, 0x4F, 0x18);
	error |= i2c_wr(fd, ADV748X_HDMI_RX_ADDR, 0x57, 0xA3);
	error |= i2c_wr(fd, ADV748X_HDMI_RX_ADDR, 0x58, 0x04);
	error |= i2c_wr(fd, ADV748X_HDMI_RX_ADDR, 0x85, 0x10);
	// Enable All Terminations
	error |= i2c_wr(fd, ADV748X_HDMI_RX_ADDR, HDMI_RX_REG_83, 0x00);
	// ADI Required Writes
	error |= i2c_wr(fd, ADV748X_HDMI_RX_ADDR, 0xA3, 0x01);
	error |= i2c_wr(fd, ADV748X_HDMI_RX_ADDR, 0xBE, 0x00);

	// HPA Manual Enable
	error |= i2c_wr(fd, ADV748X_HDMI_RX_ADDR, HDMI_RX_REG_6C, HPA_MANUAL);
	// HPA Asserted
	error |= i2c_wr(fd, ADV748X_HDMI_RX_ADDR, HDMI_RX_REG_F8, HPA_MAN_VALUE_PORT_A);
	// Audio mute speed set to fastest (smallest step size)
	error |= i2c_wr(fd, ADV748X_HDMI_RX_ADDR, HDMI_RX_REG_AUDIO_MUTE_SPEED, AUDIO_MUTE_SPEED_FASTEST);

	switch(ctx->pending.output_format) {
	case SCREEN_FORMAT_RGB888:
		error |= i2c_wr(fd, ctx->dev_addr, IO_MAP_REG_04, CP_RGB_OUT);
		// CSC Depends on ip Packets - SDR444 set
		error |= i2c_wr(fd, ctx->dev_addr, IO_MAP_REG_12, IO_MAP_CP_INP_COLOR_SPACE_AUTO | IO_MAP_CP_OUT_MODE_SDR444 | IO_MAP_CP_OUT_8BIT);
		break;
	case SCREEN_FORMAT_UYVY:
		// YCrCb output
		error |= i2c_wr(fd, ctx->dev_addr, IO_MAP_REG_04, 0x00);
		// CSC Depends on ip Packets - SDR422 set
		error |= i2c_wr(fd, ctx->dev_addr, IO_MAP_REG_12, IO_MAP_CP_INP_COLOR_SPACE_AUTO | IO_MAP_CP_OUT_MODE_SDR422_2CH | IO_MAP_CP_OUT_8BIT);
		break;
	default:
		logmsg("%s: unsupported output format %d", __FUNCTION__, ctx->pending.output_format);
		return ENOTSUP;
	}
	// CP-Insert_AV_Code
	error |= i2c_wr(fd, ctx->dev_addr, IO_MAP_REG_DATAPATH_CTRL, IO_MAP_AVCODE_INSERT_EN | IO_MAP_CP_DATA_BLANK_EN | IO_MAP_CP_FREE_RUN_EN);

	// ADI Required Write
	error |= i2c_wr(fd, ADV748X_CP_ADDR, REPEATER_REG_SYNC_CTRL_1, 0x00);

	// Enable 4-lane CSI Tx
	error |= i2c_rd(fd, ctx->dev_addr, IO_MAP_REG_10,   &reg_val);
	error |= i2c_wr(fd, ctx->dev_addr, IO_MAP_REG_10,   reg_val | IO_MAP_CSIA_EN);

	// Enable 4-lane MIPI
	// note: CSI TX is off
	error |= i2c_wr(fd, ADV748X_CSI_TXA_ADDR, CSI_TX_TOP_REG_00, CSI_TX_PWRDN | (ctx->pending.num_data_lanes << CSI_NUM_DATA_LANES_SHIFT));
	// set Auto DPHY Timing
	error |= i2c_wr(fd, ADV748X_CSI_TXA_ADDR, CSI_TX_TOP_REG_00, CSI_EN_AUTOCALC_DPHY_PARAMS |CSI_TX_PWRDN | (ctx->pending.num_data_lanes << CSI_NUM_DATA_LANES_SHIFT));
	// ADI Required Writes
	error |= i2c_wr(fd, ADV748X_CSI_TXA_ADDR, 0xDB, 0x10);
//	error |= i2c_wr(fd, ADV748X_CSI_TXA_ADDR, 0xD6, 0x07);
	error |= i2c_wr(fd, ADV748X_CSI_TXA_ADDR, CSI_TX_TOP_REG_C4, 0x0A); /* data lane 0 configurations */
	error |= i2c_wr(fd, ADV748X_CSI_TXA_ADDR, 0x71, 0x33);
	error |= i2c_wr(fd, ADV748X_CSI_TXA_ADDR, 0x72, 0x11);
	// Power up CSI2A DPHY
//	error |= i2c_wr(fd, ADV748X_CSI_TXA_ADDR, CSI_TX_TOP_REG_F0, 0x00);

	// ADI Required Writes
	error |= i2c_wr(fd, ADV748X_CSI_TXA_ADDR, 0x31, 0x82);
	error |= i2c_wr(fd, ADV748X_CSI_TXA_ADDR, CSI_TX_TOP_REG_1E, 0x40);
	// Power up MIPI PLL
	error |= i2c_wr(fd, ADV748X_CSI_TXA_ADDR, CSI_TX_TOP_REG_DA, CSI_MIPI_PLL_EN);
	_delay(2);
#if 0  // not power up CSI-Tx yet
	// Power-up CSI-TX
	error |= i2c_wr(fd, ADV748X_CSI_TXA_ADDR, CSI_TX_TOP_REG_00, CSI_EN_AUTOCALC_DPHY_PARAMS | (ctx->pending.num_data_lanes << CSI_NUM_DATA_LANES_SHIFT));
	_delay(1);
#endif
	// ADI Required Write
	error |= i2c_wr(fd, ADV748X_CSI_TXA_ADDR, CSI_TX_TOP_REG_C1, 0x2B); /* clk lane configurations */
	_delay(1);
	// ADI Required Write
	error |= i2c_wr(fd, ADV748X_CSI_TXA_ADDR, 0x31, 0x80);

	if( error ) {
	  logmsg( "%s(): error=%d", __FUNCTION__, error );
	}

	return error? EIO: EOK;
}

/************************************************* ADV748X_CVBS_CSIB_INIT() ***/
static int
adv748x_cvbs_csib_init(capture_context_t ctx)
{
	int error = 0;
	uint8_t reg_val;
	int fd = ctx->device_fd;
	int is_differential_input =( (ctx->pending.src_type == CAPTURE_ADV_SRC_DIFFERENTIAL_CVBS)? 1: 0);

	TRACE;
	assert(ctx->pending.src_type == CAPTURE_ADV_SRC_SINGLE_ENDED_CVBS || ctx->pending.src_type == CAPTURE_ADV_SRC_DIFFERENTIAL_CVBS);

	// differential CVBS only?
	if(is_differential_input) {
		// DIAG3 Diagnostic Function - Enable Upper Level Slicer Only and set slice voltage to 975m
		error |= i2c_wr(fd, ctx->dev_addr, 0x1A, 0x19);
	}

	// Exit Power Down Mode
	error |= i2c_wr(fd, ADV748X_SDP_MAIN_ADDR, SDP_MAIN_REG_0F, 0x00);
	// ADI Required Write
	error |= i2c_wr(fd, ADV748X_SDP_MAIN_ADDR, 0x52, is_differential_input? 0xC0 : 0xCD);
	// INSEL
	reg_val = is_differential_input?  INPUT_DIFFERENTIAL_CVBS(ctx->pending.src_index): INPUT_CVBS(ctx->pending.src_index);
	error |= i2c_wr(fd, ADV748X_SDP_MAIN_ADDR, SDP_MAIN_REG_INPUT_SEL, reg_val);
	// ADI Required Write
	error |= i2c_wr(fd, ADV748X_SDP_MAIN_ADDR, SDP_MAIN_REG_0E, 0x80);
	error |= i2c_wr(fd, ADV748X_SDP_MAIN_ADDR, 0x9C, 0x00);
	error |= i2c_wr(fd, ADV748X_SDP_MAIN_ADDR, 0x9C, 0xFF);
	error |= i2c_wr(fd, ADV748X_SDP_MAIN_ADDR, SDP_MAIN_REG_0E, 0x00);
	if(is_differential_input) {
		// ADI Required Writes
		error |= i2c_wr(fd, ADV748X_SDP_MAIN_ADDR, 0x5A, 0x90);
		error |= i2c_wr(fd, ADV748X_SDP_MAIN_ADDR, 0x60, 0xA0);
		_delay(25);
		error |= i2c_wr(fd, ADV748X_SDP_MAIN_ADDR, 0x60, 0xB0);
		// SHA gain for Div4
		error |= i2c_wr(fd, ADV748X_SDP_MAIN_ADDR, 0x5F, 0xA8);
		// ADI Required Writes
		error |= i2c_wr(fd, ADV748X_SDP_MAIN_ADDR, SDP_MAIN_REG_0E, 0x80);
		error |= i2c_wr(fd, ADV748X_SDP_MAIN_ADDR, 0xB6, 0x08);
		error |= i2c_wr(fd, ADV748X_SDP_MAIN_ADDR, 0xC0, 0xA0);
		error |= i2c_wr(fd, ADV748X_SDP_MAIN_ADDR, SDP_MAIN_REG_0E, 0x00);
	}
	error |= i2c_wr(fd, ADV748X_SDP_MAIN_ADDR, 0x80, 0x51);
	error |= i2c_wr(fd, ADV748X_SDP_MAIN_ADDR, 0x81, 0x51);
	error |= i2c_wr(fd, ADV748X_SDP_MAIN_ADDR, 0x82, 0x68);
	// Tri-S Output Drivers, PwrDwn 656 pads
	error |= i2c_wr(fd, ADV748X_SDP_MAIN_ADDR, SDP_MAIN_REG_03, 0x42);
	// Power-up INTRQ pad, & Enable SFL
	// Blank Cr and Cb value during VBI; Extended output range (1 <= Y <= 254, 1 <= C/P <= 254)
	error |= i2c_wr(fd, ADV748X_SDP_MAIN_ADDR, SDP_MAIN_REG_04, 0x07);
	// ADI Required Write
	error |= i2c_wr(fd, ADV748X_SDP_MAIN_ADDR, 0x13, 0x00);
	// Select SH1
	error |= i2c_wr(fd, ADV748X_SDP_MAIN_ADDR, SDP_MAIN_REG_17, 0x41);
	// ADI Required Write
	// NOTE: bit 4 enables manual postion of V and Field bits... which is not ITU-R.656 compliant!
	error |= i2c_wr(fd, ADV748X_SDP_MAIN_ADDR, SDP_MAIN_REG_31, 0x12);

	// Enable 1-Lane MIPI Tx
	error |= i2c_rd(fd, ctx->dev_addr, IO_MAP_REG_10, &reg_val);  // todo mutex?
	error |= i2c_wr(fd, ctx->dev_addr, IO_MAP_REG_10, reg_val | IO_MAP_CSIB_EN);

	// Enable 1-lane MIPI (CSI Tx off)
	error |= i2c_wr(fd, ADV748X_CSI_TXB_ADDR, CSI_TX_TOP_REG_00, CSI_TX_PWRDN | (ctx->pending.num_data_lanes << CSI_NUM_DATA_LANES_SHIFT));
	// set Auto DPHY Timing
	error |= i2c_wr(fd, ADV748X_CSI_TXB_ADDR, CSI_TX_TOP_REG_00, CSI_EN_AUTOCALC_DPHY_PARAMS |CSI_TX_PWRDN | (ctx->pending.num_data_lanes << CSI_NUM_DATA_LANES_SHIFT));
#if 0  // NOTE: the following CSIA configurations (for CSIB) are copied from ADi offical script. However, they make CSIA not work any more. Comment them for now as CSIB still works without these configurations.
	// ADI Required Write (Power up CSIA DPHY?)
	error |= i2c_wr(fd, ADV748X_CSI_TXA_ADDR, CSI_TX_TOP_REG_F0, 0x00);
	error |= i2c_wr(fd, ADV748X_CSI_TXA_ADDR, 0xD6, 0x07);
	error |= i2c_wr(fd, ADV748X_CSI_TXA_ADDR, 0xC0, 0x3C);
	error |= i2c_wr(fd, ADV748X_CSI_TXA_ADDR, 0xC3, 0x3C);
	error |= i2c_wr(fd, ADV748X_CSI_TXA_ADDR, 0xC6, 0x3C);
	error |= i2c_wr(fd, ADV748X_CSI_TXA_ADDR, 0xC9, 0x3C);
	error |= i2c_wr(fd, ADV748X_CSI_TXA_ADDR, 0xCC, 0x3C);
	error |= i2c_wr(fd, ADV748X_CSI_TXA_ADDR, 0xD5, 0x03);
#endif
	error |= i2c_wr(fd, ADV748X_CSI_TXB_ADDR, 0xD2, 0x40);
	error |= i2c_wr(fd, ADV748X_CSI_TXB_ADDR, 0xC4, 0x0A);
	error |= i2c_wr(fd, ADV748X_CSI_TXB_ADDR, 0x71, 0x33);
	error |= i2c_wr(fd, ADV748X_CSI_TXB_ADDR, 0x72, 0x11);
	// Power up CSI2B DPHY
	error |= i2c_wr(fd, ADV748X_CSI_TXB_ADDR, CSI_TX_TOP_REG_F0, 0x00);

	// ADI Required Write
	error |= i2c_wr(fd, ADV748X_CSI_TXB_ADDR, 0x31, 0x82);
	error |= i2c_wr(fd, ADV748X_CSI_TXB_ADDR, CSI_TX_TOP_REG_1E, 0x40);
	// Power up MIPI PLL
	error |= i2c_wr(fd, ADV748X_CSI_TXB_ADDR, CSI_TX_TOP_REG_DA, CSI_MIPI_PLL_EN);
	_delay(2);
#if 0  // not power up CSI-Tx yet
	// Power-up CSI-TX
	error |= i2c_wr(fd, ADV748X_CSI_TXB_ADDR, CSI_TX_TOP_REG_00, CSI_EN_AUTOCALC_DPHY_PARAMS | (ctx->pending.num_data_lanes << CSI_NUM_DATA_LANES_SHIFT));
	_delay(1);
#endif
	// ADI Required Write
	error |= i2c_wr(fd, ADV748X_CSI_TXB_ADDR, 0xC1, 0x2B);
	_delay(1);
	// ADI Required Write
	error |= i2c_wr(fd, ADV748X_CSI_TXB_ADDR, 0x31, 0x80);

	if( error ) {
	  logmsg( "%s(): error=%d", __FUNCTION__, error );
	}

	return error? EIO: EOK;
}

/********************************************************** ADV748X_INIT() ***/
static int
adv748x_init(capture_context_t ctx)
{
	int error = 0;
	uint8_t reg_val;
	int fd = ctx->device_fd;

	// TODO use a shared mutex to protect the access to IO_MAP registers
	error = i2c_rd(fd, ctx->dev_addr, IO_MAP_REG_PDN_CTRL, &reg_val);
	if( error ) {
		logmsg( "%s: failed to read from register IO_MAP_REG_PDN_CTRL, error=%d", __FUNCTION__, error );
	}
	if(reg_val & (IO_MAP_CORE_PDN | IO_MAP_MASTER_PDN)) { // chip is not initialized yet
		// IO Map - SW reset
		error = i2c_wr(fd, ctx->dev_addr, IO_MAP_REG_FF, 0xFF);
		_delay(5);
		// ADI Required Write
		error |= i2c_wr(fd, ctx->dev_addr, IO_MAP_REG_01, 0x76);
		// Disable chip powerdown
		error |= i2c_wr(fd, ctx->dev_addr, IO_MAP_REG_PDN_CTRL, (1 << 4) );
		// Enable I2C Read Auto-Increment
		error |= i2c_wr(fd, ctx->dev_addr, IO_MAP_REG_F2, IO_MAP_READ_AUTO_INC_EN);
		// TODO make the I2C addresses configurable
		error |= i2c_wr(fd, ctx->dev_addr, IO_MAP_REG_I2C_SLAVE_ADDR_1, ADV748X_DLL_ADDR << 1);
		error |= i2c_wr(fd, ctx->dev_addr, IO_MAP_REG_I2C_SLAVE_ADDR_2, ADV748X_CP_ADDR << 1);
		error |= i2c_wr(fd, ctx->dev_addr, IO_MAP_REG_I2C_SLAVE_ADDR_3, ADV748X_HDMI_RX_ADDR << 1);
		error |= i2c_wr(fd, ctx->dev_addr, IO_MAP_REG_I2C_SLAVE_ADDR_4, ADV748X_EDID_ADDR << 1);
		error |= i2c_wr(fd, ctx->dev_addr, IO_MAP_REG_I2C_SLAVE_ADDR_5, ADV748X_HDMI_RX_REPEATER_ADDR << 1);
		error |= i2c_wr(fd, ctx->dev_addr, IO_MAP_REG_I2C_SLAVE_ADDR_6, ADV748X_INFOFRAME_ADDR << 1);
		error |= i2c_wr(fd, ctx->dev_addr, IO_MAP_REG_I2C_SLAVE_ADDR_7, ADV748X_CBUS_ADDR << 1);
		error |= i2c_wr(fd, ctx->dev_addr, IO_MAP_REG_I2C_SLAVE_ADDR_8, ADV748X_CEC_ADDR << 1);
		error |= i2c_wr(fd, ctx->dev_addr, IO_MAP_REG_I2C_SLAVE_ADDR_9, ADV748X_SDP_MAIN_ADDR << 1);
		error |= i2c_wr(fd, ctx->dev_addr, IO_MAP_REG_I2C_SLAVE_ADDR_10, ADV748X_CSI_TXB_ADDR << 1);
		error |= i2c_wr(fd, ctx->dev_addr, IO_MAP_REG_I2C_SLAVE_ADDR_11, ADV748X_CSI_TXA_ADDR << 1);

		// LLC/PIX/SPI PINS TRISTATED AUD Outputs Enabled
		error |= i2c_wr(fd, ctx->dev_addr, IO_MAP_REG_PAD_CTRL,  IO_MAP_TRI_LLC | IO_MAP_TRI_PIX_OUTPUT | IO_MAP_TRI_SPI | PDN_LLC | IO_MAP_PDN_PIX_OUTPUT | IO_MAP_PDN_SPI);  // todo #3
		// Enable pixel output and route SD through Pixel port, route CP through CSIA
		error |= i2c_wr(fd, ctx->dev_addr, IO_MAP_REG_10, IO_MAP_PIX_OUT_EN | IO_MAP_SD_THRU_PIX_OUT | IO_MAP_CSIA_IN_SEL_CP_CORE);
		// Luma & Chroma Values Can Reach 254d
		error |= i2c_wr(fd, ctx->dev_addr, IO_MAP_REG_17, IO_MAP_BR_DITH_CCIR601_B);
		// Enable LLC_DLL & Double LLC timing
		error |= i2c_wr(fd, ctx->dev_addr, IO_MAP_REG_0C, IO_MAP_LLC_DLL_EN | IO_MAP_LLC_DLL_DOUBLE | IO_MAP_LLC_DLL_MUX);

		error |= i2c_wr(fd, ADV748X_CSI_TXA_ADDR, CSI_TX_TOP_REG_F0, 0x00);
		error |= i2c_wr(fd, ADV748X_CSI_TXA_ADDR, 0xD6, 0x07);
	}

	if( error ) {
		logmsg( "%s(): error=%d", __FUNCTION__, error );
	}

	return error? EIO: EOK;

}

/************************************************** ADV748X_GET_CHIP_REV() ***/
static int
adv748x_get_chip_rev(int fd, uint32_t dev_addr, uint16_t* chip_id)
{
	uint8_t id[2] = {0,0};
	int error = 0;

	error |= i2c_rd(fd, dev_addr, IO_MAP_REG_CHIP_REV_ID_1, &id[0]);
	error |= i2c_rd(fd, dev_addr, IO_MAP_REG_CHIP_REV_ID_2, &id[1]);

	if(error == EOK) {
		*chip_id = (id[0] << 8) | id[1];
	}
	return error ? EIO : EOK;
}

/*********************************************** ADV748X_GET_OUTPUT_IFACE() ***/
static enum adv748x_output_iface
adv748x_get_output_iface(int device)
{
	// For now, use the fixed video routing:
	// SDP output -- CSI TXB
	// CP output -- CSI TXA.
	switch (device) {
	case DEV0_CP_CORE:
		return CSI_TXA_OUT;
	case DEV1_SDP_CORE:
		return CSI_TXB_OUT;
	default:
		return INVALID_OUT;
	}
}

/********************************************************* DECODER_UPDATE() ***/
/**
 * Apply the changes to the decoder

 * Return:
 *  EOK -- Success
 *  ENODEV, EINVAL, EIO...etc
 */
SYM_INTERNAL_ONLY int
decoder_update( capture_context_t ctx, uint32_t flags )
{
	int error = 0;
	uint16_t chip_rev = 0;

	TRACE;
	if(ctx->changes & CAPTURE_CHANGE_DEVICE_ADDR) {
		if(strlen(ctx->dev_path) == 0) {
			logmsg("%s: device path is not set", __FUNCTION__);
			return ENODEV;
		}

		if((ctx->device_fd = open(ctx->dev_path, O_RDWR)) == -1) {
			logmsg( "%s:failed open i2c device %s (%s)", __FUNCTION__, ctx->dev_path, strerror(errno));
			return errno;
		}

		if(ctx->dev_addr != ADV748X_IO_MAP_ADDR_0 && ctx->dev_addr != ADV748X_IO_MAP_ADDR_1) {
			logmsg("%s: invalid device address %#x (should be %#x or %#x)",
					__FUNCTION__, ctx->dev_addr, ADV748X_IO_MAP_ADDR_0, ADV748X_IO_MAP_ADDR_1);
			return EINVAL;
		}

		// read chip revision
		error =adv748x_get_chip_rev(ctx->device_fd, ctx->dev_addr, &chip_rev);
		if(error != EOK) {
			logmsg("%s: failed to read chip revision (%s)", __FUNCTION__, strerror(error));
			return error;
		}
		LOG("       Chip revision: %s", (chip_rev == 0x2140)? "ES1": ((chip_rev == 0x2141)? "ES2" : ((chip_rev == 0x2142)? "ES3" : ((chip_rev == 0x2143)? "ES3.1" : "UNKNOWN" ))) );
		error = adv748x_init(ctx);
		if(error != EOK) {
			logmsg("%s: failed to initalize the decoder(%s)", __FUNCTION__, strerror(error));
			return error;
		}
		ctx->changes &= ~CAPTURE_CHANGE_DEVICE_ADDR;
	}

	if(ctx->changes & CAPTURE_CHANGE_DEVICE) {
		if(ctx->pending.device < ndevices) {
			// TODO for now, CP is connected to CSI2_A; SD is connected to CSI2_B.
			// this should be reflected somewhere
			ctx->active.device = ctx->pending.device;
			LOG("       %s core is used",  (ctx->active.device == 0) ? "CP" : "SDP");
			ctx->active.output_iface = adv748x_get_output_iface(ctx->active.device);
			assert(ctx->active.output_iface != INVALID_OUT);
			ctx->changes |= CAPTURE_CHANGE_SOURCE;
			ctx->changes &= ~CAPTURE_CHANGE_DEVICE;
		}
		else {
			logmsg("%s: invalid device %d", __FUNCTION__, ctx->pending.device);
			return EINVAL;
		}
	}

	if(ctx->changes & CAPTURE_CHANGE_SOURCE) {
		if((ctx->pending.num_data_lanes != -1) && (ctx->pending.num_data_lanes < 1 || ctx->pending.num_data_lanes > num_data_lanes_max[ctx->active.output_iface])) {
			logmsg("%s: invalid number of data lanes %d", __FUNCTION__, ctx->pending.num_data_lanes);
			return EINVAL;
	} else if(ctx->pending.num_data_lanes == -1) {
		// If not set, default to our maximum supported lane count
		ctx->pending.num_data_lanes = num_data_lanes_max[ctx->active.output_iface];
	}

		switch(ctx->active.device ) {
		case DEV0_CP_CORE:
			assert(ctx->active.output_iface == CSI_TXA_OUT);
			if(ctx->pending.output_format != SCREEN_FORMAT_RGB888 && ctx->pending.output_format != SCREEN_FORMAT_UYVY) {
				logmsg("%s: unsupported output format %d", __FUNCTION__, ctx->pending.output_format);
				return ENOTSUP;
			}
			error |= adv748x_hdmi_csia_init(ctx);
			break;
		case DEV1_SDP_CORE:
			assert(ctx->active.output_iface == CSI_TXB_OUT);
			if(ctx->pending.output_format != SCREEN_FORMAT_UYVY) {
				logmsg("%s: unsupported output format %d", __FUNCTION__, ctx->pending.output_format);
				return ENOTSUP;
			}
			if((ctx->pending.src_type >= 0) && (ctx->pending.src_type < CAPTURE_ADV_SRC_TYPE_NUM) &&  (ctx->pending.src_index >= 0)
					&& (ctx->pending.src_index < nsources_sdp[ctx->pending.src_type])) {
				switch(ctx->pending.src_type) {
				case CAPTURE_ADV_SRC_SINGLE_ENDED_CVBS:
				case CAPTURE_ADV_SRC_DIFFERENTIAL_CVBS:
					error |= adv748x_cvbs_csib_init(ctx);
					break;
				case CAPTURE_ADV_SRC_YC:
				case CAPTURE_ADV_SRC_PCBPR:
					logmsg("%s: non-supported source type %d", __FUNCTION__, ctx->pending.src_type);
					return ENOTSUP;
				default:
					logmsg("%s: invalid source type %d", __FUNCTION__, ctx->pending.src_type);
					return EINVAL;
				}
				if(error != EOK) {
					logmsg("%s: failed to initalize the device %d (%s)", __FUNCTION__, ctx->active.device, strerror(error));
					return error;
				}
			}
			else {
				logmsg("%s: invalid src_index %d, or invalid source_type %d",
						__FUNCTION__, ctx->pending.src_index, ctx->pending.src_type);
				return EINVAL;
			}
			break;
		default:
			logmsg("%s: invalid device %d", __FUNCTION__, ctx->active.device);
			return EINVAL;
		}

		ctx->active.num_data_lanes = ctx->pending.num_data_lanes;
		ctx->active.output_format = ctx->pending.output_format;
		ctx->active.src_type = ctx->pending.src_type;
		ctx->active.src_index = ctx->pending.src_index;
		ctx->changes &= ~CAPTURE_CHANGE_SOURCE;
	}

	if(ctx->changes & CAPTURE_CHANGE_ENABLE) {
		if(ctx->pending.output_enable) {
			error = adv748x_powerup_csi(ctx);
	// Note: CSI2 output might not get stablized yet. if there are transfer errors detected on CSI2 receiver side, or
	// there is scrolling artifact, etc, consider adding some delay here.
		}
		else {
			error = adv748x_powerdown_csi(ctx);
		}
		if(error != EOK) {
			logmsg("%s: failed to set ENABLE to the decoder:%s", __FUNCTION__, strerror(error));
			return error;
		}
		ctx->active.output_enable = ctx->pending.output_enable;
		ctx->changes &= ~ CAPTURE_CHANGE_ENABLE;
	}

	if( error ) {
	  logmsg( "%s(): error=%d", __FUNCTION__, error );
	}

	return error? EIO: EOK;
}

/********************************************************* DECODER_INIT() ***/
/**
 * Decoder initialization (open device...)

 * Return:
 *  EOK -- Success
 *  ENODEV, EINVAL, EIO...etc
 */
SYM_INTERNAL_ONLY int
decoder_init(capture_context_t ctx)
{
	ctx->device_fd = -1;

	strcpy(ctx->dev_path, "");
	ctx->dev_addr = ADV748X_IO_MAP_ADDR_0;

	ctx->device_info = device_info;
	ctx->pending.device = ctx->active.device = -1;
	ctx->pending.num_data_lanes = ctx->active.num_data_lanes = -1;
	ctx->pending.src_type = -1;
	ctx->pending.src_index = -1;
	ctx->pending.output_format = SCREEN_FORMAT_UYVY;

	ctx->changes = CAPTURE_CHANGE_DEVICE_ADDR;
	return EOK;
}

/********************************************************* DECODER_FINI() ***/
SYM_INTERNAL_ONLY void
decoder_fini(capture_context_t ctx)
{
	// NOTE: Don't power down the decoder here, as the other device(CP core, or SDP core) might be used.
	if(ctx->device_fd != -1) {
		adv748x_powerdown_csi(ctx);
		close(ctx->device_fd);
		ctx->device_fd = -1;
	}
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/graphics/branches/release-2.1.x/hardware/capture/adv748x/adv748x.c $ $Rev: 867357 $")
#endif
