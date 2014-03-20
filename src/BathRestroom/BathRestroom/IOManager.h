/*
* IOManager.h
*
* Created: 14.01.2014 20:45:00
*  Author: Dmitry
*/


#ifndef IOMANAGER_H_
#define IOMANAGER_H_

#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "pt_ext.h"
#include "settings.h"
#include "room.h"
#include "fan.h"
#include "alarm.h"

typedef union
{
	uint16_t byte;
	struct
	{
		uint8_t addr:7;
		bool write:1;
	} cmd_addr;
} byte_cmd_addr;

class IOManager
{
	struct pt pt;
	struct pt pt1w;
	byte_cmd_addr addr;

	bool read_1w_byte(byte_cmd_addr& value);
	void read_sensors();
	void set_control();
	void out_port();
	
	public:
	
	IOManager();
	
	PT_THREAD(run());
	PT_THREAD(run1w());
	
	void process_external_io();	
};

IOManager::IOManager()
{
	addr.byte = 0;
	PT_INIT(&pt);
	PT_INIT(&pt1w);
	MCUCR |= (1 << ISC11) | (0 << ISC10); // falling edge on int1 (pd3) will trigger
}

PT_THREAD(IOManager::run())
{
	PT_BEGIN(&pt);

	while(1)
	{
		PT_SCHEDULE(run1w());
		set_control();
		read_sensors();
		out_port();
		PT_YIELD(&pt);
	}
	
	PT_ENDLESS(&pt);
}

PT_THREAD(IOManager::run1w())
{
	byte_cmd_addr bca;
	
	PT_BEGIN(&pt1w);
	
	while(1)
	{
		do
		{
			PT_WAIT_UNTIL(&pt1w, read_1w_byte(bca));
		} while (bca.byte != 0xff);
		addr = bca;
		
		PT_WAIT_UNTIL(&pt1w, read_1w_byte(bca));
		if (addr.cmd_addr.write)
		{
			settings_set(addr.cmd_addr.addr, bca.byte);
		}
	}
	
	PT_ENDLESS(&pt1w);
}
	
bool IOManager::read_1w_byte(byte_cmd_addr& value)
{
	if (EIFR & (1 << INTF1))
	{
		EIFR |= (1 << INTF1);
		IO_D = 0; // all inputs
		IO_O = 0x0ff; // all with pull-ups
		value.byte = IO_I;
		return true;
	}
	return false;
}

void IOManager::read_sensors()
{
	bathroom.set_dooropen(CHECKBIT(DOORS_I, DOOR_BR));
	restroom.set_dooropen(CHECKBIT(DOORS_I, DOOR_RR));
	bathroom.set_movement(CHECKBIT(MOVE_I, MOVE_BR));
	restroom.set_movement(CHECKBIT(MOVE_I, MOVE_RR));
}
	
void IOManager::set_control()
{
	uint8_t b = settings.control;
	
	if (CHECKBIT(b, LIGHT_OVER_BR))
	{
		if (bathroom.get_light()) CLEARBIT(b, LIGHT_BR); else SETBIT(b, LIGHT_BR);
	}
	
	if (CHECKBIT(b, LIGHT_OVER_RR))
	{
		if (restroom.get_light()) CLEARBIT(b, LIGHT_RR); else SETBIT(b, LIGHT_RR);
	}
	
	if (CHECKBIT(b, FAN_OVER))
	{
		if (fan.get_active()) CLEARBIT(b, FAN); else SETBIT(b, FAN);
	}
	
	COPYBIT(LEAK_I, LEAK_BR_I, b, LEAK_BR_O);
	COPYBIT(LEAK_I, LEAK_RR_I, b, LEAK_RR_O);
	
	settings.control = b;
}

void IOManager::out_port()
{
	uint8_t b = settings_get(addr.cmd_addr.addr);
	OUT_SAFE(b);
}

IOManager iomanager = IOManager();

#endif /* IOMANAGER_H_ */