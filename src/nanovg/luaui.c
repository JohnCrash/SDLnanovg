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

static const struct luaL_Reg uimeta_methods_c[] =
{
	{ "addChild", lua_addChild },
	{ "removeFromParent", lua_removeFromParent },
	{ "getSize", lua_sizeWidget },
	{ "getPosition", lua_positionWidget },
	{ "setSize", lua_setSizeWidget },
	{ "setPosition", lua_setPositionWidget },
	{ NULL, NULL },
};

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
		for (int i = 0; i < sizeof(uimeta_methods_c) / sizeof(luaL_Reg); i++){
			if (uimeta_methods_c[i].name && strcmp(uimeta_methods_c[i].name, key) == 0){
				lua_pushcfunction(L, uimeta_methods_c[i].func);
				return 1;
			}
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