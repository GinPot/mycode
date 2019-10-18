# Each board-specific capture  includes this board.mk file
# (and can override variables if required).

include $(MKFILES_ROOT)/qmacros.mk

IS_DEBUG_BUILD:=$(filter g, $(VARIANT_LIST))

INSTALLDIR=usr/lib/

NAME=$(IMAGE_PREF_SO)capture-board-renesas-$(PROJECT)
CCFLAGS += $(if $(IS_DEBUG_BUILD),-O0)

USEFILE=$(PROJECT_ROOT)/board.use
