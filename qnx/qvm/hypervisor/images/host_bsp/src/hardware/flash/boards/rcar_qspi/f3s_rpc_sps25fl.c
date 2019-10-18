/*
 * $QNXLicenseC:
 * Copyright 2017, QNX Software Systems.
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

#include "f3s_spi.h"

const struct qspi_flash_params qspi_flash_params_table[] = {
 /* Cypress */
    {"s25fl008a",      INFO(0x010213, 0x0, 64 * 1024,    16, 0) },
    {"s25fl016a",      INFO(0x010214, 0x0, 64 * 1024,    32, 0) },
    {"s25fl032a",      INFO(0x010215, 0x0, 64 * 1024,    64, 0) },
    {"s25fl064a",      INFO(0x010216, 0x0, 64 * 1024,   128, 0) },
    {"s25fl116k",      INFO(0x014015, 0x0, 64 * 1024,   128, 0) },
    {"s25fl164k",      INFO(0x014017, 0x0140,  64 * 1024,   128, 0) },
    {"s25fl128p_256k", INFO(0x012018, 0x0300, 256 * 1024,    64, RD_FULL | WR_QPP) },
    {"s25fl128p_64k",  INFO(0x012018, 0x0301,  64 * 1024,   256, RD_FULL | WR_QPP) },
    {"s25fl032p",      INFO(0x010215, 0x4d00,  64 * 1024,    64, RD_FULL | WR_QPP) },
    {"s25fl064p",      INFO(0x010216, 0x4d00,  64 * 1024,   128, RD_FULL | WR_QPP) },
    {"s25fl128s_256k", INFO(0x012018, 0x4d00, 256 * 1024,    64, RD_FULL | WR_QPP) },
    {"s25fl128s_64k",  INFO(0x012018, 0x4d01,  64 * 1024,   256, RD_FULL | WR_QPP) },
    {"s25fl256s_256k", INFO(0x010219, 0x4d00, 256 * 1024,   128, RD_FULL | WR_QPP) },
    {"s25fl256s_64k",  INFO(0x010219, 0x4d01,  64 * 1024,   512, RD_FULL | WR_QPP) },
    {"s25fs256s_64k",  INFO6(0x010219, 0x4d0181, 64 * 1024, 512, RD_FULL | WR_QPP | SECT_4K) },
    {"s25fs512s",      INFO6(0x010220, 0x4d0081, 256 * 1024, 2048, RD_FULL | WR_QPP | SECT_4K) },
    {"s25fl512s_256k", INFO(0x010220, 0x4d00, 256 * 1024,   256, RD_FULL | WR_QPP) },
    {"s25fl512s_64k",  INFO(0x010220, 0x4d01,  64 * 1024,  1024, RD_FULL | WR_QPP) },
    {"s25fl512s_512k", INFO(0x010220, 0x4f00, 256 * 1024,   256, RD_FULL | WR_QPP) },

    {}, /* Empty entry to terminate the list */
    /*
     * Note:
     * Below paired flash devices has similar spi_flash params.
     * (s25fl129p_64k, s25fl128s_64k)
     * (w25q80bl, w25q80bv)
     * (w25q16cl, w25q16dv)
     * (w25q32bv, w25q32fv_spi)
     * (w25q64cv, w25q64fv_spi)
     * (w25q128bv, w25q128fv_spi)
     * (w25q32dw, w25q32fv_qpi)
     * (w25q64dw, w25q64fv_qpi)
     * (w25q128fw, w25q128fv_qpi)
     */
};

/*
 * This is the write callout for SPI serial NOR flash.
 */
int32_t sps25fl_write(f3s_dbase_t *dbase,
                         f3s_access_t *access,
                         uint32_t flags,
                         uint32_t offset,
                         int32_t size,
                         uint8_t *buffer)
{
    int         rc;
    // void        *buf;
    uint8_t        *buf;
    int         pagesz = dbase->buffer_size;
    int         left;
    int         cursz;
    void        *hdl = access->socket.socket_handle;
    spi_flash_t *spi = hdl;
    int         alen;
    if (access->service->page(&access->socket, 0, offset, &size) == NULL)
        return -1;

    cursz = pagesz - (offset & (pagesz - 1));
    if (cursz > size)
        cursz = size;

    alen = spi->dcap & SPI_CAP_ADDR4 ? 4 : 3;

    if ((rc = qspi_nor_page_program(hdl, offset, alen, buffer, cursz)) < 0) {
        errno = EIO;
        return -1;
    }

    if (!(flags & F3S_VERIFY_WRITE))
        return rc;

    /* verify data was written correctly */
    buf  = alloca(rc);
    if (buf == NULL)
        return -1;
    left = rc;
    while (left) {
        cursz = qspi_nor_read_memory(hdl, offset, alen, buf, left);

        if (cursz <= 0) {
            errno = EIO;
            return -1;
        }
        if (memcmp(buffer, buf, cursz)) {
            fprintf(stderr, "(devf  t%d::%s:%d) program verify error\n"
                        "between offset 0x%x and 0x%x, size = %d\n",
                        pthread_self(), __func__, __LINE__, offset, offset+cursz, cursz);
            errno = EIO;
            return -1;
        }
        /* adjust */
        left   -= cursz;
        offset += cursz;
        buffer += cursz;
    }

    return rc;
}
/*
 * Summary
 *
 * MTD Version:    2 only
 * Locking Method: Persistent
 *
 * Description
 *
 * Use this for Cypress S25FS SPI flash capable of block locking.
 */

