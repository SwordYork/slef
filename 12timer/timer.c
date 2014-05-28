#include "kernel.h" 

#define PIT_CTRL 0x0043
#define PIT_CNT0 0x0040

struct TIMERCTL timerctl; 

// bochs is 10x than qemu
void init_pit(void){
	int i;
	_io_out8(PIT_CTRL, 0x36);
	_io_out8(PIT_CNT0, 0x9c);
	_io_out8(PIT_CNT0, 0x2e);
	timerctl.count = 0;
	for (i = 0; i < MAX_TIMER; i++){
		timerctl.timer[i].flags = 0;
	}
	return;
}

void inthandler20(int *esp){
	int i;
	_io_out8(PIC0_OCW2, 0x60);
	timerctl.count ++;
	for (i = 0; i < MAX_TIMER; i++){
		if (timerctl.timer[i].flags == TIMER_FLAGS_USING){
			timerctl.timer[i].timeout --;
			if (timerctl.timer[i].timeout == 0 ){
				timerctl.timer[i].flags == TIMER_FLAGS_ALLOC;
				fifo8_put(timerctl.timer[i].fifo, timerctl.timer[i].data);
			}
		}
	}
	return;
}

void settimer(unsigned int timeout, struct FIFO8 *fifo, unsigned char data){
	int eflags;
	eflags = _io_load_eflags();
	_io_cli();
	timerctl.timeout = timeout;
	timerctl.fifo = fifo;
	timerctl.data = data;
	_io_store_eflags(eflags);
	return;
}
