#include "uvc_camera.h"









/* UVC Controls */
static int __uvc_query_ctrl(struct uvc_device *dev, u8 query, u8 unit, u8 intfnum, u8 cs, void *data, u16 size, int timeout)
{
	u8 type = USB_TYPE_CLASS | USB_RECIP_INTERFACE;
	unsigned int pipe;

	pipe = (query & 0x80) ? usb_rcvctrlpipe(dev->udev, 0) : usb_sndctrlpipe(dev->udev, 0);
	type |= (query & 0x80) ? USB_DIR_IN : USB_DIR_OUT;

	return usb_control_msg(dev->udev, pipe, query, type, cs << 8, unit << 8 | intfnum, data, size, timeout);
}

static const char *uvc_query_name(u8 query)
{
	switch (query) {
	case UVC_SET_CUR:
		return "SET_CUR";
	case UVC_GET_CUR:
		return "GET_CUR";
	case UVC_GET_MIN:
		return "GET_MIN";
	case UVC_GET_MAX:
		return "GET_MAX";
	case UVC_GET_RES:
		return "GET_RES";
	case UVC_GET_LEN:
		return "GET_LEN";
	case UVC_GET_INFO:
		return "GET_INFO";
	case UVC_GET_DEF:
		return "GET_DEF";
	default:
		return "<invalid>";
	}
}

/*
	query:		获取USB控制信息标记
	unit:		控制接口描述符bDescriptorSubtype后面一个字节ID
	intfnum:	控制描述符bInterfaceNumber
	cs:			uvc_ctrls的selector，控制选择器？
*/
int uvc_query_ctrl(struct uvc_device *dev, u8 query, u8 unit, u8 intfnum, u8 cs, void *data, u16 size)
{
	int ret;
	u8 error;
	u8 tmp;

	ret = __uvc_query_ctrl(dev, query, unit, intfnum, cs, data, size, UVC_CTRL_CONTROL_TIMEOUT);
	if(likely(ret == size))
		return 0;

	print_uvc(PRINT_ERROR, "Failed to query (%s) UVC control %u on unit %u: %d (exp. %u).\n",
		   uvc_query_name(query), cs, unit, ret, size);

	if (ret != -EPIPE)
		return ret;

	tmp = *(u8 *)data;

	ret = __uvc_query_ctrl(dev, UVC_GET_CUR, 0, intfnum,
			       UVC_VC_REQUEST_ERROR_CODE_CONTROL, data, 1,
			       UVC_CTRL_CONTROL_TIMEOUT);

	error = *(u8 *)data;
	*(u8 *)data = tmp;

	if (ret != 1)
		return ret < 0 ? ret : -EPIPE;

	print_uvc(PRINT_ERROR, "Control error %u\n", error);

	switch (error) {
	case 0:
		/* Cannot happen - we received a STALL */
		return -EPIPE;
	case 1: /* Not ready */
		return -EBUSY;
	case 2: /* Wrong state */
		return -EILSEQ;
	case 3: /* Power */
		return -EREMOTE;
	case 4: /* Out of range */
		return -ERANGE;
	case 5: /* Invalid unit */
	case 6: /* Invalid control */
	case 7: /* Invalid Request */
	case 8: /* Invalid value within range */
		return -EINVAL;
	default: /* reserved or unknown */
		break;
	}

	return -EPIPE;
}


static size_t uvc_video_ctrl_size(struct uvc_streaming *stream)
{
	/*
	 *return the size of the video probe and commit controls, which depends
	 *on the protocol version.
	 */
	if(stream->dev->uvc_version < 0x0110)
		return 26;
	else if(stream->dev->uvc_version < 0x150)
		return 34;
	else
		return 48;
}

static int uvc_get_video_ctrl(struct uvc_streaming *stream, struct uvc_streaming_control *ctrl, int probe, u8 query)
{
	u16 size = uvc_video_ctrl_size(stream);
	u8 *data;
	int ret;
	
	data = kmalloc(size, GFP_KERNEL);
	if(data == NULL)
		return -ENOMEM;

	ret = __uvc_query_ctrl(stream->dev, query, 0, stream->intfnum, probe ? UVC_VS_PROBE_CONTROL : UVC_VS_COMMIT_CONTROL, 
							data, size, UVC_CTRL_STREAMING_TIMEOUT);
	if((query == UVC_GET_MIN || query == UVC_GET_MAX) && ret == 2){
		print_uvc(PRINT_ERROR, "UVC non compliance - GET_MIN/MAX(PROBE) incorrectly supported. Enabling workaround.");
		memset(ctrl, 0, sizeof(*ctrl));
		ctrl->wCompQuality = le16_to_cpup((__le16 *)data);
		ret = 0;
		goto out;
	}else if(query == UVC_GET_DEF && probe == 1 && ret != size){
		print_uvc(PRINT_ERROR, "UVC non compliance - GET_DEF(PROBE) not supported. Enabling workaround.");
		ret = -EIO;
		goto out;
	}else if(ret != size){
		print_uvc(PRINT_ERROR, "Fail to query (%u) UVC %s control : %d (exp. %u).", query, probe ? "probe" : "commit", ret, size);
		ret = -EIO;
		goto out;
	}
	
	ctrl->bmHint = le16_to_cpup((__le16 *)&data[0]);
	ctrl->bFormatIndex = data[2];
	ctrl->bFrameIndex = data[3];
	ctrl->dwFrameInterval = le32_to_cpup((__le32 *)&data[4]);
	ctrl->wKeyFrameRate = le16_to_cpup((__le16 *)&data[8]);
	ctrl->wPFrameRate = le16_to_cpup((__le16 *)&data[10]);
	ctrl->wCompQuality = le16_to_cpup((__le16 *)&data[12]);
	ctrl->wCompWindowSize = le16_to_cpup((__le16 *)&data[14]);
	ctrl->wDelay = le16_to_cpup((__le16 *)&data[16]);
	ctrl->dwMaxVideoFrameSize = get_unaligned_le32(&data[18]);
	ctrl->dwMaxPayloadTransferSize = get_unaligned_le32(&data[22]);
	
	//print_uvc(PRINT_DEBUG, "ctrl->bmHint=%d.", ctrl->bmHint);
	//print_uvc(PRINT_DEBUG, "ctrl->bFormatIndex=%d.", ctrl->bFormatIndex);
	//print_uvc(PRINT_DEBUG, "ctrl->bFrameIndex=%d.", ctrl->bFrameIndex);
	//print_uvc(PRINT_DEBUG, "ctrl->dwFrameInterval=%d.", ctrl->dwFrameInterval);
	//print_uvc(PRINT_DEBUG, "ctrl->wKeyFrameRate=%d.", ctrl->wKeyFrameRate);
	//print_uvc(PRINT_DEBUG, "ctrl->wPFrameRate=%d.", ctrl->wPFrameRate);
	//print_uvc(PRINT_DEBUG, "ctrl->wCompQuality=%d.", ctrl->wCompQuality);
	//print_uvc(PRINT_DEBUG, "ctrl->wCompWindowSize=%d.", ctrl->wCompWindowSize);
	//print_uvc(PRINT_DEBUG, "ctrl->wDelay=%d.", ctrl->wDelay);
	//print_uvc(PRINT_DEBUG, "ctrl->dwMaxVideoFrameSize=%d.", ctrl->dwMaxVideoFrameSize);
	//print_uvc(PRINT_DEBUG, "ctrl->dwMaxPayloadTransferSize=%d.", ctrl->dwMaxPayloadTransferSize);

	if (size >= 34) {
		ctrl->dwClockFrequency = get_unaligned_le32(&data[26]);
		ctrl->bmFramingInfo = data[30];
		ctrl->bPreferedVersion = data[31];
		ctrl->bMinVersion = data[32];
		ctrl->bMaxVersion = data[33];
	} else {
		ctrl->dwClockFrequency = stream->dev->clock_frequency;
		ctrl->bmFramingInfo = 0;
		ctrl->bPreferedVersion = 0;
		ctrl->bMinVersion = 0;
		ctrl->bMaxVersion = 0;
	}
	
	
	
out:
	kfree(data);
	return ret;
}