int sps25fl_unlockall(f3s_dbase_t *dbase, f3s_access_t *access,
                 uint32_t flags, uint32_t offset)
{
    void    *hdl = access->socket.socket_handle;
    /* check that this chip supports Persistent protection. */
    if (!(dbase->flags & F3S_PROTECT_PERSISTENT))
        return (ENOTSUP);

    if (qspi_nor_write(hdl, QSPI_CMD_WREN, 0, 0, NULL, 0) < 0) {
        fprintf(stderr, "Send WREN failed.\n");
        return (EIO);
    }

    if (qspi_nor_write(hdl, QSPI_CMD_PPBE, 0, 0, NULL, 0) < 0) {
        fprintf(stderr, "Send PPBE failed.\n");
        return (EIO);
    }

    if (qspi_nor_busy_wait(hdl, 4096) == EOK)
        return (EOK);

    return (EIO);
}


/*
 * Summary
 *
 * MTD Version:    2 only
 * Locking Method: Non-persistent
 *
 * Description
 *
 * Use this for Cypress S25FS SPI flash capable of block locking.
 */

int sps25fl_unlock(f3s_dbase_t *dbase, f3s_access_t *access,
                 uint32_t flags, uint32_t offset)
{
    void        *hdl = access->socket.socket_handle;
    spi_flash_t *spi = hdl;
    uint8_t     data = 0xFF;
    /* check that this chip supports Dynamic protection. */
    if (!(dbase->flags & F3S_PROTECT_DYN))
        return (ENOTSUP);

    if (qspi_nor_write(hdl, QSPI_CMD_WREN, 0, 0, NULL, 0) < 0) {
        fprintf(stderr, "Send WREN failed.\n");
        return (EIO);
    }

    if (qspi_nor_write(hdl, QSPI_CMD_DYBWR, offset, spi->dcap & SPI_CAP_ADDR4 ? 4 : 3, &data, 1) < 0) {
        fprintf(stderr, "Lock failed.\n");
        return (EIO);
    }

    if (qspi_nor_busy_wait(hdl, 4096) == EOK)
        return (EOK);

    return (EIO);
}



/*
 * This is the sync callout for SPI serial NOR flash.
 */
int32_t sps25fl_sync (f3s_dbase_t *dbase,
                    f3s_access_t *access,
                    uint32_t flags,
                    uint32_t text_offset)
{
    if (access->service->page (&access->socket, 0, text_offset, NULL) == NULL)
        return (ERANGE);

    return qspi_nor_busy_wait(access->socket.socket_handle, 1);
}

/*
 * This is the erase suspend callout for SPI serial NOR flash.
 */
int sps25fl_suspend(f3s_dbase_t *dbase,
                     f3s_access_t *access,
                     uint32_t flags,
                     uint32_t offset)
{
    int     loop;
    uint8_t *sts = (uint8_t *) calloc( 4, sizeof( uint8_t ));
    void    *hdl = access->socket.socket_handle;
    if (access->service->page(&access->socket, 0, offset, NULL) == NULL)
        return (ERANGE);

    if (qspi_nor_write(hdl, QSPI_CMD_ERSP, 0, 0, NULL, 0) < 0)
        fprintf(stderr, "Software reset failed.\n");

    /* max 45us according to spec */
    for (loop = 64; loop > 0; loop--) {
        if (qspi_nor_read_registers(hdl, QSPI_CMD_RDSR, 0, 0, sts, 1) < 0) {
            fprintf(stderr, "Read status register failed.\n");
            return (EIO);
        }

        if (!(*sts & FLASH_WIP)) {       // not in writing
            if (qspi_nor_read_registers(hdl, QSPI_CMD_RDSR2, 0, 0, sts, 1) < 0) {
                fprintf(stderr, "Read status register failed.\n");
                return (EIO);
            }

            if ((*sts & FLASH_ES) == 0)  // no WIP, no suspend, so erase completed
                return (ECANCELED);

            /* erase is suspended */

            return (EOK);
        }

        nanospin_ns(1000);
    }

    return (EIO);
}


