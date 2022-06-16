#include "config.h"


static int blk_fd;
static __u64 blk_size;

#define DISK_DEV			"/dev/mmcblk0p3"
#define SECTOR_SHIFT		9
#define SECTOR_SIZE		(1UL << SECTOR_SHIFT)

/************************************************************************/

#define BLK_MEMORY_ADDR		0xa000000
#define BLK_VIRTIO_IRQ		97

static struct virtio_mmio *virtio;

/************************************************************************/

#define DISK_SEG_MAX			(VIRTIO_BLK_QUEUE_SIZE - 2)
#define VIRTIO_BLK_QUEUE_SIZE	256
#define NUM_VIRT_QUEUES			1
#define ARRAY_SIZE(x) 			(sizeof(x) / sizeof((x)[0]))

struct blk_dev_req {
	struct virt_queue		*vq;
	struct blk_dev			*bdev;
	struct iovec			iov[VIRTIO_BLK_QUEUE_SIZE];
	__u16					out, in, head;
};

struct blk_dev {
	struct virtio_blk_config	blk_config;
	__u32						features;

	struct virt_queue			vqs[NUM_VIRT_QUEUES];
	struct blk_dev_req			reqs[VIRTIO_BLK_QUEUE_SIZE];
	
	pthread_t					io_thread;
	int							io_efd;
};

static struct blk_dev *bdev;



/************************************disk************************************/

static inline ssize_t get_iov_size(const struct iovec *iov, int iovcnt)
{
	size_t size = 0;
	while (iovcnt--)
		size += (iov++)->iov_len;

	return size;
}

static ssize_t xpreadv(int fd, const struct iovec *iov, int iovcnt, off_t offset)
{
	ssize_t nr;

restart:
	
	nr = preadv(fd, iov, iovcnt, offset);
	if ((nr < 0) && ((errno == EAGAIN) || (errno == EINTR)))
		goto restart;

	return nr;
}

static inline void shift_iovec(const struct iovec **iov, int *iovcnt,
				size_t nr, ssize_t *total, size_t *count, off_t *offset)
{
	while (nr >= (*iov)->iov_len) {
		nr -= (*iov)->iov_len;
		*total += (*iov)->iov_len;
		*count -= (*iov)->iov_len;
		if (offset)
			*offset += (*iov)->iov_len;
		(*iovcnt)--;
		(*iov)++;
	}
}

static ssize_t preadv_in_full(int fd, const struct iovec *iov, int iovcnt, off_t offset)
{
	ssize_t total = 0;
	size_t count = get_iov_size(iov, iovcnt);

	while (count > 0) {
		ssize_t nr;

		nr = xpreadv(fd, iov, iovcnt, offset);
		if (nr <= 0) {
			if (total > 0)
				return total;

			return -1;
		}

		shift_iovec(&iov, &iovcnt, nr, &total, &count, &offset);
	}

	return total;
}

/* Same as pwritev(2) except that this function never returns EAGAIN or EINTR. */
static ssize_t xpwritev(int fd, const struct iovec *iov, int iovcnt, off_t offset)
{
	ssize_t nr;
	//int i;

restart:
	nr = pwritev(fd, iov, iovcnt, offset);
	//for(i = 0; i < iov->iov_len; i++)
	//	printf("0x%x,", *((char *)(iov->iov_base) + i));
	//printf("\n");
	if ((nr < 0) && ((errno == EAGAIN) || (errno == EINTR)))
		goto restart;

	return nr;
}

static ssize_t pwritev_in_full(int fd, const struct iovec *iov, int iovcnt, off_t offset)
{
	ssize_t total = 0;
	size_t count = get_iov_size(iov, iovcnt);

	while (count > 0) {
		ssize_t nr;

		nr = xpwritev(fd, iov, iovcnt, offset);
		if (nr < 0)
			return -1;
		if (nr == 0) {
			errno = ENOSPC;
			return -1;
		}

		shift_iovec(&iov, &iovcnt, nr, &total, &count, &offset);
	}

	return total;
}

