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

	/* uiWidget isVisible 的可能组合 */
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
		/* 控件的当前变换矩阵 */
		float xform[6];
		/* 临时数据在渲染时用 */
		float curxform[6];
		/* ox,oy旋转和缩放中心，相对于x,y */
		float x,y,angle, sx, sy,ox,oy;
		char isVisible;
		unsigned char handleEvent;
		int classRef;
		int selfRef;
		luaWidget * luaobj;
		struct uiWidget_t *parent; //父窗口
		struct uiWidget_t *child; //子窗口
		struct uiWidget_t *next; //兄弟窗口下一个
		struct uiWidget_t *prev; //兄弟窗口上一个
		struct uiWidget_t *remove; //将要删除的窗口
		struct uiWidget_t *enum_next; //正在枚举的窗口
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