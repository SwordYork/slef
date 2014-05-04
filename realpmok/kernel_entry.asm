[org 0xc400]
BOTPAK	EQU		0x00280000		; bootpackÌ[hæ
DSKCAC	EQU		0x00100000		; fBXNLbVÌê
DSKCAC0	EQU		0x00008000		; fBXNLbVÌêiA[hj

; BOOT_INFOÖW
CYLS	EQU		0x0ff0			; u[gZN^ªÝè·é
LEDS	EQU		0x0ff1
VMODE	EQU		0x0ff2			; FÉÖ·éîñB½rbgJ[©H
SCRNX	EQU		0x0ff4			; ðxÌX
SCRNY	EQU		0x0ff6			; ðxÌY
VRAM	EQU		0x0ff8			; OtBbNobt@ÌJnÔn


; æÊ[hðÝè

		MOV		AL,0x13			; VGAOtBbNXA320x200x8bitJ[
		MOV		AH,0x00
		INT		0x10
		MOV		BYTE [VMODE],8	; æÊ[hð·éiC¾êªQÆ·éj
		MOV		WORD [SCRNX],320
		MOV		WORD [SCRNY],200
		MOV		DWORD [VRAM],0x000a0000

; L[{[hÌLEDóÔðBIOSÉ³¦Äàç¤

		MOV		AH,0x02
		INT		0x16 			; keyboard BIOS
		MOV		[LEDS],AL

; PICªêØÌèÝðó¯t¯È¢æ¤É·é
;	ATÝ·@ÌdlÅÍAPICÌú»ð·éÈçA
;	±¢ÂðCLIOÉâÁÄ¨©È¢ÆA½ÜÉnOAbv·é
;	PICÌú»Í ÆÅâé

		MOV		AL,0xff
		OUT		0x21,AL
		NOP						; OUT½ßðA±³¹éÆ¤Ü­¢©È¢@íª éçµ¢ÌÅ
		OUT		0xa1,AL

		CLI						; ³çÉCPUxÅàèÝÖ~

; CPU©ç1MBÈãÌÉANZXÅ«éæ¤ÉAA20GATEðÝè

		CALL	waitkbdout
		MOV		AL,0xd1
		OUT		0x64,AL
		CALL	waitkbdout
		MOV		AL,0xdf			; enable A20
		OUT		0x60,AL
		CALL	waitkbdout

; veNg[hÚs


		LGDT	[GDTR0]			; bèGDTðÝè
		MOV		EAX,CR0
		AND		EAX,0x7fffffff	; bit31ð0É·éiy[WOÖ~Ì½ßj
		OR		EAX,0x00000001	; bit0ð1É·éiveNg[hÚsÌ½ßj
		MOV		CR0,EAX
		JMP		pipelineflush
pipelineflush:
		MOV		AX,1*8			;  ÇÝ«Â\ZOg32bit
		MOV		DS,AX
		MOV		ES,AX
		MOV		FS,AX
		MOV		GS,AX
		MOV		SS,AX
		jmp $

; bootpackÌ]

		MOV		ESI,bootpack	; ]³
		MOV		EDI,BOTPAK		; ]æ
		MOV		ECX,512*1024/4
		CALL	memcpy

; Â¢ÅÉfBXNf[^à{ÌÊuÖ]

; Ü¸Íu[gZN^©ç

		MOV		ESI,0x7c00		; ]³
		MOV		EDI,DSKCAC		; ]æ
		MOV		ECX,512/4
		CALL	memcpy

; cèS

		MOV		ESI,DSKCAC0+512	; ]³
		MOV		EDI,DSKCAC+512	; ]æ
		MOV		ECX,0
		MOV		CL,BYTE [CYLS]
		IMUL	ECX,512*18*2/4	; V_©çoCg/4ÉÏ·
		SUB		ECX,512/4		; IPLÌª¾¯·µø­
		CALL	memcpy

; asmheadÅµÈ¯êÎ¢¯È¢±ÆÍSµIíÁ½ÌÅA
;	 ÆÍbootpackÉC¹é

; bootpackÌN®

		MOV		EBX,BOTPAK
		MOV		ECX,[EBX+16]
		ADD		ECX,3			; ECX += 3;
		SHR		ECX,2			; ECX /= 4;
		JZ		skip			; ]·é×«àÌªÈ¢
		MOV		ESI,[EBX+20]	; ]³
		ADD		ESI,EBX
		MOV		EDI,[EBX+12]	; ]æ
		CALL	memcpy
skip:
		MOV		ESP,[EBX+12]	; X^bNúl
		JMP		DWORD 2*8:0x0000001b

waitkbdout:
		IN		 AL,0x64
		AND		 AL,0x02
		JNZ		waitkbdout		; ANDÌÊª0ÅÈ¯êÎwaitkbdoutÖ
		RET

memcpy:
		MOV		EAX,[ESI]
		ADD		ESI,4
		MOV		[EDI],EAX
		ADD		EDI,4
		SUB		ECX,1
		JNZ		memcpy			; ø«Zµ½Êª0ÅÈ¯êÎmemcpyÖ
		RET
; memcpyÍAhXTCYvtBNXðüêYêÈ¯êÎAXgO½ßÅà¯é

		ALIGNB	16
GDT0:
		RESB	8				; kZN^
		DW		0xffff,0x0000,0x9200,0x00cf	; ÇÝ«Â\ZOg32bit
		DW		0xffff,0x0000,0x9a28,0x0047	; ÀsÂ\ZOg32bitibootpackpj

		DW		0
GDTR0:
		DW		8*3-1
		DD		GDT0

		ALIGNB	16
bootpack:
