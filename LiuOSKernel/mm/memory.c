﻿#include"memory.h"

/*-----------------------------------------------------------------------------
* 操作系统内部初始化UEFI内存布局模型
* @name: mm_init
* @function: 操作系统内部初始化UEFI内存布局模型
* @param : 1.读取UEFI传输给操作系统的参数中内存布局部分参数
* @retValue: 返回操作系统内存是否初始化成功
*------------------------------------------------------------------------------*/
Bool 
mm_init(MEMORY_MAP_CONFIG* memory_map)
{
	/*接收来自内核BootLoader内存参数传递*/
	g_MemoryInitStruct.m_DescriptorArray = (NEW_MEMORY_DESCRIPTOR*)memory_map->m_MemoryMap; 			  //内存描述符存储数组
	g_MemoryInitStruct.m_UefiDesciptorCount = memory_map->m_MemoryMapSize / memory_map->m_DescriptorSize; //内存描述符存储数组元素个数

	if(!locate_conventional_space(memory_map)){							//寻找可用空间容纳内存布局信息结构(MM_STRUCT)
		return FALSE;
	}		
	init_pm_statistics();												//使用内存描述符初始化内存布局信息结构(MM_INFORMATION)
	debug_pm_statistics();												//输出内存调试信息
	paging_init();														//启动分页
	return TRUE;
}