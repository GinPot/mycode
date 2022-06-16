#include "DWC_ETH_QOS_yheader.h"
#include "DWC_ETH_QOS_yapphdr.h"
#include "DWC_ETH_QOS_ipa.h"
#include "DWC_ETH_QOS_yregacc.h"
#include "DWC_ETH_QOS_ioctl.h"
#include "DWC_ETH_QOS_ioctl.h"

#include <linux/firmware.h>
#include <linux/msm-bus.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/jiffies.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/debugfs.h>







static int devnp_ntn_eth_smi_glob2_write(struct DWC_ETH_QOS_prv_data *pdata, int phyaddr, int devtype, int phyreg, int phydata)
{
	//INFO_PRINT("[Q5050_GLOB2]devnp_ntn_eth_smi_glob2_write\n");
	struct hw_if_struct *hw_if = &(pdata->hw_if);
	int ret = 0, cnt_flag=0;
	int regData;

	do{
		/* write register address(phyreg) */
		ret = hw_if->read_phy_regs(0x1C, 0x18, &regData, pdata);
		if (ret != Y_SUCCESS) {
			NMSGPR_ERR("[Q5050_GLOB2]write addr:read phy regs smi busy failed\n");
			return ret;
		}

		if(cnt_flag != 0){	
			DBGPR("[Q5050_GLOB2]Write:SMI is busy 01\n");
			msleep(1);
		}
		cnt_flag++;
	}
	while ((regData >> 15) != 0);

	ret = hw_if->write_phy_regs(0x1C, 0x19, phyreg, pdata);
	if (ret != Y_SUCCESS) {
		NMSGPR_ERR("WRITE phy regs:phyreg to data reg failed\n");
		return ret;
	}

	regData = 0;
	if (7 == phyaddr)
		regData = WR_ADDR_REG_OP_EXT_CLA45 | (Q2112_SMI_ADDRESS << DEVADDR_BIT) | (devtype << REGADDR_BIT);
	else
		regData = WR_ADDR_REG_OP_IN_CLA45 | ((phyaddr & 0x1f) << DEVADDR_BIT) | (devtype << REGADDR_BIT);
	ret = hw_if->write_phy_regs(0x1C, 0x18, regData, pdata);
	if (ret != Y_SUCCESS) {
		NMSGPR_ERR("[Q5050_GLOB2]WRITE phy regs: start smi write addr failed\n");
		return ret;
	}

	/* write register data(phydata) */
	regData = 0;
	cnt_flag = 0;
	do{
		msleep(5);
		ret = hw_if->read_phy_regs(0x1C, 0x18, &regData, pdata);
		if (ret != Y_SUCCESS) {
			NMSGPR_ERR("[Q5050_GLOB2]write data:read phy regs smi busy failed\n");
			return ret;
		}

		if(cnt_flag != 0){	
			DBGPR("[Q5050_GLOB2]Write:SMI is busy 02\n");
		}
		cnt_flag++;
	}
	while ((regData >> 15) != 0);

	ret = hw_if->write_phy_regs(0x1C, 0x19, phydata, pdata);
	if (ret != Y_SUCCESS) {
		NMSGPR_ERR("[Q5050_GLOB2]WRITE phy regs:phydata to data reg failed\n");
		return ret;
	}

	regData = 0;
	if (7 == phyaddr)
		regData = WR_DATA_REG_OP_EXT_CLA45 | (Q2112_SMI_ADDRESS << DEVADDR_BIT) | (devtype << REGADDR_BIT);
	else
		regData = WR_DATA_REG_OP_IN_CLA45 | ((phyaddr & 0x1f) << DEVADDR_BIT) | (devtype << REGADDR_BIT);
	ret = hw_if->write_phy_regs(0x1C, 0x18, regData, pdata);
	if (ret != Y_SUCCESS) {
		NMSGPR_ERR("[Q5050_GLOB2]WRITE phy regs: start smi write data failed\n");
		return ret;
	}

	/* ensure write completed*/
	regData = 0;
	ret = hw_if->read_phy_regs(0x1C, 0x18, &regData, pdata);
	if (ret != Y_SUCCESS) {
		NMSGPR_ERR("[Q5050_GLOB2]read phy regs smi busy failed\n");
		return ret;
	}

	if ((regData >> 15) == 0) {
		//DBGPR("[Q5050_GLOB2]smi write data success\n");
	} else {
		NMSGPR_ERR("[Q5050_GLOB2]smi write data failed\n");
	}

	return ret;
}

