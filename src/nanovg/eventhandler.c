#include "SDL.h"
#include "sdlmain.h"
#include "eventhandler.h"
#include "luaext.h"
#include "ui.h"

SDLEventBuffer _eventBuffer;

static void clearSDLEvent()
{
	_eventBuffer.nEvent = 0;
}

static int addSDLEvent(SDL_Event *ev)
{
	if (_eventBuffer.nEvent >= sizeof(_eventBuffer.events) / sizeof(SDL_Event)){
		return 0;
	}
	_eventBuffer.events[_eventBuffer.nEvent++] = *ev;
	return 1;
}

int getSDLEventCount()
{
	return _eventBuffer.nEvent;
}

SDL_Event * getSDLEvent(int n)
{
	return &(_eventBuffer.events[n]);
}

int eventLoop(SDLState *state)
{
	SDL_Event event;

	clearSDLEvent();
	while (SDL_PollEvent(&event))
	{
		switch (event.type){
		case SDL_WINDOWEVENT:
			if (event.window.event == SDL_WINDOWEVENT_CLOSE){
				lua_EventWindowClose();
				return 1;
			}
			else if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
				state->window_w = event.window.data1;
				state->window_h = event.window.data2;
				lua_EventChangeSize(state->window_w, state->window_h);
			}
			else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST){
				lua_EventWindow("background");
			}
			else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED){
				lua_EventWindow("foreground");
			}
			break;
		case SDL_TEXTEDITING:
			printf("EDITING start: %d,'%s' %d\n",
				event.edit.start,event.edit.text,event.edit.length);
			break;
		case SDL_TEXTINPUT:
			printf("TEXTINPUT: %s\n",event.text.text);
			lua_callKeyboardFunc(event.text.text);
			break;
		}
		if (!addSDLEvent(&event))
			return 0;
	}
	return 0;
}