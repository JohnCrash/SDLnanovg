LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := luasocket_static
LOCAL_MODULE_FILENAME := luasocket
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libluasocket.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../include
include $(PREBUILT_STATIC_LIBRARY)
