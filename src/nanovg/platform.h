#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#if defined(_WIN32)
//windows
#include <Windows.h>
const char * getRootPath();		//���ظ�Ŀ¼
const char * getLuaRootPath();	//����luaԴ���Ŀ¼
const char * getLUAClibPath();	//����lua��̬��·��
const char * getLUAPath();		//����lua��·��
#elif defined(__ANDROID__)
//android
#define MAX_PATH 256
#define PACKAGE_NAME "org.libsdl.nanovg"
#define APP_NAME "SDLnanovg"

const char * getRootPath();
const char * getLuaRootPath();
const char * getLUAClibPath();
const char * getLUAPath();
#elif defined(__APPLE__)
//apple ios
#define MAX_PATH 256
const char * getRootPath();
const char * getLuaRootPath();
const char * getLUAClibPath();
const char * getLUAPath();
#else
//other unix like system
#endif

#endif