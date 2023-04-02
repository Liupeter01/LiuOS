#ifndef _PAGETABLE_H_
#define _PAGETABLE_H_
#pragma once
#include"PGD_L0.h"
#include"PUD_L1.h"
#include"PMD_L2.h"
#include"PTE_L3.h"

/*-----------------------------------------------------------------
 *   PGD,PUD,PMD到PTE页表的转化
 *   +--------+--------+--------+--------+--------+--------+--------+--------+
 *   |63    56|55    48|47    40|39    32|31    24|23    16|15     8|7      0|
 *   +--------+--------+--------+--------+--------+--------+--------+--------+
 *    |                 |         |         |         |         |
 *    |                 |         |         |         |         v
 *    |                 |         |         |         |   [11:0]  in-page offset
 *    |                 |         |         |         +-> [20:12] L3 index
 *    |                 |         |         +-----------> [29:21] L2 index
 *    |                 |         +---------------------> [38:30] L1 index
 *    |                 +-------------------------------> [47:39] L0 index
 *    +-------------------------------------------------> [63] TTBR0/1
 *
-----------------------------------------------------------------*/
#define __pgd_to_phys(pgd) __pte_to_phys(pgd_pte(pgd))                    //PGD页表项中内容转换物理地址
#define __pud_to_phys(pud) __pte_to_phys(pud_pte(pud))                    //PUD页表项中内容转换物理地址
#define __pmd_to_phys(pmd) __pte_to_phys(pmd_pte(pmd))                    //PMD页表项中内容转换物理地址

/*-----------------------------------------------------------------------------
* 
* @name: pgd_page_to_physical_address
* @function: 
*------------------------------------------------------------------------------*/
phys_addr_t pgd_page_to_physical_address(struct page_global_directory pgd);

/*-----------------------------------------------------------------------------
* 
* @name: pud_page_to_physical_address
* @function: 
*------------------------------------------------------------------------------*/
phys_addr_t pud_page_to_physical_address(struct page_upper_directory pud);

/*-----------------------------------------------------------------------------
* 
* @name: pmd_page_to_physical_address
* @function: 
*------------------------------------------------------------------------------*/
phys_addr_t pmd_page_to_physical_address(struct page_middle_directory pmd);


/*-----------------------------------------------------------------------------
* 
* @name: pgd_pte
* @function: 
*------------------------------------------------------------------------------*/
page_table_entry pgd_pte(struct page_global_directory pgd);

/*-----------------------------------------------------------------------------
* 
* @name: 
* @function: 
*------------------------------------------------------------------------------*/
page_table_entry pud_pte(struct page_upper_directory pud);

/*-----------------------------------------------------------------------------
* 
* @name: 
* @function: 
*------------------------------------------------------------------------------*/
page_upper_directory pte_pud(struct page_table_entry pte);

/*-----------------------------------------------------------------------------
* 
* @name: 
* @function: 
*------------------------------------------------------------------------------*/
page_middle_directory pud_pmd(struct page_upper_directory pud);

/*-----------------------------------------------------------------------------
* 
* @name: 
* @function: 
*------------------------------------------------------------------------------*/
page_table_entry pmd_pte(struct page_middle_directory pmd);

/*-----------------------------------------------------------------------------
* 
* @name: 
* @function: 
*------------------------------------------------------------------------------*/
page_middle_directory pte_pmd(struct page_table_entry pte);

/*-----------------------------------------------------------------
*内存属性参数(根据设备属性编号MT_*_*访问页表的AttrIndex字段数值)
-----------------------------------------------------------------*/
#define _PROT_DEFAULT		(PTE_TYPE_PAGE | PTE_AF | PTE_SHARED)
#define PROT_DEFAULT		(_PROT_DEFAULT | PTE_MAYBE_NG)
#define PROT_DEVICE_nGnRnE	(PROT_DEFAULT | PTE_PXN | PTE_UXN | PTE_DIRTY | PTE_WRITE | PTE_ATTRINDX(MT_DEVICE_nGnRnE))
#define PROT_DEVICE_nGnRE	(PROT_DEFAULT | PTE_PXN | PTE_UXN | PTE_DIRTY | PTE_WRITE | PTE_ATTRINDX(MT_DEVICE_nGnRE))
#define PROT_NORMAL_NC		(PROT_DEFAULT | PTE_PXN | PTE_UXN | PTE_DIRTY | PTE_WRITE | PTE_ATTRINDX(MT_NORMAL_NC))
#define PROT_NORMAL_WT		(PROT_DEFAULT | PTE_PXN | PTE_UXN | PTE_DIRTY | PTE_WRITE | PTE_ATTRINDX(MT_NORMAL_WT))
#define PROT_NORMAL		(PROT_DEFAULT | PTE_PXN | PTE_UXN | PTE_DIRTY | PTE_WRITE | PTE_ATTRINDX(MT_NORMAL))

/*-----------------------------------------------------------------
*内核中的页面设置
-----------------------------------------------------------------*/
typedef struct { pteval_t pgprot; } pgprot_t;
#define pgprot_val(x)	((x).pgprot)
#define __pgprot(x)	((pgprot_t) { (x) } )

#define PAGE_KERNEL		(PROT_NORMAL)                                               //内核普通内存页面
#define PAGE_KERNEL_RO		((PROT_NORMAL & ~PTE_WRITE) | PTE_RDONLY)               //内核只读普通页面
#define PAGE_KERNEL_ROX		((PROT_NORMAL & ~(PTE_WRITE | PTE_PXN)) | PTE_RDONLY)   //内核只读可执行页面
#define PAGE_KERNEL_EXEC	(PROT_NORMAL & ~PTE_PXN)                                //内核可执行普通页面
#define PAGE_KERNEL_EXEC_CONT	((PROT_NORMAL & ~PTE_PXN) | PTE_CONT)               //内核可执行普通页面且物理连续

#endif //_PAGETABLE_H_