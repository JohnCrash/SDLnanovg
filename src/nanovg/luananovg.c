#include "luananovg.h"
#include "SDL.h"
#include "gles.h"
#include "nanovg.h"
#include "nanovg_sdl.h"
#include "sdlmain.h"
#include "eventhandler.h"

/**
 * \addtogroup VG lua vg
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
 * *NVGpaint分成4种类型的绘制模式*
 *
 * 绘制模式是一个拥有type元素的表，type是1~4中的一个，不同的值表将拥有不同的其他元素。
 * 1. LinearGradient
 *		+ 其他元素sx,sy,ex,ey,ico1,ico2,
 *		+ sx,sy,ex,ey 代表开始点坐标和结束点坐标。
 *		+ ico1,ico2 代表起点 \ref lua_toNVGcolor "颜色" 和结束点  \ref lua_toNVGcolor "颜色" 
 *		+ 在lua中可以使用vg.LinearGradient(sx,sy,ex,ey,ico1,ico2)来产生该表。
 * 2. RadialGradient
 *		+ 其他元素cx,cy,inr,outr,ico1,oco1
 *		+ cx,cy中心点坐标
 *		+ inr,outr内半径和外半径
 *		+ ico1,oco1内半径颜色和外半径颜色
 *		+ 在lua中可以使用vg.RadialGradient(cx,cy,inr,outr,ico1,oco1)来产生该表。
 * 3. ImagePattern	
 *		+ 其他元素ox,oy,ex,ey,angle,alpha,image
 *		+ ox,oy起点坐标
 *		+ ex,ey图像的宽和高
 *		+ angle旋转角度
 *		+ alpha透明度
 *		+ image图像 #lua_createImage
 *		+ 在lua中可以使用vg.ImagePattern(ox,oy,ex,ey,angle,alpha,image)来产生该表。
 * 4. BoxGradient
 *		+ 其他元素x,y,w,h,r,f,ico1,ico2
 *		+ 在lua中可以使用vg.BoxGradient(x,y,w,h,r,f,ico1,ico2)来产生该表。
 *		+ x,y矩形左上角坐标
 *		+ w,h矩形的宽度和高度
 *		+ r矩形的圆角半径
 *		+ f模糊效果
 *		+ ico1,ico2内部颜色和外部颜色
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
 * \param color 拥有r,g,b,a的表
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
 * \param color 拥有r,g,b,a的表
 */
static int lua_fillColor(lua_State *L)
{
	NVGcolor c;
	if (lua_toNVGcolor(L, 1, &c))
		nvgFillColor(_vg, c);
	return 0;
}

/**
 * \brief 设置边框绘制模式
 * \param paint 见 \ref lua_toNVGpaint "绘制模式"
 */
static int lua_strokePaint(lua_State *L)
{
	NVGpaint p;
	if (lua_toNVGpaint(L, 1, &p))
		nvgStrokePaint(_vg, p);
	return 0;
}

/**
 * \brief 设置填充模式
 * \param paint 见 \ref lua_toNVGpaint "绘制模式"
 */
static int lua_fillPaint(lua_State *L)
{
	NVGpaint p;
	if (lua_toNVGpaint(L, 1, &p))
		nvgFillPaint(_vg, p);
	return 0;
}

/**
 * \brief 设置斜切极限
 * \param limit 
 */
static int lua_miterLimit(lua_State *L)
{
	nvgMiterLimit(_vg, (float)luaL_checknumber(L,1));
	return 0;
}

/**
 * \brief 设置线的宽度
 * \param width 线宽 
 */
static int lua_strokeWidth(lua_State *L)
{
	nvgStrokeWidth(_vg, (float)luaL_checknumber(L, 1));
	return 0;
}

/**
 * \brief 线的头部形状
 * \param cap  可以是下面的值：
 *	- NVG_ROUND
 *	- NVG_SQUARE
 *	- NVG_BEVEL
 *	- NVG_MITER
 */
static int lua_lineCap(lua_State *L)
{
	nvgLineCap(_vg, (int)luaL_checknumber(L, 1));
	return 0;
}

/**
 * \brief 设置线的拐角形状
 * \param join  可以是下面的值：
 *	- NVG_ROUND
 *	- NVG_SQUARE
 *	- NVG_BEVEL
 *	- NVG_MITER 
 */
static int lua_lineJoin(lua_State *L)
{
	nvgLineJoin(_vg, (int)luaL_checknumber(L, 1));
	return 0;
}

/**
 * \bried 设置全局透明值
 * \param alpha 透明值
 */
static int lua_globalAlpha(lua_State *L)
{
	nvgGlobalAlpha(_vg, (float)luaL_checknumber(L, 1));
	return 0;
}

/**
 * \brief 重置当前的变换矩阵为单位矩阵
 */
