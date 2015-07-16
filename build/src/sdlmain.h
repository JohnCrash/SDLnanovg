#ifndef _SDLMAIN_H_
#define _SDLMAIN_H_
#include "SDL.h"

#define DEFAULT_WINDOW_WIDTH  1024
#define DEFAULT_WINDOW_HEIGHT 576

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
		int logical_w;
		int logical_h;
		float scale;
		int depth;
		int refresh_rate;
		SDL_Window *window;

		/* Renderer info */
		const char *renderdriver;
		Uint32 render_flags;
		SDL_Renderer *renderer;
		SDL_Texture *target;

		/* Audio info */
		const char *audiodriver;
		SDL_AudioSpec audiospec;

		/*
		 * opengl ≥ı ºªØ…Ë÷√
		 */
		int gl_red_size;
		int gl_green_size;
		int gl_blue_size;
		int gl_alpha_size;
		int gl_buffer_size;
		int gl_depth_size;
		int gl_stencil_size;
		int gl_double_buffer;
		int gl_accum_red_size;
		int gl_accum_green_size;
		int gl_accum_blue_size;
		int gl_accum_alpha_size;
		int gl_stereo;
		int gl_multisamplebuffers;
		int gl_multisamplesamples;
		int gl_retained_backing;
		int gl_accelerated;

		int gl_major_version;
		int gl_minor_version;
		int gl_debug;
		int gl_profile_mask;
		SDL_GLContext *context;
	} SDLState;

	SDLState *createSDLState(int argc, char **argv);
	int initSDL(SDLState *state);
	void releaseSDL(SDLState *state);
#ifdef __cplusplus
}
#endif
#endif