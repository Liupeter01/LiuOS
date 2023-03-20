#include"video.h"
#include"console.h"
#include"memory.h"

/*-----------------------------------------------------------------------------
* 操作系统入口点
* @name: _start
* @function: 对于操作系统关键部分进行全部的初始化(设备驱动+CPU+内存等)
* @param : UEFI传输给操作系统的参数
*------------------------------------------------------------------------------*/
unsigned long long _start(KERNEL_PARAMS_CONFIG * kernelParam)
{
	video_init(kernelParam->m_graphic);		//初始化视频功能
	console_init(kernelParam->m_font);		//初始化控制台shell显示功能
	console_cls();

	console_puts("Welcome to LiuOS(LPH Corporation Present 2023)\n");
	if(!mm_init(kernelParam->m_memory)){			//初始化内存
		goto LiuOSStartupFailed;				//执行错误处理程序
	}

	while(1){	/*halt()*/	}
LiuOSStartupFailed:
	/*should never reach here!!!!*/

#ifdef __aarch64__
	asm("wfe"::);
	//asm("b halt"::);

#endif //__aarch64__
	return 1;
}