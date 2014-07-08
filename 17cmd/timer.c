#include "kernel.h" 

#define PIT_CTRL 0x0043
#define PIT_CNT0 0x0040

struct TIMERCTL timerctl; 

// bochs is 10x than qemu
void init_pit(void){
	int i;
	struct TIMER *t;

	_io_out8(PIT_CTRL, 0x36);
	_io_out8(PIT_CNT0, 0x9c);
	_io_out8(PIT_CNT0, 0x2e);
	timerctl.count = 0;
	timerctl.next_time = 0xffffffff;
	for (i = 0; i < MAX_TIMER; i++){
		timerctl.timers0[i].flags = 0;
	}
	t = timer_alloc();
	t->timeout = 0xffffffff;
	t->flags = TIMER_FLAGS_USING;
	t->next_timer = 0x00;
	timerctl.t0 = t;
	timerctl.next_time = 0xffffffff;
	return;
}

void inthandler20(int *esp){
	char ts = 0;
	int i,j;
	struct TIMER *timer;

	_io_out8(PIC0_OCW2, 0x60);
	timerctl.count ++;
	if(timerctl.next_time > timerctl.count){
		return;
	}
	timer = timerctl.t0;
	for (;;){
		if (timer->timeout > timerctl.count){
			/* time out all done*/
			break;
		}
		timer->flags == TIMER_FLAGS_ALLOC;
		if(timer != task_timer){
			fifo32_put(timer->fifo, timer->data);
		}else{
			ts = 1;
		}
		// find last timer
		timer = timer->next_timer;
	}
	
	// next_timer setting
	timerctl.t0 = timer;		// the lastest timer
	timerctl.next_time = timerctl.t0 -> timeout;

	if(ts != 0){
		task_switch();
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
}


