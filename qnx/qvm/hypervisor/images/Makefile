.PHONY: all clean check-host-bsp check-board-conf Copy-host-bsp-install-bins
.PRECIOUS: generated/ifs/hypervisor-FINAL-%.build

ROOT_DIR := $(notdir $(CURDIR))
ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

# Include user-configurable Make file (you can edit this one to customize the build)
include ./configure.mk
ifeq ($(BOARD_SPECIFIC_BSP_TYPE),0)
board_specific_build_file=hypervisor-intel-x86-64-APL-abl.build
board_arch=x86
else ifeq ($(BOARD_SPECIFIC_BSP_TYPE),1)
board_specific_build_file=hypervisor-renesas-rcarh3.build
board_arch=arm
else ifeq ($(BOARD_SPECIFIC_BSP_TYPE),2)
board_specific_build_file=hypervisor-intel-NUC.build
board_arch=x86
else ifeq ($(BOARD_SPECIFIC_BSP_TYPE),3)
board_specific_build_file=hypervisor-generic-x86.build
board_arch=x86
else ifeq ($(BOARD_SPECIFIC_BSP_TYPE),4)
board_specific_build_file=hypervisor-generic-arm.build
board_arch=arm
else
board_specific_build_file=NONE
board_arch=NONE
endif

HOST_MKIFS := mkifs
HOST_MKXFS := mkxfs
HOST_DISKIMAGE := diskimage
HOST_SED := sed
HOST_CAT := cat
HOST_TAR := tar

SUFFIXES := .build .ifs .bin

