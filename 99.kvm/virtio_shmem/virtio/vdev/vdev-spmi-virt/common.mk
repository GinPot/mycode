# This is an automatically generated record.
# The area between QNX Internal Start and QNX Internal End is controlled by
# the QNX IDE properties.

ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)
include $(AMSS_ROOT)/amss_defs_af.mk

#===== USEFILE - the file containing the usage message for the application. 
USEFILE=$(PROJECT_ROOT)/vdev-spmi-virt.use

#===== PINFO - the file containing the packaging information for the application. 
define PINFO
PINFO DESCRIPTION=Trap emulates SPMI command register
endef

#===== EXTRA_SRCVPATH - a space-separated list of directories to search for source files.
EXTRA_SRCVPATH+=$(PROJECT_ROOT)/src

EXTRA_INCVPATH+= $(INSTALL_ROOT_nto)/usr/include/amss/core \

#===== CCFLAGS - add the flags to the C compiler command line. 
CCFLAGS+=-DFEATURE_LIBRARY_ONLY -fvisibility=hidden -Wno-unused


#===== EXTRA_INCVPATH - a space-separated list of directories to search for include files.
INCVPATH+=$(PROJECT_ROOT)/inc  \
          $(PROJECT_ROOT)/protected \
          $(MULTIMEDIA_INC) \
          $(INSTALL_ROOT_nto)/usr/include/amss/core \
          $(INSTALL_ROOT_nto)/usr/include/qct \
          $(AMSS_INC) \
          $(AMSS_INC)/core \
          $(AMSS_INC)/amss/core \

LIBS+=spmi_client fdt_utilsS vdev_utilsS adc_client

include $(MKFILES_ROOT)/qmacros.mk
ifndef QNX_INTERNAL
#QNX_INTERNAL=$(PROJECT_ROOT)/.qnx_internal.mk
endif
include $(QNX_INTERNAL)
include $(MKFILES_ROOT)/qtargets.mk

OPTIMIZE_TYPE_g=none
OPTIMIZE_TYPE=$(OPTIMIZE_TYPE_$(filter g, $(VARIANTS)))

