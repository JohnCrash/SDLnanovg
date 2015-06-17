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
#define NANOVG_GL2_IMPLEMENTATION
#if defined(__IPHONEOS__) || defined(__ANDROID__)
#include "SDL_opengles.h"
#else
//#include "SDL_opengl.h"
#include "GL/glew.h"
#endif
#include "SDL.h"
#include "sdlmain.h"
#include "test_nanovg_sdl.h"

int main(int argc,char *argv[])
{
	int done;
	SDL_Event event;
	SDLState * state = defaultSDLState(argc, argv);

	/* Enable standard application logging */
	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

	if (!initSDL(state))
	{
		printf("initSDL failed!\n");
		return -1;
	}
	if (!initNanovg())
	{
		printf("initNanovg failed!\n");
		return -1;
	}
	done = 0;
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
				break;
			}
		}
		renderNanovg(state->window_w,state->window_h);
		SDL_GL_SwapWindow(state->window);
	}

	releaseNanovg();
	releaseSDL(state);
	return 0;
}
