#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#ifndef F_CPU
#define F_CPU 16000000
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/i2c_master.h"
#include "src/liquid_crystal_i2c.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <util/twi.h>
#include <avr/interrupt.h>

// ----------------------------------------------------------------
// keypad defines
// ----------------------------------------------------------------

enum KeyPadPressed_t
{
    KEY_NONE,
    KEY_PRESSED,
};

enum KeypadKey_t
{
    KEY_NONE2 = 0,
    KEY_0 = '0',
    KEY_1 = '1',
    KEY_2 = '2',
    KEY_3 = '3',
    KEY_4 = '4',
    KEY_5 = '5',
    KEY_6 = '6',
    KEY_7 = '7',
    KEY_8 = '8',
    KEY_9 = '9',
    KEY_A = 'A',
    KEY_B = 'B',
    KEY_C = 'C',
    KEY_D = 'D',
    KEY_E = 'E',
    KEY_F = 'F',
};

char keypad_char, prev_keypad_char;
char str[16];
LiquidCrystalDevice_t lcd;

#define KEYPAD_ISR_MAX 10

// keypad port, pins, and ddr
#define KEYPAD_DDR DDRD
#define KEYPAD_PORT PORTD
#define KEYPAD_PIN PIND

// keypad row and col pins
#define KEYPAD_R0 0
#define KEYPAD_R1 1
#define KEYPAD_R2 2
#define KEYPAD_R3 3
#define KEYPAD_C0 4
#define KEYPAD_C1 5
#define KEYPAD_C2 6
#define KEYPAD_C3 7

volatile char __KEYPAD_ISR_BUFFER[KEYPAD_ISR_MAX];

#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4

const char KEYPAD_MAP[4][4] PROGMEM = {
    {KEY_1, KEY_2, KEY_3, KEY_A},
    {KEY_4, KEY_5, KEY_6, KEY_B},
    {KEY_7, KEY_8, KEY_9, KEY_C},
    {KEY_E, KEY_0, KEY_F, KEY_D},
};

// typedef struct
// {
//     volatile uint8_t *port;
//     volatile uint8_t *pin;
//     volatile uint8_t *ddr;
//     uint8_t bit;
// } KeypadPin;

// typedef struct
// {
//     KeypadPin row_pins[KEYPAD_ROWS];
//     KeypadPin col_pins[KEYPAD_COLS];
// } Keypad;

// static Keypad _keypad;

// ----------------------------------------------------------------
// keypad function definitions
// ----------------------------------------------------------------
void Keypad_setup(void);
void Keypad_ISR(void);
enum KeypadKey_t Keypad_getKey(void);

// enum KeyPadPressed_t Keypad_keyPressed(void);
// void Keypad_waitRelease(void);

enum KeypadKey_t Keypad_getKeyISR(void);
enum KeyPadPressed_t Keypad_keyPressedISR(void);
void Keypad_waitReleaseISR(void);

// ----------------------------------------------------------------
// internal keypad function prototypes
// ----------------------------------------------------------------
static uint8_t readRows(void);
static uint8_t readCols(void);
static void toggleRowsCols(void);

// ----------------------------------------------------------------
// keypad functions
// ----------------------------------------------------------------

/*
initialize keypad row and column pins
*/
void Keypad_setup(void)
{
    // *(_keypad.row_pins[0]).ddr = DDRD;
    // *(_keypad.row_pins[1]).ddr = DDRD;

    // set rows as input
    KEYPAD_DDR &= ~_BV(KEYPAD_R0);
    KEYPAD_DDR &= ~_BV(KEYPAD_R1);
    KEYPAD_DDR &= ~_BV(KEYPAD_R2);
    KEYPAD_DDR &= ~_BV(KEYPAD_R3);

    // set cols as output
    KEYPAD_DDR |= _BV(KEYPAD_C0);
    KEYPAD_DDR |= _BV(KEYPAD_C1);
    KEYPAD_DDR |= _BV(KEYPAD_C2);
    KEYPAD_DDR |= _BV(KEYPAD_C3);

    // enable row input pullup resistors
    KEYPAD_PORT |= _BV(KEYPAD_R0);
    KEYPAD_PORT |= _BV(KEYPAD_R1);
    KEYPAD_PORT |= _BV(KEYPAD_R2);
    KEYPAD_PORT |= _BV(KEYPAD_R3);

    // set col output low
    KEYPAD_PORT &= ~_BV(KEYPAD_C0);
    KEYPAD_PORT &= ~_BV(KEYPAD_C1);
    KEYPAD_PORT &= ~_BV(KEYPAD_C2);
    KEYPAD_PORT &= ~_BV(KEYPAD_C3);
}

