/* ============================================================================
* COPYRIGHT © 2015
*
* Toshiba America Electronic Components
*
* PROJECT:   NEUTRINO
*
* Permission is hereby granted,
* free of charge, to any person obtaining a copy of this software annotated
* with this license and the Software, to deal in the Software without
* restriction, including without limitation the rights to use, copy, modify,
* merge, publish, distribute, sublicense, and/or sell copies of the Software,
* and to permit persons to whom the Software is furnished to do so, subject
* to the following conditions:
*
*
* EXAMPLE PROGRAMS ARE PROVIDED AS-IS WITH NO WARRANTY OF ANY KIND,
* EITHER EXPRESS OR IMPLIED.
*
* TOSHIBA ASSUMES NO LIABILITY FOR CUSTOMERS' PRODUCT DESIGN OR APPLICATIONS.
*
* THIS SOFTWARE IS PROVIDED AS-IS AND HAS NOT BEEN FULLY TESTED.  IT IS
* INTENDED FOR REFERENCE USE ONLY.
*
* TOSHIBA DISCLAIMS ALL EXPRESS AND IMPLIED WARRANTIES AND ALL LIABILITY OR
* ANY DAMAGES ASSOCIATED WITH YOUR USE OF THIS SOFTWARE.
*
* THIS SOFTWARE IS BEING DISTRIBUTED BY TOSHIBA SOLELY ON AN "AS IS" BASIS
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE HEREBY DISCLAIMED. IN NO EVENT SHALL TOSHIBA BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
*
* ========================================================================= */

/* =========================================================================
* The Synopsys DWC ETHER QOS Software Driver and documentation (hereinafter
* "Software") is an unsupported proprietary work of Synopsys, Inc. unless
* otherwise expressly agreed to in writing between Synopsys and you.
*
* The Software IS NOT an item of Licensed Software or Licensed Product under
* any End User Software License Agreement or Agreement for Licensed Product
* with Synopsys or any supplement thereto.  Permission is hereby granted,
* free of charge, to any person obtaining a copy of this software annotated
* with this license and the Software, to deal in the Software without
* restriction, including without limitation the rights to use, copy, modify,
* merge, publish, distribute, sublicense, and/or sell copies of the Software,
* and to permit persons to whom the Software is furnished to do so, subject
* to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THIS SOFTWARE IS BEING DISTRIBUTED BY SYNOPSYS SOLELY ON AN "AS IS" BASIS
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE HEREBY DISCLAIMED. IN NO EVENT SHALL SYNOPSYS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
* ========================================================================= */

/*! History:
 *      2-March-2016 : Initial
 *        3-Jun-2016 : Added prints for debug
 */

/*!@file: DWC_ETH_QOS_mdio.c
 * @brief: Driver functions.
 */
#include "DWC_ETH_QOS_yheader.h"
#include "DWC_ETH_QOS_yregacc.h"
#include <soc/qcom/boot_stats.h>

void DWC_ETH_QOS_reload_fqtss_cfg(struct DWC_ETH_QOS_prv_data *pdata);

/*!
* \brief read MII PHY register, function called by the driver alone
*
* \details Read MII registers through the API read_phy_reg where the
* related MAC registers can be configured.
*
* \param[in] pdata - pointer to driver private data structure.
* \param[in] phyaddr - the phy address to read
* \param[in] phyreg - the phy regiester id to read
* \param[out] phydata - pointer to the value that is read from the phy registers
*
* \return int
*
* \retval  0 - successfully read data from register
* \retval -1 - error occurred
* \retval  1 - if the feature is not defined.
*/

INT DWC_ETH_QOS_mdio_read_direct(struct DWC_ETH_QOS_prv_data *pdata,
				 int phyaddr, int phyreg, int *phydata)
{
	struct hw_if_struct *hw_if = &(pdata->hw_if);
	int phy_reg_read_status;
	int dev_type=0;

	DBGPR_MDIO("--> DWC_ETH_QOS_mdio_read_direct: phyaddr = %d, phyreg = %d\n",
		phyaddr, phyreg);

	if (!pdata->enable_phy) {
		NMSGPR_ALERT("%s: PHY is not supported.\n", __func__);
		return -1;
	}

	if (phyreg & MII_ADDR_C45) {
		dev_type = ((phyreg >> 16) & 0x1f);
		if (hw_if->read_phy_regs_c45){
			phy_reg_read_status =
				hw_if->read_phy_regs_c45(phyaddr, dev_type, phyreg, phydata, pdata);
		}else {
			phy_reg_read_status = 1;
			NMSGPR_ALERT( "%s: hw_if->read_phy_regs_c45 not defined",
			   DEV_NAME);
		}
	}else {
		if (hw_if->read_phy_regs) {
			phy_reg_read_status =
				hw_if->read_phy_regs(phyaddr, phyreg, phydata, pdata);
		}else {
			phy_reg_read_status = 1;
			NMSGPR_ALERT( "%s: hw_if->read_phy_regs not defined",
				DEV_NAME);
		}
	}

	DBGPR_MDIO("<-- DWC_ETH_QOS_mdio_read_direct: phydata = %#x\n", *phydata);

	return phy_reg_read_status;
}

/*!
* \brief write MII PHY register, function called by the driver alone
*
* \details Writes MII registers through the API write_phy_reg where the
* related MAC registers can be configured.
*
* \param[in] pdata - pointer to driver private data structure.
* \param[in] phyaddr - the phy address to write
* \param[in] phyreg - the phy regiester id
*
* to write
* \param[out] phydata - actual data to be written into the phy registers
*
* \return void
*
* \retval  0 - successfully read data from register
* \retval -1 - error occurred
* \retval  1 - if the feature is not defined.
*/

INT DWC_ETH_QOS_mdio_write_direct(struct DWC_ETH_QOS_prv_data *pdata,
				  int phyaddr, int phyreg, int phydata)
{
	struct hw_if_struct *hw_if = &(pdata->hw_if);
	int phy_reg_write_status;
	int dev_type=0;

	DBGPR_MDIO("--> DWC_ETH_QOS_mdio_write_direct: phyaddr = %d, phyreg = %d, phydata = %#x\n",
	      phyaddr, phyreg, phydata);

	if (!pdata->enable_phy) {
		NMSGPR_ALERT("%s: PHY is not supported.\n", __func__);
		return -1;
	}

	if (phyreg & MII_ADDR_C45) {
		dev_type = ((phyreg >> 16) & 0x1f);
		if (hw_if->write_phy_regs_c45){
			phy_reg_write_status =
				hw_if->write_phy_regs_c45(phyaddr, dev_type, phyreg, phydata, pdata);
		} else {
			phy_reg_write_status = 1;
			NMSGPR_ALERT( "%s: hw_if->write_phy_regs_c45 not defined",
				   DEV_NAME);
		}
	} else {
		if (hw_if->write_phy_regs) {
			phy_reg_write_status =
				hw_if->write_phy_regs(phyaddr, phyreg, phydata, pdata);
		} else {
			phy_reg_write_status = 1;
			NMSGPR_ALERT( "%s: hw_if->write_phy_regs not defined",
				DEV_NAME);
		}
	}

	DBGPR_MDIO("<-- DWC_ETH_QOS_mdio_write_direct\n");

	return phy_reg_write_status;
}

