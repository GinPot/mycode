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
 * Definitions for virtual devices
 */

#ifndef _QVM_VDEV_H_INCLUDED
#define _QVM_VDEV_H_INCLUDED

#include <sched.h>
#include <sys/qvm.h>
#include <sys/mman.h>

/**
 * The vdev interface ABI version.
 * This number should be changed every time the ABI between qvm and the loaded vdevs changes,
 * to make qvm error out upon loading a vdev of the wrong version.
 * Changing the size of a structure exposed by the public qvm headers, the prototype of a
 * function or removing a function are all qualifying changes.
 */
#define QVM_VDEV_ABI    2018092701u

struct qvm_intr;
struct qvm_vdev_s;
struct qvm_vdev_factory;
struct guest_timer;
struct guest_cpu;
struct guest_system;
typedef struct qvm_vdev_s qvm_vdev_t;


/**
 * Scheduling information for pulses/threads
 */
struct qvm_sched_info {
	/** scheduling policy (-1 if not set) */
	int					policy;
	/** scheduling priorities (sched.sched_priority == -1 if not set) */
	struct sched_param	sched;
};


/**
 * Possible types for a ::qvm_state_block object.
 * Only a subset of values is valid in any given context.
 */
enum qvm_state_types {
    /** @p location is a guest physical address */
    QST_MEMORY,
    /** @p location is a guest virtual address */
    QST_VIRTUAL,
    /** @p location is an offset within the CPU_REGISTERS structure */
    QST_CPUREGS,
    /** @p location is an offset within the FPU_REGISTERS structure */
    QST_FPUREGS,
    /** @p location is a host virtual address */
    QST_HOST,
    /** @p location identifies a device through its bus, device and
     * function numbers, optionally including an offset into its
     * configuration space. */
    QST_PCI,
    /** @p location is same as QST_MEMORY, used for PCI IO-space BAR
     * references on non-X86 architectures. */
    QST_PCI_IO,
    /** @p location is an integer value */
    QST_IMMEDIATE,

    QST_ARCH_SPECIFIC = 0x8000,

    /** @p location is an I/O port number */
    QST_X86_IO = QST_ARCH_SPECIFIC,
    /** @p location is an offset within the X86_64_SEGREGS structure */
    QST_X86_SEGREGS,
    /** @p location is an offset within the X86_64_CTRLREGS structure */
    QST_X86_CTRLREGS,
    /** @p location is an offset within the X86_64_DBGREGS structure */
    QST_X86_DBGREGS,
    /** @p location is a model-specific register number */
    QST_X86_MSR,

    /** @p location is a system register number built with ARM_MAKE_SR_NUM() */
    QST_ARM_SR = QST_ARCH_SPECIFIC,

    QST_NO_TYPE = 0xffff
};


/**
 * Construct an ARM/AARCH64 system register number from its components
 */
#define ARM_MAKE_SR_NUM(__op0, __op1, __crn, __crm, __op2) \
    (((__op0)<<14) | ((__op1)<<11) | ((__crn)<<7) | ((__crm)<<3) | (__op2))


/**
 * Possible flags for a ::qvm_state_block object.
 */
enum qvm_state_flags {
    /** No flags specified */
    QSF_NONE				= 0x0000,
    /** Bits 0-3 indicate the number of bytes to sign extend by */
    QSF_SEXT_SHIFT			= 0,
    /** Bits 4-7 indicate the number of bytes to zero extend by (after doing sign extension) */
    QSF_ZEXT_SHIFT			= 4,
    QSF_EXT_MASK			= 0xf,
    /** If there's a problem accessing the region, report an exception to the guest */
    QSF_REPORT_EXCEPTION	= 0x8000,
};

#define QSL_NO_LOCATION				(~(_Uint64t)0)
#define QSL_KEEP_LOCATION			(QSL_NO_LOCATION-(uint64_t)1)
#define QSL_PCI_EXTRACT_FUNC(loc)	(((unsigned)(loc) >> 16)  & 0x00ffu)
#define QSL_PCI_EXTRACT_DEV(loc)	(((unsigned)(loc) >> 24)  & 0x00ffu)
#define QSL_PCI_EXTRACT_BUS(loc)	(((unsigned)((loc) >> 32)) & 0xffffu)
#define QSL_PCI_EXTRACT_OFF(loc)	(((unsigned)(loc) >> 0) & 0xffffu)
#define QSL_PCI_MAKE_LOC(bus,dev,func,off)	\
    ((((uint64_t)(func))<< 16)		\
     |(((uint64_t)(dev)) << 24)		\
     |(((uint64_t)(bus)) << 32)		\
     |(((uint64_t)(off)) <<  0))

