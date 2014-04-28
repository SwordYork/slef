print_string:
	pusha
no_empty:
	mov al,[bx]
	add bx,1
	mov ah, 0x0e
	int 0x10
	cmp al,0
	jne no_empty
	popa
	ret
