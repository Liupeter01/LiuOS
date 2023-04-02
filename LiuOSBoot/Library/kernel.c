#include"kernel_reader.h"
#include"kernel_paging.h"

/*-----------------------------------------------------------------------------
* 通过GetMemoryMap获取内存布局
* @name: loadMemoryMap
* @function: 将多个内存布局结构存储在结构体中，并退出启动时服务BOOT_SERVICES
* @param :  传入MEMORY_MAP_CONFIG参数表
* @retValue：返回系统状态信息
*------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI 
loadMemoryMap(OUT MEMORY_MAP_CONFIG *MemoryMap)
{
    EFI_STATUS status = gBS->AllocatePool(                     //进行EFI_MEMORY_DESCRIPTOR内存的分配
        EfiLoaderData,
        MemoryMap->m_MemoryMapSize,                 //传入EFI_MEMORY_DESCRIPTOR数组的整体大小
        &MemoryMap->m_MemoryMap                     //初始化EFI_MEMORY_DESCRIPTOR结构
    );  
    status = gBS->GetMemoryMap(          
        &MemoryMap->m_MemoryMapSize,                    //缓冲区大小
        (EFI_MEMORY_DESCRIPTOR*)MemoryMap->m_MemoryMap, //EFI_MEMORY_DESCRIPTOR结构
        &MemoryMap->m_MapKey,
        &MemoryMap->m_DescriptorSize,                   //每一个描述符的大小
        &MemoryMap->m_DescriptorVersion                 //描述符的版本
    );
    if(status == EFI_BUFFER_TOO_SMALL){
        gBS->FreePool(MemoryMap->m_MemoryMap );         //释放内存并重新分配
        status = gBS->AllocatePool(                     //进行EFI_MEMORY_DESCRIPTOR内存的分配
            EfiLoaderData,
            MemoryMap->m_MemoryMapSize,                 //传入EFI_MEMORY_DESCRIPTOR数组的整体大小
            &MemoryMap->m_MemoryMap                     //初始化EFI_MEMORY_DESCRIPTOR结构
        );  
        if (EFI_ERROR(status)) {
            Print(L"Memory Map AllocatePool Failed! error code = %d\n",status);
            gBS->FreePool(MemoryMap->m_MemoryMap );     //释放内存
            return status;
        }
        status = gBS->GetMemoryMap(
            &MemoryMap->m_MemoryMapSize,                    //缓冲区大小
            (EFI_MEMORY_DESCRIPTOR*)MemoryMap->m_MemoryMap, //缓冲区起始内存地址
            &MemoryMap->m_MapKey,
            &MemoryMap->m_DescriptorSize,                   //每一个描述符的大小
            &MemoryMap->m_DescriptorVersion                 //描述符的版本
        );
        if (EFI_ERROR(status)) {
            Print(L"Get Memory Map Failed! error code = %d\n",status);
            gBS->FreePool(MemoryMap->m_MemoryMap );     //释放内存
            return status;
        }
    }
    Print(L"MemoryMap Array addr = 0x%x\n",MemoryMap->m_MemoryMap);
    return EFI_SUCCESS;
}

/*-----------------------------------------------------------------------------
* 将内核ELF文件加载到内存中
* @name: loadKerneltoMemory
* @function: 将加载到内存中的ELF文件加载到内存中
* @param :  1.传入系统的ImageHandle参数
            2.Kernel文件的打开描述符

* @retValue：返回当前内存加载的内核ELF缓冲区(堆内存)，为0时则代表分配失败
*------------------------------------------------------------------------------*/
EFI_PHYSICAL_ADDRESS 
EFIAPI
loadKerneltoMemory(
    IN EFI_HANDLE ImageHandle,
    IN OUT EFI_FILE_DESCRIPTOR *kernelFile
)
{
    if(getFileHandle(ImageHandle,KERNEL_FILE_PATH,kernelFile)){         //获取内核ELF文件句柄成功
        if(!EFI_ERROR (mapFiletoMemory(kernelFile))){                   //加载内核文件入内存成功
            return kernelFile->mEfiFileBuffer;                          //返回产生成功的地址
        }
    }
    return 0;                                                            //加载失败
}

