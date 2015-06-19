/*
*/
#if defined(__IPHONEOS__)
	#define __GLES__
	#include <OpenGLES/ES2/gl.h>
	#include <OpenGLES/ES2/glext.h>
	#define NANOVG_GLES2_IMPLEMENTATION
	#define LOG(...)
#elif defined(__ANDROID__)
	#define __GLES__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
	#define NANOVG_GLES2_IMPLEMENTATION
	#include <android/log.h>
	#define LOG_TAG "SDL_android"
	#define LOG(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)	
	#define GL_QUERY_RESULT_AVAILABLE                        0x8867
#else
	#include "GL/glew.h"
	#define NANOVG_GL2_IMPLEMENTATION
	#define LOG(...)
#endif

#ifndef APIENTRY
#define APIENTRY
#endif