static int uvc_set_video_ctrl(struct uvc_streaming *stream, struct uvc_streaming_control *ctrl, int probe)
{
	u16 size = uvc_video_ctrl_size(stream);
	u8 *data;
	int ret;
	
	data = kzalloc(size, GFP_KERNEL);
	if(data == NULL)
		return -ENOMEM;
	
	*(__le16 *)&data[0] = cpu_to_le16(ctrl->bmHint);
	data[2] = ctrl->bFormatIndex;
	data[3] = ctrl->bFrameIndex;
	*(__le32 *)&data[4] = cpu_to_le32(ctrl->dwFrameInterval);
	*(__le16 *)&data[8] = cpu_to_le16(ctrl->wKeyFrameRate);
	*(__le16 *)&data[10] = cpu_to_le16(ctrl->wPFrameRate);
	*(__le16 *)&data[12] = cpu_to_le16(ctrl->wCompQuality);
	*(__le16 *)&data[14] = cpu_to_le16(ctrl->wCompWindowSize);
	*(__le16 *)&data[16] = cpu_to_le16(ctrl->wDelay);
	put_unaligned_le32(ctrl->dwMaxVideoFrameSize, &data[18]);
	put_unaligned_le32(ctrl->dwMaxPayloadTransferSize, &data[22]);
	
	//print_uvc(PRINT_DEBUG, "ctrl->dwFrameInterval=%d.", ctrl->dwFrameInterval);
	//dump_stack();
	if(size >= 34){
		put_unaligned_le32(ctrl->dwClockFrequency, &data[26]);
		data[30] = ctrl->bmFramingInfo;
		data[31] = ctrl->bPreferedVersion;
		data[32] = ctrl->bMinVersion;
		data[33] = ctrl->bMaxVersion;
	}
	
	ret = __uvc_query_ctrl(stream->dev, UVC_SET_CUR, 0, stream->intfnum, 
							probe ? UVC_VS_PROBE_CONTROL : UVC_VS_COMMIT_CONTROL, data, size, UVC_CTRL_STREAMING_TIMEOUT);
	if(ret != size){
		print_uvc(PRINT_ERROR, "Failed to set UVC %s control : %d (exp. %u).", probe ? "probe" : "commit", ret, size);
		ret = -EIO;
	}
	
	kfree(data);
	return ret;
}

int uvc_probe_video(struct uvc_streaming *stream, struct uvc_streaming_control *probe)
{
	struct uvc_streaming_control probe_min, probe_max;
	u16 bandwidth;
	unsigned int i;
	int ret;
	
	ret = uvc_set_video_ctrl(stream, probe, 1);
	if(ret < 0)
		goto done;

	/* Get the minimum and maximum values for compression settings. */
	if(!(stream->dev->quirks & UVC_QUIRK_PROBE_MINMAX)){
		ret = uvc_get_video_ctrl(stream, &probe_min, 1, UVC_GET_MIN);
		if(ret < 0)
			goto done;
		ret = uvc_get_video_ctrl(stream, &probe_max, 1, UVC_GET_MAX);
		if(ret < 0)
			goto done;
		
		probe->wCompQuality = probe_max.wCompQuality;
	}
	
	for(i = 0; i < 2; ++i){
		ret = uvc_set_video_ctrl(stream, probe, 1);
		if(ret < 0)
			goto done;
		ret = uvc_get_video_ctrl(stream, probe, 1, UVC_GET_CUR);
		if(ret < 0)
			goto done;
		
		if(stream->intf->num_altsetting == 1)
			break;
		
		bandwidth = probe->dwMaxPayloadTransferSize;
		if(bandwidth <= stream->maxpsize)
			break;
		
		if(stream->dev->quirks & UVC_QUIRK_PROBE_MINMAX){
			ret = -ENOSPC;
			goto done;
		}
		
		probe->wKeyFrameRate = probe_min.wKeyFrameRate;
		probe->wPFrameRate = probe_min.wPFrameRate;
		probe->wCompQuality = probe_min.wCompQuality;
		probe->wCompWindowSize = probe_min.wCompWindowSize;
	}

done:
	return ret;
}

//static void uvc_video_decode_isoc(struct uvc_urb *uvc_urb, struct uvc_buffer *buf, struct uvc_buffer *meta_buf)
//{
//	struct urb *urb = uvc_urb->urb;
//	
//}

static void dma_uvc_callback(void *param)
{
	struct uvc_streaming *stream = (struct uvc_streaming *)param;
//pr_info("111\n");
	complete(&stream->rx_dma_complete);
}

