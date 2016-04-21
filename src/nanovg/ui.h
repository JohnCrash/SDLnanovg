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

	/* uiWidget isVisible �Ŀ������ */
	enum{
		INVISIBLE = 0,
		VISIBLE = 1,
		LINEAR = 2,
		CLIP = 4,
	};
	
	typedef struct {
		struct uiWidget * widget;
		int ref;
	} luaWidget;

	typedef struct uiWidget_t{
		float width, height;
		/* �ؼ��ĵ�ǰ�任���� */
		float xform[6];
		/* ��ʱ��������Ⱦʱ�� */
		float curxform[6];
		/* ox,oy��ת���������ģ������x,y */
		float x,y,angle, sx, sy,ox,oy;
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

	void uiBringTop(uiWidget * self);
	void uiBringBottom(uiWidget * self);
	void uiAddChild(uiWidget *parent, uiWidget *child);
	void uiAddChildToTail(uiWidget *parent, uiWidget *child);
	void uiRemoveFromParent(uiWidget *self);
	void uiSetVisible(uiWidget *self, int b);
	void uiSetPosition(uiWidget *self, float x, float y);
	void uiSetSize(uiWidget *self, float w, float h);
	void uiRotate(uiWidget *self, float angle);
	void uiScale(uiWidget *self, float sx, float sy);
	uiWidget * uiCreateWidget(const char *themes, const char *name);
	void uiDeleteWidget(uiWidget *self);
	void uiDrawWidget(uiWidget *self);
	void uiSendEvent(uiWidget *self);
	uiWidget * uiFormJson(const char *filename);
	int InWidget(uiWidget *parent, uiWidget *child);

	int loadThemes(const char *name, const char *filename);
	void unloadThemes(const char *name);

	typedef void(*uiEnumProc)(uiWidget *);
	void uiEnumWidget(uiWidget *root, uiEnumProc func);
	void clientWidget(uiWidget *self, float x, float y, float w, float h);
	void enableClipClient(uiWidget *self, int b);
#ifdef __cplusplus
}
#endif

#endif