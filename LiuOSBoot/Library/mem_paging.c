#include"kernel_paging.h"

/*-----------------------------------------------------------------------------
* 分配一个页面用于存放页表
* @name: AllocatePageTable
* @function: 分配一个页面用于存放页表
*------------------------------------------------------------------------------*/
phys_addr_t
EFIAPI 
AllocatePageTable()
{
    EFI_PHYSICAL_ADDRESS kernelMemBuffer;                                       //重新为内核分配空间
    EFI_STATUS status = gBS->AllocatePages(                                      //分配空间
        AllocateAnyPages,
        EfiLoaderData,                                                         //加载器数据
        1,                                                                     //内核页面数量
        &kernelMemBuffer
    );
    if(EFI_ERROR(status)){
        return (phys_addr_t)0;
    }
    memset((void*)kernelMemBuffer, 0, PAGE_SIZE_LIUOS);                          //清空页面数据
    return kernelMemBuffer;
}

/*-----------------------------------------------------------------------------
* 引入虚拟地址用于装载内核
* @name: mappingKernelAddress
* @function: 如果内核的基址很大，超出了物理内存范围那么我们将没有办法执行内核
* @param :  1.段表数组(提供虚拟地址)
            2.已经加载到物理内存的内核程序
            3.恒等映射PGD(TTBR0_EL1)
            4.非恒等映射的内核基地址PGD(TTBR1_EL1)
            5.页面分配函数
*------------------------------------------------------------------------------*/
VOID 
EFIAPI
mappingKernelAddress(
    IN Elf64_Phdr * _elf64_segment,    //偏移到程序头表(段表)
    IN EFI_PHYSICAL_ADDRESS kernelBufferBase,
    IN struct page_global_directory *idmap_pg_dir,
    IN struct page_global_directory *swapper_pg_dir,
    phys_addr_t (*alloc_page)()
)
{
    /*代码段恒等和内核基地址映射*/
    Elf64_Phdr * text_segment = &_elf64_segment[KERNEL_TEXT];    //代码段
    if(text_segment->p_filesz){
        Elf64_Addr kernel_textseg_va = text_segment->p_vaddr;   //内核代码段虚拟地址VA
        Elf64_Addr kernel_textseg_pa =  (Elf64_Addr)(
            (UINT8*)kernelBufferBase + text_segment->p_offset   //内核代码段实际物理地址PA
        );

        __create_pgd_mapping(                                   //创建内核的恒等映射
            idmap_pg_dir,
            kernel_textseg_pa, kernel_textseg_pa,                //物理地址等于虚拟地址
            text_segment->p_filesz,
            PAGE_KERNEL_ROX,0,
            alloc_page
        );
        __create_pgd_mapping(                                   //创建非恒等映射的内核基地址PGD
            swapper_pg_dir,
            kernel_textseg_pa, kernel_textseg_va,                //物理地址映射内核基址
            text_segment->p_filesz,
            PAGE_KERNEL_ROX,0,
            alloc_page
        );
    }

    /*只读数据段恒等和内核基地址映射*/
    Elf64_Phdr * rodata_segment = &_elf64_segment[KERNEL_RODATA];//只读数据段
    if(rodata_segment->p_filesz){
        Elf64_Addr kernel_rodataseg_va = rodata_segment->p_vaddr;       //内核代码段虚拟地址VA
        Elf64_Addr kernel_rodataseg_pa = (Elf64_Addr)(
            (UINT8*)kernelBufferBase + rodata_segment->p_offset //内核代码段实际物理地址PA
        );
        __create_pgd_mapping(                                   //创建内核的恒等映射
            idmap_pg_dir,
            kernel_rodataseg_pa, kernel_rodataseg_pa,                //物理地址等于虚拟地址
            rodata_segment->p_filesz,
            PAGE_KERNEL_RO,0,
            alloc_page
        );
        __create_pgd_mapping(                                   //创建非恒等映射的内核基地址PGD
            swapper_pg_dir,
            kernel_rodataseg_pa, kernel_rodataseg_va,                //物理地址映射内核基址
            rodata_segment->p_filesz,
            PAGE_KERNEL_RO,0,
            alloc_page
        );
    }

    /*数据段恒等和内核基地址映射*/
    Elf64_Phdr * data_segment = &_elf64_segment[KERNEL_DATA];    //数据段
    if(data_segment->p_filesz){
        Elf64_Addr kernel_dataseg_va = data_segment->p_vaddr;       //内核代码段虚拟地址VA
        Elf64_Addr kernel_dataseg_pa = (Elf64_Addr)(
            (UINT8*)kernelBufferBase + data_segment->p_offset      //内核代码段实际物理地址PA
        );
        __create_pgd_mapping(                                   //创建内核的恒等映射
            idmap_pg_dir,
            kernel_dataseg_pa, kernel_dataseg_pa,                //物理地址等于虚拟地址
            data_segment->p_filesz,
            PAGE_KERNEL,0,
            alloc_page
        );
        __create_pgd_mapping(                                   //创建非恒等映射的内核基地址PGD
            swapper_pg_dir,
            kernel_dataseg_pa, kernel_dataseg_va,                //物理地址映射内核基址
            data_segment->p_filesz,
            PAGE_KERNEL,0,
            alloc_page
        );
    }

    /*BSS段恒等和内核基地址映射*/
}

