#include "uvc_camera.h"

unsigned int uvc_clock_param = CLOCK_MONOTONIC;

static u32 uvc_quirks_param = -1;
struct usb_driver uvc_driver;

unsigned int uvc_trace_param = 1;

/* Video formats */
static struct uvc_format_desc uvc_fmts[] = {
	{
		.name	= "YUV 4:2:2(YUYV)",
		.guid	= UVC_GUID_FORMAT_YUY2,
		.fcc	= V4L2_PIX_FMT_YUYV,
	},
	
};

static void uvc_stream_delete(struct uvc_streaming *stream)
{
	if(stream->async_wq)
		destroy_workqueue(stream->async_wq);
	
	mutex_destroy(&stream->mutex);
	
	usb_put_intf(stream->intf);
	
	kfree(stream->format);
	kfree(stream->header.bmaControls);
	kfree(stream);
}

static struct uvc_streaming *uvc_stream_new(struct uvc_device *dev, struct usb_interface *intf)
{
	struct uvc_streaming *stream;
	
	if((stream = kzalloc(sizeof(*stream), GFP_KERNEL)) == NULL){
		print_uvc(PRINT_ERROR, "stream kzalloc fail");
		return NULL;
	}

	mutex_init(&stream->mutex);
	
	stream->dev = dev;
	stream->intf = usb_get_intf(intf);
	stream->intfnum = intf->cur_altsetting->desc.bInterfaceNumber;									//接口id，对应流媒体的接口

	/* allocate a stream apecific work queue for asynchronous tasks. */
	if(!(stream->async_wq = alloc_workqueue("uvcvideo", WQ_UNBOUND | WQ_HIGHPRI, 0))){				//创建新的工作队列UNBOUND，HIGHPRI
		uvc_stream_delete(stream);
		return NULL;
	}

	return stream;
}

static struct uvc_format_desc *uvc_format_by_guid(const u8 guid[16])
{
	unsigned int len = ARRAY_SIZE(uvc_fmts);
	unsigned int i;

	for(i=0; i < len; i++){
		if(memcmp(guid, uvc_fmts[i].guid, 4) == 0)
			return &uvc_fmts[i];
	}
	
	return NULL;
}

static u32 uvc_colorspace(const u8 primaries)
{
	static const u8 colorprimaries[] = {
		0,
		V4L2_COLORSPACE_SRGB,
		V4L2_COLORSPACE_470_SYSTEM_M,
		V4L2_COLORSPACE_470_SYSTEM_BG,
		V4L2_COLORSPACE_SMPTE170M,
		V4L2_COLORSPACE_SMPTE240M,
	};
	
	if(primaries < ARRAY_SIZE(colorprimaries))
		return colorprimaries[primaries];
	
	return 0;
}

static int uvc_parse_format(struct uvc_device *dev, struct uvc_streaming *streaming,
							struct uvc_format *format, u32 **intervals, unsigned char *buffer, int buflen)
{
	struct usb_interface *intf = streaming->intf;
	struct usb_host_interface *alts = intf->cur_altsetting;
	struct uvc_format_desc *fmtdesc;
	struct uvc_frame *frame;
	const unsigned char *start = buffer;
	unsigned int width_multiplier = 1;
	unsigned int interval;
	unsigned int n, i;
	u8 ftype=0;;

	
	format->type = buffer[2];
	format->index = buffer[3];
	
	print_uvc(PRINT_DEBUG, "buflen=%d, %d, %d %d %d %d %d", buflen, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);		//第二个VideoStreaming Interface Descriptor
	
	switch(buffer[2]){
		case UVC_VS_FORMAT_UNCOMPRESSED:
			n = buffer[2] == UVC_VS_FORMAT_UNCOMPRESSED ? 27 : 28;
			if(buflen < n){
				print_uvc(PRINT_ERROR, "device %d videostreaming interface %d FORMAT error", dev->udev->devnum, alts->desc.bInterfaceNumber);
				return -EINVAL;
			}
			
			/* Find the format descriptor from its GUID. */
			fmtdesc = uvc_format_by_guid(&buffer[5]);
			if(fmtdesc != NULL){
				strscpy(format->name, fmtdesc->name, sizeof(format->name));
				format->fcc = fmtdesc->fcc;
			}else{
				print_uvc(PRINT_ERROR, "Unknown video format %pUl", &buffer[5]);
				snprintf(format->name, sizeof(format->name), "%pUl", &buffer[5]);
				format->fcc = 0;
			}
			format->bpp = buffer[21];
			
			ftype = UVC_VS_FRAME_UNCOMPRESSED;
		break;

		case UVC_VS_FORMAT_MJPEG:
			if(buflen < 11){
				print_uvc(PRINT_ERROR, "device %d videostreaming interface %d FORMAT error", dev->udev->devnum, alts->desc.bInterfaceNumber);
				return -EINVAL;
			}
			
			strscpy(format->name, "MJPEG", sizeof(format->name));
			format->fcc = V4L2_PIX_FMT_MJPEG;
			format->flags = UVC_FMT_FLAG_COMPRESSED;
			format->bpp = 0;
			ftype = UVC_VS_FRAME_MJPEG;
			break;
	}
	
	print_uvc(PRINT_DEBUG, "Found foramt %s.", format->name);

/*******************************************************************************************************************/	

