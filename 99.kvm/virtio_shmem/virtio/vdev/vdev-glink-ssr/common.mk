# This is an automatically generated record.
# The area between QNX Internal Start and QNX Internal End is controlled by
# the QNX IDE properties.

ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)
include $(AMSS_ROOT)/amss_defs_af.mk

#===== USEFILE - the file containing the usage message for the application. 
USEFILE=$(PROJECT_ROOT)/vdev.use

#===== PINFO - the file containing the packaging information for the application. 
define PINFO
PINFO DESCRIPTION=glink ssr
endef

#===== EXTRA_SRCVPATH - a space-separated list of directories to search for source files.
EXTRA_SRCVPATH+=$(PROJECT_ROOT)/src

#===== EXTRA_INCVPATH - a space-separated list of directories to search for include files.
INCVPATH+=$(PROJECT_ROOT)/inc  \
          $(PROJECT_ROOT)/protected \
          $(MULTIMEDIA_INC) \
          $(INSTALL_ROOT_nto)/usr/include/amss/core \
          $(INSTALL_ROOT_nto)/usr/include/qct \
          $(AMSS_INC) \
          $(AMSS_INC)/amss/core \
		  $(BSP_ROOT)/AMSS/platform/utilities/fdt_utils\

#===== CCFLAGS - add the flags to the C compiler command line.
CCFLAGS+=-DWLAN_SSR

LIBS+=fdt_utilsS ssrS ppsS
include $(MKFILES_ROOT)/qmacros.mk
include $(QNX_INTERNAL)
include $(MKFILES_ROOT)/qtargets.mk

OPTIMIZE_TYPE_g=none
OPTIMIZE_TYPE=$(OPTIMIZE_TYPE_$(filter g, $(VARIANTS)))

