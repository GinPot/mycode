#include "uvc_camera.h"






/************************ V4L2 ioctl *******************************/
static int uvc_ioctl_querycap(struct file *file, void *fh, struct v4l2_capability *cap)
{
	struct video_device *vdev = video_devdata(file);
	struct uvc_fh *handle = file->private_data;
	struct uvc_video_chain *chain = handle->chain;
	struct uvc_streaming *stream = handle->stream;
	
	strscpy(cap->driver, "uvcvideo", sizeof(cap->driver));
	strscpy(cap->card, vdev->name, sizeof(cap->card));
	usb_make_path(stream->dev->udev, cap->bus_info, sizeof(cap->bus_info));
	cap->capabilities = V4L2_CAP_DEVICE_CAPS | V4L2_CAP_STREAMING | chain->caps;
	
	print_uvc(PRINT_DEBUG, "%s: %s", cap->driver, cap->card);
	
	return 0;
}

static int uvc_ioctl_enum_fmt_vid_cap(struct file *file, void *fh, struct v4l2_fmtdesc *fmt)
{
	struct uvc_fh *handle = fh;
	struct uvc_streaming *stream = handle->stream;
	
	struct uvc_format *format;
	enum v4l2_buf_type type = fmt->type;
	u32 index = fmt->index;
	
	if(fmt->type != stream->type || fmt->index >= stream->nformats)
		return -EINVAL;
	
	memset(fmt, 0, sizeof(*fmt));
	fmt->index = index;
	fmt->type = type;
	
	format = &stream->format[fmt->index];
	fmt->flags = 0;
	if(format->flags & UVC_FMT_FLAG_COMPRESSED)
		fmt->flags |= V4L2_FMT_FLAG_COMPRESSED;
	strscpy(fmt->description, format->name, sizeof(fmt->description));
	fmt->description[sizeof(fmt->description) - 1] = 0;
	fmt->pixelformat = format->fcc;

	print_uvc(PRINT_DEBUG, "%c%c%c%c -- %s", fmt->pixelformat & 0xff, (fmt->pixelformat >> 8) & 0xff,
									(fmt->pixelformat >> 16) & 0xff, (fmt->pixelformat >> 24) & 0xff,
									fmt->description);
	
	return 0;
}

static int uvc_ioctl_enum_framesizes(struct file *file, void *fh, struct v4l2_frmsizeenum *fsize)
{
	struct uvc_fh *handle = fh;
	struct uvc_streaming *stream = handle->stream;
	struct uvc_format *format = NULL;
	struct uvc_frame *frame = NULL;
	unsigned int index;
	unsigned int i;
	
	/* Look for the given pixel format */
	for(i = 0; i < stream->nformats; i++){
		if(stream->format[i].fcc == fsize->pixel_format){
			format = &stream->format[i];
			break;
		}
	}
	if(format == NULL)
		return -EINVAL;
	
	/* Skip duplicate frame sizes */
	for(i = 0, index = 0; i < format->nframes; i++){
		if(frame && frame->wWidth == format->frame[i].wWidth && frame->wHeight == format->frame[i].wHeight)
			continue;
		frame = &format->frame[i];
		if(index == fsize->index)
			break;
		index++;
	}
	if(i == format->nframes)
		return -EINVAL;
	
	fsize->type = V4L2_FRMSIZE_TYPE_DISCRETE;
	fsize->discrete.width = frame->wWidth;
	fsize->discrete.height = frame->wHeight;
	
	print_uvc(PRINT_DEBUG, "%d: %dx%d",fsize->type, fsize->discrete.width, fsize->discrete.height);
	
	return 0;
}

static u32 uvc_try_frame_interval(struct uvc_frame *frame, u32 interval)
{
	unsigned int i;
	
	if(frame->bFrameIntervalType){
		u32 best = -1, dist;
		
		for(i = 0; i < frame->bFrameIntervalType; ++i){
			dist = interval > frame->dwFrameInterval[i] ? interval - frame->dwFrameInterval[i] : 
														  frame->dwFrameInterval[i] - interval;
			print_uvc(PRINT_DEBUG, "%d: %d",interval, frame->dwFrameInterval[i]);
			if(dist > best)
				break;
			
			best = dist;
		}
		
		interval = frame->dwFrameInterval[i-1];
	}
	print_uvc(PRINT_DEBUG, "return %d",interval);
	return interval;
}

