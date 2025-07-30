/**
 * @file EchoUART.c
 *
 * This example shows the usage of the UART unit in combination with the Reset and Error unit.
 * The example sends all received data back to the UART sender.
 * If there is an error in the communication "ERROR\n" is sent via UART.
 *
 * The Reset unit resets the microcontroller whenever the ExtPack is reset and the ExtPack when the microcontroller was reset.
 *
 */

#include <stddef.h>
#include <avr/io.h>
#include <util/delay.h>

#include "ExtPack.h"
#include "ExtPack_U_Reset.h"
#include "ExtPack_U_Error.h"
#include "ExtPack_U_UART.h"

#define RESET_UNIT unit_U00
#define ERROR_UNIT unit_U01
#define UART_Unit unit_U03

void reset_unit_custom_ISR(unit_t unit, uint8_t data);
void error_unit_custom_ISR(unit_t unit, uint8_t data);
void UART_unit_custom_ISR(unit_t unit, uint8_t data);

int main() {
#ifndef __AVR_ATmega328P__
    uint8_t temp = CLKCTRL.MCLKCTRLB & ~CLKCTRL_PEN_bm; // Disable global clk Prescaler
    CCP = 0xD8; // Disable change protection of Prescaler to write data
    CLKCTRL.MCLKCTRLB = temp;
#endif
    init_ExtPack(NULL, error_unit_custom_ISR, NULL);
    init_ExtPack_Unit(UART_Unit, EXTPACK_UART_UNIT, UART_unit_custom_ISR);
    reset_ExtPack();
    _delay_us(100); // Wait for the ExtPack to send his reset request (which would reset this microcontroller)
    set_ExtPack_custom_ISR(RESET_UNIT, reset_unit_custom_ISR);
    while (1);
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

void error_unit_custom_ISR(unit_t unit, uint8_t data) {
    // An error occurred
    uint8_t error_string[7] = "ERROR\n";
    send_ExtPack_UART_String(UART_Unit, error_string, 10000);
}

void UART_unit_custom_ISR(unit_t unit, uint8_t data) {
    // UART data received
    send_ExtPack_UART_data(unit, data);
}