static int devnp_ntn_eth_smi_glob2_read(struct DWC_ETH_QOS_prv_data *pdata, int phyaddr, int devtype, int phyreg, int *phydata)
{
	//DBGPR("[Q5050_GLOB2]devnp_ntn_eth_smi_glob2_read\n");
	struct hw_if_struct *hw_if = &(pdata->hw_if);
	int ret = 0, cnt_flag=0;
	int regData;

	do{
		/* write register address(phyreg) */
		ret = hw_if->read_phy_regs(0x1C, 0x18, &regData, pdata);
		if (ret != Y_SUCCESS) {
			NMSGPR_ERR("[Q5050_GLOB2]write addr:read phy regs smi busy failed\n");
			return ret;
		}
		
		if(cnt_flag != 0){	
			DBGPR("[Q5050_GLOB2]Read:SMI is busy 01\n");
			msleep(1);
		}
		cnt_flag++;
	}while ((regData >> 15) != 0);

	ret = hw_if->write_phy_regs(0x1C, 0x19, phyreg, pdata);
	if (ret != Y_SUCCESS) {
		NMSGPR_ERR("[Q5050_GLOB2]WRITE phy regs:phyreg to data reg failed\n");
		return ret;
	}

	regData = 0;
	if (7 == phyaddr)
		regData = WR_ADDR_REG_OP_EXT_CLA45 | (Q2112_SMI_ADDRESS << DEVADDR_BIT) | (devtype << REGADDR_BIT);
	else
		regData = WR_ADDR_REG_OP_IN_CLA45 | ((phyaddr & 0x1f) << DEVADDR_BIT) | (devtype << REGADDR_BIT);
	/* send write addr operation */
	ret = hw_if->write_phy_regs(0x1C, 0x18, regData, pdata);
	if (ret != Y_SUCCESS) {
		NMSGPR_ERR("[Q5050_GLOB2]WRITE phy regs: start smi write addr failed\n");
		return ret;
	}

	/* read register data(phydata) */
	regData = 0;
	cnt_flag = 0;
	do{
		msleep(5);
		ret = hw_if->read_phy_regs(0x1C, 0x18, &regData, pdata);
		if (ret != Y_SUCCESS) {
			NMSGPR_ERR("[Q5050_GLOB2]read data:read phy regs smi busy failed\n");
			return ret;
		}

		if(cnt_flag != 0){	
			DBGPR("[Q5050_GLOB2]Read:SMI is busy 02\n");
		}
		cnt_flag++;
	}
	while ((regData >> 15) != 0);

	regData = 0;
	if (7 == phyaddr)
		regData = RD_DATA_REG_OP_EXT_CLA45 | (Q2112_SMI_ADDRESS << DEVADDR_BIT) | (devtype << REGADDR_BIT);
	else
		regData = RD_DATA_REG_OP_IN_CLA45 | ((phyaddr & 0x1f) << DEVADDR_BIT) | (devtype << REGADDR_BIT);
	/* send read data operation */
	ret = hw_if->write_phy_regs(0x1C, 0x18, regData, pdata);
	if (ret != Y_SUCCESS) {
		NMSGPR_ERR("[Q5050_GLOB2]WRITE phy regs: start smi write data failed\n");
		return ret;
	}

	regData = 0;
	cnt_flag = 0;
	do{
		msleep(5);
		ret = hw_if->read_phy_regs(0x1C, 0x18, &regData, pdata);
		if (ret != Y_SUCCESS) {
			NMSGPR_ERR("[Q5050_GLOB2]Ensure read op complt:read phy regs 0x18 for smi busy failed\n");
			return ret;
		}

		if(cnt_flag != 0){	
			DBGPR("[Q5050_GLOB2]Read:SMI is busy 03\n");
		}
		
		cnt_flag++;
	}
	while ((regData >> 15) != 0);

	ret = hw_if->read_phy_regs(0x1C, 0x19, phydata, pdata);
	if (ret != Y_SUCCESS) {
		NMSGPR_ERR("[Q5050_GLOB2]READ phy regs:phydata from data reg failed\n");
		return ret;
	}

	//DBGPR("[Q5050_GLOB2]GLOB2 Read, get value from 0x19 reg:%04x\n", *phydata);
	return ret;
}


/* read the phy link status */
int devnp_eth_phy_link_sta_rd(struct DWC_ETH_QOS_prv_data *pdata, struct ifr_data_struct *req) 
{
	int ret = -1;
	int phyid = 0;
	int phydata = 0;


	phyid = req->phy_id;
	req->data = 0;

	if(phyid == 7) {
		ret = devnp_ntn_eth_smi_glob2_read(pdata, phyid, 3, 0x0901, &phydata);
		if (ret != 0) {
			NMSGPR_ERR("read phy link status:read port-%d device3 reg 0x0901 for link status failed\n", phyid);
			req->data = 0xFF;
			return ret;
		}

		if ((phydata & 0x4) == 0x4) {
			req->data = 1;
		}
	} else {
		ret = devnp_ntn_eth_smi_glob2_read(pdata, phyid, 3, 0x8009, &phydata);
		if (ret != 0) {
			NMSGPR_ERR("read phy link status:read port-%d device3 reg 0x8009 for link status failed\n", phyid);
			req->data = 0xFF;
			return ret;
		}

		if ((phydata & 0x4) == 0x4) {
			req->data = 1;
		}
	}

	DBGPR("devnp_eth_phy_link_sta_rd phyid:%d data:%x\n", phyid, req->data);

	return ret;
}