	buflen -= buffer[0];																//跳过 FORMAT_MJPEG	FORMAT_UNCOMPRESSED
	buffer += buffer[0];
	print_uvc(PRINT_DEBUG, "buflen111=%d, %d, %d %d %d %d %d %d %d %d", buflen, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[23], buffer[24], buffer[25] );	//第三个VideoStreaming Interface Descriptor
	/* Parse the frame descriptors. Only uncompressed, MJPEG and frame based formats have frame descriptors.*/
	while(buflen > 2 && buffer[1] == USB_DT_CS_INTERFACE && buffer[2] == ftype){
		frame = &format->frame[format->nframes];
		n = buflen > 25 ? buffer[25] : 0;
		n = n ? n : 3;
		
		if(buflen < 26 + 4 * n){
			print_uvc(PRINT_ERROR, "device %d videostreaming interface %d FRAME error", dev->udev->devnum, alts->desc.bInterfaceNumber);
			return -EINVAL;
		}
		
		frame->bFrameIndex = buffer[3];
		frame->bmCapabilities = buffer[4];
		frame->wWidth = get_unaligned_le16(&buffer[5]) * width_multiplier;
		frame->wHeight = get_unaligned_le16(&buffer[7]);
		frame->dwMinBitRate = get_unaligned_le32(&buffer[9]);
		frame->dwMaxBitRate = get_unaligned_le32(&buffer[13]);
		frame->dwMaxVideoFrameBufferSize = get_unaligned_le32(&buffer[17]);
		frame->dwDefaultFrameInterval = get_unaligned_le32(&buffer[21]);
		frame->bFrameIntervalType = buffer[25];
		frame->dwFrameInterval = *intervals;
		
		for(i = 0; i < n; i++){
			interval = get_unaligned_le32(&buffer[26+4*i]);
			//print_uvc(PRINT_ERROR, "interval = %d", interval);
			*(*intervals)++ = interval ? interval : 1;
		}
		
		n -= frame->bFrameIntervalType ? 1 : 2;
		frame->dwDefaultFrameInterval = min(frame->dwFrameInterval[n], max(frame->dwFrameInterval[0], frame->dwDefaultFrameInterval));
		
		print_uvc(PRINT_DEBUG, "- %ux%u (%u.%u fps)", frame->wWidth, frame->wHeight,
										10000000/frame->dwDefaultFrameInterval,
										(100000000/frame->dwDefaultFrameInterval)%10);
		
		format->nframes++;
		buflen -= buffer[0];
		buffer += buffer[0];
	}

/*******************************************************************************************************************/
	
	print_uvc(PRINT_DEBUG, "buflen111=%d, %d, %d %d %d %d %d", buflen, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
	if(buflen > 2 && buffer[1] == USB_DT_CS_INTERFACE && buffer[2] == UVC_VS_STILL_IMAGE_FRAME){			//跳过STILL_IMAGE_FRAME
		buflen -= buffer[0];
		buffer += buffer[0];
	}

/*******************************************************************************************************************/

	if(buflen > 2 && buffer[1] == USB_DT_CS_INTERFACE && buffer[2] == UVC_VS_COLORFORMAT){					//保留COLORFORMAT的 bColorPrimaries
		if(buflen < 6){
			print_uvc(PRINT_ERROR, "device %d videostreaming interface %d COLORFORMAT error", dev->udev->devnum, alts->desc.bInterfaceNumber);
			return -EINVAL;
		}
		
		format->colorspace = uvc_colorspace(buffer[3]);
		
		buflen -= buffer[0];
		buffer += buffer[0];
	}
	
	print_uvc(PRINT_DEBUG, "buflen = %d, buffer - start = %d",buflen, (u32)(buffer - start));
	
	return buffer - start;
}

struct usb_host_endpoint *uvc_find_endpoint(struct usb_host_interface *alts, u8 epaddr)
{
	struct usb_host_endpoint *ep;
	unsigned int i;
	//print_uvc(PRINT_PARSE, "alts->desc.bNumEndpoints = %d", alts->desc.bNumEndpoints);
	for(i = 0; i < alts->desc.bNumEndpoints; i++){
		ep = &alts->endpoint[i];
		//print_uvc(PRINT_ERROR, "ep->desc.bEndpointAddress = %d, epaddr = %d",ep->desc.bEndpointAddress, epaddr);
		if(ep->desc.bEndpointAddress == epaddr)
			return ep;
	}
	
	return NULL;
}

static int uvc_parse_streaming(struct uvc_device *dev, struct usb_interface *intf)
{
	struct uvc_streaming *streaming = NULL;
	struct uvc_format *format;
	struct uvc_frame *frame;
	struct usb_host_interface *alts = &intf->altsetting[0];
	u8 *_buffer, *buffer = alts->extra;																		//从流媒体的第一个VideoStreaming Interface Descriptor开始
	int _buflen, buflen = alts->extralen;
	u32 size, p, n, i;
	u32 nformats = 0, nframes = 0, nintervals = 0;
	u32 *interval;
	u16 psize;
	int ret = -EINVAL;
	
	print_uvc(PRINT_DEBUG, "buflen=%d, %d, %d %d %d %d %d %d %d %d", buflen, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8]);
	
	if(intf->cur_altsetting->desc.bInterfaceSubClass != UVC_SC_VIDEOSTREAMING){						//确定streaming接口描述符信息
		print_uvc(PRINT_ERROR, "device %d interface %d isn't a video streaming interface", dev->udev->devnum, intf->altsetting[0].desc.bInterfaceNumber);
		return -EINVAL;
	}
	
	if(usb_driver_claim_interface(&uvc_driver, intf, dev)){											//声明这个流接口描述符intf已经有驱动绑定
		print_uvc(PRINT_ERROR, "device %d interface %d is already claimed", dev->udev->devnum, intf->altsetting[0].desc.bInterfaceNumber);
		return -EINVAL;
	}
	
	streaming = uvc_stream_new(dev, intf);
	if(streaming == NULL){
		usb_driver_release_interface(&uvc_driver, intf);
		return -ENOMEM;
	}
	
	
	/* skip the standard interface descriptor */
	while(buflen > 2 && buffer[1] != USB_DT_CS_INTERFACE){											//USB_DT_CS_INTERFACE=36
		buflen -= buffer[0];
		buffer += buffer[0];
		print_uvc(PRINT_DEBUG, "skip, buflen=%d buffer[2]=%d", buflen, buffer[2]);
	}
	if(buflen <= 2){
		print_uvc(PRINT_ERROR, "no class-specific streaming interface descriptors found.");
		goto error;
	}

/****************************************************INPUT_HEADER***************************************************************/	
	/* parse the header descriptor */
	switch(buffer[2]){
		//case UVC_VS_OUTPUT_HEADER:
			
		case UVC_VS_INPUT_HEADER:
			streaming->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			size = 13;
		break;
		
		default:
			print_uvc(PRINT_ERROR, "device %d videostreaming interface %d HEADER descriptor not found.", dev->udev->devnum, alts->desc.bInterfaceNumber);
			goto error;
	}

	p = buflen >= 4 ? buffer[3] : 0;				//bNumFormats
	n = buflen >= size ? buffer[size-1] : 0;		//bControlSize

	print_uvc(PRINT_DEBUG, "p=%d n=%d buffer[size-2]=%d %d %d %d %d", p, n, buffer[size], buffer[size+1], buffer[size+2], buffer[size+3], buffer[size+4]);
	
