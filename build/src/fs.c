#include "SDL.h"
#include "fs.h"

static char * _pwrite_directory[MAX_PATH] = {0};
/*
 * 取得可写目录
 * 根据不同系统改函数将返回一个可写目录
 */
char* getWriteDirectory()
{
	char * pdir;
	/* _pwrite_directory未初始化 */
	if (_pwrite_directory[0] == 0){
		pdir = SDL_GetPrefPath("nanovg.libsdl.org", "files");
		SDL_assert(pdir);
		SDL_strlcpy(_pwrite_directory, pdir, MAX_PATH);
		SDL_free(pdir);
	}
	return _pwrite_directory;
}