static void softReset(struct DWC_ETH_QOS_prv_data *pdata, int phyAddr)
{
	int ret = -1;
	int phydata = 0;

	ret = devnp_ntn_eth_smi_glob2_read(pdata, phyAddr, 3, 0x0000, &phydata);
	if (ret != 0) {
		NMSGPR_ERR("read phy reset bit failed");
		return;
	}

	phydata = phydata | 0x8000;

	ret = devnp_ntn_eth_smi_glob2_write(pdata, phyAddr, 3, 0x0000, phydata);
	if (ret != 0) {
		NMSGPR_ERR("enable phy reset failed");
		return;
	}

	ret = devnp_ntn_eth_smi_glob2_write(pdata, phyAddr, 3, 0xFA07, 0x0200);
	if (ret != 0) {
		NMSGPR_ERR("write device3 reg 0xFA07 failed");
		return;
	}

	msleep(5);
}

/* phy set to master or slave mode */
int devnp_eth_phy_set_clk_mode(struct DWC_ETH_QOS_prv_data *pdata, struct ifr_data_struct *req) {
	int ret = -1;
	int phydata = 0;
	int phyid = 0;

	phyid = req->phy_id;
	req->data = req->data & 0x1;

	ret = devnp_ntn_eth_smi_glob2_read(pdata, phyid, 1, 0x0834, &phydata);
	if (ret != 0) {
		NMSGPR_ERR("devnp_eth_phy_set_clk_mode:read port-%d device1 reg 0x0834 for origin clk mode failed\n", phyid);
		req->data = 0xFF;
		return ret;
	}

	if (req->data == 1){
		phydata |= 1 << 14;
	} else{
		phydata &= ~(1 << 14);
	}

	ret = devnp_ntn_eth_smi_glob2_write(pdata, phyid, 1, 0x0834, phydata);
	if (ret != 0) {
		NMSGPR_ERR("devnp_eth_phy_set_clk_mode:write port-%d device1 reg 0x0834 for clk mode failed\n", phyid);
		req->data = 0xFF;
		return ret;
	}

	softReset(pdata, phyid);

	DBGPR("call phy set clock mode phyid:%d data:%04x\n", phyid, req->data);

	return ret;
}

/* phy get clock mode:master or slave, 1->master, 0->slave */
int devnp_eth_phy_get_clk_mode(struct DWC_ETH_QOS_prv_data *pdata, struct ifr_data_struct *req) {
	int ret = -1;
	int phyid = 0;
	int phydata = 0;

	phyid = req->phy_id;
	req->data = 0;

	ret = devnp_ntn_eth_smi_glob2_read(pdata, phyid, 1, 0x0834, &phydata);
	if (ret != 0) {
		NMSGPR_ERR("devnp_eth_phy_get_clk_mode:read port-%d device1 reg 0x0834 for new clk mode failed\n", phyid);
		req->data = 0xFF;
		return ret;
	}

	req->data = (phydata & 0x4000) >> 14;

	DBGPR("call phy get clock mode phyid:%d data:%04x\n", phyid, req->data);

	return ret;
}

/* read regs for phy switch ports */
int devnp_eth_phy_switch_ports_regs_read(struct DWC_ETH_QOS_prv_data *pdata, struct ifr_data_struct *req) {
	struct hw_if_struct *hw_if = &(pdata->hw_if);
	int ret = -1;
	int phydata = 0;
	int phyreg = 0;
	int phyid = 0;

	phyid = req->phy_id;
	req->data = 0;
	phyreg = req->addr & 0xff;

	ret = hw_if->read_phy_regs(phyid, phyreg, &phydata, pdata);
	if (ret != Y_SUCCESS) {
		NMSGPR_ERR("%s: ERROR: read phy physical control reg failed\n",__func__);
		req->data = 0xFF;
		return ret;
	}
	req->data |= phydata;

	DBGPR("call phy get switch regs phyid:%d addr:0x%x data:%04x\n", phyid, req->addr, req->data);

	return ret;
}

