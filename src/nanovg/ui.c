#include <math.h>
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


typedef struct {
	int haveEventType; /* 在事件列表中存在指定的事件类型 */
	int isTouchDown;
	uiEvent lastTouchDown; /* 最近一次按下事件 */
	uiWidget *lastTouchDownWidget; /* 按下事件的接受对象 */
	int nEvent;
	uiEvent events[8];
} uiEventState;

uiEventState _eventState;
static uiWidget * _root = NULL;
static ThemesList * _themes = NULL;
extern NVGcontext* _vg;

uiWidget * uiRootWidget()
{
	return _root;
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
	SDLState * state = getSDLState();
	_root = (uiWidget *)malloc(sizeof(uiWidget));
	if (!_root)return 0;
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

	/* 初始化事件状态 */
	_eventState.nEvent = 0;
	_eventState.isTouchDown = 0;
	_eventState.lastTouchDownWidget = NULL;
	return 1;
}

/* luaui.c 中的方法 */
extern void lua_pushWidget(lua_State *L, uiWidget * widget);

/*
 * 调用对象的类方法如:onInit,onRelease
 */
static void callWidgetEvent(uiWidget * widget, const char *strEvent)
{
	/* 先找对象的重载方法 */
	if (widget->selfRef != LUA_REFNIL){
		lua_State * L = lua_GlobalState();
		if (L){
			lua_getref(L, widget->selfRef);
			lua_getfield(L, -1, strEvent);
			if (lua_isfunction(L, -1)){
				//lua_getref(L, widget->selfRef);
				lua_pushWidget(L, widget);
				lua_executeFunction(1);
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
				lua_executeFunction(1);
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
			callWidgetEvent(self, "onRelease");
			if (L && self->selfRef != LUA_REFNIL){
				lua_unref(L, self->selfRef);
			}
			if (L && self->luaobj){
				self->luaobj->widget = NULL;
				lua_unref(L, self->luaobj->ref);
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
				self->classRef = lua_ref(L, 1);
				lua_newtable(L);
				self->selfRef = lua_ref(L,1);
				lua_pop(L, 1);
				self->sx = 1.0f;
				self->sy = 1.0f;
				calcXForm(self);
				self->isVisible = VISIBLE;
				self->handleEvent = EVENT_NONE;
				callWidgetEvent(self, "onInit");
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
			if (!ptl)return 0;
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
		self->isVisible &= !VISIBLE;
	else
		self->isVisible |= VISIBLE;
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
		self->isVisible &= !CLIP;
}

int InWidget(uiWidget *parent, uiWidget *child)
{
	if (child->x > parent->width || child->x + child->width < 0)
		return 0;
	if (child->y > parent->height || child->y + child->height < 0)
		return 0;
	return 1;
}

/*
 * 将所有可见的对象都链接在一起，并且返回其头
 * 通过enum_next进行遍历
 */
static uiWidget * uiEnumWidgetVisible(uiWidget *root, uiWidget *tail, uiRenderProc renderFunc)
{
	uiWidget * child;
	int isclip = 0;
	if (root->parent){
		memcpy(root->curxform, root->xform, sizeof(float)* 6);
		nvgTransformMultiply(root->curxform, root->parent->curxform);
	}
	else{
		memcpy(root->curxform,root->xform,sizeof(float)*6);
	}
	nvgSetTransform(_vg, root->curxform);
	renderFunc(root);
	/* 对子窗口设置剪切区域 */
	if (root->isVisible&CLIP){
		nvgSave(_vg);
		nvgScissor(_vg, 0, 0, root->width, root->height);
		isclip = 1;
	}
	child = root->child;
	while (child){
		if (child->isVisible&VISIBLE){
			if (InWidget(root, child)){
				tail->enum_next = child;
				child->enum_prev = tail;
				tail = child;
				tail->enum_next = NULL;
				tail = uiEnumWidgetVisible(child, tail, renderFunc);
			}
		}
		child = child->next;
	}

	if (isclip){
		nvgRestore(_vg);
	}
	return tail;
}

/*
 * 将SDL_Event转变为uiEvent
 */
static void prepareUIEvent()
{
	SDL_Event *pevent;
	uiEvent *pev;
	int count = getSDLEventCount();
	_eventState.nEvent = 0;
	_eventState.haveEventType = 0;
	for (int i = 0; i < count; i++){
		if (_eventState.nEvent >= 8){
			SDL_Log("uiEvent overflow");
			break;
		}
		pevent = getSDLEvent(i);
		switch (pevent->type){
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			if (pevent->button.button == SDL_BUTTON_LEFT){
				pev = &(_eventState.events[_eventState.nEvent++]);
				pev->t = pevent->button.timestamp;
				pev->x = pevent->button.x;
				pev->y = pevent->button.y;
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
				pev->t = pevent->motion.timestamp;
				pev->x = pevent->motion.x;
				pev->y = pevent->motion.y;
				pev->type = EVENT_TOUCHDROP;
				_eventState.haveEventType |= EVENT_TOUCHDROP;
			}
			break;
		case SDL_FINGERDOWN:
			{
				pev = &(_eventState.events[_eventState.nEvent++]);
				pev->t = pevent->tfinger.timestamp;
				pev->x = pevent->tfinger.x;
				pev->y = pevent->tfinger.y;
				pev->type = EVENT_TOUCHDOWN;
				_eventState.haveEventType |= EVENT_TOUCHDOWN;
				_eventState.isTouchDown = 1;
				_eventState.lastTouchDown = *pev;
			}
			break;
		case SDL_FINGERUP:
			{
				pev = &(_eventState.events[_eventState.nEvent++]);
				pev->t = pevent->tfinger.timestamp;
				pev->x = pevent->tfinger.x;
				pev->y = pevent->tfinger.y;
				pev->type = EVENT_TOUCHUP;
				_eventState.haveEventType |= EVENT_TOUCHUP;
			}
			break;
		case SDL_FINGERMOTION:
			if (_eventState.isTouchDown){
				pev = &(_eventState.events[_eventState.nEvent++]);
				pev->t = pevent->tfinger.timestamp;
				pev->x = pevent->tfinger.x;
				pev->y = pevent->tfinger.y;
				pev->type = EVENT_TOUCHDROP;
				_eventState.haveEventType |= EVENT_TOUCHDROP;
			}
			break;
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
		_eventState.lastTouchDownWidget = NULL;
	}
}
/*
* 枚举的过程中eventFunc可能改变窗口结构甚至删除窗口
* 这需要特殊处理，首先将满足调用func的窗口放入一个表中
* 然后才调用eventFunc函数，如果在func函数中删除了窗口，后面
* 的调用会访问非法指针，因此eventFunc中将要删除的窗口标记，
* 等枚举结束才进行真正的删除。
*/
void uiEnumWidget(uiWidget *root, 
	uiRenderProc renderFunc, uiEventProc eventFunc,
	int winWidth, int winHeight, float devicePixelRatio)
{
	uiWidget * head,*temp,*tail;
	head = root;
	head->enum_next = NULL;
	head->enum_prev = NULL; 
	tail = uiEnumWidgetVisible(root, head, renderFunc);
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
		for (int i = 0; i < _eventState.nEvent; i++){
			uiEvent * pev = &(_eventState.events[i]);
			tail = temp;
			/*
			 * 从最上面的对象开始处理事件
			 */
			while (tail){
				/* 如果对象要独占此事件，终止传递 */
				if (eventFunc(tail,pev))
					break;
				tail = tail->enum_prev;
			}
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
			lua_executeFunction(1);
			lua_pop(L, 1); //classRef;
			return 0;
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
			lua_executeFunction(1);
			lua_pop(L, 1); //classRef;
		}
		else{
			lua_pop(L, 2);
		}
	}
	return 0;
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
/*
* 调用对象的类方法如:onEvent
*/
static void callWidgetOnEvent(uiWidget * widget, const char *strEvent,uiEvent *pev)
{
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
				lua_executeFunction(2);
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
				pushUiEventTable(L, pev);
				lua_executeFunction(2);
				lua_pop(L, 1); //classRef;
			}
			else{
				lua_pop(L, 2);
			}
		}
	}
}

static int eventWidget(uiWidget * widget,uiEvent *pev)
{
	/* 如果窗口含有EVENT_EXCLUSIVE并且事件在对象内部将停止传递 */
	if ( (widget->handleEvent & pev->type) || 
		 (widget->handleEvent & EVENT_EXCLUSIVE) ){ 
		float x, y;
		float w2o[6];
		if (nvgTransformInverse(w2o, widget->curxform)){
			/* 变换屏幕点到对象系 */
			nvgTransformPoint(&x, &y, w2o, pev->x, pev->y);
			if (x >= 0 && y >= 0 && x <= widget->width && y <= widget->height){
				/* 如果窗口仅仅含有EVENT_EXCLUSIVE将不处理 */
				if (widget->handleEvent & pev->type){
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
					else if (!_eventState.lastTouchDownWidget && pev->type == EVENT_TOUCHDOWN){
						_eventState.lastTouchDownWidget = widget;
					}
					callWidgetOnEvent(widget, "onEvent", &ev);
				}
				/* 独占数据不继续传递 */
				if ((widget->handleEvent&EVENT_EXCLUSIVE)||
					(widget->handleEvent&EVENT_BREAK))
					return 1;
			}
			else if (widget == _eventState.lastTouchDownWidget){
				/* 第一个接收掉EVENT_TOUCHDOWN的对象将接收直到EVENT_TOUCHEND的所有数据 */
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
	if (widget->handleEvent &EVENT_BREAK)
		return 1;
	return 0;
}

/*
 * 渲染对象树结构
 */
void uiLoop()
{
	uiWidget * self = uiRootWidget();
	if (self && self->isVisible&VISIBLE){
		glViewport(0, 0, (int)self->width, (int)self->height);
		nvgBeginFrame(_vg, (int)self->width, (int)self->height, 1);
		nvgResetTransform(_vg);
		uiEnumWidget(_root, renderWidget, eventWidget,(int)self->width, (int)self->height, 1);
		nvgEndFrame(_vg);
	}
}

void uiSendEvent(uiWidget *self)
{
	//uiEnumWidget(_root, renderWidget);
}

uiWidget * uiFormJson(const char *filename)
{
	return NULL;
}

void uiEnableEvent(uiWidget *self, int e)
{
	if (self)
		self->handleEvent |= e;
}

void uiDisableEvent(uiWidget *self, int e)
{
	if (self)
		self->handleEvent &= !e;
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
	float xform[6];
	float invs[6];
	float x, y;
	uiWidgetToRootForm(self, xform);
	nvgTransformInverse(invs, xform);
	for (int i = 0; i < n; i++){
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
	float xform[6];
	float x, y;
	uiWidgetToRootForm(self, xform);
	for (int i = 0; i < n; i++){
		x = *(pt + 2 * i);
		y = *(pt + 2 * i + 1);
		nvgTransformPoint((pt + 2 * i), (pt + 2 * i + 1), xform, x, y);
	}
}

int uiPtInWidget(uiWidget *self, float x, float y)
{

}

static void donothingFunc(uiWidget *self)
{}
/*
 * 在屏幕点x,y处穿透的widget列表，返回有多少个被穿透的widget
 * widget是一个指针数组空间，n是它的数量。最先穿透的放入0位置
 */
int uiWidgetFormPt(float x, float y, uiWidget *widget[], int n)
{
	uiWidget * head, *temp, *tail;
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