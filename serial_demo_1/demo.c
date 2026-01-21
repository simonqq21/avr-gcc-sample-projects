#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

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

volatile uint16_t millis_val;
volatile uint8_t millis_fract;
volatile uint16_t micros_val;
volatile uint16_t overflows;

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

uint16_t millis()
{
    return millis_val;
}

uint16_t micros()
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
#define UART_BUFF_SIZE 50
#define UART_STR_LEN 25
char uart_rx_buff[UART_BUFF_SIZE];
char uart_tx_buff[UART_BUFF_SIZE];
char uart_rx_string[UART_STR_LEN];
uint8_t uart_rx_string_i = 0;
uint8_t uart_rx_buff_head, uart_rx_buff_tail, uart_tx_buff_head, uart_tx_buff_tail;
uint8_t c;
uint8_t newline;

void increment_circular(uint8_t *int_ptr, int limit)
{
    if ((*int_ptr) >= limit - 1)
    {
        (*int_ptr) = 0;
    }
    else
    {
        (*int_ptr)++;
    }
}

int get_circular_difference(uint8_t head, uint8_t tail, uint8_t limit)
{
    // if buffer tail less than buffer size and buffer head less than buffer size,
    // difference is size - tail + head
    if (head >= tail)
        return head - tail;
    else
        return limit - tail + head;
}

volatile struct
{
    uint8_t rx_int;
} intflags;

static void putchr(char c)
{
    while (!(UCSR0A & _BV(UDRE0)))
    {
    }
    UDR0 = c;
}

ISR(USART_RX_vect)
{
    c = UDR0;

    if (!(UCSR0A & _BV(FE0)))
    {
        intflags.rx_int = 1;
        uart_rx_buff[uart_rx_buff_head] = c;
        if (uart_rx_buff[uart_rx_buff_head] == '\n')
        {
            newline = 1;
        }
        increment_circular(&uart_rx_buff_head, UART_BUFF_SIZE);
    }
}

static void printstr(const char *s)
{
    while (*s)
    {
        if (*s == '\n')
            putchr('\r');
        putchr(*s++);
    }
}

// static void strcpytoTXbuff(char *str)
// {
// str
// }

static void printstrfromTXbuff()
{
    while (uart_tx_buff_tail != uart_tx_buff_head)
    {
        putchr(uart_tx_buff[uart_tx_buff_tail]);
        increment_circular(&uart_tx_buff_tail, UART_BUFF_SIZE);
    }
}
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

    // OCR2B = 255;

    /*
    Configure UART
    */
    UCSR0A |= _BV(U2X0);
    UCSR0B |= _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);
    // UCSR0B |= ;
    UCSR0C |= _BV(UCSZ01) | _BV(UCSZ00);
    UBRR0L = 207;

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
            printstr("uart_rx_string: ");
            printstr(uart_rx_string);
            led_brightness = strtoul(uart_rx_string, NULL, 10);
            uart_rx_string_i = 0;
            putchr('\n');
        }

        OCR2B = led_brightness;
    }
}