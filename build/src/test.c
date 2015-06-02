#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


#if defined(__IPHONEOS__) || defined(__ANDROID__)
#include "SDL_opengles.h"
#else
#include "SDL_opengl.h"
#endif

int main(int argc, char argv[])
{
	printf("Hello World");
	return 0;
}