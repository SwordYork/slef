#include "kernel.h"

#define MAX_LENGTH 		40
extern struct KEYBUF keybuf;


void main(){
	struct BOOTINFO *binfo;
	binfo = (struct BOOTINFO *) ADR_BOOTINFO;

	char s[MAX_LENGTH];
	char mcursor[256];
	int mx,my;

	init_gdtidt();
	init_pic();
	_io_sti();
	
	init_palette();
	init_screen(binfo->vram, binfo->scrnx, binfo->scrny);
//	putfont8_asc(binfo->vram, binfo->scrnx, 8, 8, COL8_FFFFFF,"hello slef!");

//	sprintf(s,"scrnx = %d", binfo->scrnx);

	mx = (binfo->scrnx - 16) / 2; 
	my = (binfo->scrny - 28 - 16) / 2;
	init_mouse_cursor8(mcursor, COL8_008484);
	putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);

//	itoa(s,mx,MAX_LENGTH);
//	putfont8_asc(binfo->vram, binfo->scrnx, 8, 64, COL8_FFFFFF, s);

//	itoa(s,my,MAX_LENGTH);
//	putfont8_asc(binfo->vram, binfo->scrnx, 8, 80, COL8_FFFFFF, s);

	_io_out8(PIC0_IMR,0xf9);
	_io_out8(PIC1_IMR,0xef);

	char i;
	for(;;){
		_io_cli();
		if(keybuf.flag == 0) {
			_io_stihlt();
		}
		else{
			i = keybuf.data;
			keybuf.flag = 0;
			_io_sti();
			char2hex(s, i , 4);	
			boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
			putfont8_asc(binfo->vram, binfo->scrnx, 0, 16, COL8_FFFFFF, s);

		}
	}
}	





