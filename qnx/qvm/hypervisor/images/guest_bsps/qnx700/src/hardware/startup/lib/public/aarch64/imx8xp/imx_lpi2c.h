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

#ifndef IMX_LPI2C_H_
#define IMX_LPI2C_H_

#include <stdint.h>

#define IMX_LPI2C_VERID        0x00
#define IMX_LPI2C_PARAM        0x04
#define IMX_LPI2C_MCR          0x10
#define IMX_LPI2C_MSR          0x14
#define IMX_LPI2C_MIER         0x18
#define IMX_LPI2C_MDER         0x1C
#define IMX_LPI2C_MCFGR0       0x20
#define IMX_LPI2C_MCFGR1       0x24
#define IMX_LPI2C_MCFGR2       0x28
#define IMX_LPI2C_MCFGR3       0x2C
#define IMX_LPI2C_MDMR         0X40
#define IMX_LPI2C_MCCR0        0X48
#define IMX_LPI2C_MCCR1        0X50
#define IMX_LPI2C_MFCR         0X58
#define IMX_LPI2C_MFSR         0X5C
#define IMX_LPI2C_MTDR         0x60
#define IMX_LPI2C_MRDR         0X70

typedef volatile uint32_t vuint32_t;
typedef volatile uint8_t vuint8_t;

/* Formats 10 bit device address to two parts according to I2C Bus specifications */
#define IMX_LPI2C_XADDR1(addr)    (0xF0 | (((addr) >> 7) & 0x6))
#define IMX_LPI2C_XADDR2(addr)    ((addr) & 0xFF)
/* Specifies I2C read command */
#define IMX_LPI2C_ADDR_RD         1
/* Specifies I2C write command */
#define IMX_LPI2C_ADDR_WR         0

/** IMX_LPI2C - Register Layout Typedef */
typedef struct imx_lpi2c {
    vuint32_t verid;
    vuint32_t param;
    vuint8_t  reserved_0[8];
    vuint32_t mcr;
    vuint32_t msr;
    vuint32_t mier;
    vuint32_t mder;
    vuint32_t mcfgr0;
    vuint32_t mcfgr1;
    vuint32_t mcfgr2;
    vuint32_t mcfgr3;
    vuint8_t  reserved_1[16];
    vuint32_t mdmr;
    vuint8_t  reserved_2[4];
    vuint32_t mccr0;
    vuint8_t  reserved_3[4];
    vuint32_t mccr1;
    vuint8_t  reserved_4[4];
    vuint32_t mfcr;
    vuint32_t mfsr;
    vuint32_t mtdr;
    vuint8_t  reserved_5[12];
    vuint32_t mrdr;
    vuint8_t  reserved_6[156];
    vuint32_t scr;
    vuint32_t ssr;
    vuint32_t sier;
    vuint32_t sder;
    vuint8_t  reserved_7[4];
    vuint32_t scfgr1;
    vuint32_t scfgr2;
    vuint8_t  reserved_8[20];
    vuint32_t samr;
    vuint8_t  reserved_9[12];
    vuint32_t sasr;
    vuint32_t star;
    vuint8_t  reserved_10[8];
    vuint32_t stdr;
    vuint8_t  reserved_11[12];
    vuint32_t srdr;
} imx_lpi2c_t;

/*! @name VERID - Version ID Register */
#define IMX_LPI2C_VERID_FEATURE_MASK                 (0xFFFFU)
#define IMX_LPI2C_VERID_FEATURE_SHIFT                (0U)
#define IMX_LPI2C_VERID_FEATURE(x)                   (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_VERID_FEATURE_SHIFT)) & IMX_LPI2C_VERID_FEATURE_MASK)
#define IMX_LPI2C_VERID_MINOR_MASK                   (0xFF0000U)
#define IMX_LPI2C_VERID_MINOR_SHIFT                  (16U)
#define IMX_LPI2C_VERID_MINOR(x)                     (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_VERID_MINOR_SHIFT)) & IMX_LPI2C_VERID_MINOR_MASK)
#define IMX_LPI2C_VERID_MAJOR_MASK                   (0xFF000000U)
#define IMX_LPI2C_VERID_MAJOR_SHIFT                  (24U)
#define IMX_LPI2C_VERID_MAJOR(x)                     (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_VERID_MAJOR_SHIFT)) & IMX_LPI2C_VERID_MAJOR_MASK)

/*! @name PARAM - Parameter Register */
#define IMX_LPI2C_PARAM_MTXFIFO_MASK                 (0xFU)
#define IMX_LPI2C_PARAM_MTXFIFO_SHIFT                (0U)
#define IMX_LPI2C_PARAM_MTXFIFO(x)                   (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_PARAM_MTXFIFO_SHIFT)) & IMX_LPI2C_PARAM_MTXFIFO_MASK)
#define IMX_LPI2C_PARAM_MRXFIFO_MASK                 (0xF00U)
#define IMX_LPI2C_PARAM_MRXFIFO_SHIFT                (8U)
#define IMX_LPI2C_PARAM_MRXFIFO(x)                   (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_PARAM_MRXFIFO_SHIFT)) & IMX_LPI2C_PARAM_MRXFIFO_MASK)

/*! @name MCR - Master Control Register */
#define IMX_LPI2C_MCR_MEN_MASK                       (0x1U)
#define IMX_LPI2C_MCR_MEN_SHIFT                      (0U)
#define IMX_LPI2C_MCR_MEN(x)                         (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MCR_MEN_SHIFT)) & IMX_LPI2C_MCR_MEN_MASK)
#define IMX_LPI2C_MCR_RST_MASK                       (0x2U)
#define IMX_LPI2C_MCR_RST_SHIFT                      (1U)
#define IMX_LPI2C_MCR_RST(x)                         (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MCR_RST_SHIFT)) & IMX_LPI2C_MCR_RST_MASK)
#define IMX_LPI2C_MCR_DOZEN_MASK                     (0x4U)
#define IMX_LPI2C_MCR_DOZEN_SHIFT                    (2U)
#define IMX_LPI2C_MCR_DOZEN(x)                       (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MCR_DOZEN_SHIFT)) & IMX_LPI2C_MCR_DOZEN_MASK)
#define IMX_LPI2C_MCR_DBGEN_MASK                     (0x8U)
#define IMX_LPI2C_MCR_DBGEN_SHIFT                    (3U)
#define IMX_LPI2C_MCR_DBGEN(x)                       (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MCR_DBGEN_SHIFT)) & IMX_LPI2C_MCR_DBGEN_MASK)
#define IMX_LPI2C_MCR_RTF_MASK                       (0x100U)
#define IMX_LPI2C_MCR_RTF_SHIFT                      (8U)
#define IMX_LPI2C_MCR_RTF(x)                         (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MCR_RTF_SHIFT)) & IMX_LPI2C_MCR_RTF_MASK)
#define IMX_LPI2C_MCR_RRF_MASK                       (0x200U)
#define IMX_LPI2C_MCR_RRF_SHIFT                      (9U)
#define IMX_LPI2C_MCR_RRF(x)                         (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MCR_RRF_SHIFT)) & IMX_LPI2C_MCR_RRF_MASK)

