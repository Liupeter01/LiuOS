/**
* @file         KernelLoader.h
* @brief		
* @details	    Kernel的ELF文件加载器
* @author		LPH
* @date		    2023-3-3 
* @version	    v1.0
* @par Copyright(c):LPH corporation
* @par History:         
*	    2023-3-3 
*/

#ifndef _KERNEL_LOADER_H_
#define _KERNEL_LOADER_H_
#pragma once
#include "GeneralFileLoader.h"
#include "BootLoaderConf.h"
#include "RunTimeKernelConf.h"

/*-----------------------------------------------------------------------------
* 64位ELF文件格式基本类型
*------------------------------------------------------------------------------*/

#define PT_LOAD 1
#define EI_NIDENT  16                   /* Size of e_ident array. */

/*-----------------------------------------------------------------------------
* 64位ELF文件专属数据结构
*------------------------------------------------------------------------------*/
typedef UINT64	Elf64_Addr;
typedef UINT16	Elf64_Half;
typedef UINT64	Elf64_Off;
typedef UINT32	Elf64_Word;
typedef UINT64	Elf64_Xword;

/*-----------------------------------------------------------------------------
* 64位ELF文件格式的判断依据
*------------------------------------------------------------------------------*/
#define ELF_MAGIC_NUMBER (0x464C457F)       //ELF魔数
#define IS_ELF_64BIT (0x02)                 //ELF64位的判断依据

#define ELF_NOT_EXIST (0)                   //ELF不存在
#define ELF_LOADING_SUCCESS (1)             //ELF格式检测成功
#define ELF_FORMAT_ERROR (3)                //ELF格式错误
#define ELF_NONE_64BIT (4)                  //ELF不是64位的

/*-----------------------------------------------------------------------------
* 64位ELF文件头结构(对齐)
*------------------------------------------------------------------------------*/
#pragma pack(1)
typedef struct elf64_hdr {
  unsigned char	e_ident[EI_NIDENT];	/* ELF "magic number" */
  Elf64_Half e_type;    //2B
  Elf64_Half e_machine; //2B
  Elf64_Word e_version; //4B
  Elf64_Addr e_entry;		/* Entry point virtual address */
  Elf64_Off e_phoff;		/* Program header table file offset */
  Elf64_Off e_shoff;		/* Section header table file offset */
  Elf64_Word e_flags;
  Elf64_Half e_ehsize;
  Elf64_Half e_phentsize;
  Elf64_Half e_phnum;
  Elf64_Half e_shentsize;
  Elf64_Half e_shnum;
  Elf64_Half e_shstrndx;
}  __attribute__((packed))  Elf64_Ehdr;
/*-----------------------------------------------------------------------------
* 64位ELF程序头表(段表)(对齐)
*------------------------------------------------------------------------------*/
#pragma pack(1) 
typedef struct elf64_phdr {
  Elf64_Word p_type;
  Elf64_Word p_flags;
  Elf64_Off p_offset;		/* Segment file offset */
  Elf64_Addr p_vaddr;		/* Segment virtual address */
  Elf64_Addr p_paddr;		/* Segment physical address */
  Elf64_Xword p_filesz;		/* Segment size in file */
  Elf64_Xword p_memsz;		/* Segment size in memory */
  Elf64_Xword p_align;		/* Segment alignment, file & memory */
} __attribute__((packed)) Elf64_Phdr;

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
* 计算重定位内核大小
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
           2.经过内核重定位后的有效PT_LOAD段所在的新地址(属于加载到内存的二进制代码)

  @***warning***:编译不需要链接标准库stdlib。彻底消除_start
* @retValue：返回内核所在段的加载是否成功
*------------------------------------------------------------------------------*/
EFI_PHYSICAL_ADDRESS 
EFIAPI 
loadkernelSegments(
  IN EFI_PHYSICAL_ADDRESS fileBufferBase,
  OUT EFI_PHYSICAL_ADDRESS *relocateAddress 
);

/*-----------------------------------------------------------------------------
* 获取重定位内核的入口地址的函数指针。直接载入_start函数
* @name: getKernelEntryPoint
* @function: 获取重定位内核的入口地址的函数指针。直接载入_start函数
* @param :  1.传入系统的ImageHandle参数
            2.kernel.elf文件打开描述符(需要其中的打开文件后读取的缓冲区数据)
            3.经过内核重定位后的有效PT_LOAD段所在的新地址(属于加载到内存的二进制代码)
            
* @retValue：返回重定位_start函数指针
*------------------------------------------------------------------------------*/
RET_FUNC
getKernelEntryPoint(
  IN EFI_HANDLE ImageHandle,
  IN OUT EFI_FILE_DESCRIPTOR *readFile,
  OUT EFI_PHYSICAL_ADDRESS *relocateAddress
);

#endif //_KERNEL_LOADER_H_