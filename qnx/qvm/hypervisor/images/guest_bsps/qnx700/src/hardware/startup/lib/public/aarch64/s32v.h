/*
 * $QNXLicenseC:
 * Copyright 2016, QNX Software Systems. 
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

/*
 * NXP S32 SOCs:
 * S32V234 - 2x2 (2 cores per cluster) Cortex-A53 based SOC
 * S32V232 - 2x1 (1 core per cluster) Cortex-A53 based SOC
 */

#ifndef	__AARCH64_S32V_H_INCLUDED
#define	__AARCH64_S32V_H_INCLUDED


/*
 * System Memory Map
 */

/*
 * LINFlexD (used for LIN and UART/serial communication)
 */
#define S32V_LINFLEXD0_BASE					0x40053000
#define S32V_LINFLEXD1_BASE					0x400BC000
#define S32V_LINFLEXD0_SIZE					0x1000

/* Clock Generation Module */
#define S32V_MC_CGM0_BASE					0x4003C000
#define S32V_MC_CGM1_BASE					0x4003F000
#define S32V_MC_CGM2_BASE					0x40042000
#define S32V_MC_CGM3_BASE					0x40045000

#define S32V_MC_CGM0_PCS_SDUR					0x703
#define S32V_MC_CGM0_PCS_DIVC1					0x704
#define S32V_MC_CGM0_PCS_DIVE1					0x708
#define S32V_MC_CGM0_PCS_DIVS1					0x70C
#define S32V_MC_CGM0_PCS_DIVC2					0x710
#define S32V_MC_CGM0_PCS_DIVE2					0x714
#define S32V_MC_CGM0_PCS_DIVS2					0x718
#define S32V_MC_CGM0_DIV_UPD_TYPE				0x7D4
#define S32V_MC_CGM0_DIV_UPD_TRIG				0x7D8
#define S32V_MC_CGM0_DIV_UPD_STAT				0x7DC
#define S32V_MC_CGM0_SC_SS					0x7E4
#define S32V_MC_CGM0_SC_DC0					0x7E8
#define S32V_MC_CGM0_SC_DC1					0x7EC
#define S32V_MC_CGM0_SC_DC2					0x7F0
#define S32V_MC_CGM0_ACn_SC(n)					(0x800+(n*0x20))	// select current clock source
#define S32V_MC_CGM0_ACn_SS(n)					(0x804+(n*0x20))	// read current clock source
#define S32V_MC_CGM0_ACn_DC0(n)					(0x808+(n*0x20))	// divider reg 0
	#define S32V_MC_CGM0_ACn_DE				(0x1 << 31)	// Divider Enable
	#define S32V_MC_CGM0_AC0_DIV_OFFSET			16
	#define S32V_MC_CGM0_AC0_DIV_MASK			(0x3 << 16)
	#define S32V_MC_CGM0_AC1_DIV_OFFSET			16
	#define S32V_MC_CGM0_AC1_DIV_MASK			(0x3 << 16)
	#define S32V_MC_CGM0_AC2_DIV_OFFSET			16
	#define S32V_MC_CGM0_AC2_DIV_MASK			(0x3 << 16)
	#define S32V_MC_CGM0_AC3_DIV_OFFSET			16
	#define S32V_MC_CGM0_AC3_DIV_MASK			(0x3 << 16)
	#define S32V_MC_CGM0_AC4_DIV_OFFSET			16
	#define S32V_MC_CGM0_AC4_DIV_MASK			(0xf << 16)
	#define S32V_MC_CGM0_AC5_DIV_OFFSET			16
	#define S32V_MC_CGM0_AC5_DIV_MASK			(0x3 << 16)
	#define S32V_MC_CGM0_AC5_DIV1_OFFSET			16
	#define S32V_MC_CGM0_AC5_DIV1_MASK			(0x3 << 16)
	#define S32V_MC_CGM0_AC6_DIV_OFFSET			16
	#define S32V_MC_CGM0_AC6_DIV_MASK			(0x3 << 16)
	#define S32V_MC_CGM0_AC7_DIV_OFFSET			16
	// No AC7 DIV0?
	#define S32V_MC_CGM0_AC7_DIV1_MASK			(0x1f << 16)
	#define S32V_MC_CGM0_AC8_DIV_OFFSET			16
	#define S32V_MC_CGM0_AC8_DIV_MASK			(0x3 << 16)
	#define S32V_MC_CGM0_AC8_DIV1_OFFSET			16
	#define S32V_MC_CGM0_AC8_DIV1_MASK			(0xf << 16)
	// ...

