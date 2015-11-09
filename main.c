#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// global variable to count the number of overflows
volatile uint8_t tot_overflow = 0;
volatile double step = 0.0;

// TIMER0 overflow interrupt - 20ms pulse
ISR(TIMER0_OVF_vect)
{
	// keep a track of number of overflows
	tot_overflow++;
	
}

// TIMER1 compare match interrupt
ISR (TIMER1_COMPA_vect)
{
	PORTB ^= (1 << PB4); 

	step += 0.01;

	// if ((long)step / 10 % 1 == 0)

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
		PORTB &= ~(1 << PB1); // Servo pin n goes low
		// step = 0.0;
	}
}

int main ()
{
	// Enable Timer 0 overflow interrupt
	// TIMSK |= (1 << TOIE0);

	// TCCR0A |= (0 << WGM00) | (0 << WGM01);

	// Set prescaler to 256
	// TCCR0B |= (1 << CS02) | (0 << WGM02);

	// Enable CTC Mode for Timer 1 and start the clock with no prescaler
	TCCR1 |= (1 << CTC1) | (1 << CS10);
	// TCCR1 |= (1 << CTC1) | (1 << CS11) | (1 << CS13);

	// Enable Compare match interrupt for Timer 1
	TIMSK |= (1 << OCIE1A);

	// Reset Timer 0
	TCNT0 = 0;

	// Reset Timer 1
	TCNT1 = 0;

	// Enable global interrupts
	sei();

	// This will trigger the compare match interrupt
	// OCR1A = 79;

	// CTC TOP value for a 0.01ms clock time period
	OCR1C = 79;

	DDRB |= (1 << PB1);
	DDRB |= (1 << PB4);

	PORTB &= ~(1 << PB4); // Set LED low
	PORTB &= ~(1 << PB1); // 

	for (;;)
	{
		// Check if no. of overflows = 2
		if (tot_overflow >= 2)  // NOTE: '>=' is used
		{
			// Check if the timer count reaches 115
			// 20ms pulse complete
			if (TCNT0 >= 115)
			{
				// PORTB ^= (1 << PB4); 
				
				TCNT0 = 0;          // reset timer 0
				TCNT1 = 0;          // reset timer 1
				tot_overflow = 0;   // reset overflow counter
				

				// Pull servo pin HIGH
				PORTB |= (1 << PB1); 

			}
		}
	}
}