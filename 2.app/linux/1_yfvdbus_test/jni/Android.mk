LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS += -Wno-maybe-uninitialized -Wno-parentheses
LOCAL_CPPFLAGS += -Wno-conversion-null

LOCAL_SRC_FILES:= \
    DbusJni.cpp

LOCAL_C_INCLUDES += \
    vendor/yfvet/infrastructure/CommAdaptor_Android_Release/TestCommAdaptorNativeClient/include
LOCAL_SHARED_LIBRARIES := \
    libandroid_runtime \
    liblog \
    libcutils \
    libutils \
    libbinder \
    libnativehelper \
    libCommAdaptorNative

LOCAL_MODULE:= libdbus_jni

include $(BUILD_SHARED_LIBRARY)
