#include "../lib/UARTExtPack/src/UARTExtPack.h"
#define F_CPU 16000000UL
#define USED_UNITS 3

#define unit_U00_UART unit_U00
#define unit_U01_GPIO unit_U01
#define unit_U02_TIME unit_U02

void unit_U00_custom_ISR(unit_t, char);
void unit_U01_custom_ISR(unit_t, char);
void unit_U02_custom_ISR(unit_t, char);

int main() {
    // Initialize the UART Extension Pack
    init_ExtPack();
    init_ExtPack_Unit(unit_U00_UART, UART_Unit, unit_U00_custom_ISR);
    init_ExtPack_Unit(unit_U01_GPIO, GPIO_Unit, unit_U01_custom_ISR);
    init_ExtPack_Unit(unit_U02_TIME, Timer_Unit, unit_U02_custom_ISR);
    /*
     * 50 KHz
     * 250 values are a good divider.
     * --> start value: 2^8-200 = 56
     * --> prescaler: 250
     * ----> pre scaled: 50KHz / 250 = 200 Hz
     * ----> counted: 200KHz / 200 = 1 Hz
     */
    SEND_BLOCKING(set_ExtPack_timer_start_value(unit_U02_TIME, 56)); // am: 11 (0xC) -- 0x38
    SEND_BLOCKING(set_ExtPack_timer_prescaler(unit_U02_TIME, 250)); // am: 10 (0x8) -- 0xFA
    SEND_BLOCKING(restart_ExtPack_timer(unit_U02_TIME)); // am: 01 (0x4) -- 0x00
    SEND_BLOCKING(set_ExtPack_timer_enable(unit_U02_TIME, 1)); // am: 00 (0x0) -- 0x01
    while(1) {
        ;
    }
}

void unit_U00_custom_ISR(unit_t unit, char data) {
    // UART Unit data received
    SEND_BLOCKING(send_ExtPack_UART_data(unit_U00_UART, data));
}

void unit_U01_custom_ISR(unit_t unit, char data) {
    // GPIO interrupt received
    SEND_BLOCKING(send_ExtPack_UART_data(unit_U00_UART, 'X')); //0x58
}

void unit_U02_custom_ISR(unit_t unit, char data) {
    // Timer interrupt received
    SEND_BLOCKING(set_ExtPack_gpio_out(unit_U01_GPIO, get_ExtPack_data_gpio_out(unit_U01_GPIO) ^ 0b10));
}