/*
* @file         boottype.h
* @brief		
* @details	    BootLoader阶段所需的所有基础头文件
* @author		LPH
* @date		    2023-3-3 
* @version	    v1.0
* @par Copyright(c):LPH corporation
* @par History:         
*	    2023-3-3 
*/
#ifndef _BOOT_TYPE_H_
#define _BOOT_TYPE_H_
#pragma once
#pragma GCC push_options            //可以将当前的GCC选项暂存。
#pragma GCC optimize (0)            //可以调整当前GCC的优化选项。0就代表O0优化。也支持字符串：#pragma GCC optimize ("O1")
#pragma GCC pop_options             //将之前暂存的GCC编译选项还原出来。
#include<Uefi.h>
#include<Library/BaseLib.h>
#include<Library/BaseMemoryLib.h>
#include<Library/UefiLib.h>
#include<Library/UefiBootServicesTableLib.h>
#include<Library/UefiRunTimeServicesTableLib.h>
#include<Library/DebugLib.h>
#include<Guid/FileInfo.h>                   //文件信息表头文件

typedef int Bool;      
typedef UINT64 pteval_t;        //PTE   
typedef UINT64 pmdval_t;        //PMD
typedef UINT64 pudval_t;        //PUD
typedef UINT64 pgdval_t;        //PGD
typedef UINT64 phys_addr_t;

struct page_table_entry;      //PTE
struct page_middle_directory; //PMD
struct page_upper_directory;  //PUD
struct page_global_directory; //PGD

/*BootLoader阶段的UEFI系统协议*/
extern EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *g_efiSimpleFileSystem; //文件系统协议的全局变量
extern EFI_GRAPHICS_OUTPUT_PROTOCOL *g_Gop;    //图形系统协议的全局变量

/*-----------------------------------------------------------------------------
* 内核页面设置参数
*------------------------------------------------------------------------------*/
/*物理地址转换为虚拟地址(线性映射)*/
#define CONFIG_ARM64_VA_BITS 48                             //开启4KB分页
#define VA_BITS			(CONFIG_ARM64_VA_BITS)

#define PAGE_SHIFT_LIUOS 12
#define PAGE_SIZE_LIUOS 4096              //页面大小
#define PAGE_MASK_LIUOS         (~(PAGE_SIZE_LIUOS-1))
#define PAGE_ALIGN(addr) (((addr)+PAGE_SIZE_LIUOS-1) & PAGE_MASK_LIUOS)

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
#endif  //_BOOT_TYPE_H_