/**
* @file         Kernel_reader.h
* @brief		
* @details	    Kernel的ELF文件加载器
* @author		LPH
* @date		    2023-3-3 
* @version	    v1.0
* @par Copyright(c):LPH corporation
* @par History:         
*	    2023-3-3 
*/
#ifndef _KERNEL_READER_H_
#define _KERNEL_READER_H_
#pragma once
#include "kernel_struct.h"

/*-----------------------------------------------------------------------------
* 通过GetMemoryMap获取内存布局
* @name: loadMemoryMap
* @function: 将多个内存布局结构存储在结构体中，并退出启动时服务BOOT_SERVICES
* @param :  1.传入MEMORY_MAP_CONFIG参数表
* @retValue：返回系统状态信息
*------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI 
loadMemoryMap(OUT MEMORY_MAP_CONFIG *MemoryMap);

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
);

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
checkELFFormat(IN EFI_PHYSICAL_ADDRESS kernelBuffer);

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
  IN struct elf64_phdr * section_table,            //段表的基地址
  IN Elf64_Half section_table_count,                //段表中元素数量
  IN OUT UINT64 *kernelLowAddress,            //原始内核低地址
  IN OUT UINT64 *kernelHighAddress            //原始内核高地址
);

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
  IN EFI_PHYSICAL_ADDRESS kernelReallocBase         //重定位的内核基地址
);

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
  OUT EFI_PHYSICAL_ADDRESS *relocateAddress
);

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
  IN OUT KERNEL_PARAMS_CONFIG * kernelConfig
);

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
);

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
  IN KERNEL_PARAMS_CONFIG *param
);

#endif //_KERNEL_READER_H_