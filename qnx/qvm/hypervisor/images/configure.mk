# This file allows you to configure the build, based on the type of board
# you are using (the board-specific BSP), and the location of the BSPs you 
# are wrapping here. The defaults are described below. You 
# can either change the defaults in this file, or set them as 
# environment variables as described to change it. 

############################################################
#
# You can define the environment variable 
# ${BOARD_SPECIFIC_BSP_LOCATION} to point to an arbitrary 
# location where you have your board BSP unzipped to.
#
# Alternatively, you can change the default here.
#
# Default: this BSP's images/host_bsp/ folder
#
############################################################
ifndef BOARD_SPECIFIC_BSP_LOCATION
BOARD_SPECIFIC_BSP_LOCATION=host_bsp
endif

############################################################
# 
# You can define the environment variable 
# ${BOARD_SPECIFIC_BSP_TYPE} to indicate what 
# board you are using.
#
# Alternatively, you can change the default here.
#
# Key:
#    0 - Intel Gordon Ridge MRB (also called 'Apollo Lake')
#    1 - Renesas R-Car H3
#    2 - Intel NUC
#    3 - Any other compatible x86/x86_64 board
#    4 - Any other compatible ARM board
#
# Default: 0 - Intel Gordon Ridge MRB
#
############################################################
ifndef BOARD_SPECIFIC_BSP_TYPE
BOARD_SPECIFIC_BSP_TYPE=1
endif

############################################################
#
# You can define the environment variable 
# ${QNX7_GUEST_BSP_LOCATION} to point to an arbitrary 
# location where you have your QNX7 Guest BSP unzipped to.
#
# Alternatively, you can change the default here.
#
# Default: this BSP's images/guest_bsps/qnx700 folder
#
############################################################
ifndef QNX7_GUEST_BSP_LOCATION
QNX7_GUEST_BSP_LOCATION=guest_bsps/qnx700
endif

############################################################
#
# You can define the environment variable 
# ${QNX660_GUEST_BSP_LOCATION} to point to an arbitrary 
# location where you have your QNX660 Guest BSP unzipped to.
#
# Alternatively, you can change the default here.
#
# Default: this BSP's images/guest_bsps/qnx660 folder
#
############################################################
ifndef QNX660_GUEST_BSP_LOCATION
QNX660_GUEST_BSP_LOCATION=guest_bsps/qnx660
endif

