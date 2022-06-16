#include "config.h"


static char guest_mac[] = {0x02,0x15,0x15,0x15,0x15,0x15};
static char host_ip[] = "192.168.33.1";
//static char _br[IFNAMSIZ] = "br0";

#define NET_MEMORY_ADDR		0xa001000
#define NET_VIRTIO_IRQ		98

#define VIRTIO_NET_QUEUE_SIZE		256
#define VIRTIO_NET_NUM_QUEUES		8

#define MAX_PACKET_SIZE 65550
/************************************************************************/

static struct virtio_mmio *virtio;

/************************************************************************/
/* The feature bitmap for virtio net */
#define VIRTIO_NET_F_CSUM	0	/* Host handles pkts w/ partial csum */
#define VIRTIO_NET_F_GUEST_CSUM	1	/* Guest handles pkts w/ partial csum */
#define VIRTIO_NET_F_MAC	5	/* Host has given MAC address. */
#define VIRTIO_NET_F_GSO	6	/* Host handles pkts w/ any GSO type */
#define VIRTIO_NET_F_GUEST_TSO4	7	/* Guest can handle TSOv4 in. */
#define VIRTIO_NET_F_GUEST_TSO6	8	/* Guest can handle TSOv6 in. */
#define VIRTIO_NET_F_GUEST_ECN	9	/* Guest can handle TSO[6] w/ ECN in. */
#define VIRTIO_NET_F_GUEST_UFO	10	/* Guest can handle UFO in. */
#define VIRTIO_NET_F_HOST_TSO4	11	/* Host can handle TSOv4 in. */
#define VIRTIO_NET_F_HOST_TSO6	12	/* Host can handle TSOv6 in. */
#define VIRTIO_NET_F_HOST_ECN	13	/* Host can handle TSO[6] w/ ECN in. */
#define VIRTIO_NET_F_HOST_UFO	14	/* Host can handle UFO in. */
#define VIRTIO_NET_F_MRG_RXBUF	15	/* Host can merge receive buffers. */
#define VIRTIO_NET_F_STATUS	16	/* virtio_net_config.status available */
#define VIRTIO_NET_F_CTRL_VQ	17	/* Control channel available */
#define VIRTIO_NET_F_CTRL_RX	18	/* Control channel RX mode support */
#define VIRTIO_NET_F_CTRL_VLAN	19	/* Control channel VLAN filtering */
#define VIRTIO_NET_F_CTRL_RX_EXTRA 20	/* Extra RX mode control support */
#define VIRTIO_NET_F_GUEST_ANNOUNCE 21	/* Guest can announce device on the
					 * network */
#define VIRTIO_NET_F_MQ	22	/* Device supports Receive Flow
					 * Steering */
#define VIRTIO_NET_F_CTRL_MAC_ADDR 23	/* Set MAC address */

#define VIRTIO_NET_S_LINK_UP	1	/* Link is up */
#define VIRTIO_NET_S_ANNOUNCE	2	/* Announcement is needed */

#define VIRTIO_NET_CTRL_MQ   4
#define VIRTIO_NET_OK     0
#define VIRTIO_NET_ERR    1

struct virtio_net_hdr {
#define VIRTIO_NET_HDR_F_NEEDS_CSUM	1	// Use csum_start, csum_offset
#define VIRTIO_NET_HDR_F_DATA_VALID	2	// Csum is valid
	__u8 flags;
#define VIRTIO_NET_HDR_GSO_NONE		0	// Not a GSO frame
#define VIRTIO_NET_HDR_GSO_TCPV4	1	// GSO frame, IPv4 TCP (TSO)
#define VIRTIO_NET_HDR_GSO_UDP		3	// GSO frame, IPv4 UDP (UFO)
#define VIRTIO_NET_HDR_GSO_TCPV6	4	// GSO frame, IPv6 TCP
#define VIRTIO_NET_HDR_GSO_ECN		0x80	// TCP has ECN set
	__u8 gso_type;
	__u16 hdr_len;		/* Ethernet + IP + tcp/udp hdrs */
	__u16 gso_size;		/* Bytes to append to hdr_len per frame */
	__u16 csum_start;	/* Position to start checksumming from */
	__u16 csum_offset;	/* Offset after that to place checksum */
};

