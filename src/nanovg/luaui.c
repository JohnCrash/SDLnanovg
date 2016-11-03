#include "luaui.h"
#include "ui.h"

#if LUA_VERSION_NUM < 502
#  define luaL_newlib(L,l) (lua_newtable(L), luaL_register(L,NULL,l))
#endif

#define LUA_UI_HANDLE "lua_nanoui_t"
#define max(a, b)  (((a) > (b)) ? (a) : (b)) 
/**
 * \addtogroup LuaUI lua ui
 * \brief 界面相关的lua函数
 * @{
 */
/**
 * 如果没有对应的luaWidget对象，就创建一个luaWidget对象。
 * 并将其压入到lua堆栈，否则将引用的luaWidget压入到堆栈。
 */
void lua_pushWidget(lua_State *L, uiWidget * widget)
{
	if (widget->luaobj && widget->luaobj->ref!=LUA_REFNIL){
		lua_getref(L, widget->luaobj->ref);
	}
	else{
		luaWidget *pluawidget = (luaWidget *)lua_newuserdata(L, sizeof(luaWidget));
		if (pluawidget){
			pluawidget->widget = (struct uiWidget *)widget;
			luaL_getmetatable(L, LUA_UI_HANDLE);
			lua_setmetatable(L, -2);
			lua_pushvalue(L, -1);
			pluawidget->ref = lua_ref(L, 1);

			widget->luaobj = pluawidget;
		}
	}
}

static uiWidget * lua_checkWidget(lua_State *L, int idx)
{
	luaWidget * luaobj = (luaWidget *)luaL_checkudata(L, idx, LUA_UI_HANDLE);
	if (luaobj){
		return (uiWidget *)luaobj->widget;
	}
	return NULL;
}

/**
 * \brief 创建一个界面控件，在lua中的调用方式 ui.createWidget(themes,name)
 * \param themes 界面的颜色表名称，样式表可以通过 #lua_loadThemes 加载。
 * \param name 界面控件名称，由样式表决定。
 * \return 成功返回一个界面控件，失败返回nil。
 */
static int lua_createWidget(lua_State *L)
{
	const char * themes = luaL_checkstring(L, 1); 
	const char * name = luaL_checkstring(L, 2);
	uiWidget * widget = uiCreateWidget(themes, name);
	if (widget){
		lua_pushWidget(L, widget);
		return 1;
	}
	lua_pushnil(L);
	return 1;
}
	
/**
 * \brief 删除控件，在lua中的调用方式 ui.deleteWidget(widget)
 * \param self 要删除的控件
 */
static int lua_deleteWidget(lua_State *L)
{
	uiWidget *widget = lua_checkWidget(L, 1);
	if (widget)
		uiDeleteWidget(widget);
	return 0;
}

/**
 * \brief 从一个json文件创建一个控件，ui.formJson(filename)
 * \param filename json文件路径
 * \return 成功返回一个控件，失败返回nil
 */
static int lua_formJson(lua_State *L)
{
	uiWidget * widget = uiFormJson(luaL_checkstring(L, 1));
	if (widget){
		lua_pushWidget(L, widget);
	}
	else{
		lua_pushnil(L);
	}
	return 1;
}

/**
 * \brief 返回根控件，ui.rootWidget()
 * \return 成功返回根控件，失败返回nil
 */
static int lua_rootWidget(lua_State *L)
{
	lua_pushWidget(L, uiRootWidget());
	return 1;
}

/**
 * 如果自己对自己有引用，c对象存在那么lua对象就存在
 * 只有在c对象释放，断开引用关系，才会走到lua_gcWidget
 * 并且这时候luaWidget中的widget=NULL,ref=LUA_REFNIL
 * 因此不需要做任何处理。
 */
static int lua_gcWidget(lua_State *L)
{
	return 0;
}

/**
 * \brief 想控件添加子节点,self:addChild(widget)
 * \param widget 要添加的子节点
 */
static int lua_addChild(lua_State *L)
{
	uiWidget *parent = lua_checkWidget(L, 1);
	uiWidget *child = lua_checkWidget(L, 2);
	uiAddChild(parent, child);
	return 0;
}

/**
 * \brief 将控件加入到子节点的最下面，self:addChildTail(widget)
 * \param widget 要添加的子节点
 */
static int lua_addChildTail(lua_State *L)
{
	uiWidget *parent = lua_checkWidget(L, 1);
	uiWidget *child = lua_checkWidget(L, 2);
	uiAddChildToTail(parent, child);
	return 0;
}

