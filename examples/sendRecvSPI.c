/**
 * @file sendRecvSPI.c
 *
 * This example shows the usage of the SPI unit in combination with the Reset and UART unit.
 * The example sends "Hello World" via SPI and sends the received data from SPI via UART.
 *
 * The Reset unit resets the microcontroller whenever the ExtPack is reset and the ExtPack when the microcontroller was reset.
 *
 */

#include <stddef.h>
#include <avr/io.h>
#include <util/delay.h>

#include "ExtPack.h"
#include "ExtPack_U_Reset.h"
#include "ExtPack_U_UART.h"
#include "ExtPack_U_SPI.h"

#define RESET_UNIT unit_U00
#define UART_UNIT unit_U03
#define SPI_UNIT unit_U06

void reset_unit_custom_ISR(unit_t unit, uint8_t data);
void SPI_unit_custom_ISR(unit_t unit, uint8_t data);

int main() {
#ifndef __AVR_ATmega328P__
    uint8_t temp = CLKCTRL.MCLKCTRLB & ~CLKCTRL_PEN_bm; // Disable global clk Prescaler
    CCP = 0xD8; // Disable change protection of Prescaler to write data
    CLKCTRL.MCLKCTRLB = temp;
#endif
    init_ExtPack(NULL, NULL, NULL);
    init_ExtPack_Unit(UART_UNIT, EXTPACK_UART_UNIT, NULL);
    init_ExtPack_Unit(SPI_UNIT, EXTPACK_SPI_UNIT, SPI_unit_custom_ISR);
    reset_ExtPack();
    _delay_us(100); // Wait for the ExtPack to send his reset request (which would reset this microcontroller)
    set_ExtPack_custom_ISR(RESET_UNIT, reset_unit_custom_ISR);
    while (1) {
        char hello_string[13] = "Hello World\n";
        send_ExtPack_SPI_String_to_slave(SPI_UNIT, 0, (uint8_t*)hello_string, 1000);
        _delay_ms(1000);
    }
}

void reset_unit_custom_ISR(unit_t unit, uint8_t data) {
    // ExtPack was reset
    if (data == 0xFF) {
#if defined(__AVR_ATmega328P__)
        // Reset controller
        __asm__ __volatile__("jmp 0x0000");
#else
        CCP = 0xD8; // Unprotection
        RSTCTRL.SWRR = RSTCTRL_SWRE_bm; // Reset
#endif
    }
}

void SPI_unit_custom_ISR(unit_t unit, uint8_t data) {
    // UART data received
    send_ExtPack_UART_data(UART_UNIT, data);
}
