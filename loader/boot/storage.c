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



/**�����������岿��*/
/**Storage Management Definition*/



/**��������ָ��*/
void *sector_buffer;

#pragma pack(push)					// ���浱ǰ������Ϣ
#pragma pack(1)						// �趨�ṹ����һ���ֽ�Ϊ��λ����

/**��������������¼*/
struct MBR{
	char code[446];
	struct Storage_Partition SP[4];
	unsigned short int flag;
}*MBR;

#pragma pack(pop)					// �ָ�ԭ���Ķ��뵥λ

/**��������������*/
struct Storage_Description
{
	bool exist;
	struct Storage_Partition SP[4];
	char *Description;
}*SD;



/**IDE���������岿��*/
/**IDE Support Definition*/



// ����Ĵ�����
// ������������ͨ�������ݣ�
#define IDE_PRIMARY_READ_DATA 0x1f0				// ���ݼĴ���
#define IDE_PRIMARY_READ_ERROR 0x1f1			// ����Ĵ�����ֻ���Ĵ�����
#define IDE_PRIMARY_READ_SECCOUNT 0x1f2			// ���������Ĵ���
#define IDE_PRIMARY_READ_LBA_0_7 0x1f3			// LBA���ַ0~7
#define IDE_PRIMARY_READ_LBA_8_15 0x1f4			// LBA���ַ8~15
#define IDE_PRIMARY_READ_LBA_16_23 0x1f5		// LBA���ַ16~23
#define IDE_PRIMARY_READ_LBA_D_24_27 0x1f6		// ������/LBA���ַ24~27
#define IDE_PRIMARY_READ_STATUS 0x1f7			// ״̬�Ĵ���

// д����������д����ͨ����
#define IDE_PRIMARY_WRITE_DATA 0x1f0			// ���ݼĴ���
#define IDE_PRIMARY_WRITE_FEATURES 0x1f1		// �����Ĵ���
#define IDE_PRIMARY_WRITE_SECCOUNT 0x1f2		// ���������Ĵ���
#define IDE_PRIMARY_WRITE_LBA_0_7 0x1f3			// LBA���ַ0~7
#define IDE_PRIMARY_WRITE_LBA_8_15 0x1f4		// LBA���ַ8~15
#define IDE_PRIMARY_WRITE_LBA_16_23 0x1f5		// LBA���ַ16~23
#define IDE_PRIMARY_WRITE_LBA_D_24_27 0x1f6		// ������/LBA���ַ24~27
#define IDE_PRIMARY_WRITE_CMD 0x1f7				// ����Ĵ���

// ���������ӵڶ�ͨ�������ݣ�
#define IDE_SECONDARY_READ_DATA 0x170			// ���ݼĴ���
#define IDE_SECONDARY_READ_ERROR 0x171			// ����Ĵ�����ֻ���Ĵ�����
#define IDE_SECONDARY_READ_SECCOUNT 0x172		// ���������Ĵ���
#define IDE_SECONDARY_READ_LBA_0_7 0x173		// LBA���ַ0~7
#define IDE_SECONDARY_READ_LBA_8_15 0x174		// LBA���ַ8~15
#define IDE_SECONDARY_READ_LBA_16_23 0x175		// LBA���ַ16~23
#define IDE_SECONDARY_READ_LBA_D_24_27 0x176	// ������/LBA���ַ24~27
#define IDE_SECONDARY_READ_STATUS 0x177			// ״̬�Ĵ���

// д����������д��ڶ�ͨ����
#define IDE_SECONDARY_WRITE_DATA 0x170			// ���ݼĴ���
#define IDE_SECONDARY_WRITE_FEATURES 0x171		// �����Ĵ���
#define IDE_SECONDARY_WRITE_SECCOUNT 0x172		// ���������Ĵ���
#define IDE_SECONDARY_WRITE_LBA_0_7 0x173		// LBA���ַ0~7
#define IDE_SECONDARY_WRITE_LBA_8_15 0x174		// LBA���ַ8~15
#define IDE_SECONDARY_WRITE_LBA_16_23 0x175		// LBA���ַ16~23
#define IDE_SECONDARY_WRITE_LBA_D_24_27 0x176	// ������/LBA���ַ24~27
#define IDE_SECONDARY_WRITE_CMD 0x177			// ����Ĵ���

/**������Ԫ�طֱ��Ӧ������0x1f0~0x1f7�ļĴ���*/
unsigned char IDE_register[8];

