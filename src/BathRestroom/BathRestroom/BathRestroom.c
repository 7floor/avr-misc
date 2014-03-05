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
#include "io_defs.h"
#include "pt/pt.h"
#include "pt_ext.h"
#include "systimer.h"
#include "alarm.h"
#include "leak_detector.h"

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
	init_alarm();
	init_leak_detector();
	
	sei();
	
    while(1)
    {
		process_alarm();
		process_leak_detector();
    }
}