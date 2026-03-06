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
#include <util/delay.h>

#include "src/i2c_master.h"

// leds
#define ERROR_LED_1_PIN PD4
#define ERROR_LED_2_PIN PD5
#define ERROR_LED_3_PIN PD6
#define ERROR_LED_PORT PORTD
#define ERROR_LED_DDR DDRD

// 7-bit i2c address of PCF8574
#define PCF8574_ADDR 0x3F

// PCF8574 to LCD pinout
// D7 D6 D5 D4 BL E RW RS

// ----------------------------------------------------------------
// i2c LCD functions
// ----------------------------------------------------------------

// ----------------------------------------------------------------
// defines
// ----------------------------------------------------------------

// ----------------------------------------------------------------
// LCD commands
// ----------------------------------------------------------------
#define LCD_CLEAR_DISPLAY 0x01
#define LCD_RETURN_HOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// ----------------------------------------------------------------
// LCD flags
// ----------------------------------------------------------------

// ----------------------------------------------------------------
// LCD display entry mode
// ----------------------------------------------------------------

// ----------------------------------------------------------------
// LCD display control
// ----------------------------------------------------------------

// ----------------------------------------------------------------
// LCD display cursor shift
// ----------------------------------------------------------------

// ----------------------------------------------------------------
// LCD function set
// ----------------------------------------------------------------

// ----------------------------------------------------------------
// LCD backlight
// ----------------------------------------------------------------

// ----------------------------------------------------------------
// LCD E, RW, and RS bits on the PCF8974
// ----------------------------------------------------------------

#define LCD_4BITMODE 0x00

#define LCD_1_LINE 0x00

#define LCD_5x8_FONT 0x00

#define LCD_ON 0x01
#define LCD_OFF 0x00

#define LCD_CURSOR_ON 0x01
#define LCD_CURSOR_OFF 0x00

#define LCD_CURSOR_BLINK_ON 0x01
#define LCD_CURSOR_BLINK_OFF 0x00

typedef struct
{
    uint8_t address;
    uint8_t columns;
    uint8_t rows;
    uint8_t backlight;
    uint8_t displayFunction;
    uint8_t displayControl;
    uint8_t displayMode;

} LiquidCrystalDevice_t;

/*
LCD commands
*/

// ----------------------------------------------------------------
// internal functions
// ----------------------------------------------------------------

// ----------------------------------------------------------------
// exposed functions
// ----------------------------------------------------------------
/*
function to init LCD
*/
LiquidCrystalDevice_t LCD_init(uint8_t address,
                               uint8_t columns,
                               uint8_t rows,
                               uint8_t dotSize)
{
    LiquidCrystalDevice_t device;
    device.address = address;
    device.columns = columns;
    device.rows = rows;
    // set display function to 4-bit mode, 1 line, and dotSize (either 5x8 or 5x10)
    device.displayFunction =
        // set display control

        // set display mode to default text direction (for Roman languages)
        return device;
}

/*
Function to clear LCD
*/
void LCD_clear(LiquidCrystalDevice_t *device)
{
}

/*
function to set backlight of LCD
*/
void LCD_set_backlight(LiquidCrystalDevice_t *device, uint8_t bl)
{
}

/*
function to set LCD cursor
*/
void LCD_set_cursor(LiquidCrystalDevice_t *device)
{
}

/*
function to return LCD cursor to home position
*/
void LCD_return_home(LiquidCrystalDevice_t *device)
{
}

/*
function to set LCD display on/off
*/
void LCD_set_display_state(LiquidCrystalDevice_t *device)
{
}

/*
function to set LCD cursor on/off
*/
void LCD_set_cursor_state(LiquidCrystalDevice_t *device)
{
}

/*
function to set LCD cursor blink on/off
*/
void LCD_set_cursor_blink(LiquidCrystalDevice_t *device)
{
}

/*
function to scroll LCD left/right
*/
void LCD_set_scroll_dir(LiquidCrystalDevice_t *device)
{
}

/*
function to set LCD autoscroll on/off
*/
void LCD_set_autoscroll(LiquidCrystalDevice_t *device)
{
}

/*
function to create custom LCD characters
*/
void LCD_create_custom_char(LiquidCrystalDevice_t *device,
                            uint8_t slot,
                            uint8_t charmap[8])
{
}

/*
function to send command to LCD
*/
void LCD_send_command(LiquidCrystalDevice_t *device)
{
}

/*
function to write character to LCD
*/
void LCD_printChar(LiquidCrystalDevice_t *device)
{
}

/*
function to write string to LCD
*/
void LCD_print(LiquidCrystalDevice_t *device)
{
}

/*
function to send byte to LCD
*/
void LCD_write_byte(LiquidCrystalDevice_t *device)
{
}

/*
function to write 4 bits of data to the LCD
The LCD operates in 4-bit mode with the PCF8974.
*/
void LCD_write_4bits(LiquidCrystalDevice_t *device)
{
}

/*
function to transmit data to the LCD via i2c
*/
void LCD_transmitI2C(LiquidCrystalDevice_t *device, uint8_t value)
{
    i2c_master_send_byte(device->address, value | device->backlight);
}
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

    i2c_master_init(I2C_SCL_FREQUENCY_400K);
    i2c_master_send_byte(PCF8574_ADDR, 0x08);
    _delay_ms(5000);
    i2c_master_send_byte(PCF8574_ADDR, 0);
    while (1)
    {
    }
}