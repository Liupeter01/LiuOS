#ifndef _STDIO_H_
#define _STDIO_H_
#pragma once
#include "stdarg.h"

extern void console_puts(const char *str);

#define SPRINTF_BUFFER_SIZE 1024
int vsprintf(char *buf, const char *fmt, va_list args);
int printf(const char *fmt,...);
#endif //_STDIO_H_