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

static int _background = 0;

int eventLoop(SDLState *state)
{
	SDL_Event event;

	clearSDLEvent();
_continue:
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
				_background = 1;
				lua_EventWindow("background");
			}
			else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED){
				_background = 0;
				lua_EventWindow("foreground");
			}
			break;
		case SDL_TEXTEDITING:
		//	printf("EDITING start: %d,'%s' %d\n",
		//		event.edit.start,event.edit.text,event.edit.length);
			break;
		case SDL_TEXTINPUT:
		//	printf("TEXTINPUT: %s\n",event.text.text);
			lua_callKeyboardFunc("insert",event.text.text);
			break;
		case SDL_KEYDOWN:
			{
				const char * key = NULL;
				switch (event.key.keysym.scancode){
				case SDL_SCANCODE_HOME:
					key = "home";
					break;
				case SDL_SCANCODE_DELETE:
					key = "delete";
					break;
				case SDL_SCANCODE_BACKSPACE:
					key = "backspace";
					break;
				case SDL_SCANCODE_END:
					key = "end";
					break;
				case SDL_SCANCODE_LEFT:
					key = "left";
					break;
				case SDL_SCANCODE_RIGHT:
					key = "right";
					break;
				case SDL_SCANCODE_DOWN:
					key = "down";
					break;
				case SDL_SCANCODE_UP:
					key = "up";
					break;
				case SDL_SCANCODE_TAB:
					key = "tab";
					break;
				}
				lua_callKeyboardFunc2("keydown",event.key.keysym.scancode);
				if (key) lua_callKeyboardFunc(key,NULL);
			}
			break;
		case SDL_KEYUP:
			lua_callKeyboardFunc2("keyup", event.key.keysym.scancode);
			break;
		}
		if (!addSDLEvent(&event))
			return 0;
	}
	if (_background && getSDLEventCount()==0){
		//SDL_Delay(100);
		SDL_WaitEvent(NULL);
		goto _continue;
	}
	return 0;
}