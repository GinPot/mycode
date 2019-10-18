/*
 * $QNXLicenseC:
 * Copyright 2016, QNX Software Systems.
 * Copyright 2016, Freescale Semiconductor, Inc.
 * Copyright 2017 NXP
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

#ifndef IMX_LPUART_H_
#define IMX_LPUART_H_


/* LPUART registers, offset from base address */
#define IMX_LPUART_VERID                  0x00    /* Version ID Register */
#define IMX_LPUART_PARAM                  0x04    /* Parameter Register */
#define IMX_LPUART_GLOBAL                 0x08    /* LPUART Global Register */
#define IMX_LPUART_PINCFG                 0x0C    /* LPUART Pin Configuration Register */
#define IMX_LPUART_BAUD                   0x10    /* LPUART Baud Rate Register */
#define IMX_LPUART_STAT                   0x14    /* LPUART Status Register */
#define IMX_LPUART_CTRL                   0x18    /* LPUART Control Register */
#define IMX_LPUART_DATA                   0x1C    /* LPUART Data Register */
#define IMX_LPUART_MATCH                  0x20    /* LPUART Match Address Register */
#define IMX_LPUART_MODIR                  0x24    /* LPUART Modem IrDA Register */
#define IMX_LPUART_FIFO                   0x28    /* LPUART FIFO Register */
#define IMX_LPUART_WATER                  0x2C    /* LPUART Watermark Register */

/*
 * Version ID Register bits
 */
#define IMX_LPUART_VERID_MAJOR_MASK       (0xFF << 24)  /* Major Version Number */
#define IMX_LPUART_VERID_MAJOR_SHIFT      (24)
#define IMX_LPUART_VERID_MINOR_MASK       (0xFF << 16)  /* Minor Version Number */
#define IMX_LPUART_VERID_MINOR_SHIFT      (16)
#define IMX_LPUART_VERID_FEATURE_MASK     (0xFFFF << 0) /* Feature Identification Number */
#define IMX_LPUART_VERID_FEATURE_SHIFT    (0)

/*
 * Parameter Register bits
 */
#define IMX_LPUART_PARAM_RXFIFO_MASK      (0xFF << 8)   /* Receive FIFO Size */
#define IMX_LPUART_PARAM_RXFIFO_SHIFT     (8)
#define IMX_LPUART_PARAM_TXFIFO_MASK      (0xFF << 0)   /* Transmit FIFO Size */
#define IMX_LPUART_PARAM_TXFIFO_SHIFT     (0)

/*
 * LPUART Global Register bits
 */
#define IMX_LPUART_GLOBAL_RST_MASK        (1 << 1)     /* Software reset */
#define IMX_LPUART_GLOBAL_RST_SHIFT       (1)
/*
 * LPUART Pin Configuration Register bits
 */
#define IMX_LPUART_PINCFG_TRGSEL_MASK     (0x3 << 0)   /* Trigger select */
#define IMX_LPUART_PINCFG_TRGSEL_SHIFT    (0)

/*
 * LPUART Baud Rate Register bits
 */
#define IMX_LPUART_BAUD_MAEN1_MASK        (1 << 31)     /* Match Address Mode Enable 1 */
#define IMX_LPUART_BAUD_MAEN1_SHIFT       (31)
#define IMX_LPUART_BAUD_MAEN2_MASK        (1 << 30)     /* Match Address Mode Enable 2 */
#define IMX_LPUART_BAUD_MAEN2_SHIFT       (30)
#define IMX_LPUART_BAUD_M10_MASK          (1 << 29)     /* 10-bit Mode select */
#define IMX_LPUART_BAUD_M10_SHIFT         (29)
#define IMX_LPUART_BAUD_OSR_MASK          (0x1F << 24)  /* Oversampling Ratio */
#define IMX_LPUART_BAUD_OSR_SHIFT         (24)
#define IMX_LPUART_BAUD_TDMAE_MASK        (1 << 23)     /* Transmitter DMA Enable */
#define IMX_LPUART_BAUD_TDMAE_SHIFT       (23)
#define IMX_LPUART_BAUD_RDMAE_MASK        (1 << 21)     /* Receiver Full DMA Enable */
#define IMX_LPUART_BAUD_RDMAE_SHIFT       (21)
#define IMX_LPUART_BAUD_RIDMAE_MASK       (1 << 20)     /* Receiver Idle DMA Enable */
#define IMX_LPUART_BAUD_RIDMAE_SHIFT      (20)
#define IMX_LPUART_BAUD_MATCFG_MASK       (0x3 << 18)   /* Match Configuration */
#define IMX_LPUART_BAUD_MATCFG_SHIFT      (18)
#define IMX_LPUART_BAUD_BOTHEDGE_MASK     (1 << 17)     /* Both Edge Sampling */
#define IMX_LPUART_BAUD_BOTHEDGE_SHIFT    (17)
#define IMX_LPUART_BAUD_RESYNCDIS_MASK    (1 << 16)     /* Resynchronization Disable */
#define IMX_LPUART_BAUD_RESYNCDIS_SHIFT   (16)
#define IMX_LPUART_BAUD_LBKDIE_MASK       (1 << 15)     /* LIN Break Detect Interrupt Enable */
#define IMX_LPUART_BAUD_LBKDIE_SHIFT      (15)
#define IMX_LPUART_BAUD_RXEDGIE_MASK      (1 << 14)     /* RX Input Active Edge Interrupt Enable */
#define IMX_LPUART_BAUD_RXEDGIE_SHIFT     (14)
#define IMX_LPUART_BAUD_SBNS_MASK         (1 << 13)     /* Stop Bit Number Select */
#define IMX_LPUART_BAUD_SBNS_SHIFT        (13)
#define IMX_LPUART_BAUD_SBR_MASK          (0x1FFF << 0) /* Baud Rate Modulo Divisor */
#define IMX_LPUART_BAUD_SBR_SHIFT         (0)

