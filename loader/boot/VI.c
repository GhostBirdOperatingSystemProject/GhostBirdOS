/**
 * Copyright 2013-2015 by Explorer Developers.
 * made by Hu Wenjie<1@hwj.me>
 * made by Rize<heseri@ghostbirdos.org>
 * Explorer Loader Visual Interface
 * /loader/boot/shell.c
 * version:4.0.0
 * 7/5/2014 7:08 PM
 */

#include "lib/stdarg.h"
#include "lib/stdlib.h"
#include "lib/stdbool.h"
#include "lib/string.h"
#include "lib/mem.h"
#include "lib/font.h"
#include "lib/graphics.h"
#include "../configure.h"
#include "VI.h"

/**各种定义集合*/

/**printk缓存大小*/
#define		BUFFER_SIZE 4096

/**输出页的光标*/
unsigned long output_cursor_x, output_cursor_y;

/***/

/**当前活动页*/
int VI_current_page;

/**存放信息的内存*/
char *VI_output_buf, *VI_introduce_buf;
unsigned int *VI_output_back_buf;

/**各区域起始位置*/
unsigned long	VI_x, VI_title_y, VI_page_y, VI_notice_y, \
		VI_introduce_x, VI_introduce_y, VI_select_x, VI_select_y;

/**活动选项，0xffffffff代表未选择*/
unsigned long select_active;

/**选择结构体*/
struct VI_select
{
	bool exist;
	/**选项字符串*/
	char text[VI_select_font_line];
	/**回调函数*/
	int (*callback)(int n, int type);
}*VI_select;

// VI相关数据储存所需要的内存
#define		VI_output_buf_size \
		VI_output_line * VI_output_column		// 输出储存大小
#define		VI_introduce_buf_size \
		VI_introduce_line * VI_introduce_column		// 说明储存大小
#define		VI_select_buf_size \
		VI_select_number * sizeof(struct VI_select)	// 选择结构体大小

/**输出页的字符颜色*/
unsigned long output_font_color;

/**可视化界面控制函数集合*/

/**选择活动页函数*/
int VI_active(int VI_page)
{
	unsigned long x, y, n;
	
	/**选择输出页*/
	if ((VI_page == VI_page_output) && (VI_current_page != VI_page_output))
	{
		VI_current_page = VI_page_output;
		
		/**绘制输出页*/
		rectangle(VI_x, VI_page_y, VI_width, VI_page_height, output_back_color);
		for (y = 0; y < VI_output_column; y ++)
		{
			for (x = 0; x < VI_output_line; x ++)
			{
				draw_font(VI_x + (x * FONTWIDTH), VI_page_y + (y * FONTHEIGHT), VI_output_back_buf[x + (y * VI_output_line)], VI_output_buf[x + (y * VI_output_line)]);
			}
		}
		
	/**选择选择页*/
	}else if ((VI_page == VI_page_select) && (VI_current_page != VI_page_select))
	{
		VI_current_page = VI_page_select;
		
		/**绘制选择页背景*/
		rectangle(VI_x, VI_page_y, VI_width, VI_page_height, not_select_back_color);
		
		/**绘制说明区域*/
		x = 0;
		y = 0;
		for (n = 0; n < VI_introduce_line * VI_introduce_column; n ++)
		{
			draw_font(VI_introduce_x + (x * FONTWIDTH), VI_introduce_y + (y * FONTHEIGHT), introduce_font_color, VI_introduce_buf[n]);
			x ++;
			if (x >= VI_introduce_line)
			{
				x = 0;
				y ++;
			}
		}
		
		/**绘制选择项*/
		for (n = 0; n < VI_select_number; n ++)
		{
			if (VI_select[n].exist == true)
			{
				/**如果当前项不是活动选项，则按不是活动选项绘制*/
				if (n != select_active)
				{
					rectangle(VI_select_x, VI_select_y + (n * FONTHEIGHT), VI_select_width, FONTHEIGHT, not_select_back_color);
					outtextxy(VI_select_x, VI_select_y + (n * FONTHEIGHT), not_select_font_color, VI_select[n].text);
				/**否则按活动选项绘制*/
				}else{
					rectangle(VI_select_x, VI_select_y + (n * FONTHEIGHT), VI_select_width, FONTHEIGHT, select_back_color);
					outtextxy(VI_select_x, VI_select_y + (n * FONTHEIGHT), select_font_color, VI_select[n].text);
				}
			}
		}
		
	}else{
		/**错误返回*/
		return -1;
	}
}