/*! @name MSR - Master Status Register */
#define IMX_LPI2C_MSR_TDF_MASK                       (0x1U)
#define IMX_LPI2C_MSR_TDF_SHIFT                      (0U)
#define IMX_LPI2C_MSR_TDF(x)                         (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MSR_TDF_SHIFT)) & IMX_LPI2C_MSR_TDF_MASK)
#define IMX_LPI2C_MSR_RDF_MASK                       (0x2U)
#define IMX_LPI2C_MSR_RDF_SHIFT                      (1U)
#define IMX_LPI2C_MSR_RDF(x)                         (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MSR_RDF_SHIFT)) & IMX_LPI2C_MSR_RDF_MASK)
#define IMX_LPI2C_MSR_EPF_MASK                       (0x100U)
#define IMX_LPI2C_MSR_EPF_SHIFT                      (8U)
#define IMX_LPI2C_MSR_EPF(x)                         (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MSR_EPF_SHIFT)) & IMX_LPI2C_MSR_EPF_MASK)
#define IMX_LPI2C_MSR_SDF_MASK                       (0x200U)
#define IMX_LPI2C_MSR_SDF_SHIFT                      (9U)
#define IMX_LPI2C_MSR_SDF(x)                         (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MSR_SDF_SHIFT)) & IMX_LPI2C_MSR_SDF_MASK)
#define IMX_LPI2C_MSR_NDF_MASK                       (0x400U)
#define IMX_LPI2C_MSR_NDF_SHIFT                      (10U)
#define IMX_LPI2C_MSR_NDF(x)                         (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MSR_NDF_SHIFT)) & IMX_LPI2C_MSR_NDF_MASK)
#define IMX_LPI2C_MSR_ALF_MASK                       (0x800U)
#define IMX_LPI2C_MSR_ALF_SHIFT                      (11U)
#define IMX_LPI2C_MSR_ALF(x)                         (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MSR_ALF_SHIFT)) & IMX_LPI2C_MSR_ALF_MASK)
#define IMX_LPI2C_MSR_FEF_MASK                       (0x1000U)
#define IMX_LPI2C_MSR_FEF_SHIFT                      (12U)
#define IMX_LPI2C_MSR_FEF(x)                         (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MSR_FEF_SHIFT)) & IMX_LPI2C_MSR_FEF_MASK)
#define IMX_LPI2C_MSR_PLTF_MASK                      (0x2000U)
#define IMX_LPI2C_MSR_PLTF_SHIFT                     (13U)
#define IMX_LPI2C_MSR_PLTF(x)                        (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MSR_PLTF_SHIFT)) & IMX_LPI2C_MSR_PLTF_MASK)
#define IMX_LPI2C_MSR_DMF_MASK                       (0x4000U)
#define IMX_LPI2C_MSR_DMF_SHIFT                      (14U)
#define IMX_LPI2C_MSR_DMF(x)                         (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MSR_DMF_SHIFT)) & IMX_LPI2C_MSR_DMF_MASK)
#define IMX_LPI2C_MSR_MBF_MASK                       (0x1000000U)
#define IMX_LPI2C_MSR_MBF_SHIFT                      (24U)
#define IMX_LPI2C_MSR_MBF(x)                         (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MSR_MBF_SHIFT)) & IMX_LPI2C_MSR_MBF_MASK)
#define IMX_LPI2C_MSR_BBF_MASK                       (0x2000000U)
#define IMX_LPI2C_MSR_BBF_SHIFT                      (25U)
#define IMX_LPI2C_MSR_BBF(x)                         (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MSR_BBF_SHIFT)) & IMX_LPI2C_MSR_BBF_MASK)

/*! @name MIER - Master Interrupt Enable Register */
#define IMX_LPI2C_MIER_TDIE_MASK                     (0x1U)
#define IMX_LPI2C_MIER_TDIE_SHIFT                    (0U)
#define IMX_LPI2C_MIER_TDIE(x)                       (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MIER_TDIE_SHIFT)) & IMX_LPI2C_MIER_TDIE_MASK)
#define IMX_LPI2C_MIER_RDIE_MASK                     (0x2U)
#define IMX_LPI2C_MIER_RDIE_SHIFT                    (1U)
#define IMX_LPI2C_MIER_RDIE(x)                       (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MIER_RDIE_SHIFT)) & IMX_LPI2C_MIER_RDIE_MASK)
#define IMX_LPI2C_MIER_EPIE_MASK                     (0x100U)
#define IMX_LPI2C_MIER_EPIE_SHIFT                    (8U)
#define IMX_LPI2C_MIER_EPIE(x)                       (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MIER_EPIE_SHIFT)) & IMX_LPI2C_MIER_EPIE_MASK)
#define IMX_LPI2C_MIER_SDIE_MASK                     (0x200U)
#define IMX_LPI2C_MIER_SDIE_SHIFT                    (9U)
#define IMX_LPI2C_MIER_SDIE(x)                       (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MIER_SDIE_SHIFT)) & IMX_LPI2C_MIER_SDIE_MASK)
#define IMX_LPI2C_MIER_NDIE_MASK                     (0x400U)
#define IMX_LPI2C_MIER_NDIE_SHIFT                    (10U)
#define IMX_LPI2C_MIER_NDIE(x)                       (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MIER_NDIE_SHIFT)) & IMX_LPI2C_MIER_NDIE_MASK)
#define IMX_LPI2C_MIER_ALIE_MASK                     (0x800U)
#define IMX_LPI2C_MIER_ALIE_SHIFT                    (11U)
#define IMX_LPI2C_MIER_ALIE(x)                       (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MIER_ALIE_SHIFT)) & IMX_LPI2C_MIER_ALIE_MASK)
#define IMX_LPI2C_MIER_FEIE_MASK                     (0x1000U)
#define IMX_LPI2C_MIER_FEIE_SHIFT                    (12U)
#define IMX_LPI2C_MIER_FEIE(x)                       (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MIER_FEIE_SHIFT)) & IMX_LPI2C_MIER_FEIE_MASK)
#define IMX_LPI2C_MIER_PLTIE_MASK                    (0x2000U)
#define IMX_LPI2C_MIER_PLTIE_SHIFT                   (13U)
#define IMX_LPI2C_MIER_PLTIE(x)                      (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MIER_PLTIE_SHIFT)) & IMX_LPI2C_MIER_PLTIE_MASK)
#define IMX_LPI2C_MIER_DMIE_MASK                     (0x4000U)
#define IMX_LPI2C_MIER_DMIE_SHIFT                    (14U)
#define IMX_LPI2C_MIER_DMIE(x)                       (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MIER_DMIE_SHIFT)) & IMX_LPI2C_MIER_DMIE_MASK)

