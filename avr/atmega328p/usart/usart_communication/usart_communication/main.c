/*
 * main.c
 *
 * Created: 2/23/2021 8:43:46 PM
 *  Author: Bandy
 */ 
#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include "USART.c"

int main(void)
{
	DDRB = 0xFF;
	initUSART();
	printString("Hello World!\r\n");
    while(1)
    {
		uint8_t serialCharacter = receiveByte();
		transmitByte(serialCharacter);
		PORTB = serialCharacter;
		_delay_ms(500);
    }
}