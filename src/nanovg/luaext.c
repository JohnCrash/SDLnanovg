#include <string.h>
#include "SDL.h"
#include "luananovg.h"
#include "luaui.h"
#include "sdlmain.h"
#include "eventhandler.h"
#include "luaext.h"

static lua_State * _state = NULL;
static int _callFromLua = 0;
#define MAX_PATH 256

lua_State * lua_GlobalState()
{
	return _state;
}
/*
 * 自动增长的指针
 */
static void addstring( char **t,const char *s,int *len,int *cur)
{
	int l;
	char *temp;
	l = strlen(s);
	if (l + *cur >= *len){
		if (*len == MAX_PATH){
			*len = (*len + l) * 3 / 2;
			temp = (char*)malloc(*len);
			strcpy(temp, *t);
			*t = temp;
		}
		else{
			*len = (*len + l) * 3 / 2;
			*t = (char*)realloc(*t, *len);
		}
	}
	strcat(*t, s);
	*cur += l;
}

static int lua_print(lua_State * luastate)
{
	int nargs = lua_gettop(luastate);
	char buf[MAX_PATH]="";
	char *t;
	int len,cur;
	len = MAX_PATH;
	cur = 0;
	t = buf;
	for (int i = 1; i <= nargs; i++)
	{
		if (lua_istable(luastate, i))
			addstring(&t, "table", &len, &cur);
		else if (lua_isnone(luastate, i))
			addstring(&t, "none", &len, &cur);
		else if (lua_isnil(luastate, i))
			addstring(&t, "nil", &len, &cur);
		else if (lua_isboolean(luastate, i))
		{
			if (lua_toboolean(luastate, i) != 0)
				addstring(&t, "true", &len, &cur);
			else
				addstring(&t, "false", &len, &cur);
		}
		else if (lua_isfunction(luastate, i))
			addstring(&t, "function", &len, &cur);
		else if (lua_islightuserdata(luastate, i))
			addstring(&t, "lightuserdata", &len, &cur);
		else if (lua_isthread(luastate, i))
			addstring(&t, "thread", &len, &cur);
		else
		{
			const char * str = lua_tostring(luastate, i);
			if (str)
				addstring(&t, lua_tostring(luastate, i), &len, &cur);
			else
				addstring(&t, lua_typename(luastate, lua_type(luastate, i)), &len, &cur);
		}
		if (i != nargs)
			addstring(&t, "\t", &len, &cur);
	}
	SDL_Log("[LUA-print] %s", t);
	if (t!=buf)
		free(t);
	return 0;
}
/*
 * 下面的代码借鉴了cocos2d的加载器代码
 */
static void initLuaLoader(lua_CFunction func)
{
	if (!func) return;

	// stack content after the invoking of the function
	// get loader table
	lua_getglobal(_state, "package");                                  /* L: package */
	lua_getfield(_state, -1, "loaders");                               /* L: package, loaders */

	// insert loader into index 2
	lua_pushcfunction(_state, func);                                   /* L: package, loaders, func */
	for (int i = (int)(lua_objlen(_state, -2) + 1); i > 2; --i)
	{
		lua_rawgeti(_state, -2, i - 1);                                /* L: package, loaders, func, function */
		// we call lua_rawgeti, so the loader table now is at -3
		lua_rawseti(_state, -3, i);                                    /* L: package, loaders, func */
	}
	lua_rawseti(_state, -2, 2);                                        /* L: package, loaders */

	// set loaders into package
	lua_setfield(_state, -2, "loaders");                               /* L: package */

	lua_pop(_state, 1);
}

static int luaLoadBuffer(lua_State *L, const char *chunk, int chunkSize, const char *chunkName)
{
	int r = 0;
	//如果必要可以对chunk中的数据进行解码，这样可以支持对lua文件进行加密。
	r = luaL_loadbuffer(L, chunk, chunkSize, chunkName);
	if (r){
		switch (r){
		case LUA_ERRSYNTAX:
			SDL_Log("[LUA ERROR] load \"%s\", error: syntax error during pre-compilation.", chunkName);
			break;

		case LUA_ERRMEM:
			SDL_Log("[LUA ERROR] load \"%s\", error: memory allocation error.", chunkName);
			break;
			/* 未定义
		case LUA_ERRFILE:
			SDL_Log("[LUA ERROR] load \"%s\", error: cannot open/read file.", chunkName);
			break;
			*/
		default:
			SDL_Log("[LUA ERROR] load \"%s\", error: unknown.", chunkName);
		}
	}
	return r;
}

/*
 *在lua的搜索路径中找到指定的文件并加载,这是lua加载机制的一部分
 */
