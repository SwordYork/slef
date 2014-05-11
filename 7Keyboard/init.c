#include "kernel.h"

#define MAX_LENGTH 		40
extern struct FIFO8 keyfifo;
extern struct FIFO8 mousefifo;
void enable_mouse(void);
void init_keyboard(void);

void main(){
	struct BOOTINFO *binfo;
	binfo = (struct BOOTINFO *) ADR_BOOTINFO;

	char s[MAX_LENGTH];
	char mcursor[256];
	char keybuf[KEYBUF_LEN];
	char mousebuf[MOUSEBUF_LEN];
	char key_data;
	int j;

	int mx,my;

	init_gdtidt();
	init_pic();
	_io_sti();


	fifo8_init(&keyfifo, KEYBUF_LEN, keybuf);
	fifo8_init(&mousefifo, MOUSEBUF_LEN , mousebuf);
	_io_out8(PIC0_IMR,0xf9);
	_io_out8(PIC1_IMR,0xef);


	init_keyboard();

	init_palette();
	init_screen(binfo->vram, binfo->scrnx, binfo->scrny);

	mx = (binfo->scrnx - 16) / 2; 
	my = (binfo->scrny - 28 - 16) / 2;
	init_mouse_cursor8(mcursor, COL8_008484);
	putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);

	enable_mouse();


	for(;;){
		_io_cli();
		if(0 == (fifo8_status(&keyfifo) + fifo8_status(&mousefifo) ) ) {
			_io_stihlt();
		}
		else{
			if(fifo8_status(&keyfifo) != 0){
				key_data = fifo8_get(&keyfifo);
				_io_sti();
				char2hex(s, key_data , 4);	
				boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
				putfont8_asc(binfo->vram, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
			}
			else if(fifo8_status(&mousefifo) != 0){
				key_data = fifo8_get(&mousefifo);
				_io_sti();
				char2hex(s, key_data , 4);	
				boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 64, 16, 79, 31);
				putfont8_asc(binfo->vram, binfo->scrnx, 64, 16, COL8_FFFFFF, s);

			}
		}
	}
}	




#define PORT_KEYDAT				0x0060
#define PORT_KEYSTA				0x0064
#define PORT_KEYCMD				0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE		0x60
#define KBC_MODE				0x47

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

#define KEYCMD_SENDTO_MOUSE		0xd4
#define MOUSECMD_ENABLE			0xf4

void enable_mouse(void)
{
//	enable mouse
	wait_KBC_sendready();
	_io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	_io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
	return; 
//	may return ACK 0xfa
}