/*
update keypad buffer for debouncing.
Must be called in timer ISR.
*/
void Keypad_ISR(void)
{
    volatile static uint8_t i = 0;

    __KEYPAD_ISR_BUFFER[i++] = Keypad_getKey();
    if (i >= KEYPAD_ISR_MAX)
        i = 0;
}

/*
read keypad key without debouncing. Returns Key_0 to Key_F.
*/
enum KeypadKey_t Keypad_getKey(void)
{
    volatile enum KeypadKey_t key = KEY_NONE;
    volatile uint8_t r, c; // row and column values
    r = c = 0;

    /*
    check if button is pressed
    At this point, rows are inputs and columns are outputs.
    Read the row values.

    when a button is pressed it shows up as 0 instead of 1,
    thus the need for inversion.
    */
    r = ~readRows();
    // r = 1;
    /*
    if any bit is one, one button was pressed.
    continue with the keypad scanning algorithm.
    */
    if (r)
    {
        /*
        read the columns
        first swap the pin roles for the rows and columns so that
        the rows become outputs and columns become inputs.
        At this point, rows are outputs and columns are inputs.
        */
        toggleRowsCols();

        // _delay_us(20);
        // c = ~readCols();
        c = readCols();

        // c = 16;
        toggleRowsCols();

        // snprintf(str, 16, "r = %u", r);
        // LCD_clear(&lcd);
        // LCD_print(&lcd, str);
        // LCD_set_cursor(&lcd, 1, 0);
        // snprintf(str, 16, "c = %u", c);
        // LCD_print(&lcd, str);
        // _delay_ms(100);

        /*
        find the row of the pressed button
        */
        if (r & _BV(KEYPAD_R3))
            r = 3;
        else if (r & _BV(KEYPAD_R2))
            r = 2;
        else if (r & _BV(KEYPAD_R1))
            r = 1;
        else if (r & _BV(KEYPAD_R0))
            r = 0;

        /*
        find the column of the pressed button
        */
        if (c & _BV(KEYPAD_C3))
            c = 3;
        else if (c & _BV(KEYPAD_C2))
            c = 2;
        else if (c & _BV(KEYPAD_C1))
            c = 1;
        else if (c & _BV(KEYPAD_C0))
            c = 0;
        // get the key pressed with row and column
        key = pgm_read_byte(&(KEYPAD_MAP[r][c]));
    }
    // key = pgm_read_byte(&(KEYPAD_MAP[0][0]));

    // set each row low
    // for (int i = 0; i < 4; i++)
    // {
    //     KEYPAD_PORT =

    //
    //         key = pgm_read_byte(&(KEYPAD_MAP[r][c]));
    // }
    return key;
}

/*
check if key is pressed.
returns KEY_PRESSED or KEY_NONE.
*/
// enum KeyPadPressed_t Keypad_keyPressed(void)
// {
//     // read port

//     // if row ports have any value, then a key is currently pressed.
// }

/*
wait while a key in the keypad is pressed.
*/
// void Keypad_waitRelease(void)
// {
//     while (Keypad_keyPressed() != KEY_NONE)
//         ;
// }

/*
Read debounced key from keypad buffer.
Returns Key_0 to Key_F
*/
enum KeypadKey_t Keypad_getKeyISR(void)
{
    uint8_t i;
    enum KeypadKey_t key = __KEYPAD_ISR_BUFFER[0];

    // compare all elements in the keypad buffer
    for (i = 0; i < KEYPAD_ISR_MAX; i++)
    {
        if (__KEYPAD_ISR_BUFFER[i] != key)
        {
            key = KEY_NONE2;
            break;
        }
    }
    return key;
}

/*
check keypad buffer for any key pressed.
Returns KEY_
*/
enum KeyPadPressed_t Keypad_keyPressedISR(void)
{
    uint8_t status = KEY_NONE;
    if (Keypad_getKeyISR() != KEY_NONE2)
    {
        status = KEY_PRESSED;
    }
    return status;
}

/*
wait until no key on the keypad is pressed.
wait until the keypad buffer is clear.
*/
void Keypad_waitReleaseISR(void)
{
    while (Keypad_keyPressedISR() != KEY_NONE)
        ;
}

static inline uint8_t readRows(void)
{
    uint8_t port = KEYPAD_PIN | 0xf0;
    // port = ~(KEYPAD_PIN & (_BV(KEYPAD_R3) | _BV(KEYPAD_R2) | _BV(KEYPAD_R1) | _BV(KEYPAD_R0)));

    return port;
}

static inline uint8_t readCols(void)
{
    // uint8_t port = KEYPAD_PIN | 15;
    uint8_t port = 0xff;
    // port &= KEYPAD_PIN & (_BV(KEYPAD_C3) | _BV(KEYPAD_C2) | _BV(KEYPAD_C1) | _BV(KEYPAD_C0));
    port &= KEYPAD_PIN | 0x0f;
    return port;
}

