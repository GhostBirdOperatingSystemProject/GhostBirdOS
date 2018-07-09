/**
 * Copyright 2013-2016 by 2013-2015 by Explorer kernel Developers. All rights reserved.
 * made by Hu Wenjie<1@hwj.me>
 * Explorer loader Storage Management
 * Explorer loader/boot/storage.c
 * 2014.7.18 5:48 PM
 */

#define STORAGE_C_
#include "../configure.h"
#include "../problem.h"
#include "lib/stdlib.h"
#include "lib/stdbool.h"
#include "MMU.h"
#include "storage.h"
#include "VI.h"
#include "head.h"
#include "lib/mem.h"
#include "lib/color.h"



/**储存器管理定义部分*/
/**Storage Management Definition*/



/**扇区缓存指针*/
void *sector_buffer;

#pragma pack(push)					// 保存当前对齐信息
#pragma pack(1)						// 设定结构体以一个字节为单位对齐

/**储存器主引导记录*/
struct MBR{
	char code[446];
	struct Storage_Partition SP[4];
	unsigned short int flag;
}*MBR;

#pragma pack(pop)					// 恢复原来的对齐单位

/**储存器描述符表*/
struct Storage_Description
{
	bool exist;
	struct Storage_Partition SP[4];
	char *Description;
}*SD;



/**IDE控制器定义部分*/
/**IDE Support Definition*/



// 命令寄存器组
// 读（主机从主通道读数据）
#define IDE_PRIMARY_READ_DATA 0x1f0				// 数据寄存器
#define IDE_PRIMARY_READ_ERROR 0x1f1			// 错误寄存器（只读寄存器）
#define IDE_PRIMARY_READ_SECCOUNT 0x1f2			// 扇区计数寄存器
#define IDE_PRIMARY_READ_LBA_0_7 0x1f3			// LBA块地址0~7
#define IDE_PRIMARY_READ_LBA_8_15 0x1f4			// LBA块地址8~15
#define IDE_PRIMARY_READ_LBA_16_23 0x1f5		// LBA块地址16~23
#define IDE_PRIMARY_READ_LBA_D_24_27 0x1f6		// 驱动器/LBA块地址24~27
#define IDE_PRIMARY_READ_STATUS 0x1f7			// 状态寄存器

// 写（主机数据写入主通道）
#define IDE_PRIMARY_WRITE_DATA 0x1f0			// 数据寄存器
#define IDE_PRIMARY_WRITE_FEATURES 0x1f1		// 特征寄存器
#define IDE_PRIMARY_WRITE_SECCOUNT 0x1f2		// 扇区计数寄存器
#define IDE_PRIMARY_WRITE_LBA_0_7 0x1f3			// LBA块地址0~7
#define IDE_PRIMARY_WRITE_LBA_8_15 0x1f4		// LBA块地址8~15
#define IDE_PRIMARY_WRITE_LBA_16_23 0x1f5		// LBA块地址16~23
#define IDE_PRIMARY_WRITE_LBA_D_24_27 0x1f6		// 驱动器/LBA块地址24~27
#define IDE_PRIMARY_WRITE_CMD 0x1f7				// 命令寄存器

// 读（主机从第二通道读数据）
#define IDE_SECONDARY_READ_DATA 0x170			// 数据寄存器
#define IDE_SECONDARY_READ_ERROR 0x171			// 错误寄存器（只读寄存器）
#define IDE_SECONDARY_READ_SECCOUNT 0x172		// 扇区计数寄存器
#define IDE_SECONDARY_READ_LBA_0_7 0x173		// LBA块地址0~7
#define IDE_SECONDARY_READ_LBA_8_15 0x174		// LBA块地址8~15
#define IDE_SECONDARY_READ_LBA_16_23 0x175		// LBA块地址16~23
#define IDE_SECONDARY_READ_LBA_D_24_27 0x176	// 驱动器/LBA块地址24~27
#define IDE_SECONDARY_READ_STATUS 0x177			// 状态寄存器