/**滚动输入页函数*/
void roll_screen(void)
{
	unsigned long x, y, n;

	/**滚动输出储存*/
	for (n = 0; n < ((VI_output_line * VI_output_column) - VI_output_line); n ++)
	{
		VI_output_buf[n] = VI_output_buf[n + VI_output_line];
		VI_output_back_buf[n] = VI_output_back_buf[n + VI_output_line];
	}
	
	/**清空最下一行输出储存*/
	for (; n < (VI_output_line * VI_output_column); n ++)
	{
		VI_output_buf[n] = 0;
	}
	
	/**若输出页为当前活动页，则刷新界面*/
	if (VI_current_page == VI_page_output)
	{
		/**循环将第一行之外的其他行向上提一行*/
		for (y = (VI_page_y + FONTHEIGHT); y < VI_page_y + VI_page_height; y ++)
		{
			for (x = 0; x < VI_x + VI_width; x ++)
			{
				putpixel(x, y - FONTHEIGHT, getpixel(x, y));
			}
		}
		
		/**清空最下面一行*/
		rectangle(VI_x, VI_page_y + VI_page_height - FONTHEIGHT, VI_width, FONTHEIGHT, output_back_color);
	}
	
	output_cursor_y --;
}

/**设置输出页字符颜色*/
void set_output_font_color(unsigned int color)
{
	output_font_color = color;
	return;
}

/**输出一个字符到输出页上*/
void put_char(char ascii)
{
	/**对控制字符的判断*/
	if (ascii < 0x20)
	{
		if(ascii == 0xD)
		{
			output_cursor_x = 0;
		}

		/**如果输入字符是换行符，则进行换行*/
		if(ascii == 0xA)
		{
			output_cursor_x = 0;
			output_cursor_y ++;
		}
	
	/**如果不是控制字符*/
	}else{
		VI_output_buf[output_cursor_x + (output_cursor_y * VI_output_line)] = ascii;
		VI_output_back_buf[output_cursor_x + (output_cursor_y * VI_output_line)] = output_font_color;
		
		/**若当前页就是输出页，则刷新屏幕*/
		if (VI_current_page == VI_page_output)
		{
			/**在光标处位置显示字*/
			draw_font(VI_x + (output_cursor_x * FONTWIDTH), VI_page_y + (output_cursor_y * FONTHEIGHT), output_font_color, ascii);
		}
		
		/**将光标移动到下一个位置*/
		output_cursor_x ++;
	}
	
	/**判断是否需要换行*/
	if (output_cursor_x >= VI_output_line)
	{
		output_cursor_x = 0;
		output_cursor_y ++;
	}
	
	if (output_cursor_y >= VI_output_column)
	{
		roll_screen();
	}
}


/**选择清除函数*/
void select_clean(void)
{
	unsigned long n;
	
	select_active = 0xffffffff;
	
	/**清空结构体*/
	for (n = 0; n < VI_select_number; n ++)
	{
		/**标记为不存在*/
		VI_select[n].exist = false;
		/**空字符串的尾部0x00*/
		strncpy(VI_select[n].text, "", 1);
	}
	
	/**若选择页是当前活动页，则同时清除选择区域*/
	if (VI_current_page == VI_page_select)
	{
		rectangle(VI_select_x, VI_select_y, VI_select_width, VI_select_height, not_select_back_color);
	}
}

