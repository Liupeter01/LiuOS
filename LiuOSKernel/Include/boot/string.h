#ifndef _STRING_H_
#define _STRING_H_
#pragma once
#include"type.h"

typedef unsigned long size_t;

extern void *memset(void *s,int c,size_t n);
extern int memcmp(const void *cs, const void *ct, size_t count);
extern char * strcpy(char *dst,const char *src);
extern char * strcat(char *dest,const char *src);
extern int strcmp(const char *str1, const char *str2);
extern int strncmp(const char *cs, const char *ct, size_t count);
extern size_t strlen(const char *s);
extern char *strstr(const char *s1, const char *s2);
extern char *strchr(const char *s, int c);
extern size_t strnlen(const char *s, size_t maxlen);

#endif //_STRING_H_