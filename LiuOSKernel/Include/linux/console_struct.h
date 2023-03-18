#ifndef _CONSOLE_STRUCT_H_
#define _CONSOLE_STRUCT_H_
#pragma once
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

#endif // _CONSOLE_STRUCT_H_