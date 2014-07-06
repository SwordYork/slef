#ifndef _KERNEL_H
#define _KERNEL_H
#include "hankaku.h"

/***********************
 *     display 
 *     font
 *     In graphic.c
 ***********************/
extern char hankaku[4096];

#define COL8_000000		0
#define COL8_FF0000		1
#define COL8_00FF00		2
#define COL8_FFFF00		3
#define COL8_0000FF		4
#define COL8_FF00FF		5
#define COL8_00FFFF		6
#define COL8_FFFFFF		7
#define COL8_C6C6C6		8
#define COL8_840000		9
#define COL8_008400		10
#define COL8_848400		11
#define COL8_000084		12
#define COL8_840084		13
#define COL8_008484		14
#define COL8_848484		15

void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void init_screen(char *vram, int xsize, int ysize);
void putfont8(char *vram, int xsize, int x, int y, char color, char *font);
void putfont8_asc(char *vram, int xsize, int x, int y, char color, unsigned char *s);
void init_mouse_cursor8(char *mouse, char bc);
void putblock8_8(char *vram, int vxsize, int pxsize,
	int pysize, int px0, int py0, char *buf, int bxsize);

void make_window8(unsigned char *buf, int xsize, int ysize, char *title);


/* ********************
 *    asmfunc.asm
 * ********************/

void _io_hlt(void);
void _io_cli(void);
void _io_out8(int port, int data);
int _io_load_eflags(void);
void _io_store_eflags(int eflags);
void _load_gdtr(short t, int addr);
void _load_idtr(short t, int addr);
void _asm_inthandler20(void);
void _asm_inthandler21(void);
void _asm_inthandler27(void);
void _asm_inthandler2c(void);

int _load_cr0(void);
void _store_cr0(int cr0);
void _load_tr(int tr);
void _farjmp(int far, int pos);



/***************************
 * sprintf like function
 * tmp
 ***************************/
void itoa(char *s,int n, int max_length);
void char2hex(char *s,char c, int max_length);

/******************************
 * retrive bootinfo which stored in 0x0ff0 - 0x0ff9
 * dsctbl.c
 ******************************/
#define ADR_GDT			0x00270000
struct BOOTINFO{
	char cyls, leds, vmode, reserve;
	short scrnx, scrny;
	char *vram;
}; 
struct SEGMENT_DESCRIPTOR {
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
};

struct GATE_DESCRIPTOR {
	short offset_low, selector;
	char dw_count, access_right;
	short offset_high;
};



void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);


/**************************************
 *  int.c 
 **************************************/
#define ADR_BOOTINFO	0x00000ff0
#define PIC0_ICW1		0x0020
#define PIC0_OCW2		0x0020
#define PIC0_IMR		0x0021
#define PIC0_ICW2		0x0021
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC1_ICW1		0x00a0
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1
#define PIC1_ICW2		0x00a1
#define PIC1_ICW3		0x00a1
#define PIC1_ICW4		0x00a1
void init_pic(void);
void inthandler20(int *esp);
void inthandler21(int *esp);
void inthandler27(int *esp);
void inthandler2c(int *esp);




/***********************************
 * fifo.c
 **********************************/
#define BUF_LEN		128
struct  FIFO8 {
	unsigned char *buf;
	int p, q, size, free, flags;
};
void fifo8_init(struct FIFO8* fifo, int size, unsigned char* buf);
int fifo8_put(struct FIFO8* fifo, unsigned char data);
int fifo8_get(struct FIFO8* fifo);
int fifo8_status(struct FIFO8* fifo);


struct  FIFO32 {
	int *buf;
	int p, q, size, free, flags;
	struct TASK *task;
};
void fifo32_init(struct FIFO32* fifo, int size, int* buf, struct TASK *task);
int fifo32_put(struct FIFO32* fifo, int data);
int fifo32_get(struct FIFO32* fifo);
int fifo32_status(struct FIFO32* fifo);


/**************************
 *  keyboard.c
 **************************/
#define PORT_KEYDAT				0x0060
#define PORT_KEYSTA				0x0064
#define PORT_KEYCMD				0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE		0x60
#define KBC_MODE				0x47
void wait_KBC_sendready(void);
void init_keyboard(struct FIFO32* fifo, int data0);
static char keytable[0x54] = {
	0,0,'1','2','3','4','5','6','7','8','9','0','~','^',0,0,
	'Q','W','E','R','T','Y','U','I','O','P','@','[',0,0,'A','S',
	'D','F','G','H','J','K','L',';',':',0,0,']','Z','X','C','V',
	'B','N','M',',' , '.' ,'/',0,'*',0,' ',0,0,0,0,0,0,
	0,0,0,0,0,0,0,'7','8','9','-','4','5','6','+','1',
	'2','3','0','.'
};


