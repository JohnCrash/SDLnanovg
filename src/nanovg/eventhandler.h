#ifndef _EVENTHANDLER_H_
#define _EVENTHANDLER_H_

enum{
	MOUSE_DOWN = 1,
	MOUSE_UP = 2,
	MOUSE_MOVE = 3,
	MOUSE_SCROLL = 4,
	TOUCH_BEGAN = 5,
	TOUCH_MOVED = 6,
	TOUCH_ENDED = 7,
	TOUCH_CHANCELLED = 8,
	KEY_DOWN = 9,
	KEY_UP = 10,
};

typedef struct
{
	int type;
	union {
		int key;
		struct{
			float x, y, scroll_x, scroll_y;
			int button;
		};
		struct{
			float touch_x[6];
			float touch_y[6];
			int ntouch;
		};
	};
} inputEvent;

int eventLoop(SDLState *state);

#endif