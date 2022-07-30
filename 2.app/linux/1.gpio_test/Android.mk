LOCAL_PATH:= $(call my-dir)

#### Build Service ############################################
include $(CLEAR_VARS)
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE:= gpio_test

LOCAL_SRC_FILES:= \
    gpio_test.c


include $(BUILD_EXECUTABLE)

