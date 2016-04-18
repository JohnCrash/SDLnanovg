#ifndef _LUAUI_H_
#define _LUAUI_H_

#include "lua.h"
#include "lauxlib.h"

#ifdef __cplusplus
extern "C"{
#endif
	int luaopen_ui(lua_State *L);
#ifdef __cplusplus
}
#endif
#endif