int lua_resetTransform(lua_State *L)
{
	nvgResetTransform(_vg);
	return 0;
}

/**
 * \brief 乘以当前的变换矩阵。函数有6个参数，分别是a,b,c,d,e,f
 * 将以下面的形式组成一个矩阵
 * 
 * | | | | 
 * |-|-|-|
 * |a|c|e|
 * |b|d|f|
 * |0|0|1|
 */
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

/**
 * \brief 取得当前的矩阵。
 * 返回6个参数。
 */
static int lua_currentTransform(lua_State *L)
{
	float form[6];
	nvgCurrentTransform(_vg, form);
	for (int i = 0; i < 6;i++)
		lua_pushnumber(L, form[i]);
	return 6;
}

/**
 * \brief 平移变换。
 * \param x 在x方向上移动的距离
 * \param y 在y方向上移动的距离
 */
static int lua_translate(lua_State *L)
{
	float x, y;
	x = (float)luaL_checknumber(L, 1);
	y = (float)luaL_checknumber(L, 2);
	nvgTranslate(_vg,x,y);
	return 0;
}

/**
 * \brief 旋转变换。
 * \param angle 旋转一个角度(弧度)
 */
static int lua_rotate(lua_State *L)
{
	float angle = (float)luaL_checknumber(L, 1);
	nvgRotate(_vg,angle);
	return 0;
}

/**
 * \brief 在x方向上做切变换。
 * \param angle 倾斜的角度(弧度)
 */
static int lua_skewX(lua_State *L)
{
	float angle = (float)luaL_checknumber(L, 1);
	nvgSkewX(_vg, angle);
	return 0;
}

/**
 * \brief 在y方向上做切变换。
 * \param angle 倾斜的角度(弧度)
 */
static int lua_skewY(lua_State *L)
{
	float angle = (float)luaL_checknumber(L, 1);
	nvgSkewY(_vg, angle);
	return 0;
}

/**
 * \brief 缩放变换。
 * \param sx 在x方向上的缩放比例
 * \param sy 在y方向上的缩放比例
 */
static int lua_scale(lua_State *L)
{
	float x, y;
	x = (float)luaL_checknumber(L, 1);
	y = (float)luaL_checknumber(L, 1);
	nvgScale(_vg,x,y);
	return 0;
}

/**
 * \brief 创建一个图像。
 * \param filename 图像文件名
 * \param imageFlags 这是一个可选择产生。可以是下面值的组合,默认为0
 *		- NVG_IMAGE_GENERATE_MIPMAPS	产生mipmaps
 *		- NVG_IMAGE_REPEATX				在X方向上重复
 *		- NVG_IMAGE_REPEATY				在Y方向上重复
 *		- NVG_IMAGE_FLIPY				沿Y轴翻转
 *		- NVG_IMAGE_PREMULTIPLIED		图像有alpha值
 */
static int lua_createImage(lua_State *L)
{
	const char *filename = luaL_checkstring(L, 1);
	int imageFlags = 0;
	if (lua_isnumber(L, 2))
		imageFlags = (int)luaL_checknumber(L, 2);
	lua_pushnumber(L, nvgCreateImage(_vg, filename, imageFlags));
	return 1;
}

/**
 * \brief 取得图像的宽度和高度
 * \retval w 宽度
 * \retval h 高度
 */
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

/**
 * \brief 删除图像
 * \param image 图像的id，见 \ref lua_createImage "createImage的返回值"
 */
static int lua_deleteImage(lua_State *L)
{
	nvgDeleteImage(_vg, (int)luaL_checknumber(L, 1));
	return 0;
}

/**
 * \brief 设置剪切区
 * \param x 剪切区左上角x值
 * \param y 剪切区左上角y值
 * \param w 剪切区宽度
 * \param h 剪切区高度
 */
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

/**
 * \brief 用一个矩形和当前的剪切区求交集，将新矩形设置为当前的剪切区
 * \param x 左上角x值
 * \param y 左上角y值
 * \param w 宽度
 * \param h 高度
 */
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

/**
 * \brief 重置当前剪切区
 */
static int lua_resetScissor(lua_State *L)
{
	nvgResetScissor(_vg);
	return 0;
}

/**
 * \brief 绘制一个圆弧
 * \param cx 圆弧的中心点x坐标
 * \param cy 圆弧的中心点y坐标
 * \param r 圆弧的半径
 * \param a0 起始角度
 * \param a1 结束角度
 * \param dir 弧形绘制方向 
 *		- NVG_CCW 逆时针
 *		- NVG_CW 顺时针
 */
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


/**
 * \brief 绘制矩形
 * \param x 矩形的左上角x
 * \param y 矩形的左上角y
 * \param w 矩形的宽度
 * \param h 矩形的高度
 */
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

