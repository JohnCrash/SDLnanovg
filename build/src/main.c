/*
Copyright (C) 1997-2014 Sam Lantinga <slouken@libsdl.org>

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "SDL.h"
#include "gles.h"
#include "sdlmain.h"
#include "test_nanovg_sdl.h"
#include "fs.h"

int main(int argc,char *argv[])
{
	int done, mx, my;
	SDL_Event event;
	SDLState * state = createSDLState(argc, argv);

	/* Enable standard application logging */
	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
	SDL_Log("initSDL..");
	SDL_Log("Writeable directory : %s", getWriteDirectory());
	if (!initSDL(state))
	{
		SDL_Log("initSDL failed!\n");
		return -1;
	}
#if !defined(__GLES__)
	SDL_Log("glewInit..");
	if (glewInit()!=GLEW_OK)
	{
		SDL_Log("glewInit failed!");
		return -1;
	}
#endif	
	SDL_Log("initNanovg..");
	if (initNanovg()<0)
	{
		SDL_Log("initNanovg failed!\n");
		return -1;
	}
	done = 0;
	mx = my = 0;
	SDL_Log("Main loop..");
	while (!done)
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_CLOSE)
				{
					SDL_DestroyWindow(state->window);
					done = 1;
				}
				else if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{
					state->window_w = event.window.data1;
					state->window_h = event.window.data2;
				}
				break;
			case SDL_MOUSEMOTION:
				mx = event.motion.x;
				my = event.motion.y;
				break;
			}
		}
		renderNanovg(mx,my,state->window_w,state->window_h);
		SDL_GL_SwapWindow(state->window);
	}
	SDL_Log("releaseNanovg..");
	releaseNanovg();
	SDL_Log("releaseSDL..");
	releaseSDL(state);
	SDL_Log("DONE..");
	return 0;
}
