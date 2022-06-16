#include <stdlib.h>
#include <stdio.h>
#include <devctl.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <sys/procfs.h>
#include <sys/procmgr.h>
#include <sys/types.h>
#include <signal.h>

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <hw/inout.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdio.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_ether.h>
#include <net/if_types.h>
#include <errno.h>
#include <sys/iomsg.h>
#include <errno.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>

#include "eth_test.h"

#define  ETH_NAME  "eth0"

static int fd = 0;

/* switchid:0x0A51 */
static void get_switchid(int port)
{
	struct eth_info_t eth_inf = {0};
	
	strncpy(eth_inf.ifd.ifd_name, ETH_NAME, sizeof(eth_inf.ifd.ifd_name) -1);
	
	eth_inf.ifd.ifd_cmd = DEVNP_ETH_PHY_SWITCH_ID_RD_CMD;
	eth_inf.ifd.ifd_len = sizeof(int);
	eth_inf.ifd.ifd_data = NULL;
	eth_inf.data = port << 16;

	if(devctl(fd, SIOCGDRVSPEC,(void *) &eth_inf,sizeof(struct eth_info_t),NULL) == -1)
	{
		printf("ETH link failed\r\n");
	}
	else
	{
		/* high 16 bits is port num, low 16 bits is switchid */
		printf("ETH:switchid=%08x\n", eth_inf.data);
	}
}

/* phyId: 0x002B02*/
static void get_phyId(int port)
{
	struct eth_info_t eth_inf = {0};
	
	strncpy(eth_inf.ifd.ifd_name, ETH_NAME, sizeof(eth_inf.ifd.ifd_name) -1);
	
	eth_inf.ifd.ifd_cmd = DEVNP_ETH_PHY_ID_RD_CMD;
	eth_inf.ifd.ifd_len = sizeof(int);
	eth_inf.ifd.ifd_data = NULL;
	eth_inf.data = port << 24;

	if(devctl(fd, SIOCGDRVSPEC,(void *) &eth_inf,sizeof(struct eth_info_t),NULL) == -1)
	{
		printf("ETH link failed\r\n");
	}
	else
	{
		/* high 8 bits is port num, low 24 bits is phyId */
		printf("ETH:phyId=%08x\n", eth_inf.data);
	}
}

/* sqi level: 0~15, if you want use level 0~7, please use ((eth_inf.data) & 0xffff)) >> 1 */
static void get_sqi_level16(int port)
{
	struct eth_info_t eth_inf = {0};
	
	strncpy(eth_inf.ifd.ifd_name, ETH_NAME, sizeof(eth_inf.ifd.ifd_name) -1);
	
	eth_inf.ifd.ifd_cmd = DEVNP_ETH_PHY_SQI_RD_CMD;
	eth_inf.ifd.ifd_len = sizeof(int);
	eth_inf.ifd.ifd_data = NULL;
	eth_inf.data = port << 16;

	if(devctl(fd, SIOCGDRVSPEC,(void *) &eth_inf,sizeof(struct eth_info_t),NULL) == -1)
	{
		printf("ETH link failed\r\n");
	}
	else
	{
		printf("ETH:%08x\n", eth_inf.data);
	}
}

/* 0xFF fault or read error, 1 link up, 0 link down */
void get_link_status(int port)
{
	struct eth_info_t eth_inf = {0};
	
	strncpy(eth_inf.ifd.ifd_name, ETH_NAME, sizeof(eth_inf.ifd.ifd_name) -1);
	
	eth_inf.ifd.ifd_cmd = DEVNP_ETH_PHY_LINK_STA_RD_CMD;
	eth_inf.ifd.ifd_len = sizeof(int);
	eth_inf.ifd.ifd_data = NULL;
	eth_inf.data = port << 16;

	if(devctl(fd, SIOCGDRVSPEC,(void *) &eth_inf,sizeof(struct eth_info_t),NULL) == -1)
	{
		printf("ETH link failed\r\n");
	}
	else
	{
		printf("ETH:%08x\n", eth_inf.data);
	}
}

