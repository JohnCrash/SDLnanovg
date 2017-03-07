#include <stdio.h>
#include "sdlport.h"
#define FONTSTASH_IMPLEMENTATION
#include "fontstash.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "platform.h"

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
	stbi__result_info ri;
	result = NULL;
	len = (int)SDL_RWsize(f);
	if( len>0 ){
	   buffer = malloc(len);
	   SDL_RWread(f,buffer,1,len);
	   stbi__start_mem(&s,buffer,len);
	   result = stbi__load_main(&s, x, y, comp, req_comp, &ri,8);
	   free(buffer);
	}
	return result;
}

/**
 * \breif 搜索顺序直到找到文件
 * \param searchpath 文件的搜索顺序，如"c:/abc/?;?",问号将被文件名替换。分号分隔一个搜索位置
 * \param filename 要打开的文件名
 * \param mode 打开方式rw
 * \return 成功返回一个SDL的文件访问句柄。
 */
SDL_RWops * SDL_searchFile(const char * searchpath,const char *filename, const char * mode)
{
	int i, j,len;
	SDL_RWops *fp;
	char f[MAX_PATH];
	char c;

	len = strlen(filename);
	j = i = 0;
	fp = NULL;
	for (;;){
		c = searchpath[i++];
		if (c == 0 || c == ';'){
			f[j] = 0;
			fp = SDL_RWFromFile(f, mode);
			if (c == 0 || fp){
				break;
			}
			j = 0;
		}
		else if (c == '?'){
			strcpy(f + j, filename);
			j += len;
		}
		else{
			f[j++] = c;
		}
	}
	return fp;
}

STBIDEF unsigned char *stbi_load2(char const *filename, int *x, int *y, int *comp, int req_comp)
{
	SDL_RWops *f = SDL_searchFile(getFileSearchPath(),filename, "rb");
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
	fp = SDL_searchFile(getFileSearchPath(), path, "rb");
	if (fp == NULL) goto error;
	dataSize = (int)SDL_RWsize(fp);
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

static int fons__HasCodepoint(stbtt_fontinfo *font, const char * str)
{
	unsigned int codepoint;
	unsigned int utf8state = 0;
	const char * end = str + strlen(str);
	for (; str != end; ++str){
		if (fons__decutf8(&utf8state, &codepoint, *(const unsigned char*)str))
			continue;

		if (!stbtt_FindGlyphIndex(font, codepoint))
			return 0;
	}
	return 1;
}
/**
 * \brief 检查看看字体文件中包括指定的字符集吗
 * \param file		字体文件
 * \param utf8s		要检查的字符集(UTF8)
 * \param result	监测结果集，不支持为0，支持为1
 * \param len		字符集数量
 * \return 如果字体文件被正常打开就返回1,否则返回0
 */
int fonsHasCodepoint(const char * file, const char ** utf8s,int * ret,int len)
{
	SDL_RWops *fp;
	int dataSize = 0;
	unsigned char* data = NULL;
	stbtt_fontinfo font;
	int i,result = 0;

	// Read in the font data.
	fp = SDL_searchFile(getFileSearchPath(), file, "rb");
	if (fp == NULL) goto error;
	dataSize = (int)SDL_RWsize(fp);
	data = (unsigned char*)malloc(dataSize);
	if (data == NULL) goto error;
	SDL_RWread(fp, data, 1, dataSize);
	SDL_RWclose(fp);
	fp = 0;

	if (data[0] == 't' && data[1] == 't' && data[2] == 'c' && data[3] == 'f'){
		result = stbtt_InitFont(&font, data, stbtt_GetFontOffsetForIndex(data, 0));
	}
	else{
		result = stbtt_InitFont(&font, data, 0);
	}
	if (!result)goto error;

	for (i = 0; i < len; i++){
		if (utf8s[i])
			ret[i] = fons__HasCodepoint(&font, utf8s[i]);
		else
			ret[i] = 0;
	}
error:
	if (data) free(data);
	if (fp) SDL_RWclose(fp);
	return result;
}