#include"memory.h"

/*-----------------------------------------------------------------------------
* 创建PGD(L0)的页表映射
* @name: __create_pgd_mapping
* @function: 创建PGD(L0)的页表映射
* @param : 1.全局目录PGD(Kernel为swapper_pg_dir)
           2.物理地址
           3.地址长度
           4.映射虚拟地址
           5.一个页面的分配方式
*------------------------------------------------------------------------------*/
void __create_pgd_mapping(
    page_global_directory *pgd,
    phys_addr_t physical_addr,
    phys_addr_t mapping_size,
    unsigned long virt,                 //映射虚拟地址
    phys_addr_t (*alloc_method)(void))    //页表PDG的分配方式	
{
    page_global_directory *m_pgd = pgd + pgd_index(virt);    //偏移到指定PGD页表项
    m_pgd+=1;
    //for(1)                                                  //在L0上创建下一级PUD(L1)
    {
        
    }
}
