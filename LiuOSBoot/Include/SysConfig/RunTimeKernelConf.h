/**
* @file         RunTimeKernelConf.h
* @brief		
* @details	    为加载的Kernel文件进行参数传递
* @author		LPH
* @date		    2023-3-3 
* @version	    v1.0
* @par Copyright(c):LPH corporation
* @par History:         
*	    2023-3-3 
*/
#ifndef _RT_KERNEL_CONF_H_
#define _RT_KERNEL_CONF_H_
#pragma once
#include"SysGraphics.h" //图形库
#include"FontLoader.h"  //系统加载字体ASCII
#include"GetMemoryData.h"   //内存布局参数

#pragma pack(1)
typedef struct KERNEL_PARAMS_CONFIG{
    GRAPHIC_CONFIG * m_graphic;         //显示参数
    FONT_CONFIG * m_font;
    MEMORY_MAP_CONFIG * m_memory;           //内存布局
} __attribute__((__aligned__(1))) KERNEL_PARAMS_CONFIG;

/*-----------------------------------------------------------------------------
* 基本内核参数传递数据结构
*------------------------------------------------------------------------------*/
//typedef int(*RET_FUNC)();
typedef UINT64(*RET_FUNC)(KERNEL_PARAMS_CONFIG*);
#endif //_RT_KERNEL_CONF_H_