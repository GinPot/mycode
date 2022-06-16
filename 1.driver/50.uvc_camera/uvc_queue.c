#include "uvc_camera.h"





//V4L2 queue operations
//VIDIOC_REQBUFS
int uvc_request_buffers(struct uvc_video_queue *queue, struct v4l2_requestbuffers *rb)
{
	int ret;
	
	mutex_lock(&queue->mutex);
	ret = vb2_reqbufs(&queue->queue, rb);
	mutex_unlock(&queue->mutex);
	
	return ret ? ret : rb->count;
}
//VIDIOC_QUERYBUF
int uvc_query_buffer(struct uvc_video_queue *queue, struct v4l2_buffer *buf)
{
	int ret;
	
	mutex_lock(&queue->mutex);
	ret = vb2_querybuf(&queue->queue, buf);
	mutex_unlock(&queue->mutex);
	
	return ret;
}
//VIDIOC_QBUF
int uvc_queue_buffer(struct uvc_video_queue *queue, struct media_device *mdev, struct v4l2_buffer *buf)
{
	int ret;
	
	mutex_lock(&queue->mutex);
	ret = vb2_qbuf(&queue->queue, mdev, buf);
	mutex_unlock(&queue->mutex);
	
	return ret;
}
//VIDIOC_STREAMON
int uvc_queue_streamon(struct uvc_video_queue *queue, enum v4l2_buf_type type)
{
	int ret;
	
	mutex_lock(&queue->mutex);
	ret = vb2_streamon(&queue->queue, type);
	mutex_unlock(&queue->mutex);
	
	return ret;
}

int uvc_dequeue_buffer(struct uvc_video_queue *queue, struct v4l2_buffer *buf, int nonblocking)
{
	int ret;
	
	mutex_lock(&queue->mutex);
	ret = vb2_dqbuf(&queue->queue, buf, nonblocking);
	mutex_unlock(&queue->mutex);
	
	return ret;
}

//check if buffers have been allocated.
int uvc_queue_allocated(struct uvc_video_queue *queue)
{
	int allocated;
	
	mutex_lock(&queue->mutex);
	allocated = vb2_is_busy(&queue->queue);
	mutex_unlock(&queue->mutex);
	
	return allocated;
}

static void uvc_buffer_queue(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct uvc_video_queue *queue = vb2_get_drv_priv(vb->vb2_queue);
	struct uvc_buffer *buf = container_of(vbuf, struct uvc_buffer, buf);
	unsigned long flags;
	
	spin_lock_irqsave(&queue->irqlock, flags);
	if(likely(!(queue->flags & UVC_QUEUE_DISCONNECTED))){
		kref_init(&buf->ref);
		list_add_tail(&buf->queue, &queue->irqqueue);
	}else{
		buf->state = UVC_BUF_STATE_ERROR;
		vb2_buffer_done(vb, VB2_BUF_STATE_ERROR);
	}
	spin_unlock_irqrestore(&queue->irqlock, flags);
}

static void uvc_queue_buffer_requeue(struct uvc_video_queue *queue, struct uvc_buffer *buf)
{
	buf->error = 0;
	buf->state = UVC_BUF_STATE_QUEUED;
	buf->bytesused = 0;
	
	vb2_set_plane_payload(&buf->buf.vb2_buf, 0, 0);
	uvc_buffer_queue(&buf->buf.vb2_buf);
}

static void uvc_queue_buffer_complete(struct kref *ref)
{
	struct uvc_buffer *buf = container_of(ref, struct uvc_buffer, ref);
	struct vb2_buffer *vb = &buf->buf.vb2_buf;
	struct uvc_video_queue *queue = vb2_get_drv_priv(vb->vb2_queue);
	
	if((queue->flags & UVC_QUEUE_DROP_CORRUPTED) && buf->error){
		print_uvc(PRINT_ERROR, "error.");
		uvc_queue_buffer_requeue(queue, buf);
		return;
	}
	
	buf->state = buf->error ? UVC_BUF_STATE_ERROR : UVC_BUF_STATE_DONE;
	vb2_set_plane_payload(&buf->buf.vb2_buf, 0, buf->bytesused);
	vb2_buffer_done(&buf->buf.vb2_buf, VB2_BUF_STATE_DONE);
}

