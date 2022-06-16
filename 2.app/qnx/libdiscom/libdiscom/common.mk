ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

INSTALLDIR=/usr/lib
include $(MKFILES_ROOT)/qmacros.mk

DECODER_VARIANTS = adv7480 adv7481 adv7482
DECODER_VARIANT_NAME=$(filter $(VARIANT_LIST), $(DECODER_VARIANTS))

NAME=$(IMAGE_PREF_SO)

define PINFO
PINFO DESCRIPTION = QNX video capture driver for $(DECODER_VARIANT_NAME) decoder
endef

INSTALL_ROOT_nto = $(PROJECT_ROOT)/../../../../install

LDOPTS+=-Wl,--exclude-libs ALL  # don't export libc.a symbols
LDFLAGS += -Wl,--unresolved-symbols=report-all

include $(MKFILES_ROOT)/qtargets.mk
include $(MKFILES_ROOT)/qdepfile.mk  # defines "make depends"
WIN32_ENVIRON=mingw

-include $(PROJECT_ROOT)/local.mk  # optional (for debugging)
LIBS += screen