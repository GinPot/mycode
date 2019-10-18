/*
 * $QNXLicenseC:
 * Copyright 2016, 2017 QNX Software Systems.
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

#include "rcar_rpc.h"

static rcar_rpc_t  rcar_rpc = {0,};
char *bsargs = NULL;

#define HYPER_CMENR_BIT_SIZES (RCAR_RPC_SMDRENR_CDB(2) | RCAR_RPC_SMDRENR_OCDB(2) | \
                               RCAR_RPC_SMDRENR_ADB(2) | RCAR_RPC_SMDRENR_OPDB(2) | \
                               RCAR_RPC_SMDRENR_SPIDB(2))

static void rpc_qspi_config(rpc_dev_t *dev)
{
    rcar_rpc_t *rpc = dev->rpc;

    out32(rpc->vbase + RCAR_RPC_PHYCNT, RCAR_RPC_PHYCNT_CAL );

    /*
        CAL [31]            =  1 : PHY calibration
        PHYMEM[1:0]         =  00: Serial flash in SDR mode
        OCTA [23:22]        =  2    : Support sequential alignment
     */
    out32(rpc->vbase + RCAR_RPC_CMNCR, RCAR_RPC_CMNCR_MD
            | RCAR_RPC_CMNCR_IO0FV(3) | RCAR_RPC_CMNCR_IO2FV(3) | RCAR_RPC_CMNCR_IO3FV(3)
            | RCAR_RPC_CMNCR_MOIIO0(3) | RCAR_RPC_CMNCR_MOIIO1(3) | RCAR_RPC_CMNCR_MOIIO2(3) | RCAR_RPC_CMNCR_MOIIO3(3));
    /*
        bit31  MD       =  1 : Manual mode
        bit1-0 BSZ[1:0] = 00 : QSPI Flash x 1
    */
    out32(rpc->vbase + RCAR_RPC_SMDRENR, 0x00000000);
}

static void rpc_hyper_config(rpc_dev_t *dev)
{
    rcar_rpc_t *rpc = dev->rpc;

    /*
        CAL [31]            =  1 : PHY calibration
        PHYMEM[1:0]         =  11: HyperFlash
        OCTA [23:22]        =  0    : HyperFlash or serial flash memory x 1ch or 2ch is connected
     */
    out32(rpc->vbase + RCAR_RPC_PHYCNT, 0x80000263);

    /*
        bit31  MD       =  1 : Manual mode
        bit1-0 BSZ[1:0] = 01 : QSPI Flash x 2 or HyperFlash
    */
    out32(rpc->vbase + RCAR_RPC_CMNCR, RCAR_RPC_CMNCR_MD | RCAR_RPC_CMNCR_BSZ(1)
            | RCAR_RPC_CMNCR_IO0FV(3) | RCAR_RPC_CMNCR_IO2FV(3) | RCAR_RPC_CMNCR_IO3FV(3)
            | RCAR_RPC_CMNCR_MOIIO0(3) | RCAR_RPC_CMNCR_MOIIO1(3) | RCAR_RPC_CMNCR_MOIIO2(3) | RCAR_RPC_CMNCR_MOIIO3(3)
            | (3 << 10) | (1 << 24) // Undocumented fields
         );

    out32(rpc->vbase + RCAR_RPC_SMOPR, 0x00000000);
    out32(rpc->vbase + RCAR_RPC_SMDRENR, 0x00005111);
    out32(rpc->vbase + RCAR_RPC_SSLDR, 0x00010101);
}

static void rpc_init_ext_mode(rpc_dev_t *dev)
{
    rcar_rpc_t *rpc = dev->rpc;

    out32(rpc->vbase + RCAR_RPC_PHYCNT, 0x80040263);
    out32(rpc->vbase + RCAR_RPC_CMNCR, 0x01FFF301);
    out32(rpc->vbase + RCAR_RPC_DRCR, 0x00000000);
    out32(rpc->vbase + RCAR_RPC_DRCMR, 0x00A00000);
    out32(rpc->vbase + RCAR_RPC_DRENR, 0xA222D400);
    out32(rpc->vbase + RCAR_RPC_DRDMCR, 0x0000000E);
    out32(rpc->vbase + RCAR_RPC_DRDRENR, 0x00005101);
    out32(rpc->vbase + RCAR_RPC_OFFSET1, 0x21511144);
    out32(rpc->vbase + RCAR_RPC_PHYINT, 0x07070002);
}

static void rpc_xfer_init(rpc_dev_t *dev, int dummy)
{
    rcar_rpc_t *rpc = dev->rpc;

    // Disable fucntion
    out32(rpc->vbase + RCAR_RPC_SMCR, 0x00000000);

    // data dummy control
    if (dummy != 0)
    {
        out32(rpc->vbase + RCAR_RPC_SMDMCR, dummy - 1);
    }
}

/* SPI flash interface calls */
static void rcar_write_protection_disable(rpc_dev_t *dev)
{
    rcar_rpc_t *rpc = dev->rpc;
    uint32_t dataL = 0;

    dataL = in32(rpc->vbase + RCAR_RPC_PHYINT);

    if(dataL & RCAR_RPC_PHYINT_WPVAL)
    {   //bit1:  WPVAL(0:RPC_WP#=H(Protect Disable), 1:RPC_WP#=L(Protect Enable))
        dataL &= ~RCAR_RPC_PHYINT_WPVAL;
        out32(rpc->vbase + RCAR_RPC_PHYINT, dataL);
    }
}

