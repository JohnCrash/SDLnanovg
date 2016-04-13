#include "sdlmain.h"
#include <stdio.h>

static SDLState *_state = NULL;

SDLState *getSDLState()
{
	return _state;
}
/*
 *	代码来自于SDLTest,主要进行梳理和修改。
 */
SDLState *createSDLState(int argc,char **argv)
{
	SDLState *state = (SDLState *)SDL_calloc(1,sizeof(*state));
	 if (!state) {
        SDL_OutOfMemory();
        return NULL;
    }
	 SDL_zerop(state);

	state->argc = argc;
	state->argv = argv;

	state->flags = SDL_INIT_VIDEO;

	/* 默认窗口设置 */
	state->window_title = "OpenGL Test Window";
	state->window_icon = "icon.bmp";
	state->window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
	state->window_x = 100;
	state->window_y = 100;
	state->window_w = DEFAULT_WINDOW_WIDTH;
	state->window_h = DEFAULT_WINDOW_HEIGHT;

	/* 默认声音设置 */
	state->audiospec.freq = 22050;
	state->audiospec.format = AUDIO_S16;
	state->audiospec.channels = 2;
	state->audiospec.samples = 2048;

	/* OpenGL 默认设置 */
	state->gl_red_size = 3;
	state->gl_green_size = 3;
	state->gl_blue_size = 2;
	state->gl_alpha_size = 0;
	state->gl_blue_size = 0;
	state->gl_depth_size = 16;
	state->gl_stencil_size = 1;
	state->gl_double_buffer = 1;
	state->gl_accum_red_size = 0;
	state->gl_accum_green_size = 0;
	state->gl_accum_blue_size = 0;
	state->gl_accum_alpha_size = 0;
	state->gl_stereo = 0;
	state->gl_multisamplebuffers = 0;
	state->gl_multisamplesamples = 0;
	state->gl_retained_backing = 1;
	state->gl_accelerated = -1;
	state->gl_debug = 0;
#ifdef __ANDROID__
	state->gl_profile_mask = SDL_GL_CONTEXT_PROFILE_ES;
#else
	state->gl_profile_mask = SDL_GL_CONTEXT_PROFILE_CORE;
#endif
	_state = state;
	return state;
}

static SDL_Surface *
SDLTest_LoadIcon(const char *file)
{
	SDL_Surface *icon;

	/* Load the icon surface */
	icon = SDL_LoadBMP(file);
	if (icon == NULL) {
		fprintf(stderr, "Couldn't load %s: %s\n", file, SDL_GetError());
		return (NULL);
	}

	if (icon->format->palette) {
		/* Set the colorkey */
		SDL_SetColorKey(icon, 1, *((Uint8 *)icon->pixels));
	}

	return (icon);
}

static void
SDLTest_PrintRendererFlag(Uint32 flag)
{
	switch (flag) {
	case SDL_RENDERER_PRESENTVSYNC:
		fprintf(stderr, "PresentVSync");
		break;
	case SDL_RENDERER_ACCELERATED:
		fprintf(stderr, "Accelerated");
		break;
	default:
		fprintf(stderr, "0x%8.8x", flag);
		break;
	}
}