/*! @name MDER - Master DMA Enable Register */
#define IMX_LPI2C_MDER_TDDE_MASK                     (0x1U)
#define IMX_LPI2C_MDER_TDDE_SHIFT                    (0U)
#define IMX_LPI2C_MDER_TDDE(x)                       (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MDER_TDDE_SHIFT)) & IMX_LPI2C_MDER_TDDE_MASK)
#define IMX_LPI2C_MDER_RDDE_MASK                     (0x2U)
#define IMX_LPI2C_MDER_RDDE_SHIFT                    (1U)
#define IMX_LPI2C_MDER_RDDE(x)                       (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MDER_RDDE_SHIFT)) & IMX_LPI2C_MDER_RDDE_MASK)

/*! @name MCFGR0 - Master Configuration Register 0 */
#define IMX_LPI2C_MCFGR0_HREN_MASK                   (0x1U)
#define IMX_LPI2C_MCFGR0_HREN_SHIFT                  (0U)
#define IMX_LPI2C_MCFGR0_HREN(x)                     (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MCFGR0_HREN_SHIFT)) & IMX_LPI2C_MCFGR0_HREN_MASK)
#define IMX_LPI2C_MCFGR0_HRPOL_MASK                  (0x2U)
#define IMX_LPI2C_MCFGR0_HRPOL_SHIFT                 (1U)
#define IMX_LPI2C_MCFGR0_HRPOL(x)                    (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MCFGR0_HRPOL_SHIFT)) & IMX_LPI2C_MCFGR0_HRPOL_MASK)
#define IMX_LPI2C_MCFGR0_HRSEL_MASK                  (0x4U)
#define IMX_LPI2C_MCFGR0_HRSEL_SHIFT                 (2U)
#define IMX_LPI2C_MCFGR0_HRSEL(x)                    (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MCFGR0_HRSEL_SHIFT)) & IMX_LPI2C_MCFGR0_HRSEL_MASK)
#define IMX_LPI2C_MCFGR0_CIRFIFO_MASK                (0x100U)
#define IMX_LPI2C_MCFGR0_CIRFIFO_SHIFT               (8U)
#define IMX_LPI2C_MCFGR0_CIRFIFO(x)                  (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MCFGR0_CIRFIFO_SHIFT)) & IMX_LPI2C_MCFGR0_CIRFIFO_MASK)
#define IMX_LPI2C_MCFGR0_RDMO_MASK                   (0x200U)
#define IMX_LPI2C_MCFGR0_RDMO_SHIFT                  (9U)
#define IMX_LPI2C_MCFGR0_RDMO(x)                     (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MCFGR0_RDMO_SHIFT)) & IMX_LPI2C_MCFGR0_RDMO_MASK)

/*! @name MCFGR1 - Master Configuration Register 1 */
#define IMX_LPI2C_MCFGR1_PRESCALE_MASK               (0x7U)
#define IMX_LPI2C_MCFGR1_PRESCALE_SHIFT              (0U)
#define IMX_LPI2C_MCFGR1_PRESCALE(x)                 (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MCFGR1_PRESCALE_SHIFT)) & IMX_LPI2C_MCFGR1_PRESCALE_MASK)
#define IMX_LPI2C_MCFGR1_AUTOSTOP_MASK               (0x100U)
#define IMX_LPI2C_MCFGR1_AUTOSTOP_SHIFT              (8U)
#define IMX_LPI2C_MCFGR1_AUTOSTOP(x)                 (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MCFGR1_AUTOSTOP_SHIFT)) & IMX_LPI2C_MCFGR1_AUTOSTOP_MASK)
#define IMX_LPI2C_MCFGR1_IGNACK_MASK                 (0x200U)
#define IMX_LPI2C_MCFGR1_IGNACK_SHIFT                (9U)
#define IMX_LPI2C_MCFGR1_IGNACK(x)                   (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MCFGR1_IGNACK_SHIFT)) & IMX_LPI2C_MCFGR1_IGNACK_MASK)
#define IMX_LPI2C_MCFGR1_TIMECFG_MASK                (0x400U)
#define IMX_LPI2C_MCFGR1_TIMECFG_SHIFT               (10U)
#define IMX_LPI2C_MCFGR1_TIMECFG(x)                  (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MCFGR1_TIMECFG_SHIFT)) & IMX_LPI2C_MCFGR1_TIMECFG_MASK)
#define IMX_LPI2C_MCFGR1_MATCFG_MASK                 (0x70000U)
#define IMX_LPI2C_MCFGR1_MATCFG_SHIFT                (16U)
#define IMX_LPI2C_MCFGR1_MATCFG(x)                   (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MCFGR1_MATCFG_SHIFT)) & IMX_LPI2C_MCFGR1_MATCFG_MASK)
#define IMX_LPI2C_MCFGR1_PINCFG_MASK                 (0x7000000U)
#define IMX_LPI2C_MCFGR1_PINCFG_SHIFT                (24U)
#define IMX_LPI2C_MCFGR1_PINCFG(x)                   (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MCFGR1_PINCFG_SHIFT)) & IMX_LPI2C_MCFGR1_PINCFG_MASK)

