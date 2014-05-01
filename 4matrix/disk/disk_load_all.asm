;load DH sectors to MEM ES:BX 
[bits 16]
CYLS equ 3

disk_load:
	pusha
	mov ah, 0x02		;read flag
	mov al,	dh			;number of sectors
	mov ch, 0x00		;cylinder 0
	mov dh, 0x00		;pointer 0
	mov cl,	0x02		;sector 2	(start from 1)
	
	int 0x13			;interrupt
	jc disk_error
	;cmp dh, al			;al is actul read, but how to restore dh?
	;jne	disk_error
	mov ch, 0x01		;cylinder 0
	add bx, 512 * 17
	
disk_load_next_cylinder:
	mov ah, 0x02		;read flag
	mov al,	18			;number of sectors
	mov dl, [BOOT_DRIVE]
	mov dh, 0x00		;pointer 0
	mov cl,	0x01		;sector 1	(start from 1)
	int 0x13
	jc disk_error

	add ch, 1
	cmp ch, CYLS
	add bx, 512 * 18
	jb disk_load_next_cylinder
	
read_done:
	popa
	ret

disk_error:
	mov bx, DISK_ERROR_MSG
	call print_string
	jmp $

DISK_ERROR_MSG: 
	db "Disk read error", 0
