/**
* @file         GeneralFileLoader.c
* @brief		
* @details	    BootLoader阶段的通用文件加载
* @author		LPH
* @date		    2023-3-3 
* @version	    v1.0
* @par Copyright(c):LPH corporation
* @par History:         
*	    2023-3-3 
*/

#include"GeneralFileLoader.h"

/*---------------------------Gobal variables-------------------------------*/
static Bool isFileProtocolInit = FALSE;                 //标识位代表协议已经初始化
EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *g_efiSimpleFileSystem; //文件系统协议的全局变量
/*----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
* 初始化文件协议(该程序不需要用户进行手动的调用)
* @name: initFileProtocol
* @function: 用于初始化文件系统的全局变量g_efiSimpleFileSystem以方便后续文件操作
* @param : 1.传入系统的ImageHandle参数
* @retValue：返回文件协议是否初始化成功
*------------------------------------------------------------------------------*/
Bool
EFIAPI
initFileProtocol(IN EFI_HANDLE ImageHandle)
{
    if(!isFileProtocolInit){
        UINTN mNoHandles = 0;
        EFI_HANDLE *mFileSystem = NULL;                        //设备HANDLE指针数组
        EFI_STATUS status = gBS->LocateHandleBuffer(           //根据文件系统协议GUID定位Driver
            ByProtocol, &gEfiSimpleFileSystemProtocolGuid, NULL, &mNoHandles,&mFileSystem                        
        );
        if(EFI_ERROR(status)){
            Print(L"Failed to Locate File System Protocol\nStatus = %d.\n",status);
            return FALSE;
        }
        status = gBS->OpenProtocol(                                         //在硬件设备上加载Driver
            mFileSystem[0],                                                //设备HANDLE 
            &gEfiSimpleFileSystemProtocolGuid,
            (VOID**)&g_efiSimpleFileSystem,                                 //文件系统协议Driver
            ImageHandle,                                                    //ImageHandle
            NULL,
            EFI_OPEN_PROTOCOL_GET_PROTOCOL
        );
        if(EFI_ERROR(status)){
            Print(L"Failed to Open File System Protocol\nStatus = %d.\n",status);
            return FALSE;
        }
        isFileProtocolInit = TRUE;                           //文件系统协议已经初始化
    }
    return TRUE;
}

/*-----------------------------------------------------------------------------
* 获取文件的句柄(句柄实例采用定制化集成的结构进行参数传递)
* @name: getFileHandle
* @function: 根据文件名称获取文件句柄
* @param : 1.传入系统的ImageHandle参数
           2.传入文件名称
           3.传入欲打开的文件句柄的自定义数据结构

* @retValue：返回根据文件名打开文件句柄是否成功
*------------------------------------------------------------------------------*/
Bool
EFIAPI
getFileHandle(
    IN EFI_HANDLE ImageHandle,
    IN CHAR16 *fileName,                            //文件路径或名称
    OUT EFI_FILE_DESCRIPTOR *fileInstance)
{
    EFI_STATUS status = 0;
    if(initFileProtocol(ImageHandle)){              //判断是否启动文件系统协议的全局变量
        EFI_FILE_PROTOCOL * efiFileTemp;
        status = g_efiSimpleFileSystem->OpenVolume(  //打开文件卷并获取传出参数
            g_efiSimpleFileSystem,
            &efiFileTemp
        );
        if(!EFI_ERROR(status)){
            status = efiFileTemp->Open(             //create a new opened efi_file_protocol Instance
                efiFileTemp,
                &fileInstance->mFileInstance,
                fileName,
                EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
                EFI_OPEN_PROTOCOL_GET_PROTOCOL
            );
            if(EFI_ERROR(status)){
                Print(L"OPEN FILE FAILED!!! Status = %d.\n",status);
                return FALSE;
            }
        }
        else{
            Print(L"VOLUME OPEN FAILED!!! Status = %d.\n",status);
            return FALSE;
        }
    }
    else{
        Print(L"INIT FILESYSTEM FAILED!!! Status = %d.\n",status);
        return FALSE; 
    }
    return TRUE; 
}