#define S32V_MC_CGM0_ACn_DC1(n)					(0x80C+(n*0x20))	// divider reg 1 (not available for all aux clocks!)

/* Mode Entry Module - controls chip mode and mode transition sequences */
#define S32V_MC_ME_BASE						0x4004a000
#define S32V_MC_ME_SIZE						0x1000

	#define S32V_MC_ME_GS					0x0
	#define S32V_MC_ME_MCTL					0x4
	#define S32V_MC_ME_ME					0x8
	#define S32V_MC_ME_CS					0x1C0
	// ...

/*
 * I2C Controllers
 */
#define S32V_I2C0_BASE						0x40051000
#define S32V_I2C1_BASE						0x400B8000
#define S32V_I2C2_BASE						0x400BA000

#define S32V_I2C0_IRQ						99
#define S32V_I2C1_IRQ						100
#define S32V_I2C2_IRQ						101

#define S32V_I2C_SIZE						0x1000

/*
 * 10/100/1000-Mbps Ethernet MAC (ENET) Controller
 */
#define S32V_ENET_BASE						0x40032000
#define S32V_ENET_IRQ						67

#define S32V_ENET_SIZE						0x1000

/*
 * SWT
 */

#define S32V_SWT0_BASE 0x4000A000
#define S32V_SWT1_BASE 0x4000B000
#define S32V_SWT2_BASE 0x40084000
#define S32V_SWT3_BASE 0x40085000
#define S32V_SWT4_BASE 0x40086000
#define S32V_SWT_SIZE						0x1000

#define SWT_CR		0x00	/* SWT Control Register */
    #define SWT_CR_FIXED_SS	(0 << 9)	/* -> Fixed Service Sequence */
    #define SWT_CR_KEYED_SS	(1 << 9)	/* -> Keyed Service Sequence */
    #define SWT_CR_RIA		(1 << 8)	/* -> Reset on Invalid Access */
    #define SWT_CR_WND		(1 << 7)	/* -> Window Mode */
    #define SWT_CR_ITR		(1 << 6)	/* -> Interrupt then reset */
    #define SWT_CR_HLK		(1 << 5)	/* -> Hard Lock */
    #define SWT_CR_SLK		(1 << 4)	/* -> Soft Lock */
    #define SWT_CR_STP		(1 << 2)	/* -> Stop Mode Control */
    #define SWT_CR_FRZ		(1 << 1)	/* -> Debug Mode Control */
    #define SWT_CR_WEN		(1 << 0)	/* -> Watchdog Enable */
#define SWT_IR		0x04	/* SWT Interrupt Register */
#define SWT_TO		0x08	/* SWT Timeout Register */
#define SWT_SR		0x10	/* SWT Service Register */
    #define WDT_SEQ1		0xA602	        /* -> service sequence 1 */
    #define WDT_SEQ2		0xB480	        /* -> service sequence 2 */
#define SWT_CO		0x14	/* SWT Counter Output Register */
#define SWT_SK		0x18	/* SWT Service Key Register */
#define S32V_SWT4_SR 0x40086010

/*
 * SIUL2
 */
#define S32V_SIUL2_BASE 0x4006C000

#define SIUL2_MIDR1		0x04
#define SIUL2_MIDR2		0x08
#define SIUL2_DISR0		0x10
#define SIUL2_DIRER0		0x18
#define SIUL2_DIRSR0		0x20
#define SIUL2_IREER0		0x28
#define SIUL2_IFEER0		0x30
#define SIUL2_IFER0		0x38
#define SIUL2_IFMCRn(i)		(0x40 + 4 * (i))
#define SIUL2_IFCPR		0xC0

/* SIUL2_MSCR specifications as stated in Reference Manual:
 * 0 - 359 Output Multiplexed Signal Configuration Registers
 * 512- 1023 Input Multiplexed Signal Configuration Registers */
