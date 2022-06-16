#ifndef __UVC_CAMERA_H__
#define __UVC_CAMERA_H__

#include <linux/usb.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/debugfs.h>
#include <asm/unaligned.h>
#include <linux/usb/video.h>
#include <linux/videodev2.h>


#include <media/media-device.h>
#include <media/v4l2-device.h>

#include <uapi/linux/uvcvideo.h>
#include <uapi/linux/v4l2-controls.h>

#include <media/v4l2-ioctl.h>
#include <media/v4l2-event.h>
#include <media/videobuf2-core.h>
#include <media/videobuf2-v4l2.h>
#include <media/videobuf2-vmalloc.h>
#include <media/videobuf2-dma-contig.h>

#include <linux/dmaengine.h>
#include <linux/dma-mapping.h>

#define PRINT_INFO			1
#define PRINT_ERROR			1
#define PRINT_PARSE			1
#define PRINT_CTRL			1
#define PRINT_VIDEO			1
#define PRINT_DEBUG			0

#define print_uvc(flag, fmt, ...)	\
	if(flag)	\
		printk("uvc_camera: %s line %d => " fmt "\n", __func__, __LINE__, ##__VA_ARGS__)



#define UVC_TERM_INPUT				0x0000
#define UVC_TERM_OUTPUT				0x8000
#define UVC_TERM_DIRECTION(term)	((term)->type & 0x8000)

#define UVC_ENTITY_TYPE(entity)		((entity)->type & 0x7fff)
#define UVC_ENTITY_IS_TERM(entity)	(((entity)->type & 0xff00) != 0)
#define UVC_ENTITY_IS_ITERM(entity)	(UVC_ENTITY_IS_TERM(entity) && ((entity)->type & 0x8000) == UVC_TERM_INPUT)
#define UVC_ENTITY_IS_OTERM(entity)	(UVC_ENTITY_IS_TERM(entity) && ((entity)->type & 0x8000) == UVC_TERM_OUTPUT)



/* GUIDS */
#define UVC_GUID_UVC_CAMERA \
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}
#define UVC_GUID_UVC_MEDIA_TRANSPORT_INPUT \
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03}
#define UVC_GUID_UVC_PROCESSING \
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01}

#define UVC_GUID_FORMAT_YUY2 \
	{ 'Y', 'U', 'Y', '2', 0x00, 0x00, 0x10, 0x00, \
	0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}

/* Number of isochronous URBs. */
#define UVC_URBS					5
/* Maximum number of packets per URB. */	
#define UVC_MAX_PACKETS				32
	
/* Devices quirks */
#define UVC_QUIRK_PROBE_MINMAX		0x00000002
#define UVC_QUIRK_STREAM_NO_FID		0x00000010
#define UVC_QUIRK_BUILTIN_ISIGHT	0x00000008



#define UVC_FMT_FLAG_COMPRESSED 	0x00000001
#define UVC_QUIRK_FORCE_Y8			0x00000800


#define UVC_CTRL_CONTROL_TIMEOUT	500
#define UVC_CTRL_STREAMING_TIMEOUT	5000

/*******************************************************************************/

#define for_each_uvc_urb(uvc_urb, uvc_streaming)	\
	for((uvc_urb) = &(uvc_streaming)->uvc_urb[0]; \
		(uvc_urb) < &(uvc_streaming)->uvc_urb[UVC_URBS]; \
		++(uvc_urb))

		
		
/*******************************************************************************/

struct uvc_video_chain {
	struct uvc_device *dev;
	struct list_head list;

	struct list_head entities;			/* All entities */
	struct uvc_entity *processing;		/* Processing unit */

	struct mutex ctrl_mutex;			/* Protects ctrl.info */

	struct v4l2_prio_state prio;		/* V4L2 priority state */
	
	u32 caps;							/* V4L2 chain-wide caps */
};


/*******************************************************************************/


enum uvc_buffer_state {
	UVC_BUF_STATE_IDE		= 0,
	UVC_BUF_STATE_QUEUED	= 1,
	UVC_BUF_STATE_ACTIVE	= 2,
	UVC_BUF_STATE_READY		= 3,
	UVC_BUF_STATE_DONE		= 4,
	UVC_BUF_STATE_ERROR		= 5,
};

#define UVC_QUEUE_DISCONNECTED		(1 << 0)
#define UVC_QUEUE_DROP_CORRUPTED	(1 << 1)

struct uvc_device_info{
	u32 quirks;
	u32 meta_format;
};

