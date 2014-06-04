#include "kernel.h"
#define MAX_LENGTH 		40

extern struct FIFO8 keyfifo;
extern struct FIFO8 mousefifo;
extern struct TIMERCTL timerctl;

void main(){
	struct BOOTINFO *binfo;
	binfo = (struct BOOTINFO *) ADR_BOOTINFO;

	char s[MAX_LENGTH];
	char mcursor[256];

	// mouse and keyboard data
	char keybuf[KEYBUF_LEN];
	char mousebuf[MOUSEBUF_LEN];
	char timerbuf[8],timerbuf2[8], timerbuf3[8];

	struct TIMER *timer, *timer2, *timer3;

	unsigned char fifo_data;
	struct MOUSE_DEC mdec;
	int mx,my;

	// memory manager
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	unsigned int memtotal;

	unsigned int count = 0;

	// overlay control
	struct SHTCTL *shtctl;
	struct SHEET *sht_back, *sht_mouse;
	struct SHEET *sht_win;
	unsigned char *buf_back, buf_mouse[256];
	unsigned char *buf_win;

	// gdt control
	init_gdtidt();
	init_pic();
	init_pit();
	_io_sti();

	// init fifo
	fifo8_init(&keyfifo, KEYBUF_LEN, keybuf);
	fifo8_init(&mousefifo, MOUSEBUF_LEN , mousebuf);

	struct FIFO8 timerfifo, timerfifo2, timerfifo3;
	fifo8_init(&timerfifo, 8, timerbuf);
	timer = timer_alloc();
	timer_init(timer, &timerfifo, 1);
	timer_settime(timer, 1000);


	fifo8_init(&timerfifo2, 8, timerbuf2);
	timer2 = timer_alloc();
	timer_init(timer2, &timerfifo2, 1);
	timer_settime(timer2, 300);
	
	fifo8_init(&timerfifo3, 8, timerbuf3);
	timer3 = timer_alloc();
	timer_init(timer3,&timerfifo3, 1);
	timer_settime(timer3, 50);


	// must after init pit
	// qemu 1000 = 10s
	// bochs 1000 = 1s
	//settimer(10000, &timerfifo, 1);
	_io_out8(PIC0_IMR,0xf8);
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
	sht_win = sheet_alloc(shtctl);

	buf_back = (unsigned char *) memman_alloc_4k(memman, binfo->scrnx * binfo->scrny);		// background buffer
	buf_win = (unsigned char *) memman_alloc_4k(memman,160*52);


	sheet_setbuf(sht_back, buf_back, binfo->scrnx, binfo->scrny, -1);	// bind buf
	sheet_setbuf(sht_mouse, buf_mouse, 16, 16, 99);
	sheet_setbuf(sht_win, buf_win, 160, 52, -1);

	init_screen(buf_back, binfo->scrnx, binfo->scrny);	// init color
	init_mouse_cursor8(buf_mouse, 99);
	make_window8(buf_win, 160, 52, "counter");

	// init slide
	sheet_slide(sht_back, 0, 0);


	// initial mouse position
	mx = (binfo->scrnx - 16) / 2; 
	my = (binfo->scrny - 28 - 16) / 2;

	// init mouse slide
	// init position 0,0
	sheet_slide(sht_mouse, mx, my);
	sheet_slide(sht_win,80,72);

	sheet_updown(sht_back, 0);
	sheet_updown(sht_win, 1);
	sheet_updown(sht_mouse, 2);

	/***************  memory info  *********/
	itoa(s,memtotal / 1024 /1024,MAX_LENGTH);
	putfont8_asc(buf_back, binfo->scrnx, 0, 32, COL8_FFFFFF, s);

	itoa(s,memman_total(memman) / 1024,MAX_LENGTH);
	putfont8_asc(buf_back, binfo->scrnx, 0, 64, COL8_FFFFFF, s);

	sheet_refresh(sht_back, 0, 32, binfo->scrnx, 80);
	/***************  memory info  *********/


	for(;;){
//		count ++;
//		itoa(s,count,MAX_LENGTH);
		itoa(s,timerctl.count,MAX_LENGTH);
		boxfill8(buf_win, 160, COL8_C6C6C6, 40, 28,119, 43);
		putfont8_asc(buf_win, 160, 40, 28, COL8_000000, s);
		sheet_refresh(sht_win, 40, 28, 120, 44);

		_io_cli();
		if(0 == (fifo8_status(&keyfifo) + fifo8_status(&mousefifo) + fifo8_status(&timerfifo) + fifo8_status(&timerfifo2) + fifo8_status(&timerfifo3) ) ) {
//		if(0 == (fifo8_status(&keyfifo) + fifo8_status(&mousefifo)) ) {
			_io_stihlt(); 
		}
		else{
			if(fifo8_status(&keyfifo) != 0){
				fifo_data = fifo8_get(&keyfifo);
				_io_sti();
		//		char2hex(s, key_data , 4);	
		//		boxfill8(buf_back, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
		//		putfont8_asc(buf_back, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
		//		sheet_refresh(sht_back, 0, 16, 16, 32);
			}
			else if(fifo8_status(&mousefifo) != 0){
				fifo_data = fifo8_get(&mousefifo);
				_io_sti();
				if (mouse_decode(&mdec,fifo_data) != 0){
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
					sheet_slide(sht_mouse, mx, my);
				}

			}
			else if (fifo8_status(&timerfifo) != 0){
				fifo_data = fifo8_get(&timerfifo);
				_io_sti();
				putfont8_asc(buf_back, binfo->scrnx, 0, 0, COL8_FFFFFF, "10[sec]");
				sheet_refresh(sht_back, 0, 0, 56, 16);
			}
			else if(fifo8_status(&timerfifo2) != 0){
				fifo_data = fifo8_get(&timerfifo2);
				_io_sti();
				putfont8_asc(buf_back, binfo->scrnx, 100, 0, COL8_FFFFFF, "3[sec]");
				sheet_refresh(sht_back, 100, 0, 156, 16);
			}
			else if(fifo8_status(&timerfifo3) != 0){
				fifo_data = fifo8_get(&timerfifo3);
				if(fifo_data != 0){
					timer_init(timer3, &timerfifo3, 0);
					boxfill8(buf_back, binfo->scrnx, COL8_FFFFFF, 8, 96, 15, 111);
				}
				else{
					timer_init(timer3, &timerfifo3, 1);
					boxfill8(buf_back, binfo->scrnx, COL8_008484, 8, 96, 15, 111);
				}
				timer_settime(timer3, 50);
				sheet_refresh(sht_back, 8, 96, 16, 112);


			}
			
		}
	}
}	
