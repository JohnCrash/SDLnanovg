#ifndef _UI_H_
#define _UI_H_

#ifdef __cplusplus
extern "C"{
#endif

	enum{
		EVENT_NONE = 0,
		EVENT_TOUCHDOWN = 1,
		EVENT_TOUCHUP = 2,
		EVENT_DROP = 4,
		EVENT_ZOOM = 8,
	};

	enum{
		INVISIBLE = 0,
		VISIBLE = 1,
		LINEAR = 2,
	};
	
	typedef struct {
		struct uiWidget * widget;
		int ref;
	} luaWidget;

	typedef struct uiWidget_t{
		float x, y, width, height;
		char isVisible;
		unsigned char handleEvent;
		int classRef;
		int selfRef;
		luaWidget * luaobj;
		struct uiWidget_t *parent; //������
		struct uiWidget_t *child; //�Ӵ���
		struct uiWidget_t *next; //�ֵܴ�����һ��
		struct uiWidget_t *prev; //�ֵܴ�����һ��
		struct uiWidget_t *remove; //��Ҫɾ���Ĵ���
		struct uiWidget_t *enum_next; //����ö�ٵĴ���
	} uiWidget;

	typedef struct ThemesList_t{
		char *name;
		char *filename;
		int themeRef;
		struct ThemesList_t *next;
	} ThemesList;

	int initUI();
	void releaseUI();

	uiWidget * uiRootWidget();
	void uiAddChild(uiWidget *parent, uiWidget *child);
	void uiRemoveFromParent(uiWidget *self);
	void uiSetVisible(uiWidget *self, int b);
	void uiSetPosition(uiWidget *self, float x, float y);
	void uiSetSize(uiWidget *self, float w, float h);
	uiWidget * uiCreateWidget(const char *themes, const char *name);
	void uiDeleteWidget(uiWidget *self);
	void uiDrawWidget(uiWidget *self);
	void uiSendEvent(uiWidget *self);
	uiWidget * uiFormJson(const char *filename);
	int InWidget(uiWidget *parent, uiWidget *child);

	int loadThemes(const char *name, const char *filename);

	typedef void(*uiEnumProc)(uiWidget *);
	void uiEnumWidget(uiWidget *root, uiEnumProc func);

#ifdef __cplusplus
}
#endif

#endif