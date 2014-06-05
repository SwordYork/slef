#include "kernel.h"

struct FIFO32 *keyfifo;
int keydata0;


void wait_KBC_sendready(void)
{
//	wait for keyboard control circuit
	for (;;) {
		if ((_io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) {
			break;
		}
	}
	return;
}

void init_keyboard(struct FIFO32* fifo, int data0)
{
	keyfifo = fifo;
	keydata0 = data0;
	wait_KBC_sendready();
	_io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	wait_KBC_sendready();
	_io_out8(PORT_KEYDAT, KBC_MODE);
	return;
}

void inthandler21(int *esp)
{
	int data;
	_io_out8(PIC0_OCW2, 0x61);
	data = _io_in8(PORT_KEYDAT);

	fifo32_put(keyfifo, data + keydata0);
 	return ;
}

 
