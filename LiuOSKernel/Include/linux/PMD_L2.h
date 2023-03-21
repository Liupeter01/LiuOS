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
*------------------------------------------------------------------------------*/
void alloc_pgtable_pmd(
    page_upper_directory *pud,
    UINT64 va_pmd_start,            //映射虚拟地址起始
    UINT64 va_pmd_end,              //映射虚拟地址终止
    UINT64 physical_addr_pmd,
    unsigned long attribute,
    unsigned long flags,
    UINT64 (*alloc_method)(void)    //页表PDG的分配方式	
);

#endif //_PMD_L2_H_