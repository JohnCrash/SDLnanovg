#include "sdlmain.h"
#include <stdio.h>

static SDLState _state;

SDLState *defaultSDLState(int argc,char **argv)
{
	memset(&_state, 0, sizeof(_state));

	_state.argc = argc;
	_state.argv = argv;

	_state.flags = SDL_INIT_VIDEO;

	_state.window_flags = 0;
	_state.window_x = 0;
	_state.window_y = 0;
	_state.window_w = DEFAULT_WINDOW_WIDTH;
	_state.window_h = DEFAULT_WINDOW_HEIGHT;

	_state.audiospec.freq = 22050;
	_state.audiospec.format = AUDIO_S16;
	_state.audiospec.channels = 2;
	_state.audiospec.samples = 2048;
	return &_state;
}

int initSDL(SDLState *state)
{
	int i,n;
	if (state->flags & SDL_INIT_VIDEO) {
		if (state->verbose & VERBOSE_VIDEO) {
			n = SDL_GetNumVideoDrivers();
			if (n == 0) {
				fprintf(stderr, "No built-in video drivers\n");
			}
			else {
				fprintf(stderr, "Built-in video drivers:");
				for (i = 0; i < n; ++i) {
					if (i > 0) {
						fprintf(stderr, ",");
					}
					fprintf(stderr, " %s", SDL_GetVideoDriver(i));
				}
				fprintf(stderr, "\n");
			}
		}
		if (SDL_VideoInit(state->videodriver) < 0) {
			fprintf(stderr, "Couldn't initialize video driver: %s\n",
				SDL_GetError());
			return SDL_FALSE;
		}
		if (state->verbose & VERBOSE_VIDEO) {
			fprintf(stderr, "Video driver: %s\n",
				SDL_GetCurrentVideoDriver());
		}
	}
}

void releaseSDL(SDLState *state)
{

}