static u32 uvc_v4l2_get_bytesperline(const struct uvc_format *format, const struct uvc_frame *frame)
{
	switch(format->fcc){
		case V4L2_PIX_FMT_NV12:
		case V4L2_PIX_FMT_YVU420:
		case V4L2_PIX_FMT_YUV420:
		case V4L2_PIX_FMT_M420:
			return frame->wWidth;
		default:
			return format->bpp * frame->wWidth / 8;
	}

}

static int uvc_v4l2_try_format(struct uvc_streaming *stream, struct v4l2_format *fmt, struct uvc_streaming_control *probe,
								struct uvc_format **uvc_format, struct uvc_frame **uvc_frame)
{
	struct uvc_format *format = NULL;
	struct uvc_frame *frame = NULL;
	u16 rw, rh;
	unsigned int d, maxd;
	unsigned int i;
	u32 interval;
	int ret = 0;
	u8 *fcc;
	
	
	fcc = (u8 *)&fmt->fmt.pix.pixelformat;
	print_uvc(PRINT_DEBUG, "Trying format 0x%08x (%c%c%c%c): %ux%u.", fmt->fmt.pix.pixelformat,
							fcc[0], fcc[1], fcc[2], fcc[3],
							fmt->fmt.pix.width, fmt->fmt.pix.height);
							
	/* Check if the hardware supports the requested format, use the default format otherwise. */
	for(i = 0; i < stream->nformats; ++i){
		format = &stream->format[i];
		if(format->fcc == fmt->fmt.pix.pixelformat)
			break;
	}
	if(i == stream->nformats){
		format = stream->def_format;
		fmt->fmt.pix.pixelformat = format->fcc;
	}
	
	/* Find the closest image size. The distance between image sizes is
	 * the size in pixels of the non-overlapping region between the
	 * requested size and the frame-specified size*/
	rw = fmt->fmt.pix.width;
	rh = fmt->fmt.pix.height;
	maxd = (unsigned int)-1;
	
	for(i = 0; i < format->nframes; ++i){
		u16 w = format->frame[i].wWidth;
		u16 h = format->frame[i].wHeight;
		
		d = min(w, rw) * min(h, rh);
		d = w*h + rw*rh - 2*d;
		if(d < maxd){
			maxd  = d;
			frame = &format->frame[i];
		}
		if(maxd == 0)
			break;
	}
	if(frame == NULL){
		print_uvc(PRINT_ERROR, "Unsupported size %ux%u.", fmt->fmt.pix.width, fmt->fmt.pix.height);
		return -EINVAL;
	}
	
	/* Use the default frame interval. */
	interval = frame->dwDefaultFrameInterval;
	print_uvc(PRINT_DEBUG, "Using default frame interval %u.%u us (%u.%u fps).", interval/10, interval%10,
								10000000/interval, (100000000/interval)%10);
	
	/* Set the format index, frame index and frame interval. */
	memset(probe, 0, sizeof(*probe));
	probe->bmHint = 1;
	probe->bFormatIndex = format->index;
	probe->bFrameIndex = frame->bFrameIndex;
	probe->dwFrameInterval = uvc_try_frame_interval(frame, interval);
	
	mutex_lock(&stream->mutex);
	/* Probe the device. */
	ret = uvc_probe_video(stream, probe);
	mutex_unlock(&stream->mutex);
	if(ret < 0)
		goto done;
	
	fmt->fmt.pix.width = frame->wWidth;
	fmt->fmt.pix.height = frame->wHeight;
	fmt->fmt.pix.field = V4L2_FIELD_NONE;
	fmt->fmt.pix.bytesperline = uvc_v4l2_get_bytesperline(format, frame);	//每行占用的字节数
	fmt->fmt.pix.sizeimage = probe->dwMaxVideoFrameSize;
	fmt->fmt.pix.colorspace = format->colorspace;
	fmt->fmt.pix.priv = 0;
	
	if(uvc_format != NULL)
		*uvc_format = format;
	if(uvc_frame != NULL)
		*uvc_frame = frame;
	
done:
	return ret;
}								

/*
 * Only a single instance can be in a privileged state at a give time.
 * Trying to perform an operation that requires privileges will
 * automatically acquire the required privileges if possible, or return -EBUSY
 * otherwise. Privileges are dismissed when closing the instance or when
 * freeing the video buffers using VIDIOC_REQBUFS
 *
 * Operations that require privileges are:
 * - VIDIOC_S_INPUT
 * - VIDIOC_S_PARM
 * - VIDIOC_S_FMT
 * - VIDIOC_REQBUFS
 */