/*! @name MCFGR2 - Master Configuration Register 2 */
#define IMX_LPI2C_MCFGR2_BUSIDLE_MASK                (0xFFFU)
#define IMX_LPI2C_MCFGR2_BUSIDLE_SHIFT               (0U)
#define IMX_LPI2C_MCFGR2_BUSIDLE(x)                  (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MCFGR2_BUSIDLE_SHIFT)) & IMX_LPI2C_MCFGR2_BUSIDLE_MASK)
#define IMX_LPI2C_MCFGR2_FILTSCL_MASK                (0xF0000U)
#define IMX_LPI2C_MCFGR2_FILTSCL_SHIFT               (16U)
#define IMX_LPI2C_MCFGR2_FILTSCL(x)                  (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MCFGR2_FILTSCL_SHIFT)) & IMX_LPI2C_MCFGR2_FILTSCL_MASK)
#define IMX_LPI2C_MCFGR2_FILTSDA_MASK                (0xF000000U)
#define IMX_LPI2C_MCFGR2_FILTSDA_SHIFT               (24U)
#define IMX_LPI2C_MCFGR2_FILTSDA(x)                  (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MCFGR2_FILTSDA_SHIFT)) & IMX_LPI2C_MCFGR2_FILTSDA_MASK)

/*! @name MCFGR3 - Master Configuration Register 3 */
#define IMX_LPI2C_MCFGR3_PINLOW_MASK                 (0xFFF00U)
#define IMX_LPI2C_MCFGR3_PINLOW_SHIFT                (8U)
#define IMX_LPI2C_MCFGR3_PINLOW(x)                   (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MCFGR3_PINLOW_SHIFT)) & IMX_LPI2C_MCFGR3_PINLOW_MASK)

/*! @name MDMR - Master Data Match Register */
#define IMX_LPI2C_MDMR_MATCH0_MASK                   (0xFFU)
#define IMX_LPI2C_MDMR_MATCH0_SHIFT                  (0U)
#define IMX_LPI2C_MDMR_MATCH0(x)                     (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MDMR_MATCH0_SHIFT)) & IMX_LPI2C_MDMR_MATCH0_MASK)
#define IMX_LPI2C_MDMR_MATCH1_MASK                   (0xFF0000U)
#define IMX_LPI2C_MDMR_MATCH1_SHIFT                  (16U)
#define IMX_LPI2C_MDMR_MATCH1(x)                     (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MDMR_MATCH1_SHIFT)) & IMX_LPI2C_MDMR_MATCH1_MASK)

/*! @name MCCR0 - Master Clock Configuration Register 0 */
#define IMX_LPI2C_MCCR0_CLKLO_MASK                   (0x3FU)
#define IMX_LPI2C_MCCR0_CLKLO_SHIFT                  (0U)
#define IMX_LPI2C_MCCR0_CLKLO(x)                     (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MCCR0_CLKLO_SHIFT)) & IMX_LPI2C_MCCR0_CLKLO_MASK)
#define IMX_LPI2C_MCCR0_CLKHI_MASK                   (0x3F00U)
#define IMX_LPI2C_MCCR0_CLKHI_SHIFT                  (8U)
#define IMX_LPI2C_MCCR0_CLKHI(x)                     (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MCCR0_CLKHI_SHIFT)) & IMX_LPI2C_MCCR0_CLKHI_MASK)
#define IMX_LPI2C_MCCR0_SETHOLD_MASK                 (0x3F0000U)
#define IMX_LPI2C_MCCR0_SETHOLD_SHIFT                (16U)
#define IMX_LPI2C_MCCR0_SETHOLD(x)                   (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MCCR0_SETHOLD_SHIFT)) & IMX_LPI2C_MCCR0_SETHOLD_MASK)
#define IMX_LPI2C_MCCR0_DATAVD_MASK                  (0x3F000000U)
#define IMX_LPI2C_MCCR0_DATAVD_SHIFT                 (24U)
#define IMX_LPI2C_MCCR0_DATAVD(x)                    (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MCCR0_DATAVD_SHIFT)) & IMX_LPI2C_MCCR0_DATAVD_MASK)

/*! @name MCCR1 - Master Clock Configuration Register 1 */
#define IMX_LPI2C_MCCR1_CLKLO_MASK                   (0x3FU)
#define IMX_LPI2C_MCCR1_CLKLO_SHIFT                  (0U)
#define IMX_LPI2C_MCCR1_CLKLO(x)                     (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MCCR1_CLKLO_SHIFT)) & IMX_LPI2C_MCCR1_CLKLO_MASK)
#define IMX_LPI2C_MCCR1_CLKHI_MASK                   (0x3F00U)
#define IMX_LPI2C_MCCR1_CLKHI_SHIFT                  (8U)
#define IMX_LPI2C_MCCR1_CLKHI(x)                     (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MCCR1_CLKHI_SHIFT)) & IMX_LPI2C_MCCR1_CLKHI_MASK)
#define IMX_LPI2C_MCCR1_SETHOLD_MASK                 (0x3F0000U)
#define IMX_LPI2C_MCCR1_SETHOLD_SHIFT                (16U)
#define IMX_LPI2C_MCCR1_SETHOLD(x)                   (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MCCR1_SETHOLD_SHIFT)) & IMX_LPI2C_MCCR1_SETHOLD_MASK)
#define IMX_LPI2C_MCCR1_DATAVD_MASK                  (0x3F000000U)
#define IMX_LPI2C_MCCR1_DATAVD_SHIFT                 (24U)
#define IMX_LPI2C_MCCR1_DATAVD(x)                    (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MCCR1_DATAVD_SHIFT)) & IMX_LPI2C_MCCR1_DATAVD_MASK)

/*! @name MFCR - Master FIFO Control Register */
#define IMX_LPI2C_MFCR_TXWATER_MASK                  (0xFFU)
#define IMX_LPI2C_MFCR_TXWATER_SHIFT                 (0U)
#define IMX_LPI2C_MFCR_TXWATER(x)                    (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MFCR_TXWATER_SHIFT)) & IMX_LPI2C_MFCR_TXWATER_MASK)
#define IMX_LPI2C_MFCR_RXWATER_MASK                  (0xFF0000U)
#define IMX_LPI2C_MFCR_RXWATER_SHIFT                 (16U)
#define IMX_LPI2C_MFCR_RXWATER(x)                    (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MFCR_RXWATER_SHIFT)) & IMX_LPI2C_MFCR_RXWATER_MASK)

