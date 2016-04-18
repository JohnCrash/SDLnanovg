#include "lua.h"
#include "lauxlib.h"
#include "sdlmain.h"
#include "eventhandler.h"
#include "luaext.h"
#include "ui.h"

static uiWidget * _root = NULL;
static ThemesList * _themes = NULL;

uiWidget * uiRootWidget()
{
	return _root;
}

int initUI()
{
	SDLState * state = getSDLState();
	_root = (uiWidget *)malloc(sizeof(uiWidget));
	if (!_root)return 0;
	
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

void releaseUI()
{
	lua_State * L = lua_GlobalState();
	uiDeleteWidget(_root);
	while(_themes){
		ThemesList * ptemp = _themes;
		_themes = ptemp->next;
		if (ptemp->themeRef)
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
		if (strcmp(themes_name, themes->name) == 0 && themes->themeRef!=LUA_REFNIL){
			lua_getref(L, themes->themeRef);
			lua_getfield(L, -1, widget_name);
			if (lua_istable(L, -1)){
				/*
				 * 成功找到样式表中的控件原型表
				 */
				uiWidget * self = (uiWidget*)malloc(sizeof(uiWidget));
				if (!self)return NULL;
				self->classRef = lua_ref(L, 1);
				lua_newtable(L);
				self->selfRef = lua_ref(L,1);
				self->x = self->y = self->width = self->height = 0;
				self->isVisible = VISIBLE;
				self->handleEvent = EVENT_NONE;
				return self;
			}
		}
		themes = themes->next;
	}
	return NULL;
}

void uiDeleteWidget(uiWidget *self)
{
	if (self){
		uiRemoveFromParent(self);
		if (self->selfRef != LUA_REFNIL){
			lua_State * L = lua_GlobalState();
			lua_unref(L, self->selfRef);
		}
		free(self);
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
	}
}

void uiRemoveFromParent(uiWidget *self)
{
	if (self){
		self->parent = NULL;
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

int loadTheames(const char *name, const char *filename)
{
	int n;
	lua_State *L;
	L = lua_GlobalState();
	n = lua_executeScriptFile(filename);
	if (n>0 && lua_istable(L, -n)){
		ThemesList * ptl = (ThemesList *)malloc(sizeof(ThemesList));
		if (!ptl)return 0;
		ptl->filename = strdup(filename);
		ptl->name = strdup(name);
		lua_pushvalue(L, -n);
		ptl->themeRef = lua_ref(L, 1);
		ptl->next = _themes;
		_themes->next = ptl;
		return 1;
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

void uiEnumWidget(uiWidget *root, uiEnumProc func)
{

}

static void renderWidget(uiWidget * widget)
{
	if (widget->classRef != LUA_REFNIL){
		lua_State * L = lua_GlobalState();
		lua_getref(L, widget->classRef);
		lua_getfield(L, -1, "onDraw");
		lua_getref(L, widget->selfRef);
		lua_pushnumber(L, widget->x);
		lua_pushnumber(L, widget->y);
		lua_pushnumber(L, widget->width);
		lua_pushnumber(L, widget->height);
		lua_executeFunction(5);
	}
}

void uiDrawWidget(uiWidget *self)
{
	uiEnumWidget(_root, renderWidget);
}

void uiSendEvent(uiWidget *self)
{
	//uiEnumWidget(_root, renderWidget);
}

uiWidget * uiFormJson(const char *filename)
{
	return NULL;
}