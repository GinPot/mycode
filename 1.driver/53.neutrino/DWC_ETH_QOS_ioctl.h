#ifndef _DWC_ETH_IOCTL_H_
#define _DWC_ETH_IOCTL_H_



#define SMIBUSY_BIT	15
#define SMIFUNC_BIT 	13 //bit 14:13, 0 internal, 1 external, 2 smi setup
#define SMIMODE_BIT	12 //0 clause 45 frame, 1 clause 22 frame
#define SMIOP_BIT 	10 //bit 11:10
#define DEVADDR_BIT	5 //bit 9:5
#define REGADDR_BIT	0 //bit 4:0

#define START_SMI_OPR 		0x1
#define INTERNAL_ACCESS 	0x0
#define EXTERNAL_ACCESS 	0x1
#define CLAUSE45		0x0
#define WR_ADDR_REG_IN_CLA45	0x0
#define WR_DATA_REG_IN_CLA45	0x1
#define RD_DATA_REG_IN_CLA45 	0x3

#define Q2112_SMI_ADDRESS 	1

#define WR_ADDR_REG_OP_IN_CLA45 ((START_SMI_OPR << SMIBUSY_BIT) | (INTERNAL_ACCESS << SMIFUNC_BIT) | (CLAUSE45 << SMIMODE_BIT) | (WR_ADDR_REG_IN_CLA45 << SMIOP_BIT))
#define WR_DATA_REG_OP_IN_CLA45 ((START_SMI_OPR << SMIBUSY_BIT) | (INTERNAL_ACCESS << SMIFUNC_BIT) | (CLAUSE45 << SMIMODE_BIT) | (WR_DATA_REG_IN_CLA45 << SMIOP_BIT))
#define RD_DATA_REG_OP_IN_CLA45 ((START_SMI_OPR << SMIBUSY_BIT) | (INTERNAL_ACCESS << SMIFUNC_BIT) | (CLAUSE45 << SMIMODE_BIT) | (RD_DATA_REG_IN_CLA45 << SMIOP_BIT))

#define WR_ADDR_REG_OP_EXT_CLA45 ((START_SMI_OPR << SMIBUSY_BIT) | (EXTERNAL_ACCESS << SMIFUNC_BIT) | (CLAUSE45 << SMIMODE_BIT) | (WR_ADDR_REG_IN_CLA45 << SMIOP_BIT))
#define WR_DATA_REG_OP_EXT_CLA45 ((START_SMI_OPR << SMIBUSY_BIT) | (EXTERNAL_ACCESS << SMIFUNC_BIT) | (CLAUSE45 << SMIMODE_BIT) | (WR_DATA_REG_IN_CLA45 << SMIOP_BIT))
#define RD_DATA_REG_OP_EXT_CLA45 ((START_SMI_OPR << SMIBUSY_BIT) | (EXTERNAL_ACCESS << SMIFUNC_BIT) | (CLAUSE45 << SMIMODE_BIT) | (RD_DATA_REG_IN_CLA45 << SMIOP_BIT))



extern int devnp_eth_phy_link_sta_rd(struct DWC_ETH_QOS_prv_data *pdata, struct ifr_data_struct *req);
extern int devnp_eth_phy_cable_sta_rd(struct DWC_ETH_QOS_prv_data *pdata, struct ifr_data_struct *req);
extern int devnp_eth_phy_set_clk_mode(struct DWC_ETH_QOS_prv_data *pdata, struct ifr_data_struct *req);
extern int devnp_eth_phy_get_clk_mode(struct DWC_ETH_QOS_prv_data *pdata, struct ifr_data_struct *req);
extern int devnp_eth_phy_switch_ports_regs_read(struct DWC_ETH_QOS_prv_data *pdata, struct ifr_data_struct *req);
extern int devnp_eth_phy_switch_ports_regs_write(struct DWC_ETH_QOS_prv_data *pdata, struct ifr_data_struct *req);
extern int devnp_eth_phy_sqi_read_level16(struct DWC_ETH_QOS_prv_data *pdata, struct ifr_data_struct *req);
extern int devnp_eth_phy_sleepWakeup_sta_rd(struct DWC_ETH_QOS_prv_data *pdata, struct ifr_data_struct *req);
extern int devnp_eth_phy_set_tst_mode(struct DWC_ETH_QOS_prv_data *pdata, struct ifr_data_struct *req);
extern int devnp_eth_phy_get_tst_mode(struct DWC_ETH_QOS_prv_data *pdata, struct ifr_data_struct *req);



#endif