/*
 * This is the erase resume callout for SPI serial NOR flash.
 */
int sps25fl_resume(f3s_dbase_t *dbase,
					 f3s_access_t *access,
					 uint32_t flags,
					 uint32_t offset)
{
	if (access->service->page(&access->socket, 0, offset, NULL) == NULL)
		return (ERANGE);

	if (qspi_nor_write(access->socket.socket_handle, QSPI_CMD_ERRS, 0, 0, NULL, 0) < 0)
		fprintf(stderr, "Software reset failed.\n");

	nanospin_ns(100 * 1000);

	return (EOK);
}


/*
 * This is the sync callout for SPI serial NOR flash.
 */
void sps25fl_reset (f3s_dbase_t *dbase,
                    f3s_access_t *access,
                    uint32_t flags,
                    uint32_t offset)
{

    if (qspi_nor_write(access->socket.socket_handle, QSPI_CMD_RSTEN, 0, 0, NULL, 0) < 0)
        fprintf(stderr, "Software reset failed.\n");
    if (qspi_nor_write(access->socket.socket_handle, QSPI_CMD_RST, 0, 0, NULL, 0) < 0)
        fprintf(stderr, "Software reset failed.\n");
}

/*
 * This is the read callout for SPI serial NOR flash.
 */
int32_t sps25fl_read (f3s_dbase_t *dbase,
                      f3s_access_t *access,
                      uint32_t flags,
                      uint32_t text_offset,
                      int32_t buffer_size,
                      uint8_t *buffer)
{
    spi_flash_t *spi = access->socket.socket_handle;
    int         rc;
    rc = qspi_nor_read_memory(access->socket.socket_handle,
                    text_offset, spi->dcap & SPI_CAP_ADDR4 ? 4 : 3, buffer, buffer_size);
    if (rc < 0) {
        errno = EIO;
        return -1;
    }
    return  rc;     // return number of bytes read
}

/*
 * Summary
 *
 * MTD Version:    2 only
 * Locking Method: Persistent and Non-persistent
 *
 * Description
 *
 * Use this for Cypress S25FS SPI flash capable of block locking.
 */

int sps25fl_lock(f3s_dbase_t *dbase, f3s_access_t *access,
                 uint32_t flags, uint32_t offset)
{
    void    *hdl = access->socket.socket_handle;
    uint8_t data = 0;
    /* The DYB bits aren't locked, check the PPB bits */
    if (dbase->flags & F3S_PROTECT_PERSISTENT)
        return sps25fl_dolock(hdl, QSPI_CMD_PPBP, offset, NULL);

    /* check that this chip supports Dynamic protection. */
    if (dbase->flags & F3S_PROTECT_DYN)
        return sps25fl_dolock(hdl, QSPI_CMD_DYBWR, offset, &data);

    return (ENOTSUP);
}

int sps25fl_dolock(void *hdl, uint8_t cmd, uint32_t offset, uint8_t *data)
{
    spi_flash_t *spi = hdl;
    if (qspi_nor_write(hdl, QSPI_CMD_WREN, 0, 0, NULL, 0) < 0) {
        fprintf(stderr, "Send WREN failed.\n");
        return (EIO);
    }

    if (qspi_nor_write(hdl, cmd, offset, spi->dcap & SPI_CAP_ADDR4 ? 4 : 3, data, data ? 1 : 0) < 0) {
        fprintf(stderr, "Lock failed.\n");
        return (EIO);
    }

    if (qspi_nor_busy_wait(hdl, 4096) == EOK)
        return (EOK);

    return (EIO);
}
/*
 * Summary
 *
 * MTD Version:    2 only
 * Locking Method: Persistent and Non-persistent
 *
 * Description
 *
 * Use this for Cypress S25FS SPI flash capable of block locking.
 */

int sps25fl_islock(f3s_dbase_t *dbase,
                   f3s_access_t *access,
                   uint32_t flags,
                   uint32_t offset)
{
    uint8_t     data, alen;
    spi_flash_t *spi = access->socket.socket_handle;

    alen = spi->dcap & SPI_CAP_ADDR4 ? 4 : 3;
    /* check that this chip supports Dynamic protection. */
    if (dbase->flags & F3S_PROTECT_DYN) {
        if (qspi_nor_read_registers(access->socket.socket_handle, QSPI_CMD_DYBRD, offset, alen, &data, 1) < 0) {
            fprintf(stderr, " failed.\n");
            return (EIO);
        }

        if (!(data & 0x01))
            return (EROFS);
    }

    /* The DYB bits aren't locked, check the PPB bits */
    if (dbase->flags & F3S_PROTECT_PERSISTENT) {
        if (qspi_nor_read_registers(access->socket.socket_handle, QSPI_CMD_PPBRD, offset, alen, &data, 1) < 0) {
            fprintf(stderr, " failed.\n");
            return (EIO);
        }

        if (!(data & 0x01))
            return (EROFS);
    }

    return (EOK);
}