struct uvc_device{
	struct usb_device *udev;
	struct usb_interface *intf;
	
	char name[32];
	u32 quirks;
	int intfnum;
	const struct uvc_device_info *info;
	
	struct mutex lock;
	unsigned int users;

#ifdef CONFIG_MEDIA_CONTROLLER
	struct media_device mdev;
#endif
	struct v4l2_device vdev;

	/* video control interface */
	u16 uvc_version;
	u32 clock_frequency;
	
	struct list_head entities;
	struct list_head chains;
	
	/* Video Streaming interface */
	struct list_head streams;
	struct kref ref;
	
	/* Status Interrupt Endpoint */
	struct usb_host_endpoint *int_ep;
	struct urb *int_urb;

	struct uvc_ctrl_work {
		struct work_struct work;
		struct urb *urb;
		struct uvc_video_chain *chain;
		struct uvc_control *ctrl;
		const void *data;
	} async_ctrl;
};

struct uvc_frame{
	u8 bFrameIndex;
	u8 bmCapabilities;
	u16 wWidth;
	u16 wHeight;
	u32 dwMinBitRate;
	u32 dwMaxBitRate;
	u32 dwMaxVideoFrameBufferSize;
	u8 bFrameIntervalType;
	u32 dwDefaultFrameInterval;
	u32 *dwFrameInterval;
};

struct uvc_format{
	u8 type;
	u8 index;
	u8 bpp;
	u8 colorspace;
	u32 fcc;					//compressed formats
	u32 flags;
	
	char name[32];
	
	unsigned int nframes;
	struct uvc_frame *frame;
};

struct uvc_streaming_header{
	u8 bNumFormats;
	u8 bEndpointAddress;
	u8 bTerminalLink;
	u8 bControlSize;
	u8 *bmaControls;
	/* The following fields are used by input headers only. */
	u8 bmInfo;
	u8 bStillCaptureMethod;
	u8 bTriggerSupport;
	u8 bTriggerUsage;
};

struct uvc_buffer{
	struct vb2_v4l2_buffer buf;
	struct list_head queue;
	
	enum uvc_buffer_state state;
	unsigned int error;
	
	void *mem;
	unsigned int length;
	unsigned int bytesused;
	
	u32 pts;
	
	/* Asynchronous buffer handling */
	struct kref ref;
};

struct uvc_copy_op{
	struct uvc_buffer *buf;
	void *dst;
	const __u8 *src;
	dma_addr_t dma_src;
	size_t len;
};
/**
 * struct uvc_urb - URB context management structure
 *
 * @urb: the URB described by this context structure
 * @stream: UVC streaming context
 * @buffer: memory storage for the URB
 * @dma: DMA coherent addressing for the urb_buffer
 * @async_operations: counter to indicate the number of copy operations
 * @copy_operations: work descriptors for asynchronous copy operations
 * @work: work queue entry for asynchronous decode
 */
struct uvc_urb{
	struct urb *urb;
	struct uvc_streaming *stream;
	
	char *buffer;
	dma_addr_t dma;
	
	unsigned int async_operations;
	struct uvc_copy_op copy_operations[UVC_MAX_PACKETS];
	struct work_struct work;
};

struct uvc_video_queue{
	struct vb2_queue queue;
	struct mutex mutex;
	
	unsigned int flags;
	unsigned int buf_used;
	
	spinlock_t irqlock;
	struct list_head irqqueue;
};

struct uvc_stats_frame {
	unsigned int size;		/* Number of bytes captured */
	unsigned int first_data;	/* Index of the first non-empty packet */

	unsigned int nb_packets;	/* Number of packets */
	unsigned int nb_empty;		/* Number of empty packets */
	unsigned int nb_invalid;	/* Number of packets with an invalid header */
	unsigned int nb_errors;		/* Number of packets with the error bit set */

	unsigned int nb_pts;		/* Number of packets with a PTS timestamp */
	unsigned int nb_pts_diffs;	/* Number of PTS differences inside a frame */
	unsigned int last_pts_diff;	/* Index of the last PTS difference */
	bool has_initial_pts;		/* Whether the first non-empty packet has a PTS */
	bool has_early_pts;		/* Whether a PTS is present before the first non-empty packet */
	u32 pts;			/* PTS of the last packet */

	unsigned int nb_scr;		/* Number of packets with a SCR timestamp */
	unsigned int nb_scr_diffs;	/* Number of SCR.STC differences inside a frame */
	u16 scr_sof;			/* SCR.SOF of the last packet */
	u32 scr_stc;			/* SCR.STC of the last packet */
};

