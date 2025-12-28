// #define __AVR_ATmega328P__
#define F_CPU 16000000UL // 1 MHz

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

uint8_t led1 = PD5;
uint8_t btn1 = PD4;

int main()
{
    // enable pull down interrupts in MCUCR
    MCUCR &= ~_BV(PUD);
    // set btn1 as a digital input
    DDRD &= ~_BV(btn1);
    // set pullup resistor on btn1
    PORTD |= _BV(btn1);
    // set led1 as a digital output
    DDRD |= _BV(led1);
    while (1)
    {
        // read btn1
        if (!(PIND & _BV(btn1)))
        {
            PORTD |= _BV(led1);
        }
        else
        {
            PORTD &= ~_BV(led1);
        }
    }
}