static void rpc_wait_tx_end(rpc_dev_t *dev)
{
    rcar_rpc_t *rpc = dev->rpc;
    uint32_t status = 0;

    while(1)
    {
        status = in32(rpc->vbase + RCAR_RPC_CMNSR);
        if(status & RCAR_RPC_CMNSR_TEND)
            break;
    }
}

static void spi_read_data_32bit(rpc_dev_t *dev, uint32_t *getData)
{
    rcar_rpc_t  *rpc = dev->rpc;
    uint32_t rdData;
    rpc_wait_tx_end(dev);     // check status of transfer
    rdData = in32(rpc->vbase + RCAR_RPC_SMRDR0);
    *getData = rdData;
}

static void spi_write_data_32bit(rpc_dev_t *dev, uint32_t getData)
{
    rcar_rpc_t  *rpc = dev->rpc;

    rpc_wait_tx_end(dev);     // check status of transfer
    out32(rpc->vbase + RCAR_RPC_SMWDR0 , getData);
}

static int rpc_qspi_single_read(void *hdl, uint8_t cmd, uint32_t addr ,int alen, void *data, int dlen)
{
    rpc_dev_t  *dev = hdl;
    rcar_rpc_t  *rpc = dev->rpc;
    int         rc = dlen;
    uint32_t    read_data;
    uint8_t    *rdata = (uint8_t *)data;

    rpc_qspi_config(dev);
    rpc_xfer_init(dev,0);

    if (alen == 0 ) // Read resgister from flash
    {
        /* Set command */
        out32(rpc->vbase + RCAR_RPC_SMCMR, (cmd) << 16);

        /*
            CDB[31:30] = 0 bit.
            SPIDB[17:16] = 0 bit
            DME[15] = 0 = disable
            CDE[14] = 1 = enable
            SPIDE[3:0] = 0xF = 32bit transfer
        */
        out32(rpc->vbase + RCAR_RPC_SMENR, RCAR_RPC_SMDRENR_CDE | RCAR_RPC_SMDRENR_SPIDE(0xF));

        if (dlen <= 4 )     // Read status register 1
        {
            out32(rpc->vbase + RCAR_RPC_SMCR, RCAR_RPC_CMNCR_SPIRE  | RCAR_RPC_CMNCR_SPIE );
        }else   // Read Identification
        {
            /*
                SSLKP[8] = 1 : QSPIn_SSL signal is keep at the end of transfer
                SPIRE[2] = 1 : Reading
                    Start transfer
                SPIE [0] = 1 : start
            */
            out32(rpc->vbase + RCAR_RPC_SMCR, RCAR_RPC_CMNCR_SSLKP | RCAR_RPC_CMNCR_SPIRE | RCAR_RPC_CMNCR_SPIE );

            /* Set Data Enable 32bit */
            out32(rpc->vbase + RCAR_RPC_SMENR, RCAR_RPC_SMDRENR_SPIDE(0xF) );
        }

        while(dlen > 0){
            spi_read_data_32bit(dev, &read_data);

            if(dlen < 4)    // Read status register 1
            {
                for (int i=3; dlen > 0; dlen-- , i--)
                {
                     *rdata = (uint8_t) (read_data >> 8*(i));
                     rdata++;
                }
                break;
            }else
            {
                *rdata = (uint8_t) (read_data >> 24);
                rdata++;
                *rdata = (uint8_t) (read_data >> 16);
                rdata++;
                *rdata = (uint8_t) (read_data >> 8);
                rdata++;
                *rdata = (uint8_t) (read_data);
                rdata++;

                dlen -= 4;
            }

            if(dlen > 4)
            {
                out32(rpc->vbase + RCAR_RPC_SMCR, RCAR_RPC_CMNCR_SSLKP | RCAR_RPC_CMNCR_SPIRE | RCAR_RPC_CMNCR_SPIE );
            }
            else if(dlen > 0)
            {
                out32(rpc->vbase + RCAR_RPC_SMCR,                        RCAR_RPC_CMNCR_SPIRE | RCAR_RPC_CMNCR_SPIE );
            }
        }

    }else       // Read memory data, QSPI_CMD_DYBRD or QSPI_CMD_PPBRD.
    {
        /* Set command */
        out32(rpc->vbase + RCAR_RPC_SMCMR, (cmd) << 16);

        /* Set Address */
        out32(rpc->vbase + RCAR_RPC_SMADR, addr );

        /*
        CDB[31:30] = 0 bit.
        SPIDB[17:16] = 0 bit
        DME[15] = 0 = disable
        CDE[14] = 1 = enable
        ADE[11:8] = 1111 (4 byte) or 0111 ( 3 byte )
        SPIDE[3:0] = 0xF = 32bit transfer
        */
        if (alen == 3)
        {
            out32(rpc->vbase + RCAR_RPC_SMENR, RCAR_RPC_SMDRENR_CDE | RCAR_RPC_SMDRENR_ADE(0x7) | RCAR_RPC_SMDRENR_SPIDE(0xF) );
        }
        else if (alen == 4 )
        {
            out32(rpc->vbase + RCAR_RPC_SMENR, RCAR_RPC_SMDRENR_CDE | RCAR_RPC_SMDRENR_ADE(0xF) | RCAR_RPC_SMDRENR_SPIDE(0xF));
        }

        if( (cmd == QSPI_CMD_4FREAD) || (cmd == QSPI_CMD_FREAD)  )     // Read memory data by Fast Read
        {
            /* Set Dummy */
            out32(rpc->vbase + RCAR_RPC_SMDMCR, RCAR_RPC_DRDMCR_DMCYC(0x7) );
            out32(rpc->vbase + RCAR_RPC_SMENR, in32(rpc->vbase + RCAR_RPC_SMENR) | RCAR_RPC_SMDRENR_DME);
        }

        while(dlen > 0){
            /*
            SSLKP[8] = 1 : QSPIn_SSL signal is keep at the end of transfer
            SPIRE[2] = 1 : Reading
                Start transfer
            SPIE [0] = 1 : start transfer
           */
            out32(rpc->vbase + RCAR_RPC_SMCR,  RCAR_RPC_CMNCR_SPIRE | RCAR_RPC_CMNCR_SPIE );

            spi_read_data_32bit(dev, &read_data);
            if(dlen < 4)
            {
                for (int i=3; dlen > 0; dlen-- , i--)
                {
                     *rdata = (uint8_t) (read_data >> 8*(i));
                     rdata++;
                }
                break;
            }else
            {
                *rdata = (uint8_t) (read_data >> 24);
                rdata++;
                *rdata = (uint8_t) (read_data >> 16);
                rdata++;
                *rdata = (uint8_t) (read_data >> 8);
                rdata++;
                *rdata = (uint8_t) (read_data);
                rdata++;

                dlen -= 4;
            }
            /*restart transfer*/
            addr+=4;
            out32(rpc->vbase + RCAR_RPC_SMADR, addr );
        }
    }
    return rc;
}

