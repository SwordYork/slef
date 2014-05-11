#include "kernel.h" 

#define ADR_IDT			0x0026f800
#define LIMIT_IDT		0x000007ff
#define ADR_GDT			0x00270000
#define LIMIT_GDT		0x0000ffff
#define ADR_KERNEL		0x00000000
//#define LIMIT_KERNEL	0xffffffff
#define LIMIT_KERNEL	0x0007ffff
#define AR_DATA32_RW	0x4092
#define AR_CODE32_ER	0x409a
#define AR_INTGATE32	0x008e


void init_gdtidt(void)
{
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
	struct GATE_DESCRIPTOR    *idt = (struct GATE_DESCRIPTOR  *) ADR_IDT;
	int i;

	for (i = 0; i <= LIMIT_GDT / 8 ; i++) {
		set_segmdesc(gdt + i, 0, 0, 0);
	}
	set_segmdesc(gdt + 1, LIMIT_KERNEL, ADR_KERNEL, AR_CODE32_ER);
	set_segmdesc(gdt + 2, 0xffffffff, 0x00000000, AR_DATA32_RW);
	//set_segmdesc(gdt + 2, 0x0007ffff, 0x00280000, 0x409a);
	_load_gdtr(LIMIT_GDT, ADR_GDT);

	for (i = 0; i <= LIMIT_IDT / 8; i++) {
		set_gatedesc(idt + i, 0, 0, 0);
	}
	_load_idtr(LIMIT_IDT, ADR_IDT);

	set_gatedesc(idt + 0x21, (int) _asm_inthandler21, 1 * 8, AR_INTGATE32);
	set_gatedesc(idt + 0x27, (int) _asm_inthandler27, 1 * 8, AR_INTGATE32);
	set_gatedesc(idt + 0x2c, (int) _asm_inthandler2c, 1 * 8, AR_INTGATE32);

	return;
}

void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar)
{
	if (limit > 0xfffff) {
		ar |= 0x8000; /* G_bit = 1 */
		limit /= 0x1000;
	}
	sd->limit_low    = limit & 0xffff;
	sd->base_low     = base & 0xffff;
	sd->base_mid     = (base >> 16) & 0xff;
	sd->access_right = ar & 0xff;
	sd->limit_high   = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
	sd->base_high    = (base >> 24) & 0xff;
	return;
}

void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar)
{
	gd->offset_low   = offset & 0xffff;
	gd->selector     = selector;
	gd->dw_count     = (ar >> 8) & 0xff;
	gd->access_right = ar & 0xff;
	gd->offset_high  = (offset >> 16) & 0xffff;
	return;
}
