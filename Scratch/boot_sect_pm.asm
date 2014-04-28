[org 0x7c00]
VMODE equ 0x0ff2
SCRNX equ 0x0ff4
SCRNY equ 0x0ff6
VRAM  equ 0x0ff8

MOV AL, 0x13
MOV AH, 0x00
INT 0x10
MOV BYTE [VMODE],8
MOV	WORD [SCRNX],320
MOV WORD [SCRNY],200
MOV DWORD [VRAM],0x000a0000 

KERNEL_OFFSET equ 0x1000

	mov [BOOT_DRIVE], dl
	mov bp, 0x9000
	mov sp, bp
	
	mov bx, MSG_REAL_MODE
	call print_string

	call load_kernel

	call switch_to_pm

	jmp $

%include "print_string.asm"
%include "print_string_pm.asm"
%include "disk_load.asm"
%include "gdt.asm"
%include "switch_to_pm.asm"

[bits 16]
load_kernel:
	mov bx, MSG_LOAD_KERNEL
	call print_string
	
	mov bx, KERNEL_OFFSET
	mov dh, 15
	mov dl, [BOOT_DRIVE]
	call disk_load
	ret

[bits 32]
BEGIN_PM:
	mov ebx, MSG_PROT_MODE
;	call print_string_pm
	call KERNEL_OFFSET
	jmp $



BOOT_DRIVE db 0
MSG_REAL_MODE db "Started in 16-bit Real Mode",0
MSG_PROT_MODE db "Successfully landed in 32-bit Protected Mode",0
MSG_LOAD_KERNEL db "Loading kernel into memory.",0
times 510 -( $ - $$ ) db 0 
dw 0xaa55
