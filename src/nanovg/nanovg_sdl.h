#ifndef _NANOVG_SDL_H_
#define _NANOVG_SDL_H_

int initNanovg();
void renderNanovg(int _mx, int _my, int w, int h);
void releaseNanovg();
void* nanovgContext();

#endif