	if(buflen < size + p*n){
		print_uvc(PRINT_ERROR, "device %d videostreaming interface %d HEADER descriptor is invalid.", dev->udev->devnum, alts->desc.bInterfaceNumber);
		goto error;
	}

	streaming->header.bNumFormats = p;
	streaming->header.bControlSize = n;
	streaming->header.bEndpointAddress = buffer[6];
	if(buffer[2] == UVC_VS_INPUT_HEADER){
		streaming->header.bmInfo = buffer[7];
		streaming->header.bTerminalLink = buffer[8];
		streaming->header.bStillCaptureMethod = buffer[9];
		streaming->header.bTriggerSupport = buffer[10];
		streaming->header.bTriggerUsage = buffer[11];
	}
	streaming->header.bmaControls = kmemdup(&buffer[size], p * n, GFP_KERNEL);
	if(streaming->header.bmaControls == NULL){
		ret = -ENOMEM;
		goto error;
	}
/*******************************************************************************************************************/

	buflen -= buffer[0];																//跳过 INPUT_HEADER
	buffer += buffer[0];
	
	_buffer = buffer;
	_buflen = buflen;

	/* Count the format and frame descriptors. */
	while(_buflen > 2 && _buffer[1] == USB_DT_CS_INTERFACE){							//USB_DT_CS_INTERFACE=36
		//print_uvc(PRINT_PARSE, "_buflen = %d _buffer[25]=%d", _buflen, _buffer[25]);
		switch(_buffer[2]){
			case UVC_VS_FORMAT_UNCOMPRESSED:											//4
			case UVC_VS_FORMAT_MJPEG:													//6
				nformats++;																//指格式类型，这里指mjpeg编码FORMAT_MJPEG和非编码FORMAT_UNCOMPRESSED两种	,一般UVC摄像头支持多个格式(format)，每个格式又支持多个分辨率(frame)
			break;
			
			case UVC_VS_FRAME_UNCOMPRESSED:												//5
			case UVC_VS_FRAME_MJPEG:													//7
				nframes++;																//上面两种编码下各包含了几种分辨率的格式
				if(_buflen > 25)
					nintervals += _buffer[25] ? _buffer[25] : 3;
				break;
		}
		
		_buflen -= _buffer[0];
		_buffer += _buffer[0];
	}

	if(nformats == 0){
		print_uvc(PRINT_ERROR, "device %d videostreaming interface %d has no supported formats defined.", dev->udev->devnum, alts->desc.bInterfaceNumber);
		goto error;
	}
	
	print_uvc(PRINT_DEBUG, "nformats=%d nframes=%d nintervals=%d", nformats, nframes, nintervals);
	
	size = nformats * sizeof(*format) + nframes * sizeof(*frame) + nintervals * sizeof(*interval);
	format = kzalloc(size, GFP_KERNEL);
	if(format == NULL){
		ret = -ENOMEM;
		goto error;
	}
	
	frame = (struct uvc_frame *)&format[nformats];
	interval = (u32 *)&frame[nframes];
	
	streaming->format = format;
	streaming->nformats = nformats;

/*******************************************************************************************************************/

	/* Parse the format descriptors. */
	while(buflen > 2 && buffer[1] == USB_DT_CS_INTERFACE){								//USB_DT_CS_INTERFACE=36
		switch (buffer[2]){
			case UVC_VS_FORMAT_UNCOMPRESSED:											//4
			case UVC_VS_FORMAT_MJPEG:													//6
				format->frame = frame;
				ret = uvc_parse_format(dev, streaming, format, &interval, buffer, buflen);
				if(ret < 0)
					goto error;
				
				frame += format->nframes;
				format++;
				
				buflen -= ret;
				buffer += ret;
				continue;

			default:
			break;
		}

		buflen -= buffer[0];
		buffer += buffer[0];
	}
	
	if(buflen)
		print_uvc(PRINT_ERROR, "device %d videostreaming interface %d has %u bytes of trailing descriptor garbage.", dev->udev->devnum, alts->desc.bInterfaceNumber, buflen);

	/* Parse the alternate setting to find the maximum bandwidth. */
	print_uvc(PRINT_DEBUG, "num_altstting=%d", intf->num_altsetting);
	for(i = 0; i < intf->num_altsetting; i++){
		struct usb_host_endpoint *ep;
		alts = &intf->altsetting[i];
		ep = uvc_find_endpoint(alts, streaming->header.bEndpointAddress);
		if(ep == NULL)
			continue;
		
		psize = le16_to_cpu(ep->desc.wMaxPacketSize);
		print_uvc(PRINT_DEBUG, "psize=%x		%x", psize, (psize & 0x07ff) * (1 + ((psize >> 11) & 3)));
		psize = (psize & 0x07ff) * (1 + ((psize >> 11) & 3));
		if(psize > streaming->maxpsize)
			streaming->maxpsize = psize;
		print_uvc(PRINT_DEBUG, "streaming->maxpsize=%d", streaming->maxpsize);
	}
	
	list_add_tail(&streaming->list, &dev->streams);
	
	return 0;

error:
	usb_driver_release_interface(&uvc_driver, intf);
	uvc_stream_delete(streaming);
	return ret;
}

static struct uvc_entity *uvc_alloc_entity(u16 type, u8 id, unsigned int num_pads, unsigned int extra_size)
{
	struct uvc_entity *entity;
	unsigned int num_inputs, size;
	u32 i;
	
	extra_size = roundup(extra_size, sizeof(*entity->pads));		//大于x并接近x的可以整除y的那个数
	num_inputs = (type & UVC_TERM_OUTPUT) ? num_pads : num_pads - 1;
	size = sizeof(*entity) + extra_size + sizeof(*entity->pads) * num_pads + num_inputs;
	
	print_uvc(PRINT_DEBUG, "extra_size = %d, num_inputs = %d, size = %d", extra_size, num_inputs, size);
	
	entity = kzalloc(size, GFP_KERNEL);
	if(entity == NULL)
		return NULL;
	
	entity->id = id;
	entity->type = type;
	
	entity->num_links = 0;
	entity->num_pads = num_pads;
	entity->pads = ((void *)(entity + 1)) + extra_size;				//保存在数据最后面
	
	for(i = 0; i < num_inputs; i++)
		entity->pads[i].flags = MEDIA_PAD_FL_SINK;
	if(!UVC_ENTITY_IS_OTERM(entity))
		entity->pads[num_pads-1].flags = MEDIA_PAD_FL_SOURCE;
	