static void
SDLTest_PrintPixelFormat(Uint32 format)
{
	switch (format) {
	case SDL_PIXELFORMAT_UNKNOWN:
		fprintf(stderr, "Unknwon");
		break;
	case SDL_PIXELFORMAT_INDEX1LSB:
		fprintf(stderr, "Index1LSB");
		break;
	case SDL_PIXELFORMAT_INDEX1MSB:
		fprintf(stderr, "Index1MSB");
		break;
	case SDL_PIXELFORMAT_INDEX4LSB:
		fprintf(stderr, "Index4LSB");
		break;
	case SDL_PIXELFORMAT_INDEX4MSB:
		fprintf(stderr, "Index4MSB");
		break;
	case SDL_PIXELFORMAT_INDEX8:
		fprintf(stderr, "Index8");
		break;
	case SDL_PIXELFORMAT_RGB332:
		fprintf(stderr, "RGB332");
		break;
	case SDL_PIXELFORMAT_RGB444:
		fprintf(stderr, "RGB444");
		break;
	case SDL_PIXELFORMAT_RGB555:
		fprintf(stderr, "RGB555");
		break;
	case SDL_PIXELFORMAT_BGR555:
		fprintf(stderr, "BGR555");
		break;
	case SDL_PIXELFORMAT_ARGB4444:
		fprintf(stderr, "ARGB4444");
		break;
	case SDL_PIXELFORMAT_ABGR4444:
		fprintf(stderr, "ABGR4444");
		break;
	case SDL_PIXELFORMAT_ARGB1555:
		fprintf(stderr, "ARGB1555");
		break;
	case SDL_PIXELFORMAT_ABGR1555:
		fprintf(stderr, "ABGR1555");
		break;
	case SDL_PIXELFORMAT_RGB565:
		fprintf(stderr, "RGB565");
		break;
	case SDL_PIXELFORMAT_BGR565:
		fprintf(stderr, "BGR565");
		break;
	case SDL_PIXELFORMAT_RGB24:
		fprintf(stderr, "RGB24");
		break;
	case SDL_PIXELFORMAT_BGR24:
		fprintf(stderr, "BGR24");
		break;
	case SDL_PIXELFORMAT_RGB888:
		fprintf(stderr, "RGB888");
		break;
	case SDL_PIXELFORMAT_BGR888:
		fprintf(stderr, "BGR888");
		break;
	case SDL_PIXELFORMAT_ARGB8888:
		fprintf(stderr, "ARGB8888");
		break;
	case SDL_PIXELFORMAT_RGBA8888:
		fprintf(stderr, "RGBA8888");
		break;
	case SDL_PIXELFORMAT_ABGR8888:
		fprintf(stderr, "ABGR8888");
		break;
	case SDL_PIXELFORMAT_BGRA8888:
		fprintf(stderr, "BGRA8888");
		break;
	case SDL_PIXELFORMAT_ARGB2101010:
		fprintf(stderr, "ARGB2101010");
		break;
	case SDL_PIXELFORMAT_YV12:
		fprintf(stderr, "YV12");
		break;
	case SDL_PIXELFORMAT_IYUV:
		fprintf(stderr, "IYUV");
		break;
	case SDL_PIXELFORMAT_YUY2:
		fprintf(stderr, "YUY2");
		break;
	case SDL_PIXELFORMAT_UYVY:
		fprintf(stderr, "UYVY");
		break;
	case SDL_PIXELFORMAT_YVYU:
		fprintf(stderr, "YVYU");
		break;
	case SDL_PIXELFORMAT_NV12:
		fprintf(stderr, "NV12");
		break;
	case SDL_PIXELFORMAT_NV21:
		fprintf(stderr, "NV21");
		break;
	default:
		fprintf(stderr, "0x%8.8x", format);
		break;
	}
}

static void
SDLTest_PrintRenderer(SDL_RendererInfo * info)
{
	int i, count;

	fprintf(stderr, "  Renderer %s:\n", info->name);

	fprintf(stderr, "    Flags: 0x%8.8X", info->flags);
	fprintf(stderr, " (");
	count = 0;
	for (i = 0; i < sizeof(info->flags) * 8; ++i) {
		Uint32 flag = (1 << i);
		if (info->flags & flag) {
			if (count > 0) {
				fprintf(stderr, " | ");
			}
			SDLTest_PrintRendererFlag(flag);
			++count;
		}
	}
	fprintf(stderr, ")\n");

	fprintf(stderr, "    Texture formats (%d): ", info->num_texture_formats);
	for (i = 0; i < (int)info->num_texture_formats; ++i) {
		if (i > 0) {
			fprintf(stderr, ", ");
		}
		SDLTest_PrintPixelFormat(info->texture_formats[i]);
	}
	fprintf(stderr, "\n");

	if (info->max_texture_width || info->max_texture_height) {
		fprintf(stderr, "    Max Texture Size: %dx%d\n",
			info->max_texture_width, info->max_texture_height);
	}
}