// 写（主机数据写入第二通道）
#define IDE_SECONDARY_WRITE_DATA 0x170			// 数据寄存器
#define IDE_SECONDARY_WRITE_FEATURES 0x171		// 特征寄存器
#define IDE_SECONDARY_WRITE_SECCOUNT 0x172		// 扇区计数寄存器
#define IDE_SECONDARY_WRITE_LBA_0_7 0x173		// LBA块地址0~7
#define IDE_SECONDARY_WRITE_LBA_8_15 0x174		// LBA块地址8~15
#define IDE_SECONDARY_WRITE_LBA_16_23 0x175		// LBA块地址16~23
#define IDE_SECONDARY_WRITE_LBA_D_24_27 0x176	// 驱动器/LBA块地址24~27
#define IDE_SECONDARY_WRITE_CMD 0x177			// 命令寄存器

/**该数组元素分别对应上述的0x1f0~0x1f7的寄存器*/
unsigned char IDE_register[8];

// 诊断寄存器组
// 读
#define IDE_PRIMARY_READ_EXCHGSTAUS 0x3f6		// 交换状态寄存器（只读寄存器）
#define IDE_PRIMARY_READ_DEVADDR 0x3f7			// 驱动器地址寄存器

// 写
#define IDE_PRIMARY_WRITE_RESET 0x3f6			// 设备控制寄存器（复位）

// IDE错误寄存器
#define IDE_ERR_AMNF 1					// 没找到所要访问的扇区的数据区
#define IDE_ERR_TK0NF 2					// 在执行恢复RECALIBRATE命令时0磁道没有发现
#define IDE_ERR_ABRT 4					// 对硬盘发非法指令或因硬盘驱动器故障而造成命令执行的中断
#define IDE_ERR_MAC 8					// 该信号用来向主机发出通知表示介质的改变
#define IDE_ERR_IDNF 16					// 没有找到访问的扇区或CRC发生错误
#define IDE_ERR_MC 32					// 这是发送给主机一个信号以通知主机使用新的传输介质
#define IDE_ERR_UNC 64					// 在读扇区命令时出现不能校正的ECC错误因此此次数据传输无效
#define IDE_ERR_BBK 128					// 在访问扇区的ID数据场发现坏的数据块时会置1

// IDE状态寄存器

#define IDE_STATUS_ERR 1				// 错误(ERROR) 该位为1表示在结束前次的命令执行时发生了无法恢复的错误在错误寄存器中保存了更多的错误信息
#define IDE_STATUS_IDX 2				// 反映从驱动器读入的索引信号
#define IDE_STATUS_CORR 4				// 该位为1时表示已按ECC算法校正硬盘的读数据
#define IDE_STATUS_DRQ 8				// 为1表示请求主机进行数据传输(读或写)
#define IDE_STATUS_DSC 16				// 为1表示磁头完成寻道操作已停留在该道上
#define IDE_STATUS_DF 32				// 为1时表示驱动器发生写故障
#define IDE_STATUS_DRDY 64				// 为1时表示驱动器准备好可以接受命令
#define IDE_STATUS_BSY 128				// 为1时表示驱动器忙(BSY) 正在执行命令在发送命令前先判断该位

// IDE通道
#define IDE_CHANNEL_PRIMARY		0x0
#define IDE_CHANNEL_SECONDARY	0x1

// IDE驱动器
#define IDE_DRIVE_MASTER	0xA0
#define IDE_DRIVE_SLAVE		0xB0

// IDE命令
#define IDE_CMD_READ		0x20		// 读取扇区命令
#define IDE_CMD_WRITE		0x30		// 写入扇区命令
#define IDE_CMD_CHECK		0x90		// 磁盘诊断命令
#define IDE_CMD_IDENTIFY	0xEC		// 辨识命令


/**IDE控制器支持部分*/
/**IDE Controller Support*/