/*! @name MFSR - Master FIFO Status Register */
#define IMX_LPI2C_MFSR_TXCOUNT_MASK                  (0xFFU)
#define IMX_LPI2C_MFSR_TXCOUNT_SHIFT                 (0U)
#define IMX_LPI2C_MFSR_TXCOUNT(x)                    (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MFSR_TXCOUNT_SHIFT)) & IMX_LPI2C_MFSR_TXCOUNT_MASK)
#define IMX_LPI2C_MFSR_RXCOUNT_MASK                  (0xFF0000U)
#define IMX_LPI2C_MFSR_RXCOUNT_SHIFT                 (16U)
#define IMX_LPI2C_MFSR_RXCOUNT(x)                    (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MFSR_RXCOUNT_SHIFT)) & IMX_LPI2C_MFSR_RXCOUNT_MASK)

/*! @name MTDR - Master Transmit Data Register */
#define IMX_LPI2C_MTDR_DATA_MASK                     (0xFFU)
#define IMX_LPI2C_MTDR_DATA_SHIFT                    (0U)
#define IMX_LPI2C_MTDR_DATA(x)                       (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MTDR_DATA_SHIFT)) & IMX_LPI2C_MTDR_DATA_MASK)
#define IMX_LPI2C_MTDR_CMD_MASK                      (0x700U)
#define IMX_LPI2C_MTDR_CMD_SHIFT                     (8U)
#define IMX_LPI2C_MTDR_CMD(x)                        (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MTDR_CMD_SHIFT)) & IMX_LPI2C_MTDR_CMD_MASK)

/*! @name MRDR - Master Receive Data Register */
#define IMX_LPI2C_MRDR_DATA_MASK                     (0xFFU)
#define IMX_LPI2C_MRDR_DATA_SHIFT                    (0U)
#define IMX_LPI2C_MRDR_DATA(x)                       (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MRDR_DATA_SHIFT)) & IMX_LPI2C_MRDR_DATA_MASK)
#define IMX_LPI2C_MRDR_RXEMPTY_MASK                  (0x4000U)
#define IMX_LPI2C_MRDR_RXEMPTY_SHIFT                 (14U)
#define IMX_LPI2C_MRDR_RXEMPTY(x)                    (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_MRDR_RXEMPTY_SHIFT)) & IMX_LPI2C_MRDR_RXEMPTY_MASK)

/*! @name SCR - Slave Control Register */
#define IMX_LPI2C_SCR_SEN_MASK                       (0x1U)
#define IMX_LPI2C_SCR_SEN_SHIFT                      (0U)
#define IMX_LPI2C_SCR_SEN(x)                         (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SCR_SEN_SHIFT)) & IMX_LPI2C_SCR_SEN_MASK)
#define IMX_LPI2C_SCR_RST_MASK                       (0x2U)
#define IMX_LPI2C_SCR_RST_SHIFT                      (1U)
#define IMX_LPI2C_SCR_RST(x)                         (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SCR_RST_SHIFT)) & IMX_LPI2C_SCR_RST_MASK)
#define IMX_LPI2C_SCR_FILTEN_MASK                    (0x10U)
#define IMX_LPI2C_SCR_FILTEN_SHIFT                   (4U)
#define IMX_LPI2C_SCR_FILTEN(x)                      (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SCR_FILTEN_SHIFT)) & IMX_LPI2C_SCR_FILTEN_MASK)
#define IMX_LPI2C_SCR_FILTDZ_MASK                    (0x20U)
#define IMX_LPI2C_SCR_FILTDZ_SHIFT                   (5U)
#define IMX_LPI2C_SCR_FILTDZ(x)                      (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SCR_FILTDZ_SHIFT)) & IMX_LPI2C_SCR_FILTDZ_MASK)
#define IMX_LPI2C_SCR_RTF_MASK                       (0x100U)
#define IMX_LPI2C_SCR_RTF_SHIFT                      (8U)
#define IMX_LPI2C_SCR_RTF(x)                         (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SCR_RTF_SHIFT)) & IMX_LPI2C_SCR_RTF_MASK)
#define IMX_LPI2C_SCR_RRF_MASK                       (0x200U)
#define IMX_LPI2C_SCR_RRF_SHIFT                      (9U)
#define IMX_LPI2C_SCR_RRF(x)                         (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SCR_RRF_SHIFT)) & IMX_LPI2C_SCR_RRF_MASK)

