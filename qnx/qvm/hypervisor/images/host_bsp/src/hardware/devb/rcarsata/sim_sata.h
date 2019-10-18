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

// Module Description: SATA interface manifests

#ifndef _SIM_SATA_H_INCLUDED
#define _SIM_SATA_H_INCLUDED

struct _eide_target;

#define SATA_DFLT_TAG                   0
#define SATA_DEFAULT_BASE_LEN           0x2000

#define _SLOGC_SIM_RCAR_SATA            __C(_SLOGC_SIM, 1900)

#define DRV_NAME "sata_rcar"

/* SH-Navi2G/ATAPI-ATA compatible task registers */
#define DATA_REG                        0x100
#define SDEVCON_REG                     0x138

/* SH-Navi2G/ATAPI module compatible control registers */
#define ATAPI_CONTROL1_REG              0x180
#define ATAPI_STATUS_REG                0x184
#define ATAPI_INT_ENABLE_REG            0x188
#define ATAPI_DTB_ADR_REG               0x198
#define ATAPI_DMA_START_ADR_REG         0x19C
#define ATAPI_DMA_TRANS_CNT_REG         0x1A0
#define ATAPI_CONTROL2_REG              0x1A4
#define ATAPI_SIG_ST_REG                0x1B0
#define ATAPI_BYTE_SWAP_REG             0x1BC

/* ATAPI control 1 register (ATAPI_CONTROL1) bits */
#define ATAPI_CONTROL1_ISM              (1 << 16)
#define ATAPI_CONTROL1_DTA32M           (1 << 11)
#define ATAPI_CONTROL1_RESET            (1 << 7)
#define ATAPI_CONTROL1_DESE             (1 << 3)
#define ATAPI_CONTROL1_RW               (1 << 2)
#define ATAPI_CONTROL1_STOP             (1 << 1)
#define ATAPI_CONTROL1_START            (1 << 0)

/* ATAPI status register (ATAPI_STATUS) bits */
#define ATAPI_STATUS_SATAINT            (1 << 11)
#define ATAPI_STATUS_DNEND              (1 << 6)
#define ATAPI_STATUS_DEVTRM             (1 << 5)
#define ATAPI_STATUS_DEVINT             (1 << 4)
#define ATAPI_STATUS_ERR                (1 << 2)
#define ATAPI_STATUS_NEND               (1 << 1)
#define ATAPI_STATUS_ACT                (1 << 0)

/* Interrupt enable register (ATAPI_INT_ENABLE) bits */
#define ATAPI_INT_ENABLE_SATAINT        (1 << 11)
#define ATAPI_INT_ENABLE_DNEND          (1 << 6)
#define ATAPI_INT_ENABLE_DEVTRM         (1 << 5)
#define ATAPI_INT_ENABLE_DEVINT         (1 << 4)
#define ATAPI_INT_ENABLE_ERR            (1 << 2)
#define ATAPI_INT_ENABLE_NEND           (1 << 1)
#define ATAPI_INT_ENABLE_ACT            (1 << 0)

/* Access control registers for physical layer control register */
#define SATAPHYADDR_REG                 0x200
#define SATAPHYWDATA_REG                0x204
#define SATAPHYACCEN_REG                0x208
#define SATAPHYRESET_REG                0x20C
#define SATAPHYRDATA_REG                0x210
#define SATAPHYACK_REG                  0x214

/* Physical layer control address command register (SATAPHYADDR) bits */
#define SATAPHYADDR_PHYRATEMODE         (1 << 10)
#define SATAPHYADDR_PHYCMD_READ         (1 << 9)
#define SATAPHYADDR_PHYCMD_WRITE        (1 << 8)

/* Physical layer control enable register (SATAPHYACCEN) bits */
#define SATAPHYACCEN_PHYLANE            (1 << 0)

/* Physical layer control reset register (SATAPHYRESET) bits */
#define SATAPHYRESET_PHYRST             (1 << 1)
#define SATAPHYRESET_PHYSRES            (1 << 0)