/**从IDE控制器读取寄存器*/
void IDE_reg_read(char PS)
{
	unsigned long port_offset = 0;
	
	if (PS == IDE_DRIVE_SLAVE)
	{
		port_offset = IDE_SECONDARY_READ_DATA - IDE_PRIMARY_READ_DATA;
	}
	
	/**读数据*/
	IDE_register[1] = io_in8(IDE_PRIMARY_READ_ERROR + port_offset);
	IDE_register[2] = io_in8(IDE_PRIMARY_READ_SECCOUNT + port_offset);
	IDE_register[3] = io_in8(IDE_PRIMARY_READ_LBA_0_7 + port_offset);
	IDE_register[4] = io_in8(IDE_PRIMARY_READ_LBA_8_15 + port_offset);
	IDE_register[5] = io_in8(IDE_PRIMARY_READ_LBA_16_23 + port_offset);
	IDE_register[6] = io_in8(IDE_PRIMARY_READ_LBA_D_24_27 + port_offset);
	IDE_register[7] = io_in8(IDE_PRIMARY_READ_STATUS + port_offset);
}

/**向IDE控制器发送命令*/
void IDE_reg_cmd(char PS)
{
	unsigned long port_offset = 0;
	
	if (PS == IDE_DRIVE_SLAVE)
	{
		port_offset = IDE_SECONDARY_READ_DATA - IDE_PRIMARY_READ_DATA;
	}
	
	/**写数据*/
	//io_out8(IDE_PRIMARY_WRITE_FEATURES + port_offset		, IDE_register[1]);
	io_out16(IDE_PRIMARY_WRITE_SECCOUNT + port_offset		, IDE_register[2]);
	io_out8(IDE_PRIMARY_WRITE_LBA_0_7 + port_offset			, IDE_register[3]);
	io_out8(IDE_PRIMARY_WRITE_LBA_8_15 + port_offset		, IDE_register[4]);
	io_out8(IDE_PRIMARY_WRITE_LBA_16_23 + port_offset		, IDE_register[5]);
	io_out8(IDE_PRIMARY_WRITE_LBA_D_24_27 + port_offset		, IDE_register[6]);
	io_out8(IDE_PRIMARY_WRITE_CMD + port_offset				, IDE_register[7]);
}

/**IDE磁盘复位，尚不清楚是否需要区分不同通道的主从IDE磁盘*/
void IDE_reset(void)
{
	/**
	 * 设备控制寄存器，
	 * bit 3 = 1，bit 2 = SRST，
	 * bit 1 = IEN，bit 0 = 0
	 * 其中SRST置1则可使驱动器处于复位状态，
	 * IEN为是否允许中断
	 */
	io_out8(IDE_PRIMARY_WRITE_RESET, 0xc);
	io_out8(IDE_PRIMARY_WRITE_RESET, 0x8);
}

/**IDE磁盘等待*/
void IDE_wait(void)
{
	/**等待次数计时*/
	//unsigned long n;
	for (; (io_in8(IDE_PRIMARY_READ_STATUS) & (IDE_STATUS_DRQ | IDE_STATUS_BSY)) != IDE_STATUS_DRQ;);				/**循环等待*/
}

/**确保BSY为0*/
void IDE_nobusy(void)
{
	for(;;)
	{
		if ((io_in8(IDE_PRIMARY_READ_STATUS) & (IDE_STATUS_DRQ | IDE_STATUS_BSY)) == IDE_STATUS_DRQ)
		{
			return;
		}
	}
}



/**IDE控制器写扇区*/
int IDE_write(unsigned long storage_number, const void *src, unsigned long LBA_addr, unsigned long n)
{
	
}

