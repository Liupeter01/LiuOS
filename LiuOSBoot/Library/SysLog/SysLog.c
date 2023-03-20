/**
* @file         SysLog.c
* @brief		
* @details	    BootLoader的启动日志记录
* @author		LPH
* @date		    2023-3-3 
* @version	    v1.0
* @par Copyright(c):LPH corporation
* @par History:         
*	    2023-3-3 
*/


#include"SysLog.h"

/*-----------------------------------------------------------------------------
* 初始化系统日志输出
* @name: initJournalLog
* @function: 用于将系统日志输出到文件
* @param : 1.传入系统的ImageHandle参数
           2.输出的日志文件信息

* @retValue：初始化系统日志输出是否初始化成功
*------------------------------------------------------------------------------*/
Bool EFIAPI initJournalLog(
    IN EFI_HANDLE ImageHandle,
    OUT EFI_FILE_DESCRIPTOR *journalFile)
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
                &journalFile->mFileInstance,
                JOURNAL_FILE_PATH,
                EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
                EFI_OPEN_PROTOCOL_GET_PROTOCOL
            );
            if(EFI_ERROR(status)){
                Print(L"JournalLog System Loading Failed!\n");
                return FALSE;
            }
            journalFile->mEfiInfoSize = sizeof(EFI_FILE_INFO) + 128;   //EFI_FILE_INFO大小预定义
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
    Print(L"JournalLog System Loading Success!\n");
    return TRUE; 
}