static void toggleRowsCols(void)
{
    // toggle keypad col pins pin mode and output (low input and high output)
    KEYPAD_DDR ^= (_BV(KEYPAD_R3) | _BV(KEYPAD_R2) | _BV(KEYPAD_R1) | _BV(KEYPAD_R0) | _BV(KEYPAD_C3) | _BV(KEYPAD_C2) | _BV(KEYPAD_C1) | _BV(KEYPAD_C0));
    KEYPAD_PORT ^= (_BV(KEYPAD_R3) | _BV(KEYPAD_R2) | _BV(KEYPAD_R1) | _BV(KEYPAD_R0) | _BV(KEYPAD_C3) | _BV(KEYPAD_C2) | _BV(KEYPAD_C1) | _BV(KEYPAD_C0));

    // // toggle keypad row pins pin mode and output (low input and high output)
    // KEYPAD_DDR ^= (_BV(KEYPAD_C3) | _BV(KEYPAD_C2) | _BV(KEYPAD_C1) | _BV(KEYPAD_C0));
    // KEYPAD_DDR ^= (_BV(KEYPAD_R3) | _BV(KEYPAD_R2) | _BV(KEYPAD_R1) | _BV(KEYPAD_R0));

    // // toggle keypad row pins pin mode and output (low input and high output)
    // KEYPAD_DDR = KEYPAD_DDR ^ (_BV(KEYPAD_R3) | _BV(KEYPAD_R2) | _BV(KEYPAD_R1) | _BV(KEYPAD_R0));
    // KEYPAD_PORT = KEYPAD_PORT ^ (_BV(KEYPAD_R3) | _BV(KEYPAD_R2) | _BV(KEYPAD_R1) | _BV(KEYPAD_R0));

    // // toggle keypad col pins pin mode and output (low input and high output)
    // KEYPAD_DDR = KEYPAD_DDR ^ (_BV(KEYPAD_C3) | _BV(KEYPAD_C2) | _BV(KEYPAD_C1) | _BV(KEYPAD_C0));
    // KEYPAD_PORT = KEYPAD_PORT ^ (_BV(KEYPAD_C3) | _BV(KEYPAD_C2) | _BV(KEYPAD_C1) | _BV(KEYPAD_C0));

    // toggle keypad row pins pin mode and output (low input and high output)
    // KEYPAD_DDR = KEYPAD_DDR ^ 0xff;
    // KEYPAD_PORT = KEYPAD_PORT ^ 0xff;
    // KEYPAD_DDR = KEYPAD_DDR ^ 0x0f;
    // KEYPAD_DDR = KEYPAD_DDR ^ 0xf0;

    // // toggle keypad col pins pin mode and output (low input and high output)
    // KEYPAD_PORT = KEYPAD_PORT ^ 0x0f;
    // KEYPAD_PORT = KEYPAD_PORT ^ 0xf0;

    // // toggle keypad row pins pin mode and output (low input and high output)
    // KEYPAD_DDR ^= 255;
    // KEYPAD_PORT ^= 255;
}

// ----------------------------------------------------------------

ISR(TIMER0_COMPA_vect)
{
    Keypad_ISR();
}

/*
Configure Timer 0
*/
void timer0_setup(void)
{
    // CTC mode
    TCCR0A |= _BV(WGM01);
    // prescaler 64
    // 16000000/64 = 250000
    // TCCR0B |= _BV(CS02);
    TCCR0B |= _BV(CS01) | _BV(CS00);
    // 250000 / 1000 Hz - 1 = 249
    OCR0A = 249;
    // enable output compare interrupt 0A
    TIMSK0 |= _BV(OCIE0A);
    sei();
}

int main(void)
{
    // LCD setup
    i2c_master_init(I2C_SCL_FREQUENCY_400K);

    lcd = LCD_init(PCF8574_ADDR, 16, 2, LCD_CHARACTER_FONT_5X8);
    LCD_set_display_state(&lcd, LCD_DISPLAY_ON);
    LCD_clear(&lcd);
    LCD_set_backlight(&lcd, LCD_BACKLIGHT_ON);
    LCD_return_home(&lcd);

    // Timer0 setup
    timer0_setup();

    // keypad setup
    Keypad_setup();

    while (1)
    {
        keypad_char = Keypad_getKeyISR();
        if (Keypad_keyPressedISR())
        {
            // keypad_char = 'X';
            if (keypad_char != prev_keypad_char)
            {
                prev_keypad_char = keypad_char;
                LCD_return_home(&lcd);
                strncpy(str, &keypad_char, 1);
                LCD_print(&lcd, str);
            }
        }
        else
        {
            if (keypad_char != prev_keypad_char)
            {
                prev_keypad_char = keypad_char;
                LCD_clear(&lcd);
            }
        }
    }
    return 0;
}