extern void *__memcpy_aarch64_simd (void *, const void *, size_t);
/*
 * Asynchronous memcpy processing
 * copy URB data to video buffers in process context, releasing buffer
 * references and requeuing the URB when done.
 
 camera_app					47%		30fps
 
 memcpy						28%		30fps
 __memcpy_aarch64_simd		22%		30fps
 dmacpy						11%+8%	30fps
 dmacpy no wait_for			3%		30fps
 
*/
static void uvc_video_copy_data_work(struct work_struct *work)
{
	struct uvc_urb *uvc_urb = container_of(work, struct uvc_urb, work);
	struct uvc_streaming *stream = uvc_urb->stream;
	struct dma_async_tx_descriptor *tx = NULL;
	dma_cookie_t cookie;
	dma_addr_t dma_dst;
	unsigned int i;
	int ret;
//	char *src, *dst;
	
	for(i = 0; i < uvc_urb->async_operations; i++){
		struct uvc_copy_op *op = &uvc_urb->copy_operations[i];
		
		//memcpy(op->dst, op->src, op->len);
		//__memcpy_aarch64_simd(op->dst, op->src, op->len);
		
//		dst = (char *)op->dst;
//		src = (char *)op->src;
//		pr_info("%d src1: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x.\n", i, src[0], src[1], src[2], src[3], src[4], src[5], src[6], src[7], src[8], src[9]);

		
		
		dma_dst = dma_map_single(&stream->vdev.dev, op->dst, op->len, DMA_BIDIRECTIONAL);				//把内存no cache操作
		if(dma_mapping_error(&stream->vdev.dev, dma_dst))
			print_uvc(PRINT_ERROR, "dma_dst mapping failed\n");

		
		tx = dmaengine_prep_dma_memcpy(stream->chan, dma_dst, op->dma_src, op->len, DMA_CTRL_ACK | DMA_PREP_INTERRUPT);
		if(!tx){
			print_uvc(PRINT_ERROR, "device_prep_dma_memcpy error\n");
		}
		reinit_completion(&stream->rx_dma_complete);													//设置dma完成flag为0
		tx->callback = dma_uvc_callback;																//设置dma完成后的回调函数
		tx->callback_param = stream;		
		cookie = tx->tx_submit(tx);																		//开始dma传输
		ret = dma_submit_error(cookie);																	//检测dma传输是否提交成功
		if(ret){		
			print_uvc(PRINT_ERROR, "dma_submit_error %d\n", cookie);			
		}		
		dma_async_issue_pending(stream->chan);
		if(i == uvc_urb->async_operations-1){
			if(!wait_for_completion_timeout(&stream->rx_dma_complete, msecs_to_jiffies(1000))){				//睡眠等待dma中断的回调函数唤醒
				dmaengine_terminate_sync(stream->chan);
				print_uvc(PRINT_ERROR, "DMA wait_for_completion_timeout\n");
			}
		}

		if(dma_dst)
			dma_unmap_single(&stream->vdev.dev, dma_dst, op->len, DMA_BIDIRECTIONAL);
		
//		pr_info("%d dst4: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x.\n", i, dst[0], dst[1], dst[2], dst[3], dst[4], dst[5], dst[6], dst[7], dst[8], dst[9]);
		
		
		
		/* Release reference taken on this buffer. */
		uvc_queue_buffer_release(op->buf);
	}
	
	ret = usb_submit_urb(uvc_urb->urb, GFP_KERNEL);
	if(ret < 0)
		print_uvc(PRINT_ERROR, "Failed to resubmit video URB (%d).", ret);
}

static void uvc_video_stats_update(struct uvc_streaming *stream)
{
	struct uvc_stats_frame *frame = &stream->stats.frame;
	
//	print_uvc(PRINT_DEBUG, "frame %u stats: %u/%u/%u packets, "
//		  "%u/%u/%u pts (%searly %sinitial), %u/%u scr, "
//		  "last pts/stc/sof %u/%u/%u",
//		  stream->sequence, frame->first_data,
//		  frame->nb_packets - frame->nb_empty, frame->nb_packets,
//		  frame->nb_pts_diffs, frame->last_pts_diff, frame->nb_pts,
//		  frame->has_early_pts ? "" : "!",
//		  frame->has_initial_pts ? "" : "!",
//		  frame->nb_scr_diffs, frame->nb_scr,
//		  frame->pts, frame->scr_stc, frame->scr_sof);

	stream->stats.stream.nb_frames++;
	stream->stats.stream.nb_packets += stream->stats.frame.nb_packets;
	stream->stats.stream.nb_empty 	+= stream->stats.frame.nb_empty;
	stream->stats.stream.nb_errors	+= stream->stats.frame.nb_errors;
	stream->stats.stream.nb_invalid += stream->stats.frame.nb_invalid;
	
	if(frame->has_early_pts)
		stream->stats.stream.nb_pts_early++;
	if(frame->has_initial_pts)
		stream->stats.stream.nb_pts_initial++;
	if(frame->last_pts_diff <= frame->first_data)
		stream->stats.stream.nb_pts_constant++;
	if(frame->nb_scr >= frame->nb_packets - frame->nb_empty)
		stream->stats.stream.nb_scr_count_ok++;
	if(frame->nb_scr_diffs + 1 == frame->nb_scr)
		stream->stats.stream.nb_scr_diffs_ok++;

	memset(&stream->stats.frame, 0, sizeof(stream->stats.frame));
}

extern unsigned int uvc_clock_param;
static inline ktime_t uvc_video_get_time(void)
{
	if(uvc_clock_param == CLOCK_MONOTONIC)
		return ktime_get();
	else
		return ktime_get_real();
}

