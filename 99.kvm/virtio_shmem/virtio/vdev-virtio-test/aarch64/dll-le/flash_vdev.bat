rem ------------------------------------------------------------------------------
rem   File & Info:    $Id$   
rem
rem   Description:  flash qnx images from qnx_dev directory   
rem                   
rem ------------------------------------------------------------------------------

rem ------------------------------------------------------------------------------
rem   Usage: 
rem   1、copy the dailybuild xxx directory to local
rem   2、enter the current directory
rem   3、change the dhu to fastboot status
rem   4、double click the script
rem   
rem   
rem ------------------------------------------------------------------------------

adb root
adb remount
adb push ./vdev-virtio-test.so /shared
adb shell sync



@echo flash Successful burning
@pause
