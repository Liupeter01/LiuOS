#ifndef _PAGETABLE_H_
#define _PAGETABLE_H_
#pragma once
#include"type.h"

typedef UINT64 pteval_t;        //PTE   
typedef UINT64 pmdval_t;        //PMD
typedef UINT64 pudval_t;        //PUD
typedef UINT64 pgdval_t;        //PGD

typedef UINT32 phys_addr_t;     //物理地址

/*-----------------------------------------------------------------
*页全局目录Page Global Directory(L0)
*-----------------------------------------------------------------*/
typedef struct page_global_directory{ 
    pgdval_t pgd; 
} page_global_directory;

#define pgd_val(x)	((x).pgd)
#define __pgd(x)	((page_global_directory) { (x) } )

#define MAX_USER_VA_BITS 48                                     //虚拟地址位宽
#define PGDIR_SHIFT	39                                          //PGD页表在虚拟地址中的偏移
#define PGDIR_SIZE	(1UL << PGDIR_SHIFT)                        //PGD页表项所能映射的区域
#define PGDIR_MASK	(~(PGDIR_SIZE-1))                           //屏蔽其他的目录的干扰
#define PTRS_PER_PGD (1 << (MAX_USER_VA_BITS - PGDIR_SHIFT))    //PGD页表中页表项的个数

/*-----------------------------------------------------------------
*页上级目录Page Upper Directory(L1)
*-----------------------------------------------------------------*/
typedef struct PageUpperDirectory{ 
    pudval_t pud; 
} PageUpperDirectory;
#define pud_val(x)	((x).pud)
#define __pud(x)	((PageUpperDirectory) { (x) } )

#define PUD_SHIFT		30                                      //PUD页表在虚拟地址中的偏移
#define PUD_SIZE		(1UL << PUD_SHIFT)                      //PUD页表项所能映射区域的大小
#define PUD_MASK		(~(PUD_SIZE-1))                         //屏蔽其他目录的干扰
#define PTRS_PER_PUD	(1 << (PGDIR_SHIFT - PUD_SHIFT) )       //PUD页表中页表项的个数

/*-----------------------------------------------------------------
*页中间目录Page Middle Directory(L2)
*-----------------------------------------------------------------*/
typedef struct PageMiddleDirectory{ 
    pmdval_t pmd; 
} PageMiddleDirectory;
#define pmd_val(x)	((x).pmd)
#define __pmd(x)	((PageMiddleDirectory) { (x) } )

#define PMD_SHIFT		21                                      //PMD页表在虚拟地址中的偏移
#define PMD_SIZE		(1UL << PMD_SHIFT)                      //PMD页表项所能映射区域的大小
#define PMD_MASK		(~(PMD_SIZE-1))                         //屏蔽PT段的影响
#define PTRS_PER_PMD	(1 << (PUD_SHIFT - PMD_SHIFT))          //PMD页表中页表项的个数

/*-----------------------------------------------------------------
*页表PageTable PTE(L3)
*-----------------------------------------------------------------*/
typedef struct PageTableEntry{ 
    pteval_t pte; 
} PageTableEntry;
#define pte_val(x)	((x).pte)
#define __pte(x)	((PageTableEntry) { (x) } )

#define PTE_SHIFT		12                                      //PTE页表在虚拟地址中的偏移
#define PTE_SIZE		(1UL << PTE_SHIFT)                      //PTE页表项所能映射区域的大小
#define PTE_MASK		(~(PTE_SIZE-1))                         //屏蔽PT索引段的影响
#define PTRS_PER_PTE	(1 << (PMD_SHIFT - PTE_SHIFT))          //PTE页表项的个数

/*-----------------------------------------------------------------
*页表PTE(L3)的高位属性和低位属性
+---+--------+-----+-----+---+------------------------+---+----+----+----+----+------+----+----+
| R |   SW   | UXN | PXN | R | Output address [47:12] | R | AF | SH | AP | NS | INDX | TB | VB |
+---+--------+-----+-----+---+------------------------+---+----+----+----+----+------+----+----+
 63  58    55 54    53    52  47                    12 11  10   9  8 7  6 5    4    2 1    0

R    - reserve
SW   - reserved for software use
UXN  - unprivileged execute never
PXN  - privileged execute never
AF   - access flag
SH   - shareable attribute
AP   - access permission
NS   - security bit
INDX - index into MAIR register
TB   - table descriptor bit
VB   - validity descriptor bit
*-----------------------------------------------------------------*/


#endif //_PAGETABLE_H_