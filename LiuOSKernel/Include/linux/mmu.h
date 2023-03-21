/*---------------------------------------------------
*硬件MMU的操作
---------------------------------------------------*/
#ifndef _MMU_H_
#define _MMU_H_
#include"os_reg.h"              //系统寄存器
#include"mem_barrier.h"         //内存屏障指令
#pragma once

struct page_global_directory;

/*-----------------------------------------------------------------
*内存属性参数(根据设备属性编号MT_*_*访问页表的AttrIndex字段数值)
-----------------------------------------------------------------*/
#define MT_DEVICE_nGnRnE	0   //设备内存，不支持聚合、指令重排以及提前写应答
#define MT_DEVICE_nGnRE		1   //设备内存，不支持聚合、指令重排，但是支持提前写应答
#define MT_DEVICE_GRE		2   //设备内存，支持聚合、指令重排以及提前写应答
#define MT_NORMAL_NC		3   //普通内存，关闭高速缓存
#define MT_NORMAL		    4   //普通内存(内核的可执行代码段和数据段采用该部分内存!!!!!!!)
#define MT_NORMAL_WT		5   //普通内存，高速缓存采用直写回写策略

/*参考内存属性编码将内存属性信息写入MAIR_EL?
    * Memory region attributes for LPAE:
    *
    *   n = AttrIndx[2:0]
    *                  n   MAIR
    *   DEVICE_nGnRnE  000 00000000 
    *   DEVICE_nGnRE   001 00000100
    *   DEVICE_GRE     010 00001100
    *   NORMAL_NC      011 01000100
    *   NORMAL         100 11111111
    *   NORMAL_WT      101 10111011
-------------------------------------------------*/
#define MAIR(index,attribute) ((attribute) << ((index) * 8))   //每一次偏移一个字节

/*-----------------------------------------------------------------------------
* 初始化CPU
* @name: init_cpu
* @function: 初始化CPU      
*------------------------------------------------------------------------------*/
void init_cpu();

/*-----------------------------------------------------------------------------
* 启动MMU
* @name: enable_mmu
* @function: 启动MMU     
* @param: 1.传入恒等映射PGD页表
* @retvalue: 判断MMU是否启动成功
*------------------------------------------------------------------------------*/
Bool enable_mmu(IN struct page_global_directory *idmap_pg_dir);

#endif //_MMU_H_