/*
 * $QNXLicenseC:
 * Copyright 2015, QNX Software Systems. All Rights Reserved.
 *
 * You must obtain a written license from and pay applicable license fees to QNX
 * Software Systems before you may reproduce, modify or distribute this software,
 * or any work that includes all or part of this software.   Free development
 * licenses are available for evaluation and non-commercial purposes.  For more
 * information visit http://licensing.qnx.com or email licensing@qnx.com.
 *
 * This file may contain contributions from others.  Please review this entire
 * file for other proprietary rights or license notices, as well as the QNX
 * Development Suite License Guide at http://licensing.qnx.com/license-guide/
 * for other information.
 * $
 */

/**
 * @file
 * Definitions for virtual PCI devices
 */

#ifndef _QVM_VPCI_H_INCLUDED
#define _QVM_VPCI_H_INCLUDED

#include <stddef.h>
#include <qvm/hw_pci.h>
#include <qvm/vdev.h>

/**
 * Generate the writable field for the slice descriptor of a BAR.
 * TODO: >4G BARs       JIRA:1640533
 * @param   size        size of the BAR
 * @param   flags       bottom bits of the BAR value
 * @return  the set of bits a guest can write for that register.
 */
static inline uint32_t
vpci_bar_writable(uint32_t const size, unsigned const flags)
{
    uint32_t const min_size = PCI_IS_IO(flags) ? 4 : 0x10;
    return ~(next_pow2(ROUNDUP(size, min_size)) - 1);
}

/**
 * State of a given PCI BAR.
 */
struct vpci_bar {
    /**
     * Current established mapping.
     * vpci_bar::vb_mapping.host_location and vpci_bar::vb_mapping.guest.length are expected to be constant.
     */
    struct qvm_region   vb_mapping;
    /** Current value of the BAR */
    uint32_t    vb_value;
    /** Upper word for 64-bit BARs */
    uint32_t    vb_value_upper;
    /** Slice index for that BAR */
    uint16_t    vb_slice;
    /** BAR register; leave to 0 if the vpci_bar is the storage back-end */
    uint16_t    vb_breg;
};

/**
 * Read/write configuration state for a virtual PCI device
 */
struct vpci_rw_state {
    /** State of each individual BAR */
    struct vpci_bar bars[6];
	/** Current contents of the PCI command register */
	_Uint16t	command;
	/** Current contents of the PCI status register */
	_Uint16t	status;
	/** Current contents of the PCI cache line size register */
	_Uint8t		cache_line_size;
	/** Current contents of the PCI latency timer register */
	_Uint8t		latency_timer;
	/** Current contents of the PCI interrupt line register */
	_Uint8t		interrupt_line;
};

/** Slice descriptor for a command register */
#define VPCI_SLICE_DESCRIPTOR_COMMAND(wr) { 						\
	.writable = (wr), 												\
	.offset = (_Uint16t)offsetof(struct _pci_config_regs, Command),	\
	.nbytes = 2,													\
	.store_offset = (_Uint16t)offsetof(struct vpci_rw_state, command)\
	}

/** Slice descriptor for a status register */
#define VPCI_SLICE_DESCRIPTOR_STATUS(wr) { 							\
	.writable = (wr), 												\
	.offset = (_Uint16t)offsetof(struct _pci_config_regs, Status),	\
	.nbytes = 2,													\
	.store_offset = (_Uint16t)offsetof(struct vpci_rw_state, status)	\
	}

/** Slice descriptor for a cache line size register */
#define VPCI_SLICE_DESCRIPTOR_CACHE_LINE_SIZE() { 					\
	.writable = 0xff,												\
	.offset = (_Uint16t)offsetof(struct _pci_config_regs, Cache_Line_Size),	\
	.nbytes = 1,													\
	.store_offset = (_Uint16t)offsetof(struct vpci_rw_state, cache_line_size) \
	}

/** Slice descriptor for a lantency timer register */
#define VPCI_SLICE_DESCRIPTOR_LATENCY_TIMER() { 					\
	.writable = 0xff,												\
	.offset = (_Uint16t)offsetof(struct _pci_config_regs, Latency_Timer),	\
	.nbytes = 1,													\
	.store_offset = (_Uint16t)offsetof(struct vpci_rw_state, latency_timer) \
	}

/** Slice descriptor for a 32-bit BAR register */
#define VPCI_SLICE_DESCRIPTOR_BAR32(idx,wr) { 						\
	.writable = (wr), 												\
	.offset = (_Uint16t)offsetof(struct _pci_config_regs, Base_Address_Regs[(idx)]),\
	.nbytes = 4,													\
	.store_offset = (_Uint16t)offsetof(struct vpci_rw_state, bars[(idx)].vb_value) \
	}

/** Slice descriptor for a 64-bit BAR register */
#define VPCI_SLICE_DESCRIPTOR_BAR64(idx,wr) { 						    \
        .writable = (wr), 												\
        .offset = (_Uint16t)offsetof(struct _pci_config_regs, Base_Address_Regs[(idx)]),\
        .nbytes = 4,													\
        .store_offset = (_Uint16t)offsetof(struct vpci_rw_state, bars[(idx)].vb_value) \
	}, {                                                                \
        .writable = ~0u, 												\
        .offset = (_Uint16t)offsetof(struct _pci_config_regs, Base_Address_Regs[(idx)+1]),\
        .nbytes = 4,													\
        .store_offset = (_Uint16t)offsetof(struct vpci_rw_state, bars[(idx)].vb_value_upper) \
    }

