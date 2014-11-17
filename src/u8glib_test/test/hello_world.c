#include <avr/interrupt.h>
#include <util/atomic.h>
#include <avr/io.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "u8g.h"
#include "systimer.h"

u8g_t u8g;

void sys_init(void)
{
	/* select minimal prescaler (max system speed) */
	CLKPR = 0x80;
	CLKPR = 0x00;
}

void u8g_setup(void)
{
	/* activate pull-up, set ports to output, init U8glib */
	u8g_SetPinInput(PN(2,5)); u8g_SetPinLevel(PN(2,5), 1); u8g_SetPinOutput(PN(2,5));
	u8g_SetPinInput(PN(2,4)); u8g_SetPinLevel(PN(2,4), 1); u8g_SetPinOutput(PN(2,4));
	u8g_InitI2C(&u8g, &u8g_dev_ssd1306_128x64_2x_i2c, U8G_I2C_OPT_NO_ACK);
}

systime_t delta_time;

#define TIMER_F ((float)(F_CPU / 64))
// guess what
#define PI 3.1415926
// wheel diameter, m
#define WHEEL_D 0.345
// wheel perimeter length
#define WHEEL_L (WHEEL_D * PI)
// single "step " length (wheel has 6 magnets)
#define STEP_L (WHEEL_L / 6.0)
// m/s to km/h
#define KPH_IN_MPH 3.6

#define FOREVER 65535
volatile uint16_t average;
float speed;
#define SAMPLES_COUNT 5
uint16_t samples[SAMPLES_COUNT];

char running = 0;

void init_samples()
{
	for(int i = 0; i < SAMPLES_COUNT; i++)
	{
		samples[i] = FOREVER;
	}
}

void add_sample(uint16_t sample)
{
	uint32_t tmp = sample;
	for(int i = SAMPLES_COUNT - 1; i > 0; i--)
	{
		uint16_t s = samples[i-1];
		samples[i] = s;
		tmp += s;
	}
	samples[0] = sample;
	tmp /= SAMPLES_COUNT;
	average = tmp;
}

uint16_t get_average()
{
	uint16_t result;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		result = average;
	}
	return result;
}


float time_to_speed(uint16_t time)
{
	if (time == 0 || time == FOREVER) return 0.0;	
	return STEP_L * TIMER_F * KPH_IN_MPH / (float)time;
}


ISR(TIMER1_CAPT_vect)
{
	uint16_t tmp = ICR1;
	
	if (tmp < 200) return; // debounce
	
	TCNT1 = 0;
	if (running)
	{
		add_sample(tmp);
	}
	running = 1;
}

ISR(TIMER1_OVF_vect)
{
	running = 0;
	add_sample(FOREVER);
}

void draw()
{
	char buf[20];
	dtostrf(speed, 4, 1, buf);
	u8g_SetFont(&u8g, u8g_font_courB24r);
	u8g_DrawStr(&u8g, 0, 24, buf);
	
	u8g_SetFont(&u8g, u8g_font_unifont_0_8);
	u8g_DrawStr(&u8g, 88, 24, "км/ч");

	u8g_DrawFrame(&u8g, 0, 37, 128, 7);
	u8g_uint_t w = (speed < 10.0 ? speed : 10.0) / 10.0 * 124;
	u8g_DrawBox(&u8g, 2, 39, w, 3);
	
	u8g_DrawStr(&u8g, 20, 60, "Мышометр :)");
}


int main(void)
{
	sys_init();
	init_systime();
	u8g_setup();
	init_samples();

DDRB &= ~(1 << PINB0);
PORTB |= (1 << PINB0);

	// no OC1A/B outputs, normal operation
	TCCR1A = 
		  (0 << COM1A1) | (0 << COM1A0) | (0 << COM1B1) | (0 << COM1B0)
		| (0 << WGM11) | (0 << WGM10);
	// noise canceller, falling edge, normal (cont.), clk/256 prescaler (8 MHz/64 = 125000 Hz)
	TCCR1B = 
		  (1 << ICNC1) 
		| (0 << ICES1) 
		| (0 << WGM13) | (0 << WGM12) 
		| (0 << CS12) | (1 << CS11) | (1 << CS10);
		
	// enable interrupts for input capture and overflow
	TIMSK1 = (1 << ICIE1) | (1 << TOIE1);
	
	sei();
	
	for(;;)
	{
		speed = time_to_speed(get_average());
		u8g_FirstPage(&u8g);
		do
		{
			draw();
		} while ( u8g_NextPage(&u8g) );
	
	}
}