/*-----------------------------------------------------------------------------
* 启动内存0~1GB的内存映射
* @name: mappingLow1GBMemory
* @function:启动内存0~1GB的内存映射(EFI的Runtime Services会用到)
* @param :  1.恒等映射PGD(TTBR0_EL1)
            3.页面分配函数
*------------------------------------------------------------------------------*/
VOID 
EFIAPI
mappingLow1GBMemory(
    IN struct page_global_directory *idmap_pg_dir,
    phys_addr_t (*alloc_page)()
)
{
    __create_pgd_mapping(                                   //创建恒等映射的内核基地址PGD
        idmap_pg_dir,
        0x0, 0x0,                                           //物理地址映射内核基址
        1024*1024*1024,                                      //1GB
        PAGE_KERNEL,0,
        alloc_page
    );
}

/*-----------------------------------------------------------------------------
* 启动内存分页
* @name: EnableMemoryPaging
* @function: 1.启动恒等映射以及非恒等内存基地址映射
             2.启动内存0~1GB的内存映射(EFI的Runtime Services会用到)
             3.通过操作寄存器启动MMU

* @param :  1.段表数组(提供虚拟地址)
            2.已经加载到物理内存的内核程序
            3.恒等映射PGD(TTBR0_EL1)
            4.非恒等映射的内核基地址PGD(TTBR1_EL1)
            5.页面分配函数
*------------------------------------------------------------------------------*/
VOID 
EFIAPI
EnableMemoryPaging(
    IN Elf64_Phdr * _elf64_segment,    //偏移到程序头表(段表)
    IN EFI_PHYSICAL_ADDRESS kernelBufferBase,
    IN struct page_global_directory *idmap_pg_dir,
    IN struct page_global_directory *swapper_pg_dir,
    phys_addr_t (*alloc_page)()
)
{
    /*创建内核的恒等以及非恒等的内核基地址映射*/
    mappingKernelAddress(
        _elf64_segment,
        kernelBufferBase,
        idmap_pg_dir,
        swapper_pg_dir,
        alloc_page
    );

    /*创建内存0~1GB的内存映射(EFI的Runtime Services会用到)映射*/
    mappingLow1GBMemory(
        idmap_pg_dir,
        alloc_page
    );
}

/*-----------------------------------------------------------------------------
* 初始化CPU的参数并开启MMU
* @name: EnableHardware
* @function: 初始化CPU的参数并开启MMU
* @param :  1.恒等映射PGD(TTBR0_EL1)
            2.非恒等映射的内核基地址PGD(TTBR1_EL1)

* @retvalue: 返回硬件是否初始化成功
*------------------------------------------------------------------------------*/
Bool
EFIAPI
EnableHardware(
    IN struct page_global_directory * idmap_pg_dir,
	IN struct page_global_directory * swapper_pg_dir
)
{
    Enable_CPU();                                                   //开启CPU
    return Enable_MMU(
        idmap_pg_dir,                                           //恒等映射
        swapper_pg_dir                                          //内核基地址映射
    );
}

