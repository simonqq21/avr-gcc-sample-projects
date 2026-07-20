#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif
#include "millis_micros.h"

/*
ISR overflow counter
Volatile because it's modified in an ISR
*/
volatile uint32_t millis;

/**
 * ISR TIMER0_COMPA_VECT runs every 1ms
 */
void millis_timer_ISR_loop(void)
{
    millis++;
}

/**
 * @brief configure timer 0
 */
void millis_timer_init(void)
{
    // TCCR0A = 0x00;
    // TCCR0B = 0x00;
    // TCCR0B |= _BV(CS01) | _BV(CS00);
    // TIMSK0 |= _BV(TOIE0);
    // sei();
    millis = 0;

    TCCR0A = 0x00;
    TCCR0B = 0x00;
    // WGM0 = 0b010, CTC mode
    // COM0A = 0b00, COM0B = 0b00, normal port operation
    TCCR0A |= _BV(WGM01);
    // CS0 = 0b011, prescaler 64
    // each tick is 4 us
    TCCR0B |= _BV(CS01) | _BV(CS00);
    // TCCR0B |= _BV(CS02) | _BV(CS00);
    // TCNT0 counts from 0 to 249 (restart every 250 counts)
    OCR0A = 249;
    OCR0B = 0;
    // OCIE0B = 0b1, OCIE0A = 0b1
    TIMSK0 |= _BV(OCIE0A);
    // TIMSK0 |= _BV(OCIE0B);
    // TIMSK0
    // TIFR0
    sei();
}

/**
 * @brief get current milliseconds since boot
 */
uint32_t get_millis(void)
{
    uint32_t m;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        m = millis;
    }
    return m;
}

/**
 * @brief get current microseconds since boot
 *  get_micros has a resolution of 4us.
 */
uint32_t get_micros(void)
{
    uint32_t m;
    uint8_t t;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        // m = micros;
        m = millis;
        t = TCNT0;
        // if timer has overflowed but ISR hasn't run yet
        if ((TIFR0 & (1 << OCF0A)) && (t < 249))
        {
            m++;
        }
        sei();
    }
    return m * 1000 + t * 4;
}