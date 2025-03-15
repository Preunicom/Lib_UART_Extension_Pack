#include "../lib/UARTExtPack/src/UARTExtPack.h"
#include <util/delay.h>
#include <avr/io.h>

#define F_CPU 16000000UL
#define USED_UNITS 2

#define unit_U00_UART unit_U00
#define unit_U01_GPIO unit_U01

void unit_a_custom_ISR(unit_t, char);
void unit_b_custom_ISR(unit_t, char);

int main() {
    DDRD |= (1<<PORTD4);
    PORTD |= (1<<PORTD4);
    // Initialize the UART Extension Pack
    init_ExtPack();
    init_ExtPack_Unit(unit_U00_UART, UART_Unit, unit_a_custom_ISR);
    init_ExtPack_Unit(unit_U01_GPIO, GPIO_Unit, unit_b_custom_ISR);
    while(1) {
        _delay_ms(1000);
        send_ExtPack_data_blocking(unit_U01_GPIO, 0);
        _delay_ms(1000);
        send_ExtPack_data_blocking(unit_U01_GPIO, 3);
    }
}

void unit_a_custom_ISR(unit_t unit, char data) {
    // UART Unit data received
    send_ExtPack_data_nonblocking(unit_U00_UART, data);
    send_ExtPack_data_nonblocking(unit_U01_GPIO, 1);
}

void unit_b_custom_ISR(unit_t unit, char data) {
    // GPIO interrupt received
    send_ExtPack_data_blocking(unit_U00_UART, 'X'); //0x58
}