/**
 * \brief 从父节点上移除该子节点，改操作并不会删除控件。self:removeFromParent()
 * \note 在事件中调用该函数不会导致任何问题。
 */
static int lua_removeFromParent(lua_State *L)
{
	uiWidget *self = lua_checkWidget(L, 1);
	uiRemoveFromParent(self);
	return 0;
}

/**
 * \brief 返回控件的尺寸,self:getSize()
 * \retval width 宽度
 * \retval height 高度
 * \note 如果self不是一个有效的控件，返回nil
 */
static int lua_sizeWidget(lua_State *L)
{
	uiWidget *self = lua_checkWidget(L, 1);
	if (self){
		lua_pushnumber(L, self->width);
		lua_pushnumber(L, self->height);
	}
	else{
		lua_pushnil(L);
		lua_pushnil(L);
	}
	return 2;
}

/**
 * \brief 返回控件的位置,self:getPosition()
 * \retval x 位置的x坐标
 * \retval y 位置的y坐标
 * \note 如果self不是一个有效的控件，返回nil
 */
static int lua_positionWidget(lua_State *L)
{
	uiWidget *self = lua_checkWidget(L, 1);
	if (self){
		lua_pushnumber(L, self->x);
		lua_pushnumber(L, self->y);
	}
	else{
		lua_pushnil(L);
		lua_pushnil(L);
	}
	return 2;
}

/**
 * \brief 设置控件的尺寸,self:setSize(w,h)
 * \param w 宽度
 * \param h 高度
 */
static int lua_setSizeWidget(lua_State *L)
{
	uiWidget *self = lua_checkWidget(L, 1);
	float w = (float)luaL_checknumber(L, 2);
	float h = (float)luaL_checknumber(L, 3);
	if (self){
		float ow, oh;
		if (self->hookRef != LUA_REFNIL){
			ow = self->width;
			oh = self->height;
		}
		uiSetSize(self, w, h);
		/*
		 * 调用钩子函数，通知widget被设置了新的尺寸
		 */
		if (self->hookRef != LUA_REFNIL){
			lua_getref(L, self->hookRef);
			lua_pushstring(L, "setSize");
			lua_pushnumber(L, ow);
			lua_pushnumber(L, oh);
			lua_pushnumber(L, self->width);
			lua_pushnumber(L, self->height);
			lua_executeFunction(5);
		}
	}
	return 0;
}

/**
 * \brief 设置控件的位置,self:setPostion(x,y)
 * \param x 位置的x坐标
 * \param y 位置的y坐标
 */
static int lua_setPositionWidget(lua_State *L)
{
	uiWidget *self = lua_checkWidget(L, 1);
	float x = (float)luaL_checknumber(L, 2);
	float y = (float)luaL_checknumber(L, 3);
	if (self){
		float ox, oy;
		if (self->hookRef != LUA_REFNIL){
			ox = self->x;
			oy = self->y;
		}
		uiSetPosition(self, x, y);
		/*
		* 调用钩子函数，通知widget被设置了新的位置
		*/
		if (self->hookRef != LUA_REFNIL){
			lua_getref(L, self->hookRef);
			lua_pushstring(L, "setPosition");
			lua_pushnumber(L, ox);
			lua_pushnumber(L, oy);
			lua_pushnumber(L, self->x);
			lua_pushnumber(L, self->y);
			lua_executeFunction(5);
		}
	}
	return 0;
}

/**
 * \brief 加载样式表。ui.loadThemes(name,filename)
 * \param name 样式表名称
 * \param filename 样式表路径
 * \return 成功返回true,失败返回false
 */
static int lua_loadThemes(lua_State *L)
{
	const char * name = luaL_checkstring(L, 1);
	const char * filename = luaL_checkstring(L, 2);
	lua_pushboolean(L,loadThemes(name, filename));
	return 1;
}

/**
 * \brief 取得缩放比例,self:getScale()
 * \retval sx 在x方向上缩放比例
 * \retval sy 在y方向上缩放比例
 */
static int lua_getScale(lua_State *L)
{
	uiWidget *self = lua_checkWidget(L, 1);
	if (self){
		lua_pushnumber(L, self->sx);
		lua_pushnumber(L, self->sy);
	}
	else{
		lua_pushnumber(L, 0);
		lua_pushnumber(L, 0);
	}
	return 2;
}