static void uvc_video_clock_decode(struct uvc_streaming *stream, struct uvc_buffer *buf, const u8 *data, int len)
{
	struct uvc_clock_sample *sample;
	unsigned int header_size;
	bool has_pts = false;
	bool has_scr = false;
	unsigned long flags;
	ktime_t time;
	u16 host_sof;
	u16 dev_sof;
	
	switch(data[1] & (UVC_STREAM_PTS | UVC_STREAM_SCR)){
		case UVC_STREAM_PTS | UVC_STREAM_SCR:
			header_size = 12;
			has_pts = true;
			has_scr = true;
			break;
		case UVC_STREAM_PTS:
			header_size = 6;
			has_pts = true;
			break;
		case UVC_STREAM_SCR:
			header_size = 8;
			has_scr = true;
			break;
		default:
			header_size = 2;
			break;
	}
	
	if(len < header_size)
		return;
	
	if(has_pts && buf != NULL)
		buf->pts = get_unaligned_le32(&data[2]);
	
	if(!has_scr)
		return;
	
	dev_sof = get_unaligned_le16(&data[header_size - 2]);
	if(dev_sof == stream->clock.last_sof)
		return;
	
	stream->clock.last_sof = dev_sof;
	
	host_sof = usb_get_current_frame_number(stream->dev->udev);
	time = uvc_video_get_time();
	
	if(stream->clock.sof_offset == (u16)-1){
		u16 delta_sof = (host_sof - dev_sof) & 255;
		if(delta_sof >= 10)
			stream->clock.sof_offset = delta_sof;
		else
			stream->clock.sof_offset = 0;
	}
	
	dev_sof = (dev_sof + stream->clock.sof_offset) & 2047;
	
	spin_lock_irqsave(&stream->clock.lock, flags);
	
	sample = &stream->clock.samples[stream->clock.head];
	sample->dev_stc = get_unaligned_le32(&data[header_size - 6]);
	sample->dev_sof = dev_sof;
	sample->host_sof = host_sof;
	sample->host_time = time;
	
	stream->clock.head = (stream->clock.head + 1) % stream->clock.size;
	
	if(stream->clock.count < stream->clock.size)
		stream->clock.count++;
	
	spin_unlock_irqrestore(&stream->clock.lock, flags);
}

/* Stream statistics */
static void uvc_video_stats_decode(struct uvc_streaming *stream, const u8 *data, int len)
{
	unsigned int header_size;
	bool has_pts = false;
	bool has_scr = false;
	u16 uninitialized_var(scr_sof);
	u32 uninitialized_var(scr_stc);
	u32 uninitialized_var(pts);
	
	if (stream->stats.stream.nb_frames == 0 &&
	    stream->stats.frame.nb_packets == 0)
		stream->stats.stream.start_ts = ktime_get();
	
	switch(data[1] & (UVC_STREAM_PTS | UVC_STREAM_SCR)){
		case UVC_STREAM_PTS | UVC_STREAM_SCR:
			header_size = 12;
			has_pts = true;
			has_scr = true;
			break;
		case UVC_STREAM_PTS:
			header_size = 6;
			has_pts = true;
			break;
		case UVC_STREAM_SCR:
			header_size = 8;
			has_scr = true;
			break;
		default:
			header_size = 2;
			break;
	}
	
	if(len < header_size || data[0] < header_size){
		stream->stats.frame.nb_invalid++;
		return;
	}
	
	if(has_pts)
		pts = get_unaligned_le32(&data[2]);
	
	if(has_scr){
		scr_stc = get_unaligned_le32(&data[header_size - 6]);
		scr_sof = get_unaligned_le16(&data[header_size - 2]);
	}
	
	if(has_pts && stream->stats.frame.nb_pts){
		if(stream->stats.frame.pts != pts){
			stream->stats.frame.nb_pts_diffs++;
			stream->stats.frame.last_pts_diff = stream->stats.frame.nb_packets;
		}
	}
	
	if(has_pts){
		stream->stats.frame.nb_pts++;
		stream->stats.frame.pts = pts;
	}
	
	if(stream->stats.frame.size == 0){
		if(len > header_size)
			stream->stats.frame.has_initial_pts = has_pts;
		if(len == header_size && has_pts)
			stream->stats.frame.has_early_pts = true;
	}
	
	if(has_scr && stream->stats.frame.nb_scr){
		if(stream->stats.frame.scr_stc != scr_stc)
			stream->stats.frame.nb_scr_diffs++;
	}
	
	if(has_scr){
		if(stream->stats.stream.nb_frames > 0 ||
			stream->stats.frame.nb_scr > 0)
			stream->stats.stream.scr_sof_count += (scr_sof - stream->stats.stream.scr_sof) % 2048;
		stream->stats.stream.scr_sof = scr_sof;
		
		stream->stats.frame.nb_scr++;
		stream->stats.frame.scr_stc = scr_stc;
		stream->stats.frame.scr_sof = scr_sof;
		
		if(scr_sof < stream->stats.stream.min_sof)
			stream->stats.stream.min_sof = scr_sof;
		if(scr_sof > stream->stats.stream.max_sof)
			stream->stats.stream.max_sof = scr_sof;
	}
	
	if(stream->stats.frame.size == 0 && len > header_size)
		stream->stats.frame.first_data = stream->stats.frame.nb_packets;
	
	stream->stats.frame.size += len - header_size;
	
	stream->stats.frame.nb_packets++;
	if(len <= header_size)
		stream->stats.frame.nb_empty++;
	
	if(data[1] & UVC_STREAM_ERR)
		stream->stats.frame.nb_errors++;
}

static int uvc_video_decode_start(struct uvc_streaming *stream, struct uvc_buffer *buf, const u8 *data, int len)
{
	u8 fid;
	
	if(len < 2 || data[0] < 2 || data[0] > len){
		stream->stats.frame.nb_invalid++;
		return -EINVAL;
	}
	
	fid = data[1] & UVC_STREAM_FID;
	
//	print_uvc(PRINT_DEBUG, "stream->last_fid=%d fid=%d stream->sequence=%d.", stream->last_fid, fid, stream->sequence);
	if(stream->last_fid != fid){
		stream->sequence++;
		if(stream->sequence)
			uvc_video_stats_update(stream);
	}
	
	uvc_video_clock_decode(stream, buf, data, len);
	uvc_video_stats_decode(stream, data, len);
	
	if(buf == NULL){
//		print_uvc(PRINT_ERROR, "buf == NULL.");
		stream->last_fid = fid;
		return -ENODATA;
	}
	
	if(data[1] & UVC_STREAM_ERR){
		print_uvc(PRINT_ERROR, "Marking buffer as bad (error bit set).");
		buf->error = 1;
	}
	
	if(buf->state != UVC_BUF_STATE_ACTIVE){
//		print_uvc(PRINT_ERROR, "UVC_BUF_STATE_ACTIVE.");
		if(fid == stream->last_fid){
//			print_uvc(PRINT_ERROR, "Dropping payload (out of sync). stream->last_fid=%d fid=%d", stream->last_fid, fid);
			if((stream->dev->quirks & UVC_QUIRK_STREAM_NO_FID) && (data[1] & UVC_STREAM_EOF))
				stream->last_fid ^= UVC_STREAM_FID;
			return -ENODATA;
		}
		
		buf->buf.field = V4L2_FIELD_NONE;
		buf->buf.sequence = stream->sequence;
		buf->buf.vb2_buf.timestamp = ktime_to_ns(uvc_video_get_time());
		
		buf->state = UVC_BUF_STATE_ACTIVE;
	}
	
	if(fid != stream->last_fid && buf->bytesused != 0){
		print_uvc(PRINT_ERROR, "Frame complete (FID bit toggled).");
		buf->state = UVC_BUF_STATE_READY;
		return -EAGAIN;
	}
	
	stream->last_fid = fid;
	
	return data[0];
}

