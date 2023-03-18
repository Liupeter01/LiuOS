#include"console.h"
#include"string.h"

vc_data g_screen  = {	
    .vc_num = 0,			//Console number 
	.vc_cols = 0,		/* [#] Console size */
	.vc_rows = 0,
	.vc_size_row = 0,		/* Bytes per row */
	.vc_scan_lines = 0,		/* # of scan lines */
	.vc_origin = 0,		/* [!] Start of real screen */
	.vc_scr_end = 0,		/* [!] End of real screen */
	.vc_visible_origin = 0,	/* [!] Top of visible window */
	.vc_top = 0,
    .vc_bottom = 0,	/* Scrolling region */
	.vc_screenbuf = NULL,		/* In-memory character/attribute buffer */
	.vc_screenbuf_size = 0,

    .vc_origin_x = 0,       //X轴光标初始位置
    .vc_origin_y = 0,       //X轴光标初始位置
    .vc_x = 0,		        //X轴光标位置
    .vc_y = 0,              //Y轴光标位置

    .vc_ch_size = 0,        //字符大小设置(像素PX)
    .vc_offset = 0,         //光标的前进步长

    .vc_fontbuf = NULL,		//Font BMP的缓冲区地址
	.vc_fontbuf_size = 0,    //Font BMP的缓冲区大小
    .vc_fontImageWidth = 0,  //Font BMP的宽度
    .vc_fontImageHight = 0  //Font BMP的高度
};

/*-----------------------------------------------------------------------------
* 操作系统内部初始化基础Console字符输出
* @name: console_init
* @function: 在操作系统界面绘制Console基本
* @param : 1.读取UEFI传输给操作系统的参数中字体控制部分参数
*------------------------------------------------------------------------------*/
void console_init(IN FONT_CONFIG *font_config)
{
    /*static constant cursor*/
    g_screen.vc_origin_x = 16;                                   //光标X轴初始显示位置(静态常量不可改变)
    g_screen.vc_origin_y = 16;                                   //光标Y轴初始显示位置(静态常量不可改变)  

    /*dynamic cursor*/
    g_screen.vc_x = g_screen.vc_origin_x;                       //可动光标X轴
    g_screen.vc_y = g_screen.vc_origin_y;	                     //可动光标Y轴

    g_screen.vc_ch_size = font_config->m_fontPixelSize;          //字符大小设置(像素PX)
    g_screen.vc_offset = font_config->m_fontCharacterOffset;     //光标的前进步长(像素PX)

    g_screen.vc_fontbuf = (unsigned int *)font_config->m_fontBuffer;//Font BMP的缓冲区地址
	g_screen.vc_fontbuf_size = font_config->m_fontBufferSize;    //Font BMP的缓冲区大小
    g_screen.vc_fontImageWidth = font_config->m_Width;           //Font BMP的宽度
    g_screen.vc_fontImageHight = font_config->m_Height;          //Font BMP的高度

    /*----------------------------------------------------------------------------
    * 界面颜色填充黑色
    *------------------------------------------------------------------------------*/
    console_cls();
}

/*-----------------------------------------------------------------------------
* 在vc_fontbuf上根据ascii数值寻找字符所在起始地址
* @name: find_ch_pixel
* @function: 在vc_fontbuf上根据ascii数值寻找字符所在起始地址
* @param : 1.font BMP buffer(vc_fontbuf)的起始地址
		   2.ASCII数值
           
* @retvalue:返回ascii对应的字符所在的像素地址
*------------------------------------------------------------------------------*/
UINT32 *find_ch_pixel(
	OUT UINT32 *buffer_start,
	IN const UINT32 ascii)
{
    UINT32 ch_per_line = g_screen.vc_fontImageWidth / g_screen.vc_ch_size;   //一行几个字符
    UINT32 ch_line_number = ascii / ch_per_line;                             //ASCII行号
    UINT32 ch_row_number = ascii % ch_per_line;                              //ASCII列号
    
    return  buffer_start + ch_line_number * g_screen.vc_fontImageWidth * g_screen.vc_ch_size +    //列偏移
             ch_row_number *  g_screen.vc_ch_size;    //行偏移
}

