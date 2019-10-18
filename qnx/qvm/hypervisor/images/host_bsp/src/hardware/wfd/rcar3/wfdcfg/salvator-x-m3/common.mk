ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

INSTALLDIR=usr/lib/graphics/rcarh3$(DBG_DIRSUFFIX)

INSTALL_ROOT_nto = $(PROJECT_ROOT)/../../../../../../install
EXTRA_INCVPATH+=$(INSTALL_ROOT_nto)/usr/include

include $(MKFILES_ROOT)/qmacros.mk
include $(PROJECT_ROOT)/../wfdcfg.mk

include $(MKFILES_ROOT)/qtargets.mk
include $(PROJECT_ROOT)/../usemsg.mk

INSTALLDIR=$(firstword $(INSTALLDIR_$(OS)) usr/lib/graphics/rcarm3$(DBG_DIRSUFFIX))

# Install on other wfd variant that might use the same wfdcfg lib
POST_INSTALL += $(CP_HOST) $(BUILDNAME) $(INSTALL_ROOT_DLL)/usr/lib/graphics/rcarm3n$(DBG_DIRSUFFIX)/;
POST_INSTALL += $(CP_HOST) $(BUILDNAME) $(INSTALL_ROOT_DLL)/usr/lib/graphics/rcarm3_host$(DBG_DIRSUFFIX)/;
POST_INSTALL += $(CP_HOST) $(BUILDNAME) $(INSTALL_ROOT_DLL)/usr/lib/graphics/rcarm3_guest$(DBG_DIRSUFFIX)/;