/*!
* \brief read MII PHY register.
*
* \details Read MII registers through the API read_phy_reg where the
* related MAC registers can be configured.
*
* \param[in] bus - points to the mii_bus structure
* \param[in] phyaddr - the phy address to write
* \param[in] phyreg - the phy register offset to write
*
* \return int
*
* \retval  - value read from given phy register
*/

static INT DWC_ETH_QOS_mdio_read(struct mii_bus *bus, int phyaddr, int phyreg)
{
	struct net_device *dev = bus->priv;
	struct DWC_ETH_QOS_prv_data *pdata = netdev_priv(dev);
	struct hw_if_struct *hw_if = &(pdata->hw_if);
	int phydata;
	int dev_type=0;

	if (!pdata->enable_phy) {
		NMSGPR_ALERT("%s: PHY is not supported.\n", __func__);
		return -1;
	}

	DBGPR_MDIO("--> DWC_ETH_QOS_mdio_read: phyaddr = %d, phyreg = %d\n",
	      phyaddr, phyreg);

	if (phyreg & MII_ADDR_C45) {
		dev_type = ((phyreg >> 16) & 0x1f);
		if (hw_if->read_phy_regs_c45){
			hw_if->read_phy_regs_c45(phyaddr, dev_type, phyreg, &phydata, pdata);
		} else {
			NMSGPR_ALERT( "%s: hw_if->read_phy_regs_c45 not defined",
			DEV_NAME);
		}
	} else {
		if (hw_if->read_phy_regs) {
			hw_if->read_phy_regs(phyaddr, phyreg, &phydata, pdata);
		} else {
			NMSGPR_ALERT( "%s: hw_if->read_phy_regs not defined",
				DEV_NAME);
		}
	}

	DBGPR_MDIO("<-- DWC_ETH_QOS_mdio_read: phydata = %#x\n", phydata);

	return phydata;
}

/*!
* \brief API to write MII PHY register
*
* \details This API is expected to write MII registers with the value being
* passed as the last argument which is done in write_phy_regs API
* called by this function.
*
* \param[in] bus - points to the mii_bus structure
* \param[in] phyaddr - the phy address to write
* \param[in] phyreg - the phy register offset to write
* \param[in] phydata - the register value to write with
*
* \return 0 on success and -ve number on failure.
*/

static INT DWC_ETH_QOS_mdio_write(struct mii_bus *bus, int phyaddr, int phyreg,
				  u16 phydata)
{
	struct net_device *dev = bus->priv;
	struct DWC_ETH_QOS_prv_data *pdata = netdev_priv(dev);
	struct hw_if_struct *hw_if = &(pdata->hw_if);
	INT ret = Y_SUCCESS;
	int dev_type=0;

	DBGPR_MDIO("--> DWC_ETH_QOS_mdio_write: phyaddr = %d, phyreg = %d, phydata = %#x\n",
	      phyaddr, phyreg, phydata);

	/* Prevent PHY powerdown during WOL */
	if (phyreg == MII_BMCR && pdata->wolopts == WAKE_MAGIC)
		phydata &= ~BMCR_PDOWN;

        if (!pdata->enable_phy) {
                NMSGPR_ALERT("%s: PHY is not supported.\n", __func__);
                return -1;
        }

	if (phyreg & MII_ADDR_C45) {
		dev_type = ((phyreg >> 16) & 0x1f);
		if (hw_if->write_phy_regs_c45){
			hw_if->write_phy_regs_c45(phyaddr, dev_type, phyreg, phydata, pdata);
		}else {
			ret = -1;
			NMSGPR_ALERT( "%s: hw_if->write_phy_regs_c45 not defined",
				DEV_NAME);
		}
	}
	else {
		if (hw_if->write_phy_regs) {
			hw_if->write_phy_regs(phyaddr, phyreg, phydata, pdata);
		} else {
			ret = -1;
			NMSGPR_ALERT( "%s: hw_if->write_phy_regs not defined",
				DEV_NAME);
		}
	}

	DBGPR_MDIO("<-- DWC_ETH_QOS_mdio_write\n");

	return ret;
}


/*!
* \brief API to reset PHY
*
* \details This API is issue soft reset to PHY core and waits
* until soft reset completes.
*
* \param[in] bus - points to the mii_bus structure
*
* \return 0 on success and -ve number on failure.
*/

static INT DWC_ETH_QOS_mdio_reset(struct mii_bus *bus)
{
	struct net_device *dev = bus->priv;
	struct DWC_ETH_QOS_prv_data *pdata = netdev_priv(dev);
	struct hw_if_struct *hw_if = &(pdata->hw_if);
	INT phydata;

	DBGPR_MDIO("-->DWC_ETH_QOS_mdio_reset: phyaddr : %d\n", pdata->phyaddr);

        if (!pdata->enable_phy) {
                NMSGPR_ALERT("%s: PHY is not supported.\n", __func__);
                return -ENODEV;
        }

	hw_if->read_phy_regs(pdata->phyaddr, MII_BMCR, &phydata, pdata);

	if (phydata < 0)
		return 0;

	/* issue soft reset to PHY */
	phydata |= BMCR_RESET;
	hw_if->write_phy_regs(pdata->phyaddr, MII_BMCR, phydata, pdata);

	/* wait until software reset completes */
	do {
		hw_if->read_phy_regs(pdata->phyaddr, MII_BMCR, &phydata, pdata);
	} while ((phydata >= 0) && (phydata & BMCR_RESET));

	DBGPR_MDIO("<--DWC_ETH_QOS_mdio_reset\n");

	return 0;
}

/*!
 * \details This function is invoked by other functions to get the PHY register
 * dump. This function is used during development phase for debug purpose.
 *
 * \param[in] pdata – pointer to private data structure.
 *
 * \return 0
 */

