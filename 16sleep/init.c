#include "kernel.h"
#define MAX_LENGTH 		40



extern struct TIMERCTL timerctl;

void task_b_main(struct SHEET *sht_win_b);

//static int count = 0;
struct SHEET *sht_back; 

void main(){
	struct BOOTINFO *binfo;
	struct FIFO32 fifo;
	binfo = (struct BOOTINFO *) ADR_BOOTINFO;

	char s[MAX_LENGTH];
	char mcursor[256];

	// mouse and keyboard data
	int fifobuf[BUF_LEN];

	struct TIMER *timer;

	int fifo_data;
	struct MOUSE_DEC mdec;
	int mx,my;

	// memory manager
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	unsigned int memtotal;


	// overlay control
	struct SHTCTL *shtctl;
	struct SHEET *sht_win, *sht_mouse, *sht_win_b[3];
	unsigned char *buf_back, buf_mouse[256];
	unsigned char *buf_win, *buf_win_b;
	struct TASK *task_a,*task_b[3];

	// gdt control
	init_gdtidt();
	init_pic();
	init_pit();
	_io_sti();

	// init fifo
	fifo32_init(&fifo, BUF_LEN, fifobuf, 0);

	init_keyboard(&fifo, 256);
	enable_mouse(&fifo, 512, &mdec);


	timer = timer_alloc();
	timer_init(timer,&fifo, 1);
	timer_settime(timer, 50);


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

	int i;
	task_a = (struct TASK *) task_init(memman);
	// task win b
	for (i = 0; i < 3; ++i){
		sht_win_b[i] = sheet_alloc(shtctl);
		buf_win_b = (unsigned char *) memman_alloc_4k(memman, 144 * 52);
		sheet_setbuf(sht_win_b[i], buf_win_b, 144, 52 , -1);
		make_window8(buf_win_b, 144, 52, "back window");
		task_b[i] = task_alloc();
		// why -8, see p306
		// 8 = 4 byte address + 4 byte  value
		task_b[i]->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024 - 8;
		task_b[i]->tss.eip = (int) &task_b_main;
		task_b[i]->tss.eflags = 0x00000202; // IF = 1;
		task_b[i]->tss.es = 2 * 8;
		task_b[i]->tss.cs = 1 * 8;
		task_b[i]->tss.ss = 2 * 8;
		task_b[i]->tss.ds = 2 * 8;
		task_b[i]->tss.fs = 2 * 8;
		task_b[i]->tss.gs = 2 * 8;
		*((int *) (task_b[i]->tss.esp+4)) = (int) sht_win_b[i];
		task_submit(task_b[i], i + 1);
	}

	// task win
	sht_win = sheet_alloc(shtctl);

	buf_back = (unsigned char *) memman_alloc_4k(memman, binfo->scrnx * binfo->scrny);		// background buffer
	buf_win = (unsigned char *) memman_alloc_4k(memman,160*52);


	sheet_setbuf(sht_back, buf_back, binfo->scrnx, binfo->scrny, -1);	// bind buf
	sheet_setbuf(sht_mouse, buf_mouse, 16, 16, 99);
	sheet_setbuf(sht_win, buf_win, 160, 52, -1);

	init_screen(buf_back, binfo->scrnx, binfo->scrny);	// init color
	init_mouse_cursor8(buf_mouse, 99);
	make_window8(buf_win, 160, 52, "counter");



	// initial mouse position
	mx = (binfo->scrnx - 16) / 2; 
	my = (binfo->scrny - 28 - 16) / 2;

	int cursor_x, cursor_c;
	make_textbox8(sht_win, 8, 28, 144, 16, COL8_FFFFFF);
	cursor_x = 8;
	cursor_c = COL8_FFFFFF;

	// init mouse slide
	// init position 0,0
	// init slide
	sheet_slide(sht_back, 0, 0);
	sheet_slide(sht_mouse, mx, my);
    sheet_slide(sht_win_b[0],320,52);
	sheet_slide(sht_win_b[1],40,152);
	sheet_slide(sht_win_b[2],320,152);
	sheet_slide(sht_win,40,52);

	sheet_updown(sht_back, 0);
	sheet_updown(sht_win, 4);
	sheet_updown(sht_win_b[0], 1);
	sheet_updown(sht_win_b[1], 2);
	sheet_updown(sht_win_b[2], 3);
	sheet_updown(sht_mouse, 5);
	/*-------------------------------------------------
	 *  screen 
	 *  ***********************************************/

	/***************  memory info  *********/
	itoa(s,memtotal / 1024 /1024,MAX_LENGTH);
	putfont8_asc(buf_back, binfo->scrnx, 0, 32, COL8_FFFFFF, s);

	itoa(s,memman_total(memman) / 1024,MAX_LENGTH);
	putfont8_asc(buf_back, binfo->scrnx, 0, 64, COL8_FFFFFF, s);

	sheet_refresh(sht_back, 0, 32, binfo->scrnx, 80);
	/***************  memory info  *********/

	/*
	 * TSS
	 */
	fifo.task = task_a;
		
	int count = 0;

	for(;;){
		count ++;
		_io_cli();
		if(0 == (fifo32_status(&fifo) )){
		//	task_sleep(task_a);
			_io_stihlt(); 
			//_io_sti();  // too fast
		}
		else{
			fifo_data = fifo32_get(&fifo);
			_io_sti();
			if(256 <= fifo_data && fifo_data < 512){ // keyboard data
				if(fifo_data < 256 + 0x54){
					if(keytable[fifo_data-256] != 0){
						s[0] = keytable[fifo_data-256];
						s[1] = 0;
						putfont8_asc_sht(sht_win, cursor_x, 28, COL8_000000, COL8_FFFFFF,s, 1);
						cursor_x += 8;		// next pos
					}
					if(fifo_data == 256 + 0x0e && cursor_x >= 8){
						putfont8_asc_sht(sht_win, cursor_x, 28, COL8_000000, COL8_FFFFFF," ", 1);
						cursor_x -= 8;
					}
				}
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
					if( (mdec.btn & 0x01) != 0 && (mx > sht_win->vx0 && mx < (sht_win->vx0 + sht_win->bxsize)) && ( my > sht_win->vy0 - sht_win->bysize*2 && my < (sht_win->vy0 + sht_win->bysize * 2) ) ){
						sheet_slide(sht_win, mx - 80, my - 8);
					}
				}

			}
			else{
			  	if(fifo_data != 0){
					timer_init(timer, &fifo, 0);
					boxfill8(buf_back, binfo->scrnx, COL8_FFFFFF, 8, 96, 15, 111);
				}
				else{
					timer_init(timer, &fifo, 1);
					boxfill8(buf_back, binfo->scrnx, COL8_008484, 8, 96, 15, 111);
				}
				timer_settime(timer, 50);
				sheet_refresh(sht_back, 8, 96, 16, 112);
			}

		}
	}
}

void task_b_main(struct SHEET *sht_win_b)
{
	struct FIFO32 fifo;
	struct TIMER  *timer;
	int i, fifobuf[128];
	//extern int count;
	char s[MAX_LENGTH];

	int count = 0;
	fifo32_init(&fifo, 128, fifobuf, 0);
	timer = timer_alloc();
	timer_init(timer, &fifo, 100);
	timer_settime(timer, 100);
	extern struct SHEET *sht_back;


	for (;;) { 
		count ++ ;
		_io_cli(); 
		if(fifo32_status(&fifo) == 0){
			_io_stihlt();
			//_io_sti();
		}
		else{
			i = fifo32_get(&fifo);
			_io_sti();
			if(i==100){
				itoa(s,count,MAX_LENGTH);
				putfont8_asc_sht(sht_win_b, 24, 28, COL8_000000, COL8_C6C6C6, s ,12 );
				timer_settime(timer, 100);
			}
		}
	}
}