// ��ϼĴ�����
// ��
#define IDE_PRIMARY_READ_EXCHGSTAUS 0x3f6		// ����״̬�Ĵ�����ֻ���Ĵ�����
#define IDE_PRIMARY_READ_DEVADDR 0x3f7			// ��������ַ�Ĵ���

// д
#define IDE_PRIMARY_WRITE_RESET 0x3f6			// �豸���ƼĴ�������λ��

// IDE����Ĵ���
#define IDE_ERR_AMNF 1					// û�ҵ���Ҫ���ʵ�������������
#define IDE_ERR_TK0NF 2					// ��ִ�лָ�RECALIBRATE����ʱ0�ŵ�û�з���
#define IDE_ERR_ABRT 4					// ��Ӳ�̷��Ƿ�ָ�����Ӳ�����������϶��������ִ�е��ж�
#define IDE_ERR_MAC 8					// ���ź���������������֪ͨ��ʾ���ʵĸı�
#define IDE_ERR_IDNF 16					// û���ҵ����ʵ�������CRC��������
#define IDE_ERR_MC 32					// ���Ƿ��͸�����һ���ź���֪ͨ����ʹ���µĴ������
#define IDE_ERR_UNC 64					// �ڶ���������ʱ���ֲ���У����ECC������˴˴����ݴ�����Ч
#define IDE_ERR_BBK 128					// �ڷ���������ID���ݳ����ֻ������ݿ�ʱ����1

// IDE״̬�Ĵ���

#define IDE_STATUS_ERR 1				// ����(ERROR) ��λΪ1��ʾ�ڽ���ǰ�ε�����ִ��ʱ�������޷��ָ��Ĵ����ڴ���Ĵ����б����˸���Ĵ�����Ϣ
#define IDE_STATUS_IDX 2				// ��ӳ������������������ź�
#define IDE_STATUS_CORR 4				// ��λΪ1ʱ��ʾ�Ѱ�ECC�㷨У��Ӳ�̵Ķ�����
#define IDE_STATUS_DRQ 8				// Ϊ1��ʾ���������������ݴ���(����д)
#define IDE_STATUS_DSC 16				// Ϊ1��ʾ��ͷ���Ѱ��������ͣ���ڸõ���
#define IDE_STATUS_DF 32				// Ϊ1ʱ��ʾ����������д����
#define IDE_STATUS_DRDY 64				// Ϊ1ʱ��ʾ������׼���ÿ��Խ�������
#define IDE_STATUS_BSY 128				// Ϊ1ʱ��ʾ������æ(BSY) ����ִ�������ڷ�������ǰ���жϸ�λ

// IDEͨ��
#define IDE_CHANNEL_PRIMARY		0x0
#define IDE_CHANNEL_SECONDARY	0x1

// IDE������
#define IDE_DRIVE_MASTER	0xA0
#define IDE_DRIVE_SLAVE		0xB0

// IDE����
#define IDE_CMD_READ		0x20		// ��ȡ��������
#define IDE_CMD_WRITE		0x30		// д����������
#define IDE_CMD_CHECK		0x90		// �����������
#define IDE_CMD_IDENTIFY	0xEC		// ��ʶ����


/**IDE������֧�ֲ���*/
/**IDE Controller Support*/



/**��IDE��������ȡ�Ĵ���*/
void IDE_reg_read(char PS)
{
	unsigned long port_offset = 0;
	
	if (PS == IDE_DRIVE_SLAVE)
	{
		port_offset = IDE_SECONDARY_READ_DATA - IDE_PRIMARY_READ_DATA;
	}
	
	/**������*/
	IDE_register[1] = io_in8(IDE_PRIMARY_READ_ERROR + port_offset);
	IDE_register[2] = io_in8(IDE_PRIMARY_READ_SECCOUNT + port_offset);
	IDE_register[3] = io_in8(IDE_PRIMARY_READ_LBA_0_7 + port_offset);
	IDE_register[4] = io_in8(IDE_PRIMARY_READ_LBA_8_15 + port_offset);
	IDE_register[5] = io_in8(IDE_PRIMARY_READ_LBA_16_23 + port_offset);
	IDE_register[6] = io_in8(IDE_PRIMARY_READ_LBA_D_24_27 + port_offset);
	IDE_register[7] = io_in8(IDE_PRIMARY_READ_STATUS + port_offset);
}