/* This is the version of the header to use when the MRG_RXBUF
 * feature has been negotiated. */
struct virtio_net_hdr_mrg_rxbuf {
	struct virtio_net_hdr hdr;
	__u16 num_buffers;	/* Number of merged rx buffers */
};

struct virtio_net_ctrl_hdr {
	__u8 class;
	__u8 cmd;
} __attribute__((packed));

typedef __u8 virtio_net_ctrl_ack;


struct virtio_net_config {
	/* The config defining mac address (if VIRTIO_NET_F_MAC) */
	__u8 mac[ETH_ALEN];
	/* See VIRTIO_NET_F_STATUS and VIRTIO_NET_S_* above */
	__u16 status;
	/* Maximum number of each of transmit and receive queues;
	 * see VIRTIO_NET_F_MQ and VIRTIO_NET_CTRL_MQ.
	 * Legal values are between 1 and 0x8000
	 */
	__u16 max_virtqueue_pairs;
} __attribute__((packed));

struct virtio_net_params {
	//const char *guest_ip;
	const char *host_ip;
	//const char *script;
	//const char *downscript;
	//const char *trans;
	char tapif;
	char guest_mac[6];
	//char host_mac[6];
	//int mode;
	//int vhost;
	//int fd;
	//int mq;
};

struct mutex {
	pthread_mutex_t mutex;
};

struct net_dev_queue {
	int						id;
	struct net_dev			*ndev;
	struct virt_queue		vq;
	pthread_t				thread;
	pthread_cond_t			cond;
	
	struct mutex			lock;
};

struct net_dev {
	struct net_dev_queue		queues[VIRTIO_NET_NUM_QUEUES * 2 + 1];
	struct virtio_net_config	config;
	__u32						features, queue_pairs;
			
	int							tap_fd;
	char						tap_name[IFNAMSIZ];
	int 						tap_ufo;

	struct virtio_net_params	params;
};
struct net_dev *ndev;


/************************************************************************/

static inline int tap_ops_tx(struct iovec *iov, __u16 out, struct net_dev *ndev)
{
	return writev(ndev->tap_fd, iov, out);
}

static inline int tap_ops_rx(struct iovec *iov, __u16 in, struct net_dev *ndev)
{
	return readv(ndev->tap_fd, iov, in);
}

static int virtio_net_request_tap(struct ifreq *ifr)
{
	int ret;

	memset(ifr, 0, sizeof(*ifr));
	ifr->ifr_flags = IFF_TAP | IFF_NO_PI | IFF_VNET_HDR;

	ret = ioctl(ndev->tap_fd, TUNSETIFF, ifr);
	if (ret >= 0)
		strncpy(ndev->tap_name, ifr->ifr_name, sizeof(ndev->tap_name));
	return ret;
}

static int virtio_net_tap_create(void)
{
	int offload;
	struct ifreq ifr;

	const char *tap_file = "/dev/net/tun";

	ndev->tap_fd = open(tap_file, O_RDWR);
	if (ndev->tap_fd < 0) {
		printf("Unable to open %s", tap_file);
		return 0;
	}

	if (virtio_net_request_tap(&ifr) < 0) {
		printf("Config tap device error. Are you root?");
		goto fail;
	}

	/*
	 * The UFO support had been removed from kernel in commit:
	 * ID: fb652fdfe83710da0ca13448a41b7ed027d0a984
	 * https://www.spinics.net/lists/netdev/msg443562.html
	 * In oder to support the older kernels without this commit,
	 * we set the TUN_F_UFO to offload by default to test the status of
	 * UFO kernel support.
	 */
	ndev->tap_ufo = true;
	offload = TUN_F_CSUM | TUN_F_TSO4 | TUN_F_TSO6 | TUN_F_UFO;
	if (ioctl(ndev->tap_fd, TUNSETOFFLOAD, offload) < 0) {
		/*
		 * Is this failure caused by kernel remove the UFO support?
		 * Try TUNSETOFFLOAD without TUN_F_UFO.
		 */
		offload &= ~TUN_F_UFO;
		if (ioctl(ndev->tap_fd, TUNSETOFFLOAD, offload) < 0) {
			printf("Config tap device TUNSETOFFLOAD error");
			goto fail;
		}
		ndev->tap_ufo = false;
	}

	return 1;

fail:
	if (ndev->tap_fd >= 0)
		close(ndev->tap_fd);

	return 0;
}


