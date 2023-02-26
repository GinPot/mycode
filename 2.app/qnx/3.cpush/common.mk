# This is an automatically generated record.
# The area between QNX Internal Start and QNX Internal End is controlled by
# the QNX IDE properties.

ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)
include $(AMSS_ROOT)/amss_defs.mk

#===== USEFILE - the file containing the usage message for the application. 
USEFILE=$(PROJECT_ROOT)/usefile

#===== PINFO - the file containing the packaging information for the application. 
define PINFO
PINFO DESCRIPTION="Clean+Inval CPU cache by set-way"
endef

#===== EXTRA_SRCVPATH - a space-separated list of directories to search for source files.
INCVPATH+=$(AMSS_INC)

#===== EXTRA_INCVPATH - a space-separated list of directories to search for include files.
INCVPATH+=$(INSTALL_ROOT_nto)/usr/include/amss/core \

#===== NAME - name of the project (default - name of project directory).

include $(MKFILES_ROOT)/qmacros.mk
ifndef QNX_INTERNAL
QNX_INTERNAL=$(BSP_ROOT)/.qnx_internal.mk
endif
include $(QNX_INTERNAL)

include $(MKFILES_ROOT)/qtargets.mk

OPTIMIZE_TYPE_g=none
OPTIMIZE_TYPE=$(OPTIMIZE_TYPE_$(filter g, $(VARIANTS)))
