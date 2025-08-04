/**
* @file DataLogger_Example__Reset_SRAM_UART.c
 *
 * This example shows the usage of the SRAM unit in combination with the UART and Reset units.
 * The example saves the received UART data to the SRAM at address 0x05734,
 * requests it and sends it back via UART.
 *
 * @note: This works only for 1-2 UART characters or at very low BAUD rates
 * as there are 6 ExtPack commands sent when one is received. This leads to a lot of UART traffic.
 * Additionally, interrupts have to disabled in the UART ISR as there could be race conditions when setting the SRAM address.
 *
 * The Reset unit resets the microcontroller whenever the ExtPack is reset and the ExtPack when the microcontroller was reset.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stddef.h>
#include <util/delay.h>

#include "ExtPack/Core/ExtPack_Events.h"
#include "ExtPack/Util/ExtPack_U_Reset.h"
#include "ExtPack/Util/ExtPack_U_UART.h"
#include "ExtPack/Service/ExtPack_U_SRAM_Advanced.h"
#include "ExtPack/Util/Dynamic_Delay.h"


#define RESET_UNIT unit_U00
#define UART_UNIT unit_U03
#define SRAM_UNIT unit_U08

void reset_unit_custom_ISR(unit_t unit, uint8_t data);
void UART_unit_custom_ISR(unit_t unit, uint8_t data);

int main() {
#ifndef __AVR_ATmega328P__
    uint8_t temp = CLKCTRL.MCLKCTRLB & ~CLKCTRL_PEN_bm; // Disable global clk Prescaler
    CCP = 0xD8; // Disable change protection of Prescaler to write data
    CLKCTRL.MCLKCTRLB = temp;
#endif
    init_ExtPack(nullptr, nullptr, nullptr);
    init_ExtPack_Unit(UART_UNIT, EXTPACK_UART_UNIT, UART_unit_custom_ISR);
    init_ExtPack_Unit(SRAM_UNIT, EXTPACK_SRAM_UNIT, nullptr);
    reset_ExtPack();
    _delay_us(100); // Wait for the ExtPack to send his reset request (which would reset this microcontroller)
    set_ExtPack_custom_ISR(RESET_UNIT, reset_unit_custom_ISR);
    while (1) {
        if (get_ExtPack_event(SRAM_UNIT)) {
            send_ExtPack_UART_data(UART_UNIT, get_last_received_ExtPack_SRAM_data(SRAM_UNIT));
            clear_ExtPack_event(SRAM_UNIT);
        }
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

void UART_unit_custom_ISR(unit_t unit, uint8_t data) {
    // UART data received
    //Prepare SRAM address to (0x05734)
    reset_ExtPack_SRAM_address(SRAM_UNIT);
    delay_us(get_ExtPack_send_duration_us());
    set_ExtPack_SRAM_next_address_byte(SRAM_UNIT, 0x34);
    delay_us(get_ExtPack_send_duration_us());
    set_ExtPack_SRAM_next_address_byte(SRAM_UNIT, 0x57);
    delay_us(get_ExtPack_send_duration_us());
    // Address ready
    write_ExtPack_SRAM_data(SRAM_UNIT, data);
    // Address still valid, only next address write block is reset to block zero
    delay_us(get_ExtPack_send_duration_us());
    request_ExtPack_SRAM_data(SRAM_UNIT);
}