/* write regs for phy switch ports */
int devnp_eth_phy_switch_ports_regs_write(struct DWC_ETH_QOS_prv_data *pdata, struct ifr_data_struct *req) {
	struct hw_if_struct *hw_if = &(pdata->hw_if);
	int ret = -1;
	int phyreg = 0;
	int valreg = 0;
	int phyid = 0;

	phyid = req->phy_id;
	phyreg = req->addr & 0xff;
	valreg = req->data & 0xffff;

	ret = hw_if->write_phy_regs(phyid, phyreg, valreg, pdata);
	if(ret < 0) {
		NMSGPR_ERR("%s: ERROR: write phy physical control reg failed\n",__func__);
		req->data = 0xFF;
		return ret;
	}

	DBGPR("call phy write switch regs phyid:%d addr:0x%x data:%04x\n", phyid, req->addr, req->data);

	return ret;
}

/* read the phy sleep/wakeup status */
int devnp_eth_phy_sleepWakeup_sta_rd(struct DWC_ETH_QOS_prv_data *pdata, struct ifr_data_struct *req) {
	int ret = -1;
	int phyid = 0;
	int phydata = 0;

	phyid = req->phy_id;
	req->data = 0;

	ret = devnp_ntn_eth_smi_glob2_read(pdata, phyid, 3, 0x8700, &phydata);
	if (ret != 0) {
		NMSGPR_ERR("read phy sleepWakeup status:read port-%d device3 reg 0x8700 for wake status failed\n", phyid);
		req->data = 0xFF;
		return ret;
	} else if ((phydata & 0x1) == 0) {
		NMSGPR_ERR("read phy sleepWakeup status:port-%d LSPD power down not enabled\n", phyid);
		req->data = 0xFE;
		return ret;
	}

	if ((phydata & 0x2) && (phydata & 0x4)) {
		req->data = 0x3; //lpsd local wakeup & remote wakeup
	} else if (phydata & 0x2) {
		req->data = 0x1; //lpsd local wakeup
	} else if (phydata & 0x4) {
		req->data = 0x2; //lpsd remote wakeup
	}

	DBGPR("call phy sleepWakeup status phyid:%d read %04x", phyid, req->data);


	return ret;
}

/* phy set test mode */
int devnp_eth_phy_set_tst_mode(struct DWC_ETH_QOS_prv_data *pdata, struct ifr_data_struct *req) {
	int ret = -1;
	int testmode = 0;
	int phydata = 0;
	int phyid = 0;
	int reg_addr = 0;

	phyid = req->phy_id;
	testmode = req->data & 0x7;

	if (7 == phyid)
		reg_addr = 0x0904;
	else
		reg_addr = 0x0836;

	ret = devnp_ntn_eth_smi_glob2_read(pdata, phyid, 1, reg_addr, &phydata);
	if (ret != 0) {
		NMSGPR_ERR("devnp_eth_phy_set_tst_mode:read port-%d device1 reg:%x for origin clk mode failed\n", phyid, reg_addr);
		req->data = 0xFF;
		return ret;
	}

	phydata = (phydata & 0x1FFF) | (testmode << 13);

	ret = devnp_ntn_eth_smi_glob2_write(pdata, phyid, 1, reg_addr, phydata);
	if (ret != 0) {
		NMSGPR_ERR("devnp_eth_phy_set_tst_mode:write port-%d device1 reg:%x for test mode failed\n", phyid, reg_addr);
		req->data = 0xFF;
		return ret;
	}

	if ((7 == phyid) && (5 == testmode)) {
		devnp_ntn_eth_smi_glob2_write(pdata, phyid, 3, 0xfc10, 0x0000);
		devnp_ntn_eth_smi_glob2_write(pdata, phyid, 3, 0xfc11, 0x003d);
		devnp_ntn_eth_smi_glob2_write(pdata, phyid, 3, 0xfc12, 0x0003);
		devnp_ntn_eth_smi_glob2_write(pdata, phyid, 3, 0xfc13, 0x0017);
	}

	req->data = (phydata & 0xE000) >> 13;
	
	DBGPR("call phy set test phyid:%d mode %04x\n", phyid, req->data);

	return ret;
}

/* phy get test mode */
int devnp_eth_phy_get_tst_mode(struct DWC_ETH_QOS_prv_data *pdata, struct ifr_data_struct *req) {
	int ret = -1;
	int phydata = 0;
	int phyid = 0;
	int reg_addr = 0;

	phyid = req->phy_id;
	req->data = 0;

	if (7 == phyid)
		reg_addr = 0x0904;
	else
		reg_addr = 0x0836;

	ret = devnp_ntn_eth_smi_glob2_read(pdata, phyid, 1, reg_addr, &phydata);
	if (ret != 0) {
		NMSGPR_ERR("devnp_eth_phy_get_tst_mode:read port-%d device1 reg:%x for new test mode failed\n", phyid, reg_addr);
		req->data = 0xFF;
		return ret;
	}

	req->data = (phydata & 0xE000) >> 13;

	DBGPR("call phy get test phyid:%d mode %04x\n", phyid, req->data);

	return ret;
}

