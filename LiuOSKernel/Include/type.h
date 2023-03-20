#ifndef _TYPE_H_
#define _TYPE_H_
#pragma once
#include"RunTimeKernelConf.h"     //UEFI传递内核参数
#include"string.h"
#include"stdlib.h"
#include"stdio.h"

#define IN                        //输入参数
#define OUT                       //输出参数

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