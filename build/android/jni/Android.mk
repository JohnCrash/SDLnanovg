#include $(call all-subdir-makefiles)
#include SDL/Android.mk

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SRC_PATH := ../../src
NANOVG_SRC_PATH :=../../../src/nanovg

LOCAL_C_INCLUDES := $(SRC_PATH) \
		$(LOCAL_PATH)/../../../src/nanovg \
		$(LOCAL_PATH)/../../../src/nanovg/example

LOCAL_SRC_FILES := SDL/src/main/android/SDL_android_main.c \
	$(SRC_PATH)/sdlmain.c \
	$(SRC_PATH)/main.c \
	$(SRC_PATH)/fs.c \
	$(SRC_PATH)/test_nanovg_sdl.c \
	$(NANOVG_SRC_PATH)/nanovg.c \
	$(NANOVG_SRC_PATH)/sdlport.c \
	$(NANOVG_SRC_PATH)/example/demo.c \
	$(NANOVG_SRC_PATH)/example/perf.c
	
LOCAL_SHARED_LIBRARIES := SDL2

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)

$(call import-module,SDL)