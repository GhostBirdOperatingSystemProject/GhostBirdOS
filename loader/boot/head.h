// Explorer Boot
// /boot/head.h
// consisting all the definition of Boot

#ifndef HEAD_H_
#define HEAD_H_

/**重置函数*/
void reset(void);

/**停机函数
 * 该函数执行实体是汇编指令hlt
 * 执行该函数后，计算机会一直处于停机状态(低功耗)，
 * 直到有硬件中断的发生
 */
void io_hlt(void);

/**关闭打开中断位函数
 * 这两个函数执行实体是汇编指令cli和sti
 * io_cli函数执行后，会将PSW寄存器的中断位置0，关闭硬件中断
 * io_sti函数执行后，会将PSW寄存器的中断位置1，接收硬件中断
 */
void io_cli(void);
void io_sti(void);

/**输入函数*/
unsigned char io_in8(unsigned long port);
unsigned short io_in16(unsigned long port);
unsigned long io_in32(unsigned long port);

#pragma pack(push)					//保存当前对齐信息
#pragma pack(1)						//设定结构体以一个字节为单位对齐

/**地址范围描述符结构体*/
#define ARDS_FREE	1
#define ARDS_RESV	2
struct Address_Range_Descriptor_Structure
{
	unsigned int BaseAddrLow;
	unsigned int BaseAddrHigh;
	unsigned int LengthLow;
	unsigned int LengthHigh;
	unsigned int Type;
};

/**VBE信息块结构体*/
#define VBE_SIGN_NUM	4
struct VbeInfoBlock
{
	unsigned char VbeSignature[VBE_SIGN_NUM];
	unsigned short VbeVersion;
	unsigned short OemStringPtr_Off;
	unsigned short OemStringPtr_Seg;
	unsigned char Capabilities[4];
	unsigned short VideoModePtr_Off;
	unsigned short VideoModePtr_Seg;
	unsigned short TotalMemory;
	unsigned short OemSoftwareRev;
	unsigned int OemVendorNamePtr;
	unsigned int OemProductNamePtr;
	unsigned int OemProductRevPtr;
	unsigned char Reserved[222];
	unsigned char OemData[256];
};

struct ModeInfoBlock
{
	/**Mandatory information for all VBE revisions*/
	unsigned short ModeAttributes;
	unsigned char WinAAttributes;
	unsigned char WinBAttributes;
	unsigned short WinGranularity;
	unsigned short WinSize;
	unsigned short WinASegment;
	unsigned short WinBSegment;
	unsigned int WinFuncPtr;
	unsigned short BytesPerScanLine;
	
	/**Mandatory information for VBE 1.2 and above*/
	unsigned short XResolution;
	unsigned short YResolution;
	unsigned char XCharSize;
	unsigned char YCharSize;
	unsigned char NumberOfPlanes;
	unsigned char BitsPerPixel;
	unsigned char NumberOfBanks;
	unsigned char MemoryModel;
	unsigned char BankSize;
	unsigned char NUmberOfImagePages;
	unsigned char Reserved1;
	
	/**Direct Color fields(required for direct/6 and YUV/7 memory models)*/
	unsigned char RedMaskSize;
	unsigned char RedFieldPosition;
	unsigned char GreenMaskSize;
	unsigned char GreenFieldPosition;
	unsigned char BlueMaskSize;
	unsigned char BlueFieldPosition;
	unsigned char RsvdMaskSize;
	unsigned char RsvdFieldPosition;
	unsigned char DirectColorModeInfo;
	
	/**Mandatory infomation for VBE 2.0 and above*/
	unsigned int PhysBasePtr;
	unsigned int Reserved2;
	unsigned short Reserved3;
	
	/**Mandatory information for VBE 3.0 and above*/
	unsigned short LinBytesPerScanLine;
	unsigned char BnkNumberOfImagePages;
	unsigned char LinNumberOfImagePages;
	unsigned char LinRedMaskSize;
	unsigned char LinRedFieldPosition;
	unsigned char LinGreenMaskSize;
	unsigned char LinGreenFieldPosition;
	unsigned char LinBlueMaskSize;
	unsigned char LinBlueFieldPosition;
	unsigned char LinRsvdMaskSize;
	unsigned char LinRsvdFieldPosition;
	unsigned int MaxPixelClock;
	unsigned char Reserved4[189];
	unsigned char Reserved5;
};

#define BOOT_FLAG_MAX	4
#define BOOT_ARDS_NUM	8
/**启动信息结构体*/
struct boot_info
{
	/**信息一定要是"EBI"(Explorer Boot Information)*/
	char flag[BOOT_FLAG_MAX];
	/**启动信息长度*/
	unsigned int size;
	
	/**保护模式 32位 0-4GB 代码段段选择子*/
	unsigned short code_sel;
	
	/**保护模式 32位 0-4GB 数据段段选择子*/
	unsigned short data_sel;
	
	/**内存分布信息*/
	struct Address_Range_Descriptor_Structure ARDS[BOOT_ARDS_NUM];
	
	/**VBE信息块结构体*/
	struct VbeInfoBlock VbeInfoBlock;
	/**VBE模式信息结构体*/
	struct ModeInfoBlock ModeInfoBlock;
};

#pragma pack(pop)					//恢复原来的对齐单位

// 中断描述符表属性
#define IDT_P		0x8000
#define IDT_DPL_0	0x00
#define IDT_DPL_1	0x2000
#define IDT_DPL_2	0x4000
#define IDT_DPL_3	0x6000
#define IDT_16		0x00
#define IDT_32		0x800
#define trap_gate		0x700
#define interrupt_gate	0x600

// 主从PIC的寄存器端口定义
#define PIC0_ICW1		0x0020
#define PIC0_OCW2		0x0020
#define PIC0_IMR		0x0021
#define PIC0_ICW2		0x0021
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC1_ICW1		0x00a0
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1
#define PIC1_ICW2		0x00a1
#define PIC1_ICW3		0x00a1
#define PIC1_ICW4		0x00a1

#define PIC0_intr_offset	0x20
#define PIC1_intr_offset	PIC0_intr_offset + 0x8

#endif
