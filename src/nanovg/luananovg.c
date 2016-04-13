#include "lua.h"
#include "lauxlib.h"
#include "luananovg.h"
#include "SDL.h"
#include "gles.h"
#include "nanovg.h"
#include "nanovg_sdl.h"
#include "sdlmain.h"

/*
 * 
 */
static int nanovgRenderTable(lua_State *L, int n)
{
	return 0;
}
/*
 * Ê¹ÓÃnanovgäÖÈ¾
 * {
 *		{ fill,stroke,text,trasform,}
  * }
 */
int lua_nanovgRender(lua_State *L)
{
	int fbWidth, fbHeight;
	int winWidth, winHeight;
	float pxRatio;
	NVGcontext * vg = nanovgContext();
	SDLState * state = getSDLState();

	if (state&&vg){
		fbWidth = winWidth = state->window_w;
		fbHeight = winHeight = state->window_h;

		pxRatio = (float)fbWidth / (float)winWidth;
		glViewport(0, 0, fbWidth, fbHeight);
		nvgBeginFrame(vg, winWidth, winHeight, pxRatio);
		nanovgRenderTable(L, 1);
		nvgEndFrame(vg);
	}
	return 0;
}