/**************************************************************

  This program connects to the hypervisor shared memory region "testing" from
  a guest system. Anything typed at the terminal will be sent to all other
  clients connected to the region and anything typed on the terminals of
  the other clients will be sent to us and displayed.

**************************************************************/


/*************************************************
Example command line to start a guest with the virtual shared
memory device enabled.

Note that since it appears in the guest as a PCI device (if you don't
specify a "loc" option for it), you need to have the pci server in the guest
and start it before running this program.

qvm	ram 16M \
	vdev vdev-8259.so loc 0x20 intr cpu:irq name master	\
	vdev vdev-8259.so loc 0xa0 intr master:2 name slave	\
	vdev vdev-timer8254.so intr master:0 \
	vdev vdev-ser8250.so intr master:4 \
	vdev vdev-ser8250.so hostdev >/dev/null loc 0x2f8,8 intr master:3 \
	vdev vdev-mc146818.so \
	vdev vdev-shmem.so \
	load qnx.ifs

**************************************************/
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/iomgr.h>
#include <sys/neutrino.h>
#include <stdlib.h>
#include <errno.h>
#include <stddef.h>
#include <inttypes.h>

#if _NTO_VERSION < 700
	#define USE_LEGACY_PCI_API
#endif

// Note if the legacy PCI API is not used, then we must link to libpci
#ifndef USE_LEGACY_PCI_API
# include <pci/pci.h>
# include <pci/pci_id.h>
#else
# include <hw/pci.h>
#endif

//
// <guest_shm.h> holds the data structures associated with the guest
// shared memory device.
//
#include <guest_shm.h>

volatile struct guest_shm_factory *factory;
volatile struct guest_shm_control *ctrl;
volatile uint8_t *ptr;

unsigned intr = ~0;


#define PULSE_CODE_NOTIFY	0
#define PULSE_CODE_INPUT	1

#define BUFF_SIZE	0x1000

