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
};


Room::Room(room_timeouts_t *timeouts)
{
	this->timeouts = timeouts;
	PT_INIT(&pt);
}

PT_THREAD(Room::run())
{
	PT_BEGIN(&pt);
	
	bool d, m, t;

	while(1) 
	{
		if(dooropen)
		{
			timer_s_set(&tmr, presence ? timeouts->open_present.get_seconds() : timeouts->open_absent.get_seconds());
			PT_WAIT_UNTIL(&pt, !(d = dooropen) || (m = movement) || (t = timer_s_expired(&tmr)));
			if (!d)
			{
				if (!presence) light = false; else presence = false;
			}				
		}
		else
		{
			timer_s_set(&tmr, presence ? timeouts->closed_present.get_seconds() : timeouts->closed_absent.get_seconds());
			PT_WAIT_UNTIL(&pt, (d = dooropen) || (m = movement) || (t = timer_s_expired(&tmr)));
			if (d) light = true;
		}
		if(t) { light = presence = false; }
		if(m) { light = presence = true; }
		
		PT_YIELD(&pt);
	}

	PT_ENDLESS(pt);
}

Room bathroom = Room(&settings.bathroom);
Room restroom = Room(&settings.restroom);

#endif /* ROOM_H_ */