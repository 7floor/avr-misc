/*
 * Alarm.h
 *
 * Created: 04.03.2014 19:53:02
 *  Author: Dmitry
 */ 


#ifndef ALARM_H_
#define ALARM_H_

#define MORSE_CODE_SOS
#define DOT_TIME 50
#define DASH_TIME (3 * DOT_TIME)

#include <stdbool.h>
#include "pt_ext.h"
extern "C" 
{
	#include "timers/timers.h"
}

class Alarm
{
	struct pt pt;
	stopwatch sw;	
	char i, j;

	public:
	
	Alarm()
	{
		PT_INIT(&pt);
	}
	
	PT_THREAD(run())
	{
		PT_BEGIN(&pt);
	
		while(1)
		{
			PT_WAIT_UNTIL(&pt, settings.alarm && (!CHECKBIT(LEAK_I, LEAK_BR_I) || !CHECKBIT(LEAK_I, LEAK_RR_I)));
		
			#ifdef MORSE_CODE_SOS
		
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
		
			#else

			stopwatch_start(&sw);
			SETBIT(ALARM_O, ALARM);
			PT_WAIT_UNTIL(&pt, stopwatch_elapsed(&sw, 250));
			CLEARBIT(ALARM_O, ALARM);
			PT_WAIT_UNTIL(&pt, stopwatch_elapsed(&sw, 500));
		
			#endif
	
			PT_YIELD(&pt);
		}
	
		PT_ENDLESS(&pt);
	}
};

Alarm alarm = Alarm();

#endif /* ALARM_H_ */