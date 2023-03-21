#include"tlb.h"

/*-----------------------------------------------------------------------------
* 使所有核的所有TLB表项失效
* @name: flush_all_tlb
* @function: 使所有核的所有TLB表项失效
*------------------------------------------------------------------------------*/
void flush_all_tlb()
{
#ifdef __aarch64__
    asm volatile("dsb ishst": : : "memory");  //确保屏障前面的存储指令执行完
    asm volatile("tlbi vmalle1is"::);          //核上匹配当前VMID、阶段1和异常级别1的所有TLB表项失
    asm volatile("dsb ish": : : "memory");    //确保前面的TLB失效指令执行完
    isb();                                   //冲洗流水线的所有指令
#else
#endif //__aarch64__
}