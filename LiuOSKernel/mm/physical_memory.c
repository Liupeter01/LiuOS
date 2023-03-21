#include"pm_statistics.h"

MEMORY_INIT_STRUCT g_MemoryInitStruct = {   //全局内核BootLoader内存参数
	.m_UefiDesciptorCount = 0,				//内存描述符存储数组元素个数
	.m_DescriptorArray = NULL				//内存描述符存储数组
};

PHYSICAL_MEMORY_STATISTICS g_MemoryDistribution = {//用于描述内存布局信息结构的数组和大小
	.m_infoArr = NULL,           			//描述内存信息结构的数组
	.m_infoCount = 0                    	//描述内存信息结构的数组索引总数
};

unsigned short memory_map[NR_PAGES] = {OS_MEMORY_UNKOWN};

/*-----------------------------------------------------------------------------
* 初始化物理内存分配的位图
* @name: init_memory_bitmap
* @function: 初始化物理内存分配的位图用于空闲内存分配
*------------------------------------------------------------------------------*/
void init_memory_bitmap()
{
	MM_INFORMATION* mm = g_MemoryDistribution.m_infoArr;			        //物理内存占用信息结构首地址
    for(int i = 0 ; i < g_MemoryDistribution.m_infoCount ; ++i){
        if(CONVERT_BYTES_TO_PAGES(mm[i].PhysicalStart) < NR_PAGES){
            for(UINT64 start =  CONVERT_BYTES_TO_PAGES(mm[i].PhysicalStart) ;
			    start < CONVERT_BYTES_TO_PAGES(mm[i].PhysicalStart) ; start++ )
			{
                memory_map[start] = mm[i].Type;
            }
        }
    }
}

/*-----------------------------------------------------------------------------
* 获取物理内存的空闲页面
* @name: get_free_page
* @function: 获取物理内存的空闲页面
* @retvalue：返回物理地址
*------------------------------------------------------------------------------*/
UINT64 get_free_page()
{
    for(UINT64 i = 0;i < NR_PAGES ; ++i){
        if(memory_map[i] == OS_MEMORY_UNUSED){
            memory_map[i] = OS_MEMORY_USED;         //页面被占用
            return i * PAGE_SIZE;      //返回地址
        }
    }
    return -1;
}

/*-----------------------------------------------------------------------------
* 释放物理内存
* @name: free_page
* @function: 释放物理内存
* @param:1.传入地址
*------------------------------------------------------------------------------*/
void free_page(UINT64 addr)
{
    memory_map[addr / PAGE_SIZE] =  OS_MEMORY_UNUSED;   //释放
}

/*-----------------------------------------------------------------------------
* 用于在系统没有初始化页表之前进行内存分配的函数
* @name: early_pagetable_alloc
* @function: 分配一个4KB的页面用于页表
* @param : 1.
*------------------------------------------------------------------------------*/
UINT64 early_pagetable_alloc()
{
    UINT64 physcial_addr = get_free_page();
    memset((void*)physcial_addr,0,PAGE_SIZE);
    return physcial_addr;
}

/*-----------------------------------------------------------------------------
* 寻找可用空间容纳内存布局信息结构PHYSICAL_MEMORY_STATISTICS
* @name: locate_conventional_space
* @function: 在UEFI所传递的内存描述符元素中寻找可以容纳MM_INFORMATION的区域
* @param : 1.读取UEFI传输给操作系统的参数中内存布局部分参数
           2.内存描述符存储数组
           3.内存描述符存储数组元素个数

* @retValue: 返回操作系统内存是否初始化成功
*------------------------------------------------------------------------------*/
Bool 
locate_conventional_space(
    MEMORY_MAP_CONFIG* memory_map,
    NEW_MEMORY_DESCRIPTOR *DescriptorArray,
    UINTN UefiDesciptorCount
)
{
	Bool isMemoryDistributionFounded = FALSE;			//是否找到合适的内存
	for (int i = 0; i < UefiDesciptorCount; ++i) {
		/*---------------------------------------------------------------------
		*内存类型为空闲内存EfiConventionalMemory，且内存描述符的页面大小满足最低要求
		---------------------------------------------------------------------*/
		if (DescriptorArray[i].Type == EfiConventionalMemory &&
			//g_MemoryInitStruct.m_DescriptorArray[i].PhysicalStart > LOW_MEMORY &&
			DescriptorArray[i].NumberOfPages > CONVERT_BYTES_TO_PAGES(memory_map->m_MemoryMapSize))
		{
			isMemoryDistributionFounded = TRUE;
			g_MemoryDistribution.m_infoArr = (MM_INFORMATION*)DescriptorArray[i].PhysicalStart;	//将内存地址挂载映射
			break;
		}
	}
	if (g_MemoryDistribution.m_infoArr == NULL && !isMemoryDistributionFounded) {		//内存分布结构体没有被初始化
		console_puts("Memory Init Process Failed! OS StartUp Failed!\n");
	}
	return isMemoryDistributionFounded;
}