static uint16_t regRead(struct DWC_ETH_QOS_prv_data *pdata, uint16_t port, uint16_t dev_type, uint16_t reg_addr)
{
	int phyid=0, phydata=0;

	// read
	phyid = port & 0xff;

	if (devnp_ntn_eth_smi_glob2_read(pdata, phyid, dev_type, reg_addr, &phydata))
		NMSGPR_ERR("regRead port:%d dev_type:%d, reg_addr:0x%x failed.\n", port, dev_type, reg_addr);

	return phydata;
}

static void regWrite(struct DWC_ETH_QOS_prv_data *pdata, uint16_t port, uint16_t dev_type, uint16_t reg_addr, uint16_t reg_data)
{
	int phyid=0;

	// write
	phyid = port & 0xff;

	if (devnp_ntn_eth_smi_glob2_write(pdata, phyid, dev_type, reg_addr, reg_data))
		NMSGPR_ERR("regWrite port:%d dev_type:%d, reg_addr:0x%x reg_data:0x%x failed.\n", port, dev_type, reg_addr, reg_data);
}

static bool getAnegEnabled(struct DWC_ETH_QOS_prv_data *pdata, uint16_t phyAddr)
{
	return (regRead(pdata, phyAddr, 7, 0x0200) & 0x1000) == 0x1000;
}

static uint16_t getSpeed(struct DWC_ETH_QOS_prv_data *pdata, uint16_t phyAddr)
{
	// check if AN enabled
	if (getAnegEnabled(pdata, phyAddr)) {
		return ((regRead(pdata, phyAddr, 7, 0x801a) & 0x4000) == 0x4000);
	} else {
		return (regRead(pdata, phyAddr, 1, 0x0834) & 0x0001) == 0x0001;
	}
}

static uint16_t getRevNum(struct DWC_ETH_QOS_prv_data *pdata, uint16_t phyAddr)
{
	uint16_t devAddr = 1;
	uint16_t regAddr = 0x0003;
	uint16_t retData = 0;
	uint16_t revNum = 0;

	retData = regRead(pdata, phyAddr, devAddr, regAddr);
	revNum = (retData & 0xf);

	return revNum;
}

//static double mypow(double num,double n)
//{
//	double value = 1;
//	int i = 1;
//
//	if (n == 0)
//		value = 1;
//	else {
//		while (i++ <= n)
//			value *= num;
//	}
//
//	return value;
//}

