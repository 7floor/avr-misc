/*
 * LeakDetector.h
 *
 * Created: 04.03.2014 21:06:46
 *  Author: Dmitry
 */ 


#ifndef LEAK_DETECTOR_H_
#define LEAK_DETECTOR_H_

#include <stdbool.h>
#include "pt_ext.h"
#include "systimer.h"
#include "alarm.h"

static void init_leak_detector();
static void process_leak_detector();
PT_THREAD(leak_detector_thread(struct pt *pt));

static struct pt pt_leak_detector;

static void init_leak_detector()
{
	PT_INIT(&pt_leak_detector);
}

void process_leak_detector()
{
	PT_SCHEDULE(leak_detector_thread(&pt_leak_detector));
}

PT_THREAD(leak_detector_thread(struct pt *pt))
{
	static timer detect_period;
	
	PT_BEGIN(pt);
	while(1)
	{
		set_timer(&detect_period, 1000);
		PT_WAIT_UNTIL(pt, is_timer_expired(&detect_period));
		
		bool detected = !(LEAKDET_PIN & (1 << LEAKDET_P));
		set_alarm(detected);
	}
	PT_ENDLESS(pt);
}



#endif /* LEAK_DETECTOR_H_ */