/************************************************************************/

static inline int virt_desc__test_flag(struct virt_queue *vq,
					struct vring_desc *desc, __u16 flag)
{
	return !!(desc->flags & flag);
}

static unsigned next_desc(struct virt_queue *vq, struct vring_desc *desc,
			  unsigned int i, unsigned int max)
{
	unsigned int next;

	/* If this descriptor says it doesn't chain, we're done. */
	if (!virt_desc__test_flag(vq, &desc[i], VRING_DESC_F_NEXT))
		return max;

	next = desc[i].next;

	/* Ensure they're not leading us off end of descriptors. */
	return min(next, max);
}

static inline int virt_queue__available(struct virt_queue *vq)
{
	__u16 last_avail_idx = vq->last_avail_idx;

	if (!vq->vring.avail){
		printf("vring not avail.\n");
		return 0;
	}

	if (vq->use_event_idx) {
		vring_avail_event(&vq->vring) = last_avail_idx;
		/*
		 * After the driver writes a new avail index, it reads the event
		 * index to see if we need any notification. Ensure that it
		 * reads the updated index, or else we'll miss the notification.
		 */
		asm volatile ("dmb ish"		: : : "memory");
	}

	return vq->vring.avail->idx != last_avail_idx;
}

static inline __u16 virt_queue__pop(struct virt_queue *queue)
{
	/*
	 * The guest updates the avail index after writing the ring entry.
	 * Ensure that we read the updated entry once virt_queue__available()
	 * observes the new index.
	 */
	asm volatile ("dmb ishld"	: : : "memory");

	return queue->vring.avail->ring[queue->last_avail_idx++ % queue->vring.num];
}

static __u16 virt_queue__get_head_iov(struct virt_queue *vq, struct iovec iov[], __u16 *out, __u16 *in, __u16 head)
{
	struct vring_desc *desc;
	__u16 idx;
	__u16 max;

	idx = head;
	*out = *in = 0;
	max = vq->vring.num;
	desc = vq->vring.desc;

	if (virt_desc__test_flag(vq, &desc[idx], VRING_DESC_F_INDIRECT)) {
		max = desc[idx].len / sizeof(struct vring_desc);
		desc = GUEST_TO_HOST_ADDR(desc[idx].addr);
		idx = 0;
	}

	do {
		/* Grab the first descriptor, and check it's OK. */
		iov[*out + *in].iov_len = desc[idx].len;
		iov[*out + *in].iov_base = GUEST_TO_HOST_ADDR(desc[idx].addr);
		/* If this is an input descriptor, increment that count. */
		if (virt_desc__test_flag(vq, &desc[idx], VRING_DESC_F_WRITE))
			(*in)++;
		else
			(*out)++;
	} while ((idx = next_desc(vq, desc, idx, max)) != max);

	return head;
}

static __u16 virt_queue__get_iov(struct virt_queue *vq, struct iovec iov[], __u16 *out, __u16 *in)
{
	__u16 head;

	head = virt_queue__pop(vq);

	return virt_queue__get_head_iov(vq, iov, out, in, head);
}

static struct vring_used_elem *
virt_queue__set_used_elem_no_update(struct virt_queue *queue, __u32 head,
				    __u32 len, __u16 offset)
{
	struct vring_used_elem *used_elem;
	__u16 idx = queue->vring.used->idx;

	idx += offset;
	used_elem	= &queue->vring.used->ring[idx % queue->vring.num];
	used_elem->id	= head;
	used_elem->len	= len;

	return used_elem;
}

static void virt_queue__used_idx_advance(struct virt_queue *queue, __u16 jump)
{
	__u16 idx = queue->vring.used->idx;

	/*
	 * Use wmb to assure that used elem was updated with head and len.
	 * We need a wmb here since we can't advance idx unless we're ready
	 * to pass the used element to the guest.
	 */
	asm volatile ("dmb ishst"	: : : "memory");
	idx += jump;
	queue->vring.used->idx = idx;
}

static struct vring_used_elem *virt_queue__set_used_elem(struct virt_queue *queue, __u32 head, __u32 len)
{
	struct vring_used_elem *used_elem;

