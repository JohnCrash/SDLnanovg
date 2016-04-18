#ifndef _UI_H_
#define _UI_H_

typedef struct{
	float x, y, width, height;
	uiWidget *parent;
	uiWidget *child;
	uiWidget *next;
} uiWidget;

typedef struct{

} uiWidgetArray;
#endif