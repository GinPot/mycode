#
# Copyright  2010, QNX Software Systems Ltd.  All Rights Reserved
#
# This source code has been published by QNX Software Systems Ltd.
# (QSSL).  However, any use, reproduction, modification, distribution
# or transfer of this software, or any software which includes or is
# based upon any of this code, is only permitted under the terms of
# the QNX Open Community License version 1.0 (see licensing.qnx.com for
# details) or as otherwise expressly authorized by a written license
# agreement from QSSL.  For more information, please email licensing@qnx.com.
#
ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

#===== USEFILE - the file containing the usage message for the application.
USEFILE=$(PROJECT_ROOT)/touch_hijacker.use

define PINFO
PINFO DESCRIPTION=touch_hijacker
endef

#===== INSTALLDIR - Subdirectory where the executable or library is to be installed.
INSTALLDIR=usr/bin
ifeq ($(CPULIST),aarch64)
cpulist=aarch64le
else
cpulist=armle-v7
endif

ifeq ($(CPU), arm)
	ifneq ($(filter v7, $(VARIANT_LIST)), v7)
		GCCVER:= $(if $(GCC_VERSION), $(GCC_VERSION), $(shell qcc -V 2>&1 | grep default | sed -e 's/,.*//'))
		ifneq ($(filter 4.%, $(strip $(GCCVER))),)
			CCFLAGS += -mfpu=vfp -mfloat-abi=softfp
			LIBS += m-vfp
		else
			LIBS += m
		endif
	else
		LIBS += m
	endif
else

	LIBS += m
endif

#===== EXTRA_INCVPATH - a space-separated list of directories to search for include files.
EXTRA_INCVPATH=\
	$(PROJECT_ROOT)/../LIB_PNG/include \
	$(PROJECT_ROOT)/../LIB_PNG/include/libpng \
	$(PROJECT_ROOT)/../LIB_PNG/include/libz \
    $(PROJECT_ROOT)/../../../../../../../../../qnx_bins/prebuilt_QNX700/target/qnx7/usr/include \
    $(PROJECT_ROOT)/../../../../../../../../../qnx_bins/prebuilt_QNX700FIXME/target/qnx7/usr/include

#===== EXTRA_LIBVPATH - a space-separated list of directories to search for library files.
EXTRA_LIBVPATH=$(PROJECT_ROOT)/../LIB_PNG/arm/a-le-v7 \
	$(INSTALL_ROOT_nto)/$(cpulist)/usr/lib/graphics/qc \
	$(QNX_TARGET)/$(cpulist)/usr/lib \

#===== LIBS - a space-separated list of library items to be included in the link.
LIBS+=ESXEGL_Adreno ESXGLESv2_Adreno screen slog2 png

#===== CCFLAGS - add the flags to the C compiler command line.
CCFLAGS+=

#Set to 0 to disable frame capture and linking to libpng
#CAPTURE_FRAME := 1
#ifeq ($(CAPTURE_FRAME), 1)
#	LIBS+=^LIB_PNG
#	CCFLAGS+= -DCAPTURE_FRAME
#endif

include $(MKFILES_ROOT)/qmacros.mk
ifndef QNX_INTERNAL
##QNX_INTERNAL=$(PROJECT_ROOT)/.qnx_internal.mk
endif
include $(QNX_INTERNAL)

include $(MKFILES_ROOT)/qtargets.mk

OPTIMIZE_TYPE_g=none
OPTIMIZE_TYPE=$(OPTIMIZE_TYPE_$(filter g, $(VARIANTS)))

