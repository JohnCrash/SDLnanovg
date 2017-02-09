#include "debug.h"

#define SELECT2(type,t1,t2) (type==t1?(""#t1):(""#t2))
#define SELECT3(type,t1,t2,t3) (type==t3?(""#t3):SELECT2(type,t1,t2))

void dumpEvent(uiEvent * pev)
{

}

void dumpSDLEvent(SDL_Event * pe)
{
	switch (pe->type){
	//SDL_MouseButtonEvent
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
	{
		SDL_MouseButtonEvent * pb = pe;
		SDL_Log("MouseButton %s %s clicks:%d %d", 
			SELECT2(pb->type, SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP),
			SELECT2(pb->state, SDL_PRESSED, SDL_RELEASED),
			pb->clicks,
			pb->timestamp);
	}break;
	//SDL_MouseMotionEvent
	case SDL_MOUSEMOTION:
	{
		SDL_MouseMotionEvent * pb = pe;
		SDL_Log("MouseMotion (%d,%d) %d", pb->x, pb->y, pb->timestamp);
	}break;
	//SDL_MouseWheelEvent
	case SDL_MOUSEWHEEL:
	{
		SDL_MouseWheelEvent * pb = pe;
		SDL_Log("MouseWheel (%d,%d) %d", pb->x, pb->y, pb->timestamp);
	}break;
	//SDL_TouchFingerEvent
	case SDL_FINGERDOWN:
	case SDL_FINGERUP:
	case SDL_FINGERMOTION:
	{
		SDL_TouchFingerEvent * pb = pe;
		SDL_Log("TouchFinger %s fingerId:%d (%f,%f)-(%f,%f) %d", 
			SELECT3(pb->type, SDL_FINGERMOTION, SDL_FINGERDOWN, SDL_FINGERUP),
			pb->fingerId,
			pb->x, pb->y, 
			pb->dx, pb->dy, 
			pb->timestamp);
	}break;
	//SDL_MultiGestureEvent
	case SDL_MULTIGESTURE:
	{
		SDL_MultiGestureEvent * pb = pe;
		SDL_Log("MultiGesture touchId:%d (%f,%f)-(%f,%f) numFingers:%d %d",pb->touchId,
			pb->x,pb->y,
			pb->dTheta,pb->dDist,
			pb->numFingers,
			pb->timestamp);
	}break;
	//SDL_DollarGestureEvent
	case SDL_DOLLARGESTURE:
	case SDL_DOLLARRECORD:
	{
		SDL_DollarGestureEvent * pb = pe;
		SDL_Log("DollarGesture %s touchId:%d gestureId:%d numFingers:%d (%f,%f) %d", 
			SELECT2(pb->type, SDL_DOLLARGESTURE, SDL_DOLLARRECORD),
			pb->touchId,
			pb->gestureId,pb->numFingers,
			pb->x, pb->y,
			pb->timestamp);
	}break;
	}
}