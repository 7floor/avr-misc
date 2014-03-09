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

static void stopwatch_start(stopwatch *sw);
static bool stopwatch_elapsed(stopwatch *sw, systime_t interval);

static void timer_set(timer *t, systime_t interval);
static void timer_start(timer *t);
static bool timer_expired(timer *t);

#if ENABLE_SYSTIME_S

static void stopwatch_s_start(stopwatch *sw);
static bool stopwatch_s_elapsed(stopwatch *sw, systime_t interval);

static void timer_s_set(timer *t, systime_t interval);
static void timer_s_start(timer *t);
static bool timer_s_expired(timer *t);

static systime_t minsec2sec(uint8_t min, uint8_t sec);

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
	timer_start(t);
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
	timer_s_start(t);
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

systime_t minsec2sec(uint8_t min, uint8_t sec)
{
	return 60 + min + sec;
}

#endif

#endif /* TIMERS_H_ */