static int rpc_qspi_single_write(void *hdl, uint8_t cmd, int addr , int alen, void *data, int dlen)
{
    rpc_dev_t  *dev = hdl;
    rcar_rpc_t  *rpc = dev->rpc;
    uint8_t    *wdata = (uint8_t *)data;
    uint32_t    writeData;

    rpc_qspi_config(dev);
    rpc_xfer_init(dev,0);

    if( alen == 0 )  // write register
    {
        /* Set command */
        out32(rpc->vbase + RCAR_RPC_SMCMR, cmd << 16);
        /*
            CDB[31:30] = 0 bit.
            SPIDB[17:16] = 0 bit
            DME[15] = 0 = disable
            CDE[14] = 1 = enable
        */
        out32(rpc->vbase + RCAR_RPC_SMENR, RCAR_RPC_SMDRENR_CDE );

        if (dlen == 0 ) // norspi_write() : WREN
        {
            /*Start transfer*/
            out32(rpc->vbase + RCAR_RPC_SMCR, RCAR_RPC_CMNCR_SPIE );
        } else      // write status & config register
        {
            /*write cmd & addr*/
            out32(rpc->vbase + RCAR_RPC_SMCR, RCAR_RPC_CMNCR_SSLKP  | RCAR_RPC_CMNCR_SPIE );

            out32(rpc->vbase + RCAR_RPC_SMENR, RCAR_RPC_SMDRENR_SPIDE(RCAR_RPC_SINGLE_8BIT));

            while(dlen > 0)
            {
                writeData = 0;
                writeData |= (( (*wdata) << 24) & 0xFF000000);
                spi_write_data_32bit(dev,writeData);
                dlen--;

                if ( (dlen) > 0)
                {
                    wdata++;
                    out32(rpc->vbase + RCAR_RPC_SMCR, RCAR_RPC_CMNCR_SSLKP | RCAR_RPC_CMNCR_SPIWE | RCAR_RPC_CMNCR_SPIE );
                }
                else
                {
                    out32(rpc->vbase + RCAR_RPC_SMCR,                        RCAR_RPC_CMNCR_SPIWE | RCAR_RPC_CMNCR_SPIE );
                }

                rpc_wait_tx_end(dev);
            }
        }
    } else      // In case : norspi_write_memory() ; erase .

    {

        /* Set command */
        out32(rpc->vbase + RCAR_RPC_SMCMR, cmd << 16);
        /* Set Address */
        out32(rpc->vbase + RCAR_RPC_SMADR, addr );
        /*
            CDB[31:30] = 0 bit.
            SPIDB[17:16] = 0 bit
            DME[15] = 0 = disable
            CDE[14] = 1 = enable
            ADE[11:8 = 1111 (4 byte) or 0111 ( 3 byte )
            SPIDE[3:0] = 0xF = 32bit transfer
        */
        if (alen == 3)
        {
            out32(rpc->vbase + RCAR_RPC_SMENR, RCAR_RPC_SMDRENR_CDE | RCAR_RPC_SMDRENR_ADE(0x7));
        }
        else if (alen == 4 )
        {
            out32(rpc->vbase + RCAR_RPC_SMENR, RCAR_RPC_SMDRENR_CDE | RCAR_RPC_SMDRENR_ADE(0xF));
        }

        if (dlen == 0)      // norspi_write() : erase sector
        {
            out32(rpc->vbase + RCAR_RPC_SMCR,RCAR_RPC_CMNCR_SPIE );
            rpc_wait_tx_end(dev);
        }else               // norspi_write_memory()
        {
            /*write cmd & addr*/
            out32(rpc->vbase + RCAR_RPC_SMCR, RCAR_RPC_CMNCR_SSLKP  | RCAR_RPC_CMNCR_SPIE );

            rpc_wait_tx_end(dev);

            out32(rpc->vbase + RCAR_RPC_SMENR, RCAR_RPC_SMDRENR_SPIDE(RCAR_RPC_SINGLE_32BIT));

            while(dlen > 0)
            {
                if(dlen < 4)
                {
                    writeData = 0;
                    out32(rpc->vbase + RCAR_RPC_SMENR, RCAR_RPC_SMDRENR_SPIDE(RCAR_RPC_SINGLE_8BIT));
                    writeData |= (( *(wdata++) << 24) & 0xFF000000);
                    spi_write_data_32bit(dev,writeData);
                    dlen--;

                }else
                {
                    writeData  = (*(wdata++) << 24);
                    writeData |= (*(wdata++) << 16);
                    writeData |= (*(wdata++) << 8);
                    writeData |= (*(wdata++));

                    spi_write_data_32bit(dev,writeData);
                    dlen-=4;
                }

                if ( (dlen) > 0)
                {
                    out32(rpc->vbase + RCAR_RPC_SMCR, RCAR_RPC_CMNCR_SSLKP | RCAR_RPC_CMNCR_SPIWE | RCAR_RPC_CMNCR_SPIE );
                }
                else
                {
                    out32(rpc->vbase + RCAR_RPC_SMCR,                        RCAR_RPC_CMNCR_SPIWE | RCAR_RPC_CMNCR_SPIE );
                }

                rpc_wait_tx_end(dev);
            }
        }
    }
    return 1;
}

