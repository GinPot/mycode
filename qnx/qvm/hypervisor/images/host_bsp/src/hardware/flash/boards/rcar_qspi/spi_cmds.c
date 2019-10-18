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

#include <f3s_spi.h>

/**** Hyper flash functions *******/

int hyper_nor_write_memory(void *hdl, uint32_t addr, uint8_t *dbuf, int dlen)
{
    spi_flash_t *spic = (spi_flash_t *)hdl;

    return spic->spi_hyper_write_memory(hdl, addr, dbuf, dlen);
}

int hyper_nor_write_command(void *hdl, uint32_t addr, uint32_t data)
{
  spi_flash_t *spic = (spi_flash_t *)hdl;

  return spic->spi_hyper_write_cmd(hdl, addr, data);
}

int hyper_nor_sector_erase(void *hdl, uint32_t addr, int sector_size)
{
    /* Write Entry Command */
    if (hyper_nor_write_command(hdl, HYPER_UNLOCK1_ADDR, HYPER_UNLOCK1_DATA) < 0) {
        fprintf(stderr, "Unlock1 failed.\n");
        return (EIO);
    }

    if (hyper_nor_write_command(hdl, HYPER_UNLOCK2_ADDR, HYPER_UNLOCK2_DATA) < 0) {
        fprintf(stderr, "Unlock2 failed.\n");
        return (EIO);
    }

    if (hyper_nor_write_command(hdl, HYPER_ERASE_ADDR1, HYPER_ERASE_DATA1) < 0) {
        fprintf(stderr, "Third Erase Command failed.\n");
        return (EIO);
    }

    if (hyper_nor_write_command(hdl, HYPER_ERASE_ADDR2, HYPER_ERASE_DATA2) < 0) {
        fprintf(stderr, "Fourth Erase Command failed.\n");
        return (EIO);
    }

    if (hyper_nor_write_command(hdl, HYPER_ERASE_ADDR3, HYPER_ERASE_DATA3) < 0) {
        fprintf(stderr, "Fifth Erase Command failed.\n");
        return (EIO);
    }

    if (hyper_nor_write_command(hdl, addr/2, HYPER_ERASE_DATA4) < 0) {
        fprintf(stderr, "Sixth Erase Command failed.\n");
        return (EIO);
    }

    if (hyper_nor_busy_wait(hdl, HYPER_SECTOR_ERASE_TIME) == EOK)
        return (EOK);

    return (EIO);
}

int hyper_nor_read_registers(void *hdl, uint32_t addr, uint8_t *buf, int len)
{
    spi_flash_t *spic = (spi_flash_t *)hdl;

    if (len == 0)
        return 0;

    return spic->spi_hyper_read_registers(hdl, addr, buf, len);
}

int hyper_nor_read_memory(void *hdl, uint32_t addr, uint8_t *buf, int len)
{
    spi_flash_t *spic = (spi_flash_t *)hdl;

    if (len == 0)
        return 0;

    return spic->spi_hyper_read_memory(hdl, addr, buf, len);
}

int hyper_nor_busy_wait(void *hdl, uint32_t tmo)
{
    uint8_t sts;

    while (tmo--) {
        if (hyper_nor_write_command(hdl, HYPER_READ_STATUS_ADDR, HYPER_READ_STATUS_DATA) < 0) {
            fprintf(stderr, "Write command Read Status failed.\n");
            return (EIO);
        }

        if (hyper_nor_read_registers(hdl, 0x0, &sts, 1) < 0) {
            fprintf(stderr, "Read status register failed.\n");
            return (EIO);
        }

        if (sts & HYPER_DEVICE_READY)
            return (EOK);

        nanospin_ns(1000);
    }

    return (EAGAIN);
}

/**** QSPI flash functions *******/

int qspi_nor_busy_wait(void *hdl, uint32_t tmo)
{
    uint8_t *sts = (uint8_t *) calloc( 4, sizeof( uint8_t ));

    while (tmo--) {
        if (qspi_nor_read_registers(hdl, QSPI_CMD_RDSR, 0, 0, sts, 1)< 0)
        {
            return (EIO);
        }

        if (!((*sts) & FLASH_WIP))
        {
            return (EOK);
        }
    }

    return (EAGAIN);
}


int qspi_nor_enable_quad(void *hdl, int enable)
{
#if 0
    spi_flash_t *spi = (spi_flash_t *)hdl;

    uint8_t reg[2];

    if (qspi_nor_read_registers(hdl, QSPI_CMD_RDCR, 0, 0, &reg[1], 1) < 0)
        return (EIO);

    if ((reg[1] & (1 << 1)) && enable)
        return (EOK);
    if (!(reg[1] & (1 << 1)) && !enable)
        return (EOK);

    if (qspi_nor_read_registers(hdl, QSPI_CMD_RDSR, 0, 0, &reg[0], 1) < 0)
        return (EIO);

    /* write enable */
    if (qspi_nor_write(hdl, QSPI_CMD_WREN, 0, 0, NULL, 0) < 0) {
        fprintf(stderr, "Enable write failed.\n");
        return (EIO);
    }

    reg[1] ^= 1 << 1;

    // write registers
    if (qspi_nor_write(hdl, QSPI_CMD_WRSR, 0, 0, reg, 2) < 0) {
        fprintf(stderr, "Quad enable failed.\n");
        return (EIO);
    }

    if (qspi_nor_busy_wait(hdl, 1024 * 16) != EOK)
        return (EIO);
#endif
    return (EOK);
}


