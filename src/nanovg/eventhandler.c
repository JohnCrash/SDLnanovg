#include "SDL.h"
#include "sdlmain.h"
#include "eventhandler.h"
#include "luaext.h"
#include "ui.h"

#include "debug.h"

SDLEventBuffer _eventBuffer;

static void clearSDLEvent()
{
	_eventBuffer.nEvent = 0;
}

static int addSDLEvent(SDL_Event *ev)
{
	if (_eventBuffer.nEvent >= sizeof(_eventBuffer.events) / sizeof(SDL_Event)){
		return 0;
	}
	_eventBuffer.events[_eventBuffer.nEvent++] = *ev;
	return 1;
}

int getSDLEventCount()
{
	return _eventBuffer.nEvent;
}

SDL_Event * getSDLEvent(int n)
{
	return &(_eventBuffer.events[n]);
}

/*
 *  处理SDL事件如果需要退出返回1，否则返回0
 */
int handleSDLEvent(SDLState *state)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type){
		case SDL_WINDOWEVENT:
			if (event.window.event == SDL_WINDOWEVENT_CLOSE){
				lua_EventWindowClose();
				return 1;
			}
			else if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
				state->screen_w = event.window.data1;
				state->screen_h = event.window.data2;
				lua_EventChangeSize(state->screen_w, state->screen_h);
			}
			else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST){
				_background = 1;
				_isediting = 0;
				lua_EventWindow("background");
			}
			else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED){
				_background = 0;
				lua_EventWindow("foreground");
			}
			break;
		case SDL_TEXTEDITING:
			if (!event.edit.text[0]){
				_isediting = 0;
			}
			else{
				_isediting = 1;
			}
			lua_callKeyboardFunc3("editing", event.edit.text, event.edit.start);
			break;
		case SDL_TEXTINPUT:
			_isediting = 0;
			lua_callKeyboardFunc("insert", event.text.text);
			break;
		case SDL_KEYDOWN:
			if (!_isediting){
				const char * key = NULL;
				switch (event.key.keysym.scancode){
				case SDL_SCANCODE_HOME:
					key = "home";
					break;
				case SDL_SCANCODE_DELETE:
					key = "delete";
					break;
				case SDL_SCANCODE_BACKSPACE:
					key = "backspace";
					break;
				case SDL_SCANCODE_END:
					key = "end";
					break;
				case SDL_SCANCODE_LEFT:
					key = "left";
					break;
				case SDL_SCANCODE_RIGHT:
					key = "right";
					break;
				case SDL_SCANCODE_DOWN:
					key = "down";
					break;
				case SDL_SCANCODE_UP:
					key = "up";
					break;
				case SDL_SCANCODE_TAB:
					key = "tab";
					break;
				}
				if (key) lua_callKeyboardFunc(key, NULL);
			}
			lua_callKeyboardFunc2("keydown", event.key.keysym.scancode);
			break;
		case SDL_KEYUP:
			lua_callKeyboardFunc2("keyup", event.key.keysym.scancode);
			break;
		}
		if (!addSDLEvent(&event))
			return 0;
	}
	return 0;
}

static Uint32 _prevEventTick = 0;
static Uint32 _prevRenderTick = 0;
static int _background = 0;
static int _isediting = 0; //是否处于IME输入状态
/*
 * uss 是一个将所有可见对象都做OR操作得到的值
 *	控制事件循环的更新频率
 *		UPDATE_HIGH 60帧
 *		UPDATE_MID	30帧
 *		UPDATE_LOW	10帧
 *		UPDATE_OFF	事件驱动
 * 上一次循环耗时(ms)
 */
#define HIGH_VALUE 1000/60
#define MID_VALUE 1000/30
#define LOW_VALUE 1000/10

int eventLoop(SDLState *state)
{
	Uint32 curTick;
	Uint32 renderInterval;
	if (_prevEventTick == 0)_prevEventTick = SDL_GetTicks();
	if (_prevRenderTick == 0)_prevRenderTick = SDL_GetTicks();

	curTick = SDL_GetTicks();
	clearSDLEvent();

	/*
	 * 如果有SDL事件来就马上返回处理
	 */
	while (handleSDLEvent(state) == 0){
		ussClear();
		lua_EventLoop(dt);
		t1 = SDL_GetTicks();
		DEBUG("USS %d dt = %d value = %d HIGHT_VALUE = %d", uss, dt, dt + t1 - t0, HIGH_VALUE);
		if (getSDLEventCount() > 0){
			return 0;
		}
		else if (uss&VISIBLE && uss&UPDATE_HIGH){
			if (dt + t1 - t0 < HIGH_VALUE){
				SDL_Delay(1);
				DEBUG("HIGH_VALUE %d %d", dt, t1 - t0);
				goto _continue;
			}
		}
		else if (uss&VISIBLE && uss&UPDATE_MID){
			if (dt + t1 - t0 < MID_VALUE){
				SDL_Delay(5);
				DEBUG("UPDATE_MID %d %d", dt, t1 - t0);
				goto _continue;
			}
		}
		else if (uss&VISIBLE && uss&UPDATE_LOW){
			if (dt + t1 - t0 < LOW_VALUE){
				SDL_Delay(10);
				DEBUG("UPDATE_LOW %d %d", dt, t1 - t0);
				goto _continue;
			}
		}
		else if (getSDLEventCount() == 0){
			DEBUG("WaitEvent %d %d", dt, t1 - t0);
			SDL_WaitEvent(NULL);
			goto _continue;
		}
	}
	return 1; //退出程序
}