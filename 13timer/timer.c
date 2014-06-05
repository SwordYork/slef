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
	timerctl.next_time = 0xffffffff;
	for (i = 0; i < MAX_TIMER; i++){
		timerctl.timers0[i].flags = 0;
	}
	return;
}

void inthandler20(int *esp){
	int i,j;
	_io_out8(PIC0_OCW2, 0x60);
	timerctl.count ++;
	if(timerctl.next_time > timerctl.count){
		return;
	}
	timerctl.next_time = 0xffffffff;
	for (i = 0; i < timerctl.using_num; i++){
		if (timerctl.timers[i]->timeout > timerctl.count){
			/* time out all done*/
			break;
		}
		timerctl.timers[i]->flags == TIMER_FLAGS_ALLOC;
		fifo32_put(timerctl.timers[i]->fifo, timerctl.timers[i]->data);
	}
	timerctl.using_num -= i;
	for( j = 0; j < timerctl.using_num; j ++ ){
		timerctl.timers[j] = timerctl.timers[i + j];
	}

	if(timerctl.using_num > 0){
		timerctl.next_time = timerctl.timers[0]->timeout;
	}
	else{
		timerctl.next_time = 0xffffffff;
	}

	return;
}

/* 
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
*/

// find a timer to be allocate
struct TIMER *timer_alloc(void){
	int i;
	for (i = 0;i < MAX_TIMER; ++i){
		if(timerctl.timers0[i].flags == 0 ){
			timerctl.timers0[i].flags = TIMER_FLAGS_ALLOC;
			return &timerctl.timers0[i];
		}
	}
	return 0x00;
}

// flag the timer
void timer_free(struct TIMER *timer){
	timer->flags = 0;
	return;
}

// pointer to memory
void timer_init(struct TIMER *timer, struct FIFO32 *fifo, int data){
	timer->fifo = fifo;
	timer->data = data;
	return;
}

// set timer out
void timer_settime(struct TIMER *timer, unsigned int timeout){
	int e,i,j;
	timer->timeout = timeout + timerctl.count;
	timer->flags = TIMER_FLAGS_USING;
	e = _io_load_eflags();
	_io_cli();

	for( i = 0; i < timerctl.using_num; ++i){
		if(timerctl.timers[i] -> timeout >= timer->timeout){
			break;
		}
	}
	for (j = timerctl.using_num;j>i;j--){
		timerctl.timers[j] = timerctl.timers[j-1];
	}

	timerctl.using_num ++;
	
	timerctl.timers[i] = timer;
	timerctl.next_time = timerctl.timers[0]->timeout;
	_io_store_eflags(e);
	return;
}


