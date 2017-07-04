# Date:2017/6/8 2:21
# Author:HuWenjie<1@hwj.me>
# Ghost Bird File System Reader
# Makefile
# 
# BSD 2-Clause License
# 
# Copyright (c) 2017, Ghost Bird Operating System Project Developers.
# All rights reserved.

CC			:= gcc
DIRS		:= entry fs bf io
CFILES		:= $(foreach dir,$(DIRS),$(wildcard $(dir)/*.c))
OBJ			:= $(CFILES:.c=.o)

EXECUTE := gfr.exe
CFLAGS := -Wall -O2

ifneq ($(MAKECMDGOALS), clean)
-include $(CFILES:.c=.d)
endif

.PHONY: $(EXECUTE)

$(EXECUTE):all
	$(CC) -o $(EXECUTE) $(OBJ)

all:$(OBJ)

%.d:%.c
	gcc -M $(CFLAGS) $< > $@
	@sed -i 's:$(notdir $(<:.c=.o)):$@ $(<:.c=.o):' $@

# %.d:%.c
# @echo $@ $(dir $<) > $@
# @gcc -M $(CFLAGS) $< -o $@
# $(shell gcc -M $(CFLAGS) $<) >> $@
	

%.o:%.c
	gcc -c $(CFLAGS) $< -o $@
	
clean:
	$(RM) $(EXECUTE)
	$(RM) $(CFILES:.c=.d)
	$(RM) $(OBJ)
