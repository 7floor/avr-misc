/*
 * systimer.c
 *
 * Created: 04.03.2014 20:12:48
 *  Author: Dmitry
 */ 
#include "systimer.h"
#include <avr/io.h>
#include <util/atomic.h>

#define TCNT0_TICKS_PER_MS (F_CPU / 64 / 1000)

static volatile systime_t _systime;

ISR(TIMER0_OVF_vect)
{
	TCNT0 -= TCNT0_TICKS_PER_MS;
	_systime++;
}

void init_systimer()
{
	TCCR0B = 0 << CS02 | 1 << CS01 | 1 << CS00;
	TIMSK = 1 << TOIE0;
}

systime_t get_systime()
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		return _systime;
	}
}

void set_timer(timer *t, systime_t msecs)
{
	t->interval = msecs;
	t->start = get_systime();
}

bool is_timer_expired(timer *t)
{
	return (get_systime() - t->start) >= t->interval;
}