void uvc_queue_buffer_release(struct uvc_buffer *buf)
{
	kref_put(&buf->ref, uvc_queue_buffer_complete);
}


static inline struct uvc_streaming *uvc_queue_to_stream(struct uvc_video_queue *queue)
{
	return container_of(queue, struct uvc_streaming, queue);
}

/* videobuf2 queue operations */
static int uvc_queue_setup(struct vb2_queue *vq,
							unsigned int *nbuffers, unsigned int *nplanes,
							unsigned int sizes[], struct device *alloc_devs[])
{
	struct uvc_video_queue *queue = vb2_get_drv_priv(vq);
	struct uvc_streaming *stream;
	unsigned int size;
	
	switch(vq->type){
		case V4L2_BUF_TYPE_META_CAPTURE:
			size = UVC_METATADA_BUF_SIZE;
			break;
		default:
			stream = uvc_queue_to_stream(queue);
			size = stream->ctrl.dwMaxVideoFrameSize;
		break;
	}
	
	if(*nplanes)
		return *nplanes != 1 || sizes[0] < size ? -EINVAL : 0;
	
	*nplanes = 1;
	sizes[0] = size;
	return 0;
}

/* Return all queued buffers to videobuf2 in the requested state.
 * This function must be called with the queue spinlock held.
 */
static void uvc_queue_return_buffers(struct uvc_video_queue *queue, enum uvc_buffer_state state)
{
	enum vb2_buffer_state vb2_state = state == UVC_BUF_STATE_ERROR ?
												VB2_BUF_STATE_ERROR : VB2_BUF_STATE_QUEUED;
	
	while(!list_empty(&queue->irqqueue)){
		struct uvc_buffer *buf = list_first_entry(&queue->irqqueue, struct uvc_buffer, queue);
		
		list_del(&buf->queue);
		buf->state = state;
		vb2_buffer_done(&buf->buf.vb2_buf, vb2_state);
	}
}

static int uvc_start_streaming(struct vb2_queue *vq, unsigned int count)
{
	struct uvc_video_queue *queue = vb2_get_drv_priv(vq);
	struct uvc_streaming *stream = uvc_queue_to_stream(queue);
	int ret;
	
	lockdep_assert_irqs_enabled();
	
	queue->buf_used = 0;
	
	ret = uvc_video_start_streaming(stream);
	if(ret == 0)
		return 0;

	spin_lock_irq(&queue->irqlock);
	uvc_queue_return_buffers(queue, UVC_BUF_STATE_QUEUED);
	spin_unlock_irq(&queue->irqlock);
	
	return ret;
}

static u16 uvc_video_clock_host_sof(const struct uvc_clock_sample *sample)
{
	s8 delta_sof;
	
	delta_sof = (sample->host_sof - sample->dev_sof) & 255;
	
	return (sample->dev_sof + delta_sof) % 2047;
}

