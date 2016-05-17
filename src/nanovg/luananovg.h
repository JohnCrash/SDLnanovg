#ifndef _LUANANOVG_H_
#define _LUANANOVG_H_

#include "lua.h"
#include "lauxlib.h"

#ifdef __cplusplus
extern "C"{
#endif
	int lua_nanovgRender(lua_State *L);
	int luaopen_nanovg(lua_State *L);
#ifdef __cplusplus
}
#endif
#endif