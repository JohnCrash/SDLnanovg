#ifndef _LUAEXT_H_
#define _LUAEXT_H_
#ifdef __cplusplus
extern "C"{
#endif
	enum LuaEventType{
		EVENT_INIT = 0,
		EVENT_RELEASE,
		EVENT_LOOP,
		EVENT_TOUCH,
		EVENT_COUNT,
	} ;

	int initLua();
	void releaseLua();
	void lua_EventLoop(double dt);
	void lua_EventInit();
	void lua_EventRelease();

	void lua_addSearchPath(const char* path);
	int lua_executeFunction(int numArgs);
	int lua_executeString(const char *code);
	int lua_executeScriptFile(const char *filename);
#ifdef __cplusplus
}
#endif
#endif