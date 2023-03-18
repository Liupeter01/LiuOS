#ifndef _MM_H_
#define _MM_H_
#pragma once
#include"mm_struct.h"
#include"memory.h"

/*-----------------------------------------------------------------------------
*将字节转换为页面数量
*-----------------------------------------------------------------------------*/
#define CONVERT_BYTES_TO_PAGES(x)  (((x) >> 12) + 1)

/*-----------------------------------------------------------------------------
*将页面数量转换为字节
*-----------------------------------------------------------------------------*/
#define CONVERT_PAGES_TO_BYTES(x)   ((x) << 12)

extern void console_putc(const char ch);
extern void console_puts(const char *str);
extern void console_cls();

/*-----------------------------------------------------------------------------
* 寻找可用空间容纳内存布局信息结构(MM_STRUCT)
* @name: locate_conventional_space
* @function: 在UEFI所传递的内存描述符元素中寻找可以容纳MM_INFORMATION的区域
* @param : 1.读取UEFI传输给操作系统的参数中内存布局部分参数
* @retValue: 返回操作系统内存是否初始化成功
*------------------------------------------------------------------------------*/
Bool 
locate_conventional_space(
    MEMORY_MAP_CONFIG* memory_map
);

/*-----------------------------------------------------------------------------
* 操作系统内部初始化UEFI内存布局模型
* @name: mm_init
* @function: 操作系统内部初始化UEFI内存布局模型
* @param : 1.读取UEFI传输给操作系统的参数中内存布局部分参数
* @retValue: 返回操作系统内存是否初始化成功
*------------------------------------------------------------------------------*/
Bool
mm_init(
    MEMORY_MAP_CONFIG *memory_map
);


#endif //_MM_H_