/*-----------------------------------------------------------------------------
* 进入了分页模式，使用了虚拟地址，我们需要通知EFI更改他内部的指针，以适应这个变化
* @name: prepareVirtualMemoryMapping
* @function: 1.退出BOOTSERVICE并使用RUNTIMESERVICE设置他的VirtualStart通知EFI更改指针
             2.如果启动错误则执行软重置系统

* @param :   1.传入系统的ImageHandle参数
             2.传入MEMORY_MAP_CONFIG参数表
            
* @retValue：返回系统状态信息
*------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI 
prepareVirtualMemoryMapping(
    IN EFI_HANDLE ImageHandle,
    OUT MEMORY_MAP_CONFIG *MemoryMap)
{
    EFI_STATUS status = loadMemoryMap(MemoryMap);
    if(EFI_ERROR(status)){
        Print(L"Load Memory Map Failed! Error code = %x\n",status);
        goto uefi_error;
    }
    /*退出BOOT SERVICES*/
    gBS->ExitBootServices(ImageHandle,MemoryMap->m_MapKey); 

    /*------------------------------------------------------------
     *特殊处理EFI_MEMORY_RUNTIME类型的内存区块
    
    EFI_MEMORY_DESCRIPTOR *memoryMapEntry = ( EFI_MEMORY_DESCRIPTOR *)MemoryMap->m_MemoryMap;//内存区块入口
    UINTN memoryMapEntries = MemoryMap->m_MemoryMapSize / MemoryMap->m_DescriptorSize;       //多少个内存块
    for(UINTN i = 0 ; i < memoryMapEntries ; ++i){
        //前1GB内存采用PA=VA
        if(memoryMapEntry->Attribute  & EFI_MEMORY_RUNTIME){
            memoryMapEntry->VirtualStart = memoryMapEntry->PhysicalStart;
        }
        memoryMapEntry = (EFI_MEMORY_DESCRIPTOR *)((char*)memoryMapEntry + MemoryMap->m_DescriptorSize);
    }

    status = gRT->SetVirtualAddressMap(
        MemoryMap->m_MemoryMapSize,                         //内存描述符数组总大小
        MemoryMap->m_DescriptorSize,                        //每一个描述符的大小
        EFI_MEMORY_DESCRIPTOR_VERSION,
        MemoryMap->m_MemoryMap                              //入口地址
    );
     ------------------------------------------------------------------------------*/

    if(EFI_ERROR(status)){
uefi_error:
        gRT->ResetSystem(EfiResetWarm, EFI_LOAD_ERROR, 62, (CHAR16*)L"Setting Memory mapping failed.");
        return status;
    }
    return EFI_SUCCESS;
}