struct uvc_stats_stream {
	ktime_t start_ts;		/* Stream start timestamp */
	ktime_t stop_ts;		/* Stream stop timestamp */

	unsigned int nb_frames;		/* Number of frames */

	unsigned int nb_packets;	/* Number of packets */
	unsigned int nb_empty;		/* Number of empty packets */
	unsigned int nb_invalid;	/* Number of packets with an invalid header */
	unsigned int nb_errors;		/* Number of packets with the error bit set */

	unsigned int nb_pts_constant;	/* Number of frames with constant PTS */
	unsigned int nb_pts_early;	/* Number of frames with early PTS */
	unsigned int nb_pts_initial;	/* Number of frames with initial PTS */

	unsigned int nb_scr_count_ok;	/* Number of frames with at least one SCR per non empty packet */
	unsigned int nb_scr_diffs_ok;	/* Number of frames with varying SCR.STC */
	unsigned int scr_sof_count;	/* STC.SOF counter accumulated since stream start */
	unsigned int scr_sof;		/* STC.SOF of the last packet */
	unsigned int min_sof;		/* Minimum STC.SOF value */
	unsigned int max_sof;		/* Maximum STC.SOF value */
};

#define UVC_METATADA_BUF_SIZE	1024

struct uvc_streaming{
	struct list_head list;
	struct uvc_device *dev;
	struct video_device vdev;
	struct uvc_video_chain *chain;
	atomic_t active;
	
	
	struct usb_interface *intf;
	int intfnum;
	u16 maxpsize;

	struct uvc_streaming_header header;
	enum v4l2_buf_type type;

	unsigned int nformats;
	struct uvc_format *format;
	
	struct uvc_streaming_control ctrl;
	struct uvc_format *def_format;
	struct uvc_format *cur_format;
	struct uvc_frame *cur_frame;

	
	/* Buffers queue. */
	struct uvc_video_queue queue;
	struct workqueue_struct *async_wq;
	
	/* protect access to ctrl, cur_format, cur_frame and hardware video probe control. */
	struct mutex mutex;
	
	/* Buffers queue. */
	unsigned int frozen : 1;
	
	void (*decode)(struct uvc_urb *uvc_urb, struct uvc_buffer *buf, struct uvc_buffer *meta_buf);
	
	struct {
		struct video_device vdev;
		struct uvc_video_queue queue;
		u32 format;
	} meta;
	
	/* Context data used by the bulk completion headler */
	struct {
		u8 header[256];
		unsigned int header_size;
		int skip_payload;
		u32 payload_size;
		u32 max_payload_size;
	} bulk;

	struct uvc_urb uvc_urb[UVC_URBS];
	unsigned int urb_size;
	
	u32 sequence;
	u8 last_fid;
	
	
	
	
	
	struct {
		struct uvc_stats_frame frame;
		struct uvc_stats_stream stream;
	} stats;
	
	/* Timestamps support */
	struct uvc_clock{
		struct uvc_clock_sample {
			u32 dev_stc;
			u16 dev_sof;
			u16 host_sof;
			ktime_t host_time;
		} *samples;
		
		unsigned int head;
		unsigned int count;
		unsigned int size;
		
		u16 last_sof;
		u16 sof_offset;
		
		u8 last_scr[6];
		
		spinlock_t lock;
	} clock;

	struct dma_chan *chan;						//DMA主结构体
	dma_cap_mask_t dma_test_mask;				//位掩码，设置dma通道的传输类型
	struct completion rx_dma_complete;			//dma完成flag
};

struct uvc_format_desc{
	char *name;
	u8 guid[16];
	u32 fcc;
};




int uvc_request_buffers(struct uvc_video_queue *queue, struct v4l2_requestbuffers *rb);
int uvc_query_buffer(struct uvc_video_queue *queue, struct v4l2_buffer *buf);
int uvc_queue_buffer(struct uvc_video_queue *queue, struct media_device *mdev, struct v4l2_buffer *buf);
int uvc_queue_streamon(struct uvc_video_queue *queue, enum v4l2_buf_type type);
int uvc_dequeue_buffer(struct uvc_video_queue *queue, struct v4l2_buffer *buf, int nonblocking);

int uvc_queue_allocated(struct uvc_video_queue *queue);
void uvc_queue_buffer_release(struct uvc_buffer *buf);

int uvc_queue_init(struct uvc_video_queue *queue, enum v4l2_buf_type type, int drop_corrupted);

