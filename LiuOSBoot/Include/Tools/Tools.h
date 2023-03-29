/**
* @file         Tools.h
* @brief		
* @details	    实用工具
* @author		LPH
* @date		    2023-3-3 
* @version	    v1.0
* @par Copyright(c):LPH corporation
* @par History:         
*	    2023-3-3 
*/
#ifndef _TOOLS_H_
#define _TOOLS_H_
#pragma once
#include "boottype.h"

/*-----------------------------------------------------------------------------
* /将EFI文件FILE的大小和存储指针封装
* @2023-2-21：修改原先mEfiFileBuffer的数据类型，避免在分配内存出错
*------------------------------------------------------------------------------*/
typedef struct EFI_FILE_DESCRIPTOR          //将EFI文件FILE的大小和存储指针结合
{
    EFI_FILE_PROTOCOL *mFileInstance;      //文件指针实例
    EFI_FILE_INFO * mEfiFileInfo;           //从文件实例获得的文件信息
    UINTN mEfiInfoSize;                     //EFI_FILE_INFO结构体的大小
    UINTN mEfiFileSize;                     //实际存储文件的大小
    EFI_PHYSICAL_ADDRESS mEfiFileBuffer;     //存储根据FILE_INFO读取的信息缓冲地址
}EFI_FILE_DESCRIPTOR;

/*-----------------------------------------------------------------------------
* 通用工具函数
*------------------------------------------------------------------------------*/
VOID *memcpy(
    VOID * __restrict s1, 
    const VOID * __restrict s2, 
    UINT64 n
);

VOID *memset(
    void *s,int c,UINT64 n
);

/*-----------------------------------------------------------------------------
* BMP行像素颠倒工具
* @name: linePixelSwap
* @function: 传入BMP中的两行进行像素的互换
* @param :  1.传入源
            2.传入目标
            3.每行的大小
*------------------------------------------------------------------------------*/
VOID EFIAPI linePixelSwap(
  IN OUT VOID *src,
  IN OUT VOID *dst,
  IN UINTN lineSize
);

/*-----------------------------------------------------------------------------
* 初始化文件协议(该程序不需要用户进行手动的调用)
* @name: initFileProtocol
* @function: 用于初始化文件系统的全局变量g_efiSimpleFileSystem以方便后续文件操作
* @param : 1.传入系统的ImageHandle参数
* @retValue：返回文件协议是否初始化成功
*------------------------------------------------------------------------------*/
Bool EFIAPI initFileProtocol(IN EFI_HANDLE ImageHandle);

/*-----------------------------------------------------------------------------
* 获取文件的句柄(句柄实例采用定制化集成的结构进行参数传递)
* @name: getFileHandle
* @function: 根据文件名称获取文件句柄
* @param : 1.传入系统的ImageHandle参数
           2.传入文件名称
           3.传入欲打开的文件句柄的自定义数据结构

* @retValue：返回根据文件名打开文件句柄是否成功
*------------------------------------------------------------------------------*/
Bool EFIAPI getFileHandle(
    IN EFI_HANDLE ImageHandle,
    IN CHAR16 *fileName,                            //文件路径或名称
    OUT EFI_FILE_DESCRIPTOR *fileInstance
);

/*-----------------------------------------------------------------------------
* 根据文件句柄实例将文件数据拷贝到自定义文件描述符的缓冲区中
* @name: mapFiletoMemory
* @function: 根据文件句柄将文件内容映射内存
* @param : 1.传入欲打开的文件句柄的自定义数据结构
* @retValue：返回文件读取到内存的状态
* @update/modify:
    LPH 2023-2-23:修复了EFI_FILE_INFO长度的计算方法，修改原先的硬编码逻辑
    
*------------------------------------------------------------------------------*/
EFI_STATUS EFIAPI mapFiletoMemory(
    IN OUT EFI_FILE_DESCRIPTOR *fileInstance   //外部传递文件的存储结构
);  

#endif //_TOOLS_H_