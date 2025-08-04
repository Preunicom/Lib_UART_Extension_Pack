/**
 * @file ReadRTC_Example__Reset_I2C_UART_ACK.c
 *
 * This example shows the usage of the I2C unit in combination with the UART, Reset and ACK units.
 * As communication partner an DS3231 real time clock is used.
 * The example reads the RTC registers with an one second delay and sends the result as readable chars via UART.
 * Acknowledgements are used to ensure the ExtPack receives the commands.
 *
 * The Reset unit resets the microcontroller whenever the ExtPack is reset and the ExtPack when the microcontroller was reset.
 */

#include <stddef.h>
#include <util/delay.h>
#include <avr/io.h>

#include "ExtPack/Util/ExtPack_U_Reset.h"
#include "ExtPack/Util/ExtPack_U_I2C.h"
#include "ExtPack/Util/ExtPack_U_UART.h"
#include "ExtPack/Service/ExtPack_U_Acknowledge_Advanced.h"

#define RESET_UNIT unit_U00
#define I2C_UNIT unit_U07
#define UART_UNIT unit_U03

void reset_unit_custom_ISR(unit_t, uint8_t);
void i2c_unit_custom_ISR(unit_t, uint8_t);

int main() {
#ifndef __AVR_ATmega328P__
    uint8_t temp = CLKCTRL.MCLKCTRLB & ~CLKCTRL_PEN_bm; // Disable global clk Prescaler
    CCP = 0xD8; // Disable change protection of Prescaler to write data
    CLKCTRL.MCLKCTRLB = temp;
#endif
    // Initialize the UART Extension Pack
    init_ExtPack(NULL, NULL, NULL);
    init_ExtPack_Unit(UART_UNIT, EXTPACK_UART_UNIT, NULL);
    init_ExtPack_Unit(I2C_UNIT, EXTPACK_I2C_UNIT, i2c_unit_custom_ISR);
    reset_ExtPack();
    _delay_us(100); // Wait for the ExtPack to send his reset request (which would reset this microcontroller)
    set_ExtPack_custom_ISR(RESET_UNIT, reset_unit_custom_ISR);
    // Init ACK
    clear_ExtPack_ack_event(); // Before waiting for an ACK the ACK event should be cleared to ensure correct operation
    do {
        set_ExtPack_ACK_enable(1);
    } while (wait_for_ExtPack_ACK_data(1, 100) != EXT_PACK_SUCCESS);
    do {
        set_ExtPack_I2C_partner_adr(I2C_UNIT, 0x68);
    } while (wait_for_ExtPack_ACK_data(0x68, 100) != EXT_PACK_SUCCESS);
    while(1) {
        // Read all RTC registers every second
        for (uint8_t temp = 0; temp < 7; temp++) {
            clear_ExtPack_ack_event();
            do {
                send_ExtPack_I2C_data(I2C_UNIT, temp);
            } while (wait_for_ExtPack_ACK_data(temp, 100) != EXT_PACK_SUCCESS);
            clear_ExtPack_ack_event();
            do {
                receive_ExtPack_I2C_data(I2C_UNIT);
            } while (wait_for_ExtPack_ACK_data(0x00, 100) != EXT_PACK_SUCCESS);
            _delay_us(500);
        }
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

void i2c_unit_custom_ISR(unit_t unit, uint8_t data) {
    // I2C data received
    send_ExtPack_UART_data(UART_UNIT, (data >> 4) + '0'); // Send first char
    send_ExtPack_UART_data(UART_UNIT, (data & 0x0F) + '0'); // Send second char
    _delay_ms(10); // Gives the ExtPack time to send the data to a lower BAUD rate partner
    send_ExtPack_UART_data(UART_UNIT, '\n');
}