/**
 * \brief 设置控件的缩放比例,self:setScale(sx,sy)
 * \param sx x方向上的缩放比例
 * \param sy y方向上的缩放比例
 * \param ox 旋转、缩放中心点x坐标
 * \param oy 旋转、缩放中心点y坐标
 * \note ox,oy是可选值，默认为0 
 */
static int lua_setScale(lua_State *L)
{
	uiWidget *self = lua_checkWidget(L, 1);
	float sx = (float)luaL_checknumber(L, 2);
	float sy = (float)luaL_checknumber(L, 3);
	if (self){
		/* 设置缩放中心点 */
		if (lua_isnumber(L, 4) && lua_isnumber(L, 5)){
			float ox = (float)luaL_checknumber(L, 4);
			float oy = (float)luaL_checknumber(L, 5);
			self->ox = ox;
			self->oy = oy;
		}
		uiScale(self, sx, sy);
	}
	return 0;
}

/**
 * \brief 取得控件的旋转角度(弧度),self:getRotate()
 * \return 返回角度。
 * \note 如果self不是一个有效的控件，返回nil
 */
static int lua_getRotate(lua_State *L)
{
	uiWidget *self = lua_checkWidget(L, 1);
	if (self){
		lua_pushnumber(L, self->angle);
	}
	else{
		lua_pushnumber(L, 0);
	}
	return 1;
}

/**
 * \brief 设置控件的旋转角度,self:setRotate(angle,ox,oy)
 * \param angle 旋转角度
 * \param ox 旋转、缩放中心点x坐标
 * \param oy 旋转、缩放中心点y坐标
 * \note ox,oy是可选值，默认为0
 */
static int lua_setRotate(lua_State *L)
{
	uiWidget *self = lua_checkWidget(L, 1);
	float angle = (float)luaL_checknumber(L, 2);
	if (self){
		/* 设置旋转中心点 */
		if (lua_isnumber(L, 3) && lua_isnumber(L, 4)){
			float ox = (float)luaL_checknumber(L, 3);
			float oy = (float)luaL_checknumber(L, 4);
			self->ox = ox;
			self->oy = oy;
		}
		uiRotate(self,angle);
	}
	return 0;
}

/**
 * \brief 取得旋转中心点,self:getOrigin()
 * \retval ox 旋转、缩放中心点x坐标
 * \retval oy 旋转、缩放中心点y坐标
 */
static int lua_getOrigin(lua_State *L)
{
	uiWidget *self = lua_checkWidget(L, 1);
	if (self){
		lua_pushnumber(L, self->ox);
		lua_pushnumber(L, self->oy);
	}
	else{
		lua_pushnumber(L, 0);
		lua_pushnumber(L, 0);
	}
	return 2;
}

/**
 * \brief 打开控件的剪切区，self:enableClip(b)
 * \param b 打开或者关闭剪切区
 */
static int lua_enableClip(lua_State *L)
{
	uiWidget *self = lua_checkWidget(L, 1);
	if (self){
		if (lua_isboolean(L,2))
			enableClipClient(self, lua_toboolean(L, 2)?1:0);
		else
			enableClipClient(self, 0);
	}
	return 0;
}

static int lua_enableFlags(lua_State *L)
{
	uiWidget *self = lua_checkWidget(L, 1);
	int flags = luaL_checkint(L, 2);
	if (self){
		self->isVisible |= flags;
	}
	return 0;
}

static int lua_disableFlags(lua_State *L)
{
	uiWidget *self = lua_checkWidget(L, 1);
	int flags = luaL_checkint(L, 2);
	if (self){
		self->isVisible &= !flags;
	}
	return 0;
}

/**
 * \brief 返回样式表中的给定函数,ui.themeFunction(func)
 * \param func 函数名称
 * \return 成功返回一个函数，失败返回nil
 */
static int lua_themeFunction(lua_State *L)
{
	uiWidget *self = lua_checkWidget(L, 1);
	const char *fname = luaL_checkstring(L, 2);
	/* 在类表中找对应的元素 */
	if (self->classRef != LUA_REFNIL){
		lua_getref(L, self->classRef);
		lua_getfield(L, -1, fname);
		if (!lua_isnoneornil(L, -1)){
			lua_remove(L, -2);
			lua_remove(L, -2);
			return 1;
		}
		else{
			lua_pop(L, 3);
		}
	}
	lua_pushnil(L);
	return 1;
}

