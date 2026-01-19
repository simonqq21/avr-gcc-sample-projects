#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif
#define F_CPU 16000000UL // 16 MHz

#include <stdlib.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include "utils.h"

/*
one button, two LEDs
all off
two LEDs on
two LEDs alternately flashing ON/OFF
two LEDs alternately fading
*/

// #define F_CPU 16000000UL   /* CPU clock in Hertz */
// #define SOFTCLOCK_FREQ 300 /* internal software clock is 100Hz */
// #define TMR1_SCALE (F_CPU / 2048UL / SOFTCLOCK_FREQ * 10 + 9) / 10
// // scale factor from hardware clock to software clock
// #define TMR0_SCALE (F_CPU / 512UL / SOFTCLOCK_FREQ * 10 + 9) / 10

#define SOFTCLOCK_FREQ 124
#define TMR0_SCALE 63 * 4
#define TMR1_SCALE 63

/*
pin definitions
*/
#define LED1_PIN PD5
#define LED1_PORT PORTD
#define LED1_DDR DDRD

#define LED2_PIN PD6
#define LED2_PORT PORTD
#define LED2_DDR DDRD

#define LED3_PIN PD7
#define LED3_PORT PORTD
#define LED3_DDR DDRD

#define LED4_PIN PB0
#define LED4_PORT PORTB
#define LED4_DDR DDRB

#define BTN1_PIN PD2
#define BTN1_PORT PORTD
#define BTN1_DDR DDRD
#define BTN1_PIND PIND

/*
PinConfig structs
*/
const PinConfig led1 = {.port = &LED1_PORT, .ddr = &LED1_DDR, .pin = LED1_PIN};
const PinConfig led2 = {.port = &LED2_PORT, .ddr = &LED2_DDR, .pin = LED2_PIN};
const PinConfig led3 = {.port = &LED3_PORT, .ddr = &LED3_DDR, .pin = LED3_PIN};
const PinConfig led4 = {.port = &LED4_PORT, .ddr = &LED4_DDR, .pin = LED4_PIN};
const PinConfig btn1 = {.port = &BTN1_PORT, .ddr = &BTN1_DDR, .pind = &BTN1_PIND, .pin = BTN1_PIN};

volatile struct
{
    uint8_t tmr0_ovf_int : 1;
    uint8_t tmr1_ovf_int : 1;
    uint8_t tmr2_ovf_int : 1;
} intflags;

volatile uint8_t led1_pwm;
volatile uint8_t led2_pwm;

volatile uint16_t millis, cnt1;

/*
ISRs
*/
ISR(TIMER0_OVF_vect)
{
    static uint8_t scaler0 = TMR0_SCALE;
    if (--scaler0 == 0)
    {
        scaler0 = TMR0_SCALE;
        intflags.tmr0_ovf_int = 1;
    }
}

ISR(TIMER1_OVF_vect)
{
    static uint16_t scaler1 = TMR1_SCALE;
    if (--scaler1 == 0)
    {
        scaler1 = TMR1_SCALE;
        intflags.tmr1_ovf_int = 1;
    }
}

ISR(TIMER2_OVF_vect)
{
    static uint16_t scaler2 = TMR0_SCALE;
    if (--scaler2 == 0)
    {
        scaler2 = TMR0_SCALE;
        intflags.tmr2_ovf_int = 1;
    }
}

void ioinit()
{
    // set button as input with pull-up resistor
    *btn1.ddr &= ~_BV(btn1.pin);
    *btn1.port |= _BV(btn1.pin);
    // set LEDs as outputs
    *led1.ddr |= _BV(led1.pin);
    *led2.ddr |= _BV(led2.pin);
    // *led1.port |= _BV(led1.pin);
    // *led2.port |= _BV(led2.pin);

    *led3.ddr |= _BV(led3.pin);
    // *led3.port |= _BV(led3.pin);
    *led4.ddr |= _BV(led4.pin);
    // *led4.port |= _BV(led4.pin);
    // set timer 0 as PWM generator
    /*
    PWM, phase correct
    */
    TCCR0A |= (_BV(WGM00));
    /*
    Clear OC0A on compare match when up-counting. Set OC0A on compare match
    when down-counting.
    */
    TCCR0A |= (_BV(COM0A1) | _BV(COM0B1));
    /*
    no prescaling
    */
    TCCR0B |= _BV(CS00);
    /*
    timer 0 overflow interrupt enable
    */
    TIMSK0 |= _BV(TOIE0);

    TCCR2A |= (_BV(WGM20));
    TCCR2B |= _BV(CS20);
    TIMSK2 |= _BV(TOIE2);

    // set timer 1 as software clock generator
    /*
    PWM, phase correct, 10-bit
    */
    TCCR1A |= (_BV(WGM11) | _BV(WGM10));
    /*
    Clear OC1A/OC1B on compare match when up-counting. Set
    OC1A/OC1B on compare match when down counting.
    */
    TCCR1A |= (_BV(COM1A1));
    /*
    no prescaling
    */
    TCCR1B |= _BV(CS10);
    /*
    timer 1 overflow interrupt enable
    */
    TIMSK1 |= _BV(TOIE1);

    sei();

    OCR0A = 64;
    OCR0B = 254;
}

int main()
{
    ioinit();
    while (1)
    {
        wdt_reset();
        //
        if (intflags.tmr0_ovf_int)
        {
            millis++;
            if (millis == SOFTCLOCK_FREQ)
            {
                *led3.port ^= _BV(led3.pin);
                millis = 0;
            }
            intflags.tmr0_ovf_int = 0;

            // *led1.port ^= _BV(led1.pin);
        }

        // if (intflags.tmr1_ovf_int)
        // {
        //     cnt1++;
        //     if (cnt1 == SOFTCLOCK_FREQ)
        //     {
        //         *led4.port ^= _BV(led4.pin);
        //         cnt1 = 0;
        //     }
        //     intflags.tmr1_ovf_int = 0;

        //     // *led2.port ^= _BV(led2.pin);
        // }
        if (intflags.tmr2_ovf_int)
        {
            cnt1++;
            if (cnt1 == SOFTCLOCK_FREQ)
            {
                *led4.port ^= _BV(led4.pin);
                cnt1 = 0;
            }
            intflags.tmr2_ovf_int = 0;

            // *led2.port ^= _BV(led2.pin);
        }
        sleep_mode();
    }
    return 0;
}