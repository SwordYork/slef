void _io_hlt(void);
void _io_cli(void);
void _io_out8(int port, int data);
int _io_load_eflags(void);
void _io_store_eflags(int eflags);

//void _write_mem8(int addr, int data);

void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);


void main(){
	//char* video_memory = (char*) 0xa0000;
	//	*video_memory = 'X';
	int i;
	char *p;		// char* p,q <===> char *p,q
	init_palette();

	p = (char *) 0xa0000;


//	for (i = 0xa0000;i<0xaffff;i++){
	for (i = 0; i <= 0xffff; i++){
//		p = (char *) i;			
//	    *(p + i) = i & 0x1f;
	    i[p] = i & 0x0f;		//2[p] all ok!!!!!!!!!!!!!!
//		p[i] = i & 0x2f;
		//_write_mem8(i,i & 0x0f);
	}
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


