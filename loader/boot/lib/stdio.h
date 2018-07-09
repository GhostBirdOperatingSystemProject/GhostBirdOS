/**
 * Copyright 2013-2018 by Explorer Developers.
 * made by Hu Wenjie<1@hwj.me>
 * stdio.h
 * /loader/boot/lib/stdio.h
 * version:1.0.0
 * 7/9/2018 14:32
 */

#ifndef STDIO_H_
#define STDIO_H_

#include "stdarg.h"

int vsprintf(char *buf, const char *fmt, va_list args);
int sprintf(char *buf, const char *fmt, ...);

#endif

