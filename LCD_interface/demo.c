#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif
#define F_CPU 16000000UL // 16 MHz

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/wdt.h>

#define RS PB0
#define RW PB1
#define E PB2
#define LCD_SIG_PORT PORTB
#define LCD_SIG_DDR DDRB
#define LCD_D_PORT PORTD
#define LCD_D_DDR DDRD
#define LCD_D_READ PIND
#define LCD_DELAY_US 1000
/*
software clock
*/
#define MILLIS_FRACT_INT 24 >> 3
#define MILLIS_FRACT_MAX 1000 >> 3

#define LEFT 0
#define RIGHT 1
#define CURSOR 0
#define DISPLAY 1

#define LCD_IR 0
#define LCD_DR 1
#define LCD_WRITE 0
#define LCD_READ 1

char str[80];

/*
software clock
*/
volatile uint16_t millis_val;
volatile uint8_t millis_fract;

/*
ISRs
*/

/*
16000000/64 = 250000
counter0 increments every 1/250000 seconds, or 4 us.
counter0 overflows every 256 * 4us = 1.024 ms.
1ms has elapsed every counter0 overflow.
As for the fractional part, accumulate the 0.024 ms by incrementing a variable by 24.
Every time that variable exceeds 1000, add one millisecond to the millis_val variable.
*/
ISR(TIMER0_OVF_vect)
{
    millis_val += 2;
    millis_fract += MILLIS_FRACT_INT * 2;
    if (millis_fract >= MILLIS_FRACT_MAX)
    {
        millis_fract -= MILLIS_FRACT_MAX;
        millis_val++;
    }
}

/*
****************************************************************
LCD functions
*/

/*
initialize the LCD
*/
void LCDInit()
{
    _delay_ms(30);
    // set entire port D as output
    LCD_D_DDR = 0b11111111;
    // E, RS, and RW' DDR
    LCD_SIG_DDR |= 0b111;
}

/*
set enable value
*/
void LCDSetEnable(uint8_t en)
{
    if (en)
    {
        LCD_SIG_PORT |= _BV(E);
        _delay_us(LCD_DELAY_US);
    }
    else
    {
        _delay_us(LCD_DELAY_US);
        LCD_SIG_PORT &= ~_BV(E);
    }
}

/*
set RS and RW value
*/
void LCDSetRSRW(uint8_t rs, uint8_t rw)
{
    if (rs)
    {
        LCD_SIG_PORT |= _BV(RS);
    }
    else
    {
        LCD_SIG_PORT &= ~_BV(RS);
    }
    if (rw)
    {
        LCD_SIG_PORT |= _BV(RW);
    }
    else
    {
        LCD_SIG_PORT &= ~_BV(RW);
    }
    _delay_us(LCD_DELAY_US);
}

/*
set LCD function
@param length 0 if 4 bit, 1 if 8 bit
@param lines 0 if 1 line, 1 if 2 lines
@param font 0 if 5x8 font, 1 if 5x10
*/
void LCDSetFunction(uint8_t length, uint8_t lines, uint8_t font)
{

    // set to 8-bit operation, 2-line display, 5x8 dot character font
    LCDSetRSRW(0, 0);
    LCDSetEnable(1);
    // 0 0 1 DL N F - -
    LCD_D_PORT = 0b00100000;
    if (length)
    {
        LCD_D_PORT |= _BV(4);
    }
    if (lines)
    {
        LCD_D_PORT |= _BV(3);
    }
    if (font)
    {
        LCD_D_PORT |= _BV(2);
    }
    LCDSetEnable(0);
}

// turn on display and cursor
void LCDOn(uint8_t display, uint8_t cursor, uint8_t blinking)
{
    LCDSetRSRW(0, 0);
    LCDSetEnable(1);
    // 0 0 0 0 0 0 1 D C B
    LCD_D_PORT = 0b00001000;
    if (display)
    {
        LCD_D_PORT |= _BV(2);
    }
    if (cursor)
    {
        LCD_D_PORT |= _BV(1);
    }
    if (blinking)
    {
        LCD_D_PORT |= _BV(0);
    }
    LCDSetEnable(0);
}

/*
set LCD entry mode
*/
void LCDSetEntryMode(uint8_t increment, uint8_t shift)
{
    LCDSetRSRW(0, 0);
    LCDSetEnable(1);
    LCD_D_PORT = 0b00000100;
    if (increment)
    {
        LCD_D_PORT |= _BV(1);
    }
    if (shift)
    {
        LCD_D_PORT |= _BV(0);
    }
    LCDSetEnable(0);
}

/*
clear the LCD
*/
void LCDClear()
{
    LCDSetRSRW(0, 0);
    LCDSetEnable(1);
    LCD_D_PORT = 0b00000001;
    LCDSetEnable(0);
}

/*
shift cursor
*/
void LCDShiftCursor(uint8_t mode, uint8_t dir)
{
    LCDSetRSRW(0, 0);
    LCDSetEnable(1);
    LCD_D_PORT = 0b00010000;
    if (mode)
    {
        LCD_D_PORT |= _BV(3);
    }
    if (dir)
    {
        LCD_D_PORT |= _BV(2);
    }
    LCDSetEnable(0);
}

/*
move cursor to home
*/
void LCDGoHome()
{
    LCDSetRSRW(0, 0);
    LCDSetEnable(1);
    LCD_D_PORT = 0b00000010;
    LCDSetEnable(0);
}

