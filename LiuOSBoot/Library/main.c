#include"StartLogo.h"
#include"KernelLoader.h"
#include"GeneralFileLoader.h"
#include"FontLoader.h"
#include"GetMemoryData.h"

EFI_STATUS
EFIAPI
UefiMain(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable)
{
    /*-----------------------------------------------------------------------------
    * 系统分辨率设置并显示启动LOGO(此部分e缓冲区需要释放!!!!!)
    *------------------------------------------------------------------------------*/  
    GRAPHIC_CONFIG efiGraphicConfig;                               //图形设置
    displayStartupImage(                                           //显示开启LOGO函数              
        ImageHandle,
        STARTUP_IMAGE_PATH,                                         //BMP文件路径
        &efiGraphicConfig                                          //图形设置数据结构
    );

    /*-----------------------------------------------------------------------------
    * Font.bmp字体文件的加载(此部分的efiFontFile缓冲区严禁释放!!!!!)
    *------------------------------------------------------------------------------*/  
    EFI_FILE_DESCRIPTOR efiFontFile;                             //Font BMP文件的打开描述符
    BMP_STRUCT efiFontBmp;                                       //Font BMP文件结构
    FONT_CONFIG font_config;                                     //Font 字体设置
    loadKernelFont(
        ImageHandle,
        &efiFontFile,
        &efiFontBmp,
        &font_config
    );

    /*-----------------------------------------------------------------------------
    * 内存布局的获取
    *------------------------------------------------------------------------------*/   
    MEMORY_MAP_CONFIG efiMemoryMap = {
        .m_MemoryMapSize = 4096,              //缓冲区大小
        .m_MemoryMap = NULL,
        .m_MapKey = 0,
        .m_DescriptorSize = 0,             //每一个描述符的大小
        .m_DescriptorVersion = 0            //描述符的版本
    };
    loadMemoryMap(ImageHandle,&efiMemoryMap);                   //获取内存布局
    Print(L"DescriptorSize = %d\n",efiMemoryMap.m_DescriptorSize);
    Print(L"sizeof(EFI_MEMORY_DESCRIPTOR) = %d\n",sizeof(EFI_MEMORY_DESCRIPTOR));

    /*-----------------------------------------------------------------------------
    * Kernel.elf操作系统加载(此部分的efiTestKernelFile缓冲区严禁释放!!!!!)
    *------------------------------------------------------------------------------*/  
    EFI_FILE_DESCRIPTOR efiTestKernelFile;                         //内核文件
    EFI_PHYSICAL_ADDRESS relocateKernelBase;                       //重定位地址
    RET_FUNC func = getKernelEntryPoint(ImageHandle,&efiTestKernelFile,&relocateKernelBase);

    KERNEL_PARAMS_CONFIG kernelParam = {                          //OS内核传参
        .m_graphic = &efiGraphicConfig,                           //传入给OS的显示参数设置
        .m_font = &font_config,                                    //传入给OS的字体参数设置
        .m_memory = &efiMemoryMap                                 //传入给OS的内存布局
    };
    UINT64 retValue = func(&kernelParam);
    gBS->FreePages(efiFontFile.mEfiFileBuffer,efiFontFile.mEfiFileSize);                //释放字体
    gBS->FreePages(efiTestKernelFile.mEfiFileBuffer,efiTestKernelFile.mEfiFileSize);    //释放内核
    gBS->FreePool(efiMemoryMap.m_MemoryMap);                                            //释放内存映射
    Print(L"PassBack RetValue = %d\n" ,retValue);
    return EFI_SUCCESS;
}