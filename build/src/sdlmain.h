#ifndef _SDLMAIN_H_
#define _SDLMAIN_H_
#include "SDL.h"

#define DEFAULT_WINDOW_WIDTH  640
#define DEFAULT_WINDOW_HEIGHT 480

#define VERBOSE_VIDEO   0x00000001
#define VERBOSE_MODES   0x00000002
#define VERBOSE_RENDER  0x00000004
#define VERBOSE_EVENT   0x00000008
#define VERBOSE_AUDIO   0x00000010

#ifdef __cplusplus
extern "C"{
#endif
	typedef struct{
		/* SDL init flags */
		int argc;
		char **argv;
		Uint32 flags;
		Uint32 verbose;

		/* Video info */
		const char *videodriver;
		const char *window_title;
		const char *window_icon;
		Uint32 window_flags;
		int window_x;
		int window_y;
		int window_w;
		int window_h;
		int window_minW;
		int window_minH;
		int window_maxW;
		int window_maxH;

		/* Audio info */
		const char *audiodriver;
		SDL_AudioSpec audiospec;

	} SDLState;

	SDLState *defaultSDLState(int argc, char **argv);
	int initSDL(SDLState *state);
	void releaseSDL(SDLState *state);
#ifdef __cplusplus
}
#endif
#endif