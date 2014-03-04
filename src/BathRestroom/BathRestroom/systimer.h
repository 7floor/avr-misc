/*
 * systimer.h
 *
 * Created: 02.03.2014 21:27:31
 *  Author: Dmitry
 */ 

#ifndef SYSTIMER_H_
#define SYSTIMER_H_

#include <stdint.h>
#include <stdbool.h>
#include <util/atomic.h>
#include <avr/io.h>

typedef uint_least16_t systime_t;

typedef struct 
{
	systime_t start;
	systime_t interval;
} timer;

void init_systimer();
systime_t get_systime();
void set_timer(timer *t, systime_t msecs);
bool is_timer_expired(timer *t);

#endif /* SYSTIMER_H_ */