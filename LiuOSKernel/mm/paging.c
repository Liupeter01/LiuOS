#include"paging.h"

/*-----------------------------------------------------------------------------
* 操作系统初始化内存分页
* @name: paging_init
* @function: 
* @param : 
* @retValue: 
*------------------------------------------------------------------------------*/
void 
paging_init()
{
    /*
    for(int i = 0; i < g_MemoryDistribution.m_infoCount ; ++i){          //遍历内存布局信息结构数组
        MM_INFORMATION * mm_info = &g_MemoryDistribution.m_infoArr[i];
        phys_addr_t start = mm_info->PhysicalStart;                     //物理内存起始
        phys_addr_t end = mm_info->PhysicalStart + (NumberOfPages << 12);//物理内存终点
        __create_pgd_mapping(
            swapper_pg_dir,                 //内核PDG
            start,                          //物理起始地址
            end-start,                      //物理地址大小
            __phys_to_virt(start),          //物理线性地址转换
            //XXXXXX??????XXXXXX
        );
    }
    */
    /*将内核文件代码段.text映射到swapper_pg_dir PGD*/
    map_kernel_segment((page_global_directory *)swapper_pg_dir, (void*)_text_start, (void*)_text_end, NULL);
    /*将内核文件只读数据段.rodata区映射到swapper_pg_dir PGD*/
    map_kernel_segment((page_global_directory *)swapper_pg_dir, (void*)_rodata_start, (void*)_rodata_end, NULL);
    /*将内核文件数据段.data映射到swapper_pg_dir PGD*/
    map_kernel_segment((page_global_directory *)swapper_pg_dir, (void*)_data_start, (void*)_data_end, NULL);
}

/*-----------------------------------------------------------------------------
* 操作系统创建链接文件中关键内核映像中段的映射
* @name: paging_init
* @function: 操作系统创建链接文件中关键内核映像中段的映射
* @param : 1.全局目录PGD(Kernel为swapper_pg_dir)
           2.映射虚拟地址
           3.虚拟地址长度
           4.一个页面的分配方式 
*------------------------------------------------------------------------------*/
void 
map_kernel_segment(
    page_global_directory *pgd,
    void* virtual_addr,
    void* virtual_end,
    phys_addr_t (*alloc_method)(void))   //页表PDG的分配方式
{

}

/*-----------------------------------------------------------------------------
* 关闭TLB的结构
* @name: disable_tlb_cache
* @function: 关闭TLB的结构
*------------------------------------------------------------------------------*/
void disable_tlb_cache()
{
#ifdef __aarch64__
    asm volatile("TLBI ALLE1"::);       //invalid all TLB entries
#endif //__aarch64__
#ifdef __x86_64__
    asm volatile(""::);
#endif //__aarch64__
}