static int rpc_qspi_quad_read(void *hdl, uint8_t cmd, uint8_t addr , uint8_t alen, void *data, int dlen)
{
    rpc_dev_t  *dev = hdl;
    rcar_rpc_t  *rpc = dev->rpc;
    uint32_t val_tmp = 0;
    uint8_t rc=dlen;
    uint32_t    dummyRd_w;
    uint8_t    *wdata = (uint8_t *)data;

    rpc_qspi_config(dev);

    /* Set command */
    out32(rpc->vbase + RCAR_RPC_SMCMR, cmd << 16);
    /* Set Address */
    out32(rpc->vbase + RCAR_RPC_SMADR, addr );
    /* Set option data */
    out32(rpc->vbase + RCAR_RPC_SMOPR, 0x00 << 24 ); // 0xa0 : Quad I/O High Performance Read Mode
    /* Set Dummy */
    out32(rpc->vbase + RCAR_RPC_SMDMCR, RCAR_RPC_DRDMCR_DMCYC(0x7) );
    /* Set the size in bit unit */
    out32(rpc->vbase + RCAR_RPC_SMENR, RCAR_RPC_SMDRENR_CDB(2) | RCAR_RPC_SMDRENR_ADB(2) | RCAR_RPC_SMDRENR_OPDB(2) | RCAR_RPC_SMDRENR_SPIDB(2));

    /*
    CDB[31:30] = 0 bit.
    SPIDB[17:16] = 0 bit
    DME[15] = 0 = disable
    CDE[14] = 1 = enable
    ADE[11:8] = 1111 (4 byte) or 0111 ( 3 byte )
    SPIDE[3:0] = 0xF = 32bit transfer
    */
    val_tmp = in32(rpc->vbase + RCAR_RPC_SMENR);
    val_tmp &= 0xffff0000;
    if (alen == 3)
    {
        out32(rpc->vbase + RCAR_RPC_SMENR, val_tmp | RCAR_RPC_SMDRENR_CDE | RCAR_RPC_SMDRENR_ADE(0x7) | RCAR_RPC_SMDRENR_OPDE(0x8) | RCAR_RPC_SMDRENR_DME | RCAR_RPC_SMDRENR_SPIDE(0xF) );
    }
    else if (alen == 4 )
    {
        out32(rpc->vbase + RCAR_RPC_SMENR, val_tmp | RCAR_RPC_SMDRENR_CDE | RCAR_RPC_SMDRENR_ADE(0xF) | RCAR_RPC_SMDRENR_OPDE(0x8) | RCAR_RPC_SMDRENR_DME | RCAR_RPC_SMDRENR_SPIDE(0xF));
    }


    while(dlen > 0){
            /*
            SSLKP[8] = 1 : QSPIn_SSL signal is keep at the end of transfer
            SPIRE[2] = 1 : Reading
                Start transfer
            SPIE [0] = 1 : start transfer
           */
            out32(rpc->vbase + RCAR_RPC_SMCR,  RCAR_RPC_CMNCR_SPIRE | RCAR_RPC_CMNCR_SPIE );

            spi_read_data_32bit(dev, &dummyRd_w);
            if(dlen < 4)
            {
                for (int i=3; dlen > 0; dlen-- , i--)
                {
                     *wdata = (uint8_t) (dummyRd_w >> 8*(i));
                     wdata++;
                }
                break;
            }else
            {
                *wdata = (uint8_t) (dummyRd_w >> 24);
                wdata++;
                *wdata = (uint8_t) (dummyRd_w >> 16);
                wdata++;
                *wdata = (uint8_t) (dummyRd_w >> 8);
                wdata++;
                *wdata = (uint8_t) (dummyRd_w);
                wdata++;

                dlen -= 4;
            }
            /*restart transfer*/
            addr+=4;
            out32(rpc->vbase + RCAR_RPC_SMADR, addr );
        }

    return rc;
}

