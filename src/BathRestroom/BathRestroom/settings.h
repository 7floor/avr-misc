/*
 * settings.h
 *
 * Created: 09.03.2014 16:43:11
 *  Author: Dmitry
 */ 


#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <stdint.h>
#include "eeprom.h"

typedef struct
{
	uint8_t min, sec;
} timeout_t;

typedef struct
{
	timeout_t open_absent, open_present, closed_absent, closed_present;
} room_timeouts_t;

typedef struct
{
	room_timeouts_t bathroom, restroom;
	timeout_t fan;
} settings_t;


settings_t settings;

#define FIX_POINTER(_ptr) __asm__ __volatile__("" : "=b" (_ptr) : "0" (_ptr))

static void settings_read_all()
{
	// read eeprom from address 1
	for(uint8_t eeaddr = 0; eeaddr < sizeof(settings); eeaddr++)
	{
		*(((uint8_t*)&settings) + eeaddr) = EEPROM_read(eeaddr);
	}
}

static void settings_set(uint8_t addr, uint8_t data)
{
	if (addr >= sizeof(settings)) return;
	
	uint8_t* ptr = (uint8_t*)(&settings) + addr;
	*ptr = data;
	EEPROM_write(addr, data);
}

static uint8_t settings_get(uint8_t addr)
{
	if (addr >= sizeof(settings)) return 0xff;
	uint8_t* ptr = (uint8_t*)(&settings) + addr;
	return *ptr;
}



#endif /* SETTINGS_H_ */