/**
 * \brief 将给定的控件放到屏幕最上面,self:bringTop()
 */
static int lua_bringTop(lua_State *L)
{
	uiWidget *self = lua_checkWidget(L, 1);
	if (self){
		uiBringTop(self);
	}
	return 0;
}

/**
 * \brief 将给定的控件放到屏幕最下面,self:bringBottom()
 */
static int lua_bringBottom(lua_State *L)
{
	uiWidget *self = lua_checkWidget(L, 1);
	if (self){
		uiBringBottom(self);
	}
	return 0;
}

/**
 * \brief 返回控件的子节点,self:childs()
 * \return 成功返回一个子节点表，失败返回nil
 */
static int lua_childs(lua_State *L)
{
	uiWidget *self = lua_checkWidget(L, 1);
	if (self){
		int idx = 1;
		uiWidget * child = self->child;
		lua_newtable(L);
		while (child){
			lua_pushWidget(L, child);
			/*
			 * -1 child
			 * -2 table
			 */
			lua_rawseti(L, -2, idx++);
			child = child->next;
		}
	}
	else{
		lua_pushnil(L);
	}
	return 1;
}

/**
 * \brief 打开控件的事件处理，self:enableEvent(e)
 * \param e 接收的事件以及处理事件的方式，可以是下面的一个和多个值的组合
 *	- ui. #EVENT_NONE
 *	- ui. #EVENT_TOUCHDOWN
 *	- ui. #EVENT_TOUCHDROP
 *	- ui. #EVENT_TOUCHUP
 *	- ui. #EVENT_ZOOM
 *	- ui. #EVENT_BREAK
 *	- ui. #EVENT_EXCLUSIVE
 */
static int lua_enableEvent(lua_State *L)
{
	uiWidget *self = lua_checkWidget(L, 1);
	int v = luaL_checkinteger(L, 2);
	if (self){
		uiEnableEvent(self,v);
	}
	return 0;
}

/**
 * \brief 关闭控件的指定事件.self:disableEvent(e)
 * \param e 关闭控件的指定事件以及处理事件的方式，可以是下面的一个和多个值的组合
 *	- ui. #EVENT_NONE
 *	- ui. #EVENT_TOUCHDOWN
 *	- ui. #EVENT_TOUCHDROP
 *	- ui. #EVENT_TOUCHUP
 *	- ui. #EVENT_ZOOM
 *	- ui. #EVENT_BREAK
 *	- ui. #EVENT_EXCLUSIVE 
 */
static int lua_disableEvent(lua_State *L)
{
	uiWidget *self = lua_checkWidget(L, 1);
	int v = luaL_checkinteger(L, 2);
	if (self){
		uiDisableEvent(self, v);
	}
	return 0;
}

/**
 * \brief 卸载掉给定名称的样式表,ui.unloadTheme(name)
 * \param name 样式表名称
 */
static int lua_unloadTheme(lua_State *L)
{
	const char * name = luaL_checkstring(L, 1);
	unloadThemes(name);
	return 0;
}

/**
 * \brief 将根节点坐标向控件坐标转换,self:rootToWidget(x,y)
 * \param x 要转换点的x坐标 
 * \param y 要转换点的y坐标
 * \retval x 转换完成的x坐标
 * \retval y 转换完成的y坐标
 */
static int lua_rootToWidget(lua_State *L)
{
	uiWidget * widget = lua_checkWidget(L, 1);
	float pt[2];
	pt[0] = (float) luaL_checknumber(L, 2);
	pt[1] = (float)luaL_checknumber(L, 3);
	if (widget){
		uiRootToWidget(widget, pt, 2);
		lua_pushnumber(L, pt[0]);
		lua_pushnumber(L, pt[1]);
	}
	else{
		lua_pushnumber(L, 0);
		lua_pushnumber(L, 0);
	}
	return 2;
}

/**
 * \brief 将控件坐标向根节点坐标转换,self:widgetToRoot(x,y)
 * \param x 要转换点的x坐标 
 * \param y 要转换点的y坐标
 * \retval x 转换完成的x坐标
 * \retval y 转换完成的y坐标
 */