/*! @name SSR - Slave Status Register */
#define IMX_LPI2C_SSR_TDF_MASK                       (0x1U)
#define IMX_LPI2C_SSR_TDF_SHIFT                      (0U)
#define IMX_LPI2C_SSR_TDF(x)                         (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SSR_TDF_SHIFT)) & IMX_LPI2C_SSR_TDF_MASK)
#define IMX_LPI2C_SSR_RDF_MASK                       (0x2U)
#define IMX_LPI2C_SSR_RDF_SHIFT                      (1U)
#define IMX_LPI2C_SSR_RDF(x)                         (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SSR_RDF_SHIFT)) & IMX_LPI2C_SSR_RDF_MASK)
#define IMX_LPI2C_SSR_AVF_MASK                       (0x4U)
#define IMX_LPI2C_SSR_AVF_SHIFT                      (2U)
#define IMX_LPI2C_SSR_AVF(x)                         (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SSR_AVF_SHIFT)) & IMX_LPI2C_SSR_AVF_MASK)
#define IMX_LPI2C_SSR_TAF_MASK                       (0x8U)
#define IMX_LPI2C_SSR_TAF_SHIFT                      (3U)
#define IMX_LPI2C_SSR_TAF(x)                         (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SSR_TAF_SHIFT)) & IMX_LPI2C_SSR_TAF_MASK)
#define IMX_LPI2C_SSR_RSF_MASK                       (0x100U)
#define IMX_LPI2C_SSR_RSF_SHIFT                      (8U)
#define IMX_LPI2C_SSR_RSF(x)                         (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SSR_RSF_SHIFT)) & IMX_LPI2C_SSR_RSF_MASK)
#define IMX_LPI2C_SSR_SDF_MASK                       (0x200U)
#define IMX_LPI2C_SSR_SDF_SHIFT                      (9U)
#define IMX_LPI2C_SSR_SDF(x)                         (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SSR_SDF_SHIFT)) & IMX_LPI2C_SSR_SDF_MASK)
#define IMX_LPI2C_SSR_BEF_MASK                       (0x400U)
#define IMX_LPI2C_SSR_BEF_SHIFT                      (10U)
#define IMX_LPI2C_SSR_BEF(x)                         (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SSR_BEF_SHIFT)) & IMX_LPI2C_SSR_BEF_MASK)
#define IMX_LPI2C_SSR_FEF_MASK                       (0x800U)
#define IMX_LPI2C_SSR_FEF_SHIFT                      (11U)
#define IMX_LPI2C_SSR_FEF(x)                         (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SSR_FEF_SHIFT)) & IMX_LPI2C_SSR_FEF_MASK)
#define IMX_LPI2C_SSR_AM0F_MASK                      (0x1000U)
#define IMX_LPI2C_SSR_AM0F_SHIFT                     (12U)
#define IMX_LPI2C_SSR_AM0F(x)                        (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SSR_AM0F_SHIFT)) & IMX_LPI2C_SSR_AM0F_MASK)
#define IMX_LPI2C_SSR_AM1F_MASK                      (0x2000U)
#define IMX_LPI2C_SSR_AM1F_SHIFT                     (13U)
#define IMX_LPI2C_SSR_AM1F(x)                        (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SSR_AM1F_SHIFT)) & IMX_LPI2C_SSR_AM1F_MASK)
#define IMX_LPI2C_SSR_GCF_MASK                       (0x4000U)
#define IMX_LPI2C_SSR_GCF_SHIFT                      (14U)
#define IMX_LPI2C_SSR_GCF(x)                         (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SSR_GCF_SHIFT)) & IMX_LPI2C_SSR_GCF_MASK)
#define IMX_LPI2C_SSR_SARF_MASK                      (0x8000U)
#define IMX_LPI2C_SSR_SARF_SHIFT                     (15U)
#define IMX_LPI2C_SSR_SARF(x)                        (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SSR_SARF_SHIFT)) & IMX_LPI2C_SSR_SARF_MASK)
#define IMX_LPI2C_SSR_SBF_MASK                       (0x1000000U)
#define IMX_LPI2C_SSR_SBF_SHIFT                      (24U)
#define IMX_LPI2C_SSR_SBF(x)                         (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SSR_SBF_SHIFT)) & IMX_LPI2C_SSR_SBF_MASK)
#define IMX_LPI2C_SSR_BBF_MASK                       (0x2000000U)
#define IMX_LPI2C_SSR_BBF_SHIFT                      (25U)
#define IMX_LPI2C_SSR_BBF(x)                         (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SSR_BBF_SHIFT)) & IMX_LPI2C_SSR_BBF_MASK)

/*! @name SIER - Slave Interrupt Enable Register */
#define IMX_LPI2C_SIER_TDIE_MASK                     (0x1U)
#define IMX_LPI2C_SIER_TDIE_SHIFT                    (0U)
#define IMX_LPI2C_SIER_TDIE(x)                       (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SIER_TDIE_SHIFT)) & IMX_LPI2C_SIER_TDIE_MASK)
#define IMX_LPI2C_SIER_RDIE_MASK                     (0x2U)
#define IMX_LPI2C_SIER_RDIE_SHIFT                    (1U)
#define IMX_LPI2C_SIER_RDIE(x)                       (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SIER_RDIE_SHIFT)) & IMX_LPI2C_SIER_RDIE_MASK)
#define IMX_LPI2C_SIER_AVIE_MASK                     (0x4U)
#define IMX_LPI2C_SIER_AVIE_SHIFT                    (2U)
#define IMX_LPI2C_SIER_AVIE(x)                       (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SIER_AVIE_SHIFT)) & IMX_LPI2C_SIER_AVIE_MASK)
#define IMX_LPI2C_SIER_TAIE_MASK                     (0x8U)
#define IMX_LPI2C_SIER_TAIE_SHIFT                    (3U)
#define IMX_LPI2C_SIER_TAIE(x)                       (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SIER_TAIE_SHIFT)) & IMX_LPI2C_SIER_TAIE_MASK)
#define IMX_LPI2C_SIER_RSIE_MASK                     (0x100U)
#define IMX_LPI2C_SIER_RSIE_SHIFT                    (8U)
#define IMX_LPI2C_SIER_RSIE(x)                       (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SIER_RSIE_SHIFT)) & IMX_LPI2C_SIER_RSIE_MASK)
#define IMX_LPI2C_SIER_SDIE_MASK                     (0x200U)
#define IMX_LPI2C_SIER_SDIE_SHIFT                    (9U)
#define IMX_LPI2C_SIER_SDIE(x)                       (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SIER_SDIE_SHIFT)) & IMX_LPI2C_SIER_SDIE_MASK)
#define IMX_LPI2C_SIER_BEIE_MASK                     (0x400U)
#define IMX_LPI2C_SIER_BEIE_SHIFT                    (10U)
#define IMX_LPI2C_SIER_BEIE(x)                       (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SIER_BEIE_SHIFT)) & IMX_LPI2C_SIER_BEIE_MASK)
#define IMX_LPI2C_SIER_FEIE_MASK                     (0x800U)
#define IMX_LPI2C_SIER_FEIE_SHIFT                    (11U)
#define IMX_LPI2C_SIER_FEIE(x)                       (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SIER_FEIE_SHIFT)) & IMX_LPI2C_SIER_FEIE_MASK)
#define IMX_LPI2C_SIER_AM0IE_MASK                    (0x1000U)
#define IMX_LPI2C_SIER_AM0IE_SHIFT                   (12U)
#define IMX_LPI2C_SIER_AM0IE(x)                      (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SIER_AM0IE_SHIFT)) & IMX_LPI2C_SIER_AM0IE_MASK)
#define IMX_LPI2C_SIER_AM1F_MASK                     (0x2000U)
#define IMX_LPI2C_SIER_AM1F_SHIFT                    (13U)
#define IMX_LPI2C_SIER_AM1F(x)                       (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SIER_AM1F_SHIFT)) & IMX_LPI2C_SIER_AM1F_MASK)
#define IMX_LPI2C_SIER_GCIE_MASK                     (0x4000U)
#define IMX_LPI2C_SIER_GCIE_SHIFT                    (14U)
#define IMX_LPI2C_SIER_GCIE(x)                       (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SIER_GCIE_SHIFT)) & IMX_LPI2C_SIER_GCIE_MASK)
#define IMX_LPI2C_SIER_SARIE_MASK                    (0x8000U)
#define IMX_LPI2C_SIER_SARIE_SHIFT                   (15U)
#define IMX_LPI2C_SIER_SARIE(x)                      (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SIER_SARIE_SHIFT)) & IMX_LPI2C_SIER_SARIE_MASK)

