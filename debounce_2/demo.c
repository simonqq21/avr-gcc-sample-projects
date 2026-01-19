#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif
#define F_CPU 16000000UL // 16 MHz

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define bit_set_8(var, mask) ((var) |= (uint8_t)(mask))
#define bit_clear_8(var, mask) ((var) &= (uint8_t)~(mask))
#define bit_toggle_8(var, mask) ((var) ^= (uint8_t)(mask))
#define bit_read_8(var, mask) ((var) & (uint8_t)(mask))

volatile unsigned char ledState = 0;

static unsigned char debounceB0, debounceB1, debouncedBState;

/*
Vertical counter
- This algorithm treats every bit in a byte as an individual counter.
- It uses two variables, debounceB0 and debounceB1, to form a 2-bit
    counter for every pin on Port C.
- Timer 0 samples the port regularly.
- If a pin's state differs from the debouncedBState (the "confirmed" state),
    the counter for that pin increments.
- If the state stays consistent for 4 consecutive samples (until the 2-bit
    counter overflows), the change is confirmed.
- If the pin state changes back before 4 samples, the counter is reset.
- This is extremely fast because it processes all 8 pins of a port simultaneously
    using bitwise logic.

- The debounce logic is triggered by the Timer 0 Overflow Interrupt.
- The timer is configured with a prescaler of 64 (CS00 and CS01 set).
- 16,000,000/(64×256 levels)≈976 Hz.
- The buttons are sampled roughly every 1.024 milliseconds. Since it
    takes 4 samples to confirm a state change, the debounce time is
    approximately 4ms.
*/
// vertical stacked counter based debounce
unsigned debounceB(unsigned char sample)
{
    unsigned char delta, changes;

    // Set delta to changes from last sample
    // sample is the raw input value
    // debouncedBState is the stable, "debounced" value
    delta = sample ^ debouncedBState;

    // Increment two byte counter
    debounceB1 = debounceB1 ^ debounceB0;
    debounceB0 = ~debounceB0;

    // reset any unchanged bits
    // if delta[i] is 0, there is no difference between sample[i] and debouncedBState[i]
    // if delta[i] is 1, there is a difference between the raw sample[i] and stable,
    //      "debounced" debouncedBState[i]
    debounceB0 &= delta;
    debounceB1 &= delta;

    // update state & calculate returned change set
    // check if the 2 bit counter has overflowed (counted up to 4) and delta is 1
    changes = ~(~delta | debounceB0 | debounceB1);
    // update the debounced state;
    // flip the debouncedBState[i] if changes[i] is 1
    debouncedBState ^= changes;

    return changes;
}

/*
example:

debouncedBState = 1
sample = 1
delta = 1 ^ 1 = 0
debounceB1 = 0 ^ 0 = 0
debounceB0 = ~0 = 1
debounceB0 = 1 & 0 = 0
debounceB1 = 0 & 0 = 0
changes = ~(~0 | 0 | 0) = 0
debouncedBState = 1 ^ 0 = 1
return 0

sample = 0
delta = 0 ^ 1 = 1
debounceB1 = 0 ^ 0 = 0
debounceB0 = ~0 = 1
debounceB0 = 1 & 1 = 1
debounceB1 = 0 & 1 = 0
changes = ~(~1 | 1 | 0) = 0
debouncedBState = 1 ^ 0 = 1
return 0

sample = 0
delta = 0 ^ 1 = 1
debounceB1 = 0 ^ 1 = 1
debounceB0 = ~1 = 0
debounceB0 = 0 & 1 = 0
debounceB1 = 1 & 1 = 1
changes = ~(~1 | 0 | 1) = 0
debouncedBState = 1 ^ 0 = 1
return 0




*/

// timer 0 overflow interrupt handler
ISR(TIMER0_OVF_vect)
{
    unsigned char sample = PINC;
    unsigned char changes = debounceB(sample);

    if (bit_read_8(changes, _BV(PC0)) &&
        (!bit_read_8(sample, _BV(PC0))))
    {
        ledState++;
        // ledState = ledState % 8;
    }
    if (bit_read_8(changes, _BV(PC1)) &&
        (!bit_read_8(sample, _BV(PC1))))
    {
        ledState--;
        // ledState = ledState % 8;
    }
}

int main(void)
{
    DDRD = 0xFF;  // Make all PB* -- PORT B -- pins output.
    PORTD = 0xFF; // turn all PB* -- PORT B -- pins off.

    bit_clear_8(DDRC, _BV(PC0) | _BV(PC1)); // PC0, PC1 is an input
    bit_set_8(PORTC, _BV(PC0) | _BV(PC1));  // Turn on pull-up resistor on PC0, PC1

    bit_set_8(TCCR0B, _BV(CS00) | _BV(CS01));
    bit_set_8(TIMSK0, _BV(TOIE0)); // turn on overflow interrupt for timer 0

    sei(); // turn on interrupts

    while (1)
    {
        PORTD = ledState;
    }
}