/****************************
 *     mouse.c
 ****************************/
#define KEYCMD_SENDTO_MOUSE		0xd4
#define MOUSECMD_ENABLE			0xf4
struct MOUSE_DEC{
	unsigned char buf[3], phase;
	int x,y,btn;
};
void enable_mouse(struct FIFO32* fifo, int data0, struct MOUSE_DEC *mdec);
int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat);


/************************************
 *  memory info
 ************************************/
#define EFLAGS_AC_BIT		0x00040000
#define CR0_CACHE_DISABLE	0x60000000
#define MEMMAN_FREES		4090	
#define MEMMAN_ADDR			0x003c0000

// memory free location infomation
struct FREEINFO {	
	unsigned int addr, size;
};

// memory manager block
struct MEMMAN {	
	int frees, maxfrees, lostsize, losts;
	struct FREEINFO free[MEMMAN_FREES];
};

unsigned int memtest(unsigned int start, unsigned int end);
unsigned int memtest_sub(unsigned int start, unsigned int end);
void memman_init(struct MEMMAN *man);
unsigned int memman_total(struct MEMMAN *man);
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size);
int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size);
unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size);
int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size);



/*****************************
 *       SHEET
 *****************************/
#define MAX_SHEETS 	256
#define SHEET_USE 	1

/*
 *  One layer
 *  buf: point color info
 *	bxsize,bysize:  the window(layer size)
 *	vx0,vy0:  position
 *	col_inv: ?  color num
 *	height: layer height
 *	flags: set info
*/
struct SHEET {
	unsigned char *buf;
	int bxsize, bysize, vx0, vy0, col_inv, height, flags;
	struct SHTCTL *ctl;
};

/*
 * MAX_SHEETS: 256
 * vram: video ram address
 * xsize,ysize: screen size
 * top: the height of top sheet
 * sheets: the address of sheet, which sort according to height
 * sheets0: total 256 sheet 
 */
struct SHTCTL {
	unsigned char *vram;
	unsigned char *map;  // accupation
	int xsize, ysize, top;
	struct SHEET *sheets[MAX_SHEETS];
	struct SHEET sheets0[MAX_SHEETS];
};

struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned char *vram, int xsize, int ysize);
struct SHEET *sheet_alloc(struct SHTCTL *ctl);
void sheet_setbuf(struct SHEET *sht, unsigned char *buf, int xsize, int ysize, int col_inv);
void sheet_updown(struct SHEET *sht, int height);
void sheet_slide(struct SHEET *sht, int vx0, int vy0);
void sheet_free(struct SHEET *sht);
void sheet_refreshsub(struct SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1, int h0, int h1);
void sheet_refresh(struct SHEET *sht, int bx0, int by0, int bx1, int by1);
void sheet_refreshmap(struct SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1, int h0);
void  putfont8_asc_sht(struct SHEET *sht, int x, int y, int c, int b, char *s, int l);

/*
 * timer.c
 */
#define MAX_TIMER 	500
#define TIMER_FLAGS_ALLOC	1
#define TIMER_FLAGS_USING	2
struct TIMER {
	unsigned int timeout, flags;
	struct TIMER *next_timer;
	struct FIFO32 *fifo;
	int data;
};

struct TIMERCTL {
	unsigned int count, next_time;
	struct TIMER *t0;
	struct TIMER timers0[MAX_TIMER];
};

void init_pit(void);
struct TIMER *timer_alloc(void);
void timer_free(struct TIMER *timer);
void timer_init(struct TIMER *timer, struct FIFO32 *fifo, int data);
void timer_settime(struct TIMER *timer, unsigned int timeout);


void make_textbox8(struct SHEET *sht, int x0, int y0, int sx, int sy, int c);

/*
 * TSS
 */
#define MAX_TASKS 	1000
#define TASK_GDT0 	3
#define AR_TSS32		0x0089
struct TSS32 {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
};

struct TASK{
	// sel is gdt number
	int sel, flags;
	int priority;
	struct TSS32 tss;
};

struct TASKCTL {
	int running_task;
	int runnow;
	struct TASK *tasks[MAX_TASKS];
	struct TASK tasks0[MAX_TASKS];
};

struct TASKCTL *taskctl;
struct TIMER *task_timer;
struct TASK *task_alloc(void);
void task_submit(struct TASK *task, int priority);
void task_switch(void);
void task_sleep(struct TASK *task);

#endif