/**��IDE��������������*/
void IDE_reg_cmd(char PS)
{
	unsigned long port_offset = 0;
	
	if (PS == IDE_DRIVE_SLAVE)
	{
		port_offset = IDE_SECONDARY_READ_DATA - IDE_PRIMARY_READ_DATA;
	}
	
	/**д����*/
	io_out8(IDE_PRIMARY_WRITE_FEATURES + port_offset		, IDE_register[1]);
	io_out8(IDE_PRIMARY_WRITE_SECCOUNT + port_offset		, IDE_register[2]);
	io_out8(IDE_PRIMARY_WRITE_LBA_0_7 + port_offset			, IDE_register[3]);
	io_out8(IDE_PRIMARY_WRITE_LBA_8_15 + port_offset		, IDE_register[4]);
	io_out8(IDE_PRIMARY_WRITE_LBA_16_23 + port_offset		, IDE_register[5]);
	io_out8(IDE_PRIMARY_WRITE_LBA_D_24_27 + port_offset		, IDE_register[6]);
	io_out8(IDE_PRIMARY_WRITE_CMD + port_offset				, IDE_register[7]);
}

/**IDE���̸�λ���в�����Ƿ���Ҫ���ֲ�ͬͨ��������IDE����*/
void IDE_reset(void)
{
	/**
	 * �豸���ƼĴ�����
	 * bit 3 = 1��bit 2 = SRST��
	 * bit 1 = IEN��bit 0 = 0
	 * ����SRST��1���ʹ���������ڸ�λ״̬��
	 * IENΪ�Ƿ������ж�
	 */
	io_out8(IDE_PRIMARY_WRITE_RESET, 0xc);
	io_out8(IDE_PRIMARY_WRITE_RESET, 0x8);
}

/**IDE���̵ȴ�*/
void IDE_wait(void)
{
	/**�ȴ�������ʱ*/
	//unsigned long n;
	for (; (io_in8(IDE_PRIMARY_READ_STATUS) & (IDE_STATUS_DRQ | IDE_STATUS_BSY)) != IDE_STATUS_DRQ;);				/**ѭ���ȴ�*/
}

/**ȷ��BSYΪ0*/
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



/**IDE������д����*/
int IDE_write(unsigned long storage_number, const void *src, unsigned long LBA_addr, unsigned long n)
{
	
}

/**��ATA��׼�У�IDE�����30�����������10����ͨ��������*/
#define HD_CMD_READ		0x20			//��ȡ��������
#define HD_CMD_WRITE	0x30			//д����������
#define HD_CMD_CHECK	0x90			//�����������
	
/**��������ʱ��������������*/
#define MAX_ERRORS	10

/**ATӲ�̿������Ĵ����˿ڼ�����*/
/**��ʱ*/
#define HD_DATA			0x1f0
#define HD_ERROR		0x1f1
#define HD_NSECTOR		0x1f2
#define HD_SECTOR		0x1f3
#define HD_LCYL			0x1f4
#define HD_HCYL			0x1f5
#define HD_CURRENT		0x1f6
#define HD_STATUS		0x1f7
/**дʱ*/
#define HD_PRECOMP		0x1f1
#define HD_COMMAND		0x1f7

void hdd_wait(void)
{
	/**�ȴ�������ʱ*/
	//unsigned long n;
	for (; (io_in8(HD_STATUS) & 0x88) != 0x08;);				/**ѭ���ȴ�*/
}

void read_disk(unsigned long LBA, unsigned short *buffer, unsigned long number)
{
	unsigned long offset, i;
	io_out16(HD_NSECTOR,number);								/**����*/
	io_out8(HD_SECTOR,(LBA & 0xff));							/**LBA��ַ7~0*/
	io_out8(HD_LCYL,((LBA >> 8) & 0xff));						/**LBA��ַ15~8*/
	io_out8(HD_HCYL,((LBA >> 16) & 0xff));						/**LBA��ַ23~16*/
	io_out8(HD_CURRENT,(((LBA >> 24) & 0xff) + 0xe0));			/**LBA��ַ27~24 + LBAģʽ����Ӳ��*/
	io_out8(HD_STATUS,HD_CMD_READ);								/**������*/
	/**ѭ����DATA�˿ڶ�ȡ����*/
	for (i = 0; i != number; i ++)
	{
		hdd_wait();
		for (offset = 0; offset < 256; offset ++)
		{
			buffer[(i * 256) + offset] = io_in16(HD_DATA);		/**��DATA�˿��ж�ȡ����*/
		}
	}
	
	return;
}

