#include"mmu.h"

/*-----------------------------------------------------------------------------
* 初始化CPU
* @name: init_cpu
* @function: 初始化CPU      
*------------------------------------------------------------------------------*/
void 
init_cpu()
{
#ifdef __aarch64__
    asm volatile("tlbi vmalle1");           //使当前EL1的TLB所有表项全部失效
    asm volatile("dsb nsh": : : "memory");  //保证TLBI指令在当前核心顺利执行完成
    
    /*---写入MAIR_EL1内存属性设置寄存器--------
    *   MT_DEVICE_nGnRnE  000 00000000 
    *   MT_DEVICE_nGnRE   001 00000100
    *   MT_DEVICE_GRE     010 00001100
    *   MT_NORMAL_NC      011 01000100
    *   MT_NORMAL         100 11111111
    *   MT_NORMAL_WT      101 10111011
    *----------------------------------------*/
    unsigned long mairel1_val ={
        MAIR( MT_DEVICE_nGnRnE , 0x00UL) |
        MAIR( MT_DEVICE_nGnRE , 0x04UL) |
        MAIR( MT_DEVICE_GRE , 0x0CUL) |
        MAIR( MT_NORMAL_NC , 0x44UL) |
        MAIR( MT_NORMAL , 0xFFUL) |
        MAIR( MT_NORMAL_WT , 0xBBUL)
    }; 
    //write_sysreg(mairel1_val,mair_el1);    
    /*写入MAIR_EL1内存属性设置寄存器*/
	asm volatile("msr mair_el1, %x0"::"rZ"(mairel1_val)); 

    unsigned long tcr_reg = {
        TCR_TxSZ(VA_BITS) |                 //地址最大值参数
        TCR_TG1_4K |                        //TTBR1页面粒度
        TCR_TG0_4K                          //TTBR0页面粒度
    };

    //unsigned long max_physical_mem = read_sysreg(ID_AA64MMFR0_EL1) & 0xF;
    /*读取系统支持的最大物理内存*/
    unsigned long max_physical_mem = 0;
    asm volatile("mrs %0, ID_AA64MMFR0_EL1" : "=r" (max_physical_mem)); 
    {
        char str[256] = {"[cpu init info]:max_physical_mem = "};
        itoa(max_physical_mem, str + strlen(str), 16);
        console_puts(str);
        console_putc('\n');
    }
    if(max_physical_mem > ID_AA64MMFR0_PARANGE_48){  //如果系统物理内存数量大于48位               
        max_physical_mem = ID_AA64MMFR0_PARANGE_48; //将系统物理内存加以限制
    }

    tcr_reg |= (max_physical_mem << TCR_IPS_SHIFT);  //配置地址转换MMU后输出物理地址的最大值

    //write_sysreg(tcr_reg, SYS_TCR_EL1);
    /*写入tcr_el1寄存器*/
	asm volatile("msr tcr_el1, %x0"::"rZ"(tcr_reg)); //写入tcr_el1寄存器

#endif //__aarch64__
}

/*-----------------------------------------------------------------------------
* 启动MMU
* @name: enable_mmu
* @function: 启动MMU     
* @param: 1.传入恒等映射PGD页表
* @retvalue: 判断MMU是否启动成功
*------------------------------------------------------------------------------*/
Bool 
enable_mmu(
    IN struct page_global_directory *idmap_pg_dir
)
{
#ifdef __aarch64__
    /*ID_AA64MMFR0_EL1的tgran4字段判断系统是否支持4KB粒度*/
    unsigned long tgran4;
    asm volatile("mrs %0, " __stringify(ID_AA64MMFR0_EL1) : "=r" (tgran4)); 
    tgran4 = (tgran4 >> ID_AA64MMFR0_TGRAN4_SHIFT) & 0xF;
    if(tgran4 != ID_AA64MMFR0_TGRAN4_SUPPORTED){            //不支持4KB页面粒度
        return FALSE;
    }

    /*-----------------------------------------------------------
    *设置PGD页表基地址的恒等映射到TTBR0_EL1寄存器中
    -----------------------------------------------------------*/
    asm volatile("msr " __stringify(TTBR0_EL1) ", %x0"::"rZ"(idmap_pg_dir)); //设置恒等映射页表
    asm volatile("isb" : : : "memory");                  
                                                     //CPU重新取指令
    /*-----------------------------------------------------------
    *准备在SCTLR_EL1中使能开启MMU
    -----------------------------------------------------------*/
    asm volatile("msr " __stringify(SCTLR_EL1) ", %x0"::"rZ"(SCTLR_ELx_M)); //使能MMUSCTLR_ELx_M
    asm volatile("isb" : : : "memory");                                      //CPU重新取指令
    asm("ic iallu");                                                         //使得所有指令的高速缓存cache失效
    asm volatile("dsb nsh": : : "memory");                                    //非共享（non-shareable），表示数据同步屏障指令仅仅在当前核起作用
    asm volatile("isb" : : : "memory");                                      //CPU重新取指令(从告诉缓存取指令)
#else
#endif //__aarch64__
    return TRUE;
}