/**
 * Base structure to describe a data element (typically in the guest)
 */
struct qvm_state_block {
    /**
     * The exact meaning of this field depend on qvm_state_block::type.
     *
     * It is generally understood as an offset into the object implied
     * by qvm_state_block::type.
     *
     * For instance, if it is ::QST_CPUREGS, @p location is the offset
     * of a field that contains the value for a specific register in a
     * CPU-dependent context structure.
     *
     * This mechanism allows writing type-agnostic code when dealing
     * with data movement.
     */
    _Uint64t    location;
    _Uint32t    length;
    /**
     * A value from ::qvm_state_types.
     */
    _Uint16t    type; /* QST_* */
    /** A bitset of values from ::qvm_state_flags. */
    _Uint16t    flags; /* QSF_* */
};


/**
 * Flags for the qvm_region structure
 */
enum qvm_region_flags {
    QRF_NONE			= 0x00000000,
    /** A read access causes vmexit and invokes the cpu callback. */
    QRF_VIRT_RD			= 0x00000001,
    /** A write access causes vmexit and invokes the cpu callback. */
    QRF_VIRT_WR			= 0x00000002,
    /** An execute access causes vmexit and invokes the cpu callback. */
    QRF_VIRT_EX			= 0x00000004,
    QRF_VIRT_ANY		= (QRF_VIRT_RD|QRF_VIRT_WR|QRF_VIRT_EX),
    /** A read access is allowed through with no vmexit. */
    QRF_PASS_RD			= 0x00000008,
    /** A write access is allowed through with no vmexit. */
    QRF_PASS_WR			= 0x00000010,
    /** An execute access is allowed through with no vmexit. */
    QRF_PASS_EX			= 0x00000020,
    QRF_PASS_ANY		= (QRF_PASS_RD|QRF_PASS_WR|QRF_PASS_EX),
    /** Passthru mappings should be marked as no-cache */
    QRF_NOCACHE			= 0x00000040,
    /** Whether the region is active or not */
    QRF_ACTIVE          = 0x00000080,
    /** region is available as normal system ram */
    QRF_MEMORY			= 0x00000100,
    /** Item was specified on with a "pass" option */
    QRF_PASSTHRU		= 0x00000200,
    /** Item uses DMA */
    QRF_DMA_USES		= 0x00000400,
    /** Item does not use DMA */
    QRF_DMA_NO_USE		= 0x00000800,
    /** Item can be a DMA source */
    QRF_DMA_SOURCE		= 0x00001000,
    /** Item can be a DMA target */
    QRF_DMA_TARGET		= 0x00002000,
    /** Region cannot overlap another one */
    QRF_PRECIOUS        = 0x00004000,
    /** Mask for saved bits */
    QRF_BACKUP_MASK     = 0xf0000000,
    /** Shift for saved bits (saved bits are PASS_* and NOCACHE) */
    QRF_BACKUP_SHIFT    = (28-3),
    /** Flags that should be preserved */
    QRF_STICKY          = (QRF_MEMORY|QRF_DMA_SOURCE|QRF_DMA_TARGET|QRF_PASSTHRU),
};

/**
 * Description of a guest resource.
 */
struct qvm_region {
    /** A bitset of values from ::qvm_region_flags. */
    _Uint32t				flags; /* QRF_* */
	/** Cookie that is passed back to the vdev vread/vwrite routines */
    _Uint32t				cookie;
    /**
     * Only used if one or more of QRF_VIRT_* flags is set, value passed
     * back in the vdev callbacks.
     */
    qvm_vdev_t				*vdev;
    /**
     * Where the region should map to on the host system.
     */
    _Uint64t				host_location;
    /** Where the region should appear in the guest system */
    struct qvm_state_block	guest;
};


/**
 * Structure for timer information
 */
struct qvm_timer_data {
    /** Host's idea of the time at which the timer was activated */
    _Uint64t	host_notify_start;
    /** Host's idea of the current time */
    _Uint64t	host_current;
    /** Guest's idea of the time at which the timer was activated */
    _Uint64t	guest_notify_start;
    /** Guest's idea of the current time */
    _Uint64t	guest_current;
};



