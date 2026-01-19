#include <avr/io.h>

typedef struct
{
    volatile uint8_t *ddr;
    volatile uint8_t *port;
    uint8_t pin;
} PinConfig;