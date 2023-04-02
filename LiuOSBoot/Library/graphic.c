/**
* @file         graphic.c
* @brief		
* @details	    BootLoader阶段的图形设置函数
* @author		LPH
* @date		    2023-3-3 
* @version	    v1.0
* @par Copyright(c):LPH corporation
* @par History:         
*	    2023-3-3 
*/
#include"console_font.h"
#include"console_logo.h"

static Bool isGraphicProtocolInit = FALSE;      //标识位代表协议已经初始化
EFI_GRAPHICS_OUTPUT_PROTOCOL *g_Gop;            //图形系统协议的全局变量

/*-----------------------------------------------------------------------------
* 初始化图形协议(该程序不需要用户进行手动的调用)
* @name: initGraphicProtocol
* @function: 用于初始化图形系统的全局变量g_Gop以方便后续图形操作
* @param : 1.传入系统的ImageHandle参数
* @retValue：返回图形协议是否初始化成功
*------------------------------------------------------------------------------*/
Bool
EFIAPI
initGraphicProtocol(IN EFI_HANDLE ImageHandle)
{
    if(!isGraphicProtocolInit){
        UINTN mHandleCount = 0;
        EFI_HANDLE * mBuffer = NULL;
	    EFI_STATUS status = gBS->LocateHandleBuffer(
		    ByProtocol,
            &gEfiGraphicsOutputProtocolGuid,
            NULL,
            &mHandleCount,
            &mBuffer             
        );
        if(EFI_ERROR(status)){
            Print(L"Failed to LocateHandleBuffer\nStatus = %d.\n",status);
            return FALSE;
        }
        status = gBS->OpenProtocol(
            mBuffer[0],
            &gEfiGraphicsOutputProtocolGuid,
            (VOID**)&g_Gop,
            ImageHandle,
            NULL,
            EFI_OPEN_PROTOCOL_GET_PROTOCOL
        );
        if(EFI_ERROR(status)){
            Print(L"Failed to OpenProtocol\nStatus = %d.\n",status);
            return FALSE;
        }
        isGraphicProtocolInit = TRUE;
    }
    return TRUE;
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
    IN UINT32 PixelHeight)
{
    return g_Gop->Blt(                                                 //输出图像
        g_Gop,
        (EFI_GRAPHICS_OUTPUT_BLT_PIXEL*)PixelStart,   //像素起始地址
        EfiBltBufferToVideo,
        0,0,
        Xpos,Ypos,
        PixelWidth,PixelHeight,
        0
    );
}

/*-----------------------------------------------------------------------------
* 设置显示分辨率
* @name: resolutionSetting
* @function: 设置UEFI显示分辨率
* @param : 1.传入系统的ImageHandle参数
           2.获取系统核心图形显示参数
           
* @retValue：返回分辨率设置setMode状态
*------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
resolutionSetting(
    IN EFI_HANDLE ImageHandle,
    OUT GRAPHIC_CONFIG *graphicConfig)
{
    EFI_STATUS status = EFI_SUCCESS;
    if(initGraphicProtocol(ImageHandle))
    {
        Print(L"Graphic Init Finished!!!\n");
        UINTN sizeOfInfo = 0;
        EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *graphicOuputInfo;
        for(UINTN i = 0; i < g_Gop->Mode->MaxMode; i++){
            status = g_Gop->QueryMode(g_Gop,i, &sizeOfInfo,&graphicOuputInfo); //引入Protocol实例，传入以MaxMode为循环的显示模式序号 
            if(EFI_ERROR(status)){
                Print(L"Failed to QueryMode\nStatus = %d.\n",status);
                return status;
            }
            Print(L"HorizontalResolution = %X   \
                VerticalResolution = %X\n",
                graphicOuputInfo->HorizontalResolution,
                graphicOuputInfo->VerticalResolution
            );
            
            if(graphicOuputInfo->HorizontalResolution == HORIZONTAL_RESOLUTION &&    //设置指定的分辨率
               graphicOuputInfo->VerticalResolution == VERTICAL_RESOLUTION){ 
                status = g_Gop->SetMode(g_Gop, i);
                if(EFI_ERROR(status)){
                    Print(L"Failed to Set Resolution\nStatus = %d.\n",status);
                    return status;
                }  
                break;  
            }
        }
        graphicConfig->FrameBufferBase = g_Gop->Mode->FrameBufferBase;
        graphicConfig->FrameBufferSize = g_Gop->Mode->FrameBufferSize;
        graphicConfig->HorizontalResolution = g_Gop->Mode->Info->HorizontalResolution;
        graphicConfig->VerticalResolution = g_Gop->Mode->Info->VerticalResolution;
        graphicConfig->PixelsPerScanLine = g_Gop->Mode->Info->PixelsPerScanLine; 
        Print(L"FrameBufferBase = %X\n \
            FrameBufferSize = %X\n \
            HorizontalResolution = %d\n \
            VerticalResolution = %d\n \
            PixelsPerScanLine = %d\n",
            graphicConfig->FrameBufferBase,
            graphicConfig->FrameBufferSize,
            graphicConfig->HorizontalResolution,
            graphicConfig->VerticalResolution,
            graphicConfig->PixelsPerScanLine 
        );
    }                                       
    return status;
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

/*-----------------------------------------------------------------------------
* 将FONT文件加载到内存中
* @name: loadFonttoMemory
* @function: 将加载到内存中的Font文件加载到内存中
* @param :  1.传入系统的ImageHandle参数
            2.Font文件的打开描述符
            3.传出需要的BMP文件信息表
* @update:
            2023-3-10: 修复错误的EFI_ERROR判断逻辑
*------------------------------------------------------------------------------*/
VOID
EFIAPI
loadFonttoMemory(
    IN EFI_HANDLE ImageHandle,
    IN EFI_FILE_DESCRIPTOR *FontFile,
    OUT BMP_STRUCT *FontBmpImage)
{
    Bool flag = getFileHandle(ImageHandle,FONT_IMAGE_PATH,FontFile);  //获取字体文件句柄成功
    if(!flag){                                                        //是否失败
        Print(L"Get Font File Error!\n");
        return;
    }
    EFI_STATUS status = mapFiletoMemory(FontFile);                   //内存映射字体文件ASCII
    if(EFI_ERROR (status)){                                        //加载字体文件入内存失败
        Print(L"Memory Font File Error!\n");
        return;
    }
    transformBMPFormat(FontFile,FontBmpImage);                       //提取可用的BMP像素 
    return ;                                                          //加载失败
}

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
    OUT FONT_CONFIG * font_config)
{
    loadFonttoMemory(
        ImageHandle,
        fontFile,                                   //FONT文件打开描述符
        FontBmpImage
    );
    font_config->m_fontBuffer = FontBmpImage->PixelStart;      //FONT的BMP内存映射
    font_config->m_fontBufferSize = FontBmpImage->Size;                //FONT的BMP内存大小
    font_config->m_fontPixelSize = FONT_PIXEL_SIZE;  //ASCII的大小(单位PX)
    font_config->m_fontCharacterOffset = FONT_CHARACTER_OFFSET; //ASCII之间的距离(单位PX)
    font_config->m_Width = FontBmpImage->Width;                
    font_config->m_Height = FontBmpImage->Height;               
    Print(L"font_config->m_Width  = %d\n \
           font_config->m_Height = %d\n",
        font_config->m_Width,
        font_config->m_Height
    );
}