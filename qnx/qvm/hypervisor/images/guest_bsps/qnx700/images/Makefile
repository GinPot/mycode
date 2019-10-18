ROOT_DIR := $(notdir $(CURDIR))
ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

HOST_MKIFS := mkifs

SUFFIXES := .build .ifs

.PHONY: all clean

all: qnx700-guest.ifs

clean:
	$(RM_HOST) *.ifs *.sym

qnx700-guest.ifs: build
	$(HOST_MKIFS) -r../install -vvvvv $(MKIFSFLAGS) $^ $@