static void uvc_video_validate_buffer(const struct uvc_streaming *stream, struct uvc_buffer *buf)
{
	if(stream->ctrl.dwMaxVideoFrameSize != buf->bytesused && !(stream->cur_format->flags & UVC_FMT_FLAG_COMPRESSED))
		buf->error = 1;
}

/* Completion handler for video URBs */
static void uvc_video_next_buffers(struct uvc_streaming *stream, struct uvc_buffer **video_buf, struct uvc_buffer **meta_buf)
{
	uvc_video_validate_buffer(stream, *video_buf);
	
	if(*meta_buf){
		struct vb2_v4l2_buffer *vb2_meta = &(*meta_buf)->buf;
		const struct vb2_v4l2_buffer *vb2_video = &(*video_buf)->buf;
		
		vb2_meta->sequence = vb2_video->sequence;
		vb2_meta->field = vb2_video->field;
		vb2_meta->vb2_buf.timestamp = vb2_video->vb2_buf.timestamp;
		
		(*meta_buf)->state = UVC_BUF_STATE_READY;
		if(!(*meta_buf)->error)
			(*meta_buf)->error = (*video_buf)->error;
		*meta_buf = uvc_queue_next_buffer(&stream->meta.queue, *meta_buf);
	}
	
	*video_buf = uvc_queue_next_buffer(&stream->queue, *video_buf);
}

static void uvc_video_decode_meta(struct uvc_streaming *stream, struct uvc_buffer *meta_buf,
									const u8 *mem, unsigned int length)
{
	struct uvc_meta_buf *meta;
	size_t len_std = 2;
	bool has_pts, has_scr;
	unsigned long flags;
	unsigned int sof;
	ktime_t time;
	const u8 *scr;
	
	if(!meta_buf || length ==2 ){
//		print_uvc(PRINT_ERROR, "!meta_buf || length ==2.");
		return;
	}
		
	
	if(meta_buf->length - meta_buf->bytesused < length + sizeof(meta->ns) + sizeof(meta->sof)){
		print_uvc(PRINT_ERROR, "meta_buf->error.");
		meta_buf->error = 1;
		return;
	}
	
	has_pts = mem[1] & UVC_STREAM_PTS;
	has_scr = mem[1] & UVC_STREAM_SCR;
	
	if(has_pts){
		len_std += 4;
		scr = mem + 6;
	}else{
		scr = mem + 2;
	}
	
	if(has_scr)
		len_std += 6;
	
	if(stream->meta.format == V4L2_META_FMT_UVC)
		length = len_std;
	
	if(length == len_std && (!has_scr || !memcmp(scr, stream->clock.last_scr, 6))){
		print_uvc(PRINT_ERROR, "length == len_std && (!has_scr || !memcmp(scr, stream->clock.last_scr, 6)).");
		return;
	}
	
	meta = (struct uvc_meta_buf *)((u8 *)meta_buf->mem + meta_buf->bytesused);
	
	local_irq_save(flags);
	time = uvc_video_get_time();
	sof = usb_get_current_frame_number(stream->dev->udev);
	local_irq_restore(flags);
	
	put_unaligned(ktime_to_ns(time), &meta->ns);
	put_unaligned(sof, &meta->sof);
	
	if(has_scr)
		memcpy(stream->clock.last_scr, scr, 6);
	
	memcpy(&meta->length, mem, length);
	meta_buf->bytesused += length + sizeof(meta->ns) + sizeof(meta->sof);
	
	print_uvc(PRINT_DEBUG, "%s(): t-sys %lluns, SOF %u, len %u, flags 0x%x, PTS %u, STC %u frame SOF %u\n",
		  __func__, ktime_to_ns(time), meta->sof, meta->length,
		  meta->flags,
		  has_pts ? *(u32 *)meta->buf : 0,
		  has_scr ? *(u32 *)scr : 0,
		  has_scr ? *(u32 *)(scr + 4) & 0x7ff : 0);
}

static void uvc_video_decode_data(struct uvc_urb *uvc_urb, struct uvc_buffer *buf, const u8 *data, dma_addr_t dma_addr, int len)
{
	unsigned int active_op = uvc_urb->async_operations;
	struct uvc_copy_op *op = &uvc_urb->copy_operations[active_op];
	unsigned int maxlen;
	
	if(len <= 0)
		return;
	
	maxlen = buf->length - buf->bytesused;
	
	kref_get(&buf->ref);
	
	op->buf = buf;
	op->src = data;
	op->dma_src = dma_addr;
	op->dst = buf->mem + buf->bytesused;
	op->len = min_t(unsigned int, len, maxlen);
	
	buf->bytesused += op->len;
	
	/* Complete the current frame if the buffer size was exceeded. */
	if(len > maxlen){
		print_uvc(PRINT_ERROR, "Frame complete (overflow) len=%d maxlen=%d.", len, maxlen);
		buf->error = 1;
		buf->state = UVC_BUF_STATE_READY;
	}
	
//	print_uvc(PRINT_DEBUG, "async_operations++.");
	uvc_urb->async_operations++;
}

static void uvc_video_decode_end(struct uvc_streaming *stream, struct uvc_buffer *buf, const u8 *data, int len)
{
	/* Mark the buffer as done if the EOF marker is set. */
	if(data[1] & UVC_STREAM_EOF && buf->bytesused != 0){
//		print_uvc(PRINT_ERROR, "Frame complete (EOF found).");
		if(data[0] == len)
			print_uvc(PRINT_ERROR, "EOF in empty payload.");
		buf->state = UVC_BUF_STATE_READY;
		if(stream->dev->quirks & UVC_QUIRK_STREAM_NO_FID)
			stream->last_fid ^= UVC_STREAM_FID;
	}
}