/*! @name SDER - Slave DMA Enable Register */
#define IMX_LPI2C_SDER_TDDE_MASK                     (0x1U)
#define IMX_LPI2C_SDER_TDDE_SHIFT                    (0U)
#define IMX_LPI2C_SDER_TDDE(x)                       (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SDER_TDDE_SHIFT)) & IMX_LPI2C_SDER_TDDE_MASK)
#define IMX_LPI2C_SDER_RDDE_MASK                     (0x2U)
#define IMX_LPI2C_SDER_RDDE_SHIFT                    (1U)
#define IMX_LPI2C_SDER_RDDE(x)                       (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SDER_RDDE_SHIFT)) & IMX_LPI2C_SDER_RDDE_MASK)
#define IMX_LPI2C_SDER_AVDE_MASK                     (0x4U)
#define IMX_LPI2C_SDER_AVDE_SHIFT                    (2U)
#define IMX_LPI2C_SDER_AVDE(x)                       (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SDER_AVDE_SHIFT)) & IMX_LPI2C_SDER_AVDE_MASK)

/*! @name SCFGR1 - Slave Configuration Register 1 */
#define IMX_LPI2C_SCFGR1_ADRSTALL_MASK               (0x1U)
#define IMX_LPI2C_SCFGR1_ADRSTALL_SHIFT              (0U)
#define IMX_LPI2C_SCFGR1_ADRSTALL(x)                 (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SCFGR1_ADRSTALL_SHIFT)) & IMX_LPI2C_SCFGR1_ADRSTALL_MASK)
#define IMX_LPI2C_SCFGR1_RXSTALL_MASK                (0x2U)
#define IMX_LPI2C_SCFGR1_RXSTALL_SHIFT               (1U)
#define IMX_LPI2C_SCFGR1_RXSTALL(x)                  (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SCFGR1_RXSTALL_SHIFT)) & IMX_LPI2C_SCFGR1_RXSTALL_MASK)
#define IMX_LPI2C_SCFGR1_TXDSTALL_MASK               (0x4U)
#define IMX_LPI2C_SCFGR1_TXDSTALL_SHIFT              (2U)
#define IMX_LPI2C_SCFGR1_TXDSTALL(x)                 (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SCFGR1_TXDSTALL_SHIFT)) & IMX_LPI2C_SCFGR1_TXDSTALL_MASK)
#define IMX_LPI2C_SCFGR1_ACKSTALL_MASK               (0x8U)
#define IMX_LPI2C_SCFGR1_ACKSTALL_SHIFT              (3U)
#define IMX_LPI2C_SCFGR1_ACKSTALL(x)                 (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SCFGR1_ACKSTALL_SHIFT)) & IMX_LPI2C_SCFGR1_ACKSTALL_MASK)
#define IMX_LPI2C_SCFGR1_GCEN_MASK                   (0x100U)
#define IMX_LPI2C_SCFGR1_GCEN_SHIFT                  (8U)
#define IMX_LPI2C_SCFGR1_GCEN(x)                     (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SCFGR1_GCEN_SHIFT)) & IMX_LPI2C_SCFGR1_GCEN_MASK)
#define IMX_LPI2C_SCFGR1_SAEN_MASK                   (0x200U)
#define IMX_LPI2C_SCFGR1_SAEN_SHIFT                  (9U)
#define IMX_LPI2C_SCFGR1_SAEN(x)                     (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SCFGR1_SAEN_SHIFT)) & IMX_LPI2C_SCFGR1_SAEN_MASK)
#define IMX_LPI2C_SCFGR1_TXCFG_MASK                  (0x400U)
#define IMX_LPI2C_SCFGR1_TXCFG_SHIFT                 (10U)
#define IMX_LPI2C_SCFGR1_TXCFG(x)                    (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SCFGR1_TXCFG_SHIFT)) & IMX_LPI2C_SCFGR1_TXCFG_MASK)
#define IMX_LPI2C_SCFGR1_RXCFG_MASK                  (0x800U)
#define IMX_LPI2C_SCFGR1_RXCFG_SHIFT                 (11U)
#define IMX_LPI2C_SCFGR1_RXCFG(x)                    (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SCFGR1_RXCFG_SHIFT)) & IMX_LPI2C_SCFGR1_RXCFG_MASK)
#define IMX_LPI2C_SCFGR1_IGNACK_MASK                 (0x1000U)
#define IMX_LPI2C_SCFGR1_IGNACK_SHIFT                (12U)
#define IMX_LPI2C_SCFGR1_IGNACK(x)                   (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SCFGR1_IGNACK_SHIFT)) & IMX_LPI2C_SCFGR1_IGNACK_MASK)
#define IMX_LPI2C_SCFGR1_HSMEN_MASK                  (0x2000U)
#define IMX_LPI2C_SCFGR1_HSMEN_SHIFT                 (13U)
#define IMX_LPI2C_SCFGR1_HSMEN(x)                    (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SCFGR1_HSMEN_SHIFT)) & IMX_LPI2C_SCFGR1_HSMEN_MASK)
#define IMX_LPI2C_SCFGR1_ADDRCFG_MASK                (0x70000U)
#define IMX_LPI2C_SCFGR1_ADDRCFG_SHIFT               (16U)
#define IMX_LPI2C_SCFGR1_ADDRCFG(x)                  (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SCFGR1_ADDRCFG_SHIFT)) & IMX_LPI2C_SCFGR1_ADDRCFG_MASK)