/* Physical layer control acknowledge register (SATAPHYACK) bits */
#define SATAPHYACK_PHYACK               (1 << 0)

/* Serial-ATA HOST control registers */
#define BISTCONF_REG                    0x102C
#define SDATA_REG                       0x1100
#define SSERROR_REG                     0x1104
#define SSFEATURES_REG                  0x1104
#define SDEVHEAD_REG                    0x1118
#define SSSTATUS_REG                    0x111C
#define SSDEVCON_REG                    0x1204
#define SSALTSTS_REG                    0x1204

#define SCRSSTS_REG                     0x1400
#define SCRSERR_REG                     0x1404
#define SCRSCON_REG                     0x1408
#define SCRSACT_REG                     0x140C

#define SATAINTSTAT_REG                 0x1508
#define SATAINTMASK_REG                 0x150C

/* SATA INT status register (SATAINTSTAT) bits */
#define SATAINTSTAT_SERR                (1 << 3)
#define SATAINTSTAT_ATA                 (1 << 0)

/* SATA INT mask register (SATAINTSTAT) bits */
#define SATAINTMASK_SERRMSK             (1 << 3)
#define SATAINTMASK_ERRMSK              (1 << 2)
#define SATAINTMASK_ERRCRTMSK           (1 << 1)
#define SATAINTMASK_ATAMSK              (1 << 0)

#define SATA_RCAR_INT_MASK              (SATAINTMASK_SERRMSK | SATAINTMASK_ATAMSK)

/* Physical Layer Control Registers */
#define SATAPCTLR1_REG                  0x43
#define SATAPCTLR2_REG                  0x52
#define SATAPCTLR3_REG                  0x5A
#define SATAPCTLR4_REG                  0x60

/* Descriptor table word 0 bit (when DTA32M = 1) */
#define SATA_RCAR_DTEND                 (1 << 0)

#define SATA_RCAR_DMA_BOUNDARY          0x1FFFFFFEUL

/* SATA HBA capabilities */
#define SATA_CAP_SNCQ                   0x00000010  // Supports Native Command Queuing
#define SATA_CAP_SSS                    0x00000008  // Supports Staggered Spin-up
#define SATA_CAP_SPM                    0x00000004  // Supports Port Multiplier
#define SATA_CAP_PMD                    0x00000002  // PIO Multiple DRQ Block
#define SATA_CAP_ATAPI                  0x00000001  // Supports ATAPI devices

#define SATA_COMM_INIT_DELAY            1           // 1ms
#define SATA_COMM_INIT_RETRIES          2
#define SATA_COMM_INIT_PWAIT            200         // 200ms
/* Delay before performing ATA Read Capacity command */
#define ATA_RD_CAP_CMD_DELAY            500

#define SCR_STATUS                      0
 #define SSTS_DET_MSK                   0x0000000f
 #define SSTS_DET_NO_DEVICE             0x00000000
 #define SSTS_DET_DEV_PRESENT           0x00000001
 #define SSTS_DET_PHY_ONLINE            0x00000003
 #define SSTS_DET_PHY_OFFLINE           0x00000004

 #define SSTS_SPD_MSK                   0x000000f0
 #define SSTS_SPD_GEN1                  0x00000010
 #define SSTS_SPD_GEN2                  0x00000020
 #define SSTS_SPD_GEN3                  0x00000030

 #define SSTS_IPM_MSK                   0x00000f00
 #define SSTS_IPM_NO_DEVICE             0x00000000
 #define SSTS_IPM_ACTIVE                0x00000100
 #define SSTS_IPM_PARTIAL               0x00000200
 #define SSTS_IPM_SLUMBER               0x00000600

