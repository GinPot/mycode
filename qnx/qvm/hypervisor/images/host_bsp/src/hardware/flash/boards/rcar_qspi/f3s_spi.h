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

#ifndef __F3S_SPI_H_INCLUDED
#define __F3S_SPI_H_INCLUDED

/*
** Includes
*/
#include <sys/f3s_mtd.h>
#include <pthread.h>

/*
** Function Prototypes
*/
int32_t f3s_qspi_open(f3s_socket_t *socket, uint32_t flags);
uint8_t *f3s_qspi_page(f3s_socket_t *socket, uint32_t page, uint32_t offset, int32_t *size);
int32_t f3s_qspi_status(f3s_socket_t *socket, uint32_t flags);
void f3s_qspi_close(f3s_socket_t *socket, uint32_t flags);

#define JEDEC_MFR(info)     ((info)->id[0])
#define JEDEC_ID(info)      (((info)->id[1]) << 8 | ((info)->id[2]))
#define JEDEC_EXT(info)     (((info)->id[3]) << 8 | ((info)->id[4]))
#define JEDEC_FID(info)     ((info)->id[5])
#define SPI_FLASH_MAX_ID_LEN    6

/* Spansion HyperFlash S26KS512 */
#define HYPER_BUS_WIDTH                 (4)
#define HYPER_CHIP_INTERLEAVE           (2)
#define HYPER_ENTRY_ADDR                (0x555)
#define HYPER_UNLOCK1_ADDR              (0x555)
#define HYPER_UNLOCK1_DATA              (0xAA << 24)
#define HYPER_UNLOCK2_ADDR              (0x2AA)
#define HYPER_UNLOCK2_DATA              (0x55 << 24)
#define HYPER_ID_ENTRY_ADDR             (HYPER_ENTRY_ADDR)
#define HYPER_ID_ENTRY_DATA             (0x90 << 24)
#define HYPER_ERASE_ADDR1               (0x555)
#define HYPER_ERASE_DATA1               (0x80 << 24)
#define HYPER_ERASE_ADDR2               (0x555)
#define HYPER_ERASE_DATA2               (0xAA << 24)
#define HYPER_ERASE_ADDR3               (0x2AA)
#define HYPER_ERASE_DATA3               (0x55 << 24)
#define HYPER_ERASE_DATA4               (0x30 << 24)
#define HYPER_READ_STATUS_ADDR          (0x555)
#define HYPER_READ_STATUS_DATA          (0x70 << 24)
#define HYPER_RESET_ADDR                (0x00)
#define HYPER_RESET_DATA                (0xF0 << 24)
#define HYPER_WORD_PROGRAM_DATA         (0xA0 << 24)
#define HYPER_BUFFER_PROGRAM_DATA       (0x25 << 24)
#define HYPER_BUFFER_PROGRAM_CONFIRM    (0x29 << 24)
#define HYPER_SECTOR_ERASE_DATA         (0x30 << 24)
#define HYPER_CHIP_ERASE_DATA           (0x10 << 24)
#define HYPER_ERASE_RESUME_DATA         (0x30 << 24)
#define HYPER_ERASE_SUSPEND_DATA        (0xB0 << 24)

/* HyperFlash PPB address and data command */
#define HYPER_PPB_ENTRY_ADDR            (HYPER_ENTRY_ADDR)
#define HYPER_PPB_ENTRY_DATA            (0xC0 << 24)
#define HYPER_PPB_LOCK_ENTRY_DATA       (0x50 << 24)
#define HYPER_PPB_PROGRAM_DATA          (0xA0 << 24)
#define HYPER_PPB_ERASE_DATA1           (0x80 << 24)
#define HYPER_PPB_ERASE_DATA2           (0x30 << 24)

/* HyperFlash DYB address and data command */
#define HYPER_DYB_ENTRY_ADDR            (HYPER_ENTRY_ADDR)
#define HYPER_DYB_ENTRY_DATA            (0xE0 << 24)
#define HYPER_DYB_PROGRAM_DATA          (0xA0 << 24)

/* HyperFlash status register */
#define HYPER_DEVICE_READY              (1 << 7)    // Device ready bit
#define HYPER_ERASE_SUSPEND             (1 << 6)    // erase suspended

