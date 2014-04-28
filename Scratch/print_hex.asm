print_hex:
	pusha
	cmp dx,0xface
	jne _return 
	mov bx, HEX_OUT
	call print_string
	popa
	ret
_return:
	mov bx,HEX_OUT2
	call print_string
	popa
	ret


HEX_OUT: db '0xface',0 
HEX_OUT2: db '0xdada',0 
