#include "uvc_camera.h"










void uvc_status_cleanup(struct uvc_device *dev)
{
	usb_free_urb(dev->int_urb);
	//kfree(dev->status);
}

int uvc_status_start(struct uvc_device *dev, gfp_t flags)
{
	if(dev->int_urb == NULL)
		return 0;
	
	return usb_submit_urb(dev->int_urb, flags);
}

void uvc_status_stop(struct uvc_device *dev)
{
	usb_kill_urb(dev->int_urb);
}