/**
 * \brief 绘制圆角矩形
 * \param x 矩形的左上角x
 * \param y 矩形的左上角y
 * \param w 矩形的宽度
 * \param h 矩形的高度
 * \param r 圆角半径
 */
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

/**
 * \brief 绘制椭圆
 * \param cx 椭圆中心x
 * \param cy 椭圆中心y
 * \param rx x轴方向的半径
 * \param ry y轴方向的半径
 */
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

/**
 * \brief 绘制圆形
 * \param cx 圆心的x
 * \param cy 圆心的y
 * \param r 圆的半径
 */
static int lua_circle(lua_State *L)
{
	float cx, cy, r;
	cx = (float)luaL_checknumber(L, 1);
	cy = (float)luaL_checknumber(L, 2);
	r = (float)luaL_checknumber(L, 3);
	nvgCircle(_vg, cx, cy, r);
	return 0;
}

/**
 * \brief 开始描述一次绘制，后面跟随 rect,arc,circle path 等函数。最后以fill或者stroke
 * 表明是要绘制路径还是填充。
 */
static int lua_beginPath(lua_State *L)
{
	nvgBeginPath(_vg);
	return 0;
}

/**
 * \brief 将紧密的数据分解为绘制片段
 */
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

/**
 * \brief 描述一个路径，必须被放在beginPath和endPath中间。
 * \param path 一个表结构
 *		- MOVETO(1) 设置绘制起点，如{vg.MOVETO,x,y}
 *		- LINETO(2) 从起点绘制一条直线，如{vg.LINETO,x,y}
 *		- BEZIERTO(3) 绘制贝塞尔曲线，如{vg.BEZIERTO,x1,y2,x2,y2,x3,y3}
 *		- QUADTO(4) 绘制矩形
 *		- ARCTO(5) 绘制弧线
 *		- CLOSE(6) 封闭曲线
 *		- END 结束
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

/**
 * \brief 设置当前路径的方向
 * \param winding 路径方向
 *		- NVG_CCW 逆时针
 * 		- NVG_CC 顺时针
 */
static int lua_pathWinding(lua_State *L)
{
	nvgPathWinding(_vg, (int)luaL_checknumber(L, 1));
	return 0;
}

/**
 * \brief 使用当前填充模式填充当前路径
 */
static int lua_fill(lua_State *L)
{
	nvgFill(_vg);
	return 0;
}

/**
 * \brief 使用当前填充模式绘制路径
 */
static int lua_stroke(lua_State *L)
{
	nvgStroke(_vg);
	return 0;
}

/**
 * \brief 开始一次完整的绘制，以 #lua_endFrame 结尾将描述的绘制真实的绘制到屏幕上。
 */
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

/**
 * \brief 丢弃全部描述的绘制。
 */
static int lua_cancelFrame(lua_State *L)
{
	nvgCancelFrame(_vg);
	return 0;
}

/**
 * \brief 将描述的绘制发送给显卡进行绘制。
 */
static int lua_endFrame(lua_State *L)
{
	nvgEndFrame(_vg);
	return 0;
}

/**
 * \brief 创建字体。
 * \param name 字体名
 * \param filename 字体文件名
 * \return 成功返回值大于等于0(字体id)，失败返回FONS_INVALID (-1)
 */
static int lua_createFont(lua_State *L)
{
	const char *name = luaL_checkstring(L, 1);
	const char *filename = luaL_checkstring(L,2);
	lua_pushinteger(L, nvgCreateFont(_vg, name, filename));
	return 1;
}

/**
 * \brief 查找字体
 * \param name 字体名称
 * \return 成功返回值大于等于0(字体id)，失败返回FONS_INVALID (-1)
 */
static int lua_findFont(lua_State *L)
{
	lua_pushinteger(L, nvgFindFont(_vg, luaL_checkstring(L, 1)));
	return 1;
}

/**
 * \brief 返回字体的尺寸
 * \return 返回字体尺寸
 */
static int lua_fontSize(lua_State *L)
{
	nvgFontSize(_vg, (float)luaL_checknumber(L, 1));
	return 0;
}

/**
 * \brief 设置字体的模糊值
 * \param blur 模糊值
 */
static int lua_fontBlur(lua_State *L)
{
	nvgFontBlur(_vg, (float)luaL_checknumber(L, 1));
	return 0;
}

/**
 * \brief 设置字间距
 * \param space 字间距
 */
static int lua_textLetterSpacing(lua_State *L)
{
	nvgTextLetterSpacing(_vg, (float)luaL_checknumber(L, 1));
	return 0;
}

/**
 * \brief 设置行高
 * \param height 行高
 */
static int lua_textLineHeight(lua_State *L)
{
	nvgTextLineHeight(_vg, (float)luaL_checknumber(L, 1));
	return 0;
}