static int rpc_qspi_quad_write(void *hdl, uint8_t cmd, uint8_t addr , uint8_t alen, void *data, int dlen)
{
    rpc_dev_t  *dev = hdl;
    rcar_rpc_t  *rpc = dev->rpc;
    uint8_t    *wdata = (uint8_t *)data;
    uint32_t    writeData;
    uint32_t val_tmp;

    rpc_qspi_config(dev);
    /* Set command */
    out32(rpc->vbase + RCAR_RPC_SMCMR, cmd << 16);
    /* Set Address */
    out32(rpc->vbase + RCAR_RPC_SMADR, addr );
    /* Set the size in bit unit */
    out32(rpc->vbase + RCAR_RPC_SMENR, RCAR_RPC_SMDRENR_CDB(2) | RCAR_RPC_SMDRENR_ADB(2) | RCAR_RPC_SMDRENR_SPIDB(2));
    /*
        CDB[31:30] = 0 bit.
        SPIDB[17:16] = 0 bit
        DME[15] = 0 = disable
        CDE[14] = 1 = enable
        ADE[11:8 = 1111 (4 byte) or 0111 ( 3 byte )
        SPIDE[3:0] = 0xF = 32bit transfer
    */
    val_tmp = in32(rpc->vbase + RCAR_RPC_SMENR);
    val_tmp &= 0xffff0000;
    if (alen == 3)
    {
        out32(rpc->vbase + RCAR_RPC_SMENR, val_tmp | RCAR_RPC_SMDRENR_CDE | RCAR_RPC_SMDRENR_ADE(0x7));
    }
    else if (alen == 4 )
    {
        out32(rpc->vbase + RCAR_RPC_SMENR, val_tmp | RCAR_RPC_SMDRENR_CDE | RCAR_RPC_SMDRENR_ADE(0xF));
    }

    /* write cmd & addr */
    out32(rpc->vbase + RCAR_RPC_SMCR, RCAR_RPC_CMNCR_SSLKP  | RCAR_RPC_CMNCR_SPIE );

    val_tmp = in32(rpc->vbase + RCAR_RPC_SMENR);
    val_tmp &= 0xffff0000;
    out32(rpc->vbase + RCAR_RPC_SMENR, RCAR_RPC_SMDRENR_SPIDE(0xF));

    for (; dlen > 0; dlen-=4)
    {
        if(dlen < 4)
        {
            writeData = 0;
            switch(dlen)
            {
                case 3 :
                writeData |= ((wdata[2] << 8) & 0x0000FF00 );
                case 2 :
                writeData |= ((wdata[1] << 16) & 0x00FF0000);
                case 1 :
                writeData |= ((wdata[0] << 24) & 0xFF000000);
                break;
            }
            spi_write_data_32bit(dev,writeData);
        }else
        {
            writeData = 0;
            writeData = ((wdata[0]<<24) | (wdata[1]<<16) | (wdata[2]<<8) | wdata[3]);
            wdata+=4;
            spi_write_data_32bit(dev,writeData);
        }

        if ( (dlen) > 4)
        {
            out32(rpc->vbase + RCAR_RPC_SMCR, RCAR_RPC_CMNCR_SSLKP | RCAR_RPC_CMNCR_SPIWE | RCAR_RPC_CMNCR_SPIE );
        }
        else
        {
            out32(rpc->vbase + RCAR_RPC_SMCR,                        RCAR_RPC_CMNCR_SPIWE | RCAR_RPC_CMNCR_SPIE );
        }
    }
    return EOK;
}


static void rpc_hyper_write_4bytes(rpc_dev_t *dev, uint32_t addr, uint32_t data)
{
    rcar_rpc_t *rpc = dev->rpc;

    out32(rpc->vbase + RCAR_RPC_SMCMR, 0x0);
    out32(rpc->vbase + RCAR_RPC_SMADR, addr>>1);
    out32(rpc->vbase + RCAR_RPC_SMENR,
            (HYPER_CMENR_BIT_SIZES | RCAR_RPC_SMDRENR_CDE | RCAR_RPC_SMDRENR_OCDE |
             RCAR_RPC_SMDRENR_ADE(4) | RCAR_RPC_SMDRENR_SPIDE(0xC)));
    spi_write_data_32bit(dev,data);
    out32(rpc->vbase + RCAR_RPC_SMCR, RCAR_RPC_CMNCR_SPIWE | RCAR_RPC_CMNCR_SPIE);
    rpc_wait_tx_end(dev);
}