/**选择注册函数*/
int select_register(const unsigned long n, int (*callback)(int n, int type), const char *text, ...)
{
	unsigned long select_cursor_x;
	char buffer[BUFFER_SIZE];
	va_list arg;
	
	/**进行判断是否越界*/
	if (n >= VI_select_number) return -1;
	
	/**format*/
	va_start(arg, text);/*init argument point*/
	vsprintf(buffer, text, arg);/*write format & get length*/
	
	/**结构体赋值注册*/
	strncpy(VI_select[n].text, buffer, VI_select_font_line);
	
	VI_select[n].callback = callback;
	
	/**标记为存在*/
	VI_select[n].exist = true;
	
	/**防止空指针引发危险*/
	va_end(arg);
	
	if (select_active == n)
	{
		/**Before calling the callback function, we have to check if pointer 
		callback equal (void *)0*/
		if (!VI_select[select_active].callback)
		{
			select_default_callback(n, VI_DO_OVER);
		}else{
			VI_select[select_active].callback(n, VI_DO_OVER);
		}
	}

	/**若之前无选项则默认选择这个*/
	if (select_active == 0xffffffff)
	{
		select_set_active(n);
	/**若选择页是当前活动页，则同时绘制选项*/
	}else if (VI_current_page == VI_page_select)
	{
		/**重绘单个选择背景*/
		rectangle(VI_select_x, VI_select_y + (n * FONTHEIGHT), VI_select_width, FONTHEIGHT, not_select_back_color);
		
		/**循环将字写入标题*/
		for (select_cursor_x = 0; VI_select[n].text[select_cursor_x] != 0; select_cursor_x ++)
		{
			draw_font(VI_select_x + (select_cursor_x * FONTWIDTH), VI_select_y + (n * FONTHEIGHT), not_select_font_color, VI_select[n].text[select_cursor_x]);
		}
	}
	return n;
}

/**设置活动选择函数*/
int select_set_active(unsigned long n)
{
	/**超出范围则异常返回*/
	if (n >= VI_select_number) return -1;
	
	/**若与所选择的一样，则直接返回，防止闪烁*/
	if (n == select_active) return n;
	
	/**若不存在该选项，则直接返回*/
	if (VI_select[n].exist == false)
	{
		return -2;
	}
	
	/**若选择页是当前活动页，则同时清除选择区域*/
	if (VI_current_page == VI_page_select)
	{
		/**若有老的选择则擦除老的活动选择*/
		if (select_active != 0xffffffff)
		{
			rectangle(VI_select_x, VI_select_y + (select_active * FONTHEIGHT), VI_select_width, FONTHEIGHT, not_select_back_color);
			outtextxy(VI_select_x, VI_select_y + (select_active * FONTHEIGHT), not_select_font_color, VI_select[select_active].text);
		}
		
		/**绘制新的活动选择*/
		rectangle(VI_select_x, VI_select_y + (n * FONTHEIGHT), VI_select_width, FONTHEIGHT, select_back_color);
		outtextxy(VI_select_x, VI_select_y + (n * FONTHEIGHT), select_font_color, VI_select[n].text);
	}

	/**设置新的活动选择*/
	select_active = n;
	
	/**Before calling the callback function, we have to check if pointer 
	callback equal (void *)0*/
	if (!VI_select[select_active].callback)
	{
		select_default_callback(n, VI_DO_OVER);
	}else{
		VI_select[select_active].callback(n, VI_DO_OVER);
	}

	/**正常返回*/
	return n;
}

/**Default operating handle*/
int select_default_callback(int n, int type)
{
	printak("<0xadff2f>Vistual Information:</>");

	switch (type)
	{
		case VI_DO_PRES:
			printak("<0xcc2222>Pressed key ENTER on selection No.%d.\n</>", n);
			break;

		case VI_DO_OVER:
			printak("<0x2222cc>Over the selection No.%d.\n</>", n);
			break;

		default:
			printak("<0x666666>Unknown No.%d.\n</>", n);
			break;
	}
	
	return 0;
}