/*
 * LPUART Status Register bits
 */
#define IMX_LPUART_STAT_LBKDIF_MASK       (1 << 31)     /* LIN Break Detect Interrupt Flag */
#define IMX_LPUART_STAT_LBKDIF_SHIFT      (31)
#define IMX_LPUART_STAT_RXEDGIF_MASK      (1 << 30)     /* RXD Pin Active Edge Interrupt Flag */
#define IMX_LPUART_STAT_RXEDGIF_SHIFT     (30)
#define IMX_LPUART_STAT_MSBF_MASK         (1 << 29)     /* MSB First */
#define IMX_LPUART_STAT_MSBF_SHIFT        (29)
#define IMX_LPUART_STAT_RXINV_MASK        (1 << 28)     /* Receive Data Inversion */
#define IMX_LPUART_STAT_RXINV_SHIFT       (28)
#define IMX_LPUART_STAT_RWUID_MASK        (1 << 27)     /* Receive Wake Up Idle Detect */
#define IMX_LPUART_STAT_RWUID_SHIFT       (27)
#define IMX_LPUART_STAT_BRK13_MASK        (1 << 26)     /* Break Character Generation Length */
#define IMX_LPUART_STAT_BRK13_SHIFT       (26)
#define IMX_LPUART_STAT_LBKDE_MASK        (1 << 25)     /* LIN Break Detection Enable */
#define IMX_LPUART_STAT_LBKDE_SHIFT       (25)
#define IMX_LPUART_STAT_RAF_MASK          (1 << 24)     /* Receiver Active Flag */
#define IMX_LPUART_STAT_RAF_SHIFT         (24)
#define IMX_LPUART_STAT_TDRE_MASK         (1 << 23)     /* Transmit Data Register Empty Flag */
#define IMX_LPUART_STAT_TDRE_SHIFT        (23)
#define IMX_LPUART_STAT_TC_MASK           (1 << 22)     /* Transmission Complete Flag */
#define IMX_LPUART_STAT_TC_SHIFT          (22)
#define IMX_LPUART_STAT_RDRF_MASK         (1 << 21)     /* Receive Data Register Full Flag */
#define IMX_LPUART_STAT_RDRF_SHIFT        (21)
#define IMX_LPUART_STAT_IDLE_MASK         (1 << 20)     /* Idle Line Flag */
#define IMX_LPUART_STAT_IDLE_SHIFT        (20)
#define IMX_LPUART_STAT_OR_MASK           (1 << 19)     /* Receiver Overrun Flag */
#define IMX_LPUART_STAT_OR_SHIFT          (19)
#define IMX_LPUART_STAT_NF_MASK           (1 << 18)     /* Noise Flag */
#define IMX_LPUART_STAT_NF_SHIFT          (18)
#define IMX_LPUART_STAT_FE_MASK           (1 << 17)     /* Framing Error Flag */
#define IMX_LPUART_STAT_FE_SHIFT          (17)
#define IMX_LPUART_STAT_PF_MASK           (1 << 16)     /* Parity Error Flag */
#define IMX_LPUART_STAT_PF_SHIFT          (16)
#define IMX_LPUART_STAT_MA1F_MASK         (1 << 15)     /* Match 1 Flag */
#define IMX_LPUART_STAT_MA1F_SHIFT        (15)
#define IMX_LPUART_STAT_MA2F_MASK         (1 << 14)     /* Match 2 Flag */
#define IMX_LPUART_STAT_MA2F_SHIFT        (14)

