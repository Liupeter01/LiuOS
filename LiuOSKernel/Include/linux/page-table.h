#ifndef _PAGETABLE_H_
#define _PAGETABLE_H_
#pragma once
#include"PGD_L0.h"
#include"PUD_L1.h"
#include"PMD_L2.h"
#include"PTE_L3.h"
#include"pm_statistics.h"       //物理内存数据(来自UEFI内存描述符)

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
phys_addr_t pgd_page_to_physical_address(page_global_directory pgd);

/*-----------------------------------------------------------------------------
* 
* @name: pud_page_to_physical_address
* @function: 
*------------------------------------------------------------------------------*/
phys_addr_t pud_page_to_physical_address(page_upper_directory pud);

/*-----------------------------------------------------------------------------
* 
* @name: pmd_page_to_physical_address
* @function: 
*------------------------------------------------------------------------------*/
phys_addr_t pmd_page_to_physical_address(page_middle_directory pmd);


/*-----------------------------------------------------------------------------
* 
* @name: pgd_pte
* @function: 
*------------------------------------------------------------------------------*/
page_table_entry pgd_pte(page_global_directory pgd);

/*-----------------------------------------------------------------------------
* 
* @name: 
* @function: 
*------------------------------------------------------------------------------*/
page_table_entry pud_pte(page_upper_directory pud);

/*-----------------------------------------------------------------------------
* 
* @name: 
* @function: 
*------------------------------------------------------------------------------*/
page_upper_directory pte_pud(page_table_entry pte);

/*-----------------------------------------------------------------------------
* 
* @name: 
* @function: 
*------------------------------------------------------------------------------*/
page_middle_directory pud_pmd(page_upper_directory pud);

/*-----------------------------------------------------------------------------
* 
* @name: 
* @function: 
*------------------------------------------------------------------------------*/
page_table_entry pmd_pte(page_middle_directory pmd);

/*-----------------------------------------------------------------------------
* 
* @name: 
* @function: 
*------------------------------------------------------------------------------*/
page_middle_directory pte_pmd(page_table_entry pte);

/*-----------------------------------------------------------------
*仅供启动测试用途的内存映射
*-----------------------------------------------------------------*/
extern unsigned short memory_map[NR_PAGES];

/*-----------------------------------------------------------------
*内存属性参数(根据设备属性编号MT_*_*访问页表的AttrIndex字段数值)
-----------------------------------------------------------------*/
#define MT_DEVICE_nGnRnE	0   //设备内存，不支持聚合、指令重排以及提前写应答
#define MT_DEVICE_nGnRE		1   //设备内存，不支持聚合、指令重排，但是支持提前写应答
#define MT_DEVICE_GRE		2   //设备内存，支持聚合、指令重排以及提前写应答
#define MT_NORMAL_NC		3   //普通内存，关闭高速缓存
#define MT_NORMAL		    4   //普通内存(内核的可执行代码段和数据段采用该部分内存!!!!!!!)
#define MT_NORMAL_WT		5   //普通内存，高速缓存采用直写回写策略

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

/*----------------------------------------------------------
*引入来自链接文件ld中SECTIONS中的重要参数信息
*1.idmap_pg_dir：恒等映射的页表PDG
 2.swapper_pg_dir
 3.数据段的起始和终止：_data_start[],_data_end[];
 4.代码段的起始和终止：_text_start[], _text_end[];
 5.只读数据段的起始和终止_rodata_start[], _rodata_end[];
*----------------------------------------------------------*/
extern char _data_start[],_data_end[];
extern char _text_start[], _text_end[];
extern char _rodata_start[], _rodata_end[];
extern char _here[];

extern page_global_directory swapper_pg_dir[];
extern page_global_directory idmap_pg_dir[];

/*-----------------------------------------------------------------------------
* 初始化物理内存分配的位图
* @name: init_memory_bitmap
* @function: 初始化物理内存分配的位图用于空闲内存分配
*------------------------------------------------------------------------------*/
void init_memory_bitmap();

/*-----------------------------------------------------------------------------
* 获取物理内存的空闲页面
* @name: get_free_page
* @function: 获取物理内存的空闲页面
* @retvalue：返回物理地址
*------------------------------------------------------------------------------*/
UINT64 get_free_page();

/*-----------------------------------------------------------------------------
* 释放物理内存
* @name: free_page
* @function: 释放物理内存
* @param:1.传入地址
*------------------------------------------------------------------------------*/
void free_page(UINT64 addr);

/*-----------------------------------------------------------------------------
* 用于在系统没有初始化页表之前进行内存分配的函数
* @name: early_pagetable_alloc
* @function: 分配一个4KB的页面用于页表
* @param : 1.
*------------------------------------------------------------------------------*/
UINT64 early_pagetable_alloc();

#endif //_PAGETABLE_H_