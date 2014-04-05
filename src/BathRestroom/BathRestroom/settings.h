/*
 * settings.h
 *
 * Created: 09.03.2014 16:43:11
 *  Author: Dmitry
 */ 


#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <stdint.h>
#include <avr/eeprom.h>
extern "C"
{
	#include "timers/timers.h"
}

// bit 7: 1 = min, 0 = sec
// bits 6..0 value
typedef struct
{
	uint8_t val:7;
	bool is_min:1;
	systime_t get_seconds()
	{
		return  is_min ? min2sec(val) : val;
	}
} timeout_t;

typedef struct
{
	timeout_t door_opened, door_closed, open_present, closed_present;
} room_timeouts_t;

typedef struct  
{
	timeout_t min_presence, duration;
} fan_timeouts_t;

typedef struct
{
	uint8_t control;
	bool alarm;
	uint8_t move_inhibit_128ms_ticks;
	room_timeouts_t bathroom, restroom;
	fan_timeouts_t fan;
} settings_t;

settings_t settings;

settings_t EEMEM settings_ee = {
	control: 0xff,
	alarm: true,
	move_inhibit_128ms_ticks: 8,
	bathroom: { door_opened: { val: 5, is_min: false }, door_closed: { val: 3, is_min: false }, open_present: { val: 30, is_min: false }, closed_present: { val: 5, is_min: true } },
	restroom: { door_opened: { val: 5, is_min: false }, door_closed: { val: 3, is_min: false }, open_present: { val: 30, is_min: false }, closed_present: { val: 5, is_min: true } },
	fan: { min_presence: { val: 1, is_min: true }, duration: { val: 1, is_min: true } }	,
};

static void settings_read_all()
{
	eeprom_read_block(&settings, &settings_ee, sizeof(settings));
}

static void settings_set(uint8_t addr, uint8_t data)
{
	if (addr >= sizeof(settings)) return;
	
	uint8_t* ptr = (uint8_t*)(&settings) + addr;
	
	if (*ptr == data) return; // will save eeprom
	
	*ptr = data;
	
	if (addr == 0) return; // don't store the 'immediate' member
	
	eeprom_write_byte((uint8_t*)(&settings_ee) + addr, data);
}

static uint8_t settings_get(uint8_t addr)
{
	if (addr >= sizeof(settings)) return 0xff;
	uint8_t* ptr = (uint8_t*)(&settings) + addr;
	return *ptr;
}

#endif /* SETTINGS_H_ */