static int lua_widgetToRoot(lua_State *L)
{
	uiWidget * widget = lua_checkWidget(L, 1);
	float pt[2];
	pt[0] = (float)luaL_checknumber(L, 2);
	pt[1] = (float)luaL_checknumber(L, 3);
	if (widget){
		uiWidgetToRoot(widget, pt, 1);
		lua_pushnumber(L, pt[0]);
		lua_pushnumber(L, pt[1]);
	}
	else{
		lua_pushnumber(L, 0);
		lua_pushnumber(L, 0);
	}
	return 2;
}

/**
 * \brief 返回在给定屏幕点处的全部控件,ui.widgetFormPt(x,y)
 * \param x 点的x坐标
 * \param y 点的y坐标
 * \return 返回一个被x,y点穿透的控件表。
 */
static int lua_widgetFormPt(lua_State *L)
{
	float x = (float)luaL_checknumber(L, 1);
	float y = (float)luaL_checknumber(L, 2);
	uiWidget * wp[32];
	int n = uiWidgetFormPt(x, y, wp, 32);
	lua_newtable(L);
	for (int i = 0; i < n; i++){
		lua_pushWidget(L, wp[i]);
		lua_rawseti(L, -2, i + 1);
	}
	return 1;
}

/**
 * \brief 设置控件的事件回调，self:widgetHook(hookFunc)
 * \param hookFunc 事件回调函数，nil将清除事件回调
 * \return 返回老的事件回调函数，如果没有返回nil
 */
static int lua_widgetHook(lua_State *L)
{
	uiWidget * widget = lua_checkWidget(L, 1);
	int oldHookRef = widget->hookRef;
	if (lua_isfunction(L, 2)){
		lua_pushvalue(L, 2);
		widget->hookRef = lua_ref(L, 1);
	}
	if (widget->hookRef != LUA_REFNIL){
		lua_getref(L, oldHookRef);
		lua_unref(L, oldHookRef);
	}else{
		lua_pushnil(L);
	}
	return 1;
}

static void relayout_raw(uiWidget *col[], float w[], float maxh, float sx,float y,int n, int mode)
{
	float x = 0;
	for (int i = 0; i < n; i++){
		if (mode&ALIGN_LEFT)
			col[i]->x = x+sx;
		else if (mode&ALIGN_RIGHT)
			col[i]->x = x+w[i]-col[i]->width-sx;
		else
			col[i]->x = x + (w[i] - col[i]->width)/2;
		if (mode&ALIGN_TOP)
			col[i]->y = y;
		else if (mode&ALIGN_BOTTOM)
			col[i]->y = y+maxh-col[i]->height;
		else
			col[i]->y = y + (maxh - col[i]->height)/2;
		uiSetPosition(col[i], col[i]->x, col[i]->y);
		x += w[i];
	}
}
/**
 * \brief 排列子窗口self:relayout(mode,sx,sy,grid_n),函数将设置控件的尺寸
 * \param mode 可以是下面值的组合
 *	- ui.ALIGN_LEFT 左对齐
 *	- ui.ALIGN_CENTER 横向中心对齐
 *	- ui.ALIGN_RIGHT 右对齐
 *	- ui.ALIGN_TOP 上对齐
 *	- ui.ALIGN_MIDDLE 纵向中心对齐
 *	- ui.ALIGN_BOTTOM 下对齐
 *	- ui.HORIZONTAL	横向排序，这时sx表示物件的间距，sy表示上部的间距
 *	- ui.VERTICAL 纵向排序，这时sx表示左侧间距，sy表示每个物件的间距
 *	- ui.GRID 网格排序，和HORIZONTAL组合先横向排布grid_n个物件，然后排布第二排
 *和VERTICAL组合先纵向排布grid_n个物件
 *	- ui.REVERSE 方向顺序
 * \param sx 横向间隔
 * \param sy 纵向间隔
 * \param grid_n 表示网格
 */
