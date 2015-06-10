#ifndef _SDLMAIN_H_
#define _SDLMAIN_H_
#include "SDL.h"

#ifdef __cplusplus
extern "C"{
#endif
	typedef struct{
		const char *window_title;
		const char *window_icon;
		Uint32 window_flags;
		int window_x;
		int window_h;
		int window_w;
		int window_h;
		int window_minW;
		int window_minH;
		int window_maxW;
		int window_maxH;
	} SDLState;
#ifdef __cplusplus
}
#endif
#endif