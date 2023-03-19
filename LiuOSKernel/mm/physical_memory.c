#include"pm_statistics.h"

MEMORY_INIT_STRUCT g_MemoryInitStruct = {   //全局内核BootLoader内存参数
	.m_UefiDesciptorCount = 0,				//内存描述符存储数组元素个数
	.m_DescriptorArray = NULL				//内存描述符存储数组
};

PHYSICAL_MEMORY_STATISTICS g_MemoryDistribution = {			//用于描述内存布局信息结构的数组和大小
	.m_infoArr = NULL,           			//描述内存信息结构的数组
	.m_infoCount = 0                    	//描述内存信息结构的数组索引总数
};

/*-----------------------------------------------------------------------------
* 为内存布局信息结构(MM_STRUCT)寻找空闲空间
* @name: locate_conventional_space
* @function: 在UEFI所传递的内存描述符元素中寻找可以容纳MM_INFORMATION的区域
* @param : 1.读取UEFI传输给操作系统的参数中内存布局部分参数
* @retValue: 返回操作系统内存是否初始化成功
*------------------------------------------------------------------------------*/
Bool 
locate_conventional_space(
	MEMORY_MAP_CONFIG* memory_map)
{
	Bool isMemoryDistributionFounded = FALSE;			//是否找到合适的内存
	for (int i = 0; i < g_MemoryInitStruct.m_UefiDesciptorCount; ++i) {
		/*---------------------------------------------------------------------
		*内存类型为空闲内存EfiConventionalMemory，且内存描述符的页面大小满足最低要求
		---------------------------------------------------------------------*/
		if (g_MemoryInitStruct.m_DescriptorArray[i].Type == EfiConventionalMemory &&
			g_MemoryInitStruct.m_DescriptorArray[i].NumberOfPages > CONVERT_BYTES_TO_PAGES(memory_map->m_MemoryMapSize))
		{
			isMemoryDistributionFounded = TRUE;
			g_MemoryDistribution.m_infoArr = (MM_INFORMATION*)g_MemoryInitStruct.m_DescriptorArray[i].PhysicalStart;	//将内存地址挂载映射
			break;
		}
	}
	if (g_MemoryDistribution.m_infoArr == NULL && !isMemoryDistributionFounded) {		//内存分布结构体没有被初始化
		console_puts("Memory Init Process Failed! OS StartUp Failed!\n");
	}
	return isMemoryDistributionFounded;
}

/*-----------------------------------------------------------------------------
* 使用UEFI内存描述符初始化物理内存的页面信息( PHYSICAL_MEMORY_STATISTICS)
* @name: init_pm_statistics
* @function: 使用UEFI内存描述符初始化物理内存的页面信息(PHYSICAL_MEMORY_STATISTICS)
*------------------------------------------------------------------------------*/
void
init_pm_statistics()
{
	int m_infoCount = 0;													//定义计数变量
	NEW_MEMORY_DESCRIPTOR* src = g_MemoryInitStruct.m_DescriptorArray;
	MM_INFORMATION* mm_dst = g_MemoryDistribution.m_infoArr;			    //内存布局信息结构目的地址
	for(int i = 0 ; i < g_MemoryInitStruct.m_UefiDesciptorCount ;++i){
		SET_MEMORY_TYPE(&src[i],mm_dst);
		if(GET_MEMORY_TYPE(&src[i]) == OS_MEMORY_UNUSED && 					//额外处理内存信息结构的内存并标记为已经占用g_MemoryDistribution.m_infoArr
			src[i].PhysicalStart == (EFI_PHYSICAL_ADDRESS)g_MemoryDistribution.m_infoArr){
			mm_dst->Type = OS_MEMORY_USED;									//修正当前部分内存为已经使用
		}
		mm_dst->PhysicalStart = src[i].PhysicalStart;      					//物理起始地址
		mm_dst->NumberOfPages = src[i].NumberOfPages;      					//页面数量
		if(i && mm_dst->Type == (mm_dst - 1)->Type){						//上一个和当前内存布局结构类型
			if(mm_dst->PhysicalStart ==  (mm_dst - 1)->PhysicalStart + CONVERT_PAGES_TO_BYTES((mm_dst - 1)->NumberOfPages))
			{
				(mm_dst - 1)->NumberOfPages += mm_dst->NumberOfPages;		//如果类型和连续则合并
				continue;
			}
		}
		mm_dst++;															//前往下一个内存布局信息结构
		m_infoCount++;														//自增描述内存信息结构的数组索引总数
	}	
	g_MemoryDistribution.m_infoCount = m_infoCount;							//获取
}

/*-----------------------------------------------------------------------------
* 打印物理内存的页面信息结构的数组(DEBUG专用)
* @name: debug_pm_statistics
* @function: 打印物理内存的页面信息结构的数组(DEBUG专用)
*------------------------------------------------------------------------------*/
void
debug_pm_statistics()
{
	unsigned long long to = 0;
	char str[256] = {0};
	for(int i = 0 ; i < g_MemoryDistribution.m_infoCount ; ++i){
		memset(str,0,256);
		itoa(g_MemoryDistribution.m_infoArr[i].Type,str,16);
		console_puts(str);
		console_putc(' ');

		memset(str,0,256);
		itoa(g_MemoryDistribution.m_infoArr[i].PhysicalStart,str,16);
		console_puts(str);
		console_putc(' ');

		memset(str,0,256);
		itoa(g_MemoryDistribution.m_infoArr[i].NumberOfPages,str,16);
		console_puts(str);
		console_putc('\n');

		if(g_MemoryDistribution.m_infoArr[i].Type!= OS_MEMORY_MMIO){
			to+=g_MemoryDistribution.m_infoArr[i].NumberOfPages;
		}
	}
	memset(str,0,256);
	strcpy(str,"Memory Detected! Total MemorySize Calculation= ");
	itoa(CONVERT_PAGES_TO_BYTES(to)>>20,str+strlen(str),10);
	strcat(str," MB\n");
	console_puts(str);
}