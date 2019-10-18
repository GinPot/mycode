/*
 * $QNXLicenseC:
 * Copyright 2009, QNX Software Systems.
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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/resmgr.h>
#include <sys/neutrino.h>
#include <hw/inout.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>
#include <errno.h>
#include <sys/procmgr.h>
#include <drvr/hwinfo.h>

#define MAX_NUM_REG		5
#define DEFAULT_REG_LEN	0x1000

void (*write_wdt)(uintptr_t, uint32_t);
uint32_t (*read_wdt)(uintptr_t);

void write_wdt_32(uintptr_t addr, uint32_t data) {
	out32(addr, data);
}

void write_wdt_16(uintptr_t addr, uint32_t data) {
	out16(addr, (uint16_t)data);
}

void write_wdt_8(uintptr_t addr, uint32_t data) {
	out8(addr, (uint8_t)data);
}

uint32_t read_wdt_32(uintptr_t addr) {
	return in32(addr);
}

uint32_t read_wdt_16(uintptr_t addr) {
	return (uint32_t)in16(addr);
}

uint32_t read_wdt_8(uintptr_t addr) {
	return (uint32_t)in8(addr);
}

typedef enum {
	BITSWAP,
	WRITE,
	NUM_OPERATIONS
} operation_id_t;

typedef enum {
	REG_OFFSET,
	REG_VAL
} reg_pairs_t;

int isValidRange(uint32_t val, int err)
{
    if(val != 0 && err == ERANGE) {
        return 0;
    }
    return 1;
}

int isValidSeparator(char separator) {
    if(separator == ':') {
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
	int opt;
	int priority = 10;	/*default priority:default 10 */
	int time = -1;		/*default time for watchdog timer kick*/
	uintptr_t base = 0;
	size_t len = DEFAULT_REG_LEN;
	paddr_t physbase = 0;
	hwiattr_timer_t wdog_attr;
	unsigned hwi_off;
	int r;
	struct sched_param threadparam;
	int sched_type;
	uint32_t write_width = 32;
	uint32_t enable_width = 32;
	uint32_t enable_mask = 0;
	uint32_t enable_condition = 0;
	uint32_t enable_reg_offset = 0;
	uint32_t write_reg[MAX_NUM_REG][2];
	uint32_t bitswap_value;
	uint32_t num_writes = 0;
	hwi_tag  *tag;
	unsigned  tag_idx;
	int operation = -1;
	int verbose =0;
	int idx;

	/* Getting the WDOG Base addresss from the Hwinfo Section if available */
	hwi_off = hwi_find_device("wdog", 0);
	if (hwi_off != HWI_NULL_OFF) {
		r = hwiattr_get_timer(hwi_off, &wdog_attr);
		if (r == EOK) {
			if(wdog_attr.common.location.len > len) {
				len = wdog_attr.common.location.len;
			}
			if(wdog_attr.common.location.base > 0)
				physbase = wdog_attr.common.location.base;
			time = hwitag_find_clkfreq(hwi_off, NULL);
		}
	}

	/* Getting the WDOG options from the Hwinfo Section if available */
	hwi_off = hwi_find_device("wdt,options", 0);
	if (hwi_off != HWI_NULL_OFF) {
		tag_idx = 0;
		while(((tag = hwi_tag_find(hwi_off, HWI_TAG_NAME_regname, &tag_idx)) != NULL)) {
			if(strcmp("write_width",  __hwi_find_string(tag->regname.regname)) == 0) {
				write_width = tag->regname.offset;
			} else if(strcmp("enable_width",  __hwi_find_string(tag->regname.regname)) == 0) {
				enable_width = tag->regname.offset;
			} else if(strcmp("enable_mask",  __hwi_find_string(tag->regname.regname)) == 0) {
				enable_mask = tag->regname.offset;
			} else if(strcmp("enable_condition",  __hwi_find_string(tag->regname.regname)) == 0) {
				enable_condition = tag->regname.offset;
			} else if(strcmp("enable_offset",  __hwi_find_string(tag->regname.regname)) == 0) {
				enable_reg_offset = tag->regname.offset;
				if(enable_reg_offset > len) {
					len = enable_reg_offset;
				}
			} else {
				slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick error : %s is not a valid Hwinfo default setting.", \
					__hwi_find_string(tag->regname.regname));
				break;
			}
		}
	}

	if ((hwi_off = hwi_find_device("wdt,bitswap", 0)) != HWI_NULL_OFF) {
		operation = BITSWAP;
	} else if ((hwi_off = hwi_find_device("wdt,regwrite", 0)) != HWI_NULL_OFF) {
		operation = WRITE;
	}

	tag_idx = 0;
	while((tag = hwi_tag_find(hwi_off, HWI_TAG_NAME_regname, &tag_idx)) != NULL) {
		if(num_writes >= MAX_NUM_REG) {
			slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick error : %s only supports up to %d kick registers. Please check "
				"the command line or Hwinfo default setting.", argv[0], MAX_NUM_REG);
			return EXIT_FAILURE;
		}
		if(strcmp("offset", __hwi_find_string(tag->regname.regname)) == 0) {
next_offset:
			write_reg[num_writes][REG_OFFSET] = tag->regname.offset;
			if(write_reg[num_writes][REG_OFFSET] > len) {
				len = write_reg[num_writes][REG_OFFSET];
			}
			if((tag = hwi_tag_find(hwi_off, HWI_TAG_NAME_regname, &tag_idx)) != NULL) {
				if(strcmp("value",  __hwi_find_string(tag->regname.regname)) == 0) {
					write_reg[num_writes][REG_VAL] = tag->regname.offset;
				} else if(strcmp("offset",  __hwi_find_string(tag->regname.regname)) == 0) {
					if(operation == BITSWAP) {
						//Bitswapping should default to 0xFFFFFFFF
						write_reg[num_writes][REG_VAL] = 0xFFFFFFFF;
						num_writes++;

						if(num_writes >= MAX_NUM_REG) {
							slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick error : %s only supports up to %d kick registers. Please check "
								"the command line or Hwinfo default setting.", argv[0], MAX_NUM_REG);
							return EXIT_FAILURE;
						} else {
							//This is a new offset entry
							goto next_offset;
						}
					} else {
						slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick error : Missing value for corresponding offset. "
							"Please check the command line or Hwinfo default setting.");
						return EXIT_FAILURE;
					}
				} else {
					slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick error : Missing value for corresponding offset. "
						"Please check the command line or Hwinfo default setting.");
					return EXIT_FAILURE;
				}
			} else {
				if(operation == BITSWAP) {
					//Bitswapping should default to 0xFFFFFFFF
					write_reg[num_writes][REG_VAL] = 0xFFFFFFFF;

					//This is the last element in the table, so break
					num_writes++;
					break;
				} else {
					slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick error : Missing value for corresponding offset. "
						"Please check the command line or Hwinfo default setting.");
					return EXIT_FAILURE;
				}
			}
		} else if (strcmp("value", __hwi_find_string(tag->regname.regname)) == 0) {
			write_reg[num_writes][REG_VAL] = tag->regname.offset;
			if((tag = hwi_tag_find(hwi_off, HWI_TAG_NAME_regname, &tag_idx)) != NULL) {
				if(strcmp("offset",  __hwi_find_string(tag->regname.regname)) == 0) {
					write_reg[num_writes][REG_OFFSET] = tag->regname.offset;
					if(write_reg[num_writes][REG_OFFSET] > len) {
						len = write_reg[num_writes][REG_OFFSET];
					}
				} else {
					slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick error : Missing offset for corresponding value. "
						"Please check the command line or Hwinfo default setting.");
					return EXIT_FAILURE;
				}
			} else {
				slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick error : Missing offset for corresponding value. "
					"Please check the command line or Hwinfo default setting.");
				return EXIT_FAILURE;
			}
		} else {
			slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick error : Invalid offset/value entry. Please check the command line or Hwinfo default setting.");
			return EXIT_FAILURE;
		}
		num_writes++;
	}

	/* Process dash options.*/
	while ((opt = getopt(argc, argv, "a:l:p:t:w:r:m:R:E:B:W:v")) != -1) {
		switch (opt) {
			case 'a':	// WDOG register physical base address
				physbase = strtoull(optarg, NULL, 0);
				break;
			case 'l':
				len = strtoul(optarg, NULL, 0);
				break;
			case 'p':	// priority
				priority = strtoul(optarg, NULL, 0);
				break;
			case 't':
				time = strtoul(optarg, NULL, 0);
				break;
			case 'w':
				write_width = strtoul(optarg, NULL, 0);
				break;
			case 'r':
				enable_width = strtoul(optarg, NULL, 0);
				break;
			case 'E':
				enable_reg_offset = strtol(optarg, &optarg, 16);
				if(enable_reg_offset > len) {
					len = enable_reg_offset;
				}
				if(!isValidRange(enable_reg_offset, errno)) {
					slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick error : Enable offset is out of range. Please check "
						"the command line or Hwinfo default setting.");
					return EXIT_FAILURE;
				}
				if(!isValidSeparator(optarg[0])) {
					slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick error : Invalid separator for option '-E'. Please check "
						"the command line or Hwinfo default setting.");
					return EXIT_FAILURE;
				}

				if(optarg[1] == 0) {
					slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick error : Invalid args for option '-E'. Please check "
						"the command line or Hwinfo default setting.");
					return EXIT_FAILURE;
				}

				enable_mask = strtoul(optarg + 1, &optarg, 16);
				if(!isValidRange(enable_mask, errno)) {
					slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick error : Enable mask is out of range. Please check "
						"the command line or Hwinfo default setting.");
					return EXIT_FAILURE;
				}

				if(optarg[0] == 0) {
					enable_condition = enable_mask; //If the enable condition is not present, set it to enable_mask
				} else if(!isValidSeparator(optarg[0])) {
					slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick error : Invalid separator for option '-E'. Please check "
						"the command line or Hwinfo default setting.");
					return EXIT_FAILURE;
				} else {
					enable_condition = strtoul(optarg + 1, NULL, 16);
					if(enable_condition != 0) {
						if(!isValidRange(enable_condition, errno)) {
							slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick error : Enable condition is out of range. Please check "
								"the command line or Hwinfo default setting.");
							return EXIT_FAILURE;
						}
					} else {
						enable_condition = enable_mask; //If the enable condition is not present, set it to enable_mask
					}
				}
				break;
			case 'B':
				if(num_writes >= MAX_NUM_REG) {
					slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick error : %s only supports up to %d kick registers. Please check "
						"the command line or Hwinfo default setting.", argv[0], MAX_NUM_REG);
					return EXIT_FAILURE;
				}

				write_reg[num_writes][REG_OFFSET] = strtol(optarg, &optarg, 16);
				if(!isValidRange(write_reg[num_writes][REG_OFFSET], errno)) {
					slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick error : Offset to bitswap is out of range. Please check "
						"the command line or Hwinfo default setting.");
					return EXIT_FAILURE;
				}

				if(write_reg[num_writes][REG_OFFSET] > len) {
					len = write_reg[num_writes][REG_OFFSET];
				}

				if(optarg[0] != 0 && !isValidSeparator(optarg[0])) {
					slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick error : Invalid separator for option '-B'. Please check "
						"the command line or Hwinfo default setting.");
					return EXIT_FAILURE;
				}

				bitswap_value = strtoul(optarg + 1, NULL, 16);
				if(bitswap_value != 0) {
					if(!isValidRange(bitswap_value, errno)) {
						slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick error : Value to bitswap is out of range. Please check "
							"the command line or Hwinfo default setting.");
						return EXIT_FAILURE;
					}
					write_reg[num_writes][REG_VAL] = bitswap_value;
				} else {
					//Default to 0xFFFFFFFF for bitswap if no value entered
					write_reg[num_writes][REG_VAL] = 0xFFFFFFFF;
				}
				num_writes++;
				operation = BITSWAP;
				break;
			case 'W':
				if(num_writes >= MAX_NUM_REG) {
					slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick error : %s only supports up to %d kick registers. Please check "
						"the command line or Hwinfo default setting.", argv[0], MAX_NUM_REG);
					return EXIT_FAILURE;
				}

				write_reg[num_writes][REG_OFFSET] = strtol(optarg, &optarg, 16);
				if(!isValidRange(write_reg[num_writes][REG_OFFSET], errno)) {
					slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick error : Offset to write is out of range. Please check "
						"the command line or Hwinfo default setting.");
					return EXIT_FAILURE;
				}

				if(write_reg[num_writes][REG_OFFSET] > len) {
					len = write_reg[num_writes][REG_OFFSET];
				}

				if(!isValidSeparator(optarg[0])) {
					slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick error : Invalid separator for option '-W'. Please check "
						"the command line or Hwinfo default setting.");
					return EXIT_FAILURE;
				}

				write_reg[num_writes][REG_VAL] = strtoul(optarg + 1, NULL, 16);
				if(!isValidRange(write_reg[num_writes][REG_VAL], errno)) {
					slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick error : Value to write is out of range. Please check "
						"the command line or Hwinfo default setting.");
					return EXIT_FAILURE;
				}

				num_writes++;
				operation = WRITE;
				break;
			case 'v':
				verbose++;
				break;
		}
	}

	/*check if the params are valid*/
	if (physbase == 0) {
		slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick error : Invalid  WDOG register physical base address. Please check the command line or Hwinfo default setting.");
		return EXIT_FAILURE;
	}
	if (time == -1) {
		slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick error : Invalid default time for watchdog timer kick. Please check the command line or Hwinfo default setting.");
		return EXIT_FAILURE;
	}
	if (operation == -1) {
		slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick error : Invalid operation for watchdog timer kick. Please check the command line or Hwinfo default setting.");
		return EXIT_FAILURE;
	}
	if (num_writes == 0) {
		slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick error : No registers specified for watchdog timer kick. Please check the command line or Hwinfo default setting.");
		return EXIT_FAILURE;
	}

	switch(write_width) {
		case 32:
			write_wdt = &write_wdt_32;
			break;
		case 16:
			write_wdt = &write_wdt_16;
			break;
		case 8:
			write_wdt = &write_wdt_8;
			break;
		default:
			slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick: Invalid register write_width (%d) for watchdog timer kick. Please check the command line or Hwinfo default setting.", write_width);
			return EXIT_FAILURE;
	}

	switch(enable_width) {
		case 32:
			read_wdt = &read_wdt_32;
			break;
		case 16:
			read_wdt = &read_wdt_16;
			break;
		case 8:
			read_wdt = &read_wdt_8;
			break;
		default:
			slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick: Invalid register enable_width (%d) for watchdog timer kick. Please check the command line or Hwinfo default setting.", enable_width);
			return EXIT_FAILURE;
	}

	// Enable IO capability.
	if ( ThreadCtl( _NTO_TCTL_IO, NULL ) == -1 ) {
		slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick: failure to acquire IO privity");
		return EXIT_FAILURE;
	}
	//run in the background
	if ( procmgr_daemon( EXIT_SUCCESS, PROCMGR_DAEMON_NOCLOSE | PROCMGR_DAEMON_NODEVNULL ) == -1 ) {
		slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"%s:  procmgr_daemon",argv[0]);
		return EXIT_FAILURE;
	}

	// configure information
	if (verbose) {
		slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "wdtkick: paddr = 0x%zx, size = 0x%zx, kick = %d, priority = %d",
			  physbase, len, time, priority);
	}

	// Set priority
	if (pthread_getschedparam(pthread_self(),&sched_type, &threadparam) != EOK){
		slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_WARNING,"wdtkick: get priority request failed");
		return EXIT_FAILURE;
	}
	if (priority != threadparam.sched_priority){
		threadparam.sched_priority = priority;
		if (pthread_setschedparam(pthread_self(),sched_type, &threadparam) != EOK) {
			slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_WARNING,"wdtkick: can't change priority");
		}
	}

	base = mmap_device_io(len, physbase);
	if (base == MAP_DEVICE_FAILED) {
		slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick: failed to map watchdog timer registers");
		return EXIT_FAILURE;
	}

	slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick: enable mask: 0x%x", enable_mask);

	// Configure the enable register
	if (enable_mask && ((read_wdt(base + enable_reg_offset) & enable_mask) != enable_mask)) {
		slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick: enabling the watchdog timer");
		write_wdt(base + enable_reg_offset, read_wdt(base + enable_reg_offset) | enable_mask);
	}

	while (1) {
		for(idx = 0; idx < num_writes; idx ++) {
			if (operation == BITSWAP) {
				write_wdt(base + write_reg[idx][REG_OFFSET], read_wdt(base + write_reg[idx][REG_OFFSET]) ^ write_reg[idx][REG_VAL]);
			} else if (operation == WRITE) {
				write_wdt(base + write_reg[idx][REG_OFFSET], write_reg[idx][REG_VAL]);
			}
		}
		delay(time);
	}

	return EXIT_SUCCESS;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/hardware/support/wdtkick/main.c $ $Rev: 845551 $")
#endif
