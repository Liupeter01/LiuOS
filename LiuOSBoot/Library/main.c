#include"console_font.h"
#include"console_logo.h"
#include"kernel_paging.h"

EFI_STATUS
EFIAPI
UefiMain(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable)
{
    /*-----------------------------------------------------------------------------
    * 系统分辨率设置并显示启动LOGO(此部分e缓冲区需要释放!!!!!)
    *------------------------------------------------------------------------------*/  
    EFI_FILE_DESCRIPTOR efiKernelFile;                         //内核文件
    GRAPHIC_CONFIG efiGraphicConfig;                               //图形设置
    FONT_CONFIG font_config;                                     //Font 字体设置
    MEMORY_MAP_CONFIG efiMemoryMap = {
        .m_MemoryMapSize = 4096,              //缓冲区大小
        .m_MemoryMap = NULL,
        .m_MapKey = 0,
        .m_DescriptorSize = 0,             //每一个描述符的大小
        .m_DescriptorVersion = 0            //描述符的版本
    };
    KERNEL_PARAMS_CONFIG kernelParam = {                          //OS内核传参
        .m_graphic = NULL,                           //传入给OS的显示参数设置
        .m_font = NULL,                                    //传入给OS的字体参数设置
        .m_memory =NULL                                 //传入给OS的内存布局
    };

    InitKernelStartup(
        ImageHandle,
        &efiGraphicConfig,
        &font_config,
        &efiKernelFile,
        &kernelParam
    );

    RET_FUNC func =getKernelEntryPoint(
        ImageHandle,
        &efiKernelFile,
        &efiMemoryMap,
        &kernelParam
    );
    //UINT64 retValue = RunKernel(
    //    func,
    //    &kernelParam
    //);
    UINT64 retValue = func();
    //gBS->FreePages(efiFontFile.mEfiFileBuffer,efiFontFile.mEfiFileSize);                //释放字体
    //gBS->FreePages(efiTestKernelFile.mEfiFileBuffer,efiTestKernelFile.mEfiFileSize);    //释放内核
    //gBS->FreePool(efiMemoryMap.m_MemoryMap);                                            //释放内存映射
    Print(L"PassBack RetValue = %d\n" ,retValue);
    return EFI_SUCCESS;
}