void dump_phy_registers(struct DWC_ETH_QOS_prv_data *pdata)
{
	int phydata = 0;

	NMSGPR_ALERT(
	       "\n************* PHY Reg dump *************************\n");
	DWC_ETH_QOS_mdio_read_direct(pdata, pdata->phyaddr, MII_BMCR, &phydata);
	NMSGPR_ALERT(
	       "Phy Control Reg(Basic Mode Control Reg) (%#x) = %#x\n",
	       MII_BMCR, phydata);

	DWC_ETH_QOS_mdio_read_direct(pdata, pdata->phyaddr, MII_BMSR, &phydata);
	NMSGPR_ALERT( "Phy Status Reg(Basic Mode Status Reg) (%#x) = %#x\n",
	       MII_BMSR, phydata);

	DWC_ETH_QOS_mdio_read_direct(pdata, pdata->phyaddr, MII_PHYSID1,
	    &phydata);
	NMSGPR_ALERT( "Phy Id (PHYS ID 1) (%#x)= %#x\n", MII_PHYSID1,
	    phydata);

	DWC_ETH_QOS_mdio_read_direct(pdata, pdata->phyaddr, MII_PHYSID2,
	    &phydata);
	NMSGPR_ALERT( "Phy Id (PHYS ID 2) (%#x)= %#x\n", MII_PHYSID2,
	    phydata);

	DWC_ETH_QOS_mdio_read_direct(pdata, pdata->phyaddr, MII_ADVERTISE,
	    &phydata);
	NMSGPR_ALERT( "Auto-nego Adv (Advertisement Control Reg)"\
	    " (%#x) = %#x\n", MII_ADVERTISE, phydata);

	/* read Phy Control Reg */
	DWC_ETH_QOS_mdio_read_direct(pdata, pdata->phyaddr, MII_LPA,
	    &phydata);
	NMSGPR_ALERT( "Auto-nego Lap (Link Partner Ability Reg)"\
	    " (%#x)= %#x\n", MII_LPA, phydata);

	DWC_ETH_QOS_mdio_read_direct(pdata, pdata->phyaddr, MII_EXPANSION,
	    &phydata);
	NMSGPR_ALERT( "Auto-nego Exp (Extension Reg)"\
	    "(%#x) = %#x\n", MII_EXPANSION, phydata);

	DWC_ETH_QOS_mdio_read_direct(pdata, pdata->phyaddr,
	    DWC_ETH_QOS_AUTO_NEGO_NP, &phydata);
	NMSGPR_ALERT( "Auto-nego Np (%#x) = %#x\n",
	    DWC_ETH_QOS_AUTO_NEGO_NP, phydata);

	DWC_ETH_QOS_mdio_read_direct(pdata, pdata->phyaddr, MII_ESTATUS,
				     &phydata);
	NMSGPR_ALERT( "Extended Status Reg (%#x) = %#x\n", MII_ESTATUS,
	       phydata);

	DWC_ETH_QOS_mdio_read_direct(pdata, pdata->phyaddr, MII_CTRL1000,
	    &phydata);
	NMSGPR_ALERT( "1000 Ctl Reg (1000BASE-T Control Reg)"\
	    "(%#x) = %#x\n", MII_CTRL1000, phydata);

	DWC_ETH_QOS_mdio_read_direct(pdata, pdata->phyaddr, MII_STAT1000,
	    &phydata);
	NMSGPR_ALERT( "1000 Sts Reg (1000BASE-T Status)(%#x) = %#x\n",
	       MII_STAT1000, phydata);

	DWC_ETH_QOS_mdio_read_direct(pdata, pdata->phyaddr, DWC_ETH_QOS_PHY_CTL,
			&phydata);
	NMSGPR_ALERT( "PHY Ctl Reg (%#x) = %#x\n", DWC_ETH_QOS_PHY_CTL,
	    phydata);

	DWC_ETH_QOS_mdio_read_direct(pdata, pdata->phyaddr,
	    DWC_ETH_QOS_PHY_STS, &phydata);
	NMSGPR_ALERT( "PHY Sts Reg (%#x) = %#x\n", DWC_ETH_QOS_PHY_STS,
	    phydata);

	NMSGPR_ALERT(
	       "\n****************************************************\n");
}

void DWC_ETH_QOS_init_eee_handler(struct work_struct *work)
{
	struct DWC_ETH_QOS_prv_data *pdata = container_of(work,
					struct DWC_ETH_QOS_prv_data, eee_work);

	if (!pdata->pcie_link_enable) {
		NMSGPR_ALERT("%s: PCI link is disabled returning.\n", __func__);
		return;
	}

	if (!pdata->eee_active)
		pdata->eee_enabled = DWC_ETH_QOS_eee_init(pdata);

	return;
}

int DWC_ETH_QOS_read_status(struct DWC_ETH_QOS_prv_data *pdata)
{
	int adv;
	int lpa;
	int lpagb = 0;
	int common_adv;
	int common_adv_gb = 0;
	INT phydata;

	/* Update the link, but return if there was an error */
	DWC_ETH_QOS_mdio_read_direct(pdata, pdata->phyaddr, MII_BMSR, &phydata);
	if ((phydata & BMSR_LSTATUS) == 0)
		pdata->phydev->link = 0;
	else
		pdata->phydev->link = 1;

	pdata->phydev->lp_advertising = 0;

	if (AUTONEG_ENABLE == pdata->phydev->autoneg) {
		if (pdata->phydev->supported & (SUPPORTED_1000baseT_Half
					| SUPPORTED_1000baseT_Full)) {
			DWC_ETH_QOS_mdio_read_direct(pdata, pdata->phyaddr, MII_STAT1000, &lpagb);
			if (lpagb < 0)
				return lpagb;

			DWC_ETH_QOS_mdio_read_direct(pdata, pdata->phyaddr, MII_CTRL1000, &adv);
			if (adv < 0)
				return adv;

			pdata->phydev->lp_advertising =
				mii_stat1000_to_ethtool_lpa_t(lpagb);
			common_adv_gb = lpagb & adv << 2;
		}

		DWC_ETH_QOS_mdio_read_direct(pdata, pdata->phyaddr, MII_LPA, &lpa);
		if (lpa < 0)
			return lpa;

		pdata->phydev->lp_advertising |= mii_lpa_to_ethtool_lpa_t(lpa);

		DWC_ETH_QOS_mdio_read_direct(pdata, pdata->phyaddr, MII_ADVERTISE, &adv);
		if (adv < 0)
			return adv;

		common_adv = lpa & adv;

		pdata->phydev->speed = SPEED_10;
		pdata->phydev->duplex = DUPLEX_HALF;
		pdata->phydev->pause = 0;
		pdata->phydev->asym_pause = 0;

		if (common_adv_gb & (LPA_1000FULL | LPA_1000HALF)) {
			pdata->phydev->speed = SPEED_1000;

			if (common_adv_gb & LPA_1000FULL)
				pdata->phydev->duplex = DUPLEX_FULL;
		} else if (common_adv & (LPA_100FULL | LPA_100HALF)) {
			pdata->phydev->speed = SPEED_100;

			if (common_adv & LPA_100FULL)
				pdata->phydev->duplex = DUPLEX_FULL;
		} else
			if (common_adv & LPA_10FULL)
				pdata->phydev->duplex = DUPLEX_FULL;

		if (pdata->phydev->duplex == DUPLEX_FULL) {
			pdata->phydev->pause = lpa & LPA_PAUSE_CAP ? 1 : 0;
			pdata->phydev->asym_pause = lpa & LPA_PAUSE_ASYM ? 1 : 0;
		}
	} else {
		int bmcr = 0;
		DWC_ETH_QOS_mdio_read_direct(pdata, pdata->phyaddr, MII_BMCR, &bmcr);

		if (bmcr < 0)
			return bmcr;

		if (bmcr & BMCR_FULLDPLX)
			pdata->phydev->duplex = DUPLEX_FULL;
		else
			pdata->phydev->duplex = DUPLEX_HALF;

		if (bmcr & BMCR_SPEED1000)
			pdata->phydev->speed = SPEED_1000;
		else if (bmcr & BMCR_SPEED100)
			pdata->phydev->speed = SPEED_100;
		else
			pdata->phydev->speed = SPEED_10;

		pdata->phydev->pause = 0;
		pdata->phydev->asym_pause = 0;
	}

	return 0;
}

