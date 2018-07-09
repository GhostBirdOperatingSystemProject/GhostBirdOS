// Explorer loader configure
// /configure.h

#ifndef CONFIGURE_H_
#define CONFIGURE_H_

// 描述信息
#define		author "Explorer Developers"		// 作者信息（版权所有方）
#define		copyright "Copyright 2016"		// 版权信息
#define		software_name "Explorer loader"		// 软件名称
#define		version "1.0"				// 主版本
#define		build "Build 1"				// 版本号
#define		info "A loader for Explorer kernel and Ghost Bird Operating System."		// 软件信息

// 可视化界面信息
#define		VI_width	VI_output_line * FONTWIDTH	// 可视化界面宽度
#define		VI_page_height	VI_output_column * FONTHEIGHT	// 可视化界面高度（不含标题区域和提示区域）
#define		title_height		FONTHEIGHT	// 标题栏高度
#define		title_back_color	0x444444	// 标题栏背景颜色
#define		title_font_color	0x00ccff	// 标题栏字符串颜色
#define		select_back_color	0xcccccc	// 选择区域背景颜色
#define		select_font_color	0x000000	// 选择区域字符串颜色
#define		not_select_back_color	0x222222	// 非选择区域背景颜色
#define		not_select_font_color	0xeeeeee	// 非选择区域字符串颜色
#define		output_back_color	0x111111	// 输出区域背景颜色
#define		D_output_font_color	0xffffff	// 输出区域字符串颜色（默认）
#define		notice_height		FONTHEIGHT	// 提示区域高度
#define		notice_back_color	0x669999	// 提示区域背景颜色
#define		notice_font_color	0x000000	// 提示区域字符串颜色
#define		introduce_font_color	0xcccccc
#define		VI_output_line					120
#define		VI_output_column				42
#define		VI_output_page_number				1
#define		VI_introduce_line				70
#define		VI_introduce_column				7
#define		VI_introduce_width				VI_introduce_line * FONTWIDTH
#define		VI_introduce_height				VI_introduce_column * FONTHEIGHT
#define		VI_select_font_line				VI_introduce_line
#define		VI_select_number				16
#define		VI_select_width					VI_select_font_line * FONTWIDTH
#define		VI_select_height				VI_select_number * FONTHEIGHT

// 1MB开始到4MB以内的内存静态分配
#define		mem_start	0x100000
#define		mem_size	0x300000

// 引导配置文件的相关属性
#define		CONFIG_FILENAME "/CONFIG.LDR"		// 引导配置文件的文件名及目录
#define		CONFIG_MAX 4096				// 引导配置文件的最大大小

// 内核属性
#define		KERNEL_NAME "KERNEL.BIN"		// 内核名字
#define		KERNEL_ADDR (void *)0x400000		// 内核地址

#endif