	used_elem = virt_queue__set_used_elem_no_update(queue, head, len, 0);
	virt_queue__used_idx_advance(queue, 1);

	return used_elem;
}

static int virtio_queue__should_signal(struct virt_queue *vq)
{
	__u16 old_idx, new_idx, event_idx;

	/*
	 * Use mb to assure used idx has been increased before we signal the
	 * guest, and we don't read a stale value for used_event. Without a mb
	 * here we might not send a notification that we need to send, or the
	 * guest may ignore the queue since it won't see an updated idx.
	 */
	asm volatile ("dmb ish"		: : : "memory");

	if (!vq->use_event_idx) {
		/*
		 * When VIRTIO_RING_F_EVENT_IDX isn't negotiated, interrupt the
		 * guest if it didn't explicitly request to be left alone.
		 */
		return !(vq->vring.avail->flags & VRING_AVAIL_F_NO_INTERRUPT);
	}

	old_idx		= vq->last_used_signalled;
	new_idx		= vq->vring.used->idx;
	event_idx	= vring_used_event(&vq->vring);

	if (vring_need_event(event_idx, new_idx, old_idx)) {
		vq->last_used_signalled = new_idx;
		return 1;
	}

	return -1;
}


/************************************************************************/

static int is_ctrl_vq(struct net_dev *ndev, __u32 vq)
{
	return vq == (__u32)(ndev->queue_pairs * 2);
}
static inline void mutex_init(struct mutex *lock)
{
	if (pthread_mutex_init(&lock->mutex, NULL) != 0)
		printf("unexpected pthread_mutex_init() failure!");
}
static inline void mutex_lock(struct mutex *lock)
{
	if (pthread_mutex_lock(&lock->mutex) != 0)
		printf("unexpected pthread_mutex_lock() failure!");

}
static inline void mutex_unlock(struct mutex *lock)
{
	if (pthread_mutex_unlock(&lock->mutex) != 0)
		printf("unexpected pthread_mutex_unlock() failure!");
}

static int signal_vq(void)
{
	//printf("signal_vq\n");
	virtio->hdr.interrupt_state |= VIRTIO_MMIO_INT_VRING;
	kvm__irq_line(NET_VIRTIO_IRQ, VIRTIO_IRQ_HIGH);
	kvm__irq_line(NET_VIRTIO_IRQ, VIRTIO_IRQ_LOW);

	return 0;
}

static virtio_net_ctrl_ack virtio_net_handle_mq(struct net_dev *ndev, struct virtio_net_ctrl_hdr *ctrl)
{
	/* Not much to do here */
	return VIRTIO_NET_OK;
}

static inline size_t iov_size(const struct iovec *iovec, size_t len)
{
	size_t size = 0, i;

	for (i = 0; i < len; i++)
		size += iovec[i].iov_len;

	return size;
}

int memcpy_toiovec(struct iovec *iov, unsigned char *kdata, int len)
{
	while (len > 0) {
		if (iov->iov_len) {
			int copy = min_t(unsigned int, iov->iov_len, len);
			memcpy(iov->iov_base, kdata, copy);
			kdata += copy;
			len -= copy;
			iov->iov_len -= copy;
			iov->iov_base += copy;
		}
		iov++;
	}

	return 0;
}

static int has_virtio_feature(struct net_dev *ndev, __u32 feature)
{
	return ndev->features & (1 << feature);
}

static void *virtio_net_ctrl_thread(void *p)
{
	struct iovec iov[VIRTIO_NET_QUEUE_SIZE];
	struct net_dev_queue *queue = p;
	struct virt_queue *vq = &queue->vq;
	struct net_dev *ndev = queue->ndev;
	__u16 out, in, head;
	struct virtio_net_ctrl_hdr *ctrl;
	virtio_net_ctrl_ack *ack;

	prctl(PR_SET_NAME, "virtio-net-ctrl");

	while (1) {
		mutex_lock(&queue->lock);
		if (!virt_queue__available(vq))
			pthread_cond_wait(&queue->cond, &queue->lock.mutex);
		mutex_unlock(&queue->lock);

		while (virt_queue__available(vq)) {
			head = virt_queue__get_iov(vq, iov, &out, &in);
			ctrl = iov[0].iov_base;
			ack = iov[out].iov_base;

			switch (ctrl->class) {
				case VIRTIO_NET_CTRL_MQ:
					*ack = virtio_net_handle_mq(ndev, ctrl);
					break;
				default:
					*ack = VIRTIO_NET_ERR;
					break;
			}
			virt_queue__set_used_elem(vq, head, iov[out].iov_len);
		}
		
		if(virtio_queue__should_signal(vq))
			signal_vq();
	}

	pthread_exit(NULL);

	return NULL;
}