static int lua_loader(lua_State *L)
{
	const char *filename, *searchpath;
	int i,j,len;
	char fn[MAX_PATH];
	char f[MAX_PATH];
	char c;
	SDL_RWops *fp;
	unsigned char * data;

	filename = luaL_checkstring(L, 1);
	//去掉后缀.lua
	strcpy(fn, filename);
	i = strlen(fn);
	if (fn[i - 1] == 'a'&&fn[i - 2] == 'u'&&fn[i - 3] == 'l'&&
		fn[i - 4] == '.'){
		fn[i - 4] = 0;
		len = i - 4;
	}
	else
		len = i;
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "path");
	searchpath = lua_tostring(L, -1);
	j = i = 0;
	fp = NULL;
	for (;;){
		c = searchpath[i++];
		if (c == 0 || c == ';'){
			f[j] = 0;
			fp = SDL_RWFromFile(f, "rb");
			if (!fp){
				f[j] = 'c';
				f[j+1] = 0;
				fp = SDL_RWFromFile(f, "rb");
			}
			if (c==0||fp)break;
			j = 0;
		}else if (c == '?'){
			strcpy(f + j, fn);
			j += len;
		}else{
			f[j++] = c;
		}
	}
	if (fp){
		len = (int)SDL_RWsize(fp);
		data = (unsigned char *)malloc(len);
		if (!data){
			SDL_RWclose(fp);
			SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,"lua_loader out of memory,%s",filename);
			return 0;
		}
		SDL_RWread(fp, data,1, len);
		SDL_RWclose(fp);
		//装入lua脚本
		luaLoadBuffer(_state,data,len,f);
		free(data);
		return 1;
	}

	return 0;
}

void lua_addSearchPath(const char* path)
{
	lua_getglobal(_state, "package");                                  /* L: package */
	lua_getfield(_state, -1, "path");                /* get package.path, L: package path */
	const char* cur_path = lua_tostring(_state, -1);
	lua_pushfstring(_state, "%s;%s/?.lua", cur_path, path);            /* L: package path newpath */
	lua_setfield(_state, -3, "path");          /* package.path = newpath, L: package path */
	lua_pop(_state, 2);                                                /* L: - */
}

int lua_executeFunction(int numArgs)
{
	if (lua_executeFunctionResult(numArgs, 1)){
		// get return value
		int result = 0;
		if (lua_isnumber(_state, -1))
		{
			result = (int)lua_tointeger(_state, -1);
		}
		else if (lua_isboolean(_state, -1))
		{
			result = (int)lua_toboolean(_state, -1);
		}
		lua_pop(_state, 1);
		return result;
	}
	else{
		return 0;
	}
}

/*
 * -1 arg2
 * -2 arg1
 * -3 function
 * -4 __G__TRACKBACK__
 */
int lua_executeFunctionResult(int numArgs,int nRet)
{
	int functionIndex = -(numArgs + 1);
	if (!lua_isfunction(_state, functionIndex))
	{
		SDL_Log("value at stack [%d] is not function", functionIndex);
		lua_pop(_state, numArgs + 1); // remove function and arguments
		return 0;
	}

	int traceback = 0;
	lua_getglobal(_state, "__G__TRACKBACK__");                         /* L: ... func arg1 arg2 ... G */
	if (!lua_isfunction(_state, -1))
	{
		lua_pop(_state, 1);                                            /* L: ... func arg1 arg2 ... */
	}
	else
	{
		lua_insert(_state, functionIndex - 1);                         /* L: ... G func arg1 arg2 ... */
		traceback = functionIndex - 1;
	}

	int error = 0;
	++_callFromLua;
	error = lua_pcall(_state, numArgs, nRet, traceback);                  /* L: ... [G] ret */
	--_callFromLua;
	if (error)
	{
		if (traceback == 0)
		{
			SDL_Log("[LUA ERROR] %s", lua_tostring(_state, -1));        /* L: ... error */
			lua_pop(_state, 1); // remove error message from stack
		}
		else                                                            /* L: ... G error */
		{
			lua_pop(_state, 2); // remove __G__TRACKBACK__ and error message from stack
		}
		return 0;
	}
	if (traceback)
	{
		lua_remove(_state, -nRet-1);// remove __G__TRACKBACK__ from stack      /* L: ... */
	}

	return 1;
}

int lua_executeString(const char *code)
{
	luaL_loadstring(_state, code);
	return lua_executeFunction(0);
}

int lua_executeScriptFileResult(const char *filename,int nRet)
{
	char code[MAX_PATH];
	strcpy(code, "return require \"");
	strcat(code, filename);
	strcat(code, "\"");
	luaL_loadstring(_state, code);
	return lua_executeFunctionResult(0, nRet);
}

int lua_executeScriptFile(const char *filename)
{
	char code[MAX_PATH];
	strcpy(code, "require \"");
	strcat(code, filename);
	strcat(code, "\"");
	return lua_executeString(code);
}