/*-----------------------------------------------------------------------------
* 检查内核ELF文件的合法性
* @name: checkELFFormat
* @function: 1.确定ELF是否有固定magic number开头(0x464C457F)
             2.确定ELF文件为有效的64位文件

* @param :  1.传入载入内核文件缓冲区地址
* @retValue：返回当前内存加载的内核ELF的状态
            #define ELF_NOT_EXIST (0)                   //ELF不存在
            #define ELF_LOADING_SUCCESS (1)             //ELF格式检测成功
            #define ELF_FORMAT_ERROR (3)                //ELF格式错误
            #define ELF_NONE_64BIT (4)                  //ELF不是64位的
*------------------------------------------------------------------------------*/
UINT32 
EFIAPI
checkELFFormat(IN EFI_PHYSICAL_ADDRESS kernelBuffer)
{
    if((EFI_PHYSICAL_ADDRESS*)kernelBuffer == NULL){            //没有数据
        Print(L"[BOOTLOADER]:File Not Exist!!\n");
        return ELF_NOT_EXIST;   
    }
    Elf64_Ehdr * _elf64_header = (Elf64_Ehdr*)kernelBuffer;     //强制类型转换
    if(*((UINT32*)_elf64_header) != ELF_MAGIC_NUMBER){          //读取ELF的头4字节的数据判断ELF格式是否正确
        Print(L"[BOOTLOADER]:File Format Error!!\n");
        return ELF_FORMAT_ERROR;
    }
    if(_elf64_header->e_ident[sizeof(UINT32)] != IS_ELF_64BIT){ //读取ELF的第5字节的数据判断位数
        Print(L"[BOOTLOADER]:File Not Exist!!\n");
        return ELF_NONE_64BIT;
    }
    Print(L"[BOOTLOADER]:File Loading Success!!\n");
    return ELF_LOADING_SUCCESS;                                   //ELF格式检测成功
}

/*-----------------------------------------------------------------------------
* 得到ELF文件中所有节的总页数
* @name: calculateKernelSize
* @function: 根据段表中的数据，通过选择排序法确定最大值和最小值
* @param :  1.段表数组基地址
            2.段表中PT_LOAD段数量
            3.原始内核低地址
            4.原始内核高地址

* @retvalue:返回UINT64类型的内核大小页面数量
* @update:
            !!!2023-3-30 : 修复内核页面分配的bug(修复默认只能加载4KB大小的内核的错误)
                           修复重大程序bug，原先程序忽略了传参为指针计算的是传参指针的差距导致内核错误
*------------------------------------------------------------------------------*/
UINT64 
EFIAPI 
calculateKernelSize(
  IN struct elf64_phdr * section_table,              //段表的基地址
  IN Elf64_Half section_table_count,                //段表中元素数量
  IN OUT UINT64 *kernelLowAddress,            //原始内核低地址
  IN OUT UINT64 *kernelHighAddress)           //原始内核高地址
{
    for(Elf64_Half i = 0 ; i < section_table_count ; ++i){         //遍历段表寻找当前段表最低和最高地址
        if(section_table[i].p_type == PT_LOAD){                    //段表必须属于可加载类型PT_LOAD
            if(*kernelLowAddress > section_table[i].p_paddr){       //低地址高于当前段的最低物理地址
                *kernelLowAddress = section_table[i].p_paddr;
            }
            if(*kernelHighAddress < section_table[i].p_paddr+section_table[i].p_memsz){ //高地址低于当前段的最高物理地址
                *kernelHighAddress = section_table[i].p_paddr+section_table[i].p_memsz;
            }
        }
    }
    return (((*kernelHighAddress - *kernelLowAddress) >> 12) + 1);      //计算当前重定位内核的页面数
}


/*-----------------------------------------------------------------------------
* ELF内核重定位拷贝函数
* @name: relocateKernel
* @function: ELF内核重定位拷贝程序段，并将偏移量进行返回
* @param :  1.段表数组基地址
            2.段表中PT_LOAD段数量
            3.原始ELF文件内存映射基地址
            4.原始内核低地址
            5.重定位的内核基地址

* @retvalue:返回重定位内核基地址和原始内核低地址的偏移参数
*------------------------------------------------------------------------------*/
UINT64
EFIAPI
relocateKernel(
  IN struct elf64_phdr * section_table,               //段表的基地址
  IN Elf64_Half section_table_count,                 //段表中元素数量
  IN EFI_PHYSICAL_ADDRESS kernelBufferBase,          //原始ELF文件内存映射基地址
  IN EFI_PHYSICAL_ADDRESS kernelLowAddress,          //原始内核低地址
  IN EFI_PHYSICAL_ADDRESS kernelReallocBase)         //重定位的内核基地址
{
    UINT64 reallocOffset = kernelReallocBase - kernelLowAddress;          //计算原始地址到新程序地址的偏移
    for(Elf64_Half i = 0 ; i < section_table_count ; ++i){                      //遍历段表进行内存数据拷贝
        if( section_table[i].p_type == PT_LOAD){                           //段表必须属于可加载类型PT_LOAD
            UINT8 *src = (UINT8*)kernelBufferBase + section_table[i].p_offset;//寻找每一个段表位置 
            UINT8* dst = (UINT8*)section_table[i].p_paddr                   ;//获取偏移后的目标写入地址
            Print(L"src=%X ,Dst=%X ,Size=%X\n",src,dst,section_table[i].p_filesz);

            for(UINT64 j = 0 ; j < section_table[i].p_filesz ; ++j){
                *dst = *src;
                dst++;
                src++;
            }
        }
    }
    return reallocOffset;
}

