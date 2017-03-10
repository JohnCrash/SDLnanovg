#include "platform.h"
#include <math.h>
#include <memory.h>
#include "SDL.h"
#include "lua.h"
#include "lauxlib.h"
#include "gles.h"
#include "sdlmain.h"
#include "eventhandler.h"
#include "luaext.h"
#include "ui.h" 
#include "nanovg.h"
#include "nanovg_sdl.h"
#include "debug.h"

#define ES_TDWS_MAX 32
#define ES_EVENT_MAX 16

typedef struct {
	int haveEventType;		/* 在事件列表中存在指定的事件类型 */
	int isTouchDown;		/* 在按下状态下 */
	uiEvent lastTouchDown;	/* 最近一次按下事件 */
	uiWidget *lastTouchDownWidgets[ES_TDWS_MAX];	/* 接收了按下事件的对象 */
	int nEvent;
	uiEvent events[ES_EVENT_MAX];
} uiEventState;

uiEventState _eventState;
static uiWidget * _root = NULL;
static ThemesList * _themes = NULL;
static unsigned int _uss = 0;
extern NVGcontext* _vg;

void ussClear()
{
	_uss = 0;
}

unsigned int uss()
{
	return _uss;
}

uiWidget * uiRootWidget()
{
	return _root;
}

/*
 * 向_eventState lastTouchDownWidgets 中加入一个uiWidget *
 */
int pushLastTouchDownWidgets(uiWidget * pwidget)
{
	int i;
	for (i = 0; i < ES_TDWS_MAX; i++){
		if (!_eventState.lastTouchDownWidgets[i]){
			_eventState.lastTouchDownWidgets[i] = pwidget;
			return 1;
		}
	}
	return 0;
}

/*
 * 查找看看pwidget是不是在lastTouchDownWidgets中
 */
int inLastTouchDownWidgets(uiWidget * pwidget)
{
	int i;
	uiWidget * pw;
	for (i = 0; i < ES_TDWS_MAX; i++){
		pw = _eventState.lastTouchDownWidgets[i];
		if (!pw)
			return 0;
		else if (pw == pwidget)
			return 1;
	}
	return 0;
}

/*
 * 通过sx,sy,angle计算出矩阵的旋转部分和缩放部分，这里舍弃了切变换
 */
static void calcXForm(uiWidget *self)
{
	float tx[6];
	nvgTransformIdentity(self->xform); 
	nvgTransformTranslate(tx, -self->ox, -self->oy);
	nvgTransformMultiply(self->xform, tx);
	nvgTransformScale(tx, self->sx,self->sy);
	nvgTransformMultiply(self->xform, tx);
	nvgTransformRotate(tx, self->angle);
	nvgTransformMultiply(self->xform, tx);
	nvgTransformTranslate(tx, self->ox, self->oy);
	nvgTransformMultiply(self->xform, tx);

	nvgTransformTranslate(tx, self->x, self->y);
	nvgTransformMultiply(self->xform, tx);
}

int initUI()
{
	SDL_DisplayMode mode;
	SDLState * state = getSDLState();
	_root = (uiWidget *)malloc(sizeof(uiWidget));
	if (!_root)return 0;
	
	if (SDL_GetDisplayMode(0, 0, &mode) == 0){
		state->screen_w = mode.w;
		state->screen_h = mode.h;
	}
#if defined(__ANDROID__) || defined(__IOS__)
	state->window_w = (float)state->screen_w;
	state->window_h = (float)state->screen_h;
#endif
	memset(_root, 0, sizeof(uiWidget));
	_root->width = (float)state->window_w;
	_root->height = (float)state->window_h;
	_root->sx = 1.0f;
	_root->sy = 1.0f;
	calcXForm(_root);
	_root->isVisible = VISIBLE;
	_root->handleEvent = EVENT_NONE;
	_root->classRef = LUA_REFNIL;
	_root->selfRef = LUA_REFNIL;
	_root->hookRef = LUA_REFNIL;

	/* 初始化事件状态 */
	_eventState.nEvent = 0;
	_eventState.isTouchDown = 0;

	memset(_eventState.lastTouchDownWidgets, 0, sizeof(_eventState.lastTouchDownWidgets));

	return 1;
}

/* luaui.c 中的方法 */
extern void lua_pushWidget(lua_State *L, uiWidget * widget);