void uvc_queue_cancel(struct uvc_video_queue *queue, int disconnect);
struct uvc_buffer *uvc_queue_next_buffer(struct uvc_video_queue *queue, struct uvc_buffer *buf);
struct uvc_buffer *uvc_queue_get_current_buffer(struct uvc_video_queue *queue);


/*******************************************************************************/

/* V4L2 interface */
extern const struct v4l2_ioctl_ops uvc_ioctl_ops;
extern const struct v4l2_file_operations uvc_fops;

static inline int uvc_queue_streaming(struct uvc_video_queue *queue)
{
	return vb2_is_streaming(&queue->queue);
}


/*******************************************************************************/

enum uvc_handle_state{
	UVC_HANDLE_PASSIVE = 0,
	UVC_HANDLE_ACTIVE  = 1,
};

int uvc_status_start(struct uvc_device *dev, gfp_t flags);
void uvc_status_stop(struct uvc_device *dev);
void uvc_status_cleanup(struct uvc_device *dev);


/*******************************************************************************/



struct uvc_control_mapping{
	struct list_head list;
	struct list_head ev_subs;

	u32 id;
	u8 name[32];
	u8 entity[16];
	u8 selector;

	u8 size;
	u8 offset;
	enum v4l2_ctrl_type v4l2_type;
	u32 data_type;

	const struct uvc_menu_info *menu_info;
	u32 menu_count;

	u32 master_id;
	u32 master_manual;
	u32 slave_ids[2];

	s32 (*get)(struct uvc_control_mapping *mapping, u8 query, const u8 *data);
	void (*set)(struct uvc_control_mapping *mapping, s32 value, u8 *data);
};

/* Put the most frequently accessed fields at the beginning of structures to maximize cache affiiency */
struct uvc_control_info{
	struct list_head mappings;
	
	u8 entity[16];
	u8 index;					/* Bit idex in bmControls */
	u8 selector;
	
	u16 size;
	u32 flags;
};

struct uvc_fh{
	struct v4l2_fh vfh;
	struct uvc_video_chain *chain;
	struct uvc_streaming *stream;
	enum uvc_handle_state state;
};

struct uvc_control{
	struct uvc_entity *entity;
	struct uvc_control_info info;
	
	u8 index;					/* Used to math the uvc_control entry with a uvc_control_info*/

	u8 dirty:1,
	   loaded:1,
	   modified:1,
	   cached:1,
	   initialized:1;
	
	u8 *uvc_data;
	
	//struct uvc_fh *handle;
};

#define UVC_ENTITY_FLAG_DEFAULT		(1 << 0)

struct uvc_entity{
	struct list_head list;		/* Entity as part of a UVC device. */
	struct list_head chain;		/* Entity as part of a video device */

	unsigned int flags;
	
	u8 id;
	u16 type;
	char name[64];
	
	/*Media controller-related fields*/
	struct video_device *vdev;
	unsigned int num_pads;
	unsigned int num_links;
	struct media_pad *pads;

	union{
		struct{
			u16 wObjectiveFocalLengthMin;
			u16 wObjectiveFocalLengthMax;
			u16 wOcularFocalLength;
			u8 bControlSize;
			u8 *bmControls;
		}camera;
		
		struct{
			u16 wMaxMultiplier;
			u8 bControlSize;
			u8 *bmControls;
			u8 bmVideoStandards;
		}processing;
		
		struct{
			u8 guidExtensionCode[16];
			u8 bNumControls;
			u8 bControlSize;
			u8 *bmControls;
			u8 *bmControlsType;
		}extension;
	};
	
	
	u8 bNrInPins;
	u8 *baSourceID;

	unsigned int ncontrols;
	struct uvc_control *controls;
};
int uvc_ctrl_init_device(struct uvc_device *dev);
void uvc_ctrl_cleanup_device(struct uvc_device *dev);


/*******************************************************************************/
struct usb_host_endpoint *uvc_find_endpoint(struct usb_host_interface *alts, u8 epaddr);


int uvc_probe_video(struct uvc_streaming *stream, struct uvc_streaming_control *probe);

int uvc_query_ctrl(struct uvc_device *dev, u8 query, u8 unit, u8 intfnum, u8 cs, void *data, u16 size);
int uvc_video_init(struct uvc_streaming *stream);

int uvc_video_start_streaming(struct uvc_streaming *stream);
void uvc_video_stop_streaming(struct uvc_streaming *stream);


/*******************************************************************************/


void uvc_debugfs_init(void);
void uvc_debugfs_clean(void);


#endif	/* __UVC_CAMERA_H__ */