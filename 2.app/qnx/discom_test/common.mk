ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

define PINFO
PINFO DESCRIPTION=Hypervisor shared memory device tutorial for a host
endef

#LIBS=hyp
LIBS += screen
#LIBS += socket


#####AUTO-GENERATED by packaging script... do not checkin#####
   INSTALL_ROOT_nto = $(PROJECT_ROOT)/../../../../../install
   USE_INSTALL_ROOT=1
##############################################################

include $(MKFILES_ROOT)/qtargets.mk