/*
 * 调用对象的类方法如:onInit,onRelease
 */
static void callWidgetEvent(uiWidget * widget, int themeRef,const char *strEvent)
{
	int nArg = 1;
	/* 先找对象的重载方法 */
	if (widget->selfRef != LUA_REFNIL){
		lua_State * L = lua_GlobalState();
		if (L){
			lua_getref(L, widget->selfRef);
			lua_getfield(L, -1, strEvent);
			if (lua_isfunction(L, -1)){
				//lua_getref(L, widget->selfRef);
				lua_pushWidget(L, widget);
				if (themeRef != LUA_REFNIL){
					nArg = 2;
					lua_getref(L, themeRef);
				}
				lua_executeFunction(nArg);
				lua_pop(L, 1); //classRef;
				return;
			}
			else{
				lua_pop(L, 2);
			}
		}
	}
	/* 然后访问类方法方法 */
	if (widget->classRef != LUA_REFNIL){
		lua_State * L = lua_GlobalState();
		if (L){
			lua_getref(L, widget->classRef);
			lua_getfield(L, -1, strEvent);
			if (lua_isfunction(L, -1)){
				//lua_getref(L, widget->selfRef);
				lua_pushWidget(L, widget);
				if (themeRef != LUA_REFNIL ){
					nArg = 2;
					lua_getref(L, themeRef);
				}
				lua_executeFunction(nArg);
				lua_pop(L, 1); //classRef;
			}
			else{
				lua_pop(L, 2);
			}
		}
	}
}

static uiWidget * _delayList = NULL;
static int _delayEnable = 0;
/*
 * 打开关闭延时删除功能，当开启延时删除时。
 * 要删除的对象都被链接起来稍后集中删除。
 */
static void uiDelayDelete(int b)
{
	_delayEnable = b;
}

static void uiDeleteWidgetSelf(uiWidget *self)
{
	if (self){
		uiRemoveFromParent(self);
		if (_delayEnable){
			/* 放入延时删除表 */
			self->remove = _delayList;
			_delayList = self;
		}
		else{
			/* 立刻删除对象 */
			lua_State * L = lua_GlobalState();
			callWidgetEvent(self, LUA_REFNIL,"onRelease");
			if (L && self->selfRef != LUA_REFNIL){
				lua_unref(L, self->selfRef);
			}
			if (L && self->luaobj){
				self->luaobj->widget = NULL;
				lua_unref(L, self->luaobj->ref);
			}
			if (L && self->hookRef != LUA_REFNIL){
				lua_unref(L, self->hookRef);
			}
			free(self);
		}
	}
}

void releaseUI()
{
	lua_State * L = lua_GlobalState();
	/* 删除根节点 */
	uiDeleteWidget(_root);
	uiDeleteWidgetSelf(_root);
	/* 删除样式表 */
	while(_themes){
		ThemesList * ptemp = _themes;
		_themes = ptemp->next;
		if (L && ptemp->themeRef){
			lua_getref(L, ptemp->themeRef);
			lua_getfield(L, -1, "onRelease");
			if (lua_isfunction(L, -1)){
				lua_pushvalue(L, -2);
				lua_executeFunction(1);
				lua_pop(L, 1);
			}
			else{
				lua_pop(L, 2);
			}
			lua_unref(L, ptemp->themeRef);
		}
		free(ptemp->filename);
		free(ptemp->name);
		free(ptemp);
	}
}

uiWidget * uiCreateWidget(const char *themes_name, const char *widget_name)
{
	ThemesList * themes = _themes;
	lua_State * L = lua_GlobalState();
	while (themes){
		/* 先在样式表中查找对应的样式 */
		if (strcmp(themes_name, themes->name) == 0 && themes->themeRef!=LUA_REFNIL){
			lua_getref(L, themes->themeRef);
			lua_getfield(L, -1, widget_name);
			/*
			 * -1 widget_table
			 * -2 themes_table
			 */
			if (lua_istable(L, -1)){
				/*
				 * 成功找到样式表中的控件原型表
				 */
				uiWidget * self = (uiWidget*)malloc(sizeof(uiWidget));
				if (!self)return NULL;
				memset(self, 0, sizeof(uiWidget));
				self->hookRef = LUA_REFNIL;
				self->classRef = lua_ref(L, 1);
				lua_newtable(L);
				self->selfRef = lua_ref(L,1);
				lua_pop(L, 1);
				self->sx = 1.0f;
				self->sy = 1.0f;
				calcXForm(self);
				self->isVisible = VISIBLE;
				self->handleEvent = EVENT_NONE;
				callWidgetEvent(self,themes->themeRef,"onInit");
				return self;
			}
			else{
				lua_pop(L, 2);
			}
		}
		themes = themes->next;
	}
	return NULL;
}