#define HYPER_SECTOR_ERASE_TIME         (930000)
#define HYPER_RESET_TIME                (4096)

/* QSPI S25FL */
#define QSPI_CMD_WRR     0x01    // Write Status Register
#define QSPI_CMD_PP      0x02    // Page programming
#define QSPI_CMD_READ    0x03    // Read memory
#define QSPI_CMD_WRDI    0x04    // Write disable
#define QSPI_CMD_RDSR    0x05    // Read Status Register
#define QSPI_CMD_WREN    0x06    // Write enable
#define QSPI_CMD_RDSR2   0x07    // Read Status Register 2
#define QSPI_CMD_FREAD   0x0B    // Fast read memory
#define QSPI_CMD_4FREAD  0x0C    // Fast read memory, 4 bytes address
#define QSPI_CMD_4PP     0x12    // page programming, 4 bytes address
#define QSPI_CMD_4READ   0x13    // Read memory, 4 byte address
#define QSPI_CMD_P4E     0x20    // 4K sector erase
#define QSPI_CMD_4P4E    0x21    // 4K sector erase, 4 bytes address
//#define QSPI_CMD_QPP     0x32    // Quad Page Program, 3 bytes address
//#define QSPI_CMD_4QPP    0x34    // Quad Page Program
#define QSPI_CMD_RDCR    0x35    // Read Configuration Register 1
#define QSPI_CMD_EWSR    0x50    // Enable write status register
#define QSPI_CMD_RDAR    0x65    // Read anny register
#define QSPI_CMD_QOR     0xEB    // Quad Read Out, 3 bytes address
#define QSPI_CMD_4QOR    0xEC    // Quad Read Out, 4 bytes address
#define QSPI_CMD_WRAR    0x71    // Write anny register
#define QSPI_CMD_ERSP    0x75    // Erase suspend
#define QSPI_CMD_ERRS    0x7A    // Erase resume
#define QSPI_CMD_RDID    0x9F    // JEDEC Read-ID
#define QSPI_CMD_AAIP    0xAD    // Auto address increment programming
#define QSPI_CMD_SE      0xD8    // 64K sector erase
#define QSPI_CMD_4SE     0xDC    // 64K sector erase, 4 bytes address
#define QSPI_CMD_DYBRD   0xE0    // Read DRB register
#define QSPI_CMD_DYBWR   0xE1    // Write DRB register
#define QSPI_CMD_PPBRD   0xE2    // Read PPB register
#define QSPI_CMD_PPBP    0xE3    // PPB program
#define QSPI_CMD_PPBE    0xE4    // PPB erase
#define QSPI_CMD_RSTEN   0x66
#define QSPI_CMD_RST     0x99
#define QSPI_CMD_RESET     0xF0    // Legacy software reset

#define FLASH_WIP       (1 << 0)    // Write in Progress (WIP) bit
#define FLASH_ES        (1 << 1)    // erase suspended



/* Used when the "_ext_id" is two bytes at most */
#define INFO(_jedec_id, _ext_id, _sector_size, _n_sectors, _flags)  \
        .id = {                         \
            ((_jedec_id) >> 16) & 0xff,         \
            ((_jedec_id) >> 8) & 0xff,          \
            (_jedec_id) & 0xff,             \
            ((_ext_id) >> 8) & 0xff,            \
            (_ext_id) & 0xff,               \
            },                      \
        .id_len = (!(_jedec_id) ? 0 : (3 + ((_ext_id) ? 2 : 0))),   \
        .sector_size = (_sector_size),              \
        .n_sectors = (_n_sectors),              \
        .page_size = 256,                   \
        .flags = (_flags),
#define INFO6(_jedec_id, _ext_id, _sector_size, _n_sectors, _flags) \
        .id = {                         \
            ((_jedec_id) >> 16) & 0xff,         \
            ((_jedec_id) >> 8) & 0xff,          \
            (_jedec_id) & 0xff,             \
            ((_ext_id) >> 16) & 0xff,           \
            ((_ext_id) >> 8) & 0xff,            \
            (_ext_id) & 0xff,               \
            },                      \
        .id_len = 6,                        \
        .sector_size = (_sector_size),              \
        .n_sectors = (_n_sectors),              \
        .page_size = 256,                   \
        .flags = (_flags),


