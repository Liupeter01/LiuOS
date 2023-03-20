#include"page-table.h"

/*-----------------------------------------------------------------------------
* 获取物理内存的空闲页面
* @name: get_free_page
* @function: 获取物理内存的空闲页面
* @retvalue：返回物理地址
*------------------------------------------------------------------------------*/
unsigned long get_free_page()
{

}

/*-----------------------------------------------------------------------------
* 释放物理内存
* @name: free_page
* @function: 释放物理内存
* @param:1.传入地址
*------------------------------------------------------------------------------*/
void free_page(unsigned long addr)
{


}

/*-----------------------------------------------------------------------------
* 用于在系统没有初始化页表之前进行内存分配的函数
* @name: early_pagetable_alloc
* @function: 分配一个4KB的页面用于页表
* @param : 1.
*------------------------------------------------------------------------------*/
unsigned long early_pagetable_alloc()
{
    unsigned long physcial_addr = get_free_page();
    memset((void*)physcial_addr,0,PAGE_SIZE);
    return physcial_addr;
}

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
)
{
    page_global_directory *m_pgd = pgd + pgd_index(virt);    //偏移到指定PGD页表项
    m_pgd+=1;
    //for(1)                                                  //在L0上创建下一级PUD(L1)
    {
        
    }
}

/*-----------------------------------------------------------------------------
* 操作系统创建恒等映射，映射代码区和数据区
* @name: create_identical_mapping
* @function: 映射VA=PA的区域，主要目的就是为了打开MMU进行必要的准备
*------------------------------------------------------------------------------*/
void create_identical_mapping()
{
    /*代码段恒等映射*/
    unsigned long address_start = (unsigned long)_text_start;   //代码段起始
    unsigned long address_end = (unsigned long)_text_end;       //代码段终止
    unsigned long area_size = address_end - address_start;      //代码段长度

    __create_pgd_mapping(                       //创建代码段恒等映射
        (page_global_directory *)idmap_pg_dir, //PGD
        (unsigned long)address_start,          //物理起始地址
        (unsigned long)address_start,          //映射虚拟地址
        area_size,                             //区域长度
        PAGE_KERNEL_ROX,                       //属性只读可执行
        0,
        early_pagetable_alloc
    );

    /*只读数据段恒等映射*/
    address_start = (unsigned long)_data_start;             //只读数据段起始
    address_end = (unsigned long)_data_end;                 //只读数据段终止
    area_size = address_end - address_start;                //只读数据段长度

     __create_pgd_mapping(                       //创建只读数据段恒等映射
        (page_global_directory *)idmap_pg_dir,   //创建恒等映射
        (unsigned long)address_start,            //物理起始地址
        (unsigned long)address_start,            //映射虚拟地址
        area_size,                               //区域长度
        PAGE_KERNEL_RO,                          //属性内核只读普通页面
        0,
        early_pagetable_alloc
    );

    /*数据段恒等映射*/
    address_start = (unsigned long)_data_start;      //数据段起始
    address_end = (unsigned long)_data_end;          //数据段终止
    area_size = address_end - address_start;         //数据段长度
    
    __create_pgd_mapping(                           //创建数据段恒等映射
        (page_global_directory *)idmap_pg_dir,      //创建恒等映射
        (unsigned long)address_start,               //物理起始地址
        (unsigned long)address_start,               //映射虚拟地址
        area_size,                                  //区域长度
        PAGE_KERNEL,                                //属性内核普通内存页面
        0,
        early_pagetable_alloc
    );
}