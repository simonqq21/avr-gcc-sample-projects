#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#ifndef F_CPU
#define F_CPU 16000000
#endif

#include <avr/io.h>
#include <util/twi.h>
#include <avr/interrupt.h>
#include <stdint.h>

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

// ----------------------------------------------------------------
// i2c functions
// ----------------------------------------------------------------

// ----------------------------------------------------------------
// defines
// ----------------------------------------------------------------
#define I2C_SCL_FREQUENCY_400K 400000UL
#define I2C_SCL_FREQUENCY_100K 100000UL

#define I2C_READ 0x01
#define I2C_WRITE 0x00

// i2c errors
#define I2C_STATUS_SUCCESS 0
#define I2C_STATUS_ERROR_START_NOT_ACCEPTED 10
#define I2C_STATUS_ERROR_WRITE_OR_READ_NOT_ACK 20
#define I2C_STATUS_ERROR_WRITE_NOT_ACK 21
#define I2C_STATUS_ERROR_READ_NOT_ACK 22

#define I2C_SCL_FREQUENCY_PRESCALER 1
#define I2C_PRESCALER_MASK 0xF8

// ----------------------------------------------------------------
// internal functions
// ----------------------------------------------------------------
/*
function to send i2c start
*/
uint8_t i2c_master_start(uint8_t addr, uint8_t mode)
{
    uint8_t twst;

    // reset control register
    TWCR = 0;

    // transmit start condition
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);

    // wait for start condition to be sent
    while (!(TWCR & _BV(TWINT)))
        ;

    // check if the start condition was successfully transmitted
    // check value of TWI status register
    // #define TW_STATUS (TWSR & TW_STATUS_MASK)
    twst = TW_STATUS & I2C_PRESCALER_MASK;
    if (twst != TW_START && twst != TW_REP_START)
    {
        return I2C_STATUS_ERROR_START_NOT_ACCEPTED;
    }

    // load slave address to TWDR with specified mode
    // mode is 1 for read, 0 for write
    // TWDR = (PCF8574_ADDR << 1) + TW_WRITE;
    TWDR = (addr << 1) | mode;

    // start transmission of address
    // Clear TWINT bit in TWCR
    TWCR = _BV(TWINT) | _BV(TWEN);

    // wait for end of transmission
    // Wait for TWINT flag set. This
    // indicates that the SLA+W has
    // been transmitted, and
    // ACK/NACK has been received.
    while (!(TWCR & _BV(TWINT)))
        ;

    // Check value of TWI status
    // register. Mask prescaler bits. If
    // status different from
    // MT_SLA_ACK go to ERROR
    twst = TW_STATUS & I2C_PRESCALER_MASK;
    if (twst != TW_MT_SLA_ACK && twst != TW_MR_SLA_ACK)
    {
        return I2C_STATUS_ERROR_WRITE_OR_READ_NOT_ACK;
    }

    return I2C_STATUS_SUCCESS;
}

/*
function to write one byte to the i2c peripheral
*/
uint8_t i2c_master_write(uint8_t data)
{
    uint8_t twst;

    // Load DATA into TWDR register.
    TWDR = data;

    // clear TWINT bit in TWCR to
    // start transmission of data
    TWCR = _BV(TWINT) | _BV(TWEN);

    // wait for end of transmission
    // Wait for TWINT flag set. This
    // indicates that the DATA has
    // been transmitted, and
    // ACK/NACK has been received.
    while (!(TWCR & _BV(TWINT)))
        ;

    // Check value of TWI status
    // register. Mask prescaler bits. If
    // status different from
    // MT_DATA_ACK go to ERROR
    twst = TW_STATUS & I2C_PRESCALER_MASK;
    if (twst != TW_MT_DATA_ACK)
    {
        return I2C_STATUS_ERROR_WRITE_NOT_ACK;
    }
    return I2C_STATUS_SUCCESS;
}

/*
function to read one byte from the i2c peripheral and send an ACK
*/
uint8_t i2c_master_read_ACK(void)
{
    // setup i2c bus to read
    // TWINT - reset TWI
    // TWEN - enable TWI
    // TWEA - TWI enable ACK
    TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);

    // wait for end of transmission
    while (!(TWCR & _BV(TWINT)))
        ;

    return TWDR;
}

/*
function to read one byte from the i2c peripheral and send an NACK
*/
uint8_t i2c_master_read_NACK(void)
{
    // setup i2c bus to read
    // TWINT - reset TWI
    // TWEN - enable TWI
    // NACK
    TWCR = _BV(TWINT) | _BV(TWEN);

    // wait for end of transmission
    while (!(TWCR & _BV(TWINT)))
        ;

    return TWDR;
}

/*
function to send i2c stop
*/
uint8_t i2c_master_stop(void)
{
    // Transmit STOP condition
    TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTO);
}

// ----------------------------------------------------------------
// exposed functions
// ----------------------------------------------------------------

/*
function to initialize i2c
*/
uint8_t i2c_master_init(uint16_t frequency)
{
    // reset TWI status register
    TWSR = 0;
    // set TWBR value based on the formula
    /*
    scl = f_cpu / (16 + 2 * TWBR * prescaler)
    16 + 2 * TWBR * prescaler = f_cpu / scl
    2 * TWBR * prescaler = f_cpu / scl - 16
    TWBR = (f_cpu / scl - 16) / 2 / prescaler
    */
    TWBR = (F_CPU / frequency - 16) / 2 / I2C_SCL_FREQUENCY_PRESCALER;
}

/*
function to send one byte via i2c
*/
uint8_t i2c_master_send_byte(uint8_t address, uint8_t data)
{
    // send i2c start

    // check status for error

    // write byte to i2c slave

    // check status for error

    // send i2c stop

    return I2C_STATUS_SUCCESS;
}

/*
function to send n bytes via i2c
*/
uint8_t i2c_master_send(uint8_t address, uint8_t *data, uint16_t length)
{
}

/*
function to receive n bytes via i2c
*/
uint8_t i2c_master_receive(uint8_t address, uint8_t *data, uint16_t length)
{
}

/*
function to get status code (error codes) of the i2c bus
*/
char *i2c_master_get_error_message()
{
}

// ----------------------------------------------------------------
// i2c LCD functions
// ----------------------------------------------------------------
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

// ----------------------------------------------------------------

void ioinit(void)
{
    ERROR_LED_DDR |= _BV(ERROR_LED_1_PIN);
    ERROR_LED_DDR |= _BV(ERROR_LED_2_PIN);
    ERROR_LED_DDR |= _BV(ERROR_LED_3_PIN);
    ERROR_LED_PORT &= ~_BV(ERROR_LED_1_PIN);
    ERROR_LED_PORT &= ~_BV(ERROR_LED_2_PIN);
    ERROR_LED_PORT &= ~_BV(ERROR_LED_3_PIN);
}

int main(void)
{
    ioinit();

    while (1)
    {
    }
}