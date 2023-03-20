/**
* @file         GetMemoryData.h
* @brief		
* @details	    
* @author		LPH
* @date		    2023-3-9
* @version	    v1.0
* @par Copyright(c):LPH corporation
* @par History:         
*	    2023-3-9 
*/
#ifndef _GET_MEMORY_DATA_H_
#define _GET_MEMORY_DATA_H_
#pragma once
#include"CommonHeaders.h"

/*-----------------------------------------------------------------------------
* 内存布局模型结构体(1B)
*------------------------------------------------------------------------------*/
#pragma pack(1)
typedef struct MEMORY_MAP_CONFIG{
    UINTN m_MemoryMapSize;              //缓冲区大小
    //指针指向缓冲的起始内存地址
    //可通过AllocatePool开辟内存(其中含有多个内存描述符结构体)
    VOID *m_MemoryMap; 
    UINTN m_MapKey;
    UINTN m_DescriptorSize;             //每一个描述符的大小
    UINT32 m_DescriptorVersion;         //描述符的版本
} __attribute__((__aligned__(1)))  MEMORY_MAP_CONFIG;

/*-----------------------------------------------------------------------------
* 通过GetMemoryMap获取内存布局
* @name: loadMemoryMap
* @function: 将多个内存布局结构存储在结构体中，并退出启动时服务BOOT_SERVICES
* @param :  1.传入系统的ImageHandle参数
            2.传入MEMORY_MAP_CONFIG参数表

* @retValue：返回系统状态信息
*------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI 
loadMemoryMap(
    IN EFI_HANDLE ImageHandle,
    OUT MEMORY_MAP_CONFIG *MemoryMap
);
#endif //_GET_MEMORY_DATA_H_