/*
 * LPUART Status Register bits
 */
#define IMX_LPUART_CTRL_R8T9_MASK         (1 << 31)     /* Receive Bit 8 / Transmit Bit 9 */
#define IMX_LPUART_CTRL_R8T9_SHIFT        (31)
#define IMX_LPUART_CTRL_R9T8_MASK         (1 << 30)     /* Receive Bit 9 / Transmit Bit 8 */
#define IMX_LPUART_CTRL_R9T8_SHIFT        (30)
#define IMX_LPUART_CTRL_TXDIR_MASK        (1 << 29)     /* TXD Pin Direction in Single-Wire Mode */
#define IMX_LPUART_CTRL_TXDIR_SHIFT       (29)
#define IMX_LPUART_CTRL_TXINV_MASK        (1 << 28)     /* Transmit Data Inversion */
#define IMX_LPUART_CTRL_TXINV_SHIFT       (28)
#define IMX_LPUART_CTRL_ORIE_MASK         (1 << 27)     /* Overrun Interrupt Enable */
#define IMX_LPUART_CTRL_ORIE_SHIFT        (27)
#define IMX_LPUART_CTRL_NEIE_MASK         (1 << 26)     /* Noise Error Interrupt Enable */
#define IMX_LPUART_CTRL_NEIE_SHIFT        (26)
#define IMX_LPUART_CTRL_FEIE_MASK         (1 << 25)     /* Framing Error Interrupt Enable */
#define IMX_LPUART_CTRL_FEIE_SHIFT        (25)
#define IMX_LPUART_CTRL_PEIE_MASK         (1 << 24)     /* Parity Error Interrupt Enable */
#define IMX_LPUART_CTRL_PEIE_SHIFT        (24)
#define IMX_LPUART_CTRL_TIE_MASK          (1 << 23)     /* Transmit Interrupt Enable */
#define IMX_LPUART_CTRL_TIE_SHIFT         (23)
#define IMX_LPUART_CTRL_TCIE_MASK         (1 << 22)     /* Transmission Complete Interrupt Enable */
#define IMX_LPUART_CTRL_TCIE_SHIFT        (22)
#define IMX_LPUART_CTRL_RIE_MASK          (1 << 21)     /* Receiver Interrupt Enable */
#define IMX_LPUART_CTRL_RIE_SHIFT         (21)
#define IMX_LPUART_CTRL_ILIE_MASK         (1 << 20)     /* Idle Line Interrupt Enable */
#define IMX_LPUART_CTRL_ILIE_SHIFT        (20)
#define IMX_LPUART_CTRL_TE_MASK           (1 << 19)     /* Transmitter Enable */
#define IMX_LPUART_CTRL_TE_SHIFT          (19)
#define IMX_LPUART_CTRL_RE_MASK           (1 << 18)     /* Receiver Enable */
#define IMX_LPUART_CTRL_RE_SHIFT          (18)
#define IMX_LPUART_CTRL_RWU_MASK          (1 << 17)     /* Receiver Wakeup Control */
#define IMX_LPUART_CTRL_RWU_SHIFT         (17)
#define IMX_LPUART_CTRL_SBK_MASK          (1 << 16)     /* Send Break */
#define IMX_LPUART_CTRL_SBK_SHIFT         (16)
#define IMX_LPUART_CTRL_MA1IE_MASK        (1 << 15)     /* Match 1 Interrupt Enable */
#define IMX_LPUART_CTRL_MA1IE_SHIFT       (15)
#define IMX_LPUART_CTRL_MA2IE_MASK        (1 << 14)     /* Match 2 Interrupt Enable */
#define IMX_LPUART_CTRL_MA2IE_SHIFT       (14)
#define IMX_LPUART_CTRL_M7_MASK           (1 << 11)     /* 7-Bit Mode Select */
#define IMX_LPUART_CTRL_M7_SHIFT          (11)
#define IMX_LPUART_CTRL_IDLECFG_MASK      (0x7 << 8)    /* Idle Configuration */
#define IMX_LPUART_CTRL_IDLECFG_SHIFT     (8)
#define IMX_LPUART_CTRL_LOOPS_MASK        (1 << 7)      /* Loop Mode Select */
#define IMX_LPUART_CTRL_LOOPS_SHIFT       (7)
#define IMX_LPUART_CTRL_DOZEEN_MASK       (1 << 6)      /* Doze Enable */
#define IMX_LPUART_CTRL_DOZEEN_SHIFT      (6)
#define IMX_LPUART_CTRL_RSRC_MASK         (1 << 5)      /* Receiver Source Select */
#define IMX_LPUART_CTRL_RSRC_SHIFT        (5)
#define IMX_LPUART_CTRL_M_MASK            (1 << 4)      /* 9-Bit or 8-Bit Mode Select */
#define IMX_LPUART_CTRL_M_SHIFT           (4)
#define IMX_LPUART_CTRL_WAKE_MASK         (1 << 3)      /* Receiver Wakeup Method Select */
#define IMX_LPUART_CTRL_WAKE_SHIFT        (3)
#define IMX_LPUART_CTRL_ILT_MASK          (1 << 2)      /* Idle Line Type Select */
#define IMX_LPUART_CTRL_ILT_SHIFT         (2)
#define IMX_LPUART_CTRL_PE_MASK           (1 << 1)      /* Parity Enable */
#define IMX_LPUART_CTRL_PE_SHIFT          (1)
#define IMX_LPUART_CTRL_PT_MASK           (1 << 0)      /* Parity Type */
#define IMX_LPUART_CTRL_PT_SHIFT          (0)