static uint32_t rpc_hyper_read_4bytes_reg(rpc_dev_t *dev, uint32_t addr)
{
    rcar_rpc_t *rpc = dev->rpc;
    uint32_t rddata;

    rpc_hyper_config(dev);
    rpc_xfer_init(dev, 16);
    out32(rpc->vbase + RCAR_RPC_SMCMR, 0x00800000);
    out32(rpc->vbase + RCAR_RPC_SMADR, addr>>1);
    out32(rpc->vbase + RCAR_RPC_SMENR,
            (HYPER_CMENR_BIT_SIZES | RCAR_RPC_SMDRENR_DME | RCAR_RPC_SMDRENR_CDE | RCAR_RPC_SMDRENR_OCDE |
             RCAR_RPC_SMDRENR_ADE(4) | RCAR_RPC_SMDRENR_SPIDE(0xC)));
    out32(rpc->vbase + RCAR_RPC_SMCR, RCAR_RPC_CMNCR_SPIRE | RCAR_RPC_CMNCR_SPIE);
    spi_read_data_32bit(dev, &rddata);

    return rddata;
}

static int rpc_hyper_write_cmd(void *hdl, uint32_t addr, uint32_t data)
{
    rpc_dev_t  *dev = hdl;
    rcar_rpc_t *rpc = dev->rpc;

    rpc_hyper_config(dev);
    rpc_xfer_init(dev, 0);
    out32(rpc->vbase + RCAR_RPC_SMCMR, 0x0);
    out32(rpc->vbase + RCAR_RPC_SMADR, addr);
    out32(rpc->vbase + RCAR_RPC_SMENR,
            (HYPER_CMENR_BIT_SIZES | RCAR_RPC_SMDRENR_CDE | RCAR_RPC_SMDRENR_OCDE |
             RCAR_RPC_SMDRENR_ADE(4) | RCAR_RPC_SMDRENR_SPIDE(8)));

    spi_write_data_32bit(dev,data);
    out32(rpc->vbase + RCAR_RPC_SMCR, RCAR_RPC_CMNCR_SPIWE | RCAR_RPC_CMNCR_SPIE);
    rpc_wait_tx_end(dev);

    return EOK;
}

static int rpc_hyper_read_reg(void *hdl, uint32_t addr, uint8_t *dbuf, int dlen)
{
    rpc_dev_t   *dev = hdl;
    uint32_t rddata;
    uint32_t add = addr;
    int i;

    if (dlen == 1)
    {
        rddata = rpc_hyper_read_4bytes_reg(dev, add<<1);
        *dbuf = (rddata >> 8) & 0xFF;
    }
    else
    {
        for (i = 0; i < dlen; i+=2)
        {
            rddata = rpc_hyper_read_4bytes_reg(dev, add<<1);
            dbuf[i] = (rddata >> 8) & 0xFF;
            dbuf[i + 1] = (rddata >> 24) & 0xFF;
            add += 0x2;
        }
    }

    return dlen;
}

static void rpc_hyper_enable_write(void *hdl)
{
    rpc_hyper_write_cmd(hdl, HYPER_UNLOCK1_ADDR, HYPER_UNLOCK1_DATA);
    rpc_hyper_write_cmd(hdl, HYPER_UNLOCK2_ADDR, HYPER_UNLOCK2_DATA);
}

static void rpc_hyper_read_status(void *hdl, uint16_t *status)
{
    rpc_dev_t  *dev = hdl;
    uint32_t rdstatus;

    rpc_hyper_write_cmd(hdl, HYPER_READ_STATUS_ADDR, HYPER_READ_STATUS_DATA);
    rdstatus = rpc_hyper_read_4bytes_reg(dev, 0x0);
    *status = ((rdstatus & 0xFF00) >> 8) | ((rdstatus & 0x00FF) << 8);
}

static int rpc_hyper_write_buffer(void *hdl, uint32_t addr, uint8_t *data, int dlen)
{
    rpc_dev_t   *dev = hdl;
    rcar_rpc_t *rpc = dev->rpc;
    uint16_t status;
    int rc = -1;

    rpc_hyper_enable_write(hdl);
    rpc_hyper_write_cmd(hdl, HYPER_ENTRY_ADDR, HYPER_WORD_PROGRAM_DATA);
    out32(rpc->vbase + RCAR_RPC_DRCR, 0x01FF0301);
    out32(rpc->vbase + RCAR_RPC_PHYCNT, 0x80000277);
    out32(rpc->vbase + RCAR_RPC_SMADR, addr>>1);
    out32(rpc->vbase + RCAR_RPC_SMENR,
            (HYPER_CMENR_BIT_SIZES | RCAR_RPC_SMDRENR_CDE | RCAR_RPC_SMDRENR_OCDE |
             RCAR_RPC_SMDRENR_ADE(4) | RCAR_RPC_SMDRENR_SPIDE(0xF)));

    memcpy((void *)(rpc->dbuf.vaddr + RCAR_TXDMA_OFF), data, dlen);
    rpc_setup_dma(dev, dlen, RCAR_RPC_WRITE, RCAR_TXDMA_OFF);

    if (rpc_dma_xfer(dev, dlen, RCAR_RPC_WRITE) == EOK)
        rc = dlen;
    out32(rpc->vbase + RCAR_RPC_SMCR, RCAR_RPC_CMNCR_SPIWE | RCAR_RPC_CMNCR_SPIE);
    rpc_wait_tx_end(dev);
    out32(rpc->vbase + RCAR_RPC_DRCR, 0x01FF0301);
    out32(rpc->vbase + RCAR_RPC_PHYCNT, 0x80000273);

    while (1)
    {
        rpc_hyper_read_status(dev, &status);

        if (status & HYPER_DEVICE_READY)
            break;
    }

    return rc;
}