void DWC_ETH_QOS_adjust_link_isr(struct DWC_ETH_QOS_prv_data *pdata)
{
	struct hw_if_struct *hw_if = &(pdata->hw_if);
	struct phy_device *phydev = pdata->phydev;
	int new_state = 0;

	DBGPR_MDIO("-->DWC_ETH_QOS_adjust_link_isr. address %d link %d\n", phydev->addr,
		phydev->link);

	if (!pdata->enable_phy) {
		NMSGPR_ALERT("%s: PHY is not supported.\n", __func__);
		return;
	}
	if (phydev == NULL) {
		NMSGPR_ALERT("%s: PHY dev is NULL\n", __func__);
		return;
	}

	/* Ignore adjust_link interrupts if PCI link is down. PCI
		 * link down event will eventually stop the phy state machine */
	if (pdata->pdev->error_state != pci_channel_io_normal) {
		NMSGPR_ALERT("%s: PCI link is down returning.\n", __func__);
		return;
	}

	DWC_ETH_QOS_read_status(pdata);

	if (phydev->link) {
		/* Now we make sure that we can be in full duplex mode.
		 * If not, we operate in half-duplex mode */
		if (phydev->duplex != pdata->oldduplex) {
			new_state = 1;
			if (phydev->duplex)
				hw_if->set_full_duplex(pdata);
			else
				hw_if->set_half_duplex(pdata);
			pdata->oldduplex = phydev->duplex;
		}
		/* FLOW ctrl operation */
		if (phydev->pause || phydev->asym_pause) {
			if (pdata->flow_ctrl != pdata->oldflow_ctrl)
				DWC_ETH_QOS_configure_flow_ctrl(pdata);
		}
		if (phydev->speed != pdata->speed) {
			new_state = 1;
			switch (phydev->speed) {
			case SPEED_1000:
				hw_if->set_gmii_speed(pdata);
				hw_if->ntn_set_tx_clk_125MHz(pdata);
				break;
			case SPEED_100:
				hw_if->set_mii_speed_100(pdata);
				hw_if->ntn_set_tx_clk_25MHz(pdata);
				break;
			case SPEED_10:
				hw_if->set_mii_speed_10(pdata);
				hw_if->ntn_set_tx_clk_2_5MHz(pdata);
				break;
			}
			pdata->speed = phydev->speed;
		}

		if (!pdata->oldlink) {
			new_state = 1;
			pdata->oldlink = 1;
			netif_carrier_on(pdata->dev);
		}
		DBGPR_MDIO("M - Driver Ethernet Link up detected");
		place_marker("M - Driver Ethernet Link up detected");
	} else if (pdata->oldlink) {
		new_state = 1;
		pdata->oldlink = 0;
		pdata->speed = 0;
		pdata->oldduplex = -1;
		netif_carrier_off(pdata->dev);
		DBGPR_MDIO("M - Driver Ethernet Link down detected");
		place_marker("M - Driver Ethernet Link down detected");
	}
	if (new_state) {
		phy_print_status(phydev);
	}

	if (new_state)
		DWC_ETH_QOS_reload_fqtss_cfg(pdata);

	schedule_work(&pdata->eee_work);

	DBGPR_MDIO("<--DWC_ETH_QOS_adjust_link_isr\n");
}

INT DWC_ETH_QOS_mdio_config_intr(struct DWC_ETH_QOS_prv_data *pdata, UINT flag)
{
	struct hw_if_struct *hw_if = &(pdata->hw_if);
	INT ret = Y_SUCCESS;
	INT phydata;
	unsigned long varmac_imr;

	DBGPR_MDIO("--> DWC_ETH_QOS_mdio_config_intr: phyaddr = %d, phyreg = %d, phydata = %#x\n",
	      pdata->phyaddr, DWC_ETH_QOS_PHY_INT_REG, DWC_ETH_QOS_PHY_INT_MASK_DEFAULT);

	if (flag == PHY_INTR_ENABLED){
		//set phy interrupt to active high
		DWC_ETH_QOS_mdio_read_direct(pdata,pdata->phyaddr, DWC_ETH_QOS_PHY_CTRL_REG, &phydata);
		phydata |= (1 << 14);
		hw_if->write_phy_regs(pdata->phyaddr,DWC_ETH_QOS_PHY_CTRL_REG,phydata, pdata);

		DWC_ETH_QOS_mdio_read_direct(pdata,pdata->phyaddr, DWC_ETH_QOS_PHY_CTRL_REG, &phydata);

		hw_if->write_phy_regs(pdata->phyaddr, DWC_ETH_QOS_PHY_INT_REG, DWC_ETH_QOS_PHY_INT_MASK_DEFAULT, pdata);

		/* Enable following interrupts */
		/* PHYIE - PHY Interrupt Enable */
		MAC_IMR_RgRd(varmac_imr);
		varmac_imr = varmac_imr | ((0x1) << 3);
		MAC_IMR_RgWr(varmac_imr);
		MAC_IMR_RgRd(varmac_imr);
	} else {
		/* Disable following interrupts */
		/* PHYIE - PHY Interrupt Disable */
		MAC_IMR_RgRd(varmac_imr);
		varmac_imr &= ~((0x1) << 3);
		MAC_IMR_RgWr(varmac_imr);
		MAC_IMR_RgRd(varmac_imr);
		hw_if->write_phy_regs(pdata->phyaddr, DWC_ETH_QOS_PHY_INT_REG, 0, pdata);
	}

	DBGPR_MDIO("<-- DWC_ETH_QOS_mdio_config_intr\n");

	return ret;
}

