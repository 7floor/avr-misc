/*
 * pt_ext.h
 *
 * Created: 05.03.2014 0:33:02
 *  Author: Dmitry
 */ 


#ifndef PT_EXT_H_
#define PT_EXT_H_

#include "pt/pt.h"

typedef PT_THREAD((*pt_thread)(struct pt *pt, void *context));

typedef struct  
{
	struct pt pt;
	pt_thread thread;
	void *context;
	char status;
} pt_task;

typedef struct
{
	pt_task (*tasks)[];
	uint8_t task_count;
	uint8_t task_index;
} scheduler_context;

PT_THREAD(scheduler(struct pt *pt, scheduler_context *context));

//PT_THREAD(test(struct pt *pt, void *params));
//static pt_task tasks[] = {{thread: test}, {thread: scheduler}};

void init_tasks(pt_task *task, uint8_t count)
{
	uint8_t i = count;
	while (i > 0)
	{
		i--;
		PT_INIT(&task->pt);
		task->status = PT_WAITING; 
		task++;
	}
}

/*
void init_tasks2(scheduler_task tasks[], uint8_t count)
{
	for(uint8_t i = 0; i < count; i++)
	{
		PT_INIT(&tasks[i].pt);
	}
}
*/

PT_THREAD(scheduler(struct pt *pt, scheduler_context *context))
{
	PT_BEGIN(pt);

	while(1)
	{
		for(context->task_index = 0; context->task_index < context->task_count; context->task_index++)
		{
			pt_task *task = &(*context->tasks)[context->task_index];
			if (task->status < PT_EXITED)
			{
				task->status = PT_SCHEDULE(task->thread(&task->pt, task->context));
			}
			PT_YIELD(pt);
		}
		PT_YIELD(pt);
	}
	
	PT_END(pt);
}

#endif /* PT_EXT_H_ */