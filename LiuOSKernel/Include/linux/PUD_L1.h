#ifndef _PUD_L1_H_
#define _PUD_L1_H_
#pragma once
#include"type.h"

/*-----------------------------------------------------------------
*页上级目录Page Upper Directory(L1)
*-----------------------------------------------------------------*/
typedef struct page_upper_directory{ 
    pudval_t pud; 
} page_upper_directory;

#define pud_val(x)	((x).pud)
#define __pud(x)	((page_upper_directory) { (x) } )

#define PUD_SHIFT		30                                      //PUD页表在虚拟地址中的偏移
#define PUD_SIZE		(1UL << PUD_SHIFT)                      //PUD页表项所能映射区域的大小
#define PUD_MASK		(~(PUD_SIZE-1))                         //屏蔽其他目录的干扰
#define PTRS_PER_PUD	(1 << (PGDIR_SHIFT - PUD_SHIFT) )       //PUD页表中页表项的个数

/*-----------------------------------------------------------------
*用于服务PUD页表功能函数的宏定义
-----------------------------------------------------------------*/
#define pud_index(virt)	(((virt) >> PUD_SHIFT) & (PTRS_PER_PUD - 1))     //PUD页表项的索引功能

/*
 * Hardware page table definitions.
 *
 * Level 1 descriptor (PUD).
 */
#define PUD_TYPE_TABLE		(3UL << 0)
#define PUD_TABLE_BIT		(1UL << 1)
#define PUD_TYPE_MASK		(3UL << 0)
#define PUD_TYPE_SECT		(1UL << 0)

/*PUD页表是否抵达最底端(PUD是否越界)*/    
#define is_pud_arrive_end(virt,end_virt) ({ \
	unsigned long __boundary = ((virt) + PUD_SIZE) & PUD_MASK;	\
	(((__boundary) - 1) < ((end_virt) - 1))? (__boundary): (end_virt);	\
})

/*-----------------------------------------------------------------------------
* PUD(L1)和PMD(L2)之间的关联
* @name: set_l1_l2_connection
* @function: 创建 PUD(L1)和PMD(L2)之间的关联
* @param : 1.PUD
           2.PMD
           3.页面属性(标记第三位为页表类型TABLE类型而不是页面)

* @bug:  2023-3-21 可能存在进程线程冲突，毕竟锁机制还没有开发
*------------------------------------------------------------------------------*/
void set_l1_l2_connection( 
    page_upper_directory *pud,
    UINT64 pmd_addr,
    pgdval_t attribute
);

/*-----------------------------------------------------------------------------
* 取消PUD(L1)和PMD(L2)之间的关联
* @name: close_l1_l2_connection
* @function: 断开PUD(L1)和PMD(L2)之间的关联
* @param : 1.PUD
* @bug:  2023-3-21 可能存在进程线程冲突，毕竟锁机制还没有开发
*------------------------------------------------------------------------------*/
void close_l1_l2_connection( 
    page_upper_directory *pud
);

/*-----------------------------------------------------------------------------
* 创建PUD(L1)的页表映射
* @name: alloc_pgtable_pud
* @function: 1.如果pgd中的内容(*pgd)为空，则其中必定没有创建PUDL1页表
             2.创建PUD(L1)的页表并分配物理内存
                填充高级PGD的指定页表项链接PUD，形成链接PGD->PUD

* @param : 1.全局目录PGD
           2.虚拟地址起始(为PUD修正后的虚拟地址)
           3.虚拟地址终止(为PUD修正后的虚拟地址)
           4.物理地址(为PUD修正后的物理地址)
           5.创建映射的内存属性
           6.页表创建过程的标识位
           7.分配下级页表的内存分配函数
*------------------------------------------------------------------------------*/
void alloc_pgtable_pud(
    page_global_directory *pgd,
    UINT64 va_pud_start,            //映射虚拟地址起始
    UINT64 va_pud_end,              //映射虚拟地址终止
    UINT64 physical_addr_pud,
    unsigned long attribute,
    unsigned long flags,
    UINT64 (*alloc_method)(void)    //页表PDG的分配方式	
);

#endif //_PUD_L1_H_