/**
 * \brief 设置文本的对齐方式
 * \param align 对齐方式可以是下列的组合
 *	- NVG_ALIGN_LEFT		左对齐
 *	- NVG_ALIGN_CENTER		横向中心对齐
 *	- NVG_ALIGN_RIGHT		右对齐
 *	- NVG_ALIGN_TOP			顶部对齐
 *	- NVG_ALIGN_MIDDLE		纵向中间对齐
 *	- NVG_ALIGN_BOTTOM		底对齐
 *	- NVG_ALIGN_BASELINE	字符基线对齐
 */
static int lua_textAlign(lua_State *L)
{
	nvgTextAlign(_vg, (int)luaL_checkinteger(L, 1));
	return 0;
}

/**
 * \brief 当前绘制使用该字体
 * \param name 字体名称
 */
static int lua_fontFace(lua_State *L)
{
	nvgFontFace(_vg, luaL_checkstring(L, 1));
	return 0;
}

/**
 * \brief 当前字体的id
 * \return 返回当前字体的id,或者失败返回FONS_INVALID (-1)
 */
static int lua_fontFaceId(lua_State *L)
{
	lua_pushinteger(nvgFontFaceId(_vg, (int)luaL_checkinteger(L, 1)));
	return 1;
}

/**
 * \brief 使用当前字体绘制文本。
 * \param x 绘制点x坐标
 * \param y 绘制点y坐标
 * \param str 要绘制的文本(编码UTF8)
 */
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

/**
 * \brief 使用当前字体绘制文本，有宽度限制会自动折行。
 * \param x 绘制点x坐标
 * \param y 绘制点y坐标
 * \param breakWidth 折行宽度
 * \param str 要绘制的文本(编码UTF8)
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

/**
 * \brief 返回使用 #lua_text 绘制的文本的包围盒。
 * \param x 绘制点x坐标
 * \param y 绘制点y坐标
 * \param str 要绘制的文本(编码UTF8)
 * \return 返回5个值
 * \retval width 文字总宽度
 * \retval x1 包围盒左上角x值
 * \retval y1 包围盒左上角y值
 * \retval x2 包围盒右下角x值
 * \retval y2 包围盒右下角y值
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

/**
 * \brief 返回使用 #lua_textBox 绘制的文本的包围盒。
 * \param x 绘制点x坐标
 * \param y 绘制点y坐标
 * \param breakWidth 折行宽度
 * \param str 要绘制的文本(编码UTF8)
 * \return 返回4个值
 * \retval x1 包围盒左上角x值
 * \retval y1 包围盒左上角y值
 * \retval x2 包围盒右下角x值
 * \retval y2 包围盒右下角y值
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

/**
 * \brief 范围每个字符绘制的位置以及占据的宽度范围
 * \param x 绘制点x
 * \param y 绘制点y
 * \param str 绘制的文本
 * \return 返回一个表的数组，每个表具有下列元素。
 *	- pos 字形在字符串中的索引位置(从1开始)
 *	- x 字形在x轴的位置
 *	- minx 字形的范围
 *	- maxx 字形的范围
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

/**
 * \brief 返回当前字体下文本的几个参数。
 * \image html 361px-Typography_Line_Terms.svg.png
 * \retval ascender 字母顶部距基线的距离
 * \retval descender 字母底部距基线的距离 
 * \retval lineh 行的高度
 */
static int lua_textMetrics(lua_State *L)
{
	float ascender, descender, lineh;
	nvgTextMetrics(_vg, &ascender, &descender, &lineh);
	lua_pushnumber(L, ascender);
	lua_pushnumber(L, descender);
	lua_pushnumber(L, lineh);
	return 3;
}

/**
 * \brief 对当前字体下的文本进行折行计算，返回多行文本的折行位置和每一行的宽度。
 * \return 返回一个表的数组每一个表代表一行。表的拥有下列元素。
 *	- head 行开始的文字索引(从1开始)
 *	- tail 行的结束索引(从1开始)
 *	- next 下一行的开始文字索引(从1开始)
 *	- width 行的宽度
 *	- minx 行的x范围
 *	- maxx 行的x范围
 */
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

/**
 * 完成路径描述。见 #lua_beginPath
 */
static int lua_closePath(lua_State *L)
{
	nvgClosePath(_vg);
	return 0;
}

/**
 * \breif 从HSL颜色转换为RGB
 * \param h 色调
 * \param s 饱和度
 * \param l 亮度
 * \param a 透明度
 * \retval r
 * \retval g
 * \retval b
 * \retval a
 */
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

/**
 * 将度转换为弧度
 */
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

/**
 * 将unicode字符转换为utf8字符
 * \param c unicode字符
 * \return 返回utf8字符
 */
static int lua_cpToUTF8(lua_State *L)
{
	char str[8];
	cpToUTF8(luaL_checkinteger(L, 1), str);
	lua_pushstring(L, str);
	return 1;
}

/**
 * @}
 */
 
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