static void verboseModes(SDLState *state)
{
	int n, i,j,m;
	if (state->verbose & VERBOSE_MODES) {
		SDL_Rect bounds;
		SDL_DisplayMode mode;
		int bpp;
		Uint32 Rmask, Gmask, Bmask, Amask;
#if SDL_VIDEO_DRIVER_WINDOWS
		int adapterIndex = 0;
		int outputIndex = 0;
#endif
		n = SDL_GetNumVideoDisplays();
		fprintf(stderr, "Number of displays: %d\n", n);
		for (i = 0; i < n; ++i) {
			fprintf(stderr, "Display %d: %s\n", i, SDL_GetDisplayName(i));

			SDL_zero(bounds);
			SDL_GetDisplayBounds(i, &bounds);
			fprintf(stderr, "Bounds: %dx%d at %d,%d\n", bounds.w, bounds.h, bounds.x, bounds.y);

			SDL_GetDesktopDisplayMode(i, &mode);
			SDL_PixelFormatEnumToMasks(mode.format, &bpp, &Rmask, &Gmask,
				&Bmask, &Amask);
			fprintf(stderr,
				"  Current mode: %dx%d@%dHz, %d bits-per-pixel (%s)\n",
				mode.w, mode.h, mode.refresh_rate, bpp,
				SDL_GetPixelFormatName(mode.format));
			if (Rmask || Gmask || Bmask) {
				fprintf(stderr, "      Red Mask   = 0x%.8x\n", Rmask);
				fprintf(stderr, "      Green Mask = 0x%.8x\n", Gmask);
				fprintf(stderr, "      Blue Mask  = 0x%.8x\n", Bmask);
				if (Amask)
					fprintf(stderr, "      Alpha Mask = 0x%.8x\n", Amask);
			}

			/* Print available fullscreen video modes */
			m = SDL_GetNumDisplayModes(i);
			if (m == 0) {
				fprintf(stderr, "No available fullscreen video modes\n");
			}
			else {
				fprintf(stderr, "  Fullscreen video modes:\n");
				for (j = 0; j < m; ++j) {
					SDL_GetDisplayMode(i, j, &mode);
					SDL_PixelFormatEnumToMasks(mode.format, &bpp, &Rmask,
						&Gmask, &Bmask, &Amask);
					fprintf(stderr,
						"    Mode %d: %dx%d@%dHz, %d bits-per-pixel (%s)\n",
						j, mode.w, mode.h, mode.refresh_rate, bpp,
						SDL_GetPixelFormatName(mode.format));
					if (Rmask || Gmask || Bmask) {
						fprintf(stderr, "        Red Mask   = 0x%.8x\n",
							Rmask);
						fprintf(stderr, "        Green Mask = 0x%.8x\n",
							Gmask);
						fprintf(stderr, "        Blue Mask  = 0x%.8x\n",
							Bmask);
						if (Amask)
							fprintf(stderr,
							"        Alpha Mask = 0x%.8x\n",
							Amask);
					}
				}
			}

#if SDL_VIDEO_DRIVER_WINDOWS
			/* Print the D3D9 adapter index */
			adapterIndex = SDL_Direct3D9GetAdapterIndex(i);
			fprintf(stderr, "D3D9 Adapter Index: %d", adapterIndex);

			/* Print the DXGI adapter and output indices */
			SDL_DXGIGetOutputInfo(i, &adapterIndex, &outputIndex);
			fprintf(stderr, "DXGI Adapter Index: %d  Output Index: %d", adapterIndex, outputIndex);
#endif
		}
	}
}

static int initSDLVideo(SDLState *state)
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
	return 	SDL_TRUE;
}

