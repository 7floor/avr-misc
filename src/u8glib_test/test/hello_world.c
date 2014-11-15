/*

main.c

Hello World for ATMEGA Controller

Universal 8bit Graphics Library

Copyright (c) 2012, olikraus@gmail.com
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list
of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or other
materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "u8g.h"
#include "systimer.h"

#if defined(__AVR__)
#include <avr/interrupt.h>
#include <avr/io.h>
#endif
#include <math.h>
#include <stdio.h>

/*
Software SPI:
uint8_t u8g_InitSPI(u8g_t *u8g, u8g_dev_t *dev, uint8_t sck, uint8_t mosi, uint8_t cs, uint8_t a0, uint8_t reset);

Hardware SPI:
uint8_t u8g_InitHWSPI(u8g_t *u8g, u8g_dev_t *dev, uint8_t cs, uint8_t a0, uint8_t reset);

Parallel Interface:
uint8_t u8g_Init8Bit(u8g_t *u8g, u8g_dev_t *dev, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7,
uint8_t en, uint8_t cs1, uint8_t cs2, uint8_t di, uint8_t rw, uint8_t reset);

Visit
http://code.google.com/p/u8glib/wiki/device
for a list of valid devices (second argument of the constructor).

The following examples will use the dogm132 device: u8g_dev_st7565_dogm132_sw_spi

Note: The device must match the setup: For example, do not use a sw_spi device with u8g_InitHWSPI().
*/

u8g_t u8g;

void u8g_setup(void)
{
	/*
	Test Envionment 2, ATMEGA and Seeedstudio 96x96 OLED via I2C
	SCL: Port C, Bit 5
	SDA: Port C, Bit 4
	*/
	
	/* activate pull-up, set ports to output, init U8glib */
	u8g_SetPinInput(PN(2,5)); u8g_SetPinLevel(PN(2,5), 1); u8g_SetPinOutput(PN(2,5));
	u8g_SetPinInput(PN(2,4)); u8g_SetPinLevel(PN(2,4), 1); u8g_SetPinOutput(PN(2,4));
	u8g_InitI2C(&u8g, &u8g_dev_ssd1306_128x64_2x_i2c, U8G_I2C_OPT_NO_ACK);
}

char *text[33] = {
	"",
	"",
	"",
	"",
	"           Episode IV",
	"           A NEW HOPE",
	"",
	"It  is  a period  of  civil war.",
	"Rebel    spaceships,    striking",
	"from  a  hidden  base,  have won",
	"their   first  victory   against",
	"the  evil  Galactic  Empire.",
	"",
	"During    the   battle,    Rebel",
	"spies  managed  to steal  secret",
	"plans     to     the    Empire's",
	"ultimate   weapon,   the   DEATH",
	"STAR,     an    armored    space",
	"station   with    enough   power",
	"to  destroy  an  entire planet.",
	"",
	"Pursued    by    the    Empire's",
	"sinister    agents,     Princess",
	"Leia   races  home  aboard   her",
	"starship,   custodian   of   the",
	"stolen  plans   that   can  save",
	"her    people     and    restore",
	"freedom   to   the   galaxy ....",
	"",
	"",
	"",
	"",
	""
};

void sys_init(void)
{
#if defined(__AVR__)
	/* select minimal prescaler (max system speed) */
	CLKPR = 0x80;
	CLKPR = 0x00;
#endif
}

int line = 0;
int pixel = 15;
systime_t delta_time;

void draw(void)
{
	//u8g_SetFont(&u8g, u8g_font_unifont_0_8); // russian in ISO charset
	
	u8g_SetFont(&u8g, u8g_font_courR08);
	char buf[20];
//	sprintf(buf, "%d ms", delta_time);
	sprintf(buf, "%2d FPS", 1000 / delta_time);
//	sprintf(buf, "%2d FPS (%d ms)", 1000 / delta_time, delta_time);
	u8g_DrawStr(&u8g, 0, 8, buf);
	
	systime_t ms = get_systime();
	sprintf(buf, "%u", ms);
	u8g_DrawStr(&u8g, 32, 32, buf);
	
/*
	u8g_SetFont(&u8g, u8g_font_micro);
	for (int i = 0; i < 5; i++)
	{
		u8g_DrawStr(&u8g, 0, i * 16 + pixel, text[line + i]);
	}
	*/
}


int main(void)
{
	sys_init();
	init_systime();
	u8g_setup();
	sei();
	
	systime_t last = get_systime();

	for(;;)
	{
		systime_t now = get_systime();
		delta_time = now - last;
		last = now;
		
		u8g_FirstPage(&u8g);
		do
		{
			draw();
		} while ( u8g_NextPage(&u8g) );
		
		pixel--;
		if (pixel < 0)
		{
			pixel = 15;
			line++;
			if (line > 27)
			{
				line = 0;
			}
		}
	}
}

