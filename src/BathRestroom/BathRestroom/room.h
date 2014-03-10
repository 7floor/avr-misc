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
	uint8_t door_port_mask;
	uint8_t movement_port_mask;
	bool light;
	bool presence;

	bool dooropen();
	bool movement();
	systime_t timeout2seconds(timeout_t *t);
	void set_timer(systime_t sec);
	
	public:

	Room(room_timeouts_t *timeouts, uint8_t door_pin, uint8_t movement_pin);
	PT_THREAD(run());
	bool is_light();
};

inline
bool Room::is_light()
{
	return light;
}
inline
bool Room::dooropen()
{
	return PINA & door_port_mask;
}
inline
bool Room::movement()
{
	return PIND & movement_port_mask;
}

systime_t Room::timeout2seconds(timeout_t *t)
{
	//return t->get_seconds();
	return get_seconds(t);
}

void Room::set_timer(systime_t sec)
{
	timer_s_set(&tmr, sec);
}


Room::Room(room_timeouts_t *timeouts, uint8_t door_port_mask, uint8_t movement_port_mask)
{
	this->timeouts = timeouts;
	this->door_port_mask = door_port_mask;
	this->movement_port_mask = movement_port_mask;
	PT_INIT(&pt);
}

PT_THREAD(Room::run())
{
	PT_BEGIN(&pt);
	
	bool t, m, d;
	timeout_t *timeout;

	while(1) 
	{
		if(dooropen())
		{
			timeout = presence ? &timeouts->open_present : &timeouts->open_absent; 
			
//			timer_s_set(&tmr, get_seconds(timeout));
//			timer_s_set(&tmr, timeout2seconds(presence ? &timeouts->open_present : &timeouts->open_absent));
			set_timer(timeout2seconds(presence ? &timeouts->open_present : &timeouts->open_absent));
			PT_WAIT_UNTIL(&pt, !(d = dooropen()) || (m = movement()) || (t = timer_s_expired(&tmr)));
			if (!d) presence = false;
		}
		else
		{
			timeout = presence ? &timeouts->closed_present : &timeouts->closed_absent;

//			timer_s_set(&tmr, get_seconds(timeout));
//			timer_s_set(&tmr, timeout2seconds(presence ? &timeouts->closed_present : &timeouts->closed_absent));
			set_timer(timeout2seconds(presence ? &timeouts->closed_present : &timeouts->closed_absent));
			PT_WAIT_UNTIL(&pt, (d = dooropen()) || (m = movement()) || (t = timer_s_expired(&tmr)));
			if (d) light = true;
		}
		if(t) { light = presence = false; }
		if(m) { light = presence = true; }
	}

	PT_ENDLESS(pt);
}

Room bathroom = Room(&settings.bathroom, 1 << BR_DOOR_PIN, 1 << BR_MOVE_PIN);
Room restroom = Room(&settings.restroom, 1 << WC_DOOR_PIN, 1 << WC_MOVE_PIN);

#endif /* ROOM_H_ */