#include "kernel.h"
#define MAX_LENGTH 		40

extern struct TIMERCTL timerctl;

void main(){
	struct BOOTINFO *binfo;
	struct FIFO32 fifo;
	binfo = (struct BOOTINFO *) ADR_BOOTINFO;

	char s[MAX_LENGTH];
	char mcursor[256];

	// mouse and keyboard data
	int fifobuf[BUF_LEN];

	struct TIMER *timer, *timer2, *timer3;

	int fifo_data;
	struct MOUSE_DEC mdec;
	int mx,my;

	// memory manager
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	unsigned int memtotal;


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
	fifo32_init(&fifo, BUF_LEN, fifobuf);

	init_keyboard(&fifo, 256);
	enable_mouse(&fifo, 512, &mdec);

	timer = timer_alloc();
	timer_init(timer, &fifo, 10);
	timer_settime(timer, 1000);


	timer2 = timer_alloc();
	timer_init(timer2, &fifo, 3);
	timer_settime(timer2, 300);
	
	timer3 = timer_alloc();
	timer_init(timer3,&fifo, 1);
	timer_settime(timer3, 50);


	// must after init pit
	// qemu 1000 = 10s
	// bochs 1000 = 1s
	_io_out8(PIC0_IMR,0xf8);
	_io_out8(PIC1_IMR,0xef);

	// mouse and keyboard control

	// memory set
	memtotal = memtest(0x00400000, 0xbfffffff);
	memman_init(memman);
	// this is ERROR!
	// c400 is system
	memman_free(memman, 0x00400000, memtotal - 0x00400000);


	/**************************************************
	 *  screen 
	 *  ----------------------------------------------*/
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
	/*-------------------------------------------------
	 *  screen 
	 *  ***********************************************/

	/***************  memory info  *********/
	/*  
	itoa(s,memtotal / 1024 /1024,MAX_LENGTH);
	putfont8_asc(buf_back, binfo->scrnx, 0, 32, COL8_FFFFFF, s);

	itoa(s,memman_total(memman) / 1024,MAX_LENGTH);
	putfont8_asc(buf_back, binfo->scrnx, 0, 64, COL8_FFFFFF, s);

	sheet_refresh(sht_back, 0, 32, binfo->scrnx, 80);
	*/
	/***************  memory info  *********/


	for(;;){
//		itoa(s,count,MAX_LENGTH);
		itoa(s,timerctl.count,MAX_LENGTH);
		putfont8_asc_sht(sht_win, 40, 28, COL8_000000, COL8_C6C6C6, s, 6);
		_io_cli();
		if(0 == (fifo32_status(&fifo) )){
			_io_stihlt(); 
		}
		else{
			fifo_data = fifo32_get(&fifo);
		//	itoa(s, fifo_data , MAX_LENGTH);	
		//	putfont8_asc_sht(sht_back, 200, 160, COL8_FFFFFF, COL8_008484, s, 14);
			_io_sti();
			if(256 <= fifo_data && fifo_data < 512){ // keyboard data
				fifo_data -= 256;
				char2hex(s, fifo_data , 4);	
				putfont8_asc_sht(sht_back, 200, 160, COL8_FFFFFF, COL8_008484, s, 4);
			}
			else if(512 <= fifo_data && fifo_data < 768){ // mouse data
				// fifo_data -= 512; 	//have some error
				if (mouse_decode(&mdec,fifo_data - 512) != 0){
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
			else if( fifo_data == 10){
				putfont8_asc_sht(sht_back, 0, 140, COL8_FFFFFF, COL8_008484,"10[sec]", 7);
				}
			else if( fifo_data == 3){
				putfont8_asc_sht(sht_back, 100, 0, COL8_FFFFFF, COL8_008484,"3[sec]", 6);
			}
			else{
				if(fifo_data != 0){
					timer_init(timer3, &fifo, 0);
					boxfill8(buf_back, binfo->scrnx, COL8_FFFFFF, 8, 96, 15, 111);
				}
				else{
					timer_init(timer3, &fifo, 1);
					boxfill8(buf_back, binfo->scrnx, COL8_008484, 8, 96, 15, 111);
				}
				timer_settime(timer3, 50);
				sheet_refresh(sht_back, 8, 96, 16, 112);
			}

		}
	}	
}
