#ifndef _TOOLS_H_
#define _TOOLS_H_
#pragma once

/*强制写入内存*/
#define READ_ONCE(x) (*(volatile typeof(x) *) &(x))

/*强制在内存中读取*/
#define WRITE_ONCE(var,val) ((*(volatile typeof(var) *) &(var)) = (val))

#endif //_TOOLS_H_