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

//ljm for memory operation
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grabbuffer.h"

#define PULSE_CODE_NOTIFY   0
#define PULSE_CODE_INPUT    1

#define BUFF_SIZE           0x1000

typedef struct hyp_fbinfo {
    uint64_t paddr;
    void *vaddr;
    uint32_t handle;
    uint32_t fbid;
    uint32_t owner;
    uint32_t order;
    uint32_t isfront;
    uint32_t len;
} hyp_fbinfo_t;



hyp_fbinfo_t * hyp_fb[2];
void *qnx_virt_addr[2] = {NULL};
void *qnx_current_front_buffer = NULL;
char * linux_fbinfo_buf;

void* getDisplayBuffer(unsigned int idx) {
    if(idx >= DISPLAY_BUDDER_TOP ) {
        return NULL;
    }
    return qnx_virt_addr[idx];
}


BUF_INDEX getDisplayBufferIndex(void * pionter) {
    int i = 0;
    if (NULL == pionter) {
        return DISPLAY_BUDDER_TOP;
    }

    for (i=0; i < DISPLAY_BUDDER_TOP; i++) {
        if(pionter == qnx_virt_addr[i]) {
            return i;
        }
    }
    return DISPLAY_BUDDER_TOP;
}

static int update_linux_fbinfo_buf()
{
    hyp_fb[0] = (hyp_fbinfo_t *)linux_fbinfo_buf;
    hyp_fb[1] = (hyp_fbinfo_t *)(linux_fbinfo_buf += sizeof(hyp_fbinfo_t));
    //printf("ljm hyp_fb[0] addr %p, hyp_fb[1] addr %p\n",hyp_fb[0],hyp_fb[1]);
    while((hyp_fb[0]->isfront == 9999) || (hyp_fb[1]->isfront == 9999))
        sleep(1);
    {
        //dump fbinfo
        int i;
        for (i = 0;i < 2 ;i++)
        {
            ;//printf("fb[%d] paddr %lx, vaddr %p, len %d isfront %d\n",
            //        i,hyp_fb[i]->paddr,hyp_fb[i]->vaddr,hyp_fb[i]->len,hyp_fb[i]->isfront);
        }
    }
    return 0;
}

void *  find_front_buffer_by_paddr()
{
    static int s_mapped_agl_framebuffer = 0;
    void *fb_front_vaddr = NULL;
    int i;
    if (s_mapped_agl_framebuffer == 0)
    {
        //do the first time map here, notice, hyp_fb should be ok XXX , must be ok
        for (i = 0;i < 2 ;i++)
        {
            //printf("mapping framebuffer addr %lx\n", hyp_fb[i]->paddr);
            qnx_virt_addr[i] = mmap_device_memory( 0, hyp_fb[i]->len,
                    PROT_READ|PROT_WRITE|PROT_NOCACHE, 0, hyp_fb[i]->paddr);
            if (qnx_virt_addr[i] == MAP_FAILED) {
                perror( "mmap_device_memory for physical address failed");
                exit( EXIT_FAILURE );
            }

            //printf("fb[%d] mapped qnx device vaddr %p from %lx len %d\n",
            //        i,qnx_virt_addr[i],hyp_fb[i]->paddr,hyp_fb[i]->len);
        }
        s_mapped_agl_framebuffer = 1;
    }

    for (i = 0;i < 2 ;i++) {
        if (hyp_fb[i]->isfront == 1) {
            fb_front_vaddr = qnx_virt_addr[i];
            //printf("fb[%d] is now front vaddr %p\n", i,qnx_virt_addr[i]);
        }
    }
    //now we should get the front buffer
    qnx_current_front_buffer = fb_front_vaddr;
    sendsignal();
    return fb_front_vaddr;
}

//ljm
//this will listen the share memory read write
//and then notify demo screen app.
int shmem_main() {
    struct sigevent     ev;
    int                 channel;
    int                 coid;

    channel = ChannelCreate_r(0);
    if (channel < 0) {
        //printf("channel failure: %d\n", -channel);
        return 1;
    }
    coid = ConnectAttach_r(0, 0, channel, _NTO_SIDE_CHANNEL, 0);
    if (coid < 0) {
        printf("connect attach failure: %d\n", -coid);
        return 1;
    }
    //
    // Allocate the memory of the hyperivsor shared memory control structure
    //
    struct hyp_shm *hsp = hyp_shm_create(0);
    if (hsp == NULL) {
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
    if (err != EOK) {
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
    if (size != HYP_SHM_MAX_CLIENTS) {
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
    //printf("shared memory index %u\n", myidx);

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
    //ljm debug, this buffer is from linux kernel.
    //here should set to linux kernel index
    linux_fbinfo_buf = &shmdata[0*BUFF_SIZE];
    update_linux_fbinfo_buf();
    //agl should run, then get there.
    find_front_buffer_by_paddr();

    for ( ;; ) {
        if (0) {
            SIGEV_PULSE_INIT(&ev, coid, -1, PULSE_CODE_INPUT, NULL);
            int r = ionotify(0, _NOTIFY_ACTION_POLLARM, _NOTIFY_COND_INPUT, &ev);
            if (r & _NOTIFY_COND_INPUT) {
                MsgSendPulse(coid, -1, PULSE_CODE_INPUT, 0);
            }
            need_notify = 0;
        }
        struct _pulse       pulse;
        MsgReceivePulse(channel, &pulse, sizeof(pulse), NULL);
        switch (pulse.code) {
            case PULSE_CODE_INPUT:
                // terminal input
                //need_notify = 1;
                //int const len = read(0, mybuff, BUFF_SIZE);
                //if (len <= 0) goto done;
                //mybuff[len-1] = '\0';
                //if (strcmp(mybuff, "quit") == 0) goto done;
                //
                // The hyp_shm_poke API takes a bitset that
                // tells which other clients should be informed that something
                // has happened and they should look at the shared memory region.
                // E.g. if you want to notify the client whose index is 2, you
                // should turn on (1 << 2) in the bitset.
                // The value ~0 is specially handled to mean "everybody but me".
                //
                //hyp_shm_poke(hsp, HYP_SHM_POKE_ALL_BUT_ME);
                break;
            case PULSE_CODE_NOTIFY: {
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
                                    for (i = 0; i < HYP_SHM_MAX_CLIENTS; ++i) {
                                        if (status & (1 << i)) {
                                            //printf("connection %2d sent '%s'\n", i, &shmdata[i*BUFF_SIZE]);
                                            //we should map the two buffer first time, and get the front buffe
                                            //printf("got notify from %d\n",i);
                                            find_front_buffer_by_paddr();
                                        }
                                    }
                                    status >>= HYP_SHM_MAX_CLIENTS;
                                    if (status != attach_list) {
                                        //printf("attach/detach %4x => %4x\n", attach_list, status);
                                        attach_list = status;
                                    }
                                    break;
                                   }
            default:
                                    //printf("unexpected pulse %d\n", pulse.code);
                                    break;
        }
    }
done:

    //
    // The hyp_shm_detach() cleanly disconnects from the shared memory
    // region and cleans everything up.
    //
    hyp_shm_detach(hsp);
    //printf("destroyed!\n");
    return 0;
}

void *memory_monitor_thread(void *data)

{
    shmem_main();
    return NULL;
}

