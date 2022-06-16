#include "uvc_camera.h"












static struct dentry *uvc_debugfs_root_dir;
















void uvc_debugfs_init(void)
{
	struct dentry *dir;
	
	dir = debugfs_create_dir("uvcvideo", usb_debug_root);				//sys/kernel/debug/usb/uvcvideo/
	if(IS_ERR_OR_NULL(dir)){
		print_uvc(PRINT_ERROR, "Unable to create debugfs directory,%ld", PTR_ERR(dir));
		return;
	}

	uvc_debugfs_root_dir = dir;
}

void uvc_debugfs_clean(void)
{
	debugfs_remove_recursive(uvc_debugfs_root_dir);
}