/**IDE控制器读扇区*/
int IDE_read(unsigned long storage_number, void *dest, unsigned long LBA_addr, unsigned long n)
{
	unsigned short int *buffer = (unsigned short int *)dest;
	/**进行IDE通道判断*/
	if ((storage_number == SD_IDE_00) | (storage_number == SD_IDE_01))
	{
		/**若是第一通道的储存器*/
		
		/**循环读取扇区*/
		for (; n != 0; n --, buffer += (512 / sizeof(short int)), LBA_addr ++)
		{
			IDE_register[2] = 1;
			IDE_register[3] = (LBA_addr & 0xff);
			IDE_register[4] = ((LBA_addr >> 8) & 0xff);
			IDE_register[5] = ((LBA_addr >> 16) & 0xff);
			if (storage_number == SD_IDE_00)
			{
				/**若是主盘*/
				IDE_register[6] = (((LBA_addr >> 24) & 0xff) + 0b11100000);
			}else{
				IDE_register[6] = (((LBA_addr >> 24) & 0xff) + 0b11110000);
			}
			IDE_register[7] = IDE_CMD_READ;
			
			/**发出命令*/
			IDE_reg_cmd(IDE_CHANNEL_PRIMARY);
			
			/**等到IDE第一通道不忙时*/
			IDE_wait();
			/**读取数据*/
			unsigned long r;
			for (r = 0; r < (512 / sizeof(short int)); r ++)
			{
				buffer[r] = io_in16(IDE_PRIMARY_READ_DATA);
				
			}
		}
	}else if ((storage_number == SD_IDE_10) | (storage_number == SD_IDE_11))
	{
		/**如果是第二通道的储存器*/
		warning(WARN_STORAGE_NOT_SUPPORT, "The IDE Secondary is not yet supported.");
		return STORAGE_RETVAL_ERR_NOT_SUPPORT;
	}else{
		/**若非IDE控制器的储存器*/
		return STORAGE_RETVAL_ERR_NOT_SUPPORT;
	}
	return STORAGE_RETVAL_NORMAL;
}

/**IDE控制器初始化*/
void init_IDE(void)
{
	char *IDE_Description = "IDE Controller";
	unsigned long n;
	
	/**增加对设备的描述*/
	SD[SD_IDE_00].Description = IDE_Description;
	SD[SD_IDE_01].Description = IDE_Description;
	SD[SD_IDE_10].Description = IDE_Description;
	SD[SD_IDE_11].Description = IDE_Description;
	
	/**检测主通道主磁盘状态*/
	io_out8(IDE_PRIMARY_WRITE_LBA_D_24_27, IDE_DRIVE_MASTER);
	io_out8(IDE_PRIMARY_WRITE_FEATURES, 0);
	io_out8(IDE_PRIMARY_WRITE_SECCOUNT, 0);
	io_out8(IDE_PRIMARY_WRITE_LBA_0_7, 0);
	io_out8(IDE_PRIMARY_WRITE_LBA_8_15, 0);
	io_out8(IDE_PRIMARY_WRITE_LBA_16_23, 0);
	io_out8(IDE_PRIMARY_WRITE_CMD, IDE_CMD_IDENTIFY);
	if (io_in8(IDE_PRIMARY_READ_STATUS) != 0)
	{
		// 存在磁盘
				
		/**在储存器描述符表部分注册*/
		SD[SD_IDE_00].exist = true;
		
		// The first read is a sector of 512 Bytes unknown information
		IDE_read(SD_IDE_00, MBR, 0, 1);

		IDE_read(SD_IDE_00, MBR, 0, 1);
		
		SD[SD_IDE_00].SP[0] = MBR->SP[0];
		SD[SD_IDE_00].SP[1] = MBR->SP[1];
		SD[SD_IDE_00].SP[2] = MBR->SP[2];
		SD[SD_IDE_00].SP[3] = MBR->SP[3];
		
		printak("<%#X>IDE Controller:</>Found Primary Master.\n", DeepPink);
	}
	
	/**检测主通道从磁盘状态*/
	io_out8(IDE_PRIMARY_WRITE_LBA_D_24_27, IDE_DRIVE_SLAVE);
	io_out8(IDE_PRIMARY_WRITE_FEATURES, 0);
	io_out8(IDE_PRIMARY_WRITE_SECCOUNT, 0);
	io_out8(IDE_PRIMARY_WRITE_LBA_0_7, 0);
	io_out8(IDE_PRIMARY_WRITE_LBA_8_15, 0);
	io_out8(IDE_PRIMARY_WRITE_LBA_16_23, 0);
	io_out8(IDE_PRIMARY_WRITE_CMD, IDE_CMD_IDENTIFY);
	if (io_in8(IDE_PRIMARY_READ_STATUS) != 0)
	{
		// 存在磁盘
		
		
		/**在储存器描述符表部分注册*/
		SD[SD_IDE_01].exist = true;
		
		IDE_read(SD_IDE_01, MBR, 0, 1);
		
		SD[SD_IDE_01].SP[0] = MBR->SP[0];
		SD[SD_IDE_01].SP[1] = MBR->SP[1];
		SD[SD_IDE_01].SP[2] = MBR->SP[2];
		SD[SD_IDE_01].SP[3] = MBR->SP[3];
		
		printak("<%#X>IDE Controller:</>Found Primary Slave.\n", DeepPink);
	}

	/**输出信息*/
	printk("IDE Controller finished.\n");
}

