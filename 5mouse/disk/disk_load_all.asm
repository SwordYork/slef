;load DH sectors to MEM ES:BX 
[bits 16]
RCYLS equ 10

disk_load:
	mov ax, 0x0820
	mov es, ax
	mov dl, [BOOT_DRIVE]
	mov ch, 0x00		;cylinder 0
	mov dh, 0x00		;pointer 0
	mov cl,	0x02		;sector 2	(start from 1)

read_loop:
	mov si, 0			;error times

retry:
	mov ah, 0x02		;read flag
	mov al,	1			;number of sectors
	mov bx, 0
;	mov dl, 0x00
	int 0x13			;interrupt
	jnc disk_load_next
	
	add si, 1
	cmp si, 5
	jae	disk_error

	mov ah, 0x00
	mov dl, 0x00
	int 0x13			;reset disk
	jmp retry


disk_load_next:
	mov ax, es
	add ax, 0x0020		;512 * 18
	mov es, ax
	
	add cl, 1
	cmp cl, 18
	jbe read_loop
	mov cl, 1
	add dh, 1
	cmp dh, 2
	jb 	read_loop
	mov dh, 0		;pointer 1
	add ch, 1
	cmp ch, RCYLS
	jb read_loop
	jmp read_done 

disk_error:
	mov bx, DISK_ERROR_MSG
	call print_string
	jmp $

DISK_ERROR_MSG: 
	db "Disk read error", 0
