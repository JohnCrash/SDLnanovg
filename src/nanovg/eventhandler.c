#include "SDL.h"
#include "sdlmain.h"
#include "eventhandler.h"
#include "luaext.h"
#include "ui.h"
#define DBG
#include "debug.h"

SDLEventBuffer _eventBuffer;
static Uint32 _prevEventTick = 0;
static Uint32 _prevRenderTick = 0;
static int _background = 0;
static int _isediting = 0; //是否处于IME输入状态

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

/*
 * uss 是一个将所有可见对象都做OR操作得到的值
 */
int eventLoop(SDLState *state)
{
	Uint32 curTick;
	double dt;
	double fps = 1.0 / uiGetFPS();
	unsigned int uss = uiUSSGet();
	uiUSSClear();

	clearSDLEvent();

	/*
	 * 如果有SDL事件来就马上返回处理
	 * 对更新进行优化仅仅在有下面情况下才会做更新渲染
	 * 1.有事件。(虽然事件不一定会必然导致更新渲染，但是很难进行区别出来)
	 * 2.在渲染周期中uss被设置为KEEP_UPDATE。
	 *		使用self:enableFlags(ui.KEEP_UPDATE)来请求标志位。
	 *		使用self:disableFlags(ui.KEEP_UPDATE)关闭标志位。
	 * 3.在loop事件或者schedule中uss被设置为KEEP_UPDATE。
	 *		在loop事件或者schedule中使用requestUpdate()
	 *		eventFunction("loop",function(dt)
	 *			requestUpdate()
	 *		end)
	 *		或者
	 *		schedule(0,function(dt)
	 *			requestUpdate()
	 *		end)
	 */
	while (handleSDLEvent(state) == 0){
		//保持帧率
		do{
			curTick = SDL_GetTicks();
			dt = (curTick - _prevEventTick)/1000.0;
			SDL_Delay(1);
		} while (dt<fps);

		//处理loop事件和schedule回调
		lua_EventLoop(dt);
		_prevEventTick = curTick;

		if (getSDLEventCount() > 0 || uss&KEEP_UPDATE || uiUSSGet()&KEEP_UPDATE ){
			setLoopInterval((curTick-_prevRenderTick)/1000.0);
			DEBUG("uiLoop %d", curTick - _prevRenderTick);
			_prevRenderTick = curTick;
			return 0;
		}
	}
	return 1; //退出程序
}

void initEventLoop(SDLState *state)
{
	_background = 0;
	_isediting = 0;
	_prevEventTick = SDL_GetTicks();
	_prevRenderTick = SDL_GetTicks();
}