	entity->bNrInPins = num_inputs;
	entity->baSourceID = (u8 *)(&entity->pads[num_pads]);
	
	return entity;
}

static int uvc_parse_standard_control(struct uvc_device *dev, const unsigned char *buffer, int buflen)
{
	struct usb_interface *intf;
	struct uvc_entity *unit, *term;
	struct usb_device *udev = dev->udev;
	struct usb_host_interface *alts = dev->intf->cur_altsetting;
	u32 n, p, len, i;
	u16 type;

	print_uvc(PRINT_DEBUG, "dev->intf->num_altsetting=%d    buflen = %d, %d %d %d %d %d %d %d %d", dev->intf->num_altsetting, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[11], buffer[12], buffer[13]);

	switch(buffer[2]){
		case UVC_VC_HEADER:											//第一个VideoControl Interface Descriptor
			n = buflen >= 12 ? buffer[11] : 0;						//bInCollection 表示有几个流接口描述符
			print_uvc(PRINT_DEBUG, "UVC_VC_HEADER, n=%d", n);

			if(buflen < 12 + n){
				print_uvc(PRINT_ERROR, "device %d videocontrol interface %d HEADER error", udev->devnum, alts->desc.bInterfaceNumber);
				return -EINVAL;
			}

			dev->uvc_version = get_unaligned_le16(&buffer[3]);		//buffer[3] | buffer[4] << 8,	1.00
			dev->clock_frequency = get_unaligned_le32(&buffer[7]);	//buffer[7] | buffer[8] << 8 | buffer[9] << 16 | buffer[10] << 24, 30.000000
			print_uvc(PRINT_DEBUG, "dev->uvc_version=%d dev->clock_frequency=%d", dev->uvc_version, dev->clock_frequency);

			/* parse all usb video streaming interfaces */
			for(i = 0; i < n; i++){
				intf = usb_ifnum_to_if(udev, buffer[12+i]);			//第二个Interface Descriptor，streaming

				if(intf == NULL){
					print_uvc(PRINT_ERROR, "device %d interface %d doesn't exists.", udev->devnum, i);
					continue;
				}
				uvc_parse_streaming(dev, intf);
			}
		break;
		
		case UVC_VC_INPUT_TERMINAL:
			print_uvc(PRINT_DEBUG, "buflen=%d  %d, %d %d %d %d %d %d %d %d", buflen, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8]);
			print_uvc(PRINT_DEBUG, "1111111111  %d %d %d %d %d",  buffer[12], buffer[13], buffer[14], buffer[15], buffer[16]);
			if(buflen < 8){
				print_uvc(PRINT_ERROR, "device %d videocontrol interface %d INPUT_TERMINAL error", udev->devnum, alts->desc.bInterfaceNumber);
				return -EINVAL;
			}
			
			type = get_unaligned_le16(&buffer[4]);
			if ((type & 0x7f00) == 0 || (type & 0x8000) != 0) {
				print_uvc(PRINT_ERROR, "device %d videocontrol "
					"interface %d INPUT_TERMINAL %d has invalid "
					"type 0x%04x, skipping\n", udev->devnum,
					alts->desc.bInterfaceNumber,
					buffer[3], type);
				return 0;
			}
			
			n = 0;
			p = 0;
			len = 8;
			
			if(type == UVC_ITT_CAMERA){
				n = buflen >= 15 ? buffer[14] : 0;
				len = 15;
			}
			
			if(buflen < len + n + p){
				print_uvc(PRINT_ERROR, "device %d videocontrol interface %d INPUT_TERMINAL error", udev->devnum, alts->desc.bInterfaceNumber);
				return -EINVAL;
			}
			
			term = uvc_alloc_entity(type | UVC_TERM_INPUT, buffer[3], 1, n + p);
			if(term == NULL)
				return -ENOMEM;
			
			if(UVC_ENTITY_TYPE(term) == UVC_ITT_CAMERA){
				term->camera.bControlSize = n;
				term->camera.bmControls = (u8 *)term + sizeof(*term);
				term->camera.wObjectiveFocalLengthMin = get_unaligned_le16(&buffer[8]);
				term->camera.wObjectiveFocalLengthMax = get_unaligned_le16(&buffer[10]);
				term->camera.wOcularFocalLength = get_unaligned_le16(&buffer[12]);
				memcpy(term->camera.bmControls, &buffer[15], n);
			}
			
			if(UVC_ENTITY_TYPE(term) == UVC_ITT_CAMERA){
				sprintf(term->name, "Camera %u", buffer[3]);
			}
			print_uvc(PRINT_DEBUG, "term->name=%s.", term->name);
			list_add_tail(&term->list, &dev->entities);
		break;
		
		case UVC_VC_OUTPUT_TERMINAL:
			if(buflen < 9){
				print_uvc(PRINT_ERROR, "device %d videocontrol interface %d OUTPUT_TERMINAL error", udev->devnum, alts->desc.bInterfaceNumber);
				return -EINVAL;
			}

			type = get_unaligned_le16(&buffer[4]);
			if ((type & 0xff00) == 0) {
				print_uvc(PRINT_ERROR, "device %d videocontrol "
					"interface %d OUTPUT_TERMINAL %d has invalid "
					"type 0x%04x, skipping\n", udev->devnum,
					alts->desc.bInterfaceNumber, buffer[3], type);
				return 0;
			}
			print_uvc(PRINT_DEBUG, "UVC_VC_OUTPUT_TERMINAL type=0x%x.", type | UVC_TERM_OUTPUT);
			term = uvc_alloc_entity(type | UVC_TERM_OUTPUT, buffer[3], 1, 0);
			if(term == NULL)
				return -ENOMEM;
			
			memcpy(term->baSourceID, &buffer[7], 1);
			
			sprintf(term->name, "Output %u", buffer[3]);
			print_uvc(PRINT_DEBUG, "term->name=%s.", term->name);
			list_add_tail(&term->list, &dev->entities);
		break;
		
		case UVC_VC_PROCESSING_UNIT:
			n = buflen >= 8 ? buffer[7] : 0;
			p = dev->uvc_version >= 0x0110 ? 10 : 9;
			
			if(buflen < n + p){
				print_uvc(PRINT_ERROR, "device %d videocontrol interface %d PROCESSING_UNIT error", udev->devnum, alts->desc.bInterfaceNumber);
				return -EINVAL;
			}
			
			unit = uvc_alloc_entity(buffer[2], buffer[3], 2, n);
			if(unit == NULL)
				return -ENOMEM;
			
			memcpy(unit->baSourceID, &buffer[4], 1);
			unit->processing.wMaxMultiplier = get_unaligned_le16(&buffer[5]);
			unit->processing.bControlSize = buffer[7];
			unit->processing.bmControls = (u8 *)unit + sizeof(*unit);
			memcpy(unit->processing.bmControls, &buffer[8], n);
			
			print_uvc(PRINT_DEBUG, "dev->uvc_version=%x. buffer[8+n]=%d", dev->uvc_version, buffer[8+n]);

			sprintf(unit->name, "Processing %u", buffer[3]);
			print_uvc(PRINT_DEBUG, "unit->name=%s.", unit->name);
		
			list_add_tail(&unit->list, &dev->entities);
		break;
		
		case UVC_VC_EXTENSION_UNIT:
			p = buflen >= 22 ? buffer[21] : 0;
			n = buflen >= 24 + p ? buffer[22+p] : 0;
			
			if(buflen < 24 + n + p){
				print_uvc(PRINT_ERROR, "device %d videocontrol interface %d EXTENSION_UNIT error", udev->devnum, alts->desc.bInterfaceNumber);
				return -EINVAL;
			}
			
			unit = uvc_alloc_entity(buffer[2],buffer[3], p + 1, n);
			if(unit == NULL)
				return -ENOMEM;
				
			memcpy(unit->extension.guidExtensionCode, &buffer[4], 16);
			unit->extension.bNumControls = buffer[20];
			memcpy(unit->baSourceID, &buffer[22], p);
			unit->extension.bControlSize = buffer[22+p];
			unit->extension.bmControls = (u8 *)unit + sizeof(*unit);
			memcpy(unit->extension.bmControls, &buffer[23+p], n);
			
			print_uvc(PRINT_DEBUG, "buffer[23+p+n]=%d", buffer[23+p+n]);
			
			sprintf(unit->name, "Extension %u", buffer[3]);
			print_uvc(PRINT_DEBUG, "unit->name=%s.", unit->name);
			
			list_add_tail(&unit->list, &dev->entities);
		break;
		
		default:
			print_uvc(PRINT_ERROR, "Found an unknow CS_INTERFACE descriptor (%u)", buffer[2]);
		break;
	}

	return 0;
}