/*-----------------------------------------------------------------------------
* 根据文件句柄实例将文件数据拷贝到自定义文件描述符的缓冲区中
* @name: mapFiletoMemory
* @function: 根据文件句柄将文件内容映射内存
* @param : 1.传入欲打开的文件句柄的自定义数据结构
* @retValue：返回文件读取到内存的状态
* @update/modify:
    @LPH 2023-2-23:修复了EFI_FILE_INFO长度的计算方法，修改原先的硬编码逻辑
    @LPH 2023-2-25: 1.增加了对于文件的关闭
                    2.修复内存泄露的问题
    
*------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
mapFiletoMemory(IN OUT EFI_FILE_DESCRIPTOR *fileInstance)   //外部传递文件的存储结构
{
    /*当前步骤仅仅启动获取EFI_FILE_INFO长度的功能，不填充缓冲区*/
    EFI_STATUS status = fileInstance->mFileInstance->GetInfo(//首先获取EFI_FILE_INFO的长度
        fileInstance->mFileInstance,                        //EFI_FILE协议的实例
        &gEfiFileInfoGuid,                                  //传入GUID
        &fileInstance->mEfiInfoSize,                        //buffer参数中欲存储EFI_FILE_INFO结构体大小
        NULL                                               //不传入缓冲区地址，仅仅用于获取EFI_FILE_INFO结构体长度
    );
    gBS->AllocatePool(                         //进行EFI_FILE_INFO内存的分配
        EfiLoaderData,
        fileInstance->mEfiInfoSize,            //传入BMP文件大小
        (VOID**)&fileInstance->mEfiFileInfo    //初始化EFI_FILE_INFO结构
    );    
    status = fileInstance->mFileInstance->GetInfo(//读取EFI_FILE_INFO
        fileInstance->mFileInstance,           //EFI_FILE协议的实例
        &gEfiFileInfoGuid,                      //传入GUID
        &fileInstance->mEfiInfoSize,           //buffer参数中欲存储EFI_FILE_INFO结构体大小
        fileInstance->mEfiFileInfo             //将文件的信息数据存储到EFI_FILE_INFO结构中
    );
    if(EFI_ERROR(status)){
        Print(L"\nGetinfo error!!! Status = %d.\n",status);
        return status;
    }

    fileInstance->mEfiFileSize = fileInstance->mEfiFileInfo->FileSize;  //获取文件大小
    status = gBS->AllocatePages(           //为文件映射的内存进行空间分配
        AllocateAnyPages,                  //分配任意内存空间
        EfiLoaderData,
        (fileInstance->mEfiFileInfo->FileSize >> 12) + 1,   //转换为页的大小
        &fileInstance->mEfiFileBuffer                       //为文件映射的内存进行空间分配
    );
    if(EFI_ERROR(status)){
        Print(L"AllocatePages failed!!! Status = %d.\n",status);
        return status;
    }
    UINTN readFileSize = fileInstance->mEfiFileInfo->FileSize;//实际读取文件的大小
    Print(L"FileCtrl.c readFileSize = %d\n",readFileSize);
    status = fileInstance->mFileInstance->Read(               //将文件缓存到内存中
        fileInstance->mFileInstance, 
        &readFileSize, 
        (VOID *)fileInstance->mEfiFileBuffer
    );
    if(EFI_ERROR(status)){
        Print(L"File Read failed!!! Status = %d.\n",status);
    }
    
    /*
     *此时的EFI_FILE_INFO使命已经结束，读取已经完成
     */
    gBS->FreePool(fileInstance->mEfiFileInfo);                          //释放内存
    status = fileInstance->mFileInstance->Close( fileInstance->mFileInstance);   //文件已经被读取到内存，关闭文件
    if(EFI_ERROR(status)){
        Print(L"File Close Error! error code = %d\n",status);
        return status;
    }
    return EFI_SUCCESS;
}