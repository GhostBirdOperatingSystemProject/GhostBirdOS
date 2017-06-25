// Explorer Memory Management Unit
// /boot/MMU.c

#include "../configure.h"
#include "../problem.h"
#include "MMU.h"
#include "VI.h"
#include "lib/stdlib.h"

/**δ�����ڴ�ָ��*/
unsigned long unalloc_ptr = mem_start;
/**���ɻ����ڴ���亯��*/
void *bmalloc(unsigned long size)
{
	void *retval;
	
	/**�����㹻���ڴ湩���䣬��ɹ����䣬�����Ѿ�������ڴ��ַ*/
	if ((unalloc_ptr + size) < (mem_start + mem_size))
	{
		retval = (void *)unalloc_ptr;
		unalloc_ptr += size;
		return retval;
	/**�ռ䲻��*/
	}else{
		warning(WARN_NO_MEM, "The available memory for the boot program is not enough.");
		return NULL;
	}
}
