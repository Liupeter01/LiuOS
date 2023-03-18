/**
* @file         StartLogo.h
* @brief		
* @details	    启动画面显示
* @author		LPH
* @date		    2023-3-3 
* @version	    v1.0
* @par Copyright(c):LPH corporation
* @par History:         
*	    2023-3-3 
*/

#ifndef _STARTUP_LOGO_H_
#define _STARTUP_LOGO_H_
#pragma once
#include"SysGraphics.h"
#include"GeneralFileLoader.h"

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
#endif	//_STARTUP_LOGO_H_