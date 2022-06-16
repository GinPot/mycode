#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <linux/videodev2.h>

#include "ImageConfig.h"








int InitUvcCamera(struct MainInfo *maininfo);
void ExitUvcCamera(struct MainInfo *maininfo);

#endif	/* __CAMERA_H__ */