static int uvc_parse_control(struct uvc_device *dev)
{
	struct usb_host_interface *alts = dev->intf->cur_altsetting;
	unsigned char *buffer = alts->extra;							//得到的是第一个接口描述符里具体的描述信息(除去接口描述符本身的描述和端点描述)
	int buflen = alts->extralen;
	int ret;

	print_uvc(PRINT_DEBUG, "buflen = %d", buflen);

	while(buflen > 2){
		if((ret = uvc_parse_standard_control(dev, buffer, buflen)) < 0)
			return ret;

		buflen -= buffer[0];
		buffer += buffer[0];
	}
	
	if (alts->desc.bNumEndpoints == 1 && !(dev->quirks & UVC_QUIRK_BUILTIN_ISIGHT))
	{
		struct usb_host_endpoint *ep = &alts->endpoint[0];
		struct usb_endpoint_descriptor *desc = &ep->desc;
		
		if(usb_endpoint_is_int_in(desc) && le16_to_cpu(desc->wMaxPacketSize) >= 8 && desc->bInterval != 0){
			print_uvc(PRINT_INFO, "Found a VideoControl endpoint (addr %02x).", desc->bEndpointAddress);
			dev->int_ep = ep;
		}
	}
	
	return 0;
}

static struct uvc_video_chain *uvc_alloc_chain(struct uvc_device *dev)
{
	struct uvc_video_chain *chain;
	
	chain = kzalloc(sizeof(*chain), GFP_KERNEL);
	if(chain == NULL)
		return NULL;

	INIT_LIST_HEAD(&chain->entities);
	mutex_init(&chain->ctrl_mutex);
	chain->dev = dev;
	v4l2_prio_init(&chain->prio);

	return chain;
}

static int uvc_scan_chain_entity(struct uvc_video_chain *chain, struct uvc_entity *entity)
{
	print_uvc(PRINT_DEBUG, "UVC_ENTITY_TYPE=0x%x", UVC_ENTITY_TYPE(entity));
	
	switch(UVC_ENTITY_TYPE(entity)){
		case UVC_VC_EXTENSION_UNIT:
			if(uvc_trace_param & PRINT_INFO)
				print_uvc(PRINT_DEBUG, "<- XU %d", entity->id);
			
			if(entity->bNrInPins != 1){
				print_uvc(PRINT_DEBUG, "Extension unit %d has more "
					"than 1 input pin.", entity->id);
				return -1;
			}
			break;
		case UVC_VC_PROCESSING_UNIT:
			if (uvc_trace_param & PRINT_INFO)
				print_uvc(PRINT_DEBUG, "<- PU %d", entity->id);
	
			if (chain->processing != NULL) {
				print_uvc(PRINT_ERROR, "Found multiple "
					"Processing Units in chain.");
				return -1;
			}
	
			chain->processing = entity;
		break;
		case UVC_ITT_CAMERA:
			if (uvc_trace_param & PRINT_INFO)
				print_uvc(PRINT_DEBUG, "<- IT %d", entity->id);
	
			break;
		case UVC_TT_STREAMING:
			if(UVC_ENTITY_IS_ITERM(entity)){
				if(uvc_trace_param & PRINT_INFO)
					print_uvc(PRINT_DEBUG, "<- IT %d", entity->id);
			}else{
				if(uvc_trace_param & PRINT_INFO)
					print_uvc(PRINT_DEBUG, "OT %d", entity->id);
			}
			break;
		default:
			print_uvc(PRINT_ERROR, "Unsupported entity type "
				"0x%04x found in chain.", UVC_ENTITY_TYPE(entity));
		return -1;
	};

	list_add_tail(&entity->chain, &chain->entities);			//所有的控制entity放在chain链表中
	return 0;
}

static struct uvc_entity *uvc_entity_by_reference(struct uvc_device *dev, int id, struct uvc_entity *entity)
{
	unsigned int i;