unsigned int uvc_hw_timestamps_param;
void uvc_video_clock_update(struct uvc_streaming *stream, struct vb2_v4l2_buffer *vbuf, struct uvc_buffer *buf)
{
	struct uvc_clock *clock = &stream->clock;
	struct uvc_clock_sample *first;
	struct uvc_clock_sample *last;
	unsigned long flags;
	u64 timestamp;
	u32 delta_stc;
	u32 y1, y2;
	u32 x1, x2;
	u32 mean;
	u32 sof;
	u64 y;
	
	if(uvc_hw_timestamps_param)
		return;
	
	if(!clock->samples)
		return;
	
	spin_lock_irqsave(&clock->lock, flags);
	
	if(clock->count < clock->size)
		goto done;
	
	first = &clock->samples[clock->head];
	last = &clock->samples[(clock->head - 1) % clock->size];
	
	/* First step, PTS to SOF conversion. */
	delta_stc = buf->pts - (1UL << 31);
	x1 = first->dev_stc - delta_stc;
	x2 = last->dev_stc - delta_stc;
	if(x1 == x2)
		goto done;
	
	y1 = (first->dev_sof + 2048) << 16;
	y2 = (last->dev_sof + 2048) << 16;
	if(y2 < y1)
		y2 += 2048 << 16;
	
	y = (u64)(y2 - y1) * (1UL << 31) + (u64)y1 * (u64)x2 - (u64)y2 * (u64)x1;
	y = div_u64(y, x2 - x1);
	
	sof = y;
	
//	print_uvc(PRINT_DEBUG, "%s: PTS %u y %llu.%06llu SOF %u.%06llu "
//		  "(x1 %u x2 %u y1 %u y2 %u SOF offset %u)\n",
//		  stream->dev->name, buf->pts,
//		  y >> 16, div_u64((y & 0xffff) * 1000000, 65536),
//		  sof >> 16, div_u64(((u64)sof & 0xffff) * 1000000LLU, 65536),
//		  x1, x2, y1, y2, clock->sof_offset);
	
	/* Second step, SOF to host clock conversion. */
	x1 = (uvc_video_clock_host_sof(first) + 2048) << 16;
	x2 = (uvc_video_clock_host_sof(last) + 2048) << 16;
	if(x2 < x1)
		x2 += 2048 << 16;
	if(x1 == x2)
		goto done;
	
	y1 = NSEC_PER_SEC;
	y2 = (u32)ktime_to_ns(ktime_sub(last->host_time, first->host_time)) + y1;
	
	mean = (x1 + x2) / 2;
	if(mean - (1024 << 16) > sof)
		sof += 2048 << 16;
	else if(sof > mean + (1024 << 16))
		sof -= 2048 << 16;
	
	y = (u64)(y2 - y1) * (u64)sof + (u64)y1 * (u64)x2 - (u64)y2 * (u64)x1;
	y = div_u64(y, x2 - x1);
	
	timestamp = ktime_to_ns(first->host_time) + y - y1;
	
//	print_uvc(PRINT_DEBUG, "%s: SOF %u.%06llu y %llu ts %llu "
//		  "buf ts %llu (x1 %u/%u/%u x2 %u/%u/%u y1 %u y2 %u)\n",
//		  stream->dev->name,
//		  sof >> 16, div_u64(((u64)sof & 0xffff) * 1000000LLU, 65536),
//		  y, timestamp, vbuf->vb2_buf.timestamp,
//		  x1, first->host_sof, first->dev_sof,
//		  x2, last->host_sof, last->dev_sof, y1, y2);
	
	/* Update the v4l2 buffer. */
	vbuf->vb2_buf.timestamp = timestamp;
	
done:
	spin_unlock_irqrestore(&clock->lock, flags);
}

static void uvc_buffer_finish(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = container_of(vb, struct vb2_v4l2_buffer, vb2_buf);
	struct uvc_video_queue *queue = vb2_get_drv_priv(vb->vb2_queue);
	struct uvc_streaming *stream = uvc_queue_to_stream(queue);
	struct uvc_buffer *buf = container_of(vbuf, struct uvc_buffer, buf);

	if(vb->state == VB2_BUF_STATE_DONE)
		uvc_video_clock_update(stream, vbuf, buf);
}

static int uvc_buffer_prepare(struct vb2_buffer *vb)
{
	struct vb2_v4l2_buffer *vbuf = container_of(vb, struct vb2_v4l2_buffer, vb2_buf);
	struct uvc_video_queue *queue = vb2_get_drv_priv(vb->vb2_queue);
	struct uvc_buffer *buf = container_of(vbuf, struct uvc_buffer, buf);
	
	//print_uvc(PRINT_DEBUG, ".");
	//dump_stack();
	if(unlikely(queue->flags & UVC_QUEUE_DISCONNECTED))
		return -ENODEV;
	
	buf->state = UVC_BUF_STATE_QUEUED;
	buf->error = 0;
	buf->mem = vb2_plane_vaddr(vb, 0);
	buf->length = vb2_plane_size(vb, 0);
	buf->bytesused = vb2_get_plane_payload(vb, 0);
	
	return 0;
}



