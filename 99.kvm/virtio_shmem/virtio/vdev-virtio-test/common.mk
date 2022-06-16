# This is an automatically generated record.
# The area between QNX Internal Start and QNX Internal End is controlled by
# the QNX IDE properties.

ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)
include $(AMSS_ROOT)/amss_defs_af.mk

#===== USEFILE - the file containing the usage message for the application.
USEFILE=$(PROJECT_ROOT)/vdev-virtio-test.use

#===== PINFO - the file containing the packaging information for the application.
define PINFO
PINFO DESCRIPTION=vdev-i2c traps and emulate i2c controller
endef

#===== EXTRA_SRCVPATH - a space-separated list of directories to search for source files.
EXTRA_SRCVPATH+=$(PROJECT_ROOT)/src

#===== CCFLAGS - add the flags to the C compiler command line.
CCFLAGS+=-DFEATURE_LIBRARY_ONLY -fvisibility=hidden -Wno-unused

#===== EXTRA_INCVPATH - a space-separated list of directories to search for include files.
INCVPATH+=$(PROJECT_ROOT)/inc  \
          $(PROJECT_ROOT)/protected \
          $(MULTIMEDIA_INC) \
          $(INSTALL_ROOT_nto)/usr/include/amss/core \
          $(INSTALL_ROOT_nto)/usr/include/qct \
          $(AMSS_INC) \
          $(AMSS_INC)/amss/core \
		  $(AMSS_INC)/../../target/hypervisor/qvm/prebuilt/usr/include/

#===== INSTALLDIR - Subdirectory where the executable or library is to be installed.
INSTALLDIR=$(INSTALLDIR_ASIC_BF_LIB)

CCFLAGS_64 += -D_PADDR_BITS=64
CCFLAGS_gcc_ = -O2 -fomit-frame-pointer
CCFLAGS_clang_ = -O2 -fomit-frame-pointer

CCFLAGS_gcc_qcc = -O2 -Wc,-fomit-frame-pointer
CCFLAGS_$(BUILDENV) = -DBUILDENV_$(BUILDENV)
CCFLAGS += $(CCFLAGS_$(COMPILER_TYPE)_$(COMPILER_DRIVER)) $(CCFLAG_$(PADDR_SIZE)) $(CCFLAGS_$(BUILDENV))

#===== LDFLAGS - add the flags to the linker command line.
LDFLAGS_gcc_qcc = -Wl,-Map,$@.map
LDFLAGS = $(LDFLAGS_$(COMPILER_TYPE)_$(COMPILER_DRIVER))

ifeq ($(CPULIST),aarch64)
#LIBS+=
else
#LIBS+=
endif
LIBS+= slog2 hypS i2c_client

#===== NAME - name of the project (default - name of project directory).
NAME=

include $(MKFILES_ROOT)/qmacros.mk
ifndef QNX_INTERNAL
#QNX_INTERNAL=$(PROJECT_ROOT)/.qnx_internal.mk
endif
include $(QNX_INTERNAL)
include $(MKFILES_ROOT)/qtargets.mk

OPTIMIZE_TYPE_g=none
OPTIMIZE_TYPE=$(OPTIMIZE_TYPE_$(filter g, $(VARIANTS)))
