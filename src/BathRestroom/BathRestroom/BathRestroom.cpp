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
extern "C"
{
	#include "timers/timers.h"
}
#include "io_defs.h"
#include "IOManager.h"
#include "alarm.h"
#include "leak_detector.h"
#include "room.h"

static void init_io()
{
	ALARM_DDR |= (1 << ALARM_P);
	
	LEAKDET_DDR &= ~(1 << LEAKDET_P);
	LEAKDET_PORT |= (1 << LEAKDET_P);
}

int main(void)
{
	init_io();
	init_systimer();
	
	sei();
	
    while(1)
    {
		iomanager.run();
		alarm.run();
		brld.run();
		rrld.run();
		bathroom.run();
		restroom.run();
    }
}