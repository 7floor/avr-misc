/*
 * Alarm.h
 *
 * Created: 04.03.2014 19:53:02
 *  Author: Dmitry
 */ 


#ifndef ALARM_H_
#define ALARM_H_

#define DOT_TIME 50
#define DASH_TIME (3 * DOT_TIME)

#include <stdbool.h>
#include "pt_ext.h"
extern "C" 
{
	#include "timers/timers.h"
}

typedef enum alarm_cmd {none, do_beep, do_sos};
typedef struct alarm_task {
	alarm_cmd cmd;
	uint8_t times; // 0 = until other cmd
	systime_t period;
	systime_t pulse;
};

class Alarm
{
	struct pt pt;
	stopwatch sw;	
	char i, j;
	alarm_task task;
	
	public:
	
	Alarm()
	{
		PT_INIT(&pt);
	}
	
	void beep(uint8_t times, systime_t period, systime_t pulse)
	{
		task.cmd = do_beep;
		task.times = times;
		task.period = period;
		task.pulse = pulse;
	}
	
	void morse_sos(uint8_t times)
	{
		task.cmd = do_sos;
		task.times = times;
	}
	
	void off()
	{
		task.cmd = none;
	}
	
	PT_THREAD(run())
	{
		PT_BEGIN(&pt);

		while(1)
		{
			PT_YIELD_UNTIL(&pt, task.cmd != none);
			
			if (!settings.options.sound_enabled) task.cmd = none;
			
			if (task.cmd == do_beep)
			{
				SETBIT(ALARM_O, ALARM);
				stopwatch_start(&sw);
				PT_WAIT_UNTIL(&pt, stopwatch_elapsed(&sw, task.pulse));
				CLEARBIT(ALARM_O, ALARM);
				PT_WAIT_UNTIL(&pt, stopwatch_elapsed(&sw, task.period));
			} 
			
			if (task.cmd == do_sos)
			{
				for (i = 0; i < 3; i++)
				{
					for (j = 0; j < 3; j++)
					{
						stopwatch_start(&sw);
						SETBIT(ALARM_O, ALARM);
						PT_WAIT_UNTIL(&pt, stopwatch_elapsed(&sw, i == 1 ? DASH_TIME : DOT_TIME));
						stopwatch_start(&sw);
						CLEARBIT(ALARM_O, ALARM);
						PT_WAIT_UNTIL(&pt, stopwatch_elapsed(&sw, DOT_TIME * 1)); // inside char
					}
					PT_WAIT_UNTIL(&pt, stopwatch_elapsed(&sw, DOT_TIME * 3)); // inter char
				}
				PT_WAIT_UNTIL(&pt, stopwatch_elapsed(&sw, DOT_TIME * 7)); // inter word
			}
	
			if (task.times == 1) task.cmd = none;
			if (task.times > 0) task.times--;
		}

		PT_ENDLESS(&pt);
	}
};

Alarm alarm = Alarm();

#endif /* ALARM_H_ */