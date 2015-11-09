#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


#define DEGREES_ZERO 	1.0
#define DEGREES_90 		1.5
#define DEGREES_180 	2.0

// global variable to count the number of overflows
volatile uint8_t tot_overflow = 0;
volatile double step = 0.0;
volatile double goto_step = DEGREES_90;

// TIMER0 compare match interrupt - 0.01ms
ISR(TIMER0_COMPA_vect)
{
	PORTB ^= (1 << PB4); 

	// Keep servo pin HIGH for at least 1ms = 0 degrees
	// 2ms = 180 degrees

	if (step >= 0.5)
	{
		// Reset
		PORTB &= ~(1 << PB1); // Servo pin n goes low
		step = 0.5;
	}
	else step += 0.01;
}

// TIMER1 overflow interrupt - 20ms pulse
ISR(TIMER1_OVF_vect)
{
	// keep a track of number of overflows
	tot_overflow++;
}


int main()
{
	// Enable CTC Mode for Timer 0
	TCCR0A |= (1 << WGM01);

	// Start Timer 0 with no prescaler
	TCCR0B |= (1 << CS00);

	// Enable Timer 0 Compare match interrupt
	TIMSK |= (1 << OCIE0A);

	// CTC TOP value for a 0.01ms clock time period
	OCR0A = 79;

	// Timer 1 clock with 256 prescaler
	TCCR1 |= (1 << CS10) | (1 << CS13); // Start clock with 256 prescaler

	// Enable Timer 1 Overflow interrupt
	TIMSK |= (1 << TOIE1);

	// Reset Timer 0
	TCNT0 = 0;

	// Reset Timer 1
	TCNT1 = 0;

	// Enable global interrupts
	sei();


	DDRB |= (1 << PB1);
	DDRB |= (1 << PB3);
	DDRB |= (1 << PB4);

	PORTB &= ~(1 << PB3); // Set LED low
	PORTB &= ~(1 << PB4); // Set LED low
	PORTB &= ~(1 << PB1); // Set Servo pin low

	for (;;)
	{
		// Check if no. of overflows = 2
		if (tot_overflow >= 2)  // NOTE: '>=' is used
		{
			// Check if the timer count reaches 115
			// 20ms pulse complete
			if (TCNT1 >= 115)
			{
				PORTB ^= (1 << PB3); 
				
				TCNT0 = 0;          // reset timer 0
				TCNT1 = 0;          // reset timer 1
				tot_overflow = 0;   // reset overflow counter
				

				// Pull servo pin HIGH
				PORTB |= (1 << PB1); 

			}
		}
	}
}