/*-----------------------------------------------------------------------------
* 在文件映射的内存中寻找可加载段并将段重定位到新的地址
* @name: loadkernelSegments
* @function: 根据ELF的内核文件缓冲区的数据计算PT_LOAD内核段中的内核加载地址
* @param : 1.kernel.elf文件映射内存的首地址(属于内核文件)
           2.提取有效的PT_LOAD段到新地址(属于加载到内存的二进制代码)

  @***warning***:编译不需要链接标准库stdlib。彻底消除_start
* @retValue：返回内核所在段的加载是否成功
* @update:
            !!!2023-2-30 : 修复内核页面分配的bug(修复默认只能加载4KB大小的内核的错误)
            !!!2023-3-24 : 当前版本即将去除重定位内核，启动MMU分页
*------------------------------------------------------------------------------*/
EFI_PHYSICAL_ADDRESS
EFIAPI
loadkernelSegments(
  IN EFI_PHYSICAL_ADDRESS fileBufferBase,
  OUT EFI_PHYSICAL_ADDRESS *relocateAddress)
{
    Print(L"kernelBuffer(Base) = %X\n",fileBufferBase);
    /*-----------------------------------------------------------------------------
    * 计算重定位内核大小
    *------------------------------------------------------------------------------*/
    Elf64_Ehdr * _elf64_header = (Elf64_Ehdr*)fileBufferBase;           //强制类型转换用于获取段表起始偏移
    Elf64_Phdr * _elf64_segment = (Elf64_Phdr *)(_elf64_header + 1); //偏移到程序头表(段表)
    Elf64_Half _segment_count = _elf64_header->e_phnum;               //段表的数量
    Elf64_Addr e_entry = _elf64_header->e_entry;                      //获取原始入口

    Print(L"Segment Count = %X\nEntry_point = %X\n",_segment_count,e_entry);
    UINT64 kernelLowAddress = ~0;                       //原始内核低地址
    UINT64 kernelHighAddress = 0;                       //原始内核高地址
    UINT64 kernelPageCount = calculateKernelSize(       //计算当前重定位内核的页面数
        _elf64_segment,                                  
        _segment_count,                                 
        &kernelLowAddress,
        &kernelHighAddress
    );
    Print(L"kernelLowAddress = %X\nkernelHighAddress = %X\n",kernelLowAddress,kernelHighAddress);

    EFI_PHYSICAL_ADDRESS kernelMemBuffer = 0x40000000;                         //重新为内核分配空间
    EFI_STATUS status = gBS->AllocatePages(                                      //分配空间
        AllocateAddress,
        EfiLoaderData,                                                         //加载器数据
        kernelPageCount,                                                       //内核页面数量
        &kernelMemBuffer
    );
    if(EFI_ERROR(status))
    {
        Print(L"Kernal Space Allocate Failed! status = %d\n \
               Remove Original Kernel File From System!!!\n",status);
        return 0;
    }
    *relocateAddress = kernelMemBuffer;                           //将内核重定位地址传递到外部

    Print(L"kernelMemBuffer = %X\nKernelPageCount = 0x%X\n",kernelMemBuffer,kernelPageCount);
    Print(L"OK\n");
    /*-----------------------------------------------------------------------------
    * 执行内核的重定位和拷贝工作
    *-----------------------------------------------------------------------------*/
    UINT64 relocateOffset = relocateKernel(                          //ELF内核重定位拷贝函数
        _elf64_segment,                                             //段表基地址
        _segment_count,                                             //段表的数量
        fileBufferBase,
        kernelLowAddress,
        kernelMemBuffer
    );
    Print(L"relocateOffset = %X\n",relocateOffset);
    return e_entry;                                              //返回重定位的入口点main
}

