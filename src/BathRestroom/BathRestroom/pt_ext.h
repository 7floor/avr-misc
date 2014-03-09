/*
 * pt_ext.h
 *
 * Created: 05.03.2014 0:33:02
 *  Author: Dmitry
 */ 


#ifndef PT_EXT_H_
#define PT_EXT_H_

#include <stddef.h>
//#define LC_INCLUDE "lc-addrlabels.h"
#include "pt/pt.h"

#define PT_ENDLESS(pt) LC_END((pt)->lc); return 0; }

//to do: think about scheduler for classes

// cpp

/*
class Task
{
	protected:
	struct pt pt;
	
	public:
	Task()
	{
		PT_INIT(&pt);
	}
	virtual PT_THREAD(run());
};
*/

// c

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

static PT_THREAD(scheduler(struct pt *pt, scheduler_context *context));

//PT_THREAD(test(struct pt *pt, void *params));
//static pt_task tasks[] = {{thread: test}, {thread: scheduler}};

static void init_tasks(pt_task *task, uint8_t count)
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


static void init_tasks2(pt_task tasks[], uint8_t count)
{
	for(uint8_t i = 0; i < count; i++)
	{
		PT_INIT(&tasks[i].pt);
	}
}



PT_THREAD(scheduler(struct pt *pt, scheduler_context *context))
{
	PT_BEGIN(pt);

	while(1)
	{
		for(context->task_index = 0; context->task_index < context->task_count; context->task_index++)
		{
			//pt_task *task = &(*context->tasks)[context->task_index];
			//if (task->status < PT_EXITED)
			//{
			//	task->status = PT_SCHEDULE(task->thread(&task->pt, task->context));
			//}
			PT_YIELD(pt);
		}
		PT_YIELD(pt);
	}
	
	PT_ENDLESS(pt);
}


#endif /* PT_EXT_H_ */