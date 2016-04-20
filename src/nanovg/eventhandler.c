#include "SDL.h"
#include "sdlmain.h"
#include "eventhandler.h"
#include "luaext.h"
#include "ui.h"

int eventLoop(SDLState *state)
{
	SDL_Event event;
	inputEvent ie;

	while (SDL_PollEvent(&event))
	{
		ie.type = 0;
		switch (event.type)
		{
		case SDL_WINDOWEVENT:
			if (event.window.event == SDL_WINDOWEVENT_CLOSE)
			{
				return 1;
			}
			else if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
			{
				uiWidget * root = uiRootWidget();
				state->window_w = event.window.data1;
				state->window_h = event.window.data2;
				uiSetSize(root, state->window_w, state->window_h);
			}
			break;
		case SDL_MOUSEMOTION:
			ie.type = MOUSE_MOVE;
			ie.x = event.motion.x;
			ie.y = event.motion.y;
			break;
		case SDL_MOUSEBUTTONDOWN:
			ie.type = MOUSE_DOWN;
			ie.button = event.button.button;
			ie.x = event.button.x;
			ie.y = event.button.y;
			break;
		case SDL_MOUSEBUTTONUP:
			ie.type = MOUSE_UP;
			ie.button = event.button.button;
			ie.x = event.button.x;
			ie.y = event.button.y;
			break;
		case SDL_KEYDOWN:
			ie.type = KEY_DOWN;
			ie.key = event.key.keysym.scancode;
			break;
		case SDL_KEYUP:
			ie.type = KEY_UP;
			ie.key = event.key.keysym.scancode;
			break;
		}
		if (ie.type)
			lua_EventInput(&ie);
	}
	return 0;
}