static void *virtio_net_tx_thread(void *p)
{
	struct iovec iov[VIRTIO_NET_QUEUE_SIZE];
	struct net_dev_queue *queue = p;
	struct virt_queue *vq = &queue->vq;
	struct net_dev *ndev = queue->ndev;
	__u16 out, in;
	__u16 head;
	int len;

	prctl(PR_SET_NAME, "virtio-net-tx");

	while (1) {
		mutex_lock(&queue->lock);
		if (!virt_queue__available(vq))
			pthread_cond_wait(&queue->cond, &queue->lock.mutex);
		mutex_unlock(&queue->lock);

		while (virt_queue__available(vq)) {
			head = virt_queue__get_iov(vq, iov, &out, &in);
			len = tap_ops_tx(iov, out, ndev);
			if (len < 0) {
				printf("%s: tx on vq %u failed (%d)\n",__func__, queue->id, errno);
				goto out_err;
			}

			virt_queue__set_used_elem(vq, head, len);
		}

		if (virtio_queue__should_signal(vq))
			signal_vq();
	}

out_err:
	pthread_exit(NULL);
	return NULL;
}

static void *virtio_net_rx_thread(void *p)
{
	struct iovec iov[VIRTIO_NET_QUEUE_SIZE];
	struct net_dev_queue *queue = p;
	struct virt_queue *vq = &queue->vq;
	struct net_dev *ndev = queue->ndev;
	__u16 out, in;
	__u16 head;
	int len, copied;

	prctl(PR_SET_NAME, "virtio-net-rx");

	while (1) {
		mutex_lock(&queue->lock);
		if (!virt_queue__available(vq))
			pthread_cond_wait(&queue->cond, &queue->lock.mutex);
		mutex_unlock(&queue->lock);

		while (virt_queue__available(vq)) {
			unsigned char buffer[MAX_PACKET_SIZE + sizeof(struct virtio_net_hdr_mrg_rxbuf)];
			struct iovec dummy_iov = {
				.iov_base = buffer,
				.iov_len  = sizeof(buffer),
			};
			struct virtio_net_hdr_mrg_rxbuf *hdr;
			__u16 num_buffers;

			len = tap_ops_rx(&dummy_iov, 1, ndev);
			if (len < 0) {
				printf("%s: rx on vq %u failed (%d), exiting thread\n", __func__, queue->id, len);
				goto out_err;
			}

			copied = num_buffers = 0;
			head = virt_queue__get_iov(vq, iov, &out, &in);
			hdr = iov[0].iov_base;
			while (copied < len) {
				size_t iovsize = min_t(size_t, len - copied, iov_size(iov, in));

				memcpy_toiovec(iov, buffer + copied, iovsize);
				copied += iovsize;
				virt_queue__set_used_elem_no_update(vq, head, iovsize, num_buffers++);
				if (copied == len)
					break;
				while (!virt_queue__available(vq))
					sleep(0);
				head = virt_queue__get_iov(vq, iov, &out, &in);
			}

			if (has_virtio_feature(ndev, VIRTIO_NET_F_MRG_RXBUF))
				hdr->num_buffers =  num_buffers;

			virt_queue__used_idx_advance(vq, num_buffers);

			/* We should interrupt guest right now, otherwise latency is huge. */
			if (virtio_queue__should_signal(vq))
				signal_vq();
		}
	}

out_err:
	pthread_exit(NULL);
	return NULL;

}