/*
 * This is the ident callout for Cypress S25FS SPI serial NOR flash.
 */
int32_t sps25fl_ident(f3s_dbase_t *dbase,
                       f3s_access_t *access,
                       uint32_t flags,
                       uint32_t offset)
{
    int32_t     unit_size;
    int32_t     geo_index = 0;
    uint8_t     buf[88];
    int32_t     chip_size = 0;
    spi_flash_t *spi = access->socket.socket_handle;
    const struct qspi_flash_params *params;

    if (qspi_nor_read_registers(access->socket.socket_handle, QSPI_CMD_RDID, 0, 0, buf, 81) < 0) {
        fprintf(stderr, "Read JEDEC-ID failed\n");
        return (ENOENT);
    }

    uint8_t manufacture_id = buf[0];
    uint16_t jedec = buf[1] << 8 | buf[2];
    uint16_t ext_jedec = buf[3] << 8 | buf[4];
    uint8_t family_id = buf[5];

    /* Validate params from qspi_flash_params table */
    params = qspi_flash_params_table;
    for (; params->name != NULL; params++) {
        if (JEDEC_MFR(params) == manufacture_id) {
            if (JEDEC_ID(params) == jedec) {
                if (params->id_len - 3 == 0) {
                    break;
                } else if (params->id_len - 3 == 2) {
                    if (JEDEC_EXT(params) == ext_jedec) {
                        break;
                    }
                } else {
                    if ((JEDEC_EXT(params) == (ext_jedec & 0xFF00)) && (JEDEC_FID(params) == family_id)) {
                        break;
                    }
                }
            }
        }
    }

    /* Fill dbase entry */
    dbase->struct_size = sizeof(*dbase);
    dbase->jedec_hi    = buf[0];
    dbase->jedec_lo    = ((uint16_t)buf[1] << 8) | buf[2];
    dbase->name        = "Cypress MirrotBit Multi-I/O";

    /* Read buffer size information */
    dbase->buffer_size = buf[0x2b];
    dbase->buffer_size <<= 8;
    dbase->buffer_size += buf[0x2a];

    /* Value is 2^N bytes per chip */
    dbase->buffer_size = 1 << dbase->buffer_size;

    /* Read number of geometries */
    dbase->geo_num = 1;
    dbase->geo_vect[0].unit_num = 1024 ;
    unit_size = 256;

    if (unit_size == 0) unit_size  = 128;
    else                unit_size *= 256;

    chip_size += unit_size * dbase->geo_vect[geo_index].unit_num;

    /* Convert size to power of 2 */
        dbase->geo_vect[0].unit_pow2 = 0;
        while (unit_size > 1) {
            unit_size >>= 1;
            dbase->geo_vect[0].unit_pow2++;
        }

    access->socket.window_size = chip_size;

    if( (params->flags & RD_FULL) && (params->flags & WR_QPP)){
        spi->dcap |= SPI_CAP_QUAD | SPI_CAP_ADDR4;
    }

    /* Detect read / write suspend */
    if      (buf[0x46] == 1) dbase->flags = F3S_ERASE_FOR_READ;
    else if (buf[0x46] == 2) dbase->flags = F3S_ERASE_FOR_READ | F3S_ERASE_FOR_WRITE;
    else                     dbase->flags = 0;

    return (EOK);
}


/*
 * This is the erase callout for SPI serial NOR flash.
 */
int sps25fl_erase(f3s_dbase_t *dbase,
                     f3s_access_t *access,
                     uint32_t flags,
                     uint32_t offset)
{
    int         geo_index;
    int         size = 0;
    spi_flash_t *spi = access->socket.socket_handle;
    if (access->service->page(&access->socket, 0, offset, NULL) == NULL)
        return (ERANGE);

    for (geo_index = 0; geo_index < dbase->geo_num; geo_index++) {
        size += (1 << dbase->geo_vect[geo_index].unit_pow2) * dbase->geo_vect[geo_index].unit_num;

        if (size > offset)
            break;
    }

    /* 4 bytes address cycle, variable sector size */
    return qspi_nor_sector_erase(access->socket.socket_handle,
                    offset, spi->dcap & SPI_CAP_ADDR4 ? 4 : 3, 1 << dbase->geo_vect[geo_index].unit_pow2);
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/flash/boards/rcar_qspi/f3s_rpc_sps25fl.c $ $Rev: 839581 $")
#endif
