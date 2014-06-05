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
	timerctl.using_num = 0;
	for (i = 0; i < MAX_TIMER; i++){
		timerctl.timers0[i].flags = 0;
	}
	return;
}

void inthandler20(int *esp){
	int i,j;
	struct TIMER *timer;

	_io_out8(PIC0_OCW2, 0x60);
	timerctl.count ++;
	if(timerctl.next_time > timerctl.count){
		return;
	}
	timer = timerctl.t0;
	for (i = 0; i < timerctl.using_num; i++){
		if (timer->timeout > timerctl.count){
			/* time out all done*/
			break;
		}
		timer->flags == TIMER_FLAGS_ALLOC;
		fifo32_put(timer->fifo, timer->data);
		// find last timer
		timer = timer->next_timer;
	}
	timerctl.using_num -= i;
	
	// next_timer setting
	timerctl.t0 = timer;		// the lastest timer
	if( 0 < timerctl.using_num){
		timerctl.next_time = timerctl.t0 -> timeout;
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
	int e;
	struct TIMER *t,*s;
	timer->timeout = timeout + timerctl.count;
	timer->flags = TIMER_FLAGS_USING;
	e = _io_load_eflags();
	_io_cli();
	timerctl.using_num ++;

	// only one timer
	if( 1 == timerctl.using_num){
		timerctl.t0 = timer;
		timer->next_timer = 0;
		timerctl.next_time = timer->timeout;
		_io_store_eflags(e);
		return;
	}

	t = timerctl.t0;
	if(timer->timeout < t->timeout){
		timerctl.t0 = timer;
		timer->next_timer = t;
		timerctl.next_time = timer->timeout;
		_io_store_eflags(e);
		return;
	}

	for (;;){
		s = t;
		t = t->next_timer;
		if(t == 0){
			break;
		}

		if(timer->timeout < t->timeout){
			s->next_timer = timer;
			timer->next_timer = t;
			_io_store_eflags(e);
			return;
		}
	}

	// last one
	s->next_timer = timer;
	timer->next_timer = 0;
	_io_store_eflags(e);
	return;
}