	if(entity == NULL)
		entity = list_entry(&dev->entities, struct uvc_entity, list);			//取链表中第一个

	list_for_each_entry_continue(entity, &dev->entities, list){
		for(i = 0; i < entity->bNrInPins; ++i)
			if(entity->baSourceID[i] == id)
				return entity;
	}

	return NULL;
}

static int uvc_scan_chain_forward(struct uvc_video_chain *chain, struct uvc_entity *entity, struct uvc_entity *prev)
{
	struct uvc_entity *forward;
	int found;
	
	print_uvc(PRINT_DEBUG, "enter");
	
	/* Forward scan */
	forward = NULL;
	found = 0;

	while(1){
		forward = uvc_entity_by_reference(chain->dev, entity->id, forward);
		if(forward == NULL)
			break;
		if(forward == prev)
			continue;

		print_uvc(PRINT_DEBUG, "UVC_ENTITY_TYPE(forward)=0x%x", UVC_ENTITY_TYPE(forward));
		
		//switch(UVC_ENTITY_TYPE(forward)){
		//	
		//}
	}

	return 0;
}

struct uvc_entity *uvc_entity_by_id(struct uvc_device *dev, int id)
{
	struct uvc_entity *entity;

	list_for_each_entry(entity, &dev->entities, list){
		if(entity->id == id)
			return entity;
	}

	return NULL;
}

static int uvc_scan_chain_backward(struct uvc_video_chain *chain, struct uvc_entity **_entity)
{
	struct uvc_entity *entity = *_entity;
	//struct uvc_entity *term;
	int id = -EINVAL;//, i;

	print_uvc(PRINT_DEBUG, "UVC_ENTITY_TYPE=%x", UVC_ENTITY_TYPE(entity));
	
	switch(UVC_ENTITY_TYPE(entity)){
		case UVC_VC_EXTENSION_UNIT:
		case UVC_VC_PROCESSING_UNIT:
			id = entity->baSourceID[0];
			break;
		case UVC_ITT_CAMERA:
		case UVC_TT_STREAMING:
			id = UVC_ENTITY_IS_OTERM(entity) ? entity->baSourceID[0] : 0;
			break;
	}
	print_uvc(PRINT_DEBUG, "id=%d", id);
	if(id <= 0){
		*_entity = NULL;
		return id;
	}

	entity = uvc_entity_by_id(chain->dev, id);
	if(entity == NULL){
		print_uvc(PRINT_ERROR, "Found reference to "
			"unknown entity %d.", id);
		return -EINVAL;
	}

	*_entity = entity;
	return 0;
}

static int uvc_scan_chain(struct uvc_video_chain *chain, struct uvc_entity *term)
{
	struct uvc_entity *entity, *prev;

	print_uvc(PRINT_DEBUG, "Scanning UVC chain, term=%s", term->name);

	entity = term;
	prev = NULL;

	while(entity != NULL){
		if(entity->chain.next || entity->chain.prev){
			print_uvc(PRINT_ERROR, "Found reference to "
				"entity %d already in chain.\n", entity->id);
			return -EINVAL;
		}
		
		print_uvc(PRINT_DEBUG, "Scanning UVC chain");
		
		/* Process entity */
		if(uvc_scan_chain_entity(chain, entity) < 0)
			return -EINVAL;

		/* Forward scan */
		if(uvc_scan_chain_forward(chain, entity, prev) < 0)
			return -EINVAL;
		
		/* Backward scan */
		prev = entity;
		if(uvc_scan_chain_backward(chain, &entity) < 0)
			return -EINVAL;
	}

	return 0;
}

static unsigned int uvc_print_terms(struct list_head *terms, u16 dir,
		char *buffer)
{
	struct uvc_entity *term;
	unsigned int nterms = 0;
	char *p = buffer;

	list_for_each_entry(term, terms, chain) {
		if (!UVC_ENTITY_IS_TERM(term) ||
		    UVC_TERM_DIRECTION(term) != dir)
			continue;

		if (nterms)
			p += sprintf(p, ",");
		if (++nterms >= 4) {
			p += sprintf(p, "...");
			break;
		}
		p += sprintf(p, "%u", term->id);
	}

	return p - buffer;
}

static const char *uvc_print_chain(struct uvc_video_chain *chain)
{
	static char buffer[43];
	char *p = buffer;

	p += uvc_print_terms(&chain->entities, UVC_TERM_INPUT, p);
	p += sprintf(p, " -> ");
	uvc_print_terms(&chain->entities, UVC_TERM_OUTPUT, p);

	return buffer;
}

/* Scan the device for video chains and register video deivces.
 * Chains are scanned starting at their output terminals and walked backwards.
 */
static int uvc_scan_device(struct uvc_device *dev)
{
	struct uvc_video_chain  *chain;
	struct uvc_entity *term;

	list_for_each_entry(term, &dev->entities, list){
		if(!UVC_ENTITY_IS_OTERM(term))												//找到 OUTPUT_TERMINAL 的接口描述符
			continue;
		if(term->chain.next || term->chain.prev)
			continue;

		chain = uvc_alloc_chain(dev);
		if(chain == NULL)
			return -ENOMEM;
		
		term->flags |= UVC_ENTITY_FLAG_DEFAULT;

		if(uvc_scan_chain(chain, term) < 0){
			kfree(chain);
			continue;
		}

		print_uvc(PRINT_INFO, "Found a valid video chain (%s).",
			  uvc_print_chain(chain));
		
		list_add_tail(&chain->list, &dev->chains);
	}

	//if(list_empty(&dev->chains))
	//	uvc_scan_fallback(dev);
	
	if(list_empty(&dev->chains)){
		print_uvc(KERN_INFO, "No valid video chain found.");
		return -1;
	}

	return 0;
}

static struct uvc_streaming *uvc_stream_by_id(struct uvc_device *dev, int id)
{
	struct uvc_streaming *stream;
	
	list_for_each_entry(stream, &dev->streams, list){
		if(stream->header.bTerminalLink == id)
			return stream;
	}
	
	return NULL;
}