static int rpc_hyper_write_word(void *hdl, uint32_t addr, uint8_t *data, int dlen)
{
    rpc_dev_t   *dev = hdl;
    uint32_t    writeData;
    uint32_t    add = addr;
    uint16_t    status;
    int         rc = 0;
    int         i;

    for (i = 0; i < dlen; i+=4)
    {
        rpc_hyper_enable_write(hdl);
        rpc_hyper_write_cmd(hdl, HYPER_ENTRY_ADDR, HYPER_WORD_PROGRAM_DATA);
        writeData = (data[i] | (data[i + 1]<<8) | (data[i + 2]<<16) | (data[i + 3]<<24));
        rpc_hyper_write_4bytes(dev, add, writeData);

        while (1)
        {
            rpc_hyper_read_status(dev, &status);
            if (status & HYPER_DEVICE_READY){
                rc = dlen;
                break;
            }
        }

        add += 0x4;
    }

    return rc;
}

static int rpc_hyper_write(void *hdl, uint32_t addr, uint8_t *data, int dlen)
{
    rpc_dev_t   *dev = hdl;
    int         rc = 0;
    int         i;
    uint32_t    add = addr;
    int         length;
    uint8_t     buf[256];
    int length1, length2;

    rpc_hyper_config(dev);
    rpc_xfer_init(dev, 0);
    rcar_write_protection_disable(dev);

    length1 = dlen % 8;
    length2 = dlen - length1;

    if (dlen >= 8)
    {
        for (i = 0; i < length2; i+=256)
        {
            if((length2 - i) >= 256)
                length = 256;
            else
                length = length2 - i;

            memcpy(buf, data + i, length);
            rpc_hyper_write_buffer(hdl, add, buf, length);

            if((length2 - i) >= 256)
                add+=256;
            else
                add+= (length2 - i);
        }
        rc = dlen;
    }

    if (length1 != 0){
        memcpy(buf, data + length2, length1);
        rpc_hyper_write_word(hdl, add, buf, length1);
        rc = dlen;
    }

    return (rc);
}

static int rpc_hyper_read(void *hdl, uint32_t addr, uint8_t *data, int dlen)
{
    rpc_dev_t  *dev = hdl;
    rcar_rpc_t *rpc = dev->rpc;
    uint8_t     *pbuf;
    int         len;
    int         rc = 0;

    if (dlen > RCAR_DMABUF_SIZE) {
        /* The RCAR_DMABUF_SIZE constant is set to the max message size in the F3S library (F3S_DATA_MAX).
         * The length we get here should never be larger than F3S_DATA_MAX, so RCAR_DMABUF_SIZE is sized
         * to match it to best use the DMA buffer.  See JI:2525409 for more information
         */
        fprintf(stderr, "RCAR_RPC: Requested read is greater than DMA buffer size\n");
        return EIO;
    }

    /* This calculation is to make 'dlen' divisible by 4 */
    if ((dlen%4) != 0)
    {
        len = dlen + (4 - (dlen%4));
    }
    else
    {
        len = dlen;
    }

    rpc_init_ext_mode(dev);
    rpc_setup_dma(dev, len, RCAR_RPC_READ, addr);

    if (rpc_dma_xfer(dev, len, RCAR_RPC_READ) == EOK)
    {
        pbuf = (uint8_t *)rpc->dbuf.vaddr;
        memcpy(data, pbuf, dlen);
        rc = dlen;
    }

    return rc;
}

static int rpc_dinit(void *hdl)
{
    rpc_dev_t  *dev = hdl;
    rcar_rpc_t *rpc = dev->rpc;

    if (--rpc->ndev)
        goto fini;

    // Disable fucntion
    out32(rpc->vbase + RCAR_RPC_SMCR, 0x00000000);

    ConnectDetach(rpc->coid);
    ChannelDestroy(rpc->chid);
    InterruptDetach(rpc->iid);

    rpc->dmafuncs.free_buffer(rpc->txdma, &rpc->dbuf);
    rpc->dmafuncs.channel_release(rpc->txdma);
    rpc->dmafuncs.channel_release(rpc->rxdma);
    rpc->dmafuncs.fini();

    munmap_device_io(rpc->vbase, rpc->size);

    rpc->vbase = 0;

fini:
    free(dev);

    return EOK;
}

