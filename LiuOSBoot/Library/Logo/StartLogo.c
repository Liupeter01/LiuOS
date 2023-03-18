/**
* @file         StartLogo.c
* @brief		
* @details	    启动画面显示
* @author		LPH
* @date		    2023-3-3 
* @version	    v1.0
* @par Copyright(c):LPH corporation
* @par History:         
*	    2023-3-3 
*/

#include"StartLogo.h"

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
  IN UINTN lineSize)
{
    const UINTN loopCnt = lineSize / sizeof(UINTN);
    const UINTN remainder = lineSize % sizeof(UINTN);

    UINTN swapBuf1 = 0;
    UINTN* buf1_UINTN_ptr = (UINTN *)src;
    UINTN* buf2_UINTN_ptr = (UINTN *)dst;
    for (UINTN i = 0; i < loopCnt; i++) {
        swapBuf1 = buf1_UINTN_ptr[i];
        buf1_UINTN_ptr[i] = buf2_UINTN_ptr[i];
        buf2_UINTN_ptr[i] = swapBuf1;
    }

    UINT8 swapBuf2 = 0;
    UINT8* buf1_UINT8_ptr = (UINT8 *)src + (lineSize - remainder);
    UINT8* buf2_UINT8_ptr = (UINT8 *)dst + (lineSize - remainder);
    for (UINTN i = 0; i < remainder; i++) {
        swapBuf2 = buf1_UINT8_ptr[i];
        buf1_UINT8_ptr[i] = buf2_UINT8_ptr[i];
        buf2_UINT8_ptr[i] = swapBuf2;
    }
}

/*-----------------------------------------------------------------------------
* BMP文件格式转换工具
* @name: transformBMPFormat
* @function: 将BMP进行翻转并转换为可使用的像素数组
* @param :  1.传入读取到的BMP原始二进制文件
            2.传出需要的BMP文件信息表
*------------------------------------------------------------------------------*/
VOID EFIAPI transformBMPFormat(
  IN EFI_FILE_DESCRIPTOR * efiBmpFile,
  OUT BMP_STRUCT *efiStartBmpImage)
{
    BITMAPFILEHEADER *bitmap_header = (BITMAPFILEHEADER *)efiBmpFile->mEfiFileBuffer;   //读取位图的文件头(14B)
    efiStartBmpImage->Size = bitmap_header->bfSize;                            //计算大小
    efiStartBmpImage->PageSize = (efiStartBmpImage->Size >> 12) + 1;        //计算页面数量
    efiStartBmpImage->Offset = bitmap_header->bfOffBits;                      //偏移参数

    BITMAPINFOHEADER *bitmap_infoheader = (BITMAPINFOHEADER*)(bitmap_header + 1);      //偏移到信息头
    efiStartBmpImage->Width = bitmap_infoheader->biWidth;
    efiStartBmpImage->Height = bitmap_infoheader->biHeight;

    efiStartBmpImage->PixelStart = efiBmpFile->mEfiFileBuffer + efiStartBmpImage->Offset;

    Print(L"BmpImage->Size = %d\n \
            BmpImage->Offset = %d\n \
            Image->Width = %d\n \
            Image->Height = %d\n \
            BmpImage->PixelStart = %p\n",
        efiStartBmpImage->Size,
        efiStartBmpImage->Offset,
        efiStartBmpImage->Width,
        efiStartBmpImage->Height,
        efiStartBmpImage->PixelStart
    );

    EFI_GRAPHICS_OUTPUT_BLT_PIXEL *PixelBuffer =
         (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)efiStartBmpImage->PixelStart;     //拷贝位置
    
    const UINTN Height = efiStartBmpImage->Height;
    const UINTN Width  = efiStartBmpImage->Width;
    for(UINTN i = 0 ; i < Height / 2 ; ++i){
        linePixelSwap(
            PixelBuffer + i * Width, 
            PixelBuffer + (Height - i - 1) * Width, 
            Width * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
        );
    }
}

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
    OUT GRAPHIC_CONFIG *graphicConfig)
{
    EFI_FILE_DESCRIPTOR m_StartUpLogoFile;                               //bmp文件内存映射加载
    BMP_STRUCT m_StartUpLogoPixel;                                       //bmp的有效像素地址
    /*----------------------------------------------------------
     *设置系统的分辨率并更新配置结构
     *----------------------------------------------------------*/
    EFI_STATUS status = resolutionSetting(                         //设置系统分辨率
        ImageHandle,
        graphicConfig                                          //图形设置数据结构
    );
    if(EFI_ERROR (status)){
        Print(L"Resolution Setting Error! error code = %d",status);
        return status;
    }
    /*----------------------------------------------------------
     *获取BMP文件的句柄
     *----------------------------------------------------------*/
    Bool flag = getFileHandle(ImageHandle,filename,&m_StartUpLogoFile);  //获取BMP文件句柄
    if(!flag){                                                         //获取和打开BMP文件失败
        Print(L"StartUp Logo Loading Failed!\n");
        return FALSE;
    }
    /*----------------------------------------------------------
     *将BMP文件以ASCII方式映射到内存，并进行有效像素地址的提取
     *----------------------------------------------------------*/
    status = mapFiletoMemory(&m_StartUpLogoFile);                       //将文件映射到内存中
    if(EFI_ERROR(status)){
        Print(L"Failed to mapFiletoMemory! error code = %d.\n",status);
        return FALSE;
    }
    transformBMPFormat(&m_StartUpLogoFile,&m_StartUpLogoPixel);         //将BMP进行翻转并转换为可使用的像素数组
    status = flushPixeltoFrameBuffer(                                   //将经过处理的BMP像素输出到Framebuffer
        m_StartUpLogoPixel.PixelStart,                                   //像素起始地址
        (HORIZONTAL_RESOLUTION / 2) - (m_StartUpLogoPixel.Width / 2),
        (VERTICAL_RESOLUTION / 2) - (m_StartUpLogoPixel.Height / 2) - STARTUP_IMAGE_OFFSET,
        m_StartUpLogoPixel.Width,
        m_StartUpLogoPixel.Height
    );
    if(EFI_ERROR(status))    {
        Print(L"Failed to Display StartUpImage\nStatus = %d.\n",status);
        return FALSE;
    }
    /*----------------------------------------------------------
     *将临时使用的启动LOGO图标缓冲区进行释放
     *----------------------------------------------------------*/
    gBS->FreePages(m_StartUpLogoFile.mEfiFileBuffer,m_StartUpLogoFile.mEfiFileSize);
    Print(L"StartUp Logo Loading Success!\n");
    return TRUE;
}