static ssize_t raw_image__read_sync(__u64 sector, const struct iovec *iov, int iovcount, void *param)
{
	return preadv_in_full(blk_fd, iov, iovcount, sector << SECTOR_SHIFT);
}
static ssize_t raw_image__write_sync(__u64 sector, const struct iovec *iov, int iovcount, void *param)
{
	return pwritev_in_full(blk_fd, iov, iovcount, sector << SECTOR_SHIFT);
}
static int disk_image__flush(void)
{
	return fsync(blk_fd);
}

static int blkdev_open(void)
{
	if((blk_fd = open(DISK_DEV, O_RDWR)) < 0){
		printf("%s open fail, %s\n", DISK_DEV, strerror(errno));
		return -1;
	}

	if (ioctl(blk_fd, BLKGETSIZE64, &blk_size) < 0) {
		printf("%s ioctl fail, %s\n", DISK_DEV, strerror(errno));
		close(blk_fd);
		return -1;
	}

	printf("%s size is %dM\n", DISK_DEV, blk_size/1024/1024);

	return 0;
}

/************************************virtio************************************/

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

static int signal_vq(void)
{
	//printf("signal_vq\n");
	virtio->hdr.interrupt_state |= VIRTIO_MMIO_INT_VRING;
	kvm__irq_line(BLK_VIRTIO_IRQ, VIRTIO_IRQ_HIGH);
	kvm__irq_line(BLK_VIRTIO_IRQ, VIRTIO_IRQ_LOW);

	return 0;
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
		desc = GUEST_TO_HOST_ADDR(desc[idx].addr);//printf("11desc[idx].addr=0x%llx\n", desc[idx].addr);
		idx = 0;
	}

	do {
		/* Grab the first descriptor, and check it's OK. */
		iov[*out + *in].iov_len = desc[idx].len;
		iov[*out + *in].iov_base = GUEST_TO_HOST_ADDR(desc[idx].addr);//printf("22desc[idx].addr=0x%llx\n", desc[idx].addr);
		/* If this is an input descriptor, increment that count. */
		if (virt_desc__test_flag(vq, &desc[idx], VRING_DESC_F_WRITE))
			(*in)++;
		else
			(*out)++;
	} while ((idx = next_desc(vq, desc, idx, max)) != max);

	return head;
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

static void virtio_blk_complete(void *param, long len)
{
	struct blk_dev_req *req = param;
	struct blk_dev *bdev = req->bdev;
	int queueid = req->vq - bdev->vqs;
	__u8 *status;

	/* status */
	status	= req->iov[req->out + req->in - 1].iov_base;
	*status	= (len < 0) ? VIRTIO_BLK_S_IOERR : VIRTIO_BLK_S_OK;

	virt_queue__set_used_elem(req->vq, req->head, len);

	if (virtio_queue__should_signal(&bdev->vqs[queueid]))
		signal_vq();
}

static ssize_t disk_image__read(__u64 sector, const struct iovec *iov, int iovcount, void *param)
{
	ssize_t total = 0;

	total =raw_image__read_sync(sector, iov, iovcount, param);
	if (total < 0) {
		printf("disk_image__read error: total=%ld\n", (long)total);
		return total;
	}

	virtio_blk_complete(param, total);

	return total;
}

static ssize_t disk_image__write(__u64 sector, const struct iovec *iov, int iovcount, void *param)
{
	ssize_t total = 0;


	total = raw_image__write_sync(sector, iov, iovcount, param);
	if (total < 0) {
		printf("disk_image__write error: total=%ld\n", (long)total);
		return total;
	}


	virtio_blk_complete(param, total);

	return total;
}


