/*
 * io_defs.h
 *
 * Created: 04.03.2014 19:55:28
 *  Author: Dmitry
 */ 


#ifndef IO_DEFS_H_
#define IO_DEFS_H_

#include <avr/io.h>

#define ALARM_DDR DDRD
#define ALARM_PORT PORTD
//#define ALARM_PIN PD6
#define ALARM_P PD4

#define LEAKDET_DDR DDRB
#define LEAKDET_PORT PORTB
#define LEAKDET_PIN PINB
#define LEAKDET_P PB0


#endif /* IO_DEFS_H_ */