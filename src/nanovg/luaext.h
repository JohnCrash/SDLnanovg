#ifndef _LUAEXT_H_
#define _LUAEXT_H_

#include "lua.h"
#include "lauxlib.h"

#ifdef __cplusplus
extern "C"{
#endif
	enum LuaEventType{
		EVENT_INIT = 0, /* 程序进入 */
		EVENT_RELEASE, /* 程序退出 */
		EVENT_LOOP,  /* 主循环，每次循环都会触发 */
		EVENT_INPUT, /* 输入事件，包括键盘，鼠标，触摸等 */
		EVENT_WINDOW, /* 窗口相关事件，包括尺寸改变，切换到前台后台等 */
		EVENT_COUNT,
	};

	int initLua();
	void releaseLua();
	void lua_EventLoop(double dt);
	double getLoopInterval();
	void lua_EventInit();
	void lua_EventRelease();
	void lua_EventChangeSize(int w, int h);
	void lua_EventWindowClose();
	void lua_EventWindow(const char * eventName);

	void lua_addSearchPath(const char* path);
	int lua_pushEventFunction(int n);
	int lua_executeFunction(int numArgs);
	int lua_executeFunctionResult(int numArgs, int nRet);
	int lua_executeString(const char *code);
	int lua_executeScriptFile(const char *filename);
	int lua_executeScriptFileResult(const char *filename,int nRet);
	void lua_callKeyboardFunc(const char *event,char *str);
	lua_State * lua_GlobalState();
#ifdef __cplusplus
}
#endif
#endif