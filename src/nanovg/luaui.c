#include "luaui.h"
#include "ui.h"

#if LUA_VERSION_NUM < 502
#  define luaL_newlib(L,l) (lua_newtable(L), luaL_register(L,NULL,l))
#endif

#define LUA_UI_HANDLE "lua_nanoui_t"

typedef struct {
	uiWidget * widget;
} luaWidget;

int lua_createWidget(lua_State *L)
{
	const char * themes = luaL_checkstring(L, 1); 
	const char * name = luaL_checkstring(L, 1);
	uiWidget * widget = uiCreateWidget(themes, name);
	if (widget){
		luaWidget *pluawidget = (luaWidget *)lua_newuserdata(L, sizeof(luaWidget));
		if (pluawidget){
			pluawidget->widget = widget;
			luaL_getmetatable(L, LUA_UI_HANDLE);
			lua_setmetatable(L, -2);
			return 1;
		}
	}
	lua_pushnil(L);
	return 1;
}
	
int lua_deleteWidget(lua_State *L)
{
	return 0;
}

int lua_formJson(lua_State *L)
{
	return 0;
}

int lua_rootWidget(lua_State *L)
{
	return 0;
}

int lua_gcWidget(lua_State *L)
{
	return 0;
}

static const struct luaL_Reg uimeta_methods[] =
{
	{ "__gc", lua_gcWidget },
	{ NULL, NULL },
};

static const  struct luaL_Reg uimember_methods[] =
{
	{ NULL, NULL },
};

static const struct luaL_Reg nanoui_methods[] =
{
	{ "createWidget", lua_createWidget },
	{ "deleteWidget", lua_deleteWidget },
	{ "rootWidget", lua_rootWidget },
	{ "formJson", lua_formJson },
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