/**IDE������������*/
int IDE_read(unsigned long storage_number, void *dest, unsigned long LBA_addr, unsigned long n)
{
	read_disk(LBA_addr, dest, n);
	return 0;
	
	short int *buffer = (short int *)dest;
	/**����IDEͨ���ж�*/
	if ((storage_number == SD_IDE_00) | (storage_number == SD_IDE_01))
	{
		/**���ǵ�һͨ���Ĵ�����*/
		
		/**ѭ����ȡ����*/
		for (; n != 0; n --, buffer += (512 / sizeof(short int)), LBA_addr ++)
		{
			IDE_register[2] = 1;
			IDE_register[3] = (LBA_addr & 0xff);
			IDE_register[4] = ((LBA_addr >> 8) & 0xff);
			IDE_register[5] = ((LBA_addr >> 16) & 0xff);
			if (storage_number == SD_IDE_00)
			{
				/**��������*/
				IDE_register[6] = (((LBA_addr >> 24) & 0xff) + 0b11100000);
			}else{
				IDE_register[6] = (((LBA_addr >> 24) & 0xff) + 0b11110000);
			}
			IDE_register[7] = IDE_CMD_READ;
			
			/**��������*/
			IDE_reg_cmd(IDE_CHANNEL_PRIMARY);
			
			/**�ȵ�IDE��һͨ����æʱ*/
			IDE_wait();
			printk("FFF4.3\n");
			/**��ȡ����*/
			unsigned long r;
			for (r = 0; r < (512 / sizeof(short int)); r ++)
			{
				buffer[r] = io_in16(IDE_PRIMARY_READ_DATA);
			}
			printk("FFF3.2\n");
		}
	}else if ((storage_number == SD_IDE_10) | (storage_number == SD_IDE_11))
	{
		/**����ǵڶ�ͨ���Ĵ�����*/
		warning(WARN_STORAGE_NOT_SUPPORT, "The IDE Secondary is not yet supported.");
		return STORAGE_RETVAL_ERR_NOT_SUPPORT;
	}else{
		/**����IDE�������Ĵ�����*/
		return STORAGE_RETVAL_ERR_NOT_SUPPORT;
	}
	return STORAGE_RETVAL_NORMAL;
}

/**IDE��������ʼ��*/
void init_IDE(void)
{
	char *IDE_Description = "IDE Controller";
	unsigned long n;
	
	/**���Ӷ��豸������*/
	SD[SD_IDE_00].Description = IDE_Description;
	SD[SD_IDE_01].Description = IDE_Description;
	SD[SD_IDE_10].Description = IDE_Description;
	SD[SD_IDE_11].Description = IDE_Description;
	
	/**�����ͨ��������״̬*/
	/*io_out8(IDE_PRIMARY_WRITE_LBA_D_24_27, IDE_DRIVE_MASTER);
	io_out8(IDE_PRIMARY_WRITE_FEATURES, 0);
	io_out8(IDE_PRIMARY_WRITE_SECCOUNT, 0);
	io_out8(IDE_PRIMARY_WRITE_LBA_0_7, 0);
	io_out8(IDE_PRIMARY_WRITE_LBA_8_15, 0);
	io_out8(IDE_PRIMARY_WRITE_LBA_16_23, 0);
	io_out8(IDE_PRIMARY_WRITE_CMD, IDE_CMD_IDENTIFY);
	if (io_in8(IDE_PRIMARY_READ_STATUS) != 0)
	{*/
		// ���ڴ���
				
		/**�ڴ���������������ע��*/
		SD[SD_IDE_00].exist = true;
		
		IDE_read(SD_IDE_00, MBR, 0, 1);
		
		SD[SD_IDE_00].SP[0] = MBR->SP[0];
		SD[SD_IDE_00].SP[1] = MBR->SP[1];
		SD[SD_IDE_00].SP[2] = MBR->SP[2];
		SD[SD_IDE_00].SP[3] = MBR->SP[3];
		
		printak("<%#X>IDE Controller:</>Found Primary Master.\n", DeepPink);
	// }
	
	/**�����ͨ���Ӵ���״̬*/
	// io_out8(IDE_PRIMARY_WRITE_LBA_D_24_27, IDE_DRIVE_SLAVE);
	// io_out8(IDE_PRIMARY_WRITE_FEATURES, 0);
	// io_out8(IDE_PRIMARY_WRITE_SECCOUNT, 0);
	// io_out8(IDE_PRIMARY_WRITE_LBA_0_7, 0);
	// io_out8(IDE_PRIMARY_WRITE_LBA_8_15, 0);
	// io_out8(IDE_PRIMARY_WRITE_LBA_16_23, 0);
	// io_out8(IDE_PRIMARY_WRITE_CMD, IDE_CMD_IDENTIFY);
	// if (io_in8(IDE_PRIMARY_READ_STATUS) != 0)
	// {
		// ���ڴ���
		
		
		/**�ڴ���������������ע��*/
		SD[SD_IDE_01].exist = true;
		
		IDE_read(SD_IDE_01, MBR, 0, 1);
		
		SD[SD_IDE_01].SP[0] = MBR->SP[0];
		SD[SD_IDE_01].SP[1] = MBR->SP[1];
		SD[SD_IDE_01].SP[2] = MBR->SP[2];
		SD[SD_IDE_01].SP[3] = MBR->SP[3];
		
		printak("<%#X>IDE Controller:</>Found Primary Slave.\n", DeepPink);
	// }

	/**�����Ϣ*/
	printk("IDE Controller finished.\n");
}

