[bits 32]
VIDEO_MEMORY	equ	0xa0000			
WHITE_ON_BLACK	equ 0x0f

print_string_pm:
	pusha
	mov edx,VIDEO_MEMORY

print_string_pm_loop:
	mov al,[ebx]
	mov ah, WHITE_ON_BLACK			;color
	cmp al,0				
	je print_string_pm_done			;end
	mov [edx],ax
	add ebx,1
	add edx,2						;two bytes
	jmp print_string_pm_loop

print_string_pm_done:
	popa
	ret