/**
 * Flags for the state read/write requests.
 */
enum qvm_xfer_flags {
    QXF_NONE		= 0x00000000,
    /** Pause the CPU before the transaction */
    QXF_PAUSE		= 0x00000001,
    /** Unpause the CPU after the transaction */
    QXF_UNPAUSE		= 0x00000002,
    /**
     * Perform the action even if the target CPU is running.
     * Use this flag with caution.
     */
    QXF_ASYNC		= 0x00000004,
    /** For GDB calls, do not check PTE access permissions */
    QXF_GDB			= 0x00000008,
    /** Flush the data cache after an operation */
    QXF_FLUSH		= 0x00000010
};


#if defined(__X86__) || defined(__X86_64__)
#define QST_DEFAULT_TYPE_IO	(QST_X86_IO)
#else
#define QST_DEFAULT_TYPE_IO	(QST_MEMORY)
#endif

/** Round a value up to a given power of two */
#define ROUNDUP(val, round) \
    (((val) + ((round)-1)) & ~(((typeof (val))((round)-1))))
/** Round a value down to a given power of two */
#define ROUNDDOWN(val, round) \
    ((val) & ~(((typeof (val))((round)-1))))

typedef void qvm_callback_func_t(void *data);
struct qvm_callback {
    qvm_callback_func_t		*func;
};

enum qvm_intr_state {
	QIS_ON	= 0x01,	/**< assert the interrupt */
	QIS_OFF	= 0x02, /**< de-assert the interrupt */
};

#define ID_FAILED	(~(uint64_t)0)
uint64_t    qvm_unique(uint64_t start, uint64_t end);
int			qvm_channel(void);
void		qvm_global_lock(void);
void		qvm_global_unlock(void);
int			qvm_overlap(uint64_t loc_start, uint64_t loc_size, uint64_t rgn_start, uint64_t rgn_size);
void		qvm_register_vdev_factory(struct qvm_vdev_factory *factory, unsigned abi_version);
int			qvm_cpu_write(struct guest_cpu *gcp, unsigned flags, const struct qvm_state_block *blk, unsigned nblks, const void *data, unsigned data_len);
int         qvm_all_ones(struct guest_cpu *gcp, unsigned flags, const struct qvm_state_block *blk);
int			qvm_system_write(struct guest_system *gsp, unsigned flags, const struct qvm_state_block *blk, unsigned nblks, const void *data, unsigned data_len);
int			qvm_vdev_write(struct guest_cpu *gcp, struct qvm_state_block *blk, const void * data);
int			qvm_cpu_read(struct guest_cpu *gcp, unsigned flags, const struct qvm_state_block *blk, unsigned nblks, void *data, unsigned data_len);
int			qvm_system_read(struct guest_system *gsp, unsigned flags, const struct qvm_state_block *blk, unsigned nblks, void *data, unsigned data_len);
int			qvm_vdev_read(struct guest_cpu *gcp, struct qvm_state_block *blk, void *data);
int			qvm_signal_intr(struct qvm_intr *ip, enum qvm_intr_state state);
int			qvm_signal_msi(const qvm_vdev_t *src, uint64_t addr, unsigned data);
void		qvm_register_msi(qvm_vdev_t *const pic, uint64_t const paddr, uint64_t const mask, void (*deliver)(qvm_vdev_t *pic, uint64_t paddr, unsigned data, unsigned cookie), unsigned cookie);
unsigned	qvm_intr_level(const struct qvm_intr *ip);
void		qvm_intr_notify(const qvm_vdev_t *vdp, unsigned param);
int         qvm_region_lock(struct guest_system *gsp, uint16_t type);
int         qvm_region_unlock(struct guest_system *gsp, uint16_t type);
unsigned	qvm_region_set_soft(struct guest_system *gsp, unsigned num_regions, const struct qvm_region *rgn);
void		qvm_region_set(struct guest_system *gsp, unsigned num_regions, const struct qvm_region *rgn);
int         qvm_region_info(struct guest_system *gsp, const struct qvm_state_block *blk, struct qvm_region *rgn);
uint64_t	qvm_region_find_free(const struct guest_system *gsp, unsigned qst, uint64_t size, uint64_t align);
void		qvm_vdev_pulse(qvm_vdev_t *vdp, int8_t code, int priority, struct sigevent *ev);
void		qvm_callback_pulse(qvm_callback_func_t *cbfp, struct qvm_callback *cbp, int priority, struct sigevent *ev);
struct guest_timer *qvm_gtimer_create(const struct guest_cpu *gcp, const qvm_vdev_t *vdp, const void *data, _Uint64t *resolutionp);
int			qvm_gtimer_notify(struct guest_timer *vtp, const struct qvm_timer_data *tdp, _Uint64t incr);
void		qvm_gtimer_state(const struct guest_timer *vtp, struct qvm_timer_data *tdp);
void		*qvm_map_guest(struct guest_system *gsp, int prot, _Uint64t guest_paddr, size_t length);
void		qvm_unmap_guest(struct guest_system *gsp, const void *ptr, size_t length, int const flush);
void		qvm_vdev_set_paused_cpu(const qvm_vdev_t *vdp, struct guest_cpu *gcp, int do_pause);
struct guest_cpu *qvm_vdev_get_paused_cpu(const qvm_vdev_t *vdp, int do_unpause);
void		qvm_cpu_set_data(struct guest_cpu *gcp, void *data);
void		*qvm_cpu_get_data(struct guest_cpu *gcp);
int         qvm_vdev_path(const qvm_vdev_t *vdp, char *buf, size_t size);
int			qvm_write_core_file(struct guest_system *gsp);

