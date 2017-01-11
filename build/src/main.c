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
#include "nanovg_sdl.h"
#include "fs.h"
#include "eventhandler.h"
#include "luaext.h"
#include "ui.h"

#if defined(_WIN32) && defined(_DEBUG)
#include <Windows.h>
static void openConsole()
{
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
}
#endif

#if defined(__ANDROID__)
int SDL_main(int argc, char *argv[])
#else
int main(int argc,char *argv[])
#endif
{
	Uint32 t,t0;
	SDLState * state;

#ifdef defined(_WIN32) && defined(_DEBUG)
	openConsole();
rerun:
#endif
	state = createSDLState(argc, argv);
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
	SDL_Log("initLua..");
	if (!initLua())
	{
		SDL_Log("initLua failed!\n");
		return -1;
	}
	if (!initUI())
	{
		SDL_Log("initUI failed!\n");
		return -1;
	}
	lua_EventInit();
	SDL_Log("Main loop..");
	t0 = SDL_GetTicks();
	while (!eventLoop(state))
	{
		glClearColor(0.3f, 0.3f, 0.32f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, state->window_w,state->window_h);
		t = SDL_GetTicks();
		lua_EventLoop((double)(t - t0) / 1000.0);
		uiLoop();
		t0 = t;
		SDL_GL_SwapWindow(state->window);
	}
	lua_EventRelease();
	SDL_Log("releaseLua..");
	releaseUI();
	releaseLua();
	SDL_Log("releaseNanovg..");
	releaseNanovg();
	SDL_Log("releaseSDL..");
	releaseSDL(state);
	SDL_Log("DONE..");
#ifdef defined(_WIN32) && defined(_DEBUG)
	goto rerun;
#endif
	return 0;
}
