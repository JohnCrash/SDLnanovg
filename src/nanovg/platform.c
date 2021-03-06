#include "platform.h"
#include "SDL.h"

static char _pwrite_directory[MAX_PATH] = { 0 };

/**
 * 取得可写目录
 * 根据不同系统改函数将返回一个可写目录
 */
const char* getWriteDirectory()
{
	char * pdir;
	/* _pwrite_directory未初始化 */
	if (_pwrite_directory[0] == 0){
		pdir = SDL_GetPrefPath(PACKAGE_NAME, "files");
		SDL_assert(pdir);
		SDL_strlcpy(_pwrite_directory, pdir, MAX_PATH);
		SDL_free(pdir);
	}
	return _pwrite_directory;
}

#if defined(_WIN32)
//windows
#include "windows.h"
static char _execPath[MAX_PATH] = { 0 };
static char _luaPath[MAX_PATH] = { 0 };
static char _clibPath[MAX_PATH] = { 0 };
static char _filePath[MAX_PATH] = { 0 };

/*
 * 返回执行程序所在目录
 */
const char * getRootPath()
{
	int i, l;
	int first = 1;
	if (!_execPath[0]){
		if (GetModuleFileNameA(NULL, _execPath, MAX_PATH) != 0){
			l = strlen(_execPath);
			//将文件名去掉，同时将\转换为/
			for (i = l-1; i >=0; i--){
				if (_execPath[i] == '\\'){
					if (first){
						_execPath[i] = '\0';
						first = 0;
					}
					else{
						_execPath[i] = '/';
					}
				}
			}
			return _execPath;
		}
	}

	return _execPath;
}

/*
 * 返回lua动态库的搜索路径
 */
const char * getLUAClibPath()
{
	if (!_clibPath[0]){
		strcpy(_clibPath, getRootPath());
		strcat(_clibPath,"/?.dll");
	}
	return _clibPath;
}

/*
 * 返回lua动态库的搜索路径
 */
const char * getLUAPath()
{
	if (!_luaPath[0]){
		strcpy(_luaPath, getRootPath());
		strcat(_luaPath, "/lua/?.lua;");
		strcat(_luaPath, getRootPath());
		strcat(_luaPath, "/lua/?.luac");
	}
	return _luaPath;
}

const char * getFileSearchPath()
{
	if (!_filePath[0]){
		strcpy(_filePath, getRootPath());
		strcat(_filePath, "/?;?"); //先到绝对路径下找，然后直接找
	}
	return _filePath;
}
#elif defined(__ANDROID__)
//android
const char * getRootPath()
{
	return "";
}

/*
 * android下.so文件将被展开在/data/data/"PACKAGE_NAME"/lib/下
 */
const char * getLUAClibPath()
{
	return "/data/data/"PACKAGE_NAME"/lib/lib?.so";
}

/*
 * android系统为了便于调试，优先从/sdcard/APP_NAME下装载lua代码
 * 这样可以在/sdcard/APP_NAME下重载默认库中的lua源代码
 */
const char * getLUAPath()
{
	return "/sdcard/"APP_NAME"/lua/?.lua;lua/?.lua;lua/?.luac";
}

const char * getFileSearchPath()
{
	return "/sdcard/"APP_NAME"/?;?";
}

#elif defined(__APPLE__)
//apple ios
const char * getRootPath()
{
	return "";
}

const char * getLUAClibPath()
{
	return "?.so";
}

const char * getLUAPath()
{
	return "lua/?.lua;lua/?.luac";
}

const char * getFileSearchPath()
{
	return "?";
}
#else
//other unix like system
#endif