/** flags to control qvm_parse_num() operation */
enum qvm_parse_num_flags {
	/** No flags specified */
	QPNF_NONE			= 0x00,
	/** Allow number to be scaled by 'k', 'm', 'g' suffixes */
	QPNF_ALLOW_SUFFIXES	= 0x01,
	/** Allow a negative sign on the number */
	QPNF_ALLOW_NEGATIVE	= 0x02,
	/** Allow trailing characters following the number */
	QPNF_ALLOW_TRAILING = 0x04,
    /** A maximum value is specified in the return paramater */
    QPNF_SIZE_CHECK     = 0x08,
};

char		*qvm_parse_num(const char *str, enum qvm_parse_num_flags flags, uint64_t *nump);
char		*parse_block(const char *str, struct qvm_state_block *bp, int *pci_idp);
char		*parse_intr(const char *str, struct qvm_intr *ip);

/*
 * NOTE: the order of these constants is important. An interrupt is allowed
 * to change type upwards, (e.g. SHARABLE => EXCLUSIVE), but not the inverse
 * (e.g. RESERVED => SHAREABLE).
 */
enum qvm_intr_assign_type {
	QIAT_UNUSED,
	QIAT_SHAREABLE,
	QIAT_RESERVED,
	QIAT_EXCLUSIVE,
};

void		qvm_intr_assign(qvm_vdev_t *pic, unsigned pnum, enum qvm_intr_assign_type const type, struct qvm_intr *ip);

int			qvm_system_get_ram_info(const struct guest_system *const gsp, uint16_t *const blks, struct qvm_region *const ram);
int			qvm_memory_fd(const struct guest_system *const gsp);

