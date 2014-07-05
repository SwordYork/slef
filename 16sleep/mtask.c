#include "kernel.h"


struct TASK *task_init(struct MEMMAN *memman){
	int i;
	struct TASK *task;
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
	taskctl = (struct TASKCTL *) memman_alloc_4k(memman, sizeof(struct TASKCTL));
	for( i=0; i<MAX_TASKS; i++){
		taskctl->tasks0[i].flags = 0;
		taskctl->tasks0[i].sel = (TASK_GDT0 + i) * 8;
		set_segmdesc(gdt + TASK_GDT0 + i, 103, (int) &(taskctl->tasks0[i]).tss, AR_TSS32);
	}

	/*
	 * set the init task
	 * with pid 1
	 */
	task = task_alloc();
	task->flags = 2;
	taskctl->running_task = 1;
	taskctl->runnow = 0;
	taskctl->tasks[0] = task;
	_load_tr(task->sel);
	task_timer = timer_alloc();
	timer_settime(task_timer, 2);
	return task;
}

struct TASK *task_alloc(void){
	int i;
	struct TASK *task;
	// alloc a new task
	for (i = 0; i < MAX_TASKS; ++i){
		if(taskctl->tasks0[i].flags == 0){
			task = &taskctl->tasks0[i];
			task->flags = 1;
			task->tss.eflags = 0x00000202;
			task->tss.eax = 0;
			task->tss.ecx = 0;
			task->tss.edx = 0;
			task->tss.ebx = 0;
			task->tss.ebp = 0;
			task->tss.esi = 0;
			task->tss.edi = 0;
			task->tss.es = 0;
			task->tss.ds = 0;
			task->tss.fs = 0;
			task->tss.gs = 0;
			task->tss.ldtr = 0;
			task->tss.iomap = 0x40000000;
			return task;

		}
	}

	return 0; // no empty
}


void task_submit(struct TASK *task){
	task->flags = 2;
	taskctl->tasks[taskctl->running_task] = task;
	taskctl->running_task ++;
	return;
}

void task_switch(void){
	timer_settime(task_timer, 2);
	if(taskctl->running_task >= 2){
		// run next task or go to head
		taskctl->runnow = (taskctl->runnow + 1) % taskctl->running_task;
		// jmp tss
		_farjmp(0, taskctl->tasks[taskctl->runnow]->sel);
	}
	return;
}
