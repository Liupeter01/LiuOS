#include"tlb.h"

/*-----------------------------------------------------------------------------
* 关闭TLB的结构
* @name: disable_tlb_cache
* @function: 关闭TLB的结构
*------------------------------------------------------------------------------*/
void disable_tlb_cache()
{
#ifdef __aarch64__
    asm volatile("TLBI ALLE1"::);       //invalid all TLB entries
#endif //__aarch64__
#ifdef __x86_64__
    asm volatile(""::);
#endif //__aarch64__
}