#ifndef _TLB_H_
#define _TLB_H_
#pragma once

/*-----------------------------------------------------------------------------
* 关闭TLB的结构
* @name: disable_tlb_cache
* @function: 关闭TLB的结构
* @param : 1.读取UEFI传输给操作系统的参数中内存布局部分参数
* @retValue: 返回操作系统内存是否初始化成功
*------------------------------------------------------------------------------*/
void disable_tlb_cache();

#endif //_TLB_H_