;load DH sectors to MEM ES:BX 
[bits 16]
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
	popa
	ret

disk_error:
	mov bx, DISK_ERROR_MSG
	call print_string
	jmp $

DISK_ERROR_MSG: 
	db "Disk read error", 0
