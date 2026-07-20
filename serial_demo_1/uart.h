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

/**
 * @brief initialize UART
 */
void init_uart(unsigned int ubrr);

/**
 * @brief increment ring buffer pointer
 */
void inc_ring_buf_ptr(uint8_t *int_ptr, int limit);

/**
 * @brief get the gap between ring buffer head and tail pointers
 */
int get_ring_buf_head_tail_gap(uint8_t head, uint8_t tail, uint8_t limit);

/**
 * @brief send character via UART
 */
void printchar(unsigned char c);

/**
 * send string of characters via UART
 */
void printstr(const char *s, unsigned int len);

/**
 * @brief get character from UART
 */
char getchar_blocking(void);

/**
 * @brief ISR function that runs when character has been received over UART.
 *
 */
void getchar_ISR(void);

/**
 * read string from the UART RX buffer
 */
void read_rx_buffer(char *s, unsigned int *len);

/**
 *
 */
void printstrfromTXbuff(void);

#endif