INT DWC_ETH_QOS_mdio_ack_intr(struct DWC_ETH_QOS_prv_data *pdata)
{
	struct hw_if_struct *hw_if = &(pdata->hw_if);
	INT ret = Y_SUCCESS;
	INT phydata;

	DBGPR_MDIO("--> DWC_ETH_QOS_mdio_ack_intr: phyaddr = %d\n", pdata->phyaddr);

	/* Clear the interrupt status bit by reading phy control register (bits 0-7). */
	hw_if->read_phy_regs(pdata->phyaddr, DWC_ETH_QOS_PHY_INT_REG, &phydata, pdata);

	DBGPR_MDIO("<-- DWC_ETH_QOS_mdio_ack_intr\n");

	return ret;
}
/*
extern irqreturn_t DWC_ETH_QOS_ISR_SW_DWC_ETH_QOS(int irq, void *device_id);
void DWC_ETH_QOS_intr_hardl(struct work_struct *work)
{
	struct DWC_ETH_QOS_prv_data *pdata = container_of(work,
					struct DWC_ETH_QOS_prv_data,
					intr_hardl.work);

	//DBGPR_MDIO("-->DWC_ETH_QOS_intr_hardl\n");
	
	DWC_ETH_QOS_ISR_SW_DWC_ETH_QOS(pdata->irq_number, pdata);

	schedule_delayed_work(&pdata->intr_hardl, usecs_to_jiffies(1000));
}*/
/*!
* \brief API to adjust link parameters.
*
* \details This function will be called by PAL to inform the driver
* about various link parameters like duplex and speed. This function
* will configure the MAC based on link parameters.
*
* \param[in] dev - pointer to net_device structure
*
* \return void
*/
void DWC_ETH_QOS_adjust_link(struct work_struct *work)
{
	struct DWC_ETH_QOS_prv_data *pdata = container_of(work,
					struct DWC_ETH_QOS_prv_data,
					phy_dwork.work);
	struct hw_if_struct *hw_if = &(pdata->hw_if);
	struct phy_device *phydev = pdata->phydev;
	//unsigned long flags;
	int new_state = 0;

	if (pdata->enable_phy_interrupt) {
		NMSGPR_ALERT("%s: PHY interrupt enabled. stop polling\n", __func__);
		return;
	}

	if (!pdata->enable_phy) {
		NMSGPR_ALERT("%s: PHY is not supported.\n", __func__);
		return;
	}

	if (phydev == NULL)
		return;

	phydev->link = 1;
	phydev->speed = SPEED_1000;
	//phydev->addr = pdata->phyaddr;
	//DBGPR_MDIO("-->DWC_ETH_QOS_adjust_link. address %x link %d phydev->speed=%d,pdata->phyaddr=%x\n", phydev->addr,
	 //     phydev->link,phydev->speed,pdata->phyaddr);

	/* Ignore adjust_link interrupts if PCI link is down. PCI
	 * link down event will eventually stop the phy state machine */
	if (pdata->pdev->error_state != pci_channel_io_normal) {
		NMSGPR_ALERT("%s: PCI link is down returning.\n", __func__);
		return;
	}

	if (!pdata->pcie_link_enable) {
		NMSGPR_ALERT("%s: PCI link is disabled returning.\n", __func__);
		return;
	}

	//spin_lock_irqsave(&pdata->lock, flags);
	//printk("GinPot %s: phydev->state=%d\n",__func__, phydev->state);
	if (phydev->state != PHY_UP) {
		schedule_delayed_work(&pdata->phy_dwork, usecs_to_jiffies(1000000));
		return;
	}
	genphy_read_status(phydev);

	if (phydev->link) {
		/* Now we make sure that we can be in full duplex mode.
		 * If not, we operate in half-duplex mode */
		if (phydev->duplex != pdata->oldduplex) {
			new_state = 1;
			if (phydev->duplex)
				hw_if->set_full_duplex(pdata);
			else {
				hw_if->set_half_duplex(pdata);
#ifdef DWC_ETH_QOS_CERTIFICATION_PKTBURSTCNT_HALFDUPLEX
				/* For Synopsys testing and debugging only */
				{
					UINT phydata;

					/* setting 'Assert CRS on transmit' */
					phydata = 0;
					DWC_ETH_QOS_mdio_read_direct(pdata, pdata->phyaddr,
						DWC_ETH_QOS_PHY_CTL, &phydata);
					phydata |= (1 << 11);
					DWC_ETH_QOS_mdio_write_direct(pdata, pdata->phyaddr,
						DWC_ETH_QOS_PHY_CTL, phydata);
				}
#endif
			}
			pdata->oldduplex = phydev->duplex;
		}

		/* FLOW ctrl operation */
		if (phydev->pause || phydev->asym_pause) {
			if (pdata->flow_ctrl != pdata->oldflow_ctrl)
				DWC_ETH_QOS_configure_flow_ctrl(pdata);
		}

		if (phydev->speed != pdata->speed) {
			new_state = 1;
			switch (phydev->speed) {
			case SPEED_1000:
				//printk("GinPot111111111: SPEED_1000\n");
				hw_if->set_gmii_speed(pdata);
				hw_if->ntn_set_tx_clk_125MHz(pdata);
				break;
			case SPEED_100:
				//printk("GinPot111111111: SPEED_100\n");
				hw_if->set_mii_speed_100(pdata);
				hw_if->ntn_set_tx_clk_25MHz(pdata);
				break;
			case SPEED_10:
				//printk("GinPot111111111: SPEED_10\n");
				hw_if->set_mii_speed_10(pdata);
				hw_if->ntn_set_tx_clk_2_5MHz(pdata);
				break;
			}
			pdata->speed = phydev->speed;
		}

		if (!pdata->oldlink) {
			new_state = 1;
			pdata->oldlink = 1;
			netif_carrier_on(pdata->dev);
		}
	} else if (pdata->oldlink) {
		new_state = 1;
		pdata->oldlink = 0;
		pdata->speed = 0;
		pdata->oldduplex = -1;
		netif_carrier_off(pdata->dev);
	}

	if (new_state)
		phy_print_status(phydev);

	if (new_state)
		DWC_ETH_QOS_reload_fqtss_cfg(pdata);

	/* At this stage, it could be need to setup the EEE or adjust some
	 * MAC related HW registers.
	 * */
	if (!pdata->eee_active)
		pdata->eee_enabled = DWC_ETH_QOS_eee_init(pdata);

	schedule_delayed_work(&pdata->phy_dwork, usecs_to_jiffies(1000000));

	//spin_unlock_irqrestore(&pdata->lock, flags);

	DBGPR_MDIO("<--DWC_ETH_QOS_adjust_link\n");
}