#define MRVL_88Q2112_100BASE_T1		0x0000
#define MRVL_88Q2112_Z1				0x0000
/* read phy sqi value 0~15 */
int devnp_eth_phy_sqi_read_level16(struct DWC_ETH_QOS_prv_data *pdata, struct ifr_data_struct *req) {
	int ret = 0;
	int phydata = 0;
	int phyid = 0;
	uint16_t SQI = 0;
    uint16_t regVal = 0;
	uint16_t trigVal = 0;
	//double test = 0;

	phyid = req->phy_id;
	req->data = 0;

	if (7 == phyid) {
		// port 7: 88Q2112
		// read different register for different rates
	    if (MRVL_88Q2112_100BASE_T1 == getSpeed(pdata, phyid)) {
			// 100 BASE-T1
	        regVal = regRead(pdata, phyid, 3, 0x8230);
	        SQI = (regVal & 0xF000) >> 12;
	    } else {
			// 1000 BASE-T1
	        if (MRVL_88Q2112_Z1 != getRevNum(pdata, phyid)) {

				// A2/A1/A0
	            regVal = regRead(pdata, phyid, 3, 0xfC4C);

				if(regVal > 0x100)
					SQI = 0x0;
				else{
					if ((regVal & 0x03FF) == 0)
						regVal = 0x03FF;

					SQI = regVal * 2;
					if(SQI > 0xf)
						SQI = 0xf;
				}

	            //valDouble = (regVal & 0x03FF) / mypow(2.0, 10);
	            //if (valDouble > 0.35) valDouble = 0.35;
	            //if (valDouble < 8e-3) valDouble = 8e-3;
	            //test = 4.657790851448008 * mypow(valDouble, 6)
	            //    - 5.602322633777736 * mypow(valDouble, 5)
	            //    + 2.659063584758740 * mypow(valDouble, 4)
	            //    - 0.637339073242494 * mypow(valDouble, 3)
	            //    + 0.083157751272959 * mypow(valDouble, 2)
	            //    - 0.006409214385559 * valDouble + 0.000394024510294;
	            //test = test*1e5;
				//INFO_PRINT("test = %f\n", test);
				
				
	            //if (test>28.2)
	            //    SQI = 15;
				//else if (test>26.93)
	            //    SQI = 14;
				//else if (test>25.51)
	            //    SQI = 13;
	            //else if (test>24.04)
	            //    SQI = 12;
	            //else if (test>23.39)
	            //    SQI = 11;
	            //else if (test>22.41)
	            //    SQI = 10;
	            //else if (test>21.70)
	            //    SQI = 9;
	            //else if (test>21.06)
	            //    SQI = 8;
	            //else if (test>20.30)
	            //    SQI = 7;
	            //else if (test>19.69)
	            //    SQI = 6;
	            //else if (test>19.33)
	            //    SQI = 5;
	            //else if (test>18.88)
	            //    SQI = 4;
	            //else if (test>18.46)
	            //    SQI = 3;
	            //else if (test>17.88)
	            //    SQI = 2;
	            //else if (test>17.43)
	            //    SQI = 1;
	            //else
	            //    SQI = 0;
	        } else {
				// Z1 Case
	            regWrite(pdata, phyid, 3, 0xFC5D, 0x06BF);

	            //request SQI value
	            trigVal = regRead(pdata, phyid, 3, 0xFC89);
	            regVal = trigVal | 0x1;
	            regWrite(pdata, phyid, 3, 0xFC89, regVal);
	            msleep(5); //delay 5ms

	            //read SQI value
	            regWrite(pdata, phyid, 3, 0xFC5D, 0x86BF);
	            regVal = regRead(pdata, phyid, 3, 0xFC88);
	            SQI = regVal & 0xF;

	            // release the request
	            regWrite(pdata, phyid, 3, 0xFC5D, 0x06BF);
	            regWrite(pdata, phyid, 3, 0xFC89, trigVal);
	        }
	    }

	    req->data = SQI;
	} else {
		ret = devnp_ntn_eth_smi_glob2_read(pdata, phyid, 3, 0x8009, &phydata);
		if (ret != 0) {
			NMSGPR_ERR("read phy sqi with level16:read port-%d device3 reg 0x8009 for link status failed\n", phyid);
			req->data = 0xFF;
			return ret;
		}

		phydata = 0;
		ret = devnp_ntn_eth_smi_glob2_read(pdata, phyid, 3, 0xFF5D, &phydata);
		if (ret != 0) {
			NMSGPR_ERR("read phy sqi with level16:read port-%d device3 reg 0xFF5D failed\n", phyid);
			req->data = 0xFF;
			return ret;
		}

		if ((phydata & 0xFFFF) != 0x6862) {
			devnp_ntn_eth_smi_glob2_write(pdata, phyid, 3, 0xFF5D, 0x6862);
			devnp_ntn_eth_smi_glob2_write(pdata, phyid, 3, 0xFF5E, 0x736e);
			devnp_ntn_eth_smi_glob2_write(pdata, phyid, 3, 0xFF5F, 0x7f79);
			devnp_ntn_eth_smi_glob2_write(pdata, phyid, 3, 0xFF60, 0x8a85);
			devnp_ntn_eth_smi_glob2_write(pdata, phyid, 3, 0xFF61, 0x9790);
			devnp_ntn_eth_smi_glob2_write(pdata, phyid, 3, 0xFF63, 0xa39d);
			devnp_ntn_eth_smi_glob2_write(pdata, phyid, 3, 0xFF64, 0xb0aa);
			devnp_ntn_eth_smi_glob2_write(pdata, phyid, 3, 0xFF65, 0xb8);
		}

		phydata = 0;
		ret = devnp_ntn_eth_smi_glob2_read(pdata, phyid, 3, 0x8230, &phydata);
		if (ret != 0) {
			NMSGPR_ERR("read phy sqi with level16:read port-%d device3 reg 0x8230 for sqi value failed\n", phyid);
			req->data = 0xFF;
			return ret;
		}

		req->data = ((phydata & 0xF000) >> 12);
	}

	DBGPR("call phy sqi phyid:%d rd %04x\n", phyid, req->data);

	return ret;
}

static int getPhyMasterSlave(struct DWC_ETH_QOS_prv_data *pdata, int phyaddr) {
	int ret = -1;
	int phydata = 0;

	ret = devnp_ntn_eth_smi_glob2_read(pdata, phyaddr, 1, 0x0834, &phydata);
	if (ret != 0) {
		NMSGPR_ERR("set phy MasterSlave:read port-%d device1 reg 0x0834 for mode failed", phyaddr);
		return ret;
	}

	ret = (phydata & 0x4000) >> 14;
	return ret;
}