static void query_hwinfo(rpc_dev_t *dev)
{
    unsigned hwi_off, tag_idx = 0;
    hwi_tag *tag;

    hwi_off = hwi_find_bus(RCAR_HWI_SPI, 0);
    if(hwi_off != HWI_NULL_OFF){
        tag = hwi_tag_find(hwi_off, HWI_TAG_NAME_location, 0);
        if(tag){
            rcar_rpc.pbase = tag->location.base;
            rcar_rpc.size = tag->location.len;

            while((tag = hwi_tag_find(hwi_off, HWI_TAG_NAME_regname, &tag_idx)) != NULL) {
                if(strcmp(RCAR_HWI_RPC_WBUF,  __hwi_find_string(tag->regname.regname)) == 0) {
                    rcar_rpc.buffer = rcar_rpc.pbase + tag->regname.offset;
                } else if(strcmp(RCAR_HWI_RPC_WBUF_SIZE,  __hwi_find_string(tag->regname.regname)) == 0) {
                    rcar_rpc.buffer_size = tag->regname.offset;
                }
            }
        }

        tag = hwi_tag_find(hwi_off, HWI_TAG_NAME_inputclk, 0);
        if(tag){
            rcar_rpc.clock = tag->inputclk.clk/tag->inputclk.div;
        }
    }
}

void* rpc_init(void)
{
    rpc_dev_t           *dev;
    int                 opt;
    char                *value;
    char                *freeptr;
    char                *options;
    static char         *opts[] = {"cs", "drate", "mode", "clock", "base", "size", "buffer", "buffersize", NULL};

    if ((dev = calloc(1, sizeof(rpc_dev_t))) == NULL) {
        fprintf(stderr, "RCAR_RPC: Could not allocate memory\n");
        return (NULL);
    }

    dev->bus   = 1;
    dev->csel  = 0;
    dev->drate = 160000000;

    //dev->spic.hcap                      = SPI_CAP_QUAD;
    dev->spic.dinit                     = rpc_dinit;
    dev->spic.spi_read_flash            = rpc_qspi_single_read;
    dev->spic.spi_write_flash           = rpc_qspi_single_write;
    dev->spic.spi_quad_read             = rpc_qspi_quad_read;
    dev->spic.spi_quad_write            = rpc_qspi_quad_write;
    dev->spic.spi_hyper_read_registers  = rpc_hyper_read_reg;
    dev->spic.spi_hyper_read_memory     = rpc_hyper_read;
    dev->spic.spi_hyper_write_cmd       = rpc_hyper_write_cmd;
    dev->spic.spi_hyper_write_memory    = rpc_hyper_write;

    query_hwinfo(dev);

    if(bsargs) {
        options = freeptr = strdup(bsargs);
        while (options && *options != '\0') {
            opt = getsubopt(&options, opts, &value);
            switch (opt) {
            case 0:     // chip select
                dev->csel  = strtoul(value, 0, 0);
                break;
            case 1:     // data rate
                dev->drate = strtoul(value, 0, 0);
                break;
            case 2:     // mode
                dev->mode  = strtoul(value, 0, 0);
                break;
            case 3:     // clock
                rcar_rpc.clock = strtoul(value, 0, 0);
                break;
            case 4:     // controller base
                rcar_rpc.pbase = strtoul(value, 0, 0);
                break;
            case 5:     // controller reg size
                rcar_rpc.size = strtoul(value, 0, 0);
                break;
            case 6:     // buffer base
                rcar_rpc.buffer = strtoul(value, 0, 0);
                break;
            case 7:     // buffer size
                rcar_rpc.buffer_size = strtoul(value, 0, 0);
                break;
            }
        }

        free(freeptr);
    }

    // First time called
    if (rcar_rpc.vbase == 0) {
        if(!rcar_rpc.pbase || !rcar_rpc.size || !rcar_rpc.buffer || !rcar_rpc.buffer_size) {
            fprintf(stderr, "RCAR_RPC: missing controller parameters\n");
            goto fail0;
        }

        if ((rcar_rpc.vbase = mmap_device_io(rcar_rpc.size, rcar_rpc.pbase)) == (uintptr_t)MAP_FAILED)
            goto fail0;

        /* attach interrupt */
        if ((rcar_rpc.chid = ChannelCreate(_NTO_CHF_DISCONNECT | _NTO_CHF_UNBLOCK)) == -1)
            goto fail1;

        if ((rcar_rpc.coid = ConnectAttach(0, 0, rcar_rpc.chid, _NTO_SIDE_CHANNEL, 0)) == -1)
            goto fail2;

        if (rcar_dma_init(&rcar_rpc) != EOK)
            goto fail3;
    }

    rcar_rpc.ndev++;
    dev->rpc = &rcar_rpc;

    return (dev);

fail3:
    ConnectDetach(rcar_rpc.coid);
fail2:
    ChannelDestroy(rcar_rpc.chid);
fail1:
    munmap_device_io(rcar_rpc.vbase, rcar_rpc.size);
fail0:
    free(dev);

    return (NULL);
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/flash/boards/rcar_qspi/gen3/rcar_rpc.c $ $Rev: 853179 $")
#endif
