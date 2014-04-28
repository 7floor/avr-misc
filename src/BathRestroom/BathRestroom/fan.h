/*
 * fan.h
 *
 * Created: 16.03.2014 17:50:22
 *  Author: Dmitry
 */ 


#ifndef FAN_H_
#define FAN_H_

#include <stdint.h>
#include "pt_ext.h"
#include "settings.h"
extern "C"
{
	#include "timers/timers.h"
}

class Fan
{
	private:
	
	struct pt pt;
	timer tmr;
	bool active;

	bool restroom_occupied() { return restroom.get_light() && !restroom.get_dooropen(); }
	public:

	Fan();
	PT_THREAD(run());
	
	bool get_active() { return active; };
};


Fan::Fan()
{
	PT_INIT(&pt);
}

/*
on presence detected, start timer min_resence
when timer expired 
  start fan and when presence ended, continue fan for timeout
*/

PT_THREAD(Fan::run())
{
	PT_BEGIN(&pt);
	
	bool t;
	
	while(1) 
	{
		PT_WAIT_UNTIL(&pt, restroom_occupied());
		timer_s_set(&tmr, settings.fan.min_presence.get_seconds());
		PT_WAIT_UNTIL(&pt, (t = timer_s_expired(&tmr), (t || !restroom_occupied())));
		if (t)
		{
			active = true;
			PT_WAIT_UNTIL(&pt, !restroom_occupied());
			timer_s_set(&tmr, settings.fan.duration.get_seconds());
			PT_WAIT_UNTIL(&pt, timer_s_expired(&tmr));
			active = false;
		}
	}

	PT_ENDLESS(pt);
}

Fan fan = Fan();

#endif /* FAN_H_ */