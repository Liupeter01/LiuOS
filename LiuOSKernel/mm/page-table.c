#include"page-table.h"

inline phys_addr_t pgd_page_to_physical_address(page_global_directory pgd){
    return __pgd_to_phys(pgd);
}

inline page_table_entry pgd_pte(page_global_directory pgd){
	return __pte(pgd_val(pgd));
}

inline page_table_entry pud_pte(page_upper_directory pud){
	return __pte(pud_val(pud));
}

inline page_upper_directory pte_pud(page_table_entry pte){
	return __pud(pte_val(pte));
}

inline page_middle_directory pud_pmd(page_upper_directory pud){
	return __pmd(pud_val(pud));
}

inline page_table_entry pmd_pte(page_middle_directory pmd){
	return __pte(pmd_val(pmd));
}

inline page_middle_directory pte_pmd(page_table_entry pte){
	return __pmd(pte_val(pte));
}

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
*------------------------------------------------------------------------------*/
void 
__create_pgd_mapping(
    page_global_directory *pgd,
    UINT64 physical_addr,
    UINT64 virt,                 //映射虚拟地址
    UINT64 mapping_size,
    unsigned long attribute,
    unsigned long flags,
    UINT64 (*alloc_method)(void)    //页表PDG的分配方式	
)
{
    page_global_directory *pgd_entry = pgd + pgd_index(virt);    //偏移到指定PGD页表项pgd[???]
    UINT64 virt_end = PAGE_ALIGN(virt + mapping_size);           //虚拟地址+地址长度对应的终止地址(考虑4KB对齐)
    UINT64 pa_addr = physical_addr & PAGE_MASK;                 //

    /*需要创建的内存区域可能很大，需要以PGDIR_SIZE步长进行遍历*/
    for(UINT64 va_start = virt & PAGE_MASK ; va_start!=virt_end ; va_start=is_pgd_arrive_end(va_start,virt_end))
    {  
        UINT64 va_end = is_pgd_arrive_end(va_start,virt_end);                //获得当前PUD段的终止地址
        alloc_pgtable_pud(                                             //将PGD页表项传递到PUD中       
            pgd_entry++,
            va_start,                                                   //虚拟起始
            va_end,                                                     //虚拟终止
            pa_addr,                                                    //对应物理起始
            attribute,
            flags,
            alloc_method
        );
        pa_addr += (va_end - va_start);                                 //进行物理地址的偏移                     
    }
}

/*-----------------------------------------------------------------------------
* 创建PUD(L1)的页表映射
* @name: alloc_pgtable_pud
* @function: 1.如果pgd中的内容(*pgd)为空，则其中必定没有创建PUDL1页表
             2.创建PUD(L1)的页表并分配物理内存，？？？
                创建PUD(L1)的页表映射，同时填充高级PGD(L0)的链接地址形成链接PGD->PUD
* @param : 1.全局目录PGD
           2.虚拟地址起始(为PUD修正后的虚拟地址)
           3.虚拟地址终止(为PUD修正后的虚拟地址)
           4.物理地址(为PUD修正后的物理地址)
           5.创建映射的内存属性
           6.页表创建过程的标识位
           7.分配下级页表的内存分配函数
*------------------------------------------------------------------------------*/
void 
alloc_pgtable_pud(
    page_global_directory *pgd,
    UINT64 va_pud_start,            //映射虚拟地址起始
    UINT64 va_pud_end,              //映射虚拟地址终止
    UINT64 physical_addr_pud,
    unsigned long attribute,
    unsigned long flags,
    UINT64 (*alloc_method)(void)    //页表PDG的分配方式	
)
{
    //page_upper_directory m_pud;                                //新建 PUD结构
    page_global_directory L0 = *pgd;                             //取出 PGD结构的数据
    if(!pgd_val(L0)){                                            //如果PGD中没有关于PUD的内容(PGD没有PUD地址)则新建一个PUD
        UINT64 pud_pa_address = alloc_method();                  //获取一个物理地址(大小4KB)
        set_l0_l1_connection(                                    //创建链接PGD->PUD
             pgd,
             pud_pa_address, 
             PUD_TYPE_TABLE
        ); 

        L0 = *pgd;                                               //更新数字hi
    }

    //page_global_directory updated_pgd_entry = READ_ONCE(pgd);        //获取最新PGD页表项中的地址数值
    page_global_directory updated_pgd_entry = (*(volatile typeof(pgd) ) pgd);


    page_upper_directory *L1_target_entry =                          //偏移到指定PUD页表项pud[???]
        (page_upper_directory*)pgd_page_to_physical_address(updated_pgd_entry) + pud_index(va_pud_start);

    /*------------------------------------------------------------------------------
    *创建多个PMD层(L2)的页表
    *------------------------------------------------------------------------------*/
    UINT64 pa_addr = physical_addr_pud;                 //物理地址初始化
    for(UINT64 va_start = va_pud_start ; va_start!=va_pud_end ; va_start=is_pud_arrive_end(va_start,va_pud_end))
    {  
        UINT64 va_end = is_pgd_arrive_end(va_start,va_pud_end);                //获得当前PUD段的终止地址
        alloc_pgtable_pmd(                                             //将PGD页表项传递到PUD中       
            L1_target_entry++,                                          //PUD地址传入
            va_start,                                                   //虚拟起始
            va_end,                                                     //虚拟终止
            pa_addr,                                                    //对应物理起始
            attribute,
            flags,
            alloc_method
        );
        pa_addr += (va_end - va_start);                                 //进行物理地址的偏移                     
    }
}