/**储存器管理部分*/
/**Storage Management*/



/**读储存器扇区函数*/
int storage_read(unsigned long storage_number, void *dest, unsigned long LBA_addr, unsigned long n)
{
	/**选择相应的储存器读取函数*/
	switch(storage_number)
	{
		case SD_IDE_00:return IDE_read(storage_number, dest, LBA_addr, n);
		case SD_IDE_01:return IDE_read(storage_number, dest, LBA_addr, n);
		case SD_IDE_10:return IDE_read(storage_number, dest, LBA_addr, n);
		case SD_IDE_11:return IDE_read(storage_number, dest, LBA_addr, n);
		default:return STORAGE_RETVAL_ERR_NOT_SUPPORT;
	}
}

/**返回活动分区的数量*/
unsigned long storage_active_partition(void)
{
	unsigned long r, n;
	for (r = 0, n = 0; n < SD_NUM; n ++)
	{
		if (SD[n].exist == true)
		{
			// printk("A point");
			if (SD[n].SP[0].active == 0x80) r ++;
			if (SD[n].SP[1].active == 0x80) r ++;
			if (SD[n].SP[2].active == 0x80) r ++;
			if (SD[n].SP[3].active == 0x80) r ++;
		}
	}
	
	return r;
}

/**初始化储存器管理函数*/
unsigned int init_storage(void)
{
	unsigned long n, r;
	
	/**为储存器描述符表分配内存*/
	SD = bmalloc((SD_NUM) * sizeof(struct Storage_Description));
	if (SD == NULL) error(ERR_NO_MEM_FOR_SD, "No memory for Storage Description.");
	
	/**将储存器描述符表全部清0*/
	memset(SD, 0, (SD_NUM) * sizeof(struct Storage_Description));
	
	/**为储存器的扇区缓存配备的内存*/
	sector_buffer = bmalloc(sizeof(struct MBR));
	if (sector_buffer == NULL) error(ERR_NO_MEM_FOR_SCTBUF, "No memory for Storage MBR.");
	
	/**MBR指针指向扇区缓存*/
	MBR = (struct MBR *)sector_buffer;
	
	/**其它储存器控制器的初始化函数在这里调用*/
	
	/**初始化IDE控制器*/
	init_IDE();
	
	/**打印所有储存器信息*/
	printak("<%#X>Storage List:</>\n", Snow1);
	for (n = 0; n < SD_NUM; n ++)
	{
		if (SD[n].exist == true)
		{
			/**打印储存器描述信息*/
			printak("    <%#X>(Storage Descriptor %d)%s</>\n", Snow2, n, SD[n].Description);
			
			/**打印储存器的分区信息*/
			for (r = 0; r < 4; r ++)
			{
				printak("        <%#X>Partition%d:</>", Snow3, r);
				/**划分活动分区和非活动分区*/
				if (SD[n].SP[r].active == 0x80)
				{
					printak("<%#X>Active.</>", Orange1);
				}else{
					printak("<%#X>Inactive.</>", Snow3);
				}
				/**分区位置信息*/
				printak("<%#X>File System:%#X,Start(LBA):%d,Length:%dMiB.\n</>", Snow3, SD[n].SP[r].fs, SD[n].SP[r].start_LBA, SD[n].SP[r].size_sector * 512 / 1048576);
			}
		}else{
			/**打印储存器描述信息*/
			printak("    <%#X>(Storage Descriptor %d)%s: Unmounted</>\n", Snow4, n, SD[n].Description);
		}
		
	}
}