# Use build_file_blacklist to ignore certain build files that are known to not be BSP build files (rather, they are disk image build files).
host_build_file_path_prefix=$(BOARD_SPECIFIC_BSP_LOCATION)/images
build_file_blacklist=$(host_build_file_path_prefix)/root.build
# The list of available host BSP build files
bsp_build_files=$(filter-out $(build_file_blacklist),$(wildcard $(host_build_file_path_prefix)/*.build))
# Has 'make', or at least 'make prebuilt' been run yet in the host BSP?
is_bsp_built=$(wildcard $(BOARD_SPECIFIC_BSP_LOCATION)/install/*)

# The list of build files that we will generate below, from $bsp_build_files
hyp_build_files=$(subst $(host_build_file_path_prefix)/,generated/ifs/hyp-,$(bsp_build_files))

# The list of IFSs that we will build
hyp_bin_files=$(patsubst %.build,generated/ifs/hypervisor-ifs-%.bin,$(notdir $(bsp_build_files)))

# The list of available Guest IFSs
guest_ifses=$(wildcard $(QNX7_GUEST_BSP_LOCATION)/images/*-guest.ifs) $(wildcard $(QNX660_GUEST_BSP_LOCATION)/images/*-guest.ifs)

# Generated files
boot_partition_build_file=generated/diskimage/boot-dos-partition.build
#data_partition_build_file=generated/diskimage/data-qnx6-partition.build
boot_partition_file=generated/diskimage/boot.partition
#data_partition_file=generated/diskimage/data.partition

#This first check allows the internal build to work, even though no board BSP is provided.
ifeq ($(board_arch),NONE)
all:
	@echo "***ERROR: Board type defined by BOARD_SPECIFIC_BSP_TYPE is missing or invalid: $(BOARD_SPECIFIC_BSP_TYPE)"
	@echo "Please check your enviroment variable or configure.mk."
	@echo "Please follow instructions in images/README.txt"
else 
all:check-board-conf check-host-bsp hypervisor.diskimage
endif
	@echo Done

# .PHONY: check-board-conf
check-board-conf:
ifeq ($(BOARD_SPECIFIC_BSP_TYPE),0)
	@echo "Found board type 0 (Intel Gordon Ridge MRB)"
else ifeq ($(BOARD_SPECIFIC_BSP_TYPE),1)
	@echo "Found board type 1 (Renesas R-Car H3)"
else ifeq ($(BOARD_SPECIFIC_BSP_TYPE),2)
	@echo "Found board type 2 (Intel NUC)"
else ifeq ($(BOARD_SPECIFIC_BSP_TYPE),3)
	@echo "Found board type 3 (Generic x86)"
else ifeq ($(BOARD_SPECIFIC_BSP_TYPE),4)
	@echo "Found board type 4 (Generic arm)"
else
	@echo "***ERROR: Board type defined by BOARD_SPECIFIC_BSP_TYPE is invalid: $(BOARD_SPECIFIC_BSP_TYPE)"
	@echo "Please check your enviroment variable or configure.mk."
	@echo "Please follow instructions in images/README.txt"
	false
endif

# .PHONY: check-host-bsp
check-host-bsp:
ifeq ($(bsp_build_files),)
	@echo "***ERROR: No board-specific BSP appears to be installed at location '$(BOARD_SPECIFIC_BSP_LOCATION)', can not proceed."
	@echo "Please follow instructions in images/README.txt"
	false
else ifeq ($(is_bsp_built),)
	@echo "The installed board-specific BSP, at location '$(BOARD_SPECIFIC_BSP_LOCATION)' has not been built. Proceeding with the build to gather prebuilt binaries and setup any required build file links"
	$(MAKE) -C $(BOARD_SPECIFIC_BSP_LOCATION)
endif

# .PHONY: Copy-host-bsp-install-bins
# Copy all binaries from the host BSP's install/ folder into our install/ folder
# Don't try to make the host BSP here, because that process can generate more build files
# that this build process depends on, but won't know about if you built it from here.
Copy-host-bsp-install-bins: $(BOARD_SPECIFIC_BSP_LOCATION)/install
	$(CP_HOST) -R $(BOARD_SPECIFIC_BSP_LOCATION)/install/* ../install

# Generate the boot partition, and add all generated IFSs into it
$(boot_partition_build_file): disk_config/boot-dos-partition.build.template Copy-host-bsp-install-bins
	$(ECHO_HOST) "# **** THIS FILE IS GENERATED, DO NOT EDIT ****" > $@
	$(ECHO_HOST) "# To customize this file, edit $< instead, then run make" >> $@
	$(ECHO_HOST) "# " >> $@
	$(ECHO_HOST) " " >> $@
	$(HOST_CAT) $< >> $@
	$(foreach file,$(hyp_bin_files), \
		$(ECHO_HOST) "[type=file] $(file)" >> $@ ;\
	)
ifeq ($(BOARD_SPECIFIC_BSP_TYPE),2)
	$(ECHO_HOST) "On Intel NUC with UEFI, the IFS must live at /EFI/BOOT/BOOTx64.EFI of the boot partition, for autoboot to work. Here, we are copying hypervisor-ifs-x86_64-nuc-graphics.bin to this location in the boot partition of the disk image generated." > README_NUC.txt
	$(ECHO_HOST) "[type=file] /EFI/BOOT/BOOTx64.EFI = generated/ifs/hypervisor-ifs-x86_64-nuc-graphics.bin" >> $@
	$(ECHO_HOST) "[type=file] README_NUC.txt" >> $@
endif

# detectBSP.sh below will add the correct entry for the linux guest below
$(data_partition_build_file): disk_config/data-qnx6-partition.build.template
	$(ECHO_HOST) "# **** THIS FILE IS GENERATED, DO NOT EDIT ****" > $@
	$(ECHO_HOST) "# To customize this file, edit $< instead, then run make" >> $@
	$(ECHO_HOST) "# " >> $@
	$(ECHO_HOST) " " >> $@
	$(HOST_CAT) $< >> $@
	$(ECHO_HOST) "[-followlink] /guests/qnx700/qnx700-guest.ifs = $(QNX7_GUEST_BSP_LOCATION)/images/qnx700-guest.ifs" >> $@
	$(ECHO_HOST) "[-followlink] /guests/qnx700/qnx700-guest.qvmconf = $(QNX7_GUEST_BSP_LOCATION)/images/qnx700-guest.qvmconf" >> $@
	$(ECHO_HOST) "[-followlink] /guests/qnx660/qnx660-guest.ifs = $(QNX660_GUEST_BSP_LOCATION)/images/qnx660-guest.ifs" >> $@
	$(ECHO_HOST) "[-followlink] /guests/qnx660/qnx660-guest.qvmconf = $(QNX660_GUEST_BSP_LOCATION)/images/qnx660-guest.qvmconf" >> $@
	$(ECHO_HOST) "[-followlink] /guests/linux = ./guest_bsps/linux/$(board_arch)" >> $@

# Modify the stock BSP's build files to add the expected/required-by-hypervisor kernel attributes/modules to the procnto line of the build file... 
generated/ifs/hyp-%.build: $(host_build_file_path_prefix)/%.build Copy-host-bsp-install-bins
	$(ECHO_HOST) "# **** THIS FILE IS GENERATED, DO NOT EDIT ****" > $@
	$(ECHO_HOST) "# To customize this file, edit $< instead, then run make" >> $@
	$(ECHO_HOST) "# " >> $@
ifeq ($(BOARD_SPECIFIC_BSP_TYPE),2)
	$(HOST_SED) -f ./scripts/HV_Mod_Build_file.sed -f ./scripts/HV_Mod_Build_file-NUC.sed $< >> $@
else
	$(HOST_SED) -f ./scripts/HV_Mod_Build_file.sed $< >> $@
endif


# Attempt to find one of the supported boards we have, and generate the appropriate board-specific build files.
# Also this will add the correct entry into the boot-dos-partition.build file, for which linux guest it will use.
# Make sure that the hyp_build_files have been generated already, since the script may operate on them.
hypervisor-board-specific.build: $(board_specific_build_file) $(boot_partition_build_file) $(data_partition_build_file) $(hyp_build_files)
	$(ECHO_HOST) "# **** THIS FILE IS GENERATED, DO NOT EDIT ****" > $@
	$(ECHO_HOST) "# To customize this file, edit $< instead, then run make" >> $@
	$(ECHO_HOST) "# " >> $@
	$(HOST_CAT) $< >> $@

# Combine the modified BSP build file, the board-specific build file, and the common build file together to make the final build files that will be turned into IFSs
generated/ifs/hypervisor-FINAL-%.build: generated/ifs/hyp-%.build hypervisor-board-specific.build hypervisor-common-$(board_arch).build hypervisor-common.build
	$(HOST_CAT) $^ > $@

generated/ifs/hypervisor-ifs-%.bin: generated/ifs/hypervisor-FINAL-%.build
	$(HOST_MKIFS) -vvv -r../install $(MKIFSFLAGS) $< $@

# The boot partition
$(boot_partition_file): $(boot_partition_build_file) $(hyp_bin_files)
	$(HOST_MKXFS) -vv -t fatfsimg $< $@

# The data partition, which will hold the guest IFS and QVM configs
$(data_partition_file): $(data_partition_build_file) $(guest_ifses)
	$(HOST_MKXFS) -vv -t qnx6fsimg $< $@

# Some platforms need a MBR-based disk
hypervisor.diskimage: disk_config/diskimage.cfg $(boot_partition_file) $(data_partition_file)
	$(HOST_DISKIMAGE) -c $< -o $@

# Some platforms can use a GPT-based disk
hypervisor_gpt.diskimage: disk_config/diskimage_gpt.cfg $(boot_partition_file) $(data_partition_file)
	$(HOST_DISKIMAGE) -g -c $< -o $@

clean:
	$(RM_HOST) *.sym *.diskimage hypervisor-board-specific.build generated/*/*
