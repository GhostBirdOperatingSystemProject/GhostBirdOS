// Explorer loader problem list
// /problem.h

#ifndef PROBLEM_H_
#define PROBLEM_H_

/**错误列表*/
#define ERR_NO_MEM_FOR_ID 1			// 没有可用于中断描述符表的内存
#define ERR_NO_MEM_FOR_SD 2			// 没有可用于储存器描述符表的内存
#define ERR_NO_MEM_FOR_SCTBUF 3			// 没有可用于扇区缓存的内存
#define ERR_NO_MEM_FOR_CONFIG 4			// 没有可以分配给引导配置文件的内存
#define ERR_NO_MEM_FOR_BUFFER 5			// 没有可以分配给缓冲系统的内存
#define ERR_NO_MEM_FOR_FS 6			// 没有可以分配给文件系统的内存
#define ERR_NO_MEM_FOR_MMU 7			// 没有可以分配给MMU的内存
#define ERR_NO_FILE 8				// 没有文件
#define ERR_CONFIG_OVERSIZE 9			// CONFIG.LDR oversized


/**警告列表*/
#define WARN_NO_MEM 0x80000001			// 无充足内存
#define WARN_STORAGE_NOT_SUPPORT 0x80000002	// 暂时不支持这个储存器
#define WARN_SCRIPT_SIZE_BAD 0x80000003		// length of cript incorrect

#endif

