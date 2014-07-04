#include "kernel.h"
void mt_init(void){
	mt_timer = timer_alloc();
	timer_settime(mt_timer, 2);
	mt_tr = 3 * 8;
	return;
}	

void mt_taskswitch(void){
	if(mt_tr == 3 * 8){
		mt_tr = 4 * 8;
	}
	else{
		mt_tr = 3 * 8;
	}
	timer_settime(mt_timer, 2);
	_farjmp(0, mt_tr);
	return;
}
