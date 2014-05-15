#include "kernel.h"

#define MAX_LENGTH 		40

#define EFLAGS_AC_BIT		0x00040000
#define CR0_CACHE_DISABLE	0x60000000
#define MEMMAN_FREES		4090	
#define MEMMAN_ADDR			0x003c0000

struct FREEINFO {	
	unsigned int addr, size;
};

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
void memman_init(struct MEMMAN *man)
{
	man->frees = 0;			
	man->maxfrees = 0;		
	man->lostsize = 0;		
	man->losts = 0;			
	return;
}

unsigned int memman_total(struct MEMMAN *man)
{
	unsigned int i, t = 0;
	for (i = 0; i < man->frees; i++) {
		t += man->free[i].size;
	}
	return t;
}

unsigned int memman_alloc(struct MEMMAN *man, unsigned int size)
{
	unsigned int i, a;
	for (i = 0; i < man->frees; i++) {
		if (man->free[i].size >= size) {
			a = man->free[i].addr;
			man->free[i].addr += size;
			man->free[i].size -= size;
			if (man->free[i].size == 0) {
				man->frees--;
				for (; i < man->frees; i++) {
					man->free[i] = man->free[i + 1]; 
				}
			}
			return a;
		}
	}
	return 0; 
}

int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size)
{
	int i, j;
	for (i = 0; i < man->frees; i++) {
		if (man->free[i].addr > addr) {
			break;
		}
	}
	/* free[i - 1].addr < addr < free[i].addr */
	if (i > 0) {
		if (man->free[i - 1].addr + man->free[i - 1].size == addr) {
			man->free[i - 1].size += size;
			if (i < man->frees) {
				if (addr + size == man->free[i].addr) {
					man->free[i - 1].size += man->free[i].size;
					man->frees--;
					for (; i < man->frees; i++) {
						man->free[i] = man->free[i + 1];
					}
				}
			}
			return 0;
		}
	}
	if (i < man->frees) {
		if (addr + size == man->free[i].addr) {
			man->free[i].addr = addr;
			man->free[i].size += size;
			return 0;
		}
	}
	if (man->frees < MEMMAN_FREES) {
		for (j = man->frees; j > i; j--) {
			man->free[j] = man->free[j - 1];
		}
		man->frees++;
		if (man->maxfrees < man->frees) {
			man->maxfrees = man->frees; 
		}
		man->free[i].addr = addr;
		man->free[i].size = size;
		return 0;
	}
	man->losts++;
	man->lostsize += size;
	return -1;
}

unsigned int memtest(unsigned int start, unsigned int end){
	char flg486 = 0;
	unsigned int eflg, cr0, i;

	/* detect 486 */
	eflg = _io_load_eflags();
	eflg |= EFLAGS_AC_BIT;
	_io_store_eflags(eflg);
	eflg = _io_load_eflags();
	if ((eflg & EFLAGS_AC_BIT) != 0){
		flg486 = 1;
	}
	eflg &= ~EFLAGS_AC_BIT; // AC-bit = 0
	_io_store_eflags(eflg);

	if (flg486 != 0){
		cr0 = _load_cr0();
		cr0 |= CR0_CACHE_DISABLE; // disable cache
		_store_cr0(cr0);
	}

	i = memtest_sub(start,end);

	if(flg486 != 0){
		cr0 = _load_cr0();
		cr0 &= ~ CR0_CACHE_DISABLE;  // enable cache
		_store_cr0(cr0);
	}

	return i;

}

unsigned int memtest_sub(unsigned int start, unsigned int end){
	unsigned int i, *p, old, pat0 = 0xaa55aa55, pat1 = 0x55aa55aa;

	for(i = start;i<=end;i+= 0x1000){
		p = (unsigned int *) (i + 0xffc);  // test last 4 bytes
		old = *p;
		*p = pat0;
		*p ^= 0xffffffff;
		if( *p != pat1 ){
not_memory:
			*p = old;
			break;
		}
		*p ^= 0xffffffff;

		if (*p != pat0){
			goto not_memory;
		}
		*p = old;
	}
	return i;
}





extern struct FIFO8 keyfifo;
extern struct FIFO8 mousefifo;


void main(){
	struct BOOTINFO *binfo;
	binfo = (struct BOOTINFO *) ADR_BOOTINFO;

	char s[MAX_LENGTH];
	char mcursor[256];
	char keybuf[KEYBUF_LEN];
	char mousebuf[MOUSEBUF_LEN];
	unsigned char key_data;

	struct MOUSE_DEC mdec;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	unsigned int memtotal;


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
	memtotal = memtest(0x00400000, 0xbfffffff);
	memman_init(memman);
	memman_free(memman, 0x00001000, 0x0009e000); /* 0x00001000 - 0x0009efff */
	memman_free(memman, 0x00400000, memtotal - 0x00400000);

	itoa(s,memtotal / 1024 /1024,MAX_LENGTH);
	putfont8_asc(binfo->vram, binfo->scrnx, 0, 32, COL8_FFFFFF, s);

	itoa(s,memman_total(memman) / 1024,MAX_LENGTH);
	putfont8_asc(binfo->vram, binfo->scrnx, 0, 64, COL8_FFFFFF, s);


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
