The boot images for the guests that are to be run on the Hypervisor host are to be installed here. 
Linux guests that you build should be copied to the appropriate linux/<architecture>/ folder. For the 
QNX 6.6 and 7.0 guests, the guest BSP for the architecture that you plan to run Hypervisor on, should 
be unzipped into the appropriate (qnx660 or qnx700) folder. So for example, if running on an ARM 
platform, you could unzip BSP_hypervisor-guest-aarch64_<version>.zip into the guests/qnx700/ folder, make any 
changes you want, build it as you would any other BSP (by running 'make' in the root folder or with the IDE), 
and then when you build the Hypervisor Host BSP, it will pull the boot IFS and QVM configuration file 
from the guest onto the host's disk image.
