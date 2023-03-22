#ifndef _PMD_L2_H_
#define _PMD_L2_H_
#pragma once
#include"type.h"

/*-----------------------------------------------------------------
*页中间目录Page Middle Directory(L2)
*-----------------------------------------------------------------*/
typedef struct page_middle_directory{ 
    pmdval_t pmd; 
} page_middle_directory;

#define pmd_val(x)	((x).pmd)
#define __pmd(x)	((page_middle_directory) { (x) } )

#define PMD_SHIFT		21                                      //PMD页表在虚拟地址中的偏移
#define PMD_SIZE		(1UL << PMD_SHIFT)                      //PMD页表项所能映射区域的大小
#define PMD_MASK		(~(PMD_SIZE-1))                         //屏蔽PT段的影响
#define PTRS_PER_PMD	(1 << (PUD_SHIFT - PMD_SHIFT))          //PMD页表中页表项的个数

/*-----------------------------------------------------------------
*用于服务PUD页表功能函数的宏定义
-----------------------------------------------------------------*/
#define pmd_index(virt)	(((virt) >> PMD_SHIFT) & (PTRS_PER_PMD - 1))     //PUD页表项的索引功能
#define pmd_offset_physicaladdr(pmd_entry,addr) \
    ((struct page_middle_directory *)(pud_page_to_physical_address(pmd_entry) + pmd_index(addr) * sizeof(struct page_middle_directory)))

/*
 * Level 2 descriptor (PMD).
 */
#define PMD_TYPE_MASK		(3UL << 0)
#define PMD_TYPE_FAULT		(0UL << 0)
#define PMD_TYPE_TABLE		(3UL << 0)
#define PMD_TYPE_SECT		(1UL << 0)
#define PMD_TABLE_BIT		(1UL << 1)

/*PMD页表是否抵达最底端(PMD是否越界)*/    
#define is_pmd_arrive_end(virt,end_virt) ({ \
	unsigned long __boundary = ((virt) + PMD_SIZE) & PMD_MASK;	\
	(((__boundary) - 1) < ((end_virt) - 1))? (__boundary): (end_virt);	\
})

/*-----------------------------------------------------------------------------
* PMD(L2)和PTE(L3)之间的关联
* @name: set_l2_l3_connection
* @function: 创建PMD(L2)和PTE(L3)之间的关联
* @param : 1.PMD
           2.PTE
           3.页面属性(标记第三位为页表类型TABLE类型而不是页面)

* @bug:  2023-3-21 可能存在进程线程冲突，毕竟锁机制还没有开发
*------------------------------------------------------------------------------*/
void set_l2_l3_connection( 
    page_middle_directory *pmd,
    phys_addr_t pte_addr,
    pgdval_t attribute
);

/*-----------------------------------------------------------------------------
* 取消PMD(L2)和PTE(L3)之间的关联
* @name: close_l2_l3_connection
* @function: 断开PMD(L2)和PTE(L3)之间的关联
* @param : 1.PMD
* @bug:  2023-3-21 可能存在进程线程冲突，毕竟锁机制还没有开发
*------------------------------------------------------------------------------*/
void close_l2_l3_connection( 
    page_middle_directory *pmd
);

/*-----------------------------------------------------------------------------
* 创建PMD(L2)的页表映射
* @name: alloc_pgtable_pmd
* @function: 1.
             2.填充PUD的指定页表项链接PMD，形成链接PGD->PUD->PMD
                
* @param : 1.PUD
           2.虚拟地址起始(为PMD修正后的虚拟地址)
           3.虚拟地址终止(为PMD修正后的虚拟地址)
           4.物理地址(为PUD修正后的物理地址)
           5.创建映射的内存属性
           6.页表创建过程的标识位
           7.分配下级页表的内存分配函数
* @update:
            2023-3-22 :修复PMD页表项遍历越界的异常BUG
*------------------------------------------------------------------------------*/
void alloc_pgtable_pmd(
    page_upper_directory *pud,
    phys_addr_t va_pmd_start,            //映射虚拟地址起始
    phys_addr_t va_pmd_end,              //映射虚拟地址终止
    phys_addr_t physical_addr_pmd,
    unsigned long attribute,
    unsigned long flags,
    phys_addr_t (*alloc_method)(void)    //页表PDG的分配方式	
);

#endif //_PMD_L2_H_