/*
 * 将子节点都删除，如果自己不是root也将自己删除
 */
void uiDeleteWidget(uiWidget *self)
{
	uiWidget *child;
	uiWidget *temp;
	if (!self)return;
	child = self->child;
	
	while (child){
		temp = child;
		child = child->next;
		uiDeleteWidget(temp);
	}
	if (self != _root){
		uiDeleteWidgetSelf(self);
	}
}

static uiWidget * getTail(uiWidget *self)
{
	uiWidget * tail;
	if (!self->next)return self;

	tail = self->next;
	while (tail->next){
		tail = tail->next;
	}
	return tail;
}

static uiWidget * getHead(uiWidget *self)
{
	uiWidget * head;
	if (!self->prev)return self;

	head = self->prev;
	while (head->prev){
		head = head->prev;
	}
	return head;
}

/* 将对象放入显示最上面 */
void uiBringTop(uiWidget * self)
{
	if (self&&self->parent){
		if (self->parent->child != self){
			uiWidget * parent = self->parent;
			uiAddChild(parent, self);
		}
	}
}

/* 将对象放入显示最下面 */
void uiBringBottom(uiWidget * self)
{
	if (self&&self->parent){
		uiWidget * tail = getTail(self);
		if (tail != self){
			uiWidget * parent = self->parent;
			uiAddChildToTail(parent, self);
		}
	}
}

/*
 * 将对象插入到队列的开始
 */
void uiAddChild(uiWidget *parent, uiWidget *child)
{
	if (parent && child){
		uiRemoveFromParent(child);
		child->parent = parent;
		if (parent->child){
			uiWidget * tail = getTail(parent->child);
			tail->next = child;
			child->prev = tail;
			child->next = NULL;
		}
		else{
			parent->child = child;
		}
	}
}

/*
* 将对象插入到队列的结尾
*/
void uiAddChildToTail(uiWidget *parent, uiWidget *child)
{
	if (parent && child){
		uiRemoveFromParent(child);
		child->parent = parent;
		if (parent->child){
			parent->child->prev = child;
			child->next = parent->child;
			parent->child = child;
		}
		else{
			parent->child = child;
		}
	}
}

void uiEnableFlags(uiWidget * widget,int flags)
{
	widget->isVisible |= flags;
	_uss |= widget->isVisible;
}

int uiDisableFlags(uiWidget * widget,int flags)
{
	int old = widget->isVisible;
	widget->isVisible &= ~flags;
	_uss |= widget->isVisible;
	return old;
}

void uiRemoveFromParent(uiWidget *self)
{
	if (self){
		if (self->parent){
			if (self->parent->child == self)
				self->parent->child = self->next;
			self->parent = NULL;
		}
		if (self->next){
			self->next->prev = self->prev;
		}
		if (self->prev){
			self->prev->next = self->next;
		}
		self->next = NULL;
		self->prev = NULL;
	}
}

int loadThemes(const char *name, const char *filename)
{
	lua_State *L;
	ThemesList * theme = _themes;
	while (theme){
		if (strcmp(theme->name, name) == 0)
			return 1;
		theme = theme->next;
	}
	L = lua_GlobalState();
	if (lua_executeScriptFileResult(filename, 1)){
		if (lua_istable(L, -1)){
			ThemesList * ptl = (ThemesList *)malloc(sizeof(ThemesList));
			if (!ptl){
				lua_pop(L, 1);
				return 0;
			}
			//对样式表设置名称
			lua_pushstring(L, "name");
			lua_pushstring(L, name);
			lua_settable(L, -3);

			ptl->filename = strdup(filename);
			ptl->name = strdup(name);
			lua_pushvalue(L, -1);
			ptl->themeRef = lua_ref(L, 1);
			ptl->next = _themes;
			_themes = ptl;
			/*
			 * 调用样式初始化
			 */
			lua_getfield(L, -1, "onInit");
			/*
			 * -1 onInit function
			 * -2 themeTable
			 */
			if (lua_isfunction(L, -1)){
				lua_pushvalue(L,-2);
				/*
				 * -1 thememTable
				 * -2 onInit function
				 * -3 themeTable
				 */
				lua_executeFunction(1);
				lua_pop(L, 1);
			}
			else{
				lua_pop(L, 2);
			}
			return 1;
		}
		else{
			lua_pop(L, 1);
		}
	}
	return 0;
}