#define SIUL2_MSCRn(i)		(0x240 + 4 * (i))
#define SIUL2_IMCRn(i)		(0xA40 +  4 * (i))
#define SIUL2_GPDOn(i)		(0x1300 + 4 * (i))
#define SIUL2_GPDIn(i)		(0x1500 + 4 * (i))
#define SIUL2_PGPDOn(i)		(0x1700 +  2 * (i))
#define SIUL2_PGPDIn(i)		(0x1740 + 2 * (i))
#define SIUL2_MPGPDOn(i)	(0x1780 + 4 * (i))

/* GPIO */
/* 163 GPIOs in output mode, we assume the GPIO number is in range */
#define SIUL2_GPDO_for_GPIO(i)		(((i) & (~0x3))>>2)
#define SIUL2_GPDO_PDO_off_for_GPIO(i)	(((i) & (0x3))
#define SIUL2_PDOn(i)			(SIUL2_GPDOn(SIUL2_GPDO_for_GPIO(i) + \
						SIUL2_GPDO_PDO_off_for_GPIO(i))
#define SIUL2_GPIO_VALUE0		(0x00)
#define SIUL2_GPIO_VALUE1		(0x01)

/* SIUL2_MSCR masks */
#define SIUL2_MSCR_DDR_DO_TRIM(v)	((v) & 0xC0000000)
#define SIUL2_MSCR_DDR_DO_TRIM_MIN	(0 << 30)
#define SIUL2_MSCR_DDR_DO_TRIM_50PS	(1 << 30)
#define SIUL2_MSCR_DDR_DO_TRIM_100PS	(2 << 30)
#define SIUL2_MSCR_DDR_DO_TRIM_150PS	(3 << 30)

#define SIUL2_MSCR_DDR_INPUT(v)		((v) & 0x20000000)
#define SIUL2_MSCR_DDR_INPUT_CMOS	(0 << 29)
#define SIUL2_MSCR_DDR_INPUT_DIFF_DDR	(1 << 29)

#define SIUL2_MSCR_DDR_SEL(v)		((v) & 0x18000000)
#define SIUL2_MSCR_DDR_SEL_DDR3		(0 << 27)
#define SIUL2_MSCR_DDR_SEL_LPDDR2	(2 << 27)

#define SIUL2_MSCR_DDR_ODT(v)		((v) & 0x07000000)
#define SIUL2_MSCR_DDR_ODT_120ohm	(1 << 24)
#define SIUL2_MSCR_DDR_ODT_60ohm	(2 << 24)
#define SIUL2_MSCR_DDR_ODT_40ohm	(3 << 24)
#define SIUL2_MSCR_DDR_ODT_30ohm	(4 << 24)
#define SIUL2_MSCR_DDR_ODT_24ohm	(5 << 24)
#define SIUL2_MSCR_DDR_ODT_20ohm	(6 << 24)
#define SIUL2_MSCR_DDR_ODT_17ohm	(7 << 24)

#define SIUL2_MSCR_DCYCLE_TRIM(v)	((v) & 0x00C00000)
#define SIUL2_MSCR_DCYCLE_TRIM_NONE	(0 << 22)
#define SIUL2_MSCR_DCYCLE_TRIM_LEFT	(1 << 22)
#define SIUL2_MSCR_DCYCLE_TRIM_RIGHT	(2 << 22)

#define SIUL2_MSCR_OBE(v)		((v) & 0x00200000)
#define SIUL2_MSCR_OBE_EN		(1 << 21)

#define SIUL2_MSCR_ODE(v)		((v) & 0x00100000)
#define SIUL2_MSCR_ODE_EN		(1 << 20)

#define SIUL2_MSCR_IBE(v)		((v) & 0x00010000)
#define SIUL2_MSCR_IBE_EN		(1 << 19)

#define SIUL2_MSCR_HYS(v)		((v) & 0x00400000)
#define SIUL2_MSCR_HYS_EN		(1 << 18)

#define SIUL2_MSCR_INV(v)		((v) & 0x00020000)
#define SIUL2_MSCR_INV_EN		(1 << 17)

