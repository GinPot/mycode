ROOT_DIR := $(notdir $(CURDIR))
ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

HOST_MKIFS := mkifs

SUFFIXES := .build .ifs .bin

.PHONY: all clean m3

all: m3

m3: ifs-rcar_m3-graphics.bin

ifs-rcar_m3-graphics.bin: rcar_m3-graphics.build
	$(HOST_MKIFS) -vvv -r../install $(MKIFSFLAGS) $^ $@

clean:
	$(RM_HOST) *.bin