static void DWC_ETH_QOS_micrel_ksz9031_phy_init(struct DWC_ETH_QOS_prv_data *pdata)
{
	int val[2] = {pdata->phy_rxc_skew_ps, pdata->phy_txc_skew_ps};
	u16 mask = 0;
	u16 maxval = 0;
	u16 newval = 0;
	int i = 0;

	/* Apply clock skew settings */
	DBGPR_MDIO("Detected the ksz9031 phy, applying specific settings\n");

	if ((val[0] < 0) && (val[1] < 0)) {
		DBGPR_MDIO("Micrel PHY properties not defined\n");
		return;
	}

	/* Only read from PHY if either one of the phy settings is
	 * not specified */
	if ((val[0] < 0) || (val[1] < 0)) {
		newval = DWC_ETH_QOS_phy_read_mmd_indirect(pdata->phydev->bus,
							   MICREL_PHY_SKEW_REGADDR,
							   MICREL_PHY_SKEW_DEVADDR,
							   pdata->phyaddr);
	}

	maxval = (MICREL_KSZ9031_PHY_SKEW_BITS == 4) ? 0xf : 0x1f;
	for (i = 0; i < 2; i++) {
		if (val[i] != -(i + 1)) {
			mask = 0xffff;
			mask ^= maxval << (MICREL_KSZ9031_PHY_SKEW_BITS * i);
			newval = (newval & mask) |
				 (((val[i] / MICREL_KSZ9031_PS_TO_REG) & maxval)
				 << (MICREL_KSZ9031_PHY_SKEW_BITS * i));
		}
	}

	DWC_ETH_QOS_phy_write_mmd_indirect(pdata->phydev->bus,
					   MICREL_PHY_SKEW_REGADDR,
					   MICREL_PHY_SKEW_DEVADDR,
					   pdata->phyaddr, newval);

	DBGPR_MDIO("%s: Updated PHY clock skew: 0x%x\n", __func__, newval);
}

/*!
* \brief API to set specific PHY registers.
*
* \details This function will identify the PHY and set specific phy registers
* based on its specifications.
*
* \param[in] pdata - pointer to DWC_ETH_QOS_prv_data
*
* \return integer
*
* \retval 0 on success & negative number on failure.
*/
static int DWC_ETH_QOS_set_phy_config(struct DWC_ETH_QOS_prv_data *pdata)
{
	int regval = 0;
	int regval_ext = 0;

	if ((pdata->phy_rxc_skew_ps < 0) && (pdata->phy_txc_skew_ps < 0)) {
		DBGPR_MDIO("PHY properties not defined\n");
		return 0;
	}

	/* Set specific PHY registers */
	if (pdata->phydev != NULL) {
		regval = phy_read(pdata->phydev, MII_PHYSID1);
		if (regval < 0)
			return regval;

		regval_ext = phy_read(pdata->phydev, MII_PHYSID2);
		if (regval_ext < 0)
			return regval_ext;

		regval = ((regval << 16) | regval_ext);
		if ((regval & MICREL_KSZ9031_PHY_ID_MASK) == MICREL_KSZ9031_PHY_ID)
			DWC_ETH_QOS_micrel_ksz9031_phy_init(pdata);
		else
			DBGPR_MDIO("No specific init settings for this phy\n");
	}

	return 0;
}

/*!
* \brief API to initialize PHY.
*
* \details This function will initializes the driver's PHY state and attaches
* the PHY to the MAC driver.
*
* \param[in] dev - pointer to net_device structure
*
* \return integer
*
* \retval 0 on success & negative number on failure.
*/

static int DWC_ETH_QOS_init_phy(struct net_device *dev)
{
	struct DWC_ETH_QOS_prv_data *pdata = netdev_priv(dev);
	struct phy_device *phydev = NULL;
	int regval = 0;

	DBGPR_MDIO("-->DWC_ETH_QOS_init_phy\n");

	if (!pdata->enable_phy) {
		NMSGPR_ALERT("%s: PHY is not supported.\n", __func__);
		return -ENODEV;
	}

	phydev = kzalloc(sizeof(*phydev), GFP_KERNEL);
	if (NULL == phydev) {
		NMSGPR_ALERT("%s: PHY device create failed.\n", __func__);
		return PTR_ERR(phydev);
	}

	phydev->speed = SPEED_10;
	phydev->duplex = 0;
	phydev->pause = 0;
	phydev->asym_pause = 0;
	phydev->link = 0;
	phydev->interface = pdata->interface;
	phydev->autoneg = AUTONEG_ENABLE;
	phydev->addr = pdata->phyaddr;
	phydev->phy_id = pdata->phyaddr;
	phydev->state = PHY_READY;
	phydev->bus = pdata->mii;
	mutex_init(&phydev->lock);

	if ((pdata->interface == PHY_INTERFACE_MODE_GMII) ||
		(pdata->interface == PHY_INTERFACE_MODE_RGMII)) {
		phydev->supported |= PHY_GBIT_FEATURES;
#ifdef DWC_ETH_QOS_CERTIFICATION_PKTBURSTCNT_HALFDUPLEX
		phydev->supported &= ~SUPPORTED_1000baseT_Full;
#endif
	} else if ((pdata->interface == PHY_INTERFACE_MODE_MII) ||
			   (pdata->interface == PHY_INTERFACE_MODE_RMII)) {
		phydev->supported |= PHY_BASIC_FEATURES;
	} else {
		NMSGPR_ALERT("%s : %d: Check me\n", __FUNCTION__,__LINE__);
	}

	phydev->advertising = phydev->supported;
	pdata->phydev = phydev;

	if (pdata->phy_speed_mode != DWC_ETH_QOS_PHY_SPEED_AUTO) {
		phydev->autoneg = AUTONEG_DISABLE;
		regval = 0;
		if (pdata->phy_speed_mode == DWC_ETH_QOS_PHY_SPEED_1000M)
			regval = BMCR_SPEED1000;
		else if (pdata->phy_speed_mode == DWC_ETH_QOS_PHY_SPEED_100M)
			regval = BMCR_SPEED100;

		regval |= BMCR_FULLDPLX;

		DWC_ETH_QOS_mdio_write_direct(pdata, pdata->phyaddr, MII_BMCR, regval);
	}

	/* Set specific PHY registers */
	regval = DWC_ETH_QOS_set_phy_config(pdata);
	if (regval != 0)
		DBGPR_MDIO("Unable to set PHY configuration\n");;

	DBGPR_MDIO("<--DWC_ETH_QOS_init_phy\n");

	return 0;
}