/*-----------------------------------------------------------------------------
* 在字符所在起始地址上根据相对坐标获得像素数值
* @name: get_pixel_color
* @function: 在字符所在基地址上根据相对坐标(X,Y){(0,0)->(g_screen.vc_ch_size,g_screen.vc_ch_size)}获得像素
* @param : 1.已经根据ASCII计算的pixel坐标(来源：find_ch_pixel)
		   2.相对字符基地址X偏移
           3.相对字符基地址Y偏移
           
* @retvalue:返回坐标(X,Y)的像素
*------------------------------------------------------------------------------*/
EFI_GRAPHICS_OUTPUT_BLT_PIXEL 
get_pixel_color(
    IN UINT32 *fontbuffer,     //当前字体起始地址
    IN UINT32 x_pos,           //当前字体像素点坐标X
    IN UINT32 y_pos)           //当前字体像素点坐标Y
{
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL *getColor = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL*)fontbuffer + 
              y_pos * g_screen.vc_fontImageWidth +  x_pos;
    return *getColor;
}

/*-----------------------------------------------------------------------------
* 将光标进行换行操作
* @name: cursor_wrapping_lines
* @function: 将光标X坐标设置为默认值并移动Y轴坐标
* @param : 输入需要换的行数
*------------------------------------------------------------------------------*/
void 
cursor_wrapping_lines(IN int lines)
{
    g_screen.vc_x = g_screen.vc_origin_x;           //光标X回溯到初始位置
    g_screen.vc_y += (lines * g_screen.vc_ch_size); //光标Y向下移动一个lines个单位长度
}

/*-----------------------------------------------------------------------------
* 将字符以BMP像素方式输出到屏幕
* @name: console_putc
* @function: 将单个字符输出的屏幕上
* @param : 输入的单个字符(32<=ascii<=126)
* @update: 2023-3-9 修复控制逻辑问题，引入对于换行符的处理：__line__ 141
*------------------------------------------------------------------------------*/
void 
console_putc(IN const char ch)
{
    UINT32 asciiCode;                  //从ch转换为ASCII代码
    if(ch < 32 || ch >126){            //不可显示字符
          asciiCode = 127 - 32;        //统一处理为“_”
    }
    if(ch == '\n'){                //如果输入字符中存在换行符 
        cursor_wrapping_lines(1);      //光标换行
        return;
    }
    asciiCode = ch - 32;                //计算ASCII在图片中的编码值

    UINT32 *targetAscii = (UINT32*)find_ch_pixel(g_screen.vc_fontbuf , asciiCode);  //寻找字符像素起始地址
    for(int i =  g_screen.vc_y ; i < g_screen.vc_y + g_screen.vc_ch_size ; ++i){     //从Y光标出发
        for(int j =  g_screen.vc_x ; j < g_screen.vc_x + g_screen.vc_ch_size ; ++j){ //从X光标触发
            EFI_GRAPHICS_OUTPUT_BLT_PIXEL color =  get_pixel_color(
                targetAscii,                                                    //当前字符像素起始地址
                j -  g_screen.vc_x,                                            //当前字符在内存相对偏移
                i -  g_screen.vc_y
            );
            POINT point = {j,i};                  //像素点坐标
            PIXEL_COLOR pixel = { point,color };  //像素点颜色
            draw_pixel(&pixel);                   //画像素点
        }
    }
    //当前输出一个字符，因此横坐标偏移一个字符的宽度
    g_screen.vc_x += g_screen.vc_ch_size;
    //检测当前一行下能否容纳这个下一个字符，否则换行
    if( g_screen.vc_x+g_screen.vc_ch_size > g_screen.vc_scan_lines)
    {
        cursor_wrapping_lines(1);                   //换行
        //如果当前页面无法容纳Y轴内容，则向上滚动
    }
}


/*-----------------------------------------------------------------------------
* 将字符串以BMP像素方式输出到屏幕
* @name: console_puts
* @function: 将字符串输出的屏幕上
* @param : 输入字符串
* @update:2023-3-9 优化对于换行符的处理，将其合并到console_putc函数中
*------------------------------------------------------------------------------*/
void 
console_puts(IN const char *str)
{
    for(size_t i = 0 ; i < strlen(str); ++i){
        console_putc(str[i]);                   //输出字符
    }
}

/*-----------------------------------------------------------------------------
* 控制台console清空画面
* @name: console_cls
* @function: 将整个屏幕用黑色覆盖进行清空
*------------------------------------------------------------------------------*/
void console_cls()
{
    draw_rectangle(                                     //画黑长方形填充
        UPPER_LEFT,LOWER_RIGHT,PADDING,
        0,
        BLACK
    );
}