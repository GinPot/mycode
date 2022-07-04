# This is an automatically generated record.
# The area between QNX Internal Start and QNX Internal End is controlled by
# the QNX IDE properties.

ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)
#include $(AMSS_ROOT)/amss_defs_af.mk
#===== USEFILE - the file containing the usage message for the application. 
USEFILE=

#===== PINFO - the file containing the packaging information for the application. 
define PINFO
PINFO DESCRIPTION=Memory dumper tool 
endef


CCFLAGS_arm += -marm -fPIC -D_PADDR_BITS=64
CCFLAGS_gcc_ = -O2 -fomit-frame-pointer
CCFLAGS_clang_ = -O2 -fomit-frame-pointer

CCFLAGS_gcc_qcc = -O2 -Wc,-fomit-frame-pointer 
CCFLAGS_$(BUILDENV) = -DBUILDENV_$(BUILDENV)
CCFLAGS += $(CCFLAGS_$(COMPILER_TYPE)_$(COMPILER_DRIVER)) $(CCFLAG_$(PADDR_SIZE)) $(CCFLAGS_$(BUILDENV))

#===== LDFLAGS - add the flags to the linker command line.
LDFLAGS_gcc_qcc = -Map,$@.map
LDFLAGS = $(LDFLAGS_$(COMPILER_TYPE)_$(COMPILER_DRIVER))

#===== LDFLAGS - add the flags to the linker command line.
LDFLAGS+=-Wl,-E

#===== NAME - name of the project (default - name of project directory).
NAME=dmesg

INSTALLDIR=bin

include $(MKFILES_ROOT)/qmacros.mk
ifndef QNX_INTERNAL
#QNX_INTERNAL=$(PROJECT_ROOT)/.qnx_internal.mk
endif
#include $(QNX_INTERNAL)

include $(MKFILES_ROOT)/qtargets.mk
OPTIMIZE_TYPE_g=none
OPTIMIZE_TYPE=$(OPTIMIZE_TYPE_$(filter g, $(VARIANTS)))
