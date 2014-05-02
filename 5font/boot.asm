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
	KERNEL_OFFSET equ 0x1000

	mov [BOOT_DRIVE], dl		;drive number
	mov bp, 0x9000				;stack position for popa pusha
	mov sp, bp
	
	mov bx, MSG_REAL_MODE
	call print_string

	call load_kernel

	call switch_to_pm

	jmp $

%include "./message/print_string.asm"
%include "./disk/disk_load_all.asm"
%include "./pm/gdt.asm"
;%include "./pm/print_string_pm.asm"
%include "./pm/switch_to_pm.asm"

[bits 16]
load_kernel:
	mov bx, MSG_LOAD_KERNEL
	call print_string
	
	mov bx, KERNEL_OFFSET
	mov dh, 17
	mov dl, [BOOT_DRIVE]
	call disk_load
	ret


[bits 32]
BEGIN_PM:
	mov ebx, MSG_PROT_MODE
;	call print_string_pm

	call KERNEL_OFFSET + 512
	jmp $



BOOT_DRIVE db 0
MSG_REAL_MODE db "Started in 16-bit Real Mode",0
MSG_PROT_MODE db "Successfully landed in 32-bit Protected Mode",0
MSG_LOAD_KERNEL db "Loading kernel into memory.",0
times 510 -( $ - $$ ) db 0 
dw 0xaa55
