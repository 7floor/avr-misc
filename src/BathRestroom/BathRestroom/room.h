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
	private:
	
	struct pt pt;
	timer tmr;
	room_timeouts_t *timeouts;

	bool light;
	bool presence;
	bool dooropen;
	bool movement;

	public:

	Room(room_timeouts_t *timeouts);
	PT_THREAD(run());
	
	void set_dooropen(bool value) { dooropen = value; }
	void set_movement(bool value) { movement = value; }

	bool get_light() { return light; };
	bool get_presence() { return presence; }
	bool get_dooropen() { return dooropen; }
};


Room::Room(room_timeouts_t *timeouts)
{
	this->timeouts = timeouts;
	PT_INIT(&pt);
}

PT_THREAD(Room::run())
{
	PT_BEGIN(&pt);
	
	systime_t timeout;
	bool d, m, t;

	timeout = 1;

	while(1) 
	{
		timer_s_set(&tmr, timeout);

		if(dooropen)
		{
			PT_YIELD_UNTIL(&pt, (d = dooropen, m = movement, t = timer_s_expired(&tmr), (!d || m || t)));
			if (!d)
			{
				m = t = false;
				if(!presence) light = false;
				timeout = timeouts->closed_absent.get_seconds();
			}
			else if (m)
			{
				timeout = timeouts->open_present.get_seconds();
			}
		}
		else
		{
			PT_YIELD_UNTIL(&pt, (d = dooropen, m = movement, t = timer_s_expired(&tmr), (d || m || t)));
			if (d)
			{
				m = t = false;
				light = true;
				timeout = timeouts->open_absent.get_seconds();
			}
			else if (m)
			{
				timeout = timeouts->closed_present.get_seconds();
			}
		}
		if (m)
		{
			 light = presence = true;
		}		
		else if (t) 
		{
			light = presence = false; 
			timeout = 1;
		}
	}

	PT_ENDLESS(pt);
}

Room bathroom = Room(&settings.bathroom);
Room restroom = Room(&settings.restroom);

#endif /* ROOM_H_ */