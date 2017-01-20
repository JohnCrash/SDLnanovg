LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := socket-prebuilt

LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libsocket.so

include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := mime-prebuilt

LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libmime.so

include $(PREBUILT_SHARED_LIBRARY)