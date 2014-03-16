/*
 * io_defs.h
 *
 * Created: 04.03.2014 19:55:28
 *  Author: Dmitry
 */ 


#ifndef IO_DEFS_H_
#define IO_DEFS_H_

#include <avr/io.h>

#define SETBIT(ADDRESS,BIT) (ADDRESS |= (1<<BIT)) 
#define CLEARBIT(ADDRESS,BIT) (ADDRESS &= ~(1<<BIT)) 
#define FLIPBIT(ADDRESS,BIT) (ADDRESS ^= (1<<BIT)) 
#define CHECKBIT(ADDRESS,BIT) (ADDRESS & (1<<BIT)) 
#define COPYBIT(RADDRESS,RBIT,WADDRESS,WBIT) (CHECKBIT(RADDRESS,RBIT) ? SETBIT(WADDRESS,WBIT) : CLEARBIT(WADDRESS,WBIT))

// IO
#define IO_D DDRB
#define IO_I PINB
#define IO_O PORTB
#define LEAK_BR_O 0
#define LEAK_RR_O 1
#define LIGHT_BR 2
#define LIGHT_OVER_BR 3
#define LIGHT_RR 4
#define LIGHT_OVER_RR 5
#define FAN 6
#define FAN_OVER 7

// DOORS
#define DOORS_D DDRA
#define DOORS_I PINA
#define DOOR_BR 1
#define DOOR_RR 0

// MOVE
#define MOVE_D DDRD
#define MOVE_I PIND
#define MOVE_BR 0
#define MOVE_RR 1

// LEAK
#define LEAK_D DDRD
#define LEAK_I PIND
#define LEAK_BR_I 4
#define LEAK_RR_I 5

// ALARM
#define ALARM_D DDRD
#define ALARM_O PORTD
#define ALARM 6

// STROBE
#define STROBE_D DDRD
#define STROBE_O PORTD
#define STROBE 3

#define SET_PORTA() do { DDRA = 0; PORTA = 0xFF; } while (0) // all inputs with pull-ups
#define SET_PORTB() do { DDRB = 0; PORTB = 0xFF; } while (0) // all inputs with pull-ups
#define SET_PORTD() do { DDRD = (1 << ALARM); PORTD = ~(1 << ALARM); } while (0)

#define OUT_SAFE(b) do { \
	IO_O &= b; \
	IO_D = ~b; \
	IO_O = b; \
} while (0) 






#endif /* IO_DEFS_H_ */