static ssize_t DWC_ETH_QOS_phy_interrupt_enable_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct net_device *ndev = pci_get_drvdata(to_pci_dev(dev));
	struct DWC_ETH_QOS_prv_data *pdata = netdev_priv(ndev);

	return snprintf(buf, PAGE_SIZE, "%d\n", pdata->enable_phy_interrupt);
}

static ssize_t DWC_ETH_QOS_phy_interrupt_enable_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct net_device *ndev = pci_get_drvdata(to_pci_dev(dev));
	struct DWC_ETH_QOS_prv_data *pdata = netdev_priv(ndev);

	sscanf(buf, "%d", &pdata->enable_phy_interrupt);

	if(1 == pdata->enable_phy_interrupt) {
		NMSGPR_ALERT( "DWC_ETH_QOS phy Interrupt enabled\n");
		/*wait enough time to avoid polling task still running*/
		msleep(500);
		DWC_ETH_QOS_mdio_config_intr(pdata, PHY_INTR_ENABLED);
	} else {
		DWC_ETH_QOS_mdio_config_intr(pdata, PHY_INTR_DISABLED);
		/*cleam interrupy status to avoid exception*/
		DWC_ETH_QOS_mdio_ack_intr(pdata);
		schedule_delayed_work(&pdata->phy_dwork, usecs_to_jiffies(1000000));
		NMSGPR_ALERT( "DWC_ETH_QOS phy Interrupt disabled\n");
	}

	return count;
}

static DEVICE_ATTR(phy_inten, S_IRWXU, DWC_ETH_QOS_phy_interrupt_enable_show, DWC_ETH_QOS_phy_interrupt_enable_store);

/*!
* \brief API to register mdio.
*
* \details This function will allocate mdio bus and register it
* phy layer.
*
* \param[in] dev - pointer to net_device structure
*
* \return 0 on success and -ve on failure.
*/

int DWC_ETH_QOS_mdio_register(struct net_device *dev)
{
	struct DWC_ETH_QOS_prv_data *pdata = netdev_priv(dev);
	struct mii_bus *new_bus = NULL;
	int phyaddr = 0;
	unsigned short phy_detected = 0;
	int ret = Y_SUCCESS;
	int phyCheckLoop = 0;

	DBGPR_MDIO("-->DWC_ETH_QOS_mdio_register\n");

	if (!pdata->enable_phy) {
		NMSGPR_ALERT("%s: PHY is not supported.\n", __func__);
		return -ENODEV;
	}

	if (pdata->phyaddr == NTN_INVALID_PHY_ADDR) {
		/* find the phy ID or phy address which is connected to our MAC */
		do{
			for (phyaddr = 0; phyaddr < 32; phyaddr++) {
				int phy_reg_read_status, mii_status;

				phy_reg_read_status =
				    DWC_ETH_QOS_mdio_read_direct(pdata, phyaddr, MII_BMSR,
					&mii_status);
				if (phy_reg_read_status == 0) {
					if (mii_status != 0x0000 && mii_status != 0xffff) {
						NMSGPR_ALERT( "%s: Phy detected at"\
						    " ID/ADDR %d\n", DEV_NAME, phyaddr);
						phy_detected = 1;
						break;
					}
				} else if (phy_reg_read_status < 0) {
					NMSGPR_ALERT( "%s: Error reading the phy register"\
					    " MII_BMSR for phy ID/ADDR %d\n",
					    DEV_NAME, phyaddr);
				}
			}
			if(phy_detected) {
				break;
			}
			phyCheckLoop++;
		}while(phyCheckLoop < 100);

		if (!phy_detected) {
			NMSGPR_ALERT( "%s: No phy could be detected\n", DEV_NAME);
			return -ENOLINK;
		}
		pdata->phyaddr = phyaddr;
	}

	pdata->bus_id = pdata->mdio_bus_id;
	pdata->oldlink = 0;
	pdata->speed = 0;
	pdata->oldduplex = -1;

	DBGPHY_REGS(pdata);

	new_bus = mdiobus_alloc();
	if (new_bus == NULL) {
		NMSGPR_ALERT( "Unable to allocate mdio bus\n");
		return -ENOMEM;
	}

	new_bus->name = "dwc_phy";
	new_bus->read = DWC_ETH_QOS_mdio_read;
	new_bus->write = DWC_ETH_QOS_mdio_write;
	new_bus->reset = DWC_ETH_QOS_mdio_reset;
	snprintf(new_bus->id, MII_BUS_ID_SIZE, "%s-%x", new_bus->name,
		 pdata->bus_id);
	new_bus->priv = dev;
	new_bus->phy_mask = 0;
	new_bus->parent = &pdata->pdev->dev;
	mutex_init(&new_bus->mdio_lock);

	pdata->mii = new_bus;

	INIT_WORK(&pdata->eee_work, DWC_ETH_QOS_init_eee_handler);

	ret = DWC_ETH_QOS_init_phy(dev);
	if (unlikely(ret)) {
		NMSGPR_ALERT( "Cannot attach to PHY (error: %d)\n", ret);
		goto err_out_phy_connect;
	}

	ret = device_create_file(&pdata->pdev->dev, &dev_attr_phy_inten);

	DBGPR_MDIO("<--DWC_ETH_QOS_mdio_register\n");

	return ret;

 err_out_phy_connect:
	DWC_ETH_QOS_mdio_unregister(dev);
	return ret;
}

/*!
* \brief API to unregister mdio.
*
* \details This function will unregister mdio bus and free's the memory
* allocated to it.
*
* \param[in] dev - pointer to net_device structure
*
* \return void
*/

void DWC_ETH_QOS_mdio_unregister(struct net_device *dev)
{
	struct DWC_ETH_QOS_prv_data *pdata = netdev_priv(dev);

	DBGPR_MDIO("-->DWC_ETH_QOS_mdio_unregister\n");

        if (!pdata->enable_phy) {
                NMSGPR_ALERT("%s: PHY is not supported.\n", __func__);
                return;
        }

	if (pdata->phydev) {
		kfree(pdata->phydev);
		pdata->phydev = NULL;
	}

	flush_work(&pdata->eee_work);

	if (pdata->mii) {
		pdata->mii->priv = NULL;
		mdiobus_free(pdata->mii);
		pdata->mii = NULL;
	}

	device_remove_file(&pdata->pdev->dev, &dev_attr_phy_inten);

	DBGPR_MDIO("<--DWC_ETH_QOS_mdio_unregister\n");
}

