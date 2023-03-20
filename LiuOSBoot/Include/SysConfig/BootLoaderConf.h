/**
* @file         BootLoaderConf.h
* @brief		
* @details	    BootLoader阶段的所有重要参数设置
* @author		LPH
* @date		    2023-3-3 
* @version	    v1.0
* @par Copyright(c):LPH corporation
* @par History:         
*	    2023-3-3 
*/
#ifndef _BOOTLOADER_CONF_H_
#define _BOOTLOADER_CONF_H_
#pragma once

#include "CommonHeaders.h"

/*-----------------------------------------------------------------------------
* BootLoader阶段的UEFI系统协议
*------------------------------------------------------------------------------*/
extern EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *g_efiSimpleFileSystem; //文件系统协议的全局变量
extern EFI_GRAPHICS_OUTPUT_PROTOCOL *g_Gop;    //图形系统协议的全局变量

/*-----------------------------------------------------------------------------
* 系统配置
*------------------------------------------------------------------------------*/
/*显示分辨率设置*/
#define HORIZONTAL_RESOLUTION 1600
#define VERTICAL_RESOLUTION 900

/*显示LOGO向上偏移量*/
#define STARTUP_IMAGE_PATH L"StartUpImage.bmp"  //启动LOGO路径
#define STARTUP_IMAGE_OFFSET 100

/*系统字体设置设置(专供OS内核)*/
#define FONT_IMAGE_PATH L"font.bmp"             //字模路径
#define FONT_PIXEL_SIZE 16                      //ASCII的大小为16px
#define FONT_CHARACTER_OFFSET 1                 //每两个ASCII之间距离为1px

/*OS Kernel名称*/
#define KERNEL_FILE_PATH L"Kernel.elf"

/*系统日志路径*/
#define JOURNAL_FILE_PATH L"LiuOSJournalFile.log"

#endif //_BOOTLOADER_CONF_H_