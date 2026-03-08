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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/i2c_master.h"

// leds
#define ERROR_LED_1_PIN PD4
#define ERROR_LED_2_PIN PD5
#define ERROR_LED_3_PIN PD6
#define ERROR_LED_PORT PORTD
#define ERROR_LED_DDR DDRD

// 7-bit i2c address of PCF8574
#define PCF8574_ADDR 0x3F

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
#define LCD_ENTRY_LEFT 0x02
#define LCD_ENTRY_RIGHT 0x00
#define LCD_ENTRY_SHIFT_INCREMENT 0x01 // accompanies display shift
#define LCD_ENTRY_SHIFT_DECREMENT 0x00

// ----------------------------------------------------------------
// LCD display control
// ----------------------------------------------------------------
#define LCD_DISPLAY_ON 0x04
#define LCD_DISPLAY_OFF 0x00
#define LCD_CURSOR_ON 0x02
#define LCD_CURSOR_OFF 0x00
#define LCD_CURSOR_BLINK 0x01
#define LCD_CURSOR_NOBLINK 0x00

// ----------------------------------------------------------------
// LCD display cursor shift
// ----------------------------------------------------------------
#define LCD_DISPLAY_SHIFT 0x08
#define LCD_CURSOR_MOVE 0x00
#define LCD_SHIFT_RIGHT 0x04
#define LCD_SHIFT_LEFT 0x00

// ----------------------------------------------------------------
// LCD function set
// ----------------------------------------------------------------
#define LCD_DATA_LENGTH_8BITS 0x10
#define LCD_DATA_LENGTH_4BITS 0x00
#define LCD_DISPLAY_2LINES 0x08
#define LCD_DISPLAY_1LINE 0x00
#define LCD_CHARACTER_FONT_5X10 0x04
#define LCD_CHARACTER_FONT_5X8 0x00

// ----------------------------------------------------------------
// LCD backlight
// ----------------------------------------------------------------
#define LCD_BACKLIGHT_ON 0x08
#define LCD_BACKLIGHT_OFF 0x00

// ----------------------------------------------------------------
// LCD E, RW, and RS bits on the PCF8974
// PCF8574 to LCD pinout
// D7 D6 D5 D4 BL E RW RS
// ----------------------------------------------------------------
#define LCD_E_PCF8974 0x04
#define LCD_RW_PCF8974 0x02
#define LCD_RS_PCF8974 0x01

// ----------------------------------------------------------------
// LCD device struct
// ----------------------------------------------------------------
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

LiquidCrystalDevice_t LCD_init(uint8_t address,
                               uint8_t columns,
                               uint8_t rows,
                               uint8_t dotSize);
void LCD_clear(LiquidCrystalDevice_t *device);
void LCD_set_backlight(LiquidCrystalDevice_t *device, uint8_t bl);
void LCD_set_cursor(LiquidCrystalDevice_t *device, uint8_t row, uint8_t column);
void LCD_return_home(LiquidCrystalDevice_t *device);
void LCD_set_display_state(LiquidCrystalDevice_t *device, uint8_t displayState);
void LCD_set_cursor_state(LiquidCrystalDevice_t *device, uint8_t cursor);
void LCD_set_cursor_blink(LiquidCrystalDevice_t *device, uint8_t blink);
void LCD_scroll(LiquidCrystalDevice_t *device, uint8_t direction);
void LCD_set_scroll_dir(LiquidCrystalDevice_t *device, uint8_t direction);
void LCD_set_autoscroll(LiquidCrystalDevice_t *device, uint8_t autoscroll);
void LCD_create_custom_char(LiquidCrystalDevice_t *device,
                            uint8_t slot,
                            uint8_t charmap[8]);