#define SCR_ERROR                       1
 #define SERR_DATA_RECOVERED            0x00000001  /* recovered data error */
 #define SERR_COMM_RECOVERED            0x00000002  /* recovered comm failure */
 #define SERR_DATA                      0x00000100  /* unrecovered data error */
 #define SERR_PERSISTENT                0x00000200  /* persistent data/comm error */
 #define SERR_PROTOCOL                  0x00000400  /* protocol violation */
 #define SERR_INTERNAL                  0x00000800  /* host internal error */
 #define SERR_PHYRDY_CHG                0x00010000  /* PHY RDY changed */
 #define SERR_PHY_INT_ERR               0x00020000  /* PHY internal error */
 #define SERR_COMM_WAKE                 0x00040000  /* Comm wake */
 #define SERR_10B_8B_ERR                0x00080000  /* 10b to 8b decode error */
 #define SERR_DISPARITY                 0x00100000  /* Disparity */
 #define SERR_CRC                       0x00200000  /* CRC error */
 #define SERR_HANDSHAKE                 0x00400000  /* Handshake error */
 #define SERR_LINK_SEQ_ERR              0x00800000  /* Link sequence error */
 #define SERR_TRANS_ST_ERROR            0x01000000  /* Transport state trans. error */
 #define SERR_UNRECOG_FIS               0x02000000  /* Unrecognized FIS */
 #define SERR_DEV_XCHG                  0x04000000  /* device exchanged */

#define SCR_CONTROL                     2
 #define SCON_DET_MSK                   0x0000000f
 #define SCON_DET_IDLE                  0x00000000
 #define SCON_DET_RESET                 0x00000001
 #define SCON_DET_DISABLE               0x00000004
 #define SCON_SPD_MSK                   0x000000f0
 #define SCON_SPD_ANY                   0x00000000
 #define SCON_SPD_GEN1                  0x00000010
 #define SCON_SPD_GEN2                  0x00000020
 #define SCON_SPD_GEN3                  0x00000030
 #define SCON_IPM_MSK                   0x00000f00
 #define SCON_IPM_NONE                  0x00000000
 #define SCON_IPM_DIS_PARTIAL           0x00000100
 #define SCON_IPM_DIS_SLUMBER           0x00000200
 #define SCON_IPM_DIS_PARTIAL_SLUMBER   0x00000300

#define SCR_ACTIVE                      3
#define SCR_NOTIFICATION                4

#define SATA_SIG_ATA                    0x00000101  /* SATA drive */
#define SATA_SIG_ATAPI                  0xEB140101  /* SATAPI drive */
#define SATA_SIG_SEMB                   0xC33C0101  /* Enclosure management bridge */
#define SATA_SIG_PM                     0x96690101  /* Port multiplier */

/* SSTATUS register */
#define SSTATUS_DET_MASK                0x0000000F
#define SSTATUS_DET_NO_DEV              0x00000000
#define SSTATUS_DET_NO_COMM             0x00000001
#define SSTATUS_DET_DEV_PRES            0x00000003
#define SSTATUS_DET_DEV_OFF             0x00000004

#define SSTATUS_SPD_MASK                0x000000F0
#define SSTATUS_SPD_GEN_1               0x00000010
#define SSTATUS_SPD_GEN_2               0x00000020

#define SSTATUS_IPM_MASK                0x00000F00

/******************************************************************************/
#define COMMANDS_MAX_NUM                16

/*
 * SATA host controller supports a max of 16 direct PRD entries, but if use
 * chained indirect PRD entries, then the controller supports up to a max of 63
 * entries including direct and indirect PRD entries. The PRDT is an array of 63
 * PRD entries contiguously, but the PRD entries#15 will be setup as an indirect
 * descriptor, pointing to it's next (contiguous) PRD entries#16.
 */

#define COMMAND_PRD_MAX_NUM             63
#define SATA_HC_MAX_PRD_DIRECT          16 /* Direct PRDT entries */

#define SATA_REGISTER_H2D_FIS           0x27

/* Flags - word 4 of PRD */
#define PRD_LEN_MASK                    0x003FFFFC
#define PRD_EXT_MASK                    0x80000000

#define SNOOP_ENABLED                   0

