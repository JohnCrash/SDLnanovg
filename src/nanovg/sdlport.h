#ifndef _SDLPORT_H_
#define _SDLPORT_H_
#include "SDL.h"
#include "fontstash.h"

SDL_RWops * SDL_searchFile(const char * searchpath, const char *filename, const char * mode);
unsigned char *stbi_load2(char const *filename, int *x, int *y, int *comp, int req_comp);
int fonsAddFont2(FONScontext* stash, const char* name, const char* path);
#endif