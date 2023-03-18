/**
* @file         SysGraphics.h
* @brief		
* @details	    BootLoader阶段的图形设置函数
* @author		LPH
* @date		    2023-3-3 
* @version	    v1.0
* @par Copyright(c):LPH corporation
* @par History:         
*	    2023-3-3 
*/

#ifndef _SYS_GRAPHIC_H_
#define _SYS_GRAPHIC_H_
#pragma once
#include"BootLoaderConf.h"

/*-----------------------------------------------------------------------------
* 获取系统核心图形显示参数
*------------------------------------------------------------------------------*/
typedef struct GRAPHIC_CONFIG{
    EFI_PHYSICAL_ADDRESS       FrameBufferBase;
    UINTN                      FrameBufferSize;
    UINT32                     HorizontalResolution;
    UINT32                     VerticalResolution;
    UINT32                     PixelsPerScanLine;
} GRAPHIC_CONFIG;

/*-----------------------------------------------------------------------------
* 初始化图形协议(该程序不需要用户进行手动的调用)
* @name: initGraphicProtocol
* @function: 用于初始化图形系统的全局变量g_Gop以方便后续图形操作
* @param : 1.传入系统的ImageHandle参数
* @retValue：返回图形协议是否初始化成功
*------------------------------------------------------------------------------*/
Bool EFIAPI initGraphicProtocol(IN EFI_HANDLE ImageHandle);

/*-----------------------------------------------------------------------------
* 设置显示分辨率(默认为1080P)
* @name: resolutionSetting
* @function: 设置UEFI显示分辨率
* @param : 1.传入系统的ImageHandle参数
           2.获取系统核心图形显示参数
           
* @retValue：返回分辨率设置setMode状态
*------------------------------------------------------------------------------*/
EFI_STATUS EFIAPI resolutionSetting(
    IN EFI_HANDLE ImageHandle,
    OUT GRAPHIC_CONFIG *graphicConfig
);

/*-----------------------------------------------------------------------------
* 将像素信息传递到显卡Framebuffer中
* @name: resolutionSetting
* @function: 进行图形的绘制
* @param : 1.传入像素起始地址
           2.传入图像起点X坐标
           3.传入图像起点Y坐标
           4.传入图像的宽度
           5.传入图像的高度
           
* @retValue：返回像素信息传递的参数结果
*------------------------------------------------------------------------------*/
EFI_STATUS EFIAPI flushPixeltoFrameBuffer(
    IN EFI_PHYSICAL_ADDRESS PixelStart,
    IN UINT32 Xpos,
    IN UINT32 Ypos,
    IN UINT32 PixelWidth,
    IN UINT32 PixelHeight
);


#endif  //_SYS_GRAPHIC_H_