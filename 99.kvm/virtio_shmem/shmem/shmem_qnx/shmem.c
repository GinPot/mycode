/**************************************************************
  This program connects to the hypervisor shared memory region "testing" from
  the host system. Anything typed at the terminal will be sent to all other
  clients connected to the region and anything typed on the terminals of
  the other clients will be sent to us and displayed.
**************************************************************/

#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <sys/iomgr.h>
#include <sys/neutrino.h>
#include <sys/siginfo.h>
#include <hyp_shm.h>


#define PULSE_CODE_NOTIFY	0
#define PULSE_CODE_INPUT	1

#define BUFF_SIZE		0x1000

int
main(int argc, char *argv[]) {
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
	// Allocate the memory of the hyperivsor shared memory control structure
	//
	struct hyp_shm *hsp = hyp_shm_create(0);
	if(hsp == NULL) {
		printf("no memory for shared memor control structure\n");
		return 1;
	}

	//
	// Attach to the shared memory region. If the region status (see below)
	// changes, a pulse will be delivered to the channel 'channel' with
	// a code of 'PULSE_CODE_NOTIFY'. A pointer can be passed in the
	// pulse value field as well, though we're not using it here.
	//
	int err = hyp_shm_attach(hsp, "testing", HYP_SHM_MAX_CLIENTS,
										channel, -1, PULSE_CODE_NOTIFY, NULL);
	if(err != EOK) {
		printf("shmem create failure: %d\n", err);
		return 1;
	}

	//
	// The first client that attempts to create the shared memory name
	// gets to choose the actual size of the shared memory region. Since you
	// can't tell if you were the first or not, you need to check the 'size'
	// field after the creation - it will have the actual number of pages
	// of the region. This may be larger, smaller, or the same size as what
	// you requested.
	//
	unsigned const size = hyp_shm_size(hsp);
	if(size != HYP_SHM_MAX_CLIENTS) {
		printf("unexpected shared memory size (%d != %d)\n", size, HYP_SHM_MAX_CLIENTS);
		return 1;
	}

	//
	// Each client that connects to shared memory region gets a unique
	// connection index number (0 through 15). This index is to form various
	// bitsets. The hyp_shm_idx() API tells you what your connection index
	// number is.
	//
	unsigned myidx = hyp_shm_idx(hsp);
	printf("shared memory index %u\n", myidx);

	//
	// The hyp_shm_data() API returns a pointer to the start of the
	// shared memory region data
	//
	char *shmdata = hyp_shm_data(hsp);

	unsigned attach_list = hyp_shm_status(hsp) >> HYP_SHM_MAX_CLIENTS;

	//
	// Now we go into a loop waiting for input or notifications from
	// another client of the shared memory region
	//
	int need_notify = 1;
	char *const mybuff = &shmdata[myidx*BUFF_SIZE];
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
			// The hyp_shm_poke API takes a bitset that
			// tells which other clients should be informed that something
			// has happened and they should look at the shared memory region.
			// E.g. if you want to notify the client whose index is 2, you
			// should turn on (1 << 2) in the bitset.
			// The value ~0 is specially handled to mean "everybody but me".
			//
			hyp_shm_poke(hsp, HYP_SHM_POKE_ALL_BUT_ME);
			break;
		case PULSE_CODE_NOTIFY:	;
			// shared memory notification
			//
			// The hyp_shm_status API returns two pieces of information. The
			// bottom 16 bits holds a bitset of all the clients that have
			// notified you since the last time you read the status field.
			// E.g. If bit (1 << 3) is on in the status value, that means that
			// the client using connection index 3 has notified you. The top
			// 16 bits of the field is the bitset of active connections
			// for the region. E.g. if the status field has (1 << (16+0)) and
			// (1 << (16+1)) on, connection index 0 and 1 are in use.
			//
			unsigned status = hyp_shm_status(hsp);
			unsigned i;
			for(i = 0; i < HYP_SHM_MAX_CLIENTS; ++i) {
				if(status & (1 << i)) {
					printf("connection %2d sent '%s'\n", i, &shmdata[i*BUFF_SIZE]);
				}
			}
			status >>= HYP_SHM_MAX_CLIENTS;
			if(status != attach_list) {
				printf("attach/detach %4x => %4x\n", attach_list, status);
				attach_list = status;
			}
			break;
		default:
			printf("unexpected pulse %d\n", pulse.code);
			break;
		}
	}
done:

	//
	// The hyp_shm_detach() cleanly disconnects from the shared memory
	// region and cleans everything up.
	//
	hyp_shm_detach(hsp);
	printf("destroyed!\n");
	return 0;
}