LOCAL_PATH:= $(call my-dir)

#### Build Service ############################################
include $(CLEAR_VARS)
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE:= gpio_test

LOCAL_SRC_FILES:= \
    gpio_test.c

#LOCAL_SHARED_LIBRARIES := \
#    libyfvdbus_cfg \
#    libyfvdbus \
#	libdbus \
#
#LOCAL_CFLAGS := -pthread\

#LOCAL_C_INCLUDES += \
#    $(LOCAL_PATH)/../include \


include $(BUILD_EXECUTABLE)