static void uvc_video_decode_isoc(struct uvc_urb *uvc_urb, struct uvc_buffer *buf, struct uvc_buffer *meta_buf)
{
	struct urb *urb = uvc_urb->urb;
	struct uvc_streaming *stream = uvc_urb->stream;
	u8 *mem;
	dma_addr_t dma_addr;
	int ret, i;
	
//	print_uvc(PRINT_DEBUG, "urb->number_of_packets=%d.", urb->number_of_packets);
	for(i = 0; i < urb->number_of_packets; ++i){
//		print_uvc(PRINT_DEBUG, "%d.", i);
		if(urb->iso_frame_desc[i].status < 0){
			print_uvc(PRINT_ERROR, "USB isochronous frame lost %d (%d).", i, urb->iso_frame_desc[i].status);
			if(buf != NULL){
				print_uvc(PRINT_ERROR, "buf->error.");
				buf->error = 1;
			}
			continue;
		}
		
		/* Decode the payload header. */
		mem = urb->transfer_buffer + urb->iso_frame_desc[i].offset;
		//print_uvc(PRINT_INFO, "mem=0x%llx  urb->transfer_buffer=0x%llx urb->iso_frame_desc[i].offset=%x.", mem, urb->transfer_buffer, urb->iso_frame_desc[i].offset);
		dma_addr =  urb->transfer_dma + urb->iso_frame_desc[i].offset;
		//print_uvc(PRINT_INFO, "dma_addr=0x%llx  urb->transfer_dma=0x%llx urb->iso_frame_desc[i].offset=%x.", dma_addr, urb->transfer_dma, urb->iso_frame_desc[i].offset);
		do {
			ret = uvc_video_decode_start(stream, buf, mem, urb->iso_frame_desc[i].actual_length);
//			print_uvc(PRINT_DEBUG, "urb->iso_frame_desc[i].actual_length=%d ret=%d.", urb->iso_frame_desc[i].actual_length, ret);
			if(ret == -EAGAIN)
				uvc_video_next_buffers(stream, &buf, &meta_buf);
		} while(ret == -EAGAIN);
			
		if(ret < 0)
			continue;
		
		uvc_video_decode_meta(stream, meta_buf, mem, ret);
		
		/* Decode the payload data. */
		//print_uvc(PRINT_INFO, "mem=0x%llx", mem);
		uvc_video_decode_data(uvc_urb, buf, mem + ret, dma_addr + ret, urb->iso_frame_desc[i].actual_length - ret);
		
		/* Process the header again. */
		uvc_video_decode_end(stream, buf, mem, urb->iso_frame_desc[i].actual_length);
		
		if(buf->state == UVC_BUF_STATE_READY)
			uvc_video_next_buffers(stream, &buf, &meta_buf);
	}
}

/*----------------------------------------------------------------------------------------
 * Video device
 */

/*
 * Initialize the UVC video device by switching to alternate setting 0 and
 * retrieve the default format.
 *
 * This function is called before registering the device with V4L.
 */
int uvc_video_init(struct uvc_streaming *stream)
{
	int ret, i;
	struct uvc_format *format = NULL;
	struct uvc_frame *frame = NULL;
	struct uvc_urb *uvc_urb;
	struct uvc_streaming_control *probe = &stream->ctrl;
	
	if(stream->nformats ==0){
		print_uvc(PRINT_ERROR, "No supported video formats found.");
		return -EINVAL;
	}
	atomic_set(&stream->active, 0);

	print_uvc(PRINT_DEBUG, "stream->intfnum = %d", stream->intfnum);
	usb_set_interface(stream->dev->udev, stream->intfnum, 0);
	
	if(uvc_get_video_ctrl(stream, probe, 1, UVC_GET_DEF) == 0){
		print_uvc(PRINT_ERROR, "UVC_GET_DEF fail.");
	}
		
	ret = uvc_get_video_ctrl(stream, probe, 1, UVC_GET_CUR);
	if( ret < 0)
		return -1;
	
	for(i = stream->nformats; i > 0; --i){
		format = &stream->format[i - 1];
		if(format->index == probe->bFormatIndex)
			break;
	}
	
	if(format->nframes == 0){
		print_uvc(PRINT_ERROR, "No frame descriptor found for the default format.");
		return -EINVAL;
	}
	
	for(i = format->nframes; i > 0; --i){
		frame = &format->frame[i - 1];
		if(frame->bFrameIndex == probe->bFrameIndex)
			break;
	}

	probe->bFormatIndex = format->index;
	probe->bFrameIndex = frame->bFrameIndex;
	
	stream->def_format = format;
	stream->cur_format = format;
	stream->cur_frame = frame;
	
	/* Select the video decoding function */
	if(stream->type == V4L2_BUF_TYPE_VIDEO_CAPTURE){
		if(stream->intf->num_altsetting > 1)
			stream->decode = uvc_video_decode_isoc;
	}
	
	/* Prepare asynchronous works items. */
	for_each_uvc_urb(uvc_urb, stream)
		INIT_WORK(&uvc_urb->work, uvc_video_copy_data_work);
	

	
	return 0;
}

static void uvc_video_clock_reset(struct uvc_streaming *stream)
{
	struct uvc_clock *clock = &stream->clock;
	
	clock->head = 0;
	clock->count = 0;
	clock->last_sof = -1;
	clock->sof_offset = -1;
}

static int uvc_video_clock_init(struct uvc_streaming *stream)
{
	struct uvc_clock *clock = &stream->clock;
	
	spin_lock_init(&clock->lock);
	clock->size = 32;
	
	clock->samples = kmalloc_array(clock->size, sizeof(*clock->samples), GFP_KERNEL);
	if(clock->samples == NULL)
		return -ENOMEM;
	
	uvc_video_clock_reset(stream);
	
	return 0;
}

static void uvc_video_clock_cleanup(struct uvc_streaming *stream)
{
	kfree(stream->clock.samples);
	stream->clock.samples = NULL;
}

static int uvc_commit_video(struct uvc_streaming *stream, struct uvc_streaming_control *probe)
{
	return uvc_set_video_ctrl(stream, probe, 0);
}

