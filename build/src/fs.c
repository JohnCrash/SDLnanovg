#include "SDL.h"
#include "fs.h"

static char * _pwrite_directory[MAX_PATH] = {0};
/*
 * ȡ�ÿ�дĿ¼
 * ���ݲ�ͬϵͳ�ĺ���������һ����дĿ¼
 */
char* getWriteDirectory()
{
	char * pdir;
	/* _pwrite_directoryδ��ʼ�� */
	if (_pwrite_directory[0] == 0){
		pdir = SDL_GetPrefPath("nanovg.libsdl.org", "files");
		SDL_assert(pdir);
		SDL_strlcpy(_pwrite_directory, pdir, MAX_PATH);
		SDL_free(pdir);
	}
	return _pwrite_directory;
}