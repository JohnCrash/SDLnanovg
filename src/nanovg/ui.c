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
	int haveEventType; /* ���¼��б��д���ָ�����¼����� */
	int isTouchDown;
	uiEvent lastTouchDown; /* ���һ�ΰ����¼� */
	uiWidget *lastTouchDownWidget; /* �����¼��Ľ��ܶ��� */
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
 * ͨ��sx,sy,angle������������ת���ֺ����Ų��֣������������б任
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

	/* ��ʼ���¼�״̬ */
	_eventState.nEvent = 0;
	_eventState.isTouchDown = 0;
	_eventState.lastTouchDownWidget = NULL;
	return 1;
}

/* luaui.c �еķ��� */
extern void lua_pushWidget(lua_State *L, uiWidget * widget);

/*
 * ���ö�����෽����:onInit,onRelease
 */
static void callWidgetEvent(uiWidget * widget, const char *strEvent)
{
	/* ���Ҷ�������ط��� */
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
	/* Ȼ������෽������ */
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
 * �򿪹ر���ʱɾ�����ܣ���������ʱɾ��ʱ��
 * Ҫɾ���Ķ��󶼱����������Ժ���ɾ����
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
			/* ������ʱɾ���� */
			self->remove = _delayList;
			_delayList = self;
		}
		else{
			/* ����ɾ������ */
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
	/* ɾ�����ڵ� */
	uiDeleteWidget(_root);
	uiDeleteWidgetSelf(_root);
	/* ɾ����ʽ�� */
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
		/* ������ʽ���в��Ҷ�Ӧ����ʽ */
		if (strcmp(themes_name, themes->name) == 0 && themes->themeRef!=LUA_REFNIL){
			lua_getref(L, themes->themeRef);
			lua_getfield(L, -1, widget_name);
			/*
			 * -1 widget_table
			 * -2 themes_table
			 */
			if (lua_istable(L, -1)){
				/*
				 * �ɹ��ҵ���ʽ���еĿؼ�ԭ�ͱ�
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
 * ���ӽڵ㶼ɾ��������Լ�����rootҲ���Լ�ɾ��
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

/* �����������ʾ������ */
void uiBringTop(uiWidget * self)
{
	if (self&&self->parent){
		if (self->parent->child != self){
			uiWidget * parent = self->parent;
			uiAddChild(parent, self);
		}
	}
}

/* �����������ʾ������ */
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
 * ��������뵽���еĿ�ʼ
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
* ��������뵽���еĽ�β
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
			 * ������ʽ��ʼ��
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
			 * �����ͷź���
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
 * �����пɼ��Ķ���������һ�𣬲��ҷ�����ͷ
 * ͨ��enum_next���б���
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
	/* ���Ӵ������ü������� */
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
 * ��SDL_Eventת��ΪuiEvent
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

/* ���¼������������ */
static void endUIEvent()
{
	/* 
	 * �����̧���¼�������isTouchDown 
	 */
	if (_eventState.isTouchDown && (_eventState.haveEventType&EVENT_TOUCHUP)){
		_eventState.isTouchDown = 0;
		_eventState.lastTouchDownWidget = NULL;
	}
}
/*
* ö�ٵĹ�����eventFunc���ܸı䴰�ڽṹ����ɾ������
* ����Ҫ���⴦�����Ƚ��������func�Ĵ��ڷ���һ������
* Ȼ��ŵ���eventFunc�����������func������ɾ���˴��ڣ�����
* �ĵ��û���ʷǷ�ָ�룬���eventFunc�н�Ҫɾ���Ĵ��ڱ�ǣ�
* ��ö�ٽ����Ž���������ɾ����
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
	 * ����׼���ַ��¼�
	 */
	prepareUIEvent();
	/* ����ʱɾ���� */
	uiDelayDelete(1);
	/*
	 * �����¼�����Ԥ����
	 */
	if (_eventState.haveEventType){
		temp = tail;
		for (int i = 0; i < _eventState.nEvent; i++){
			uiEvent * pev = &(_eventState.events[i]);
			tail = temp;
			/*
			 * ��������Ķ���ʼ�����¼�
			 */
			while (tail){
				/* �������Ҫ��ռ���¼�����ֹ���� */
				if (eventFunc(tail,pev))
					break;
				tail = tail->enum_prev;
			}
		}
	}
	uiDelayDelete(0);
	endUIEvent();
	/* ɾ���ӳٱ��еĶ��� */
	head = _delayList;
	while (head){
		temp = head->remove;
		uiDeleteWidgetSelf(head);
		head = temp;
	}
}

/*
 * ���ö������Ⱦ����onDraw,�����������
 */
static void renderWidget(uiWidget * widget)
{
	/* �������������onDraw���� */
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
	/* �������෽�� */
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
* ���ö�����෽����:onEvent
*/
static void callWidgetOnEvent(uiWidget * widget, const char *strEvent,uiEvent *pev)
{
	/* ���Ҷ�������ط��� */
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
	/* Ȼ������෽������ */
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
	/* ������ں���EVENT_EXCLUSIVE�����¼��ڶ����ڲ���ֹͣ���� */
	if ( (widget->handleEvent & pev->type) || 
		 (widget->handleEvent & EVENT_EXCLUSIVE) ){ 
		float x, y;
		float w2o[6];
		if (nvgTransformInverse(w2o, widget->curxform)){
			/* �任��Ļ�㵽����ϵ */
			nvgTransformPoint(&x, &y, w2o, pev->x, pev->y);
			if (x >= 0 && y >= 0 && x <= widget->width && y <= widget->height){
				/* ������ڽ�������EVENT_EXCLUSIVE�������� */
				if (widget->handleEvent & pev->type){
					/* Ͷ���¼������� */
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
				/* ��ռ���ݲ��������� */
				if ((widget->handleEvent&EVENT_EXCLUSIVE)||
					(widget->handleEvent&EVENT_BREAK))
					return 1;
			}
			else if (widget == _eventState.lastTouchDownWidget){
				/* ��һ�����յ�EVENT_TOUCHDOWN�Ķ��󽫽���ֱ��EVENT_TOUCHEND���������� */
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
	/* ��ֹ���� */
	if (widget->handleEvent &EVENT_BREAK)
		return 1;
	return 0;
}

/*
 * ��Ⱦ�������ṹ
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
 * ��ȫ������ת��Ϊwidget����
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
 * ��widget����ת��Ϊȫ������
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
 * ����Ļ��x,y����͸��widget�б������ж��ٸ�����͸��widget
 * widget��һ��ָ������ռ䣬n���������������ȴ�͸�ķ���0λ��
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