static int initSDLWindow(SDLState *state)
{
	int i,n,w,h;
	SDL_DisplayMode fullscreen_mode;
	char title[512];
	
	/* 创建主窗口 */
	if (state->window_title)
		SDL_strlcpy(title, state->window_title, SDL_arraysize(title));
	else
		title[0] = 0;
	state->window = SDL_CreateWindow(title, state->window_x, state->window_y,
		state->window_w, state->window_h, state->window_flags);
	if (!state->window) {
		fprintf(stderr, "Couldn't create window: %s\n",
			SDL_GetError());
		return SDL_FALSE;
	}
	if (state->window_minW || state->window_minH) {
		SDL_SetWindowMinimumSize(state->window, state->window_minW, state->window_minH);
	}
	if (state->window_maxW || state->window_maxH) {
		SDL_SetWindowMaximumSize(state->window, state->window_maxW, state->window_maxH);
	}
	SDL_GetWindowSize(state->window, &w, &h);
	if (!(state->window_flags & SDL_WINDOW_RESIZABLE) &&
		(w != state->window_w || h != state->window_h)) {
		printf("Window requested size %dx%d, got %dx%d\n", state->window_w, state->window_h, w, h);
		state->window_w = w;
		state->window_h = h;
	}
	if (state->verbose & VERBOSE_RENDER) {
		SDL_RendererInfo info;

		n = SDL_GetNumRenderDrivers();
		if (n == 0) {
			fprintf(stderr, "No built-in render drivers\n");
		}
		else {
			fprintf(stderr, "Built-in render drivers:\n");
			for (i = 0; i < n; ++i) {
				SDL_GetRenderDriverInfo(i, &info);
				SDLTest_PrintRenderer(&info);
			}
		}
	}
	/* 初始化窗口 */
	SDL_zero(fullscreen_mode);
	switch (state->depth) {
	case 8:
		fullscreen_mode.format = SDL_PIXELFORMAT_INDEX8;
		break;
	case 15:
		fullscreen_mode.format = SDL_PIXELFORMAT_RGB555;
		break;
	case 16:
		fullscreen_mode.format = SDL_PIXELFORMAT_RGB565;
		break;
	case 24:
		fullscreen_mode.format = SDL_PIXELFORMAT_RGB24;
		break;
	default:
		fullscreen_mode.format = SDL_PIXELFORMAT_RGB888;
		break;
	}
	fullscreen_mode.refresh_rate = state->refresh_rate;
	if (SDL_SetWindowDisplayMode(state->window, &fullscreen_mode) < 0) {
		fprintf(stderr, "Can't set up fullscreen display mode: %s\n",
			SDL_GetError());
		return SDL_FALSE;
	}

	if (state->window_icon) {
		SDL_Surface *icon = SDLTest_LoadIcon(state->window_icon);
		if (icon) {
			SDL_SetWindowIcon(state->window, icon);
			SDL_FreeSurface(icon);
		}
	}
	SDL_ShowWindow(state->window);
	return SDL_TRUE;
}

static int initSDLRenderer(SDLState *state)
{
	int j,m,n;
	if (state->renderdriver || !(state->window_flags & SDL_WINDOW_OPENGL)){
		m = -1;
		if (state->renderdriver) {
			SDL_RendererInfo info;
			n = SDL_GetNumRenderDrivers();
			for (j = 0; j < n; ++j) {
				SDL_GetRenderDriverInfo(j, &info);
				if (SDL_strcasecmp(info.name, state->renderdriver) ==
					0) {
					m = j;
					break;
				}
			}
			if (m == -1) {
				fprintf(stderr,
					"Couldn't find render driver named %s",
					state->renderdriver);
				return SDL_FALSE;
			}
		}
		state->renderer = SDL_CreateRenderer(state->window,
			m, state->render_flags);
		if (!state->renderer) {
			fprintf(stderr, "Couldn't create renderer: %s\n",
				SDL_GetError());
			return SDL_FALSE;
		}
		if (state->logical_w && state->logical_h) {
			SDL_RenderSetLogicalSize(state->renderer, state->logical_w, state->logical_h);
		}
		else if (state->scale) {
			SDL_RenderSetScale(state->renderer, state->scale, state->scale);
		}
		if (state->verbose & VERBOSE_RENDER) {
			SDL_RendererInfo info;

			fprintf(stderr, "Current renderer:\n");
			SDL_GetRendererInfo(state->renderer, &info);
			SDLTest_PrintRenderer(&info);
		}
	}
	return SDL_TRUE;
}

