#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/mii.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/sockios.h>
#include <linux/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include "mdio.h"


int main(int argc, char *argv[])
{
	struct ifr_data_struct req;
	struct ifreq ifr;
	int sockfd;
	int port;
	int ret;

    if((argc != 4 && argc != 5 && argc != 6) || !strcmp(argv[1], "-h")) {
		printf("argc=%d, The parameter is not right, please use it as:\n",argc);
		printf("phy_id: 1~5 100Mbps, 7 1000Mbps\n");

		printf("mdio eth0 -clkmodeget <phy_id>\n");
		printf("mdio eth0 -clkmodeset <phy_id> <mode>\n");

		printf("mdio eth0 -tstmodeget <phy_id>\n");
		printf("mdio eth0 -tstmodeset <phy_id> <mode>\n");

		printf("mdio eth0 -switchregget <phy_id> <reg>\n");
		printf("mdio eth0 -switchregset <phy_id> <reg> <val>\n");

		printf("mdio eth0 -sqi <phy_id>\n");
		printf("mdio eth0 -linkstatus <phy_id>\n");
		printf("mdio eth0 -cablestatus <phy_id>\n");
		printf("mdio eth0 -sleepwakeup <phy_id>\n");

		return 0;
	}

	sockfd = socket(PF_LOCAL, SOCK_DGRAM, 0);
	if(sockfd < 0){
		printf("%s: socket fail!\n", argv[0]);
		return 0;
	}

	memset(&ifr, 0, sizeof(ifr));
	memset(&req, 0, sizeof(req));
	strncpy(ifr.ifr_name, argv[1], IFNAMSIZ - 1);
	ifr.ifr_ifru.ifru_data = &req;



	if (argc == 4 && !strcmp(argv[2], "-linkstatus"))
	{
/* 0xFF fault or read error, 1 link up, 0 link down */
		req.phy_id = strtoul(argv[3], 0, 16);
		req.cmd = DEVNP_ETH_PHY_LINK_STA_RD_CMD;

		ret = ioctl(sockfd, SIOCDEVPRIVATE, &ifr);
		if(ret < 0){
			printf("%s: linkstatus fail!\n", argv[0]);
			goto err;
		}
		printf("linkstatus phy_id=%d, data=0x%x\n", req.phy_id, req.data);
	}
	else if (argc == 4 && !strcmp(argv[2], "-clkmodeget"))
	{
/* 1 master, 0 slave */
		req.phy_id = strtoul(argv[3], 0, 16);
		req.cmd = DEVNP_ETH_PHY_GET_CLK_MODE;

		ret = ioctl(sockfd, SIOCDEVPRIVATE, &ifr);
		if(ret < 0){
			printf("%s: clkmodeget fail!\n", argv[0]);
			goto err;
		}
		printf("clkmodeget phy_id=%d, data=0x%x\n", req.phy_id, req.data);;
	}
	else if (argc == 5 && !strcmp(argv[2], "-clkmodeset"))
	{
/* 1 master, 0 slave */
		req.phy_id = strtoul(argv[3], 0, 16);
		req.data = strtoul(argv[4], 0, 16);
		req.cmd = DEVNP_ETH_PHY_SET_CLK_MODE;

		ret = ioctl(sockfd, SIOCDEVPRIVATE, &ifr);
		if(ret < 0){
			printf("%s: clkmodeset fail!\n", argv[0]);
			goto err;
		}
		printf("clkmodeset phy_id=%d, data=0x%x\n", req.phy_id, req.data);
	}
	else if (argc == 5 && !strcmp(argv[2], "-switchregget"))
	{
		req.phy_id = strtoul(argv[3], 0, 16);
		req.addr = strtoul(argv[4], 0, 16);
		req.cmd = DEVNP_ETH_PHY_GET_SWITCH_REG;

		ret = ioctl(sockfd, SIOCDEVPRIVATE, &ifr);
		if(ret < 0){
			printf("%s: switchregget fail!\n", argv[0]);
			goto err;
		}
		printf("switchregget phy_id=%d, addr=0x%x, data=0x%x\n", req.phy_id, req.addr, req.data);
	}
	else if (argc == 6 && !strcmp(argv[2], "-switchregset"))
	{
		req.phy_id = strtoul(argv[3], 0, 16);
		req.addr = strtoul(argv[4], 0, 16);
		req.data = strtoul(argv[5], 0, 16);
		req.cmd = DEVNP_ETH_PHY_SET_SWITCH_REG;

		ret = ioctl(sockfd, SIOCDEVPRIVATE, &ifr);
		if(ret < 0){
			printf("%s: switchregset fail!\n", argv[0]);
			goto err;
		}
		printf("switchregset phy_id=%d, addr=0x%x, data=0x%x\n", req.phy_id, req.addr, req.data);
	}
	else if (argc == 4 && !strcmp(argv[2], "-sqi"))
	{
/* sqi level: 0~15 */
		req.phy_id = strtoul(argv[3], 0, 16);
		req.cmd = DEVNP_ETH_PHY_SQI_RD_CMD;

		ret = ioctl(sockfd, SIOCDEVPRIVATE, &ifr);
		if(ret < 0){
			printf("%s: sqi fail!\n", argv[0]);
			goto err;
		}
		printf("sqi phy_id=%d, data=0x%x\n", req.phy_id, req.data);
	}
	else if (argc == 4 && !strcmp(argv[2], "-sleepwakeup"))
	{
/* 
*  1 lpsd local wakeup
*  2 lpsd remote wakeup
*  3 lpsd local wakeup & lpsd remote wakeup
*  0xFE lpsd is not enabled, it means the phy work in normal mode
*  0xFF Fault, read failed
*/
		req.phy_id = strtoul(argv[3], 0, 16);
		req.cmd = DEVNP_ETH_PHY_SLPWAK_STA_RD_CMD;

		ret = ioctl(sockfd, SIOCDEVPRIVATE, &ifr);
		if(ret < 0){
			printf("%s: sleepwakeup fail!\n", argv[0]);
			goto err;
		}
		printf("sleepwakeup phy_id=%d, data=0x%x\n", req.phy_id, req.data);
	}
	else if (argc == 4 && !strcmp(argv[2], "-tstmodeget"))
	{
/*
* testmode:0 normal, 1 amplitude, 2 period, 3 reserved, 4 distortion,
*          5 spectral effiency.
*/
		req.phy_id = strtoul(argv[3], 0, 16);
		req.cmd = DEVNP_ETH_PHY_GET_TST_MODE;

		ret = ioctl(sockfd, SIOCDEVPRIVATE, &ifr);
		if(ret < 0){
			printf("%s: tstmodeget fail!\n", argv[0]);
			goto err;
		}
		printf("tstmodeget phy_id=%d, data=0x%x\n", req.phy_id, req.data);;
	}
	else if (argc == 5 && !strcmp(argv[2], "-tstmodeset"))
	{
/*
* testmode:0 normal, 1 amplitude, 2 period, 3 reserved, 4 distortion,
*          5 spectral effiency.
*/
		req.phy_id = strtoul(argv[3], 0, 16);
		req.data = strtoul(argv[4], 0, 16);
		req.cmd = DEVNP_ETH_PHY_SET_TST_MODE;

		ret = ioctl(sockfd, SIOCDEVPRIVATE, &ifr);
		if(ret < 0){
			printf("%s: tstmodeset fail!\n", argv[0]);
			goto err;
		}
		printf("tstmodeset phy_id=%d, data=0x%x\n", req.phy_id, req.data);
	}
	else if (argc == 4 && !strcmp(argv[2], "-cablestatus"))
	{
/* 
*  0=good cable
*  't'=terminated, 116		0x74
*  'o'=open, 111			0x6F
*  's'=short, 115			0x73
*  0xFE polarity is negative, distance is 0xFF, it means the length is
*       (0xFF-16) * 1.5 meters.
*  0xFF read failed. 
*/
		req.phy_id = strtoul(argv[3], 0, 16);
		req.cmd = DEVNP_ETH_PHY_CABLE_STA_RD_CMD;

		ret = ioctl(sockfd, SIOCDEVPRIVATE, &ifr);
		if(ret < 0){
			printf("%s: cablestatus fail!\n", argv[0]);
			goto err;
		}
		printf("cablestatus phy_id=%d, data=0x%x\n", req.phy_id, req.data);
	}

err:
	close(sockfd);
	return 0;
}