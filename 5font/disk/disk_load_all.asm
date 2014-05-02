;load DH sectors to MEM ES:BX 
[bits 16]
RCYLS equ 1

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

	mov bx, KERNEL_OFFSET
	mov ax, es
	add ax, 0x220 		;512 * 17
	mov es, ax

	;next pointer
	mov ch, 0x00		;cylinder 0
	mov ah, 0x02		;read flag
	mov al,	18			;number of sectors
	mov dh, 0x01		;pointer 0
	mov cl,	0x01		;sector 1	(start from 1)
	int 0x13
	jc disk_error
	;next cylinder
	mov ch, 0x01		;cylinder 1
	

disk_load_next_cylinder:
	mov bx, KERNEL_OFFSET
	mov ax, es
	add ax, 0x240		;512 * 18
	mov es, ax
	
	mov ah, 0x02		;read flag
	mov al,	18			;number of sectors
	mov dh, 0x00		;pointer 0
	mov cl,	0x01		;sector 1	(start from 1)
	int 0x13
	jc disk_error

;   almost 0x7c00 - 0x500
;   512 * 60
;   read 3 times
;	mov bx, KERNEL_OFFSET 
;	mov ax, es
;	add ax, 0x240
;	mov es, ax

;	mov ah, 0x02		;read flag
;	mov al,	18			;number of sectors
;	mov dh, 0x01		;pointer 1
;	mov cl,	0x01		;sector 1	(start from 1)
;	int 0x13
;	jc disk_error


;	add ch, 1
;	cmp ch, RCYLS
;	jb disk_load_next_cylinder
	
read_done:
	popa
	ret

disk_error:
	mov bx, DISK_ERROR_MSG
	call print_string
	jmp $

DISK_ERROR_MSG: 
	db "Disk read error", 0
