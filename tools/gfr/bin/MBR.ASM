; Date:2017/6/30 22:13
; Author:HuWenjie<1@hwj.me>
; Ghost Bird File System Reader
; BIN - MBR
; 
; BSD 2-Clause License
; 
; Copyright (c) 2017, Ghost Bird Operating System Project Developers.
; All rights reserved.

	ORG		0X7C00
	
	; init stack & segment register
	XOR		AX, AX
	MOV		SS,	AX
	MOV		SP, $$
	
	
	

	TIMES	510-($-$$)	DB	0
	DW		0XAA55
