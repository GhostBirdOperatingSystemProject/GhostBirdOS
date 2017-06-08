/**
 * Copyright 2013-2016 by Explorer Developers.
 * made by Hu Wenjie<1@hwj.me>
 * Explorer loader storage Support Definition
 * Explorer loader/boot/storage.h
 * version:2.0
 * 6/23/2016 12:09 PM
 */

#ifndef STORAGE_H_
#define STORAGE_H_

/**����������С*/
#define SECTOR_SIZE 512

/**�������ŷ���*/
#define SD_IDE_00 0
#define SD_IDE_01 1
#define SD_IDE_10 2
#define SD_IDE_11 3
#define SD_NUM (SD_IDE_11 + 1)

/**�����������ṹ����Ҫ���ڴ��С*/
#define storage_SD_size SD_MAX * sizeof(struct Storage_Description)					// ���������������С

#pragma pack(push)					// ���浱ǰ������Ϣ
#pragma pack(1)						// �趨�ṹ����һ���ֽ�Ϊ��λ����

/**�����������ṹ��*/
struct Storage_Partition{
	unsigned char active;
	unsigned char CHS_start[3];
	unsigned char fs;
	unsigned char CHS_end[3];
	unsigned int start_LBA;
	unsigned int size_sector;
};

#pragma pack(pop)					// �ָ�ԭ���Ķ��뵥λ

#ifndef STORAGE_C_

/**��������ڴ�ָ��*/
extern void *sector_buffer;

/**��������������*/
struct Storage_Description
{
	_Bool exist;
	struct Storage_Partition SP[4];
};
extern struct Storage_Description *SD;

#endif

/**����ֵ���*/
#define STORAGE_RETVAL_NORMAL 0				// ��֧��
#define STORAGE_RETVAL_ERR_NOT_SUPPORT -1	// ��֧��

/**����������������*/
int storage_read(unsigned long storage_number, void *dest, unsigned long LBA_addr, unsigned long n);

/**���ػ����������*/
unsigned long storage_active_partition(void);

/**��ʼ��������������*/
unsigned int init_storage(void);

#endif

