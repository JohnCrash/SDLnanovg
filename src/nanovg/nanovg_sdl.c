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
#include "nanovg_sdl.h"

NVGcontext* _vg = NULL;

int initNanovg()
{
#ifdef DEMO_MSAA
	vg = nvgCreateGL2(NVG_STENCIL_STROKES | NVG_DEBUG);
#else
#ifdef __GLES__
	//vg = nvgCreateGLES2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
	_vg = nvgCreateGLES2(NVG_STENCIL_STROKES | NVG_DEBUG);
#else
	_vg = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
#endif
#endif
	if (_vg == NULL) {
		printf("Could not init nanovg.\n");
		return -1;
	}

	return 0;
}


void releaseNanovg()
{
#ifdef __GLES__
	nvgDeleteGLES2(_vg);
#else
	nvgDeleteGL2(_vg);
#endif
}

void* nanovgContext()
{
	return _vg;
}