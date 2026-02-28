#ifndef TIMELIB_H
#define TIMELIB_H

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define MILLIS_FRACT_INC 24 >> 3
#define MILLIS_FRACT_MAX 1000 >> 3

uint32_t millis();
uint32_t micros();
void init_millis_timer();
#endif