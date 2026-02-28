#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#define F_CPU 16000000

#include <avr/io.h>
#include <util/twi.h>
#include <avr/interrupt.h>

// leds
#define ERROR_LED_1_PIN PD4
#define ERROR_LED_2_PIN PD5
#define ERROR_LED_3_PIN PD6
#define ERROR_LED_PORT PORTD
#define ERROR_LED_DDR DDRD

// 7-bit i2c address of PCF8574
#define PCF8574_ADDR 0x3F

// TWI data
uint8_t twi_data;

// PCF8574 to LCD pinout
// D7 D6 D5 D4 BL E RW RS

// --------------------------------------
/*
function to set backlight of LCD
*/
void LCD_set_backlight(uint8_t state)
{
}

/*
clear display
*/
void LCD_clear_display()
{
}

/*
 */

// --------------------------------------
void ioinit(void)
{
    ERROR_LED_DDR |= _BV(ERROR_LED_1_PIN);
    ERROR_LED_DDR |= _BV(ERROR_LED_2_PIN);
    ERROR_LED_DDR |= _BV(ERROR_LED_3_PIN);
    ERROR_LED_PORT &= ~_BV(ERROR_LED_1_PIN);
    ERROR_LED_PORT &= ~_BV(ERROR_LED_2_PIN);
    ERROR_LED_PORT &= ~_BV(ERROR_LED_3_PIN);
    /*
    setup i2c
    */
    TWBR = ((F_CPU / 100000 - 16) / 2);
    // TWBR = 100;
    TWSR = 0;

    // start condition
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);

    // wait for start condition to be sent
    while (!(TWCR & _BV(TWINT)))
    {
    }

    // check value of TWI status register
    if ((TWSR & 0xF8) != TW_START)
    {
        // error();
        ERROR_LED_PORT |= _BV(ERROR_LED_1_PIN);
    }

    // load slave address to TWDR
    // Clear TWINT bit in TWCR to start transmission of address
    // +1 for read, +0 for write
    TWDR = (PCF8574_ADDR << 1) + TW_WRITE;
    TWCR = _BV(TWINT) | _BV(TWEN);

    // Wait for TWINT flag set. This
    // indicates that the SLA+W has
    // been transmitted, and
    // ACK/NACK has been received.
    while (!(TWCR & _BV(TWINT)))
    {
    }

    // Check value of TWI status
    // register. Mask prescaler bits. If
    // status different from
    // MT_SLA_ACK go to ERROR
    if ((TWSR & 0xF8) != TW_MT_SLA_ACK)
    {
        ERROR_LED_PORT |= _BV(ERROR_LED_2_PIN);
    }
    // ERROR();

    // Load DATA into TWDR register.
    // clear TWINT bit in TWCR to
    // start transmission of data
    twi_data = 0;
    TWDR = twi_data;
    TWCR = _BV(TWINT) | _BV(TWEN);

    // Wait for TWINT flag set. This
    // indicates that the DATA has
    // been transmitted, and
    // ACK/NACK has been received.
    while (!(TWCR & _BV(TWINT)))
    {
    }

    //         Check value of TWI status
    // register. Mask prescaler bits. If
    // status different from
    // MT_DATA_ACK go to ERROR
    if ((TWSR & 0xF8) != TW_MT_DATA_ACK)
    {
        // ERROR();
        ERROR_LED_PORT |= _BV(ERROR_LED_3_PIN);
    }

    // Transmit STOP condition
    TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTO);
}

int main(void)
{
    ioinit();

    while (1)
    {
    }
}