#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "uart.h"

#define LED_PIN PD3
#define LED_PORT PORTD
#define LED_DDR DDRD

#define F_CPU 16000000UL

uint8_t led_brightness;

/*
****************************************************************
millis and micros code
*/
#define MILLIS_FRACT_INC 24 >> 3
#define MILLIS_FRACT_MAX 1000 >> 3

volatile uint32_t millis_val;
volatile uint8_t millis_fract;
volatile uint32_t micros_val;
volatile uint16_t overflows;

char buff[60];

ISR(TIMER0_OVF_vect)
{
    millis_val++;
    overflows++;
    millis_fract += MILLIS_FRACT_INC;
    if (millis_fract >= MILLIS_FRACT_MAX)
    {
        millis_fract -= MILLIS_FRACT_MAX;
        millis_val++;
    }
}

uint32_t millis()
{
    return millis_val;
}

uint32_t micros()
{
    uint16_t cur_overflows = overflows;
    if (TIFR0 & _BV(TOV0) && TCNT0 < 50)
        cur_overflows += 1;
    return cur_overflows * 1024 + TCNT0 * 4;
}

/*
****************************************************************
UART code
*/

/*
****************************************************************

*/

void ioinit(void)
{
    /*
    Configure IO
    */
    LED_DDR |= _BV(LED_PIN);

    /*
    Configure timer0 and PWM
    */
    TCCR0A |= _BV(COM0A1);
    TCCR0A |= _BV(WGM00);
    TCCR0B |= _BV(CS01) | _BV(CS00);
    TIMSK0 |= _BV(TOIE0);

    TCCR2A |= _BV(COM2B1);
    TCCR2A |= _BV(WGM20);
    TCCR2B |= _BV(CS22);

    init_uart();

    sei();
}

int main()
{
    ioinit();
    printstr("helloworldA");
    led_brightness = 5;

    while (1)
    {

        if (newline)
        {
            newline = 0;
            while (uart_rx_buff_tail != uart_rx_buff_head)
            {
                // putchr('0');
                uart_rx_string[uart_rx_string_i] = uart_rx_buff[uart_rx_buff_tail];
                uart_rx_string_i++;
                increment_circular(&uart_rx_buff_tail, UART_BUFF_SIZE);
            }

            // process the complete string
            printstr("uart_rx_string: ");
            printstr(uart_rx_string);

            led_brightness = strtoul(uart_rx_string, NULL, 10);
            uart_rx_string_i = 0;
            putchr('\n');

            snprintf(buff, 60, "millis = %lu, micros = %lu\n", millis(), micros());
            printstr(buff);
        }

        OCR2B = led_brightness;
    }
}