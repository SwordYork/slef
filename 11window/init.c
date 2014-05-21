#include "kernel.h"
#define MAX_LENGTH 		40

extern struct FIFO8 keyfifo;
extern struct FIFO8 mousefifo;



void main(){
	struct BOOTINFO *binfo;
	binfo = (struct BOOTINFO *) ADR_BOOTINFO;

	char s[MAX_LENGTH];
	char mcursor[256];

	// mouse and keyboard data
	char keybuf[KEYBUF_LEN];
	char mousebuf[MOUSEBUF_LEN];
	unsigned char key_data;
	struct MOUSE_DEC mdec;
	int mx,my;

	// memory manager
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	unsigned int memtotal;

	// overlay control
	struct SHTCTL *shtctl;
	struct SHEET *sht_back, *sht_mouse;
	unsigned char *buf_back, buf_mouse[256];

	// gdt control
	init_gdtidt();
	init_pic();
	_io_sti();

	// init fifo
	fifo8_init(&keyfifo, KEYBUF_LEN, keybuf);
	fifo8_init(&mousefifo, MOUSEBUF_LEN , mousebuf);
	_io_out8(PIC0_IMR,0xf9);
	_io_out8(PIC1_IMR,0xef);

	// mouse and keyboard control
	init_keyboard();
	enable_mouse(&mdec);

	// memory set
	memtotal = memtest(0x00400000, 0xbfffffff);
	memman_init(memman);
	// this is ERROR!
	// c400 is system
//	memman_free(memman, 0x00001000, 0x0009e000); /* 0x00001000 - 0x0009efff */
	memman_free(memman, 0x00400000, memtotal - 0x00400000);


	// video initialize
	init_palette();

	shtctl = shtctl_init(memman, binfo->vram, binfo->scrnx, binfo->scrny);
	sht_back = sheet_alloc(shtctl);			// background sheet
	sht_mouse = sheet_alloc(shtctl);		// mouse sheet
	buf_back = (unsigned char *) memman_alloc_4k(memman, binfo->scrnx * binfo->scrny);		// background buffer
	sheet_setbuf(sht_back, buf_back, binfo->scrnx, binfo->scrny, -1);	// bind buf
	sheet_setbuf(sht_mouse, buf_mouse, 16, 16, 99);

	init_screen(buf_back, binfo->scrnx, binfo->scrny);	// init color
	//init_mouse_cursor8(mcursor, COL8_008484);
	init_mouse_cursor8(buf_mouse, 99);

	// init slide
	sheet_slide(sht_back, 0, 0);


	// initial mouse position
	mx = (binfo->scrnx - 16) / 2; 
	my = (binfo->scrny - 28 - 16) / 2;

	// init mouse slide
	// init position 0,0
	sheet_slide(sht_mouse, mx, my);
	sheet_updown(sht_back, 0);
	sheet_updown(sht_mouse, 1);


	/***************  memory info  *********/
	itoa(s,memtotal / 1024 /1024,MAX_LENGTH);
	putfont8_asc(buf_back, binfo->scrnx, 0, 32, COL8_FFFFFF, s);

	itoa(s,memman_total(memman) / 1024,MAX_LENGTH);
	putfont8_asc(buf_back, binfo->scrnx, 0, 64, COL8_FFFFFF, s);

	sheet_refresh(sht_back, 0, 32, binfo->scrnx, 80);
	/***************  memory info  *********/


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
				boxfill8(buf_back, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
				putfont8_asc(buf_back, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
				sheet_refresh(sht_back, 0, 16, 16, 32);
			}
			else if(fifo8_status(&mousefifo) != 0){
				key_data = fifo8_get(&mousefifo);
				_io_sti();
				if (mouse_decode(&mdec, key_data) != 0){
			//		char2hex(s, 0x00 , 4);	
			//		if( (mdec.btn & 0x01) != 0) s[0] = 'L';
			//		if( (mdec.btn & 0x02) != 0) s[0] = 'R';
			//		if( (mdec.btn & 0x04) != 0) s[0] = 'C';
			//		boxfill8(buf_back, binfo->scrnx, COL8_008484, 64, 16, 79, 31);
			//		putfont8_asc(buf_back, binfo->scrnx, 64, 16, COL8_FFFFFF, s);

			//		char2hex(s, mdec.x , 4);	
			//		boxfill8(buf_back, binfo->scrnx, COL8_008484, 96, 16, 111, 31);
			//		putfont8_asc(buf_back, binfo->scrnx, 96, 16, COL8_FFFFFF, s);
			//		char2hex(s, mdec.y , 4);	
			//		boxfill8(buf_back, binfo->scrnx, COL8_008484, 128, 16, 143, 31);
			//		putfont8_asc(buf_back, binfo->scrnx, 128, 16, COL8_FFFFFF, s);
			//		sheet_refresh(shtctl, sht_back, 32, 16, 32 + 15 * 8, 32);

					mx += mdec.x;
					my += mdec.y;
					if(mx < 0){
						mx = 0;
					}
					if(my < 0){
						my = 0;
					}
					if(mx > binfo->scrnx - 1){
						mx = binfo->scrnx -1;
					}
					if(my > binfo->scrny - 1){
						my = binfo->scrny -1;
					}
					itoa(s, mx,MAX_LENGTH);
					boxfill8(buf_back, binfo->scrnx, COL8_008484, 0, 0, 79, 15);
					putfont8_asc(buf_back, binfo->scrnx, 0, 0, COL8_FFFFFF, s);
					itoa(s, my,MAX_LENGTH);
					boxfill8(buf_back, binfo->scrnx, COL8_008484, 32, 0, 111, 15);
					putfont8_asc(buf_back, binfo->scrnx, 32, 0, COL8_FFFFFF, s);

					sheet_refresh(sht_back, 0, 0, 111, 16);
					sheet_slide(sht_mouse, mx, my);
				}

			}
		}
	}
}	