/**确定选择函数*/
void select_press(void)
{
	if (VI_current_page != VI_page_select) return;

	/**判断是否越界*/
	if (select_active == 0xffffffff) return;
	
	/**Before calling the callback function, we have to check if pointer 
	callback equal (void *)0*/
	if (!VI_select[select_active].callback)
	{
		select_default_callback(select_active, VI_DO_PRES);
	}else{
		VI_select[select_active].callback(select_active, VI_DO_PRES);
	}
}

/**向上选择函数*/
void select_up(void)
{
	long n;
	
	if (VI_current_page != VI_page_select) return;
	
	/**判断是否越界*/
	if (select_active == 0xffffffff) return;
	if (select_active == 0) return;
	
	/**活动选项上移*/
	for (n = -1; select_set_active(select_active + n) == -2; n --);
}

/**向下选择函数*/
void select_down(void)
{
	long n;
	
	if (VI_current_page != VI_page_select) return;

	/**判断是否越界*/
	if (select_active == 0xffffffff) return;
	
	/**活动选项下移*/
	for (n = 1; select_set_active(select_active + n) == -2; n ++);
}

/**打印函数集合*/

/**打印标题函数*/
int printt(const char *fmt, ...)
{
	char buffer[BUFFER_SIZE];
	va_list arg;
	unsigned long n;
	
	/**format*/
	va_start(arg, fmt);/*init argument point*/
	vsprintf(buffer, fmt, arg);/*write format & get length*/
	
	/**重绘标题*/
	rectangle(VI_x, VI_title_y, VI_width, title_height, title_back_color);
	
	/**循环将字写入标题*/
	for (n = 0; buffer[n] != 0; n ++)
	{
		draw_font(VI_x + (n * FONTWIDTH), VI_title_y, title_font_color, buffer[n]);
	}
	
	/**防止空指针引发危险*/
	va_end(arg);
	
	/**正常返回*/
	return n;
}

/**高级内核信息打印函数*/
int printak(const char *fmt, ...)
{
	char buffer[BUFFER_SIZE];
	va_list arg;
	unsigned long n;
	unsigned int output_font_color_save;
	
	/**format*/
	va_start(arg, fmt);/*init argument point*/
	vsprintf(buffer, fmt, arg);/*write format & get length*/
	
	/**防止空指针引发危险*/
	va_end(arg);
	
	/**循环处理字*/
	for (n = 0; buffer[n] != 0; n ++)
	{
		// if ((buffer[n] == '<') && (buffer[n + 1] == '<'))
		// {
			// /**输出'<'*/
			// printk("Here");
		// }else 
		if (buffer[n] == '<')
		{
			if ((buffer[n + 1] == '/') && (buffer[n + 2] == '>'))
			{
				n += 2;
				output_font_color = output_font_color_save;
			}else{
				/**信息头*/
				output_font_color_save = output_font_color;
				strtoul(&(buffer[n + 1]), &output_font_color);
				n += strlenctc(&(buffer[n]), '>');
			}
		}else{
			/**输出字符*/
			put_char(buffer[n]);
		}
	}
	
	/**正常返回*/
	return n;
}

/**标准内核信息打印函数*/
int printk(const char *fmt, ...)
{
	char buffer[BUFFER_SIZE];
	va_list arg;
	unsigned long n;
	
	/**format*/
	va_start(arg, fmt);/*init argument point*/
	vsprintf(buffer, fmt, arg);/*write format & get length*/
	
	/**防止空指针引发危险*/
	va_end(arg);
	
	/**循环将字写入屏幕*/
	for (n = 0; buffer[n] != 0; n ++) put_char(buffer[n]);
	
	/**正常返回*/
	return n;
}

