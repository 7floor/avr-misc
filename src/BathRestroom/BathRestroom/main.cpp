/*
 * BathRestroom.c
 *
 * Created: 04.03.2014 18:47:44
 *  Author: Dmitry
 */ 

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

extern "C"
{
	#include "timers/timers.h"
}
#include "io_defs.h"
#include "IOManager.h"
#include "room.h"
#include "fan.h"
#include "alarm.h"

static void reset_ds2408()
{
	STROBE_O &= ~(1 << STROBE);
	STROBE_D |= (1 << STROBE);
	_delay_us(10);
	STROBE_D &= ~(1 << STROBE);
	STROBE_O |= (1 << STROBE);
}

static void init_io()
{
	SET_PORTA();
	SET_PORTB();
	SET_PORTD();
}

int main(void) 
{
	init_io();
	reset_ds2408();
	settings_read_all();
	init_systime();
	
	sei();
	
    while(1)
    {
		iomanager.run();
		bathroom.run();
		restroom.run();
		fan.run();
		alarm.run();
    }
}