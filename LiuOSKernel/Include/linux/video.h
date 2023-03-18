#ifndef _VIDEO_H
#define _VIDEO_H
#pragma once
#include"type.h"
#include"console_struct.h"

extern struct vc_data g_screen;
extern void console_cls();

/*-----------------------------------------------------------------------------
* 操作系统内部初始化基础Console输出
* @name: video_init
* @function: 在操作系统界面绘制Console基本轮廓和填充图
* @param : 1.读取UEFI传输给操作系统的参数中显示控制部分参数
*------------------------------------------------------------------------------*/
void video_init(IN GRAPHIC_CONFIG * graphic_settings);

/*-----------------------------------------------------------------------------
* 画屏幕像素点
* @name: video_init
* @function: 在操作系统界面绘制Console基本狂徒
* @param : 1.输出像素点所在的坐标pos(X,Y)和颜色
* @update:
        2023-3-4: 修复像素点画图程序
*------------------------------------------------------------------------------*/
void draw_pixel(IN PIXEL_COLOR *pixel_color);

/*-----------------------------------------------------------------------------
* 画屏幕方块
* @name: draw_rectangle
* @function: 在界面上绘制方形
* @param : 1.屏幕方块左上角坐标
           2.屏幕方块右下角坐标
           3.轮廓或填充选项(enum DRAWMETHOD)
           4.方形轮廓粗细(像素大小)(填充模式粗细默认为0)
           5.轮廓或内部填充颜色设置
*------------------------------------------------------------------------------*/
void draw_rectangle(
    IN POINT upperLeft,                        //start
    IN POINT lowerRight,                       //end
    IN UINT32 drawMethod,
    IN UINT32 outlineSize,
    IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL colorPixelValue
);

/*-----------------------------------------------------------------------------
* 画圆
* @name: draw_square
* @function: 在界面上绘制圆
* @param : 1.读取UEFI传输给操作系统的参数中显示控制部分参数
*------------------------------------------------------------------------------*/
void draw_square();

#endif //_VIDEO_H