/* Delete the UVC device. */
static void uvc_delete(struct kref *kref)
{
	struct uvc_device *dev = container_of(kref, struct uvc_device, ref);
	struct list_head *p, *n;
	
	uvc_status_cleanup(dev);
	uvc_ctrl_cleanup_device(dev);
	
	usb_put_intf(dev->intf);
	usb_put_dev(dev->udev);
	
#ifdef CONFIG_MEDIA_CONTROLLER
	media_device_cleanup(&dev->mdev);
#endif
	
	list_for_each_safe(p, n, &dev->chains){
		struct uvc_video_chain *chain;
		chain = list_entry(p, struct uvc_video_chain, list);
		kfree(chain);
	}
	
	list_for_each_safe(p, n, &dev->entities){
		struct uvc_entity *entity;
		entity = list_entry(p, struct uvc_entity, list);
#ifdef CONFIG_MEDIA_CONTROLLER
		if(UVC_ENTITY_TYPE(entity) != UVC_TT_STREAMING)
			;//media_entity_cleanup(&entity->subdev.entity);
		else if(entity->vdev != NULL)
			media_entity_cleanup(&entity->vdev->entity);
#endif
		kfree(entity);
	}
	
	list_for_each_safe(p, n, &dev->streams){
		struct uvc_streaming *streaming;
		streaming = list_entry(p, struct uvc_streaming, list);
		usb_driver_release_interface(&uvc_driver, streaming->intf);
		uvc_stream_delete(streaming);
	}

	kfree(dev);
}

static void uvc_release(struct video_device *vdev)
{
	struct uvc_streaming *stream = video_get_drvdata(vdev);
	struct uvc_device *dev = stream->dev;
	
	kref_put(&dev->ref, uvc_delete);
}

static void uvc_unregister_video(struct uvc_device *dev)
{
	struct uvc_streaming *stream;
	
	list_for_each_entry(stream, &dev->streams, list){
		if(!video_is_registered(&stream->vdev))
			continue;
		
		video_unregister_device(&stream->vdev);
		video_unregister_device(&stream->meta.vdev);
		
		//uvc_debugfs_cleanup_steam(stream);
	}
	
	if(dev->vdev.dev)
		v4l2_device_unregister(&dev->vdev);
#ifdef CONFIG_MEDIA_CONTROLLER
	if(media_devnode_is_registered(dev->mdev.devnode))
		media_device_unregister(&dev->mdev);
#endif	
}

int uvc_register_video_device(struct uvc_device *dev, struct uvc_streaming *stream,
								struct video_device *vdev,
								struct uvc_video_queue *queue,
								enum v4l2_buf_type type,
								const struct v4l2_file_operations *fops,
								const struct v4l2_ioctl_ops *ioctl_ops)
{
	int ret;
	
	/* Initialize the video buffer queue. */
	queue->queue.dev = &stream->vdev.dev;
	ret = uvc_queue_init(queue, type, 1);

	ret = dma_coerce_mask_and_coherent(&vdev->dev, DMA_BIT_MASK(32));						//设置设备通过DMA方式可寻址的物理地址范围
	if(ret){
		print_uvc(PRINT_ERROR, "dma_coerce_mask_and_coherent fail.");
		return ret;
	}
	
	/* Register the device with V4L. */
	vdev->v4l2_dev = &dev->vdev;
	vdev->fops = fops;
	vdev->ioctl_ops = ioctl_ops;
	vdev->release = uvc_release;
	vdev->prio = &stream->chain->prio;
	if(type == V4L2_BUF_TYPE_VIDEO_OUTPUT)
		vdev->vfl_dir = VFL_DIR_TX;
	else
		vdev->vfl_dir = VFL_DIR_RX;
	
	switch(type){
		case V4L2_BUF_TYPE_VIDEO_CAPTURE:
		default:
			vdev->device_caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING;
		break;
		
		case V4L2_BUF_TYPE_VIDEO_OUTPUT:
			vdev->device_caps = V4L2_CAP_VIDEO_OUTPUT | V4L2_CAP_STREAMING;
		break;
		
		case V4L2_BUF_TYPE_META_CAPTURE:
			vdev->device_caps = V4L2_CAP_META_CAPTURE | V4L2_CAP_STREAMING;
		break;
	}
	
	strscpy(vdev->name, dev->name, sizeof(vdev->name));
	
	video_set_drvdata(vdev, stream);
	
	
	ret = video_register_device(vdev, VFL_TYPE_GRABBER, -1);
	if(ret < 0){
		print_uvc(PRINT_ERROR, "Failed to register %s device (%d).", v4l2_type_names[type], ret);
		return ret;
	}
	
	kref_get(&dev->ref);
	return 0;
}

static int uvc_register_video(struct uvc_device *dev, struct uvc_streaming *stream)
{
	int ret;
	
	/*Initialize the streaming interface with default parameters.*/
	ret = uvc_video_init(stream);
	if(ret < 0){
		print_uvc(PRINT_ERROR, "Failed to initialize the device (%d).", ret);
		return ret;
	}
	
	if(stream->type == V4L2_BUF_TYPE_VIDEO_CAPTURE)
		stream->chain->caps |= V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_META_CAPTURE;
	
	
	/* Register the device with V4L2. */
	return uvc_register_video_device(dev, stream, &stream->vdev, &stream->queue,
									stream->type, &uvc_fops, &uvc_ioctl_ops);
	
	return 0;
}

/* Register all video devices in all chains */
static int uvc_register_terms(struct uvc_device *dev, struct uvc_video_chain *chain)
{
	struct uvc_streaming *stream;
	struct uvc_entity *term;
	int ret;

	list_for_each_entry(term, &chain->entities, chain){
		if(UVC_ENTITY_TYPE(term) != UVC_TT_STREAMING)			//选出 OUTPUT_TERMINAL
			continue;

		stream = uvc_stream_by_id(dev, term->id);				//通过OUTPUT_TERMINAL的 bTerminalID ，找到stream中INPUT_HEADER对应的 bTerminalLink
		if(stream == NULL){
			print_uvc(PRINT_ERROR, "No streaming interface found for terminal %u", term->id);
			continue;
		}
		
		stream->chain = chain;
		ret = uvc_register_video(dev, stream);
		if(ret < 0)
			return ret;
		
		term->vdev = &stream->vdev;
	}

	return 0;
}

static int uvc_register_chains(struct uvc_device *dev)
{
	struct uvc_video_chain *chain;
	int ret;

	list_for_each_entry(chain, &dev->chains, list){
		ret = uvc_register_terms(dev, chain);
		if(ret < 0)
			return ret;
	}

	return 0;
}

static const struct uvc_device_info uvc_quirk_none = { 0 };

