#ifndef _UI_H_
#define _UI_H_

#ifdef __cplusplus
extern "C"{
#endif
	/**
	 * \addtogroup UI
	 * \brief 界面相关的函数和结构
	 * @{
	 */
	 
	/**
	 * 一个 ::uiWidget 可以设置下列的事件类型或者类型组合。框架会根据类型来调整对该
	 * ::uiWidget 的处理流程。
	 */
	enum{
		EVENT_NONE = 0,			/**<此 ::uiWidget 不处理任何事件*/
		EVENT_TOUCHDOWN = 1,	/**<触摸屏被按下，或者鼠标左键被按下*/
		EVENT_TOUCHDROP = 2,	/**<在触摸屏幕上拖动，或者鼠标左键拖动*/
		EVENT_TOUCHUP = 4,		/**<触摸抬起，或者鼠标左键抬起*/
		EVENT_ZOOM = 8,			/**<触摸屏两手指拖动操作*/
		EVENT_BREAK = 16,		/**<停止将事件传递到此 ::uiWidget_t Z轴下的uiWidget_t对象*/
		EVENT_EXCLUSIVE = 32,	/**<如果事件在对象内部发生就停止传递*/
		EVENT_UNBOUNDED = 64,	/**<不考虑节点的尺寸直接将事件传递给该节点*/
	};

	enum{
		ALIGN_LEFT = 1,
		ALIGN_CENTER = 2,
		ALIGN_RIGHT = 4,
		ALIGN_TOP = 8,
		ALIGN_MIDDLE = 16,
		ALIGN_BOTTOM = 32,
		ALIGN_BASELINE = 64,
		HORIZONTAL = 128,
		VERTICAL = 256,
		GRID = 512,
		REVERSE = 1024,
	};
	/**
	 * #uiWidget isVisible 的可能组合 
	 */
	enum{
		INVISIBLE = 0,		///<不可见
		VISIBLE = 1,		///<可见
		LINEAR = 2,			///<线性排列，在一些情况下设置这个可以提高效率
		CLIP = 4,			///< #uiWidget 的子对象不能绘制到uiWidget外面
		SCROLL_CLIP = 8,	///<滚动去中的子剪切区
		UPDATE_HIGH = 16,	///<高速更新界面
		UPDATE_MID = 32,	///<中速更新
		UPDATE_LOW = 64,	///<低速更新
		UPDATE_OFF = 128,	///<低速更新
	};
	
	typedef struct {
		int type;
		int inside;	/**< 事件在对象内部发生 */
		double t;	/**< 发生的时间 getLoopInterval() */
		float x, y;	/**< touch */
		double t2;	/**< 发生的时间 SDL_GetTicks */
		float x2, y2;	/**< zoom */
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
		unsigned int isVisible;
		/**
		 * 向框架表明如何处理事件
		 */
		unsigned char handleEvent;
		int classRef;
		int selfRef;
		int hookRef;
		luaWidget * luaobj;
		struct uiWidget_t *parent;	/**<父窗口*/
		struct uiWidget_t *child;	/**<子窗口*/
		struct uiWidget_t *next;	/**<兄弟窗口下一个*/
		struct uiWidget_t *prev;	/**<兄弟窗口上一个*/
		struct uiWidget_t *remove;	/**<将要删除的窗口*/
		struct uiWidget_t *enum_next;	/**<正在枚举的窗口*/
		struct uiWidget_t *enum_prev;
	} uiWidget;

	/**
	 * \brief  界面样式表
	 *
	 * 通过使用函数 #loadThemes #unloadThemes 来加载样式。
	 *
	 * 通过使用函数 #uiCreateWidget 来创建一个指定样式的 #uiWidget_t    
	 */
	typedef struct ThemesList_t{
		char *name;
		char *filename;
		int themeRef;
		struct ThemesList_t *next;
	} ThemesList;
	
	/**
	 * \brief 初始化UI,这个将创建一个事件机制，并且创建一个根节点。也看 #uiRootWidget
	 * 在退出的适当地方调用 #releaseUI 来释放分配的资源。
	 * \return 成功返回1，失败返回0
	 */
	int initUI();
	
	/**
	 * \brief 释放由 #initUI 分配的资源。
	 */
	void releaseUI();

	/**
	 * \brief 得到根节点。
	 * \return 成功返回根节点 #uiWidget
	 */
	uiWidget * uiRootWidget();

	/**
	 * \brief 将uiWidget对象放到最上面。
	 */
	void uiBringTop(uiWidget * self);
	
	/**
	 * \brief 将 *uiWidget* 对象放到最下面。
	 */
	void uiBringBottom(uiWidget * self);
	
	/**
	 * \brief 向父节点 \a parent 加入一个子节点 \a child。
	 * 并且加入到其他的子节点的上面。 看 #uiAddChildToTail
	 */	
	void uiAddChild(uiWidget *parent, uiWidget *child);
	
	/**
	 * \brief 向父节点 \a parent 加入一个子节点 \a child。
	 * 并且加入到其他的子节点的最下面。看 #uiAddChild
	 */
	void uiAddChildToTail(uiWidget *parent, uiWidget *child);
	
	/**
	 * \brief 将控件 \a self 从其父对象的子对象列表中移除。
	 */
	void uiRemoveFromParent(uiWidget *self);
	
	/**
	 * \brief 设置控件 \a self 的可见性。
	 * \param b 如果等于 #VISIBLE 可见， #INVISIBLE 不可见。
	 */
	void uiSetVisible(uiWidget *self, int b);
	
	/**
	 * \brief 设置控件的位置，相对于父节点。
	 * \param x 在父节点中的x坐标
	 * \param y 在父节点中的y坐标
	 */
	void uiSetPosition(uiWidget *self, float x, float y);
	
	/*
	 * \brief 设置控件的尺寸。
	 * \param w 宽度
	 * \param h 高度
	 */
	void uiSetSize(uiWidget *self, float w, float h);
	
	/**
	 * \brief 将控件旋转一个角度 \a angle ,旋转点由 #uiWidget 的ox,oy给定。
	 * \param angle 要旋转的角度(弧度)
	 */
	void uiRotate(uiWidget *self, float angle);
	
	/**
	 * \brief 将控件进行缩放，缩放中心点由 #uiWidget 的ox,oy给定。
	 * \param sx 在x方向上的比例
	 * \param sy 在y方向上的比例
	 */	
	void uiScale(uiWidget *self, float sx, float sy);
	
	/**
	 * \brief 判断点 \a x \a y 是否在 \a self 内部。
	 * \param x 屏幕坐标x
	 * \param y 屏幕坐标y
	 * \return 成功返回1，失败返回0
	 */
	int uiPtInWidget(uiWidget *self, float x, float y);

	/**
	 * \brief 将点从根节点坐标系转换到控件 \a self 的坐标系。
	 * \param pt 点数组，序列是x,y,x2,y2...
	 * \param n 表示点的个数。
	 */
	void uiRootToWidget(uiWidget *self,float *pt,int n);
	
	/**
	 * \brief 将点从节点 \a self 坐标系转换到根节点的坐标系。 看 #uiRootToWidget
	 * \param pt 点数组，序列是x,y,x2,y2...
	 * \param n 表示点的个数。
	 */	
	void uiWidgetToRoot(uiWidget *self, float *pt, int n);
	
	/**
	 * \brief 取得在屏幕坐标x,y位置穿透的全部控件
	 * \param x 屏幕坐标x
	 * \param y 屏幕坐标y
	 * \param widget 取回的控件数组
	 * \param n 控件数组的个数
	 * \return 成功返回取回控件的数量，失败返回0
	 */
	int uiWidgetFormPt(float x,float y,uiWidget *widget[],int n);

	/**
	 * \brief 创建一个样式为 \a themes 的控件。控件类型由 \a name 指定。
	 * \param themes 样式名称，看 #loadThemes #unloadThemes
	 * \param name 控件类型名称，由样式表决定。
	 */
	uiWidget * uiCreateWidget(const char *themes, const char *name);
	
	/**
	 * 删除控件 \a self
	 */
	void uiDeleteWidget(uiWidget *self);
	
	/**
	 * \brief 遍历绘制根节点和子节点，这个函数由框架调用。
	 */
	unsigned int  uiLoop();
	
	/**
	 * 
	 */	
	void uiSendEvent(uiWidget *self);
	
	/**
	 * \brief 根据json文件的描述创建一个uiWidget树。
	 */	
	uiWidget * uiFormJson(const char *filename);
	
	/**
	 * \brief 判断节点 \a child 是否为 \a parent 的子节点。
	 * \return 是返回1，不是返回0
	 */	
	int InWidget(uiWidget *parent, uiWidget *child);

	/**
	 * \brief 装入一个新的样式表文件 \a filename ,并且将其命名为 \a name 。
	 * \param name 样式表名称
	 * \param filename 样式表文件
	 * \return 成功返回1，失败返回0
	 */	
	int loadThemes(const char *name, const char *filename);
	
	/**
	 * 卸载掉样式表。
	 */	
	void unloadThemes(const char *name);

	
	/**
	 * #uiEnumWidget
	 */	
	typedef void (*uiRenderProc)(uiWidget *);
	
	/**
	 * #uiEnumWidget
	 */	
	typedef int(*uiEventProc)(uiWidget *, uiEvent *);
	
	/**
	 * \brief 设置 \a self 的剪切区
	 */
	void clientWidget(uiWidget *self, float x, float y, float w, float h);
	
	/**
	 * \brief 打开或者关闭控件 \a self 的剪切区
	 * \param b 不为0设置，为0关闭。看 #CLIP
	 */	
	void enableClipClient(uiWidget *self, int b);
	
	/**
	 * \brief 打开一个事件设置，e可以是 #EVENT_TOUCHDOWN #EVENT_TOUCHDROP ... 
	 * 中的一个或者组合。
	 */
	void uiEnableEvent(uiWidget *self, unsigned int e);
	
	/**
	 * \brief 关闭一个事件设置，e可以是 #EVENT_TOUCHDOWN #EVENT_TOUCHDROP ... 
	 * 中的一个或者组合。
	 */	
	void uiDisableEvent(uiWidget *self, unsigned int e);
	
	 /** @} */
#ifdef __cplusplus
}
#endif

#endif