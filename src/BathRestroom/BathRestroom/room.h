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
	
	systime_t timeout;
	timeout = 0;
	
	while(1) 
	{
		timer_s_set(&tmr, timeout);

		PT_YIELD(&pt);

		if(dooropen)
		{
			PT_WAIT_UNTIL(&pt, !dooropen || movement || timer_s_expired(&tmr));
			if (!dooropen)
			{
				if(!presence) light = false;
				timeout = timeouts->closed_absent.get_seconds();
				continue;
			}
			else if(movement)
			{
				timeout = timeouts->open_present.get_seconds();
				presence = true;
				continue;
			}
		}
		else
		{
			PT_WAIT_UNTIL(&pt, dooropen || movement || timer_s_expired(&tmr));
			if (dooropen) 
			{
				light = true;
				timeout = timeouts->open_absent.get_seconds();
				continue;
			}
			else if (movement)
			{
				timeout = timeouts->closed_present.get_seconds();
				presence = true;
				continue;
			}				
		}
		light = presence = false;
		timeout = 1;
	}

	PT_ENDLESS(pt);
}

Room bathroom = Room(&settings.bathroom);
Room restroom = Room(&settings.restroom);

#endif /* ROOM_H_ */