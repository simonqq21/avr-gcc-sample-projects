#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "timelib.h"

/*
IO pins
 */
#define MILLIS_LED_PORT PORTD
#define MILLIS_LED_PIN PD4
#define MILLIS_LED_DDR DDRD
#define MICROS_LED_PORT PORTD
#define MICROS_LED_PIN PD5
#define MICROS_LED_DDR DDRD

/*
LED states
*/
#define MILLIS_LED_STATE 0
#define MICROS_LED_STATE 1
volatile uint8_t leds_states = 0;

/*
previous timestamp vars
 */
volatile uint32_t prevMillis;
volatile uint32_t prevMicros;

void ioinit(void)
{
    /*
    GPIO
    */
    MILLIS_LED_DDR |= _BV(MILLIS_LED_PIN);
    MICROS_LED_DDR |= _BV(MICROS_LED_PIN);

    // TWCR
    sei();
}

int main()
{
    ioinit();
    init_millis_timer();

    while (1)
    {
        if (millis() - prevMillis > 1000)
        {
            prevMillis = millis();
            leds_states ^= _BV(MILLIS_LED_STATE);
        }
        if (micros() - prevMicros > 1000000)
        {
            prevMicros = micros();
            leds_states ^= _BV(MICROS_LED_STATE);
        }

        if (leds_states & _BV(MILLIS_LED_STATE))
        {
            MILLIS_LED_PORT |= _BV(MILLIS_LED_PIN);
        }
        else
        {
            MILLIS_LED_PORT &= ~_BV(MILLIS_LED_PIN);
        };

        if (leds_states & _BV(MICROS_LED_STATE))
        {
            MICROS_LED_PORT |= _BV(MICROS_LED_PIN);
        }
        else
        {
            MICROS_LED_PORT &= ~_BV(MICROS_LED_PIN);
        };
    }
}