static int init_vq(__u32 vq, __u32 page_size, __u32 align, __u32 pfn)
{
//	int i;

	struct net_dev_queue *net_queue;
	struct virt_queue *queue;
	void *p;


	net_queue		= &ndev->queues[vq];
	net_queue->id	= vq;
	net_queue->ndev	= ndev;
	queue			= &net_queue->vq;
	queue->pfn		= pfn;

	p = GUEST_TO_HOST_ADDR(queue->pfn * page_size);						//通过guest的物理地址，结算得到host端用的虚拟地址

	vring_init(&queue->vring, VIRTIO_NET_QUEUE_SIZE, p, align);
	queue->use_event_idx = (ndev->features & VIRTIO_RING_F_EVENT_IDX);

	mutex_init(&net_queue->lock);
	pthread_cond_init(&net_queue->cond, NULL);
	if (is_ctrl_vq(ndev, vq)) {
		pthread_create(&net_queue->thread, NULL, virtio_net_ctrl_thread, net_queue);
		return 0;
	}else{
		if (vq & 1)
			pthread_create(&net_queue->thread, NULL, virtio_net_tx_thread, net_queue);
		else
			pthread_create(&net_queue->thread, NULL, virtio_net_rx_thread, net_queue);

		return 0;
	}

	return 0;
}

/************************************net************************************/

static void notify_vq(int queue)
{
	struct net_dev_queue *net_queue = &ndev->queues[queue];

	if ((__u32)queue >= (ndev->queue_pairs * 2 + 1)) {
		printf("Unknown queue index %u", queue);
		return;
	}

	mutex_lock(&net_queue->lock);
	pthread_cond_signal(&net_queue->cond);
	mutex_unlock(&net_queue->lock);
}

//static int bridge_init(void)
//{
//	int br_socket_fd = -1;
//	unsigned long arg[3] = {BRCTL_ADD_BRIDGE, (unsigned long) _br};
//
//	printf("%s\n", __func__);
//
//	if ((br_socket_fd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0){
//		printf("bridge_init fail: %s", strerror(errno));
//		return -1;
//	}
//
//	if ((ioctl(br_socket_fd, SIOCSIFBR, arg)) < 0){
//		printf("bridge add fail: %s", strerror(errno));
//		return -1;
//	}
//	return 0;
//}

static int virtio_net_exec_script(const char* script)
{
	pid_t pid;
	int status;

	pid = fork();
	if (pid == 0) {
		execl(script, script, NULL);
		_exit(1);
	} else {
		waitpid(pid, &status, 0);
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
			printf("Fail to setup tap by %s", script);
			return -1;
		}
	}
	return 0;
}

static int virtio_net__tap_init(void)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	int hdr_len;
	struct sockaddr_in sin = {0};
	struct ifreq ifr;
	const struct virtio_net_params *params = &ndev->params;
	int skipconf = !!params->tapif;

	//bridge_init();

	hdr_len = has_virtio_feature(ndev, VIRTIO_NET_F_MRG_RXBUF) ?
			sizeof(struct virtio_net_hdr_mrg_rxbuf) :
			sizeof(struct virtio_net_hdr);
	if(ioctl(ndev->tap_fd, TUNSETVNETHDRSZ, &hdr_len) < 0)
		printf("Config tap device TUNSETVNETHDRSZ error");

	if(!skipconf){
		memset(&ifr, 0, sizeof(ifr));
		strncpy(ifr.ifr_name, ndev->tap_name, sizeof(ifr.ifr_name));
		sin.sin_addr.s_addr = inet_addr(params->host_ip);
		memcpy(&(ifr.ifr_addr), &sin, sizeof(ifr.ifr_addr));
		ifr.ifr_addr.sa_family = AF_INET;
		if (ioctl(sock, SIOCSIFADDR, &ifr) < 0) {
			printf("Could not set ip address on tap device");
			goto fail;
		}
	}

	if(!skipconf){
		memset(&ifr, 0, sizeof(ifr));
		strncpy(ifr.ifr_name, ndev->tap_name, sizeof(ifr.ifr_name));
		ioctl(sock, SIOCGIFFLAGS, &ifr);
		ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
		if (ioctl(sock, SIOCSIFFLAGS, &ifr) < 0)
			printf("Could not bring tap device up");

		//memset(&ifr, 0, sizeof(ifr));
		//strncpy(ifr.ifr_name, _br, sizeof(_br));
		//ioctl(sock, SIOCGIFFLAGS, &ifr);
		//ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
		//if (ioctl(sock, SIOCSIFFLAGS, &ifr) < 0)
		//	printf("Could not bring %s device up", _br);
	}

	close(sock);

	return 1;