#define SIUL2_MSCR_PKE(v)		((v) & 0x00010000)
#define SIUL2_MSCR_PKE_EN		(1 << 16)

#define SIUL2_MSCR_SRE(v)		((v) & 0x0000C000)
#define SIUL2_MSCR_SRE_SPEED_LOW_50	(0 << 14)
#define SIUL2_MSCR_SRE_SPEED_LOW_100	(1 << 14)
#define SIUL2_MSCR_SRE_SPEED_HIGH_100	(2 << 14)
#define SIUL2_MSCR_SRE_SPEED_HIGH_200	(3 << 14)


#define SIUL2_MSCR_PUE(v)		((v) & 0x00002000)
#define SIUL2_MSCR_PUE_EN		(1 << 13)

#define SIUL2_MSCR_PUS(v)		((v) & 0x00001800)
#define SIUL2_MSCR_PUS_100K_DOWN	(0 << 11)
#define SIUL2_MSCR_PUS_50K_UP		(1 << 11)
#define SIUL2_MSCR_PUS_100K_UP		(2 << 11)
#define SIUL2_MSCR_PUS_33K_UP		(3 << 11)

#define SIUL2_MSCR_DSE(v)		((v) & 0x00000700)
#define SIUL2_MSCR_DSE_240ohm		(1 << 8)
#define SIUL2_MSCR_DSE_120ohm		(2 << 8)
#define SIUL2_MSCR_DSE_80ohm		(3 << 8)
#define SIUL2_MSCR_DSE_60ohm		(4 << 8)
#define SIUL2_MSCR_DSE_48ohm		(5 << 8)
#define SIUL2_MSCR_DSE_40ohm		(6 << 8)
#define SIUL2_MSCR_DSE_34ohm		(7 << 8)

#define SIUL2_MSCR_CRPOINT_TRIM(v)	((v) & 0x000000C0)
#define SIUL2_MSCR_CRPOINT_TRIM_1	(1 << 6)

#define SIUL2_MSCR_SMC(v)		((v) & 0x00000020)
#define SIUL2_MSCR_MUX_MODE(v)		((v) & 0x0000000f)
#define SIUL2_MSCR_MUX_MODE_ALT0	(0x0)
#define SIUL2_MSCR_MUX_MODE_ALT1	(0x1)
#define SIUL2_MSCR_MUX_MODE_ALT2	(0x2)
#define SIUL2_MSCR_MUX_MODE_ALT3	(0x3)

/*
 * EDMA
 */
#define S32V_EDMA_BASE              0x40002000
#define S32V_EDMATX0_IRQ            40      /*EDMA tx channel 0~15 interrupt */
#define S32V_EDMATX1_IRQ            41      /*EDMA tx channel 16~31 interrupt */
#define S32V_EDMAERR_IRQ            42
#define S32V_EDMA_NUM_CH            32

#define S32V_DMAMUX0_BASE           0x40031000
#define S32V_DMAMUX1_BASE           0x400A1000
#define S32V_DMAMUX_NUM_SLOTS       31

/*
 * CAN
 */
#define S32V_CAN1_PORT			0x40055000
#define S32V_CAN1_MEM			0x40055080
#define S32V_CAN1_IRQ			74

#define S32V_CAN2_PORT			0x400BE000
#define S32V_CAN2_MEM			0x400BE080
#define S32V_CAN2_IRQ			76

#define S32V_CAN_SIZE			0x1000

/*
 * QuadSPI
 */
#define S32V_QSPI_BASE			0x400A6000
#define S32V_QSPI_IRQ			56
#define S32V_QSPI_SIZE			0x1000

/*
 * DSPI
 */
#define S32V_SPI0_BASE			0x40057000
#define S32V_SPI0_IRQ			93
#define S32V_SPI1_BASE			0x400C0000
#define S32V_SPI1_IRQ			94
#define S32V_SPI2_BASE			0x40059000
#define S32V_SPI2_IRQ			95
#define S32V_SPI3_BASE			0x400C2000
#define S32V_SPI3_IRQ			96

#define S32V_SPI_SIZE			0x140


#endif	/* __AARCH64_S32V_H_INCLUDED */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/aarch64/s32v.h $ $Rev: 846673 $")
#endif
