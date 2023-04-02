/**
* @file         Kernel_paging.h
* @brief		
* @details	    Kernel的MMU和分页设置装置
* @author		LPH
* @date		    2023-3-3 
* @version	    v1.0
* @par Copyright(c):LPH corporation
* @par History:         
*	    2023-3-3 
*/
#ifndef _KERNEL_PAGING_H_
#define _KERNEL_PAGING_H_
#pragma once
#include"kernel_reader.h"
#include"os_hardwaredef.h"
#include"page_table.h"

/*经过处理的内核链接段*/
#define KERNEL_TEXT 0 //代码段
#define KERNEL_RODATA 1 //只读数据段
#define KERNEL_DATA 2 //数据段
#define KERNEL_BSS 3 //BSS段

/*-----------------------------------------------------------------------------
* 分配一个页面用于存放页表
* @name: AllocatePageTable
* @function: 分配一个页面用于存放页表
*------------------------------------------------------------------------------*/
phys_addr_t
EFIAPI 
AllocatePageTable();

/*-----------------------------------------------------------------------------
* 引入虚拟地址用于装载内核
* @name: mappingKernelAddress
* @function: 如果内核的基址很大，超出了物理内存范围那么我们将没有办法执行内核
* @param :  1.段表数组(提供虚拟地址)
            2.已经加载到物理内存的内核程序
            3.恒等映射PGD(TTBR0_EL1)
            4.非恒等映射的内核基地址PGD(TTBR1_EL1)
            5.页面分配函数
*------------------------------------------------------------------------------*/
VOID 
EFIAPI
mappingKernelAddress(
    IN Elf64_Phdr * _elf64_segment,    //偏移到程序头表(段表)
    IN EFI_PHYSICAL_ADDRESS kernelBufferBase,
    IN struct page_global_directory *idmap_pg_dir,
    IN struct page_global_directory *swapper_pg_dir,
    phys_addr_t (*alloc_page)()
);

/*-----------------------------------------------------------------------------
* 启动内存0~1GB的内存映射
* @name: mappingLow1GBMemory
* @function:启动内存0~1GB的内存映射(EFI的Runtime Services会用到)
* @param :  1.恒等映射PGD(TTBR0_EL1)
            3.页面分配函数
*------------------------------------------------------------------------------*/
VOID 
EFIAPI
mappingLow1GBMemory(
    IN struct page_global_directory *idmap_pg_dir,
    phys_addr_t (*alloc_page)()
);

/*-----------------------------------------------------------------------------
* 启动内存分页
* @name: EnableMemoryPaging
* @function: 1.启动恒等映射以及非恒等内存基地址映射
             2.启动内存0~1GB的内存映射(EFI的Runtime Services会用到)
             3.通过操作寄存器启动MMU

* @param :  1.段表数组(提供虚拟地址)
            2.已经加载到物理内存的内核程序
            3.恒等映射PGD(TTBR0_EL1)
            4.非恒等映射的内核基地址PGD(TTBR1_EL1)
            5.页面分配函数
*------------------------------------------------------------------------------*/
VOID 
EFIAPI
EnableMemoryPaging(
    IN Elf64_Phdr * _elf64_segment,    //偏移到程序头表(段表)
    IN EFI_PHYSICAL_ADDRESS kernelBufferBase,
    IN struct page_global_directory *idmap_pg_dir,
    IN struct page_global_directory *swapper_pg_dir,
    phys_addr_t (*alloc_page)()
);

/*-----------------------------------------------------------------------------
* 初始化CPU的参数并开启MMU
* @name: EnableHardware
* @function: 初始化CPU的参数并开启MMU
* @param :  1.恒等映射PGD(TTBR0_EL1)
            2.非恒等映射的内核基地址PGD(TTBR1_EL1)

* @retvalue: 返回硬件是否初始化成功
*------------------------------------------------------------------------------*/
Bool
EFIAPI
EnableHardware(
    IN struct page_global_directory * idmap_pg_dir,
	IN struct page_global_directory * swapper_pg_dir
);

/*-----------------------------------------------------------------------------
* 进入了分页模式，使用了虚拟地址，我们需要通知EFI更改他内部的指针，以适应这个变化
* @name: prepareVirtualMemoryMapping
* @function: 退出BOOTSERVICE并使用RUNTIMESERVICE设置他的VirtualStart通知EFI更改指针
* @param :  1.传入系统的ImageHandle参数
            2.传入MEMORY_MAP_CONFIG参数表
            
* @retValue：返回系统状态信息
*------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI 
prepareVirtualMemoryMapping(
    IN EFI_HANDLE ImageHandle,
    OUT MEMORY_MAP_CONFIG *MemoryMap
);

#endif //_KERNEL_PAGING_H_