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
software clock
*/
#define MILLIS_FRACT_INT 24 >> 3
#define MILLIS_FRACT_MAX 1000 >> 3

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
const PinConfig btn1 = {.port = &BTN1_PORT, .ddr = &BTN1_DDR, .pind = &BTN1_PIND, .pin = BTN1_PIN};

volatile uint8_t btn_pressed_confidence;
volatile uint8_t btn_released_confidence;
volatile uint8_t btn_pressed;
#define btn_press_confidence_threshold 20

/*
interrupt flags
*/
volatile struct
{
    uint8_t tmr0_ovf_int : 1;
    uint8_t tmr1_ovf_int : 1;
    uint8_t tmr2_ovf_int : 1;
} intflags;

volatile uint8_t led1_pwm;
volatile uint8_t led2_pwm;

volatile uint16_t millis;
volatile uint8_t millis_fract;

volatile enum {
    MODE_OFF,
    MODE_ON,
    MODE_BLINKING,
    MODE_FADING
} __attribute__((packed)) mode = MODE_FADING;

volatile enum {
    BLINKING_S1,
    BLINKING_S2
} __attribute__((packed)) blinking_mode = BLINKING_S1;
const uint16_t durations_blinking_modes[] = {1000, 1000};

volatile enum {
    FADING_S1,
    FADING_S2,
    FADING_S3,
    FADING_S4,
    FADING_S5,
    FADING_S6
} __attribute__((packed)) fading_mode = FADING_S1;
const uint16_t durations_fading_modes[] = {
    350,
    300,
    350,
    350,
    300,
    350,
};

volatile uint16_t previous_millis_state_changed;
volatile uint16_t elapsed;

/*
ISRs
*/

/*
16000000/64 = 250000
counter0 increments every 1/250000 seconds, or 4 us.
counter0 overflows every 256 * 4us = 1.024 ms.
1ms has elapsed every counter0 overflow.
As for the fractional part, accumulate the 0.024 ms by incrementing a variable by 24.
Every time that variable exceeds 1000, add one millisecond to the millis variable.
*/
ISR(TIMER0_OVF_vect)
{
    millis += 2;
    millis_fract += MILLIS_FRACT_INT * 2;
    if (millis_fract >= MILLIS_FRACT_MAX)
    {
        millis_fract -= MILLIS_FRACT_MAX;
        millis++;
    }
    intflags.tmr0_ovf_int = 1;
}

void ioinit()
{
    // set button as input with pull-up resistor
    *btn1.ddr &= ~_BV(btn1.pin);
    *btn1.port |= _BV(btn1.pin);
    // set LEDs as outputs
    *led1.ddr |= _BV(led1.pin);
    *led2.ddr |= _BV(led2.pin);
    *led1.port &= ~_BV(led1.pin);
    *led2.port &= ~_BV(led2.pin);

    *led3.ddr |= _BV(led3.pin);

    /*Configure timer0 as a 10-bit phase correct PWM with clock prescaled by 64 */
    /*
    Clear OC0A on compare match when up-counting. Set OC0A on compare match
    when down-counting.
    */
    TCCR0A |= (_BV(COM0A1) | _BV(COM0B1));
    /* PWM, phase correct */
    TCCR0A |= (_BV(WGM00));
    /* /64 prescaling */
    TCCR0B |= (_BV(CS01) | _BV(CS00));
    /* timer 0 overflow interrupt enable */
    TIMSK0 |= _BV(TOIE0);

    /* set interrupts enable */
    sei();
}