void unloadThemes(const char *name)
{
	ThemesList * theme = _themes;
	ThemesList * prev = NULL;
	while (theme){
		if (strcmp(name, theme->name) == 0){
			if (prev){
				prev->next = theme->next;
			}
			else{
				_themes = theme->next;
			}
			/*
			 * 调用释放函数
			 */
			lua_State *L = lua_GlobalState();
			if (L&&theme->themeRef != LUA_REFNIL){
				lua_getref(L, theme->themeRef);
				lua_getfield(L, -1, "onRelease");
				if (lua_isfunction(L, -1)){
					lua_pushvalue(L, -2);
					lua_executeFunction(1);
					lua_pop(L, 1);
				}
				else{
					lua_pop(L, 2);
				}
				lua_unref(L, theme->themeRef);
			}
			free(theme);
			return;
		}
		prev = theme;
		theme = theme->next;
	}
}

void uiSetVisible(uiWidget *self, int b)
{
	if (b)
		self->isVisible |= VISIBLE;
	else
		self->isVisible &= ~VISIBLE;
}

void uiSetPosition(uiWidget *self, float x, float y)
{
	self->x = x;
	self->y = y;
	calcXForm(self);
}

void uiRotate(uiWidget *self, float angle)
{
	self->angle = angle;
	calcXForm(self);
}

void uiScale(uiWidget *self, float sx, float sy)
{
	self->sx = sx;
	self->sy = sy;
	calcXForm(self);
}

void uiSetSize(uiWidget *self, float w, float h)
{
	self->width = w;
	self->height = h;
}

void enableClipClient(uiWidget *self, int b)
{
	if (b)
		self->isVisible |= CLIP;
	else
		self->isVisible &= ~CLIP;
}

static int InParentWidget(uiWidget *parent, uiWidget *child, float x, float y)
{
	if (child->x+x > parent->width || child->x+x + child->width < 0)
		return 0;
	if (child->y+y > parent->height || child->y+y + child->height < 0)
		return 0;
	return 1;
}

int InWidget(uiWidget *parent, uiWidget *child)
{
	if (child->x > parent->width || child->x + child->width < 0)
		return 0;
	if (child->y > parent->height || child->y + child->height < 0)
		return 0;
	return 1;
}

/**
 * \brief 将所有可见的对象都链接在一起，并且返回其头，通过enum_next进行遍历
 * \param root 遍历的根节点
 * \param tail 将遍历的全部节点顺序连接，通过uiWidget的enum_next,enum_prev
 * \param 收集所有isVisible的标志位
 * \param renderFunc 对每个遍历到的uiWidget调用renderFunc(uiWidget*)
 * \return 返回tail
 */
static uiWidget * uiEnumWidgetVisible(uiWidget *root, uiWidget *tail,uiRenderProc renderFunc)
{
	uiWidget * child;
	int isclip = 0;

	memcpy(root->curxform, root->xform, sizeof(float) * 6);
	/* 如果有父控件将父控件的当前矩阵应用到变换中 */
	if (root->parent)
		nvgTransformMultiply(root->curxform, root->parent->curxform);

	nvgSetTransform(_vg, root->curxform);
	renderFunc(root);
	/* 对子窗口设置剪切区域 */
	if (root->isVisible&CLIP){
		nvgSave(_vg);
		nvgIntersectScissor(_vg, 0, 0, root->width, root->height);
		isclip = 1;
	}
	child = root->child;
	if ( (root->isVisible&SCROLL_CLIP) && root->parent){
		//根据父窗口的大小进行剪切
		uiWidget *pParent = root->parent;
		float x, y;
		x = root->x;
		y = root->y;
		while (child){
			if (child->isVisible&VISIBLE){
				if (InParentWidget(pParent, child,x,y)){
					_uss |= child->isVisible;
					tail->enum_next = child;
					child->enum_prev = tail;
					tail = child;
					tail->enum_next = NULL;
					tail = uiEnumWidgetVisible(child, tail,renderFunc);
				}
			}
			child = child->next;
		}
	}
	else{
		//正常情况
		while (child){
			if (child->isVisible&VISIBLE){
				if (InWidget(root, child)){
					_uss |= child->isVisible;
					tail->enum_next = child;
					child->enum_prev = tail;
					tail = child;
					tail->enum_next = NULL;
					tail = uiEnumWidgetVisible(child, tail,renderFunc);
				}
			}
			child = child->next;
		}
	}

	if (isclip){
		nvgRestore(_vg);
	}
	return tail;
}

