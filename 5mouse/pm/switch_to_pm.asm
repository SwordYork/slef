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