void LCD_send_command(LiquidCrystalDevice_t *device, uint8_t command);
void LCD_print(LiquidCrystalDevice_t *device, char *s);
void LCD_write_byte(LiquidCrystalDevice_t *device, uint8_t value, uint8_t mode);
void LCD_write_4bits(LiquidCrystalDevice_t *device, uint8_t value);
void LCD_transmitI2C(LiquidCrystalDevice_t *device, uint8_t value);
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
    device.displayFunction = LCD_DATA_LENGTH_4BITS | LCD_DISPLAY_1LINE | dotSize;
    // set display control
    device.displayControl = LCD_DISPLAY_ON | LCD_CURSOR_ON | LCD_CURSOR_NOBLINK;
    // set LCD display mode
    // set display mode to default text direction (for Roman languages)
    device.displayMode = LCD_ENTRY_SHIFT_DECREMENT | LCD_ENTRY_LEFT;

    /*
    2 line display
    */
    if (rows > 1)
    {
        device.displayFunction |= LCD_DISPLAY_2LINES;
    }

    /*
    5x10 character font
    */
    if (dotSize > LCD_CHARACTER_FONT_5X8 && rows == 1)
    {
        device.displayFunction |= LCD_CHARACTER_FONT_5X10;
    }

    /*
    commence initialization
    */
    _delay_ms(50);
    LCD_write_4bits(&device, 0x03 << 4);
    _delay_ms(5);
    LCD_write_4bits(&device, 0x03 << 4);
    _delay_ms(5);
    LCD_write_4bits(&device, 0x03 << 4);
    _delay_us(150);

    LCD_write_4bits(&device, 0x02 << 4);
    _delay_us(150);
    LCD_send_command(&device, LCD_FUNCTIONSET | device.displayFunction);

    LCD_set_display_state(&device, LCD_DISPLAY_OFF);
    LCD_clear(&device);
    LCD_send_command(&device, LCD_ENTRYMODESET | device.displayMode);
    LCD_return_home(&device);
    return device;
}

/*
Function to clear LCD
*/
void LCD_clear(LiquidCrystalDevice_t *device)
{
    LCD_send_command(device, LCD_CLEAR_DISPLAY);
    _delay_us(2000);
}

/*
function to set backlight of LCD
*/
void LCD_set_backlight(LiquidCrystalDevice_t *device, uint8_t bl)
{
    if (bl == LCD_BACKLIGHT_ON)
    {
        device->backlight = LCD_BACKLIGHT_ON;
    }
    else
    {
        device->backlight = LCD_BACKLIGHT_OFF;
    }
    /*
    refresh PCF8974 with updated backlight settings
    */
    LCD_transmitI2C(device, 0);
}

/*
function to set LCD cursor
*/
void LCD_set_cursor(LiquidCrystalDevice_t *device, uint8_t row, uint8_t column)
{
    // addresses of the start of each row on the HD44780
    uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    LCD_send_command(device, LCD_SETDDRAMADDR | (row_offsets[row] + column));
}

/*
function to return LCD cursor to home position
*/
void LCD_return_home(LiquidCrystalDevice_t *device)
{
    LCD_send_command(device, LCD_RETURN_HOME);
    _delay_us(2000);
}

/*
function to set LCD display on/off
*/
void LCD_set_display_state(LiquidCrystalDevice_t *device, uint8_t displayState)
{
    if (displayState == LCD_DISPLAY_ON)
    {
        device->displayControl |= LCD_DISPLAY_ON;
    }
    else
    {
        device->displayControl &= ~LCD_DISPLAY_ON;
    }
    LCD_send_command(device, LCD_DISPLAYCONTROL | device->displayControl);
}

/*
function to set LCD cursor on/off
*/
void LCD_set_cursor_state(LiquidCrystalDevice_t *device, uint8_t cursor)
{
    if (cursor == LCD_CURSOR_ON)
    {
        device->displayControl |= LCD_CURSOR_ON;
    }
    else
    {
        device->displayControl &= ~LCD_CURSOR_ON;
    }
    LCD_send_command(device, LCD_DISPLAYCONTROL | device->displayControl);
}

/*
function to set LCD cursor blink on/off
*/
void LCD_set_cursor_blink(LiquidCrystalDevice_t *device, uint8_t blink)
{
    if (blink == LCD_CURSOR_BLINK)
    {
        device->displayControl |= LCD_CURSOR_BLINK;
    }
    else
    {
        device->displayControl &= ~LCD_CURSOR_BLINK;
    }
    LCD_send_command(device, LCD_DISPLAYCONTROL | device->displayControl);
}

/*
function to scroll LCD left/right
direction is either LCD_SHIFT_LEFT or LCD_SHIFT_RIGHT.
*/
void LCD_scroll(LiquidCrystalDevice_t *device, uint8_t direction)
{
    LCD_send_command(device, LCD_CURSORSHIFT | LCD_DISPLAY_SHIFT | direction);
}

/*
function to set LCD scroll direction to either LTR or RTL
*/
void LCD_set_scroll_dir(LiquidCrystalDevice_t *device, uint8_t direction)
{
    if (direction == LCD_ENTRY_LEFT)
    {
        device->displayMode |= LCD_ENTRY_LEFT;
    }
    else
    {
        device->displayMode &= ~LCD_ENTRY_LEFT;
    }
    LCD_send_command(device, LCD_ENTRYMODESET | device->displayMode);
}

