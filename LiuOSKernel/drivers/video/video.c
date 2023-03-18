#include"video.h"

/*-----------------------------------------------------------------------------
* 定义并初始化屏幕极值点
*------------------------------------------------------------------------------*/
POINT UPPER_LEFT = {0,0};   //左上角极值
POINT LOWER_RIGHT = {0,0};  //右下角极值

/*定义基础颜色BGRR*/
EFI_GRAPHICS_OUTPUT_BLT_PIXEL BLUE = {255,0,0,0};     //蓝色像素
EFI_GRAPHICS_OUTPUT_BLT_PIXEL RED = {0,0,255,0};     //红像素
EFI_GRAPHICS_OUTPUT_BLT_PIXEL BLACK = {0,0,0,0};       //黑像素

/*-----------------------------------------------------------------------------
* 操作系统内部初始化基础Console输出
* @name: video_init
* @function: 在操作系统界面绘制Console基本轮廓和填充图
* @param : 1.读取UEFI传输给操作系统的参数中显示控制部分参数
* @update:
        2023-3-7: 将视频初始化和VC Console通用结构体进行合并
*------------------------------------------------------------------------------*/
void video_init(IN GRAPHIC_CONFIG * graphic_settings)
{
	g_screen.vc_cols = graphic_settings->HorizontalResolution;		    //列
	g_screen.vc_rows = graphic_settings->VerticalResolution;            //行
	g_screen.vc_size_row = g_screen.vc_rows * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
	g_screen.vc_scan_lines = graphic_settings->PixelsPerScanLine;		//扫描线数据
	g_screen.vc_origin = 0;		/* [!] Start of real screen */
	g_screen.vc_scr_end = 0;		/* [!] End of real screen */
	g_screen.vc_visible_origin = 0;	/* [!] Top of visible window */
	g_screen.vc_top = 0;
    g_screen.vc_bottom = 0;	        /* Scrolling region */
	g_screen.vc_screenbuf = (unsigned int *)graphic_settings->FrameBufferBase;	//显存地址
	g_screen.vc_screenbuf_size = graphic_settings->FrameBufferSize;//显存界限


    /*-----------------------------------------------------------------------------
    * 赋值屏幕右下角极值点
    *------------------------------------------------------------------------------*/
    LOWER_RIGHT.X = g_screen.vc_cols;
    LOWER_RIGHT.Y = g_screen.vc_rows;

    /*----------------------------------------------------------------------------
    * 界面颜色填充黑色
    *------------------------------------------------------------------------------*/
    console_cls();
}

/*-----------------------------------------------------------------------------
* 画屏幕像素点
* @name: video_init
* @function: 在操作系统界面绘制Console基本狂徒
* @param : 1.输出像素点所在的坐标pos(X,Y)和颜色
* @update:
        2023-3-4: 修复像素点画图程序
*------------------------------------------------------------------------------*/
void 
draw_pixel(IN PIXEL_COLOR *pixel_color)
{
    UINTN graphics_bltpixel_size = sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL);       //像素单位大小
    EFI_PHYSICAL_ADDRESS BlueStart = (EFI_PHYSICAL_ADDRESS)g_screen.vc_screenbuf 
                                    + g_screen.vc_scan_lines * graphics_bltpixel_size * (pixel_color->m_pos.Y); 
                                                                          //帧缓冲区基地址预空50行(一个像素4B)  
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL *videoMemory = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)BlueStart + pixel_color->m_pos.X; //BltBuffer
    *videoMemory = pixel_color->m_color;                                   //写入像素颜色
}

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
    IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL colorPixelValue)
{
    /*-------------------------------------------------------
    (0,0)*****
         *****
         *****
         *****
         *****(x.y)
    -------------------------------------------------------*/
    if(drawMethod == PADDING)               //当前采用填充模式
    {
        for(int i = upperLeft.Y; i < lowerRight.Y; i++){
            for(int j = upperLeft.X; j < lowerRight.X; j++){
                POINT pos = {j,i};
                PIXEL_COLOR pixel_color = {pos,colorPixelValue};
                draw_pixel(&pixel_color);
            }
        }
    }
    else if(drawMethod == OUTLINE && outlineSize) //当前采用轮廓模式且粗细不为0
    {
        /*-------------------------------------------------------
        (0,0)*****
             *   *
             *   *
             *   *
             *****(x.y)
        -------------------------------------------------------*/
        for(int i = upperLeft.Y; i < upperLeft.Y + outlineSize; i++){    //遍历列(上边框)
            for(int j = upperLeft.X; j < lowerRight.X; j++){             //遍历行(上边框)
                POINT pos = {j,i};
                PIXEL_COLOR pixel_color = {pos,colorPixelValue};
                draw_pixel(&pixel_color);
            }
        }
        for(int i = lowerRight.Y - outlineSize; i < lowerRight.Y; i++){    //遍历列(下边框)
            for(int j = upperLeft.X; j < lowerRight.X; j++){             //遍历行(下边框)
                POINT pos = {j,i};
                PIXEL_COLOR pixel_color = {pos,colorPixelValue};
                draw_pixel(&pixel_color);
            }
        }
        for(int i =  upperLeft.Y + outlineSize; i < lowerRight.Y - outlineSize; i++){  //遍历列(左/右边框)
            for(int j = upperLeft.X; j < upperLeft.X + outlineSize; j++){              //左边框
                POINT pos = {j,i};
                PIXEL_COLOR pixel_color = {pos,colorPixelValue};
                draw_pixel(&pixel_color);
            }
            for(int j = lowerRight.X - outlineSize; j <  lowerRight.X; j++){           //右边框
                POINT pos = {j,i};
                PIXEL_COLOR pixel_color = {pos,colorPixelValue};
                draw_pixel(&pixel_color);
            }
        }
    }
    return;
}