/*
 * main.c
 *
 * Created: 2/20/2021 12:36:59 PM
 *  Author: Bandy
 */ 

#define F_CPU 16000000
#define TIMER0_TOP 0xFF

#include<inttypes.h>
#include<avr/io.h>
#include<avr/interrupt.h>
#include<avr/sleep.h>

enum {UP, DOWN};

static volatile uint16_t pwm = 5;
static volatile uint8_t direction =UP;


void init_timer0()
{
	TCCR0A = (1 << WGM01); //tryb CTC (Clear Timer On Compare)
	OCR0A = TIMER0_TOP;		//255 jako wartoœæ do której ma zliczaæ

	TIMSK0 |= (1<<TOIE0); //ustaw bit Timer/Counter0 Overflow interrupt enable
	
	TCNT0 = 0; //inicjalizacja licznika
	TCCR0B = (1<< CS02 ) | (1<<CS00); //prescaler 1/1024;

	sei(); //enable global interrupts
}



int main(void)
{
	
	DDRB = (1<<DDB1) | (1<<DDB2) | (1 << DDB3); //PB1 i PB2 jako output
	PORTB = 0;
	
	ICR1 = 0xFFFF; //ustaw TOP na 16-bitów
	
	OCR1A = 0x00FF; //ustaw PWM na 25% dla PB1
	OCR1B = 0xBFFF; //ustaw PWM na 75% dla PB2
	
	TCCR1A |= (1 << COM1A1) | (1<<COM1B1); //ustaw non-inverting mode
	
	TCCR1A |= (1<< WGM11);				   //ustaw FAST PWM u¿ywaj¹c ICR1 jako TOP (Mode 14)
	TCCR1B |= (1 << WGM12) | (1 << WGM13); //ustaw FAST PWM u¿ywaj¹c ICR1 jako TOP (Mode 14)
	TCCR1B |= (1<< CS10); //brak prescalera
	
	sei();
	
	
	init_timer0();

    while(1)
    {
		sleep_mode();
    }
}

//procedura obs³ugi przerwania (Interrupt service routine)
//OVF_vect = Overflow vector
ISR(TIMER0_OVF_vect)
{
	switch (direction){
		case UP:
			if(++pwm == TIMER0_TOP)
				direction = DOWN;
			break;
		case DOWN:
			if(--pwm == 0)
				direction = UP;
			break;
	}
	if(pwm >= 0 && pwm < 65){
		PORTB |= (1 << PORTB3);
		ICR1 = 0x000F;
		//OCR1B = 0x000F;
	} else if (pwm >= 65 && pwm < 130){
		PORTB &= ~(1 << PORTB3);
		ICR1 = 0x00FF;
		//OCR1B = 0x00FF;
	} else if(pwm >= 130 && pwm < 195){
		PORTB |= (1 << PORTB3);
		ICR1 = 0x0FFF;
		//OCR1B = 0x0FFF;
	} else if(pwm >= 195 && pwm < 255){
		PORTB &= ~(1 << PORTB3);
		ICR1 = 0xFFFF;
		//OCR1B = 0xFFFF;
	} else if(pwm < 0 || pwm > 255){
		pwm = 0;
	}
}


