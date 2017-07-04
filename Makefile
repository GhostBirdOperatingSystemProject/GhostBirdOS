# Date:2017/6/20 16:10
# Author:HuWenjie<1@hwj.me>
# Ghost Bird OS
# Makefile
# 
# BSD 2-Clause License
# 
# Copyright (c) 2017, Ghost Bird Operating System Project Developers.
# All rights reserved.
# Problems:
# 	1.I don't know how to write else ifeq.

# Which of environment are you using
Windows	= Win32
Linux	= Linux
ARCH	= $(Windows)

CC = gcc
AS = nasm
gfr = gfr
qemu = qemu-system-i386
image = $(CURDIR)/image/image.vhd

ifeq ($(ARCH), $(Windows))
	clear_scn = cls
	compress = makecab
	depress = expand
else
	clear_scn = clear
	compress = zip
	depress = unzip
endif

export clear_scn
export CC
export AS
export image
export qemu
export gfr



.PHONY: gfr loader help clean run install all

gfr:
	cd tools/gfr && make all
	
loader:
	cd loader && make all

prepare:$(image:.vhd = .zip) $(image)
	$(depress) $(image:.vhd=.zip) $(image)
		
clean:
	cd loader && make clean
	cd tools/gfr && make clean

dist:clean
	$(compress) $(image) $(image:.vhd=.zip)
	$(RM) $(image)
	
install:
	cd loader && make install
	
all:
	cd loader && make all
	
run:all install
	$(qemu) -drive file=$(image),format=vpc

help:
	$(clean_screen)
	@echo Ghost Bird Project Makefile - Help
	@echo target		introduction
	@echo gfr		make Ghost Bird File System Reader
	@echo loader		make Explorer Loader
	@echo clean		make cl
	@echo loader		make Explorer Loader

