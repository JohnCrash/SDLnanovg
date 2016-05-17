#include "luananovg.h"
#include "SDL.h"
#include "gles.h"
#include "nanovg.h"
#include "nanovg_sdl.h"
#include "sdlmain.h"
#include "eventhandler.h"

/**
 * \addtogroup VG
 * \brief lua 矢量绘制函数
 *
 * 这些函数都是在lua环境中调用的绘制函数。lua中的函数名称就是C函数名去掉lua_前缀。
 * 例如函数lua_beginNanoVG，的lua函数名称为beginNanoVG。
 * @{
 */
#if LUA_VERSION_NUM < 502
#  define luaL_newlib(L,l) (lua_newtable(L), luaL_register(L,NULL,l))
#endif

extern NVGcontext* _vg;
/**
 * 
 */
static int nanovgRenderTable(lua_State *L, int n)
{
	return 0;
}
/*
 * 使用nanovg渲染
 * {
 *		{ fill,stroke,text,trasform,}
  * }
 */
static int lua_nanovgRender(lua_State *L)
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

/**
 * \brief 设置渲染区域的宽度和高度
 * \param width 渲染Viewport的宽度
 * \param height 渲染Viewport的高度 
 */
static int lua_beginNanoVG(lua_State *L)
{
	int fbWidth, fbHeight;
	fbWidth = (int)luaL_checknumber(L, 1);
	fbHeight = (int)luaL_checknumber(L, 2);
	glViewport(0, 0, fbWidth, fbHeight);
	return 0;
}

static float lua_tableNumber(lua_State *L, int n, const char *key)
{
	float r;
	lua_getfield(L, n, key);
	r = (float)luaL_checknumber(L, -1);
	lua_pop(L, 1);
	return r;
}

/**
 * n位置的值是一个表{r,g,b,a}，没填充的用0补充
 * 如果n位置不是一个表返回0,否则返回1
 */
static int lua_toNVGcolor(lua_State *L, int n,NVGcolor *c)
{
	if (lua_istable(L, n)){
		c->r = lua_tableNumber(L, n, "r");
		c->g = lua_tableNumber(L, n, "g");
		c->b = lua_tableNumber(L, n, "b");
		c->a = lua_tableNumber(L, n, "a");
		return 1;
	}
	else{
		*c = nvgRGBA(0, 0, 0, 0);
	}
	return 0;
}

/**
 * NVGpaint分成4种类型
 * - LinearGradient
 * - BoxGradient
 * - RadialGradient
 * - ImagePattern
 */