/*
Write data to the CGRAM/DDRAM
*/
void LCDWriteRAM(uint8_t data)
{
    LCDSetRSRW(1, 0);
    LCDSetEnable(1);
    LCD_D_PORT = data;
    LCDSetEnable(0);
}

/*
read data from CGRAM/DDRAM
*/
uint8_t LCDReadRAM()
{
    uint8_t data;
    LCDSetRSRW(1, 1);
    LCDSetEnable(1);
    LCD_D_DDR = 0b0;
    data = LCD_D_READ;
    LCDSetEnable(0);
    LCD_D_DDR = 0b11111111;
    LCD_D_PORT = 0;
    return data;
}

/*
set CGRAM address
*/
void LCDSetCGRAMAddr(uint8_t addr)
{
    LCDSetRSRW(0, 0);
    LCDSetEnable(1);
    LCD_D_PORT = 0b01000000;
    LCD_D_PORT |= addr;
    LCDSetEnable(0);
}
/*
set DDRAM address
*/
void LCDSetDDRAMAddr(uint8_t addr)
{
    LCDSetRSRW(0, 0);
    LCDSetEnable(1);
    LCD_D_PORT = 0b10000000;
    LCD_D_PORT |= addr;
    LCDSetEnable(0);
}

/*
read busy flag
*/
uint8_t LCDReadBusy()
{
    uint8_t ac;
    // RS = 0, RW = 1
    LCDSetRSRW(0, 1);
    LCDSetEnable(1);
    // set LCD data port as inputs
    LCD_D_DDR = 0b0;
    // read LCD data port
    ac = LCD_D_READ;
    LCDSetEnable(0);
    // set LCD data port as outputs
    LCD_D_DDR = 0b11111111;
    LCD_D_PORT = 0b0;
    return ac;
}

/*
****************************************************************
*/

void ioinit()
{
    LCDInit();

    /*Configure timer0 as a 10-bit phase correct PWM with clock prescaled by 64 */
    /*
    Clear OC0A on compare match when up-counting. Set OC0A on compare match
    when down-counting.
    */
    // TCCR0A |= (_BV(COM0A1) | _BV(COM0B1));
    // /* PWM, phase correct */
    // TCCR0A |= (_BV(WGM00));
    // /* /64 prescaling */
    // TCCR0B |= (_BV(CS01) | _BV(CS00));
    /* timer 0 overflow interrupt enable */
    // TIMSK0 |= _BV(TOIE0);

    /* set interrupts enable */
    sei();
}

int main()
{
    ioinit();
    LCDOn(1, 1, 1);

    LCDSetFunction(1, 1, 0);
    LCDSetEntryMode(1, 0);
    LCDClear();
    _delay_ms(500);
    LCDSetDDRAMAddr(0);
    strcpy(str, "0Simon Que");
    for (int i = 0; i < strlen(str); ++i)
    {
        LCDWriteRAM(str[i]);
    }
    _delay_ms(500);
    LCDSetDDRAMAddr(64);
    strcpy(str, "1Simon Que");
    for (int i = 0; i < strlen(str); ++i)
    {
        LCDWriteRAM(str[i]);
    }
    _delay_ms(500);
    LCDSetDDRAMAddr(20);
    strcpy(str, "2Simon Que");
    for (int i = 0; i < strlen(str); ++i)
    {
        LCDWriteRAM(str[i]);
    }
    _delay_ms(500);
    LCDSetDDRAMAddr(84);
    strcpy(str, "3Simon Que");
    for (int i = 0; i < strlen(str); ++i)
    {
        LCDWriteRAM(str[i]);
    }

    _delay_ms(2000);

    LCDClear();

    for (int i = 0; i < 20; i++)
    {
        LCDWriteRAM('0');
        _delay_ms(100);
    }
    for (int i = 0; i < 20; i++)
    {
        LCDWriteRAM('1');
        _delay_ms(100);
    }
    for (int i = 0; i < 20; i++)
    {
        LCDWriteRAM('2');
        _delay_ms(100);
    }
    for (int i = 0; i < 20; i++)
    {
        LCDWriteRAM('3');
        _delay_ms(100);
    }
    _delay_ms(1000);

    LCDSetCGRAMAddr(0);
    LCDWriteRAM(0b00010101);
    LCDWriteRAM(0b10001010);
    LCDWriteRAM(0b00010101);
    LCDWriteRAM(0b10001010);
    LCDWriteRAM(0b00010101);
    LCDWriteRAM(0b10001010);
    LCDWriteRAM(0b00010101);
    LCDWriteRAM(0b10001010);
    LCDSetCGRAMAddr(0b00001000);
    LCDWriteRAM(0b10001010);
    LCDWriteRAM(0b00010101);
    LCDWriteRAM(0b10001010);
    LCDWriteRAM(0b00010101);
    LCDWriteRAM(0b10001010);
    LCDWriteRAM(0b00010101);
    LCDWriteRAM(0b10001010);
    LCDWriteRAM(0b00010101);

    LCDClear();
    LCDSetDDRAMAddr(0);
    for (int i = 0; i < 40; i++)
    {
        LCDWriteRAM(0);
        LCDWriteRAM(1);
    }

    while (1)
    {
        wdt_reset();
        sleep_mode();
    }
    return 0;
}