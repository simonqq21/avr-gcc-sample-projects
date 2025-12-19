#define __AVR_ATmega328P__
#define F_CPU 16000000UL // 1 MHz

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

uint8_t led1;

int main()
{
    led1 = PD5;
    DDRD |= _BV(led1);
    while (1)
    {
        PORTD = PORTD | _BV(led1);
        _delay_ms(500);
        PORTD = PORTD & ~_BV(led1);
        _delay_ms(1000);
    }

    // DDRD = 1 << PD3;
    // PORTD = 1 << PD3;
    // _delay_ms(500);
    // PORTD = 0 << PD3;
    // _delay_ms(1000);
}

// /* Blinker Demo */

// // ------- Preamble -------- //
// #include <avr/io.h>     /* Defines pins, ports, etc */
// #include <util/delay.h> /* Functions to waste time */
// #include <avr/interrupt.h>

// pb5

//     int
//     main(void)
// {
//     // -------- Inits --------- //
//     DDRB |= 0b00000001; /* Data Direction Register B:
//                           writing a one to the bit
//                           enables output. */

//     // ------ Event loop ------ //
//     while (1)
//     {

//         PORTB = 0b00000001; /* Turn on first LED bit/pin in PORTB */
//         _delay_ms(1000);    /* wait */

//         PORTB = 0b00000000; /* Turn off all B pins, including LED */
//         _delay_ms(1000);    /* wait */

//     } /* End event loop */
//     return 0; /* This line is never reached */
// }