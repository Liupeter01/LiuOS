/**
* @file         SysGraphics.c
* @brief		
* @details	    BootLoader阶段的图形设置函数
* @author		LPH
* @date		    2023-3-3 
* @version	    v1.0
* @par Copyright(c):LPH corporation
* @par History:         
*	    2023-3-3 
*/
#include"SysGraphics.h"

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