/* 
*  0=good cable
*  't'=terminated, 116		0x74
*  'o'=open, 111			0x6F
*  's'=short, 115			0x73
*  0xFE polarity is negative, distance is 0xFF, it means the length is
*       (0xFF-16) * 1.5 meters.
*  0xFF read failed. 
*/
static void get_cable_status(int port)
{
	struct eth_info_t eth_inf = {0};
	
	strncpy(eth_inf.ifd.ifd_name, ETH_NAME, sizeof(eth_inf.ifd.ifd_name) -1);
	
	eth_inf.ifd.ifd_cmd = DEVNP_ETH_PHY_CABLE_STA_RD_CMD;
	eth_inf.ifd.ifd_len = sizeof(int);
	eth_inf.ifd.ifd_data = NULL;
	eth_inf.data = port << 16;

	if(devctl(fd, __DION(_DCMD_IP, SIOCGDRVSPEC),(void *) &eth_inf,sizeof(struct eth_info_t),NULL) == -1)
	{
		printf("ETH link failed\r\n");
	}
	else
	{
		printf("222ETH:%08x\n", eth_inf.data);
	}
}

/* start vct test for cable status detection */
void start_vct_tst(int port)
{
	struct eth_info_t eth_inf = {0};
	
	strncpy(eth_inf.ifd.ifd_name, ETH_NAME, sizeof(eth_inf.ifd.ifd_name) -1);
	
	eth_inf.ifd.ifd_cmd = DEVNP_ETH_PHY_START_VCT_TST;
	eth_inf.ifd.ifd_len = sizeof(int);
	eth_inf.ifd.ifd_data = NULL;
	eth_inf.data = port << 16;

	if(devctl(fd, SIOCSDRVSPEC,(void *) &eth_inf,sizeof(struct eth_info_t),NULL) == -1)
	{
		printf("ETH link failed\r\n");
	}
	else
	{
		/* high 8 bits is port num*/
		printf("ETH:%08x\n", eth_inf.data);
	}
}

/* inspect whether vct test completed or not 
*  1->completed, 0->not start or in progress
*/
void get_vct_cplt_flag(int port)
{
	struct eth_info_t eth_inf = {0};
	
	strncpy(eth_inf.ifd.ifd_name, ETH_NAME, sizeof(eth_inf.ifd.ifd_name) -1);
	
	eth_inf.ifd.ifd_cmd = DEVNP_ETH_PHY_GET_VCT_CPLT_FLAG;
	eth_inf.ifd.ifd_len = sizeof(int);
	eth_inf.ifd.ifd_data = NULL;
	eth_inf.data = port << 16;

	if(devctl(fd, SIOCGDRVSPEC,(void *) &eth_inf,sizeof(struct eth_info_t),NULL) == -1)
	{
		printf("ETH link failed\r\n");
	}
	else
	{
		printf("ETH:%08x\n", eth_inf.data);
	}
}

/*
* Low 8-bit of return data:
*  0=good cable
*  't'=terminated, 116
*  'o'=open, 111
*  's'=short, 115
*  0xFE polarity is negative, distance is 0xFF, it means the length is
*       (0xFF-16) * 1.5 meters.
*  0xFF read failed. 
*/
void get_cable_sta(int port)
{
	struct eth_info_t eth_inf = {0};
	
	strncpy(eth_inf.ifd.ifd_name, ETH_NAME, sizeof(eth_inf.ifd.ifd_name) -1);
	
	eth_inf.ifd.ifd_cmd = DEVNP_ETH_PHY_GET_CABLE_STA;
	eth_inf.ifd.ifd_len = sizeof(int);
	eth_inf.ifd.ifd_data = NULL;
	eth_inf.data = port << 16;

	if(devctl(fd, SIOCGDRVSPEC,(void *) &eth_inf,sizeof(struct eth_info_t),NULL) == -1)
	{
		printf("ETH link failed\r\n");
	}
	else
	{
		printf("ETH:%08x\n", eth_inf.data);
	}
}