int qspi_nor_page_program(void *hdl, uint32_t addr, int alen, uint8_t *buf, int len)
{
    uint8_t cmd;

    if (len == 0)
        return 0;

    if (qspi_nor_write(hdl, QSPI_CMD_WREN, 0, 0, NULL, 0) < 0) {
        fprintf(stderr, "Send WREN failed.\n");
        return (-1);
    }

    if (alen == 4)
        cmd = QSPI_CMD_4PP;
    else
        cmd = QSPI_CMD_PP;

    if (qspi_nor_write_memory(hdl, cmd, addr, alen, buf, len) < 0) {
        fprintf(stderr, "Page program failed.\n");
        return (-1);
    }

    if (qspi_nor_busy_wait(hdl, 4096 * len) != EOK)
    {
        return (-1);
    }

    return (len);
}


int qspi_nor_read_memory(void *hdl, uint32_t addr, int alen, uint8_t *buf, int len)
{
    spi_flash_t *spic = (spi_flash_t *)hdl;
    uint8_t     cmd;
    uint8_t     cmd_quad;

    if (len == 0)
        return 0;


    if (alen == 4) {
        cmd = QSPI_CMD_4FREAD;
    } else
        cmd = QSPI_CMD_FREAD;


    if ((spic->dcap & spic->hcap & SPI_CAP_QUAD) && spic->spi_quad_read) {
        if (!(spic->mode & SPI_MODE_QUAD)) {
#if 0
            qspi_nor_enable_quad(hdl, 1);
#else
            if (qspi_nor_enable_quad(hdl, 1) == EOK)
                spic->mode |= SPI_MODE_QUAD;
            else
                spic->hcap &= ~SPI_CAP_QUAD;
#endif
        }
        if (spic->mode & SPI_MODE_QUAD) {
           int rc;

            cmd_quad = alen == 4 ? QSPI_CMD_4QOR : QSPI_CMD_QOR;
            if ((rc = spic->spi_quad_read(hdl, cmd_quad, addr ,alen, buf, len)) > 0)
                return (rc);

            // fall down to single read
            qspi_nor_enable_quad(hdl, 0);

            spic->hcap &= ~SPI_CAP_QUAD;
        }
    }

     return spic->spi_read_flash(hdl, cmd, addr, alen, buf, len);
}


int qspi_nor_read_registers(void *hdl, uint8_t cmd, uint32_t arg, int alen, uint8_t *data, int dlen)
{
    spi_flash_t *spic = (spi_flash_t *)hdl;

    return spic->spi_read_flash(hdl, cmd, arg , alen, data, dlen);
}


int qspi_nor_sector_erase(void *hdl, uint32_t addr, int alen, int sector_size)
{
    uint8_t cmd = QSPI_CMD_P4E;  // default command

    /* write enable */
    if (qspi_nor_write(hdl, QSPI_CMD_WREN, 0, 0, NULL, 0) < 0) {
        fprintf(stderr, "Enable write failed.\n");
        return (EIO);
    }

    if (sector_size == 4096) {
        if (alen == 4)
            cmd = QSPI_CMD_4P4E;
    } else {
        if (alen == 4)
            cmd = QSPI_CMD_4SE;
        else
            cmd = QSPI_CMD_SE;
    }

    if (qspi_nor_write(hdl, cmd, addr, alen, NULL, 0) < 0) {
        fprintf(stderr, "Sector erase failed.\n");
        return (EIO);
    }

    if (qspi_nor_busy_wait(hdl, 1024 * 4096) != EOK)
    {
        return (-1);
    }
    return (EOK);
}


int qspi_nor_write_memory(void *hdl, uint8_t cmd, uint32_t arg, int alen, uint8_t *dbuf, int dlen)
{
    spi_flash_t *spic = (spi_flash_t *)hdl;
    uint8_t     cmd_quad;

    if ((spic->dcap & spic->hcap & SPI_CAP_QUAD) && spic->spi_quad_write) {
        if (!(spic->mode & SPI_MODE_QUAD)) {
            if (qspi_nor_enable_quad(hdl, 1) == EOK)
                spic->mode |= SPI_MODE_QUAD;
            else
                spic->hcap &= ~SPI_CAP_QUAD;
        }
        if (spic->mode & SPI_MODE_QUAD) {
            int rc;

            cmd_quad = alen == 4 ? QSPI_CMD_4PP : QSPI_CMD_PP;
            rc = spic->spi_quad_write(hdl, cmd_quad, arg , alen, dbuf, dlen);
            if (rc > 0)
                return (rc);

            // fall down to single read
            qspi_nor_enable_quad(hdl, 0);

            spic->hcap &= ~SPI_CAP_QUAD;
        }
    }

    return spic->spi_write_flash(hdl, cmd , arg , alen, dbuf, dlen);
}


int qspi_nor_write(void *hdl, uint8_t cmd, uint32_t arg, int alen, uint8_t *dbuf, int dlen)
{
    spi_flash_t *spic = (spi_flash_t *)hdl;

    return spic->spi_write_flash(hdl, cmd , arg , alen, dbuf, dlen);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/flash/boards/rcar_qspi/spi_cmds.c $ $Rev: 839522 $")
#endif