static void uvc_video_stats_start(struct uvc_streaming *stream)
{
	memset(&stream->stats, 0, sizeof(stream->stats));
	stream->stats.stream.min_sof = 2048;
}

static unsigned int uvc_endpoint_max_bpi(struct usb_device *dev, struct usb_host_endpoint *ep)
{
	u16 psize;
	u16 mult;
	
	switch(dev->speed){
		case USB_SPEED_HIGH:
			psize = usb_endpoint_maxp(&ep->desc);
			mult = usb_endpoint_maxp_mult(&ep->desc);
			//print_uvc(PRINT_INFO, "psize=%d, mult=%d.", psize, mult);
			return psize * mult;
		default:
			psize = usb_endpoint_maxp(&ep->desc);
			return psize;
	};
	
}

static void uvc_free_urb_buffers(struct uvc_streaming *stream)
{
	struct uvc_urb *uvc_urb;
	
	for_each_uvc_urb(uvc_urb, stream){
		if(!uvc_urb->buffer)
			continue;
		
		usb_free_coherent(stream->dev->udev, stream->urb_size, uvc_urb->buffer, uvc_urb->dma);
		uvc_urb->buffer = NULL;
	}
	
	stream->urb_size = 0;

	if(stream->chan)
		dma_release_channel(stream->chan);
}

static int uvc_alloc_urb_buffers(struct uvc_streaming *stream, unsigned int size, unsigned int psize, gfp_t gfp_flags)
{
	unsigned int npackets;
	unsigned int i;
	
	/* Buffers are already allocated, bail out. */
	if(stream->urb_size)
		return stream->urb_size / psize;
	print_uvc(PRINT_DEBUG, "size=%d  psize=%d.", size, psize);
	/* Compute the number of packets. Bulk endpoints might transfer UVC payloads across multiple URBs. */
	npackets = DIV_ROUND_UP(size, psize);
	if(npackets > UVC_MAX_PACKETS)
		npackets = UVC_MAX_PACKETS;
	print_uvc(PRINT_DEBUG, "npackets=%d.", npackets);
	
	/* Retry allocations until one succeed. */
	for(; npackets > 1; npackets /=2){
		for(i = 0; i < UVC_URBS; ++i){
			struct uvc_urb *uvc_urb = &stream->uvc_urb[i];
			
			stream->urb_size = psize * npackets;
			uvc_urb->buffer = usb_alloc_coherent(stream->dev->udev, stream->urb_size,
												gfp_flags | __GFP_NOWARN, &uvc_urb->dma);
			
			if(!uvc_urb->buffer){
				uvc_free_urb_buffers(stream);
				break;
			}
			
			uvc_urb->stream = stream;
		}
		
		if(i == UVC_URBS){
			dma_cap_zero(stream->dma_test_mask);
			dma_cap_set(DMA_MEMCPY, stream->dma_test_mask); 												//Memory to memory copy
			stream->chan = dma_request_chan_by_mask(&stream->dma_test_mask);
			if (IS_ERR(stream->chan)) {
				print_uvc(PRINT_ERROR, "dma request channel failed\n");
				return 0;
			}
			init_completion(&stream->rx_dma_complete);														//初始化dma完成flag
			
			print_uvc(PRINT_INFO, "DMA SUCCESS, Allocated %u URB buffers of %ux%u bytes each.", UVC_URBS, npackets, psize);
			return npackets;
		}
	}
	
	print_uvc(PRINT_ERROR, "Failed to allocate URB buffers (%u bytes per packet).", psize);
	
	return 0;
}

/* Uninitialize isochronous/bulk URBs and free transfer buffers. */
static void uvc_video_stop_transfer(struct uvc_streaming *stream, int free_buffers)
{
	struct uvc_urb *uvc_urb;
	
	stream->stats.stream.stop_ts = ktime_get();
	
	for_each_uvc_urb(uvc_urb, stream)
		usb_poison_urb(uvc_urb->urb);
		
	flush_workqueue(stream->async_wq);
	
	for_each_uvc_urb(uvc_urb, stream){
		usb_free_urb(uvc_urb->urb);
		uvc_urb->urb = NULL;
	}
	
	if(free_buffers)
		uvc_free_urb_buffers(stream);
}

static void uvc_video_complete(struct urb *urb)
{
	struct uvc_urb *uvc_urb = urb->context;
	struct uvc_streaming *stream = uvc_urb->stream;
	struct uvc_video_queue *queue = &stream->queue;
	struct uvc_video_queue *qmeta = &stream->meta.queue;
	struct vb2_queue *vb2_qmeta = stream->meta.vdev.queue;
	struct uvc_buffer *buf = NULL;
	struct uvc_buffer *buf_meta = NULL;
	unsigned long flags;
	int ret;
	
	switch(urb->status) {
		case 0:
		break;
		
		default:
			print_uvc(PRINT_ERROR, "Non-zero status (%d) in video completion handler.", urb->status);
		case -ENOENT:
			if(stream->frozen)
				return;
		case -ECONNRESET:
		case -ESHUTDOWN:
			uvc_queue_cancel(queue, urb->status == -ESHUTDOWN);
			if(vb2_qmeta)
				uvc_queue_cancel(qmeta, urb->status == -ESHUTDOWN);
			return;
	}
	
	buf = uvc_queue_get_current_buffer(queue);
	
	if(vb2_qmeta){
//		print_uvc(PRINT_DEBUG, "vb2_qmeta.");
		spin_lock_irqsave(&qmeta->irqlock, flags);
		if(!list_empty(&qmeta->irqqueue))
			buf_meta = list_first_entry(&qmeta->irqqueue, struct uvc_buffer, queue);
		spin_unlock_irqrestore(&qmeta->irqlock, flags);
	}
	
//	if(buf_meta == NULL)
//		print_uvc(PRINT_DEBUG, "buf_meta == NULL.");
	
	/* Re-initialise the URB async work. */
	uvc_urb->async_operations = 0;
	
	/* Process the URB headers, and optionally queue expensive memcpy tasks to be deferred to a work queue. */
	stream->decode(uvc_urb, buf, buf_meta);
	
	/* If no async work is needed, resubmit the URB immediately. */
	if(!uvc_urb->async_operations){
//		print_uvc(PRINT_ERROR, "async_operations");
		ret = usb_submit_urb(uvc_urb->urb, GFP_ATOMIC);
		if(ret < 0)
			print_uvc(PRINT_ERROR, "Failed to resubmit video URB (%d).", ret);
		return;
	}
	
	queue_work(stream->async_wq, &uvc_urb->work);
}

