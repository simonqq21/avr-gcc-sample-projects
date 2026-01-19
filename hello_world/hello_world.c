// #define __AVR_ATmega328P__ // comment this out before building
#define F_CPU 16000000UL // 1 MHz

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

uint8_t led1 = PD5;

int main()
{
    // led1 = PD5;
    DDRD |= _BV(led1);
    while (1)
    {
        PORTD = PORTD | _BV(led1);
        _delay_ms(500);
        PORTD = PORTD & ~_BV(led1);
        _delay_ms(1000);
    }
}