static int uvc_acquire_privileges(struct uvc_fh *handle)
{
	/* Always succeed if the handle is already privileged */
	if(handle->state == UVC_HANDLE_ACTIVE)
		return 0;
	
	if(atomic_inc_return(&handle->stream->active) != 1){
		atomic_dec(&handle->stream->active);
		return -EBUSY;
	}
	handle->state = UVC_HANDLE_ACTIVE;
	
	return 0;
}

static void uvc_dismiss_privileges(struct uvc_fh *handle)
{
	if(handle->state == UVC_HANDLE_ACTIVE)
		atomic_dec(&handle->stream->active);
	
	handle->state = UVC_HANDLE_PASSIVE;
}

static int uvc_ioctl_s_fmt_vid_cap(struct file *file, void *fh, struct v4l2_format *fmt)
{
	struct uvc_fh *handle = fh;
	struct uvc_streaming *stream = handle->stream;
	
	struct uvc_streaming_control probe;
	struct uvc_format *format;
	struct uvc_frame *frame;
	int ret;

	ret = uvc_acquire_privileges(handle);
	if(ret < 0)
		return ret;
	
	
	if(fmt->type != stream->type)
		return -EINVAL;
	
	ret = uvc_v4l2_try_format(stream, fmt, &probe, &format, &frame);
	if(ret < 0)
		return ret;
	
	mutex_lock(&stream->mutex);
	
	if(uvc_queue_allocated(&stream->queue)){
		ret = -EBUSY;
		goto done;
	}
	
	stream->ctrl = probe;
	stream->cur_format = format;
	stream->cur_frame = frame;

done:
	mutex_unlock(&stream->mutex);
	return ret;
}

static int uvc_v4l2_get_format(struct uvc_streaming *stream, struct v4l2_format *fmt)
{
	struct uvc_format *format;
	struct uvc_frame *frame;
	int ret = 0;
	
	if(fmt->type != stream->type)
		return -EINVAL;
	
	mutex_lock(&stream->mutex);
	format = stream->cur_format;
	frame = stream->cur_frame;
	
	if(format == NULL || frame == NULL){
		ret = -EINVAL;
		goto done;
	}
	
	fmt->fmt.pix.pixelformat	= format->fcc;
	fmt->fmt.pix.width			= frame->wWidth;
	fmt->fmt.pix.height			= frame->wHeight;
	fmt->fmt.pix.field			= V4L2_FIELD_NONE;
	fmt->fmt.pix.bytesperline	= uvc_v4l2_get_bytesperline(format, frame);
	fmt->fmt.pix.sizeimage		= stream->ctrl.dwMaxVideoFrameSize;
	fmt->fmt.pix.colorspace		= format->colorspace;
	fmt->fmt.pix.priv			= 0;
	
done:
	mutex_unlock(&stream->mutex);
	return ret;
}

static int uvc_ioctl_g_fmt_vid_cap(struct file *file, void *fh, struct v4l2_format *fmt)
{
	struct uvc_fh *handle = fh;
	struct uvc_streaming *stream = handle->stream;
	
	return uvc_v4l2_get_format(stream, fmt);
}

static u32 uvc_fraction_to_interval(u32 numerator, u32 denominator)
{
	u32 multiplier;
	
	/* Saturate the result if the operation would overflow. */
	if(denominator == 0 || numerator/denominator > ((u32)-1)/10000000)
		return (u32)-1;
	
	multiplier = 10000000;
	while(numerator > ((u32)-1)/multiplier){
		multiplier /= 2;
		denominator /= 2;
	}
	
	return denominator ? numerator * multiplier / denominator : 0;
}

static int uvc_ioctl_s_parm(struct file *file, void *fh, struct v4l2_streamparm *parm)
{
	struct uvc_fh *handle = fh;
	struct uvc_streaming *stream = handle->stream;
	struct uvc_streaming_control probe;
	struct v4l2_fract timeperframe;
	struct uvc_format *format;
	struct uvc_frame *frame;
	u32 interval, maxd;
	//unsigned int i;
	int ret;
	
	ret = uvc_acquire_privileges(handle);
	if(ret < 0)
		return ret;
	
	if(parm->type != stream->type)
		return -EINVAL;
	
	timeperframe = parm->parm.capture.timeperframe;
	
	interval = uvc_fraction_to_interval(timeperframe.numerator, timeperframe.denominator);
	print_uvc(PRINT_DEBUG, "Setting frame interval to %u/%u (%u).", timeperframe.numerator, timeperframe.denominator, interval);
	
	mutex_lock(&stream->mutex);
	
	if(uvc_queue_streaming(&stream->queue)){
		mutex_unlock(&stream->mutex);
		return -EBUSY;
	}
	
	format = stream->cur_format;
	frame = stream->cur_frame;
	probe = stream->ctrl;
	probe.dwFrameInterval = uvc_try_frame_interval(frame, interval);
	maxd = abs((s32)probe.dwFrameInterval - interval);
	
	//...
	
	/* Probe the device with the new setting */
	ret = uvc_probe_video(stream, &probe);
	if(ret < 0){
		mutex_unlock(&stream->mutex);
		return ret;
	}

	stream->ctrl = probe;
	mutex_unlock(&stream->mutex);

	return 0;
}

