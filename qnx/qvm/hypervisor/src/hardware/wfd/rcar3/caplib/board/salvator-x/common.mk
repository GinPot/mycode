ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

include $(MKFILES_ROOT)/qmacros.mk
include $(PROJECT_ROOT)/../board.mk
define PINFO
PINFO DESCRIPTION=Video Capture Library for Renesas Salvator-X EVM Board
endef

include $(MKFILES_ROOT)/qtargets.mk
include $(MKFILES_ROOT)/qdepfile.mk  # defines "make depends"
WIN32_ENVIRON=mingw

-include $(PROJECT_ROOT)/local.mk  # optional (for debugging)