/* Initialize isochronous URBs and allocate transfer buffers. The packet size is given by the endpoint. */
static int uvc_init_video_isoc(struct uvc_streaming *stream, struct usb_host_endpoint *ep, gfp_t gfp_flags)
{
	struct urb *urb;
	struct uvc_urb *uvc_urb;
	unsigned int npackets, i;
	u16 psize;
	u32 size;
	
	psize = uvc_endpoint_max_bpi(stream->dev->udev, ep);
	size = stream->ctrl.dwMaxVideoFrameSize;
	
	npackets = uvc_alloc_urb_buffers(stream, size, psize, gfp_flags);
	if(npackets == 0)
		return -ENOMEM;
	
	size = npackets * psize;
	
	for_each_uvc_urb(uvc_urb, stream){
//		print_uvc(PRINT_DEBUG, ".");
		urb = usb_alloc_urb(npackets, gfp_flags);
		if(urb == NULL){
			uvc_video_stop_transfer(stream, 1);
			return -ENOMEM;
		}
		
		urb->dev 					= stream->dev->udev;
		urb->context 				= uvc_urb;
		urb->pipe 					= usb_rcvisocpipe(stream->dev->udev, ep->desc.bEndpointAddress);
				
		urb->transfer_flags 		= URB_ISO_ASAP | URB_NO_TRANSFER_DMA_MAP;
		urb->transfer_dma 			= uvc_urb->dma;
			
		urb->interval 				= ep->desc.bInterval;
		urb->transfer_buffer 		= uvc_urb->buffer;
		urb->complete 				= uvc_video_complete;
		urb->number_of_packets		= npackets;
		urb->transfer_buffer_length	= size;
		
		for(i = 0; i < npackets; ++i){
			urb->iso_frame_desc[i].offset = i * psize;
			urb->iso_frame_desc[i].length = psize;
		}
		
		uvc_urb->urb = urb;
	}
	
	return 0;
}

static inline u32 uvc_urb_index(const struct uvc_urb *uvc_urb)
{
	return uvc_urb - &uvc_urb->stream->uvc_urb[0];
}

/* Initialize isochronous/bulk URBs and allocate transfer buffers. */
static int uvc_video_start_transfer(struct uvc_streaming *stream, gfp_t gfp_flags)
{
	struct usb_interface *intf = stream->intf;
	struct usb_host_endpoint *ep;
	struct uvc_urb *uvc_urb;
	unsigned int i;
	int ret = -1;
	
	stream->sequence = -1;
	stream->last_fid = -1;
	stream->bulk.header_size = 0;
	stream->bulk.skip_payload = 0;
	stream->bulk.payload_size = 0;
	
	uvc_video_stats_start(stream);

	//print_uvc(PRINT_DEBUG, "intf->num_altsetting=%d", intf->num_altsetting);
	if(intf->num_altsetting > 1){
		struct usb_host_endpoint *best_ep = NULL;
		unsigned int best_psize = UINT_MAX;
		unsigned int bandwidth;
		unsigned int uninitialized_var(altsetting);
		int intfnum = stream->intfnum;
		
		/* Isochronous endpoint, select the alternate setting. */
		bandwidth = stream->ctrl.dwMaxPayloadTransferSize;
		if(bandwidth == 0){
			print_uvc(PRINT_DEBUG, "Device requested null bandwidth, defaulting to lowest.");
			bandwidth = 1;
		}else{
			print_uvc(PRINT_DEBUG, "Device requested %u, B/frame bandwidth.", bandwidth);
		}
		
		for(i = 0; i < intf->num_altsetting; ++i){
			struct usb_host_interface *alts;
			unsigned int psize;
			
			alts = &intf->altsetting[i];
			ep = uvc_find_endpoint(alts, stream->header.bEndpointAddress);
			if(ep == NULL)
				continue;
			
			/* Check if the bandwidth is high enough. */
			psize = uvc_endpoint_max_bpi(stream->dev->udev, ep);
			if(psize >= bandwidth && psize <= best_psize){
				altsetting = alts->desc.bAlternateSetting;
				best_psize = psize;
				best_ep = ep;
			}
		}

		if(best_ep == NULL){
			print_uvc(PRINT_ERROR, "No fast enough alt setting for requested bandwidth.");
			return -EIO;
		}
		
		print_uvc(PRINT_DEBUG, "Selecting alternate setting %u (%u B/frame bandwidth).", altsetting, best_psize);
		
		ret = usb_set_interface(stream->dev->udev, intfnum, altsetting);
		if(ret < 0)
			return ret;
		
		ret = uvc_init_video_isoc(stream, best_ep, gfp_flags);
	}

	if(ret < 0)
		return ret;

	/* Submit the URBs */
	for_each_uvc_urb(uvc_urb, stream){
		ret = usb_submit_urb(uvc_urb->urb, gfp_flags);
		if(ret < 0){
			print_uvc(PRINT_ERROR, "Failed to submit URB %u (%d).", uvc_urb_index(uvc_urb), ret);
			uvc_video_stop_transfer(stream, 1);
			return ret;
		}
	}
	
	return 0;
}


int uvc_video_start_streaming(struct uvc_streaming *stream)
{
	int ret;
	
	ret = uvc_video_clock_init(stream);
	if(ret < 0)
		return ret;
	
	/* Commit the streaming parameters. */
	ret = uvc_commit_video(stream, &stream->ctrl);
	if(ret < 0)
		goto error_commit;
	
	ret = uvc_video_start_transfer(stream, GFP_KERNEL);
	if(ret < 0)
		goto error_video;
	
	return 0;
	
error_video:
	usb_set_interface(stream->dev->udev, stream->intfnum, 0);
error_commit:
	uvc_video_clock_cleanup(stream);
	
	return ret;
}

void uvc_video_stop_streaming(struct uvc_streaming *stream)
{
	uvc_video_stop_transfer(stream, 1);
	
	if(stream->intf->num_altsetting > 1)
		usb_set_interface(stream->dev->udev, stream->intfnum, 0);
	
	uvc_video_clock_cleanup(stream);
}





















