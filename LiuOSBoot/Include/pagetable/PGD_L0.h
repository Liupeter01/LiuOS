#ifndef _PGD_L0_H_
#define _PGD_L0_H_
#pragma once
#include"boottype.h"

/*-----------------------------------------------------------------
*页全局目录Page Global Directory(L0)
*-----------------------------------------------------------------*/
typedef struct page_global_directory{ 
    pgdval_t pgd; 
} page_global_directory;

#define MAX_USER_VA_BITS 48                                     //虚拟地址位宽
#define PGDIR_SHIFT	39                                          //PGD页表在虚拟地址中的偏移
#define PGDIR_SIZE	(1UL << PGDIR_SHIFT)                        //PGD页表项所能映射的区域
#define PGDIR_MASK	(~(PGDIR_SIZE-1))                           //屏蔽其他的目录的干扰
#define PTRS_PER_PGD (1 << (MAX_USER_VA_BITS - PGDIR_SHIFT))    //PGD页表中页表项的个数

/*-----------------------------------------------------------------
*用于服务PGD页表功能函数的宏定义
-----------------------------------------------------------------*/
#define pgd_val(x)	((x).pgd)
#define __pgd(x)	((page_global_directory) { (x) } )
#define pgd_index(virt)	(((virt) >> PGDIR_SHIFT) & (PTRS_PER_PGD - 1))     //PGD页表项的索引功能
#define pgd_entry_offset(pgd,addr) ((pgd) + pgd_index(addr))

 /*PGD页表是否抵达最底端(PGD是否越界)*/    
#define is_pgd_arrive_end(virt,end_virt) ({ \
	unsigned long __boundary = ((virt) + PGDIR_SIZE) & PGDIR_MASK;	\
	(((__boundary) - 1) < ((end_virt) - 1))? (__boundary): (end_virt);	\
})

/*-----------------------------------------------------------------------------
* PGD(L0)和PUD(L1)之间的关联
* @name: set_l0_l1_connection
* @function: 创建 PGD(L0)和PUD(L1)之间的关联
* @param : 1.全局目录PGD
           2.PUD
           3.页面属性(标记第三位为页表类型TABLE类型而不是页面)

* @bug:  2023-3-21 可能存在进程线程冲突，毕竟锁机制还没有开发
*------------------------------------------------------------------------------*/
void set_l0_l1_connection( 
    page_global_directory *pgd,
    phys_addr_t pud_addr,
    pgdval_t attribute
);

/*-----------------------------------------------------------------------------
* 取消PGD(L0)和PUD(L1)之间的关联
* @name: close_l0_l1_connection
* @function: 断开PGD(L0)和PUD(L1)之间的关联
* @param : 1.全局目录PGD
* @bug:  2023-3-21 可能存在进程线程冲突，毕竟锁机制还没有开发
*------------------------------------------------------------------------------*/
void close_l0_l1_connection( 
    page_global_directory *pgd
);

/*-----------------------------------------------------------------------------
* 创建PGD(L0)的页表映射
* @name: __create_pgd_mapping
* @function: 创建PGD(L0)的页表映射,此时的PGD还没有登录下一级L1的地址
* @param : 1.全局目录PGD
           2.物理地址
           3.映射虚拟地址
           4.地址长度
           5.创建映射的内存属性
           6.页表创建过程的标识位
           7.分配下级页表的内存分配函数
* @update:
            2023-3-22 :修复PGD页表项遍历越界的异常BUG
*------------------------------------------------------------------------------*/
void 
__create_pgd_mapping(
    page_global_directory *pgd,
    phys_addr_t physical_addr,
    phys_addr_t virt,                 //映射虚拟地址
    unsigned long mapping_size,
    unsigned long attribute,
    unsigned long flags,
    phys_addr_t (*alloc_method)(void)    //页表PDG的分配方式	
);

#endif //_PGD_L0_H_