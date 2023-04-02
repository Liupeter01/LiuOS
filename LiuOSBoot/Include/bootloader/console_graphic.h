/**
* @file         console_graphic.h
* @brief		
* @details	    BootLoader阶段的图形设置函数
* @author		LPH
* @date		    2023-3-3 
* @version	    v1.0
* @par Copyright(c):LPH corporation
* @par History:         
*	    2023-3-3 
*/
#ifndef _CONSOLE_GRAPHIC_H_
#define _CONSOLE_GRAPHIC_H_
#pragma once
#include"boottype.h"
#include"tools.h"

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
* BMP文件信息表(用于进行UEFI的输出工作)
*------------------------------------------------------------------------------*/
#pragma pack(1)
typedef struct BMP_STRUCT{
    UINT32 Size;
    UINT32 PageSize;
    UINT32 Width;
    UINT32 Height;
    UINT32 Offset;
    EFI_PHYSICAL_ADDRESS PixelStart;        //像素起始地址
} __attribute__((packed)) BMP_STRUCT;

/*-----------------------------------------------------------------------------
* 位图的文件头分析(设置字节对齐为1B)
*------------------------------------------------------------------------------*/
#pragma pack(1)
typedef struct tagBITMAPFILEHEADER {
  UINT16  bfType;         //声明位图文件的类型（0X4D42='BM'）
  UINT32  bfSize;          //声明BMP文件的大小
  UINT16  bfReserved1;    //保留字段，必须设置为0
  UINT16  bfReserved2;    //保留字段，必须设置为0
  UINT32 bfOffBits;       //声明从文件头开始到实际的图象数据之间的字节的偏移量
}__attribute__((packed)) BITMAPFILEHEADER; 

/*-----------------------------------------------------------------------------
* 位图的信息头分析(设置字节对齐为1B)
*------------------------------------------------------------------------------*/
#pragma pack(1)
typedef struct tagBITMAPINFOHEADER {
  UINT32 biSize;
  UINT32  biWidth;
  UINT32  biHeight;
  UINT16  biPlanes;
  UINT16  biBitCount;
  UINT32 biCompression;
  UINT32 biSizeImage;
  UINT32  biXPelsPerMeter;
  UINT32  biYPelsPerMeter;
  UINT32 biClrUsed;
  UINT32 biClrImportant;
} __attribute__((packed)) BITMAPINFOHEADER;

/*-----------------------------------------------------------------------------
* 位图颜色模式RGB(设置字节对齐为1B)
*------------------------------------------------------------------------------*/
#pragma pack(1)
typedef struct PIXEL
{
    unsigned char blue;
    unsigned char green;
    unsigned char red;
    unsigned char reserved;
}__attribute__((packed)) PIXEL;

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
#endif  //_CONSOLE_GRAPHIC_H_