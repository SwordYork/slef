#include "kernel.h"

struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned char *vram, int xsize, int ysize){
	struct SHTCTL *ctl;
	int i;

	// alloc control table memory
	ctl = (struct SHTCTL *) memman_alloc_4k(memman, sizeof(struct SHTCTL));
	if( ctl == 0){
		return ctl;
	}

	// set VRAM
	ctl->vram = vram;
	ctl->xsize = xsize;
	ctl->ysize = ysize;
	ctl->top = -1;	// not sheet

	for( i = 0; i < MAX_SHEETS; ++i){
		ctl->sheets0[i].flags = 0;	// no sheet use the space
	}
	return ctl;
}

// assign a sheet in shtctl to new sheet
struct SHEET *sheet_alloc(struct SHTCTL *ctl){
	struct SHEET *sht;
	int i;
	for(i=0; i < MAX_SHEETS; i++){
		// find a empty sheet
		if(ctl->sheets0[i].flags == 0){
			sht = &ctl->sheets0[i];
			sht->flags = SHEET_USE;
			sht->height = -1;
			return sht;
		}
	}
	return 0;
}

void sheet_setbuf(struct SHEET *sht, unsigned char *buf, int xsize, int ysize, int col_inv){
	sht->buf = buf;
	sht->bxsize = xsize;
	sht->bysize = ysize;
	sht->vx0 = 0; // var initial
	sht->vy0 = 0;
	sht->col_inv = col_inv;		// transipant?
	return;
}



// adjust sheet relative position
void sheet_updown(struct SHTCTL *ctl, struct SHEET *sht, int height)
{
	int h, old = sht->height;

	if (height > ctl->top + 1) {
		height = ctl->top + 1;
	}
	if (height < -1) {
		height = -1;
	}

	// set height according to given
	sht->height = height; 

	if (old > height) {
		// the sheet become lower
		if (height >= 0) {
			for (h = old; h > height; h--) {
				// grap behind one to front
				ctl->sheets[h] = ctl->sheets[h - 1];
				ctl->sheets[h]->height = h;
			}
			ctl->sheets[height] = sht;
		} else {	
			// the sheet is hidden
			if (ctl->top > old) {
				for (h = old; h < ctl->top; h++) {
					ctl->sheets[h] = ctl->sheets[h + 1];
					ctl->sheets[h]->height = h;
				}
			}
			ctl->top--;
		}
		//refresh the origin position
		sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize);
	} else if (old < height) {	
		// the sheet become lower
		if (old >= 0) {
			for (h = old; h < height; h++) {
				ctl->sheets[h] = ctl->sheets[h + 1];
				ctl->sheets[h]->height = h;
			}
			ctl->sheets[height] = sht;
		} else {	
			for (h = ctl->top; h >= height; h--) {
				ctl->sheets[h + 1] = ctl->sheets[h];
				ctl->sheets[h + 1]->height = h + 1;
			}
			ctl->sheets[height] = sht;
			ctl->top++; 
		}
		sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize);
	}
	return;
}

// refresh background and char
void sheet_refresh(struct SHTCTL *ctl, struct SHEET *sht, int bx0, int by0, int bx1, int by1)
{
	if (sht->height >= 0) { 
		sheet_refreshsub(ctl, sht->vx0 + bx0, sht->vy0 + by0, sht->vx0 + bx1, sht->vy0 + by1);
	}
	return;
}


void sheet_slide(struct SHTCTL *ctl, struct SHEET *sht, int vx0, int vy0){
	int old_vx0 = sht->vx0, old_vy0 = sht->vy0;	
	sht->vx0 = vx0;
	sht->vy0 = vy0;

	// refresh only in vx0~vx1, vy0~vy1
	// two step:
	// 1. move origin position layer
	// 2. add new position layer
	if(sht->height >= 0){
		sheet_refreshsub(ctl, old_vx0, old_vy0, old_vx0 + sht->bxsize, old_vy0 + sht->bysize);
		sheet_refreshsub(ctl, vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize);

	}
	return;
}


void sheet_free(struct SHTCTL *ctl, struct SHEET *sht){
	if(sht->height >= 0){
		sheet_updown(ctl, sht, -1);		// hide this sheet
	}
	sht->flags = 0;					// indicate not using
	return ;
}


void sheet_refreshsub(struct SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1)
{
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
	unsigned char *buf, c, *vram = ctl->vram;
	struct SHEET *sht;
	// from bottom to top
	// draw sheets
	for (h = 0; h <= ctl->top; h++) {
		sht = ctl->sheets[h];
		buf = sht->buf;
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;

		// detect region
		if (bx0 < 0) { bx0 = 0; }
		if (by0 < 0) { by0 = 0; }
		if (bx1 > sht->bxsize) { bx1 = sht->bxsize; }
		if (by1 > sht->bysize) { by1 = sht->bysize; }
		for (by = by0; by < by1; by++) {
			vy = sht->vy0 + by;
			for (bx = bx0; bx < bx1; bx++) {
				vx = sht->vx0 + bx;
				c = buf[by * sht->bxsize + bx];	// the corresponding layer color
				if (c != sht->col_inv) {		// visible , then set color
					vram[vy * ctl->xsize + vx] = c;
				}
			}
		}
	}
	return;
}
