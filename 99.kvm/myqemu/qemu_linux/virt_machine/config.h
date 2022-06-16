#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <poll.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/kvm.h>
#include <sys/prctl.h>

#include <pty.h>
#include <termios.h>
#include <linux/serial_reg.h>

#include <stdlib.h>
#include <sys/uio.h>
#include <linux/fs.h>
#include <sys/eventfd.h>
#include <linux/virtio_blk.h>
#include <linux/virtio_ring.h>

#include <net/if.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <linux/if_tun.h>
#include <linux/if_ether.h>
#include <linux/if_bridge.h>

#include "virtio_mmio.h"



#define GUEST_PHY_ADDR			0x40000000
#define GUEST_PHY_ADDR_SIZE		0x20000000

#define GUEST_IMAGE_ADD			0x40080000
#define GUEST_DTB_ADD			0x48600000


#define GUEST_BIN    			"./guest_image/Image"
#define GUEST_DTB    			"./guest_image/mykvm.dtb"
//#define GUEST_ROOTFS   			"./guest_image/rootfs.cpio.gz"
#define print_virt(fmt, ...)	printf("MY_QUEM %s => " fmt "\n", __func__, ##__VA_ARGS__)	

#define true	1
#define false	0

#define min(x, y) ({				\
	typeof(x) _min1 = (x);			\
	typeof(y) _min2 = (y);			\
	(void) (&_min1 == &_min2);		\
	_min1 < _min2 ? _min1 : _min2; })

#define min_t(type, x, y) ({                    \
	type __min1 = (x);                      \
	type __min2 = (y);                      \
	__min1 < __min2 ? __min1: __min2; })

int kvm_fd, vm_fd, vcpu_fd, gic_fd, guest_fd, guest_dtb_fd, guest_rootfs_fd;
void *userspace_addr;

#define GUEST_TO_HOST_ADDR(addr)	(userspace_addr + ((addr) - GUEST_PHY_ADDR))


#define VIRTIO_IRQ_LOW		0
#define VIRTIO_IRQ_HIGH		1

struct virt_queue {
	struct vring	vring;
	__u32			pfn;
	__u16			last_avail_idx;
	__u16			last_used_signalled;
	__u16			use_event_idx;
};

struct virtio_mmio_hdr {
	char	magic[4];
	__u32	version;
	__u32	device_id;
	__u32	vendor_id;
	__u32	host_features;
	__u32	host_features_sel;
	__u32	reserved_1[2];
	__u32	guest_features;
	__u32	guest_features_sel;
	__u32	guest_page_size;
	__u32	reserved_2;
	__u32	queue_sel;
	__u32	queue_num_max;
	__u32	queue_num;
	__u32	queue_align;
	__u32	queue_pfn;
	__u32	reserved_3[3];
	__u32	queue_notify;
	__u32	reserved_4[3];
	__u32	interrupt_state;
	__u32	interrupt_ack;
	__u32	reserved_5[2];
	__u32	status;
} __attribute__((packed));

struct virtio_mmio {
	__u32						addr;
	__u8						irq;
	struct virtio_mmio_hdr	hdr;
};

//tty
extern int ter_init(void);
extern int term_putc(char *addr, int cnt);
extern void serial8250_mmio(__u64 addr, __u8 *data, __u32 len, __u8 is_write);

//gic
extern void mgic_mmio(__u64 addr, __u8 *data, __u32 len, __u8 is_write);
extern void kvm__irq_line(int irq, int level);

//virtio_blk
extern int virtio_blk_init(void);
extern void blk_mmio(__u64 addr, __u8 *data, __u32 len, __u8 is_write);

//virtio_net
extern int virtio_net_init(void);
extern void net_mmio(__u64 addr, __u8 *data, __u32 len, __u8 is_write);




#endif /* _CONFIG_H_ */