/* 
*  1 lpsd local wakeup
*  2 lpsd remote wakeup
*  3 lpsd local wakeup & lpsd remote wakeup
*  0xFE lpsd is not enabled, it means the phy work in normal mode
*  0xFF Fault, read failed
*/
static void get_sleepwakeup_status(int port)
{
	struct eth_info_t eth_inf = {0};
	
	strncpy(eth_inf.ifd.ifd_name, ETH_NAME, sizeof(eth_inf.ifd.ifd_name) -1);
	
	eth_inf.ifd.ifd_cmd = DEVNP_ETH_PHY_SLPWAK_STA_RD_CMD;
	eth_inf.ifd.ifd_len = sizeof(int);
	eth_inf.ifd.ifd_data = NULL;
	eth_inf.data = port << 16;

	if(devctl(fd, SIOCGDRVSPEC,(void *) &eth_inf,sizeof(struct eth_info_t),NULL) == -1)
	{
		printf("ETH link failed\r\n");
	}
	else
	{
		printf("ETH:%08x\n", eth_inf.data);
	}
}

/*
* low power singal detect function enable/disable
*/
static void set_lpsd_en_ctl(int port, int en)
{
	struct eth_info_t eth_inf = {0};
	
	strncpy(eth_inf.ifd.ifd_name, ETH_NAME, sizeof(eth_inf.ifd.ifd_name) -1);
	
	eth_inf.ifd.ifd_cmd = DEVNP_ETH_PHY_EN_LPSD_CTRL;
	eth_inf.ifd.ifd_len = sizeof(int);
	eth_inf.ifd.ifd_data = NULL;
	eth_inf.data = (port << 16) | en;

	if(devctl(fd, SIOCSDRVSPEC,(void *) &eth_inf,sizeof(struct eth_info_t),NULL) == -1)
	{
		printf("ETH link failed\r\n");
	}
	else
	{
		/* high 8 bits is port num, low 8 bits is en value: 0/1 */
		printf("ETH:%08x\n", eth_inf.data);
	}
}

/* 1 enabled, 0 disabled */
static void get_lpsd_en_status(int port)
{
	struct eth_info_t eth_inf = {0};
	
	strncpy(eth_inf.ifd.ifd_name, ETH_NAME, sizeof(eth_inf.ifd.ifd_name) -1);
	
	eth_inf.ifd.ifd_cmd = DEVNP_ETH_PHY_GET_LPSD_EN_STA;
	eth_inf.ifd.ifd_len = sizeof(int);
	eth_inf.ifd.ifd_data = NULL;
	eth_inf.data = port << 16;

	if(devctl(fd, SIOCGDRVSPEC,(void *) &eth_inf,sizeof(struct eth_info_t),NULL) == -1)
	{
		printf("ETH link failed\r\n");
	}
	else
	{
		/* high 8 bits is port num, low 8 bits is en status */
		printf("ETH:%08x\n", eth_inf.data);
	}
}

/*
* testmode:0 normal, 1 amplitude, 2 period, 3 reserved, 4 distortion,
*          5 spectral effiency.
*/
void set_testmode(int port, int mode)
{
	struct eth_info_t eth_inf = {0};
	
	strncpy(eth_inf.ifd.ifd_name, ETH_NAME, sizeof(eth_inf.ifd.ifd_name) -1);
	
	eth_inf.ifd.ifd_cmd = DEVNP_ETH_PHY_TST_MODE_SET_CMD;
	eth_inf.ifd.ifd_len = sizeof(int);
	eth_inf.ifd.ifd_data = NULL;
	eth_inf.data = (port << 16) | mode;

	if(devctl(fd, SIOCSDRVSPEC,(void *) &eth_inf,sizeof(struct eth_info_t),NULL) == -1)
	{
		printf("ETH link failed\r\n");
	}
	else
	{
		sleep(1);
		printf("ETH:%08x\n", eth_inf.data);
	}
}