static int lua_toNVGpaint(lua_State *L, int n, NVGpaint *p)
{
	if (lua_istable(L, n)){
		lua_getfield(L, n, "type");
		int type = (int)luaL_checknumber(L, -1);
		lua_pop(L, 1);
		switch (type){
		case 1:
			{
				  float sx, sy, ex, ey;
				  NVGcolor ico1, ico2;
				  sx = lua_tableNumber(L, n, "sx");
				  sy = lua_tableNumber(L, n, "sy");
				  ex = lua_tableNumber(L, n, "ex");
				  ey = lua_tableNumber(L, n, "ey");
				  lua_getfield(L, n, "ico1");
				  lua_toNVGcolor(L, -1,&ico1);
				  lua_pop(L, 1);
				  lua_getfield(L, n, "ico2");
				  lua_toNVGcolor(L, -1,&ico2);
				  lua_pop(L, 1);
				  *p = nvgLinearGradient(_vg, sx, sy, ex, ey, ico1, ico2);
			}
			break;
		case 2:
			{
				  float cx, cy, inr, outr;
				  NVGcolor icol, ocol;
				  cx = lua_tableNumber(L, n, "cx");
				  cy = lua_tableNumber(L, n, "cy");
				  inr = lua_tableNumber(L, n, "inr");
				  outr = lua_tableNumber(L, n, "outr");
				  lua_getfield(L, n, "icol");
				  lua_toNVGcolor(L, -1, &icol);
				  lua_pop(L, 1);
				  lua_getfield(L, n, "ocol");
				  lua_toNVGcolor(L, -1, &ocol);
				  lua_pop(L, 1);
				  *p = nvgRadialGradient(_vg, cx, cy, inr, outr, icol, ocol);
			}
			break;
		case 3:
			{
				  float ox,oy,ex,ey,angle,alpha;
				  int image;
				  ox = lua_tableNumber(L, n, "ox");
				  oy = lua_tableNumber(L, n, "oy");
				  ex = lua_tableNumber(L, n, "ex");
				  ey = lua_tableNumber(L, n, "ey");
				  angle = lua_tableNumber(L, n, "angle");
				  alpha = lua_tableNumber(L, n, "alpha");
				  image = (int)lua_tableNumber(L, n, "image");
				  *p = nvgImagePattern(_vg, ox, oy, ex, ey, angle, image,alpha);
			}
			break;
		case 4:
			{
				  float x, y, w, h, r, f;
				  NVGcolor ico1, ico2;
				  x = lua_tableNumber(L, n, "x");
				  y = lua_tableNumber(L, n, "y");
				  w = lua_tableNumber(L, n, "w");
				  h = lua_tableNumber(L, n, "h");
				  r = lua_tableNumber(L, n, "r");
				  f = lua_tableNumber(L, n, "f");
				  lua_getfield(L, n, "ico1");
				  lua_toNVGcolor(L, -1, &ico1);
				  lua_pop(L, 1);
				  lua_getfield(L, n, "ico2");
				  lua_toNVGcolor(L, -1, &ico2);
				  lua_pop(L, 1);
				  *p = nvgBoxGradient(_vg, x, y, w, h, r, f, ico1, ico2);
			}
			break;
		default:
			return 0;
		}
		return 1;
	}
	return 0;
}

/**
 * \brief 存储绘制状态，稍后可以使用restore恢复之前的绘制状态。
 */
static int lua_save(lua_State *L)
{
	nvgSave(_vg);
	return 0;
}

/**
 * \brief 恢复之前save的绘制状态。
 */
static int lua_restore(lua_State *L)
{
	nvgRestore(_vg);
	return 0;
}

/**
 * \brief 重置绘制状态。
 */
static int lua_reset(lua_State *L)
{
	nvgReset(_vg);
	return 0;
}

/**
 * \brief 设置边框的颜色。
 */
static int lua_strokeColor(lua_State *L)
{
	NVGcolor c;
	if (lua_toNVGcolor(L, 1, &c))
		nvgStrokeColor(_vg, c);
	return 0;
}

/**
 * \brief 设置填充颜色。
 */
static int lua_fillColor(lua_State *L)
{
	NVGcolor c;
	if (lua_toNVGcolor(L, 1, &c))
		nvgFillColor(_vg, c);
	return 0;
}

static int lua_strokePaint(lua_State *L)
{
	NVGpaint p;
	if (lua_toNVGpaint(L, 1, &p))
		nvgStrokePaint(_vg, p);
	return 0;
}

static int lua_fillPaint(lua_State *L)
{
	NVGpaint p;
	if (lua_toNVGpaint(L, 1, &p))
		nvgFillPaint(_vg, p);
	return 0;
}

static int lua_miterLimit(lua_State *L)
{
	nvgMiterLimit(_vg, (float)luaL_checknumber(L,1));
	return 0;
}

static int lua_strokeWidth(lua_State *L)
{
	nvgStrokeWidth(_vg, (float)luaL_checknumber(L, 1));
	return 0;
}

static int lua_lineCap(lua_State *L)
{
	nvgLineCap(_vg, (int)luaL_checknumber(L, 1));
	return 0;
}

static int lua_lineJoin(lua_State *L)
{
	nvgLineJoin(_vg, (int)luaL_checknumber(L, 1));
	return 0;
}

static int lua_globalAlpha(lua_State *L)
{
	nvgGlobalAlpha(_vg, (float)luaL_checknumber(L, 1));
	return 0;
}

int lua_resetTransform(lua_State *L)
{
	nvgResetTransform(_vg);
	return 0;
}

