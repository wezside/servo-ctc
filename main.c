#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "utilz.h"



volatile double reading = 0;
volatile long tot_overflow = 0;
volatile long reading_map = 0;

double read_analog();

ISR(TIMER0_COMPA_vect)
{
	tot_overflow++;
	if (tot_overflow >= reading_map)
	{
		// Servo pin low
		PORTB &= ~(1 << PB1); 
		// PORTB ^= (1 << PB3); 
	}

	if (tot_overflow >= 2000)
	{
		tot_overflow = 0;
		TCNT0 = 0;
		
		// Servo pin high
		PORTB |= (1 << PB1); 
	}
}

ISR(PCINT0_vect)
{
	send_debug(reading_map);
}

void init_adc()
{
	//Configure ADMUX register
	ADMUX =
	 (1 << ADLAR)| 		// Shift in a 1 and follow 8bit procedure
	 (1 << MUX0)| 		// Use ADC1 or PB2 pin for Vin
	 (0 << REFS0)| 		// set refs0 and 1 to 0 to use Vcc as Vref
	 (0 << REFS1);
	
	//Configure ADCSRA register
	ADCSRA =
	 (1 << ADEN)| //set ADEN bit to 1 to enable the ADC
	 (0 << ADSC); //set ADSC to 0 to make sure no conversions are happening
}

void disable_adc()
{
	ADCSRA &= ~(1 << ADEN);
}

double read_analog()
{
	ADCSRA |= (1 << ADSC);
	while (((ADCSRA >> ADSC) & 1)){}
	return ADCH;
}

void init_timer_0()
{
	// Enable CTC Mode for Timer 0
	TCCR0A |= (1 << WGM01) | (1 << COM0A1);

	// Start Timer 0 with prescaler CK/8
	TCCR0B |= (1 << CS01);

	// Enable Timer 0 Compare match interrupt
	TIMSK |= (1 << OCIE0A);

	// CTC TOP value 
	OCR0A = 10;	

	// Reset Timer 0
	TCNT0 = 0;
}

void init_timer_1()
{
	// Timer 1 clock with 64 prescaler
	TCCR1 |= (1 << CS12) | (1 << CS11) | (1 << CS10); // Start clock with 256 prescaler

	// TOP value 
	OCR1A = 255;

	// Enable Timer 1 Overflow interrupt
	TIMSK |= (1 << TOIE1);

	// Reset Timer 1
	TCNT1 = 0;
}

void init_pin_interrupt(int p)
{	
	GIMSK |= (1 << PCIE);
	PCMSK |= (1 << p);
}

int main()
{
	init_timer_0();

	init_pin_interrupt(PCINT0);

	init_adc();

	util_init();


	// Outputs
	DDRB |= (1 << PB1);
	DDRB |= (1 << PB3);
	DDRB |= (1 << PB4);

	PORTB &= ~(1 << PB3); // Set LED low
	PORTB &= ~(1 << PB1); // Set Servo pin low

	// Inputs	
	DDRB &= ~(1 << PB0); /* Set PB0 as input */
    PORTB |= (1 << PB0); /* Activate PULL UP resistor */ 


	// Toggle LED
	PORTB ^= (1 << PB3); 
	_delay_ms(1000);
	PORTB ^= (1 << PB3); 	

	// Enable global interrupts
	sei();

	while(1)
	{
		reading = read_analog();
		reading_map = map(reading, 0, 255, 410, 1580, 1) / 1000.0 / 0.01;	
	}
}