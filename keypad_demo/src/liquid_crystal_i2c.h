#ifndef LIQUID_CRYSTAL_I2C_H
#define LIQUID_CRYSTAL_I2C_H
#ifndef F_CPU
#define F_CPU 16000000
#endif
#include "i2c_master.h"
#include <util/delay.h>

// ----------------------------------------------------------------
// defines
// ----------------------------------------------------------------

#ifndef F_CPU
#define F_CPU 16000000
#endif

// 7-bit i2c address of PCF8574
#define PCF8574_ADDR 0x3F

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

// ----------------------------------------------------------------
// Function definitions
// ----------------------------------------------------------------
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

#endif