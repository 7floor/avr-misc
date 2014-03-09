/*
 * eeprom.h
 *
 * Created: 26.01.2014 13:04:01
 *  Author: Dmitry
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

uint8_t EEPROM_read(uint8_t address)
{
	// wait for completion of previous write
	while(EECR & (1<<EEPE))	;
	// set up address
	EEAR = address;
	// trigger read
	EECR |= (1<<EERE);
	// return data
	return EEDR;
}

void EEPROM_write(uint8_t address, uint8_t data)
{
	// wait for completion of previous write
	while(EECR & (1<<EEPE))	;
	// set up address and data
	EEAR = address;
	EEDR = data;
	// trigger write sequence
	EECR |= (1<<EEMPE);
	EECR |= (1<<EEPE);
}

#endif /* EEPROM_H_ */