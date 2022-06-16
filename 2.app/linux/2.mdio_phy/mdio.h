#ifndef _MDIO_H_
#define _MDIO_H_




#define DEVNP_ETH_PHY_LINK_STA_RD_CMD 	63
#define DEVNP_ETH_PHY_SQI_RD_CMD 		64
#define DEVNP_ETH_PHY_CABLE_STA_RD_CMD 	66
#define DEVNP_ETH_PHY_SLPWAK_STA_RD_CMD 68
#define DEVNP_ETH_PHY_SET_CLK_MODE 		71
#define DEVNP_ETH_PHY_GET_CLK_MODE 		72
#define DEVNP_ETH_PHY_SET_TST_MODE 		73
#define DEVNP_ETH_PHY_GET_TST_MODE 		74
#define DEVNP_ETH_PHY_SET_SWITCH_REG	84
#define DEVNP_ETH_PHY_GET_SWITCH_REG	85

//#define DEVNP_ETH_PHY_ID_RD_CMD 		69
//#define DEVNP_ETH_PHY_SWITCH_ID_RD_CMD 	70
//#define DEVNP_ETH_PHY_CLK_MODE_SET_CMD 	65
//#define DEVNP_ETH_PHY_TST_MODE_SET_CMD 	67
//#define DEVNP_ETH_PHY_ID_RD_CMD 		69
//#define DEVNP_ETH_PHY_SWITCH_ID_RD_CMD 	70
//#define DEVNP_ETH_PHY_START_VCT_TST 	75
//#define DEVNP_ETH_PHY_GET_VCT_CPLT_FLAG 76
//#define DEVNP_ETH_PHY_GET_CABLE_STA 	77
//#define DEVNP_ETH_PHY_EN_LPSD_CTRL 		78
//#define DEVNP_ETH_PHY_GET_LPSD_EN_STA 	79
//#define DEVNP_ETH_PHY_DOWNGRADE2A0		80
//#define DEVNP_ETH_PHY_READ_REG          81
//#define DEVNP_ETH_PHY_WRITE_REG         82
//#define DEVNP_ETH_PHY_INIT				83


/* common data structure between driver and application for
 * sharing info through ioctl
 * */
#define DWC_ETH_QOS_RWK_FILTER_LENGTH	8
struct ifr_data_struct {
	unsigned int flags;
	unsigned int chInx; /* dma channel no to be configured */
	unsigned int cmd;
	unsigned int phy_id;
	unsigned int context_setup;
	unsigned int connected_speed;
	unsigned int rwk_filter_values[DWC_ETH_QOS_RWK_FILTER_LENGTH];
	unsigned int rwk_filter_length;
	int command_error;
	int test_done;
	void *ptr;
	unsigned int adrs;
	int bar_num;
	uint32_t unit;
	uint32_t addr;
	uint32_t data;
};

#endif
