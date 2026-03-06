#ifndef I2C_MASTER_H
#define I2C_MASTER_H

#include <avr/io.h>
#include <util/twi.h>
#include <stdint.h>

// ----------------------------------------------------------------
// defines
// ----------------------------------------------------------------
#define I2C_SCL_FREQUENCY_100K 100000UL
#define I2C_SCL_FREQUENCY_400K 400000UL

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
uint8_t i2c_master_start(uint8_t addr, uint8_t mode);
uint8_t i2c_master_write(uint8_t data);
uint8_t i2c_master_read_ACK(void);
uint8_t i2c_master_read_NACK(void);
void i2c_master_stop(void);

// ----------------------------------------------------------------
// exposed functions
// ----------------------------------------------------------------
void i2c_master_init(uint16_t frequency);
uint8_t i2c_master_send_byte(uint8_t address, uint8_t data);
uint8_t i2c_master_send(uint8_t address, uint8_t *data, uint16_t length);
uint8_t i2c_master_receive(uint8_t address, uint8_t *data, uint16_t length);
const char *i2c_master_get_error_message(uint8_t errorCode);

#endif