static void virtio_blk_do_io_request(struct virt_queue *vq, struct blk_dev_req *req)
{
	struct virtio_blk_outhdr *req_hdr;
	ssize_t block_cnt;
	struct iovec *iov;
	__u16 out, in;
	__u32 type;
	__u64 sector;

	block_cnt	= -1;
	bdev		= req->bdev;
	iov			= req->iov;
	out			= req->out;
	in			= req->in;
	req_hdr		= iov[0].iov_base;

	type = req_hdr->type;
	sector = req_hdr->sector;

	switch (type) {
	case VIRTIO_BLK_T_IN:
		block_cnt = disk_image__read( sector, iov + 1, in + out - 2, req);//printf("VIRTIO_BLK_T_IN: block_cnt=%d\n", block_cnt);
		break;
	case VIRTIO_BLK_T_OUT:
		block_cnt = disk_image__write(sector, iov + 1, in + out - 2, req);//printf("VIRTIO_BLK_T_OUT: block_cnt=%d\n", block_cnt);
		break;
	case VIRTIO_BLK_T_FLUSH: //printf("VIRTIO_BLK_T_FLUSH\n");
		block_cnt = disk_image__flush();
		virtio_blk_complete(req, block_cnt);
		break;
	//case VIRTIO_BLK_T_GET_ID:
	//	//block_cnt = VIRTIO_BLK_ID_BYTES;
	//	//disk_image__get_serial(bdev->disk,
	//	//		(iov + 1)->iov_base, &block_cnt);
	//	//virtio_blk_complete(req, block_cnt);
	//	break;
	default:
		printf("request type %d", type);
		block_cnt	= -1;
		break;
	}
	
	//printf("vq->last_avail_idx=%d req->vq->vring.used->idx=%d\n", vq->last_avail_idx, req->vq->vring.used->idx);
}

static void virtio_blk_do_io( struct virt_queue *vq, struct blk_dev *bdev)
{
	struct blk_dev_req *req;
	__u16 head;

	while (virt_queue__available(vq)) {
		head		= virt_queue__pop(vq);
		req			= &bdev->reqs[head];
		req->head	= virt_queue__get_head_iov(vq, req->iov, &req->out, &req->in, head);
		req->vq		= vq;

		virtio_blk_do_io_request(vq, req);
	}
}

static void *virtio_blk_thread(void *dev)
{
	struct blk_dev *bdev = dev;
	__u64 data;
	int r;

	//kvm__set_thread_name
	prctl(PR_SET_NAME, "virtio-blk-io");

	while (1) {
		r = read(bdev->io_efd, &data, sizeof(__u64));
		if (r < 0)
			continue;
		//printf("get guest notify.\n");
		virtio_blk_do_io(&bdev->vqs[0], bdev);
	}

	pthread_exit(NULL);
	return NULL;
}

static int init_vq(__u32 vq, __u32 page_size, __u32 align, __u32 pfn)
{
	int i;
	void *p;
	
	bdev->vqs[vq].pfn = pfn;

	p = GUEST_TO_HOST_ADDR(bdev->vqs[vq].pfn * page_size);						//通过guest的物理地址，结算得到host端用的虚拟地址

	vring_init(&bdev->vqs[vq].vring, VIRTIO_BLK_QUEUE_SIZE, p, align);
	bdev->vqs[vq].use_event_idx = (bdev->features & VIRTIO_RING_F_EVENT_IDX);

	for (i = 0; i < ARRAY_SIZE(bdev->reqs); i++) {
		bdev->reqs[i] = (struct blk_dev_req) {
			.bdev = bdev,
		};
	}

	bdev->io_efd = eventfd(0, 0);
	if (bdev->io_efd < 0){
		printf("%s, eventfd fail.\n", __func__);
		return -errno;
	}

	if (pthread_create(&bdev->io_thread, NULL, virtio_blk_thread, bdev)){
		printf("%s, pthread_create fail.\n", __func__);
		return -errno;
	}

	return 0;
}


/************************************blk************************************/

//1~8是virtio device加载阶段，获取virtio magic、version、device_id、reset host等；
//9~18是virtio driver注册时还未probe阶段，主要是设置host status，获取features；
//19~26driver probe阶段，主要在创建虚拟队列过程
//27~29中断通知

