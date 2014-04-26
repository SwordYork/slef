mov ah, 0x0e		;display char


[org 0x7c00]
;X
mov al,the_secret
int 0x10

; memory
mov al,[the_secret]
int 0x10

;plus
mov bx, the_secret
add bx, 0x7c00
mov al,[bx]
int 0x10

mov al, [0x7c1d]  ; start from header
int 0x10

jmp $

the_secret:
	db "X"

times 510 -( $ - $$ ) db 0 
dw 0xaa55
