#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif
#define F_CPU 16000000UL // 1 MHz

#include <avr/io.h>

uint8_t led1 = PD5;
uint8_t btn1 = PD2;

// 20 ms
// 20*10^-3 / (1/16*10^6) =
const uint16_t btn_press_confidence_threshold = 1000;
uint16_t btn_pressed_confidence;
uint16_t btn_released_confidence;
uint8_t btn_pressed = 0;

uint8_t led_state;

int main()
{
    // enable pull down resistors in MCUCR
    MCUCR &= ~_BV(PUD);
    // set btn1 as a digital input
    DDRD &= ~_BV(btn1);
    // set pullup resistor on btn1
    PORTD |= _BV(btn1);
    // set led1 as a digital output
    DDRD |= _BV(led1);

    while (1)
    {
        // read btn1
        // if button is pressed
        if (!(PIND & _BV(btn1)))
        {
            // if button pressed latch is still false,
            if (!btn_pressed)
            {
                // increment the button pressed confidence
                btn_pressed_confidence++;

                // if button pressed confidence exceeds threshold, perform action.
                if (btn_pressed_confidence > btn_press_confidence_threshold)
                {
                    // toggle the led_state
                    if (!led_state)
                    {
                        led_state = 1;
                    }
                    else
                    {
                        led_state = 0;
                    }
                    // set btn_pressed latch to 1
                    btn_pressed = 1;
                }
            }
            // reset the button released confidence.
            btn_released_confidence = 0;
        }
        else
        {
            // if button pressed latch is still true,
            if (btn_pressed)
            {
                // increment the button released confidence
                btn_released_confidence++;

                // if button pressed confidence exceeds threshold,
                if (btn_released_confidence > btn_press_confidence_threshold)
                {
                    // reset btn_pressed latch to 0
                    btn_pressed = 0;
                }
            }
            // reset the button pressed confidence.
            btn_pressed_confidence = 0;
        }

        // set the LED state
        if (led_state)
        {
            PORTD |= _BV(led1);
        }
        else
        {
            PORTD &= ~_BV(led1);
        }
    }
}