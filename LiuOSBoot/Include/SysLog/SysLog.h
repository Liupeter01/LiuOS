/**
* @file         SysLog.h
* @brief		
* @details	    BootLoader的启动日志记录
* @author		LPH
* @date		    2023-3-3 
* @version	    v1.0
* @par Copyright(c):LPH corporation
* @par History:         
*	    2023-3-3 
*/

#ifndef _SYS_LOG_H_
#define _SYS_LOG_H_
#pragma once
#include"GeneralFileLoader.h"
#include "BootLoaderConf.h"
    
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
    OUT EFI_FILE_DESCRIPTOR *journalFile
);

/*-----------------------------------------------------------------------------
* 系统日志输出系统
* @name: initJournalLog
* @function: 用于将系统日志输出到文件
* @param : 1.传入系统的ImageHandle参数
           2.输出的日志文件信息
           
* @retValue：初始化系统日志输出是否初始化成功
*------------------------------------------------------------------------------*/
//Bool EFIAPI WriteLog(IN CHAR16 *log);
#endif //_SYS_LOG_H_