/*
function to set LCD autoscroll on/off
*/
void LCD_set_autoscroll(LiquidCrystalDevice_t *device, uint8_t autoscroll)
{
    if (autoscroll == LCD_ENTRY_SHIFT_INCREMENT)
    {
        device->displayMode |= LCD_ENTRY_SHIFT_INCREMENT;
    }
    else
    {
        device->displayMode &= ~LCD_ENTRY_SHIFT_INCREMENT;
    }
    LCD_send_command(device, LCD_ENTRYMODESET | device->displayMode);
}

/*
function to create custom LCD characters
PCF8574 to LCD pinout
D7 D6 D5 D4 BL E RW RS
*/
void LCD_create_custom_char(LiquidCrystalDevice_t *device,
                            uint8_t slot,
                            uint8_t charmap[8])
{
    uint8_t i = 0;
    // only 8 locations for custom characters 0-7
    slot &= 0x7;
    LCD_send_command(device, LCD_SETCGRAMADDR | (slot << 3));

    for (i = 0; i < 8; i++)
    {
        LCD_write_byte(device, charmap[i], LCD_RS_PCF8974);
    }
}

/*
function to send command to LCD
PCF8574 to LCD pinout
D7 D6 D5 D4 BL E RW RS
*/
void LCD_send_command(LiquidCrystalDevice_t *device, uint8_t command)
{
    // mode is 0 because when sending a command, RW is 0 and RS is 0.
    LCD_write_byte(device, command, 0);
}

/*
function to write string to LCD
*/
void LCD_print(LiquidCrystalDevice_t *device, char *s)
{
    uint8_t i = 0;
    while (s[i] != 0x00)
    {
        LCD_write_byte(device, s[i], LCD_RS_PCF8974);
        i++;
    }
}

/*
function to send byte to LCD
PCF8574 to LCD pinout
D7 D6 D5 D4 BL E RW RS
*/
void LCD_write_byte(LiquidCrystalDevice_t *device, uint8_t value, uint8_t mode)
{
    uint8_t highnib = value & 0xf0;
    uint8_t lownib = (value << 4) & 0xf0;

    LCD_write_4bits(device, highnib | mode);
    LCD_write_4bits(device, lownib | mode);
}

/*
function to write 4 bits of data to the LCD
The LCD operates in 4-bit mode with the PCF8974.
*/
void LCD_write_4bits(LiquidCrystalDevice_t *device, uint8_t value)
{
    LCD_transmitI2C(device, value);
    _delay_us(200);
    LCD_transmitI2C(device, value | LCD_E_PCF8974);
    _delay_us(200);
    LCD_transmitI2C(device, value & ~LCD_E_PCF8974);
    _delay_us(200);
}

/*
function to transmit data to the LCD via i2c
*/
void LCD_transmitI2C(LiquidCrystalDevice_t *device, uint8_t value)
{
    // LCD backlight setting is preserved in the lowest function call.
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
    char str[16];
    i2c_master_init(I2C_SCL_FREQUENCY_400K);
    // i2c_master_send_byte(PCF8574_ADDR, 0x08);
    // _delay_ms(5000);
    // i2c_master_send_byte(PCF8574_ADDR, 0);
    LiquidCrystalDevice_t lcd;
    lcd = LCD_init(PCF8574_ADDR, 16, 2, LCD_CHARACTER_FONT_5X8);
    _delay_ms(1000);
    LCD_clear(&lcd);
    _delay_ms(500);
    LCD_set_backlight(&lcd, LCD_BACKLIGHT_OFF);
    _delay_ms(500);
    LCD_set_backlight(&lcd, LCD_BACKLIGHT_ON);
    _delay_ms(500);
    LCD_set_display_state(&lcd, LCD_DISPLAY_ON);

    LCD_return_home(&lcd);
    LCD_set_cursor_state(&lcd, LCD_CURSOR_ON);
    LCD_set_cursor_blink(&lcd, LCD_CURSOR_BLINK);
    _delay_ms(500);
    strncpy(str, "Hello world i2c0", 16);
    LCD_print(&lcd, str);
    _delay_ms(500);

    LCD_set_cursor(&lcd, 1, 0);
    strncpy(str, "Hello world i2c1", 16);
    LCD_print(&lcd, str);
    while (1)
    {
        // LCD_set_backlight(&lcd, LCD_BACKLIGHT_OFF);
        // _delay_ms(500);
        // LCD_set_backlight(&lcd, LCD_BACKLIGHT_ON);
        // _delay_ms(500);
        strncpy(str, "Hello world i2c0", 16);
        LCD_print(&lcd, str);
        _delay_ms(1000);

        LCD_set_cursor(&lcd, 1, 0);
        strncpy(str, "Hello world i2c1", 16);
        LCD_print(&lcd, str);
        _delay_ms(1000);
        LCD_clear(&lcd);
        _delay_ms(1000);
    }
}