//将正则坐标转换到root坐标系
static void transfromToWindow(float x,float y,float *px,float *py)
{
	*px = _root->width * x;
	*py = _root->height * y;
}

/*
 * 将SDL_Event转变为uiEvent
 */
static void prepareUIEvent()
{
	int i;
	SDL_Event *pevent;
	uiEvent *pev;
	int count = getSDLEventCount();
	_eventState.nEvent = 0;
	_eventState.haveEventType = 0;
	for (i = 0; i < count; i++){
		if (_eventState.nEvent >= ES_EVENT_MAX){
			SDL_Log("uiEvent overflow");
			break;
		}
		pevent = getSDLEvent(i);
		
		//dumpSDLEvent(pevent);

		switch (pevent->type){
/*
 * 在android,ios下SDL会模拟产生MouseButton,MouseMotion事件
 */
#if defined(_WIN32) || defined(__MAC__)
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			SDL_CaptureMouse(pevent->type == SDL_MOUSEBUTTONDOWN);
			if (pevent->button.button == SDL_BUTTON_LEFT){
				pev = &(_eventState.events[_eventState.nEvent++]);
				pev->t = pevent->button.timestamp/1000.0;
				pev->x = (float)pevent->button.x;
				pev->y = (float)pevent->button.y;
				if (pevent->type==SDL_MOUSEBUTTONDOWN){
					_eventState.haveEventType |= EVENT_TOUCHDOWN;
					pev->type = EVENT_TOUCHDOWN;
					_eventState.isTouchDown = 1;
					_eventState.lastTouchDown = *pev;
				}
				else{
					pev->type = EVENT_TOUCHUP;
					_eventState.haveEventType |= EVENT_TOUCHUP;
				}
			}
			break;
		case SDL_MOUSEMOTION:
			if (_eventState.isTouchDown){
				pev = &(_eventState.events[_eventState.nEvent++]);
				pev->t = pevent->motion.timestamp / 1000.0;
				pev->x = (float)pevent->motion.x;
				pev->y = (float)pevent->motion.y;
				pev->type = EVENT_TOUCHDROP;
				_eventState.haveEventType |= EVENT_TOUCHDROP;
			}
			break;
#endif
/*
 * TouchFinger事件中的坐标都是被正则为0-1范围
 */
#if defined(__ANDROID__) || defined(__IOS__)
		case SDL_FINGERDOWN:
			{
				pev = &(_eventState.events[_eventState.nEvent++]);
				pev->t = pevent->tfinger.timestamp / 1000.0;
				transfromToWindow(pevent->tfinger.x, pevent->tfinger.y,&pev->x,&pev->y);
				pev->type = EVENT_TOUCHDOWN;
				_eventState.haveEventType |= EVENT_TOUCHDOWN;
				_eventState.isTouchDown = 1;
				_eventState.lastTouchDown = *pev;
			}
			break;
		case SDL_FINGERUP:
			{
				pev = &(_eventState.events[_eventState.nEvent++]);
				pev->t = pevent->tfinger.timestamp / 1000.0;
				transfromToWindow(pevent->tfinger.x, pevent->tfinger.y, &pev->x, &pev->y);
				pev->type = EVENT_TOUCHUP;
				_eventState.haveEventType |= EVENT_TOUCHUP;
			}
			break;
		case SDL_FINGERMOTION:
			if (_eventState.isTouchDown){
				pev = &(_eventState.events[_eventState.nEvent++]);
				pev->t = pevent->tfinger.timestamp / 1000.0;
				transfromToWindow(pevent->tfinger.x, pevent->tfinger.y, &pev->x, &pev->y);
				pev->type = EVENT_TOUCHDROP;
				_eventState.haveEventType |= EVENT_TOUCHDROP;
			}
			break;
#endif
		}
	}
}

