#include"paging.h"

/*-----------------------------------------------------------------------------
* 操作系统初始化内存分页
* @name: paging_init
* @function: 创建内存的恒等映射后开启MMU虚拟地址映射
* @retValue: 初始化内存分页是否成功
*------------------------------------------------------------------------------*/
Bool 
paging_init()
{
    init_memory_bitmap();               //初始化物理内存分配的位图
    debug_kernel_segment();             //输出内核段信息
    create_identical_mapping();         //创建内核的恒等映射
    create_mmio_mapping();              //创建MMIO的恒等映射

    init_cpu();                         //检查并初始化基础设置

    if(!enable_mmu(idmap_pg_dir)){                   //开启MMU
        console_puts("[paging module FAILED]:enable mmu FAILED!\n");
        return FALSE;
    }
    console_puts("[paging module SUCCESSED]:enable mmu successed!\n");
    
    char str[256] = {"current addr = "};
    itoa((int)paging_init,str+strlen(str),16);
    console_puts(str);
    console_putc('\n');

    /*将内核文件代码段.text映射到swapper_pg_dir PGD*/
    map_kernel_segment((page_global_directory *)swapper_pg_dir, (void*)_text_start, (void*)_text_end, NULL);
    /*将内核文件只读数据段.rodata区映射到swapper_pg_dir PGD*/
    map_kernel_segment((page_global_directory *)swapper_pg_dir, (void*)_rodata_start, (void*)_rodata_end, NULL);
    /*将内核文件数据段.data映射到swapper_pg_dir PGD*/
    map_kernel_segment((page_global_directory *)swapper_pg_dir, (void*)_data_start, (void*)_data_end, NULL);
    return TRUE;
}

/*-----------------------------------------------------------------------------
* 操作系统创建恒等映射，映射代码区和数据区
* @name: create_identical_mapping
* @function: 映射VA=PA的区域，主要目的就是为了打开MMU进行必要的准备
*------------------------------------------------------------------------------*/
void create_identical_mapping()
{
    /*代码段恒等映射*/
	phys_addr_t  address_start = (phys_addr_t)_text_start;   	//代码段起始
    phys_addr_t address_end = (phys_addr_t)_text_end;       	//代码段终止
    unsigned long area_size = address_end - address_start;      //代码段长度

	console_puts("[IDENTICAL MAPPING PROCESS]:START SEGMENT IDENTICAL MAPPING PROCESS NOW\n");
    __create_pgd_mapping(                       				//创建代码段恒等映射
        (page_global_directory *)idmap_pg_dir, 					//PGD恒等映射
        address_start,          								//物理起始地址
        address_start,          								//映射虚拟地址
        area_size,                             					//区域长度
        PAGE_KERNEL_ROX,                       					//属性只读可执行
        0,
        early_pagetable_alloc
    );
	console_puts("[IDENTICAL MAPPING PROCESS]: TEXT SEGMENT IDENTICAL MAPPING FINISHED!\n");

    /*只读数据段恒等映射*/
    address_start = (phys_addr_t)_rodata_start;             	//只读数据段起始
    address_end = (phys_addr_t)_rodata_end;                 	//只读数据段终止
    area_size = address_end - address_start;                	//只读数据段长度

     __create_pgd_mapping(                       				//创建只读数据段恒等映射
        (page_global_directory *)idmap_pg_dir,   				
        address_start,            								
        address_start,            
        area_size,                               
        PAGE_KERNEL_RO,                          				//属性内核只读普通页面
        0,
        early_pagetable_alloc
    );
	console_puts("[IDENTICAL MAPPING PROCESS]: RODATA SEGMENT IDENTICAL MAPPING FINISHED!\n");
	
    /*数据段恒等映射*/
    address_start = (phys_addr_t)_data_start;      				//数据段起始
    address_end = (phys_addr_t)_data_end;          				//数据段终止
    area_size = address_end - address_start;         			//数据段长度
    
    __create_pgd_mapping(                           			//创建数据段恒等映射
        (page_global_directory *)idmap_pg_dir,      			
        address_start,               
        address_start,               
        area_size,           
        PAGE_KERNEL,                                			//属性内核普通内存页面
        0,
        early_pagetable_alloc
    );
	console_puts("[IDENTICAL MAPPING PROCESS]: DATA SEGMENT IDENTICAL MAPPING FINISHED!\n");
    console_puts("[IDENTICAL MAPPING SUCCESSED]: identical mapping successed!\n");
}

/*-----------------------------------------------------------------------------
* 创建MMIO的恒等映射
* @name:  create_mmio_mapping
* @function: 创建MMIO的恒等映射(MMIO位于内存高地址处)
*------------------------------------------------------------------------------*/
void create_mmio_mapping()
{

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
* 打印内核段的物理加载地址信息(DEBUG专用)
* @name: debug_kernel_segment
* @function: 打印内核段的物理加载地址信息(DEBUG专用)
*------------------------------------------------------------------------------*/
void debug_kernel_segment()
{
    /*_text*/
    char str[256] = {"_text_start = "};
    itoa((UINT64)_text_start,str + strlen(str),16);
    strcat(str,"    _text_end = ");
    itoa((UINT64)_text_end,str + strlen(str),16);
    console_puts(str);
    console_putc('\n');

    /*rodata*/
    memset(str,0,256);
    strcpy(str,"_rodata_start = ");
    itoa((UINT64)_rodata_start,str + strlen(str),16);
    strcat(str,"     _rodata_end");
    itoa((UINT64)_rodata_end,str + strlen(str),16);
    console_puts(str);
    console_putc('\n');

    /*data*/
    memset(str,0,256);
    strcpy(str,"_data_start = ");
    itoa((UINT64)_data_start,str + strlen(str),16);
    strcat(str,"     _data_end");
    itoa((UINT64)_data_end,str + strlen(str),16);
    console_puts(str);
    console_putc('\n');

    /*swapper_pg_dir*/
    memset(str,0,256);
    strcpy(str,"swapper_pg_dir = ");
    itoa((UINT64)swapper_pg_dir,str + strlen(str),16);
    console_puts(str);
    console_putc('\n');

    /*idmap_pg_dir*/
    memset(str,0,256);
    strcpy(str,"idmap_pg_dir = ");
    itoa((UINT64)idmap_pg_dir,str + strlen(str),16);
    console_puts(str);
    console_putc('\n');
}