/*! @name SCFGR2 - Slave Configuration Register 2 */
#define IMX_LPI2C_SCFGR2_CLKHOLD_MASK                (0xFU)
#define IMX_LPI2C_SCFGR2_CLKHOLD_SHIFT               (0U)
#define IMX_LPI2C_SCFGR2_CLKHOLD(x)                  (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SCFGR2_CLKHOLD_SHIFT)) & IMX_LPI2C_SCFGR2_CLKHOLD_MASK)
#define IMX_LPI2C_SCFGR2_DATAVD_MASK                 (0x3F00U)
#define IMX_LPI2C_SCFGR2_DATAVD_SHIFT                (8U)
#define IMX_LPI2C_SCFGR2_DATAVD(x)                   (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SCFGR2_DATAVD_SHIFT)) & IMX_LPI2C_SCFGR2_DATAVD_MASK)
#define IMX_LPI2C_SCFGR2_FILTSCL_MASK                (0xF0000U)
#define IMX_LPI2C_SCFGR2_FILTSCL_SHIFT               (16U)
#define IMX_LPI2C_SCFGR2_FILTSCL(x)                  (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SCFGR2_FILTSCL_SHIFT)) & IMX_LPI2C_SCFGR2_FILTSCL_MASK)
#define IMX_LPI2C_SCFGR2_FILTSDA_MASK                (0xF000000U)
#define IMX_LPI2C_SCFGR2_FILTSDA_SHIFT               (24U)
#define IMX_LPI2C_SCFGR2_FILTSDA(x)                  (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SCFGR2_FILTSDA_SHIFT)) & IMX_LPI2C_SCFGR2_FILTSDA_MASK)

/*! @name SAMR - Slave Address Match Register */
#define IMX_LPI2C_SAMR_ADDR0_MASK                    (0x7FEU)
#define IMX_LPI2C_SAMR_ADDR0_SHIFT                   (1U)
#define IMX_LPI2C_SAMR_ADDR0(x)                      (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SAMR_ADDR0_SHIFT)) & IMX_LPI2C_SAMR_ADDR0_MASK)
#define IMX_LPI2C_SAMR_ADDR1_MASK                    (0x7FE0000U)
#define IMX_LPI2C_SAMR_ADDR1_SHIFT                   (17U)
#define IMX_LPI2C_SAMR_ADDR1(x)                      (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SAMR_ADDR1_SHIFT)) & IMX_LPI2C_SAMR_ADDR1_MASK)

/*! @name SASR - Slave Address Status Register */
#define IMX_LPI2C_SASR_RADDR_MASK                    (0x7FFU)
#define IMX_LPI2C_SASR_RADDR_SHIFT                   (0U)
#define IMX_LPI2C_SASR_RADDR(x)                      (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SASR_RADDR_SHIFT)) & IMX_LPI2C_SASR_RADDR_MASK)
#define IMX_LPI2C_SASR_ANV_MASK                      (0x4000U)
#define IMX_LPI2C_SASR_ANV_SHIFT                     (14U)
#define IMX_LPI2C_SASR_ANV(x)                        (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SASR_ANV_SHIFT)) & IMX_LPI2C_SASR_ANV_MASK)

/*! @name STAR - Slave Transmit ACK Register */
#define IMX_LPI2C_STAR_TXNACK_MASK                   (0x1U)
#define IMX_LPI2C_STAR_TXNACK_SHIFT                  (0U)
#define IMX_LPI2C_STAR_TXNACK(x)                     (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_STAR_TXNACK_SHIFT)) & IMX_LPI2C_STAR_TXNACK_MASK)

/*! @name STDR - Slave Transmit Data Register */
#define IMX_LPI2C_STDR_DATA_MASK                     (0xFFU)
#define IMX_LPI2C_STDR_DATA_SHIFT                    (0U)
#define IMX_LPI2C_STDR_DATA(x)                       (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_STDR_DATA_SHIFT)) & IMX_LPI2C_STDR_DATA_MASK)

/*! @name SRDR - Slave Receive Data Register */
#define IMX_LPI2C_SRDR_DATA_MASK                     (0xFFU)
#define IMX_LPI2C_SRDR_DATA_SHIFT                    (0U)
#define IMX_LPI2C_SRDR_DATA(x)                       (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SRDR_DATA_SHIFT)) & IMX_LPI2C_SRDR_DATA_MASK)
#define IMX_LPI2C_SRDR_RXEMPTY_MASK                  (0x4000U)
#define IMX_LPI2C_SRDR_RXEMPTY_SHIFT                 (14U)
#define IMX_LPI2C_SRDR_RXEMPTY(x)                    (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SRDR_RXEMPTY_SHIFT)) & IMX_LPI2C_SRDR_RXEMPTY_MASK)
#define IMX_LPI2C_SRDR_SOF_MASK                      (0x8000U)
#define IMX_LPI2C_SRDR_SOF_SHIFT                     (15U)
#define IMX_LPI2C_SRDR_SOF(x)                        (((uint32_t)(((uint32_t)(x)) << IMX_LPI2C_SRDR_SOF_SHIFT)) & IMX_LPI2C_SRDR_SOF_MASK)

typedef enum _imx_i2c_cmd {
    IMX_I2C_TX_DATA = (0 << IMX_LPI2C_MTDR_CMD_SHIFT),
    IMX_I2C_RX_DATA = (1 << IMX_LPI2C_MTDR_CMD_SHIFT),
    IMX_I2C_STOP = (2 << IMX_LPI2C_MTDR_CMD_SHIFT),
    IMX_I2C_RX_DISCARD = (3 << IMX_LPI2C_MTDR_CMD_SHIFT),
    IMX_I2C_START = (4 << IMX_LPI2C_MTDR_CMD_SHIFT),
    IMX_I2C_START_NACK = (5 << IMX_LPI2C_MTDR_CMD_SHIFT),
    IMX_I2C_START_HISPEED = (6 << IMX_LPI2C_MTDR_CMD_SHIFT),
    IMX_I2C_START_NACK_HISPEED = (7 << IMX_LPI2C_MTDR_CMD_SHIFT)
} imx_i2c_cmd_t;

#endif /* IMX_LPI2C_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/aarch64/imx8xp/imx_lpi2c.h $ $Rev: 850159 $")
#endif
