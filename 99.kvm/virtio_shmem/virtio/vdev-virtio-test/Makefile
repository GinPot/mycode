LIST=CPU
EARLY_DIRS=
LATE_DIRS=

# Bring in configuration macros which includes qconf-override.mk
ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

# First we need to have the sytem defined protected header staging locations
include $(AMSS_ROOT)/amss_defs_af.mk

# Now define this projects protected header staging location (INSTALL_ROOT_PHDR is used by PROTECTED_MAKE)
INSTALL_ROOT_PHDR=$(AMSS_INC)

# include makefile to stage the protected files (searches for protected folder and stages to $(INSTALL_ROOT_PHDR)
include $(PROTECTED)
