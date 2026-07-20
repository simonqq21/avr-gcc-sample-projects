#ifndef GPIO_H
#define GPIO_H

#include <avr/io.h>

/**
 * pin struct
 * PORT
 * PIN
 * DDR
 * pin number
 */
typedef struct
{
    volatile uint8_t *ddr;
    volatile uint8_t *port;
    volatile uint8_t *pin;
    uint8_t pin_num;
} Pin;

/**
 * @brief GPIO set pin output
 *
 * @param pin the GPIO Pin to set as output
 */
void gpio_set_pin_output(Pin *pin);

/**
 * @brief GPIO set pin input
 *
 * @param pin the GPIO Pin to set as input
 */
void gpio_set_pin_input(Pin *pin);

/**
 * @brief GPIO set pin pulled up input
 *
 * @param pin the GPIO Pin to set as pulled up input
 */
void gpio_set_pin_input_pullup(Pin *pin);

/**
 * @brief GPIO toggle pin mode
 *
 * @param pin the GPIO Pin to toggle pin mode
 */
void gpio_toggle_pin_mode(Pin *pin);

/**
 * @brief GPIO read pin
 *
 * @param pin the GPIO pin to read
 * @return the logic value of the pin
 */
uint8_t gpio_read_pin(Pin *pin);

/**
 * @brief GPIO set pin output
 *
 * @param pin the GPIO pin to set output
 */
void gpio_set_pin_val(Pin *pin, uint8_t value);

/**
 * @brief GPIO set pin high
 *
 * @param pin the GPIO pin to set high
 */
void gpio_set_pin_high(Pin *pin);

/**
 * @brief GPIO set pin low
 *
 * @param pin the GPIO pin to set low
 */
void gpio_set_pin_low(Pin *pin);

/**
 * @brief GPIO toggle pin output
 *
 * @param pin the GPIO pin to toggle pin output
 */
void gpio_toggle_pin_output(Pin *pin);

#endif // GPIO_H
