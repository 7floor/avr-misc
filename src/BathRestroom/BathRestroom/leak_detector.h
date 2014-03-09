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
extern "C"
{
	#include "timers/timers.h"
}

class LeakDetector
{
	struct pt pt;
	//stopwatch sw;
	bool detected;
	
	
	public:
	
	LeakDetector()
	{
		PT_INIT(&pt);
	}
	
	PT_THREAD(run())
	{	
		PT_BEGIN(&pt);
	
		while(1)
		{
			//stopwatch_start(&sw);
			//PT_WAIT_UNTIL(&pt, stopwatch_elapsed(&sw, 1000));
			PT_YIELD(&pt);
		
			detected = !(LEAKDET_PIN & (1 << LEAKDET_P));
			//set_alarm(detected);
		}
	
		PT_ENDLESS(&pt);
	}
	
	bool is_detected() { return detected; }
};

LeakDetector brld = LeakDetector();
LeakDetector rrld = LeakDetector();

#endif /* LEAK_DETECTOR_H_ */