static int uvc_ioctl_g_parm(struct file *file, void *fh, struct v4l2_streamparm *parm)
{
	struct uvc_fh *handle = fh;
	struct uvc_streaming *stream = handle->stream;
	u32 denominator;
	
	if(parm->type != stream->type)
		return -EINVAL;
	
	mutex_lock(&stream->mutex);
	denominator = stream->ctrl.dwFrameInterval;
	mutex_unlock(&stream->mutex);

	
	memset(parm, 0, sizeof(*parm));
	parm->type = stream->type;
	
	parm->parm.capture.capability = V4L2_CAP_TIMEPERFRAME;
	parm->parm.capture.capturemode = 0;
	parm->parm.capture.timeperframe.numerator = 1;
	parm->parm.capture.timeperframe.denominator = 10000000 / denominator;
	parm->parm.capture.extendedmode = 0;
	parm->parm.capture.readbuffers = 0;
	
	return 0;
}

static int uvc_ioctl_reqbufs(struct file *file, void *fh, struct v4l2_requestbuffers *rb)
{
	struct uvc_fh *handle = fh;
	struct uvc_streaming *stream = handle->stream;
	int ret;
	
	ret = uvc_acquire_privileges(handle);
	if(ret < 0)
		return ret;
	
	mutex_lock(&stream->mutex);
	ret = uvc_request_buffers(&stream->queue, rb);
	mutex_unlock(&stream->mutex);
	if(ret < 0)
		return ret;
	
	if(ret == 0)
		uvc_dismiss_privileges(handle);
	
	return 0;
}

static int uvc_ioctl_querybuf(struct file *file, void *fh, struct v4l2_buffer *buf)
{
	struct uvc_fh *handle = fh;
	struct uvc_streaming *stream = handle->stream;
	
	if(!(handle->state == UVC_HANDLE_ACTIVE))
		return -EBUSY;
	
	return uvc_query_buffer(&stream->queue, buf);
}

static int uvc_ioctl_qbuf(struct file *file, void *fh, struct v4l2_buffer *buf)
{
	struct uvc_fh *handle = fh;
	struct uvc_streaming *stream = handle->stream;
	
	if(!(handle->state == UVC_HANDLE_ACTIVE))
		return -EBUSY;
	
	return uvc_queue_buffer(&stream->queue, stream->vdev.v4l2_dev->mdev, buf);
}

static int uvc_ioctl_streamon(struct file *file, void *fh, enum v4l2_buf_type type)
{
	struct uvc_fh *handle = fh;
	struct uvc_streaming *stream = handle->stream;
	int ret;
	
	if(!(handle->state == UVC_HANDLE_ACTIVE))
		return -EBUSY;
	
	mutex_lock(&stream->mutex);
	ret = uvc_queue_streamon(&stream->queue, type);
	mutex_unlock(&stream->mutex);
	
	return ret;
}

static int uvc_ioctl_dqbuf(struct file *file, void *fh, struct v4l2_buffer *buf)
{
	struct uvc_fh *handle = fh;
	struct uvc_streaming *stream = handle->stream;
	
	if(!(handle->state == UVC_HANDLE_ACTIVE))
		return -EBUSY;
	
	return uvc_dequeue_buffer(&stream->queue, buf, file->f_flags & O_NONBLOCK);
	
}