/* 在事件处理结束调用 */
static void endUIEvent()
{
	/* 
	 * 如果有抬起事件就清理isTouchDown 
	 */
	if (_eventState.isTouchDown && (_eventState.haveEventType&EVENT_TOUCHUP)){
		_eventState.isTouchDown = 0;
		memset(_eventState.lastTouchDownWidgets, 0, sizeof(_eventState.lastTouchDownWidgets));
	}
}

static void pushUiEventTable(lua_State *L, uiEvent *pev)
{
	lua_newtable(L);
	lua_pushinteger(L, pev->type);
	lua_setfield(L, -2, "type");
	lua_pushnumber(L, pev->x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, pev->y);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L, pev->x2);
	lua_setfield(L, -2, "x2");
	lua_pushnumber(L, pev->y2);
	lua_setfield(L, -2, "y2");
	lua_pushnumber(L, pev->t);
	lua_setfield(L, -2, "time");
	lua_pushnumber(L, pev->t2);
	lua_setfield(L, -2, "time2");
	lua_pushboolean(L, pev->inside);
	lua_setfield(L, -2, "inside");
}

static void lua_EventInput(uiEvent * pev)
{
	if (lua_pushEventFunction(EVENT_INPUT)){
		pushUiEventTable(lua_GlobalState(), pev);
		lua_executeFunction(1);
	}
}

/**
* \brief 枚举的过程中eventFunc可能改变窗口结构甚至删除窗口
* 这需要特殊处理，首先将满足调用func的窗口放入一个表中
* 然后才调用eventFunc函数，如果在func函数中删除了窗口，后面
* 的调用会访问非法指针，因此eventFunc中将要删除的窗口标记，
* 等枚举结束才进行真正的删除。
* \param root 要枚举的根节点
* \param renderFunc 
* \param eventFunc
* \param winWidth
* \param winHeight
* \param devicePixelRatio
*/
void uiEnumWidget(uiWidget *root, 
	uiRenderProc renderFunc, uiEventProc eventFunc,
	int winWidth, int winHeight, float devicePixelRatio)
{
	int i;
	uiWidget * head,*temp,*tail;
	head = root;
	head->enum_next = NULL;
	head->enum_prev = NULL; 
	tail = uiEnumWidgetVisible(root, head,renderFunc);
	/*
	 * 下面准备分发事件
	 */
	prepareUIEvent();
	/* 打开延时删除表 */
	uiDelayDelete(1);
	/*
	 * 处理事件，先预处理
	 */
	if (_eventState.haveEventType){
		temp = tail;
		for (i = 0; i < _eventState.nEvent; i++){
			uiEvent * pev = &(_eventState.events[i]);
			tail = temp;
			/*
			 * 从最上面的对象开始处理事件
			 */
			int b = 1;
			while (tail){
				/* 如果对象要独占此事件，终止传递 */
				if (eventFunc(tail, pev)){
					b = 0;
					break;
				}
				tail = tail->enum_prev;
			}
			if (b) lua_EventInput(pev);
		}
	}
	uiDelayDelete(0);
	endUIEvent();
	/* 删除延迟表中的对象 */
	head = _delayList;
	while (head){
		temp = head->remove;
		uiDeleteWidgetSelf(head);
		head = temp;
	}
}

/*
 * 调用对象的渲染方法onDraw,允许对象重载
 */
static void renderWidget(uiWidget * widget)
{
	/* 首先搜索对象的onDraw方法 */
	if (widget->selfRef != LUA_REFNIL){
		lua_State * L = lua_GlobalState();
		lua_getref(L, widget->selfRef);
		lua_getfield(L, -1, "onDraw");
		if (lua_isfunction(L, -1)){
			//lua_getref(L, widget->selfRef);
			lua_pushWidget(L, widget);
			lua_pushnumber(L, getLoopInterval());
			lua_executeFunction(2);
			lua_pop(L, 1); //classRef;
			return;
		}
		else{
			lua_pop(L, 2);
		}
	}
	/* 在搜索类方法 */
	if (widget->classRef != LUA_REFNIL){
		lua_State * L = lua_GlobalState();
		lua_getref(L, widget->classRef);
		lua_getfield(L, -1, "onDraw");
		if (lua_isfunction(L, -1)){
			//lua_getref(L, widget->selfRef);
			lua_pushWidget(L, widget);
			lua_pushnumber(L, getLoopInterval());
			lua_executeFunction(2);
			lua_pop(L, 1); //classRef;
		}
		else{
			lua_pop(L, 2);
		}
	}
	return;
}

