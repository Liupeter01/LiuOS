/**
* @file         kernel_struct.h
* @brief		
* @details	    Kernel的ELF文件加载器
* @author		LPH
* @date		    2023-3-3 
* @version	    v1.0
* @par Copyright(c):LPH corporation
* @par History:         
*	    2023-3-3 
*/
#ifndef _KERNEL_STRUCT_H_
#define _KERNEL_STRUCT_H_
#pragma once
#include "console_logo.h"
#include "console_font.h"

/*64位ELF文件格式基本类型*/
#define PT_LOAD 1
#define EI_NIDENT  16                   /* Size of e_ident array. */

/*64位ELF文件专属数据结构*/
typedef UINT64	Elf64_Addr;
typedef UINT16	Elf64_Half;
typedef UINT64	Elf64_Off;
typedef UINT32	Elf64_Word;
typedef UINT64	Elf64_Xword;

/*-----------------------------------------------------------------------------
* 64位ELF文件格式的判断依据
*------------------------------------------------------------------------------*/
#define ELF_MAGIC_NUMBER (0x464C457F)       //ELF魔数
#define IS_ELF_64BIT (0x02)                 //ELF64位的判断依据

#define ELF_NOT_EXIST (0)                   //ELF不存在
#define ELF_LOADING_SUCCESS (1)             //ELF格式检测成功
#define ELF_FORMAT_ERROR (3)                //ELF格式错误
#define ELF_NONE_64BIT (4)                  //ELF不是64位的

/*-----------------------------------------------------------------------------
* 64位ELF文件头结构(对齐)
*------------------------------------------------------------------------------*/
#pragma pack(1)
typedef struct elf64_hdr {
  unsigned char	e_ident[EI_NIDENT];	/* ELF "magic number" */
  Elf64_Half e_type;    //2B
  Elf64_Half e_machine; //2B
  Elf64_Word e_version; //4B
  Elf64_Addr e_entry;		/* Entry point virtual address */
  Elf64_Off e_phoff;		/* Program header table file offset */
  Elf64_Off e_shoff;		/* Section header table file offset */
  Elf64_Word e_flags;
  Elf64_Half e_ehsize;
  Elf64_Half e_phentsize;
  Elf64_Half e_phnum;
  Elf64_Half e_shentsize;
  Elf64_Half e_shnum;
  Elf64_Half e_shstrndx;
}  __attribute__((packed))  Elf64_Ehdr;
/*-----------------------------------------------------------------------------
* 64位ELF程序头表(段表)(对齐)
*------------------------------------------------------------------------------*/
#pragma pack(1) 
typedef struct elf64_phdr {
  Elf64_Word p_type;
  Elf64_Word p_flags;
  Elf64_Off p_offset;		/* Segment file offset */
  Elf64_Addr p_vaddr;		/* Segment virtual address */
  Elf64_Addr p_paddr;		/* Segment physical address */
  Elf64_Xword p_filesz;		/* Segment size in file */
  Elf64_Xword p_memsz;		/* Segment size in memory */
  Elf64_Xword p_align;		/* Segment alignment, file & memory */
} __attribute__((packed)) Elf64_Phdr;

/*-----------------------------------------------------------------------------
* 内存布局模型结构体(1B)
*------------------------------------------------------------------------------*/
#pragma pack(1)
typedef struct MEMORY_MAP_CONFIG{
    UINTN m_MemoryMapSize;              //缓冲区大小
    //指针指向缓冲的起始内存地址
    //可通过AllocatePool开辟内存(其中含有多个内存描述符结构体)
    VOID *m_MemoryMap; 
    UINTN m_MapKey;
    UINTN m_DescriptorSize;             //每一个描述符的大小
    UINT32 m_DescriptorVersion;         //描述符的版本
} __attribute__((__aligned__(1)))  MEMORY_MAP_CONFIG;

/*-----------------------------------------------------------------------------
* 内核传递参数
*------------------------------------------------------------------------------*/
#pragma pack(1)
typedef struct KERNEL_PARAMS_CONFIG{
    GRAPHIC_CONFIG * m_graphic;         //显示参数
    FONT_CONFIG * m_font;
    MEMORY_MAP_CONFIG * m_memory;           //内存布局
} __attribute__((__aligned__(1))) KERNEL_PARAMS_CONFIG;

/*基本内核参数传递函数指针数据结构*/
//typedef UINT64(*RET_FUNC)(KERNEL_PARAMS_CONFIG*);
typedef UINT64(*RET_FUNC)();

#endif //_KERNEL_STRUCT_H_