/**������������*/
/**Storage Management*/



/**����������������*/
int storage_read(unsigned long storage_number, void *dest, unsigned long LBA_addr, unsigned long n)
{
	/**ѡ����Ӧ�Ĵ�������ȡ����*/
	switch(storage_number)
	{
		case SD_IDE_00:return IDE_read(storage_number, dest, LBA_addr, n);
		case SD_IDE_01:return IDE_read(storage_number, dest, LBA_addr, n);
		case SD_IDE_10:return IDE_read(storage_number, dest, LBA_addr, n);
		case SD_IDE_11:return IDE_read(storage_number, dest, LBA_addr, n);
		default:return STORAGE_RETVAL_ERR_NOT_SUPPORT;
	}
}

/**���ػ����������*/
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

/**��ʼ��������������*/
unsigned int init_storage(void)
{
	unsigned long n, r;
	
	/**Ϊ������������������ڴ�*/
	SD = bmalloc((SD_NUM) * sizeof(struct Storage_Description));
	if (SD == NULL) error(ERR_NO_MEM_FOR_SD, "No memory for Storage Description.");
	
	/**����������������ȫ����0*/
	memset(SD, 0, (SD_NUM) * sizeof(struct Storage_Description));
	
	/**Ϊ�����������������䱸���ڴ�*/
	sector_buffer = bmalloc(sizeof(struct MBR));
	if (sector_buffer == NULL) error(ERR_NO_MEM_FOR_SCTBUF, "No memory for Storage MBR.");
	
	/**MBRָ��ָ����������*/
	MBR = (struct MBR *)sector_buffer;
	
	/**�����������������ĳ�ʼ���������������*/
	
	/**��ʼ��IDE������*/
	init_IDE();
	
	/**��ӡ���д�������Ϣ*/
	printak("<%#X>Storage List:</>\n", Snow1);
	for (n = 0; n < SD_NUM; n ++)
	{
		if (SD[n].exist == true)
		{
			/**��ӡ������������Ϣ*/
			printak("    <%#X>%s(%d)</>\n", Snow2, SD[n].Description, n);
			
			/**��ӡ�������ķ�����Ϣ*/
			for (r = 0; r < 4; r ++)
			{
				printak("        <%#X>Partition%d:</>", Snow3, r);
				/**���ֻ�����ͷǻ����*/
				if (SD[n].SP[r].active == 0x80)
				{
					printak("<%#X>Active.</>", Orange1);
				}else{
					printak("<%#X>Inactive.</>", Snow3);
				}
				/**����λ����Ϣ*/
				printak("<%#X>File System:%#X,Start(LBA):%d,Length(Sector):%d.\n</>", Snow3, SD[n].SP[r].fs, SD[n].SP[r].start_LBA, SD[n].SP[r].size_sector);
			}
		}else{
			/**��ӡ������������Ϣ*/
			printak("    <%#X>%s(%d, Unmounted)</>\n", Snow4, SD[n].Description, n);
		}
		
	}
}
