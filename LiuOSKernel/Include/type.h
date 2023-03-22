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

extern struct vc_data g_screen;
extern void console_cls();

typedef UINT64 pteval_t;        //PTE   
typedef UINT64 pmdval_t;        //PMD
typedef UINT64 pudval_t;        //PUD
typedef UINT64 pgdval_t;        //PGD
typedef UINT64 phys_addr_t;     //物理地址

/*----------------------------------------------------------
*物理地址转换为虚拟地址(线性映射)
*----------------------------------------------------------*/
#define CONFIG_ARM64_VA_BITS 48                             //开启4KB分页
#define VA_BITS			(CONFIG_ARM64_VA_BITS)

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
* 定义并初始化屏幕VC的基础显示参数(屏幕显示功能位单例模式)
 * Example: vc_data of a console that was scrolled 3 lines down.
 *                              Console buffer
 * vc_screenbuf ---------> +----------------------+-.
 *                         | initializing W       |  \
 *                         | initializing X       |   |
 *                         | initializing Y       |    > scroll-back area
 *                         | initializing Z       |   |
 *                         |                      |  /
 * vc_visible_origin ---> ^+----------------------+-:
 * (changes by scroll)    || Welcome to linux     |  \
 *                        ||                      |   |
 *           vc_rows --->< | login: root          |   |  visible on console
 *                        || password:            |    > (vc_screenbuf_size is
 * vc_origin -----------> ||                      |   |   vc_size_row * vc_rows)
 * (start when no scroll) || Last login: 12:28    |  /
 *                        v+----------------------+-:
 *                         | Have a lot of fun... |  \
 * vc_pos -----------------|--------v             |   > scroll-front area
 *                         | ~ # cat_             |  /
 * vc_scr_end -----------> +----------------------+-:
 * (vc_origin +            |                      |  \ EMPTY, to be filled by
 *  vc_screenbuf_size)     |                      |  / vc_video_erase_char
 *                         +----------------------+-'
 *                         <---- 2 * vc_cols ----->
 *                         <---- vc_size_row ----->
 * Note that every character in the console buffer is accompanied with an
 * attribute in the buffer right after the character. This is not depicted
 * in the figure.
------------------------------------------------------------------------------*/
typedef struct vc_data {
	unsigned short	vc_num;			/* Console number */
	unsigned int	vc_cols;		/* [#] Console size */
	unsigned int	vc_rows;
	unsigned int	vc_size_row;		/* Bytes per row */
	unsigned int	vc_scan_lines;		/* # of scan lines */
	unsigned long	vc_origin;		/* [!] Start of real screen */
	unsigned long	vc_scr_end;		/* [!] End of real screen */
	unsigned long	vc_visible_origin;	/* [!] Top of visible window */
	unsigned int	vc_top;
    unsigned int    vc_bottom;
	unsigned int	*vc_screenbuf;		//显存地址
	unsigned int	vc_screenbuf_size;  //显存界限

	/* -------------------光标位置设定(cursor) --------------------*/
	/*static constant cursor*/
	unsigned int 	vc_origin_x;		//X轴光标初始位置
	unsigned int    vc_origin_y;		//y轴光标初始位置
	/*dynamic cursor*/
    unsigned int	vc_x;		        //X轴光标位置
    unsigned int    vc_y;               //Y轴光标位置

    unsigned int    vc_ch_size;         //字符大小设置(像素PX)
    unsigned int    vc_offset;          //光标的前进步长
    
    unsigned int	*vc_fontbuf;		//Font BMP的缓冲区地址
	unsigned int	vc_fontbuf_size;    //Font BMP的缓冲区大小
    unsigned int	vc_fontImageWidth;  //Font BMP的宽度
    unsigned int	vc_fontImageHight;  //Font BMP的高度
}vc_data;

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