/*-----------------------------------------------------------------------------
* 创建PMD(L2)的页表映射
* @name: alloc_pgtable_pmd
* @function: 1.
             2.填充PUD的指定页表项链接PMD，形成链接PGD->PUD->PMD
                
* @param : 1.PUD
           2.虚拟地址起始(为PMD修正后的虚拟地址)
           3.虚拟地址终止(为PMD修正后的虚拟地址)
           4.物理地址(为PUD修正后的物理地址)
           5.创建映射的内存属性
           6.页表创建过程的标识位
           7.分配下级页表的内存分配函数
*------------------------------------------------------------------------------*/
void alloc_pgtable_pmd(
    page_upper_directory *pud,
    UINT64 va_pmd_start,            //映射虚拟地址起始
    UINT64 va_pmd_end,              //映射虚拟地址终止
    UINT64 physical_addr_pmd,
    unsigned long attribute,
    unsigned long flags,
    UINT64 (*alloc_method)(void)    //页表PDG的分配方式	
)
{
    
}

/*-----------------------------------------------------------------------------
* PGD(L0)和PUD(L1)之间的关联
* @name: set_l0_l1_connection
* @function: 创建 PGD(L0)和PUD(L1)之间的关联
* @param : 1.全局目录PGD
           2.PUD
           3.页面属性(标记第三位为页表类型TABLE类型而不是页面)

* @bug:  2023-3-21 可能存在进程线程冲突，毕竟锁机制还没有开发
*------------------------------------------------------------------------------*/
void 
set_l0_l1_connection( 
    page_global_directory *pgd,
    UINT64 pud_addr,
    pgdval_t attribute)
{
    //将指向PUD页面地址和页面属性填充在PGD表项中
    page_global_directory pgd_entry = __pgd(__phys_to_pte_val(pud_addr) | attribute);   //最后几位加上属性
    //lock();
    //WRITE_ONCE(*pgd,pgd_entry);               //强制写内存


    *(volatile typeof(pgd)) pgd = pgd_entry;
    //unlock();
}

/*-----------------------------------------------------------------------------
* 取消PGD(L0)和PUD(L1)之间的关联
* @name: close_l0_l1_connection
* @function: 断开PGD(L0)和PUD(L1)之间的关联
* @param : 1.全局目录PGD
* @bug:  2023-3-21 可能存在进程线程冲突，毕竟锁机制还没有开发
*------------------------------------------------------------------------------*/
void 
close_l0_l1_connection( 
    page_global_directory *pgd
)
{
    //lock();
    *(volatile typeof(pgd)) pgd = __pgd(0);
    //WRITE_ONCE(*pgd,__pgd(0));               //强制写内存
    //unlock();
}