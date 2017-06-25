// Explorer Boot
// /boot/bootc.c
// C file of the Explorer Boot

#include "lib/stdlib.h"
#include "../configure.h"
#include "../problem.h"
#include "head.h"
#include "MMU.h"
#include "lib/mem.h"
#include "storage.h"
#include "fs.h"
#include "VI.h"

// A pointer that push argument, eip and jump to the start of kernel
void (*kernel_start)(const struct boot_info *boot_info) = KERNEL_ADDR;

// ��Ƶģʽ��Ϣ
struct Video_Info Video_Info;

/**��ʾ���غ���*/
void (*putpixel)(unsigned int x, unsigned int y, unsigned int color);

/**��ȡ���غ���*/
unsigned int (*getpixel)(unsigned int x, unsigned int y);

/**24λɫ��ģʽ��ȡ���غ���*/
unsigned int getpixel24(unsigned int x, unsigned int y)
{
	unsigned int i;
	
	/**���жϸ������Ƿ�����Ļ��*/
	if (x < Video_Info.xres & y < Video_Info.yres)
	{
		i = ((y * Video_Info.xres) + x) * 3;
		return (Video_Info.vram[i] + (Video_Info.vram[i+1] << 8) + (Video_Info.vram[i+2] << 16));
	}
}

/**32λɫ��ģʽ��ȡ���غ���*/
unsigned int getpixel32(unsigned int x, unsigned int y)
{
	/**���жϸ������Ƿ�����Ļ��*/
	if (x < Video_Info.xres & y < Video_Info.yres)
	{
		return ((unsigned int *)Video_Info.vram)[(y * Video_Info.xres) + x];
	}
}

/**24λɫ��ģʽ�������غ���*/
void putpixel24(unsigned int x, unsigned int y, unsigned int color)
{
	int i;
	unsigned char *vram = Video_Info.vram;
	
	/**���жϸ������Ƿ�����Ļ��*/
	if ((x < Video_Info.xres) & (y < Video_Info.yres))
	{
		i = ((y * Video_Info.xres) + x)*3;
		vram[i] = color;
		vram[i+1] = color >> 8;
		vram[i+2] = color >> 16;
	}
}

/**32λɫ��ģʽ�������غ���*/
void putpixel32(unsigned int x, unsigned int y, unsigned int color)
{
	/**���жϸ������Ƿ�����Ļ��*/
	if ((x < Video_Info.xres) & (y < Video_Info.yres))
	{
		((unsigned int *)Video_Info.vram)[(y * Video_Info.xres) + x] = color;
	}
}

/*����Ļָ���ط�������*/
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

