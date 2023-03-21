﻿#ifndef _PAGES_H_
#define _PAGES_H_
#pragma once
#include"page-table.h"
#include"tlb.h"

/*-----------------------------------------------------------------------------
* 操作系统初始化内存分页
* @name: paging_init
* @function: 
* @param : 
* @retValue: 
*------------------------------------------------------------------------------*/
void paging_init();

/*-----------------------------------------------------------------------------
* 操作系统创建恒等映射，映射代码区和数据区
* @name: create_identical_mapping
* @function: 映射VA=PA的区域，主要目的就是为了打开MMU进行必要的准备
*------------------------------------------------------------------------------*/
void create_identical_mapping();

/*-----------------------------------------------------------------------------
* 创建MMIO的恒等映射
* @name:  create_mmio_mapping
* @function: 创建MMIO的恒等映射(MMIO位于内存高地址处)
*------------------------------------------------------------------------------*/
void create_mmio_mapping();

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
#endif //_PAGES_H_