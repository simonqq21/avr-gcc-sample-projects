#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif
#define F_CPU 16000000UL // 1 MHz

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

uint8_t led1 = PB5;

int main()
{
    // led1 = PD5;
    DDRB |= _BV(led1);
    while (1)
    {
        PORTB = PORTB | _BV(led1);
        _delay_ms(500);
        PORTB = PORTB & ~_BV(led1);
        _delay_ms(1000);
    }
}
