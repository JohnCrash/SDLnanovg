#ifndef _EVENTHANDLER_H_
#define _EVENTHANDLER_H_

typedef struct
{
	int nEvent;
	SDL_Event events[16];
} SDLEventBuffer;

typedef struct {
	int type;

} simpleEvent;

int eventLoop(SDLState *state);

int getSDLEventCount();
SDL_Event * getSDLEvent(int n);
#endif