/**
 * 调用对象的类方法如:onEvent
 */
static int callWidgetOnEvent(uiWidget * widget, const char *strEvent,uiEvent *pev)
{
	int result = 0;
	/* 先找对象的重载方法 */
	if (widget->selfRef != LUA_REFNIL){
		lua_State * L = lua_GlobalState();
		if (L){
			lua_getref(L, widget->selfRef);
			lua_getfield(L, -1, strEvent);
			if (lua_isfunction(L, -1)){
				//lua_getref(L, widget->selfRef);
				lua_pushWidget(L, widget);
				pushUiEventTable(L, pev);
				result = lua_executeFunction(2);
				lua_pop(L, 1); //classRef;
			}
			else{
				lua_pop(L, 2);
			}
		}
	}
	/* 然后访问类方法方法 */
	if (widget->classRef != LUA_REFNIL){
		lua_State * L = lua_GlobalState();
		if (L){
			lua_getref(L, widget->classRef);
			lua_getfield(L, -1, strEvent);
			if (lua_isfunction(L, -1)){
				//lua_getref(L, widget->selfRef);
				lua_pushWidget(L, widget);
				pushUiEventTable(L, pev);
				result = lua_executeFunction(2);
				lua_pop(L, 1); //classRef;
			}
			else{
				lua_pop(L, 2);
			}
		}
	}
	return result;
}

/*
 * 返回1终止事件继续传递，0可以继续传递事件
 */
static int eventWidget(uiWidget * widget,uiEvent *pev)
{
	float x, y;
	float w2o[6];

	/* 首先判断控件是否处理该事件 */
	if ( widget->handleEvent & pev->type ){ 
		if (nvgTransformInverse(w2o, widget->curxform)){
			/* 变换屏幕点到对象系 */
			nvgTransformPoint(&x, &y, w2o, pev->x, pev->y);
			/* 如果有无界标记就直接传递 */
			if ((widget->handleEvent & EVENT_UNBOUNDED) ||
				(x >= 0 && y >= 0 && x <= widget->width && y <= widget->height)){
				/* 投递事件到对象 */
				uiEvent ev = *pev;
				ev.x = x;
				ev.y = y;
				ev.inside = 1;
				if (_eventState.isTouchDown&&pev->type == EVENT_TOUCHUP){
					ev.x2 = _eventState.lastTouchDown.x;
					ev.y2 = _eventState.lastTouchDown.y;
					ev.t2 = _eventState.lastTouchDown.t;
				}
				else if (pev->type == EVENT_TOUCHDOWN){
					pushLastTouchDownWidgets(widget);
				}
				if (callWidgetOnEvent(widget, "onEvent", &ev))
					return 1; /* 如果事件被处理停止传递 */
				/* 独占数据不继续传递 */
				if ((widget->handleEvent&EVENT_EXCLUSIVE)||
					(widget->handleEvent&EVENT_BREAK))
					return 1;
			}
			else if (inLastTouchDownWidgets(widget)){
				/* 第一个接收掉EVENT_TOUCHDOWN的对象将接收直到EVENT_TOUCHEND的所有数据 */
				uiEvent ev = *pev;
				ev.x = x;
				ev.y = y;
				ev.inside = 0;
				if (_eventState.isTouchDown&&pev->type == EVENT_TOUCHUP){
					ev.x2 = _eventState.lastTouchDown.x;
					ev.y2 = _eventState.lastTouchDown.y;
					ev.t2 = _eventState.lastTouchDown.t;
					ev.type = EVENT_TOUCHUP;
				}
				if (callWidgetOnEvent(widget, "onEvent", &ev))
					return 1; /* 如果事件被处理停止传递 */
				if ((widget->handleEvent&EVENT_EXCLUSIVE) ||
					(widget->handleEvent&EVENT_BREAK))
					return 1;
			}
		}
		else{
			SDL_Log("ERROR : eventWidget nvgTransformInverse return 0");
			return 0;
		}
	}
	/* 终止传递 */
	if (widget->handleEvent&EVENT_BREAK || widget->handleEvent&EVENT_EXCLUSIVE){
		/* 
		 * 对于EVENT_BREAK对象，当该对象处理一种事件当不在对象内部发生时也传递给对象
		 * 当到达这里表示已经不在对象内部了。注意：x,y在上面已经计算好了
		 */
		if (widget->handleEvent&pev->type){
			uiEvent ev = *pev;
			ev.x = x;
			ev.y = y;
			ev.inside = 0;
			if (_eventState.isTouchDown&&pev->type == EVENT_TOUCHUP){
				ev.x2 = _eventState.lastTouchDown.x;
				ev.y2 = _eventState.lastTouchDown.y;
				ev.t2 = _eventState.lastTouchDown.t;
			}
			callWidgetOnEvent(widget, "onEvent", &ev);
		}
		return 1;
	}
	return 0;
}