int
main(int argc, char **argv) {
	uint64_t	factory_paddr;

	printf("test started\n");

	if(ThreadCtl(_NTO_TCTL_IO, NULL) == -1) {
		fprintf(stderr, "Unable to obtain I/O privity. You must be userid root.\n");
		return 1;
	}

	const char *const pstring = argv[1];
	if((pstring != NULL) && isdigit(*pstring)) {
		factory_paddr = strtoull(pstring, NULL, 16);
		const char *const istring = argv[2];
		if((istring != NULL) && isdigit(*istring)) {
			intr = strtoull(istring, NULL, 0);
		}
	} else {

#ifdef USE_LEGACY_PCI_API

		if(pci_attach(0) < 0) {
			printf("pci attach failed: %d\n", errno);
			return 1;
		}

		//
		// Find the guest shared memory device. Right now it's got
		// fixed vendor and device ids, but that will be generalized later
		//
		unsigned bus;
		unsigned devfunc;
		switch(pci_find_device(PCI_DID_QNX_GUEST_SHM, PCI_VID_BlackBerry_QNX, 0, &bus, &devfunc)) {
		case -1:
			printf("pci find device failed: %d\n", errno);
			return 1;
		case PCI_DEVICE_NOT_FOUND:
			printf("Can't find device\n");
			return 1;
		}

		//
		// Make sure the PCI device is enabled
		//
		uint16_t	val = ~0;
		pci_write_config16(bus, devfunc,
				offsetof(struct _pci_config_regs, Command), 1, &val);

		//
		// Get the interrupt line to use.
		//
		uint8_t iline;
		pci_read_config8(bus, devfunc,
				offsetof(struct _pci_config_regs, Interrupt_Line), 1, &iline);
		intr = iline;

		//
		// Get the BAR to find out the physical memory address of the shared memory
		// 'factory' page. This page is used to create new inter-guest shared
		// memory regions.
		//
		uint32_t bar;
		pci_read_config32(bus, devfunc,
				offsetof(struct _pci_config_regs, Base_Address_Regs[0]), 1, &bar);

		factory_paddr = bar & ~0xfu;

#else // ! USE_LEGACY_PCI_API

		//
		// Find the guest shared memory device. Right now it's got
		// fixed vendor and device ids, but that will be generalized later
		//
		pci_bdf_t pci_bdf = pci_device_find(0, PCI_VID_BlackBerry_QNX, PCI_DID_QNX_GUEST_SHM, PCI_CCODE_ANY);
		if(pci_bdf == PCI_BDF_NONE) {
			printf("Can't find device\n");
			return 1;
		}

		//
		// Attach to the PCI device
		//
		pci_err_t		pci_err;
		pci_devhdl_t pci_devhdl = pci_device_attach(pci_bdf, pci_attachFlags_MULTI_OWNER, &pci_err);
		if(pci_devhdl == NULL) {
			printf("Unable to attach to device (%s)\n", pci_strerror(pci_err));
			return 1;
		}

		//
		// Make sure the PCI device is enabled
		//
		const pci_cmd_t cmd = ~0;
		pci_device_write_cmd(pci_devhdl, cmd, NULL);

		//
		// Get the interrupt line to use.
		//
		pci_irq_t iline;
		pci_err = pci_device_read_irq(pci_devhdl, NULL, &iline);
		if(pci_err != PCI_ERR_OK) {
			printf("Unable to read irq of pci device (%s)\n", pci_strerror(pci_err));

			pci_err = pci_device_detach(pci_devhdl);
			if(pci_err != PCI_ERR_OK) {
				printf("Unable to detach from pci device (%s)\n", pci_strerror(pci_err));
			}

			return 1;
		}
		intr = iline;

		//
		// Get the BAR to find out the physical memory address of the shared memory
		// 'factory' page. This page is used to create new guest shared
		// memory regions.
		//
		pci_ba_t	pci_bar;
		pci_bar.bar_num = 0;

		pci_err = pci_device_read_ba(pci_devhdl, NULL, &pci_bar, pci_reqType_e_MANDATORY);
		if(pci_err != PCI_ERR_OK) {
			printf("Unable to read BAR of pci device (%s)", pci_strerror(pci_err));

			pci_err = pci_device_detach(pci_devhdl);
			if(pci_err != PCI_ERR_OK) {
				printf("Unable to detach from pci device (%s)\n", pci_strerror(pci_err));
			}

			return 1;
		}

		factory_paddr = pci_bar.addr;

#endif // USE_LEGACY_PCI_API

	}
	factory = mmap(NULL, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_PHYS,
						NOFD, factory_paddr);
	printf("factory (%" PRIx64 ") mapped at %p\n", factory_paddr, factory);

	if(factory->signature != GUEST_SHM_SIGNATURE) {
		printf("Signature incorrect. %" PRIx64 " != %" PRIx64 "\n", GUEST_SHM_SIGNATURE, factory->signature);
		return 1;
	}
	if(intr == ~0u) {
		intr = factory->vector;
		printf("Auto select ");
	}
	printf("interrupt vector %d\n", intr);

	//
	// Set up for main loop
	//
	struct sigevent		ev;
	int					channel;
	int					coid;

	channel = ChannelCreate_r(0);
	if(channel < 0) {
		printf("channel failure: %d\n", -channel);
		return 1;
	}
	coid = ConnectAttach_r(0, 0, channel, _NTO_SIDE_CHANNEL, 0);
	if(coid < 0) {
		printf("connect attach failure: %d\n", -coid);
		return 1;
	}

	//
	// Every time the ctrl->status field (see below) changes value, an
	// 'intr' interrupt will be delivered to the guest.
	//
	SIGEV_PULSE_INIT(&ev, coid, -1, PULSE_CODE_NOTIFY, NULL);
	int const iid = InterruptAttachEvent(intr, &ev, _NTO_INTR_FLAGS_TRK_MSK);
	if(iid == -1) {
		printf("InterruptAttach failed: %d\n", errno);
		return 1;
	}


	//
	// Set the name field on the factory page to identify the shared memory
	// region that you want to create/attach to.
	//
	strcpy((char *)factory->name, "testing");
	printf("about to create object '%s'\n", factory->name);

	//
	// Set the size field to indicate the number of 4K pages you want allocated
	// for the shared memory region. The act of writing this field causes the
	// hypervisor to create the shared memory region (or attach to it if it
	// already exists), so it must be done after the name has been set.
    // Although not technically required, this should be done using the
	// inline function guest_shm_create() since it has GCC directives to
	// prevent the compiler from re-ordering other factory page accesses
	// before/after the creation request.
	//
	guest_shm_create(factory, GUEST_SHM_MAX_CLIENTS);

	//
	// After the size has been set, we need to check the status field to
	// see if we successfully created/attached to the region.
	//
	if(factory->status != GSS_OK) {
		printf("creation failed: %d\n", factory->status);
		return 1;
	}

	//
	// If we successfully created/attached to the region, the 'shmem' field
	// holds the physical address used to access the memory.
	//
	// NOTE: The first client that attempts to create the shared memory name
	// gets to choose the actual size of the shared memory region. Since you
	// can't tell if you were the first or not, you need to check the 'size'
	// field after the creation - it will have the actual number of pages
	// of the region. This may be larger, smaller, or the same size as what
	// you requested.
	//
	uint64_t	paddr = factory->shmem;
	printf("creation size %x, paddr=%llx\n", factory->size, (unsigned long long)paddr);

	//
	// Now we get a virtual address to reference the region. The returned
	// paddr is actually the 'control' page. It's used to receive notifications
	// that another connection has 'poked' the region (made some change that
	// you need to be aware of) or to 'poke' other connections when you make a
	// change. The physical pages following are the actual shared memory.
	//
	ctrl = mmap(NULL, (factory->size+1)*0x1000, PROT_READ|PROT_WRITE,
									MAP_SHARED|MAP_PHYS, NOFD, paddr);
	printf("ctrl=%p\n", ctrl);

	//
	// Each client that connects to shared memory region gets a unique
	// connection index number (0 through 15). This index is to form various
	// bitsets. The ctrl->idx field tells you what your connection index
	// number is.
	//

	printf("shared memory index %u\n", ctrl->idx);

	char *shmdata = (char *)ctrl + 0x1000u;
	unsigned attach_list = ctrl->status >> GUEST_SHM_MAX_CLIENTS;
	int need_notify = 1;
	char *const mybuff = &shmdata[ctrl->idx*BUFF_SIZE];

	//
	// Now we go into a loop waiting for input or notifications from
	// another client of the shared memory region
	//
	for( ;; ) {
		if(need_notify) {
			SIGEV_PULSE_INIT(&ev, coid, -1, PULSE_CODE_INPUT, NULL);
			int r = ionotify(0, _NOTIFY_ACTION_POLLARM, _NOTIFY_COND_INPUT, &ev);
			if(r & _NOTIFY_COND_INPUT) {
				MsgSendPulse(coid, -1, PULSE_CODE_INPUT, 0);
			}
			need_notify = 0;
		}
		struct _pulse		pulse;
		MsgReceivePulse(channel, &pulse, sizeof(pulse), NULL);
		switch(pulse.code) {
		case PULSE_CODE_INPUT:
			// terminal input
			need_notify = 1;
			int const len = read(0, mybuff, BUFF_SIZE);
			if(len <= 0) goto done;
			mybuff[len-1] = '\0';
			if(strcmp(mybuff, "quit") == 0) goto done;
			//
			// The value written to the notify field is a bitset that
			// tells which other clients should be informed that something
			// has happened and they should look at the shared memory region.
			// E.g. if you want to notify the client whose index is 2, you
			// should turn on (1 << 2) in the bitset.
			// The value ~0 is specially handled to mean "everybody but me".
			//
			ctrl->notify = ~0;
			break;
		case PULSE_CODE_NOTIFY:	;
			// shared memory notification
			//
			// The status field contains two pieces of information. The bottom
			// 16 bits holds a bitset of all the clients that have notified
			// you since the last time you read the status field. E.g. If
			// bit (1 << 3) is on in the status value, that means that the
			// client using connection index 3 has notified you. The top
			// 16 bits of the field is the bitset of active connections
			// for the region. E.g. if the status field has (1 << (16+0)) and
			// (1 << (16+1)) on, connection index 0 and 1 are in use.
			//
			unsigned status = ctrl->status;
			unsigned i;
			for(i = 0; i < GUEST_SHM_MAX_CLIENTS; ++i) {
				if(status & (1 << i)) {
					printf("connection %2d sent '%s'\n", i, &shmdata[i*BUFF_SIZE]);
				}
			}
			status >>= GUEST_SHM_MAX_CLIENTS;
			if(status != attach_list) {
				printf("attach/detach %4x => %4x\n", attach_list, status);
				attach_list = status;
			}
			InterruptUnmask(intr, iid);
			break;
		default:
			printf("unexpected pulse %d\n", pulse.code);
			break;
		}
	}


done:
	//
	// A write of any value to the detach field cleanly disconnects from
	// the shared memory region. All other clients will be notified and
	// the physical address range for the control and data fields in this
	// client will no longer be accessable.
	//
	ctrl->detach = 0;

	return 0;
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/mainline/apps/hypervisor/demos/shmem-guest/ghstest.c $ $Rev: 847001 $")
#endif