static int lua_transform(lua_State *L)
{
	float a, b, c, d, e, f;
	a = (float)luaL_checknumber(L, 1);
	b = (float)luaL_checknumber(L, 2);
	c = (float)luaL_checknumber(L, 3);
	d = (float)luaL_checknumber(L, 4);
	e = (float)luaL_checknumber(L, 5);
	f = (float)luaL_checknumber(L, 6);
	nvgTransform(_vg, a, b, c, d, e, f);
	return 0;
}

static int lua_currentTransform(lua_State *L)
{
	float form[6];
	nvgCurrentTransform(_vg, form);
	for (int i = 0; i < 6;i++)
		lua_pushnumber(L, form[i]);
	return 6;
}

static int lua_translate(lua_State *L)
{
	float x, y;
	x = (float)luaL_checknumber(L, 1);
	y = (float)luaL_checknumber(L, 2);
	nvgTranslate(_vg,x,y);
	return 0;
}

static int lua_rotate(lua_State *L)
{
	float angle = (float)luaL_checknumber(L, 1);
	nvgRotate(_vg,angle);
	return 0;
}

static int lua_skewX(lua_State *L)
{
	float angle = (float)luaL_checknumber(L, 1);
	nvgSkewX(_vg, angle);
	return 0;
}

static int lua_skewY(lua_State *L)
{
	float angle = (float)luaL_checknumber(L, 1);
	nvgSkewY(_vg, angle);
	return 0;
}

static int lua_scale(lua_State *L)
{
	float x, y;
	x = (float)luaL_checknumber(L, 1);
	y = (float)luaL_checknumber(L, 1);
	nvgScale(_vg,x,y);
	return 0;
}

static int lua_createImage(lua_State *L)
{
	const char *filename = luaL_checkstring(L, 1);
	int imageFlags = 0;
	if (lua_isnumber(L, 2))
		imageFlags = (int)luaL_checknumber(L, 2);
	lua_pushnumber(L, nvgCreateImage(_vg, filename, imageFlags));
	return 1;
}

static int lua_imageSize(lua_State *L)
{
	int image,w, h;
	w = h = 0;
	image = (int)luaL_checknumber(L, 1);
	nvgImageSize(_vg, image,&w, &h);
	lua_pushnumber(L, w);
	lua_pushnumber(L, h);
	return 2;
}

static int lua_deleteImage(lua_State *L)
{
	nvgDeleteImage(_vg, (int)luaL_checknumber(L, 1));
	return 0;
}

static int lua_scissor(lua_State *L)
{
	float x, y, w, h;
	x = (float)luaL_checknumber(L, 1);
	y = (float)luaL_checknumber(L, 2);
	w = (float)luaL_checknumber(L, 3);
	h = (float)luaL_checknumber(L, 4);
	nvgScissor(_vg, x,y,w,h);
	return 0;
}

static int lua_intersectScissor(lua_State *L)
{
	float x, y, w, h;
	x = (float)luaL_checknumber(L, 1);
	y = (float)luaL_checknumber(L, 2);
	w = (float)luaL_checknumber(L, 3);
	h = (float)luaL_checknumber(L, 4);
	nvgIntersectScissor(_vg, x, y, w, h);
	return 0;
}

static int lua_resetScissor(lua_State *L)
{
	nvgResetScissor(_vg);
	return 0;
}

static int lua_arc(lua_State *L)
{
	float cx, cy, r, a0, a1;
	int dir;
	cx = (float)luaL_checknumber(L, 1);
	cy = (float)luaL_checknumber(L, 2);
	r = (float)luaL_checknumber(L, 3);
	a0 = (float)luaL_checknumber(L, 4);
	a1 = (float)luaL_checknumber(L, 5);
	dir = (int)luaL_checknumber(L, 6);
	nvgArc(_vg,cx,cy,r,a0,a1,dir);
	return 0;
}


static int lua_rect(lua_State *L)
{
	float x, y, w, h;
	x = (float)luaL_checknumber(L, 1);
	y = (float)luaL_checknumber(L, 2);
	w = (float)luaL_checknumber(L, 3);
	h = (float)luaL_checknumber(L, 4);
	nvgRect(_vg,x,y,w,h);
	return 0;
}


