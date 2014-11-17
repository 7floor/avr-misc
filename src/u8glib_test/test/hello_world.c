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

#define TIMER_SCALE 256
#define TIMER_F ((float)(F_CPU / TIMER_SCALE))
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

#define SAMPLES_COUNT 4
uint16_t samples[SAMPLES_COUNT];
int sample_index = 0;
char running = 0;

float speed;

void clear_samples()
{
	memset(samples, 0, sizeof(samples));
}

void add_sample(uint16_t sample)
{
	samples[sample_index] = sample;
	sample_index++;
	if (sample_index == SAMPLES_COUNT)
		sample_index = 0;
}

uint16_t get_average()
{
	uint32_t accumulator = 0;
	int valid_samples_num = 0;
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		for (int i = 0; i < SAMPLES_COUNT; i++)
		{
			uint16_t sample = samples[i];
			if (sample > 0)
			{
				accumulator += sample;
				valid_samples_num++;
			}
		}
	}
	
	return valid_samples_num == 0 
		? 0 
		: accumulator / valid_samples_num;
}


float time_to_speed(uint16_t time)
{
	if (time == 0) return 0.0;	
	return STEP_L * TIMER_F * KPH_IN_MPH / (float)time;
}


ISR(TIMER1_CAPT_vect)
{
	uint16_t tmp = ICR1;
	
	//if (tmp < (uint16_t)(STEP_L * TIMER_F * KPH_IN_MPH / 50)) return; // debounce: can't be faster than 50 km/h // doesn't really help; a capacitor @ 0.01uF is way better
	
	if (running)
	{
		TCNT1 = 0;
		add_sample(tmp);
	}
	running = 1;
}

ISR(TIMER1_OVF_vect)
{
	running = 0;
	clear_samples();
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
	clear_samples();

	DDRB &= ~(1 << PINB0);
	PORTB |= (1 << PINB0);

	// OC1A/B outputs disconnected
	// Fast PWM with TOP=OCR1A
	// Input capture enabled at falling edge
	// noise canceler enabled
	// prescaler at 256
	TCCR1A = 
		  (0 << COM1A1) | (0 << COM1A0) | (0 << COM1B1) | (0 << COM1B0)
		| (1 << WGM11) | (1 << WGM10);
	TCCR1B = 
		  (1 << ICNC1) | (0 << ICES1) 
		| (1 << WGM13) | (1 << WGM12) 
		| (1 << CS12) | (0 << CS11) | (0 << CS10);
		
	OCR1A = F_CPU / TIMER_SCALE; // will overflow each 1 s.
		
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