/** Slice descriptor for an interrupt line register */
#define VPCI_SLICE_DESCRIPTOR_INTERRUPT_LINE() { 					\
	.writable = 0xff,												\
	.offset = (_Uint16t)offsetof(struct _pci_config_regs, Interrupt_Line),	\
	.nbytes = 1,													\
	.store_offset = (_Uint16t)offsetof(struct vpci_rw_state, interrupt_line) \
	}

/** Slice descriptor array for a virtual PCI device with one 32 bit BAR */
#define VPCI_SLICE_DESCRIPTOR_ONE_BAR32(cmd, status, addr_bits)	\
	VPCI_SLICE_DESCRIPTOR_COMMAND((cmd)),						\
	VPCI_SLICE_DESCRIPTOR_STATUS((status)),						\
	VPCI_SLICE_DESCRIPTOR_CACHE_LINE_SIZE(),					\
	VPCI_SLICE_DESCRIPTOR_LATENCY_TIMER(),						\
	VPCI_SLICE_DESCRIPTOR_BAR32(0, (addr_bits)),				\
	VPCI_SLICE_DESCRIPTOR_INTERRUPT_LINE() /* <STAN_MACRO1> */

/** Number of descriptor entries in a virtual PCI device with one 32 bit BAR */
#define VPCI_NUM_ONE_BAR32_SLICES	6

/** Slice descriptor array for a virtual PCI device with one 64 bit BAR */
#define VPCI_SLICE_DESCRIPTOR_ONE_BAR64(cmd, status, addr_bits)	\
	VPCI_SLICE_DESCRIPTOR_COMMAND((cmd)),						\
	VPCI_SLICE_DESCRIPTOR_STATUS((status)),						\
	VPCI_SLICE_DESCRIPTOR_CACHE_LINE_SIZE(),					\
	VPCI_SLICE_DESCRIPTOR_LATENCY_TIMER(),						\
	VPCI_SLICE_DESCRIPTOR_BAR64(0, (addr_bits)),				\
	VPCI_SLICE_DESCRIPTOR_INTERRUPT_LINE() /* <STAN_MACRO1> */

/** Number of descriptor entries in a virtual PCI device with one 64 bit BAR */
#define VPCI_NUM_ONE_BAR64_SLICES	(VPCI_NUM_ONE_BAR32_SLICES + 1)

/**
 * Return the base address of a given BAR
 * @param	rw_cfg		pointer to r/w data for the vPCI device config space
 * @param	idx			identify which BAR register we want the info for
 * @return	the base address of the identified BAR
 */
static inline uint64_t
vpci_bar_base(const struct vpci_rw_state *const rw_cfg, unsigned const idx) {
    return rw_cfg->bars[idx].vb_mapping.guest.location;
}


/**
 * Return the offset of a given location within a BAR region
 * @param	rw_cfg		pointer to r/w data for the vPCI device config space
 * @param	idx			identify which BAR register we want the info for
 * @param	loc			Guest paddr or I/O port number
 * @return	the offset from the start of the BAR of the location
 */
static inline unsigned
vpci_bar_offset(const struct vpci_rw_state *const rw_cfg, unsigned const idx, uint64_t const loc) {
	return (unsigned)(loc - vpci_bar_base(rw_cfg, idx));
}

void
vpci_init_rw(qvm_vdev_t * const vdp,
                struct vpci_rw_state *rw_cfg,
				const struct qvm_slice_descriptor *desc,
				const struct _pci_config_regs *ro_cfg);

unsigned
vpci_find_bar(const struct vpci_rw_state *rw_cfg,
		const struct qvm_slice_descriptor *desc,
		const struct qvm_state_block *blk);

unsigned
vpci_cfg_read(const void *rw_cfg,
				const struct qvm_slice_descriptor *desc,
				const struct qvm_state_block *blk,
				void *val,
				const struct _pci_config_regs *ro_cfg);

unsigned
vpci_cfg_write(void *rw_cfg,
				const struct qvm_slice_descriptor *desc,
				const struct qvm_state_block *blk,
				const void *val);

void
vpci_region_update(qvm_vdev_t *vdp,
					struct vpci_rw_state *rw_cfg);

unsigned
vpci_update_bars(struct guest_system *gsp, unsigned cmd,
                    struct vpci_bar *bars, unsigned num_bars);

int
vpci_ctrl_pci_bars(struct vpci_rw_state *rw_cfg, const char *arg);

uint32_t	vdev_rd_cfg(qvm_vdev_t *vdp, unsigned off, unsigned len);
void		vdev_wr_cfg(qvm_vdev_t *vdp, unsigned off, unsigned val, unsigned len);

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.0.0/trunk/utils/q/qvm/public/qvm/vpci.h $ $Rev: 849145 $")
#endif