/* 初始化声音驱动 */
static int initSDLAudio(SDLState *state)
{
	int i,n;
	if (state->flags & SDL_INIT_AUDIO) {
		if (state->verbose & VERBOSE_AUDIO) {
			n = SDL_GetNumAudioDrivers();
			if (n == 0) {
				fprintf(stderr, "No built-in audio drivers\n");
			}
			else {
				fprintf(stderr, "Built-in audio drivers:");
				for (i = 0; i < n; ++i) {
					if (i > 0) {
						fprintf(stderr, ",");
					}
					fprintf(stderr, " %s", SDL_GetAudioDriver(i));
				}
				fprintf(stderr, "\n");
			}
		}
		if (SDL_AudioInit(state->audiodriver) < 0) {
			fprintf(stderr, "Couldn't initialize audio driver: %s\n",
				SDL_GetError());
			return SDL_FALSE;
		}
		if (state->verbose & VERBOSE_VIDEO) {
			fprintf(stderr, "Audio driver: %s\n",
				SDL_GetCurrentAudioDriver());
		}

		if (SDL_OpenAudio(&state->audiospec, NULL) < 0) {
			fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
			return SDL_FALSE;
		}
	}
	return 	SDL_TRUE;
}

/* 创建OpenGL上行文 */
static int initSDLGL(SDLState *state)
{
	int status;
	state->context = SDL_GL_CreateContext(state->window);
	if (!state->context) {
		fprintf(stderr, "SDL_GL_CreateContext(): %s\n", SDL_GetError());
		return SDL_FALSE;
	}
	status = SDL_GL_MakeCurrent(state->window, state->context);
	if (status) {
		fprintf(stderr, "SDL_GL_MakeCurrent(): %s\n", SDL_GetError());
		return SDL_FALSE;
	}
	if (state->render_flags & SDL_RENDERER_PRESENTVSYNC) {
		SDL_GL_SetSwapInterval(1);
	}
	else {
		SDL_GL_SetSwapInterval(0);
	}	
	return SDL_TRUE;
}

int initSDL(SDLState *state)
{
	if(!initSDLVideo(state))
		return SDL_FALSE;
	
	/* Upload GL settings */
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, state->gl_red_size);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, state->gl_green_size);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, state->gl_blue_size);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, state->gl_alpha_size);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, state->gl_double_buffer);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, state->gl_buffer_size);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, state->gl_depth_size);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, state->gl_stencil_size);

	SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, state->gl_accum_red_size);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, state->gl_accum_green_size);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, state->gl_accum_blue_size);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, state->gl_accum_alpha_size);
	SDL_GL_SetAttribute(SDL_GL_STEREO, state->gl_stereo);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, state->gl_multisamplebuffers);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, state->gl_multisamplesamples);
	if (state->gl_accelerated >= 0) {
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL,
			state->gl_accelerated);
	}
	SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, state->gl_retained_backing);
	if (state->gl_major_version) {
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, state->gl_major_version);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, state->gl_minor_version);
	}
	if (state->gl_debug) {
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	}
	if (state->gl_profile_mask) {
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, state->gl_profile_mask);
	}

	verboseModes(state);

	if(!initSDLWindow(state))
		return SDL_FALSE;
	
	/* 初始化renderer */
	if(!initSDLRenderer(state))
		return SDL_FALSE;
	
	if(!initSDLGL(state))
		return SDL_FALSE;
	
	if(!initSDLAudio(state))
		return SDL_FALSE;
	
	return SDL_TRUE;
}

void releaseSDL(SDLState *state)
{
	if( state->window )
		SDL_DestroyWindow(state->window);
	if( state->target )
		SDL_DestroyTexture(state->target);
	if(state->renderer)
	{
		SDL_DestroyRenderer(state->renderer);
		SDL_free(state->renderer);
	}
    if (state->flags & SDL_INIT_VIDEO) {
        SDL_VideoQuit();
    }
    if (state->flags & SDL_INIT_AUDIO) {
        SDL_AudioQuit();
    }
	SDL_free(state);
}