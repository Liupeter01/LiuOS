#include"memory.h"

char swapper_pg_dir[];
char _text_start[], _text_end[];

void 
paging_init()
{
	map_kernel_segment(swapper_pg_dir, _text_start, _text_end);
}

void 
map_kernel_segment(
    page_global_directory *pgd,
    char * start_addr,
    char * end_addr)
{
	//phys_addr_t pa_start = __pa_symbol(va_start);
	unsigned long size = end_addr - start_addr;

	//__create_pgd_mapping(pgdp, pa_start, (unsigned long)va_start, size, prot,
	//	early_pgtable_alloc, flags);
}

void __create_pgd_mapping(
    page_global_directory *pgd,
    phys_addr_t physical_addr,
    phys_addr_t mapping_size,
    unsigned long virt)			//映射虚拟地址
{

}