static int setPhyMasterSlave(struct DWC_ETH_QOS_prv_data *pdata, int phyaddr, int forceMaster) {
	int ret = -1;
	int phydata = 0;

	ret = devnp_ntn_eth_smi_glob2_read(pdata, phyaddr, 1, 0x0834, &phydata);
	if (ret != 0) {
		NMSGPR_ERR("set phy MasterSlave:read port-%d device1 reg 0x0834 for origin mode failed", phyaddr);
		return ret;
	}

	if (forceMaster == 1){
		phydata |= 1 << 14;
	} else{
		phydata &= ~(1 << 14);
	}

	ret = devnp_ntn_eth_smi_glob2_write(pdata, phyaddr, 1, 0x0834, phydata);
	if (ret != 0) {
		NMSGPR_ERR("set clock mode:write port-%d device1 reg 0x0834 for mode failed", phyaddr);
		return ret;
	}

	softReset(pdata, phyaddr);

	return ret;
}

/* read phy cable status */
int devnp_eth_phy_cable_sta_rd(struct DWC_ETH_QOS_prv_data *pdata, struct ifr_data_struct *req) {
	int ret = -1;
	int phydata = 0;
	int phyid = 0;
	
	uint16_t cnt = 0;
	uint16_t vct_polarity = 0;
	int cable_status = 0;
	uint16_t vct_dis = 0;
	uint16_t vct_amp = 0;
	int origMS = 1;

	phyid = req->phy_id;
	req->data = 0;


	origMS = getPhyMasterSlave(pdata, phyid);
	setPhyMasterSlave(pdata, phyid, 0);
	msleep(20);

	if(phyid == 7) {
		ret = devnp_ntn_eth_smi_glob2_read(pdata, phyid, 3, 0xfec9, &phydata);
		if (ret != 0) {
			NMSGPR_ERR("read phy cable status:read port-%d device3 reg 0xfec9 for origin para failed\n", phyid);
			req->data = 0xFF;
			return ret;
		}
		//fprintf(stderr, "0xfec9 -> 0x%x\n", phydata);
		ret = devnp_ntn_eth_smi_glob2_write(pdata, phyid, 3, 0xfec9, phydata | 0x0080);
		if (ret != 0) {
			NMSGPR_ERR("read phy cable status:write port-%d device3 reg 0xfec9 for vct impedance threshold\n", phyid);
			req->data = 0xFF;
			return ret;
		}

		ret = devnp_ntn_eth_smi_glob2_read(pdata, phyid, 3, 0xfec3, &phydata);
		if (ret != 0) {
			NMSGPR_ERR("read phy cable status:read port-%d device3 reg 0xfec3 for origin para failed\n", phyid);
			req->data = 0xFF;
			return ret;
		}
		//fprintf(stderr, "0xfec3 -> 0x%x\n", phydata);
		ret = devnp_ntn_eth_smi_glob2_write(pdata, phyid, 3, 0xfec3, phydata | 0x2000);
		if (ret != 0) {
			NMSGPR_ERR("read phy cable status:write port-%d device3 reg 0xfec3 for vct impedance threshold\n", phyid);
			req->data = 0xFF;
			return ret;
		}

		ret = devnp_ntn_eth_smi_glob2_read(pdata, phyid, 3, 0xfe5d, &phydata);
		if (ret != 0) {
			NMSGPR_ERR("read phy cable status:read port-%d device3 reg 0xfec3 for origin para failed\n", phyid);
			req->data = 0xFF;
			return ret;
		}
		//fprintf(stderr, "0xfe5d -> 0x%x\n", phydata);
		ret = devnp_ntn_eth_smi_glob2_write(pdata, phyid, 3, 0xfe5d, phydata & 0xEFFF);
		if (ret != 0) {
			NMSGPR_ERR("read phy cable status:write port-%d device3 reg 0xfe5d for vct impedance threshold\n", phyid);
			req->data = 0xFF;
			return ret;
		}

		ret = devnp_ntn_eth_smi_glob2_write(pdata, phyid, 3, 0xfec4, 0x0f1f);
		if (ret != 0) {
			NMSGPR_ERR("read phy cable status:write port-%d device3 reg 0xfec4 for vct impedance threshold\n", phyid);
			req->data = 0xFF;
			return ret;
		}

		ret = devnp_ntn_eth_smi_glob2_write(pdata, phyid, 3, 0xfec7, 0x1115);
		if (ret != 0) {
			NMSGPR_ERR("read phy cable status:write port-%d device3 reg 0xfec7 for vct impedance threshold\n", phyid);
			req->data = 0xFF;
			return ret;
		}

		ret = devnp_ntn_eth_smi_glob2_read(pdata, phyid, 3, 0xfec3, &phydata);
		if (ret != 0) {
			NMSGPR_ERR("read phy cable status:read port-%d device3 reg 0xfec3 for origin para failed\n", phyid);
			req->data = 0xFF;
			return ret;
		}
		//fprintf(stderr, "0xfec3 -> 0x%x\n", phydata);
		ret = devnp_ntn_eth_smi_glob2_write(pdata, phyid, 3, 0xfec3, phydata | 0x4000);
		if (ret != 0) {
			NMSGPR_ERR("read phy cable status:write port-%d device3 reg 0xfec3 for vct impedance threshold\n", phyid);
			req->data = 0xFF;
			return ret;
		}

		msleep(210);
		while (1){
			phydata = 0;
			ret = devnp_ntn_eth_smi_glob2_read(pdata, phyid, 3, 0xFEDB, &phydata);
			//fprintf(stderr, "0xFEDB -> 0x%x\n", phydata);
			if (ret != 0) {
				NMSGPR_ERR("read phy cable status:read port-%d device3 reg 0xFEDB for vct test status failed\n", phyid);
				req->data = 0xFF;
				return ret;
			}

			if (0x0 == (phydata & 0x0400))
				msleep(20);
			else
				break;
			cnt++;
			if (cnt>6)
			{
				req->data = 0xFF;
				return ret;
			}
		}

		phydata = 0;
		ret = devnp_ntn_eth_smi_glob2_read(pdata, phyid, 3, 0xfedb, &phydata);
		if (ret != 0) {
			NMSGPR_ERR("read phy cable status:read port-%d device3 reg 0xfedb for tdr result failed\n", phyid);
			req->data = 0xFF;
			return ret;
		}

		ret = devnp_ntn_eth_smi_glob2_read(pdata, phyid, 3, 0xfedc, &phydata);
		if (ret != 0) {
			NMSGPR_ERR("read phy cable status:read port-%d device3 reg 0xfedc for tdr result failed\n", phyid);
			req->data = 0xFF;
			return ret;
		}
		vct_amp = phydata & 0x007f;
		vct_polarity = phydata & 0x0080;



		if ((0x0 == vct_amp) && (0x0 != vct_polarity)){
			cable_status = 't';
		}
		else if (0x0 != vct_polarity){
			cable_status = 'o';
		}
		else{
			cable_status = 's';
		}
	} else {

		ret = devnp_ntn_eth_smi_glob2_write(pdata, phyid, 3, 0x851a, 0x0f1e);
		if (ret != 0) {
			NMSGPR_ERR("read phy cable status:write port-%d device3 reg 0x851a for vct impedance threshold\n", phyid);
			req->data = 0xFF;
			return ret;
		}

		ret = devnp_ntn_eth_smi_glob2_read(pdata, phyid, 3, 0x8517, &phydata);
		if (ret != 0) {
			NMSGPR_ERR("read phy cable status:read port-%d device3 reg 0x8517 for origin para failed\n", phyid);
			req->data = 0xFF;
			return ret;
		}

		phydata = (phydata & 0xFF3F) | 0x8000;

		ret = devnp_ntn_eth_smi_glob2_write(pdata, phyid, 3, 0x8517, phydata);
		if (ret != 0) {
			NMSGPR_ERR("read phy cable status:enable vct test failed\n");
			req->data = 0xFF;
			return ret;
		}

		msleep(210);

		while (1){
			phydata = 0;
			ret = devnp_ntn_eth_smi_glob2_read(pdata, phyid, 3, 0x8517, &phydata);
			if (ret != 0) {
				NMSGPR_ERR("read phy cable status:read port-%d device3 reg 0x8517 for vct test status failed\n", phyid);
				req->data = 0xFF;
				return ret;
			}

			if (((phydata & 0x4000) >> 14) != 1)
				msleep(20);
			else
				break;
			cnt++;
			if (cnt>6)
			{
				req->data = 0xFF;
				return ret;
			}
		}

		phydata = 0;
		ret = devnp_ntn_eth_smi_glob2_read(pdata, phyid, 3, 0x8510, &phydata);
		if (ret != 0) {
			NMSGPR_ERR("read phy cable status:read port-%d device3 reg 0x8510 for tdr result failed\n", phyid);
			req->data = 0xFF;
			return ret;
		}

		if (phydata == 0x00FF) {
			req->data = 0xFE; //polarity is negative, distance is 0xFF
			return ret;
		} else if (phydata == 0x8000) {
			cable_status = 0;
			goto good_cable;
		}

		vct_polarity = phydata >> 15;
		vct_dis = phydata & 0x00FF;

		if (vct_dis == 0 && vct_polarity == 1){
			cable_status = 't';
		}
		else if (vct_polarity == 1){
			cable_status = 'o';
		}
		else{
			cable_status = 's';
		}
	}

good_cable:
	setPhyMasterSlave(pdata, phyid, origMS);
	msleep(5);

	req->data = cable_status;

	DBGPR("call phy cable status phyid:%d rd %04x\n", phyid, req->data);

	return ret;
}
