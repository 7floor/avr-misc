/*
 * pt_ext.h
 *
 * Created: 05.03.2014 0:33:02
 *  Author: Dmitry
 */ 


#ifndef PT_EXT_H_
#define PT_EXT_H_

#include "pt/pt.h"

typedef struct 
{
	struct pt pt;
	void* params;	
} pt_context;

typedef PT_THREAD( (*pt_thread)(pt_context *context) );

typedef struct  
{
	pt_thread thread;
	pt_context context;
} scheduler_task;

typedef struct
{
	scheduler_task (*tasks)[];
	uint8_t task_count;
} scheduler_params;

PT_THREAD(pt_scheduler(pt_context *context));
PT_THREAD(test(pt_context *context));

static scheduler_task tasks[] = {{thread: test, context: {}}};

PT_THREAD(pt_scheduler(pt_context *context))
{
	scheduler_params* params = context->params;
	static uint8_t i;

	PT_BEGIN(&context->pt);
	
	while(1)
	{
		for(i = 0; i < params->task_count; i++)
		{
			scheduler_task *task = &(*params->tasks)[i];
			task->thread(&task->context);
			PT_YIELD(&context->pt);
		}
	}
	
	PT_END(&context->pt);
}

#endif /* PT_EXT_H_ */