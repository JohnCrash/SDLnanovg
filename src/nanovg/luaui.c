#include "luaui.h"
#include "ui.h"

#if LUA_VERSION_NUM < 502
#  define luaL_newlib(L,l) (lua_newtable(L), luaL_register(L,NULL,l))
#endif

#define LUA_UI_HANDLE "lua_nanoui_t"

/*
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
			pluawidget->widget = widget;
			luaL_getmetatable(L, LUA_UI_HANDLE);
			lua_setmetatable(L, -2);
			lua_pushvalue(L, -1);
			pluawidget->ref = lua_ref(L, 1);

			widget->luaobj = pluawidget;
		}
	}
}

uiWidget * lua_checkWidget(lua_State *L, int idx)
{
	luaWidget * luaobj = (luaWidget *)luaL_checkudata(L, idx, LUA_UI_HANDLE);
	if (luaobj){
		return luaobj->widget;
	}
	return NULL;
}

int lua_createWidget(lua_State *L)
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
	
int lua_deleteWidget(lua_State *L)
{
	uiWidget *widget = lua_checkWidget(L, 1);
	if (widget)
		uiDeleteWidget(widget);
	return 0;
}

int lua_formJson(lua_State *L)
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

int lua_rootWidget(lua_State *L)
{
	lua_pushWidget(L, uiRootWidget());
	return 1;
}

/*
 * 如果自己对自己有引用，c对象存在那么lua对象就存在
 * 只有在c对象释放，断开引用关系，才会走到lua_gcWidget
 * 并且这时候luaWidget中的widget=NULL,ref=LUA_REFNIL
 * 因此不需要做任何处理。
 */
int lua_gcWidget(lua_State *L)
{
	return 0;
}

int lua_addChild(lua_State *L)
{
	uiWidget *parent = lua_checkWidget(L, 1);
	uiWidget *child = lua_checkWidget(L, 2);
	uiAddChild(parent, child);
	return 0;
}

int lua_addChildTail(lua_State *L)
{
	uiWidget *parent = lua_checkWidget(L, 1);
	uiWidget *child = lua_checkWidget(L, 2);
	uiAddChildToTail(parent, child);
	return 0;
}

int lua_removeFromParent(lua_State *L)
{
	uiWidget *self = lua_checkWidget(L, 1);
	uiRemoveFromParent(self);
	return 0;
}

int lua_sizeWidget(lua_State *L)
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

int lua_positionWidget(lua_State *L)
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

int lua_setSizeWidget(lua_State *L)
{
	uiWidget *self = lua_checkWidget(L, 1);
	float w = (float)luaL_checknumber(L, 2);
	float h = (float)luaL_checknumber(L, 3);
	if (self){
		uiSetSize(self, w, h);
	}
	return 0;

}

int lua_setPositionWidget(lua_State *L)
{
	uiWidget *self = lua_checkWidget(L, 1);
	float x = (float)luaL_checknumber(L, 2);
	float y = (float)luaL_checknumber(L, 3);
	if (self){
		uiSetPosition(self, x, y);
	}
	return 0;
}

int lua_loadThemes(lua_State *L)
{
	const char * name = luaL_checkstring(L, 1);
	const char * filename = luaL_checkstring(L, 2);
	lua_pushboolean(L,loadThemes(name, filename));
	return 1;
}

int lua_getScale(lua_State *L)
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

int lua_setScale(lua_State *L)
{
	uiWidget *self = lua_checkWidget(L, 1);
	float sx = luaL_checknumber(L, 2);
	float sy = luaL_checknumber(L, 3);
	if (self){
		/* 设置缩放中心点 */
		if (lua_isnumber(L, 4) && lua_isnumber(L, 5)){
			float ox = luaL_checknumber(L, 4);
			float oy = luaL_checknumber(L, 5);
			self->ox = ox;
			self->oy = oy;
		}
		uiScale(self, sx, sy);
	}
	return 0;
}

int lua_getRotate(lua_State *L)
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

int lua_setRotate(lua_State *L)
{
	uiWidget *self = lua_checkWidget(L, 1);
	float angle = luaL_checknumber(L, 2);
	if (self){
		/* 设置旋转中心点 */
		if (lua_isnumber(L, 3) && lua_isnumber(L, 4)){
			float ox = luaL_checknumber(L, 3);
			float oy = luaL_checknumber(L, 4);
			self->ox = ox;
			self->oy = oy;
		}
		uiRotate(self,angle);
	}
	return 0;
}

int lua_getOrigin(lua_State *L)
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

int lua_enableClip(lua_State *L)
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

int lua_themeFunction(lua_State *L)
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

int lua_bringTop(lua_State *L)
{
	uiWidget *self = lua_checkWidget(L, 1);
	if (self){
		uiBringTop(self);
	}
	return 0;
}

int lua_bringBottom(lua_State *L)
{
	uiWidget *self = lua_checkWidget(L, 1);
	if (self){
		uiBringBottom(self);
	}
	return 0;
}

int lua_childs(lua_State *L)
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
int lua_enableEvent(lua_State *L)
{
	uiWidget *self = lua_checkWidget(L, 1);
	int v = luaL_checkinteger(L, 2);
	if (self){
		uiEnableEvent(self,v);
	}
	return 0;
}

int lua_disableEvent(lua_State *L)
{
	uiWidget *self = lua_checkWidget(L, 1);
	int v = luaL_checkinteger(L, 2);
	if (self){
		uiDisableEvent(self, v);
	}
	return 0;
}

int lua_unloadTheme(lua_State *L)
{
	const char * name = luaL_checkstring(L, 1);
	unloadThemes(name);
	return 0;
}

int lua_rootToWidget(lua_State *L)
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

int lua_widgetToRoot(lua_State *L)
{
	uiWidget * widget = lua_checkWidget(L, 1);
	float pt[2];
	pt[0] = (float)luaL_checknumber(L, 2);
	pt[1] = (float)luaL_checknumber(L, 3);
	if (widget){
		uiWidgetToRoot(widget, pt, 2);
		lua_pushnumber(L, pt[0]);
		lua_pushnumber(L, pt[1]);
	}
	else{
		lua_pushnumber(L, 0);
		lua_pushnumber(L, 0);
	}
	return 2;
}

int lua_widgetFormPt(lua_State *L)
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

int lua_widgetFunction(lua_State *L);

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
	{ "enableClip", lua_enableClip },
	{ "widgetFunction", lua_widgetFunction },
	{ "themeFunction", lua_themeFunction },
	{ "bringTop", lua_bringTop },
	{ "bringBottom", lua_bringBottom },
	{ "childs", lua_childs },
	{ "enableEvent",lua_enableEvent },
	{ "disableEvent", lua_disableEvent },
	{ NULL, NULL },
};

/*
 * 根据名称返回函数
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

int lua_widgetFunction(lua_State *L)
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

/*
 * 取一个对象的元素顺序是先对象表，然后类表，然后才是c提供的默认方法
 */
int lua_indexWidget(lua_State *L)
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

/*
 * 将新加的索引项都加入到selfRef表中
 */
int lua_newindexWidget(lua_State *L)
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