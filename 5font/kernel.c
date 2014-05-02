#include "hankaku.h"

void _io_hlt(void);
void _io_cli(void);
void _io_out8(int port, int data);
int _io_load_eflags(void);
void _io_store_eflags(int eflags);


void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void init_screen(char *vram, int xsize, int ysize);
void putfont8(char *vram, int xsize, int x, int y, char color, char *font);
void putfont8_asc(char *vram, int xsize, int x, int y, char color, unsigned char *s);
void itoa(char *s,int n, int max_length);


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


#define MAX_LENGTH 		40
/*
 * retrive bootinfo which stored in 0x0ff0 - 0x0ff9
 */
struct BOOTINFO{
	char cyls, leds, vmode, reserve;
	short scrnx, scrny;
	char *vram;
};



extern char hankaku[4096];
void main(){
	struct BOOTINFO *binfo;
	binfo = (struct BOOTINFO *) 0x0ff0;
	/*  
	static char font_A[16] = {
		0x00, 0x18, 0x18, 0x18, 0x18, 0x24, 0x24, 0x24,
		0x24, 0x7e, 0x42, 0x42, 0x42, 0xe7, 0x00, 0x00
	};
	*/

	char s[MAX_LENGTH];
	
	init_palette();
	init_screen(binfo->vram, binfo->scrnx, binfo->scrny);
	putfont8_asc(binfo->vram, binfo->scrnx, 8, 8, COL8_FFFFFF,"hello slef!");

//	sprintf(s,"scrnx = %d", binfo->scrnx);
	itoa(s,binfo->scrnx,MAX_LENGTH);
	putfont8_asc(binfo->vram, binfo->scrnx, 8, 64, COL8_FFFFFF, s);

	itoa(s,binfo->scrny,MAX_LENGTH);
	putfont8_asc(binfo->vram, binfo->scrnx, 8, 80, COL8_FFFFFF, s);

	for(;;){
		_io_hlt();
	}
}	


/*
 * color setting
 */
void init_palette(void)
{
	static unsigned char table_rgb[16 * 3] = {
		0x00, 0x00, 0x00,	/*  0: 黑  */
		0xff, 0x00, 0x00,	/*  1: 亮红*/
		0x00, 0xff, 0x00,	/*  2: 亮绿*/
		0xff, 0xff, 0x00,	/*  3: 亮黄*/
		0x00, 0x00, 0xff,	/*  4: 亮蓝*/
		0xff, 0x00, 0xff,	/*  5: 亮紫*/
		0x00, 0xff, 0xff,	/*  6: 浅紫*/
		0xff, 0xff, 0xff,	/*  7: 白  */
		0xc6, 0xc6, 0xc6,	/*  8: 亮灰*/
		0x84, 0x00, 0x00,	/*  9: 暗红*/
		0x00, 0x84, 0x00,	/* 10: 暗绿*/
		0x84, 0x84, 0x00,	/* 11: 暗黄*/
		0x00, 0x00, 0x84,	/* 12: 暗青*/
		0x84, 0x00, 0x84,	/* 13: 暗紫*/
		0x00, 0x84, 0x84,	/* 14: 浅蓝*/
		0x84, 0x84, 0x84	/* 15: 暗灰*/
	};
	set_palette(0, 15, table_rgb);
	return;

}


/*
 * 	set color value
 * 	use 16 color
 */

void set_palette(int start, int end, unsigned char *rgb)
{
	int i, eflags;
	eflags = _io_load_eflags();
	_io_cli(); 				
	_io_out8(0x03c8, start);
	for (i = start; i <= end; i++) {
		_io_out8(0x03c9, rgb[0] / 4);
		_io_out8(0x03c9, rgb[1] / 4);
		_io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	_io_store_eflags(eflags);	
	return;
}

/*
 *	draw box
 */

void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1)
{
	int x,y;
	for (y = y0; y <= y1; ++y) {
		for (x = x0; x <= x1; ++x){
			vram[y * xsize + x] = c;
		}
	}
	return;
}

/*
 * draw task bar
 */

void init_screen(char *vram, int xsize, int ysize){

	boxfill8(vram, xsize, COL8_008484,  0,         0,          xsize -  1, ysize - 29);
	boxfill8(vram, xsize, COL8_C6C6C6,  0,         ysize - 28, xsize -  1, ysize - 28);
	boxfill8(vram, xsize, COL8_FFFFFF,  0,         ysize - 27, xsize -  1, ysize - 27);
	boxfill8(vram, xsize, COL8_C6C6C6,  0,         ysize - 26, xsize -  1, ysize -  1);

	boxfill8(vram, xsize, COL8_FFFFFF,  3,         ysize - 24, 59,         ysize - 24);
	boxfill8(vram, xsize, COL8_FFFFFF,  2,         ysize - 24,  2,         ysize -  4);
	boxfill8(vram, xsize, COL8_848484,  3,         ysize -  4, 59,         ysize -  4);
	boxfill8(vram, xsize, COL8_848484, 59,         ysize - 23, 59,         ysize -  5);
	boxfill8(vram, xsize, COL8_000000,  2,         ysize -  3, 59,         ysize -  3);
	boxfill8(vram, xsize, COL8_000000, 60,         ysize - 24, 60,         ysize -  3);

	boxfill8(vram, xsize, COL8_848484, xsize - 47, ysize - 24, xsize -  4, ysize - 24);
	boxfill8(vram, xsize, COL8_848484, xsize - 47, ysize - 23, xsize - 47, ysize -  4);
	boxfill8(vram, xsize, COL8_FFFFFF, xsize - 47, ysize -  3, xsize -  4, ysize -  3);
	boxfill8(vram, xsize, COL8_FFFFFF, xsize -  3, ysize - 24, xsize -  3, ysize -  3);

	return;
}

/*
 *  print single char
 *  with color c
 *  font matrix font
 */
void putfont8(char *vram, int xsize, int x, int y, char c, char *font)
{
	int i;
	char *p, d /* data */;
	for (i = 0; i < 16; i++) {
		p = vram + (y + i) * xsize + x;
		d = font[i];
		if ((d & 0x80) != 0) { p[0] = c; }
		if ((d & 0x40) != 0) { p[1] = c; }
		if ((d & 0x20) != 0) { p[2] = c; }
		if ((d & 0x10) != 0) { p[3] = c; }
		if ((d & 0x08) != 0) { p[4] = c; }
		if ((d & 0x04) != 0) { p[5] = c; }
		if ((d & 0x02) != 0) { p[6] = c; }
		if ((d & 0x01) != 0) { p[7] = c; }
	}
	return;
}

/*
 * 	print string to screen
 * 	string last byte is 0x00
 */
void putfont8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s)
{
	for(; *s != 0x00; s++){
		putfont8(vram, xsize, x, y, c, hankaku + *s * 16);
		x += 8;
	}
	return;
}

/*
 *	n >= 0
 */
void itoa(char *s,int n, int max_length){
	if (n == 0){
		s[0] = '0';
		s[1] = '\0';
	}

	int pos = 0;
	int m = n;
	while(m != 0){
		++ pos;
		m /= 10;
	}

	if(pos >= max_length){
		s[0] = '\0';
		return;
	}

	s[pos] = '\0';
	--pos;
	while( n != 0){
		s[pos--] = '0' + ( n % 10);
		n /= 10;
	}

	return;
}
