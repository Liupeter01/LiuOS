/**
* @file         console_font.h
* @brief		
* @details	    BootLoader阶段的加载的字体BMP
* @author		LPH
* @date		    2023-3-6 
* @version	    v1.0
* @par Copyright(c):LPH corporation
* @par History:         
*	    2023-3-6 
*/
#ifndef _CONSOLE_FONT_H_
#define _CONSOLE_FONT_H_
#pragma once
#include"console_graphic.h"

/*-----------------------------------------------------------------------------
* 字体设置结构
*------------------------------------------------------------------------------*/
#pragma pack(1)
typedef struct FONT_CONFIG{
    EFI_PHYSICAL_ADDRESS m_fontBuffer;      //FONT的BMP内存映射
    UINT64 m_fontBufferSize;                //FONT的BMP内存大小
    UINT32 m_Width;                         //FONT的宽度
    UINT32 m_Height;                        //FONT的高度
    UINT32 m_fontPixelSize;                 //ASCII的大小(单位PX)
    UINT32 m_fontCharacterOffset;           //ASCII之间的距离(单位PX)         
}__attribute__((packed)) FONT_CONFIG;

/*-----------------------------------------------------------------------------
* 将FONT文件加载到内存中
* @name: loadFonttoMemory
* @function: 将加载到内存中的Font文件加载到内存中
* @param :  1.传入系统的ImageHandle参数
            2.Font文件的打开描述符
            3.传出需要的BMP文件信息表
*------------------------------------------------------------------------------*/
VOID
EFIAPI
loadFonttoMemory(
    IN EFI_HANDLE ImageHandle,
    IN EFI_FILE_DESCRIPTOR *FontFile,
    OUT BMP_STRUCT *FontBmpImage
);

/*-----------------------------------------------------------------------------
* 为系统内核加载字体
* @name: loadKernelFont
* @function: 将加载到内存中的Font文件为参数传递给操作系统
* @param :  1.传入系统的ImageHandle参数
            2.Font文件的文件打开描述符
            3.传出需要的BMP文件信息表
            4.Font文件的配置系统

* @retValue：返回当前内存加载的Font缓冲区(堆内存)，为0时则代表分配失败
*------------------------------------------------------------------------------*/
VOID 
EFIAPI
loadKernelFont(
    IN EFI_HANDLE ImageHandle,
    IN EFI_FILE_DESCRIPTOR *fontFile,
    IN BMP_STRUCT *FontBmpImage,
    OUT FONT_CONFIG * font_config
);

#endif //_CONSOLE_FONT_H_