#include"stdio.h"

int 
vsprintf(char *buf, const char *fmt, va_list args)
{
    //for(const char *p = buf;*fmt;++fmt){}
    return 1;
}

static char sprintf_buffer[SPRINTF_BUFFER_SIZE]; 

//int 
//printf(const char *fmt,...)
//{
//    va_list args;
//    int number = vsprintf(sprintf_buffer,fmt,...);
//    console_puts(sprintf_buffer);
//    memset(sprintf_buffer,0,SPRINTF_BUFFER_SIZE);
//    va_end(args);
//    return number;
//}