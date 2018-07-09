/**
 * Copyright 2013-2015 by Explorer Developers.
 * made by Hu Wenjie<1@hwj.me>
 * Explorer C library string
 * boot/string.c
 * version:from Linux
 * 7/20/2014
 */

#include "string.h"
#include "mem.h"

/**将字符串转换为大写函数*/
char *strupr(char *str)
{
   char *p = str;
   while (*p != '\0')
   {
      if(*p >= 'a' && *p <= 'z')
      *p -= 0x20;
      p++;
    }
   return str;
}

/**最大处理字符串数据长度定义，仅作判断长度限制处理，非真正可处理长度*/
#define strnum_max 16

/**返回最近出现c的地方到当前指针的长度*/
unsigned long strlenctc(const char *string, char c)
{
	unsigned long n, retval = 0;
	for (n = 0; string[n] != 0; n ++)
	{
		if (string[n] == c)
		{
			retval = n;
			break;
		}
	}

	return retval;
}

/**将字符串转换成长整形数，返回值 = 1：成功，返回值 = 0：不成功*/
int strtol(const char *number, long *result)
{
	unsigned long n;
	
	/**判断数据类型*/
	// 默认为十进制数字
	
	/***/
}

/**将字符串转换成无符号长整形数，返回值 = 1：成功，返回值 = 0：不成功*/
int strtoul(const char *number, unsigned long *result)
{
	unsigned long n, n_num, weight;
	
	/**清空result*/
	*result = 0;
	
	/**判断数据类型*/
	if (((number[0] == '0') && (number[1] == 'x')) || ((number[0] == '0') && (number[1] == 'X')))
	{
		/**十六进制*/
		weight = 16;
		n = 2;
	}else if (((number[0] == '0') && (number[1] == 'b')) || ((number[0] == '0') && (number[1] == 'B'))){
		/**二进制*/
		weight = 2;
		n = 2;
	}else{
		/**十进制*/
		weight = 10;
		n = 0;
	}
	
	/**判断数字长度*/
	for (; n < strnum_max; n ++)
	{
		/**判断是否超出数字范围*/
		if ((weight == 02) && ((number[n] < '0') || (number[n] > '1'))) break;
		if ((weight == 10) && ((number[n] < '0') || (number[n] > '9'))) break;
		if ((weight == 16) && ((number[n] < '0') || (number[n] > '9')) && ((number[n] < 'a') || (number[n] > 'f')) && ((number[n] < 'A') || (number[n] > 'F'))) break;
		
		/**将*result的值乘权值*/
		*result = *result * weight;
		
		/**将该位字符串数字从ASCII转化成同值数字*/
		if (number[n] <= '9')
		{
			*result = *result + (number[n] - '0');
		}else if (number[n] <= 'Z'){
			*result = *result + (number[n] - 'A' + 10);
		}else{
			*result = *result + (number[n] - 'a' + 10);
		}
	}
	
	if (n == 0) return 0;
	return 1;
}

/**
 *strncmp - Compare two length-limited strings
 *@cs: One string
 *@ct: Another string
 *@count: The maximum number of bytes to compare
 */
int strncmp(const char *cs, const char *ct, unsigned long count)
{
	unsigned char c1, c2;

	while (count)
	{
		c1 = *cs++;
		c2 = *ct++;
		if (c1 != c2)
			return c1 < c2 ? -1 : 1;
		if (!c1)
			break;
		count--;
	}
	return 0;
}

/**
 *strcpy - Copy a %NUL terminated string
 *@dest: Where to copy the string to
 *@src: Where to copy the string from
 */
char *strcpy(char *dest, const char *src)
{
	char *tmp = dest;

	while ((*dest++ = *src++) != '\0')
		/*nothing */;
	return tmp;
}

/**
 *strncpy - Copy a length-limited, %NUL-terminated string
 *@dest: Where to copy the string to
 *@src: Where to copy the string from
 *@count: The maximum number of bytes to copy
 *
 *The result is not %NUL-terminated if the source exceeds
 *@count bytes.
 *
 *In the case where the length of @src is less than  that  of
 *count, the remainder of @dest will be padded with %NUL.
 *
 */
char *strncpy(char *dest, const char *src, unsigned long count)
{
	char *tmp = dest;

	while (count--) {
		if ((*tmp = *src) != 0)
			src++;
		tmp++;
	}
	return dest;
}

/**
 *strlcpy - Copy a %NUL terminated string into a sized buffer
 *@dest: Where to copy the string to
 *@src: Where to copy the string from
 *@size: size of destination buffer
 *
 *Compatible with *BSD: the result is always a valid
 *NUL-terminated string that fits in the buffer (unless,
 *of course, the buffer size is zero). It does not pad
 *out the result like strncpy() does.
 */
unsigned long strlcpy(char *dest, const char *src, unsigned long size)
{
	unsigned long ret = strlen(src);

	if (size) {
		unsigned long len = (ret >= size) ? size - 1 : ret;
		memcpy(dest, src, len);
		dest[len] = '\0';
	}
	return ret;
}

/**
 *strlen - Find the length of a string
 *@s: The string to be sized
 */
unsigned long strlen(const char *s)
{
	const char *sc;

	for (sc = s; *sc != '\0'; ++sc)
		/* nothing */;
	return sc - s;
}

/**
 * strnlen - Find the length of a length-limited string
 * @s: The string to be sized
 * @count: The maximum number of bytes to search
 */
unsigned long strnlen(const char *s, unsigned long count)
{
	const char *sc;

	for (sc = s; count-- && *sc != '\0'; ++sc)
		/* nothing */;
	return sc - s;
}

/**
 * strspn - Calculate the length of the Initial substring of @s which only contain letters in @accept
 * @s: The string to be searched
 * @accept: The string to search for
 */
unsigned long strspn(const char *s, const char *accept)
{
	const char *p;
	const char *a;
	unsigned long count = 0;

	for (p = s; *p != '\0'; ++p) {
		for (a = accept; *a != '\0'; ++a) {
			if (*p == *a)
				break;
		}
		if (*a == '\0')
			return count;
		++count;
	}
	return count;
}

char *strnstr(const char *big, const char *little, unsigned long len)
{
	const char *p;

	unsigned long count;
	
	for (p = big; *p != '\0', len --; p ++)
		for (count = 0; ; count ++)
		{
			if (little[count] == '\0')
				return p;
			if (p[count] != little[count])
				break;
		}

	return big;
}

