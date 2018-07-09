/**
 * Copyright 2013-2015 by Explorer Developers.
 * made by Lab Explorer Developers<1@GhostBirdOS.org>
 * Smilodon Project - Rose<roselyu@163.com>
 * Explorer Graphics Library
 * Explorer/lib/graphics.c
 * version:Alpha
 * 11/1/2014 11:56 AM
 */

#include "../head.h"
#include "mem.h"
#include "font.h"
//#include <lib/math.h>
#include "graphics.h"

/**显示像素函数*/
void (*putpixel)(unsigned int x, unsigned int y, unsigned int color);

/**获取像素函数*/
unsigned int (*getpixel)(unsigned int x, unsigned int y);

/**graphical initialization*/
void init_graphics
(unsigned long xres, unsigned long yres, unsigned long bpp, unsigned char *vram)
{
	Video_Info.xres = xres;
	Video_Info.yres = yres;
	Video_Info.bpp  = bpp;
	Video_Info.vram = vram;
	Video_Info.vlen = (((xres * yres) * (bpp / sizeof(char))) & 0xfffff000) + 0x1000;

	/**根据BPP判断所需要的相应显示像素函数*/
	switch (bpp)
	{
		case 24:
			putpixel = putpixel24;
			getpixel = getpixel24;
			break;
		case 32:
			putpixel = putpixel32;
			getpixel = getpixel32;
			break;
		default: reset();
	}
}

/**put a string in screen*/
void outtextxy(unsigned long x, unsigned long y, unsigned int color, unsigned char *string)
{
	unsigned long point;

	for (point = 0; string[point] != 0x00; point ++)
	{
		draw_font(x, y, color, string[point]);
		x += 8;
	}
}

/*显示字*/
void draw_font(unsigned long x, unsigned long y, unsigned int color, unsigned char ascii)
{
	unsigned long p, i, font_offset;/*字库偏移量*/
	unsigned char d;
	font_offset = ascii * 16;
	for (i = 0; i < 16; i++)
	{
		d = font[font_offset + i];
		if ((d & 0x80) != 0) { putpixel(x, y + i, color); }
		if ((d & 0x40) != 0) { putpixel(x + 1, y + i, color); }
		if ((d & 0x20) != 0) { putpixel(x + 2, y + i, color); }
		if ((d & 0x10) != 0) { putpixel(x + 3, y + i, color); }
		if ((d & 0x08) != 0) { putpixel(x + 4, y + i, color); }
		if ((d & 0x04) != 0) { putpixel(x + 5, y + i, color); }
		if ((d & 0x02) != 0) { putpixel(x + 6, y + i, color); }
		if ((d & 0x01) != 0) { putpixel(x + 7, y + i, color); }
	}
}

/*在屏幕指定地方画方块*/
void rectangle(unsigned long x, unsigned long y, unsigned long height, unsigned long width, unsigned int color)
{
	unsigned long m, n;
	for (n = 0; n != width; n ++)
	{
		for (m = 0; m != height; m ++)
		{
			putpixel(x + m, y + n, color);
		}
	}
}

void line(unsigned long x0, unsigned long y0, unsigned long x1, unsigned long y1, unsigned int color)
{
	int dx, dy, n, k, i, f;
	int x, y;
	dx = abs(x1 - x0);
	dy = abs(y1 - y0);
	n = dx + dy;
	
	/**根据x1和x0的关系，选择合适的k运算方法*/
	if (x1 == x0)
	{
		k=2;
		x=x0;
		y=y0;
	}else if(x1>=x0)
	{
		k=y1>=y0?1:4;
		x=x0;
		y=y0;
	}else{
		k=y1>=y0?2:4;
		x=x0;
		y=y0;
	}

	for(i=0,f=0;i<n;i++)
		if(f>=0)
		switch(k)
		{
			case 1:putpixel(x++, y, color);f-=dy;break;
			case 2:putpixel(x, y++, color);f-=dx;break;
			case 3:putpixel(x--, y, color);f-=dy;break;
			case 4:putpixel(x, y--, color);f-=dx;break;
		}else
		switch(k)
		{
			case 1:putpixel(x, y++, color);f+=dx;break;
			case 2:putpixel(x--, y, color);f+=dy;break;
			case 3:putpixel(x, y--, color);f+=dx;break;
			case 4:putpixel(x++, y, color);f+=dy;break;
		}
}

/**24位色彩模式获取像素函数*/
unsigned int getpixel24(unsigned int x, unsigned int y)
{
	unsigned int i;
	
	/**先判断该像素是否在屏幕上*/
	if (x < Video_Info.xres & y < Video_Info.yres)
	{
		i = ((y * Video_Info.xres) + x) * 3;
		return (Video_Info.vram[i] + (Video_Info.vram[i+1] << 8) + (Video_Info.vram[i+2] << 16));
	}
}

/**24位色彩模式获取像素函数*/
unsigned int getpixel32(unsigned int x, unsigned int y)
{
	/**先判断该像素是否在屏幕上*/
	if (x < Video_Info.xres & y < Video_Info.yres)
	{
		return ((unsigned int *)Video_Info.vram)[(y * Video_Info.xres) + x];
	}
}

/**24位色彩模式绘制像素函数*/
void putpixel24(unsigned int x, unsigned int y, unsigned int color)
{
	int i;
	unsigned char *vram = Video_Info.vram;
	
	/**先判断该像素是否在屏幕上*/
	if ((x < Video_Info.xres) & (y < Video_Info.yres))
	{
		i = ((y * Video_Info.xres) + x)*3;
		vram[i] = color;
		vram[i+1] = color >> 8;
		vram[i+2] = color >> 16;
	}
}

void putpixel32(unsigned int x, unsigned int y, unsigned int color)
{
	/**先判断该像素是否在屏幕上*/
	if ((x < Video_Info.xres) & (y < Video_Info.yres))
	{
		((unsigned int *)Video_Info.vram)[(y * Video_Info.xres) + x] = color;
	}
}

/**清除屏幕函数*/
void clear_screen(void)
{
	memset(Video_Info.vram, 0, Video_Info.vlen);	
}
