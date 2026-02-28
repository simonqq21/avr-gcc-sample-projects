#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif
#include "timelib.h"

/*
millis and micros values
*/
volatile uint32_t _millis;
volatile uint8_t _millis_fract;
volatile uint32_t _micros;
volatile uint32_t overflows;

/*
timer0 ISR
*/
ISR(TIMER0_OVF_vect)
{
    _millis++;
    overflows++;
    _millis_fract += MILLIS_FRACT_INC;
    if (_millis_fract >= MILLIS_FRACT_MAX)
    {
        _millis_fract -= MILLIS_FRACT_MAX;
        _millis++;
    }
}

/*
return millis value
*/
uint32_t millis()
{
    uint32_t m;
    cli();
    m = _millis;
    sei();
    return m;
}

/*
return micros values
*/
uint32_t micros()
{
    uint32_t m;
    cli();
    m = overflows * 1024 + TCNT0 * 4;
    sei();
    // if (TIFR0 & _BV(TOV0) && TCNT0 < 50)
    //     overflows += 1;
    return m;
}

void init_millis_timer()
{
    /*
    Configure Timer 0
    */
    TCCR0A = _BV(WGM01) | _BV(WGM00);
    TCCR0B = _BV(CS01) | _BV(CS00);
    TIMSK0 = _BV(TOIE0);
}