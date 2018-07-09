// Explorer Boot
// /boot/bootc.c
// C file of the Explorer Boot

#include "lib/stdlib.h"
#include "../configure.h"
#include "../problem.h"
#include "head.h"
#include "MMU.h"
#include "lib/mem.h"
#include "lib/graphics.h"
#include "storage.h"
#include "fs.h"
#include "VI.h"
#include "script.h"

// A pointer that push argument, eip and jump to the start of kernel
void (*kernel_start)(const struct boot_info *boot_info) = KERNEL_ADDR;

/**重置函数*/
void reset(void)
{
	io_out8(0x64,0xfe);
}

/**停机函数*/
void halt(void)
{
	io_hlt();
}

/**持续停机函数*/
void stillhalt(void)
{
	loop:
		io_hlt();
	goto loop;
}

extern int_0x20, int_0x21, int_0x22, int_0x23;
extern int_0x24, int_0x25, int_0x26, int_0x27;
extern int_0x28, int_0x29, int_0x2A, int_0x2B;
extern int_0x2C, int_0x2D, int_0x2E, int_0x2F;

// IDT地址及大小
void *IDT_base;
#define		IDT_size	8 * 256

/**清空中断描述符表函数*/
void clean_IDT(void)
{
	/**进行判断*/
	if (IDT_base == NULL) error (ERR_NO_MEM_FOR_ID, "IDT_base = NULL when clean the IDT");
	/**清空中断描述符表*/
	memset(IDT_base, 0, IDT_size);
}

/**中断初始化函数*/
void init_interrupt(void)
{
	extern void write_IDTR(void *base, unsigned short size);
	/**若分配内存不成功则输出错误信息*/
	IDT_base = bmalloc(IDT_size);
	if (IDT_base == NULL) error (ERR_NO_MEM_FOR_ID, "No memory for Interrupt Description Table.");
	/**初始化中断描述符表*/
	write_IDTR(IDT_base, IDT_size - 1);
	
	clean_IDT();
	/**创建中断描述符*/
	create_ID(0x20 , 0x08, &int_0x20 , interrupt_gate + IDT_32 + IDT_DPL_0 + IDT_P);
	create_ID(0x21 , 0x08, &int_0x21 , interrupt_gate + IDT_32 + IDT_DPL_0 + IDT_P);
	create_ID(0x22 , 0x08, &int_0x22 , interrupt_gate + IDT_32 + IDT_DPL_0 + IDT_P);
	create_ID(0x23 , 0x08, &int_0x23 , interrupt_gate + IDT_32 + IDT_DPL_0 + IDT_P);
	create_ID(0x24 , 0x08, &int_0x24 , interrupt_gate + IDT_32 + IDT_DPL_0 + IDT_P);
	create_ID(0x25 , 0x08, &int_0x25 , interrupt_gate + IDT_32 + IDT_DPL_0 + IDT_P);
	create_ID(0x26 , 0x08, &int_0x26 , interrupt_gate + IDT_32 + IDT_DPL_0 + IDT_P);
	create_ID(0x27 , 0x08, &int_0x27 , interrupt_gate + IDT_32 + IDT_DPL_0 + IDT_P);
	create_ID(0x28 , 0x08, &int_0x28 , interrupt_gate + IDT_32 + IDT_DPL_0 + IDT_P);
	create_ID(0x29 , 0x08, &int_0x29 , interrupt_gate + IDT_32 + IDT_DPL_0 + IDT_P);
	create_ID(0x2A , 0x08, &int_0x2A , interrupt_gate + IDT_32 + IDT_DPL_0 + IDT_P);
	create_ID(0x2B , 0x08, &int_0x2B , interrupt_gate + IDT_32 + IDT_DPL_0 + IDT_P);
	create_ID(0x2C , 0x08, &int_0x2C , interrupt_gate + IDT_32 + IDT_DPL_0 + IDT_P);
	create_ID(0x2D , 0x08, &int_0x2D , interrupt_gate + IDT_32 + IDT_DPL_0 + IDT_P);
	create_ID(0x2E , 0x08, &int_0x2E , interrupt_gate + IDT_32 + IDT_DPL_0 + IDT_P);
	create_ID(0x2F , 0x08, &int_0x2F , interrupt_gate + IDT_32 + IDT_DPL_0 + IDT_P);
}

