#ifndef _MM_STRUCT_H_
#define _MM_STRUCT_H_
#pragma once
#include"type.h"

/*------------------------------------------------------------
typedef enum {
  EfiReservedMemoryType,           os_memory_uefi
  EfiLoaderCode,                   os_memory_uefi
  EfiLoaderData,                    os_memory_uefi
  EfiBootServicesCode,(可以回收)     os_memory_unused √
  EfiBootServicesData,(可以回收)     os_memory_unused √
  EfiRuntimeServicesCode,(严禁回收)  os_memory_rt   √
  EfiRuntimeServicesData,(严禁回收)  os_memory_rt   √
  EfiConventionalMemory,(可以回收)    os_memory_unused √
  EfiUnusableMemory,                 os_memory_uefi
  EfiACPIReclaimMemory,              os_memory_acpi  √  
  EfiACPIMemoryNVS,                  os_memory_acpi  √ 
  EfiMemoryMappedIO,                 os_memory_mmio √
  EfiMemoryMappedIOPortSpace,        os_memory_uefi
  EfiPalCode,                        os_memory_uefi
  EfiPersistentMemory,               os_memory_uefi
  EfiMaxMemoryType                   os_memory_uefi
} EFI_MEMORY_TYPE;
------------------------------------------------------------*/
#define OS_MEMORY_UNKOWN 0      //未知类型(用于区别内存空洞)
#define OS_MEMORY_UNUSED 1      //没有使用空闲
#define OS_MEMORY_USED 2        //已经使用
#define OS_MEMORY_ACPI 3        //ACPI
#define OS_MEMORY_UEFI 4        //UEFI内存
#define OS_MEMORY_RT 5          //RT内存
#define OS_MEMORY_MMIO 6        //MMIO内存
#define OS_MEMORY_VOID 7       //内存孔洞

/*-----------------------------------------------------------------
 *经过改良并内存对齐填补的内存描述符结构(0x30B)
 *用于读取从UEFI传入的原始内存信息
*-----------------------------------------------------------------*/
typedef struct NEW_MEMORY_DESCRIPTOR{
  UINT32                Type;
  UINT32                ReservedA;  //用于与(EFI_PHYSICAL_ADDRESS)进行字节对齐
  EFI_PHYSICAL_ADDRESS  PhysicalStart;
  EFI_VIRTUAL_ADDRESS   VirtualStart;
  UINT64                NumberOfPages;
  UINT64                Attribute;
  UINT64                ReservedB;  //预留
} NEW_MEMORY_DESCRIPTOR;

/*-----------------------------------------------------------------
 *接收来自内核BootLoader内存参数传递
*-----------------------------------------------------------------*/
#pragma pack(1)
typedef struct MEMORY_INIT_STRUCT
{
    UINTN m_UefiDesciptorCount;                     //内存描述符存储数组元素个数
    NEW_MEMORY_DESCRIPTOR *m_DescriptorArray;       //内存描述符存储数组
} __attribute__((__aligned__(1))) MEMORY_INIT_STRUCT;

/*-----------------------------------------------------------------
 *物理内存布局信息结构(鉴于内核BootLoader内存参数基础上)(4B对齐)
*-----------------------------------------------------------------*/
#pragma pack(1)
typedef struct MM_INFORMATION{              //用于形容每一个内存孔洞的参数信息
  UINT32                Type;               //内存类型数据
  EFI_PHYSICAL_ADDRESS  PhysicalStart;      //物理起始地址
  UINT64                NumberOfPages;      //页面数量
}__attribute__((__aligned__(1))) MM_INFORMATION;

/*-----------------------------------------------------------------
 *用于描述物理内存的页面信息用于初始化后续虚拟地址(4B对齐)
*-----------------------------------------------------------------*/
#pragma pack(1)
typedef struct PHYSICAL_MEMORY_STATISTICS{  //用于描述物理内存的页面信息
  int m_infoCount;                          //描述内存信息结构的数组索引总数
  MM_INFORMATION * m_infoArr;               //描述内存信息结构的数组
}__attribute__((__aligned__(1))) PHYSICAL_MEMORY_STATISTICS;

