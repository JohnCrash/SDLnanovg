﻿#include <string.h>
#include "SDL.h"
#include "luananovg.h"
#include "luaui.h"
#include "sdlmain.h"
#include "eventhandler.h"
#include "luaext.h"
#include "ui.h"
#include "utf8.h"
#include "platform.h"
#include "sdlport.h"

/**
* \addtogroup LuaEXT lua global
* \brief lua全局函数
* @{
*/

static lua_State * _state = NULL;
static int _callFromLua = 0;

static unsigned int _scheduleID = 0;
static struct schedule * _schedule = NULL;
static int _isTextInputOpen = 0;
static char * _execPath = NULL;

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
	int len,cur,i;
	len = MAX_PATH;
	cur = 0;
	t = buf;
	for (i = 1; i <= nargs; i++)
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
	int i;
	if (!func) return;

	/* 
	 * 设置LUA_PATH环境变量,使用相对的路径。
	 */
	lua_getglobal(_state, "package");
	lua_pushstring(_state, getLUAPath());
	lua_setfield(_state, -2, "path");
	lua_pop(_state, 1);

	lua_getglobal(_state, "package");
	lua_pushstring(_state, getLUAClibPath());
	lua_setfield(_state, -2, "cpath");
	lua_pop(_state, 1);

	// stack content after the invoking of the function
	// get loader table
	lua_getglobal(_state, "package");                                  /* L: package */
	lua_getfield(_state, -1, "loaders");                               /* L: package, loaders */

	// insert loader into index 2
	lua_pushcfunction(_state, func);                                   /* L: package, loaders, func */
	for (i = (int)(lua_objlen(_state, -2) + 1); i > 2; --i)
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
 * 在lua的搜索路径中找到指定的文件并加载,这是lua加载机制的一部分
 */
