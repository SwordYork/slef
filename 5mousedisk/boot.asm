; haribote-ipl
; TAB=4

RCYLS	EQU		10				;

		ORG		0x7c00			; 


		JMP		entry
		DB		0x90
		DB		"SLEF-IPL"		
		DW		512				
		DB		1				
		DW		1				
		DB		2				
		DW		224				
		DW		2880			
		DB		0xf0			
		DW		9				
		DW		18				
		DW		2				
		DD		0				
		DD		2880			
		DB		0,0,0x29		
		DD		0xffffffff		
		DB		"Slef-OS    "	
		DB		"FAT12   "		
		RESB	18				


entry:
		MOV		AX,0			; 
		MOV		SS,AX
		MOV		SP,0x7c00
		MOV		DS,AX


		MOV		AX,0x0820
		MOV		ES,AX
		MOV		CH,0			; 
		MOV		DH,0			; 
		MOV		CL,2			; 
readloop:
		MOV		SI,0			;
retry:
		MOV		AH,0x02			
		MOV		AL,1			
		MOV		BX,0
		MOV		DL,0x00			
		INT		0x13			
		JNC		next			
		ADD		SI,1		
		CMP		SI,5		
		JAE		error		
		MOV		AH,0x00
		MOV		DL,0x00		
		INT		0x13		
		JMP		retry
next:
		MOV		AX,ES		
		ADD		AX,0x0020
		MOV		ES,AX			
		ADD		CL,1			;
		CMP		CL,18			;
		JBE		readloop		;
		MOV		CL,1
		ADD		DH,1
		CMP		DH,2
		JB		readloop		; 
		MOV		DH,0
		ADD		CH,1
		CMP		CH,RCYLS
		JB		readloop		;

		MOV		[0x0ff0],CH		; 
		jmp		switch_to_pm	
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
fin:
		HLT						
		JMP		fin				
msg:
		DB		0x0a, 0x0a		;
		DB		"load error"
		DB		0x0a			;
		DB		0

gdt_start:

gdt_null:
	dd 0x0
	dd 0x0

gdt_core:
;	dw 0xffff
;	dw 0x0
;	db 0x0
;	db 10011010b
;	db 11001111b		;code seg
;	db 0x0
	dw	0xffff,0x0000,0x9a00,0x0047

gdt_data:
	dw 0xffff
	dw 0x0
	db 0x0
	db 10010010b
	db 11001111b
	db 0x0

gdt_end:

gdt_descriptor:
	dw gdt_end - gdt_start - 1 
	dd gdt_start

CODE_SEG equ	gdt_core - gdt_start
DATA_SEG equ 	gdt_data - gdt_start

[bits 16]
CYLS	equ 0x0ff0
LEDS 	equ 0x0ff1			;memory to set parm
VMODE	equ	0x0ff2
SCRNX	equ	0x0ff4
SCRNY	equ	0x0ff6
VRAM 	equ	0x0ff8

switch_to_pm:
							;set VIDEO
							;see http://www.ctyme.com/rbrown.htm
	mov	al, 0x13
	mov ah, 0x00
	int 0x10				; call interrupt to set graphic mode
						
	mov byte [VMODE],8
	mov	word [SCRNX],320
	mov word [SCRNY],200
	mov dword [VRAM],0x000a0000 

	mov ah, 0x02
	int 0x16		; keyboard bios

; PIC init
	mov al, 0xff
	out 0x21, al
	nop
	out 0xa1, al

	cli				;disable cpu interrupt

;enable A20GATE
;	call	waitkbdout
;	mov		al,0xd1
;	out 	0x64,al
;	call 	waitkbdout
;	mov 	al,0xdf
;	out 	0x60,al
;	call 	waitkbdout
;	jmp		toload_gdt


; waitkbdout
;waitkbdout:
;	in 	al,0x64
;	and al,0x64
;	jnz	waitkbdout
;	ret

;toload_gdt:
	lgdt [gdt_descriptor]
	
	mov eax, cr0
	or 	eax, 0x1
	mov cr0, eax

	jmp CODE_SEG:init_pm	;flush

	;to do memory copy

[bits 32]
init_pm:
	mov ax,DATA_SEG
	mov ds,ax
	mov ss,ax
 	mov es,ax 
	mov fs,ax
	mov gs,ax

	mov ebp, 0x90000		;stack postion
	mov esp,ebp

	jmp BEGIN_PM			;begin protect mode

[bits 32]
BEGIN_PM:
	JMP	0xc400


[bits 16]
		RESB 	510-($-$$)		; 0x7dfe‚Ü‚Å‚ð0x00‚Å–„‚ß‚é–½—ß

		DB		0x55, 0xaa

		DB		0xf0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00
		RESB	4600
		DB		0xf0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00
		RESB	1469432
