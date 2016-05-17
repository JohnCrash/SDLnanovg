#ifndef _UI_H_
#define _UI_H_

#ifdef __cplusplus
extern "C"{
#endif
	/**
	 * \addtogroup UI
	 * @{
	 */
	 
	/**
	 * 一个 ::uiWidget 可以设置下列的事件类型或者类型组合。框架会根据类型来调整对该
	 * ::uiWidget 的处理流程。
	 */
	enum{
		EVENT_NONE = 0, /**<此 ::uiWidget 不处理任何事件*/
		EVENT_TOUCHDOWN = 1, /**<触摸屏被按下，或者鼠标左键被按下*/
		EVENT_TOUCHDROP = 2, /**<在触摸屏幕上拖动，或者鼠标左键拖动*/
		EVENT_TOUCHUP = 4, /**<触摸抬起，或者鼠标左键抬起*/
		EVENT_ZOOM = 8, /**<触摸屏两手指拖动操作*/
		EVENT_BREAK = 16, /**<停止将事件传递到此 ::uiWidget_t Z轴下的uiWidget_t对象*/
		EVENT_EXCLUSIVE = 32, /**<如果事件在对象内部发生就停止传递*/
	};

	/**
	 * #uiWidget isVisible 的可能组合 
	 */
	enum{
		INVISIBLE = 0, ///<不可见
		VISIBLE = 1, ///<可见
		LINEAR = 2, ///<线性排列，在一些情况下设置这个可以提高效率
		CLIP = 4, ///< #uiWidget 的子对象不能绘制到uiWidget外面
	};
	
	typedef struct {
		int type;
		int inside; /**< 事件在对象内部发生 */
		unsigned int t; /**< 发生的时间 SDL_GetTicks */
		float x, y; /**< touch */
		unsigned int t2; /**< 发生的时间 SDL_GetTicks */
		float x2, y2; /**< zoom */
	} uiEvent;

	typedef struct {
		struct uiWidget * widget;
		int ref;
	} luaWidget;

	/**
	 * 界面控件。
	 */
	typedef struct uiWidget_t{
		float width, height;
		/** 控件的当前变换矩阵,通过它可以将本地坐标变换到屏幕坐标 */
		float xform[6];
		/** 临时数据在渲染时用 */
		float curxform[6];
		/** ox,oy旋转和缩放中心，相对于x,y */
		float x,y,angle, sx, sy,ox,oy;
		char isVisible;
		/**
		 * 向框架表明如果处理事件，看\ref 事件类型
		 */
		unsigned char handleEvent;
		int classRef;
		int selfRef;
		luaWidget * luaobj;
		struct uiWidget_t *parent; /**<父窗口*/
		struct uiWidget_t *child; /**<子窗口*/
		struct uiWidget_t *next; /**<兄弟窗口下一个*/
		struct uiWidget_t *prev; /**<兄弟窗口上一个*/
		struct uiWidget_t *remove; /**<将要删除的窗口*/
		struct uiWidget_t *enum_next; /**<正在枚举的窗口*/
		struct uiWidget_t *enum_prev;
	} uiWidget;

	/**
	 * 界面样式表，通过使用函数 #loadThemes #unloadThemes 来加载样式。
	 * 通过使用函数 #uiCreateWidget 来创建一个指定样式的 #uiWidget_t
	 */
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
	int uiPtInWidget(uiWidget *self, float x, float y);

	void uiRootToWidget(uiWidget *self,float *pt,int n);
	void uiWidgetToRoot(uiWidget *self, float *pt, int n);
	int uiWidgetFormPt(float x,float y,uiWidget *widget[],int n);

	uiWidget * uiCreateWidget(const char *themes, const char *name);
	void uiDeleteWidget(uiWidget *self);
	void uiLoop();
	void uiSendEvent(uiWidget *self);
	uiWidget * uiFormJson(const char *filename);
	int InWidget(uiWidget *parent, uiWidget *child);

	int loadThemes(const char *name, const char *filename);
	void unloadThemes(const char *name);

	typedef void (*uiRenderProc)(uiWidget *);
	typedef int(*uiEventProc)(uiWidget *, uiEvent *);

	void clientWidget(uiWidget *self, float x, float y, float w, float h);
	void enableClipClient(uiWidget *self, int b);

	void uiEnableEvent(uiWidget *self,int e);
	void uiDisableEvent(uiWidget *self,int e);
	
	 /** @} */
#ifdef __cplusplus
}
#endif

#endif