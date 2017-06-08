// Explorer Boot
// /boot/head.h
// consisting all the definition of Boot

#ifndef HEAD_H_
#define HEAD_H_


// ͼ����Ϣ
struct Video_Info
{
	unsigned char *vram;
	unsigned long xres, yres;
	unsigned long vram_length;
	unsigned long bit_per_pixel;
};

extern struct Video_Info Video_Info;

/**�������غ���*/
void (*putpixel)(unsigned int x, unsigned int y, unsigned int color);

/**��ȡ���غ���*/
unsigned int (*getpixel)(unsigned int x, unsigned int y);

#define FONTDATAMAX 4096
extern unsigned char font[FONTDATAMAX];


/**ͣ������
 * �ú���ִ��ʵ���ǻ��ָ��hlt
 * ִ�иú����󣬼������һֱ����ͣ��״̬(�͹���)��
 * ֱ����Ӳ���жϵķ���
 */
void io_hlt(void);

/**�رմ��ж�λ����
 * ����������ִ��ʵ���ǻ��ָ��cli��sti
 * io_cli����ִ�к󣬻ὫPSW�Ĵ������ж�λ��0���ر�Ӳ���ж�
 * io_sti����ִ�к󣬻ὫPSW�Ĵ������ж�λ��1������Ӳ���ж�
 */
void io_cli(void);
void io_sti(void);

/**���뺯��*/
unsigned char io_in8(unsigned long port);
unsigned short io_in16(unsigned long port);
unsigned long io_in32(unsigned long port);

#pragma pack(push)					//���浱ǰ������Ϣ
#pragma pack(1)						//�趨�ṹ����һ���ֽ�Ϊ��λ����

/**��ַ��Χ�������ṹ��*/
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

/**VBE��Ϣ��ṹ��*/
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
/**������Ϣ�ṹ��*/
struct boot_info
{
	/**��Ϣһ��Ҫ��"EBI"(Explorer Boot Information)*/
	char flag[BOOT_FLAG_MAX];
	/**������Ϣ����*/
	unsigned int size;
	
	/**����ģʽ 32λ 0-4GB ����ζ�ѡ����*/
	unsigned short code_sel;
	
	/**����ģʽ 32λ 0-4GB ���ݶζ�ѡ����*/
	unsigned short data_sel;
	
	/**�ڴ�ֲ���Ϣ*/
	struct Address_Range_Descriptor_Structure ARDS[BOOT_ARDS_NUM];
	
	/**VBE��Ϣ��ṹ��*/
	struct VbeInfoBlock VbeInfoBlock;
	/**VBEģʽ��Ϣ�ṹ��*/
	struct ModeInfoBlock ModeInfoBlock;
};

#pragma pack(pop)					//�ָ�ԭ���Ķ��뵥λ

// �ж�������������
#define IDT_P		0x8000
#define IDT_DPL_0	0x00
#define IDT_DPL_1	0x2000
#define IDT_DPL_2	0x4000
#define IDT_DPL_3	0x6000
#define IDT_16		0x00
#define IDT_32		0x800
#define trap_gate		0x700
#define interrupt_gate	0x600

// ����PIC�ļĴ����˿ڶ���
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
