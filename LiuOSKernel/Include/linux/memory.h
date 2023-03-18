#ifndef _MEMORY_H_
#define _MEMORY_H_
#pragma once
#include"page-table.h"

/*----------------------------------------------------------
*引入链接文件中的重要pgd目录地址以及代码段和数据段起始终止地址
----------------------------------------------------------*/
extern char swapper_pg_dir[];
extern char idmap_pg_dir[];
extern char _data_end[],_data_start[];
extern char  _text_start[], _text_end[];

void paging_init();

void map_kernel_segment(
    page_global_directory *pgd,
    char * start_addr,
    char * end_addr
);

void __create_pgd_mapping(
    page_global_directory *pgd,
    phys_addr_t physical_addr,
    phys_addr_t mapping_size,
    unsigned long virt          //映射虚拟地址
);


#endif //_MEMORY_H_