static int lua_roundedRect(lua_State *L)
{
	float x, y, w, h,r;
	x = (float)luaL_checknumber(L, 1);
	y = (float)luaL_checknumber(L, 2);
	w = (float)luaL_checknumber(L, 3);
	h = (float)luaL_checknumber(L, 4);
	r = (float)luaL_checknumber(L, 5);
	nvgRoundedRect(_vg, x, y, w, h,r);
	return 0;
}

static int lua_ellipse(lua_State *L)
{
	float cx,cy,rx,ry;
	cx = (float)luaL_checknumber(L, 1);
	cy = (float)luaL_checknumber(L, 2);
	rx = (float)luaL_checknumber(L, 3);
	ry = (float)luaL_checknumber(L, 4);
	nvgEllipse(_vg, cx, cy, rx, ry);
	return 0;
}

static int lua_circle(lua_State *L)
{
	float cx, cy, r;
	cx = (float)luaL_checknumber(L, 1);
	cy = (float)luaL_checknumber(L, 2);
	r = (float)luaL_checknumber(L, 3);
	nvgCircle(_vg, cx, cy, r);
	return 0;
}

static int lua_beginPath(lua_State *L)
{
	nvgBeginPath(_vg);
	return 0;
}

static void drawPath(float *p, int c)
{
	if (c > 0){
		switch ((int)p[0]){
		case 1:
			nvgMoveTo(_vg, p[1], p[2]);
			break;
		case 2:
			nvgLineTo(_vg, p[1], p[2]);
			break;
		case 3:
			nvgBezierTo(_vg, p[1], p[2], p[3], p[4], p[5], p[6]);
			break;
		case 4:
			nvgQuadTo(_vg, p[1], p[2], p[3], p[4]);
			break;
		case 5:
			nvgArcTo(_vg, p[1], p[2], p[3], p[4], p[5]);
			break;
		case 6:
			nvgClosePath(_vg);
			break;
		}
	}
}

static int pathN(float p)
{
	switch ((int)p){
	case 1:return 2;
	case 2:return 2;
	case 3:return 6;
	case 4:return 4;
	case 5:return 5;
	case 6:return 0;
	}
	return -1;
}
/*
 * MOVETO(1),LINETO(2),BEZIERTO(3),QUADTO(4),ARCTO(5),CLOSE(6),END
 */
static int lua_path(lua_State *L)
{
	if (lua_istable(L, 1)){
		int i = 1;
		float p[8], code;
		int c, n;
		n = c = 0;
		while (1){
			lua_rawgeti(L, 1, i++);
			if (lua_isnumber(L, -1)){
				if (c == n){
					if (c != 0){
						drawPath(p, c);
						c = 0;
					}
					code = (float)luaL_checknumber(L, -1);
					n = pathN(code)+1;
					if (n == -1)break; //invalid code
					p[c++] = code;
				}
				else{
					p[c++] = (float)luaL_checknumber(L, -1);
				}
				lua_pop(L, 1);
			}
			else{
				lua_pop(L, 1);
				break;
			}
			if (c > 7)break;
		}
		if (c == n&&c != 0){
			drawPath(p, c);
		}
	}
	return 0;
}

static int lua_pathWinding(lua_State *L)
{
	nvgPathWinding(_vg, (int)luaL_checknumber(L, 1));
	return 0;
}

static int lua_fill(lua_State *L)
{
	nvgFill(_vg);
	return 0;
}

static int lua_stroke(lua_State *L)
{
	nvgStroke(_vg);
	return 0;
}

static int lua_beginFrame(lua_State *L)
{
	int windowWidth, windowHeight;
	float devicePixelRatio;
	windowWidth = (int)luaL_checknumber(L, 1);
	windowHeight = (int)luaL_checknumber(L, 2);
	devicePixelRatio = (float)luaL_checknumber(L, 3);
	nvgBeginFrame(_vg, windowWidth, windowHeight, devicePixelRatio);
	return 0;
}

