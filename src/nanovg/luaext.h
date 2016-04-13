#ifndef _LUAEXT_H_
#define _LUAEXT_H_
#ifdef __cplusplus
extern "C"{
#endif
	enum LuaEventType{
		EVENT_INIT,
		EVENT_RELEASE,
		EVENT_MAINLOOP,
		EVENT_TOUCH,
	} ;

	typedef struct{
		union{
			struct{
				int ntouch;
				float pt[8];
			};
		};
	} EventParam;

	int initLua();
	void releaseLua();
	int luaEvent(int type,EventParam *param);

	void lua_addSearchPath(const char* path);
	int lua_executeFunction(int numArgs);
	int lua_executeString(const char *code);
	int lua_executeScriptFile(const char *filename);
#ifdef __cplusplus
}
#endif
#endif