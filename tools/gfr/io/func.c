/**
 * Date:2017/6/30 22:14
 * Author:HuWenjie<1@hwj.me>
 * Ghost Bird File System Reader
 * IO - main
 * 
 * BSD 2-Clause License
 * 
 * Copyright (c) 2017, Ghost Bird Operating System Project Developers.
 * All rights reserved.
 **/

#include <stdio.h>
#include "../include/return.h"
#include "func.h"
 
FILE *dst_img = NULL;

struct io_func *io_func;

// Write to an image
int IO_write(int sector, int offset, char c)
{
	return 0;
}

// Read from an image
int IO_read(int sector, int offset)
{
	return 0;
}

// Close image
int IO_close(void)
{
	return 0;
}

// Set the type of image
int IO_type(int type)
{
	return 0;
}

// Open an image
int IO_open(char *image)
{
	int retval = RET_SUCC;
	
	// Open the file
	dst_img = fopen(image, "rb+");
	
	if ((dst_img == NULL))
	{
		printf("Destination image error!\n");
		retval = RET_FAIL;
		goto finish;
	}
	
	// Analyze the type of image
	
	
	// Close the file and return
finish:
	fclose(dst_img);
	return retval;
}