extern PHYSICAL_MEMORY_STATISTICS g_MemoryDistribution ;      //内存分布
extern MEMORY_INIT_STRUCT g_MemoryInitStruct;//专门用于读取信息


/*-----------------------------------------------------------------------------
* 将内存描述符源的内存类型信息整合为内存布局信息结构的类型信息
* @name: GET_MEMORY_TYPE(src)
* @param : 1.src:内存描述符源
*------------------------------------------------------------------------------*/
#define GET_MEMORY_TYPE(src) \
({ \
    int retValue = 0; \
    if((((*src).Type) == EfiConventionalMemory) || \
         (((*src).Type) == EfiBootServicesCode) || \
         (((*src).Type)  == EfiBootServicesData)){ \
        retValue = (OS_MEMORY_UNUSED); \
    } \
	else if((((*src).Type) == EfiMemoryMappedIO)){ \
        retValue = (OS_MEMORY_MMIO); \
    } \
	else if((((*src).Type) == EfiRuntimeServicesCode) || \
           (((*src).Type) == EfiRuntimeServicesCode)){ \
        retValue = (OS_MEMORY_RT); \
    } \
	else if((((*src).Type) == EfiACPIMemoryNVS) || \
            (((*src).Type) == EfiACPIReclaimMemory)){ \
        retValue = (OS_MEMORY_ACPI); \
    } \
	else{ \
        retValue = (OS_MEMORY_UEFI); \
    } \
    retValue; \
})

/*-----------------------------------------------------------------------------
* 整合内存类型信息后并将数据写入内存布局信息结构
* @name: SET_MEMORY_TYPE(src,dst)
* @param : 1.src:内存描述符源
           2.dst:内存布局信息结构
*------------------------------------------------------------------------------*/
#define SET_MEMORY_TYPE(src,dst)  \
({ \
    (dst->Type) = GET_MEMORY_TYPE(src); \
})

/*-----------------------------------------------------------------------------
* 根据物理内存占用信息结构重新计算物理内存占用
* @name: calculate_pm_statistics
* @function: 根据物理内存占用信息结构重新计算物理内存占用，并排序
*------------------------------------------------------------------------------*/
void recalculate_pm_statistics();

/*-----------------------------------------------------------------------------
* 使用UEFI内存描述符初始化物理内存的页面信息( PHYSICAL_MEMORY_STATISTICS)
* @name: init_pm_statistics
* @function: 使用UEFI内存描述符初始化物理内存的页面信息(PHYSICAL_MEMORY_STATISTICS)
* @param:  1.内存描述符存储数组
           2.内存描述符存储数组元素个数
*------------------------------------------------------------------------------*/
void init_pm_statistics(
    NEW_MEMORY_DESCRIPTOR *DescriptorArray,
    UINTN UefiDesciptorCount
);

/*-----------------------------------------------------------------------------
* 打印物理内存的页面信息结构的数组(DEBUG专用)
* @name: debug_pm_statistics
* @function: 打印物理内存的页面信息结构的数组(DEBUG专用)
*------------------------------------------------------------------------------*/
void debug_pm_statistics();

/*-----------------------------------------------------------------------------
* 获取当前UEFI内存的类型
* @name: getUefiMemoryType
* @function: 获取UEFI数字Type信息对应的字符串类型
* @param : 1.Type对应的字符串的输出内容的缓冲区
           2.输出内容的缓冲区的大小限制
           3.输入UEFI内存Type编号
*------------------------------------------------------------------------------*/
char *
getUefiMemoryType(
    char*str,
    UINTN size,
    const UINTN Type
);

#endif //_MM_STRUCT_H_