#ifndef __UTF8_H__
#define __UTF8_H__
#ifdef __cplusplus
extern "C"{
#endif
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
long cc_utf8_strlen(const char * p, int max);

char *cc_utf8_next(char *p);
#ifdef __cplusplus
}
#endif
#endif