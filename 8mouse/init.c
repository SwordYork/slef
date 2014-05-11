#include "kernel.h"

#define MAX_LENGTH 		40
struct MOUSE_DEC{
	unsigned char buf[3], phase;
	int x,y,btn;
};

extern struct FIFO8 keyfifo;
extern struct FIFO8 mousefifo;
void enable_mouse(struct MOUSE_DEC *mdec);
int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat);
void init_keyboard(void);


void main(){
	struct BOOTINFO *binfo;
	binfo = (struct BOOTINFO *) ADR_BOOTINFO;

	char s[MAX_LENGTH];
	char mcursor[256];
	char keybuf[KEYBUF_LEN];
	char mousebuf[MOUSEBUF_LEN];
	unsigned char key_data;

	struct MOUSE_DEC mdec;

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

	enable_mouse(&mdec);


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
				if (mouse_decode(&mdec, key_data) != 0){
					char2hex(s, 0x00 , 4);	
					if( (mdec.btn & 0x01) != 0) s[0] = 'L';
					if( (mdec.btn & 0x02) != 0) s[0] = 'R';
					if( (mdec.btn & 0x04) != 0) s[0] = 'C';
					boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 64, 16, 79, 31);
					putfont8_asc(binfo->vram, binfo->scrnx, 64, 16, COL8_FFFFFF, s);

					char2hex(s, mdec.x , 4);	
					boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 96, 16, 111, 31);
					putfont8_asc(binfo->vram, binfo->scrnx, 96, 16, COL8_FFFFFF, s);
					char2hex(s, mdec.y , 4);	
					boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 128, 16, 143, 31);
					putfont8_asc(binfo->vram, binfo->scrnx, 128, 16, COL8_FFFFFF, s);

					boxfill8(binfo->vram, binfo->scrnx, COL8_008484, mx, my, mx + 15, my + 15);
					mx += mdec.x;
					my += mdec.y;
					if(mx < 0){
						mx = 0;
					}
					if(my < 0){
						my = 0;
					}
					if(mx > binfo->scrnx - 16){
						mx = binfo->scrnx -16;
					}
					if(my > binfo->scrny - 16){
						my = binfo->scrny -16;
					}
					itoa(s, mx,MAX_LENGTH);
					boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 0, 79, 15);
					putfont8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);
					itoa(s, my,MAX_LENGTH);
					boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 32, 0, 111, 15);
					putfont8_asc(binfo->vram, binfo->scrnx, 32, 0, COL8_FFFFFF, s);
					putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);
				}

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

void enable_mouse(struct MOUSE_DEC *mdec)
{
//	enable mouse
	wait_KBC_sendready();
	_io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	_io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);

//	may return ACK 0xfa
	mdec->phase = 0;
	return; 
}

int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat){
	if (mdec->phase == 0){
		if(dat == 0xfa){
			mdec->phase = 1;
		}
		return 0;
	}
	if (mdec->phase == 1){
		if((dat & 0xc8) == 0x08){
			mdec->buf[0] = dat;
			mdec->phase = 2;
		}
		return 0;
	}
	if (mdec->phase == 2){
		mdec->buf[1] = dat;
		mdec->phase = 3;
		return 0;
	}
	if (mdec->phase == 3){
		mdec->buf[2] = dat;
		mdec->phase = 1;
		mdec->btn = mdec->buf[0] & 0x07;
		mdec->x = mdec->buf[1];

		mdec->y = mdec->buf[2];
		if ((mdec->buf[0] & 0x10) != 0) {
			mdec->x |= 0xffffff00;
		}
		if ((mdec->buf[0] & 0x20) != 0) {
			mdec->y |= 0xffffff00;
		}
		mdec->y = - mdec->y; 
		return 1;
	}
	return -1;



}
