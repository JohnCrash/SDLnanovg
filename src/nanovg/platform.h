#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#if defined(_WIN32)
	//windows
	#include <Windows.h>
	const char * getRootPath();		//返回根目录
	const char * getLUAClibPath();	//返回lua动态库搜索路径
	const char * getLUAPath();		//返回lua包搜索路径
	const char * getFileSearchPath();	//返回普通文件搜索路径
#elif defined(__ANDROID__)
	//android
	#define MAX_PATH 256
	#define PACKAGE_NAME "org.libsdl.nanovg"
	#define APP_NAME "SDLnanovg"

	const char * getRootPath();
	const char * getLUAClibPath();
	const char * getLUAPath();
	const char * getFileSearchPath();
#elif defined(__APPLE__)
	//apple ios
	#define MAX_PATH 256
	const char * getRootPath();
	const char * getLUAClibPath();
	const char * getLUAPath();
	const char * getFileSearchPath();
#else
	//other unix like system
#endif

#endif