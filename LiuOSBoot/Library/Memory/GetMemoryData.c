#include"GetMemoryData.h"

/*-----------------------------------------------------------------------------
* 通过GetMemoryMap获取内存布局
* @name: loadMemoryMap
* @function: 将多个内存布局结构存储在结构体中，并退出启动时服务BOOT_SERVICES
* @param :  1.传入系统的ImageHandle参数
            2.传入MEMORY_MAP_CONFIG参数表

* @retValue：返回系统状态信息
*------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI 
loadMemoryMap(
    IN EFI_HANDLE ImageHandle,
    OUT MEMORY_MAP_CONFIG *MemoryMap)
{
    EFI_STATUS status = gBS->AllocatePool(                     //进行EFI_MEMORY_DESCRIPTOR内存的分配
        EfiLoaderData,
        MemoryMap->m_MemoryMapSize,                 //传入EFI_MEMORY_DESCRIPTOR数组的整体大小
        &MemoryMap->m_MemoryMap                     //初始化EFI_MEMORY_DESCRIPTOR结构
    );  
    status = gBS->GetMemoryMap(          
        &MemoryMap->m_MemoryMapSize,                    //缓冲区大小
        (EFI_MEMORY_DESCRIPTOR*)MemoryMap->m_MemoryMap, //EFI_MEMORY_DESCRIPTOR结构
        &MemoryMap->m_MapKey,
        &MemoryMap->m_DescriptorSize,                   //每一个描述符的大小
        &MemoryMap->m_DescriptorVersion                 //描述符的版本
    );
    if(status == EFI_BUFFER_TOO_SMALL){
        gBS->FreePool(MemoryMap->m_MemoryMap );         //释放内存并重新分配
        status = gBS->AllocatePool(                     //进行EFI_MEMORY_DESCRIPTOR内存的分配
            EfiLoaderData,
            MemoryMap->m_MemoryMapSize,                 //传入EFI_MEMORY_DESCRIPTOR数组的整体大小
            &MemoryMap->m_MemoryMap                     //初始化EFI_MEMORY_DESCRIPTOR结构
        );  
        if (EFI_ERROR(status)) {
            Print(L"Memory Map AllocatePool Failed! error code = %d\n",status);
            gBS->FreePool(MemoryMap->m_MemoryMap );     //释放内存
            return status;
        }
        status = gBS->GetMemoryMap(
            &MemoryMap->m_MemoryMapSize,                    //缓冲区大小
            (EFI_MEMORY_DESCRIPTOR*)MemoryMap->m_MemoryMap, //缓冲区起始内存地址
            &MemoryMap->m_MapKey,
            &MemoryMap->m_DescriptorSize,                   //每一个描述符的大小
            &MemoryMap->m_DescriptorVersion                 //描述符的版本
        );
        if (EFI_ERROR(status)) {
            Print(L"Get Memory Map Failed! error code = %d\n",status);
            gBS->FreePool(MemoryMap->m_MemoryMap );     //释放内存
            return status;
        }
    }
    Print(L"MemoryMap Array addr = 0x%x\n",MemoryMap->m_MemoryMap);
    /*----------------------------------------------
    *退出EFI_BOOT_SERVICES服务
    ----------------------------------------------*/
    return gBS->ExitBootServices(ImageHandle,MemoryMap->m_MapKey);
}