/**允许中断*/
void allow_interrupt(void)
{
	io_sti();
}

/**不允许中断*/
void disallow_interrupt(void)
{
	io_cli();
}

#define PORT_8042_DATA		0x60
#define PORT_8042_COMMAND	0x64

/**some port of Intel 8042 chip*/
#define PORT_DATA				0x0060
#define PORT_COMMAND			0x0064
#define port_keysta				0x0064
#define keysta_send_notready	0x02
#define keycmd_write_mode		0x60
#define kbc_mode				0x47
	
/**wait for Intel 8042*/
void wait_8042(void)
{
	for(;;)
	{
		if ((io_in8(port_keysta) & keysta_send_notready) == 0) break;
	}
	return;
}

/**write to Intel 8042*/
void write_8042(char b)
{
	wait_8042();
	io_out8(PORT_8042_DATA, b);
}

/**send command for keyboard*/
void cmd_keyboard(char command)
{
	write_8042(command);
	wait_8042();
}

/**
 * made by Lab Explorer Developers<1@GhostBirdOS.org>
 * 开放8259特定中断的函数
 * 入口：IRQ号
 * 出口：(-1):错误 0:正确
 */
int open_PIC(unsigned char IRQ)
{
	if (IRQ > 15)
	{
		return (-1);
	}else if (IRQ <= 7)/*主8259*/
	{
		io_out8(PIC0_IMR, (io_in8(PIC0_IMR) & (~(1 << IRQ))));
	}else{/*从8259*/
		io_out8(PIC1_IMR, (io_in8(PIC1_IMR) & (~(1 << IRQ - 8))));
	}	
	return 0;
}

/**初始化键盘*/
void init_keyboard(void)
{
	/**初始化中断系统*/
	
	/**初始化8259A芯片*/
	/**首先将主、从8259A的中断关闭，防止出现错误*/
	io_out8(PIC0_IMR, 0xff);				/*禁止主8259的所有中断*/
	io_out8(PIC1_IMR, 0xff);				/*禁止从8259的所有中断*/

	io_out8(PIC0_ICW1, 0x11);				/*边沿触发模式*/
	io_out8(PIC0_ICW2, PIC0_intr_offset);	/*PIC 0~7 由PIC0_intr_offset ~ PIC0_intr_offset+8负责*/
	io_out8(PIC0_ICW3, 1 << 2);				/*从PIC跟主PIC的第2号中断线相连*/
	io_out8(PIC0_ICW4, 0x01);				/*无缓冲模式*/
	
	io_out8(PIC1_ICW1, 0x11);				/*边沿触发*/
	io_out8(PIC1_ICW2, PIC1_intr_offset);	/*PIC 8~15 由int 0x28~0x2f负责*/
	io_out8(PIC1_ICW3, 2);					/*从8259跟PIC 2 相连*/
	io_out8(PIC1_ICW4, 0x01);				/*无缓冲模式*/

	io_out8(PIC0_IMR, 0xfb);				/*禁止所有中断*/
	io_out8(PIC1_IMR, 0xff);				/*禁止所有中断*/
	
	/**初始化键盘*/
	cmd_keyboard(0xED);	/*设置LED灯命令*/
	cmd_keyboard(0x00);	/*设置LED状态全部关闭*/
	cmd_keyboard(0xF4);	/*清空键盘缓冲*/
	
	open_PIC(1);
	
	/**返回*/
	return;
}

/**键盘中断处理函数*/
unsigned long r = 0;
unsigned int color = 0x333333;
void keyboard_handle(void)
{
	unsigned char key_val;
	key_val = io_in8(keycmd_write_mode);
	
	/**
	 * 键盘上键是0x48，下键是0x50，
	 * F1键是0x3B，F2是0x3C
	 * Esc键是0x1
	 */
	
	/**向上键触发*/
	if (key_val == 0x48) select_up();
	
	/**向下键触发*/
	if (key_val == 0x50) select_down();
	
	/**F1键触发*/
	if (key_val == 0x3B) VI_active(VI_page_output);
	
	/**F2键触发*/
	if (key_val == 0x3C) VI_active(VI_page_select);

	/**Enter*/
	if (key_val == 0x1C) select_press();
	
	/**Esc键触发*/
	if (key_val == 0x1) reset();
	
	/**EOI*/
	io_out8(0x20, 0x20);
	io_out8(0xA0, 0x20);
}

