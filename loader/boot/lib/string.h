/**
 *Copyright 2013-2015 by Explorer Developers.
 * Hu Wenjie<1@hwj.me>
 *Explorer C standard string head
 *boot/string.h
 *version:Alpha
 *9/7/2014 2:19 PM
 */

#ifndef STRING_H_
#define STRING_H_

/**将字符串转换为大写函数*/
char *strupr(char *str);

/**判断C是否是小写字母，若是，则返回TRUE，否则返回FALSE*/
#define islower(c) ((_ctype+1)[c]&(_L)) 

/**将字符c转换为大写英文字母*/
#define toupper(c) (_ctmp=c,islower(_ctmp)?_ctmp-('a'-'A'):_ctmp)

/**返回最近出现c的地方到当前指针的长度*/
unsigned long strlenctc(const char *string, char c);

/**将字符串转换成长整形数，返回值 = 1：成功，返回值 = 0：不成功*/
int strtol(const char *number, long *result);

/**将字符串转换成无符号长整形数，返回值 = 1：成功，返回值 = 0：不成功*/
int strtoul(const char *number, unsigned long *result);

char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, unsigned long count);
unsigned long strlcpy(char *dest, const char *src, unsigned long size);
int strncmp(const char *cs, const char *ct, unsigned long count);
unsigned long strlen(const char *s);

/**
 * strnlen - Find the length of a length-limited string
 * @s: The string to be sized
 * @count: The maximum number of bytes to search
 */
unsigned long strnlen(const char *s, unsigned long count);

unsigned long strspn(const char *s, const char *accept);

char *strnstr(const char *big, const char *little, unsigned long len);

#endif