/************************ V4L2 file operations *******************************/
static int uvc_v4l2_open(struct file *file)
{
	int ret = 0;
	struct uvc_fh *handle;
	struct uvc_streaming *stream;
	
	stream = video_drvdata(file);
	
	print_uvc(PRINT_DEBUG, "stream->dev->users=%d.", stream->dev->users);
	
	ret = usb_autopm_get_interface(stream->dev->intf);
	if(ret < 0)
		return ret;
	
	/* Create the device handle. */
	handle = kzalloc(sizeof(*handle), GFP_KERNEL);
	if(handle == NULL){
		usb_autopm_put_interface(stream->dev->intf);
		return -ENOMEM;
	}
	
	mutex_lock(&stream->dev->lock);
	if(stream->dev->users == 0){
		ret = uvc_status_start(stream->dev, GFP_KERNEL);
		if(ret < 0){
			mutex_unlock(&stream->dev->lock);
			usb_autopm_put_interface(stream->dev->intf);
			kfree(handle);
			return ret;
		}
	}
	
	stream->dev->users++;
	mutex_unlock(&stream->dev->lock);
	
	v4l2_fh_init(&handle->vfh, &stream->vdev);
	v4l2_fh_add(&handle->vfh);
	handle->chain = stream->chain;
	handle->stream = stream;
	handle->state = UVC_HANDLE_PASSIVE;
	file->private_data = handle;
	
	return 0;
}

void uvc_queue_release(struct uvc_video_queue *queue)
{
	mutex_lock(&queue->mutex);
	vb2_queue_release(&queue->queue);
	mutex_unlock(&queue->mutex);
}

static int uvc_v4l2_release(struct file *file)
{
	struct uvc_fh *handle = file->private_data;
	struct uvc_streaming *stream = handle->stream;
	
	print_uvc(PRINT_DEBUG, ".");
	if(handle->state == UVC_HANDLE_ACTIVE)
		uvc_queue_release(&stream->queue);
	
	/* Release the file handle */
	if(handle->state == UVC_HANDLE_ACTIVE)
		atomic_dec(&handle->stream->active);
	handle->state = UVC_HANDLE_PASSIVE;
	
	v4l2_fh_del(&handle->vfh);
	v4l2_fh_exit(&handle->vfh);
	kfree(handle);
	file->private_data = NULL;
	
	mutex_lock(&stream->dev->lock);
	if(--stream->dev->users == 0)
		uvc_status_stop(stream->dev);
	mutex_unlock(&stream->dev->lock);
	
	usb_autopm_put_interface(stream->dev->intf);
	
	return 0;
}

static ssize_t uvc_v4l2_read(struct file *file, char __user *data,
							size_t count, loff_t *ppos)
{
	print_uvc(PRINT_DEBUG, ": not implemented.");
	return -EINVAL;
}

static int uvc_v4l2_mmap(struct file *file, struct vm_area_struct *vma)
{
	struct uvc_fh *handle = file->private_data;
	struct uvc_streaming *stream = handle->stream;
	
	//print_uvc(PRINT_DEBUG, ".");
	
	return vb2_mmap(&stream->queue.queue, vma);
}

static __poll_t uvc_v4l2_poll(struct file *file, poll_table *wait)
{
	__poll_t ret;
	
	struct uvc_fh *handle = file->private_data;
	struct uvc_streaming *stream = handle->stream;
	
	print_uvc(PRINT_DEBUG, ".");
	
	mutex_lock(&stream->queue.mutex);
	ret = vb2_poll(&stream->queue.queue, file, wait);
	mutex_unlock(&stream->queue.mutex);
	
	return ret;
}


const struct v4l2_ioctl_ops uvc_ioctl_ops = {
	.vidioc_querycap			= uvc_ioctl_querycap,
	.vidioc_enum_fmt_vid_cap	= uvc_ioctl_enum_fmt_vid_cap,
	.vidioc_enum_framesizes		= uvc_ioctl_enum_framesizes,
	.vidioc_s_fmt_vid_cap		= uvc_ioctl_s_fmt_vid_cap,
	.vidioc_g_fmt_vid_cap		= uvc_ioctl_g_fmt_vid_cap,
	.vidioc_s_parm				= uvc_ioctl_s_parm,
	.vidioc_g_parm				= uvc_ioctl_g_parm,
	.vidioc_reqbufs				= uvc_ioctl_reqbufs,
	.vidioc_querybuf			= uvc_ioctl_querybuf,
	.vidioc_qbuf				= uvc_ioctl_qbuf,
	.vidioc_streamon			= uvc_ioctl_streamon,
	.vidioc_dqbuf				= uvc_ioctl_dqbuf,
};


const struct v4l2_file_operations uvc_fops = {
	.owner 			= THIS_MODULE,
	.open			= uvc_v4l2_open,
	.release 		= uvc_v4l2_release,
	
	.unlocked_ioctl	= video_ioctl2,
	.read			= uvc_v4l2_read,
	.mmap			= uvc_v4l2_mmap,
	.poll			= uvc_v4l2_poll,
};