/*
 * 渲染对象树结构
 */
unsigned int uiLoop()
{
	_uss = 0;
	uiWidget * root = uiRootWidget();
	if (root && root->isVisible&VISIBLE){
		nvgBeginFrame(_vg, (int)root->width, (int)root->height, 1);
		nvgResetTransform(_vg);
		uiEnumWidget(root, renderWidget, eventWidget, (int)root->width, (int)root->height, 1);
		nvgEndFrame(_vg);
	}
	return _uss;
}

void uiSendEvent(uiWidget *self)
{
	//uiEnumWidget(_root, renderWidget);
}

uiWidget * uiFormJson(const char *filename)
{
	return NULL;
}

void uiEnableEvent(uiWidget *self, unsigned int e)
{
	if (self)
		self->handleEvent |= e;
}

void uiDisableEvent(uiWidget *self, unsigned int e)
{
	if (self){
		self->handleEvent &= ~e;
	}
}

static void uiWidgetToRootForm(uiWidget *self,float xform[6])
{
	nvgTransformIdentity(xform);
	while (self){
		nvgTransformMultiply(xform, self->xform);
		self = self->parent;
	}
}

/*
 * 将全局坐标转换为widget坐标
 */
void uiRootToWidget(uiWidget *self, float *pt, int n)
{
	int i;
	float xform[6];
	float invs[6];
	float x, y;
	uiWidgetToRootForm(self, xform);
	nvgTransformInverse(invs, xform);
	for (i = 0; i < n; i++){
		x = *(pt + 2 * i);
		y = *(pt + 2 * i+1);
		nvgTransformPoint((pt + 2 * i), (pt + 2 * i + 1), invs, x, y);
	}
}

/*
 * 将widget坐标转换为全局坐标
 */
void uiWidgetToRoot(uiWidget *self, float *pt, int n)
{
	int i;
	float xform[6];
	float x, y;
	uiWidgetToRootForm(self, xform);
	for (i = 0; i < n; i++){
		x = *(pt + 2 * i);
		y = *(pt + 2 * i + 1);
		nvgTransformPoint((pt + 2 * i), (pt + 2 * i + 1), xform, x, y);
	}
}

/**
 * 未实现
 */
int uiPtInWidget(uiWidget *self, float x, float y)
{
	return 0;
}

static void donothingFunc(uiWidget *self)
{}

/**
 * \brief 在屏幕点x,y处穿透的widget列表，返回有多少个被穿透的widget
 * widget是一个指针数组空间，n是它的数量。最先穿透的放入0位置
 * \param x,y 穿透位置
 * \param widget[] 提供一个uiWidget指针表，记录穿透的uiWidget指针
 * \param n widget的个数
 * \return 返回穿透的数量
 */
int uiWidgetFormPt(float x, float y, uiWidget *widget[], int n)
{
	uiWidget * head, *tail;
	int idx = 0;
	head = _root;
	head->enum_next = NULL;
	head->enum_prev = NULL;
	tail = uiEnumWidgetVisible(_root, head, donothingFunc);
	while (tail){
		if (uiPtInWidget(tail, x, y)){
			if (idx < n)
				widget[idx++] = tail;
			else
				return idx;
		}
		tail = tail->enum_prev;
	}
	return idx;
}