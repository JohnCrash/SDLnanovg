/*
 * opengl 头文件配置文件，根据不同的系统配置相应的头文件。
 */
#if defined(__IPHONEOS__)
	#define __GLES__
	#include <OpenGLES/ES2/gl.h>
	#include <OpenGLES/ES2/glext.h>
	#define NANOVG_GLES2_IMPLEMENTATION
#elif defined(__ANDROID__)
	#define __GLES__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
	#define NANOVG_GLES2_IMPLEMENTATION
	#define GL_QUERY_RESULT_AVAILABLE                        0x8867
#else
	#include "GL/glew.h"
	#define NANOVG_GL2_IMPLEMENTATION
#endif

#ifndef APIENTRY
#define APIENTRY
#endif
