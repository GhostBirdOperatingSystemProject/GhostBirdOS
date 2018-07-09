/**
 * Copyright 2013-2015 by Explorer Developers.
 * made by Lab Explorer Developers<1@GhostBirdOS.org>
 * Explorer Graphics Library Definition
 * Explorer/include/lib/graphics.h
 * version:Alpha
 * 11/1/2014 12:09 AM
 */

#ifndef GRAPHICS_H_
#define GRAPHICS_H_

struct Video_Info{
	unsigned long xres, yres;
	unsigned long bpp;
	unsigned char *vram;
	unsigned long vlen;
}Video_Info;


/**绘制像素函数*/
void (*putpixel)(unsigned int x, unsigned int y, unsigned int color);

/**获取像素函数*/
unsigned int (*getpixel)(unsigned int x, unsigned int y);

void init_graphics
(unsigned long xres, unsigned long yres, unsigned long bpp, unsigned char *vram);

void outtextxy(unsigned long x, unsigned long y, unsigned int color, unsigned char *string);
void draw_font(unsigned long x, unsigned long y, unsigned int color, unsigned char ascii);
void rectangle(unsigned long x, unsigned long y, unsigned long height, unsigned long width, unsigned int color);
void line(unsigned long x0, unsigned long y0, unsigned long x1, unsigned long y1, unsigned int color);

/**24位色彩模式获取像素函数*/
unsigned int getpixel24(unsigned int x, unsigned int y);

/**32位色彩模式获取像素函数*/
unsigned int getpixel32(unsigned int x, unsigned int y);

/**24位色彩模式绘制像素函数*/
void putpixel24(unsigned int x, unsigned int y, unsigned int color);

/**32位色彩模式绘制像素函数*/
void putpixel32(unsigned int x, unsigned int y, unsigned int color);

void clear_screen(void);

#endif
