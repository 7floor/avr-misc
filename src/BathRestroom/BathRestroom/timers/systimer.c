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

#if ENABLE_SYSTIME_S
	static volatile systime_t _systime_s;
#endif

ISR(TIMER0_OVF_vect)
{
	TCNT0 -= TCNT0_TICKS_PER_MS;
	_systime++;
	
#if ENABLE_SYSTIME_S
	static uint16_t _temp_ms = 0;
	if (_temp_ms-- == 0)
	{
		_temp_ms = 999;
		_systime_s++;
	}
#endif
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

#if ENABLE_SYSTIME_S

systime_t get_systime_s()
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		return _systime_s;
	}
}

#endif