/*-----------------------------------------------------------------------------
* 根据物理内存占用信息结构重新计算物理内存占用
* @name: recalculate_pm_statistics
* @function: 根据物理内存占用信息结构重新计算物理内存占用，并排序
*------------------------------------------------------------------------------*/
void recalculate_pm_statistics()
{
	int arrayCount =  g_MemoryDistribution.m_infoCount;						//MM_INFORMATION结构的数量
	MM_INFORMATION* mm = g_MemoryDistribution.m_infoArr;			        //物理内存占用信息结构首地址
	MM_INFORMATION tempTailInsert = {										//将剩余的内存在尾部进行初始化
		.Type = OS_MEMORY_UNUSED,              //内存类型数据
  		.PhysicalStart = 0,      //物理起始地址
  		.NumberOfPages = 0      //页面数量
	};
	for(int i = 0 ; i < arrayCount ; ++i){
		if(mm[i].Type == OS_MEMORY_UNUSED &&					//找到为当前内存分配空间的地方
			mm[i].PhysicalStart <= (EFI_PHYSICAL_ADDRESS)mm &&
			mm[i].PhysicalStart + CONVERT_PAGES_TO_BYTES(mm[i].NumberOfPages) > (EFI_PHYSICAL_ADDRESS)mm)
		{				

			int occupiedPages = CONVERT_BYTES_TO_PAGES(sizeof(MM_INFORMATION) * (arrayCount+1) ); //统计当前结构占用页面数量
			tempTailInsert.Type = mm[i].Type;												//记录原始类型
			tempTailInsert.NumberOfPages = mm[i].NumberOfPages - occupiedPages;				//剩余内存为空闲内存
			tempTailInsert.PhysicalStart = mm[i].PhysicalStart + CONVERT_PAGES_TO_BYTES(occupiedPages);	//新内存开始地址
			mm[i].Type = OS_MEMORY_USED;													//内存已经被占用
			mm[i].NumberOfPages = occupiedPages;											//更新当前内存段的占用情况
			
			mm[arrayCount].Type =  tempTailInsert.Type;										//在尾部添加一个新块
			mm[arrayCount].PhysicalStart = tempTailInsert.PhysicalStart;
			mm[arrayCount].NumberOfPages = tempTailInsert.NumberOfPages;
			g_MemoryDistribution.m_infoCount = arrayCount + 1;								//更新总数
			break;
		}
	}
	/*------------------------------------------------------------
	*冒泡排序从小到大(之后可能进行修改)
	------------------------------------------------------------*/
	for (int i = 1; i < g_MemoryDistribution.m_infoCount; i++)
	{
		for (int j = 0; j < g_MemoryDistribution.m_infoCount - i; j++)
		{
			if (mm[j].PhysicalStart > mm[j + 1].PhysicalStart)
			{
				MM_INFORMATION temp = {							//临时数据
					.Type = mm[j].Type,   						//内存类型数据
					.PhysicalStart = mm[j].PhysicalStart,      	//物理起始地址
					.NumberOfPages = mm[j].NumberOfPages      	//页面数量
				};
				mm[j].Type = mm[j + 1].Type;
				mm[j].PhysicalStart = mm[j + 1].PhysicalStart;
				mm[j].NumberOfPages = mm[j + 1].NumberOfPages;

				mm[j + 1].Type = temp.Type;
				mm[j + 1].PhysicalStart = temp.PhysicalStart;
				mm[j + 1].NumberOfPages = temp.NumberOfPages;
			}
		}
	}
}

/*-----------------------------------------------------------------------------
* 使用UEFI内存描述符初始化物理内存的页面信息( PHYSICAL_MEMORY_STATISTICS)
* @name: init_pm_statistics
* @function: 使用UEFI内存描述符初始化物理内存的页面信息(PHYSICAL_MEMORY_STATISTICS)
* @param:  1.内存描述符存储数组
           2.内存描述符存储数组元素个数
*------------------------------------------------------------------------------*/
void 
init_pm_statistics(
    NEW_MEMORY_DESCRIPTOR *DescriptorArray,
    UINTN UefiDesciptorCount
)
{
	int m_infoCount = 0;													//定义计数变量
	NEW_MEMORY_DESCRIPTOR* src = DescriptorArray;		//拷贝源
	MM_INFORMATION* mm_dst = g_MemoryDistribution.m_infoArr;			    //物理内存占用信息结构目的地址
	for(int i = 0 ; i < UefiDesciptorCount ;++i){
		SET_MEMORY_TYPE(&src[i],mm_dst);
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
	console_puts("init_pm_statistics\n");
	recalculate_pm_statistics();
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