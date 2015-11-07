#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// global variable to count the number of overflows
volatile uint8_t tot_overflow = 0;
volatile double step = 0.0;

// TIMER0 overflow interrupt
ISR(TIMER0_OVF_vect)
{
	// keep a track of number of overflows
	tot_overflow++;
}

// TIMER1 compare match interrupt
ISR (TIMER1_COMPA_vect)
{
	step += 0.01;

	// 1ms
	if (step <= 1.0)
	{
		// 0 degrees
	}
	else if (step <= 2.0)
	{
		// 90 degrees
	}
	else
	{
		// Reset
		PORTB &= ~(1 << PB1); // Pin n goes low
		// step = 0.0;
	}
}

int main ()
{

	// Enable overflow interrupt
	TIMSK |= (1 << TOIE0);

	// Set prescaler to 256
	TCCR0B |= (1 << CS02);

	// Initialise Timer 0 value
	TCNT0 = 0;

	// Timer 2 - every 0.01ms
	// Initialise Timer 0 value
	TCNT1 = 0;


	// Enable CTC Mode and start the clock
	TCCR1 |= (1 << CTC1) | (1 << CS10);
	
	// This will clear the timer on compare match but won't generate an interrupt
	OCR1C = 79;

	// This will trigger the compare match interrupt
	OCR1A = 79;

	// Enable Compare match interrupt for Timer 1
	TIMSK |= (1 << OCIE1A);		

	// Enable global interrupts
	sei();

	DDRB |= (1 << PB1);
	DDRB |= (1 << PB4);

	for (;;)
	{
		// Check if no. of overflows = 2
		if (tot_overflow >= 2)  // NOTE: '>=' is used
		{
			// Check if the timer count reaches 115
			// 20ms pulse complete
			if (TCNT0 >= 115)
			{
				PORTB ^= (1 << PB4); 
				_delay_ms(2000);

				TCNT0 = 0;          // reset timer 0
				tot_overflow = 0;   // reset overflow counter

				// Pull servo pin HIGH
				PORTB |= (1 << PB1); 

			}
		}
	}
}