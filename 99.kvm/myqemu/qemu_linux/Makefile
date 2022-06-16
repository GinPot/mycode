VIRT_MACH_BIN_NAME		= qemu_linux


CC						= aarch64-linux-gnu-gcc
LD						= aarch64-linux-gnu-ld
OBJCOPY					= aarch64-linux-gnu-objcopy
OBJDUMP					= aarch64-linux-gnu-objdump

OBJ_DIR					= obj
VIRT_MACHINE_SRC_DIR	= virt_machine
VIRT_MACHINE_OBJ		= $(VIRT_MACHINE_SRC_DIR)/$(OBJ_DIR)


VIRT_MACHINE_C_SRC		= $(wildcard $(VIRT_MACHINE_SRC_DIR)/*.c)
VIRT_OBJ_OBJ_C_O 		= $(VIRT_MACHINE_C_SRC:$(VIRT_MACHINE_SRC_DIR)/%.c=$(VIRT_MACHINE_OBJ)/%.o)

INCLUDES 				:=
CFLAGS					:= -Wall -Wstrict-prototypes -g -ffreestanding$(INCLUDES)
LDFLAGS 				:= -lpthread


all: mkdir_obj $(VIRT_MACH_BIN_NAME) FORCE
	@echo ******done******


$(VIRT_MACH_BIN_NAME): $(VIRT_OBJ_OBJ_C_O)
	$(CC) -o $@ $^ $(LDFLAGS)


$(VIRT_MACHINE_OBJ)/%.o : $(VIRT_MACHINE_SRC_DIR)/%.c FORCE
	$(CC) $(CFLAGS) -c -o $@ $<



mkdir_obj:
	@mkdir $(VIRT_MACHINE_OBJ)

clean:
	rm -rf $(VIRT_MACHINE_OBJ) $(VIRT_GUEST_OBJ) $(VIRT_MACH_BIN_NAME)




PHONY += FORCE
FORCE:

.PHONY: $(PHONY)


#gcc -o start start.S

