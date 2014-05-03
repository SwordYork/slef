;load DH sectors to MEM ES:BX 
[bits 16]
RCYLS equ 10

disk_load:
	pusha

	mov ax, 0x0820
	mov es, ax
	mov dl, [BOOT_DRIVE]

	mov ah, 0x02		;read flag
	mov al,	17			;number of sectors
	mov ch, 0x00		;cylinder 0
	mov dh, 0x00		;pointer 0
;	mov dl, 0x00
	mov cl,	0x02		;sector 2	(start from 1)
	mov bx, 0

	int 0x13			;interrupt
	jc disk_error
	;cmp dh, al			;al is actul read, but how to restore dh?
	;jne	disk_error

	mov ax, es
	add ax, 0x0220 		;512 * 17
	mov es, ax

	;next pointer
	mov ch, 0x00		;cylinder 0
	mov ah, 0x02		;read flag
	mov al,	18			;number of sectors
	mov dh, 0x01		;pointer 0
	mov cl,	0x01		;sector 1	(start from 1)
	mov bx, 0
	int 0x13
	jc disk_error
	;next cylinder
	mov ch, 0x01		;cylinder 1
	

disk_load_next_cylinder:
	mov ax, es
	add ax, 0x0240		;512 * 18
	mov es, ax
	
	mov ah, 0x02		;read flag
	mov al,	18			;number of sectors
	mov dh, 0x00		;pointer 0
	mov cl,	0x01		;sector 1	(start from 1)
	mov bx, 0
	int 0x13
	jc disk_error

	mov ah, 0x00
	mov dl, 0x00
	int 0x13

;   almost 0x7c00 - 0x500
;   512 * 60
;   read 3 times
	mov ax, es
	add ax, 0x0240
	mov es, ax

	mov ah, 0x02		;read flag
	mov al,	18			;number of sectors
	mov dh, 0x01		;pointer 1
	mov cl,	0x01		;sector 1	(start from 1)
	mov bx, 0
	int 0x13
	jc disk_error

	add ch, 1
	cmp ch, 2
	jne disk_load_next_cylinder
	
;	cmp ch, RCYLS
;	je disk_error
;	jmp disk_error
	popa
	jmp read_done 
;	ret

disk_error:
	mov bx, DISK_ERROR_MSG
	call print_string
	jmp $

DISK_ERROR_MSG: 
	db "Disk read error", 0
