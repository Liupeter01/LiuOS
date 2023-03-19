#ifndef _PAGES_H_
#define _PAGES_H_
#pragma once
#include"page-table.h"

/*----------------------------------------------------------
*引入链接文件中的重要pgd目录地址以及代码段和数据段起始终止地址
*----------------------------------------------------------*/
extern char swapper_pg_dir[];
extern char idmap_pg_dir[];
extern char _data_start[],_data_end[];
extern char _text_start[], _text_end[];
extern char _rodata_start[], _rodata_end[];
extern char _here[];

/*-----------------------------------------------------------------------------
* 操作系统初始化内存分页
* @name: paging_init
* @function: 
* @param : 
* @retValue: 
*------------------------------------------------------------------------------*/
void paging_init();

/*-----------------------------------------------------------------------------
* 操作系统创建链接文件中关键内核映像中段的映射
* @name: paging_init
* @function: 操作系统创建链接文件中关键内核映像中段的映射
* @param : 1.全局目录PGD(Kernel为swapper_pg_dir)
           2.映射虚拟地址
           3.虚拟地址长度
           4.一个页面的分配方式 
*------------------------------------------------------------------------------*/
void 
map_kernel_segment(
    page_global_directory *pgd,
    void* virtual_addr,
    void* virtual_end,
    phys_addr_t (*alloc_method)(void)   //页表PDG的分配方式
);   

/*-----------------------------------------------------------------------------
* 关闭TLB的结构
* @name: disable_tlb_cache
* @function: 关闭TLB的结构
* @param : 1.读取UEFI传输给操作系统的参数中内存布局部分参数
* @retValue: 返回操作系统内存是否初始化成功
*------------------------------------------------------------------------------*/
void disable_tlb_cache();

#endif //_PAGES_H_