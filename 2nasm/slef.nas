; hello-os
; TAB=4

		ORG		0x7c00			; Program load address 
								; origin memory address to load
								; 	
section .text
; standard FAT16

		JMP		entry			; go to entry
		DB		0x90
		DB		"HELLOIPL"		; 
		DW		512				;
		DB		1				;
		DW		1				;
		DB		2				;
		DW		224				;
		DW		2880			;
		DB		0xf0			;
		DW		9				;
		DW		18				;
		DW		2				; 
		DD		0				; 
		DD		2880			; 
		DB		0,0,0x29		; 
		DD		0xffffffff		; 
		DB		"Slef-OS    "	; 
		DB		"FAT12   "		; 
		RESB	18				; 

; Program  body

; Main Register:
; (E)AX(H,L)	|	accumulator
; (E)CX(H,L)	|	counter
; (E)DX(H,L)	|	data
; (E)BX(H,L)	|	base				| mem
; Index Register
; (E)SP			| 	stack pointer
; (E)BP			|	base pointer		| mem
; (E)SI			| 	source index		| mem
; (E)DI			| 	destination index	| mem
; Segment register
; ES			|	extra segment
; CS			| 	code segment
; SS			| 	stack segment
; DS			|	data segment
; FS			| 	segment part2
; GS			| 	segment part3


entry:
		MOV		AX,0			; Register initial: AX = 0
		MOV		SS,AX
		MOV		SP,0x7c00
		MOV		DS,AX
		MOV		ES,AX

		MOV		SI,msg			; msg Address: 0x7c74

putloop:
		MOV		AL,[SI]			; Get data in memory SI
		ADD		SI,1			; SI add 1
		CMP		AL,0
		JE		fin				; jump if equal
		MOV		AH,0x0e			; display a char
		MOV		BX,15			; color 
		INT		0x10			; call BIOS video: interrupt 16th function
		JMP		putloop

fin:
		HLT						; halt cpu
		JMP		fin				; 

msg:
		DB		0x0a, 0x0a		; new lines
		DB		"NASM Success!!"
		DB		0x0a			; new line
		DB		0

		RESB	0x1fe-($-$$)	; fill 0x00 only .text section, so $$ = 0

		;RESB	0x7dfe-($-$$)	; This is nask, nasm not support	

		DB		0x55, 0xaa


; other

		DB		0xf0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00
		RESB	4600
		DB		0xf0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00
		RESB	1469432