static void virtio_mmio_config_guest_write(__u64 addr, void *data, __u32 len)
{
	__u64 iodata = 1;
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
			virtio->hdr.status = *(__u32 *)data;//printf("status=%d\n", virtio->hdr.status);
			break;
		
		case VIRTIO_MMIO_HOST_FEATURES_SEL:				//11virtio->hdr.host_features_sel=1(实际写1时是读features的高32位，写0读低32位)；
														//13virtio->hdr.host_features_sel=0
		case VIRTIO_MMIO_GUEST_FEATURES_SEL:			//15同上virtio->hdr.guest_features_sel=1
														//17同上virtio->hdr.guest_features_sel=0
		case VIRTIO_MMIO_QUEUE_SEL:						//19设置virtio->hdr.queue_sel用哪个队列，blk只设置了一个NUM_VIRT_QUEUES
			(*(__u32 *)((void *)&(virtio->hdr) + addr)) = *(__u32 *)data;
			break;
		
		//16因为15设置为1，host不读；
		//18因为15设置为0，保存guest的features
		case VIRTIO_MMIO_GUEST_FEATURES:
			if (virtio->hdr.guest_features_sel == 0) {
				bdev->features = *(__u32 *)data;
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
			if (write(bdev->io_efd, &iodata, sizeof(iodata)) < 0)
				printf("guest notify fail.\n");
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
static void virtio_mmio_config_guest_read(__u64 addr, void *data, __u32 len)
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
			if (virtio->hdr.host_features_sel == 0)
				*(__u32 *)data = 1UL << VIRTIO_BLK_F_SEG_MAX	\
							   | 1UL << VIRTIO_BLK_F_FLUSH		\
							   | 1UL << VIRTIO_BLK_F_MQ		\
							   | 1UL << VIRTIO_RING_F_EVENT_IDX	\
							   | 1UL << VIRTIO_RING_F_INDIRECT_DESC	\
							   | (0 ? 1UL << VIRTIO_BLK_F_RO : 0);
			else
				*(__u32 *)data = 0;
			break;

		case VIRTIO_MMIO_QUEUE_PFN:						//20读取host的pfn地址(guest vring的物理地址)，测试还没设置，期望读取的地址为0
			*(__u32 *)data = bdev->vqs[virtio->hdr.queue_sel].pfn;
			break;
		
		case VIRTIO_MMIO_QUEUE_NUM_MAX:					//21读取最大支持的一个队列的深度
			*(__u32 *)data = VIRTIO_BLK_QUEUE_SIZE;
			break;
		
		default:
			printf("%s error: addr=0x%x\n", __func__, addr);
			break;
	};
}

//主要就是读取了bdev->blk_config设置的三个参数
static void virtio_mmio_config_guest_specific(__u64 addr, __u8 *data, __u32 len, __u8 is_write)
{
	__u32 i;

	for (i = 0; i < len; i++) {
		if (is_write){
			((__u8 *)(&bdev->blk_config))[addr + i] = *(__u8 *)data + i;
		}else{
			data[i] = ((__u8 *)(&bdev->blk_config))[addr + i];
		}
	}
}

		
void blk_mmio(__u64 addr, __u8 *data, __u32 len, __u8 is_write)									
{
	__u32 offset = addr - BLK_MEMORY_ADDR;
	
	if (offset >= VIRTIO_MMIO_CONFIG) {
		offset -= VIRTIO_MMIO_CONFIG;
		virtio_mmio_config_guest_specific(offset, data, len, is_write);
		return;
	}

	if (is_write)
		virtio_mmio_config_guest_write(offset, data, len);
	else
		virtio_mmio_config_guest_read(offset, data, len);
}

int virtio_blk_init(void)
{

	blkdev_open();

	//blk init
	bdev = calloc(1, sizeof(struct blk_dev));
	if (!bdev)
		return -ENOMEM;
	
	bdev->blk_config.capacity = blk_size / SECTOR_SIZE;
	bdev->blk_config.seg_max = DISK_SEG_MAX;
	bdev->blk_config.num_queues = NUM_VIRT_QUEUES;			//块设备描述硬件的传输通道，一般都为1，所以我们也只创建1个vritio队列，相对于虚拟网络设备创建2个virtio队列

	//virtio init
	virtio = calloc(1, sizeof(struct virtio_mmio));
	if (!virtio)
		return -ENOMEM;

	virtio->addr = BLK_MEMORY_ADDR;
	virtio->irq = BLK_VIRTIO_IRQ;
	
	virtio->hdr.magic[0] = 'v';
	virtio->hdr.magic[1] = 'i';
	virtio->hdr.magic[2] = 'r';
	virtio->hdr.magic[3] = 't';
	virtio->hdr.version	= 1;
	virtio->hdr.device_id	= VIRTIO_ID_BLOCK;
	virtio->hdr.vendor_id	= 0x4d564b4c; /* 'LKVM' */
	virtio->hdr.queue_num_max	= 256;

	return 0;
}

								