/*
 * LPUART Data Register bits
 */
#define IMX_LPUART_DATA_NOISY_MASK        (1 << 15)     /* NOISY */
#define IMX_LPUART_DATA_NOISY_SHIFT       (15)
#define IMX_LPUART_DATA_PARITYE_MASK      (1 << 14)     /* PARITYE */
#define IMX_LPUART_DATA_PARITYE_SHIFT     (14)
#define IMX_LPUART_DATA_FRETSC_MASK       (1 << 13)     /* Frame Error / Transmit Special Character */
#define IMX_LPUART_DATA_FRETSC_SHIFT      (13)
#define IMX_LPUART_DATA_RXEMPT_MASK       (1 << 12)     /* Receive Buffer Empty */
#define IMX_LPUART_DATA_RXEMPT_SHIFT      (12)
#define IMX_LPUART_DATA_IDLINE_MASK       (1 << 11)     /* Idle Line */
#define IMX_LPUART_DATA_IDLINE_SHIFT      (11)
#define IMX_LPUART_DATA_RT_MASK           (0x3FF << 0)  /* Rx/Tx buffer */
#define IMX_LPUART_DATA_RT_SHIFT          (0)

/*
 * LPUART Match Address Register bits
 */
#define IMX_LPUART_MATCH_MA2_MASK         (0x3FF << 16) /* Match Address 2 */
#define IMX_LPUART_MATCH_MA2_SHIFT        (16)
#define IMX_LPUART_MATCH_MA1_MASK         (0x3FF << 0)  /* Match Address 1 */
#define IMX_LPUART_MATCH_MA1_SHIFT        (0)

/*
 * LPUART Modem IrDA Register bits
 */
#define IMX_LPUART_MODIR_IREN_MASK        (1 << 18)     /* Enables/disables the infrared modulation/demodulation. */
#define IMX_LPUART_MODIR_IREN_SHIFT       (18)
#define IMX_LPUART_MODIR_TNP_MASK         (0x3 << 16)   /* Transmitter narrow pulse */
#define IMX_LPUART_MODIR_TNP_SHIFT        (16)
#define IMX_LPUART_MODIR_RTSWATER_MASK    (0xFF << 8)   /* Receive RTS Configuration */
#define IMX_LPUART_MODIR_RTSWATER_SHIFT   (8)
#define IMX_LPUART_MODIR_TXCTSSRC_MASK    (1 << 5)      /* Transmit CTS Source */
#define IMX_LPUART_MODIR_TXCTSSRC_SHIFT   (5)
#define IMX_LPUART_MODIR_TXCTSC_MASK      (1 << 4)      /* Transmit CTS Configuration */
#define IMX_LPUART_MODIR_TXCTSC_SHIFT     (4)
#define IMX_LPUART_MODIR_RXRTSE_MASK      (1 << 3)      /* Receiver request-to-send enable */
#define IMX_LPUART_MODIR_RXRTSE_SHIFT     (3)
#define IMX_LPUART_MODIR_TXRTSPOL_MASK    (1 << 2)      /* Transmitter request-to-send polarity */
#define IMX_LPUART_MODIR_TXRTSPOL_SHIFT   (2)
#define IMX_LPUART_MODIR_TXRTSE_MASK      (1 << 1)      /* Transmitter request-to-send enable */
#define IMX_LPUART_MODIR_TXRTSE_SHIFT     (1)
#define IMX_LPUART_MODIR_TXCTSE_MASK      (1 << 0)      /* Transmitter clear-to-send enable */
#define IMX_LPUART_MODIR_TXCTSE_SHIFT     (0)

