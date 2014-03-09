/*
 * systimer.c
 *
 * Created: 04.03.2014 20:12:48
 *  Author: Dmitry
 */ 
#include "systimer.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#define TCNT0_TICKS_PER_MS (F_CPU / 64 / 1000)

#define FIX_POINTER(_ptr) __asm__ __volatile__("" : "=b" (_ptr) : "0" (_ptr))

struct _systime
{
	volatile systime_t milliseconds;
	#if ENABLE_SYSTIME_S
		volatile systime_t seconds;
		volatile int16_t _temp_ms;
	#endif
	
} systime;
 
ISR(TIMER0_OVF_vect)
{
	TCNT0 -= TCNT0_TICKS_PER_MS;
	
	struct _systime *v = &systime;
	FIX_POINTER(v);
	
	v->milliseconds++;
	
#if ENABLE_SYSTIME_S

    int16_t t = v->_temp_ms - 1;
	if (t == 0)
	{
		t = 1000;
		v->seconds++;
	}
	v->_temp_ms = t;

#endif

}

void init_systime()
{
#if ENABLE_SYSTIME_S
	systime._temp_ms = 1;
#endif
	TCCR0B = 0 << CS02 | 1 << CS01 | 1 << CS00;
	TIMSK = 1 << TOIE0;
}

systime_t get_systime()
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		return systime.milliseconds;
	}
	return 0; // for compiler
}

#if ENABLE_SYSTIME_S

systime_t get_systime_s()
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		return systime.seconds;
	}
	return 0; // for compiler
}

#endif