/*-----------------------------------------------------------------------------
* 初始化内核的启动程序
* @name: InitKernelStartup
* @function: * 初始化内核的启动程序
* @param :  1.传入系统的ImageHandle参数
            2.传入系统的图形设置
            3.传入系统的字体功能
            4.传入内核文件
            5.传入内核参数

* @warning: 注意，仅用于加载常用参数，其他部分内容将在入口点加载函数进行执行
* @retvalue: 返回初始化是否成功
*------------------------------------------------------------------------------*/
Bool 
EFIAPI
InitKernelStartup(
    IN EFI_HANDLE ImageHandle,
    IN OUT GRAPHIC_CONFIG *efiGraphicConfig,
    IN OUT FONT_CONFIG *efiFontConfig,
    IN OUT EFI_FILE_DESCRIPTOR *efiKernelFile,
    IN OUT KERNEL_PARAMS_CONFIG *efiKernelParam
)
{
    EFI_FILE_DESCRIPTOR efiFontFile;                             //Font BMP文件的打开描述符
    BMP_STRUCT efiFontBmp;                                       //Font BMP文件结构

    displayStartupImage(                                           //显示开启LOGO函数              
        ImageHandle,
        STARTUP_IMAGE_PATH,                                         //BMP文件路径
        efiGraphicConfig                                          //图形设置数据结构
    );
    
    loadKernelFont(
        ImageHandle,
        &efiFontFile,
        &efiFontBmp,
        efiFontConfig
    );

    EFI_PHYSICAL_ADDRESS kernelfileBase = loadKerneltoMemory(ImageHandle,efiKernelFile);  //文件缓冲区数据
    if(checkELFFormat(kernelfileBase) != ELF_LOADING_SUCCESS){                             //内核加载失败
        Print(L"[BOOTLOADER]:Unable to Load Kernel File\n");
        return FALSE;
    }
    /*内核参数传递*/
    efiKernelParam->m_graphic = efiGraphicConfig;
    efiKernelParam->m_font = efiFontConfig;
    //efiKernelParam->idmap_pg_dir = (struct page_global_directory *)AllocatePageTable();
    //efiKernelParam->swapper_pg_dir = (struct page_global_directory *)AllocatePageTable();
    return TRUE;
}

/*-----------------------------------------------------------------------------
* 重新定位内核的位置并获取内核的入口点，打开MMU分页
* @name: getKernelEntryPoint
* @function: 1.获取重定位内核的入口地址的函数指针用于载入_start函数
* @param :  1.传入系统的ImageHandle参数
            2.kernel.elf文件打开描述符(需要其中的打开文件后读取的缓冲区数据)
            3.传入MEMORY_MAP_CONFIG参数表
            4.内核设置参数
            
* @retValue：返回重定位_start函数指针
*------------------------------------------------------------------------------*/
RET_FUNC
getKernelEntryPoint(
  IN EFI_HANDLE ImageHandle,
  IN EFI_FILE_DESCRIPTOR *kernelFile,
  IN OUT MEMORY_MAP_CONFIG *MemoryMap,
  IN OUT KERNEL_PARAMS_CONFIG * kernelConfig)
{
    EFI_PHYSICAL_ADDRESS kernelFileBuffer = kernelFile->mEfiFileBuffer;
    if(!kernelFileBuffer){
        Print(L"[BOOTLOADER]:Invalid Kernel File\n");
        return (RET_FUNC)NULL;
    }
    EFI_PHYSICAL_ADDRESS relocateAddress;                                                     //内核重定位的物理地址
    EFI_PHYSICAL_ADDRESS reallocAddr = loadkernelSegments(kernelFileBuffer,&relocateAddress); //返回恒等映射地址
    Print(L"[BOOTLOADER]:RelocatePAddr = %X",relocateAddress);
    
    //EnableMemoryPaging(
    //    (Elf64_Phdr*)((Elf64_Ehdr*)kernelFileBuffer + 1),
    //    relocateAddress,
    //    kernelConfig->idmap_pg_dir,                                             //恒等映射
    //    kernelConfig->swapper_pg_dir,                                           //内核基地址映射
    //    AllocatePageTable
    //);
    
    //EnableHardware(
    //    kernelConfig->idmap_pg_dir,                                             //恒等映射
    //    kernelConfig->swapper_pg_dir                                            //内核基地址映射
    //);

    prepareVirtualMemoryMapping(
        ImageHandle,
        MemoryMap
    );

    //loadMemoryMap(MemoryMap);
    /*内核参数传递*/
    kernelConfig->m_memory = MemoryMap;
    return (RET_FUNC)(reallocAddr);
}

/*-----------------------------------------------------------------------------
* 启动内核
* @name: RunKernel
* @function: 启动内核
* @param :  1.传入指向内核入口的函数指针
            2.内核的启动参数

* @retValue：返回内核执行错误代码
*------------------------------------------------------------------------------*/
UINT64 
EFIAPI
RunKernel(
    IN RET_FUNC KernelEntryPoint,
    IN KERNEL_PARAMS_CONFIG *param)
{
    return KernelEntryPoint(param);
}