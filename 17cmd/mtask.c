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

	for( i = 0; i < MAX_TASKLEVELS; ++i){
		taskctl->task_level[i].runnow = 0;
		taskctl->task_level[i].total_running_task = 0;
	}

	/*
	 * set the init task
	 * with pid 1
	 */
	task = task_alloc();
	task->flags = 2;
	task->priority = 2;		// 0.02 s
	task->level = 0;		// highest
	task_add_tolevel(task);
	task_switch_sub();		//level setting
	// must be 0, taskctl start from 0.
	// represent init task
	_load_tr(task->sel);
	task_timer = timer_alloc();
	timer_settime(task_timer, task->priority);
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

struct TASK *task_now(void){
	struct TASKLEVEL *tl = &taskctl->task_level[taskctl->now_lv];
	return tl->tasks[tl->runnow];
}

void task_add_tolevel(struct TASK *task){
	struct TASKLEVEL *tl = &taskctl->task_level[task->level];
	tl->tasks[tl->total_running_task] = task;
	tl->total_running_task ++;
	task->flags = 2; // active
	return;
}

void task_remove_fromlevel(struct TASK *task){
	int i;
	struct TASKLEVEL *tl = &taskctl->task_level[task->level];
	// iter to get task address
	for( i = 0; i < tl->total_running_task; ++i){
		if(tl->tasks[i] == task){
			break;
		}
	}

	tl->total_running_task --;
	if(i<tl->runnow){
		tl->runnow --;
	}
	// impossible
	if(tl->runnow >= tl->total_running_task){
		tl->runnow = 0;
	}
	task->flags = 1;
	for(; i < tl->total_running_task; i ++){
		tl->tasks[i] = tl->tasks[i + 1];
	}

	return;
}


void task_submit(struct TASK *task,int level, int priority){
	level = level < 0 ? task->level : level;  // not change level
	task->priority = priority > 0 ? priority : task->priority;
	if(task->flags == 2 && task->level != level){
		task_remove_fromlevel(task);	// flag = 1
	}	
	// wake from sleep
	// or from last step change level
	if( task->flags != 2){
		task->level = level;
		task_add_tolevel(task);
	}

	taskctl -> lv_change = 1;	// test lv in task switch

	return;
}

void task_switch_sub(void){
	int i;

	for(i = 0; i < MAX_TASKLEVELS; ++i){
		// higher will first run
		if(taskctl->task_level[i].total_running_task > 0){
			break;
		}
	}
	taskctl->now_lv = i;
	taskctl->lv_change = 0;
	return;

}

void task_switch(void){
	struct TASKLEVEL *tl = &taskctl->task_level[taskctl->now_lv];
	struct TASK *new_task;
	struct TASK *now_task = tl->tasks[tl->runnow];
	// run next task or go to head
	// inthandler20 start very early
	// may be 0
	if(tl->total_running_task != 0){
		tl->runnow = (tl->runnow + 1) % tl->total_running_task;
	}
	/*
	tl->runnow ++;
	if(tl->runnow == tl->total_running_task){
		tl->runnow = 0;
	}
	*/

	if(taskctl->lv_change != 0){
		task_switch_sub();
		tl = &taskctl->task_level[taskctl->now_lv];
	}
	new_task = tl->tasks[tl->runnow];
	// priority is running timer
	// priority higher will get more running timer 
	timer_settime(task_timer, new_task->priority);
	// jmp tss
	if(new_task != now_task){
		_farjmp(0, new_task->sel);
	}
	return;
}

void task_sleep(struct TASK *task){
	struct TASK *now_task;
	if(task -> flags == 2){
		// current task is running
		// first to switch task
		now_task = task_now();
		task_remove_fromlevel(task);
		if(task == now_task){
			// if sleep itself
			// to switch task
			task_switch_sub();
			now_task = task_now();	// get current task
			_farjmp(0, now_task->sel);
		}

		return;

	}
}
