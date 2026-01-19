#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif
#define F_CPU 16000000UL // 16 MHz

#include <avr/io.h>
#include <util/delay.h>

typedef struct
{
    volatile uint8_t *ddr;
    volatile uint8_t *port;
    uint8_t pin;
} PinConfig;

PinConfig leds[] = {
    {&DDRB, &PORTB, PB1},
    {&DDRB, &PORTB, PB0},
    {&DDRD, &PORTD, PD7},
    {&DDRD, &PORTD, PD6},
    {&DDRD, &PORTD, PD5},
    {&DDRD, &PORTD, PD4},
    {&DDRD, &PORTD, PD3}};
uint8_t nLeds = 7;
const uint16_t speed_val = 80;

int main()
{
    // set all LEDs as outputs
    for (int i = 0; i < nLeds; i++)
    {
        *leds[i].ddr |= _BV(leds[i].pin);
    }
    while (1)
    {
        for (int i = 0; i < nLeds; i++)
        {

            *leds[i - 1].port = 0;
            *leds[i].port = _BV(leds[i].pin);
            _delay_ms(speed_val);
        }
        for (int i = nLeds - 2; i > 0; i--)
        {
            *leds[i + 1].port = 0;
            *leds[i].port = _BV(leds[i].pin);
            _delay_ms(speed_val);
        }
    }
}