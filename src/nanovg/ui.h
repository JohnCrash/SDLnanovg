#ifndef _UI_H_
#define _UI_H_

enum{
	EVENT_TOUCHDOWN = 1,
	EVENT_TOUCHUP = 2,
	EVENT_DROP = 4,
	EVENT_ZOOM = 8,
};

typedef struct{
	float x, y, width, height;
	char isVisible;
	unsigned char handleEvent;
	int classRef;
	int selfRef;
	uiWidget *parent;
	uiWidget *child;
	uiWidget *next;
	uiWidget *prev;
} uiWidget;

typedef struct{
	char *name;
	char *filename;
	int themeRef;
	ThemesList *next;
} ThemesList;

uiWidget * uiRootWidget();
void uiAddChild(uiWidget *parent, uiWidget *child);
void uiRemoveFromParent(uiWidget *self);
void uiSetVisible(uiWidget *self, int b);
void uiSetPosition(uiWidget *self, float x, float y);
void uiSetSize(uiWidget *self, float w, float h);
uiWidget * uiCreateWidget(const char *themes, const char *name);
uiWidget * uiDeleteWidget(uiWidget *self);
void uiDrawWidget(uiWidget *self);
void uiSendEvent(uiWidget *self);
uiWidget * uiFormJson(const char *filename);

int loadTheames(const char *name,const char *filename);

#endif