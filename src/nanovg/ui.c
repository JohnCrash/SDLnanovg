#include <stdio.h>
#include "ui.h"

static uiWidget * _root = NULL;

uiWidget * uiRootWidget()
{
	return _root;
}

uiWidget * uiCreateWidget(const char *themes, const char *name)
{

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

