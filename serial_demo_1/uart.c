#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#include "uart.h"

char uart_rx_buff[UART_BUFF_SIZE];
char uart_tx_buff[UART_BUFF_SIZE];
char uart_rx_string[UART_STR_LEN];
uint8_t uart_rx_string_i = 0;
uint8_t uart_rx_buff_head, uart_rx_buff_tail, uart_tx_buff_head, uart_tx_buff_tail;
uint8_t c;
uint8_t newline;

void init_uart(void)
{
    /*
Configure UART
*/
    UCSR0A |= _BV(U2X0);
    UCSR0B |= _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);
    // UCSR0B |= ;
    UCSR0C |= _BV(UCSZ01) | _BV(UCSZ00);
    UBRR0L = 207;
}

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

void putchr(char c)
{
    while (!(UCSR0A & _BV(UDRE0)))
    {
    }
    UDR0 = c;
}

void printstr(const char *s)
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

void printstrfromTXbuff(void)
{
    while (uart_tx_buff_tail != uart_tx_buff_head)
    {
        putchr(uart_tx_buff[uart_tx_buff_tail]);
        increment_circular(&uart_tx_buff_tail, UART_BUFF_SIZE);
    }
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