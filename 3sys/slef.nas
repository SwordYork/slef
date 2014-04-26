; slef-os
	ORG		0xc400
	MOV		AL,0x13
	MOV		AH,0x00
	INT		0x10
fin:
	HLT
	JMP		fin 
