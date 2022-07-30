LOCAL_PATH:= $(call my-dir)

#### Build Service ############################################
include $(CLEAR_VARS)
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE:= mdio

LOCAL_SRC_FILES:= \
    mdio.c




include $(BUILD_EXECUTABLE)

