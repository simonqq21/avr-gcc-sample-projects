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

void init_uart(unsigned int ubrr)
{
    // set baud rate
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    // UBRR0 = 207;
    // double UART transmission speed
    // UCSR0A |= _BV(U2X0);
    // enable UART RX complete interrupt
    UCSR0B |= _BV(RXCIE0);
    // enable UART receiver and transmitter
    UCSR0B |= _BV(RXEN0) | _BV(TXEN0);
    // 8-bit frame size
    UCSR0C |= _BV(UCSZ01) | _BV(UCSZ00);
    // 2 stop bits
    UCSR0C |= (1 << USBS0);
}

void inc_ring_buf_ptr(uint8_t *int_ptr, int limit)
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

int get_ring_buf_head_tail_gap(uint8_t head, uint8_t tail, uint8_t limit)
{
    // if buffer tail less than buffer size and buffer head less than buffer size,
    // difference is size - tail + head
    if (head >= tail)
        return head - tail;
    else
        return limit - tail + head;
}

void printchar(unsigned char c)
{
    // Wait for empty transmit buffer
    while (!(UCSR0A & (1 << UDRE0)))
    {
    }
    // Put data into buffer, sends the data
    UDR0 = c;
}

void printstr(const char *s, unsigned int len)
{
    while (len > 0)
    {
        printchar(*s++);

        len--;
    }
}

char getchar_blocking(void)
{
    // Wait for data to be received
    while (!(UCSR0A & _BV(RXC0)))
        ;
    // Get and return received data from buffer
    return UDR0;
}

void getchar_ISR(void)
{
    // grab character from UDR0
    c = UDR0;
    // if no errors, append it to ring buffer.
    if (UCSR0A & _BV(FE0))
    {
        uart_rx_buff[uart_rx_buff_head] = c;
        if (c == '\n')
        {
            newline = 1;
        }
        inc_ring_buf_ptr(&uart_rx_buff_head, UART_BUFF_SIZE);
    }
}

void read_rx_buffer(char *s, unsigned int *len)
{
    while (uart_rx_buff_tail != uart_rx_buff_head)
    {
        *s++ = uart_rx_buff[uart_rx_buff_tail];
        inc_ring_buf_ptr(&uart_rx_buff_tail, UART_BUFF_SIZE);
        (*len)++;
    }
}

// unsigned char dummy;
// while (UCSRnA & (1<<RXCn)) dummy = UDRn;
void printstrfromTXbuff(void)
{
    while (uart_tx_buff_tail != uart_tx_buff_head)
    {
        printchar(uart_tx_buff[uart_tx_buff_tail]);
        inc_ring_buf_ptr(&uart_tx_buff_tail, UART_BUFF_SIZE);
    }
}
