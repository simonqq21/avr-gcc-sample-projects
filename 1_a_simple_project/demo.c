/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <joerg@FreeBSD.ORG> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.        Joerg Wunsch
 * ----------------------------------------------------------------------------
 *
 * Simple AVR demonstration.  Controls a LED that can be directly
 * connected from OC1/OC1A to GND.  The brightness of the LED is
 * controlled with the PWM.  After each period of the PWM, the PWM
 * value is either incremented or decremented, that's all.
 *
 * $Id: demo.c 1637 2008-03-17 21:49:41Z joerg_wunsch $
 */
#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif
#define F_CPU 16000000UL // 16 MHz
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

enum
{
    UP,
    DOWN
};
#define TIMER1_TOP 1023

/*
interrupt service routine runs every time timer1 overflows.
*/
ISR(TIMER1_OVF_vect)
{
    static uint16_t pwm;
    static uint8_t direction;
    switch (direction)
    {
    case UP:
        if (++pwm == TIMER1_TOP)
            direction = DOWN;
        break;
    case DOWN:
        if (--pwm == 0)
            direction = UP;
        break;
    }
    /*
    OCR1A is composed of two 8-bit registers:
        OCR1AH
        OCR1AL

        OCR1A contains a 16-bit value that is continuously compared with the counter
    value in TCNT1.
        A match can be used to generate an output compare interrupt, or generate
    a waveform output on the OC1A pin.

    TCNT1 is composed of two 8-bit registers:
        TCNTH
        TCNTL

        TCNT1 gives direct read/write access to the timer/counter unit 16-bit counter.
    */
    // set the compare value for timer/counter 1.
    OCR1A = pwm;
}

void ioinit(void)
{
    // timer 1 is 10-bit PWM
    /*
    Register
    TCCR1A - Timer/Counter1 Control Register A

    WGM10 - Waveform Generation Mode
    WGM11 - Waveform Generation Mode
    -
    -
    COM1B0 - Compare Output Mode for Channel B
    COM1B1 - Compare Output Mode for Channel B
    COM1A0 - Compare Output Mode for Channel A
    COM1A1 - Compare Output Mode for Channel A

    when WGM13:0 is 0b0011, PWM, phase correct, 10-bit.
    When COM1A1:0 is 0b10, Clear OC1A/OC1B on compare match when up-counting. Set
    OC1A/OC1B on compare match when down counting.

    The OC1A output can be set to toggle its logical level on each compare match by
    setting the compare output mode bits to toggle mode (COM1A1:0 = 1).
    */
    // set timer/counter1 mode to PWM, phase correct, 10-bit
    // set PWM mode to Clear OC1A/OC1B on compare match when up-counting. Set
    // OC1A/OC1B on compare match when down counting.
    TCCR1A = _BV(WGM10) | _BV(WGM11) | _BV(COM1A1);
    /*
     * NB: TCCR1A and TCCR1B could actually be the same register, so
     * take care to not clobber it.
     *
     * Register
     * TCCR1B
     *
     * ICNC1 - Input Capture Noise Canceler
     * ICES1 - Input Capture Edge Select
     * -
     * WGM13 - Waveform Generation Mode
     * WGM12 - Waveform Generation Mode
     * CS12 - Clock Select
     * CS11 - Clock Select
     * CS10 - Clock Select
     *
     * When CS12:0 is 0b001, clock source is clk/1
     * When CS12:0 is 0b010, clock source is clk/8 (8 times slower prescaled)
     * When CS12:0 is 0b011, clock source is clk/64 (64 times slower prescaled)
     */
    // Start timer 1, set clock source to clk/8 (8 times slower prescaled)
    // TCCR1B |= _BV(CS10);
    TCCR1B |= _BV(CS11);
    // TCCR1B |= _BV(CS11) | _BV(CS10);
    /*Set timer/counter1 value to 0. */
    OCR1A = 0;
    /* enable PB1 (OC1A) as output*/
    DDRB = _BV(PB1);

    /*
    Register
    TIMSK1 - Timer/Counter1 Interrupt Mask Register

    -
    -
    ICIE1 - Timer/Counter1, Input Capture Interrupt Enable
    -
    -
    OCIE1B - Timer/Counter1, Output Compare B Match Interrupt Enable
    OCIE1A - Timer/Counter1, Output Compare A Match Interrupt Enable
    TOIE1 - Timer/Counter1, Overflow Interrupt Enable

    */
    // enable timer 1 overflow interrupt
    TIMSK1 = _BV(TOIE1);
    /*
    opposite of sei() is cli()
    */
    // set enable interrupts
    sei();
}

int main(void)
{
    // run ioinit function once.
    ioinit();

    /* loop forever, the interrupts are doing the rest. */
    while (1)
    {
        sleep_mode();
    }
}
