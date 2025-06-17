#include "Dynamic_Delay.h"
#include <util/delay.h>

void delay_us(unsigned int __us) {
    for (unsigned int i = 0; i < __us; i++) {
        _delay_us(1);
    }
}

void delay_ms(unsigned int __ms) {
    for (unsigned int i = 0; i < __ms; i++) {
        _delay_ms(1);
    }
}