static int lua_relayout(lua_State *L)
{
	uiWidget * it, * last;
	uiWidget * widget = lua_checkWidget(L, 1);
	int mode = luaL_checkinteger(L, 2);
	float sx = (float)luaL_checknumber(L, 3);
	float sy = (float)luaL_checknumber(L, 4);
	float w, h, x, y;
	w = h = 0;
	if (mode&HORIZONTAL){
		it = widget->child;
		last = it;
		while (it){
			last = it;
			h = max(h, it->height);
			it = it->next;
		}
		if (mode&REVERSE)
			it = last;
		else
			it = widget->child;
		x = sx;
		while (it){
			it->x = x;
			if (mode&ALIGN_TOP)
				it->y = sy + sx;
			else if (mode&ALIGN_BOTTOM)
				it->y = h-it->height-sx+sy;
			else
				it->y = (h-it->height)/2+sy;
			uiSetPosition(it, it->x, it->y);
			x += (it->width+sx);
			if (mode&REVERSE)
				it = it->prev;
			else
				it = it->next;
		}
		widget->width = x;
		widget->height = h+sy+2*sx;
	}
	else if (mode&VERTICAL){
		it = widget->child;
		last = it;
		while (it){
			last = it;
			w = max(w, it->width);
			it = it->next;
		}
		if (mode&REVERSE)
			it = last;
		else
			it = widget->child;
		y = sy;
		while (it){
			it->y = y;
			if (mode&ALIGN_LEFT)
				it->x = sy + sx;
			else if (mode&ALIGN_RIGHT)
				it->x = w - it->width - sy + sx;
			else
				it->x = (w - it->width) / 2 + sx;
			uiSetPosition(it, it->x, it->y);
			y += (it->height + sy);
			if (mode&REVERSE)
				it = it->prev;
			else
				it = it->next;
		}
		widget->width = w + sx + 2*sy;
		widget->height = y;
	}
	else if ( (mode&GRID) && lua_isnumber(L,5)){
		float ww[32];
		uiWidget * col[32];
		int i = 0;
		int grid_n = lua_tointeger(L, 5);
		if (grid_n >= 32 || grid_n < 0)return 0;

		it = widget->child;
		last = it;
		while (it){
			last = it;
			ww[i%grid_n] = max(ww[i%grid_n], it->width+2*sx);
			i++;
			it = it->next;
		}
		if (mode&REVERSE)
			it = last;
		else
			it = widget->child;
		i = 0;
		y = sy;
		while (it){
			h = max(h, it->height);
			if ((i%grid_n == 0) && i != 0){
				relayout_raw(col, ww, h,sx,y, grid_n,mode);
				y += (h + sy);
				h = 0;
			}
			col[i%grid_n] = it;
			if (mode&REVERSE)
				it = it->prev;
			else
				it = it->next;
			i++;
		}
		if (i>0){
			int n = i%grid_n == 0 ? grid_n : i%grid_n;
			relayout_raw(col, ww, h, sx, y, n, mode);
			y += (h + sy);
		}
		x = 0;
		int n = i < grid_n ? i : grid_n;
		for (i = 0; i < n; i++)
			x += ww[i];
		widget->width = x;
		widget->height = y;
	}
	else{
		return 0;
	}
	lua_pushboolean(L, 1);
	return 1;
}

static int lua_widgetFunction(lua_State *L);

static const struct luaL_Reg uimeta_methods_c[] =
{
	{ "addChild", lua_addChild },
	{ "addChildTail", lua_addChildTail },
	{ "removeFromParent", lua_removeFromParent },
	{ "getSize", lua_sizeWidget },
	{ "getPosition", lua_positionWidget },
	{ "setSize", lua_setSizeWidget },
	{ "setPosition", lua_setPositionWidget },
	{ "getScale", lua_getScale },
	{ "setScale", lua_setScale },
	{ "getRotate", lua_getRotate },
	{ "setRotate", lua_setRotate },
	{ "getOrigin", lua_getOrigin },
	{ "widgetFunction", lua_widgetFunction },
	{ "themeFunction", lua_themeFunction },
	{ "bringTop", lua_bringTop },
	{ "bringBottom", lua_bringBottom },
	{ "childs", lua_childs },
	{ "enableEvent",lua_enableEvent },
	{ "disableEvent", lua_disableEvent },
	{ "relayout", lua_relayout },
	{ "enableClip", lua_enableClip },
	{ "enableFlags", lua_enableFlags },
	{ "disableFlags", lua_disableFlags },
	{ "widgetHook", lua_widgetHook },
	{ NULL, NULL },
};

/**
 * **根据名称返回函数**
 * 
 * 函数在查找名称的时候做了速度优化。
 */
static lua_CFunction getWidgetCFunction(const char * name)
{
	int j;
	int len = strlen(name);
	for (int i = 0; i < sizeof(uimeta_methods_c) / sizeof(luaL_Reg)-1; i++){
		const char * s = uimeta_methods_c[i].name;
		/*
		 * 这里优化strcmp(s,name)==0
		 */
		for (j = 0; j < len; j++){
			if (s[j] && s[j] == name[j]){
				continue;
			}
			else break;
		}
		if (j==len)
			return uimeta_methods_c[i].func;
	}
	return NULL;
}

