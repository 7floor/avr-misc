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
#include "room.h"
#include "alarm.h"
#include "leak_detector.h"
#include "settings.h"
#include "pt_ext.h"

class IOManager
{
	struct pt pt;
	uint8_t cmd_addr;
	bool is_reading;

	void out_port()
	{
		if (bathroom.light) set_pin_0(BR_LIGHT); else set_pin_1(BR_LIGHT);
		if (restroom.light) set_pin_0(WC_LIGHT); else set_pin_1(WC_LIGHT);
		if (brld.is_detected()) set_pin_0(BR_LEAKOUT); else set_pin_1(BR_LEAKOUT);
		if (rrld.is_detected()) set_pin_0(WC_LEAKOUT); else set_pin_1(WC_LEAKOUT);
	}
	
	public:
	
	IOManager()
	{
		cmd_addr = 0xff;
		is_reading = false;
		PT_INIT(&pt);
		MCUCR |= (1 << ISC11) | (0 << ISC10); // falling edge on int1 (pd3) will trigger
		GIMSK = (0 << INT1); // don't enable interrupt
	}
	
	PT_THREAD(run())
	{
		PT_BEGIN(&pt);
		
		while(1)
		{
			if (EIFR & (1 << INTF1))
			{
				EIFR |= (1 << INTF1); // reset flag
				process_external_io();
				if (is_reading)
				PT_WAIT_UNTIL(&pt, EIFR & (1 << INTF1));
			}
			
			out_port();
			PT_YIELD(&pt);
		}
		
		PT_ENDLESS(&pt);
	}
	
	void process_external_io()
	{
		DDRB = 0; // all inputs
		PORTB = 0x0ff; // all with pull-ups
		uint8_t data = PINB;
		is_reading = false;
		
		if (cmd_addr == 0xff)
		{
			cmd_addr = data;
		}
		else
		{
			uint8_t addr = cmd_addr & 0x7f;
			if ((cmd_addr & 0x80) != 0) // write
			{
				settings_set(addr, data);
			}
			else // read
			{
				data = settings_get(addr);
				is_reading = true;
				PORTB = data;
				DDRB = ~data;
			}
			cmd_addr = 0xff;
		}
	}
	
};

IOManager iomanager = IOManager();

#endif /* IOMANAGER_H_ */