// ���ֺ���
void draw_font(unsigned long x, unsigned long y, unsigned int color, unsigned char ascii)
{
	unsigned long i, font_offset;/*�ֿ�ƫ����*/
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

/**��Ļ��ָ��λ�����һ���ַ���*/
void outtextxy(unsigned long x, unsigned long y, unsigned int color, unsigned char *string)
{
	unsigned long point;
	for (point = 0; string[point] != 0x00; point ++)
	{
		draw_font(x, y, color, string[point]);
		x += 8;
	}
}

// ���ߺ���
void line(unsigned long x0, unsigned long y0, unsigned long x1, unsigned long y1, unsigned int color)
{
	int dx,dy,n,k,i,f;
	int x,y;
	dx=abs(x1-x0);
	dy=abs(y1-y0);
	n=dx+dy;
	
	/**����x1��x0�Ĺ�ϵ��ѡ����ʵ�k���㷽��*/
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

/**���ú���*/
void reset(void)
{
	io_out8(0x64,0xfe);
}

/**ͣ������*/
void halt(void)
{
	io_hlt();
}

/**����ͣ������*/
void stillhalt(void)
{
	loop:
		io_hlt();
	goto loop;
}

/**���Ժ�����������ʾ�����ڴ�����*/
void debug(void * address, unsigned long size)
{
	unsigned char count;
	unsigned char *data_ptr = address;
	
	/**���size��С���ԣ���ֱ�������Ϣ��ֱ�ӷ���*/
	if (size == 0)
	{
		printk("debug invalid.\n");
		return;
	}
	
	/**���size����16�ֽڶ��룬�Ͱ�16�ֽڶ���*/
	if ((size % 16) != 0)
	{
		size = size & 0xfffffff0;
		size + 16;
	}
	/**���Ʊ�ͷ*/
	printk ("Memory %#x to %#x:\n", address, address + size);
	printk("Offset(h)  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
	/**������*/
	for (; size != 0;)
	{
		printk("%08X   %02X %02X %02X %02X ", (unsigned long)data_ptr & 0xfffffff0, *(data_ptr), *(data_ptr + 1), *(data_ptr + 2), *(data_ptr + 3));
		printk("%02X %02X %02X %02X ", *(data_ptr + 4), *(data_ptr + 5), *(data_ptr + 6), *(data_ptr + 7));
		printk("%02X %02X %02X %02X ", *(data_ptr + 8), *(data_ptr + 9), *(data_ptr + 10), *(data_ptr + 11));
		printk("%02X %02X %02X %02X  ", *(data_ptr + 12), *(data_ptr + 13), *(data_ptr + 14), *(data_ptr + 15));
		
		for (count = 0; count < 16; count++)
		{
			if (*(data_ptr + count) == 0)
			{
				put_char(46);
			}else if (*(data_ptr + count) == 0x0A){
				put_char(46);
			}else{
				put_char(*(data_ptr + count));
			}
		}
		
		printk("\n");
		
		size -= 16;
		data_ptr += 16;
	}
}

extern int_0x20, int_0x21, int_0x22, int_0x23;
extern int_0x24, int_0x25, int_0x26, int_0x27;
extern int_0x28, int_0x29, int_0x2A, int_0x2B;
extern int_0x2C, int_0x2D, int_0x2E, int_0x2F;

// IDT��ַ����С
void *IDT_base;
#define		IDT_size	8 * 256

/**����ж�����������*/
void clean_IDT(void)
{
	/**�����ж�*/
	if (IDT_base == NULL) error (ERR_NO_MEM_FOR_ID, "IDT_base = NULL when clean the IDT");
	/**����ж���������*/
	memset(IDT_base, 0, IDT_size);
}

/**�жϳ�ʼ������*/
void init_interrupt(void)
{
	extern void write_IDTR(void *base, unsigned short size);
	/**�������ڴ治�ɹ������������Ϣ*/
	IDT_base = bmalloc(IDT_size);
	if (IDT_base == NULL) error (ERR_NO_MEM_FOR_ID, "No memory for Interrupt Description Table.");
	/**��ʼ���ж���������*/
	write_IDTR(IDT_base, IDT_size - 1);
	
	clean_IDT();
	/**�����ж�������*/
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

/**�����ж�*/
void allow_interrupt(void)
{
	io_sti();
}

/**�������ж�*/
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
 * ����8259�ض��жϵĺ���
 * ��ڣ�IRQ��
 * ���ڣ�(-1):���� 0:��ȷ
 */
int open_PIC(unsigned char IRQ)
{
	if (IRQ > 15)
	{
		return (-1);
	}else if (IRQ <= 7)/*��8259*/
	{
		io_out8(PIC0_IMR, (io_in8(PIC0_IMR) & (~(1 << IRQ))));
	}else{/*��8259*/
		io_out8(PIC1_IMR, (io_in8(PIC1_IMR) & (~(1 << IRQ - 8))));
	}	
	return 0;
}

/**��ʼ������*/
void init_keyboard(void)
{
	/**��ʼ���ж�ϵͳ*/
	
	/**��ʼ��8259AоƬ*/
	/**���Ƚ�������8259A���жϹرգ���ֹ���ִ���*/
	io_out8(PIC0_IMR, 0xff);				/*��ֹ��8259�������ж�*/
	io_out8(PIC1_IMR, 0xff);				/*��ֹ��8259�������ж�*/

	io_out8(PIC0_ICW1, 0x11);				/*���ش���ģʽ*/
	io_out8(PIC0_ICW2, PIC0_intr_offset);	/*PIC 0~7 ��PIC0_intr_offset ~ PIC0_intr_offset+8����*/
	io_out8(PIC0_ICW3, 1 << 2);				/*��PIC����PIC�ĵ�2���ж�������*/
	io_out8(PIC0_ICW4, 0x01);				/*�޻���ģʽ*/
	
	io_out8(PIC1_ICW1, 0x11);				/*���ش���*/
	io_out8(PIC1_ICW2, PIC1_intr_offset);	/*PIC 8~15 ��int 0x28~0x2f����*/
	io_out8(PIC1_ICW3, 2);					/*��8259��PIC 2 ����*/
	io_out8(PIC1_ICW4, 0x01);				/*�޻���ģʽ*/

	io_out8(PIC0_IMR, 0xfb);				/*��ֹ�����ж�*/
	io_out8(PIC1_IMR, 0xff);				/*��ֹ�����ж�*/
	
	/**��ʼ������*/
	cmd_keyboard(0xED);	/*����LED������*/
	cmd_keyboard(0x00);	/*����LED״̬ȫ���ر�*/
	cmd_keyboard(0xF4);	/*��ռ��̻���*/
	
	open_PIC(1);
	
	/**����*/
	return;
}

/**�����жϴ�����*/
unsigned long r = 0;
unsigned int color = 0x333333;
void keyboard_handle(void)
{
	unsigned char key_val;
	key_val = io_in8(keycmd_write_mode);
	
	/**
	 * �����ϼ���0x48���¼���0x50��
	 * F1����0x3B��F2��0x3C
	 * Esc����0x1
	 */
	
	/**���ϼ�����*/
	if (key_val == 0x48) select_up();
	
	/**���¼�����*/
	if (key_val == 0x50) select_down();
	
	/**F1������*/
	if (key_val == 0x3B) VI_active(VI_page_output);
	
	/**F2������*/
	if (key_val == 0x3C) VI_active(VI_page_select);
	
	/**Esc������*/
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

/**�����жϵ��Զ�����*/
void int_auto_return(void)
{
	printk("Unexpected interrupt happened.");
}

int callback(int n, int type)
{
	if (n == 0)
	{
		
	}else{
		
	}
}

/**configure�ļ������ڴ�ָ��*/
static void *config_buf;
/**�����������������������*/
void BOOT_main(const struct boot_info *boot_info)
{
	/**�ж��Ƿ�ɹ���ȡ������Ϣ*/
	if (boot_info->flag[0] != 'E' |
		boot_info->flag[1] != 'B' |
		boot_info->flag[2] != 'I' )
	{
		reset();		/**ϵͳ����*/
	}
	
	/**�ж�������Ϣ�ĳ����Ƿ����Ҫ��*/
	if (boot_info->size != sizeof(struct boot_info))
	{
		reset();		/**ϵͳ����*/
	}
	/**��ʼ��ͼ��ģʽ*/
	Video_Info.xres = boot_info->ModeInfoBlock.XResolution;
	Video_Info.yres = boot_info->ModeInfoBlock.YResolution;
	Video_Info.bit_per_pixel = boot_info->ModeInfoBlock.BitsPerPixel;
	Video_Info.vram_length = (((Video_Info.xres * Video_Info.yres) * (Video_Info.bit_per_pixel / 8)) & 0xfffff000) + 0x1000;
	Video_Info.vram = (unsigned char*)boot_info->ModeInfoBlock.PhysBasePtr;
	
	/**����BPP�ж�����Ҫ����Ӧ��ʾ���غ���*/
	if (Video_Info.bit_per_pixel == 24)
	{
		putpixel = putpixel24;
		getpixel = getpixel24;
	}else if (Video_Info.bit_per_pixel == 32)
	{
		putpixel = putpixel32;
		getpixel = getpixel32;
	}
	
	/**��ʼ�����ӻ�����*/
	init_VI();
	
	/**��ʼ���ж���������*/
	init_interrupt();
	
	/**��ʼ������*/
	init_keyboard();
	
	/**�����ж�*/
	allow_interrupt();	
	
	/**��ʼ������������*/
	init_storage();
	
	/**�������������*/
	printk("Active storage partition:%d\n", storage_active_partition());
	
	/**��ʼ���ļ�ϵͳ*/
	init_FS();
	
	/**�������configure���ڴ�*/
	config_buf = bmalloc(CONFIG_MAX);
	if (config_buf == NULL) error(ERR_NO_MEM_FOR_CONFIG, "No memory for loader's configure file.");
	
	// Example
	printak("<0xaaaaff>Hello, This is Explorer loader!\n</>");
	select_register(0, callback, "show 'Hello, world!'");
	select_register(1, callback, "show 'Explorer loader'");
	io_hlt();
	
	/**�����ں�*/
	read_file(0, 0, KERNEL_NAME, KERNEL_ADDR, 1);
	
	printk("Kernel address:%#X", kernel_start);
	
	/**�����ں�*/
	kernel_start(boot_info);
}

