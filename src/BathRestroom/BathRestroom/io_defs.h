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
#define ALARM_P PD4

#define LEAKDET_DDR DDRB
#define LEAKDET_PORT PORTB
#define LEAKDET_PIN PINB
#define LEAKDET_P PB0

/////

#define BR_DOOR
#define BR_DOOR_PORT A
#define BR_DOOR_PIN 1

#define WC_DOOR
#define WC_DOOR_PORT A
#define WC_DOOR_PIN 0

#define BR_MOVE
#define BR_MOVE_PORT D
#define BR_MOVE_PIN 0

#define WC_MOVE
#define WC_MOVE_PORT D
#define WC_MOVE_PIN 1

#define BR_LEAKIN
#define BR_LEAKIN_PORT D
#define BR_LEAKIN_PIN 4

#define WC_LEAKIN
#define WC_LEAKIN_PORT D
#define WC_LEAKIN_PIN 5

#define BR_LEAKOUT
#define BR_LEAKOUT_PORT B
#define BR_LEAKOUT_PIN 0

#define WC_LEAKOUT
#define WC_LEAKOUT_PORT B
#define WC_LEAKOUT_PIN 1

#define BR_LIGHT
#define BR_LIGHT_PORT B
#define BR_LIGHT_PIN 2

#define BR_LIGHT_OVER
#define BR_LIGHT_OVER_PORT B
#define BR_LIGHT_OVER_PIN 3

#define WC_LIGHT
#define WC_LIGHT_PORT B
#define WC_LIGHT_PIN 4

#define WC_LIGHT_OVER
#define WC_LIGHT_OVER_PORT B
#define WC_LIGHT_OVER_PIN 5

#define WC_VENT
#define WC_VENT_PORT B
#define WC_VENT_PIN 6

#define WC_VENT_OVER
#define WC_VENT_OVER_PORT B
#define WC_VENT_OVER_PIN 7

//// port b uses shared bus logic
// down is real down, up is input with pullup
// so use the following macros

#define set_pin_0(pin) set_portpin_0(pin ## _PORT, pin ## _PIN)
#define set_portpin_0(port, pin) _set_portpin_0_(port, pin)
#define _set_portpin_0_(port, pin) \
do { \
	PORT ## port &= ~(1 << (pin)); \
	DDR ## port |= (1 << (pin)); \
} while(0)

#define set_pin_1(pin) set_portpin_1(pin ## _PORT, pin ## _PIN)
#define set_portpin_1(port, pin) _set_portpin_1_(port, pin)
#define _set_portpin_1_(port, pin) \
do { \
	DDR ## port &= ~(1 << (pin)); \
	PORT ## port |= (1 << (pin)); \
} while(0)

#define pin_is_0(pin) portpin_is_0(pin ## _PORT, pin ## _PIN)
#define portpin_is_0(port, pin) _portpin_is_0_(port, pin)
#define _portpin_is_0_(port, pin) \
((PIN ## port & (1 << (pin))) == 0)

#define pin_is_1(pin) portpin_is_1(pin ## _PORT, pin ## _PIN)
#define portpin_is_1(port, pin) _portpin_is_1_(port, pin)
#define _portpin_is_1_(port, pin) \
((PIN ## port & (1 << (pin))) != 0)









#endif /* IO_DEFS_H_ */