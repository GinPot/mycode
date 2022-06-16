# This is an automatically generated record.
# The area between QNX Internal Start and QNX Internal End is controlled by
# the QNX IDE properties.

ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

#===== USEFILE - the file containing the usage message for the application. 
USEFILE=
define PINFO
PINFO DESCRIPTION=SPI utility for BCM8953X_spiutils
endef
CCFLAGS += -Werror

#===== INCVPATH - a space-separated list of directories to search for include files.
INCVPATH+=$(INSTALL_ROOT_nto)/usr/include/amss

#===== LIBS - a space-separated list of library items to be included in the link.
LIBS+=hyp pci

include $(MKFILES_ROOT)/qmacros.mk

#===== EXTRA_OBJS - object files and libraries contributed additionaly on link/archive step

#===== EXTRA_SRCVPATH - a space-separated list of directories to search for source files.

include $(MKFILES_ROOT)/qmacros.mk
ifndef QNX_INTERNAL
QNX_INTERNAL=$(BSP_ROOT)/.qnx_internal.mk
endif
include $(QNX_INTERNAL)

include $(MKFILES_ROOT)/qtargets.mk

OPTIMIZE_TYPE_g=none
OPTIMIZE_TYPE=$(OPTIMIZE_TYPE_$(filter g, $(VARIANTS)))

