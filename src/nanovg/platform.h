#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#define MAX_PATH 256
#define APP_NAME "SDLnanovg"
#define PACKAGE_NAME "org.libsdl.nanovg"

#if defined(__APPLE__)
#include "TargetConditionals.h"
#if defined(TARGET_OS_MAC)
#define __MAC__
#elif defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
#define __IOS__
#endif
#endif

#if defined(_WIN32)
	//windows
	const char * getRootPath();		//返回根目录
	const char * getLUAClibPath();	//返回lua动态库搜索路径
	const char * getLUAPath();		//返回lua包搜索路径
	const char * getFileSearchPath();	//返回普通文件搜索路径
	const char* getWriteDirectory();	// 取得可写目录
#elif defined(__ANDROID__)
	//android
	const char * getRootPath();
	const char * getLUAClibPath();
	const char * getLUAPath();
	const char * getFileSearchPath();
	const char* getWriteDirectory();
#elif defined(__APPLE__)
	//apple ios
	const char * getRootPath();
	const char * getLUAClibPath();
	const char * getLUAPath();
	const char * getFileSearchPath();
	const char* getWriteDirectory();
#else
	//other unix like system
#endif

#endif