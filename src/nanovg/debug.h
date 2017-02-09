#ifndef __DEBUG_H__
#define __DEBUG_H__
#include "SDL.h"
#include "ui.h"

void dumpEvent(uiEvent * pev);
void dumpSDLEvent(SDL_Event * pe);
#endif