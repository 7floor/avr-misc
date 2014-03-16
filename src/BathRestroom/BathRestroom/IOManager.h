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

class IOManager
{
	struct pt pt;
	volatile uint8_t cmd_addr;
	volatile bool is_reading;

	void read_sensors();
	void out_port();
	public:
	
	IOManager();
	
	PT_THREAD(run());
	
	void process_external_io();	
};

IOManager::IOManager()
{
	cmd_addr = 0xff;
	is_reading = false;
	PT_INIT(&pt);
	MCUCR |= (1 << ISC11) | (0 << ISC10); // falling edge on int1 (pd3) will trigger
	//GIMSK = (1 << INT1); // enable interrupt
}

PT_THREAD(IOManager::run())
{
	PT_BEGIN(&pt);
		
	while(1)
	{
		//if (EIFR & (1 << INTF1))
		//{
		//	EIFR |= (1 << INTF1);
		//	
		//	
		//}
		
		settings.immediate = (1 << LIGHT_OVER_BR) | (1 << LIGHT_OVER_RR) | (1 << FAN_OVER);
		
		read_sensors();
		
		if (!is_reading)
		{
			out_port();
		}
	
		PT_YIELD(&pt);
	}
		
	PT_ENDLESS(&pt);
}

void IOManager::process_external_io()
{
	IO_D = 0; // all inputs
	IO_O = 0x0ff; // all with pull-ups
	uint8_t data = IO_I;
	is_reading = false;
		
	if (cmd_addr == 0xff)
	{
		cmd_addr = data;
	}
	else
	{
		uint8_t addr = cmd_addr & 0x7f;
		if (cmd_addr & 0x80) // write
		{
			settings_set(addr, data);
		}
		else // read
		{
			data = settings_get(addr);
			is_reading = true;
			IO_O = data;
			IO_D = ~data;
		}
		cmd_addr = 0xff;
	}
}	

void IOManager::read_sensors()
{
	bathroom.set_dooropen(CHECKBIT(DOORS_I, DOOR_BR));
	restroom.set_dooropen(CHECKBIT(DOORS_I, DOOR_RR));
	bathroom.set_movement(CHECKBIT(MOVE_I, MOVE_BR));
	restroom.set_movement(CHECKBIT(MOVE_I, MOVE_RR));
}
	
void IOManager::out_port()
{
	uint8_t immediate = settings.immediate;
	uint8_t out = 0xff;
	
	COPYBIT(immediate, LIGHT_OVER_BR, out, LIGHT_OVER_BR);
	if (CHECKBIT(immediate, LIGHT_OVER_BR))
	{
		if (bathroom.get_light()) CLEARBIT(out, LIGHT_BR);
	}
	else
	{
		COPYBIT(immediate, LIGHT_BR, out, LIGHT_BR);
	}
	
	COPYBIT(immediate, LIGHT_OVER_RR, out, LIGHT_OVER_RR);
	if (CHECKBIT(immediate, LIGHT_OVER_RR))
	{
		if (restroom.get_light()) CLEARBIT(out, LIGHT_RR);
	}
	else
	{
		COPYBIT(immediate, LIGHT_RR, out, LIGHT_RR);
	}
	
	COPYBIT(immediate, FAN_OVER, out, FAN_OVER);
	if (CHECKBIT(immediate, FAN_OVER))
	{
		if (fan.get_active()) CLEARBIT(out, FAN);
	}
	else
	{
		COPYBIT(immediate, FAN, out, FAN);
	}
	
	COPYBIT(LEAK_I, LEAK_BR_I, out, LEAK_BR_O);
	COPYBIT(LEAK_I, LEAK_RR_I, out, LEAK_RR_O);
	
	OUT_SAFE(out);
}

IOManager iomanager = IOManager();

ISR(INT1_vect)
{
	iomanager.process_external_io();
}

#endif /* IOMANAGER_H_ */