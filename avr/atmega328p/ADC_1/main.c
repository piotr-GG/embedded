/*
 * main.c
 *
 * Created: 3/13/2021 7:49:21 PM
 *  Author: Bandy
 */ 
# define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>

static inline void initADC0(void){
	ADMUX |= (1<<REFS0);     //reference voltage on AVCC
	ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); // ADC clock prescaler /128
	ADCSRA |= (1<<ADEN); //enable ADC
}

int main(void)
{
	uint8_t ledValue;
	uint16_t adcValue;
	uint8_t i;
	initADC0();
	DDRD = 0b11111100;
	PORTD |= (1<<PORTD2);
    while(1)
    {
        ADCSRA |= (1 << ADSC); //start ADC conversion
		do 
		{

		} while (ADCSRA & (1<<ADSC));
		
		adcValue = ADC >> 2; //pomin dwa najmniej znaczace bity
		adcValue &= 0b11111100;
		PORTD = adcValue;
		//_delay_ms(50);
    }
	return 0;
}