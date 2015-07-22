#include "SDL.h"
#include <stdio.h>
#define FONTSTASH_IMPLEMENTATION
#include "fontstash.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "sdlport.h"

/*
 * stb库不能访问android assets中的文件(apk包中的文件)
 * sdl库可以访问assets中的文件，并且提供统一的接口。
 * 如访问文件fonts/entypo.ttf,windows下放在exe同目录下就可以了
 * android下将访问assets下的fonts/entypo.ttf
 * stbi_load2
 * fonsAddFont2
 * 重写stb库中对应的函数stbi_load和fonsAddFont
 */
static unsigned char *stbi_load_from_sdlrw(SDL_RWops *f, int *x, int *y, int *comp, int req_comp)
{
	stbi__context s;
	stbi_uc *buffer;
	unsigned char *result;
	int len;
	result = NULL;
	len = SDL_RWsize(f);
	if( len>0 ){
	   buffer = malloc(len);
	   SDL_RWread(f,buffer,1,len);
	   stbi__start_mem(&s,buffer,len);
	   result = stbi_load_main(&s,x,y,comp,req_comp);
	   free(buffer);
	}
	return result;
}

STBIDEF unsigned char *stbi_load2(char const *filename, int *x, int *y, int *comp, int req_comp)
{
   SDL_RWops *f = SDL_RWFromFile(filename,"rb");
   unsigned char *result;
   if (!f){
	   return stbi__errpuc("can't fopen", "Unable to open file");
   }
   result = stbi_load_from_sdlrw(f,x,y,comp,req_comp);
   SDL_RWclose(f);
   return result;
}

int fonsAddFont2(FONScontext* stash, const char* name, const char* path)
{
	SDL_RWops *fp;
	int dataSize = 0;
	unsigned char* data = NULL;

	// Read in the font data.
	fp = SDL_RWFromFile(path,"rb");	
	if (fp == NULL) goto error;
	dataSize = SDL_RWsize(fp);
	data = (unsigned char*)malloc(dataSize);
	if (data == NULL) goto error;
	SDL_RWread(fp,data,1,dataSize);
	SDL_RWclose(fp);
	fp = 0;

	return fonsAddFontMem(stash, name, data, dataSize, 1);

error:
	if (data) free(data);
	if (fp) SDL_RWclose(fp);
	return FONS_INVALID;
}