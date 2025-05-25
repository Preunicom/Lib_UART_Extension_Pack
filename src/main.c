#include <stdlib.h>
#include <avr/interrupt.h>

#include "../lib/UARTExtPack/src/UARTExtPack.h"

#define USED_UNITS 3

#define unit_U00_RST unit_U00
#define unit_U01_ERR unit_U01
#define unit_U02_ACK unit_U02
#define unit_U03_UART unit_U03
#define unit_U04_GPIO unit_U04
#define unit_U05_TIME unit_U05
#define unit_U06_SPI unit_U06

void unit_U00_custom_ISR(unit_t, uint8_t);
void unit_U01_custom_ISR(unit_t, uint8_t);
void unit_U02_custom_ISR(unit_t, uint8_t);
void unit_U03_custom_ISR(unit_t, uint8_t);
void unit_U04_custom_ISR(unit_t, uint8_t);
void unit_U05_custom_ISR(unit_t, uint8_t);
void unit_U06_custom_ISR(unit_t, uint8_t);

int main() {
    // Initialize the UART Extension Pack
    init_ExtPack(unit_U00_custom_ISR, unit_U01_custom_ISR, unit_U02_custom_ISR);
    init_ExtPack_Unit(unit_U03_UART, UART_Unit, unit_U02_custom_ISR);
    init_ExtPack_Unit(unit_U04_GPIO, GPIO_Unit, unit_U03_custom_ISR);
    init_ExtPack_Unit(unit_U05_TIME, Timer_Unit, unit_U04_custom_ISR);
    init_ExtPack_Unit(unit_U06_SPI, SPI_Unit, unit_U05_custom_ISR);
    /*
     * 50 KHz
     * 250 values are a good divider.
     * --> start value: 2^8-200 = 56
     * --> prescaler: 250
     * ----> pre scaled: 50KHz / 250 = 200 Hz
     * ----> counted: 200KHz / 200 = 1 Hz
     */
    configure_ExtPack_timer(unit_U05_TIME, 250, 56, 1000, 10, 1000);
    while(1) {
        ;
    }
}

void unit_U00_custom_ISR(unit_t unit, uint8_t data) {
    // ExtPack was reset
    if ((uint8_t)data == 0xFF) {
        // Reset controller
        __asm__ __volatile__("jmp 0x0000");
    }
}

void unit_U01_custom_ISR(unit_t unit, uint8_t data) {
    // An error occurred
    SEND_MAX_ATTEMPTS(set_ExtPack_gpio_out(unit_U04_GPIO, get_ExtPack_data_gpio_out(unit_U04_GPIO) ^ 0b01), 10, 1000);
}

void unit_U02_custom_ISR(unit_t unit, uint8_t data) {
    // An ACK got received
    ;
}

void unit_U03_custom_ISR(unit_t unit, uint8_t data) {
    // UART Unit data received
    SEND_MAX_ATTEMPTS(send_ExtPack_UART_data(unit_U03_UART, data), 10, 10);
}

void unit_U04_custom_ISR(unit_t unit, uint8_t data) {
    // GPIO interrupt received
    if(data == 1) {
        uint8_t string[13] = "Hello World!\n";
        send_ExtPack_UART_String(unit_U03_UART, string, 1000, 10, 1000);
        send_ExtPack_SPI_String(unit_U06_SPI, string, 1000, 10, 1000);
    }
}

void unit_U05_custom_ISR(unit_t unit, uint8_t data) {
    // Timer interrupt received
    SEND_MAX_ATTEMPTS(set_ExtPack_gpio_out(unit_U04_GPIO, get_ExtPack_data_gpio_out(unit_U04_GPIO) ^ 0b10), 10, 1000);
}

void unit_U06_custom_ISR(unit_t unit, uint8_t data) {
    // SPI message received
    if (data != 0) {
        // Got content
        ;
    }
}