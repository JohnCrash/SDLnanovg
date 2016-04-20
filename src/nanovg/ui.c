#include "lua.h"
#include "lauxlib.h"
#include "gles.h"
#include "sdlmain.h"
#include "eventhandler.h"
#include "luaext.h"
#include "ui.h"
#include "nanovg.h"
#include "nanovg_sdl.h"

static uiWidget * _root = NULL;
static ThemesList * _themes = NULL;
extern NVGcontext* _vg;

uiWidget * uiRootWidget()
{
	return _root;
}

int initUI()
{
	SDLState * state = getSDLState();
	_root = (uiWidget *)malloc(sizeof(uiWidget));
	if (!_root)return 0;
	memset(_root, 0, sizeof(uiWidget));
	_root->x = 0;
	_root->y = 0;
	_root->width = (float)state->window_w;
	_root->height = (float)state->window_h;
	_root->isVisible = VISIBLE;
	_root->handleEvent = EVENT_NONE;
	_root->classRef = LUA_REFNIL;
	_root->selfRef = LUA_REFNIL;
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
		if (L && ptemp->themeRef)
			lua_unref(L, ptemp->themeRef);
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

void uiAddChild(uiWidget *parent, uiWidget *child)
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
	L = lua_GlobalState();
	if (lua_executeScriptFileResult(filename, 1)){
		if (lua_istable(L, -1)){
			ThemesList * ptl = (ThemesList *)malloc(sizeof(ThemesList));
			if (!ptl)return 0;
			ptl->filename = strdup(filename);
			ptl->name = strdup(name);
			ptl->themeRef = lua_ref(L, 1);
			ptl->next = _themes;
			_themes = ptl;
			return 1;
		}
		else{
			lua_pop(L, 1);
		}
	}
	return 0;
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
}

void uiSetSize(uiWidget *self, float w, float h)
{
	self->width = w;
	self->height = h;
}

int InWidget(uiWidget *parent, uiWidget *child)
{
	if (child->x > parent->x + parent->width || child->x + child->width < parent->x)
		return 0;
	if (child->y > parent->y + parent->height || child->y + child->height < parent->y)
		return 0;
	return 1;
}

/*
 * 枚举的过程中func可能改变窗口结构甚至删除窗口
 * 这需要特殊处理，首先将满足调用func的窗口放入一个表中
 * 然后才调用func函数，如果在func函数中删除了窗口，后面
 * 的调用会访问非法指针，因此func中将要删除的窗口标记，
 * 等枚举结束才进行真正的删除。
 */
void uiEnumWidget(uiWidget *root, uiEnumProc func)
{
	uiWidget * child;
	uiWidget * head,*tail;
	head = NULL;
	if (root && root->isVisible&VISIBLE){
		head = root;
		tail = root;
		tail->enum_next = NULL;
		child = root->child;
		while (child){
			if (child->isVisible&VISIBLE){
				if (InWidget(root, child)){
					tail->enum_next = child;
					tail = child;
					tail->enum_next = NULL;
					uiEnumWidget(child, func);
				}
			}
			child = child->next;
		}
	}
	/* 打开延时删除表 */
	uiDelayDelete(1);
	while (head){
		func(head);
		head = head->enum_next;
	}
	uiDelayDelete(0);
	/* 删除延迟表中的对象 */
	head = _delayList;
	while (head){
		tail = head->remove;
		uiDeleteWidgetSelf(head);
		head = tail;
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
			lua_pushnumber(L, widget->x);
			lua_pushnumber(L, widget->y);
			lua_pushnumber(L, widget->width);
			lua_pushnumber(L, widget->height);
			lua_executeFunction(5);
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
			lua_pushnumber(L, widget->x);
			lua_pushnumber(L, widget->y);
			lua_pushnumber(L, widget->width);
			lua_pushnumber(L, widget->height);
			lua_executeFunction(5);
			lua_pop(L, 1); //classRef;
		}
		else{
			lua_pop(L, 2);
		}
	}
}

/*
 * 渲染对象树结构
 */
void uiDrawWidget(uiWidget *self)
{
	if (self){
		glViewport(0, 0, (int)self->width, (int)self->height);
		nvgBeginFrame(_vg, (int)self->width, (int)self->height, 1);
		uiEnumWidget(_root, renderWidget);
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