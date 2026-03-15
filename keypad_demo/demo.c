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

#define KEYPAD_ISR_MAX 10

// ----------------------------------------------------------------
// keypad function definitions
// ----------------------------------------------------------------
void Keypad_setup(void);
void Keypad_ISR(void);

enum KeypadKey_t Keypad_getKey(void);
enum KeyPadPressed_t Keypad_keyPressed(void);
void Keypad_waitRelease(void);

enum KeypadKey_t Keypad_getKeyISR(void);
enum KeyPadPressed_t Keypad_keyPressedISR(void);
void Keypad_waitReleaseISR(void);

// ----------------------------------------------------------------
// keypad variables
// ----------------------------------------------------------------

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
    volatile uint8_t r, c;
    r = c = 0;

    /*
    check if button is pressed
    at this point, rows are inputs and columns are outputs.
    read the row values
    */
    /*
    when a button is pressed it shows up as 0 instead of 1,
    thus the need for inversion.
    */
    r = ~readRows();
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
        */
        toggleRowsCols();
        c = ~readCols();
        toggleRowsCols();

        /*
        find the row of the pressed button
        */
        for (int i = 0; i < 4; i++)
        {
            if ()
        }

        /*
        find the column of the pressed button
        */
    }
}

enum KeyPadPressed_t Keypad_keyPressed(void) {}
void Keypad_waitRelease(void) {}

enum KeypadKey_t Keypad_getKeyISR(void) {}
enum KeyPadPressed_t Keypad_keyPressedISR(void) {}
void Keypad_waitReleaseISR(void) {}

int main(void)
{
    while (1)
    {
    }
    return 0;
}