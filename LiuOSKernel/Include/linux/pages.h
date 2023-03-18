﻿#ifndef _PAGES_H_
#define _PAGES_H_
#pragma once

#ifdef __aarch64__
/*-----------------------------------------------------------------------------
*内核空间结构的虚拟地址高地址以及低地址，最高位为1，因此访问TTBR1_EL1内核页表
-----------------------------------------------------------------------------*/
#define TOP_KERNEL_SPACE (0xFFFF000000000000)           //映射0xFFFF000000000000
#define BOTTOM_KERNEL_SPACE (~0)                        //映射0xFFFFFFFFFFFFFFFF

/*-----------------------------------------------------------------------------
*用户空间结构的虚拟地址高地址以及低地址，最高位为0，因此访问TTBR0_EL0用户页表
-----------------------------------------------------------------------------*/
#define TOP_USER_SPACE (0x0000FFFFFFFFFFFF)             //映射0x0000FFFFFFFFFFFF
#define BOTTOM_USER_SPACE (0x0)                         //映射0x0000000000000000
#endif //__aarch64__

/*-----------------------------------------------------------------------------
* 关闭TLB的结构
* @name: disable_tlb_cache
* @function: 关闭TLB的结构
* @param : 1.读取UEFI传输给操作系统的参数中内存布局部分参数
* @retValue: 返回操作系统内存是否初始化成功
*------------------------------------------------------------------------------*/
void disable_tlb_cache();

#endif //_PAGES_H_