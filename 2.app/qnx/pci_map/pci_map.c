#include <stdlib.h>
#include <stdio.h>
#include <devctl.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <unistd.h>
#include <hw/pci.h>
#include <pci/pci.h>
#include <pci/pci_ccode.h>
#include <pci/cap_msi.h>

#define ETHER_ADD_TX	0xAD400000
#define ETHER_ADD_RX	0xAD900000
#define ETHER_SIZE		0x500000

#define DEVNP_ETH_DEFS_PCI_VENDOR_ID	(0x1179)
#define DEVNP_ETH_DEFS_PCI_DEVICE_ID	(0x021A)

int main(int argc, char *argv[])
{
	off64_t phys;
	pci_ba_t buf_tx, buf_rx;
	pci_ba_t xlate_tx, xlate_rx;
	
	pci_devhdl_t hdl; // pci handler
	pci_bdf_t bdf;
	pci_err_t pci_err;
	int idx = 0;
	pci_cap_t cap = NULL;
	int msi_idx;
	int is_enabled = 0;
	
	int ret;


	/* find pci device using vid and did */
	bdf = pci_device_find(idx, DEVNP_ETH_DEFS_PCI_VENDOR_ID,
			DEVNP_ETH_DEFS_PCI_DEVICE_ID, PCI_CCODE_ANY);
	if (bdf == PCI_BDF_NONE) {
	    printf("PCI device not found.\n");
		return 0;
	}
	printf("PCI bus id %d\n", idx);
	
	/* attach found pci device */
	hdl = pci_device_attach(bdf, pci_attachFlags_EXCLUSIVE_OWNER, &pci_err);
	if (hdl == NULL) {
		printf("Failed to attach pci device: %s\n", pci_strerror(pci_err));
		return 0;
	}
	
	msi_idx = pci_device_find_capid(bdf, CAPID_MSI);
	if (msi_idx >= 0) {
		printf("pci_device_find_capid() returned idx: %d\n", msi_idx);
	} else {
		printf("pci_device_find_capid() idx not found\n");
        return 0;
	}
	pci_err = pci_device_read_cap(bdf, &cap, msi_idx);
    if (pci_err != PCI_ERR_OK) {
        printf("pci_device_read_cap failed: %s\n", pci_strerror(pci_err));
        return pci_err;
    }
    /* enable the capability */
    pci_err = pci_device_cfg_cap_enable(hdl, pci_reqType_e_MANDATORY, cap);
    if (pci_err != PCI_ERR_OK) {
        printf("pci_device_cfg_cap_enable() failed: %s\n", pci_strerror(pci_err));
        return 0;
    }
    /* Check MSI cap status */
    is_enabled = pci_device_cfg_cap_isenabled(hdl, cap);
    if (0 == is_enabled) {
        printf("Enable MSI Cap Failed\n");
    }

	phys = ETHER_ADD_TX;
    buf_tx.addr = phys;
    buf_tx.size = ETHER_SIZE;
    buf_tx.type = pci_asType_e_MEM;
    buf_tx.attr = pci_asAttr_e_OUTBOUND | pci_asAttr_e_CONTIG;
    ret = pci_device_map_as(hdl, &buf_tx, &xlate_tx);
    if (ret != PCI_ERR_OK)
    {
        printf("PCI Translate tx error!!! %s\n", pci_strerror(ret));
        //return 0;
    }
	printf("PCI Translate tx dma add : 0x%p\n", (void *)xlate_tx.addr);

	phys = ETHER_ADD_RX;
    buf_rx.addr = phys;
    buf_rx.size = ETHER_SIZE;
    buf_rx.type = pci_asType_e_MEM;
    buf_rx.attr = pci_asAttr_e_INBOUND | pci_asAttr_e_CONTIG;
    ret = pci_device_map_as(hdl, &buf_rx, &xlate_rx);
    if (ret != PCI_ERR_OK)
    {
        printf("PCI Translate rx error!!! %s\n", pci_strerror(ret));
        //return 0;
    }
	printf("PCI Translate rx dma add : 0x%p\n", (void *)xlate_rx.addr);
	
	
	while(1)
	{
		sleep(60);
	}

	return 0;
}

