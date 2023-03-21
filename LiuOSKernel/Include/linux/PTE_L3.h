#ifndef _PTE_L3_H_
#define _PTE_L3_H_
#pragma once
#include"type.h"

/*-----------------------------------------------------------------
*页表PageTable PTE(L3)
*-----------------------------------------------------------------*/
typedef struct page_table_entry{ 
    pteval_t pte; 
} page_table_entry;

#define PTE_SHIFT		12                                      //PTE页表在虚拟地址中的偏移
#define PTE_SIZE		(1UL << PTE_SHIFT)                      //PTE页表项所能映射区域的大小
#define PTE_MASK		(~(PTE_SIZE-1))                         //屏蔽PT索引段的影响
#define PTRS_PER_PTE	(1 << (PMD_SHIFT - PTE_SHIFT))          //PTE页表项的个数

#define PTE_ADDR_LOW	(((1UL << (48 - PAGE_SHIFT)) - 1) << PAGE_SHIFT)
#define PTE_ADDR_MASK	(PTE_ADDR_LOW)

#define pte_val(x)	((x).pte)
#define __pte(x)	((page_table_entry) { (x) } )

/*-----------------------------------------------------------------
*PTE数值和页表的互相转化
*-----------------------------------------------------------------*/
#define __pte_to_phys(pte)	(pte_val(pte) & PTE_ADDR_MASK)      //PTE数值转换物理地址
#define __phys_to_pte_val(phys)	(phys)                          //PTE物理地址转换数值


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
#define PTE_VALID		    (1UL << 0)
#define PTE_WRITE		    (PTE_DBM)		 /* same as DBM (51) */
#define PTE_DIRTY		    (1UL << 55)
#define PTE_SPECIAL		    (1UL << 56)
#define PTE_PROT_NONE		(1UL << 58) /* only when !PTE_VALID */

#define PTE_TYPE_MASK		(3UL << 0)
#define PTE_TYPE_FAULT		(0UL << 0)
#define PTE_TYPE_PAGE		(3UL << 0)
#define PTE_TABLE_BIT		(1UL << 1)
#define PTE_USER		    (1UL << 6)		
#define PTE_RDONLY		    (1UL << 7)		
#define PTE_SHARED		    (1UL << 8)		
#define PTE_AF			    (1UL << 10)	
#define PTE_NG			    (1UL << 11)	
#define PTE_DBM			    (1UL << 51)
#define PTE_CONT		    (1UL << 52)	
#define PTE_PXN			    (1UL << 53)	
#define PTE_UXN			    (1UL << 54)	
#define PTE_HYP_XN		    (1UL << 54)	

#define PTE_ATTRINDX(t)		((t) << 2)
#define PTE_ATTRINDX_MASK	(7 << 2)

#define PTE_MAYBE_NG		(PTE_NG)

void alloc_pgtable();

#endif //_PTE_L3_H_