void downgrade2A0(int port)
{
	struct eth_info_t eth_inf = {0};
	
	strncpy(eth_inf.ifd.ifd_name, ETH_NAME, sizeof(eth_inf.ifd.ifd_name) -1);
	
	eth_inf.ifd.ifd_cmd = DEVNP_ETH_PHY_DOWNGRADE2A0;
	eth_inf.ifd.ifd_len = sizeof(int);
	eth_inf.ifd.ifd_data = NULL;
	eth_inf.data = port << 16;

	if(devctl(fd, SIOCGDRVSPEC,(void *) &eth_inf,sizeof(struct eth_info_t),NULL) == -1)
	{
		printf("ETH link failed\r\n");
	}
	else
	{
		sleep(1);
		printf("ETH:%08x\n", eth_inf.data);
	}
}

void init(int port)
{
	struct eth_info_t eth_inf = {0};
	
	strncpy(eth_inf.ifd.ifd_name, ETH_NAME, sizeof(eth_inf.ifd.ifd_name) -1);
	
	eth_inf.ifd.ifd_cmd = DEVNP_ETH_PHY_INIT;
	eth_inf.ifd.ifd_len = sizeof(int);
	eth_inf.ifd.ifd_data = NULL;
	eth_inf.data = port << 16;

	if(devctl(fd, SIOCGDRVSPEC,(void *) &eth_inf,sizeof(struct eth_info_t),NULL) == -1)
		printf("ETH link failed\r\n");
	else
		printf("Init port %d successful.\n", port);
}

/*
* testmode:0 normal, 1 amplitude, 2 period, 3 reserved, 4 distortion,
*          5 spectral effiency.
*/
void set_tst_mode(int port, int mode)
{
	struct eth_info_t eth_inf = {0};
	
	strncpy(eth_inf.ifd.ifd_name, ETH_NAME, sizeof(eth_inf.ifd.ifd_name) -1);
	
	eth_inf.ifd.ifd_cmd = DEVNP_ETH_PHY_SET_TST_MODE;
	eth_inf.ifd.ifd_len = sizeof(int);
	eth_inf.ifd.ifd_data = NULL;
	eth_inf.data = (port << 16) | mode;

	if(devctl(fd, SIOCSDRVSPEC,(void *) &eth_inf,sizeof(struct eth_info_t),NULL) == -1)
	{
		printf("ETH link failed\r\n");
	}
	else
	{
		sleep(1);
		/* high 8 bits is port num, low 8 bits is testmode */
		printf("ETH:%08x\n", eth_inf.data);
	}
}

/*
* testmode:0 normal, 1 amplitude, 2 period, 3 reserved, 4 distortion,
*          5 spectral effiency.
*/
void get_tst_mode(int port)
{
	struct eth_info_t eth_inf = {0};
	
	strncpy(eth_inf.ifd.ifd_name, ETH_NAME, sizeof(eth_inf.ifd.ifd_name) -1);
	
	eth_inf.ifd.ifd_cmd = DEVNP_ETH_PHY_GET_TST_MODE;
	eth_inf.ifd.ifd_len = sizeof(int);
	eth_inf.ifd.ifd_data = NULL;
	eth_inf.data = port << 16;

	if(devctl(fd, SIOCGDRVSPEC,(void *) &eth_inf,sizeof(struct eth_info_t),NULL) == -1)
	{
		printf("ETH link failed\r\n");
	}
	else
	{
		/* high 8 bits is port num, low 8 bits is test mode */
		printf("ETH:%08x\n", eth_inf.data);
	}
}

/*
* clockmode: 1 master, 0 slave 
*/
void set_clkmode(int port, int mode)
{
	struct eth_info_t eth_inf = {0};
	
	strncpy(eth_inf.ifd.ifd_name, ETH_NAME, sizeof(eth_inf.ifd.ifd_name) -1);
	
	eth_inf.ifd.ifd_cmd = DEVNP_ETH_PHY_CLK_MODE_SET_CMD;
	eth_inf.ifd.ifd_len = sizeof(int);
	eth_inf.ifd.ifd_data = NULL;
	eth_inf.data = (port << 16) | mode;

	if(devctl(fd, SIOCSDRVSPEC,(void *) &eth_inf,sizeof(struct eth_info_t),NULL) == -1)
	{
		printf("ETH link failed\r\n");
	}
	else
	{
		sleep(1);
		printf("ETH:%08x\n", eth_inf.data);
	}
}