/* Cypress */
_int32 sps25fl_ident(f3s_dbase_t * dbase, f3s_access_t * access,
            _uint32 flags, _uint32 offset);

void sps25fl_reset(f3s_dbase_t *dbase,
                    f3s_access_t *access,
                    uint32_t flags,
                    uint32_t offset);

int32_t sps25fl_read(f3s_dbase_t *dbase,
                    f3s_access_t *access,
                    uint32_t flags,
                    uint32_t offset,
                    int32_t buffer_size,
                    uint8_t *buffer);

_int32 sps25fl_write(f3s_dbase_t * dbase, f3s_access_t * access,
            _uint32 flags, _uint32 offset,
            _int32 size, _uint8 * buffer);

int sps25fl_suspend(f3s_dbase_t *dbase, f3s_access_t *access,
                     uint32_t flags, uint32_t offset);

int sps25fl_resume(f3s_dbase_t *dbase, f3s_access_t *access,
                     uint32_t flags, uint32_t offset);

int sps25fl_erase(f3s_dbase_t * dbase, f3s_access_t * access,
            _uint32 flags, _uint32 offset);

int32_t sps25fl_sync(f3s_dbase_t *dbase, f3s_access_t *access,
                    uint32_t flags, uint32_t offset);

int sps25fl_islock(f3s_dbase_t *dbase, f3s_access_t *access,
                   uint32_t flags, uint32_t offset);

int sps25fl_dolock(void *hdl, uint8_t cmd, uint32_t offset, uint8_t *data);

int sps25fl_lock(f3s_dbase_t *dbase, f3s_access_t *access,
                 uint32_t flags, uint32_t offset);

int sps25fl_unlock(f3s_dbase_t *dbase, f3s_access_t *access,
                 uint32_t flags, uint32_t offset);

int sps25fl_unlockall(f3s_dbase_t *dbase, f3s_access_t *access,
                 uint32_t flags, uint32_t offset);

/* S26KS */
int32_t sps26ks_ident(f3s_dbase_t * dbase, f3s_access_t * access,
                      _uint32 flags, _uint32 offset);

void sps26ks_reset(f3s_dbase_t *dbase,
                    f3s_access_t *access,
                    uint32_t flags,
                    uint32_t offset);

int32_t sps26ks_read(f3s_dbase_t *dbase,
                    f3s_access_t *access,
                    uint32_t flags,
                    uint32_t offset,
                    int32_t buffer_size,
                    uint8_t *buffer);

int32_t sps26ks_write(f3s_dbase_t * dbase, f3s_access_t * access,
                      _uint32 flags, _uint32 offset,
                      _int32 size, _uint8 * buffer);

int sps26ks_suspend(f3s_dbase_t *dbase, f3s_access_t *access,
                    uint32_t flags, uint32_t offset);

int sps26ks_resume(f3s_dbase_t *dbase, f3s_access_t *access,
                   uint32_t flags, uint32_t offset);

int sps26ks_erase(f3s_dbase_t * dbase, f3s_access_t * access,
                  _uint32 flags, _uint32 offset);

int32_t sps26ks_sync(f3s_dbase_t *dbase, f3s_access_t *access,
                     uint32_t flags, uint32_t offset);

int sps26ks_islock(f3s_dbase_t *dbase, f3s_access_t *access,
                   uint32_t flags, uint32_t offset);

int sps26ks_lock(f3s_dbase_t *dbase, f3s_access_t *access,
                 uint32_t flags, uint32_t offset);

int sps26ks_unlock(f3s_dbase_t *dbase, f3s_access_t *access,
                   uint32_t flags, uint32_t offset);

int sps26ks_unlockall(f3s_dbase_t *dbase, f3s_access_t *access,
                      uint32_t flags, uint32_t offset);

