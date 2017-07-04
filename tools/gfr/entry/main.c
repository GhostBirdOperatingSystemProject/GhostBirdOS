/**
 * Date:2017/6/8 2:21
 * Author:HuWenjie<1@hwj.me>
 * Ghost Bird File System Reader
 * main function of program
 * 
 * BSD 2-Clause License
 * 
 * Copyright (c) 2017, Ghost Bird Operating System Project Developers.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/config.h"
#include "../include/return.h"
#include "../include/info.h"

#define SECTOR_SIZE 512

int glo_argc;
char **glo_argv;

// -i: init this image with MBR mode
void arg_i(void)
{
	printf("i\n");
}

// -cp: create partition
void arg_cp(void)
{
	printf("cp\n");
}

// -dp: delete partition
void arg_dp(void)
{
	printf("dp\n");
}

// -df: delete file
void arg_df(void)
{
	printf("df\n");
}

// -f: format a partition with GBFS
void arg_f(void)
{
	printf("f\n");
}

int arg_ws(void)
{
	int retval;
	int LBA;
	int src_len;
	int dst_len;
	int i;
	FILE *src_file = NULL;
	FILE *dst_file = NULL;
	
	// Check parameter number
	if (glo_argc < 5)
	{
		printf("Too few arguments.\n");
		retval = RET_FAIL;
		goto finish;
	}
	
	// Open files & get location argument
	src_file = fopen(glo_argv[2], "rb");
	dst_file = fopen(glo_argv[3], "rb+");
	
	LBA = atoi(glo_argv[4]);
	
	// Check reading files
	if ((src_file == NULL))
	{
		printf("Source sector error!\n");
		retval = RET_FAIL;
		goto finish;
	}
	
	if ((dst_file == NULL))
	{
		printf("Destination image error!\n");
		retval = RET_FAIL;
		goto finish;
	}
	
	// Get files' length
	fseek(src_file, 0, SEEK_END);
	src_len = ftell(src_file);
	
	fseek(dst_file, 0, SEEK_END);
	dst_len = ftell(dst_file);
	
	// Check if the location is correct
	if (LBA < 1)
	{
		printf("LBA start at 1.\n");
		retval = RET_FAIL;
		goto finish;
	}
	
	if (((LBA - 1) * SECTOR_SIZE + src_len) > \
	    (dst_len - SECTOR_SIZE))
	{
		printf("LBA error or Source file too big.\n");
		retval = RET_FAIL;
		goto finish;
	}
	
	// Writing dst file
	fseek(src_file, 0, SEEK_SET);
	fseek(dst_file, (LBA - 1) * SECTOR_SIZE, SEEK_SET);
	
	for (i = 0; i < src_len; i ++)
	{
		putc(getc(src_file), dst_file);
	}
	
	return RET_SUCC;
	
finish:
	fclose(src_file);
	fclose(dst_file);
	return retval;
}

// -rs: read sector
int arg_rs(void)
{
	printf("rs\n");
	return RET_FAIL;
}

// -wf: write file
void arg_wf(void)
{
	printf("wf\n");
}

// -rs: read file
void arg_rf(void)
{
	printf("rs\n");
}

// -si: show image information
void arg_si(void)
{
	printf("si\n");
}

// -sp: show partition information
void arg_sp(void)
{
	printf("sp\n");
}

// -sf: show directory information
void arg_sf(void)
{
	printf("sf\n");
}

// -sh: hex viewer
void arg_sh(void)
{
	printf("sh\n");
}

// -v: version information
void arg_v(void)
{
	// Show some necessary info
	printf(SOFTWARE " " VERSION SUBVERSION "\n");
	printf(COPYRIGHT"\n");
	printf("Support image format:\n");
	printf("Support File System:\n");
	return;
}

int main(int argc, char *argv[])
{
	// Copy argument
	glo_argc = argc;
	glo_argv = argv;
	
	// Check Arguments
	if (argc == 1)
	{
		printf("No command.\n");
		return RET_FAIL;
	}else if (argv[1][0] != '-')
	{
		printf("Command error!\n");
		return RET_FAIL;
	}
	
	switch (argv[1][1])
	{
		case 'v':
			arg_v();
			break;
			
		case 'w':
			goto write;
			
		case 'r':
			goto read;
			
		default:
			printf("'%s' is unsupported command!\n", argv[1]);
			return RET_FAIL;
	}
	
write:
	if (argv[1][2] == 's')
		return arg_ws();

read:
	if (argv[1][2] == 's')
		return arg_rs();
	
	return RET_FAIL;
}

