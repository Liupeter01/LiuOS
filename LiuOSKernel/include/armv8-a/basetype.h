/*basetype.h*/
#ifndef _BASETYPE_H_
#define _BASETYPE_H_
#pragma once
#ifndef __ASSEMBLER__
#define TRUE 1
#define FALSE 0
#define BOOL int
typedef unsigned int	uint;
typedef unsigned short	ushort;
typedef unsigned char	uchar;
typedef unsigned long	uint64;
typedef uint64	uint64_t;
typedef unsigned int	uint32;
typedef unsigned short	uint16;
typedef unsigned char	uint8;
#endif
#ifndef NULL
#define NULL ((void*)0)
#endif

#endif //_BASETYPE_H_