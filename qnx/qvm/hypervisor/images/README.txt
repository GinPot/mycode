To build a Hypervisor disk image from this folder (<BSP_ROOT>/images), where <BSP_ROOT> is the path where your QNX Hypervisor host BSP is installed to:

0. Source the qnx-env.sh in your local QNX SDP7 installation, and change directory to <BSP_ROOT>.


1. Look in <BSP_ROOT>/images/configure.mk. There are a series of environment variables listed there, that allow you to indicate various specifics about your configuration. Here, you can either: set the environment variable (useful in command-line environment); or you can edit the value found in configure.mk (useful in IDE environment). Note that environment variables override the values defined in configure.mk. The available variables are:

1A. BOARD_SPECIFIC_BSP_TYPE: the specific hardware platform you are working on. There is no default, so ***you must provide this information***. It can be one of:

    0 - Intel Gordon Ridge MRB (also called 'Apollo Lake')
    1 - Renesas R-Car H3
    2 - Intel NUC
    3 - Any other compatible x86/x86_64 board
    4 - Any other compatible ARM board

E.g., for working with Rensas R-Car H3, you could edit the configure.mk file, or you could set the following environment variable before building the BSP:

# export BOARD_SPECIFIC_BSP_TYPE=1

1B. BOARD_SPECIFIC_BSP_LOCATION: the relative or absolute path to where your specific hardware platform BSP is installed to. The default is ./images/host_bsp/. E.g., to indicate that you have installed/unzipped your BSP into ~/qnx700/bsp/mybsp/:

# export BOARD_SPECIFIC_BSP_LOCATION=~/qnx700/bsp/mybsp

1C. QNX7_GUEST_BSP_LOCATION: similar to 1B. above, this indicates the relative or absolute path to where your QNX SDP7 Hypervisor Guest BSP is located. The default is ./guest_bsps/qnx700/.

1D. QNX660_GUEST_BSP_LOCATION: similar to 1B. above, this indicates the relative or absolute path to where your QNX SDP6.6 Hypervisor Guest BSP is located. The default is ./guest_bsps/qnx660/.


2 Download and unzip the BSP zip file for your host hardware platform into the folder that you configured in 1A above. E.g. if you leave the default value specified in configure.mk you could do the following from the root of this BSP: 

	# unzip ~/Downloads/BSP_intel-x86-64-APL-abl_br-mainline_be-700_SVN830677_JBN6.zip -d $BOARD_SPECIFIC_BSP_LOCATION


3. (optional, if you are customizing the BSP first) Build the host BSP, by changing directory to the host_bsp folder and running 'make':

	# cd $BOARD_SPECIFIC_BSP_LOCATION
	# <optionally make changes>
	# make


4. (optional, if using the QNX Hypervisor Guest BSPs) Download and unzip the QNX Hypervisor Guest BSP zip files into the folders that you configured in 1C and 1D, as required, e.g.:

	# cd <BSP_ROOT>
	# unzip ~/Downloads/BSP_hypervisor-guest-x86_64_br-mainline_be-700_SVN832288_JBN22.zip -d $QNX7_GUEST_BSP_LOCATION
	# unzip ~/Downloads/BSP_hypervisor-guest-x86_br-660_be-660_SVN832288_JBN21.zip -d $QNX660_GUEST_BSP_LOCATION


5. (optional) Make any desired changes to the guest BSPs, re-building them when finished, e.g. for the QNX 7.0 guest, e.g.:

	# cd $QNX7_GUEST_BSP_LOCATION
	# <optionally make changes>
	# make

6. (optional) Adjust any of the disk image settings in the <BSP_ROOT>/images/disk_config/ folder. There are partition.template files that describe the contents of the individual partitions, and diskimage.cfg files that describe the geometry of the disk image.


7. Execute 'make' from the <BSP_ROOT>/ folder. 


8. Use dd or similar tool to copy the resulting '<BSP_ROOT>/images/hypervisor.diskimage' file onto the boot disk of your board (note that it is a complete disk image, ie including a partition table and multiple partitions, so it should be copied directly to the raw disk device).


9. Boot using your disk from the previous step to bring up Hypervisor.


10. Optionally, modify the host BSP or the guest BSPs, run 'make' in the root of any BSP you modify, then repeat steps 3,5-9 to refresh your system.