/*-----------------------------------------------------------------------------
* 初始化CPU
* @name: Enable_CPU
* @function:初始化CPU
*------------------------------------------------------------------------------*/
VOID 
EFIAPI 
Enable_CPU()
{
    asm volatile("ic iallu");      
    asm volatile("tlbi vmalle1");           //使当前EL1的TLB所有表项全部失效
    asm volatile("dsb ish": : : "memory");
    /*---写入MAIR_EL1内存属性设置寄存器--------
    *   MT_DEVICE_nGnRnE  000 00000000 
    *   MT_DEVICE_nGnRE   001 00000100
    *   MT_DEVICE_GRE     010 00001100
    *   MT_NORMAL_NC      011 01000100
    *   MT_NORMAL         100 11111111
    *   MT_NORMAL_WT      101 10111011
    *----------------------------------------*/
    unsigned long mairel1_val ={
        MAIR( MT_DEVICE_nGnRnE , 0x00UL) |
        MAIR( MT_DEVICE_nGnRE , 0x04UL) |
        MAIR( MT_DEVICE_GRE , 0x0CUL) |
        MAIR( MT_NORMAL_NC , 0x44UL) |
        MAIR( MT_NORMAL , 0xFFUL) |
        MAIR( MT_NORMAL_WT , 0xBBUL)
    }; 
    //write_sysreg(mairel1_val,mair_el1);    
    /*写入MAIR_EL1内存属性设置寄存器*/
	asm volatile("msr mair_el1, %x0"::"rZ"(mairel1_val)); 

    //unsigned long max_physical_mem = read_sysreg(ID_AA64MMFR0_EL1) & 0xF;
    /*读取系统支持的最大物理内存*/
    unsigned long max_physical_mem = 0;
    asm volatile("mrs %0, ID_AA64MMFR0_EL1" : "=r" (max_physical_mem)); 
    max_physical_mem &= 0xF;                                //提取有效位

    if(max_physical_mem> ID_AA64MMFR0_PARANGE_48){           //如果系统物理内存数量大于48位(256TB)      
        max_physical_mem = ID_AA64MMFR0_PARANGE_48;         //将系统物理内存加以限制
    }

    Print(L"[CPU INIT]:MAX PHYSICAL MEMORY CODE = %d!\n",max_physical_mem);

    unsigned long tcr_reg = {
        TCR_TxSZ(VA_BITS) |                 //地址最大值参数
        TCR_TG1_4K |                        //TTBR1页面粒度
        TCR_TG0_4K                          //TTBR0页面粒度
    };
    tcr_reg |= (max_physical_mem << TCR_IPS_SHIFT);  //配置地址转换MMU后输出物理地址的最大值

    //write_sysreg(tcr_reg, SYS_TCR_EL1);
    /*写入tcr_el1寄存器*/
	asm volatile("msr tcr_el1, %x0"::"rZ"(tcr_reg)); //写入tcr_el1寄存器
    Print(L"[CPU INIT]:CPU INIT FINISHED!\n");
}

/*-----------------------------------------------------------------------------
* 启动MMU硬件
* @name: Enable_MMU
* @function: 启动MMU硬件并将地址写入硬件寄存器中
* @param :  1.恒等映射PGD(TTBR0_EL1)
            2.非恒等映射的内核基地址PGD(TTBR1_EL1)

* @retvalue: 返回MMU是否初始化成功
*------------------------------------------------------------------------------*/
Bool 
EFIAPI 
Enable_MMU(
	IN struct page_global_directory * idmap_pg_dir,
	IN struct page_global_directory * swapper_pg_dir)
{
    /*ID_AA64MMFR0_EL1的tgran4字段判断系统是否支持4KB粒度*/
    unsigned long tgran4;
    asm volatile("mrs %0, ID_AA64MMFR0_EL1": "=r" (tgran4):); 
    tgran4 = (tgran4 >> ID_AA64MMFR0_TGRAN4_SHIFT) & 0xF;
    if(tgran4 != ID_AA64MMFR0_TGRAN4_SUPPORTED){            //不支持4KB页面粒度
        return FALSE;
    }

    /*-----------------------------------------------------------
    *设置PGD页表基地址的恒等映射到TTBR0_EL1寄存器中
    -----------------------------------------------------------*/
    
    asm volatile("msr TTBR0_EL1, %0"::"rZ"(idmap_pg_dir)); //设置恒等映射页表
    asm volatile("msr TTBR1_EL1, %0"::"rZ"(swapper_pg_dir)); //设置恒等映射页表
    asm volatile("isb" : : : "memory");                  

    asm volatile("mrs x0, SCTLR_EL1"::); 
    asm volatile("orr x0,x0,#1"::); 
    asm volatile("msr SCTLR_EL1, x0"::); 
    
    asm volatile("isb" : : : "memory");                                      //CPU重新取指令
    asm("ic iallu");                                                         //使得所有指令的高速缓存cache失效
    asm volatile("dsb nsh": : : "memory");                                    //非共享（non-shareable），表示数据同步屏障指令仅仅在当前核起作用
    asm volatile("isb" : : : "memory");                                      //CPU重新取指令(从告诉缓存取指令)
    return TRUE;
}