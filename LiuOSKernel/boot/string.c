#include"string.h"

void *
memset(
	void *s,
	int c,
	size_t n)
{
	char *xs = s;
	while (n--)
		*xs++ = c;
	return s;
}

int 
memcmp(
    const void *cs, 
    const void *ct, 
    size_t count)
{
	const unsigned char *su1 = cs, *su2 = ct, *end = su1 + count;
	int res = 0;

	while (su1 < end) {
		res = *su1++ - *su2++;
		if (res)
			break;
	}
	return res;
}

char * 
strcpy(
	char *dst,
	const char *src)
{
	char *tmp = dst;

	while ((*dst++ = *src++) != '\0')
		/* nothing */;
	return tmp;
}

char *
strcat(
	char *dest, 
	const char *src)
{
	char *tmp = dest;
	while (*dest)
		dest++;
	while ((*dest++ = *src++) != '\0')
		;
	return tmp;
}

int
strcmp(
    const char *str1, 
    const char *str2)
{
	const unsigned char *s1 = (const unsigned char *)str1;
	const unsigned char *s2 = (const unsigned char *)str2;
	int delta = 0;

	while (*s1 || *s2) {
		delta = *s1 - *s2;
		if (delta)
			return delta;
		s1++;
		s2++;
	}
	return 0;
}

int 
strncmp(
    const char *cs, 
    const char *ct, 
    size_t count)
{
	unsigned char c1, c2;

	while (count) {
		c1 = *cs++;
		c2 = *ct++;
		if (c1 != c2)
			return c1 < c2 ? -1 : 1;
		if (!c1)
			break;
		count--;
	}
	return 0;
}

size_t 
strlen(const char *s)
{
	const char *sc = s;

	while (*sc != '\0')
		sc++;
	return sc - s;
}

char *
strstr(
    const char *s1, 
    const char *s2)
{
	size_t l1, l2;

	l2 = strlen(s2);
	if (!l2)
		return (char *)s1;
	l1 = strlen(s1);
	while (l1 >= l2) {
		l1--;
		if (!memcmp(s1, s2, l2))
			return (char *)s1;
		s1++;
	}
	return NULL;
}

char *
strchr(
    const char *s, 
    int c)
{
	while (*s != (char)c)
		if (*s++ == '\0')
			return NULL;
	return (char *)s;
}

size_t 
strnlen(
    const char *s, 
    size_t count)
{
	const char *sc;

	for (sc = s; count-- && *sc != '\0'; ++sc)
		/* nothing */;
	return sc - s;
}