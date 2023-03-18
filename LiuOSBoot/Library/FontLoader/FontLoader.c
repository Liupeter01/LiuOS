#include "FontLoader.h"

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