/* 1 master, 0 slave */
void set_clk_mode(int port, int mode)
{
	struct eth_info_t eth_inf = {0};
	
	strncpy(eth_inf.ifd.ifd_name, ETH_NAME, sizeof(eth_inf.ifd.ifd_name) -1);
	
	eth_inf.ifd.ifd_cmd = DEVNP_ETH_PHY_SET_CLK_MODE;
	eth_inf.ifd.ifd_len = sizeof(int);
	eth_inf.ifd.ifd_data = NULL;
	eth_inf.data = (port << 16) | mode;

	if(devctl(fd, SIOCSDRVSPEC,(void *) &eth_inf,sizeof(struct eth_info_t),NULL) == -1)
	{
		printf("ETH link failed\r\n");
	}
	else
	{
		sleep(1);
		/* high 8 bits is port num, low 8 bits is clock mode */
		printf("ETH:%08x\n", eth_inf.data);
	}
}

/* 1 master, 0 slave */
void get_clk_mode(int port)
{
	struct eth_info_t eth_inf = {0};
	
	strncpy(eth_inf.ifd.ifd_name, ETH_NAME, sizeof(eth_inf.ifd.ifd_name) -1);
	
	eth_inf.ifd.ifd_cmd = DEVNP_ETH_PHY_GET_CLK_MODE;
	eth_inf.ifd.ifd_len = sizeof(int);
	eth_inf.ifd.ifd_data = NULL;
	eth_inf.data = port << 16;

	if(devctl(fd, SIOCGDRVSPEC,(void *) &eth_inf,sizeof(struct eth_info_t),NULL) == -1)
	{
		printf("ETH link failed\r\n");
	}
	else
	{
		/* high 8 bits is port num, low 8 bits is clock mode */
		printf("ETH:%08x\n", eth_inf.data);
	}
}