static void DWC_ETH_QOS_mmd_phy_indirect(struct mii_bus *bus,
					 int regAddr, int devAddr, int phyAddr)
{
	/* Write the desired MMD devAddr */
	bus->write(bus, phyAddr, MMD_CTRL_REG, devAddr);

	/* Write the desired MMD regAddr */
	bus->write(bus, phyAddr, MMD_ADDR_DATA_REG, regAddr);

	/* Select the Function : DATA with no post increment */
	bus->write(bus, phyAddr, MMD_CTRL_REG,
		   (devAddr | MMD_CTRL_FUNC_DATA_NOINCR));
}

/*!
* \brief API to read data from the MMD registers.
*
* \details This function will read data from the MMD(clause 45) registers
* using clause 22 registers. The procedure to read MMD registers is,
* 1. Write the desired MMD device addr into reg 13
* 2. Write the desired MMD reg addr into reg 14
* 3. Select the desired Function - MMD data command by writing in reg 13
* 4. Read the content of the MMD's selected reg through reg 14
*
* \param[in] bus - the target MII bus
* \param[in] regAddr - desired MMD reg addr to be read
* \param[in] devAddr - desired MMD address
* \param[in] phyAddr - PHY addr/id on the MII bus
*
* \return integer
*/
int DWC_ETH_QOS_phy_read_mmd_indirect(struct mii_bus *bus,
				      int regAddr, int devAddr, int phyAddr)
{
	u32 ret;

	DBGPR_MDIO("-->DWC_ETH_QOS_phy_read_mmd_indirect\n");

	DWC_ETH_QOS_mmd_phy_indirect(bus, regAddr, devAddr, phyAddr);
	/* read the content of the MMD's selected register */
	ret = bus->read(bus, phyAddr, MMD_ADDR_DATA_REG);

	DBGPR_MDIO("<--DWC_ETH_QOS_phy_read_mmd_indirect\n");

	return ret;
}

/*!
* \brief API to write data into the MMD registers.
*
* \details This function will write data into MMD(clause 45) registers
* using clause 22 registers. The procedure to write MMD registers is,
* 1. Write the desired MMD device addr into reg 13
* 2. Write the desired MMD reg addr into reg 14
* 3. Select the desired Function - MMD data command by writing in reg 13
* 4. Write the data into MMD's selected reg through reg 14
*
* \param[in] bus - the target MII bus
* \param[in] regAddr - desired MMD reg addr to be written
* \param[in] devAddr - desired MMD address
* \param[in] phyAddr - PHY addr/id on the MII bus
* \param[in] data - data to write into the MMD register
*
* \return void
*/
void DWC_ETH_QOS_phy_write_mmd_indirect(struct mii_bus *bus,
					int regAddr, int devAddr,
					int phyAddr, u32 data)
{
	DBGPR_MDIO("-->DWC_ETH_QOS_phy_write_mmd_indirect\n");

	DWC_ETH_QOS_mmd_phy_indirect(bus, regAddr, devAddr, phyAddr);
	/* Write the data into MMD's selected register */
	bus->write(bus, phyAddr, MMD_ADDR_DATA_REG, data);

	DBGPR_MDIO("<--DWC_ETH_QOS_phy_write_mmd_indirect\n");
}

void DWC_ETH_QOS_enable_phy_wol(struct DWC_ETH_QOS_prv_data *pdata)
{
	uint16_t bmcr_val, ctrl1000_val, adv_val;
	struct hw_if_struct *hw_if = &(pdata->hw_if);

	/* Disable 1000M mode */
	ctrl1000_val = phy_read(pdata->phydev, MII_CTRL1000);
	ctrl1000_val &= ~(ADVERTISE_1000HALF | ADVERTISE_1000FULL);
	phy_write(pdata->phydev, MII_CTRL1000, ctrl1000_val);
	/* Disable 100M mode */
	adv_val = phy_read(pdata->phydev, MII_ADVERTISE);
	adv_val &= ~(ADVERTISE_100HALF | ADVERTISE_100FULL);
	phy_write(pdata->phydev, MII_ADVERTISE, adv_val);
	/* Restart aneg for 10M link */
	bmcr_val = phy_read(pdata->phydev, MII_BMCR);
	bmcr_val |= BMCR_ANRESTART;
	phy_write(pdata->phydev, MII_BMCR, bmcr_val);
	/* Setup MAC for 10M */
	hw_if->set_mii_speed_10(pdata);
	hw_if->ntn_set_tx_clk_2_5MHz(pdata);
}

void DWC_ETH_QOS_disable_phy_wol(struct DWC_ETH_QOS_prv_data *pdata)
{
	uint16_t bmcr_val, ctrl1000_val, adv_val;

	/* Enable 1000M mode */
	ctrl1000_val = phy_read(pdata->phydev, MII_CTRL1000);
	if (pdata->phydev->advertising & ADVERTISED_1000baseT_Half)
		ctrl1000_val |= ADVERTISE_1000HALF;
	if (pdata->phydev->advertising & ADVERTISED_1000baseT_Full)
		ctrl1000_val |= ADVERTISE_1000FULL;
	phy_write(pdata->phydev, MII_CTRL1000, ctrl1000_val);
	/* Enable 100M mode */
	adv_val = phy_read(pdata->phydev, MII_ADVERTISE);
	if (pdata->phydev->advertising & ADVERTISED_100baseT_Half)
		adv_val |= ADVERTISE_100HALF;
	if (pdata->phydev->advertising & ADVERTISED_100baseT_Full)
		adv_val |= ADVERTISE_100FULL;
	phy_write(pdata->phydev, MII_ADVERTISE, adv_val);
	/* Restart aneg */
	bmcr_val = phy_read(pdata->phydev, MII_BMCR);
	bmcr_val |= BMCR_ANRESTART;
	phy_write(pdata->phydev, MII_BMCR, bmcr_val);
}

void DWC_ETH_QOS_mdio_mac_phy_work_handler(struct work_struct *work)
{
	struct DWC_ETH_QOS_prv_data *pdata = container_of(work,
		struct DWC_ETH_QOS_prv_data, mac_phy_work);
	DBGPR_MDIO("<--DWC_ETH_QOS_mdio_mac_phy_work_handler\n");

	if (pdata->enable_phy_interrupt){
		DWC_ETH_QOS_mdio_ack_intr(pdata);
		DWC_ETH_QOS_adjust_link_isr(pdata);
	}
	DBGPR_MDIO("-->DWC_ETH_QOS_mdio_mac_phy_work_handler\n");
}
