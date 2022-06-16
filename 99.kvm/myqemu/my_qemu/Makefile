VIRT_GUEST_BIN			= guest
VIRT_MACH_BIN_NAME		= host_mach


CC						= aarch64-linux-gnu-gcc
LD						= aarch64-linux-gnu-ld
OBJCOPY					= aarch64-linux-gnu-objcopy
OBJDUMP					= aarch64-linux-gnu-objdump

OBJ_DIR					= obj
VIRT_GUEST_SRC_DIR		= virt_guest
VIRT_MACHINE_SRC_DIR	= virt_machine
VIRT_GUEST_OBJ			= $(VIRT_GUEST_SRC_DIR)/$(OBJ_DIR)
VIRT_MACHINE_OBJ		= $(VIRT_MACHINE_SRC_DIR)/$(OBJ_DIR)

VIRT_GUEST_C_SRC 		= $(wildcard $(VIRT_GUEST_SRC_DIR)/*.c)
VIRT_GUEST_S_SRC 		= $(wildcard $(VIRT_GUEST_SRC_DIR)/*.S)
VIRT_MACHINE_C_SRC		= $(wildcard $(VIRT_MACHINE_SRC_DIR)/*.c)
VIRT_GUEST_OBJ_S_C_O	= $(VIRT_GUEST_C_SRC:$(VIRT_GUEST_SRC_DIR)/%.c=$(VIRT_GUEST_OBJ)/%.o) \
						  $(VIRT_GUEST_S_SRC:$(VIRT_GUEST_SRC_DIR)/%.S=$(VIRT_GUEST_OBJ)/%.o)
VIRT_OBJ_OBJ_C_O 		= $(VIRT_MACHINE_C_SRC:$(VIRT_MACHINE_SRC_DIR)/%.c=$(VIRT_MACHINE_OBJ)/%.o)

INCLUDES 				:=
CFLAGS					:= -Wall -Wstrict-prototypes -g -ffreestanding $(INCLUDES) 



all: mkdir_obj $(VIRT_MACH_BIN_NAME) $(VIRT_GUEST_BIN) FORCE
	@echo ******done******


$(VIRT_MACH_BIN_NAME): $(VIRT_OBJ_OBJ_C_O)
	$(CC) -o $@ $^

$(VIRT_GUEST_BIN): $(VIRT_GUEST_OBJ_S_C_O)
	$(LD) -Tguest.lds -o $(VIRT_GUEST_OBJ)/$@ $^
	$(OBJCOPY) -O binary -S $(VIRT_GUEST_OBJ)/$@ $@.bin
	$(OBJDUMP) -D $(VIRT_GUEST_OBJ)/$@ > $(VIRT_GUEST_OBJ)/$@.dump



$(VIRT_MACHINE_OBJ)/%.o : $(VIRT_MACHINE_SRC_DIR)/%.c FORCE
	$(CC) $(CFLAGS) -c -o $@ $<

$(VIRT_GUEST_OBJ)/%.o : $(VIRT_GUEST_SRC_DIR)/%.S FORCE
	$(CC) $(CFLAGS) -c -o $@ $<

$(VIRT_GUEST_OBJ)/%.o : $(VIRT_GUEST_SRC_DIR)/%.c FORCE
	$(CC) $(CFLAGS) -c -o $@ $<


mkdir_obj:
	@mkdir $(VIRT_MACHINE_OBJ)
	@mkdir $(VIRT_GUEST_OBJ)


clean:
	rm -rf $(VIRT_MACHINE_OBJ) $(VIRT_GUEST_OBJ) $(VIRT_MACH_BIN_NAME) $(VIRT_GUEST_BIN).bin




PHONY += FORCE
FORCE:

.PHONY: $(PHONY)


#gcc -o start start.S

