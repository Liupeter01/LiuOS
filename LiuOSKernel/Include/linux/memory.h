#ifndef _MEMORY_H_
#define _MEMORY_H_
#pragma once
#include"pm_statistics.h"       //物理内存数据(来自UEFI内存描述符)
#include"paging.h"

/*----------------------------------------------------------
*物理地址转换为虚拟地址(线性映射)
*----------------------------------------------------------*/
#define CONFIG_ARM64_VA_BITS 48                             //开启4KB分页
#define VA_BITS			(CONFIG_ARM64_VA_BITS)

/*----------------------------------------------------------
*线性地址空间[PAGE_OFFSET,PAGE_END)
*----------------------------------------------------------*/
#define PAGE_OFFSET		 ((0xffffffffffffffff) << (VA_BITS - 1)) //0xFFFF800000000000
#define PAGE_END        (0xffffffffffffffff)                     //0xFFFFFFFFFFFFFFFF

#define __is_lm_address(addr)   (((UINT64)(addr) - PAGE_OFFSET) < (PAGE_END - PAGE_OFFSET)) //是否为线性地址

/*----------------------------------------------------------
*物理地址转换线性地址[PAGE_OFFSET,PAGE_END)
*----------------------------------------------------------*/
#define __phys_to_virt(x)    ((unsigned long)((x) - PHYS_OFFSET) | PAGE_OFFSET)
#define __va(x)    ((void *)__phys_to_virt((phys_addr_t)(x)))

/*----------------------------------------------------------
*线性地址[PAGE_OFFSET,PAGE_END)转换物理地址
*----------------------------------------------------------*/
#define __lm_to_phys(addr)	(((addr) & ~PAGE_OFFSET) + PHYS_OFFSET)
#define __kimg_to_phys(addr)	((addr) - kimage_voffset)

#define __virt_to_phys(x) ({					\
	phys_addr_t __x = (phys_addr_t)(x);				\
	__is_lm_address(__x) ? __lm_to_phys(__x) :			\
			       __kimg_to_phys(__x);			\
})

#define __pa(x) __virt_to_phys((unsigned long)(x)) 


/*-----------------------------------------------------------------------------
* 操作系统内部初始化UEFI内存布局模型
* @name: mm_init
* @function: 操作系统内部初始化UEFI内存布局模型
* @param : 1.读取UEFI传输给操作系统的参数中内存布局部分参数
* @retValue: 返回操作系统内存是否初始化成功
*------------------------------------------------------------------------------*/
Bool
mm_init(
    MEMORY_MAP_CONFIG *memory_map
);


#endif //_MEMORY_H_