/*
 * Alarm.h
 *
 * Created: 04.03.2014 19:53:02
 *  Author: Dmitry
 */ 


#ifndef ALARM_H_
#define ALARM_H_

#include <stdbool.h>
#include "pt/pt.h"
#include "systimer.h"

static void init_alarm();
static void process_alarm();
static void set_alarm(bool on);
PT_THREAD(alarm_thread(struct pt *pt));

static struct pt pt_alarm;
static bool is_alarm_on;

void init_alarm()
{
	PT_INIT(&pt_alarm);
}

void process_alarm()
{
	PT_SCHEDULE(alarm_thread(&pt_alarm));
}

void set_alarm(bool on)
{
	is_alarm_on = on;
}

PT_THREAD(alarm_thread(struct pt *pt))
{
	#define MORSE_CODE_SOS
	#define DOT_TIME 50
	#define DASH_TIME (3 * DOT_TIME)
	
	static timer timer;
	
	PT_BEGIN(pt);
	
	while(1)
	{
		PT_WAIT_UNTIL(pt, is_alarm_on);
		
	#ifdef MORSE_CODE_SOS
		
		static char i, j;
		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 3; j++)
			{
				ALARM_PORT |= (1 << ALARM_P);
				set_timer(&timer, i == 1 ? DASH_TIME : DOT_TIME);
				PT_WAIT_UNTIL(pt, is_timer_expired(&timer));
					
				ALARM_PORT &= ~(1 << ALARM_P);
				set_timer(&timer, DOT_TIME);
				PT_WAIT_UNTIL(pt, is_timer_expired(&timer));
			}
			set_timer(&timer, DOT_TIME * 2); // +2 = 3 - a space between letters 
			PT_WAIT_UNTIL(pt, is_timer_expired(&timer));
		}
		set_timer(&timer, DOT_TIME * 4);
		PT_WAIT_UNTIL(pt, is_timer_expired(&timer)); // +4 = 7 - a space between words
		
	#else

		ALARM_PORT |= (1 << ALARM_P);
		set_timer(&timer, 250);
		PT_WAIT_UNTIL(pt, is_timer_expired(&timer));
		
		ALARM_PORT &= ~(1 << ALARM_P);
		set_timer(&timer, 250);
		PT_WAIT_UNTIL(pt, is_timer_expired(&timer));
		
	#endif
	}
	
	PT_END(pt);
}

#endif /* ALARM_H_ */