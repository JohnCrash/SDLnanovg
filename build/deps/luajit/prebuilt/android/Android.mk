LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := luajit

LOCAL_MODULE_FILENAME := luajit

LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libluajit.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../include

include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := luajit_static

LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libluajit.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../include

include $(PREBUILT_STATIC_LIBRARY)