/* common SPI flash calls */
int hyper_nor_write_memory(void *hdl, uint32_t addr, uint8_t *dbuf, int dlen);
int hyper_nor_write_command(void *hdl, uint32_t addr, uint32_t data);
int hyper_nor_sector_erase(void *hdl, uint32_t addr, int sector_size);
int hyper_nor_read_registers(void *hdl, uint32_t addr, uint8_t *buf, int len);
int hyper_nor_read_memory(void *hdl, uint32_t addr, uint8_t *buf, int len);
int hyper_nor_busy_wait(void *hdl, uint32_t tmo);
int qspi_nor_busy_wait(void *hdl, uint32_t tmo);
int qspi_nor_enable_quad(void *hdl, int enable);
int qspi_nor_page_program(void *hdl, uint32_t addr, int alen, uint8_t *buf, int len);
int qspi_nor_read_memory(void *hdl, uint32_t addr, int alen, uint8_t *buf, int len);
int qspi_nor_read_registers(void *hdl, uint8_t cmd, uint32_t arg, int alen, uint8_t *data, int dlen);
int qspi_nor_sector_erase(void *hdl, uint32_t addr, int alen, int sector_size);
int qspi_nor_write_memory(void *hdl, uint8_t cmd, uint32_t arg, int alen, uint8_t *dbuf, int dlen);
int qspi_nor_write(void *hdl, uint8_t cmd, uint32_t arg, int alen, uint8_t *dbuf, int dlen);

struct qspi_flash_params {
    /* Device name ([MANUFLETTER][DEVTYPE][DENSITY][EXTRAINFO]) */
    const char  *name;

    /*
     * This array stores the ID bytes.
     * The first three bytes are the JEDIC ID.
     * JEDEC ID zero means "no ID" (mostly older chips).
     */
    uint8_t      id[SPI_FLASH_MAX_ID_LEN];
    uint8_t      id_len;

    /*
     * The size listed here is what works with SPINOR_OP_SE, which isn't
     * necessarily called a "sector" by the vendor.
     */
    uint32_t     sector_size;
    uint32_t     n_sectors;

    uint16_t     page_size;

    uint16_t     flags;
#define SECT_4K         0  /* CMD_ERASE_4K works uniformly */
#define E_FSR           1  /* use flag status register for */
#define SST_WR          2  /* use SST byte/word programming */
#define WR_QPP          3  /* use Quad Page Program */
#define RD_QUAD         4  /* use Quad Read */
#define RD_DUAL         5  /* use Dual Read */
#define RD_QUADIO       6  /* use Quad IO Read */
#define RD_DUALIO       7  /* use Dual IO Read */
#define RD_FULL         (RD_QUAD | RD_DUAL | RD_QUADIO | RD_DUALIO)
};

/* host/board specifc SPI flash calls */
typedef struct _spi_flash_t {
    void        *hdl;
    uint32_t    dcap;       // flash device capability
    uint32_t    hcap;       // SPI controller capability
    uint32_t    mode;       // current mode
#define SPI_CAP_QUAD        (1 << 0)    // Quad mode supported
#define SPI_CAP_ADDR4       (1 << 1)    // 4 byte address
#define SPI_MODE_QUAD       (1 << 0)    // Quad mode
    int         (*dinit)(void *hdl);
    int         (*spi_read_flash)(void *hdl, uint8_t cmd, uint32_t addr ,int alen, void *data, int dlen);
    int         (*spi_write_flash)(void *hdl, uint8_t cmd, int addr ,int alen , void *data, int dlen);
    int         (*spi_quad_read)(void *hdl, uint8_t cmd, uint8_t addr , uint8_t alen, void *data, int dlen);
    int         (*spi_quad_write)(void *hdl, uint8_t cmd, uint8_t addr , uint8_t alen, void *data, int dlen);
    int         (*spi_hyper_read_registers)(void *hdl, uint32_t addr, uint8_t *dbuf, int dlen);
    int         (*spi_hyper_read_memory)(void *hdl, uint32_t addr, uint8_t *dbuf, int dlen);
    int         (*spi_hyper_write_cmd)(void *hdl, uint32_t addr, uint32_t data);
    int         (*spi_hyper_write_memory)(void *hdl, uint32_t addr, uint8_t *dbuf, int dlen);
} spi_flash_t;

#endif /* __F3S_SPI_H_INCLUDED */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/flash/boards/rcar_qspi/f3s_spi.h $ $Rev: 839522 $")
#endif