int main(int argc, char *argv[])
{
	fd = socket(AF_INET, SOCK_DGRAM, 0);

	if(fd < 0) {
		printf("ETH:%s:%d Open socket error!\n", __FILE__, __LINE__);
		return -1;
	}


	if (argc == 3 && !strcmp(argv[1], "-switchid"))
	{
		int port = 0;
		port = strtoul(argv[2], 0, 16);
		get_switchid(port);
	}
	else if (argc == 3 && !strcmp(argv[1], "-phyid"))
	{
		int port = 0;
		port = strtoul(argv[2], 0, 16);
		get_phyId(port);
	}
	else if (argc == 3 && !strcmp(argv[1], "-sqi"))
	{
		int port = 0;
		port = strtoul(argv[2], 0, 16);
		get_sqi_level16(port);
	}
	else if (argc == 3 && !strcmp(argv[1], "-linkstatus"))
	{
		int port = 0;
		port = strtoul(argv[2], 0, 16);
		get_link_status(port);
	}
	else if (argc == 3 && !strcmp(argv[1], "-cablestatus"))
	{
		int port = 0;
		port = strtoul(argv[2], 0, 16);
		get_cable_status(port);
	}
	else if (argc == 3 && !strcmp(argv[1], "-startvcttest"))
	{
		int port = 0;
		port = strtoul(argv[2], 0, 16);
		start_vct_tst(port);
	}
	else if (argc == 3 && !strcmp(argv[1], "-getvctcpltflag"))
	{
		int port = 0;
		port = strtoul(argv[2], 0, 16);
		get_vct_cplt_flag(port);
	}
	else if (argc == 3 && !strcmp(argv[1], "-getcablesta"))
	{
		int port = 0;
		port = strtoul(argv[2], 0, 16);
		get_cable_sta(port);
	}
	else if (argc == 3 && !strcmp(argv[1], "-sleepwakeup"))
	{
		int port = 0;
		port = strtoul(argv[2], 0, 16);
		get_sleepwakeup_status(port);
	}
	else if (argc == 4 && !strcmp(argv[1], "-lpsdenset"))
	{
		int en = 0;
		int port = 0;
		port = strtoul(argv[2], 0, 16);
		en = strtoul(argv[3], 0, 16);
		set_lpsd_en_ctl(port, en);
	}
	else if (argc == 3 && !strcmp(argv[1], "-lpsdenget"))
	{
		int port = 0;
		port = strtoul(argv[2], 0, 16);
		get_lpsd_en_status(port);
	}
	else if (argc == 4 && !strcmp(argv[1], "-testmode"))
	{
		int tstmode = 0;
		int port = 0;
		port = strtoul(argv[2], 0, 16);
		tstmode = strtoul(argv[3], 0, 16);
		set_testmode(port, tstmode);
	}
	else if (argc == 4 && !strcmp(argv[1], "-tstmodeset"))
	{
		int tstmode = 0;
		int port = 0;
		port = strtoul(argv[2], 0, 16);
		tstmode = strtoul(argv[3], 0, 16);
		set_tst_mode(port, tstmode);
	}
	else if (argc == 3 && !strcmp(argv[1], "-tstmodeget"))
	{
		int port = 0;
		port = strtoul(argv[2], 0, 16);
		get_tst_mode(port);
	}
	else if (argc == 4 && !strcmp(argv[1], "-clockmode"))
	{
		int clkmode = 0;
		int port = 0;
		port = strtoul(argv[2], 0, 16);
		clkmode = strtoul(argv[3], 0, 16);
		set_clkmode(port, clkmode);
	}
	else if (argc == 4 && !strcmp(argv[1], "-clkmodeset"))
	{
		int clkmode = 0;
		int port = 0;
		port = strtoul(argv[2], 0, 16);
		clkmode = strtoul(argv[3], 0, 16);
		set_clk_mode(port, clkmode);
	}
	else if (argc == 3 && !strcmp(argv[1], "-clkmodeget"))
	{
		int port = 0;
		port = strtoul(argv[2], 0, 16);
		get_clk_mode(port);
	}
	else if (argc == 3 && !strcmp(argv[1], "-downgrade2A0"))
	{
		int port = 0;
		port = strtoul(argv[2], 0, 16);
		downgrade2A0(port);
	}
	else if (argc == 3 && !strcmp(argv[1], "-init"))
	{
		int port = 0;
		port = strtoul(argv[2], 0, 16);
		init(port);
	}
	else if (argc == 4 && !strcmp(argv[1], "-regset"))
	{
		int clkmode = 0;
		int port = 0;
		port = strtoul(argv[2], 0, 16);
		clkmode = strtoul(argv[3], 0, 16);
		set_clk_mode(port, clkmode);
	}
	else if (argc == 3 && !strcmp(argv[1], "-regget"))
	{
		int port = 0;
		port = strtoul(argv[2], 0, 16);
		get_clk_mode(port);
	}
	else
	{
		close(fd );
		printf("The parameter is not right, please use it as:\n");
		printf("port:0 CPU, 1~5 100Mbps, 7 1000Mbps>\n");
		printf("88Q5050PHY -switchid <port>\n");
		printf("88Q5050PHY -sqi <port>\n");
		printf("88Q5050PHY -linkstatus <port>\n");
		printf("88Q5050PHY -cablestatus <port>\n");
		printf("88Q5050PHY -startvcttest <port>\n");
		printf("88Q5050PHY -getvctcpltflag <port>\n");
		printf("88Q5050PHY -getcablesta <port>\n");
		printf("88Q5050PHY -sleepwakeup <port>\n");
		printf("88Q5050PHY -lpsdenset <port> <en>\n");
		printf("88Q5050PHY -lpsdenget <port>\n");
		printf("88Q5050PHY -testmode <port> <mode>\n");
		printf("88Q5050PHY -tstmodeset <port> <mode>\n");
		printf("88Q5050PHY -tstmodeget <port>\n");
		printf("88Q5050PHY -clockmode <port> <mode>\n");
		printf("88Q5050PHY -clkmodeset <port> <mode>\n");
		printf("88Q5050PHY -clkmodeget <port>\n");
		printf("88Q5050PHY -downgrade2A0 <port>\n");
		printf("88Q5050PHY -init <port>\n");
		return -1;
	}

	close(fd );
	return 0;
}

