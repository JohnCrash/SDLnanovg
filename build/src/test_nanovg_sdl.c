//
// Copyright (c) 2013 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#include <stdio.h>
#include "SDL.h"
#include "gles.h"
#include "nanovg.h"
#include "nanovg_gl.h"
#include "nanovg_gl_utils.h"
#include "demo.h"
#include "perf.h"

static int winWidth, winHeight;
static DemoData data;
static NVGcontext* vg = NULL;
static PerfGraph fps;
static double prevt = 0;
static double mx, my, t, dt;
static int fbWidth, fbHeight;
static float pxRatio;
static int blowup = 0;
static int premult = 0;

int initNanovg()
{
	initGraph(&fps, GRAPH_RENDER_FPS, "Frame Time");
#ifdef DEMO_MSAA
	vg = nvgCreateGL2(NVG_STENCIL_STROKES | NVG_DEBUG);
#else
	#ifdef __GLES__
		//vg = nvgCreateGLES2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
		vg = nvgCreateGLES2(NVG_STENCIL_STROKES | NVG_DEBUG);
	#else
		vg = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
	#endif
#endif
	if (vg == NULL) {
		printf("Could not init nanovg.\n");
		return -1;
	}

	if (loadDemoData(vg, &data) == -1)
		return -1;

	pxRatio = (float)fbWidth / (float)winWidth;
	prevt = SDL_GetTicks() / 1000.0;
	return 0;
}

void renderNanovg(int _mx, int _my, int w, int h)
{
	t = SDL_GetTicks()/1000.0;
	dt = t - prevt;
	prevt = t;
	updateGraph(&fps, dt);

	mx = _mx;
	my = _my;
	fbWidth = w;
	fbHeight = h;
	winWidth = w;
	winHeight = h;
	pxRatio = (float)fbWidth / (float)winWidth;

	glViewport(0, 0, fbWidth, fbHeight);
	if (premult)
		glClearColor(0, 0, 0, 0);
	else
		glClearColor(0.3f, 0.3f, 0.32f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	nvgBeginFrame(vg, winWidth, winHeight, pxRatio);
	renderDemo(vg, mx, my, winWidth, winHeight, t, blowup, &data);
	renderGraph(vg, 5, 5, &fps);
	nvgEndFrame(vg);
}

void releaseNanovg()
{
	freeDemoData(vg, &data);
	#ifdef __GLES__
		nvgDeleteGLES2(vg);
	#else
		nvgDeleteGL2(vg);
	#endif
}

void* nanovgContext()
{
	return vg;
}