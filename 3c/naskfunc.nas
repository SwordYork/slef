; naskfunc
; TAB=4



		GLOBAL	_io_hlt			



[SECTION .text]		

_io_hlt:	; void io_hlt(void);
		JMP	error
error:
		MOV		SI,msg
putloop:
		MOV		AL,[SI]
		ADD		SI,1			
		CMP		AL,0
		JE		fin
		MOV		AH,0x0e			
		MOV		BX,15			
		INT		0x10			
		JMP		putloop
msg:
		DB		0x0a, 0x0a		
		DB		"load error"
		DB		0x0a			
		DB		0

fin:
		RET

