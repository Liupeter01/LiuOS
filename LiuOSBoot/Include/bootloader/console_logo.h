/**
* @file         console_logo.h
* @brief		
* @details	    启动画面显示
* @author		LPH
* @date		    2023-3-3 
* @version	    v1.0
* @par Copyright(c):LPH corporation
* @par History:         
*	    2023-3-3 
*/
#ifndef _CONSOLE_LOGO_H_
#define _CONSOLE_LOGO_H_
#pragma once
#include"console_graphic.h"

/*-----------------------------------------------------------------------------
* BMP文件格式转换工具
* @name: transformBMPFormat
* @function: 将BMP进行翻转并转换为可使用的像素数组
* @param :  1.传入读取到的BMP原始二进制文件
            2.传出需要的BMP文件信息表
*------------------------------------------------------------------------------*/
VOID EFIAPI transformBMPFormat(
  IN EFI_FILE_DESCRIPTOR * efiBmpFile,
  OUT BMP_STRUCT *efiStartBmpImage
);

/*-----------------------------------------------------------------------------
* 将加载到内存中的BMP文件输出到UEFI启动界面中
* @name: displayStartupImage
* @function: 设置系统的分辨率，将加载到内存中的BMP文件输出到UEFI启动界面中
* @param :  1.传入系统的ImageHandle参数
            2.传入BMP文件的路径或者名称
            3.获取系统核心图形显示参数

* @retValue：返回系统状态信息
* @update:
        2023-3-10: 优化内存结构，释放内存避免StartUoLogo内存泄漏
*------------------------------------------------------------------------------*/
Bool
EFIAPI
displayStartupImage(
    IN EFI_HANDLE ImageHandle,
    IN CHAR16* filename,
    OUT GRAPHIC_CONFIG *graphicConfig
);

#endif //_CONSOLE_LOGO_H_