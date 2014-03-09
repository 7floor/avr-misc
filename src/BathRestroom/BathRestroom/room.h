/*
 * room.h
 *
 * Created: 09.03.2014 0:30:42
 *  Author: Dmitry
 */ 


#ifndef ROOM_H_
#define ROOM_H_

#include <stdint.h>
#include "pt_ext.h"
#include "settings.h"
extern "C" 
{
	#include "timers/timers.h"
}

class Room
{
	bool presence;
	struct pt pt;
	timer tmr;
	room_timeouts_t *timeouts;

	bool dooropen()
	{
		return true;
	}

	bool movement()
	{
		return true;
	}

	systime_t timeout2seconds(timeout_t *t)
	{
		return minsec2sec(t->min, t->sec);
	}
	
	public:

	bool light;
	
	Room(room_timeouts_t *timeouts)
	{
		this->timeouts = timeouts;
		PT_INIT(&pt);
	}
	
	PT_THREAD(run())
	{
		PT_BEGIN(&pt);
	
		bool t, m, d;

		while(1)
		{
			if(dooropen())
			{
				timer_s_set(&tmr, timeout2seconds(presence ? &timeouts->open_present : &timeouts->open_absent));
				PT_WAIT_UNTIL(&pt, !(d = dooropen()) || (m = movement()) || (t = timer_s_expired(&tmr)));
			}
			else
			{
				timer_s_set(&tmr, timeout2seconds(presence ? &timeouts->closed_present : &timeouts->closed_absent));
				PT_WAIT_UNTIL(&pt, (d = dooropen()) || (m = movement()) || (t = timer_s_expired(&tmr)));
			}
			light = d;
			if(t) { light = presence = false; }
			if(m) { light = presence = true; }
			//light(l); maybe yield for light contol?
		}
	
		PT_ENDLESS(pt);
	}
	
};

static Room bathroom = Room(&settings.bathroom);
static Room restroom = Room(&settings.restroom);

#endif /* ROOM_H_ */