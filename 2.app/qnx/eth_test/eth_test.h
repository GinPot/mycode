#ifndef ETH_TEST_H
#define ETH_TEST_H

#include <pthread.h>
#include <errno.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/asoundlib.h>
#include <gulliver.h>
#include <ctype.h>
#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/slog.h>
#include <sys/neutrino.h>
#include <sys/mman.h>
#include <stdarg.h>

#define DEVNP_ETH_PHY_LINK_STA_RD_CMD 	63
#define DEVNP_ETH_PHY_SQI_RD_CMD 	64
#define DEVNP_ETH_PHY_CLK_MODE_SET_CMD 	65
#define DEVNP_ETH_PHY_CABLE_STA_RD_CMD 	66
#define DEVNP_ETH_PHY_TST_MODE_SET_CMD 	67
#define DEVNP_ETH_PHY_SLPWAK_STA_RD_CMD 68
#define DEVNP_ETH_PHY_ID_RD_CMD 	69
#define DEVNP_ETH_PHY_SWITCH_ID_RD_CMD 	70
#define DEVNP_ETH_PHY_SET_CLK_MODE 	71
#define DEVNP_ETH_PHY_GET_CLK_MODE 	72
#define DEVNP_ETH_PHY_SET_TST_MODE 	73
#define DEVNP_ETH_PHY_GET_TST_MODE 	74
#define DEVNP_ETH_PHY_START_VCT_TST 	75
#define DEVNP_ETH_PHY_GET_VCT_CPLT_FLAG 76
#define DEVNP_ETH_PHY_GET_CABLE_STA 	77
#define DEVNP_ETH_PHY_EN_LPSD_CTRL 	78
#define DEVNP_ETH_PHY_GET_LPSD_EN_STA 	79
#define DEVNP_ETH_PHY_DOWNGRADE2A0	80
#define DEVNP_ETH_PHY_READ_REG	81
#define DEVNP_ETH_PHY_WRITE_REG	82
#define DEVNP_ETH_PHY_INIT	83

struct eth_info_t
{
	struct ifdrv ifd;
	int data;
};


#endif
