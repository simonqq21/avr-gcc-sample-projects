#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif
#ifndef F_CPU
#define F_CPU 16000000UL
#endif
// #include <avr/io.h>
// #include <avr/sleep.h>
#include <util/delay.h>
// #include <avr/interrupt.h
#include "gpio.h"
#include "millis_micros.h"
// #include <stdint.h>
// #include <stdlib.h>
// #include <stdio.h>
#include <string.h>
#include "uart.h"

#define BAUD_RATE 9600
#define MYUBRR F_CPU / 16 / BAUD_RATE - 1

#define LED_PIN PB5
#define LED_PORT PORTB
#define LED_DDR DDRB

#define DEBUG_LED_PIN PB1

Pin led_pin, debug_led_pin;

// uint8_t led_brightness;

volatile uint32_t t_millis;

char buff[60];

ISR(BADISR_vect)
{
    // user code here
    gpio_set_pin_high(&debug_led_pin);
}

ISR(TIMER0_COMPA_vect)
{
    millis_timer_ISR_loop();
}

ISR(USART_RX_vect)
{
    getchar_ISR();
}

void ioinit(void)
{
    led_pin.ddr = &LED_DDR;
    led_pin.port = &LED_PORT;
    led_pin.pin_num = LED_PIN;
    gpio_set_pin_output(&led_pin);
    debug_led_pin.ddr = &LED_DDR;
    debug_led_pin.port = &LED_PORT;
    debug_led_pin.pin_num = DEBUG_LED_PIN;
    gpio_set_pin_output(&debug_led_pin);
    gpio_set_pin_low(&debug_led_pin);

    // TCCR2A |= _BV(COM2B1);
    // TCCR2A |= _BV(WGM20);
    // TCCR2B |= _BV(CS22);
}

int main()
{
    ioinit();
    init_uart(MYUBRR);
    millis_timer_init();

    // Put data into buffer, sends the data
    // cli();
    printchar('a');
    printchar('b');

    printchar('c');
    printchar('d');
    printchar('\n');
    // sei();

    char *str = "helloworldA";
    printstr(str, strlen(str));
    // led_brightness = 5;

    while (1)
    {
        gpio_set_pin_high(&led_pin);
        _delay_ms(500);
        gpio_set_pin_low(&led_pin);
        _delay_ms(500);
    }

    // if (newline)
    // {
    // newline = 0;
    // while (uart_rx_buff_tail != uart_rx_buff_head)
    // {
    //     // putchr('0');
    //     uart_rx_string[uart_rx_string_i] = uart_rx_buff[uart_rx_buff_tail];
    //     uart_rx_string_i++;
    //     inc_ring_buf_ptr(&uart_rx_buff_tail, UART_BUFF_SIZE);
    // }

    // // process the complete string
    // printstr("uart_rx_string: ");
    // printstr(uart_rx_string);

    // led_brightness = strtoul(uart_rx_string, NULL, 10);
    // uart_rx_string_i = 0;
    // printchar('\n');

    // snprintf(buff, 60, "millis = %lu, micros = %lu\n", millis(), micros());
    // printstr(buff);

    // OCR2B = led_brightness;
}