int main()
{
    ioinit();
    while (1)
    {
        wdt_reset();

        if (!(*btn1.pind & _BV(btn1.pin)))
        {

            if (!btn_pressed)
            {
                btn_pressed_confidence++;
                if (btn_pressed_confidence > btn_press_confidence_threshold)
                {
                    btn_pressed = 1;
                    // *led3.port ^= _BV(led3.pin);
                    switch (mode)
                    {
                    case (MODE_OFF):
                        mode = MODE_ON;
                        // *led3.port |= _BV(led3.pin);
                        break;
                    case (MODE_ON):
                        mode = MODE_BLINKING;
                        break;
                    case (MODE_BLINKING):
                        mode = MODE_FADING;
                        break;
                    case (MODE_FADING):
                        mode = MODE_OFF;
                        break;
                    default:
                        mode = MODE_OFF;
                        // *led3.port &= ~_BV(led3.pin);
                    }
                }
            }
            btn_released_confidence = 0;
        }
        else
        {
            if (btn_pressed)
            {
                btn_released_confidence++;
                if (btn_released_confidence > btn_press_confidence_threshold)
                {
                    btn_pressed = 0;
                }
            }
            btn_pressed_confidence = 0;
        }

        switch (mode)
        {
        case (MODE_OFF):
            led1_pwm = 0;
            led2_pwm = 0;
            break;
        case (MODE_ON):
            led1_pwm = 255;
            led2_pwm = 255;
            break;
        case (MODE_BLINKING):

            /*
            states:
            LED 1 on, LED 2 off, 1000ms
            LED 1 off, LED 2 on, 1000ms
            */
            if (millis - previous_millis_state_changed >= durations_blinking_modes[blinking_mode])
            {
                previous_millis_state_changed = millis;
                switch (blinking_mode)
                {
                case BLINKING_S1:
                    led1_pwm = 255;
                    led2_pwm = 0;
                    blinking_mode = BLINKING_S2;
                    break;

                case BLINKING_S2:
                    led1_pwm = 0;
                    led2_pwm = 255;
                    blinking_mode = BLINKING_S1;
                    break;

                default:
                    led1_pwm = 0;
                    led2_pwm = 0;
                    blinking_mode = BLINKING_S1;
                }
            }
            break;
        case (MODE_FADING):
            /*
            states:
            LED1 fade up, LED2 off, 400ms
            LED1 max, LED2 off, 200ms
            LED1 fade down, LED2 off, 400ms
            LED1 off, LED2 fade up, 400ms
            LED1 off, LED2 max, 200ms
            LED1 off, LED2 fade down, 400ms
            */
            elapsed = millis - previous_millis_state_changed;

            if (elapsed >= durations_fading_modes[fading_mode])
            {
                previous_millis_state_changed = millis;
                switch (fading_mode)
                {
                case FADING_S1:
                    fading_mode = FADING_S2;
                    break;

                case FADING_S2:
                    fading_mode = FADING_S3;
                    break;

                case FADING_S3:
                    fading_mode = FADING_S4;
                    break;

                case FADING_S4:
                    fading_mode = FADING_S5;
                    break;

                case FADING_S5:
                    fading_mode = FADING_S6;
                    break;

                case FADING_S6:
                    fading_mode = FADING_S1;
                    break;

                default:
                    fading_mode = FADING_S1;
                }
            }
            switch (fading_mode)
            {
            case FADING_S1:
                led1_pwm = (float)elapsed / (float)durations_fading_modes[fading_mode] * 255.0;
                led2_pwm = 0;
                break;

            case FADING_S2:
                led1_pwm = 255;
                led2_pwm = 0;
                break;

            case FADING_S3:
                led1_pwm = 255.0 - 255.0 * (float)elapsed / (float)durations_fading_modes[fading_mode];
                led2_pwm = 0;
                break;

            case FADING_S4:
                led1_pwm = 0;
                led2_pwm = (float)elapsed / (float)durations_fading_modes[fading_mode] * 255.0;
                break;

            case FADING_S5:
                led1_pwm = 0;
                led2_pwm = 255;
                break;

            case FADING_S6:
                led1_pwm = 0;
                led2_pwm = 255.0 - 255.0 * (float)elapsed / (float)durations_fading_modes[fading_mode];
                break;

            default:
                led1_pwm = 0;
                led2_pwm = 0;
            }
            break;
        }

        OCR0A = led1_pwm;
        OCR0B = led2_pwm;
        sleep_mode();
    }
    return 0;
}