static int lua_cancelFrame(lua_State *L)
{
	nvgCancelFrame(_vg);
	return 0;
}

static int lua_endFrame(lua_State *L)
{
	nvgEndFrame(_vg);
	return 0;
}

static int lua_createFont(lua_State *L)
{
	const char *name = luaL_checkstring(L, 1);
	const char *filename = luaL_checkstring(L,2);
	lua_pushinteger(L, nvgCreateFont(_vg, name, filename));
	return 1;
}

static int lua_findFont(lua_State *L)
{
	lua_pushinteger(L, nvgFindFont(_vg, luaL_checkstring(L, 1)));
	return 1;
}

static int lua_fontSize(lua_State *L)
{
	nvgFontSize(_vg, (float)luaL_checknumber(L, 1));
	return 0;
}

static int lua_fontBlur(lua_State *L)
{
	nvgFontBlur(_vg, (float)luaL_checknumber(L, 1));
	return 0;
}

static int lua_textLetterSpacing(lua_State *L)
{
	nvgTextLetterSpacing(_vg, (float)luaL_checknumber(L, 1));
	return 0;
}

static int lua_textLineHeight(lua_State *L)
{
	nvgTextLineHeight(_vg, (float)luaL_checknumber(L, 1));
	return 0;
}

static int lua_textAlign(lua_State *L)
{
	nvgTextAlign(_vg, (int)luaL_checkinteger(L, 1));
	return 0;
}

static int lua_fontFace(lua_State *L)
{
	nvgFontFace(_vg, luaL_checkstring(L, 1));
	return 0;
}

static int lua_fontFaceId(lua_State *L)
{
	nvgFontFaceId(_vg, (int)luaL_checkinteger(L, 1));
	return 0;
}

static int lua_text(lua_State *L)
{
	float x, y;
	size_t len;
	const char *string;
	x = (float)luaL_checknumber(L, 1);
	y = (float)luaL_checknumber(L, 2);
	string = luaL_checklstring(L, 3,&len);
	nvgText(_vg, x, y, string, string+len);
	return 0;
}

/*
 * 绘制宽度限制多行文本
 */
static int lua_textBox(lua_State *L)
{
	float x, y, breakRowWidth;
	size_t len;
	const char *string;
	x = (float)luaL_checknumber(L, 1);
	y = (float)luaL_checknumber(L, 2);
	breakRowWidth = (float)luaL_checknumber(L, 3);
	string = luaL_checklstring(L, 4, &len);
	nvgTextBox(_vg, x, y, breakRowWidth,string, NULL);
	return 0;
}

/*
 * 取文本在当前字体下的范围
 */
static int lua_textBounds(lua_State *L)
{
	float x, y;
	const char * string;
	float bounds[4];

	x = (float)luaL_checknumber(L, 1);
	y = (float)luaL_checknumber(L, 2);
	string = luaL_checkstring(L, 3);
	float width = nvgTextBounds(_vg,x, y, string, NULL, bounds);
	lua_pushnumber(L, width);
	for (int i = 0; i < 4;i++)
		lua_pushnumber(L, bounds[i]);
	return 5;
}

/*
 * 取折行文本的范围
 */
static int lua_textBoxBounds(lua_State *L)
{
	float x, y,breakRowWidth;
	float bounds[4];
	const char * string;

	x = (float)luaL_checknumber(L, 1);
	y = (float)luaL_checknumber(L, 2);
	breakRowWidth = (float)luaL_checknumber(L, 3);
	string = luaL_checkstring(L, 4);
	nvgTextBoxBounds(_vg, x, y, breakRowWidth, string, NULL, bounds);
	for (int i = 0; i < 4; i++)
		lua_pushnumber(L, bounds[i]);
	return 4;
}

/*
 * 范围每个字符绘制的位置以及占据的宽度范围
 */
