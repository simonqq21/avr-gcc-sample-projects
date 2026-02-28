#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#define UART_BUFF_SIZE 50
#define UART_STR_LEN 25

extern char uart_rx_buff[UART_BUFF_SIZE];
extern char uart_tx_buff[UART_BUFF_SIZE];
extern char uart_rx_string[UART_STR_LEN];
extern uint8_t uart_rx_string_i;
extern uint8_t uart_rx_buff_head, uart_rx_buff_tail, uart_tx_buff_head, uart_tx_buff_tail;
extern uint8_t c;
extern uint8_t newline;

volatile struct
{
    uint8_t rx_int;
} intflags;

void init_uart(void);
void increment_circular(uint8_t *int_ptr, int limit);
int get_circular_difference(uint8_t head, uint8_t tail, uint8_t limit);
void putchr(char c);
void printstr(const char *s);
static void printstrfromTXbuff(void);

#endif