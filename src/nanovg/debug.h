#ifndef __DEBUG_H__
#define __DEBUG_H__
#include "SDL.h"
#include "ui.h"

void dumpEvent(uiEvent * pev);
void dumpSDLEvent(SDL_Event * pe);

#if DBG
	#define DEBUG(format,...) SDL_Log(format,##__VA_ARGS__);
#else
	#define DEBUG(format,...)
#endif

#endif