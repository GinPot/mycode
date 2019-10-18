define PINFO
PINFO DESCRIPTION=Renesas R-Car SATA disk driver
endef
ifneq ($(wildcard $(PWD)/$(NAME).use),)
USEFILE=$(PWD)/$(NAME).use
endif