/**注意区域打印函数*/
int printn(const char *fmt, ...)
{
	char buffer[BUFFER_SIZE];
	va_list arg;
	unsigned long n;
	
	/**format*/
	va_start(arg, fmt);/*init argument point*/
	vsprintf(buffer, fmt, arg);/*write format & get length*/
	
	/**重绘背景*/
	rectangle(VI_x, VI_notice_y, VI_width, notice_height, notice_back_color);
	
	/**循环将字写入注意*/
	for (n = 0; buffer[n] != 0; n ++)
	{
		draw_font(VI_x + (n * FONTWIDTH), VI_notice_y, notice_font_color, buffer[n]);
	}
	
	/**防止空指针引发危险*/
	va_end(arg);
	
	/**正常返回*/
	return n;
}

/**说明区域打印函数*/
int printi(const char *fmt, ...)
{
	char buffer[BUFFER_SIZE];
	va_list arg;
	unsigned long n;
	/**选择页的光标*/
	unsigned long introduce_cursor_x, introduce_cursor_y;
	
	/**Clean the notice erea*/
	memset(VI_introduce_buf, 0, VI_introduce_buf_size * VI_output_page_number);
	
	/**format*/
	va_start(arg, fmt);/*init argument point*/
	vsprintf(buffer, fmt, arg);/*write format & get length*/
	
	/**重置光标*/
	introduce_cursor_x = 0;
	introduce_cursor_y = 0;
	
	/**重绘背景，若当前页就是输出页，则刷新屏幕*/
	if (VI_current_page == VI_page_select)
	{
		rectangle(VI_introduce_x, VI_introduce_y, VI_introduce_width, VI_introduce_height, not_select_back_color);
	}
	
	/**循环将字写入注意*/
	for (n = 0; buffer[n] != 0; n ++)
	{
		/**若超过注意区域大小，则退出*/
		if (introduce_cursor_y >= VI_introduce_column)
		{
			break;
		}

		/**如果输入字符是换行符，则进行换行*/
		if(buffer[n] == 0xA)
		{
			introduce_cursor_x = 0;
			introduce_cursor_y ++;
		}else{
			/**如果不是控制字符*/

			VI_introduce_buf[n] = buffer[n];
				
			/**若当前页就是输出页，则刷新屏幕*/
			if (VI_current_page == VI_page_select)
			{
				/**在光标处位置显示字*/
				draw_font(VI_introduce_x + (introduce_cursor_x * FONTWIDTH), VI_introduce_y + (introduce_cursor_y * FONTHEIGHT), introduce_font_color, VI_introduce_buf[n]);
			}
		}
			
		/**将光标移动到下一个位置*/
		introduce_cursor_x ++;

		
		/**判断是否需要换行*/
		if (introduce_cursor_x >= VI_introduce_line)
		{
			introduce_cursor_x = 0;
			introduce_cursor_y ++;
		}
	}
	
	/**防止空指针引发危险*/
	va_end(arg);
	
	/**正常返回*/
	return n;
}

/**错误输出函数*/
int error(unsigned int errcode, const char *fmt, ...)
{
	char buffer[BUFFER_SIZE];
	va_list arg;
	unsigned long n;
	
	/**关闭中断*/
	disallow_interrupt();
	
	/**使VI切换到输出界面*/
	VI_active(VI_page_output);
	
	/**输出错误*/
	printk("ERROR(0x%X):", errcode);
	
	/**format*/
	va_start(arg, fmt);/*init argument point*/
	vsprintf(buffer, fmt, arg);/*write format & get length*/
	
	/**循环将字写入屏幕*/
	for (n = 0; buffer[n] != 0; n ++) put_char(buffer[n]);
	
	/**防止空指针引发危险*/
	va_end(arg);
	
	/**不可返回*/
	stillhalt();
}