void		qvm_report(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
const char	*block_type_name(unsigned type);
int         qvm_block_string(char *buf, size_t count, const struct qvm_state_block *blk);
int         qvm_vdev_string(char *buf, size_t count, const qvm_vdev_t *vdp);
int         qvm_vdev_thread_setname(pthread_t tid, const qvm_vdev_t *vdp, const char *name);

enum walk_mem_state_flags {
    WMS_HOST	= 0x0001,	///< Need host location information
    WMS_RAM		= 0x0002,	///< Need guest ram information
    WMS_ROM		= 0x0004,	///< Need guest rom information
	WMS_DMA		= 0x0008,	///< Need regions that can be DMA targets
};

struct walk_mem_state {
    void			*span;			///< internal state field
    const struct guest_system *gsp;	///< guest system pointer
    uint64_t		guest_loc;		///< starting guest paddr
    uint64_t		host_loc;		///< starting host paddr
    uint64_t		length;			///< length of region
    uint64_t		max_length;		///< maximum allowed length
    unsigned		flags;			///< walk flags (WMS_*)
};

unsigned    walk_mem_count(const struct guest_system *gsp, uint64_t max_length, unsigned flags);
void        walk_mem_init(struct walk_mem_state *const wms, const struct guest_system *const gsp, uint64_t const max_length, unsigned const flags);
int         walk_mem_next(struct walk_mem_state *wms);

unsigned qvm_next_apic_id(void);
void	acpi_add_table(struct guest_system *gsp, void *table, size_t len, uint32_t sig, uint8_t rev,
        const char * table_id, uint32_t oem_rev);
void	acpi_register_ioapic(qvm_vdev_t *vdev, unsigned apic_id);

int		gfdt_update_node(int parent, const char *nodename, uint64_t unit);
void	gfdt_update_property(int node, const char *propname, const void *valp, size_t vall);
void	gfdt_update_prop_string(int node, const char *propname, const char *str);
void	gfdt_update_prop_strings(int node, const char *propname, ...);
void	gfdt_update_prop_cells(int node, const char *propname, uint32_t *valp, size_t vall);
int		gfdt_update_phandle(int node);
int		gfdt_get_clock(int node, unsigned freq);
void	gfdt_update_interrupts(int node, ...);
void	gfdt_update_reg(int parent, int node, ...);

/**
 * Description of a particular range of offsets in state structure
 */
struct qvm_slice_descriptor {
    uint32_t	writable;		/**< bitset of writable bits in the range */
    uint16_t	offset;			/**< starting offset in the read-only structure */
    uint16_t	store_offset;	/**< offset where modifications are stored in the r/w structure */
    uint8_t		nbytes;			/**< number of bytes represented by the descriptor */
};


/**
 * Structure describing which descriptor and for how many bytes a particular slice is
 */
struct qvm_slice_result {
    uint8_t		descriptor_idx;	/**< index of descriptor array that the slice comes from or QVM_SLICE_NO_DESCRIPTOR */
    uint8_t		nbytes;			/**< number of bytes in the slice */
};

/** Used in @p descriptor_idx to indicate that the slice has no matching descriptor */
#define QVM_SLICE_NO_DESCRIPTOR	((_Uint8t)~0)

/** Used to end a slice descriptor array */
#define QVM_SLICE_DESCRIPTOR_END {.writable=0,.offset=0,.nbytes=0,.store_offset=0}

unsigned	qvm_slice(unsigned offset, unsigned len, const struct qvm_slice_descriptor *desc, struct qvm_slice_result *slices);
void		qvm_slice_store(const struct qvm_slice_descriptor *desc, unsigned off, unsigned nbytes, void *dst, const void *src);


/** Used as a prefix on an vdev option list entry to indicate that it takes no following argument */
#define QVM_OPTION_NO_ARG_PREFIX	"0"


/** @p control function being invoked to initialize vdev state */
#define VDEV_CTRL_OPTIONS_START	0u
/** @p control function being invoked to finalize vdev state after option processing */
#define VDEV_CTRL_OPTIONS_END	1u
/** @p control function being invoked to say guest system is being started */
#define VDEV_CTRL_BOOTUP		2u
/** @p control function being invoked to say guest system is being rebooted */
#define VDEV_CTRL_RESET			3u
/** @p control function being invoked to say container process is terminating */
#define VDEV_CTRL_TERMINATE		4u
/** @p control function being invoked to make vdev output status information */
#define VDEV_CTRL_STATUS		5u
/** @p control function being invoked to generate ACPI information for the vdev */
#define VDEV_CTRL_GEN_ACPI		6u
/** @p control function being invoked to generate FDT information for the vdev */
#define VDEV_CTRL_GEN_FDT		7u
/** @p control function being invoked to indicate that the 'power' button is pressed (SIGTERM sent) */
#define VDEV_CTRL_POWER			8u
/** @p control function being invoked to list the BARs of the PCI device
 * Any positive return value means no BARs are exposed by the device.
 * The opposite of a negative return value is the number of BARs returned.
 */
#define VDEV_CTRL_PCI_BARS      9u
/** @p control function being invoked to handle device specific opton @p n
 *  when being passed <tt>VDEV_CTRL_FIRST_OPTIDX+n</tt>
 */
#define VDEV_CTRL_FIRST_OPTIDX	16u


/**
 * Operations for the vdev @p pic function
 */
enum vdev_pic_operations {
	VDEV_PIC_NUM_LINES,		/**< return the number of input lines for the device; @p value is ignored */
	VDEV_PIC_PIN_DEASSERT,	/**< input line @p value is being lowered */
	VDEV_PIC_PIN_ASSERT,	/**< input line @p value is being raised */
	VDEV_PIC_PIN_NOTIFY,	/**< configure @p inform unmask state */
	VDEV_PIC_PIN_FDT_SPEC,	/**< generate FDT interrupt property information, returns uint32_t array pointer */
};


/**
 * Return status from vdev read/write functions
 */
enum qvm_vdev_ref_status {
    /** Normal completion, advance to next instruction */
    QVRS_NORMAL,
    /** Normal completion, do not advance to next instruction */
    QVRS_NO_ADVANCE,
    /** Abort instruction emulation and restart guest execution */
    QVRS_ABORT,
    /** A bus error would have occured on a real system */
    QVRS_BUS_ERROR,
	/** keep looking for more instruction exit callbacks */
	QVRS_CONTINUE,
	/** Shutdown qvm */
	QVRS_TERMINATE,
    /** performing reference generated an error (errno value added to this constant */
    QVRS_ERRNO	= 0x100
};


/**
 * Flags controlling vdev creation
 */
enum qvm_vdev_factory_flags {
	/** No flags specified */
	QVFF_NONE			= 0x0000,
    /** Only one device of this type is allowed in a system */
    QVFF_ONLY_ONE		= 0x0001,
	/** The 'intr' option isn't allowed */
	QVFF_INTR_NO		= 0x0002,
	/** The 'intr' option is required */
	QVFF_INTR_REQ		= 0x0004,
	/** The 'intr' option is required for a non-PCI device */
	QVFF_INTR_NONPCI	= 0x0008,
	/** The 'loc' option is not allowed */
	QVFF_LOC_NO			= 0x0010,
	/** The vdev is allowed to be a "mem:" device */
	QVFF_VDEV_MEM		= 0x0020,
	/** The vdev is allowed to be an "io:" device on the x86 */
	QVFF_VDEV_X86_IO	= 0x0040,
	/** The vdev is allowed to be a pci device */
	QVFF_VDEV_PCI		= 0x0080,
	/** Set of allowed location types */
	QVFF_VDEV_LOC_TYPES = (QVFF_VDEV_MEM|QVFF_VDEV_X86_IO|QVFF_VDEV_PCI),
    /** Device needs a mutex */
    QVFF_MUTEX          = 0x0100,
    /** Mutex must be recursive */
    QVFF_MUTEX_RECURSIVE = 0x0200,
};


/**
 * Used for creating new virtual device instances.
 */
struct qvm_vdev_factory {
	/** pointer to next entry in linked list of structures */
	struct qvm_vdev_factory		*next;
	/** function to perform various opertions on the vdev (see VDEV_CTRL_* macros) */
	int							(*control)(qvm_vdev_t *vdev, unsigned ctrl, const char *arg);
	/** function to handle a read from the vdev */
	enum qvm_vdev_ref_status	(*vread)(qvm_vdev_t *vdev, unsigned cookie, const struct qvm_state_block *vopnd, const struct qvm_state_block *oopnd, struct guest_cpu *gcp);
	/** function to handle a write to the vdev */
	enum qvm_vdev_ref_status	(*vwrite)(qvm_vdev_t *vdev, unsigned cookie, const struct qvm_state_block *vopnd, const struct qvm_state_block *oopnd, struct guest_cpu *gcp);
	/** function to handle PIC specific operations (see ::vdev_pic_operations) */
	_Uintptrt					(*pic)(qvm_vdev_t *vdev, unsigned op, unsigned pin);
	/** function to handle a pulse delivery for the vdev */
	int							(*pulse)(qvm_vdev_t *vdev, int8_t code);
	/** function to handle a timer or trigger notification for the vdev */
	void						(*timer)(qvm_vdev_t *vdev, void *data, const struct qvm_timer_data *tdp, struct guest_cpu *gcp);
	/** pointer to array of vdev specific options */
	const char					*const *option_list;
	/** name of vdev type. If zero, filled in with shared object name the factory is in */
	const char					*name;
	/** bit set of QVFF_* values to control parsing/creation of the device */
	unsigned					factory_flags;
	/** bitset of (1u << byte_size) values for list of allowed access sizes */
	unsigned					acc_sizes;
	/** number of additional bytes to allocate in the @p qvm_vdev_t structure for device specific state */
	unsigned					extra_space;
};


/** Call qvm_intr_notify when unmasked (for VDEV_PIC_PIN_NOTIFY pin parameter only) */
#define QIPF_NOTIFY_UNMASK  0x80000000
/** Call qvm_intr_notify when acknowledged (for VDEV_PIC_PIN_NOTIFY pin parameter only) */
#define QIPF_NOTIFY_ACK 	0x40000000
/** Mask off pin number in qvm_intr.param field */
#define QIP_PNUM_MASK	0xffff


/**
 * Virtual interrupt information.
 */
struct qvm_intr {
	/** PIC to deliver the interrupt to */
	qvm_vdev_t	*pic;
	/** pin information (number + QIPF_* flags) */
	unsigned	param;
	/** source identification bit */
	unsigned	src_id;
};


/**
 * Virtual device information
 */
struct qvm_vdev_s {
	/** pointer to device specific state information */
	void							*v_device;
	/** pointer to device factory for the vdev */
	const struct qvm_vdev_factory	*v_factory;
	/** pointer to the guest system that the vdev is defined in */
	struct guest_system				*v_gsp;
	/** location of the vdev in the guest system */
	struct qvm_state_block			v_block;
	/** guest interrupt the vdev requests service with */
	struct qvm_intr					v_intr;
    /** mutex for device's own use */
    pthread_mutex_t                 v_mtx;
};


/**
 * Round up to the next power of 2
 * @param	val	value to be rounded up
 * @return value rounded up to the next power of 2.
 */
static inline unsigned
next_pow2(unsigned val) {
	--val;
	val |= val >> 1;
	val |= val >> 2;
	val |= val >> 4;
	val |= val >> 8;
	val |= val >> 16;
	return val + 1;
}

/** Guest instructions we can get a callback on */
enum qvm_guest_instr_type {
	QGIT_X86_CPUID	= 0,	/* qualifier: EAX value */
	/* FIXME: Add support for RDMSR/WRMSR in the future... */

	QGIT_ARM_SMC	= 0,	/* qualifier: SMC immediate value */
	QGIT_ARM_MSR,			/* qualifier: ARM_MAKE_SR_NUM() of system register */
	QGIT_ARM_MRS,			/* qualifier: ARM_MAKE_SR_NUM() of system register */

	QGIT_FLAG_NO_QUALIFIER = 0x40000000 /* accept any qualifier value */
};

typedef enum qvm_vdev_ref_status qvm_guest_instr_callback_func_t(qvm_vdev_t *vdp, struct guest_cpu *gcp, enum qvm_guest_instr_type git, unsigned qualifier);
int qvm_register_for_instr(qvm_vdev_t *vdp, enum qvm_guest_instr_type git, unsigned qualifier, qvm_guest_instr_callback_func_t *func);

void qvm_register_for_power(qvm_vdev_t *vdp);


/** Guest cpu power state flags */
enum qvm_power_state_cmd {
	/** Requesting/commanding about waiting guest CPUS (WFI/WFE) */
	QPSC_WAIT	= 0x01,
	/** Requesting/commanding about guest CPUS powered off */
	QPSC_OFF	= 0x02,
	/** Request notification when old state matched mask/bits */
	QPSC_FROM	= 0x40000000,
	/** Request notification when new state matches mask/bits */
	QPSC_TO		= 0x80000000,
};

struct guest_power_state_notify;
typedef void qvm_power_state_callback_func_t(void *data, _Uint64t curr, unsigned base);

int		qvm_power_state_current(struct guest_system *gsp, enum qvm_power_state_cmd cmd, unsigned base, unsigned nresult, _Uint64t *result);
struct guest_power_state_notify *qvm_power_state_notify_create(struct guest_system *gsp);
void	qvm_power_state_notify_delete(struct guest_power_state_notify *qpnp);
void	qvm_power_state_notify_register(struct guest_power_state_notify *qpnp,
					enum qvm_power_state_cmd cmd, unsigned base,
					uint64_t mask, uint64_t bits,
					qvm_power_state_callback_func_t *func, void *data);

/* X86 only */
typedef void qvm_apic_func_t(qvm_vdev_t *vdp, int eoi, unsigned vector);
void	qvm_register_for_apic(qvm_vdev_t *vdp, qvm_apic_func_t *apic_func);
int		qvm_signal_apic(const struct qvm_intr *ip, unsigned icrh, unsigned icrl);
#endif


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/7.0.0/trunk/utils/q/qvm/public/qvm/vdev.h $ $Rev: 868406 $")
#endif
