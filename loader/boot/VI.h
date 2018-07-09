/**
 * Copyright2013-2015 by Explorer OS Developers. All rights reserved.
 * made by Rize<heseri@GhostBirdOS.org>
 * Explorer loader VI Header
 */
 
#ifndef VI_H_
#define VI_H_

/**The minimum and maxmum resolution*/
#define		VI_min_width
#define		VI_min_height
#define		VI_min_width
#define		VI_min_height

/**页号*/
#define		VI_page_init 0
#define		VI_page_output 1
#define		VI_page_select 2

/**回调函数的参数定义*/
#define		VI_DO_PRES 1
#define		VI_DO_OVER 2

/**选择活动页函数*/
int VI_active(int VI_page);

/**选择清除函数*/
void select_clean(void);

/**选择注册函数*/
int select_register(const unsigned long n, int (*callback)(int n, int type), const char *text, ...);

/**设置活动选择函数*/
int select_set_active(unsigned long n);

/**确定选择函数*/
void select_press(void);

/**向上选择函数*/
void select_up(void);

/**向下选择函数*/
void select_down(void);

/**打印标题函数*/
int printt(const char *fmt, ...);

/**高级内核信息打印函数*/
int printak(const char *fmt, ...);

/**标准内核信息打印函数*/
int printk(const char *fmt, ...);

/**注意区域打印函数*/
int printn(const char *fmt, ...);

/**说明区域打印函数*/
int printi(const char *fmt, ...);

/**错误输出函数*/
int error(unsigned int errcode, const char *fmt, ...);

/**警告输出函数*/
int warning(unsigned int errcode, const char *fmt, ...);

/**初始化可视化界面*/
void init_VI(void);

#endif