static int lua_textGlyphPositions(lua_State *L)
{
	float x, y;
	const char * string;
	int maxPositions;
	NVGglyphPosition *positions;
	x = (float)luaL_checknumber(L, 1);
	y = (float)luaL_checknumber(L, 2);
	string = luaL_checkstring(L, 3);
	maxPositions = strlen(string);
	if (maxPositions > 0){
		positions = (NVGglyphPosition *)malloc((maxPositions+1)*sizeof(NVGglyphPosition));
		int npos = nvgTextGlyphPositions(_vg, x, y, string, NULL, positions, maxPositions);
		lua_newtable(L);
		for (int i = 0; i < maxPositions; i++){
			lua_newtable(L);
			lua_pushinteger(L, (lua_Integer)(positions[i].str - string)+1);
			lua_setfield(L, -2, "pos");
			lua_pushnumber(L, (lua_Number)positions[i].x);
			lua_setfield(L, -2, "x");
			lua_pushnumber(L, (lua_Number)positions[i].minx);
			lua_setfield(L, -2, "minx");
			lua_pushnumber(L, (lua_Number)positions[i].maxx);
			lua_setfield(L, -2, "maxx");
			lua_rawseti(L, -2, i + 1);
		}
		free(positions);
		return 1;
	}
	return 0;
}

static int lua_textMetrics(lua_State *L)
{
	float ascender, descender, lineh;
	nvgTextMetrics(_vg, &ascender, &descender, &lineh);
	lua_pushnumber(L, ascender);
	lua_pushnumber(L, descender);
	lua_pushnumber(L, lineh);
	return 3;
}

static int lua_textBreakLines(lua_State *L)
{
	const char *string = luaL_checkstring(L, 1);
	float breakRowWidth = (float)luaL_checknumber(L, 2);
	int maxRows = (int)luaL_checkinteger(L, 3);
	if (maxRows > 0){
		NVGtextRow * rows = (NVGtextRow *)malloc((maxRows+1)*sizeof(NVGtextRow));
		if (!rows)return 0;
		int nrows = nvgTextBreakLines(_vg, string, NULL, breakRowWidth, rows, maxRows);
		lua_newtable(L);
		for (int i = 0; i < nrows; i++){
			lua_newtable(L);
			lua_pushinteger(L,(lua_Integer)(rows[i].start-string)+1 );
			lua_setfield(L, -2, "head");
			lua_pushinteger(L, (lua_Integer)(rows[i].end - string)+1);
			lua_setfield(L, -2, "tail");
			lua_pushinteger(L, (lua_Integer)(rows[i].next - string)+1);
			lua_setfield(L, -2, "next");
			lua_pushnumber(L, (lua_Number)rows[i].width);
			lua_setfield(L, -2, "width");
			lua_pushnumber(L, (lua_Number)rows[i].minx);
			lua_setfield(L, -2, "minx");
			lua_pushnumber(L, (lua_Number)rows[i].maxx);
			lua_setfield(L, -2, "maxx");
			lua_rawseti(L, -2, i + 1);
		}
		free(rows);
		return 1;
	}
	return 0;
}

static int lua_closePath(lua_State *L)
{
	nvgClosePath(_vg);
	return 0;
}

static int lua_HSLA(lua_State *L)
{
	float h, s, l,a;
	h = (float)luaL_checknumber(L, 1);
	s = (float)luaL_checknumber(L, 2);
	l = (float)luaL_checknumber(L, 3);
	a = (float)luaL_checknumber(L, 4);
	NVGcolor c=nvgHSLA(h,s,l,(unsigned char)a);
	lua_pushnumber(L, c.r);
	lua_pushnumber(L, c.g);
	lua_pushnumber(L, c.b);
	lua_pushnumber(L, c.a);
	return 4;
}

static int lua_degToRad(lua_State *L)
{
	lua_pushnumber(L, nvgDegToRad((float)luaL_checknumber(L, 1)));
	return 1;
}

static char* cpToUTF8(int cp, char* str)
{
	int n = 0;
	if (cp < 0x80) n = 1;
	else if (cp < 0x800) n = 2;
	else if (cp < 0x10000) n = 3;
	else if (cp < 0x200000) n = 4;
	else if (cp < 0x4000000) n = 5;
	else if (cp <= 0x7fffffff) n = 6;
	str[n] = '\0';
	switch (n) {
	case 6: str[5] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x4000000;
	case 5: str[4] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x200000;
	case 4: str[3] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x10000;
	case 3: str[2] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x800;
	case 2: str[1] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0xc0;
	case 1: str[0] = cp;
	}
	return str;
}

