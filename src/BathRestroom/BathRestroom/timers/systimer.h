/*
 * systimer.h
 *
 * Created: 02.03.2014 21:27:31
 *  Author: Dmitry
 */ 

#ifndef SYSTIMER_H_
#define SYSTIMER_H_

#include <stdint.h>

#define ENABLE_SYSTIME_S 1

typedef uint_least16_t systime_t;

void init_systime();
systime_t get_systime();

#if ENABLE_SYSTIME_S

systime_t get_systime_s();

#endif

#endif /* SYSTIMER_H_ */