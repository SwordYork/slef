; haribote-ipl
; TAB=4

		ORG		0x7c00			; 


		JMP		entry
		DB		0x90
		DB		"HELLOIPL"		; 
		DW		512				; 
		DB		1				; 
		DW		1				; 
		DB		2				;
		DW		224				;
		DW		2880			;
		DB		0xf0			; 
		DW		9				; 
		DW		18				;
		DW		2				;
		DD		0				;
		DD		2880			;
		DB		0,0,0x29		;
		DD		0xffffffff		; 
		DB		"SLEFOS     "	; 
		DB		"FAT12   "		; 
		RESB	18				; 

; Program body

entry:
		MOV		AX,0			; Register initial
		MOV		SS,AX
		MOV		SP,0x7c00
		MOV		DS,AX			;date segment register
		mov 	AH,0x00
		mov		DL,0x80
		INT		0x13
		JC 		error

; Read Sector

		MOV		BX,0x8000		;memory ES:BX 0x8000
		MOV		ES,BX
		MOV		CH,0x00			; cylinder 0
		MOV		DH,0x00			; head 0 may be 1, obverse reverse side
		MOV		CL,0x02			; sector 2
		JMP		next

readloop:
		MOV		SI,0

retry:
		INT		0x13			; call
		JNC		pullmsg			; no error then print
		ADD		SI,1			; try 5 times
		CMP		SI,5
		JAE		error			;all error than break
		MOV		AH,0x00			;reset
		MOV		DL,0x80
		INT		0x13
		JMP		retry

next:
		MOV		BX,0x0000		
		MOV		AX,ES
		ADD		AX,0x0020
		MOV		ES,AX			;ES update
		ADD		CL,1
		CMP		CL,8
		MOV		AH,0x02			; AH=0x02: bios read sector 
		MOV		AL,0x01			; Read one
		MOV		DL,0x80			; A driver
		JBE		readloop

fin:
		HLT						; 
		JMP		fin				;

pullmsg:
		MOV		SI,[ES:BX]
;		MOV		SI,msg2

printmsg:
		MOV		AL,[SI]
		ADD		SI,1			; 
		CMP		AL,0
		JE		next
		MOV		AH,0x0e			;
		MOV		BX,15			;
		INT		0x10			;
		JMP		printmsg
		
error:
		MOV		SI,msg

putloop:
		MOV		AL,[SI]
		ADD		SI,1			; 
		CMP		AL,0
		JE		fin
		MOV		AH,0x0e			;
		MOV		BX,15			;
		INT		0x10			;
		JMP		putloop
msg2:
		DB		0x0a, 0x0a		;
		DB		"loading"
		DB		0x0a			;
		DB		0
msg:
		DB		0x0a, 0x0a		;
		DB		"load error"
		DB		0x0a			;
		DB		0

		RESB	0x1fe-($-$$)		;

		DB		0x55, 0xaa
; other

		DB		0xf0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00
		RESB	4600
		DB		0xf0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00
		RESB	1469432
