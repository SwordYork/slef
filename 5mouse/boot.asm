[org 0x7c00]

; standard FAT16

		JMP		entry			; go to entry
		DB		0x90
		DB		"SLEF-IPL"		; 
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
entry:
	KERNEL_OFFSET equ 0xc400

	mov [BOOT_DRIVE], dl		;drive number
		xor ax, ax
    ;    mov es, ax
        mov ss, ax
;		mov bp, 0x9000				;stack position for popa pusha
		mov sp, 0x7c00
        mov ds, ax

	mov bx, MSG_REAL_MODE
	call print_string

	call load_kernel

%include "./message/print_string.asm"
%include "./disk/disk_load_all.asm"
%include "./pm/gdt.asm"
%include "./pm/switch_to_pm.asm"

[bits 16]
load_kernel:
	mov bx, MSG_LOAD_KERNEL
	call print_string
	jmp disk_load

read_done:
	call switch_to_pm
	jmp $


[bits 32]
BEGIN_PM:
	jmp KERNEL_OFFSET
;	jmp 0x00280000
	jmp $



BOOT_DRIVE db 0
MSG_REAL_MODE db "Started in 16-bit Real Mode",0
MSG_LOAD_KERNEL db "Loading kernel into memory.",0
times 510 -( $ - $$ ) db 0 
dw 0xaa55

; other

		DB		0xf0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00
		RESB	4600
		DB		0xf0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00
		RESB	1469432
