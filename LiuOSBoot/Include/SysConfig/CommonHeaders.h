/*
* @file         CommonHeaders.h
* @brief		
* @details	    BootLoader阶段所需的所有基础头文件
* @author		LPH
* @date		    2023-3-3 
* @version	    v1.0
* @par Copyright(c):LPH corporation
* @par History:         
*	    2023-3-3 
*/
#ifndef _COMMON_HEADERS_H_
#define _COMMON_HEADERS_H_
#pragma once

#pragma GCC push_options            //可以将当前的GCC选项暂存。
#pragma GCC optimize (0)            //可以调整当前GCC的优化选项。0就代表O0优化。也支持字符串：#pragma GCC optimize ("O1")
#pragma GCC pop_options             //将之前暂存的GCC编译选项还原出来。

#include<Uefi.h>
#include<Library/BaseLib.h>
#include<Library/BaseMemoryLib.h>
#include<Library/UefiLib.h>
#include<Library/UefiBootServicesTableLib.h>
#include<Library/DebugLib.h>

/*-----------------------------------------------------------------------------
* 基本数据结构别名
*------------------------------------------------------------------------------*/
typedef int Bool;
#endif  //_COMMON_HEADERS_H_