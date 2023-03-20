#ifndef _PAGETABLE_H_
#define _PAGETABLE_H_
#pragma once
#include"type.h"
#include"pm_statistics.h"       //物理内存数据(来自UEFI内存描述符)

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
#define pgd_index(virt)		(((virt) >> PGDIR_SHIFT) & (PTRS_PER_PGD - 1))

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
#define PTE_MAYBE_NG		(PTE_NG)
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
* 获取物理内存的空闲页面
* @name: get_free_page
* @function: 获取物理内存的空闲页面
* @retvalue：返回物理地址
*------------------------------------------------------------------------------*/
unsigned long get_free_page();

/*-----------------------------------------------------------------------------
* 释放物理内存
* @name: free_page
* @function: 释放物理内存
* @param:1.传入地址
*------------------------------------------------------------------------------*/
void free_page(unsigned long addr);

/*-----------------------------------------------------------------------------
* 用于在系统没有初始化页表之前进行内存分配的函数
* @name: early_pagetable_alloc
* @function: 分配一个4KB的页面用于页表
* @param : 1.
*------------------------------------------------------------------------------*/
unsigned long early_pagetable_alloc();

/*-----------------------------------------------------------------------------
* 创建PGD(L0)的页表映射
* @name: __create_pgd_mapping
* @function: 创建PGD(L0)的页表映射
* @param : 1.全局目录PGD
           2.物理地址
           3.映射虚拟地址
           4.地址长度
           5.创建映射的内存属性
           6.页表创建过程的标识位
           7.分配下级页表的内存分配函数
*------------------------------------------------------------------------------*/
void __create_pgd_mapping(
    page_global_directory *pgd,
    unsigned long physical_addr,
    unsigned long virt,                 //映射虚拟地址
    unsigned long mapping_size,
    unsigned long attribute,
    unsigned long flags,
    unsigned long (*alloc_method)(void)    //页表PDG的分配方式	
);

/*-----------------------------------------------------------------------------
* 操作系统创建恒等映射，映射代码区和数据区
* @name: create_identical_mapping
* @function: 映射VA=PA的区域，主要目的就是为了打开MMU进行必要的准备
*------------------------------------------------------------------------------*/
void create_identical_mapping();

#endif //_PAGETABLE_H_