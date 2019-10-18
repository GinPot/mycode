# This filter is for use with the QNX Hypervisor 2.0 Reference Image for the Intel NUC board.
# A sed script that attempts to modify a QNX build file, to:
# 1. Make the startup driver specify the 8250 as debug device, as VGA console is not working with a UEFI boot loader.
# 2. For the NUC, with 'bios' specified for boot image format, change it to 'uefi' so that we can boot from a dos partition.

# #1
   # If there is already a -D option, leave the line alone
s#^.*startup-x86.*-D.*$#&#
t
# Else, if there is no -D option, we add our 8250 one
s#\(startup-x86.*$\)#\1 -D8250..115200#

# #2
s#\(^\[virtual=x86.*\)bios#\1uefi#

