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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/i2c_master.h"
#include "src/liquid_crystal_i2c.h"

// leds
#define ERROR_LED_1_PIN PD4
#define ERROR_LED_2_PIN PD5
#define ERROR_LED_3_PIN PD6
#define ERROR_LED_PORT PORTD
#define ERROR_LED_DDR DDRD

int main(void)
{
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
        strncpy(str, "LCD i2c0", 16);
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