static void uvc_stop_streaming(struct vb2_queue *vq)
{
	struct uvc_video_queue *queue = vb2_get_drv_priv(vq);
	
	uvc_video_stop_streaming(container_of(queue, struct uvc_streaming, queue));
	
	spin_lock_irq(&queue->irqlock);
	uvc_queue_return_buffers(queue, UVC_BUF_STATE_ERROR);
	spin_unlock_irq(&queue->irqlock);
}

/* videobuf2 for v4l2_ioctl */
static const struct vb2_ops uvc_queue_qops = {
	.queue_setup	 	= uvc_queue_setup,			//VIDIOC_REQBUFS
	
	.buf_prepare		= uvc_buffer_prepare,		//VIDIOC_QBUF
	
	.buf_queue			= uvc_buffer_queue,
	.start_streaming 	= uvc_start_streaming,		//VIDIOC_STREAMON
	
	.wait_prepare		= vb2_ops_wait_prepare,		//VIDIOC_DQBUF
	.wait_finish		= vb2_ops_wait_finish,
	.buf_finish			= uvc_buffer_finish,
	
	.stop_streaming		= uvc_stop_streaming,
};

int uvc_queue_init(struct uvc_video_queue *queue, enum v4l2_buf_type type, int drop_corrupted)
{
	int ret;
	
	queue->queue.type				= type;
	queue->queue.io_modes			= VB2_MMAP | VB2_USERPTR;
	queue->queue.drv_priv			= queue;
	queue->queue.buf_struct_size	= sizeof(struct uvc_buffer);
	queue->queue.mem_ops			= &vb2_vmalloc_memops; //&vb2_dma_contig_memops;
	queue->queue.timestamp_flags	= V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC | V4L2_BUF_FLAG_TSTAMP_SRC_SOE;
	queue->queue.lock				= &queue->mutex;
	
	
	switch(type){
		case V4L2_BUF_TYPE_META_CAPTURE:
			//queue->queue.ops = %
			break;
		default:
			queue->queue.io_modes |= VB2_DMABUF;
			queue->queue.ops = &uvc_queue_qops;
		break;
	};
	
	ret = vb2_queue_init(&queue->queue);			//q->buf_ops = &v4l2_buf_ops;
	if(ret)
		return ret;
	
	mutex_init(&queue->mutex);
	spin_lock_init(&queue->irqlock);
	INIT_LIST_HEAD(&queue->irqqueue);
	queue->flags = UVC_QUEUE_DROP_CORRUPTED;
	
	return 0;
}

void uvc_queue_cancel(struct uvc_video_queue *queue, int disconnect)
{
	unsigned long flags;
	
	spin_lock_irqsave(&queue->irqlock, flags);
	
	uvc_queue_return_buffers(queue, UVC_BUF_STATE_ERROR);
	if(disconnect)
		queue->flags |= UVC_QUEUE_DISCONNECTED;
	
	spin_unlock_irqrestore(&queue->irqlock, flags);
}

static struct uvc_buffer *__uvc_queue_get_current_buffer(struct uvc_video_queue *queue)
{
	if(list_empty(&queue->irqqueue))
		return NULL;
	
	return list_first_entry(&queue->irqqueue, struct uvc_buffer, queue);
}

struct uvc_buffer *uvc_queue_get_current_buffer(struct uvc_video_queue *queue)
{
	struct uvc_buffer *nextbuf;
	unsigned long flags;
	
	spin_lock_irqsave(&queue->irqlock, flags);
	nextbuf = __uvc_queue_get_current_buffer(queue);
	spin_unlock_irqrestore(&queue->irqlock, flags);
	
	return nextbuf;
}

struct uvc_buffer *uvc_queue_next_buffer(struct uvc_video_queue *queue, struct uvc_buffer *buf)
{
	struct uvc_buffer *nextbuf;
	unsigned long flags;
	
	spin_lock_irqsave(&queue->irqlock, flags);
	
	list_del(&buf->queue);
	nextbuf = __uvc_queue_get_current_buffer(queue);
	
	spin_unlock_irqrestore(&queue->irqlock, flags);
	
	uvc_queue_buffer_release(buf);
	
	return nextbuf;
	
}












