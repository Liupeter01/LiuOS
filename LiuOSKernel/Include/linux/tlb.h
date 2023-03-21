#ifndef _TLB_H_
#define _TLB_H_
#pragma once
#include"mem_barrier.h"
/*-----------------------------------------------------------------------------
* 使所有核的所有TLB表项失效
* @name: flush_all_tlb
* @function: 使所有核的所有TLB表项失效
*------------------------------------------------------------------------------*/
void flush_all_tlb();

#endif //_TLB_H_