/**警告输出函数*/
int warning(unsigned int warncode, const char *fmt, ...)
{
	char buffer[BUFFER_SIZE];
	va_list arg;
	unsigned long n;
	
	/**使VI切换到输出界面*/
	VI_active(VI_page_output);
	
	/**输出警告*/
	printk("WARNING(0x%X):", warncode);
	
	/**format*/
	va_start(arg, fmt);/*init argument point*/
	vsprintf(buffer, fmt, arg);/*write format & get length*/
	
	/**循环将字写入屏幕*/
	for (n = 0; buffer[n] != 0; n ++) put_char(buffer[n]);
	
	/**防止空指针引发危险*/
	va_end(arg);
	
	/**换行*/
	printk("\n");
	
	/**正常返回*/
	return n;
}

/**初始化可视化界面*/
void init_VI(void)
{
	unsigned int actual_width, actual_height;
	
	/**Calculate the actual width and height, if video resolution small
	than minimum, loader stop; Larger than the maximum, the actual 
	resolution is the largest; Otherwise actual = video.*/
	
	/**计算起始坐标，保持可视化界面在正中央*/
	VI_x = (Video_Info.xres - VI_width) / 2;
	VI_title_y = (Video_Info.yres - (title_height + VI_page_height + notice_height)) / 2;
	VI_page_y = VI_title_y + title_height;
	VI_notice_y = VI_page_y + VI_page_height;
	VI_introduce_x = VI_x + ((VI_width - VI_introduce_width) / 2);
	VI_introduce_y = VI_page_y + ((VI_page_height - (VI_introduce_height + VI_select_height)) / 2);
	VI_select_x = VI_x + ((VI_width - VI_select_width) / 2);
	VI_select_y = VI_introduce_y + VI_introduce_height;
	
	/**重绘屏幕*/
	rectangle(0, 0, Video_Info.xres, Video_Info.yres, 0x000000);
	rectangle(VI_x, VI_title_y, VI_width, title_height, title_back_color);
	rectangle(VI_x, VI_notice_y, VI_width, notice_height, notice_back_color);
	
	/**各储存分配内存*/
	VI_output_buf = (char *)bmalloc(VI_output_buf_size);
	VI_output_back_buf = (unsigned int *)bmalloc(VI_output_buf_size * sizeof(unsigned int) * VI_output_page_number);
	VI_introduce_buf = (char *)bmalloc(VI_introduce_buf_size * VI_output_page_number);
	VI_select = (struct VI_select *)bmalloc(VI_select_buf_size);
	
	/**检察分配情况*/
	if (VI_output_buf == NULL) reset();
	if (VI_output_back_buf == NULL) reset();
	if (VI_introduce_buf == NULL) reset();
	if (VI_select == NULL) reset();
	
	/**清空输出储存*/
	memset(VI_output_buf, 0, VI_output_buf_size);
	memset(VI_output_back_buf, 0, VI_output_buf_size * sizeof(unsigned int));
	
	/**输出标题信息*/
	printt(software_name " " version " " build);
	
	/**输出注意信息*/
	printn(info);
	
	/**光标初始化*/
	output_cursor_x = 0;
	output_cursor_y = 0;
	
	
	/**标记无选择*/
	select_active = 0xffffffff;
	
	/**选择默认页*/
	VI_current_page = VI_page_init;
	VI_active(VI_page_output);
	
	/**默认颜色设置*/
	set_output_font_color(D_output_font_color);
	
	/**输出必要信息*/
	// printi("Use the %c and %c to select one Operating System and use the Enter to start it.", 24, 25);
	// printi("No Option can be selected.", 24, 25);
	printn("F1 output page            F2 select page            Esc reset");
	printak("<0xFF0000>" copyright "</> <0xFF4500>" author "</>.\n<0x87CEFF>" software_name "</> <0xffff00>" version "</> <0xEE9A00>" build "</>\n" info "\n");
	
	/**模拟测试*/
	// select_register(0, "Ghost Bird test1");
	// select_register(1, "Ghost Bird test2");
	// select_register(2, "Ghost Bird 0.02(Explorer kernel version)");
	// select_register(5, "line 5");
	// select_register(10, "line 10");
	// select_set_active(2);
}