/**
 * \brief 取得对象的C函数,self:widgetFunction(func)
 * \param func 函数名称
 * \return 成功返回一个成员函数，失败返回nil
 */
static int lua_widgetFunction(lua_State *L)
{
	uiWidget *self = lua_checkWidget(L, 1);
	const char *fname = luaL_checkstring(L, 2);
	lua_CFunction func = getWidgetCFunction(fname);
	if (func)
		lua_pushcfunction(L, func);
	else
		lua_pushnil(L);
	return 1;
}

/**
 * 取一个对象的元素顺序是先对象表，然后类表，然后才是c提供的默认方法
 * \note 这个顺序允许在不同层上重载控件行为
 */
static int lua_indexWidget(lua_State *L)
{
	uiWidget *self = lua_checkWidget(L, 1);
	const char * key = luaL_checkstring(L, 2);
	if (self){
		/* 在对象表中找对应的元素 */
		if (self->selfRef != LUA_REFNIL){
			lua_getref(L, self->selfRef);
			lua_getfield(L, -1, key);
			if (!lua_isnoneornil(L, -1)){
				lua_remove(L, -2);
				lua_remove(L, -2);
				return 1;
			}
			else{
				lua_pop(L, 3);
			}
		}
		/* 在类表中找对应的元素 */
		if (self->classRef!=LUA_REFNIL){
			lua_getref(L, self->classRef);
			lua_getfield(L, -1, key);
			if (!lua_isnoneornil(L, -1)){
				lua_remove(L, -2);
				lua_remove(L, -2);
				return 1;
			}
			else{
				lua_pop(L, 3);
			}
		}
		lua_CFunction func = getWidgetCFunction(key);
		if (func){
			lua_pushcfunction(L, func);
			return 1;
		}
	}
	lua_pushnil(L);
	return 1;
}

/**
 * 将新加的索引项都加入到selfRef表中
 */
static int lua_newindexWidget(lua_State *L)
{
	uiWidget *self = lua_checkWidget(L, 1);
	const char * key = luaL_checkstring(L, 2);
	if (self){
		if (self->selfRef != LUA_REFNIL){
			lua_getref(L, self->selfRef);
			lua_pushvalue(L, 3);
			lua_setfield(L, -2, key);
			lua_pop(L, 1);
		}
	}
	return 0;
}

/**
 * @}
 */
 
static const struct luaL_Reg uimeta_methods[] =
{
	{ "__gc", lua_gcWidget },
	{ "__index",lua_indexWidget },
	{ "__newindex",lua_newindexWidget},
	{ NULL, NULL },
};

static const struct luaL_Reg nanoui_methods[] =
{
	{ "createWidget", lua_createWidget },
	{ "deleteWidget", lua_deleteWidget },
	{ "rootWidget", lua_rootWidget },
	{ "formJson", lua_formJson },
	{ "loadThemes", lua_loadThemes },
	{ "unloadTheme", lua_unloadTheme },
	{ "rootToWidget", lua_rootToWidget },
	{ "widgetToRoot", lua_widgetToRoot },
	{ "widgetFormPt", lua_widgetFormPt },
	{ NULL, NULL },
};

#define VERSION "1.0"

static void set_info(lua_State *L)
{
	lua_pushliteral(L, "_COPYRIGHT");
	lua_pushliteral(L, "Copyright (C) 2016");
	lua_settable(L, -3);
	lua_pushliteral(L, "_DESCRIPTION");
	lua_pushliteral(L, "nanovgui library.");
	lua_settable(L, -3);
	lua_pushliteral(L, "_VERSION");
	lua_pushliteral(L, "nanovgUI " VERSION);
	lua_settable(L, -3);
}

static void createmeta(lua_State *L)
{
	/*
	 * metatable.__index = metatable
	 * push metatable
	 */
	luaL_newmetatable(L, LUA_UI_HANDLE);
	lua_pushliteral(L, "__index");
	lua_pushvalue(L, -2);
	lua_rawset(L, -3);
}

int luaopen_ui(lua_State *L)
{
	createmeta(L);
	luaL_openlib(L, 0, uimeta_methods, 0);
	lua_newtable(L);
	luaL_newlib(L, nanoui_methods);
	set_info(L);
	return 1;
}