#if (SNOOP_ENABLED == 1)
    #define PRD_ENTRY_DATA_SNOOP        0x10000000 /* Data snoop enable */
    #define CMD_HDR_ATTR_SNOOP          0x00000200 /* Snoop enable for all descriptor */
#else
    #define PRD_ENTRY_DATA_SNOOP        0x00000000 /* Data snoop enable */
    #define CMD_HDR_ATTR_SNOOP          0x00000000 /* Snoop enable for all descriptor */
#endif

/* Command Descriptor */
#define SATA_HC_CMD_DESCON_CFIS_SIZE    32 /* bytes */
#define SATA_HC_CMD_DESCON_SFIS_SIZE    32 /* bytes */
#define SATA_HC_CMD_DESCON_ACMD_SIZE    16 /* bytes */
#define SATA_HC_CMD_DESCON_RES          16 /* bytes */

/* Attribute */
#define CMD_HDR_ATTR_RES                0x00000800 /* Reserved bit, should be 1 */
#define CMD_HDR_ATTR_VBIST              0x00000400 /* Vendor BIST */
#define CMD_HDR_ATTR_FPDMA              0x00000100 /* FPDMA queued command */
#define CMD_HDR_ATTR_RESET              0x00000080 /* Reset - a SRST or device reset */
#define CMD_HDR_ATTR_BIST               0x00000040 /* BIST - require the host to enter BIST mode */
#define CMD_HDR_ATTR_ATAPI              0x00000020 /* ATAPI command */
#define CMD_HDR_ATTR_TAG                0x0000001f /* TAG mask */

#define RCAR_SATA_16B_ALIGN             0x000F  /* 16 bytes */
#define RCAR_SATA_4B_ALIGN              0x0003  /*  4 bytes */
#define RCAR_SATA_2B_ALIGN              0x0001  /*  2 bytes */
/******************************************************************************/
/* SATA controller memory map */
typedef struct
{
    volatile uint8_t    rev1[0x100];
    /* OPSH-Navi2G/ATAPI-ATA compatible task registers */
    volatile uint32_t   data;
    volatile uint32_t   serr_sfeatures;
    volatile uint32_t   seccnt;
    volatile uint32_t   lbalow;
    volatile uint32_t   lbamid;
    volatile uint32_t   lbahigh;
    volatile uint32_t   devhead;
    volatile uint32_t   sstatus_scom;
    volatile uint8_t    rev2[0x0138-0x011c-4];
    volatile uint32_t   saltsts_sdevcon;
    volatile uint8_t    rev3[0x0180-0x0138-4];
    /* SH-Navi2G/ATAPI module compatible control registers */
    volatile uint32_t   atapi_control1;
    volatile uint32_t   atapi_status;
    volatile uint32_t   atapi_int_enable;
    volatile uint8_t    rev4[0x0198-0x0188-4];
    volatile uint32_t   atapi_dtb_adr;
    volatile uint32_t   atapi_dma_start_adr;
    volatile uint32_t   atapi_dma_trans_cnt;
    volatile uint32_t   atapi_control2;
    volatile uint8_t    rev5[0x01b0-0x01a4-4];
    volatile uint32_t   atapi_sig_st;
    volatile uint32_t   atapi_byte_swap;
    volatile uint8_t    rev6[0x102c-0x01bc-4];
    /*Serial-ATA HOST control registers */
    volatile uint32_t   bistconf;
    volatile uint8_t    rev7[0x1100-0x102c-4];
    volatile uint32_t   sdata;
    volatile uint32_t   sserr_ssfeatures;
    volatile uint32_t   sseccnt;
    volatile uint32_t   slbalow;
    volatile uint32_t   slbamid;
    volatile uint32_t   slbahigh;
    volatile uint32_t   sdevhead;
    volatile uint32_t   ssstatus_sscom;
    volatile uint8_t    rev8[0x1204-0x111c-4];
    volatile uint32_t   ssaltsts_ssdevcon;
    volatile uint8_t    rev9[0x1220-0x1204-4];
    volatile uint32_t   sataer;
    volatile uint8_t    rev10[0x1400-0x1220];
    volatile uint32_t   scrssts;
    volatile uint32_t   scrserr;
    volatile uint32_t   scrscon;
    volatile uint32_t   scrsact;
    volatile uint8_t    rev11[0x1508-0x140c-4];
    volatile uint32_t   sataintstat;
    volatile uint32_t   sataintmask;
    volatile uint8_t    rev12[0x1568-0x150c-4];
    volatile uint32_t   phystop;
    volatile uint8_t    rev13[0x1620-0x1568-4];
    volatile uint32_t   dmadw0;
    volatile uint32_t   dmadw1;
    volatile uint32_t   dmadw2;
    volatile uint32_t   dmadw3;
    volatile uint32_t   dmadw4;
    volatile uint32_t   dmadw5;
    volatile uint32_t   dmadw6;
} __attribute__((__packed__)) rcar_sata_regs;

