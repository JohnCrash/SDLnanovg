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

LOCAL_SRC_FILES := ../../../src/main/android/SDL_android_main.c \
	$(SRC_PATH)/main.c 					\
	$(SRC_PATH)/fs.c 					\
	$(SRC_PATH)/basetype				\
	$(NANOVG_SRC_PATH)/sdlmain.c 		\
	$(NANOVG_SRC_PATH)/nanovg.c 		\
	$(NANOVG_SRC_PATH)/eventhandler.c	\
	$(NANOVG_SRC_PATH)/sdlport.c 		\
	$(NANOVG_SRC_PATH)/luaext.c		 	\
	$(NANOVG_SRC_PATH)/luananovg.c		\
	$(NANOVG_SRC_PATH)/luaui.c			\
	$(NANOVG_SRC_PATH)/nanovg_sdl.c		\
	$(NANOVG_SRC_PATH)/ui.c				\
	$(NANOVG_SRC_PATH)/utf8.c			\
	
LOCAL_WHOLE_STATIC_LIBRARIES := luajit_static
	
#LOCAL_SHARED_LIBRARIES := SDL2 socket-prebuilt mime-prebuilt
LOCAL_SHARED_LIBRARIES := SDL2

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)

$(call import-module,SDLnanovg)
$(call import-module,luajit/prebuilt/android)
#$(call import-module,luasocket/prebuilt/android)