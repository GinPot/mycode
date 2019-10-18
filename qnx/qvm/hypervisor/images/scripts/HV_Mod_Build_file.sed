# A sed script that attempts to modify a QNX build file, to:
# 1. Add *only* the hypervisor-specific attributes to the 'start procnto' line. It attempts to keep any existing attributes, so there could end up with duplicates.
# 2. Make all [+session] shells run in the background, so that subsequent scripts don't get blocked by a process running in the foreground
# 3. Remove the -B option to the startup driver, if it exists. This is required for Hypervisor, since it needs ACPI tables to be available.

# #1
# This is basically like an 'if - else if' block
# The idea here is, if there are any [<attributes>] already there, we want to just add to them...
s#\(^\s*\)\(\[.*\)\]\(.*PATH=.*procnto.*$\)#\1\2\ +keeplinked module=aps module=qvm\] \3#
t
# ... else if there are no [<attributes>], then we just prepend them.
s#\(^\s*\)\(.*PATH=.*procnto.*$\)#\1\[+keeplinked module=aps module=qvm\] \2#

# Note that we are keeping any whitespace (\s) at the beginning. The \1, \2, \3, are references back to pieces I've kept with \(...\) blocks.

# #2 
# If there is already a '&', leave the line alone
s#^.*\[+session\].*\&$#&#
t
# ... else, add a background character
s#^.*\[+session\].*$#& \&#


# #3
s#\(startup-.*\)-B#\1#g
