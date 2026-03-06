#include "i2c_master.h"

#ifndef F_CPU
#define F_CPU 0
#endif

// ----------------------------------------------------------------
// i2c functions
// ----------------------------------------------------------------

// TWI data
uint8_t twi_data;

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
void i2c_master_stop(void)
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
void i2c_master_init(uint16_t frequency)
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
    uint8_t status = i2c_master_start(address, I2C_WRITE);
    // check status for error
    if (status)
        return status;
    // write byte to i2c slave
    status = i2c_master_write(data);
    // check status for error
    if (status)
        return status;
    // send i2c stop
    i2c_master_stop();
    return I2C_STATUS_SUCCESS;
}

/*
function to send n bytes via i2c
*/
uint8_t i2c_master_send(uint8_t address, uint8_t *data, uint16_t length)
{
    // send i2c start
    uint8_t status = i2c_master_start(address, I2C_WRITE);
    // check status for error
    if (status)
        return status;
    // send bytes
    for (uint16_t i = 0; i < length; i++)
    {
        // write byte to i2c slave
        status = i2c_master_write(data[i]);
        // check status for error
        if (status)
            return status;
    } // send i2c stop
    i2c_master_stop();
    return I2C_STATUS_SUCCESS;
}

/*
function to receive n bytes via i2c
*/
uint8_t i2c_master_receive(uint8_t address, uint8_t *data, uint16_t length)
{
    // send i2c start
    uint8_t status = i2c_master_start(address, I2C_READ);
    // check status for error
    if (status)
        return status;
    // send bytes
    for (uint16_t i = 0; i < (length - 1); i++)
    {
        // read byte from i2c slave
        data[i] = i2c_master_read_ACK();
    }
    // read the last byte then return NACK to slave
    data[length - 1] = i2c_master_read_NACK();
    // send i2c stop
    i2c_master_stop();
    return I2C_STATUS_SUCCESS;
}

/*
function to get status code (error codes) of the i2c bus
*/
const char *i2c_master_get_error_message(uint8_t errorCode)
{
    switch (errorCode)
    {
    case I2C_STATUS_ERROR_START_NOT_ACCEPTED:
        return "Slave device did not accept start condition.";
    case I2C_STATUS_ERROR_READ_NOT_ACK:
        return "Slave device did not acknowledge read.";
    case I2C_STATUS_ERROR_WRITE_NOT_ACK:
        return "Slave device did not acknowledge write.";
    case I2C_STATUS_ERROR_WRITE_OR_READ_NOT_ACK:
        return "Slave device did not acknowledge read or write.";
    case I2C_STATUS_SUCCESS:
        return "i2c ok.";
    default:
        return "Unknown i2c error code.";
    }
}
