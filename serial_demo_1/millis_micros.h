#ifndef TIMELIB_H
#define TIMELIB_H

#include <avr/interrupt.h>
#include <util/atomic.h>

// typedef enum
// {
//     TIMER_PERIOD_1US,
//     TIMER_PERIOD_1MS,
// } timer_period_enum;

extern volatile uint32_t millis;
/**
 * @brief millis incrementing function to run inside the TIMER0_COMPA_vect ISR loop
 */
void millis_timer_ISR_loop(void);

/**
 * @brief initialize timer 0 to compare-reset every 1ms.
 */
void millis_timer_init(void);

/**
 * @brief get millis value
 */
uint32_t get_millis(void);

/**
 * @brief get micros value
 */
uint32_t get_micros(void);

#endif // TIMELIB_H