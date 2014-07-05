#include "kernel.h"
#define PORT_KEYDAT		0x0060



struct FIFO8 keyfifo;
struct FIFO8 mousefifo;

void init_pic(void)
{
	_io_out8(PIC0_IMR,  0xff  ); 
	_io_out8(PIC1_IMR,  0xff  ); 

	_io_out8(PIC0_ICW1, 0x11  ); 
	_io_out8(PIC0_ICW2, 0x20  ); 
	_io_out8(PIC0_ICW3, 1 << 2); 
	_io_out8(PIC0_ICW4, 0x01  ); 

	_io_out8(PIC1_ICW1, 0x11  ); 
	_io_out8(PIC1_ICW2, 0x28  ); 
	_io_out8(PIC1_ICW3, 2     ); 
	_io_out8(PIC1_ICW4, 0x01  ); 

	_io_out8(PIC0_IMR,  0xfb  ); 
	_io_out8(PIC1_IMR,  0xff  ); 

	return;
}


void inthandler27(int *esp)
{
	_io_out8(PIC0_OCW2, 0x67); 
	return;
}
