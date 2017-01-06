#include "utf8.h"
#include <stddef.h>

/*
 * 文件来自于cocos2d-x pluginxUTF8.cpp
 */
static const char utf8_skip_data[256] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5,
	5, 5, 5, 6, 6, 1, 1
};

static const char *const g_utf8_skip = utf8_skip_data;
#define cc_utf8_next_char(p) (const char *)((p) + g_utf8_skip[*(unsigned char *)(p)])

const char *cc_utf8_next(const char *p)
{
	return cc_utf8_next_char(p);
}
/*
* cc_utf8_strlen:
* @p: pointer to the start of a UTF-8 encoded string.
* @max: the maximum number of bytes to examine. If @max
*       is less than 0, then the string is assumed to be
*       null-terminated. If @max is 0, @p will not be examined and
*       may be %NULL.
*
* Returns the length of the string in characters.
*
* Return value: the length of the string in characters
**/
long
cc_utf8_strlen(const char * p, int max)
{
	long len = 0;
	const char *start = p;

	if (!(p != NULL || max == 0))
	{
		return 0;
	}

	if (max < 0)
	{
		while (*p)
		{
			p = cc_utf8_next_char(p);
			++len;
		}
	}
	else
	{
		if (max == 0 || !*p)
			return 0;

		p = cc_utf8_next_char(p);

		while (p - start < max && *p)
		{
			++len;
			p = cc_utf8_next_char(p);
		}

		/* only do the last len increment if we got a complete
		* char (don't count partial chars)
		*/
		if (p - start == max)
			++len;
	}

	return len;
}