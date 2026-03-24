#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif
#include "timelib.h"

/*
ISR overflow counter
Volatile because it's modified in an ISR
*/
volatile uint32_t overflows = 0;

/*
timer0 ISR
*/
ISR(TIMER0_OVF_vect)
{
    overflows++;
}

/*
return millis value
*/
uint32_t millis()
{
    return micros() / 1000;
}

/*
return micros values
*/
uint32_t micros()
{
    uint32_t m;
    uint8_t t;

    // save interrupt settings
    uint8_t sreg = SREG;
    // disable interrupts to prevent race condition
    cli();

    m = overflows;
    t = TCNT0;

    /*
        If an overflow just happened but the ISR hasn't run yet,
        manually account for it.
        TOV0 in TIFR0 is set if overflow happened but ISR
        hasn't run yet.
        */
    if (TIFR0 & _BV(TOV0) && (t < 255))
    {
        m++;
    }

    // restore interrupts
    SREG = sreg;

    /*
    compute microseconds
    overflows * 128 us + current ticks * 0.5 us
    */
    return (m << 7) + t >> 1;
}

/*
Configure Timer 0
*/
void init_millis_timer()
{
    // 1. Normal Mode (WGM01, WGM00, WGM02 all 0)
    TCCR0A = 0x00;
    TCCR0B = 0x00;

    // 2. Enable Overflow Interrupt
    TIMSK0 |= (1 << TOIE0);

    // 3. Set Prescaler to 8 and Start
    // 16MHz / 8 = 2MHz (0.5us per tick)
    TCCR0B |= (1 << CS01);

    sei();
}