static int uvc_probe(struct usb_interface *intf, const struct usb_device_id *id)	//intf为第一个“Interface Descriptor”， id为probe时的 id_table
{
	struct usb_device *udev = interface_to_usbdev(intf);
	struct uvc_device *dev;
	const struct uvc_device_info *info = (const struct uvc_device_info *)id->driver_info;

	print_uvc(PRINT_INFO, "device %d, Probing generic UVC device %s", udev->devnum, udev->devpath);

	/* Allocate memory for the device and initialize it. */
	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if(dev == NULL)
		return -ENOMEM;
	
	INIT_LIST_HEAD(&dev->entities);
	INIT_LIST_HEAD(&dev->streams);
	INIT_LIST_HEAD(&dev->chains);
	kref_init(&dev->ref);
	
	mutex_init(&dev->lock);
	
	dev->udev = usb_get_dev(udev);
	dev->intf = usb_get_intf(intf);									//第一个接口描述符(控制描述符)
	dev->intfnum = intf->cur_altsetting->desc.bInterfaceNumber;		//当前的接口描述符有几个接口(一般第一个都为控制描述符，且只有一个)
	dev->info = info ? info : &uvc_quirk_none;						//uvc_quirk_none
	dev->quirks = uvc_quirks_param == -1 ? dev->info->quirks : uvc_quirks_param;
	
	//if(udev->product != NULL)
	//	strscpy(dev->name, udev->product, sizeof(dev->name));
	//else
		snprintf(dev->name, sizeof(dev->name),
			"UVC Camera (%04x:%04x)",
			le16_to_cpu(udev->descriptor.idVendor),
			le16_to_cpu(udev->descriptor.idProduct));
			
	print_uvc(PRINT_DEBUG, "dev->udev=%s, dev->intf=%s, dev->name=%s, dev->intfnum=%d, dev->quirks=%d, dev->info->quirks =%d", udev->dev.kobj.name, intf->dev.kobj.name, dev->name, dev->intfnum, dev->quirks, dev->info->quirks);
	
	/* Parse the Video Class control descriptor. */
	if(uvc_parse_control(dev) < 0){
		print_uvc(PRINT_ERROR, "Unable to parse UVC descriptor.");
		goto error;
	}
	
	print_uvc(PRINT_INFO, "Found UVC %u.%02x device %s (%04x:%04x)",
				dev->uvc_version >> 8, dev->uvc_version & 0xff,
				udev->product ? udev->product : "<unnamed>",
				le16_to_cpu(udev->descriptor.idVendor),
				le16_to_cpu(udev->descriptor.idProduct));

	/* Initialize the media device and register the V4L2 device. */
#ifdef CONFIG_MEDIA_CONTROLLER
	dev->mdev.dev = &intf->dev;
	strscpy(dev->mdev.model, dev->name, sizeof(dev->mdev.model));
	if(udev->serial)
		strscpy(dev->mdev.serial, udev->serial, sizeof(dev->mdev.serial));
	usb_make_path(udev, dev->mdev.bus_info, sizeof(dev->mdev.bus_info));
	dev->mdev.hw_revision = le16_to_cpu(udev->descriptor.bcdDevice);

	print_uvc(PRINT_DEBUG, "dev->mdev.model=%s dev->mdev.serial=%s", dev->mdev.model, dev->mdev.serial);
	
	media_device_init(&dev->mdev);
	dev->vdev.mdev = &dev->mdev;
#endif
	if(v4l2_device_register(&intf->dev, &dev->vdev) < 0)
		goto error;

	/* Initialize controls */
	if(uvc_ctrl_init_device(dev) < 0)
		goto error;
	
	/* Scan the device for video chains */
	if(uvc_scan_device(dev) < 0)
		goto error;

	/* Register video device nodes. */
	if(uvc_register_chains(dev) < 0)
		goto error;
	
	/* Save our data pointer in the interface data. */
	usb_set_intfdata(intf, dev);

	
	
	
	print_uvc(PRINT_INFO, "UVC device initialized..");
	
	return 0;

	
	
error:
	uvc_unregister_video(dev);
	kref_put(&dev->ref, uvc_delete);
	return -ENODEV;
}

static void uvc_disconnect(struct usb_interface *intf)
{
	struct uvc_device *dev = usb_get_intfdata(intf);
	
	print_uvc(PRINT_INFO, "disconnect.");
	
	usb_set_intfdata(intf, NULL);
	
	if(intf->cur_altsetting->desc.bInterfaceSubClass == UVC_SC_VIDEOSTREAMING)
		return;
	
	uvc_unregister_video(dev);
	kref_put(&dev->ref, uvc_delete);
}

static const struct usb_device_id uvc_id[] = {								//Interface Association
	{ 	.match_flags		= USB_DEVICE_ID_MATCH_INT_INFO,					//需要匹配哪些信息做标记，下面设置需要匹配的信息
		.bInterfaceClass	= USB_CLASS_VIDEO,								//14(0xe) Video
		
		.bInterfaceSubClass	= 1,											//probe第一个“Interface Descriptor”
		.bInterfaceProtocol	= 0,
	},
	{}
};

MODULE_DEVICE_TABLE(usb, uvc_id);

struct usb_driver uvc_driver = {
	.name		= "uvcvideo",
	.probe		= uvc_probe,												//根据每个接口描述符的信息probe(usb camera设备先是一个设备描述符 ==>  一个配置描述符 ==>  接口描述信息 ==>  接口描述符信息)
	.disconnect	= uvc_disconnect,
	//.suspend	= uvc_suspend,
	//.resume		= uvc_resume,
	//.reset_resume	=uvc_reset_resume,
	.id_table	= uvc_id,
	.supports_autosuspend = 1,
};

static int uvc_camera_init(void)
{
	int ret;
	print_uvc(PRINT_DEBUG,"insmod.");

	uvc_debugfs_init();

	ret = usb_register(&uvc_driver);
	if(ret < 0){
		uvc_debugfs_clean();
		return ret;
	}

	//print_uvc(PRINT_INFO,"USB video class driver init success.");

	return 0;
}

static void uvc_camera_exit(void)
{
	//print_uvc(PRINT_DEBUG,"rmmod.");
	usb_deregister(&uvc_driver);
	uvc_debugfs_clean();
}

MODULE_LICENSE("GPL");
module_init(uvc_camera_init);
module_exit(uvc_camera_exit);