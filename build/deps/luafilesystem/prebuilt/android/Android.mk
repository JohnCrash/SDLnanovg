LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := lfs-prebuilt

LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/liblfs.so

include $(PREBUILT_SHARED_LIBRARY)