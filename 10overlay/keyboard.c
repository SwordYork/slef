#include "kernel.h"

extern struct FIFO8 keyfifo;


void wait_KBC_sendready(void)
{
//	wait for keyboard control circuit
	for (;;) {
		if ((_io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) {
			break;
		}
	}
	return;
}

void init_keyboard(void)
{
	wait_KBC_sendready();
	_io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	wait_KBC_sendready();
	_io_out8(PORT_KEYDAT, KBC_MODE);
	return;
}

void inthandler21(int *esp)
{
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	unsigned char data, s[4];
	_io_out8(PIC0_OCW2, 0x61);
	data = _io_in8(PORT_KEYDAT);

	fifo8_put(&keyfifo, data);
//	char2hex(s, data, 4);	
//	boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
//	putfont8_asc(binfo->vram, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
 	return ;
}

 