/*****************************************************************************/
typedef struct
{
    volatile uint32_t   sdata;                  // data (SATA/ATAPI)        R/W 0x1100
    volatile uint32_t   sserr_ssfeatures;       // error (SATA/ATAPI)       R   0x1104
                                                // features (SATA/ATAPI)    W   0x1104
    volatile uint32_t   sseccnt;                // sector count (SATA)      R/W 0x1108
    volatile uint32_t   slbalow;                // cylinder low (SATA)      R/W 0x110C
    volatile uint32_t   slbamid;                // cylinder mid (SATA)      R/W 0x1110
    volatile uint32_t   slbahigh;               // cylinder high (SATA)     R/W 0x1114
    volatile uint32_t   sdevhead;               // device/head              R/W 0x1118
    volatile uint32_t   ssstatus_sscom;         // status                   R   0x111C
                                                // command                  W   0x111C
    volatile uint8_t    rev[0x1204-0x111C-4];   // reserved
    volatile uint32_t   ssaltsts_ssdevcon;      // alternate status         R   0x1204
                                                // device control           W   0x1204
} rcar_sata_tf_regs;

/* 4 words */
typedef struct
{
     uint32_t   ddsta;      /* Data base address, 4 bytes aligned */
     uint32_t   ddtrc;      /* Indirect PRD flags, snoop and data word count */
}  __attribute__((__packed__)) rcar_sata_prd;


#define SATA_DATA_DUMP

#define SATA_ALIGN_ADDR_AND_LEN

#if defined(SATA_ALIGN_ADDR_AND_LEN)

    #define SG_CHANGED_NONE 0x00
    #define SG_CHANGED_ADDR 0x01
    #define SG_CHANGED_LEN  0x02

    typedef struct
    {
        SG_ELEM             isge;
        void                *iptr;
        SG_ELEM             csge;
        void                *cptr;
        size_t              alloc_size;
        int                 chg;
    } sg_elem_t;

    typedef struct
    {
        sg_elem_t           sga[COMMAND_PRD_MAX_NUM + 1];
        int                 is_read_cmd;
        int                 chg_count;
        int                 all_count;
    } cmd_sg_t;

#endif

#define ACMD_BUFFER_LEN     12

typedef struct
{
    uint32_t                phy_connected;
    uint32_t                is_read_cmd;
    uint8_t                 acmd[ACMD_BUFFER_LEN];
#if defined(SATA_DATA_DUMP)
    pthread_mutex_t         mutex;
#endif  /* SATA_DATA_DUMP */
#if defined(SATA_ALIGN_ADDR_AND_LEN)
    cmd_sg_t                cmd_sg;
#endif  /* SATA_ALIGN_ADDR_AND_LEN */
    uint32_t                prd_align;
    uint32_t                num_prde;
    int                     cmd_pio;
    rcar_sata_regs          *mem_map;
} rcar_sata_drv;

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/devb/rcarsata/sim_sata.h $ $Rev: 853240 $")
#endif