static int lua_loader(lua_State *L)
{
	const char *filename;
	int i,len;
	char fn[MAX_PATH];
	char f[MAX_PATH];
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
	//将文件名中的.转换为/
	for (i = 0; i < len;i++){
		if (fn[i] == '.')fn[i] = '/';
	}
	fp = SDL_searchFile(getLUAPath(), fn, "rb");
	if (fp){
		len = (int)SDL_RWsize(fp);
		data = (unsigned char *)malloc(len);
		if (!data){
			SDL_RWclose(fp);
			SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "lua_loader out of memory,%s", fn);
			return 0;
		}
		SDL_RWread(fp, data,1, len);
		SDL_RWclose(fp);
		//装入lua脚本
		SDL_Log("lua_loader:%s", fn);
		sprintf(f,"@%s.lua",fn);
		luaLoadBuffer(_state, data, len, f);
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

int lua_pushEventFunction(int n)
{
	if (n>=0&&n < EVENT_COUNT&&_eventRef[n] != LUA_REFNIL){
		lua_getref(_state,_eventRef[n]);
		return 1;
	}
	return 0;
}
/**
 * \brief 注册触发事件或者取消注册
 * \param event 事件名称,可以是下面的值
 *	- init	在主循环开始前调用一次
 *	- release	在主循环退出时调用一次
 *	- loop	主循环，在程序运行期间不停调用
 *	- input	全局输入事件，在发送给具体的控件前发送
 *	- window 程序进入前台或后台，尺寸发送改变等事件
 * \param func 事件函数
 * \return 成功返回true,失败返回nil
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
		else if (strcmp(ev, "window") == 0){
			registerEventFunction(L, 2, EVENT_WINDOW);
		}
		else
			lua_pushnil(L);
	}
	else
		lua_pushnil(L);
	return 1;
}

/**
 * \brief 返回屏幕尺寸。
 * \return cx,cy 屏幕的尺寸
 */
static int lua_screenSize(lua_State *L)
{
	SDL_DisplayMode mode;
	if(SDL_GetDisplayMode(0,0,&mode)==0){
		lua_pushnumber(L,mode.w);
		lua_pushnumber(L, mode.h);
		return 2;
	}else return 0;
}

static int lua_isand(lua_State *L)
{
	int a = luaL_checkinteger(L, 1);
	int b = luaL_checkinteger(L, 2);
	lua_pushboolean(L, a&b);
	return 1;
}

struct schedule* add_c_schedule(c_schedule func, double dt)
{
	struct schedule * psch;
	psch = (struct schedule *)malloc(sizeof(struct schedule));
	if (psch){
		psch->t = 0;
		psch->cfunc = func;
		psch->interval = dt;
		psch->ref = LUA_REFNIL;
		psch->id = _scheduleID++;
		if (_schedule){
			psch->prev = NULL;
			psch->next = _schedule;
			_schedule->prev = psch;
			_schedule = psch;
		}
		else{
			psch->next = NULL;
			psch->prev = NULL;
			_schedule = psch;
		}
	}
	return psch;
}

void remove_c_schedule(struct schedule* next)
{
	struct schedule * temp;
	
	if (next){
		temp = next->prev;
		if (temp){
			temp->next = next->next;
			next->prev = temp;
		}
		else{
			_schedule = next->next;
			if (_schedule)
				_schedule->prev = NULL;
		}
		free(next);
	}
	return;
}

/**
 * \brief 设置一个定时器
 * \param dt 每隔dt时间就调用函数func
 * \param func 回调函数，当回调函数返回nil or false下一个周期将不调用回调。
 * \return 成功返回一个id,失败返回-1
 */
static int lua_schedule(lua_State *L)
{
	double dt = luaL_checknumber(L, 1);
	if (lua_isfunction(L, 2)){
		struct schedule * psch;

		psch = (struct schedule *)malloc(sizeof(struct schedule));
		if (psch){
			psch->t = 0;
			psch->cfunc = NULL;
			psch->interval = dt;
			lua_pushvalue(L, 2);
			psch->ref = lua_ref(L, 1);
			/* fixme:如果_scheduleID循环将发生重复 */
			psch->id = _scheduleID++;
			if (_schedule){
				psch->prev = NULL;
				psch->next = _schedule;
				_schedule->prev = psch;
				_schedule = psch;
			}
			else{
				psch->next = NULL;
				psch->prev = NULL;
				_schedule = psch;
			}
			lua_pushnumber(L, (lua_Number)psch->id);
			return 1;
		}
	}

	lua_pushnumber(L, -1);
	return 1;
}

/**
 * \brief 删除一个定时器
 * \param id 传入一个定时器id，schedule的返回值
 * \return 成功删除返回true,失败返回false
 */
static int lua_removeSchedule(lua_State *L)
{
	unsigned int id = (unsigned int)luaL_checknumber(L, 1);
	struct schedule * next = _schedule;
	struct schedule * temp;

	while (next){
		if (next->id == id){
			/* 清除此定时器 */
			temp = next->prev;
			if (temp){
				temp->next = next->next;
				next->prev = temp;
			}
			else{
				_schedule = next->next;
				if (_schedule)
					_schedule->prev = NULL;
			}
			lua_unref(_state, next->ref);
			free(next);
			lua_pushboolean(L, 1);
			return 1;
		}
		next = next->next;
	}
	lua_pushboolean(L, 0);
	return 1;
}

static int _inputRef = LUA_REFNIL;
static struct schedule * _cSchedule = NULL;

void lua_callKeyboardFunc(const char *event,char *str)
{
	if (_inputRef!=LUA_REFNIL){
		lua_getref(_state, _inputRef);
		lua_pushstring(_state, event);
		if (str)
			lua_pushstring(_state, str);
		else
			lua_pushnil(_state);
		lua_executeFunction(2);
	}
}

void lua_callKeyboardFunc2(const char *event, int scancode)
{
	if (_inputRef != LUA_REFNIL){
		lua_getref(_state, _inputRef);
		lua_pushstring(_state, event);
		lua_pushinteger(_state, scancode);
		lua_executeFunction(2);
	}
}

void lua_callKeyboardFunc3(const char *event,char *str, int start)
{
	if (_inputRef != LUA_REFNIL){
		lua_getref(_state, _inputRef);
		lua_pushstring(_state, event);
		lua_pushstring(_state, str);
		lua_pushinteger(_state, start);
		lua_executeFunction(3);
	}
}

static int closeTextInput()
{
	lua_callKeyboardFunc("detach", NULL);
	if (_inputRef != LUA_REFNIL){
		lua_unref(_state, _inputRef);
		_inputRef = LUA_REFNIL;
	}
	_isTextInputOpen = 0;
	SDL_StopTextInput();
	_cSchedule = NULL;
	return 0;
}

/**
 * \brief 设置输入栏矩形，用于判断在打开软键盘时向上推高多少
 * \param x,y,w,h 输入栏矩形
 */
static int lua_softKeyboardInputRect(lua_State *L)
{
	SDL_Rect rect;
	rect.x = luaL_checkint(L, 1);
	rect.y = luaL_checkint(L, 2);
	rect.w = luaL_checkint(L, 3);
	rect.h = luaL_checkint(L, 4);
	SDL_SetTextInputRect(&rect);
	return 0;
}

/**
 * \brief 打开或者关闭软键盘
 * \param b true打开软键盘，false关闭软件盘
 * \param func 软键盘处理函数
 * \return 成功返回true,失败返回false
 * \note 软键盘处理函数原型func(event,param),event可以是下面的值
 *	- 'attach'	键盘附属，准备开始输入。
 *	- 'detach'	键盘分离，结束输入。
 *	- 'insert'	插入utf8字符串param。
 *	- 'delete'	后向删除。
 *	- 'backspace'	前向删除。
 *	- 'home'	按下home键，跳到行开始。
 *	- 'end'		按下end键，跳到行尾。
 *	- 'left'	左移光标。
 *	- 'right'	右移光标。
 *	- 'up'		上移光标。
 *	- 'down'	下移光标。
 *	- 'tab'
 */
static int lua_enableSoftkeyboard(lua_State *L)
{
	if (lua_isboolean(L, 1)){
		if(lua_toboolean(L, 1)){
			/* 通知上一个结束输入 */
			lua_callKeyboardFunc("detach", NULL);
			if (_inputRef != LUA_REFNIL){
				lua_unref(L, _inputRef);
				_inputRef = LUA_REFNIL;
			}
			if (lua_isfunction(L, 2)){
				lua_pushvalue(L, 2);
				_inputRef = lua_ref(L, 1);
			}
			/* 打开软键盘 */
			if (_cSchedule){
				remove_c_schedule(_cSchedule);
				_cSchedule = NULL;
			}
			if (!_isTextInputOpen){
				_isTextInputOpen = 1;
				SDL_StartTextInput();
			}
			/* 通知当前输入准备输入 */
			lua_callKeyboardFunc("attach", NULL);
		}else{
			/* 关闭软键盘,使用一个定时器进行关闭 */
			if (_cSchedule){
				_cSchedule->t = 0;
			}else{
				_cSchedule = add_c_schedule(closeTextInput, 0.025);
			}
		}
	}
	return 0;
}

/**
 * \brief 返回建的状态
 * \param k 要查询的扫描键表,如果要查询{SCANCODE_A,SCANCODE_LEFT}
 * \return 返回一个表如果k中的扫描键被按下表的对应值1，否则为0
 */
int lua_keyboardState(lua_State *L)
{
	int len,sk,i = 1;
	const Uint8 * kbs = SDL_GetKeyboardState(&len);
	lua_newtable(L);
	lua_pushnil(L);
	while (lua_next(L, 1) != 0){
		if (lua_isnumber(L, -1)){
			sk = (int)lua_tonumber(L, -1);
			if (sk >= 0 && sk < len && kbs[sk] ){
				lua_pushinteger(L, 1);
			}
			else{
				lua_pushinteger(L, 0);
			}
			lua_rawseti(L,-4,i++);
		}
		lua_pop(L, 1);
	}
	return 1;
}

/**
 * \brief 将字符串拷贝到剪接板
 * \param str 要放入剪接板的字符串
 * \return 无返回值
 */
int lua_clipbaordCopy(lua_State *L)
{
	const char * str = luaL_checkstring(L, 1);
	SDL_SetClipboardText(str);
	return 0;
}

/**
 * \brief 从剪接板读取字符串
 * \return 返回剪切板字符串，没有数据返回一个空串
 */
int lua_clipbaordPast(lua_State *L)
{
	char * str = SDL_GetClipboardText();
	if (str)
		lua_pushstring(L,str);
	else
		lua_pushstring(L, "");
	return 1;
}

/**
 * \brief 返回该版本是不是一个调试版本
 * \return 如果是调试版本返回true,不是返回nil
 */
int lua_isDebug(lua_State *L)
{
#ifdef _DEBUG
	lua_pushboolean(L,1);
#else
	lua_pushnil(L);
#endif
	return 1;
}

/**
 * \brief 返回平台名称
 * \return 返回平台名称，可以是下面的值"windows","android","ios"
 */
int lua_getPlatform(lua_State *L)
{
#if defined(_WIN32)
	const char * platform = "windows";
#elif defined(__ANDROID__)
	const char * platform = "android";
#elif defined(__APPLE__)
	const char * platform = "ios";
#endif
	lua_pushstring(L, platform);
	return 1;
}

/**
 * \brief 返回utf8字符串的长度
 * \param s utf8字符串
 * \return 返回字符串长度
 */
int lua_utf8Length(lua_State *L)
{
	const char * str = luaL_checkstring(L, 1);
	lua_pushinteger(L, cc_utf8_strlen(str,-1));
	return 1;
}

/**
 * \brief 返回一个表
 * \param s utf8字符串
 * \return 返回一个索引表
 * \node 索引表的第一个值表示第一个字符的起始位置，第二个值表示第二个字符的起始位置。
 *	（第一个字符位置为1）
 */
int lua_utf8Index(lua_State *L)
{
	const char * p = luaL_checkstring(L, 1);
	const char * pp;
	int i = 1;
	int index = 1;
	lua_newtable(L);
	while (*p)
	{
		pp = p;
		p = cc_utf8_next(pp);
		lua_pushinteger(L, index);
		lua_rawseti(L, -2, i++);
		index += (p - pp);
	}
	return 1;
}

/**
* \brief 返回当前的毫秒精度的一个计时
* \return 返回一个秒为单位的计时数
*/
int lua_tick(lua_State *L)
{
	lua_pushnumber(L, SDL_GetTicks() / 1000.0);
	return 1;
}

/**
* \brief 在window系统中用来调整主窗口的尺寸
* \param w,h 屏幕宽度与高度
*/
int lua_setDeviceSize(lua_State *L)
{
#if defined(_WIN32) || defined(__MAC__)
	int w = luaL_checkint(L, 1);
	int h = luaL_checkint(L, 2);
	SDLState * state = getSDLState();
	if (state){
		state->window_w = w;
		state->window_h = h;
		SDL_SetWindowSize(state->window, w, h);
	}
#endif	
	return 0;
}

int lua_getDeviceSize(lua_State *L)
{
	SDLState * state = getSDLState();
	lua_pushinteger(L, state->window_w);
	lua_pushinteger(L, state->window_h);
	return 2;
}
/**
 * \brief 在window系统中设置主窗口的尺寸
 */
int lua_setWindowTitle(lua_State *L)
{
	const char *title = luaL_checkstring(L, 1);
	SDLState * state = getSDLState();
	SDL_SetWindowTitle(state->window, title);
	return 0;
}

/**
 * \brief 用于调试，当调用时可以让调试器停在lua调用点的c调用栈处。
 *	仅仅在windows平台可用。
 */
int lua_breakpoint(lua_State *L)
{
#ifdef _WIN32
	_asm{
		int 3
	}
#endif
	return 0;
}

/**
 * \brief 
 * \param
 */
int lua_fonsHasCodepoint(lua_State *L)
{
	const char *path = luaL_checkstring(L, 1);
	size_t l;
	int i,n = 0;
	const char * ps[128];
	int result[128];
	lua_pushnil(L);
	while (lua_next(L, 2) != 0){
		ps[n++] = luaL_checklstring(L, -1, &l);
		lua_pop(L, 1);
	}
	if (n && fonsHasCodepoint(path, ps, result, n)){
		lua_newtable(L);
		for (i = 0; i < n; i++){
			lua_pushinteger(L, result[i]);
			lua_rawseti(L, -2, i + 1);
		}
		return 1;
	}
	return 0;
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
		{ "setDeviceSize",lua_setDeviceSize},
		{ "getDeviceSize", lua_getDeviceSize },
		{ "setWindowTitle", lua_setWindowTitle },
		{ "schedule", lua_schedule },
		{ "removeSchedule", lua_removeSchedule },
		{ "softKeyboard",lua_enableSoftkeyboard },
		{ "softKeyboardInputRect", lua_softKeyboardInputRect },
		{ "clipbaordCopy", lua_clipbaordCopy },
		{ "clipbaordPast", lua_clipbaordPast },
		{ "keyboardState", lua_keyboardState },
		{ "getPlatform" ,lua_getPlatform },
		{ "utf8Length", lua_utf8Length },
		{ "utf8Index", lua_utf8Index },
		{ "isDebug", lua_isDebug },
		{ "tick",lua_tick },
		{ "fonsHasCodepoint", lua_fonsHasCodepoint },
		{ "breakpoint",lua_breakpoint },
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

	for (i = 0; i < EVENT_COUNT; i++)
		_eventRef[i] = LUA_REFNIL;
	/* 注册全局函数 */
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
	int i;
	if (_state){
		for (i = 0; i < EVENT_COUNT; i++){
			if (_eventRef[i] != LUA_REFNIL){
				lua_unref(_state, _eventRef[i]);
			}
			_eventRef[i] = LUA_REFNIL;
		}
		lua_close(_state);
		_state = NULL;
	}
}

static double _loopInterval = 0;
double getLoopInterval()
{
	return _loopInterval;
}

void setLoopInterval(double dt)
{
	_loopInterval = dt;
}

void lua_EventLoop(double dt)
{
	struct schedule * next = _schedule;
	struct schedule * temp;

	if (lua_pushEventFunction(EVENT_LOOP)){
		lua_pushnumber(_state, dt);
		lua_executeFunction(1);
	}

	/* 定时器循环 */
	while (next){
		next->t += dt;
		if (next->t >= next->interval){
			if (next->ref != LUA_REFNIL){
				lua_getref(_state, next->ref);
				lua_pushnumber(_state, next->t);
				next->t = 0;
				if (!lua_executeFunction(1)){
					/* 清除此定时器 */
					temp = next->prev;
					if (temp){
						temp->next = next->next;
						next->prev = temp;
						temp = next->next;
					}
					else{
						_schedule = next->next;
						if (_schedule)
							_schedule->prev = NULL;
						temp = _schedule;
					}
					lua_unref(_state, next->ref);
					free(next);
					next = temp;
					continue;
				}
			}
			else if (next->cfunc){
				if (!next->cfunc()){
					/* 清除此定时器 */
					temp = next->prev;
					if (temp){
						temp->next = next->next;
						next->prev = temp;
						temp = next->next;
					}
					else{
						_schedule = next->next;
						if (_schedule)
							_schedule->prev = NULL;
						temp = _schedule;
					}
					free(next);
					next = temp;
					continue;
				}
			}
		}
		next = next->next;
	}
}

void lua_EventInit()
{
	/* 执行debugger.lua*/
	//if (hasProgramParameter("-luadbg")){
		SDL_Log("execute script gdbp.lua");
		lua_executeScriptFile("gdbp");
	//}
	/* 执行初始化代码 */
	SDL_Log("execute script nanovg.lua");
	lua_executeScriptFile("nanovg");
	lua_executeScriptFile("init");

	if (lua_pushEventFunction(EVENT_INIT)){
		lua_executeFunction(0);
	}
}

void lua_EventRelease()
{
	struct schedule * next = _schedule;
	struct schedule * temp;

	if (_inputRef != LUA_REFNIL){
		lua_unref(_state, _inputRef);
		_inputRef = LUA_REFNIL;
	}
	/* 清理全部定时器 */
	while (next){
		temp = next;
		next = next->next;
		if (temp->ref!=LUA_REFNIL)
			lua_unref(_state, temp->ref);
		free(temp);
	}
	_schedule = NULL;
	_cSchedule = NULL;

	if (lua_pushEventFunction(EVENT_RELEASE)){
		lua_executeFunction(0);
	}
	/* 执行退出脚本 */
	lua_executeScriptFile("release");
}

/**
 * \brief 将屏幕变化后的尺寸通知lua事件函数
 * \param w,h 屏幕的宽高
 */
void lua_EventChangeSize(int w,int h)
{
	if (lua_pushEventFunction(EVENT_WINDOW)){
		lua_pushstring(_state, "sizeChanged");
		lua_pushinteger(_state, w);
		lua_pushinteger(_state, h);
		lua_executeFunction(3);
	}
}

void lua_EventWindowClose()
{
	if (lua_pushEventFunction(EVENT_WINDOW)){
		lua_pushstring(_state, "close");
		lua_executeFunction(1);
	}
}

/*
 * 向lua发送一个无参数的window事件
 */
void lua_EventWindow(const char * eventName)
{
	if (lua_pushEventFunction(EVENT_WINDOW)){
		lua_pushstring(_state, eventName);
		lua_executeFunction(1);
	}
}
