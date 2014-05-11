gdt_start:

gdt_null:
	dd 0x0
	dd 0x0

gdt_core:
	dw 0xffff			;0:15 limit
	dw 0x0 				;15:00 Base 1
	db 0x0 				;16:23 Base 2
	db 10011010b		;      Type
	db 11001111b		;code seg
	db 0x0				;24:31 Base 3
;	dw	0xffff,0x0000,0x9a00,0x0047

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
