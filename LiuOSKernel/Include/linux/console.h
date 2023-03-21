#ifndef _CONSOLE_H_
#define _CONSOLE_H_
#pragma once
#include"type.h"
#include"FontLoader.h"      //获取字体文件的数据结构
#include"StartLogo.h"       //获取BMP文件的数据结构
#include"video.h"


/*-----------------------------------------------------------------------------
* 操作系统内部初始化基础Console字符输出
* @name: console_init
* @function: 在操作系统界面绘制Console基本
* @param : 1.读取UEFI传输给操作系统的参数中字体控制部分参数
*------------------------------------------------------------------------------*/
void console_init(IN FONT_CONFIG *font_config);

/*-----------------------------------------------------------------------------
* 在vc_fontbuf上根据ascii数值寻找字符所在起始地址
* @name: find_ch_pixel
* @function: 在vc_fontbuf上根据ascii数值寻找字符所在起始地址
* @param : 1.font BMP buffer(vc_fontbuf)的起始地址
		   2.ASCII数值
           
* @retvalue:返回ascii对应的字符所在的像素地址
*------------------------------------------------------------------------------*/
UINT32 *find_ch_pixel(
	OUT UINT32 *buffer_start ,
	IN const UINT32 ascii
);

/*-----------------------------------------------------------------------------
* 在字符所在起始地址上根据相对坐标获得像素数值
* @name: get_pixel_color
* @function: 在字符所在基地址上根据相对坐标(X,Y){(0,0)->(g_screen.vc_ch_size,g_screen.vc_ch_size)}获得像素
* @param : 1.已经根据ASCII计算的pixel坐标(来源：find_ch_pixel)
		   2.相对字符基地址X偏移
           3.相对字符基地址Y偏移
           
* @retvalue:返回坐标(X,Y)的像素
*------------------------------------------------------------------------------*/
EFI_GRAPHICS_OUTPUT_BLT_PIXEL  get_pixel_color(
    IN UINT32 *fontbuffer,     //当前字体起始地址
    IN UINT32 x_pos,           //当前字体像素点坐标X
    IN UINT32 y_pos             //当前字体像素点坐标Y
);

/*-----------------------------------------------------------------------------
* 将光标进行换行操作
* @name: cursor_wrapping_lines
* @function: 将光标X坐标设置为默认值并移动Y轴坐标
* @param : 输入需要换的行数
*------------------------------------------------------------------------------*/
void cursor_wrapping_lines(IN int lines);

/*-----------------------------------------------------------------------------
* 将字符以BMP像素方式输出到屏幕
* @name: console_putc
* @function: 将单个字符输出的屏幕上
* @param : 输入字符
*------------------------------------------------------------------------------*/
void console_putc(IN const char ch);

/*-----------------------------------------------------------------------------
* 将字符串以BMP像素方式输出到屏幕
* @name: console_puts
* @function: 将字符串输出的屏幕上
* @param : 输入字符串
*------------------------------------------------------------------------------*/
void console_puts(IN const char *str);

/*-----------------------------------------------------------------------------
* 控制台console清空画面
* @name: console_cls
* @function: 将整个屏幕用黑色覆盖进行清空
*------------------------------------------------------------------------------*/
void console_cls();

#endif //_CONSOLE_H_
