/**
* @file         Tools.c
* @brief		
* @details	    实用工具
* @author		LPH
* @date		    2023-3-3 
* @version	    v1.0
* @par Copyright(c):LPH corporation
* @par History:         
*	    2023-3-3 
*/

#include"Tools.h"

/*-----------------------------------------------------------------------------
* 通用工具函数
*------------------------------------------------------------------------------*/
VOID *
memcpy(
    VOID * __restrict s1, 
    const VOID * __restrict s2, 
    UINT64 n)
{
  return CopyMem(s1, s2, n);
}