fail:
	if (sock >= 0)
		close(sock);
	if (ndev->tap_fd >= 0)
		close(ndev->tap_fd);

	return 0;
}

static void virtio_mmio_config_guest_write_net(__u64 addr, void *data, __u32 len)
{
	__u32 val=0;
	
	switch (addr) {
		case VIRTIO_MMIO_GUEST_PAGE_SIZE:  				//5
			virtio->hdr.guest_page_size = *(__u32 *)data;
		break;
		//6(注册virtio device时向host写0来复位)
		//8向host写VIRTIO_CONFIG_S_ACKNOWLEDGE(1)通知host有个device注册
		//10向host写VIRTIO_CONFIG_S_DRIVER(2)通知host有个driver开始probe
		//26向host写VIRTIO_CONFIG_S_DRIVER_OK(4)通知host有个driver初始化完成；若没设置，将会在virtio.c的probe后设置
		case VIRTIO_MMIO_STATUS:
			virtio->hdr.status = *(__u32 *)data;
			virtio_net__tap_init();
			break;

		case VIRTIO_MMIO_HOST_FEATURES_SEL:				//11virtio->hdr.host_features_sel=1(实际写1时是读features的高32位，写0读低32位)；
														//13virtio->hdr.host_features_sel=0
		case VIRTIO_MMIO_GUEST_FEATURES_SEL:			//15同上virtio->hdr.guest_features_sel=1
														//17同上virtio->hdr.guest_features_sel=0
		case VIRTIO_MMIO_QUEUE_SEL:						//19设置virtio->hdr.queue_sel用哪个队列，blk只设置了一个NUM_VIRT_QUEUES
			(*(__u32 *)((void *)&(virtio->hdr) + addr)) = *(__u32 *)data;
			//printf("(*(__u32 *)((void *)&(virtio->hdr) + addr))=%d\n", (*(__u32 *)((void *)&(virtio->hdr) + addr)));
			break;

		//16因为15设置为1，host不读；
		//18因为15设置为0，保存guest的features
		case VIRTIO_MMIO_GUEST_FEATURES:
			if (virtio->hdr.guest_features_sel == 0) {
				ndev->features = *(__u32 *)data;
			}
			break;

		case VIRTIO_MMIO_QUEUE_NUM:						//22guest写queue num(256)地址给到host
			virtio->hdr.queue_num = *(__u32 *)data;
			break;

		case VIRTIO_MMIO_QUEUE_ALIGN:					//23guest写page size(4096)大小给host
			virtio->hdr.queue_align = *(__u32 *)data;
			break;

		case VIRTIO_MMIO_QUEUE_PFN:						//24最重要的，guest写vring地址给到host
			init_vq(virtio->hdr.queue_sel, virtio->hdr.guest_page_size, virtio->hdr.queue_align, *(__u32 *)data);
			break;

		case VIRTIO_MMIO_QUEUE_NOTIFY:					//27后续通知方式
			notify_vq(*(__u32 *)data);
			break;

		case VIRTIO_MMIO_INTERRUPT_ACK:					//29接受到gest中断应答，清除中断状态
			val = *(__u32 *)data;
			virtio->hdr.interrupt_state &= ~val;
			break;

		default:
			printf("%s error: addr=0x%x\n", __func__, addr);
			break;
	};
}
static void virtio_mmio_config_guest_read_net(__u64 addr, void *data, __u32 len)
{
	switch (addr) {
		case VIRTIO_MMIO_MAGIC_VALUE:  					//1
		case VIRTIO_MMIO_VERSION:						//2
		case VIRTIO_MMIO_DEVICE_ID:						//3
		case VIRTIO_MMIO_VENDOR_ID:						//4
		case VIRTIO_MMIO_STATUS:						//7,9,25
		case VIRTIO_MMIO_INTERRUPT_STATUS:				//28注入中断后，guest读中断状态
			*(__u32 *)data = (*(__u32 *)((void *)&(virtio->hdr) + addr));
			break;
		
		//12因为11设置为1，guest读取为默认值0；
		//14返回host_features
		case VIRTIO_MMIO_HOST_FEATURES:
			if (virtio->hdr.host_features_sel == 0){
				*(__u32 *)data = 1UL << VIRTIO_NET_F_MAC
								| 1UL << VIRTIO_NET_F_CSUM
								| 1UL << VIRTIO_NET_F_HOST_TSO4
								| 1UL << VIRTIO_NET_F_HOST_TSO6
								| 1UL << VIRTIO_NET_F_GUEST_TSO4
								| 1UL << VIRTIO_NET_F_GUEST_TSO6
								| 1UL << VIRTIO_RING_F_EVENT_IDX
								| 1UL << VIRTIO_RING_F_INDIRECT_DESC
								| 1UL << VIRTIO_NET_F_CTRL_VQ
								| 1UL << VIRTIO_NET_F_MRG_RXBUF
								| 1UL << (ndev->queue_pairs > 1 ? VIRTIO_NET_F_MQ : 0);
				/*
				* The UFO feature for host and guest only can be enabled when the
				* kernel has TAP UFO support.
				*/
				if (ndev->tap_ufo)
					*(__u32 *)data |= (1UL << VIRTIO_NET_F_HOST_UFO
									| 1UL << VIRTIO_NET_F_GUEST_UFO);
			}
			else
				*(__u32 *)data = 0;
			break;

		case VIRTIO_MMIO_QUEUE_PFN:						//20读取host的pfn地址(guest vring的物理地址)，测试还没设置，期望读取的地址为0
			*(__u32 *)data = ndev->queues[virtio->hdr.queue_sel].vq.pfn;
			break;
		
		case VIRTIO_MMIO_QUEUE_NUM_MAX:					//21读取最大支持的一个队列的深度
			*(__u32 *)data = VIRTIO_NET_QUEUE_SIZE;
			break;
		
		default:
			printf("%s error: addr=0x%x\n", __func__, addr);
			break;
	};
}

