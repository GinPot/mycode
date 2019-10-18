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


#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/mman.h>

#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <zlib.h>

#include <hw/rcarh3_uboot_env.h>

#define CONFIG_ENV_SECT_SIZE    (128 * 1024)
#define CONFIG_ENV_OFFSET       (-CONFIG_ENV_SIZE)
#define CONFIG_ENV_SIZE         (CONFIG_ENV_SECT_SIZE)

#define ETHER_ADDR_LEN 6

static char *fw_getenv  (char *name);
static int fw_env_open(char*);
static int fw_env_close(void);

#define WHITESPACE(c) ((c == '\t') || (c == ' '))

static int fd;

struct environment {
    uint32_t            *crc;
    char                *data;
};

static struct environment environment;

static char *envmatch (char * s1, char * s2);

/*
 * Search the environment for a variable.
 * Return the value, if found, or NULL, if not found.
 */
static char *fw_getenv (char *name)
{
    char *env, *nxt;

    for (env = environment.data; *env; env = nxt + 1) {
        char *val;

        for (nxt = env; *nxt; ++nxt) {
            if (nxt >= &environment.data[CONFIG_ENV_SIZE]) {
                fprintf (stderr, "## Error: "
                    "environment not terminated\n");
                return NULL;
            }
        }
        val = envmatch (name, env);
        if (!val)
            continue;
        return val;
    }
    return NULL;
}

static int fw_env_close(void)
{
    close(fd);
    free(environment.data - 4);
    return 0;
}

/*
 * s1 is either a simple 'name', or a 'name=value' pair.
 * s2 is a 'name=value' pair.
 * If the names match, return the value of s2, else NULL.
 */

static char *envmatch (char * s1, char * s2)
{
    if (s1 == NULL || s2 == NULL)
        return NULL;

    while (*s1 == *s2++)
        if (*s1++ == '=')
            return s2;
    if (*s1 == '\0' && *(s2 - 1) == '=')
        return s2;
    return NULL;
}

/*
 * Prevent confusion if running from erased flash memory
 */
static int fw_env_open(char* filename)
{
    char *addr0;
    int ret;
    int size;
    uint32_t crc;

    addr0 = calloc(1, CONFIG_ENV_SIZE);
    if (addr0 == NULL) {
        fprintf(stderr,
            "Not enough memory for environment (%d bytes)\n",
            CONFIG_ENV_SIZE);
        return -1;
    }

    /* read environment from FLASH to local buffer */
    if ((fd = open(filename, O_RDONLY)) == -1)
    {
        free(addr0);
        return (-1);
    }
    size = 0;
    lseek(fd, CONFIG_ENV_OFFSET, SEEK_END);
    do {
        if( (ret = read(fd, addr0, CONFIG_ENV_SIZE)) == -1 ) {
            fprintf(stderr, "Error reading environment: %s", strerror(errno));
            free(addr0);
            return -1;
        }
        size += ret;
    } while ( size < CONFIG_ENV_SIZE );

    environment.crc     = (uint32_t *)addr0;
    environment.data    = addr0 + 4;

    crc = crc32(0, (const unsigned char *)environment.data, CONFIG_ENV_SIZE - 4);

    if( crc != *environment.crc ) {
        fprintf(stderr, "CRC not correct - invalid or corrupt environment\n");
        fprintf(stderr, "CRC read:       0x%08x\n", *environment.crc);
        fprintf(stderr, "CRC calculated: 0x%08x\n", crc);
        free( addr0 );
        errno = EIO;
        return -1;
    }

    return 0;
}

char* uboot_env_read (char* env_var, char* filename)
{
    char *result, *return_buf;
    unsigned int env_len;

    if(env_var == NULL)
        return NULL;

    if(fw_env_open(filename) != 0) {
        fprintf(stderr, "Open %s failed: %s\n", filename, strerror(errno));
        return NULL;
    }

    result = fw_getenv(env_var);
    if(result == NULL) {
        fprintf(stderr, "Cannot find variable [%s] in U-Boot environment at %s\n", env_var, filename);
        fw_env_close();
        return NULL;
    }

    env_len = strlen(result); /* Return character is not counted */
    return_buf = calloc(1, env_len + 1);
    if(return_buf == NULL) {
        fprintf(stderr, "Cannot allocate memory for environment variable: %s\n", strerror(errno));
        fw_env_close();
        return NULL;
    }

    memcpy(return_buf, result, env_len);
    return_buf[env_len] = '\0';

    fw_env_close();

    /* Return a string with return character terminated */
    return return_buf;
}

void uboot_env_free (char* ptr)
{
    free(ptr);
}


int main(int argc, char *argv[])
{
    char *s;
    if(argc == 3) {
        s = uboot_env_read(argv[2], argv[1]);
        if(!s)
            return EXIT_FAILURE;

        printf("%s=%s\n", argv[2], s);

        uboot_env_free(s);
    }
    else {
        fprintf(stderr, "Please see the usage information.\n");
    }

    return EXIT_SUCCESS;

}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/support/rcar-uboot-env/main.c $ $Rev: 843624 $")
#endif
