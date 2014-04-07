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
#include "alarm.h"
extern "C" 
{
	#include "timers/timers.h"
}

class Room
{
	private:
	public:
	struct pt pt;
	timer tmr;
	room_timeouts_t *timeouts;

	bool light;
	bool presence;
	bool dooropen;
	bool movement;

	bool lastdoor;
	bool lastmove;
	
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
	bool dd, d, dm, m, t;

	timeout = 1;

	while(1) 
	{
		if (timeout) timer_s_set(&tmr, timeout);
		
		PT_YIELD_UNTIL(&pt, (d = dooropen, m = movement, dd = (d != lastdoor), dm = (m != lastmove), t = timer_s_expired(&tmr), (dd || dm || t)));

		timeout = 0;
		
		if (dd) //door change
		{
			lastdoor = d;
			if (d) // opened
			{
				light = true;
				timeout = timeouts->door_opened.get_seconds();
			}
			else // closed
			{
				if(!presence) light = false;
				presence = false;
				timeout = timeouts->door_closed.get_seconds();
			}
		}
		else if (dm) //move detector change
		{
			lastmove = m;
			if (m) // front
			{
				if (!presence) alarm.beep(1, 100, 5);
				light = presence = true;
				if (d)
				{
					timeout = timeouts->presence_open.get_seconds();
				}
				else
				{
					timeout = timeouts->presence_closed.get_seconds();
				}
			}
			else // fall, don't change timer
			{
				timeout = 0;
			}			
		}
		else // timeout
		{
			timeout = timeouts->presence_guard.get_seconds();
			if (timeout > 0 && presence)
			{
				alarm.beep(3, 200, 5);
				presence = false;
			}
			else
			{
				light = presence = false;
				timeout = 1;
			}
		}
	}

	PT_ENDLESS(pt);
}

Room bathroom = Room(&settings.bathroom);
Room restroom = Room(&settings.restroom);

#endif /* ROOM_H_ */