/*
 * LPUART FIFO Register bits
 */
#define IMX_LPUART_FIFO_TXEMPT_MASK       (1 << 23)    /* Transmit Buffer/FIFO Empty */
#define IMX_LPUART_FIFO_TXEMPT_SHIFT      (23)
#define IMX_LPUART_FIFO_RXEMPT_MASK       (1 << 22)    /* Receive Buffer/FIFO Empty */
#define IMX_LPUART_FIFO_RXEMPT_SHIFT      (22)
#define IMX_LPUART_FIFO_TXOF_MASK         (1 << 17)    /* Transmitter Buffer Overflow Flag */
#define IMX_LPUART_FIFO_TXOF_SHIFT        (17)
#define IMX_LPUART_FIFO_RXUF_MASK         (1 << 16)    /* Receiver Buffer Underflow Flag */
#define IMX_LPUART_FIFO_RXUF_SHIFT        (16)
#define IMX_LPUART_FIFO_TXFLUSH_MASK      (1 << 15)    /* Transmit FIFO/Buffer Flush */
#define IMX_LPUART_FIFO_TXFLUSH_SHIFT     (15)
#define IMX_LPUART_FIFO_RXFLUSH_MASK      (1 << 14)    /* Receive FIFO/Buffer Flush */
#define IMX_LPUART_FIFO_RXFLUSH_SHIFT     (14)
#define IMX_LPUART_FIFO_RXIDEN_MASK       (0x7 << 10)  /* Receiver Idle Empty Enable */
#define IMX_LPUART_FIFO_RXIDEN_SHIFT      (10)
#define IMX_LPUART_FIFO_TXOFE_MASK        (1 << 9)     /* When this field is set, the TXOF flag generates an interrupt to the host. */
#define IMX_LPUART_FIFO_TXOFE_SHIFT       (9)
#define IMX_LPUART_FIFO_RXUFE_MASK        (1 << 8)     /* Receive FIFO Underflow Interrupt Enable */
#define IMX_LPUART_FIFO_RXUFE_SHIFT       (8)
#define IMX_LPUART_FIFO_TXFE_MASK         (1 << 7)     /* Transmit FIFO Enable */
#define IMX_LPUART_FIFO_TXFE_SHIFT        (7)
#define IMX_LPUART_FIFO_TXFIFOSIZE_MASK   (0x7 << 4)   /* Transmit FIFO. Buffer Depth */
#define IMX_LPUART_FIFO_TXFIFOSIZE_SHIFT  (4)
#define IMX_LPUART_FIFO_RXFE_MASK         (1 << 3)     /* Receive FIFO Enable */
#define IMX_LPUART_FIFO_RXFE_SHIFT        (3)
#define IMX_LPUART_FIFO_RXFIFOSIZE_MASK   (0x7 << 0)   /* Receive FIFO. Buffer Depth */
#define IMX_LPUART_FIFO_RXFIFOSIZE_SHIFT  (0)

/*
 * LPUART Watermark Register bits
 */
#define IMX_LPUART_WATER_RXCOUNT_MASK     (0xFF << 24) /* Receive Counter */
#define IMX_LPUART_WATER_RXCOUNT_SHIFT    (24)
#define IMX_LPUART_WATER_RXWATER_MASK     (0xFF << 16) /* Receive Watermark */
#define IMX_LPUART_WATER_RXWATER_SHIFT    (16)
#define IMX_LPUART_WATER_TXCOUNT_MASK     (0xFF << 8)  /* Transmit Counter */
#define IMX_LPUART_WATER_TXCOUNT_SHIFT    (8)
#define IMX_LPUART_WATER_TXWATER_MASK     (0xFF << 0)  /* Transmit Watermark */
#define IMX_LPUART_WATER_TXWATER_SHIFT    (0)

#endif /* IMX_LPUART_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/startup/lib/public/aarch64/imx8/imx8_lpuart.h $ $Rev: 837679 $")
#endif