int _eventRef[EVENT_COUNT];
static void registerEventFunction(lua_State *L, int nfunction, int eventid)
{
	if (_eventRef[eventid] != LUA_REFNIL){
		lua_getref(L, _eventRef[eventid]); //返回老的事件函数给调用者
		lua_unref(L, _eventRef[eventid]);
		_eventRef[eventid] = LUA_REFNIL;
	}
	else{
		lua_pushnil(L);
	}
	if (lua_isfunction(L, nfunction)){
		lua_pushvalue(L, nfunction);
		_eventRef[eventid] = lua_ref(L,1);
	}
}

static int lua_pushEventFunction(int n)
{
	if (n>=0&&n < EVENT_COUNT&&_eventRef[n] != LUA_REFNIL){
		lua_getref(_state,_eventRef[n]);
		return 1;
	}
	return 0;
}
/*
 * 向系统注册事件
 */
static int lua_eventFunction(lua_State *L)
{
	const char *ev = luaL_checkstring(L, 1);
	if (ev){
		if (strcmp(ev, "init") == 0){
			registerEventFunction(L, 2, EVENT_INIT);
		}
		else if (strcmp(ev, "release") == 0){
			registerEventFunction(L, 2, EVENT_RELEASE);
		}
		else if (strcmp(ev, "loop") == 0){
			registerEventFunction(L, 2, EVENT_LOOP);
		}
		else if (strcmp(ev, "input") == 0){
			registerEventFunction(L, 2, EVENT_INPUT);
		}
		else
			lua_pushnil(L);
	}
	else
		lua_pushnil(L);
	return 1;
}

static int lua_screenSize(lua_State *L)
{
	SDLState * state = getSDLState();
	lua_pushnumber(L, state->window_w);
	lua_pushnumber(L, state->window_h);
	return 2;
}

static int lua_isand(lua_State *L)
{
	int a = luaL_checkinteger(L, 1);
	int b = luaL_checkinteger(L, 2);
	lua_pushboolean(L, a&b);
	return 1;
}
/*
 * 初始Lua环境
 */
int initLua()
{
	int i;
	const luaL_reg global_functions[] = {
		{ "print", lua_print },
		{ "isand", lua_isand },
		{ "eventFunction", lua_eventFunction },
		{ "nanovgRender", lua_nanovgRender },
		{ "screenSize",lua_screenSize },
		{ NULL, NULL }
	};
	const luaL_reg luax_exts[] = {
		{ "vg", luaopen_nanovg },
		{ "ui", luaopen_ui },
		{ NULL, NULL }
	};
	luaL_Reg* lib = (luaL_Reg*)luax_exts;
	_state = lua_open();
	
	if (!_state){
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,"Can't open lua state");
		return 0;
	}
	luaL_openlibs(_state);

	/* 组成全局函数 */
	for (i = 0; i < EVENT_COUNT; i++)
		_eventRef[i] = LUA_REFNIL;
	luaL_register(_state, "_G", global_functions);
	
	/* 扩展库 */
	lua_getglobal(_state, "package");
	lua_getfield(_state, -1, "preload");
	for (; lib->func; lib++)
	{
		lua_pushcfunction(_state, lib->func);
		lua_setfield(_state, -2, lib->name);
	}
	lua_pop(_state, 2);

	/* 初始化脚本装载器 */
	initLuaLoader(lua_loader);
	return 1;
}

void releaseLua()
{
	if (_state){
		for (int i = 0; i < EVENT_COUNT; i++){
			if (_eventRef[i] != LUA_REFNIL){
				lua_unref(_state, _eventRef[i]);
			}
			_eventRef[i] = LUA_REFNIL;
		}
		lua_close(_state);
		_state = NULL;
	}
}

void lua_EventLoop(double dt)
{
	if (lua_pushEventFunction(EVENT_LOOP)){
		lua_pushnumber(_state, dt);
		lua_executeFunction(1);
	}
}

void lua_EventInit()
{
	/* 执行初始化代码 */
	lua_executeScriptFile("init.lua");

	if (lua_pushEventFunction(EVENT_INIT)){
		lua_executeFunction(0);
	}
}

void lua_EventRelease()
{
	if (lua_pushEventFunction(EVENT_RELEASE)){
		lua_executeFunction(0);
	}
	/* 执行退出脚本 */
	lua_executeScriptFile("release.lua");
}

void lua_EventInput()
{
	if (lua_pushEventFunction(EVENT_INPUT)){
		/*
		lua_pushinteger(_state,ie->type);
		if (ie->type == MOUSE_MOVE){
			lua_newtable(_state);
			lua_pushnumber(_state, ie->x);
			lua_setfield(_state, -2, "x");
			lua_pushnumber(_state, ie->y);
			lua_setfield(_state, -2, "y");
			lua_executeFunction(2);
		}else
			lua_executeFunction(1);
		*/
	}
}