static int lua_cpToUTF8(lua_State *L)
{
	char str[8];
	cpToUTF8(luaL_checkinteger(L, 1), str);
	lua_pushstring(L, str);
	return 1;
}

static const struct luaL_Reg nanovg_methods[] =
{
	{ "beginNanoVG", lua_beginNanoVG },

	{ "beginFrame", lua_beginFrame },
	{ "cancelFrame", lua_cancelFrame },
	{ "endFrame", lua_endFrame },

	{ "save", lua_save },
	{ "restore", lua_restore },
	{ "reset", lua_reset },
	{ "strokeColor", lua_strokeColor },
	{ "fillColor", lua_fillColor },
	{ "strokePaint", lua_strokePaint },
	{ "fillPaint", lua_fillPaint },
	{ "miterLimit", lua_miterLimit },
	{ "strokeWidth", lua_strokeWidth },
	{ "lineCap", lua_lineCap },
	{ "lineJoin", lua_lineJoin },
	{ "globalAlpha", lua_globalAlpha },

	{ "resetTransform", lua_resetTransform },
	{ "transform", lua_transform },
	{ "translate", lua_translate },
	{ "rotate", lua_rotate },
	{ "skewX", lua_skewX },
	{ "skewY", lua_skewY },
	{ "scale", lua_scale },
	{ "currentTransform", lua_currentTransform },

	{ "createImage", lua_createImage },
	{ "imageSize", lua_imageSize },
	{ "deleteImage", lua_deleteImage },

	{ "scissor", lua_scissor },
	{ "intersectScissor", lua_intersectScissor },
	{ "resetScissor", lua_resetScissor },

	{ "beginPath", lua_beginPath },
	{ "arc", lua_arc },
	{ "rect", lua_rect },
	{ "roundedRect", lua_roundedRect },
	{ "ellipse", lua_ellipse },
	{ "circle", lua_circle },
	{ "pathWinding", lua_pathWinding },
	{ "path", lua_path },
	{ "closePath", lua_closePath },
	{ "fill", lua_fill },
	{ "stroke", lua_stroke },

	{ "createFont", lua_createFont },
	{ "findFont", lua_findFont },
	{ "fontSize", lua_fontSize },
	{ "fontBlur", lua_fontBlur },
	{ "textLetterSpacing", lua_textLetterSpacing },
	{ "textLineHeight", lua_textLineHeight },
	{ "textAlign", lua_textAlign },
	{ "fontFaceId", lua_fontFaceId },
	{ "fontFace", lua_fontFace },
	{ "text", lua_text },
	{ "textBox", lua_textBox },
	{ "textBounds", lua_textBounds },
	{ "textBoxBounds", lua_textBoxBounds },
	{ "textGlyphPositions", lua_textGlyphPositions },
	{ "textMetrics", lua_textMetrics },
	{ "textBreakLines", lua_textBreakLines },

	{ "HSLA", lua_HSLA },
	{ "degToRad", lua_degToRad },
	{ "cpToUTF8", lua_cpToUTF8 },
	{ NULL, NULL }
};

#define VERSION "1.0"

static void set_info(lua_State *L)
{
	lua_pushliteral(L, "_COPYRIGHT");
	lua_pushliteral(L, "Copyright (C) 2016");
	lua_settable(L, -3);
	lua_pushliteral(L, "_DESCRIPTION");
	lua_pushliteral(L, "nanovg library.");
	lua_settable(L, -3);
	lua_pushliteral(L, "_VERSION");
	lua_pushliteral(L, "nanovg " VERSION);
	lua_settable(L, -3);
}

int luaopen_nanovg(lua_State *L)
{
	luaL_newlib(L, nanovg_methods);
	set_info(L);
	return 1;
}

/**
 * @}
 */