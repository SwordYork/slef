void _io_hlt(void);
void _io_cli(void);
void _io_out8(int port, int data);
int _io_load_eflags(void);
void _io_store_eflags(int eflags);

//void _write_mem8(int addr, int data);

void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);

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


void main(){
	//char* video_memory = (char*) 0xa0000;
	//	*video_memory = 'X';
	init_palette();

	/*  
	int i;
	char *p;		// char* p,q <===> char *p,q
	p = (char *) 0xa0000;

	boxfill8(p, 320, COL8_FF0000, 20, 20, 120, 120);
	boxfill8(p, 320, COL8_00FF00, 70, 50, 170, 150);
	boxfill8(p, 320, COL8_0000FF, 120, 80, 220, 180);
	
	*/
	char *vram;
	int xsize,ysize;
	vram = (char *) 0xa0000;
	xsize = 320;
	ysize = 200;

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


	for(;;){
		_io_hlt();
	}
}	


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
