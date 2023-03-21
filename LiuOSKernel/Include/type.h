#ifndef _TYPE_H_
#define _TYPE_H_
#pragma once
#include"RunTimeKernelConf.h"     //UEFI传递内核参数
#include"tools.h"
#include"string.h"
#include"stdlib.h"
#include"stdio.h"

#define IN                        //输入参数
#define OUT                       //输出参数

/*------------------------------------------------------------
*declaration!!
*------------------------------------------------------------*/
struct page_table_entry;      //PTE
struct page_middle_directory; //PMD
struct page_upper_directory;  //PUD
struct page_global_directory; //PGD

typedef UINT64 pteval_t;        //PTE   
typedef UINT64 pmdval_t;        //PMD
typedef UINT64 pudval_t;        //PUD
typedef UINT64 pgdval_t;        //PGD
typedef UINT32 phys_addr_t;     //物理地址

/*------------------------------------------------------------
*物理内存分配所用参数信息表
------------------------------------------------------------*/
#define PAGE_SHIFT 12
#define PAGE_SIZE 4096              //页面大小
#define PAGE_MASK         (~(PAGE_SIZE-1))
#define PAGE_ALIGN(addr) (((addr)+PAGE_SIZE-1) & PAGE_MASK)

#define __phys_to_pte_val(phys)	(phys)      //

#define LOW_MEMORY 0x400000                 //低地址
#define HIGH_MEMORY (0x100000*1024)         //高地址1024MB
#define NR_PAGES (HIGH_MEMORY /PAGE_SIZE)  //有效页面总数

/*-----------------------------------------------------------------------------
* 描述屏幕点的坐标
*------------------------------------------------------------------------------*/
typedef struct POINT{
    UINT32 X;
    UINT32 Y;
}POINT,POINT2D;

extern POINT UPPER_LEFT;   //左上角极值
extern POINT LOWER_RIGHT;  //右下角极值

/*-----------------------------------------------------------------------------
* 描述屏幕的像素点的位置和色彩
*------------------------------------------------------------------------------*/
extern EFI_GRAPHICS_OUTPUT_BLT_PIXEL BLUE;     //蓝色像素
extern EFI_GRAPHICS_OUTPUT_BLT_PIXEL RED ;     //红像素
extern EFI_GRAPHICS_OUTPUT_BLT_PIXEL BLACK;    //黑像素

typedef struct PIXEL_COLOR{
    POINT m_pos;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL m_color;
}PIXEL_COLOR;

/*-----------------------------------------------------------------------------
* 轮廓或填充选项(enum)
*------------------------------------------------------------------------------*/
typedef enum DRAW_METHOD{
    OUTLINE = 0,
    PADDING
}DRAW_METHOD;

#endif //_TYPE_H_