void i8259A_auto_return(void)
{
	printk("Unexpected interrupt from Intel 8259A happened.");
	/**EOI*/
	io_out8(0x20, 0x20);
	io_out8(0xA0, 0x20);
}

/**其他中断的自动返回*/
void int_auto_return(void)
{
	printk("Unexpected interrupt happened.");
}

int callback(int n, int type)
{
	if (type == VI_DO_OVER)
	{
		switch (n)
		{
			case 0:
				printi("A muti-task, Graphical Operating System with window support.");
				break;
			case 1:
				printi("A mysterious Operating System.");
				break;
			case 2:
				printi("Another mysterious Operating System.");
				break;
		}
	}else{
		switch (n)
		{
			case 0:
				printk("Select Ghost Bird OS 0.02(Explorer kernel).\n");
				break;
			case 1:
				printk("Select Dragon 0.40 Beta.\n");
				break;
			case 2:
				printk("Select DolphinOS.\n");
				break;
		}
		VI_active(VI_page_output);
	}
}

/**configure文件储存内存指针*/
static char *config_buf;
/**保留扇区引导程序的主函数*/
void BOOT_main(const struct boot_info *boot_info)
{
	/**判断是否成功获取启动信息*/
	if (boot_info->flag[0] != 'E' |
		boot_info->flag[1] != 'B' |
		boot_info->flag[2] != 'I' )
	{
		reset();		/**系统重置*/
	}
	
	/**判断启动信息的长度是否符合要求*/
	if (boot_info->size != sizeof(struct boot_info))
	{
		reset();		/**系统重置*/
	}
	/**初始化图形模式*/
	init_graphics
		(boot_info->ModeInfoBlock.XResolution , boot_info->ModeInfoBlock.YResolution,
		 boot_info->ModeInfoBlock.BitsPerPixel, boot_info->ModeInfoBlock.PhysBasePtr);
	
	/**初始化可视化界面*/
	init_VI();
	
	/**初始化中断描述符表*/
	init_interrupt();
	
	/**初始化键盘*/
	init_keyboard();
	
	/**允许中断*/
	allow_interrupt();	
	
	/**初始化储存器管理*/
	init_storage();

	/**检测活动分区的数量*/
	printk("Active storage partition:%d\n", storage_active_partition());
	
	/**初始化文件系统*/
	init_FS();
	
	/**分配放置configure的内存*/
	struct file_info loaderconfig_file_info;
	loaderconfig_file_info = read_file_info(SD_IDE_00, 0, CONFIG_FILENAME);
	if ((loaderconfig_file_info.size + 1) > CONFIG_MAX)
		error(ERR_CONFIG_OVERSIZE, CONFIG_FILENAME " oversized.");

	config_buf = bmalloc(CONFIG_MAX);
	if (config_buf == NULL) error(ERR_NO_MEM_FOR_CONFIG, "No memory for loader's configure file.");
	
	read_file(SD_IDE_00, 1, CONFIG_FILENAME, config_buf, 0);
	
	//config_buf[loaderconfig_file_info.size] = 0;
	
	//printk("%.*s\n", 50, config_buf);
	
	//printk(strnstr(config_buf, "Hu", 50));
	struct script_node config_node, list_node, item_node,
	name_node, description_node, location_node, address_node;
	
	config_node = script_init(config_buf, loaderconfig_file_info.size);
	
	list_node = script_child(config_node, "list");
	
	item_node = script_child(list_node, "item");
	name_node = script_child(item_node, "name");
	description_node = script_child(item_node, "description");
	location_node = script_child(item_node, "location");
	address_node = script_child(item_node, "address");
	


	// Example
	printak("<0xaaaaff>Hello, This is Explorer loader!\n</>");
	select_register(0, callback, "Ghost Bird OS 0.02(Explorer kernel)");
	select_register(1, callback, "Dragon 0.40 Beta");
	select_register(2, callback, "DolphinOS");
	
	VI_active(VI_page_select);
	
	
	printk("CONFIG.LDR size = %dBytes.\n", loaderconfig_file_info.size);
	io_hlt();
	io_hlt();
	fin:goto fin;
	/**加载内核*/
	read_file(0, 0, KERNEL_NAME, KERNEL_ADDR, 1);
	
	printk("Kernel address:%#X", kernel_start);
	
	/**运行内核*/
	kernel_start(boot_info);
}

