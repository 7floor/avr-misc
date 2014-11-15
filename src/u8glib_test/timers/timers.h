/*
 * timers.h
 *
 * Created: 09.03.2014 12:22:59
 *  Author: Dmitry
 */ 


#ifndef TIMERS_H_
#define TIMERS_H_

#include <stdbool.h>
#include "systimer.h"

typedef struct
{
	systime_t start;
} stopwatch;

typedef struct
{
	systime_t start;
	systime_t interval;
} timer;

void stopwatch_start(stopwatch *sw);
bool stopwatch_elapsed(stopwatch *sw, systime_t interval);

void timer_set(timer *t, systime_t interval);
void timer_start(timer *t);
bool timer_expired(timer *t);

#if ENABLE_SYSTIME_S

void stopwatch_s_start(stopwatch *sw);
bool stopwatch_s_elapsed(stopwatch *sw, systime_t interval);

void timer_s_set(timer *t, systime_t interval);
void timer_s_start(timer *t);
bool timer_s_expired(timer *t);

systime_t min2sec(systime_t sec);

#endif

/* definitions below */

void stopwatch_start(stopwatch *sw)
{
	sw->start = get_systime();
}

bool stopwatch_elapsed(stopwatch *sw, systime_t interval)
{
	return (get_systime() - sw->start) >= interval;
}

void timer_set(timer *t, systime_t interval)
{
	t->start = get_systime();
	t->interval = interval;
}

void timer_start(timer *t)
{
	t->start = get_systime();
}

bool timer_expired(timer *t)
{
	return (get_systime() - t->start) >= t->interval;
}

#if ENABLE_SYSTIME_S

void stopwatch_s_start(stopwatch *sw)
{
	sw->start = get_systime_s();
}

bool stopwatch_s_elapsed(stopwatch *sw, systime_t interval)
{
	return (get_systime_s() - sw->start) >= interval;
}

void timer_s_set(timer *t, systime_t interval)
{
	t->start = get_systime_s();
	t->interval = interval;
}

void timer_s_start(timer *t)
{
	t->start = get_systime_s();
}

bool timer_s_expired(timer *t)
{
	return (get_systime_s() - t->start) >= t->interval;
}

systime_t min2sec(systime_t sec)
{
	uint16_t a = sec << 2;
	uint16_t b = a << 4;
	return b - a;
}

#endif

#endif /* TIMERS_H_ */