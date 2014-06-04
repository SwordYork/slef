;Print String start from MEM bx
print_string:
	pusha

print_string_loop:
	mov al,[bx]		;get char
	cmp al,0
	je string_end
	add bx,1		;mem increase
	mov ah, 0x0e
	int 0x10		;display flag
	jmp print_string_loop

string_end:			;end
	popa
	ret