//主要就是读取了ndev->config设置的mac地址
static void virtio_net_mmio_config_guest_specific(__u64 addr, __u8 *data, __u32 len, __u8 is_write)
{
	__u32 i;

	for (i = 0; i < len; i++) {
		if (is_write){
			((__u8 *)(&ndev->config))[addr + i] = *(__u8 *)data + i;
		}else{
			data[i] = ((__u8 *)(&ndev->config))[addr + i];
		}
	}
}

void net_mmio(__u64 addr, __u8 *data, __u32 len, __u8 is_write)									
{
	__u32 offset = addr - NET_MEMORY_ADDR;
	
	if (offset >= VIRTIO_MMIO_CONFIG) {
		offset -= VIRTIO_MMIO_CONFIG;
		virtio_net_mmio_config_guest_specific(offset, data, len, is_write);
		return;
	}

	if (is_write)
		virtio_mmio_config_guest_write_net(offset, data, len);
	else
		virtio_mmio_config_guest_read_net(offset, data, len);
}

int virtio_net_init(void)
{
	int i;

	//net init
	ndev = calloc(1, sizeof(struct net_dev));
	if (ndev == NULL)
		return -ENOMEM;

	ndev->params.host_ip = host_ip;
	ndev->queue_pairs = 1;		//一对TX、RX

	for (i = 0 ; i < 6 ; i++) {
		ndev->config.mac[i] = guest_mac[i];
	}


	if (virtio_net_tap_create() < 0)
		printf("You have requested a TAP device, but creation of one has failed because");

	//virtio init
	virtio = calloc(1, sizeof(struct virtio_mmio));
	if (!virtio)
		return -ENOMEM;

	virtio->addr = NET_MEMORY_ADDR;
	virtio->irq = NET_VIRTIO_IRQ;

	virtio->hdr.magic[0] = 'v';
	virtio->hdr.magic[1] = 'i';
	virtio->hdr.magic[2] = 'r';
	virtio->hdr.magic[3] = 't';
	virtio->hdr.version	= 1;
	virtio->hdr.device_id	= VIRTIO_ID_NET;
	virtio->hdr.vendor_id	= 0x4d564b4c; /* 'LKVM' */
	virtio->hdr.queue_num_max	= 256;

	virtio_net_exec_script("./net_config/net_config.sh");

	return 0;
}










